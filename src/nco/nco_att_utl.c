/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_att_utl.c,v 1.180 2015-01-22 21:44:57 dywei2 Exp $ */

/* Purpose: Attribute utilities */

/* Copyright (C) 1995--2015 Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   GNU General Public License (GPL) Version 3 with exceptions described in the LICENSE file */

#include "nco_att_utl.h" /* Attribute utilities */

/* Microsoft standard C library lacks standard definitions for strtoll(), strtoull()
   20140522 fxm: strtoll() and strtull() currently only used in nco_att_utl.c
   Yes Microsoft replacement functions are generic
   Should dangling definitions be placed in other header file, e.g., nco_rth_flt.h? */
#ifdef _MSC_VER
# define strtoull _strtoui64 
# define strtoll  _strtoi64
#endif /* !_MSC_VER */

void
nco_aed_prc /* [fnc] Process single attribute edit for single variable */
(const int nc_id, /* I [id] Input netCDF file ID */
 const int var_id, /* I [id] ID of variable on which to perform attribute editing */
 const aed_sct aed) /* I [sct] Structure containing information necessary to edit */
{
  /* Purpose: Process single attribute edit for single variable */
  
  /* If var_id == NC_GLOBAL ( = -1) then global attribute will be edited */
  
  const char fnc_nm[]="nco_aed_prc()"; /* [sng] Function name */
  
#ifdef NCO_NETCDF4_AND_FILLVALUE
  char att_nm_tmp[]="eulaVlliF_"; /* String of same length as "_FillValue" for netCDF4 name hack */
  nco_bool flg_fmt_netCDF4=False; /* [flg] File format is netCDF4 */
  nco_bool flg_netCDF4_rename_trick=False; /* [flg] Re-name _FillValue in order to create/modify/overwrite it */
#endif /* !NCO_NETCDF4_AND_FILLVALUE */
  
  char att_nm[NC_MAX_NAME];
  char var_nm[NC_MAX_NAME];
  
  /* NB: netCDF2 specifies att_sz is type int, netCDF3 and netCDF4 use size_t */
  int nbr_att; /* [nbr] Number of attributes */
  int rcd=NC_NOERR; /* [rcd] Return code */
  int rcd_inq_att=NC_NOERR; /* [rcd] Return code from nco_inq_att() */
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
  
  if(nco_dbg_lvl_get() >= nco_dbg_var && nco_dbg_lvl_get() != nco_dbg_dev) (void)fprintf(stdout,"%s: INFO %s examining variable %s\n",nco_prg_nm_get(),fnc_nm,var_nm);

  /* Query attribute metadata when attribute name was specified */
  if(aed.att_nm) rcd_inq_att=nco_inq_att_flg(nc_id,var_id,aed.att_nm,&att_typ,&att_sz);

  /* Before changing metadata, change missing values to new missing value if warranted 
     This capability is add-on feature not implemented too cleanly or efficiently
     If every variable has "_FillValue" attribute and "_FillValue" is changed
     globally, then algorithm goes into and out of define mode for each variable,
     rather than collecting all information in first pass and replacing all data in second pass.
     This is because ncatted was originally designed to change only metadata and so was
     architected differently from other NCO operators. */
  if(
     aed.att_nm /* Linux strcmp() dumps core if attribute name is blank */
     && strcmp(aed.att_nm,nco_mss_val_sng_get()) == 0 /* Current attribute is "_FillValue" */
     && var_id != NC_GLOBAL /* Current attribute is not global */
     && (aed.mode == aed_modify || aed.mode == aed_overwrite)  /* Modifying or overwriting existing value */
     && rcd_inq_att == NC_NOERR /* Only when existing _FillValue attribute is modified */
     && att_sz == 1L /* Old _FillValue attribute must be of size 1 */
     && aed.sz == 1L /* New _FillValue attribute must be of size 1 */
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

    if(nco_dbg_lvl_get() >= nco_dbg_std && nco_dbg_lvl_get() != nco_dbg_dev) (void)fprintf(stdout,"%s: INFO Replacing missing value data in variable %s\n",nco_prg_nm_get(),var_nm);

    /* Take file out of define mode */
    (void)nco_enddef(nc_id);
  
    /* Initialize (partially) variable structure */
    var=(var_sct *)nco_malloc(sizeof(var_sct));
    var->nc_id=nc_id;
    var->id=var_id;
    var->sz=1L;

    /* Get type of variable and number of dimensions */
    (void)nco_inq_var(nc_id,var_id,(char *)NULL,&var->type,&var->nbr_dim,(int *)NULL,(int *)NULL);
    dmn_id=(int *)nco_malloc(var->nbr_dim*sizeof(int));
    dmn_sz=(long *)nco_malloc(var->nbr_dim*sizeof(long int));
    dmn_srt=(long *)nco_malloc(var->nbr_dim*sizeof(long int));
    (void)nco_inq_vardimid(nc_id,var_id,dmn_id);

    /* Get dimension sizes and construct variable size */
    for(idx=0;idx<var->nbr_dim;idx++){
      (void)nco_inq_dimlen(nc_id,dmn_id[idx],dmn_sz+idx);
      var->sz*=dmn_sz[idx];
      dmn_srt[idx]=0L;
    } /* end loop over dim */
    var->dmn_id=dmn_id;
    var->cnt=dmn_sz;
    var->srt=dmn_srt;
      
    /* Place nco_var_get() code inline since var struct is not truly complete */
    if((var->val.vp=(void *)nco_malloc_flg(var->sz*nco_typ_lng(var->type))) == NULL){
      (void)fprintf(stdout,"%s: ERROR Unable to malloc() %ld*%lu bytes in %s\n",nco_prg_nm_get(),var->sz,(unsigned long)nco_typ_lng(var->type),fnc_nm);
      nco_exit(EXIT_FAILURE); 
    } /* end if */
    if(var->sz > 1L){
      (void)nco_get_vara(nc_id,var_id,var->srt,var->cnt,var->val.vp,var->type);
    }else{
      (void)nco_get_var1(nc_id,var_id,var->srt,var->val.vp,var->type);
    } /* end else */
    
    /* Get current missing value attribute */
    var->mss_val.vp=NULL;
    var->has_mss_val=nco_mss_val_get(nc_id,var);

    /* Sanity check */
    if(var->has_mss_val == False){
      (void)fprintf(stdout,"%s: ERROR variable %s does not have \"%s\" attribute in %s\n",nco_prg_nm_get(),var_nm,nco_mss_val_sng_get(),fnc_nm);
      nco_exit(EXIT_FAILURE);
    } /* end if */

printf("DYW, nco_aed_prc 0 %d %d %s\n", nc_id, var_id, var_nm);
    /* Shortcuts to avoid indirection */
    var_val=var->val;
    var_sz=var->sz;

    /* Get new and old missing values in same type as variable */
    mss_val_crr.vp=(void *)nco_malloc(att_sz*nco_typ_lng(var->type));
    mss_val_new.vp=(void *)nco_malloc(aed.sz*nco_typ_lng(var->type));

    (void)nco_val_cnf_typ(var->type,var->mss_val,var->type,mss_val_crr); 
    (void)nco_val_cnf_typ(aed.type,aed.val,var->type,mss_val_new);

    /* Typecast pointer to values before access */
    (void)cast_void_nctype(var->type,&var_val);
    (void)cast_void_nctype(var->type,&mss_val_crr);
    (void)cast_void_nctype(var->type,&mss_val_new);

printf("DYW, nco_aed_prc 1 %d %d %s\n", nc_id, var_id, var_nm);
    switch(var->type){
      /*    case NC_FLOAT: for(idx=0L;idx<var_sz;idx++) {if(var_val.fp[idx] == *mss_val_crr.fp) var_val.fp[idx]=*mss_val_new.fp;} break;*/
      /*    case NC_DOUBLE: for(idx=0L;idx<var_sz;idx++) {if(var_val.dp[idx] == *mss_val_crr.dp) var_val.dp[idx]=*mss_val_new.dp;} break;*/
    case NC_FLOAT: 
      if(isfinite(*mss_val_crr.fp)){
	for(idx=0L;idx<var_sz;idx++) {if(var_val.fp[idx] == *mss_val_crr.fp) var_val.fp[idx]=*mss_val_new.fp;}
      }else{ /* Old missing value is NaN-like so arithmetic comparisons are always false---must use macros */
	for(idx=0L;idx<var_sz;idx++) {if(!isfinite(var_val.fp[idx])) var_val.fp[idx]=*mss_val_new.fp;}
      } /* endif NaN */
      break;
    case NC_DOUBLE:
      if(isfinite(*mss_val_crr.dp)){
	for(idx=0L;idx<var_sz;idx++) {if(var_val.dp[idx] == *mss_val_crr.dp) var_val.dp[idx]=*mss_val_new.dp;}
      }else{ /* Old missing value is NaN-like so arithmetic comparisons are always false---must use macros */
	for(idx=0L;idx<var_sz;idx++) {if(!isfinite(var_val.dp[idx])) var_val.dp[idx]=*mss_val_new.dp;}
      } /* endif NaN */
      break;
    case NC_INT: for(idx=0L;idx<var_sz;idx++) {if(var_val.ip[idx] == *mss_val_crr.ip) var_val.ip[idx]=*mss_val_new.ip;} break;
    case NC_SHORT: for(idx=0L;idx<var_sz;idx++) {if(var_val.sp[idx] == *mss_val_crr.sp) var_val.sp[idx]=*mss_val_new.sp;} break;
    case NC_CHAR: for(idx=0L;idx<var_sz;idx++) {if(var_val.cp[idx] == *mss_val_crr.cp) var_val.cp[idx]=*mss_val_new.cp;} break;
    case NC_BYTE: for(idx=0L;idx<var_sz;idx++) {if(var_val.bp[idx] == *mss_val_crr.bp) var_val.bp[idx]=*mss_val_new.bp;} break;
    case NC_UBYTE: for(idx=0L;idx<var_sz;idx++) {if(var_val.ubp[idx] == *mss_val_crr.ubp) var_val.ubp[idx]=*mss_val_new.ubp;} break;
    case NC_USHORT: for(idx=0L;idx<var_sz;idx++) {if(var_val.usp[idx] == *mss_val_crr.usp) var_val.usp[idx]=*mss_val_new.usp;} break;
    case NC_UINT: for(idx=0L;idx<var_sz;idx++) {if(var_val.uip[idx] == *mss_val_crr.uip) var_val.uip[idx]=*mss_val_new.uip;} break;
    case NC_INT64: for(idx=0L;idx<var_sz;idx++) {if(var_val.i64p[idx] == *mss_val_crr.i64p) var_val.i64p[idx]=*mss_val_new.i64p;} break;
    case NC_UINT64: for(idx=0L;idx<var_sz;idx++) {if(var_val.ui64p[idx] == *mss_val_crr.ui64p) var_val.ui64p[idx]=*mss_val_new.ui64p;} break;
    case NC_STRING: for(idx=0L;idx<var_sz;idx++) {if(var_val.sngp[idx] == *mss_val_crr.sngp) var_val.sngp[idx]=*mss_val_new.sngp;} break;
    default: nco_dfl_case_nc_type_err(); break;
    } /* end switch */
printf("DYW, nco_aed_prc2 %d %d %s\n", nc_id, var_id, var_nm);

    /* Un-typecast the pointer to values after access */
    (void)cast_nctype_void(var->type,&var_val);
    (void)cast_nctype_void(var->type,&mss_val_crr);
    (void)cast_nctype_void(var->type,&mss_val_new);

    /* Write to disk */
    if(var->nbr_dim == 0){
      (void)nco_put_var1(nc_id,var_id,var->srt,var->val.vp,var->type);
    }else{ /* end if variable is scalar */
      (void)nco_put_vara(nc_id,var_id,var->srt,var->cnt,var->val.vp,var->type);
    } /* end else */

    /* Free memory */
    mss_val_crr.vp=nco_free(mss_val_crr.vp);
    mss_val_new.vp=nco_free(mss_val_new.vp);
    var->mss_val.vp=nco_free(var->mss_val.vp);
    var->val.vp=nco_free(var->val.vp);
    var->dmn_id=(int *)nco_free(var->dmn_id);
    var->srt=(long *)nco_free(var->srt);
    var->cnt=(long *)nco_free(var->cnt);
    /* 20050704 try and use nco_free() to avoid valgrind error message */
    var=(var_sct *)nco_free(var);

    /* Put file back in define mode */
    (void)nco_redef(nc_id);
  } /* end if replacing missing value data */

  /* Change metadata (as written, this must be done after _FillValue data is replaced) */

#ifdef NCO_NETCDF4_AND_FILLVALUE
  /* According to netCDF4 C Reference Manual:
     "Fill values must be written while the file is still in initial define mode, that
     is, after the file is created, but before it leaves define mode for the first time.
     NC EFILLVALUE is returned when the user attempts to set the fill value after
     it is too late." 
     The netCDF4/_FillValue code (and rename trick) works around that limitation. */

  /* Bold hack which gets around problem of modifying netCDF4 "_FillValue" attributes
     netCDF4 does not allow this by default, though netCDF3 does
     Change attribute name to att_nm_tmp, modify value, then restore name */

  /* Check if file is netCDF3 classic with netCDF4 library
     If so, do not employ rename trick 
     NB: create global- rather than local-scope variable for output file format? */
  { /* Temporary scope for fl_fmt */
    int fl_fmt;
    (void)nco_inq_format(nc_id,&fl_fmt);
    flg_fmt_netCDF4=(fl_fmt==NC_FORMAT_NETCDF4 || fl_fmt==NC_FORMAT_NETCDF4_CLASSIC);
  } /* end scope */

  if(
     flg_fmt_netCDF4 && /* Output file is netCDF4 and ... */
     aed.att_nm && /* 20130419: Verify att_nm exists before using it in strcmp() below. att_nm does not exist when user leaves field blank. Fix provided by Etienne Tourigny. */
     !strcmp(aed.att_nm,nco_mss_val_sng_get()) && /* ... attribute is missing value and ... */
     aed.mode != aed_delete){ /* ... we are not deleting attribute */
    /* Rename existing attribute to netCDF4-safe name 
       After modifying missing value attribute with netCDF4-safe name below, 
       we will rename attribute to original missing value name. */
    if(nco_dbg_lvl_get() >= nco_dbg_var && nco_dbg_lvl_get() != nco_dbg_dev) (void)fprintf(stdout,"%s: INFO %s reports creating, modifying, or overwriting %s attribute %s in netCDF4 file requires re-name trick\n",nco_prg_nm_get(),fnc_nm,var_nm,aed.att_nm);
    if(rcd_inq_att == NC_NOERR) (void)nco_rename_att(nc_id,var_id,aed.att_nm,att_nm_tmp);
    flg_netCDF4_rename_trick=True; /* [flg] Re-name _FillValue in order to create/modify/overwrite it */
    strcpy(aed.att_nm,att_nm_tmp); 
  } /* endif libnetCDF may have netCDF4 restrictions */
#endif /* !NCO_NETCDF4_AND_FILLVALUE */

  switch(aed.mode){
  case aed_append:	
    if(rcd_inq_att == NC_NOERR){
      /* Append to existing attribute value */
      if(aed.type != att_typ){
	(void)fprintf(stdout,"%s: ERROR %s attribute %s is of type %s not %s, unable to append\n",nco_prg_nm_get(),var_nm,aed.att_nm,nco_typ_sng(att_typ),nco_typ_sng(aed.type));
	nco_exit(EXIT_FAILURE);
      } /* end if */
      att_val_new=(void *)nco_malloc((att_sz+aed.sz)*nco_typ_lng(aed.type));
      (void)nco_get_att(nc_id,var_id,aed.att_nm,(void *)att_val_new,aed.type);
      /* 20120903: Handle trailing NULs for strings
	 Prevent interstitial NULs accumulation by overwriting NUL-terminator with first character of append string
	 When Behavior 1 is requested, this line removes NULs appended by strdup() in nco_prs_aed_lst() */
      if(aed.type == NC_CHAR)
	if(((char *)att_val_new)[att_sz-1L] == '\0') att_sz--;

      /* NB: Following assumes sizeof(char) = 1 byte */
      (void)memcpy((void *)((char *)att_val_new+att_sz*nco_typ_lng(aed.type)),
		   (void *)aed.val.vp,
		   aed.sz*nco_typ_lng(aed.type));
      rcd+=nco_put_att(nc_id,var_id,aed.att_nm,aed.type,att_sz+aed.sz,att_val_new);
      att_val_new=nco_free(att_val_new);
    }else{
      /* Create new attribute */
      rcd+=nco_put_att(nc_id,var_id,aed.att_nm,aed.type,aed.sz,aed.val.vp);
    } /* end else */
    break;
  case aed_create:	
    if(rcd_inq_att != NC_NOERR) rcd+=nco_put_att(nc_id,var_id,aed.att_nm,aed.type,aed.sz,aed.val.vp);  
    break;
  case aed_delete:	
    /* Delete specified attribute if attribute name was specified... */
    if(aed.att_nm){
      /* ...and if attribute is known to exist from previous inquire call... */
      if(rcd_inq_att == NC_NOERR) rcd+=nco_del_att(nc_id,var_id,aed.att_nm);
    }else{
      /* ...else delete all attributes for this variable... */
      while(nbr_att){
	rcd+=nco_inq_attname(nc_id,var_id,nbr_att-1,att_nm);
	rcd+=nco_del_att(nc_id,var_id,att_nm);
	nbr_att--;
      } /* end while */
    } /* end else */
    break;
  case aed_modify:	
    if(rcd_inq_att == NC_NOERR) rcd+=nco_put_att(nc_id,var_id,aed.att_nm,aed.type,aed.sz,aed.val.vp);
    break;
  case aed_overwrite:	
    rcd+=nco_put_att(nc_id,var_id,aed.att_nm,aed.type,aed.sz,aed.val.vp);  
    break;
  default: 
    break;
  } /* end switch */

#ifdef NCO_NETCDF4_AND_FILLVALUE
  if(flg_netCDF4_rename_trick){
    rcd+=nco_rename_att(nc_id,var_id,att_nm_tmp,nco_mss_val_sng_get());
    /* Restore original name (space already allocated) */
    strcpy(aed.att_nm,nco_mss_val_sng_get()); 
  } /* !flg_netCDF4_rename_trick */
#endif /* !NCO_NETCDF4_AND_FILLVALUE */

  if(rcd != NC_NOERR) (void)fprintf(stdout,"%s: DEBUG WARNING %s reports unexpected cumulative rcd = %i on exit. Please report this to NCO project.\n",nco_prg_nm_get(),fnc_nm,rcd);

} /* end nco_aed_prc() */

void 
nco_att_cpy  /* [fnc] Copy attributes from input netCDF file to output netCDF file */
(const int in_id, /* I [id] netCDF input-file ID */
 const int out_id, /* I [id] netCDF output-file ID */
 const int var_in_id, /* I [id] netCDF input-variable ID */
 const int var_out_id, /* I [id] netCDF output-variable ID */
 const nco_bool PCK_ATT_CPY) /* I [flg] Copy attributes "scale_factor", "add_offset" */
{
  /* Purpose: Copy attributes from input netCDF file to output netCDF file
     If var_in_id == NC_GLOBAL, then copy global attributes
     Otherwise copy only indicated variable's attributes
     When PCK_ATT_CPY is false, copy all attributes except "scale_factor", "add_offset" */

  char att_nm[NC_MAX_NAME];
  char var_nm[NC_MAX_NAME];

  int fl_fmt; /* [enm] Output file format */
  int idx;
  int nbr_att;
  int rcd; /* [enm] Return code */

  long att_sz;

  nc_type att_typ_in;
  nc_type att_typ_out;

  nco_bool flg_autoconvert;

  if(var_in_id == NC_GLOBAL){
    (void)nco_inq_natts(in_id,&nbr_att);
    if(nbr_att > NC_MAX_ATTRS) (void)fprintf(stdout,"%s: WARNING Number of global attributes is %d which exceeds number permitted by netCDF NC_MAX_ATTRS = %d\n",nco_prg_nm_get(),nbr_att,NC_MAX_ATTRS);
  }else{
    (void)nco_inq_varnatts(in_id,var_in_id,&nbr_att);
    if(nbr_att > 0) (void)nco_inq_varname(out_id,var_out_id,var_nm);
    if(nbr_att > NC_MAX_ATTRS) (void)fprintf(stdout,"%s: WARNING Variable %s has %d attributes which exceeds number permitted by netCDF NC_MAX_ATTRS = %d\n",nco_prg_nm_get(),var_nm,nbr_att,NC_MAX_ATTRS);
  } /* end else */

  /* Jump back here if current attribute is treated specially */
  for(idx=0;idx<nbr_att;idx++){
    (void)nco_inq_attname(in_id,var_in_id,idx,att_nm);
    (void)nco_inq_att(in_id,var_in_id,att_nm,&att_typ_in,&att_sz);

    /* Reset default behavior */
    flg_autoconvert=False;
    att_typ_out=att_typ_in;

    /* Look for same attribute in output variable in output file */
    rcd=nco_inq_att_flg(out_id,var_out_id,att_nm,(nc_type *)NULL,(long *)NULL);

    /* If attribute is "scale_factor" or "add_offset" ... */
    if(!strcmp(att_nm,"scale_factor") || !strcmp(att_nm,"add_offset")){
      /* ...and if instructed to copy packing attributes... */
      if(PCK_ATT_CPY){
	int nco_prg_id; /* [enm] Program ID */
	nco_prg_id=nco_prg_id_get(); /* [enm] Program ID */
	/* ...and if multifile concatenator (ncrcat, ncecat)... */
	if(nco_prg_id == ncrcat || nco_prg_id == ncecat){
	  /* ...then risk exists that packing attributes in first file do not match subsequent files... */
	  static short FIRST_WARNING=True;
	  if(FIRST_WARNING) (void)fprintf(stderr,"%s: INFO/WARNING Multi-file concatenator encountered packing attribute %s for variable %s. NCO copies the packing attributes from the first file to the output file. The packing attributes from the remaining files must match exactly those in the first file or data from subsequent files will not unpack correctly. Be sure all input files share the same packing attributes. If in doubt, unpack (with ncpdq -U) the input files, then concatenate them, then pack the result (with ncpdq). This message is printed only once per invocation.\n",nco_prg_nm_get(),att_nm,var_nm);
	  FIRST_WARNING=False;
	} /* endif ncrcat or ncecat */
      }else{ /* ...do not copy packing attributes... */
        /* ...then skip remainder of loop, thereby skipping attribute copy... */
        continue;
      } /* endelse */
    } /* endif attribute is "scale_factor" or "add_offset" */

    /* Inform user when copy will overwrite an existing attribute */
    if(nco_dbg_lvl_get() >= nco_dbg_std && nco_dbg_lvl_get() != nco_dbg_dev){
      if(rcd == NC_NOERR){
        if(var_out_id == NC_GLOBAL){
          (void)fprintf(stderr,"%s: INFO Overwriting global or group attribute %s\n",nco_prg_nm_get(),att_nm);
        }else{
          (void)fprintf(stderr,"%s: INFO Overwriting attribute %s for output variable %s\n",nco_prg_nm_get(),att_nm,var_nm);
        } /* end else */
      } /* end if */
    } /* end if dbg */

    /* File format needed for autoconversion */
    (void)nco_inq_format(out_id,&fl_fmt);

    /* Allow ncks to autoconvert netCDF4 atomic types to netCDF3 output type ... */
    if(nco_prg_id_get() == ncks && fl_fmt != NC_FORMAT_NETCDF4 && !nco_typ_nc3(att_typ_in)){
      att_typ_out=nco_typ_nc4_nc3(att_typ_in);
      flg_autoconvert=True;
      if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stdout,"%s: INFO Autoconverting %s%s attribute %s from netCDF4 type %s to netCDF3 type %s\n",nco_prg_nm_get(),(var_out_id == NC_GLOBAL) ? "global or group" : "variable ",(var_out_id == NC_GLOBAL) ? "" : var_nm,att_nm,nco_typ_sng(att_typ_in),nco_typ_sng(att_typ_out));
    } /* !flg_autoconvert */

    if(strcmp(att_nm,nco_mss_val_sng_get())){
      if(flg_autoconvert){
	var_sct att_var; /* [sct] Variable structure */
	var_sct *att_var_ptr=NULL; /* [sct] Variable structure */

	/* Initialize variable structure with minimal info. for nco_var_cnf_typ() */
	att_var.nm=att_nm;
	att_var.type=att_typ_in;
	att_var.has_mss_val=False;
	att_var.sz=att_sz;
        att_var.val.vp=(void *)nco_malloc(att_sz*nco_typ_lng(att_typ_in)); /* Pointer to attribute value */
        (void)nco_get_att(in_id,var_in_id,att_nm,att_var.val.vp,att_typ_in);

	if(att_typ_in == NC_STRING && att_typ_out == NC_CHAR){
	  /* Special case for string conversion:
	     Keep first string of existing attribute (netCDF3 output attribute can only hold one string) */
	  att_var.sz=att_sz=strlen(att_var.val.sngp[0]);
	  rcd=nco_put_att(out_id,var_out_id,att_nm,att_typ_out,att_sz,att_var.val.sngp[0]);
	  (void)cast_nctype_void(att_typ_out,&att_var.val);
	  if(att_var.val.vp) att_var.val.vp=(void *)nco_free(att_var.val.vp);
	}else{ /* !NC_STRING */
	  att_var_ptr=nco_var_cnf_typ(att_typ_out,&att_var);
	  rcd=nco_put_att(out_id,var_out_id,att_nm,att_typ_out,att_sz,att_var_ptr->val.vp);
	  if(att_var_ptr->val.vp) att_var_ptr->val.vp=nco_free(att_var_ptr->val.vp);
	} /* !NC_STRING */
      }else{
	/* Copy all attributes except _FillValue with fast library routine */
	(void)nco_copy_att(in_id,var_in_id,att_nm,out_id,var_out_id);
      } /* !Autoconvert */
    }else{
      /* Convert "_FillValue" attribute to unpacked type then copy 
	 Impose NCO convention that _FillValue is same type as variable,
	 whether variable is packed or not */
      aed_sct aed;

      size_t att_lng_in;

      ptr_unn mss_tmp;

      (void)nco_inq_att(in_id,var_in_id,att_nm,&att_typ_in,&att_sz);

      if(att_sz != 1L){
        (void)fprintf(stderr,"%s: WARNING input %s attribute has %li elements, but CF convention insists that %s be scalar (i.e., one element, possibly of compound type). Will attempt to copy using nco_copy_att(). HINT: If this fails, redefine %s as scalar.\n",nco_prg_nm_get(),att_nm,att_sz,att_nm,att_nm);
        (void)nco_copy_att(in_id,var_in_id,att_nm,out_id,var_out_id);
        return;
      } /* end if */

      /* Convert "_FillValue" to unpacked type before copying */
      aed.att_nm=att_nm; /* Name of attribute */
      if(var_out_id == NC_GLOBAL){
        aed.var_nm=NULL;
      }else{
        (void)nco_inq_varname(out_id,var_out_id,var_nm);
        aed.var_nm=var_nm; /* Name of variable, or NULL for global attribute */
      } /* end if */
      aed.id=out_id; /* Variable ID or NC_GLOBAL ( = -1) for global attribute */
      aed.sz=att_sz; /* Number of elements in attribute */

      if(!flg_autoconvert){
	/* Do not convert global attributes or PCK_ATT_CPY */  
	if(PCK_ATT_CPY || var_out_id==NC_GLOBAL) att_typ_out=att_typ_in; else (void)nco_inq_vartype(out_id,var_out_id,&att_typ_out);
      } /* flg_autoconvert */

      if(att_typ_out == att_typ_in){
        aed.type=att_typ_out; /* Type of attribute */
        aed.val.vp=(void *)nco_malloc(nco_typ_lng(aed.type)); /* Pointer to attribute value */
        (void)nco_get_att(in_id,var_in_id,att_nm,aed.val.vp,att_typ_out);
      }else{ /* att_typ_out!=att_typ_in */
        /* Convert type */          
        aed.type=att_typ_out; /* Type of attribute */
        aed.val.vp=(void *)nco_malloc(nco_typ_lng(aed.type)); /* Pointer to attribute value */
        att_lng_in=att_sz*nco_typ_lng(att_typ_in);
        mss_tmp.vp=(void *)nco_malloc(att_lng_in);
        (void)nco_get_att(in_id,var_in_id,att_nm,mss_tmp.vp,att_typ_in);
        (void)nco_val_cnf_typ(att_typ_in,mss_tmp,att_typ_out,aed.val);
        mss_tmp.vp=nco_free(mss_tmp.vp);
      } /* att_typ_out!=att_typ_in */

      /* Overwrite mode causes problems with netCDF4 and "_FillValue" 
	 Use create mode instead */
      aed.mode=aed_create;
      (void)nco_aed_prc(out_id,var_out_id,aed); 
      /* Release temporary memory */
      aed.val.vp=nco_free(aed.val.vp);
    } /* endif copying _FillValue */

  } /* end loop over attributes */
} /* end nco_att_cpy() */

void 
nco_fl_lst_att_cat /* [fnc] Add input file list global attribute */
(const int out_id, /* I [id] netCDF output-file ID */
 CST_X_PTR_CST_PTR_CST_Y(char,fl_lst_in), /* I [sng] Input file list */
 const int fl_nbr) /* I [nbr] Number of files in input file list */
{
  /* Purpose: Write input file list to global metadata */
  aed_sct fl_in_lst_aed;
  aed_sct fl_in_nbr_aed;
  char att_nm_lst[]="nco_input_file_list";
  char att_nm_nbr[]="nco_input_file_number";
  char spc_sng[]=" "; /* [sng] Intervening space */
  char *fl_in_lst_sng;
  nco_int fl_nbr_lng; /* [nbr] Number of files in input file list */
  int fl_idx;
  size_t fl_in_lst_sng_lng; /* [nbr] Filename list string length */
  ptr_unn att_val;
  
  /* Unfold file list into single string */
  fl_in_lst_sng_lng=0L; /* [nbr] Filename list string length */
  for(fl_idx=0;fl_idx<fl_nbr;fl_idx++){
    fl_in_lst_sng_lng+=strlen(fl_lst_in[fl_idx]);
  } /* end loop over fl */
  /* Make room for intervening spaces and for terminating NUL character */
  fl_in_lst_sng=(char *)nco_malloc((fl_in_lst_sng_lng+(fl_nbr-1L)+1L)*sizeof(char));
  fl_in_lst_sng[0]='\0';
  for(fl_idx=0;fl_idx<fl_nbr;fl_idx++){
    fl_in_lst_sng=strcat(fl_in_lst_sng,fl_lst_in[fl_idx]);
    if(fl_idx != fl_nbr-1) fl_in_lst_sng=strcat(fl_in_lst_sng,spc_sng);
  } /* end loop over fl */
  
  /* Copy fl_nbr so can take address without endangering number */
  fl_nbr_lng=fl_nbr;
  /* Insert number of files into value */
  att_val.ip=&fl_nbr_lng;
  /* Initialize nco_input_file_number attribute edit structure */
  fl_in_nbr_aed.att_nm=att_nm_nbr;
  fl_in_nbr_aed.var_nm=NULL;
  fl_in_nbr_aed.id=NC_GLOBAL;
  fl_in_nbr_aed.sz=1L;
  fl_in_nbr_aed.type=NC_INT;
  /* Insert value into attribute structure */
  fl_in_nbr_aed.val=att_val;
  fl_in_nbr_aed.mode=aed_overwrite;
  /* Write nco_input_file_number attribute to disk */
  (void)nco_aed_prc(out_id,NC_GLOBAL,fl_in_nbr_aed);

  /* Insert file list into value */
  att_val.cp=(nco_char *)fl_in_lst_sng;
  /* Initialize nco_input_file_list attribute edit structure */
  fl_in_lst_aed.att_nm=att_nm_lst;
  fl_in_lst_aed.var_nm=NULL;
  fl_in_lst_aed.id=NC_GLOBAL;
  fl_in_lst_aed.sz=(long int)strlen(fl_in_lst_sng)+1L;
  fl_in_lst_aed.type=NC_CHAR;
  /* Insert value into attribute structure */
  fl_in_lst_aed.val=att_val;
  fl_in_lst_aed.mode=aed_overwrite;
  /* Write nco_input_file_list attribute to disk */
  (void)nco_aed_prc(out_id,NC_GLOBAL,fl_in_lst_aed);
  
  /* Free string holding file list attribute */
  fl_in_lst_sng=(char *)nco_free(fl_in_lst_sng);
} /* end nco_fl_lst_att_cat() */
 
void 
nco_hst_att_cat /* [fnc] Add command line, date stamp to history attribute */
(const int out_id, /* I [id] netCDF output-file ID */
 const char * const hst_sng) /* I [sng] String to add to history attribute */
{
  /* Purpose: Add command line and date stamp to existing history attribute, if any,
     and write them to specified output file */
  
  /* Length of string + NUL required to hold output of ctime() */
#define TIME_STAMP_SNG_LNG 25 
  
  char att_nm[NC_MAX_NAME];
  char *ctime_sng;
  char *history_crr=NULL;
  char *history_new;
  char time_stamp_sng[TIME_STAMP_SNG_LNG];
  
  const char sng_history[]="history"; /* [sng] Possible name of history attribute */
  
  int idx;
  int glb_att_nbr;
  int rcd=NC_NOERR; /* [rcd] Return code */

  long att_sz=0L;

  nc_type att_typ;
  
  time_t time_crr_time_t;

  /* Create timestamp string */
  time_crr_time_t=time((time_t *)NULL);
  ctime_sng=ctime(&time_crr_time_t);
  /* NUL-terminate time_stamp_sng */
  time_stamp_sng[TIME_STAMP_SNG_LNG-1]='\0';
  /* Get rid of carriage return in ctime_sng */
  (void)strncpy(time_stamp_sng,ctime_sng,TIME_STAMP_SNG_LNG-1UL);

  /* Get number of global attributes in file */
  (void)nco_inq(out_id,(int *)NULL,(int *)NULL,&glb_att_nbr,(int *)NULL);

  for(idx=0;idx<glb_att_nbr;idx++){
    (void)nco_inq_attname(out_id,NC_GLOBAL,idx,att_nm);
    if(!strcasecmp(att_nm,sng_history)) break;
  } /* end loop over att */

  /* Fill-in history string */
  if(idx == glb_att_nbr){
    /* Global attribute "[hH]istory" does not yet exist */

    /* Add 3 for formatting characters */
    history_new=(char *)nco_malloc((strlen(hst_sng)+strlen(time_stamp_sng)+3UL)*sizeof(char));
    (void)sprintf(history_new,"%s: %s",time_stamp_sng,hst_sng);
    /* Set attribute name to default */
    (void)strcpy(att_nm,sng_history);

  }else{ 
    /* Global attribute "[hH]istory" currently exists */
  
    /* NB: ncattinq(), unlike strlen(), counts terminating NUL for stored NC_CHAR arrays */
    (void)nco_inq_att(out_id,NC_GLOBAL,att_nm,&att_typ,&att_sz);
    if(att_typ != NC_CHAR){
      if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stderr,"%s: WARNING the \"%s\" global attribute is type %s, not %s. Therefore current command line will not be appended to %s in output file.\n",nco_prg_nm_get(),att_nm,nco_typ_sng(att_typ),nco_typ_sng(NC_CHAR),att_nm);
      return;
    } /* end if */

    /* Allocate and NUL-terminate space for current history attribute
       If history attribute is of size zero then ensure strlen(history_crr) = 0 */
    history_crr=(char *)nco_malloc((att_sz+1)*sizeof(char));
    history_crr[att_sz]='\0';
    if(att_sz > 0) (void)nco_get_att(out_id,NC_GLOBAL,att_nm,(void *)history_crr,NC_CHAR);

    /* Add 4 for formatting characters */
    history_new=(char *)nco_malloc((strlen(history_crr)+strlen(hst_sng)+strlen(time_stamp_sng)+4UL)*sizeof(char));
    (void)sprintf(history_new,"%s: %s\n%s",time_stamp_sng,hst_sng,history_crr);
  } /* endif history global attribute currently exists */

  rcd+=nco_put_att(out_id,NC_GLOBAL,att_nm,NC_CHAR,(long int)(strlen(history_new)+1UL),(void *)history_new);

  history_crr=(char *)nco_free(history_crr);
  history_new=(char *)nco_free(history_new);

  return; /* 20050109: fxm added return to void function to squelch erroneous gcc-3.4.2 warning */ 
} /* end nco_hst_att_cat() */

aed_sct * /* O [sct] List of attribute edit structures */
nco_prs_aed_lst /* [fnc] Parse user-specified attribute edits into structure list */
(const int nbr_aed, /* I [nbr] Number of attributes in list */
 X_CST_PTR_CST_PTR_Y(char,aed_arg)) /* I/O [sng] List of user-specified attribute edits (delimiters are changed to NULL on output */
{
  /* Purpose: Parse name, type, size, and value elements of comma-separated list of attribute edit information
     Routine merely evaluates syntax of input expressions
     Routine does not validate attributes or variables against those present in input netCDF file */
  
  /* Options are:
     -a att_nm,var_nm,mode,att_typ,att_val (modifies attribute att_nm for the single variable var_nm)
     
     -a att_nm,,mode,att_typ,att_val (modifies attribute att_nm for every variable in file)
     If option -a is given with var_nm = NULL, then var_nm is expanded into every variable name in file
     Thus attribute editing operation is performed on every variable in file.
     
     mode,att_nm,att_typ,att_val (modifies global attribute att_nm for file)
     This option may be combined with modes -a, -c, -d, or -o to specify 
     appending to, changing, deleting, or overwriting, any existing global attribute named att_nm
     
     One mode must be set for each edited attribute: append (a), create (c), delete (d), modify (m), or overwrite (o).
     -a: Attribute append mode
     Append value att_val to current var_nm attribute att_nm value att_val, if any. 
     If var_nm does not have an attribute att_nm, there is no effect.
     
     -c: Attribute create mode
     Create variable var_nm attribute att_nm with att_val if att_nm does not yet exist. 
     If var_nm already has an attribute att_nm, there is no effect.
     
     -d: Attribute delete mode
     Delete current var_nm attribute att_nm.
     If var_nm does not have an attribute att_nm, there is no effect.
     
     -m: Attribute modify mode
     Change value of current var_nm attribute att_nm to value att_val.
     If var_nm does not have an attribute att_nm, there is no effect.
     
     -o: Attribute overwrite mode
     Write attribute att_nm with value att_val to variable var_nm, overwriting existing attribute att_nm, if any.
     This is default mode. */
  
  aed_sct *aed_lst;
  
  char **arg_lst;
  
  char *msg_sng=NULL_CEWI; /* [sng] Error message */
  
  const char * const dlm_sng=",";
  
  const long idx_att_val_arg=4L; /* Number of required delimiters preceding attribute values in -a argument list */
  
  int idx;
  int arg_nbr;
  
  long lmn;
  
  nco_bool ATT_TYP_INHERIT; /* [flg] Inherit attribute type from pre-existing attribute */
  nco_bool NCO_SYNTAX_ERROR=False; /* [flg] Syntax error in attribute-edit specification */
  
  aed_lst=(aed_sct *)nco_malloc(nbr_aed*sizeof(aed_sct));
  
  for(idx=0;idx<nbr_aed;idx++){
    ATT_TYP_INHERIT=False; /* [flg] Inherit attribute type from pre-existing attribute */
    
    /* Process attribute edit specifications as normal text list */
    arg_lst=nco_lst_prs_2D(aed_arg[idx],dlm_sng,&arg_nbr);
    
    /* Check syntax */
    if(arg_nbr < 5){ /* Need more info */
      msg_sng=strdup("Specification has fewer than five arguments---need more information");
      NCO_SYNTAX_ERROR=True;
    }else if(arg_lst[2] == NULL){ /* mode not specified ... */
      msg_sng=strdup("Mode must be explicitly specified");
      NCO_SYNTAX_ERROR=True;
    }else if(arg_lst[3] == NULL && *(arg_lst[2]) != 'd' && *(arg_lst[2]) != 'm'){
      msg_sng=strdup("Type must be explicitly specified for all modes except delete and modify");
      NCO_SYNTAX_ERROR=True;
    }else if(arg_lst[idx_att_val_arg] == NULL && *(arg_lst[2]) != 'd' && *(arg_lst[3]) == 'c'){
      /* ... value is not specified except that att_val = "" is valid for character type */
      msg_sng=strdup("Value must be explicitly specified for all modes except delete (although an empty string value is permissible for attributes of type NC_CHAR and NC_STRING)");
      NCO_SYNTAX_ERROR=True;
    } /* end else */

    if(NCO_SYNTAX_ERROR){
      (void)fprintf(stdout,"%s: ERROR in attribute edit specification %s: %s\n",nco_prg_nm_get(),aed_arg[idx],msg_sng);
      msg_sng=(char *)nco_free(msg_sng);
      nco_exit(EXIT_FAILURE);
    } /* !NCO_SYNTAX_ERROR */

    /* Initialize structure */
    /* aed strings not explicitly set by user remain NULL,
       i.e., specifying default setting appears as if nothing was set.
       Hopefully, in routines that follow, the branch followed by an aed for which
       all default settings were specified (e.g.,"-a foo,,,,") will yield same result
       as branch for which all defaults were set. */
    aed_lst[idx].att_nm=NULL;
    aed_lst[idx].var_nm=NULL;
    aed_lst[idx].val.vp=NULL;
    aed_lst[idx].type=NC_CHAR;
    aed_lst[idx].mode=aed_overwrite;
    aed_lst[idx].sz=-1L;
    aed_lst[idx].id=-1;

    /* Fill-in structure */
    if(arg_lst[0]) aed_lst[idx].att_nm=strdup(arg_lst[0]);
    if(arg_lst[1]) aed_lst[idx].var_nm=strdup(arg_lst[1]);

    /* fxm: Change these switches to string comparisons someday */
    /* Set mode of current aed structure */
    /* Convert single letter code to mode enum */
    /*    if(!strcmp("append",arg_lst[2])){aed_lst[idx].mode=aed_append;
	  }else if(!strcmp("create",arg_lst[2])){aed_lst[idx].mode=aed_create;
	  }else if(!strcmp("delete",arg_lst[2])){aed_lst[idx].mode=aed_delete;
	  }else if(!strcmp("modify",arg_lst[2])){aed_lst[idx].mode=aed_modify;
	  }else if(!strcmp("overwrite",arg_lst[2])){aed_lst[idx].mode=aed_overwrite;} */
    switch(*(arg_lst[2])){
    case 'a': aed_lst[idx].mode=aed_append; break;
    case 'c': aed_lst[idx].mode=aed_create; break;
    case 'd': aed_lst[idx].mode=aed_delete; break;
    case 'm': aed_lst[idx].mode=aed_modify; break;
    case 'o': aed_lst[idx].mode=aed_overwrite; break;
    default: 
      (void)fprintf(stderr,"%s: ERROR `%s' is not a supported mode\n",nco_prg_nm_get(),arg_lst[2]);
      (void)fprintf(stderr,"%s: HINT: Valid modes are `a' = append, `c' = create,`d' = delete, `m' = modify, and `o' = overwrite",nco_prg_nm_get());
      nco_exit(EXIT_FAILURE);
      break;
    } /* end switch */

    /* For modify-mode, type may be inherited when not explicitly specified */
    if(aed_lst[idx].mode == aed_modify && arg_lst[3] == NULL){
      /* 20120711: Problem with inherit is that nc_id info is needed now to get att_typ to inherit
      Hence file information/validation becomes involved here, earlier than expected */
      ATT_TYP_INHERIT=True;
      (void)fprintf(stderr,"%s: ERROR: Inherited attribute type not yet supported. TODO nco1060 fxm.",nco_prg_nm_get());
      nco_exit(EXIT_FAILURE);
    } /* !ATT_TYP_INHERIT */

    /* Attribute type and value do not matter if we are deleting it */
    if(aed_lst[idx].mode != aed_delete && !ATT_TYP_INHERIT){

      /* Set type of current aed structure */
      /* Convert single letter code to type enum */
      switch(*(arg_lst[3])){
      case 'F':	
      case 'f':	aed_lst[idx].type=(nc_type)NC_FLOAT; break;
      case 'D':	
      case 'd':	aed_lst[idx].type=(nc_type)NC_DOUBLE; break;
      case 'C':	
      case 'c':	aed_lst[idx].type=(nc_type)NC_CHAR; break;
      case 'B':	
      case 'b':	aed_lst[idx].type=(nc_type)NC_BYTE; break;
      default: 
        /* Ambiguous single letters must use full string comparisons */
        if(!strcasecmp(arg_lst[3],"l") || !strcasecmp(arg_lst[3],"i")) aed_lst[idx].type=(nc_type)NC_INT; 
        else if(!strcasecmp(arg_lst[3],"s")) aed_lst[idx].type=(nc_type)NC_SHORT; 
#ifdef ENABLE_NETCDF4
        else if(!strcasecmp(arg_lst[3],"ub")) aed_lst[idx].type=(nc_type)NC_UBYTE; 
        else if(!strcasecmp(arg_lst[3],"us")) aed_lst[idx].type=(nc_type)NC_USHORT; 
        else if(!strcasecmp(arg_lst[3],"u") || !strcasecmp(arg_lst[3],"ui") || !strcasecmp(arg_lst[3],"ul")) aed_lst[idx].type=(nc_type)NC_UINT; 
        else if(!strcasecmp(arg_lst[3],"ll") || !strcasecmp(arg_lst[3],"int64")) aed_lst[idx].type=(nc_type)NC_INT64; 
        else if(!strcasecmp(arg_lst[3],"ull") || !strcasecmp(arg_lst[3],"uint64")) aed_lst[idx].type=(nc_type)NC_UINT64; 
        else if(!strcasecmp(arg_lst[3],"sng") || !strcasecmp(arg_lst[3],"string")) aed_lst[idx].type=(nc_type)NC_STRING; 
        else{
          (void)fprintf(stderr,"%s: ERROR `%s' is not a supported netCDF data type\n",nco_prg_nm_get(),arg_lst[3]);
          (void)fprintf(stderr,"%s: HINT: Valid data types are `c' = char, `f' = float, `d' = double,`s' = short, `i' = `l' = integer, `b' = byte",nco_prg_nm_get());

          (void)fprintf(stderr,", `ub' = unsigned byte, `us' = unsigned short, `u' or `ui' or `ul' = unsigned int,`ll' or `int64' = 64-bit signed integer, `ull' or `uint64` = unsigned 64-bit integer, `sng' or `string' = string");

          (void)fprintf(stderr,"\n");
          nco_exit(EXIT_FAILURE);} /*  end if error */
#endif /* ENABLE_NETCDF4 */
        break;
      } /* end switch */
    } /* end if not delete mode and !ATT_TYP_INHERIT */

    if(aed_lst[idx].mode != aed_delete){

      /* Re-assemble string list values which inadvertently contain delimiters */
      if((aed_lst[idx].type == NC_CHAR) && arg_nbr > idx_att_val_arg+1){
        /* Number of elements which must be concatenated into single string value */
        long lmn_nbr;
        lmn_nbr=arg_nbr-idx_att_val_arg; 
        if(nco_dbg_lvl_get() >= nco_dbg_var) (void)fprintf(stdout,"%s: WARNING NC_CHAR (string) attribute is embedded with %li literal element delimiters (\"%s\"), re-assembling...\n",nco_prg_nm_get(),lmn_nbr-1L,dlm_sng);
        /* Rewrite list, splicing in original delimiter string */
        /* fxm: TODO nco527 ncatted memory may be lost here */
        arg_lst[idx_att_val_arg]=sng_lst_cat(arg_lst+idx_att_val_arg,lmn_nbr,dlm_sng);
        /* Keep bookkeeping straight, just in case */
        arg_nbr=idx_att_val_arg+1L;
        lmn_nbr=1L;
      } /* endif arg_nbr > idx_att_val_arg+1 */

      /* Replace any C language '\X' escape codes with ASCII bytes */
      if(aed_lst[idx].type == NC_CHAR || aed_lst[idx].type == NC_STRING) (void)sng_ascii_trn(arg_lst[idx_att_val_arg]);

      /* Set size of current aed structure */
      if(aed_lst[idx].type == NC_CHAR){
        /* 20100409 Remove extra space formerly allocated for NUL-terminator 
	   This caused each append to insert a NUL at end of NC_CHAR attributes
	   Multiple appends would then result in attributes pockmarked with NULs
	   Solves TODO nco985
	   Not yet sure there are no ill effects though...
	   20120902 Apparently this fixed some of append mode and broke other modes
	   Dave Allured reports on NCO Discussion forum that create, modify, and overwrite 
	   modes have not added NUL to NC_CHAR attributes since 4.0.2.
	   strdup() below attaches trailing NUL to user-specified string 
	   Retaining this NUL is obliquely discussed in netCDF Best Practices document:
	   http://www.unidata.ucar.edu/software/netcdf/docs/BestPractices.html#Strings%20and%20Variables%20of%20type%20char
	   Two behaviors are possible:
	   1. Terminate user-specified strings with NUL before saving as attributes
	   This behavior was used in 4.0.2-4.2.2 (201007-201210)
	   2. Do not NUL-terminate user-specified strings
	   This behavior was re-instituted in 4.3.0 (201204)
	   Between 201210 and 201304 the behavior was unclear to me...
	   ncgen may be interpreted as utilizing Behavior 2, since attributes in CDL are not NUL-terminated after conversion to netCDF binary format
	   As of 20130327, NCO chooses Behavior 2 (like ncgen)
	   To revert to Behavior 1, uncomment next line to create space for NUL-terminator */
        /* aed_lst[idx].sz=(arg_lst[idx_att_val_arg] == NULL) ? 1L : strlen(arg_lst[idx_att_val_arg])+1L; */ /* Behavior 1 */
        aed_lst[idx].sz=(arg_lst[idx_att_val_arg] == NULL) ? 0L : strlen(arg_lst[idx_att_val_arg])+0L; /* Behavior 2 (like ncgen) */
      }else{
        /* Number of elements of numeric types is determined by number of delimiters */
        aed_lst[idx].sz=arg_nbr-idx_att_val_arg;
      } /* end else */

      /* Set value of current aed structure */
      if(aed_lst[idx].type == NC_CHAR){
        /* strdup() attaches a trailing NUL to the user-specified string 
	   Retaining is obliquely discussed in netCDF Best Practices document:
	   http://www.unidata.ucar.edu/software/netcdf/docs/BestPractices.html#Strings%20and%20Variables%20of%20type%20char */
        aed_lst[idx].val.cp=(nco_char *)strdup(arg_lst[idx_att_val_arg]);
      }else if(aed_lst[idx].type == NC_STRING){
        aed_lst[idx].val.vp=(void *)nco_malloc(aed_lst[idx].sz*nco_typ_lng(aed_lst[idx].type));
        for(lmn=0L;lmn<aed_lst[idx].sz;lmn++){
          aed_lst[idx].val.sngp[lmn]=(nco_string)strdup(arg_lst[idx_att_val_arg+lmn]);
        } /* end loop over elements */
      }else{
        char *sng_cnv_rcd=NULL_CEWI; /* [sng] strtol()/strtoul() return code */
        double *val_arg_dbl=NULL_CEWI;
        long long *val_arg_lng_lng=NULL_CEWI;
        unsigned long long *val_arg_ulng_lng=NULL_CEWI;

        aed_lst[idx].val.vp=(void *)nco_malloc(aed_lst[idx].sz*nco_typ_lng(aed_lst[idx].type));

        /* Use type-appropriate conversion */
        switch(aed_lst[idx].type){
        case NC_FLOAT: 
        case NC_DOUBLE: 
          val_arg_dbl=(double *)nco_malloc(aed_lst[idx].sz*sizeof(double));
          for(lmn=0L;lmn<aed_lst[idx].sz;lmn++){
            val_arg_dbl[lmn]=strtod(arg_lst[idx_att_val_arg+lmn],&sng_cnv_rcd);
            if(*sng_cnv_rcd) nco_sng_cnv_err(arg_lst[idx_att_val_arg+lmn],"strtod",sng_cnv_rcd);
          } /* end loop over elements */
          break; 
        case NC_BYTE:
        case NC_INT: 
        case NC_SHORT: 
        case NC_INT64: 
          val_arg_lng_lng=(long long *)nco_malloc(aed_lst[idx].sz*sizeof(long long int));
          for(lmn=0L;lmn<aed_lst[idx].sz;lmn++){
            val_arg_lng_lng[lmn]=strtoll(arg_lst[idx_att_val_arg+lmn],&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
            if(*sng_cnv_rcd) nco_sng_cnv_err(arg_lst[idx_att_val_arg+lmn],"strtoll",sng_cnv_rcd);
          } /* end loop over elements */
          break;
        case NC_CHAR:
        case NC_UBYTE: 
        case NC_USHORT: 
        case NC_UINT: 
        case NC_UINT64: 
          val_arg_ulng_lng=(unsigned long long *)nco_malloc(aed_lst[idx].sz*sizeof(unsigned long long int));
          for(lmn=0L;lmn<aed_lst[idx].sz;lmn++){
            val_arg_ulng_lng[lmn]=strtoull(arg_lst[idx_att_val_arg+lmn],&sng_cnv_rcd,NCO_SNG_CNV_BASE10); 
            if(*sng_cnv_rcd) nco_sng_cnv_err(arg_lst[idx_att_val_arg+lmn],"strtoull",sng_cnv_rcd);
          } /* end loop over elements */
          break;
          /* fxm: 20130723 save whole string! */
        case NC_STRING: break;
        default: nco_dfl_case_nc_type_err(); break;
        } /* end switch */

        /* Copy and typecast entire array of values, using implicit coercion rules of C */
        /* 20011001: Use explicit coercion rules to quiet C++ compiler warnings */
        switch(aed_lst[idx].type){
        case NC_FLOAT: for(lmn=0L;lmn<aed_lst[idx].sz;lmn++) {aed_lst[idx].val.fp[lmn]=(float)val_arg_dbl[lmn];} break; 
        case NC_DOUBLE: for(lmn=0L;lmn<aed_lst[idx].sz;lmn++) {aed_lst[idx].val.dp[lmn]=(double)val_arg_dbl[lmn];} break; 
        case NC_INT: for(lmn=0L;lmn<aed_lst[idx].sz;lmn++) {aed_lst[idx].val.ip[lmn]=(nco_int)val_arg_lng_lng[lmn];} break; 
        case NC_SHORT: for(lmn=0L;lmn<aed_lst[idx].sz;lmn++) {aed_lst[idx].val.sp[lmn]=(nco_short)val_arg_lng_lng[lmn];} break; 
        case NC_CHAR: for(lmn=0L;lmn<aed_lst[idx].sz;lmn++) {aed_lst[idx].val.cp[lmn]=(nco_char)val_arg_ulng_lng[lmn];} break; 
        case NC_BYTE: for(lmn=0L;lmn<aed_lst[idx].sz;lmn++) {aed_lst[idx].val.bp[lmn]=(nco_byte)val_arg_lng_lng[lmn];} break; 
        case NC_UBYTE: for(lmn=0L;lmn<aed_lst[idx].sz;lmn++) {aed_lst[idx].val.ubp[lmn]=(nco_ubyte)val_arg_ulng_lng[lmn];} break; 
        case NC_USHORT: for(lmn=0L;lmn<aed_lst[idx].sz;lmn++) {aed_lst[idx].val.usp[lmn]=(nco_ushort)val_arg_ulng_lng[lmn];} break; 
        case NC_UINT: for(lmn=0L;lmn<aed_lst[idx].sz;lmn++) {aed_lst[idx].val.uip[lmn]=(nco_uint)val_arg_ulng_lng[lmn];} break; 
        case NC_INT64: for(lmn=0L;lmn<aed_lst[idx].sz;lmn++) {aed_lst[idx].val.i64p[lmn]=(nco_int64)val_arg_lng_lng[lmn];} break; 
        case NC_UINT64: for(lmn=0L;lmn<aed_lst[idx].sz;lmn++) {aed_lst[idx].val.ui64p[lmn]=(nco_uint64)val_arg_ulng_lng[lmn];} break; 
          /* fxm: 20130723 save whole string! */
        case NC_STRING: break;
        default: nco_dfl_case_nc_type_err(); break;
        } /* end switch */

        /* Free array used to hold input values */
        if(val_arg_dbl) val_arg_dbl=(double *)nco_free(val_arg_dbl);
        if(val_arg_lng_lng) val_arg_lng_lng=(long long *)nco_free(val_arg_lng_lng);
        if(val_arg_ulng_lng) val_arg_ulng_lng=(unsigned long long *)nco_free(val_arg_ulng_lng);
      } /* end else */
      /* Un-typecast pointer to values after access */
      (void)cast_nctype_void(aed_lst[idx].type,&aed_lst[idx].val);

    } /* end if mode is not delete */

    arg_lst=nco_sng_lst_free(arg_lst,arg_nbr);

  } /* end loop over aed */

  if(nco_dbg_lvl_get() >= nco_dbg_io){
    for(idx=0;idx<nbr_aed;idx++){
      (void)fprintf(stderr,"aed_lst[%d].att_nm = %s\n",idx,aed_lst[idx].att_nm);
      (void)fprintf(stderr,"aed_lst[%d].var_nm = %s\n",idx,aed_lst[idx].var_nm == NULL ? "NULL" : aed_lst[idx].var_nm);
      (void)fprintf(stderr,"aed_lst[%d].id = %i\n",idx,aed_lst[idx].id);
      (void)fprintf(stderr,"aed_lst[%d].sz = %li\n",idx,aed_lst[idx].sz);
      (void)fprintf(stderr,"aed_lst[%d].type = %s\n",idx,nco_typ_sng(aed_lst[idx].type));
      /*      (void)fprintf(stderr,"aed_lst[%d].val = %s\n",idx,aed_lst[idx].val);*/
      (void)fprintf(stderr,"aed_lst[%d].mode = %i\n",idx,aed_lst[idx].mode);
    } /* end loop over idx */
  } /* end debug */

  return aed_lst;

} /* end nco_prs_aed_lst() */

int /* [flg] Error code */
nco_prs_att /* [fnc] Parse conjoined object and attribute names */
(rnm_sct * const rnm_att, /* I/O [sct] Structure [Object@]Attribute name on input, Attribute name on output */
 char * const obj_nm, /* O [sng] Object name, if any */
 nco_bool *mch_grp_all, /* O [flg] Rename all group attributes */
 nco_bool *mch_grp_glb, /* O [flg] Rename only global attributes */
 nco_bool *mch_obj_all) /* O [flg] Rename all group and variable attributes */
{
  /* Purpose: Check if attribute name space contains object name before attribute name in form obj_nm@att_nm
     Object name is then extracted from from old_nm and new_nm as necessary
     Valid syntax of combined string is actually [.][obj_nm]@att_nm
     Preceding period means (to ncrename) that object need not be present
     obj_nm is optional and, if omitted, the attribute is assumed to be a global (or group) attribute
     In this case the obj_nm string is set to "global" for (unused) compatibility with ncatted
     Also, the case-independent obj_nm "global" on input, is assumed to mean a global attribute
     NB: Function replaces delimiter character in input by NUL so that, on output, rnm_att is just attribute name */
  
  const char dlm_chr='@'; /* Character delimiting object from attribute name  */
  char *dlm_ptr; /* Delimiter pointer */

  nco_bool obj_nm_abb=False; /* [flg] Object name is an abbreviation */

  size_t att_nm_lng;
  size_t obj_nm_lng;

  /* Initialize obj_nm to NUL */
  *obj_nm='\0';

  dlm_ptr=strchr(rnm_att->old_nm,dlm_chr);	
  if(!dlm_ptr) *mch_obj_all=True;
  
  att_nm_lng=strlen(rnm_att->old_nm);
  
  /* Return if delimiter appears to be part of attribute name */
  if(att_nm_lng < 3L || dlm_ptr == rnm_att->old_nm+att_nm_lng-1L) return NCO_ERR;

  if(dlm_ptr){
    /* Handle cases where object string makes use of abbreviations */
    if(dlm_ptr == rnm_att->old_nm){
      /* Strings that start with '@' imply all groups (and no variables) */
      strcpy(obj_nm,"group");
    }else if(!strncasecmp(rnm_att->old_nm,".@",2L)){
      /* Strings that start with '.@' imply all groups are optional (and no variables) */
      strcpy(obj_nm,".group");
    } /* end else abbreviations */

    /* NUL-terminate object name in user-supplied string */
    *dlm_ptr='\0';
    
    if(!obj_nm_abb){
      /* If input object name was not abbreviated, copy it to output object name */
      obj_nm_lng=strlen(rnm_att->old_nm);
      if(obj_nm_lng > NC_MAX_NAME){
	(void)fprintf(stdout,"%s: ERROR Derived object name \"%s\" too long\n",nco_prg_nm_get(),rnm_att->old_nm);
	nco_exit(EXIT_FAILURE);
      } /* end if */ 
      /* Copy object name only */
      strcpy(obj_nm,rnm_att->old_nm);
    } /* obj_nm_abb */

    /* Output object name now searchable for standard hints */
    if(!strncasecmp(obj_nm,"global",6L)) *mch_grp_glb=True;
    else if(!strncasecmp(obj_nm,".global",7L)) *mch_grp_glb=True;
    else if(!strncasecmp(obj_nm,"group",5L)) *mch_grp_all=True;
    else if(!strncasecmp(obj_nm,".group",6L)) *mch_grp_all=True;

    /* Point old obj_nm@att_nm name to attribute name att_nm alone */
    rnm_att->old_nm=dlm_ptr+1L; 
    
  } /* endif contains '@' */
  
  /* Delimiter '@' is optional in new name */
  dlm_ptr=strchr(rnm_att->new_nm,dlm_chr);	
  if(dlm_ptr){
    /* When found in new name, point the user string to next character */
    att_nm_lng=strlen(rnm_att->new_nm);
    if((dlm_ptr-rnm_att->new_nm) < (long int)att_nm_lng) rnm_att->new_nm=dlm_ptr+1L; else return NCO_ERR;
  } /* endif */

  return NCO_NOERR;
} /* end nco_prs_att() */

char * /* O [sng] Stub of GPE applied to input path */
nco_gpe_evl_stb /* [fnc] Apply Group Path Editing (GPE) to argument safely */
(const gpe_sct * const gpe, /* I [sng] GPE structure, if any */
 const char * const grp_nm_fll_in) /* I [sng] Full group name */
{
  /* Purpose: Apply Group Path Editing (GPE) to input full group name, return stub of result
     NB: Function designed to make it easy for ncks to print GPE-transformed group stubs 
     Hence returned strings may be used directly in print functions without pointers being save and free()'d
     This kind of memory leak is acceptable since it scales only with total number of groups in printed file,
     and does not scale with data size. */

  const char fnc_nm[]="nco_gpe_evl_stb()"; /* [sng] Function name */
  const char sls_chr='/'; /* [sng] Slash character */

  char *grp_nm_stb_out; /* [sng] Returned output name. Must be free()'d in calling routine. */
  char *grp_nm_fll_out; /* [sng] Intermediate output name. Allocated and free()'d here. */
  char *sls_ptr; /* [sng] Pointer to slash character. Unsafe to free(). */

  size_t in_lng; /* [nbr] Length of grp_nm_fll_in */

  in_lng=strlen(grp_nm_fll_in);
  if(!in_lng) (void)fprintf(stdout,"%s: WARNING %s reports grp_nm_fll_in is empty\n",nco_prg_nm_get(),fnc_nm);
  grp_nm_fll_out=nco_gpe_evl(gpe,grp_nm_fll_in);

  /* Root group has no stub */
  if(in_lng == 1UL) return grp_nm_fll_out;

  /* First level and deeper group stubs begin just after final slash */
  sls_ptr=strrchr(grp_nm_fll_out,sls_chr);
  assert(sls_ptr);
  grp_nm_stb_out=(char *)strdup(sls_ptr+1UL);

  if(grp_nm_fll_out) grp_nm_fll_out=(char *)nco_free(grp_nm_fll_out);

  return grp_nm_stb_out;

} /* end nco_gpe_evl_stb() */

char * /* O [sng] Result of applying GPE to input path */
nco_gpe_evl /* [fnc] Apply Group Path Editing (GPE) to argument */
(const gpe_sct * const gpe, /* I [sng] GPE structure */
 const char * const grp_nm_fll_in) /* I [sng] Full group name */
{
  /* Purpose: Apply Group Path Editing (GPE) to input full group name, return result
     grp_nm_fll_in:  "Input"  path---usually full group path of object in input  file
     grp_nm_fll_out: "Output" path---usually full group path of object in output file
     grp_nm_fll_out: Calling routine is responsible for freeing this memory
     grp_nm_fll_in usually is not terminated by a slash
     grp_nm_fll_in is assumed to begin with a slash
     The "group path" includes only groups--variable names should be omitted
     GPE string is the path argument supplied to the GPE option */

  const char fnc_nm[]="nco_gpe_evl()"; /* [sng] Function name */
  const char sls_sng[]="/"; /* [sng] Slash string */
  const char sls_chr='/'; /* [sng] Slash character */

  char *grp_nm_fll_out=NULL; /* [sng] Returned output name. Must be free()'d in calling routine. */
  char *grp_out; /* [sng] Mutable pointer to output name. Unsafe to free(). */
  char *grp_nm_fll_in_dpl; /* [sng] Allocated pointer to output name. Allocated and free()'d here. */
  char *sls_ptr; /* [sng] Pointer to slash character. Unsafe to free(). */
  
  int lvl_idx=0; /* [idx] Level counter (gets incremented) */
  
  size_t in_lng; /* [nbr] Length of grp_nm_fll_in */
  size_t out_lng; /* [nbr] Length of grp_nm_fll_out */

  /* Default */
  in_lng=strlen(grp_nm_fll_in);
  if(!in_lng) (void)fprintf(stdout,"%s: WARNING %s reports grp_nm_fll_in is empty\n",nco_prg_nm_get(),fnc_nm);
  grp_nm_fll_in_dpl=(char *)strdup(grp_nm_fll_in);
  sls_ptr=grp_out=grp_nm_fll_in_dpl;

  /* Prevent attempts to dereference NULL. perform identity translation and return */
  if(!gpe) return grp_nm_fll_in_dpl;

  /* If GPE was not invoked, perform identity translation and return */
  if(!gpe->arg) return grp_nm_fll_in_dpl;

  /* Sanity checks */
  if(grp_out[0] != '/') (void)fprintf(stdout,"%s: WARNING %s reports GPE input path %s does not begin with slash\n",nco_prg_nm_get(),fnc_nm,grp_out);

  switch(gpe->md){
  case gpe_delete:
    /* Delete up to lvl_nbr levels by advancing one slash per level */
    while(grp_out && (lvl_idx++ < gpe->lvl_nbr)) grp_out=strchr(grp_out+1,sls_chr);

    if(grp_out){
      /* More levels remain */
      if(gpe->lng_cnn > 1L){
	/* Prepend argument to remaining levels */
	out_lng=gpe->lng_cnn+strlen(grp_out);
	grp_nm_fll_out=(char *)nco_malloc((out_lng+1L)*sizeof(char));
	(void)strcpy(grp_nm_fll_out,gpe->nm_cnn);
	grp_nm_fll_out=strcat(grp_nm_fll_out,grp_out);
      }else{
	/* No argument to prepend */
	grp_nm_fll_out=(char *)strdup(grp_out);
      } /* !gpe->nm */
    }else{ /* !grp_out */
      /* All levels already deleted */
      if(gpe->nm_cnn) grp_nm_fll_out=(char *)strdup(gpe->nm_cnn); else grp_nm_fll_out=(char *)strdup(sls_sng);
    } /* !grp_out */
    break;

  case gpe_append:
    /* Append: Append input name to GPE name */
    out_lng=gpe->lng_cnn+in_lng;
    grp_nm_fll_out=(char *)nco_malloc((out_lng+1L)*sizeof(char));
    (void)strcpy(grp_nm_fll_out,gpe->nm_cnn);
    grp_nm_fll_out=strcat(grp_nm_fll_out,grp_nm_fll_in);
    break;

  case gpe_flatten:
    /* Delete all levels */
    if(gpe->nm_cnn) grp_nm_fll_out=(char *)strdup(gpe->nm_cnn); else grp_nm_fll_out=(char *)strdup(sls_sng);
    break;

  case gpe_backspace:
    /* Truncate up-to lvl_nbr levels by backspacing one slash per level */
    while(sls_ptr && (lvl_idx++ < gpe->lvl_nbr)){
      sls_ptr=strrchr(grp_out+1,sls_chr);
      /* Replace slash by NUL */
      if(sls_ptr) *sls_ptr='\0';
    } /* end while */

    if(sls_ptr){
      /* More levels remain */
      if(gpe->lng_cnn > 1L){
	/* Append argument to remaining levels */
	out_lng=strlen(grp_out)+gpe->lng_cnn;
	grp_nm_fll_out=(char *)nco_malloc((out_lng+1L)*sizeof(char));
	(void)strcpy(grp_nm_fll_out,grp_out);
	grp_nm_fll_out=strcat(grp_nm_fll_out,gpe->nm_cnn);
      }else{ /* !gpe->nm */
	/* No argument to append */
	grp_nm_fll_out=(char *)strdup(grp_out);
      } /* !gpe->nm */
    }else{ /* !sls_ptr */
      /* All levels already truncated */
      if(gpe->nm_cnn) grp_nm_fll_out=(char *)strdup(gpe->nm_cnn); else grp_nm_fll_out=(char *)strdup(sls_sng);
    } /* !sls_ptr */
    break;

  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */

  /* Final parsing results */
  if(nco_dbg_lvl_get() >= nco_dbg_scl){
    (void)fprintf(stdout,"%s: INFO %s reports GPE changes input grp_nm_fll_in = %s to output grp_nm_fll_out = %s\n",nco_prg_nm_get(),fnc_nm,grp_nm_fll_in,grp_nm_fll_out);
  } /* end if */

  if(grp_nm_fll_in_dpl) grp_nm_fll_in_dpl=(char *)nco_free(grp_nm_fll_in_dpl);

  return grp_nm_fll_out;

} /* end nco_gpe_evl() */

gpe_sct * /* O [sct] Structure with dynamic memory free()'d */
nco_gpe_free /* [fnc] Free dynamic memory of GPE structure */
(gpe_sct * gpe) /* O [sct] GPE structure */
{
  /* Purpose: Free GPE structure */
  if(gpe->arg) gpe->arg=(char *)nco_free(gpe->arg);
  if(gpe->nm) gpe->nm=(char *)nco_free(gpe->nm);
  if(gpe->edt) gpe->edt=(char *)nco_free(gpe->edt);
  if(gpe->nm_cnn) gpe->nm_cnn=(char *)nco_free(gpe->nm_cnn);
  if(gpe) gpe=(gpe_sct *)nco_free(gpe);
  return gpe;
} /* end nco_gpe_free() */

gpe_sct * /* O [sng] GPE structure */
nco_gpe_prs_arg /* [fnc] Parse Group Path Editing (GPE) argument */
(const char * const gpe_arg) /* I [sng] User-specified GPE specification */
{
  /* Purpose: Parse user-specified Group Path Editing (GPE) argument
     ncks -O -D 3 -G test ~/nco/data/in_grp.nc ~/foo.nc
     ncks -O -D 3 -G g2:1 ~/nco/data/in_grp.nc ~/foo.nc
     ncks -O -D 3 -G @-2 ~/nco/data/in_grp.nc ~/foo.nc */

  const char fnc_nm[]="nco_gpe_prs_arg()"; /* [sng] Function name */

  char *at_cp;
  char *colon_cp;
  char *spr_cp=NULL; /* [sng] Separator location */

  gpe_sct *gpe; /* [sct] GPE structure */

  /* Initialize structure */
  /* NB: Be sure to free() all dynamic memory in nco_gpe_free() */
  gpe=(gpe_sct *)nco_malloc(sizeof(gpe_sct));
  gpe->arg=NULL; /* [sng] Full GPE specification (for debugging) */
  gpe->edt=NULL; /* [sng] Editing component of full GPE specification */
  gpe->nm=NULL; /* [sng] Group name component of full GPE specification */
  gpe->nm_cnn=NULL; /* [sng] Canonicalized (i.e., slash-prefixed) group name */
  gpe->lng=0L; /* [nbr] Length of user-specified group path */
  gpe->lng_cnn=0L; /* [nbr] Length of canonicalized user-specified group path */
  gpe->lng_edt=0L; /* [nbr] Length of editing component of full GPE specification */
  gpe->lvl_nbr=0; /* [nbr] Number of levels to shift */
  gpe->md=gpe_append; /* [enm] Editing mode to perform */
  
  /* Structure has been initialized, safe to return now on no-ops */
  if(gpe_arg) gpe->arg=(char *)strdup(gpe_arg); else return gpe;

  /* Find positions of commas and number of characters between (non-inclusive) them */
  colon_cp=strchr(gpe->arg,':');
  at_cp=strchr(gpe->arg,'@');

  /* Basic sanity checks */
  if(colon_cp && at_cp){
    (void)fprintf(stdout,"%s: ERROR %s reports GPE specification \"%s\" contains both a colon ':' and an at-sign '@'\n",nco_prg_nm_get(),fnc_nm,gpe->arg);
    nco_exit(EXIT_FAILURE);
  } /* end if */
  
  if(colon_cp){
    gpe->md=gpe_delete; /* [enm] Editing mode to perform */
    spr_cp=colon_cp; /* [sng] Separator location */
  } /* endif colon */

  if(at_cp){
    gpe->md=gpe_backspace; /* [enm] Editing mode to perform */
    spr_cp=at_cp; /* [sng] Separator location */
  } /* endif at */

  if(spr_cp){
    char *sng_cnv_rcd=NULL_CEWI; /* [sng] strtol()/strtoul() return code */
    char *nbr_sng; /* [sng] Number string */
    size_t nbr_lng;

    gpe->nm=(char *)nco_malloc(((spr_cp-gpe->arg)+1L)*sizeof(char)); /* [sng] Group name */
    gpe->nm=(char *)strncpy(gpe->nm,gpe->arg,spr_cp-gpe->arg); /* [sng] Group name */
    gpe->nm[spr_cp-gpe->arg]='\0'; /* [sng] Group name */

    /* Is there anything after the separator? */
    nbr_sng=spr_cp+1L; /* [sng] Number string */
    nbr_lng=strlen(nbr_sng);

    if(nbr_lng){
      /* Convert it */
      gpe->lvl_nbr=(int)strtol(nbr_sng,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
      if(*sng_cnv_rcd) nco_sng_cnv_err(nbr_sng,"strtol",sng_cnv_rcd);
    } /* end if */

    if(gpe->lvl_nbr < 0 && colon_cp){
      /* Equate negative number following colon to positive number following at-sign  */
      gpe->md=gpe_backspace; /* [enm] Editing mode to perform */
      gpe->lvl_nbr=-gpe->lvl_nbr;
    } /* end if */

    if(gpe->lvl_nbr < 0){
      (void)fprintf(stdout,"%s: ERROR %s reports GPE level shift number gpe->lvl_nbr = %d is less than zero. Level shift number must not be negative.\n",nco_prg_nm_get(),fnc_nm,gpe->lvl_nbr);
      nco_exit(EXIT_FAILURE);
    } /* end if */

    if(colon_cp && !nbr_lng) gpe->md=gpe_flatten; /* [enm] Editing mode to perform */

    if(at_cp && !nbr_lng) (void)fprintf(stdout,"%s: WARNING %s reports GPE specification \"%s\" specifies no level after the at-sign '@'\n",nco_prg_nm_get(),fnc_nm,gpe->arg);

    gpe->edt=(char *)strdup(spr_cp); /* [sng] Editing component of full GPE specification */
    gpe->lng_edt=strlen(gpe->edt); /* [nbr] Length of editing component of full GPE specification */
  } /* !spr_cp */

  if(!spr_cp){
    gpe->nm=(char *)strdup(gpe->arg); /* [sng] Group name */
    gpe->md=gpe_append; /* [enm] Editing mode to perform */
  } /* spr_cp */
  
  /* Name is known, save length for later use */
  gpe->lng=strlen(gpe->nm); /* [nbr] Length of user-specified group path */

  /* Obtain "canonical" (slash-prefixed) name */

  /* Slash-initiate name */
  if(gpe->nm[0] == '/'){
    gpe->nm_cnn=(char *)strdup(gpe->nm);
    gpe->lng_cnn=gpe->lng;
  }else{
    gpe->lng_cnn=gpe->lng+1L;
    gpe->nm_cnn=(char *)nco_malloc((gpe->lng_cnn+1L)*sizeof(char));
    (void)strcpy(gpe->nm_cnn+1L,gpe->nm);
    gpe->nm_cnn[0]='/';
  } /* endif */

#if 0  
  /* Slash-terminate name */
  if(gpe->nm[gpe->lng_cnn-1L] == '/'){
    gpe->nm_cnn=(char *)strdup(gpe->nm);
    gpe->lng_cnn=gpe->lng;
  }else{
    gpe->lng_cnn=gpe->lng+1L;
    gpe->nm_cnn=(char *)nco_malloc((gpe->lng_cnn+1L)*sizeof(char));
    (void)strcpy(gpe->nm_cnn,gpe->nm);
    gpe->nm_cnn[gpe->lng_cnn-1L]='/';
    gpe->nm_cnn[gpe->lng_cnn]='\0';
  } /* endif */
#endif /* !0 */  

  /* Final parsing results */
  if(nco_dbg_lvl_get() >= nco_dbg_scl){
    (void)fprintf(stdout,"%s: INFO %s reports gpe->arg = %s\n",nco_prg_nm_get(),fnc_nm,gpe->arg);
    (void)fprintf(stdout,"%s: INFO %s reports gpe->nm = %s\n",nco_prg_nm_get(),fnc_nm,gpe->nm);
    (void)fprintf(stdout,"%s: INFO %s reports gpe->lng = %zi\n",nco_prg_nm_get(),fnc_nm,gpe->lng);
    (void)fprintf(stdout,"%s: INFO %s reports gpe->nm_cnn = %s\n",nco_prg_nm_get(),fnc_nm,gpe->nm_cnn);
    (void)fprintf(stdout,"%s: INFO %s reports gpe->lng_cnn = %zi\n",nco_prg_nm_get(),fnc_nm,gpe->lng_cnn);
    (void)fprintf(stdout,"%s: INFO %s reports gpe->edt = %s\n",nco_prg_nm_get(),fnc_nm,gpe->edt);
    (void)fprintf(stdout,"%s: INFO %s reports gpe->lng_edt = %zi\n",nco_prg_nm_get(),fnc_nm,gpe->lng_edt);
    (void)fprintf(stdout,"%s: INFO %s reports gpe->md = %s\n",nco_prg_nm_get(),fnc_nm,nco_gpe_sng(gpe->md));
    (void)fprintf(stdout,"%s: INFO %s reports gpe->lvl_nbr = %i\n",nco_prg_nm_get(),fnc_nm,gpe->lvl_nbr);
  } /* end if */

  return gpe;
} /* end nco_gpe_prs_arg() */

const char * /* O [sng] String describing GPE */
nco_gpe_sng /* [fnc] Convert GPE enum to string */
(const gpe_enm gpe_md) /* I [enm] GPE mode */
{
  switch(gpe_md){
  case gpe_delete:
    return "gpe_delete";
  case gpe_append:
    return "gpe_append";
  case gpe_flatten:
    return "gpe_flatten";
  case gpe_backspace:
    return "gpe_backspace";
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */

  /* Some compilers, e.g., SGI cc, need return statement to end non-void functions */
  return (char *)NULL;
} /* end nco_gpe_sng() */

rnm_sct * /* O [sng] Structured list of old, new names */
nco_prs_rnm_lst /* [fnc] Set old_nm, new_nm elements of rename structure */
(const int nbr_rnm, /* I [nbr] Number of elements in rename list */
 char * const * const rnm_arg) /* I [sng] Unstructured list of old, new names */
{
  /* Purpose: Set old_nm, new_nm elements of rename structure
     Routine merely fills rename structure and does not attempt to validate 
     presence of variables in input netCDF file. */

  rnm_sct *rnm_lst;

  int idx;

  ptrdiff_t lng_arg_1;
  ptrdiff_t lng_arg_2;

  rnm_lst=(rnm_sct *)nco_malloc((size_t)nbr_rnm*sizeof(rnm_sct));

  for(idx=0;idx<nbr_rnm;idx++){
    char *comma_1_cp;

    /* Find positions of commas and number of characters between (non-inclusive) them */
    comma_1_cp=strchr(rnm_arg[idx],',');
    
    /* Before doing any pointer arithmetic, make sure pointers are valid */
    if(comma_1_cp == NULL){
      (void)nco_usg_prn();
      nco_exit(EXIT_FAILURE);
    } /* end if */
    
    lng_arg_1=comma_1_cp-rnm_arg[idx]; 

    /* If new name is absolute path, get rid of it (ncrename does in place, same group, changes, so absolute path is redundant) */
    char *abs_1_cp=strrchr(comma_1_cp,'/');
    /* Found path separator in new name (string after ',' ) */
    if (abs_1_cp){
      comma_1_cp=strrchr(comma_1_cp,'/');
    }

    lng_arg_2=rnm_arg[idx]+strlen(rnm_arg[idx])-comma_1_cp-1; 
    
    /* Exit if length of either argument is zero */
    if(lng_arg_1 <= 0L || lng_arg_2 <= 0L){
      (void)nco_usg_prn();
      nco_exit(EXIT_FAILURE);
    } /* end if */
    
    /* Assign pointers to member of rnm_lst */
    rnm_lst[idx].old_nm=rnm_arg[idx];
    rnm_lst[idx].new_nm=comma_1_cp+1;

    /* NUL-terminate arguments */
    rnm_lst[idx].old_nm[lng_arg_1]='\0';
    rnm_lst[idx].new_nm[lng_arg_2]='\0';
    
  } /* end loop over rnm_lst */

  if(nco_dbg_lvl_get() >= nco_dbg_io){
    for(idx=0;idx<nbr_rnm;idx++){
      (void)fprintf(stderr,"%s\n",rnm_lst[idx].old_nm);
      (void)fprintf(stderr,"%s\n",rnm_lst[idx].new_nm);
    } /* end loop over idx */
  } /* end debug */

  return rnm_lst;
} /* end nco_prs_rnm_lst() */

void 
nco_vrs_att_cat /* [fnc] Add NCO version global attribute */
(const int out_id) /* I [id] netCDF output-file ID */
{
  /* Purpose: Write NCO version information to global metadata */
  aed_sct vrs_sng_aed;
  char att_nm[]="NCO";
  char *vrs_cvs; /* [sng] Version according to CVS release tag */
  char *vrs_sng; /* [sng] NCO version */
  ptr_unn att_val;
  
  vrs_cvs=cvs_vrs_prs();
  vrs_sng=vrs_cvs;

  /* Insert thread number into value */
  att_val.cp=vrs_sng;
  /* Initialize nco_openmp_thread_number attribute edit structure */
  vrs_sng_aed.att_nm=att_nm;
  vrs_sng_aed.var_nm=NULL;
  vrs_sng_aed.id=NC_GLOBAL;
  vrs_sng_aed.sz=strlen(vrs_sng)+1L;
  vrs_sng_aed.type=NC_CHAR;
  /* Insert value into attribute structure */
  vrs_sng_aed.val=att_val;
  vrs_sng_aed.mode=aed_overwrite;
  /* Write nco_openmp_thread_number attribute to disk */
  (void)nco_aed_prc(out_id,NC_GLOBAL,vrs_sng_aed);
  vrs_sng=(char *)nco_free(vrs_sng);

} /* end nco_vrs_att_cat() */

void 
nco_thr_att_cat /* [fnc] Add threading global attribute */
(const int out_id, /* I [id] netCDF output-file ID */
 const int thr_nbr) /* I [nbr] Thread number */
{
  /* Purpose: Write thread information to global metadata */
  aed_sct thr_nbr_aed;
  char att_nm_nbr[]="nco_openmp_thread_number";
  nco_int thr_nbr_lng; /* [nbr] Thread number copy */
  ptr_unn att_val;
  
  /* Copy thr_nbr so can take address without endangering number */
  thr_nbr_lng=thr_nbr;
  /* Insert thread number into value */
  att_val.ip=&thr_nbr_lng;
  /* Initialize nco_openmp_thread_number attribute edit structure */
  thr_nbr_aed.att_nm=att_nm_nbr;
  thr_nbr_aed.var_nm=NULL;
  thr_nbr_aed.id=NC_GLOBAL;
  thr_nbr_aed.sz=1L;
  thr_nbr_aed.type=NC_INT;
  /* Insert value into attribute structure */
  thr_nbr_aed.val=att_val;
  thr_nbr_aed.mode=aed_overwrite;
  /* Write nco_openmp_thread_number attribute to disk */
  (void)nco_aed_prc(out_id,NC_GLOBAL,thr_nbr_aed);

} /* end nco_thr_att_cat() */
 
void 
nco_mpi_att_cat /* [fnc] Add MPI tasks global attribute */
(const int out_id, /* I [id] netCDF output-file ID */
 const int mpi_nbr) /* I [nbr] MPI nodes/tasks number */
{
  /* Purpose: Write number of tasks information to global metadata */
  aed_sct mpi_nbr_aed;
  char att_nm_nbr[]="nco_mpi_task_number";
  nco_int mpi_nbr_lng; /* [nbr] MPI tasks number copy */
  ptr_unn att_val;
  
  /* Copy mpi_nbr so can take address without endangering number */
  mpi_nbr_lng=mpi_nbr;
  /* Insert tasks number into value */
  att_val.ip=&mpi_nbr_lng;
  /* Initialize nco_mpi_task_number attribute edit structure */
  mpi_nbr_aed.att_nm=att_nm_nbr;
  mpi_nbr_aed.var_nm=NULL;
  mpi_nbr_aed.id=NC_GLOBAL;
  mpi_nbr_aed.sz=1L;
  mpi_nbr_aed.type=NC_INT;
  /* Insert value into attribute structure */
  mpi_nbr_aed.val=att_val;
  mpi_nbr_aed.mode=aed_overwrite;
  /* Write nco_mpi_tasks_number attribute to disk */
  (void)nco_aed_prc(out_id,NC_GLOBAL,mpi_nbr_aed);

} /* end nco_mpi_att_cat() */
