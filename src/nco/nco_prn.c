/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_prn.c,v 1.86 2013-02-28 23:25:51 zender Exp $ */

/* Purpose: Printing variables, attributes, metadata */

/* Copyright (C) 1995--2013 Charlie Zender
   License: GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */

#include "nco_prn.h" /* Printing variables, attributes, metadata */
#include "nco_grp_utl.h" /* Group utilities */

void 
nco_prn_att /* [fnc] Print all attributes of single variable or group */
(const int in_id, /* I [id] netCDF file ID */
 const int grp_id, /* I [id] netCDF group ID */
 const int var_id) /* I [id] netCDF input variable ID */
{
  /* Purpose: Print all global attributes in netCDF group,
     or all attributes for particular netCDF variable. 
     If var_id == NC_GLOBAL ( = -1) then global/group attributes are printed,
     otherwise variable's attributes are printed. */

  att_sct *att=NULL_CEWI;

  char dlm_sng[3];
  char src_sng[NC_MAX_NAME];
  char att_sng[NCO_MAX_LEN_FMT_SNG];

  long att_lmn;
  long att_sz;
  
  int idx;
  int nbr_att;

  if(var_id == NC_GLOBAL){
    /* Get number of global attributes in group */
    (void)nco_inq(grp_id,(int *)NULL,(int *)NULL,&nbr_att,(int *)NULL);
    if(in_id == grp_id) (void)strcpy(src_sng,"Global"); else (void)strcpy(src_sng,"Group");
  }else{
    /* Get name and number of attributes for variable */
    (void)nco_inq_var(grp_id,var_id,src_sng,(nc_type *)NULL,(int *)NULL,(int *)NULL,&nbr_att);
  } /* end else */

  /* Allocate space for attribute names and types */
  if(nbr_att > 0) att=(att_sct *)nco_malloc(nbr_att*sizeof(att_sct));
    
  /* Get attributes' names, types, lengths, and values */
  for(idx=0;idx<nbr_att;idx++){

    att[idx].nm=(char *)nco_malloc(NC_MAX_NAME*sizeof(char));
    (void)nco_inq_attname(grp_id,var_id,idx,att[idx].nm);
    (void)nco_inq_att(grp_id,var_id,att[idx].nm,&att[idx].type,&att[idx].sz);

    /* Copy value to avoid indirection in loop over att_sz */
    att_sz=att[idx].sz;

    /* Allocate enough space to hold attribute */
    att[idx].val.vp=(void *)nco_malloc(att_sz*nco_typ_lng(att[idx].type));
    (void)nco_get_att(grp_id,var_id,att[idx].nm,att[idx].val.vp,att[idx].type);
    (void)fprintf(stdout,"%s attribute %i: %s, size = %li %s, value = ",src_sng,idx,att[idx].nm,att_sz,nco_typ_sng(att[idx].type));
    
    /* Typecast pointer to values before access */
    (void)cast_void_nctype(att[idx].type,&att[idx].val);

    (void)strcpy(dlm_sng,", ");
    (void)sprintf(att_sng,"%s%%s",nco_typ_fmt_sng(att[idx].type));
    switch(att[idx].type){
    case NC_FLOAT:
      for(att_lmn=0;att_lmn<att_sz;att_lmn++) (void)fprintf(stdout,att_sng,att[idx].val.fp[att_lmn],(att_lmn != att_sz-1L) ? dlm_sng : "");
      break;
    case NC_DOUBLE:
      for(att_lmn=0;att_lmn<att_sz;att_lmn++) (void)fprintf(stdout,att_sng,att[idx].val.dp[att_lmn],(att_lmn != att_sz-1L) ? dlm_sng : "");
      break;
    case NC_SHORT:
      for(att_lmn=0;att_lmn<att_sz;att_lmn++) (void)fprintf(stdout,att_sng,att[idx].val.sp[att_lmn],(att_lmn != att_sz-1L) ? dlm_sng : "");
      break;
    case NC_INT:
      for(att_lmn=0;att_lmn<att_sz;att_lmn++) (void)fprintf(stdout,att_sng,(long)att[idx].val.ip[att_lmn],(att_lmn != att_sz-1L) ? dlm_sng : "");
      break;
    case NC_CHAR:
      for(att_lmn=0;att_lmn<att_sz;att_lmn++){
        char char_foo;
	/* Assume \0 is string terminator and do not print it */
	if((char_foo=att[idx].val.cp[att_lmn]) != '\0') (void)fprintf(stdout,"%c",char_foo);
      } /* end loop over element */
      break;
    case NC_BYTE:
      for(att_lmn=0;att_lmn<att_sz;att_lmn++) (void)fprintf(stdout,att_sng,att[idx].val.bp[att_lmn],(att_lmn != att_sz-1L) ? dlm_sng : "");
      break;
    case NC_UBYTE:
      for(att_lmn=0;att_lmn<att_sz;att_lmn++) (void)fprintf(stdout,att_sng,att[idx].val.ubp[att_lmn],(att_lmn != att_sz-1L) ? dlm_sng : "");
      break;
    case NC_USHORT:
      for(att_lmn=0;att_lmn<att_sz;att_lmn++) (void)fprintf(stdout,att_sng,att[idx].val.usp[att_lmn],(att_lmn != att_sz-1L) ? dlm_sng : "");
      break;
    case NC_UINT:
      for(att_lmn=0;att_lmn<att_sz;att_lmn++) (void)fprintf(stdout,att_sng,att[idx].val.uip[att_lmn],(att_lmn != att_sz-1L) ? dlm_sng : "");
      break;
    case NC_INT64:
      for(att_lmn=0;att_lmn<att_sz;att_lmn++) (void)fprintf(stdout,att_sng,att[idx].val.i64p[att_lmn],(att_lmn != att_sz-1L) ? dlm_sng : "");
      break;
    case NC_UINT64:
      for(att_lmn=0;att_lmn<att_sz;att_lmn++) (void)fprintf(stdout,att_sng,att[idx].val.ui64p[att_lmn],(att_lmn != att_sz-1L) ? dlm_sng : "");
      break;
    case NC_STRING:
      for(att_lmn=0;att_lmn<att_sz;att_lmn++) (void)fprintf(stdout,att_sng,att[idx].val.sngp[att_lmn],(att_lmn != att_sz-1L) ? dlm_sng : "");
      break;
    default: nco_dfl_case_nc_type_err();
      break;
    } /* end switch */
    (void)fprintf(stdout,"\n");
    
  } /* end loop over attributes */
  (void)fprintf(stdout,"\n");
  (void)fflush(stdout);
  
  /* Free the space holding attribute values */
  for(idx=0;idx<nbr_att;idx++){
    att[idx].val.vp=nco_free(att[idx].val.vp);
    att[idx].nm=(char *)nco_free(att[idx].nm);
  } /* end loop over attributes */

  /* Free rest of space allocated for attribute information */
  if(nbr_att > 0) att=(att_sct *)nco_free(att);

} /* end nco_prn_att() */

const char * /* O [sng] sprintf() format string for type typ */
nco_typ_fmt_sng /* [fnc] Provide sprintf() format string for specified type */
(const nc_type typ) /* I [enm] netCDF type to provide format string for */
{
  /* Purpose: Provide sprintf() format string for specified type */
  static const char fmt_NC_FLOAT[]="%g"; /* %g defaults to 6 digits of precision */
  static const char fmt_NC_DOUBLE[]="%.12g"; /* Specify 12 digits of precision for double precision */
  static const char fmt_NC_INT[]="%i"; /* NCO has stored NC_INT in native type int since 2009. Before that NC_INT was stored as native type long */
  static const char fmt_NC_SHORT[]="%hi";
  static const char fmt_NC_CHAR[]="%c";
  /* Formats useful in printing byte data as decimal notation */
  /*  static const char fmt_NC_BYTE[]="%i";*/
  /*  static const char fmt_NC_BYTE[]="%c"; */
  /*  static const char fmt_NC_BYTE[]="%d";*/
  /* NB: %hhi is GNU extension, not ANSI standard */
  static const char fmt_NC_BYTE[]="%hhi"; /* Takes signed char as arg and prints 0,1,2..,126,127,-127,-126,...-2,-1 */
  /* static const char fmt_NC_BYTE[]="%hhu"; *//* Takes unsigned char as arg and prints 0..255 */

  static const char fmt_NC_UBYTE[]="%hhu"; /*  */
  static const char fmt_NC_USHORT[]="%hu"; /*  */
  static const char fmt_NC_UINT[]="%u"; /*  */
  static const char fmt_NC_INT64[]="%lli"; /*  */
  static const char fmt_NC_UINT64[]="%llu"; /*  */
  static const char fmt_NC_STRING[]="%s"; /*  */

  switch (typ){
  case NC_FLOAT:
    return fmt_NC_FLOAT;
  case NC_DOUBLE:
    return fmt_NC_DOUBLE;
  case NC_INT:
    return fmt_NC_INT;
  case NC_SHORT:
    return fmt_NC_SHORT;
  case NC_CHAR:
    return fmt_NC_CHAR;
  case NC_BYTE:
    return fmt_NC_BYTE;
  case NC_UBYTE:
    return fmt_NC_UBYTE; 
  case NC_USHORT:
    return fmt_NC_USHORT; 
  case NC_UINT:
    return fmt_NC_UINT; 
  case NC_INT64:
    return fmt_NC_INT64; 
  case NC_UINT64:
    return fmt_NC_UINT64; 
  case NC_STRING:
    return fmt_NC_STRING; 
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */

  /* Some compilers, e.g., SGI cc, need return statement to end non-void functions */
  return (char *)NULL;
} /* end nco_typ_fmt_sng() */

void
nco_prn_var_val_lmt /* [fnc] Print variable data */
(const int in_id, /* I [id] netCDF input file ID */
 const char * const var_nm, /* I [sng] Variable name */
 const lmt_sct * const lmt, /* I [sct] Dimension limits */
 const int lmt_nbr, /* I [nbr] number of dimensions with user-specified limits */
 char * const dlm_sng, /* I [sng] User-specified delimiter string, if any */
 const nco_bool FORTRAN_IDX_CNV, /* I [flg] Hyperslab indices obey Fortran convention */
 const nco_bool PRN_DMN_UNITS, /* I [flg] Print units attribute, if any */
 const nco_bool PRN_DMN_IDX_CRD_VAL) /* I [flg] Print dimension/coordinate indices/values */
{
  /* NB: nco_prn_var_val_lmt() with same nc_id contains OpenMP critical region */
  /* Purpose: Print variable data 
     Routine truncates dimensions of printed output variable in accord with user-specified limits
     fxm: routine does not correctly print hyperslabs which are wrapped, or which use non-unity stride */

  nco_bool SRD=False; /* Stride is non-unity */
  nco_bool WRP=False; /* Coordinate is wrapped */

  char nul_chr='\0';
  char var_sng[NCO_MAX_LEN_FMT_SNG];
  char *unit_sng=NULL;
  
  int rcd=NC_NOERR; /* [rcd] Return code */
  int *dmn_id=NULL_CEWI;
  int idx;
  
  long *dmn_cnt=NULL_CEWI;
  long *dmn_map=NULL_CEWI;
  long *dmn_mod=NULL_CEWI;
  long *dmn_sbs_ram=NULL_CEWI;
  long *dmn_sbs_dsk=NULL_CEWI;
  long *dmn_srd=NULL_CEWI;
  long *dmn_srt=NULL_CEWI;
  long *hyp_mod=NULL_CEWI;
  long lmn;  
  
  dmn_sct *dim=NULL_CEWI;
  var_sct var;

  /* Initialize units string, overwrite later if necessary */
  unit_sng=&nul_chr;

  /* Copy name into variable structure for aesthetics
     Unfortunately, Solaris may overwrite var.nm with next nco_malloc(), 
     so continue to use var_nm for output just to be safe. */
  var.nm=(char *)strdup(var_nm);

  /* Is requested variable in input file? */
  rcd=nco_inq_varid(in_id,var_nm,&var.id);

  /* Get number of dimensions and type for variable */
  (void)nco_inq_var(in_id,var.id,(char *)NULL,&var.type,&var.nbr_dim,(int *)NULL,(int *)NULL);

  if(var.nbr_dim > 0){
    /* Allocate space for dimension information */
    dim=(dmn_sct *)nco_malloc(var.nbr_dim*sizeof(dmn_sct));
    dmn_cnt=(long *)nco_malloc(var.nbr_dim*sizeof(long));
    dmn_id=(int *)nco_malloc(var.nbr_dim*sizeof(int));
    dmn_map=(long *)nco_malloc(var.nbr_dim*sizeof(long));
    dmn_srd=(long *)nco_malloc(var.nbr_dim*sizeof(long));
    dmn_srt=(long *)nco_malloc(var.nbr_dim*sizeof(long));

    /* Allocate space for related arrays */
    dmn_mod=(long *)nco_malloc(var.nbr_dim*sizeof(long));
    dmn_sbs_ram=(long *)nco_malloc(var.nbr_dim*sizeof(long));
    dmn_sbs_dsk=(long *)nco_malloc(var.nbr_dim*sizeof(long));
    hyp_mod=(long *)nco_malloc(var.nbr_dim*sizeof(long));
  } /* end if var.nbr_dim > 0 */
  
  /* Get dimension IDs */
  (void)nco_inq_vardimid(in_id,var.id,dmn_id);
  
  /* Get dimension sizes and names */
  for(idx=0;idx<var.nbr_dim;idx++){
    int lmt_idx;

    dim[idx].nm=(char *)nco_malloc(NC_MAX_NAME*sizeof(char));
    dim[idx].id=dmn_id[idx];
    (void)nco_inq_dim(in_id,dim[idx].id,dim[idx].nm,&dim[idx].sz);
    
    /* Initialize indicial offset and stride arrays */
    dmn_cnt[idx]=dim[idx].sz;
    dmn_map[idx]=1L;
    dmn_srd[idx]=1L;
    dmn_srt[idx]=0L;

    /* Decide whether this dimension has any user-specified limits */
    for(lmt_idx=0;lmt_idx<lmt_nbr;lmt_idx++){
      if(lmt[lmt_idx].id == dim[idx].id){
	dmn_srt[idx]=lmt[lmt_idx].srt;
	dmn_cnt[idx]=lmt[lmt_idx].cnt;
	dmn_srd[idx]=lmt[lmt_idx].srd;
	if(lmt[lmt_idx].srd != 1L) SRD=True;
	if(lmt[lmt_idx].min_idx > lmt[lmt_idx].max_idx) WRP=True;
	break;
      } /* end if */
    } /* end loop over lmt_idx */

    if(WRP){
      (void)fprintf(stdout,"%s: ERROR %s does not print variable hyperslabs where one or more of the coordinates is wrapped using the -d option (i.e., where the minimum index exceeds the maximum index such as longitude hyperslabs which cross the date-line. The workaround is to hyperslab into a new file (without -H) and then to print the values from that file (with -H).\n",prg_nm_get(),prg_nm_get());
      nco_exit(EXIT_FAILURE);
    } /* endif error */
  
    /* Is dimension a coordinate, i.e., stored as a variable? */
    dim[idx].val.vp=NULL;
     
    rcd=nco_inq_varid_flg(in_id,dim[idx].nm,&dim[idx].cid);
    /* Read in coordinate dimensions */
    if(rcd == NC_NOERR){
      dim[idx].is_crd_dmn=True;

      /* Find out what type of variable coordinate is */
      (void)nco_inq_vartype(in_id,dim[idx].cid,&dim[idx].type);
      
      /* Allocate enough space to hold coordinate */
      dim[idx].val.vp=(void *)nco_malloc(dmn_cnt[idx]*nco_typ_lng(dim[idx].type));
      
      /* Block is critical/thread-safe for identical/distinct in_id's */
      { /* begin potential OpenMP critical */
	/* Retrieve this coordinate */
	if(dmn_srd[idx] == 1L) (void)nco_get_vara(in_id,dim[idx].cid,dmn_srt+idx,dmn_cnt+idx,dim[idx].val.vp,dim[idx].type); else nco_get_varm(in_id,dim[idx].cid,dmn_srt+idx,dmn_cnt+idx,dmn_srd+idx,(long *)NULL,dim[idx].val.vp,dim[idx].type);
      } /* end potential OpenMP critical */
      
      /* Typecast pointer to values before access */
      (void)cast_void_nctype(dim[idx].type,&dim[idx].val);
      
    }else{ /* end if dimension is coordinate */   
      dim[idx].is_crd_dmn=False;
      dim[idx].cid=-1;
    }  /* end if dimension is not a coordinate */
    
  } /* end loop over dim */
  
  /* Find total size of variable array */
  var.sz=1L;
  for(idx=0;idx<var.nbr_dim;idx++) var.sz*=dmn_cnt[idx];

  /* Allocate enough space to hold variable */
  var.val.vp=(void *)nco_malloc(var.sz*nco_typ_lng(var.type));
  if(var.val.vp == NULL){
    (void)fprintf(stderr,"%s: ERROR unable to malloc() %lu bytes for %s\n",prg_nm_get(),(unsigned long)var.sz*nco_typ_lng(var.type),var.nm);
    nco_exit(EXIT_FAILURE);
  } /* end if */

  /* Block is critical/thread-safe for identical/distinct in_id's */
  { /* begin potential OpenMP critical */
    /* Get variable */
    if(var.nbr_dim==0){
      nco_get_var1(in_id ,var.id,0L,var.val.vp,var.type); 
    }else if(!SRD){
      nco_get_vara(in_id,var.id,dmn_srt,dmn_cnt,var.val.vp,var.type);
    }else if(SRD){
      nco_get_varm(in_id,var.id,dmn_srt,dmn_cnt,dmn_srd,(long *)NULL,var.val.vp,var.type);
    } /* end else */
  } /* end potential OpenMP critical */

  /* Typecast pointer to values before access */
  (void)cast_void_nctype(var.type,&var.val);

  if(PRN_DMN_UNITS){
    const char units_nm[]="units"; /* [sng] Name of units attribute */
    int rcd_lcl; /* [rcd] Return code */
    int att_id; /* [id] Attribute ID */
    long att_sz;
    nc_type att_typ;

    /* Does variable have character attribute named units_nm? */
    rcd_lcl=nco_inq_attid_flg(in_id,var.id,units_nm,&att_id);
    if(rcd_lcl == NC_NOERR){
      (void)nco_inq_att(in_id,var.id,units_nm,&att_typ,&att_sz);
      if(att_typ == NC_CHAR){
	unit_sng=(char *)nco_malloc((att_sz+1)*nco_typ_lng(att_typ));
	(void)nco_get_att(in_id,var.id,units_nm,unit_sng,att_typ);
	unit_sng[(att_sz+1)*nco_typ_lng(att_typ)-1]='\0';
      } /* end if */
    } /* end if */
  } /* end if PRN_DMN_UNITS */

  if(dlm_sng){
    /* Print each element with user-supplied formatting code */

    /* Replace C language '\X' escape codes with ASCII bytes */
    (void)sng_ascii_trn(dlm_sng);

    /* Assume -s argument (dlm_sng) formats entire string
       Otherwise, one could assume that field will be printed with format nco_typ_fmt_sng(var.type),
       and that user is only allowed to affect text between fields. 
       This would be accomplished with:
       (void)sprintf(var_sng,"%s%s",nco_typ_fmt_sng(var.type),dlm_sng); */

    for(lmn=0;lmn<var.sz;lmn++){
      switch(var.type){
      case NC_FLOAT: (void)fprintf(stdout,dlm_sng,var.val.fp[lmn]); break;
      case NC_DOUBLE: (void)fprintf(stdout,dlm_sng,var.val.dp[lmn]); break;
      case NC_SHORT: (void)fprintf(stdout,dlm_sng,var.val.sp[lmn]); break;
      case NC_INT: (void)fprintf(stdout,dlm_sng,var.val.ip[lmn]); break;
      case NC_CHAR: (void)fprintf(stdout,dlm_sng,var.val.cp[lmn]); break;
      case NC_BYTE: (void)fprintf(stdout,dlm_sng,var.val.bp[lmn]); break;
      case NC_UBYTE: (void)fprintf(stdout,dlm_sng,var.val.ubp[lmn]); break;
      case NC_USHORT: (void)fprintf(stdout,dlm_sng,var.val.usp[lmn]); break;
      case NC_UINT: (void)fprintf(stdout,dlm_sng,var.val.uip[lmn]); break;
      case NC_INT64: (void)fprintf(stdout,dlm_sng,var.val.i64p[lmn]); break;
      case NC_UINT64: (void)fprintf(stdout,dlm_sng,var.val.ui64p[lmn]); break;
      case NC_STRING: (void)fprintf(stdout,dlm_sng,var.val.sngp[lmn]); break;
      default: nco_dfl_case_nc_type_err(); break;
      } /* end switch */
    } /* end loop over element */

  } /* end if dlm_sng */

  if(var.nbr_dim == 0 && dlm_sng == NULL){ 
    /* Variable is scalar, byte, or character */
    lmn=0;
    (void)sprintf(var_sng,"%%s = %s %%s\n",nco_typ_fmt_sng(var.type));
    switch(var.type){
    case NC_FLOAT: (void)fprintf(stdout,var_sng,var_nm,var.val.fp[lmn],unit_sng); break;
    case NC_DOUBLE: (void)fprintf(stdout,var_sng,var_nm,var.val.dp[lmn],unit_sng); break;
    case NC_SHORT: (void)fprintf(stdout,var_sng,var_nm,var.val.sp[lmn],unit_sng); break;
    case NC_INT: (void)fprintf(stdout,var_sng,var_nm,var.val.ip[lmn],unit_sng); break;
    case NC_CHAR:
      (void)sprintf(var_sng,"%%s='%s' %%s\n",nco_typ_fmt_sng(var.type));
      (void)fprintf(stdout,var_sng,var_nm,var.val.cp[lmn],unit_sng);
      break;
    case NC_BYTE: (void)fprintf(stdout,var_sng,var_nm,var.val.bp[lmn],unit_sng); break;
    case NC_UBYTE: (void)fprintf(stdout,var_sng,var_nm,var.val.ubp[lmn],unit_sng); break;
    case NC_USHORT: (void)fprintf(stdout,var_sng,var_nm,var.val.usp[lmn],unit_sng); break;
    case NC_UINT: (void)fprintf(stdout,var_sng,var_nm,var.val.uip[lmn],unit_sng); break;
    case NC_INT64: (void)fprintf(stdout,var_sng,var_nm,var.val.i64p[lmn],unit_sng); break;
    case NC_UINT64: (void)fprintf(stdout,var_sng,var_nm,var.val.ui64p[lmn],unit_sng); break;
    case NC_STRING: (void)fprintf(stdout,var_sng,var_nm,var.val.sngp[lmn],unit_sng); break;
    default: nco_dfl_case_nc_type_err(); break;
    } /* end switch */
  } /* end if variable is scalar, byte, or character */

  if(var.nbr_dim > 0 && dlm_sng == NULL){ 
    /* Generate nicely formatted output for multidimensional arrays */

    char arr_lft_dlm=char_CEWI;
    char arr_rgt_dlm=char_CEWI;
    char dmn_sng[NCO_MAX_LEN_FMT_SNG];

    int crd_idx_crr=int_CEWI; /* Current coordinate index */
    int dmn_idx=int_CEWI;
    int dmn_idx_prn_srt; /* Index of first dimension to explicitly print */
    int dmn_nbr_prn=int_CEWI; /* Number of dimensions deconvolved */
    int mod_idx;

    long dmn_sbs_prn=long_CEWI; /* Subscript adjusted for C-Fortran indexing convention */
    long hyp_srt=0L;
    long idx_crr; /* Current index into equivalent 1-D array */

    short ftn_idx_off=short_CEWI;

    /* Variable is an array */
   
    /* Determine modulo masks for each index */
    for(idx=0;idx<var.nbr_dim;idx++) dmn_mod[idx]=1L;
    for(idx=0;idx<var.nbr_dim-1;idx++)
      for(mod_idx=idx+1;mod_idx<var.nbr_dim;mod_idx++) 
	dmn_mod[idx]*=dmn_cnt[mod_idx];

    /* Compute offset of hyperslab buffer from origin */
    if(lmt_nbr > 0){
      for(idx=0;idx<var.nbr_dim;idx++) hyp_mod[idx]=1L;
      for(idx=0;idx<var.nbr_dim-1;idx++)
	for(mod_idx=idx+1;mod_idx<var.nbr_dim;mod_idx++) 
	  hyp_mod[idx]*=dim[mod_idx].sz;

      for(idx=0;idx<var.nbr_dim;idx++) hyp_srt+=dmn_srt[idx]*hyp_mod[idx];
    } /* end if */

    if(FORTRAN_IDX_CNV){
      ftn_idx_off=1;
      arr_lft_dlm='(';
      arr_rgt_dlm=')';
    }else{
      ftn_idx_off=0;
      arr_lft_dlm='[';
      arr_rgt_dlm=']';
    } /* end else */
    
    /* Until 19991226, ncks printed one-dimensional character arrays here using
       if(var.type == NC_CHAR && var.nbr_dim == 1) (void)fprintf(stdout,"%s = %s\n",var_nm,var.val.cp); */

    /* Loop over each element of variable */
    for(lmn=0;lmn<var.sz;lmn++){
      
      /* Default is to print all dimension indices and any coordinates */
      dmn_idx_prn_srt=0;
      dmn_nbr_prn=var.nbr_dim;
      /* Treat character arrays as strings if possible */
      if(var.type == NC_CHAR){
	/* Do not print final dimension (C-convention) of character arrays */
	if(FORTRAN_IDX_CNV) dmn_idx_prn_srt=1; else dmn_nbr_prn=var.nbr_dim-1;
      } /* endif */
      
      dmn_sbs_ram[var.nbr_dim-1]=lmn%dmn_cnt[var.nbr_dim-1];
      for(idx=0;idx<var.nbr_dim-1;idx++){ /* NB: loop through nbr_dim-2 only */
	dmn_sbs_ram[idx]=(long)(lmn/dmn_mod[idx]);
	dmn_sbs_ram[idx]%=dmn_cnt[idx];
      } /* end loop over dimensions */
      
      /* Convert hyperslab (RAM) subscripts to absolute (file/disk) subscripts */
      for(idx=0;idx<var.nbr_dim;idx++) dmn_sbs_dsk[idx]=dmn_sbs_ram[idx]*dmn_srd[idx]+dmn_srt[idx];
      
      /* Skip rest of loop unless element is first in string */
      if(var.type == NC_CHAR && dmn_sbs_ram[var.nbr_dim-1] != 0L) continue;
      
      /* Does user wish to print leading dimension/coordinate indices/values? */
      if(PRN_DMN_IDX_CRD_VAL){
      
	/* Loop over dimensions whose coordinates are to be printed */
	for(idx=dmn_idx_prn_srt;idx<dmn_nbr_prn;idx++){
	  
	  /* Reverse dimension ordering for Fortran convention */
	  if(FORTRAN_IDX_CNV) dmn_idx=var.nbr_dim-1-idx; else dmn_idx=idx;
	  
	  /* Printed dimension subscript includes indexing convention (C or Fortran) */
	  dmn_sbs_prn=dmn_sbs_dsk[dmn_idx]+ftn_idx_off;
	  
	  /* Format and print dimension part of output string for non-coordinate variables */
	  if(dim[dmn_idx].cid != var.id){ /* If variable is not a coordinate... */
	    if(dim[dmn_idx].is_crd_dmn){ /* If dimension is a coordinate... */
	      (void)sprintf(dmn_sng,"%%s%c%%ld%c=%s ",arr_lft_dlm,arr_rgt_dlm,nco_typ_fmt_sng(dim[dmn_idx].type));
	      /* Account for hyperslab offset in coordinate values*/
	      crd_idx_crr=dmn_sbs_ram[dmn_idx];
	      switch(dim[dmn_idx].type){
	      case NC_FLOAT: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,dim[dmn_idx].val.fp[crd_idx_crr]); break;
	      case NC_DOUBLE: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,dim[dmn_idx].val.dp[crd_idx_crr]); break;
	      case NC_SHORT: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,dim[dmn_idx].val.sp[crd_idx_crr]); break;
	      case NC_INT: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,dim[dmn_idx].val.ip[crd_idx_crr]); break;
	      case NC_CHAR: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,dim[dmn_idx].val.cp[crd_idx_crr]); break;
	      case NC_BYTE: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,dim[dmn_idx].val.bp[crd_idx_crr]); break;
	      case NC_UBYTE: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,dim[dmn_idx].val.ubp[crd_idx_crr]); break;
	      case NC_USHORT: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,dim[dmn_idx].val.usp[crd_idx_crr]); break;
	      case NC_UINT: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,dim[dmn_idx].val.uip[crd_idx_crr]); break;
	      case NC_INT64: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,dim[dmn_idx].val.i64p[crd_idx_crr]); break;
	      case NC_UINT64: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,dim[dmn_idx].val.ui64p[crd_idx_crr]); break;
	      case NC_STRING: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,dim[dmn_idx].val.sngp[crd_idx_crr]); break;
	      default: nco_dfl_case_nc_type_err(); break;
	      } /* end switch */
	    }else{ /* if dimension is not a coordinate... */
	      (void)sprintf(dmn_sng,"%%s%c%%ld%c ",arr_lft_dlm,arr_rgt_dlm);
	      (void)fprintf(stdout,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn);
	    } /* end else */
	  } /* end if */
	} /* end loop over dimensions */
      } /* end if PRN_DMN_IDX_CRD_VAL */
      
      /* Finally, print value of current element of variable */	
      idx_crr=lmn+hyp_srt+ftn_idx_off; /* Current index into equivalent 1-D array */
      (void)sprintf(var_sng,"%%s%c%%ld%c=%s %%s\n",arr_lft_dlm,arr_rgt_dlm,nco_typ_fmt_sng(var.type));
      
      if(var.type == NC_CHAR && dmn_sbs_ram[var.nbr_dim-1] == 0L){
	/* Print all characters in last dimension each time penultimate dimension subscript changes to its start value
	   Ironic that printing characters is much more tedious than numbers */
	/* Search for NUL-termination within size of last dimension */
	if(memchr((void *)(var.val.cp+lmn),'\0',(size_t)dmn_cnt[var.nbr_dim-1])){
	  /* Memory region is NUL-terminated, i.e., a valid string */
	  /* Print strings inside double quotes */
	  (void)sprintf(var_sng,"%%s%c%%ld--%%ld%c=\"%%s\" %%s",arr_lft_dlm,arr_rgt_dlm);
	  /* var.val.cp is type nco_char and strlen() requires const char * */
	  (void)fprintf(stdout,var_sng,var_nm,idx_crr,idx_crr+strlen((char *)var.val.cp+lmn),(char *)var.val.cp+lmn,unit_sng);
	}else{
	  /* Memory region is not NUL-terminated, print block of chars instead */
	  /* Print block of chars inside single quotes */
	  /* Re-use dmn_sng for temporary format string */
	  (void)sprintf(dmn_sng,"%%.%ldc",dmn_cnt[var.nbr_dim-1]);
	  (void)sprintf(var_sng,"%%s%c%%ld--%%ld%c='%s' %%s",arr_lft_dlm,arr_rgt_dlm,dmn_sng);
	  (void)fprintf(stdout,var_sng,var_nm,idx_crr,idx_crr+dmn_cnt[var.nbr_dim-1]-1L,var.val.cp+lmn,unit_sng);
	} /* endif */
	if(dbg_lvl_get() >= 6)(void)fprintf(stdout,"DEBUG: format string used for chars is dmn_sng = %s, var_sng = %s\n",dmn_sng,var_sng); 
	/* Newline separates consecutive values within given variable */
	(void)fprintf(stdout,"\n");
	(void)fflush(stdout);
	/* Skip rest of loop for this element, move to next element */
	continue;
      } /* endif */
      
      switch(var.type){
      case NC_FLOAT: (void)fprintf(stdout,var_sng,var_nm,idx_crr,var.val.fp[lmn],unit_sng); break;
      case NC_DOUBLE: (void)fprintf(stdout,var_sng,var_nm,idx_crr,var.val.dp[lmn],unit_sng); break;
      case NC_SHORT: (void)fprintf(stdout,var_sng,var_nm,idx_crr,var.val.sp[lmn],unit_sng); break;
      case NC_INT: (void)fprintf(stdout,var_sng,var_nm,idx_crr,var.val.ip[lmn],unit_sng); break;
      case NC_CHAR: (void)fprintf(stdout,var_sng,var_nm,idx_crr,var.val.cp[lmn],unit_sng); break;
      case NC_BYTE: (void)fprintf(stdout,var_sng,var_nm,idx_crr,var.val.bp[lmn],unit_sng); break;
      case NC_UBYTE: (void)fprintf(stdout,var_sng,var_nm,idx_crr,var.val.ubp[lmn],unit_sng); break;
      case NC_USHORT: (void)fprintf(stdout,var_sng,var_nm,idx_crr,var.val.usp[lmn],unit_sng); break;
      case NC_UINT: (void)fprintf(stdout,var_sng,var_nm,idx_crr,var.val.uip[lmn],unit_sng); break;
      case NC_INT64: (void)fprintf(stdout,var_sng,var_nm,idx_crr,var.val.i64p[lmn],unit_sng); break;
      case NC_UINT64: (void)fprintf(stdout,var_sng,var_nm,idx_crr,var.val.ui64p[lmn],unit_sng); break;
      case NC_STRING: (void)fprintf(stdout,var_sng,var_nm,idx_crr,var.val.sngp[lmn],unit_sng); break;
      default: nco_dfl_case_nc_type_err(); break;
      } /* end switch */
    } /* end loop over element */
  } /* end if variable is an array, not a scalar */
  
  /* Additional newline between consecutive variables or final variable and prompt */
  (void)fprintf(stdout,"\n");
  (void)fflush(stdout);
  
  /* Free space allocated for dimension */
  for(idx=0;idx<var.nbr_dim;idx++){
    dim[idx].nm=(char *)nco_free(dim[idx].nm);
    dim[idx].val.vp=nco_free(dim[idx].val.vp);
  } /* end loop over dimensions */
  if(var.nbr_dim > 0){
    dim=(dmn_sct *)nco_free(dim);
    dmn_cnt=(long *)nco_free(dmn_cnt);
    dmn_id=(int *)nco_free(dmn_id);
    dmn_map=(long *)nco_free(dmn_map);
    dmn_mod=(long *)nco_free(dmn_mod);
    dmn_sbs_ram=(long *)nco_free(dmn_sbs_ram);
    dmn_sbs_dsk=(long *)nco_free(dmn_sbs_dsk);
    dmn_srd=(long *)nco_free(dmn_srd);
    dmn_srt=(long *)nco_free(dmn_srt);
    hyp_mod=(long *)nco_free(hyp_mod);
  } /* end if nbr_dim > 0*/

  /* Free space allocated for variable */
  var.val.vp=nco_free(var.val.vp);
  var.nm=(char *)nco_free(var.nm);
  if(strlen(unit_sng) > 0) unit_sng=(char *)nco_free(unit_sng);
 
} /* end nco_prn_var_val_lmt() */



void
nco_prn_var_dfn                 /* [fnc] Print variable metadata */
(int nc_id,                     /* I [id] netCDF file ID */
 const trv_sct * const var_trv) /* I [sct] Object to print (variable) */
{
  /* Purpose: Print variable metadata */

  const char fnc_nm[]="nco_prn_var_dfn()"; /* [sng] Function name */

  int deflate; /* [flg] Deflation is on */
  int dfl_lvl; /* [enm] Deflate level [0..9] */
  int packing; /* [flg] Variable is packed */
  int shuffle; /* [flg] Shuffling is on */
  int srg_typ; /* [enm] Storage type */
  int nbr_att;
  int nbr_dim;
  int var_id;
  int grp_id;
  int dmn_idx;

  nc_type var_typ;

  nco_bool CRR_DMN_IS_REC_IN_INPUT[NC_MAX_DIMS]; /* [flg] Is record dimension */

  size_t cnk_sz[NC_MAX_DIMS]; /* [nbr] Chunk sizes */
  size_t dmn_sz[NC_MAX_DIMS]; /* [nbr] Dimension sizes */

  /* Obtain group ID from netCDF API using full group name */
  (void)nco_inq_grp_full_ncid(nc_id,var_trv->grp_nm_fll,&grp_id);

  /* Obtain variable ID from netCDF API using group ID */
  (void)nco_inq_varid(grp_id,var_trv->nm,&var_id);

  /* Get number of dimensions, type, and number of attributes for variable */
  (void)nco_inq_var(grp_id,var_id,(char *)NULL,&var_typ,&nbr_dim,(int *)NULL,&nbr_att);

  assert(var_trv->nbr_dmn == nbr_dim);
  assert(var_trv->var_typ == var_typ);
  assert(var_trv->nbr_att == nbr_att);

  /* Get storage properties */
  (void)nco_inq_var_chunking(grp_id,var_id,&srg_typ,cnk_sz);
  (void)nco_inq_var_deflate(grp_id,var_id,&shuffle,&deflate,&dfl_lvl);
  (void)nco_inq_var_packing(grp_id,var_id,&packing);

  /* Loop dimensions */
  for(dmn_idx=0;dmn_idx<nbr_dim;dmn_idx++){

    /* This dimension has a coordinate variable */
    if (var_trv->var_dmn.is_crd_var[dmn_idx] == True){

      /* Get coordinate from table */
      crd_sct *crd=var_trv->var_dmn.crd[dmn_idx];

      dmn_sz[dmn_idx]=crd->sz;
      CRR_DMN_IS_REC_IN_INPUT[dmn_idx]=crd->is_rec_dmn;
    }

    /* This dimension does not has a coordinate variable, it must have a unique dimension */
    else if (var_trv->var_dmn.is_crd_var[dmn_idx] == False){

      /* Get unique dimension */
      dmn_fll_sct *dmn_fll=var_trv->var_dmn.dmn_fll[dmn_idx];

      dmn_sz[dmn_idx]=dmn_fll->sz;
      CRR_DMN_IS_REC_IN_INPUT[dmn_idx]=dmn_fll->is_rec_dmn;
    }

  } /* Loop dimensions */


  /* Print header for variable */
  (void)fprintf(stdout,"%s: type %s, %i dimension%s, %i attribute%s, chunked? %s, compressed? %s, packed? %s\n",var_trv->nm,nco_typ_sng(var_typ),nbr_dim,(nbr_dim == 1) ? "" : "s",nbr_att,(nbr_att == 1) ? "" : "s",(srg_typ == NC_CHUNKED) ? "yes" : "no",(deflate) ? "yes" : "no",(packing) ? "yes" : "no");

  /* Print type, shape, and total size of variable */
  if(nbr_dim > 0){
    long var_sz=1L;
    char sz_sng[100];
    char sng_foo[200];

    for(dmn_idx=0;dmn_idx<nbr_dim;dmn_idx++)
    {
      var_sz*=dmn_sz[dmn_idx];
    }
    sz_sng[0]='\0';

    for(dmn_idx=0;dmn_idx<nbr_dim-1;dmn_idx++){
      (void)sprintf(sng_foo,"%li*",dmn_sz[dmn_idx]);
      (void)strcat(sz_sng,sng_foo);
    } /* end loop over dim */

    (void)sprintf(sng_foo,"%li*sizeof(%s)",dmn_sz[dmn_idx],nco_typ_sng(var_typ));
    (void)strcat(sz_sng,sng_foo);

    (void)nco_inq_var_deflate(grp_id,var_id,&shuffle,&deflate,&dfl_lvl);

    if(deflate) (void)fprintf(stdout,"%s on-disk compression (Lempel-Ziv %s shuffling) level = %d\n",
      var_trv->nm,(shuffle) ? "with" : "without",dfl_lvl);
    (void)fprintf(stdout,"%s size (RAM) = %s = %li*%lu = %lu bytes\n",
      var_trv->nm,sz_sng,var_sz,(unsigned long)nco_typ_lng(var_typ),(unsigned long)(var_sz*nco_typ_lng(var_typ)));
  }else{
    long var_sz=1L;

    (void)fprintf(stdout,"%s size (RAM) = %ld*sizeof(%s) = %ld*%lu = %lu bytes\n",
      var_trv->nm,var_sz,nco_typ_sng(var_typ),var_sz,(unsigned long)nco_typ_lng(var_typ),(unsigned long)(var_sz*nco_typ_lng(var_typ)));
  } /* end if variable is scalar */

  /* Print dimension sizes and names */

  /* Loop dimensions for object (variable)  */
  for(int dmn_idx=0;dmn_idx<var_trv->nbr_dmn;dmn_idx++) {

    /* This dimension has a coordinate variable */
    if (var_trv->var_dmn.is_crd_var[dmn_idx] == True){

      /* Get coordinate from table */
      crd_sct *crd=var_trv->var_dmn.crd[dmn_idx];

      int grp_crd_id; /* [ID] Of group of where coordinate variable is located */
      int var_crd_id; /* [ID] Of coordinate variable */

      /* Obtain group ID using full group name */
      (void)nco_inq_grp_full_ncid(nc_id,crd->crd_grp_nm_fll,&grp_crd_id);

      /* Obtain variable ID using group ID and name */
      (void)nco_inq_varid(grp_crd_id,crd->nm,&var_crd_id);

      nc_type crd_typ;

      /* Which storage type is the coordinate? */
      (void)nco_inq_vartype(grp_crd_id,var_crd_id,&crd_typ);

      if(srg_typ == NC_CHUNKED) (void)fprintf(stdout,"%s dimension %i: %s, size = %li %s, chunksize = %zu (",
        var_trv->nm,dmn_idx,crd->nm,crd->sz,nco_typ_sng(crd_typ),cnk_sz[dmn_idx]); 
      else (void)fprintf(stdout,"%s dimension %i: %s, size = %li %s (",
        var_trv->nm,dmn_idx,crd->nm,crd->sz,nco_typ_sng(crd_typ));
      (void)fprintf(stdout,"%soordinate dimension)",(CRR_DMN_IS_REC_IN_INPUT[dmn_idx]) ? "Record c" : "C");

    }

    /* This dimension does not has a coordinate variable, it must have a unique dimension pointer */
    else if (var_trv->var_dmn.is_crd_var[dmn_idx] == False){

      /* Get unique dimension */
      dmn_fll_sct *dmn_fll=var_trv->var_dmn.dmn_fll[dmn_idx];

      /* Dimension is not a coordinate */
      if(srg_typ == NC_CHUNKED) (void)fprintf(stdout,"%s dimension %i: %s, size = %li, chunksize = %zu (",
        var_trv->nm,dmn_idx,dmn_fll->nm,dmn_fll->sz,cnk_sz[dmn_idx]); 
      else (void)fprintf(stdout,"%s dimension %i: %s, size = %li (",
        var_trv->nm,dmn_idx,dmn_fll->nm,dmn_fll->sz);
      (void)fprintf(stdout,"%son-coordinate dimension)",(CRR_DMN_IS_REC_IN_INPUT[dmn_idx]) ? "Record n" : "N");

    }

    (void)fprintf(stdout,"\n"); 
  } /* Loop dimensions for object (variable)  */


  /* Caveat user */
  if((nc_type)var_typ == NC_STRING) (void)fprintf(stdout,"%s size (RAM) above is space required for pointers only, full size of strings is unknown until data are read\n",var_trv->nm);
  (void)fflush(stdout);


} /* end nco_prn_var_dfn() */

