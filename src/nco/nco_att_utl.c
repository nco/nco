/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_att_utl.c,v 1.114 2011-12-07 18:40:29 zender Exp $ */

/* Purpose: Attribute utilities */

/* Copyright (C) 1995--2011 Charlie Zender
   License: GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */

#include "nco_att_utl.h" /* Attribute utilities */

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
  char att_nm_tmp[]="eulaVlliF_"; /* String of same length as "_FillValue" for name hack with netCDF4 */
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

  if(dbg_lvl_get() >= nco_dbg_var) (void)fprintf(stdout,"%s: INFO %s examining variable %s\n",prg_nm_get(),fnc_nm,var_nm);

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

    if(dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stdout,"%s: INFO Replacing missing value data in variable %s\n",prg_nm_get(),var_nm);

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
      (void)fprintf(stdout,"%s: ERROR Unable to malloc() %ld*%lu bytes in %s\n",prg_nm_get(),var->sz,(unsigned long)nco_typ_lng(var->type),fnc_nm);
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
      (void)fprintf(stdout,"%s: ERROR variable %s does not have \"%s\" attribute in %s\n",prg_nm_get(),var_nm,nco_mss_val_sng_get(),fnc_nm);
      nco_exit(EXIT_FAILURE);
    } /* end if */

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
  
    switch(var->type){
    case NC_FLOAT: for(idx=0L;idx<var_sz;idx++) {if(var_val.fp[idx] == *mss_val_crr.fp) var_val.fp[idx]=*mss_val_new.fp;} break;
    case NC_DOUBLE: for(idx=0L;idx<var_sz;idx++) {if(var_val.dp[idx] == *mss_val_crr.dp) var_val.dp[idx]=*mss_val_new.dp;} break;
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
     !strcmp(aed.att_nm,nco_mss_val_sng_get()) && /* ... attribute is missing value and ... */
     aed.mode != aed_delete){ /* ... we are not deleting attribute */
    /* Rename existing attribute to netCDF4-safe name 
       After modifying missing value attribute with netCDF4-safe name below, 
       we will rename attribute to original missing value name. */
    if(dbg_lvl_get() >= nco_dbg_var) (void)fprintf(stdout,"%s: INFO %s reports creating, modifying, or overwriting %s attribute %s in netCDF4 file requires re-name trick\n",prg_nm_get(),fnc_nm,var_nm,aed.att_nm);
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

  if(rcd != NC_NOERR) (void)fprintf(stdout,"%s: DEBUG WARNING %s reports unexpected cumulative rcd = %i on exit. Please report this to NCO project.\n",prg_nm_get(),fnc_nm,rcd);

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

  int idx;
  int nbr_att;
  int rcd; /* [enm] Return code */

  if(var_in_id == NC_GLOBAL){
    (void)nco_inq_natts(in_id,&nbr_att);
  }else{
    (void)nco_inq_varnatts(in_id,var_in_id,&nbr_att);
  } /* end else */
  
  if(nbr_att > 0 && var_out_id != NC_GLOBAL) (void)nco_inq_varname(out_id,var_out_id,var_nm);

  /* Jump back to here if current attribute is treated specially */
  for(idx=0;idx<nbr_att;idx++){
    (void)nco_inq_attname(in_id,var_in_id,idx,att_nm);
    /* Look for same attribute in output variable in output file */
    rcd=nco_inq_att_flg(out_id,var_out_id,att_nm,(nc_type *)NULL,(long *)NULL);
      
    /* If attribute is "scale_factor" or "add_offset" ... */
    if(!strcmp(att_nm,"scale_factor") || !strcmp(att_nm,"add_offset")){
      /* ...and if instructed to copy packing attributes... */
      if(PCK_ATT_CPY){
	int prg_id; /* [enm] Program ID */
	prg_id=prg_get(); /* [enm] Program ID */
	/* ...and if multifile concatenator (ncrcat, ncecat)... */
	if(prg_id == ncrcat || prg_id == ncecat){
	  /* ...then risk exists that packing attributes in first file do not match subsequent files... */
	  static short FIRST_WARNING=True;
	  if(FIRST_WARNING) (void)fprintf(stderr,"%s: INFO/WARNING Multi-file concatenator encountered packing attribute %s for variable %s. NCO copies the packing attributes from the first file to the output file. The packing attributes from the remaining files must match exactly those in the first file or the data from the subsequent files will not be unpacked correctly. Be sure that all input files share the same packing attributes. If in doubt, unpack (with ncpdq -U) the input files, then concatenate them, then pack the result (with ncpdq). This message is printed only once per invocation.\n",prg_nm_get(),att_nm,var_nm);
	  FIRST_WARNING=False;
	} /* endif ncrcat or ncecat */
      }else{ /* ...do not copy packing attributes... */
	/* ...then skip remainder of loop, thereby skipping attribute copy... */
	continue;
      } /* endelse */
    } /* endif attribute is "scale_factor" or "add_offset" */
    
    /* Inform user when copy will overwrite an existing attribute */
    if(dbg_lvl_get() >= nco_dbg_std){
      if(rcd == NC_NOERR){
	if(var_out_id == NC_GLOBAL){
	  (void)fprintf(stderr,"%s: INFO Overwriting global attribute %s\n",prg_nm_get(),att_nm);
	}else{
	  (void)fprintf(stderr,"%s: INFO Overwriting attribute %s for output variable %s\n",prg_nm_get(),att_nm,var_nm);
	} /* end else */
      } /* end if */
    } /* end if dbg */

    if(strcmp(att_nm,nco_mss_val_sng_get())){
      /* Copy all attributes except _FillValue with fast library routine */
      (void)nco_copy_att(in_id,var_in_id,att_nm,out_id,var_out_id);
    }else{
      /* Convert "_FillValue" attribute to unpacked type then copy 
	 Impose NCO convention that _FillValue is same type as variable,
	 whether variable is packed or not */
      aed_sct aed;
      
      long att_sz;
      size_t att_lng_in;
      
      nc_type att_typ_in;
      nc_type att_typ_out;
      
      ptr_unn mss_tmp;
      
      (void)nco_inq_att(in_id,var_in_id,att_nm,&att_typ_in,&att_sz);
      
      if(att_sz != 1L){
	(void)fprintf(stderr,"%s: ERROR input \"%s\" attribute has %li elements, but nco_att_cpy() only works for size of 1\n",prg_nm_get(),att_nm,att_sz);
	nco_exit(EXIT_FAILURE); 
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

      /* Do not convert global attributes or PCK_ATT_CPY */  
      if(PCK_ATT_CPY || var_out_id==NC_GLOBAL) att_typ_out=att_typ_in; else (void)nco_inq_vartype(out_id,var_out_id,&att_typ_out);

      if(att_typ_out==att_typ_in){
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

  long att_sz=0;

  nc_type att_typ;
  
  time_t time_crr_time_t;

  /* Create timestamp string */
  time_crr_time_t=time((time_t *)NULL);
  ctime_sng=ctime(&time_crr_time_t);
  /* NUL-terminate time_stamp_sng */
  time_stamp_sng[TIME_STAMP_SNG_LNG-1]='\0';
  /* Get rid of carriage return in ctime_sng */
  (void)strncpy(time_stamp_sng,ctime_sng,TIME_STAMP_SNG_LNG-1);

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
    history_new=(char *)nco_malloc((strlen(hst_sng)+strlen(time_stamp_sng)+3)*sizeof(char));
    (void)sprintf(history_new,"%s: %s",time_stamp_sng,hst_sng);
    /* Set attribute name to default */
    (void)strcpy(att_nm,sng_history);

  }else{ 
    /* Global attribute "[hH]istory" currently exists */
  
    /* NB: ncattinq(), unlike strlen(), counts terminating NUL for stored NC_CHAR arrays */
    (void)nco_inq_att(out_id,NC_GLOBAL,att_nm,&att_typ,&att_sz);
    if(att_typ != NC_CHAR){
      if(dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stderr,"%s: WARNING the \"%s\" global attribute is type %s, not %s. Therefore current command line will not be appended to %s in output file.\n",prg_nm_get(),att_nm,nco_typ_sng(att_typ),nco_typ_sng(NC_CHAR),att_nm);
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

  (void)nco_put_att(out_id,NC_GLOBAL,att_nm,NC_CHAR,(long int)(strlen(history_new)+1UL),(void *)history_new);

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
     If var_nm does not have an attribute att_nm, there is not effect.

     -c: Attribute create mode
     Create variable var_nm attribute att_nm with att_val if att_nm does not yet exist. 
     If var_nm already has an attribute att_nm, there is not effect.

     -d: Attribute delete mode
     Delete current var_nm attribute att_nm.
     If var_nm does not have an attribute att_nm, there is not effect.

     -m: Attribute modify mode
     Change value of current var_nm attribute att_nm to value att_val.
     If var_nm does not have an attribute att_nm, there is not effect.

     -o: Attribute overwrite mode
     Write attribute att_nm with value att_val to variable var_nm, overwriting existing attribute att_nm, if any.
     This is default mode.
   */

  char **arg_lst;

  const char * const dlm_sng=",";

  const long idx_att_val_arg=4L; /* Number of required delimiters preceding attribute values in -a argument list */

  aed_sct *aed_lst;

  int idx;
  int arg_nbr;

  aed_lst=(aed_sct *)nco_malloc(nbr_aed*sizeof(aed_sct));

  for(idx=0;idx<nbr_aed;idx++){

    /* Process attribute edit specifications as normal text list */
    arg_lst=nco_lst_prs_2D(aed_arg[idx],dlm_sng,&arg_nbr);

    /* Check syntax */
    if(
       arg_nbr < 5 || /* Need more info */
       /* arg_lst[0] == NULL || */ /* att_nm not specified */
       arg_lst[2] == NULL || /* mode not specified */
       (*(arg_lst[2]) != 'd' && (arg_lst[3] == NULL || (arg_lst[idx_att_val_arg] == NULL && *(arg_lst[3]) != 'c'))) || /* att_typ and att_val must be specified when mode is not delete, except that att_val = "" is valid for character type */
       False){
      (void)fprintf(stdout,"%s: ERROR in attribute edit specification %s\n",prg_nm_get(),aed_arg[idx]);
      nco_exit(EXIT_FAILURE);
    } /* end if */

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
      (void)fprintf(stderr,"%s: ERROR `%s' is not a supported mode\n",prg_nm_get(),arg_lst[2]);
      (void)fprintf(stderr,"%s: HINT: Valid modes are `a' = append, `c' = create,`d' = delete, `m' = modify, and `o' = overwrite",prg_nm_get());
      nco_exit(EXIT_FAILURE);
      break;
    } /* end switch */

    /* Attribute type and value do not matter if we are deleting it */
    if(aed_lst[idx].mode != aed_delete){

      /* Set type of current aed structure */
      /* Convert single letter code to type enum */
      switch(*(arg_lst[3])){
      case 'f':	aed_lst[idx].type=(nc_type)NC_FLOAT; break;
      case 'd':	aed_lst[idx].type=(nc_type)NC_DOUBLE; break;
      case 'l':	
      case 'i':	aed_lst[idx].type=(nc_type)NC_INT; break;
      case 's':	aed_lst[idx].type=(nc_type)NC_SHORT; break;
      case 'c':	aed_lst[idx].type=(nc_type)NC_CHAR; break;
      case 'b':	aed_lst[idx].type=(nc_type)NC_BYTE; break;
      default: 
	if(!strcasecmp(arg_lst[3],"ub")) aed_lst[idx].type=(nc_type)NC_UBYTE; 
	else if(!strcasecmp(arg_lst[3],"us")) aed_lst[idx].type=(nc_type)NC_USHORT; 
	else if(!strcasecmp(arg_lst[3],"u") || !strcasecmp(arg_lst[3],"ui") || !strcasecmp(arg_lst[3],"ul")) aed_lst[idx].type=(nc_type)NC_UINT; 
	else if(!strcasecmp(arg_lst[3],"ll") || !strcasecmp(arg_lst[3],"int64")) aed_lst[idx].type=(nc_type)NC_INT64; 
	else if(!strcasecmp(arg_lst[3],"ull") || !strcasecmp(arg_lst[3],"uint64")) aed_lst[idx].type=(nc_type)NC_UINT64; 
	else if(!strcasecmp(arg_lst[3],"sng")) aed_lst[idx].type=(nc_type)NC_STRING; 
	else{
	  (void)fprintf(stderr,"%s: ERROR `%s' is not a supported netCDF data type\n",prg_nm_get(),arg_lst[3]);
	  (void)fprintf(stderr,"%s: HINT: Valid data types are `c' = char, `f' = float, `d' = double,`s' = short, 'l' = `i' = integer, `b' = byte",prg_nm_get());
	  nco_exit(EXIT_FAILURE);} /*  end if error */
      break;
      } /* end switch */
      
      /* Re-assemble string list values which inadvertently contain delimiters */
      if(aed_lst[idx].type == NC_CHAR && arg_nbr > idx_att_val_arg+1){
	/* Number of elements which must be concatenated into single string value */
	long lmn_nbr;
	lmn_nbr=arg_nbr-idx_att_val_arg; 
	if(dbg_lvl_get() >= nco_dbg_var) (void)fprintf(stdout,"%s: WARNING NC_CHAR (string) attribute is embedded with %li literal element delimiters (\"%s\"), re-assembling...\n",prg_nm_get(),lmn_nbr-1L,dlm_sng);
	/* Rewrite list, splicing in original delimiter string */
	/* fxm: TODO nco527 ncatted memory may be lost here */
	arg_lst[idx_att_val_arg]=sng_lst_cat(arg_lst+idx_att_val_arg,lmn_nbr,dlm_sng);
	/* Keep bookkeeping straight, just in case */
	arg_nbr=idx_att_val_arg+1L;
	lmn_nbr=1L;
      } /* endif arg_nbr > idx_att_val_arg+1 */
      
      /* Replace any C language '\X' escape codes with ASCII bytes */
      if(aed_lst[idx].type == NC_CHAR) (void)sng_ascii_trn(arg_lst[idx_att_val_arg]);

      /* Set size of current aed structure */
      if(aed_lst[idx].type == NC_CHAR){
	/* 20100409 Remove extra space formerly allocated for NUL-terminator 
	   This caused each append to insert a NUL at end of NC_CHAR attributes
	   Multiple appends would then result in attributes pockmarked with NULs
	   Solves TODO nco985
	   Not yet sure there are no ill side-effects though... */ 
	aed_lst[idx].sz=(arg_lst[idx_att_val_arg] == NULL) ? 0L : strlen(arg_lst[idx_att_val_arg]);
      }else{
	/* Number of elements of numeric types is determined by number of delimiters */
	aed_lst[idx].sz=arg_nbr-idx_att_val_arg;
      } /* end else */
      
      /* Set value of current aed structure */
      if(aed_lst[idx].type == NC_CHAR){
	aed_lst[idx].val.cp=(nco_char *)strdup(arg_lst[idx_att_val_arg]);
      }else{
	char *sng_cnv_rcd=char_CEWI; /* [sng] strtol()/strtoul() return code */
	double *val_arg_dbl=NULL_CEWI;
	long long *val_arg_lng_lng=NULL_CEWI;
	unsigned long long *val_arg_ulng_lng=NULL_CEWI;
	
	long lmn;
	
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
  
  if(dbg_lvl_get() >= nco_dbg_io){
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

int /* [flg] Variable and attribute names are conjoined */
nco_prs_att /* [fnc] Parse conjoined variable and attribute names */
(rnm_sct * const rnm_att, /* I/O [sct] Structure [Variable:]Attribute name on input, Attribute name on output */
 char * const var_nm) /* O [sng] Variable name, if any */
{
  /* Purpose: Check if attribute name space contains variable name before attribute name of form var_nm:att_nm
     Attribute name is then extracted from from old_nm and new_nm as necessary */
  
  char *dlm_ptr; /* Ampersand pointer */

  size_t att_nm_lng;
  size_t var_nm_lng;

  dlm_ptr=strchr(rnm_att->old_nm,'@');	
  if(dlm_ptr == NULL) return 0;
  
  att_nm_lng=strlen(rnm_att->old_nm);
  
  /* Return if ampersand appears to be part of attribute name */
  if(att_nm_lng < 3 || dlm_ptr == rnm_att->old_nm || dlm_ptr == rnm_att->old_nm+att_nm_lng-1) return 0;

  /* NUL-terminate variable name */
  *dlm_ptr='\0';
  var_nm_lng=strlen(rnm_att->old_nm);
  if(var_nm_lng > NC_MAX_NAME){
    (void)fprintf(stdout,"%s: ERROR Derived variable name \"%s\" too long\n",prg_nm_get(),rnm_att->old_nm);
    nco_exit(EXIT_FAILURE);
  } /* end if */ 

  /* Copy variable name only */
  strcpy(var_nm,rnm_att->old_nm);
  /* Set to attribute name alone */
  rnm_att->old_nm=dlm_ptr+1; 
    
  dlm_ptr=strchr(rnm_att->new_nm,'@');	
  if(dlm_ptr){
    att_nm_lng=strlen(rnm_att->new_nm);
    if((dlm_ptr-rnm_att->new_nm) < (long int)att_nm_lng) rnm_att->new_nm=dlm_ptr+1; else return 0;
  } /* endif */
  return 1;
} /* end nco_prs_att() */

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
    lng_arg_2=rnm_arg[idx]+strlen(rnm_arg[idx])-comma_1_cp-1; 
    
    /* Exit if length of either argument is zero */
    if(lng_arg_1 <= 0 || lng_arg_2 <= 0){
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

  if(dbg_lvl_get() >= nco_dbg_io){
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
