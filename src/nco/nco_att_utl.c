/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_att_utl.c,v 1.6 2002-06-02 02:24:23 zender Exp $ */

/* Purpose: Attribute utilities */

/* Copyright (C) 1995--2002 Charlie Zender
   This software is distributed under the terms of the GNU General Public License
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

#include "nco_att_utl.h" /* Attribute utilities */

void
aed_prc /* [fnc] Process a single attribute edit on a single variable */
(const int nc_id, /* I [id] Input netCDF file ID */
 const int var_id, /* I [id] ID of variable on which to perform attribute editing */
 const aed_sct aed) /* I [id] Structure containing information necessary to edit */
{
  /* Purpose: Process a single attribute edit on a single variable */
  
  /* If var_id == NC_GLOBAL ( = -1) then global attribute will be edited */
  
  char var_nm[NC_MAX_NAME];
  
  /* fxm: netCDF 2 specifies att_sz should be type int, netCDF 3 uses size_t */
  int nbr_att;
  int rcd=NC_NOERR; /* [rcd] Return code */
  long att_sz;
  
  nc_type att_typ;
  
  void *att_val_new=NULL;
  
  if(var_id == NC_GLOBAL){
    /* Get number of global attributes in file */
    (void)nco_inq(nc_id,(int *)NULL,(int *)NULL,&nbr_att,(int *)NULL);
    (void)strcpy(var_nm,"Global");
  }else{
    /* Get name and number of attributes for variable */
    (void)nco_inq_var(nc_id,var_id,var_nm,(nc_type *)NULL,(int *)NULL,(int *)NULL,&nbr_att);
  } /* end else */

  rcd=nco_inq_att_flg(nc_id,var_id,aed.att_nm,&att_typ,&att_sz);
  /* Before changing metadata, change missing values to new missing value if warranted 
     This capability is an add on feature and is not implemented very cleanly or efficiently
     If, for example, every variable has a "missing_value" attribute and it is changed
     globally, then this routine will go into and out of define mode for each variable,
     rather than collecting all the information in one pass and then replacing all the 
     data in a second pass.
     This is because ncatted was originally designed to change only metadata and so was
     architected differently from other NCO operators.
   */
  if(
     strcmp(aed.att_nm,"missing_value") == 0 /* Current attribute is "missing_value" */
     && var_id != NC_GLOBAL /* Current attribute is not global */
     && (aed.mode == aed_modify || aed.mode == aed_overwrite)  /* Modifying or overwriting existing value */
     && rcd == NC_NOERR /* Only when existing missing_value attribute is modified */
     && att_sz == 1L /* Old missing_value attribute must be of size 1 */
     && aed.sz == 1L /* New missing_value attribute must be of size 1 */
     ){

    int *dmn_id;
    long *dmn_sz;
    long *dmn_srt;
    long idx;
    long var_sz=long_CEWI;
    ptr_unn mss_val_crr;
    ptr_unn mss_val_new;
    ptr_unn var_val;
    var_sct *var=NULL_CEWI;

    (void)fprintf(stdout,"%s: WARNING Replacing missing value data in variable %s\n",prg_nm_get(),var_nm);

    /* Take file out of define mode */
    (void)nco_enddef(nc_id);
  
    /* Initialize (partially) variable structure */
    var=(var_sct *)nco_malloc(sizeof(var_sct));
    var->nc_id=nc_id;
    var->id=var_id;
    var->sz=1L;

    /* Get type of variable and number of dimensions */
    (void)nco_inq_var(var->nc_id,var->id,(char *)NULL,&var->type,&var->nbr_dim,(int *)NULL,(int *)NULL);
    dmn_id=(int *)nco_malloc(var->nbr_dim*sizeof(int));
    dmn_sz=(long *)nco_malloc(var->nbr_dim*sizeof(long));
    dmn_srt=(long *)nco_malloc(var->nbr_dim*sizeof(long));
    (void)nco_inq_vardimid(var->nc_id,var->id,dmn_id);

    /* Get dimension sizes and construct variable size */
    for(idx=0;idx<var->nbr_dim;idx++){
      (void)nco_inq_dimlen(var->nc_id,dmn_id[idx],dmn_sz+idx);
      var->sz*=dmn_sz[idx];
      dmn_srt[idx]=0L;
    } /* end loop over dim */
    var->dmn_id=dmn_id;
    var->cnt=dmn_sz;
    var->srt=dmn_srt;
      
    /* Place var_get() code inline since var struct is not truly complete */
    if((var->val.vp=(void *)malloc(var->sz*nco_typ_lng(var->type))) == NULL){
      (void)fprintf(stdout,"%s: ERROR Unable to malloc() %ld*%d bytes in aed_prc()\n",prg_nm_get(),var->sz,nco_typ_lng(var->type));
      nco_exit(EXIT_FAILURE); 
    } /* end if */
    if(var->sz > 1){
      (void)nco_get_vara(var->nc_id,var->id,var->srt,var->cnt,var->val.vp,var->type);
    }else{
      (void)nco_get_var1(var->nc_id,var->id,var->srt,var->val.vp,var->type);
    } /* end else */
    
    /* Get current missing value attribute */
    var->mss_val.vp=NULL;
    var->has_mss_val=mss_val_get(var->nc_id,var);

    /* Sanity check */
    if(var->has_mss_val == False){
      (void)fprintf(stdout,"%s: ERROR \"missing_value\" attribute does not exist in aed_prc()\n",prg_nm_get());
      nco_exit(EXIT_FAILURE);
    } /* end if */

    /* Shortcuts to avoid indirection */
    var_val=var->val;
    var_sz=var->sz;

    /* Get new and old missing values in same type as variable */
    mss_val_crr.vp=(void *)nco_malloc(att_sz*nco_typ_lng(var->type));
    mss_val_new.vp=(void *)nco_malloc(aed.sz*nco_typ_lng(var->type));
    (void)val_conform_type(att_typ,var->mss_val,var->type,mss_val_crr);
    (void)val_conform_type(aed.type,aed.val,var->type,mss_val_new);

    /* Typecast pointer to values before access */
    (void)cast_void_nctype(var->type,&var_val);
    (void)cast_void_nctype(var->type,&mss_val_crr);
    (void)cast_void_nctype(var->type,&mss_val_new);
  
    switch(var->type){
    case NC_FLOAT: for(idx=0L;idx<var_sz;idx++) {if(var_val.fp[idx] == *mss_val_crr.fp) var_val.fp[idx]=*mss_val_new.fp;} break;
    case NC_DOUBLE: for(idx=0L;idx<var_sz;idx++) {if(var_val.dp[idx] == *mss_val_crr.dp) var_val.dp[idx]=*mss_val_new.dp;} break;
    case NC_INT: for(idx=0L;idx<var_sz;idx++) {if(var_val.lp[idx] == *mss_val_crr.lp) var_val.lp[idx]=*mss_val_new.lp;} break;
    case NC_SHORT: for(idx=0L;idx<var_sz;idx++) {if(var_val.sp[idx] == *mss_val_crr.sp) var_val.sp[idx]=*mss_val_new.sp;} break;
    case NC_CHAR: for(idx=0L;idx<var_sz;idx++) {if(var_val.cp[idx] == *mss_val_crr.cp) var_val.cp[idx]=*mss_val_new.cp;} break;
    case NC_BYTE: for(idx=0L;idx<var_sz;idx++) {if(var_val.bp[idx] == *mss_val_crr.bp) var_val.bp[idx]=*mss_val_new.bp;} break;
    default: nco_dfl_case_nctype_err(); break;
    } /* end switch */

    /* Un-typecast the pointer to values after access */
    (void)cast_nctype_void(var->type,&var_val);
    (void)cast_nctype_void(var->type,&mss_val_crr);
    (void)cast_nctype_void(var->type,&mss_val_new);

    /* Write to disk */
    if(var->nbr_dim == 0){
      (void)nco_put_var1(nc_id,var->id,var->srt,var->val.vp,var->type);
    }else{ /* end if variable is a scalar */
      (void)nco_put_vara(nc_id,var->id,var->srt,var->cnt,var->val.vp,var->type);
    } /* end else */

    /* Free memory */
    mss_val_crr.vp=nco_free(mss_val_crr.vp);
    mss_val_new.vp=nco_free(mss_val_new.vp);
    var->mss_val.vp=nco_free(var->mss_val.vp);
    var->val.vp=nco_free(var->val.vp);
    var->dmn_id=(int *)nco_free(var->dmn_id);
    var->srt=(long *)nco_free(var->srt);
    var->cnt=(long *)nco_free(var->cnt);

    /* Put file back in define mode */
    (void)nco_redef(nc_id);
  } /* end if replacing missing value data */

  /* Change metadata (as written, this must be done after missing_value data is replaced) */
  switch(aed.mode){
  case aed_append:	
    if(rcd == NC_NOERR){
      /* Append to existing attribute value */
      if(aed.type != att_typ){
	(void)fprintf(stdout,"%s: ERROR %s attribute %s is of type %s not %s, unable to append\n",prg_nm_get(),var_nm,aed.att_nm,nco_typ_sng(att_typ),nco_typ_sng(aed.type));
	nco_exit(EXIT_FAILURE);
      } /* end if */
      att_val_new=(void *)nco_malloc((att_sz+aed.sz)*nco_typ_lng(aed.type));
      (void)nco_get_att(nc_id,var_id,aed.att_nm,(void *)att_val_new,aed.type);
      /* NB: Following assumes sizeof(char) = 1 byte */
      (void)memcpy((void *)((char *)att_val_new+att_sz*nco_typ_lng(aed.type)),
		   (void *)aed.val.vp,
		   aed.sz*nco_typ_lng(aed.type));
      (void)nco_put_att(nc_id,var_id,aed.att_nm,aed.type,att_sz+aed.sz,att_val_new);
      att_val_new=nco_free(att_val_new);
    }else{
      /* Create new attribute */
      (void)nco_put_att(nc_id,var_id,aed.att_nm,aed.type,aed.sz,aed.val.vp);
    } /* end else */
    break;
  case aed_create:	
    if(rcd != NC_NOERR) (void)nco_put_att(nc_id,var_id,aed.att_nm,aed.type,aed.sz,aed.val.vp);  
    break;
  case aed_delete:	
    if(rcd == NC_NOERR) (void)nco_del_att(nc_id,var_id,aed.att_nm);
    break;
  case aed_modify:	
    if(rcd == NC_NOERR) (void)nco_put_att(nc_id,var_id,aed.att_nm,aed.type,aed.sz,aed.val.vp);
    break;
  case aed_overwrite:	
    (void)nco_put_att(nc_id,var_id,aed.att_nm,aed.type,aed.sz,aed.val.vp);  
    break;
  default: 
    break;
  } /* end switch */
  
} /* end aed_prc() */

void 
att_cpy  /* [fnc] Copy attributes from input netCDF file to output netCDF file */
(const int in_id, /* I [id] netCDF input-file ID */
 const int out_id, /* I [id] netCDF output-file ID */
 const int var_in_id, /* I [id] netCDF input-variable ID */
 const int var_out_id) /* I [id] netCDF output-variable ID */
{
  /* Purpose: Copy attributes from input netCDF file to output netCDF file
     If var_in_id == NC_GLOBAL, then global attributes are copied. 
     Otherwise only indicated variable's attributes are copied */

  int idx;
  int nbr_att;
  int rcd; /* [enm] Return code */

  if(var_in_id == NC_GLOBAL){
    (void)nco_inq_natts(in_id,&nbr_att);
  }else{
    (void)nco_inq_varnatts(in_id,var_in_id,&nbr_att);
  } /* end else */
  
  for(idx=0;idx<nbr_att;idx++){
    char att_nm[NC_MAX_NAME];
    
    (void)nco_inq_attname(in_id,var_in_id,idx,att_nm);
    rcd=nco_inq_att_flg(out_id,var_out_id,att_nm,(nc_type *)NULL,(long *)NULL);
      
    /* Are we about to overwrite an existing attribute? */
    if(rcd == NC_NOERR){
      if(var_out_id == NC_GLOBAL){
	(void)fprintf(stderr,"%s: WARNING Overwriting global attribute %s\n",prg_nm_get(),att_nm);
      }else{
	char var_nm[NC_MAX_NAME];
	
	(void)nco_inq_varname(out_id,var_out_id,var_nm);
	(void)fprintf(stderr,"%s: WARNING Overwriting attribute %s for output variable %s\n",prg_nm_get(),att_nm,var_nm);
      } /* end else */
    } /* end if */

    (void)nco_copy_att(in_id,var_in_id,att_nm,out_id,var_out_id);

  } /* end loop over attributes */
} /* end att_cpy() */

void 
hst_att_cat /* [fnc] Add command line, date stamp to history attribute */
(const int out_id, /* I [id] netCDF output-file ID */
 const char * const hst_sng) /* I [sng] String to add to history attribute */
{
  /* Purpose: Add command line and date stamp to existing history attribute, if any,
   and write them to specified output file */

  /* Length of string + NUL required to old output of ctime() */
#define TIME_STAMP_SNG_LNG 25 

  char att_nm[NC_MAX_NAME];
  char *ctime_sng;
  char *history_crr=NULL;
  char *history_new;
  char time_stamp_sng[TIME_STAMP_SNG_LNG];
  char sng_history[]="history"; /* [sng] Possible name of history attribute */
  
  int idx;
  int nbr_glb_att;

  long att_sz=0;

  nc_type att_typ;
  
  time_t clock;

  /* Create timestamp string */
  clock=time((time_t *)NULL);
  ctime_sng=ctime(&clock);
  /* NUL-terminate time_stamp_sng */
  time_stamp_sng[TIME_STAMP_SNG_LNG-1]='\0';
  /* Get rid of carriage return in ctime_sng */
  (void)strncpy(time_stamp_sng,ctime_sng,TIME_STAMP_SNG_LNG-1);

  /* Get number of global attributes in file */
  (void)nco_inq(out_id,(int *)NULL,(int *)NULL,&nbr_glb_att,(int *)NULL);

  for(idx=0;idx<nbr_glb_att;idx++){
    (void)nco_inq_attname(out_id,NC_GLOBAL,idx,att_nm);
    if(!strcasecmp(att_nm,sng_history)) break;
  } /* end loop over att */

  /* Fill in history string */
  if(idx == nbr_glb_att){
    /* Global attribute "[hH]istory" does not yet exist */

    /* Add 3 for formatting characters */
    history_new=(char *)nco_malloc((strlen(hst_sng)+strlen(time_stamp_sng)+3)*sizeof(char));
    (void)sprintf(history_new,"%s: %s",time_stamp_sng,hst_sng);
  }else{ 
    /* Global attribute "[hH]istory" currently exists */
  
    /* NB: ncattinq(), unlike strlen(), counts terminating NUL for stored NC_CHAR arrays */
    (void)nco_inq_att(out_id,NC_GLOBAL,att_nm,&att_typ,&att_sz);
    if(att_typ != NC_CHAR){
      (void)fprintf(stderr,"%s: WARNING the \"%s\" global attribute is type %s, not %s. Therefore current command line will not be appended to %s in output file.\n",prg_nm_get(),att_nm,nco_typ_sng(att_typ),nco_typ_sng(NC_CHAR),att_nm);
      return;
    } /* end if */

    /* Allocate and NUL-terminate space for current history attribute
       If history attribute is of size zero then ensure strlen(history_crr) = 0 */
    history_crr=(char *)nco_malloc((att_sz+1)*sizeof(char));
    history_crr[att_sz]='\0';
    if(att_sz > 0) (void)nco_get_att(out_id,NC_GLOBAL,att_nm,(void *)history_crr,NC_CHAR);

    /* Add 4 for formatting characters */
    history_new=(char *)nco_malloc((strlen(history_crr)+strlen(hst_sng)+strlen(time_stamp_sng)+4)*sizeof(char));
    (void)sprintf(history_new,"%s: %s\n%s",time_stamp_sng,hst_sng,history_crr);
  } /* endif history global attribute currently exists */

  (void)nco_put_att(out_id,NC_GLOBAL,att_nm,NC_CHAR,strlen(history_new)+1,(void *)history_new);

  history_crr=(char *)nco_free(history_crr);
  history_new=(char *)nco_free(history_new);

} /* end hst_att_cat() */

