/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_prn.c,v 1.218 2014-02-19 17:44:13 zender Exp $ */

/* Purpose: Print variables, attributes, metadata */

/* Copyright (C) 1995--2014 Charlie Zender
   License: GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */

#include "nco_prn.h" /* Print variables, attributes, metadata */

void 
nco_prn_att /* [fnc] Print all attributes of single variable or group */
(const int grp_id, /* I [id] netCDF group ID */
 const prn_fmt_sct * const prn_flg, /* I [sct] Print-format information */
 const int var_id) /* I [id] netCDF input variable ID */
{
  /* Purpose: Print all global attributes in netCDF group,
     or all attributes for particular netCDF variable. 
     If var_id == NC_GLOBAL ( = -1) then global/group attributes are printed,
     otherwise variable's attributes are printed. */

  att_sct *att=NULL_CEWI;

  const char spc_sng[]=""; /* [sng] Space string */

  char *nm_cdl;
  char *sng_val_sng; /* [sng] String of NC_CHAR */
  char *sng_val_sng_cpy; /* [sng] Copy of sng_val_sng to avoid cppcheck error about using sng_val_sng as both parameter and desitnation in sprintf(). NB: free() only one of these two pointers. */
  char *spr_sng=NULL; /* [sng] Output separator string */

  char cma_sng[]=", "; /* [sng] Comma string */
  char spr_xml_chr[]="*"; /* [sng] Default XML separator for character types */
  char spr_xml_chr_bck[]="*|*"; /* [sng] Backup default XML separator for character types */
  char spr_xml_nmr[]=" "; /* [sng] Default XML separator for numeric types */

  char chr_val; /* [sng] Current character */

  char att_sng_dlm[NCO_MAX_LEN_FMT_SNG];
  char att_sng_pln[NCO_MAX_LEN_FMT_SNG];
  char src_sng[NC_MAX_NAME];
  char val_sng[NCO_ATM_SNG_LNG];

  double val_dbl;

  float val_flt;

  int att_nbr_ttl;
  int att_nbr_vsb;
  int dmn_nbr=0;
  int fl_fmt; /* I [enm] File format */
  int grp_id_prn;
  int idx;
  int prn_ndn=0; /* [nbr] Indentation for printing */
  int rcd=NC_NOERR; /* [rcd] Return code */
  int rcd_prn;

  long att_sz;
  long att_szm1;
  long chr_idx;
  long lmn;
  long sng_lng; /* [nbr] Length of NC_CHAR string */
  long sng_lngm1; /* [nbr] Length minus one of NC_CHAR string */
  
  nc_type var_typ;

  const nco_bool CDL=prn_flg->cdl; /* [flg] CDL output */
  const nco_bool XML=prn_flg->xml; /* [flg] XML output */
  const nco_bool TRD=prn_flg->trd; /* [flg] Traditional output */
  const nco_bool JSN=prn_flg->jsn; /* [flg] JSON output */
  const nco_bool CDL_OR_JSN=prn_flg->cdl || prn_flg->jsn; /* [flg] CDL or JSON output */
  const nco_bool CDL_OR_TRD=prn_flg->cdl || prn_flg->trd; /* [flg] CDL or Traditional output */
  const nco_bool CDL_OR_JSN_OR_XML=prn_flg->cdl || prn_flg->jsn || prn_flg->xml; /* [flg] CDL or JSON or XML output */

  //nco_bool has_fll_val=False; /* [flg] Has _FillValue attribute */

  nco_string sng_val; /* [sng] Current string */

  prn_ndn=prn_flg->ndn;
  if(var_id == NC_GLOBAL){
    /* Get number of global attributes in group */
    (void)nco_inq(grp_id,(int *)NULL,(int *)NULL,&att_nbr_vsb,(int *)NULL);
    /* Which group is this? */
    rcd=nco_inq_grp_parent_flg(grp_id,&grp_id_prn);
    if(rcd == NC_ENOGRP) (void)strcpy(src_sng,(CDL) ? "" : "Global"); else (void)strcpy(src_sng,(CDL) ? "" : "Group");
    if(CDL) prn_ndn+=prn_flg->sxn_fst;
  }else{
    /* Get name and number of attributes for variable */
    (void)nco_inq_var(grp_id,var_id,src_sng,&var_typ,&dmn_nbr,(int *)NULL,&att_nbr_vsb);
    if(CDL) prn_ndn+=2*prn_flg->var_fst;
    if(XML) prn_ndn+=prn_flg->sxn_fst;
    if(prn_flg->new_fmt && TRD) prn_ndn+=prn_flg->var_fst;
  } /* end else */

  /* Allocate space for attribute names and types */
  att_nbr_ttl=att_nbr_vsb;
  assert(att_nbr_ttl >= 0);
  if(att_nbr_ttl > 0) att=(att_sct *)nco_malloc(att_nbr_ttl*sizeof(att_sct));
    
  rcd=nco_inq_format(grp_id,&fl_fmt);

  if(prn_flg->hdn && ((fl_fmt == NC_FORMAT_NETCDF4) || (fl_fmt == NC_FORMAT_NETCDF4_CLASSIC))){
    char *val_hdn_sng=NULL;
    int chk_typ; /* [enm] Checksum type [0..9] */
    int deflate; /* [flg] Deflation is on */
    int dfl_lvl; /* [enm] Deflate level [0..9] */
    int flg_ndn=NC_ENDIAN_NATIVE; /* [enm] _Endianness */
    int fll_nil; /* [flg] NO_FILL */
    int shuffle; /* [flg] Shuffling is on */
    int srg_typ; /* [enm] Storage type */
    size_t cnk_sz[NC_MAX_DIMS]; /* [nbr] Chunk sizes */
    if(var_id == NC_GLOBAL){
      /* _Format */
      if(!XML){
	rcd=nco_inq_grp_parent_flg(grp_id,&grp_id_prn);
	if(rcd == NC_ENOGRP){
	  /* _Format only and always printed for root group */
	  idx=att_nbr_ttl++;
	  att=(att_sct *)nco_realloc(att,att_nbr_ttl*sizeof(att_sct));
	  att[idx].nm=(char *)strdup("_Format");
	  att[idx].type=NC_CHAR;
	  val_hdn_sng=strdup(nco_fmt_hdn_sng(fl_fmt));
	  att_sz=att[idx].sz=strlen(val_hdn_sng);
	  att[idx].val.vp=(void *)nco_malloc(att_sz*nco_typ_lng(att[idx].type));
	  strncpy(att[idx].val.cp,val_hdn_sng,att_sz);
	  if(val_hdn_sng) val_hdn_sng=(char *)nco_free(val_hdn_sng);
	} /* !rcd */	
      } /* !xml */
    }else{
      if(nco_fmt_xtn_get() != nco_fmt_xtn_hdf4){
	/* _NOFILL */
	rcd=nco_inq_var_fill(grp_id,var_id,&fll_nil,(int *)NULL);
	if(fll_nil){
	  /* Print _NOFILL for variables that are not pre-filled */
	  idx=att_nbr_ttl++;
	  att=(att_sct *)nco_realloc(att,att_nbr_ttl*sizeof(att_sct));
	  att[idx].nm=(char *)strdup("_NOFILL");
	  att[idx].type=NC_INT;
	  att_sz=att[idx].sz=1L;
	  att[idx].val.vp=(void *)nco_malloc(att_sz*nco_typ_lng(att[idx].type));
	  att[idx].val.ip[0]=fll_nil;
	} /* !fll_nil */
	/* _Storage */
	rcd=nco_inq_var_chunking(grp_id,var_id,&srg_typ,cnk_sz);
	if(!XML){
	  if(dmn_nbr > 0){
	    /* Print _Storage for arrays */
	    idx=att_nbr_ttl++;
	    att=(att_sct *)nco_realloc(att,att_nbr_ttl*sizeof(att_sct));
	    att[idx].nm=(char *)strdup("_Storage");
	    att[idx].type=NC_CHAR;
	    val_hdn_sng= (srg_typ == NC_CONTIGUOUS) ? (char *)strdup("contiguous") : (char *)strdup("chunked");
	    att_sz=att[idx].sz=strlen(val_hdn_sng);
	    att[idx].val.vp=(void *)nco_malloc(att_sz*nco_typ_lng(att[idx].type));
	    strncpy(att[idx].val.cp,val_hdn_sng,att_sz);
	    if(val_hdn_sng) val_hdn_sng=(char *)nco_free(val_hdn_sng);
	  } /* !dmn_nbr */	
	} /* !xml */
	/* _ChunkSizes */
	if(srg_typ == NC_CHUNKED){
	  /* Print _ChunkSizes for chunked arrays */
	  idx=att_nbr_ttl++;
	  att=(att_sct *)nco_realloc(att,att_nbr_ttl*sizeof(att_sct));
	  // Prior to ~20140105, NcML generated _ChunkSize (singular) attribute, not _Chunksizes (netCDFJava #JQM-616814, JIRA TDS-517)
	  //	  att[idx].nm= (XML) ? (char *)strdup("_ChunkSize") : (char *)strdup("_ChunkSizes");
	  att[idx].nm=(char *)strdup("_ChunkSizes");
	  att[idx].type=NC_INT;
	  att_sz=att[idx].sz=dmn_nbr;
	  att[idx].val.vp=(void *)nco_malloc(att_sz*nco_typ_lng(att[idx].type));
	  for(int dmn_idx=0;dmn_idx<dmn_nbr;dmn_idx++) att[idx].val.ip[dmn_idx]=cnk_sz[dmn_idx];
	} /* srg_typ != NC_CHUNKED */
	/* _DeflateLevel */
	if(!XML){
	  rcd=nco_inq_var_deflate(grp_id,var_id,&shuffle,&deflate,&dfl_lvl);
	  if(deflate){
	    /* Print _DeflateLevel for deflated variables */
	    idx=att_nbr_ttl++;
	    att=(att_sct *)nco_realloc(att,att_nbr_ttl*sizeof(att_sct));
	    att[idx].nm=(char *)strdup("_DeflateLevel");
	    att[idx].type=NC_INT;
	    att_sz=att[idx].sz=1L;
	    att[idx].val.vp=(void *)nco_malloc(att_sz*nco_typ_lng(att[idx].type));
	    att[idx].val.ip[0]=dfl_lvl;
	  } /* !deflate */
	} /* !xml */
	/* _Shuffle */
	if(!XML){
	  if(shuffle){
	    /* Print _Shuffle for shuffled variables */
	    idx=att_nbr_ttl++;
	    att=(att_sct *)nco_realloc(att,att_nbr_ttl*sizeof(att_sct));
	    att[idx].nm=(char *)strdup("_Shuffle");
	    att[idx].type=NC_CHAR;
	    val_hdn_sng=strdup("true");
	    att_sz=att[idx].sz=strlen(val_hdn_sng);
	    att[idx].val.vp=(void *)nco_malloc(att_sz*nco_typ_lng(att[idx].type));
	    strncpy(att[idx].val.cp,val_hdn_sng,att_sz);
	    if(val_hdn_sng) val_hdn_sng=(char *)nco_free(val_hdn_sng);
	  } /* !shuffle */
	} /* !xml */
	/* _Fletcher32 */
	if(!XML){
	  rcd=nco_inq_var_fletcher32(grp_id,var_id,&chk_typ);
	  if(chk_typ){
	    /* Print _Fletcher32 for checksummed variables */
	    idx=att_nbr_ttl++;
	    att=(att_sct *)nco_realloc(att,att_nbr_ttl*sizeof(att_sct));
	    att[idx].nm=(char *)strdup("_Fletcher32");
	    att[idx].type=NC_CHAR;
	    val_hdn_sng=strdup("true");
	    att_sz=att[idx].sz=strlen(val_hdn_sng);
	    att[idx].val.vp=(void *)nco_malloc(att_sz*nco_typ_lng(att[idx].type));
	    strncpy(att[idx].val.cp,val_hdn_sng,att_sz);
	    if(val_hdn_sng) val_hdn_sng=(char *)nco_free(val_hdn_sng);
	  } /* !chk_typ */
	} /* !xml */
	/* _Endianness */
	if(!XML){
	  if((var_typ == NC_USHORT) || (var_typ == NC_SHORT) || (var_typ == NC_UINT) || (var_typ == NC_INT) || (var_typ == NC_UINT64) || (var_typ == NC_INT64)){
	    /* _Endianness variable attribute always printed for integer types */
	    idx=att_nbr_ttl++;
	    att=(att_sct *)nco_realloc(att,att_nbr_ttl*sizeof(att_sct));
	    att[idx].nm=(char *)strdup("_Endianness");
	    att[idx].type=NC_CHAR;
	    rcd+=nco_inq_var_endian(grp_id,var_id,&flg_ndn);
	    val_hdn_sng=strdup(nco_ndn_sng(flg_ndn));
	    att_sz=att[idx].sz=strlen(val_hdn_sng);
	    att[idx].val.vp=(void *)nco_malloc(att_sz*nco_typ_lng(att[idx].type));
	    strncpy(att[idx].val.cp,val_hdn_sng,att_sz);
	    if(val_hdn_sng) val_hdn_sng=(char *)nco_free(val_hdn_sng);
	  } /* !INT */
	} /* !xml */
      } /* !HDF4 */
    } /* !NC_GLOBAL */
  } /* !hdn, NC_FORMAT_NETCDF4, NC_FORMAT_NETCDF4_CLASSIC */

  /* Get attributes' names, types, lengths, and values */
  for(idx=0;idx<att_nbr_ttl;idx++){

    if(idx <= att_nbr_vsb-1){
      /* Visible attributes get standard treatment */
      att[idx].nm=(char *)nco_malloc(NC_MAX_NAME*sizeof(char));
      (void)nco_inq_attname(grp_id,var_id,idx,att[idx].nm);
      (void)nco_inq_att(grp_id,var_id,att[idx].nm,&att[idx].type,&att[idx].sz);
      /* Allocate enough space to hold attribute */
      att[idx].val.vp=(void *)nco_malloc(att[idx].sz*nco_typ_lng(att[idx].type));
      (void)nco_get_att(grp_id,var_id,att[idx].nm,att[idx].val.vp,att[idx].type);
    } /* idx == att_nbr */
    
    /* Copy value to avoid indirection in loop over att_sz */
    att_sz=att[idx].sz;
    att_szm1=att_sz-1L;

    if(CDL){
      nm_cdl=nm2sng_cdl(att[idx].nm);
      (void)fprintf(stdout,"%*s%s%s:%s = ",prn_ndn,spc_sng,(att[idx].type == NC_STRING) ? "string " : "",src_sng,nm_cdl); 
      nm_cdl=(char *)nco_free(nm_cdl);
    } /* !cdl */
    if(JSN) (void)fprintf(stdout,"{\"name\": \"%s\", \"value\": ",att[idx].nm);
    if(TRD) (void)fprintf(stdout,"%*s%s attribute %i: %s, size = %li %s, value = ",prn_ndn,spc_sng,src_sng,idx,att[idx].nm,att_sz,nco_typ_sng(att[idx].type));

    spr_sng=cma_sng; /* [sng] Output separator string */
    if(XML){
      /* Official NcML XML Schema is here:
	 http://www.unidata.ucar.edu/software/thredds/current/netcdf-java/ncml/v2.2/AnnotatedSchema4.html
	 http://www.unidata.ucar.edu/schemas/netcdf/ncml-2.2.xsd */

      (void)fprintf(stdout,"%*s<attribute name=\"%s\"",prn_ndn,spc_sng,att[idx].nm);

      /* User may override default separator string for XML only */
      if(att[idx].type == NC_STRING || att[idx].type == NC_CHAR) spr_sng= (prn_flg->spr_chr) ? prn_flg->spr_chr : spr_xml_chr; else spr_sng= (prn_flg->spr_nmr) ? prn_flg->spr_nmr : spr_xml_nmr;

      /* Print type of non-string variables
	 NB: Take or lose this opportunity to distinguish char from string?
	 NcML does not preserve unsigned types, so why not turn char attributes into strings? 
	 Can turn char into string here just by omitting "type=char" attribute
	 toolsui NcML does not print "type=char" for for char attributes
	 Hence neither does ncks */
      if(att[idx].type != NC_STRING && att[idx].type != NC_CHAR) (void)fprintf(stdout," type=\"%s\"",xml_typ_nm(att[idx].type));
      
      /* Print hidden attributes */
      // if(nco_xml_typ_rqr_nsg_att(att[idx].type)) (void)fprintf(stdout,""); /* toolsui shows no way to indicate unsigned types for attributes? */

      /* Print separator element for non-whitespace separators */
      if((att[idx].sz == 1L && att[idx].type == NC_STRING) || att[idx].sz > 1L){ 
	/* Ensure string attribute value does not contain separator string */
	if(att[idx].type == NC_CHAR)
	  if(strstr(att[idx].val.cp,spr_sng)) spr_sng=spr_xml_chr_bck;
	if(att[idx].type == NC_STRING){
	  for(lmn=0;lmn<att_sz;lmn++){
	    if(strstr(att[idx].val.sngp[lmn],spr_sng)){
	      spr_sng=spr_xml_chr_bck;
	      break;
	    } /* endif */
	  } /* end loop over lmn */
	} /* !NC_STRING */

	size_t spr_sng_idx=0L;
	size_t spr_sng_lng;
	spr_sng_lng=strlen(spr_sng);
	while(spr_sng_idx < spr_sng_lng)
	  if(!isspace(spr_sng[spr_sng_idx])) break; else spr_sng_idx++;
	if(spr_sng_idx < spr_sng_lng) (void)fprintf(stdout," separator=\"%s\"",spr_sng);
      } /* att[idx].sz */
      (void)fprintf(stdout," value=\"");

      /* XML-mode if dataset defines its own _FillValue for this variable? */
      // if(!(int)strcasecmp(att[idx].nm,nco_mss_val_sng_get())) has_fll_val=True;
    } /* !xml */
    
    /* Typecast pointer to values before access */
    (void)cast_void_nctype(att[idx].type,&att[idx].val);

    (void)sprintf(att_sng_pln,"%s",(CDL_OR_JSN) ? nco_typ_fmt_sng_att_cdl(att[idx].type) : (XML) ? nco_typ_fmt_sng_att_xml(att[idx].type) : nco_typ_fmt_sng(att[idx].type));
    (void)sprintf(att_sng_dlm,"%s%%s",(CDL_OR_JSN) ? nco_typ_fmt_sng_att_cdl(att[idx].type) : (XML) ? nco_typ_fmt_sng_att_xml(att[idx].type) : nco_typ_fmt_sng(att[idx].type));
    switch(att[idx].type){
    case NC_FLOAT:
      for(lmn=0;lmn<att_sz;lmn++){
	val_flt=att[idx].val.fp[lmn];
	if(isfinite(val_flt)){
	  rcd_prn=snprintf(val_sng,(size_t)NCO_ATM_SNG_LNG,att_sng_pln,val_flt);
	  (void)sng_trm_trl_zro(val_sng,prn_flg->nbr_zro);
	}else{
	  if(isnan(val_flt)) (void)sprintf(val_sng,"NaNf"); else if(isinf(val_flt)) (void)sprintf(val_sng,"%sInfinityf",(val_flt < 0.0f) ? "-" : "");
	} /* endelse */
	(void)fprintf(stdout,"%s%s",val_sng,(lmn != att_szm1) ? spr_sng : "");
      } /* end loop */
      break;
    case NC_DOUBLE:
      for(lmn=0;lmn<att_sz;lmn++){
	val_dbl=att[idx].val.dp[lmn];
	if(isfinite(val_dbl)){
	  rcd_prn=snprintf(val_sng,(size_t)NCO_ATM_SNG_LNG,att_sng_pln,val_dbl);
	  (void)sng_trm_trl_zro(val_sng,prn_flg->nbr_zro);
	}else{
	  if(isnan(val_dbl)) (void)sprintf(val_sng,"NaNf"); else if(isinf(val_dbl)) (void)sprintf(val_sng,"%sInfinity",(val_dbl < 0.0) ? "-" : "");
	} /* endelse */
	(void)fprintf(stdout,"%s%s",val_sng,(lmn != att_szm1) ? spr_sng : "");
      } /* end loop */
      break;
    case NC_SHORT:
      for(lmn=0;lmn<att_sz;lmn++) (void)fprintf(stdout,att_sng_dlm,att[idx].val.sp[lmn],(lmn != att_szm1) ? spr_sng : "");
      break;
    case NC_INT:
      for(lmn=0;lmn<att_sz;lmn++) (void)fprintf(stdout,att_sng_dlm,(long)att[idx].val.ip[lmn],(lmn != att_szm1) ? spr_sng : "");
      break;
    case NC_CHAR:
      for(lmn=0;lmn<att_sz;lmn++){
	chr_val=att[idx].val.cp[lmn];
	if(CDL_OR_JSN_OR_XML){
	  val_sng[0]='\0';
	  if(lmn == 0L){
	    sng_lng=att_sz;
	    sng_lngm1=sng_lng-1UL;
	    /* Worst case is printable strings are six or four times longer than unformatted, i.e., '\"' == "&quot;" or '\\' == "\\\\" */
	    sng_val_sng_cpy=sng_val_sng=(char *)nco_malloc(6*sng_lng+1UL);
	  } /* endif first element of string array */
	  /* New string begins each element where penultimate dimension changes */
	  if(lmn%sng_lng == 0L){
	    if(CDL_OR_JSN) (void)fprintf(stdout,"\"");
	    sng_val_sng[0]='\0';
	  } /* endif new string */
	  (void)strcat(sng_val_sng,(CDL_OR_JSN) ? chr2sng_cdl(chr_val,val_sng) : chr2sng_xml(chr_val,val_sng));
	  if(chr_val == '\n' && lmn != att_szm1 && CDL_OR_JSN) (void)sprintf(sng_val_sng,"%s\",\n%*s\"",sng_val_sng_cpy,prn_ndn+prn_flg->var_fst,spc_sng);
	  if(lmn%sng_lng == sng_lngm1){
	    (void)fprintf(stdout,"%s%s",sng_val_sng,(CDL_OR_JSN) ? "\"" : "");
	    /* Print separator after non-final string */
	    if(lmn != att_szm1) (void)fprintf(stdout,"%s",spr_sng);
	  } /* endif string end */
	  if(lmn == att_szm1) sng_val_sng=(char *)nco_free(sng_val_sng);
	}else{ /* Traditional */
	  /* Assume \0 is string terminator and do not print it */
	  if(chr_val != '\0') (void)fprintf(stdout,"%c",chr_val);
	} /* endelse CDL, XML, Traditional */
      } /* end loop over element */
      break;
    case NC_BYTE:
      for(lmn=0;lmn<att_sz;lmn++) (void)fprintf(stdout,att_sng_dlm,att[idx].val.bp[lmn],(lmn != att_szm1) ? spr_sng : "");
      break;
    case NC_UBYTE:
      for(lmn=0;lmn<att_sz;lmn++) (void)fprintf(stdout,att_sng_dlm,att[idx].val.ubp[lmn],(lmn != att_szm1) ? spr_sng : "");
      break;
    case NC_USHORT:
      for(lmn=0;lmn<att_sz;lmn++) (void)fprintf(stdout,att_sng_dlm,att[idx].val.usp[lmn],(lmn != att_szm1) ? spr_sng : "");
      break;
    case NC_UINT:
      for(lmn=0;lmn<att_sz;lmn++) (void)fprintf(stdout,att_sng_dlm,att[idx].val.uip[lmn],(lmn != att_szm1) ? spr_sng : "");
      break;
    case NC_INT64:
      for(lmn=0;lmn<att_sz;lmn++) (void)fprintf(stdout,att_sng_dlm,att[idx].val.i64p[lmn],(lmn != att_szm1) ? spr_sng : "");
      break;
    case NC_UINT64:
      for(lmn=0;lmn<att_sz;lmn++) (void)fprintf(stdout,att_sng_dlm,att[idx].val.ui64p[lmn],(lmn != att_szm1) ? spr_sng : "");
      break;
    case NC_STRING:
      for(lmn=0;lmn<att_sz;lmn++){
	sng_val=att[idx].val.sngp[lmn];
	sng_lng=strlen(sng_val);
	sng_lngm1=sng_lng-1UL;
	if(CDL_OR_JSN_OR_XML){
	  /* Worst case is printable strings are six or four times longer than unformatted, i.e., '\"' == "&quot;" or '\\' == "\\\\" */
	  sng_val_sng=(char *)nco_malloc(6*sng_lng+1UL);
	  if(CDL) (void)fprintf(stdout,"\"");
	  sng_val_sng[0]='\0';
	  for(chr_idx=0;chr_idx<sng_lng;chr_idx++){
	    val_sng[0]='\0';
	    chr_val=sng_val[chr_idx];
	    (void)strcat(sng_val_sng,(CDL) ? chr2sng_cdl(chr_val,val_sng) : chr2sng_xml(chr_val,val_sng));
	  } /* end loop over character */
	  (void)fprintf(stdout,"%s%s",sng_val_sng,(XML) ? "" : "\"");
	  /* Print separator after non-final string */
	  if(lmn != att_szm1) (void)fprintf(stdout,"%s",spr_sng);
	  sng_val_sng=(char *)nco_free(sng_val_sng);
	}else if(TRD){ /* Traditional */
	  (void)fprintf(stdout,att_sng_dlm,att[idx].val.sngp[lmn],(lmn != att_szm1) ? spr_sng : "");
	} /* endelse CDL, XML, Traditional */
      } /* end loop over element */
      break;
    default: nco_dfl_case_nc_type_err();
      break;
    } /* end switch */
    if(CDL) (void)fprintf(stdout," ;\n");
    if(JSN) (void)fprintf(stdout,"}%s",(idx < att_nbr_ttl-1) ? ", " : "");
    if(TRD) (void)fprintf(stdout,"\n");
    if(XML) (void)fprintf(stdout,"\" />\n");
    rcd_prn+=0; /* CEWI */
    
  } /* end loop over attributes */

  if(!prn_flg->new_fmt && CDL_OR_TRD) (void)fprintf(stdout,"\n");
  (void)fflush(stdout);
  
  /* Print additional hidden attributes */
  if(XML && var_id != NC_GLOBAL){
    /* _FillValue, _Netcdf4Dimid, _Unsigned:
       _FillValue: No documentation. Seems like a kludge.
       Set to -1 for unsigned types: ubyte, ushort, uint, 
       Set to -2 for unsigned types: uint64
       How will kludge work when unsigned variable already has _FillValue?
       20131231 Based on netCDFJava VKJ-807633 discussion, this is a bug, an artifact of HDF5
       Do not emulate toolsUI _FillValue behavior for unsigned types

       _Netcdf4Dimid: https://bugtracking.unidata.ucar.edu/browse/NCF-244
       "Netcdf4Dimid is an artifact stored in the HDF5 dataset of a dimension scale variable to record dimids when nc_enddef() detects that the coordinate variables have a different creation order than the corresponding dimensions. In that case, the documentation says: If this attribute is present on any dimension scale, it must be present on all dimension scales in the file."
       Is usually (always?) present in array variables in toolsui
       However, it appears at most once even in multi-dimensional arrays
       Thus information provided appears to be ambiguous
       
       _Unsigned: http://www.unidata.ucar.edu/software/thredds/current/netcdf-java/CDM/
       "There are not separate unsigned integer types. The Variable and Array objects have isUnsigned() methods, and conversion to wider types is correctly done. Since Java does not have unsigned types, the alternative is to automatically widen unsigned data arrays, which would double the memory used. */
    nc_type var_type;
    (void)nco_inq_vartype(grp_id,var_id,&var_type);
    if(nco_xml_typ_rqr_nsg_att(var_type)) (void)fprintf(stdout,"%*s<attribute name=\"_Unsigned\" value=\"true\" />\n",prn_ndn,spc_sng);
    /* 20131231: Emulate toolsUI 4.3 _FillValue behavior for unsigned types (present in NCO 4.3.7-4.3.9, deprecated in 4.4.0 */
    // if(nco_xml_typ_rqr_flv_att(var_type) && !has_fll_val) (void)fprintf(stdout,"%*s<attribute name=\"_FillValue\" type=\"%s\" value=\"%d\" />\n",prn_ndn,spc_sng,xml_typ_nm(var_type),(var_type == NC_UINT64) ? -2 : -1);
  } /* !xml */

  /* Free space holding attribute values */
  for(idx=0;idx<att_nbr_ttl;idx++){
    att[idx].val.vp=nco_free(att[idx].val.vp);
    att[idx].nm=(char *)nco_free(att[idx].nm);
  } /* end loop over attributes */

  /* Free rest of space allocated for attribute information */
  if(att_nbr_ttl > 0) att=(att_sct *)nco_free(att);

} /* end nco_prn_att() */

const char * /* O [sng] sprintf() format string for CDL type typ */
nco_typ_fmt_sng_var_cdl /* [fnc] Provide sprintf() format string for specified type in CDL */
(const nc_type typ) /* I [enm] netCDF type to provide CDL format string for */
{
  /* Purpose: Provide sprintf() format string for specified type variable
     Unidata formats shown in ncdump.c near ling 459
     Float formats called float_att_fmt, double_att_fmt are in dumplib.c,
     and are user-configurable with -p float_digits,double_digits.
     These default to 7 and 15, respectively. */

  static const char fmt_NC_FLOAT[]="%.7g"; /* %g defaults to 6 digits of precision */
  static const char fmt_NC_DOUBLE[]="%.15g"; /* %g defaults to 6 digits of precision */
  static const char fmt_NC_INT[]="%i"; /* NCO has stored NC_INT in native type int since 2009. Before that NC_INT was stored as native type long */
  static const char fmt_NC_SHORT[]="%hi";
  static const char fmt_NC_CHAR[]="%c";
  static const char fmt_NC_BYTE[]="%hhi"; /* Takes signed char as arg and prints 0,1,2..,126,127,-127,-126,...-2,-1 */

  static const char fmt_NC_UBYTE[]="%hhu"; /*  */
  static const char fmt_NC_USHORT[]="%hu"; /*  */
  static const char fmt_NC_UINT[]="%u"; /*  */
  static const char fmt_NC_INT64[]="%lli"; /*  */
  static const char fmt_NC_UINT64[]="%llu"; /*  */
  static const char fmt_NC_STRING[]="\"%s\""; /*  */

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
} /* end nco_typ_fmt_sng_var_cdl() */

const char * /* O [sng] sprintf() format string for CDL attribute type typ */
nco_typ_fmt_sng_att_cdl /* [fnc] Provide sprintf() format string for specified attribute type in CDL */
(const nc_type typ) /* I [enm] netCDF attribute type to provide CDL format string for */
{
  /* Purpose: Provide sprintf() format string for specified type attribute in CDL
     Unidata formats shown in ncdump.c near line 459
     Float formats called float_att_fmt, double_att_fmt are in dumplib.c,
     and are user-configurable with -p float_digits,double_digits.
     These default to 7 and 15, respectively. */

  static const char fmt_NC_FLOAT[]="%#.7gf"; /* %g defaults to 6 digits of precision */
  static const char fmt_NC_DOUBLE[]="%#.15g"; /* %g defaults to 6 digits of precision */
  static const char fmt_NC_INT[]="%i"; /* NCO has stored NC_INT in native type int since 2009. Before that NC_INT was stored as native type long */
  static const char fmt_NC_SHORT[]="%his";
  static const char fmt_NC_CHAR[]="%c";
  static const char fmt_NC_BYTE[]="%hhib"; /* Takes signed char as arg and prints 0,1,2..,126,127,-127,-126,...-2,-1 */

  static const char fmt_NC_UBYTE[]="%hhuub"; /*  */
  static const char fmt_NC_USHORT[]="%huus"; /*  */
  static const char fmt_NC_UINT[]="%uu"; /*  */
  static const char fmt_NC_INT64[]="%llil"; /*  */
  static const char fmt_NC_UINT64[]="%lluul"; /*  */
  static const char fmt_NC_STRING[]="\"%s\""; /*  */

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
} /* end nco_typ_fmt_sng_att_cdl() */

const char * /* O [sng] sprintf() format string for XML attribute type typ */
nco_typ_fmt_sng_att_xml /* [fnc] Provide sprintf() format string for specified attribute type in XML */
(const nc_type typ) /* I [enm] netCDF attribute type to provide XML format string for */
{
  /* Purpose: Provide sprintf() format string for specified type attribute
     Unidata formats shown in ncdump.c near line 459
     Float formats called float_att_fmt, double_att_fmt are in dumplib.c,
     and are user-configurable with -p float_digits,double_digits.
     These default to 7 and 15, respectively
     Use these formats for XML attributes AND variables because distinct formats for XML variables, if any, are not known (because ncdump does not dump XML variable values */

  static const char fmt_NC_FLOAT[]="%#.7g"; /* %g defaults to 6 digits of precision */
  static const char fmt_NC_DOUBLE[]="%#.15g"; /* %g defaults to 6 digits of precision */
  static const char fmt_NC_INT[]="%i"; /* NCO has stored NC_INT in native type int since 2009. Before that NC_INT was stored as native type long */
  static const char fmt_NC_SHORT[]="%hi";
  static const char fmt_NC_CHAR[]="%c";
  static const char fmt_NC_BYTE[]="%hhi"; /* Takes signed char as arg and prints 0,1,2..,126,127,-127,-126,...-2,-1 */

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
} /* end nco_typ_fmt_sng_att_xml() */

const char * /* O [sng] sprintf() format string for type typ */
nco_typ_fmt_sng /* [fnc] Provide sprintf() format string for specified type */
(const nc_type typ) /* I [enm] netCDF type to provide format string for */
{
  /* Purpose: Provide sprintf() format string for specified type 
     nco_typ_fmt_sng() is "master" NCO formatting function that defines formatting 
     in traditional NCO mode, i.e., when not explicitly requesting CDL or XML */

  static const char fmt_NC_FLOAT[]="%g"; /* %g defaults to 6 digits of precision */
  static const char fmt_NC_DOUBLE[]="%.12g"; /* %g defaults to 6 digits of precision */
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
     NB: routine does not correctly print hyperslabs which are wrapped, or which use non-unity stride
     NB: nco_prn_var_val_lmt() is simple, pre-MSA, pre-GTT code kept only for historical interest 
     It lacks most of the clutter caused by weird corner cases */

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
      (void)fprintf(stdout,"%s: ERROR %s does not print variable hyperslabs where one or more of the coordinates is wrapped using the -d option (i.e., where the minimum index exceeds the maximum index such as longitude hyperslabs which cross the date-line. The workaround is to hyperslab into a new file (without -H) and then to print the values from that file (with -H).\n",nco_prg_nm_get(),nco_prg_nm_get());
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
    (void)fprintf(stderr,"%s: ERROR unable to malloc() %lu bytes for %s\n",nco_prg_nm_get(),(unsigned long)var.sz*nco_typ_lng(var.type),var.nm);
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
	if(nco_dbg_lvl_get() >= 6)(void)fprintf(stdout,"DEBUG: format string used for chars is dmn_sng = %s, var_sng = %s\n",dmn_sng,var_sng); 
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
nco_prn_var_dfn                     /* [fnc] Print variable metadata */
(const int nc_id,                   /* I [id] netCDF file ID */
 const prn_fmt_sct * const prn_flg, /* I [sct] Print-format information */
 const trv_sct * const var_trv)     /* I [sct] Object to print (variable) */
{
  /* Purpose: Print variable metadata */
  const char spc_sng[]=""; /* [sng] Space string */

  char sz_sng[100];
  char dmn_sng[200];
  char sng_foo[200];

  char *nm_cdl;

  int deflate; /* [flg] Deflation is on */
  int dfl_lvl; /* [enm] Deflate level [0..9] */
  int dmn_idx;
  int grp_id;
  int nbr_att;
  int nbr_dim;
  int packing; /* [flg] Variable is packed */
  int prn_ndn=0; /* [nbr] Indentation for printing */
  int shuffle; /* [flg] Shuffling is on */
  int srg_typ; /* [enm] Storage type */
  int var_id;

  long var_sz=1L;

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

  /* Storage properties */
  if(nco_fmt_xtn_get() != nco_fmt_xtn_hdf4){
    (void)nco_inq_var_chunking(grp_id,var_id,&srg_typ,cnk_sz);
    (void)nco_inq_var_deflate(grp_id,var_id,&shuffle,&deflate,&dfl_lvl);
  } /* endif */
  (void)nco_inq_var_packing(grp_id,var_id,&packing);

  /* Loop over dimensions */
  for(dmn_idx=0;dmn_idx<nbr_dim;dmn_idx++){

    /* This dimension has a coordinate variable */
    if(var_trv->var_dmn[dmn_idx].is_crd_var){

      /* Get coordinate from table */
      crd_sct *crd=var_trv->var_dmn[dmn_idx].crd;

      /* Use the hyperslabbed size */
      dmn_sz[dmn_idx]=crd->lmt_msa.dmn_cnt;

      CRR_DMN_IS_REC_IN_INPUT[dmn_idx]=crd->is_rec_dmn;

    }else if(var_trv->var_dmn[dmn_idx].is_crd_var == False){

      /* Dimension does not have associated coordinate variable */
      /* Get unique dimension */

      dmn_trv_sct *dmn_trv=var_trv->var_dmn[dmn_idx].ncd;

      /* Use the hyperslabbed size */
      dmn_sz[dmn_idx]=dmn_trv->lmt_msa.dmn_cnt;

      CRR_DMN_IS_REC_IN_INPUT[dmn_idx]=dmn_trv->is_rec_dmn;
    } /* end else */

  } /* end loop over dimensions */

  /* Print header for variable */
  if(prn_flg->new_fmt && !prn_flg->xml) prn_ndn=prn_flg->sxn_fst+prn_flg->var_fst+var_trv->grp_dpt*prn_flg->spc_per_lvl;
  if(prn_flg->xml) prn_ndn=prn_flg->sxn_fst+var_trv->grp_dpt*prn_flg->spc_per_lvl;
  if(prn_flg->trd && (nco_fmt_xtn_get() == nco_fmt_xtn_hdf4)) (void)fprintf(stdout,"%*s%s: type %s, %i dimension%s, %i attribute%s, chunked? HDF4_UNKNOWN, compressed? HDF4_UNKNOWN, packed? %s\n",prn_ndn,spc_sng,var_trv->nm,nco_typ_sng(var_typ),nbr_dim,(nbr_dim == 1) ? "" : "s",nbr_att,(nbr_att == 1) ? "" : "s",(packing) ? "yes" : "no");
  if(prn_flg->trd && (nco_fmt_xtn_get() != nco_fmt_xtn_hdf4)) (void)fprintf(stdout,"%*s%s: type %s, %i dimension%s, %i attribute%s, chunked? %s, compressed? %s, packed? %s\n",prn_ndn,spc_sng,var_trv->nm,nco_typ_sng(var_typ),nbr_dim,(nbr_dim == 1) ? "" : "s",nbr_att,(nbr_att == 1) ? "" : "s",(srg_typ == NC_CHUNKED) ? "yes" : "no",(deflate) ? "yes" : "no",(packing) ? "yes" : "no");

  if(prn_flg->xml) (void)fprintf(stdout,"%*s<variable name=\"%s\" type=\"%s\"",prn_ndn,spc_sng,var_trv->nm,xml_typ_nm(var_typ));

  /* Print type, shape, and total size of variable */
  dmn_sng[0]='\0';
  if(nbr_dim == 0){
    if(prn_flg->trd) (void)fprintf(stdout,"%*s%s size (RAM) = %ld*sizeof(%s) = %ld*%lu = %lu bytes\n",prn_ndn,spc_sng,var_trv->nm,var_sz,nco_typ_sng(var_typ),var_sz,(unsigned long)nco_typ_lng(var_typ),(unsigned long)(var_sz*nco_typ_lng(var_typ)));
    /* 20131122: Implement ugly NcML requirement that scalars have shape="" attribute */
    if(prn_flg->xml) (void)sprintf(dmn_sng," shape=\"\"");
  }else{
    for(dmn_idx=0;dmn_idx<nbr_dim;dmn_idx++){
      if(prn_flg->xml){
	(void)sprintf(sng_foo,"%s%s%s",(dmn_idx == 0) ? " shape=\"" : "",var_trv->var_dmn[dmn_idx].dmn_nm,(dmn_idx < nbr_dim-1) ? " " : "\""); 
      }else{
	nm_cdl=nm2sng_cdl(var_trv->var_dmn[dmn_idx].dmn_nm);
	(void)sprintf(sng_foo,"%s%s%s",(dmn_idx == 0) ? "(" : "",nm_cdl,(dmn_idx < nbr_dim-1) ? "," : ")");
	nm_cdl=(char *)nco_free(nm_cdl);
      } /* !xml */
      (void)strcat(dmn_sng,sng_foo);
    } /* end loop over dim */

    sz_sng[0]='\0';
    for(dmn_idx=0;dmn_idx<nbr_dim-1;dmn_idx++){
      (void)sprintf(sng_foo,"%li*",(long)dmn_sz[dmn_idx]);
      (void)strcat(sz_sng,sng_foo);
    } /* end loop over dim */
    (void)sprintf(sng_foo,"%li*sizeof(%s)",(long)dmn_sz[dmn_idx],nco_typ_sng(var_typ));
    (void)strcat(sz_sng,sng_foo);

    for(dmn_idx=0;dmn_idx<nbr_dim;dmn_idx++) var_sz*=dmn_sz[dmn_idx];
    if(nco_fmt_xtn_get() != nco_fmt_xtn_hdf4) (void)nco_inq_var_deflate(grp_id,var_id,&shuffle,&deflate,&dfl_lvl);

    if(prn_flg->trd){
      if((nco_fmt_xtn_get() != nco_fmt_xtn_hdf4) && deflate) (void)fprintf(stdout,"%*s%s compression (Lempel-Ziv %s shuffling) level = %d\n",prn_ndn,spc_sng,var_trv->nm,(shuffle) ? "with" : "without",dfl_lvl);
      if(nco_fmt_xtn_get() == nco_fmt_xtn_hdf4) (void)fprintf(stdout,"%*s%s compression and shuffling characteristics are HDF4_UNKNOWN\n",prn_ndn,spc_sng,var_trv->nm);
      (void)fprintf(stdout,"%*s%s size (RAM) = %s = %li*%lu = %lu bytes\n",prn_ndn,spc_sng,var_trv->nm,sz_sng,var_sz,(unsigned long)nco_typ_lng(var_typ),(unsigned long)(var_sz*nco_typ_lng(var_typ)));
    } /* !prn_flg->trd */

  } /* end if variable is scalar */
  if(prn_flg->cdl){
    nm_cdl=nm2sng_cdl(var_trv->nm);
    (void)fprintf(stdout,"%*s%s %s%s ;\n",prn_ndn,spc_sng,cdl_typ_nm(var_typ),nm_cdl,dmn_sng);
    nm_cdl=(char *)nco_free(nm_cdl);
  } /* !cdl */
  if(prn_flg->xml){
    if(prn_flg->PRN_VAR_DATA || prn_flg->PRN_VAR_METADATA) (void)fprintf(stdout,"%s>\n",dmn_sng); else (void)fprintf(stdout,"%s />\n",dmn_sng);
  } /* !xml */

  /* Print dimension sizes and names */

  /* Loop dimensions for object (variable)  */
  if(prn_flg->trd){
    for(dmn_idx=0;dmn_idx<var_trv->nbr_dmn;dmn_idx++){

      /* Casting from "size_t" to "unsigned long" necessary since %zu format only available in C99 */
      /* Coordinate dimension */
      if(var_trv->var_dmn[dmn_idx].is_crd_var){

        crd_sct *crd=var_trv->var_dmn[dmn_idx].crd;

        /* NOTE: Use hyperslabbed sizes for dimension size */
        if(srg_typ == NC_CHUNKED) (void)fprintf(stdout,"%*s%s dimension %i: %s, size = %li %s, chunksize = %lu (",prn_ndn,spc_sng,var_trv->nm,dmn_idx,(!strcmp(crd->dmn_grp_nm_fll,var_trv->grp_nm_fll)) ? crd->nm : crd->dmn_nm_fll,crd->lmt_msa.dmn_cnt,nco_typ_sng(crd->var_typ),(unsigned long)cnk_sz[dmn_idx]); else (void)fprintf(stdout,"%*s%s dimension %i: %s, size = %li %s (",prn_ndn,spc_sng,var_trv->nm,dmn_idx,(!strcmp(crd->dmn_grp_nm_fll,var_trv->grp_nm_fll)) ? crd->nm : crd->dmn_nm_fll,crd->lmt_msa.dmn_cnt,nco_typ_sng(crd->var_typ));

        (void)fprintf(stdout,"%soordinate is %s)",(CRR_DMN_IS_REC_IN_INPUT[dmn_idx]) ? "Record c" : "C",(!strcmp(crd->crd_grp_nm_fll,var_trv->grp_nm_fll)) ? crd->nm : crd->crd_nm_fll);

        /* Non-coordinate dimension */
      }else if(!var_trv->var_dmn[dmn_idx].is_crd_var){

        dmn_trv_sct *dmn_trv=var_trv->var_dmn[dmn_idx].ncd;

        /* NOTE: Use hyperslabbed sizes for dimension size */
        if(srg_typ == NC_CHUNKED) (void)fprintf(stdout,"%*s%s dimension %i: %s, size = %li, chunksize = %lu (",prn_ndn,spc_sng,var_trv->nm,dmn_idx,(!strcmp(dmn_trv->grp_nm_fll,var_trv->grp_nm_fll)) ? dmn_trv->nm : dmn_trv->nm_fll,dmn_trv->lmt_msa.dmn_cnt,(unsigned long)cnk_sz[dmn_idx]); else (void)fprintf(stdout,"%*s%s dimension %i: %s, size = %li (",prn_ndn,spc_sng,var_trv->nm,dmn_idx,(!strcmp(dmn_trv->grp_nm_fll,var_trv->grp_nm_fll)) ? dmn_trv->nm : dmn_trv->nm_fll,dmn_trv->lmt_msa.dmn_cnt);
        (void)fprintf(stdout,"%son-coordinate dimension)",(CRR_DMN_IS_REC_IN_INPUT[dmn_idx]) ? "Record n" : "N");

      } /* end non-coordinate dimension */
      (void)fprintf(stdout,"\n"); 
    } /* end loop over dimensions */

    /* Caveat user */
    if((nc_type)var_typ == NC_STRING) (void)fprintf(stdout,"%*s%s size (RAM) above is space required for pointers only, full size of strings is unknown until data are read\n",prn_ndn,spc_sng,var_trv->nm);
  } /* !prn_flg->trd */

  (void)fflush(stdout);
} /* end nco_prn_var_dfn() */

void
nco_prn_var_val_trv /* [fnc] Print variable data (GTT version) */
(const int nc_id, /* I [ID] netCDF file ID */
 const prn_fmt_sct * const prn_flg, /* I [sct] Print-format information */
 const trv_sct * const var_trv, /* I [sct] Object to print (variable) */
 const trv_tbl_sct * const trv_tbl) /* I [sct] GTT (Group Traversal Table) */
{
  /* Purpose:
     Get variable with limits from input file
     User supplied dlm_sng, print var (includes nbr_dim == 0)
     Get dimensional units
     if nbr_dim == 0 and dlm_sng == NULL Print variable
     if PRN.. == False Print variable taking account of FORTRAN (need variable indices)
     if PRN_DMN_IDX_CRD_VAL then read in co-ordinate dimensions
     if PRN.. == True Print variable taking account of FORTRAN (Use dimensions to calculate variable indices)
     
     Similar to nco_msa_prn_var_val() but uses limit information contained in GTT 
     Differences are marked "GTT"
     1) It is not needed to retrieve dimension IDs for variable, these were used in nco_msa_prn_var_val()
     to match limits; Group Traversal Table (GTT) should be "ID free".
     2) Object to print (variable) is passed as parameter
     3) MSA: Modulo arrays: Changing subscript of first (least rapidly varying) dimension by one moves most quickly through 
     address space. Changing the subscript of the last (most rapidly varying) dimension by one moves exactly one location 
     (e.g., 8 bytes for a double) in address space. Each dimension has its own "stride" or length of RAM space between
     consecutive entries. mod_map_in and mod_map_cnt keep track of these distances. They are mappings between index-based 
     access and RAM-based access. The location of an N-dimensional array element in RAM is the sum of the products of 
     each index (dimensional subscript) times the stride (mod_map) of the corresponding dimension.
     
     Limit Tests:
     ncks -D 11 -d lat,1,1,1  -v area -H ~/nco/data/in_grp.nc # area(lat)
     ncks -D 11 -v unique -H ~/nco/data/in_grp.nc # scalar
     ncks -D 11 -C -d time,1,2,1 -v two_dmn_rec_var -H ~/nco/data/in_grp.nc # two_dmn_rec_var(time,lev);
     ncks -D 11 -C -d time,1,2,1 -d lev,1,1,1 -v two_dmn_rec_var -H ~/nco/data/in_grp.nc # two_dmn_rec_var(time,lev);           
     Tests for coordinate variables in ancestor groups:
     ncks -D 11 -d lat,1,1,1 -H  -v area ~/nco/data/in_grp.nc */

  const char fnc_nm[]="nco_prn_var_val_trv()"; /* [sng] Function name  */
  const char spc_sng[]=""; /* [sng] Space string */

  char cma_sng[]=", "; /* [sng] Comma string */
  char mss_val_sng[]="_"; /* [sng] Print this instead of numerical missing value */
  char spr_xml_chr[]="*"; /* [sng] Default XML separator for character types */
  char spr_xml_chr_bck[]="*|*"; /* [sng] Backup default XML separator for character types */
  char spr_xml_nmr[]=" "; /* [sng] Default XML separator for numeric types */
  char nul_chr='\0'; /* [sng] Character to end string */ 

  char *dlm_sng=NULL;                        /* [sng] User-specified delimiter string, if any */
  char *nm_cdl;
  char *sng_val_sng;                         /* [sng] String of NC_CHAR */
  char *sng_val_sng_cpy; /* [sng] Copy of sng_val_sng to avoid cppcheck error about using sng_val_sng as both parameter and desitnation in sprintf(). NB: free() only one of these two pointers. */
  char *spr_sng=NULL; /* [sng] Output separator string */
  char *unit_sng;                            /* [sng] Units string */ 

  char chr_val;                              /* [sng] Current character */

  char val_sng[NCO_ATM_SNG_LNG];
  char var_nm[NC_MAX_NAME+1];                /* [sng] Variable name (used for validation only) */ 
  char var_sng[NCO_MAX_LEN_FMT_SNG];         /* [sng] Variable string */

  dmn_sct dim[NC_MAX_DIMS];                  /* [sct] Dimension structure  */

  double val_dbl;

  float val_flt;

  int dmn_idx;                              /* [idx] Counter over dimensions */
  int grp_id;                                 /* [ID] Group ID where variable resides (passed to MSA) */
  int rcd_prn;
  int prn_ndn=0;                             /* [nbr] Indentation for printing */
  int val_sz_byt=int_CEWI;                   /* [nbr] Type size */

  lmt_msa_sct **lmt_msa=NULL_CEWI;           /* [sct] MSA Limits for only for variable dimensions  */          
  lmt_sct **lmt=NULL_CEWI;                   /* [sct] Auxiliary Limit used in MSA */

  long dmn_sbs_dsk[NC_MAX_DIMS];             /* [nbr] Indices of hyperslab relative to original on disk */
  long dmn_sbs_ram[NC_MAX_DIMS];             /* [nbr] Indices in hyperslab */
  long lmn;                                  /* [nbr] Index to print variable data */
  long mod_map_cnt[NC_MAX_DIMS];             /* [nbr] MSA modulo array */
  long mod_map_in[NC_MAX_DIMS];              /* [nbr] MSA modulo array */
  long sng_lng;                              /* [nbr] Length of NC_CHAR string */
  long sng_lngm1;                            /* [nbr] Length minus one of NC_CHAR string */
  long var_dsk;                              /* [nbr] Variable index relative to disk */
  long var_szm1;

  const nco_bool CDL=prn_flg->cdl; /* [flg] CDL output */
  const nco_bool XML=prn_flg->xml; /* [flg] XML output */
  const nco_bool TRD=prn_flg->trd; /* [flg] Traditional output */
  const nco_bool JSN=prn_flg->jsn; /* [flg] JSON output */
  const nco_bool CDL_OR_JSN=prn_flg->cdl || prn_flg->jsn; /* [flg] CDL or JSON output */
  const nco_bool CDL_OR_TRD=prn_flg->cdl || prn_flg->trd; /* [flg] CDL or Traditional output */
  const nco_bool CDL_OR_JSN_OR_XML=prn_flg->cdl || prn_flg->jsn || prn_flg->xml; /* [flg] CDL or JSON or XML output */

  nco_bool is_mss_val=False;                 /* [flg] Current value is missing value */
  nco_bool MALLOC_UNITS_SNG=False;           /* [flg] Allocated memory for units string */

  nco_string sng_val;                        /* [sng] Current string */

  var_sct var;                               /* [sct] Variable structure */
  var_sct var_crd;                           /* [sct] Variable structure for associated coordinate variable */

  if(prn_flg->new_fmt && CDL_OR_TRD) prn_ndn=prn_flg->ndn+prn_flg->var_fst;
  if(XML) prn_ndn=prn_flg->ndn;

  /* Obtain group ID where variable is located using full group name */
  (void)nco_inq_grp_full_ncid(nc_id,var_trv->grp_nm_fll,&grp_id);

  /* Set defaults */
  (void)var_dfl_set(&var); 

  /* Initialize units string, overwrite later if necessary */
  unit_sng=&nul_chr;

  /* Get ID for requested variable */
  var.nm=(char *)strdup(var_trv->nm);
  var.nc_id=grp_id;
  (void)nco_inq_varid(grp_id,var_trv->nm,&var.id);

  /* Get type of variable (get also name and number of dimensions for validation against parameter object) */
  (void)nco_inq_var(grp_id,var.id,var_nm,&var.type,&var.nbr_dim,(int *)NULL,(int *)NULL);

  /* Ensure we have correct variable */
  assert(var_trv->nco_typ == nco_obj_typ_var);
  assert(var.nbr_dim == var_trv->nbr_dmn);
  assert(!strcmp(var_nm,var_trv->nm));

  /* Scalars */
  if(var.nbr_dim == 0){
    var.sz=1L;
    var.val.vp=nco_malloc(nco_typ_lng(var.type));
    /* Block is critical/thread-safe for identical/distinct grp_id's */
    { /* begin potential OpenMP critical */
      (void)nco_get_var1(grp_id,var.id,0L,var.val.vp,var.type);
    } /* end potential OpenMP critical */
  } else { /* ! Scalars */
    /* Allocate local MSA */
    lmt_msa=(lmt_msa_sct **)nco_malloc(var_trv->nbr_dmn*sizeof(lmt_msa_sct *));
    lmt=(lmt_sct **)nco_malloc(var_trv->nbr_dmn*sizeof(lmt_sct *));

    /* Copy from table to local MSA */
    (void)nco_cpy_msa_lmt(var_trv,&lmt_msa);

    /* Call super-dooper recursive routine */
    var.val.vp=nco_msa_rcr_clc((int)0,var.nbr_dim,lmt,lmt_msa,&var);
  } /* ! Scalars */

  /* Call also initializes var.sz with final size */
  if(prn_flg->md5)
    if(prn_flg->md5->dgs) (void)nco_md5_chk(prn_flg->md5,var_nm,var.sz*nco_typ_lng(var.type),grp_id,(long *)NULL,(long *)NULL,var.val.vp);

  /* Warn if variable is packed */
  if(nco_dbg_lvl_get() > 0)
    if(nco_pck_dsk_inq(grp_id,&var))
      (void)fprintf(stderr,"%s: WARNING will print packed values of variable \"%s\". Unpack first (with ncpdq -U) to see actual values.\n",nco_prg_nm_get(),var_nm);

  /* Refresh number of attributes and missing value attribute, if any */
  var.has_mss_val=nco_mss_val_get(var.nc_id,&var);
  if(var.has_mss_val) val_sz_byt=nco_typ_lng(var.type);

  /* User-supplied dlm_sng, print variable (includes nbr_dmn == 0) */  
  if(prn_flg->dlm_sng) dlm_sng=strdup(prn_flg->dlm_sng); /* [sng] User-specified delimiter string, if any */
  if(dlm_sng){
    char *fmt_sng_mss_val=NULL;

    /* Print each element with user-supplied formatting code */
    /* Replace C language '\X' escape codes with ASCII bytes */
    (void)sng_ascii_trn(dlm_sng);

    /* Assume -s argument (dlm_sng) formats entire string
    Otherwise, one could assume that field will be printed with format nco_typ_fmt_sng(var.type),
    and that user is only allowed to affect text between fields. 
    This would be accomplished with:
    (void)sprintf(var_sng,"%s%s",nco_typ_fmt_sng(var.type),dlm_sng);*/

    /* Find replacement format string at most once, then re-use */
#ifdef NCO_HAVE_REGEX_FUNCTIONALITY
    /* Replace printf()-format statements with format for missing values */
    fmt_sng_mss_val=nco_fmt_sng_printf_subst(dlm_sng);
#endif /* !NCO_HAVE_REGEX_FUNCTIONALITY */

    for(lmn=0;lmn<var.sz;lmn++){

      /* memcmp() triggers pedantic warning unless pointer arithmetic is cast to type char * */
      if(prn_flg->PRN_MSS_VAL_BLANK) is_mss_val = var.has_mss_val ? !memcmp((char *)var.val.vp+lmn*val_sz_byt,var.mss_val.vp,(size_t)val_sz_byt) : False; 

      if(prn_flg->PRN_MSS_VAL_BLANK && is_mss_val){
        if(strcmp(dlm_sng,fmt_sng_mss_val)) (void)fprintf(stdout,fmt_sng_mss_val,mss_val_sng); else (void)fprintf(stdout,"%s, ",mss_val_sng);
      }else{ /* !is_mss_val */
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
      } /* !is_mss_val */
    } /* end loop over element */
    (void)fprintf(stdout,"\n");

    if(fmt_sng_mss_val) fmt_sng_mss_val=(char *)nco_free(fmt_sng_mss_val);
  } /* end if dlm_sng */

  spr_sng=cma_sng; /* [sng] Output separator string */
  if(CDL_OR_JSN_OR_XML){
    char fmt_sng[NCO_MAX_LEN_FMT_SNG];
    dmn_trv_sct *dmn_trv; /* [sct] Unique dimension object */
    int cpd_rec_dmn_idx[NC_MAX_DIMS]; /* [idx] Indices of non-leading record dimensions */
    int cpd_nbr=0; /* [nbr] Number of non-leading record dimensions */
    long chr_idx;
    nco_bool is_compound; /* [flg] Variable is compound (has non-leading record dimension) */
    nco_bool cpd_rec_dmn[NC_MAX_DIMS]; /* [flg] Dimension is compound */
    char * (*chr2sng_sf)(const char chr_val, /* I [chr] Character to process */
			  char * const val_sng); /* I/O [sng] String to stuff printable result into */
    if(CDL) chr2sng_sf=chr2sng_cdl; else chr2sng_sf=chr2sng_xml;
    if(CDL_OR_JSN) (void)sprintf(fmt_sng,"%s",nco_typ_fmt_sng_var_cdl(var.type));
    if(XML) (void)sprintf(fmt_sng,"%s",nco_typ_fmt_sng_att_xml(var.type));
    if(JSN) (void)fprintf(stdout,"\"data\": [");

    nm_cdl=nm2sng_cdl(var_nm);
    if(XML){
      /* User may override default separator string for XML only */
      if(var.type == NC_STRING || var.type == NC_CHAR) spr_sng= (prn_flg->spr_chr) ? prn_flg->spr_chr : spr_xml_chr; else spr_sng= (prn_flg->spr_nmr) ? prn_flg->spr_nmr : spr_xml_nmr;

      (void)fprintf(stdout,"%*s<values",prn_ndn+prn_flg->var_fst,spc_sng);
      /* Print non-whitespace separators between elements */
      if((var.sz == 1L && var.type == NC_STRING) || var.sz > 1L){
	/* Ensure string variable value does not contain separator string */
	if(var.type == NC_CHAR)
	  if(strstr(var.val.cp,spr_sng)) spr_sng=spr_xml_chr_bck;
	if(var.type == NC_STRING){
	  for(lmn=0;lmn<var.sz;lmn++){
	    if(strstr(var.val.sngp[lmn],spr_sng)){
	      spr_sng=spr_xml_chr_bck;
	      break;
	    } /* endif */
	  } /* end loop over lmn */
	} /* !NC_STRING */

	size_t spr_sng_idx=0L;
	size_t spr_sng_lng;
	static short FIRST_WARNING=True;
	spr_sng_lng=strlen(spr_sng);
	while(spr_sng_idx < spr_sng_lng)
	  if(!isspace(spr_sng[spr_sng_idx])) break; else spr_sng_idx++;
	if(spr_sng_idx < spr_sng_lng) (void)fprintf(stdout," separator=\"%s\"",spr_sng);
	if(var.type == NC_CHAR && var.nbr_dim > 1 && FIRST_WARNING && nco_dbg_lvl_get() > 0){
	  /* 20131122: Warnings about XML ambiguities caused by spr_sng appearing in val_sng would go here
	     New procedure to pre-check strings above obviates need for this in all all cases except one:
	     Multi-dimensional NC_CHAR variables may have embedded NULs that prevent strstr(val,spr_sng)
	     appearances of spr_sng after first NUL. Could use GNU-specific memmem() instead?
	     Balance of simplicity and readability suggests warning at most once */
	  (void)fprintf(stderr,"%s: WARNING %s converting to NcML multi-dimensional variable %s, presumably an array of strings of type %s, with NcML separator \"%s\". NCO performs precautionary checks with strstr(val,spr) to identify presence of separator string (spr) in data (val) and, if it detects a match, automatically switches to a backup separator string (\"%s\"). However limitations of strstr() may lead to false negatives when separator string occurs in data beyond the first string in multi-dimensional NC_CHAR arrays. Hence, result may be ambiguous to NcML parsers. HINT: If problems arise, use --xml_spr_chr to specify a multi-character separator that 1. does not appear in the string array and 2. does not include an NcML formatting characters (e.g., commas, angles, quotes). This warning is printed at most once per file.\n",nco_prg_nm_get(),fnc_nm,var.nm,nco_typ_sng(var.type),spr_sng,spr_xml_chr_bck);
	  FIRST_WARNING=False;
	} /* endif WARNING */
      } /* var.sz */
      (void)fprintf(stdout,">");
    } /* !xml */
    if(CDL) (void)fprintf(stdout,"%*s%s = ",prn_ndn,spc_sng,nm_cdl);
    nm_cdl=(char *)nco_free(nm_cdl);
    var_szm1=var.sz-1L;
    is_compound=nco_prn_cpd_chk(var_trv,trv_tbl);

    /* Pre-compute elements that need brace punctuation */
    if(is_compound){
      /* Create brace list */
      for(dmn_idx=1;dmn_idx<var.nbr_dim;dmn_idx++){ /* NB: dimension index starts at 1 */
        dmn_trv=nco_dmn_trv_sct(var_trv->var_dmn[dmn_idx].dmn_id,trv_tbl); 
        cpd_rec_dmn[dmn_idx]=False;
        if(dmn_trv->is_rec_dmn){
          cpd_rec_dmn[dmn_idx]=True; 
          cpd_rec_dmn_idx[cpd_nbr]=dmn_idx;
          cpd_nbr++;
        } /* endif */
      } /* end loop over dimensions */
    } /* !is_compound */

    for(lmn=0;lmn<var.sz;lmn++){

      /* memcmp() triggers pedantic warning unless pointer arithmetic is cast to type char * */
      if(prn_flg->PRN_MSS_VAL_BLANK) is_mss_val = var.has_mss_val ? !memcmp((char *)var.val.vp+lmn*val_sz_byt,var.mss_val.vp,(size_t)val_sz_byt) : False; 

      if(prn_flg->PRN_MSS_VAL_BLANK && is_mss_val){
        (void)sprintf(val_sng,"%s",mss_val_sng);
      }else{ /* !is_mss_val */
        switch(var.type){
        case NC_FLOAT: 
          val_flt=var.val.fp[lmn];
          if(isfinite(val_flt)){
            rcd_prn=snprintf(val_sng,(size_t)NCO_ATM_SNG_LNG,fmt_sng,val_flt);
            (void)sng_trm_trl_zro(val_sng,prn_flg->nbr_zro);
          }else{
            if(isnan(val_flt)) (void)sprintf(val_sng,"NaN"); else if(isinf(val_flt)) (void)sprintf(val_sng,"%sInfinity",(val_flt < 0.0f) ? "-" : "");
          } /* endelse */
          break;
        case NC_DOUBLE:
          val_dbl=var.val.dp[lmn];
          if(isfinite(val_dbl)){
            rcd_prn=snprintf(val_sng,(size_t)NCO_ATM_SNG_LNG,fmt_sng,val_dbl);
            (void)sng_trm_trl_zro(val_sng,prn_flg->nbr_zro);
          }else{
            if(isnan(val_dbl)) (void)sprintf(val_sng,"NaN"); else if(isinf(val_dbl)) (void)sprintf(val_sng,"%sInfinity",(val_dbl < 0.0) ? "-" : "");
          } /* endelse */
          break;
        case NC_SHORT: (void)sprintf(val_sng,fmt_sng,var.val.sp[lmn]); break;
        case NC_INT: (void)sprintf(val_sng,fmt_sng,var.val.ip[lmn]); break;
        case NC_CHAR: 
          chr_val=var.val.cp[lmn];
          if(var.nbr_dim == 0){
            if(CDL_OR_TRD) (void)fprintf(stdout,"\"");
            if(chr_val != '\0') (void)fprintf(stdout,"%s",(*chr2sng_sf)(chr_val,val_sng));
            if(CDL_OR_TRD) (void)fprintf(stdout,"\"");
            val_sng[0]='\0';
          }else{ /* var.nbr_dim > 0 */
            /* Multi-dimensional string arrays of NC_CHAR */
            val_sng[0]='\0';
            if(lmn == 0L){
              sng_lng=lmt_msa[var.nbr_dim-1]->dmn_cnt;
              sng_lngm1=sng_lng-1UL;
              /* Worst case is printable strings are four times longer than unformatted, i.e. '\\' == "\\\\" */
              sng_val_sng_cpy=sng_val_sng=(char *)nco_malloc(4*sng_lng+1UL);
            } /* endif first element of string array */
            /* New string begins each element where penultimate dimension changes */
            if(lmn%sng_lng == 0L){
              if(CDL) (void)fprintf(stdout,"\"");
              sng_val_sng[0]='\0';
            } /* endif new string */
            if(chr_val != '\0') (void)fprintf(stdout,"%s",(*chr2sng_sf)(chr_val,val_sng));
            if(chr_val == '\n' && lmn != var_szm1) (void)sprintf(sng_val_sng,"%s\",\n%*s\"",sng_val_sng_cpy,prn_ndn+prn_flg->var_fst,spc_sng);
            if(lmn%sng_lng == sng_lngm1){
              (void)fprintf(stdout,"%s%s",sng_val_sng,(CDL) ? "\"" : "");
              /* Print separator after non-final string */
              if(lmn != var_szm1) (void)fprintf(stdout,"%s",spr_sng);
            } /* endif string end */
            if(lmn == var_szm1) sng_val_sng=(char *)nco_free(sng_val_sng);
          } /* var.nbr_dim > 0 */
          break;
        case NC_BYTE: (void)sprintf(val_sng,fmt_sng,var.val.bp[lmn]); break;
        case NC_UBYTE: (void)sprintf(val_sng,fmt_sng,var.val.ubp[lmn]); break;
        case NC_USHORT: (void)sprintf(val_sng,fmt_sng,var.val.usp[lmn]); break;
        case NC_UINT: (void)sprintf(val_sng,fmt_sng,var.val.uip[lmn]); break;
        case NC_INT64: (void)sprintf(val_sng,fmt_sng,var.val.i64p[lmn]); break;
        case NC_UINT64: (void)sprintf(val_sng,fmt_sng,var.val.ui64p[lmn]); break;
        case NC_STRING: 
          sng_val=var.val.sngp[lmn];
          sng_lng=strlen(sng_val);
          sng_lngm1=sng_lng-1UL;
          /* Worst case is printable strings are six or four times longer than unformatted, i.e., '\"' == "&quot;" or '\\' == "\\\\" */
          sng_val_sng=(char *)nco_malloc(6*sng_lng+1UL);
          if(CDL) (void)fprintf(stdout,"\"");
          sng_val_sng[0]='\0';
          for(chr_idx=0;chr_idx<sng_lng;chr_idx++){
            val_sng[0]='\0';
            chr_val=sng_val[chr_idx];
	    (void)strcat(sng_val_sng,(*chr2sng_sf)(chr_val,val_sng));
          } /* end loop over character */
          (void)fprintf(stdout,"%s%s",sng_val_sng,(XML) ? "" : "\"");
          /* Print separator after non-final string */
          if(lmn != var_szm1) (void)fprintf(stdout,"%s",spr_sng);
          sng_val_sng=(char *)nco_free(sng_val_sng);
          break;
        default: nco_dfl_case_nc_type_err(); break;
        } /* end switch */
      } /* !is_mss_val */
      if(var.type != NC_CHAR && var.type != NC_STRING) (void)fprintf(stdout,"%s%s",val_sng,(lmn != var_szm1) ? spr_sng : "");
    } /* end loop over element */
    rcd_prn+=0; /* CEWI */
    if(CDL) (void)fprintf(stdout," ;\n");
    if(XML) (void)fprintf(stdout,"</values>\n");
    if(JSN) (void)fprintf(stdout,"], ");

  } /* end if CDL_OR_JSN_OR_XML */

  if(prn_flg->PRN_DMN_UNITS){
    const char units_nm[]="units"; /* [sng] Name of units attribute */
    int rcd_lcl; /* [rcd] Return code */
    int att_id; /* [id] Attribute ID */
    long att_sz;
    nc_type att_typ;

    /* Does variable have character attribute named units_nm? */
    rcd_lcl=nco_inq_attid_flg(grp_id,var.id,units_nm,&att_id);
    if(rcd_lcl == NC_NOERR){
      (void)nco_inq_att(grp_id,var.id,units_nm,&att_typ,&att_sz);
      if(att_typ == NC_CHAR){
        unit_sng=(char *)nco_malloc((att_sz+1L)*nco_typ_lng(att_typ));
        (void)nco_get_att(grp_id,var.id,units_nm,unit_sng,att_typ);
        unit_sng[(att_sz+1L)*nco_typ_lng(att_typ)-1L]='\0';
        MALLOC_UNITS_SNG=True; /* [flg] Allocated memory for units string */
      } /* end if */
    } /* end if */
  } /* end if PRN_DMN_UNITS */

  if(var.nbr_dim == 0 && !dlm_sng && TRD){
    /* Variable is scalar, byte, or character */
    lmn=0L;
    if(prn_flg->PRN_MSS_VAL_BLANK) is_mss_val = var.has_mss_val ? !memcmp(var.val.vp,var.mss_val.vp,(size_t)val_sz_byt) : False; 
    if(prn_flg->PRN_DMN_VAR_NM) (void)sprintf(var_sng,"%*s%%s = %s %%s\n",prn_ndn,spc_sng,nco_typ_fmt_sng(var.type)); else (void)sprintf(var_sng,"%*s%s\n",prn_ndn,spc_sng,nco_typ_fmt_sng(var.type));
    if(prn_flg->PRN_MSS_VAL_BLANK && is_mss_val){
      if(prn_flg->PRN_DMN_VAR_NM) (void)fprintf(stdout,"%*s%s = %s %s\n",prn_ndn,spc_sng,var_nm,mss_val_sng,unit_sng); else (void)fprintf(stdout,"%*s%s\n",prn_ndn,spc_sng,mss_val_sng); 
    }else{ /* !is_mss_val */
      if(prn_flg->PRN_DMN_VAR_NM){
        switch(var.type){
        case NC_FLOAT: (void)fprintf(stdout,var_sng,var_nm,var.val.fp[lmn],unit_sng); break;
        case NC_DOUBLE: (void)fprintf(stdout,var_sng,var_nm,var.val.dp[lmn],unit_sng); break;
        case NC_SHORT: (void)fprintf(stdout,var_sng,var_nm,var.val.sp[lmn],unit_sng); break;
        case NC_INT: (void)fprintf(stdout,var_sng,var_nm,var.val.ip[lmn],unit_sng); break;
        case NC_CHAR:
          if(var.val.cp[lmn] != '\0'){
            (void)sprintf(var_sng,"%*s%%s = '%s' %%s\n",prn_ndn,spc_sng,nco_typ_fmt_sng(var.type));
            (void)fprintf(stdout,var_sng,var_nm,var.val.cp[lmn],unit_sng);
          }else{ /* Deal with NUL character here */
            (void)fprintf(stdout,"%*s%s = \"\" %s\n",prn_ndn,spc_sng,var_nm,unit_sng);
          } /* end if */
          break;
        case NC_BYTE: (void)fprintf(stdout,var_sng,var_nm,(unsigned char)var.val.bp[lmn],unit_sng); break;
        case NC_UBYTE: (void)fprintf(stdout,var_sng,var_nm,var.val.ubp[lmn],unit_sng); break;
        case NC_USHORT: (void)fprintf(stdout,var_sng,var_nm,var.val.usp[lmn],unit_sng); break;
        case NC_UINT: (void)fprintf(stdout,var_sng,var_nm,var.val.uip[lmn],unit_sng); break;
        case NC_INT64: (void)fprintf(stdout,var_sng,var_nm,var.val.i64p[lmn],unit_sng); break;
        case NC_UINT64: (void)fprintf(stdout,var_sng,var_nm,var.val.ui64p[lmn],unit_sng); break;
        case NC_STRING: (void)fprintf(stdout,var_sng,var_nm,var.val.sngp[lmn],unit_sng); break;
        default: nco_dfl_case_nc_type_err(); break;
        } /* end switch */
      }else{ /* !PRN_DMN_VAR_NM */
        switch(var.type){
        case NC_FLOAT: (void)fprintf(stdout,var_sng,var.val.fp[lmn]); break;
        case NC_DOUBLE: (void)fprintf(stdout,var_sng,var.val.dp[lmn]); break;
        case NC_SHORT: (void)fprintf(stdout,var_sng,var.val.sp[lmn]); break;
        case NC_INT: (void)fprintf(stdout,var_sng,var.val.ip[lmn]); break;
        case NC_CHAR:
          if(var.val.cp[lmn] != '\0'){
            (void)sprintf(var_sng,"'%s'\n",nco_typ_fmt_sng(var.type));
            (void)fprintf(stdout,var_sng,var.val.cp[lmn]);
          }else{ /* Deal with NUL character here */
            (void)fprintf(stdout, "\"\"\n");
          } /* end if */
          break;
        case NC_BYTE: (void)fprintf(stdout,var_sng,(unsigned char)var.val.bp[lmn]); break;
        case NC_UBYTE: (void)fprintf(stdout,var_sng,var.val.ubp[lmn]); break;
        case NC_USHORT: (void)fprintf(stdout,var_sng,var.val.usp[lmn]); break;
        case NC_UINT: (void)fprintf(stdout,var_sng,var.val.uip[lmn]); break;
        case NC_INT64: (void)fprintf(stdout,var_sng,var.val.i64p[lmn]); break;
        case NC_UINT64: (void)fprintf(stdout,var_sng,var.val.ui64p[lmn]); break;
        case NC_STRING: (void)fprintf(stdout,var_sng,var.val.sngp[lmn]); break;
        default: nco_dfl_case_nc_type_err(); break;
        } /* end switch */
      } /* !PRN_DMN_VAR_NM */
    } /* !is_mss_val */
  } /* end if variable is scalar, byte, or character */

  if(var.nbr_dim > 0 && !dlm_sng && TRD){

    /* Create mod_map_in */
    for(int idx=0;idx<var.nbr_dim;idx++) mod_map_in[idx]=1L;
    for(int idx=0;idx<var.nbr_dim;idx++)
      for(int jdx=idx+1;jdx<var.nbr_dim;jdx++)
        mod_map_in[idx]*=lmt_msa[jdx]->dmn_sz_org;

    /* Create mod_map_cnt */
    for(int idx=0;idx<var.nbr_dim;idx++) mod_map_cnt[idx]=1L;
    for(int idx=0;idx<var.nbr_dim;idx++)
      for(int jdx=idx;jdx<var.nbr_dim;jdx++)
        mod_map_cnt[idx]*=lmt_msa[jdx]->dmn_cnt;

    /* Read coordinate dimensions if required */
    if(prn_flg->PRN_DMN_IDX_CRD_VAL){

      for(int idx=0;idx<var_trv->nbr_dmn;idx++){

        assert(!strcmp(lmt_msa[idx]->dmn_nm,var_trv->var_dmn[idx].dmn_nm));

        if(nco_dbg_lvl_get() == nco_dbg_old) (void)fprintf(stdout,"%s: INFO %s <%s>: reading dimension[%d]:%s: ",nco_prg_nm_get(),fnc_nm,var_trv->nm_fll,idx,var_trv->var_dmn[idx].dmn_nm_fll);

        dim[idx].val.vp=NULL;
        dim[idx].nm=lmt_msa[idx]->dmn_nm;

        /* Dimension does not have coordinate variable, do not read... */
        if(!var_trv->var_dmn[idx].is_crd_var){
          dim[idx].is_crd_dmn=False;
          dim[idx].cid=-1;
          continue;
        }else if(var_trv->var_dmn[idx].is_crd_var){
          /* Dimension is a coordinate */

          /* Get coordinate from table */
          crd_sct *crd=var_trv->var_dmn[idx].crd;

          /* Obtain group ID using full group name */
          (void)nco_inq_grp_full_ncid(nc_id,crd->crd_grp_nm_fll,&var_crd.nc_id);

          /* Obtain variable ID using group ID and name */
          (void)nco_inq_varid(var_crd.nc_id,crd->nm,&var_crd.id);

          /* Store "var_sct" members for MSA read */
          var_crd.type=crd->var_typ;  
          var_crd.nm=crd->nm;

          /* Read coordinate variable with limits applied */
          dim[idx].val.vp=nco_msa_rcr_clc((int)0,1,lmt,lmt_msa+idx,&var_crd);

          /* Store "dmn_sct" members */
          dim[idx].is_crd_dmn=True;
          dim[idx].type=crd->var_typ;
          dim[idx].cid=var_crd.id;

          /* Ooopssy */ 
        }else assert(0);

        /* Typecast pointer before use */  
        (void)cast_void_nctype(dim[idx].type,&dim[idx].val);
      } /* end for */
    } /* end if */

    for(lmn=0;lmn<var.sz;lmn++){

      /* memcmp() triggers pedantic warning unless pointer arithmetic is cast to type char * */
      if(prn_flg->PRN_MSS_VAL_BLANK) is_mss_val = var.has_mss_val ? !memcmp((char *)var.val.vp+lmn*val_sz_byt,var.mss_val.vp,(size_t)val_sz_byt) : False; 

      /* Calculate RAM indices from current limit */
      for(int idx=0;idx<var.nbr_dim;idx++)
        dmn_sbs_ram[idx]=(lmn%mod_map_cnt[idx])/(idx == var.nbr_dim-1 ? 1L : mod_map_cnt[idx+1]);

      /* Calculate disk indices from RAM indices */
      (void)nco_msa_ram_2_dsk(dmn_sbs_ram,lmt_msa,var.nbr_dim,dmn_sbs_dsk,(lmn == var.sz-1L));

      /* Find variable index relative to disk */
      var_dsk=0;
      for(int idx=0;idx<var.nbr_dim;idx++) var_dsk+=dmn_sbs_dsk[idx]*mod_map_in[idx];

      /* Skip rest of loop unless element is first in string */
      if(var.type == NC_CHAR && dmn_sbs_ram[var.nbr_dim-1] > 0) goto lbl_chr_prn;

      /* Print dimensions with indices along with values if they are coordinate variables */
      if(prn_flg->PRN_DMN_IDX_CRD_VAL){
        long dmn_sbs_prn;
        long crd_idx_crr;
        char dmn_sng[NCO_MAX_LEN_FMT_SNG];

        /* Loop over dimensions whose coordinates are to be printed */
        for(int idx=0;idx<var_trv->nbr_dmn;idx++){

          /* Reverse dimension ordering for Fortran convention */
          if(prn_flg->FORTRAN_IDX_CNV) dmn_idx=var.nbr_dim-1-idx; else dmn_idx=idx;

          /* Format and print dimension part of output string for non-coordinate variables */

          /* If variable is a coordinate then skip printing until later */
          if(var_trv->is_crd_var) continue;

          if(!dim[dmn_idx].is_crd_dmn){ /* If dimension is not a coordinate... */
            if(prn_flg->PRN_DMN_VAR_NM){
              if(prn_flg->FORTRAN_IDX_CNV) (void)fprintf(stdout,"%*s%s(%ld) ",(idx == 0) ? prn_ndn : 0,spc_sng,dim[dmn_idx].nm,dmn_sbs_dsk[dmn_idx]+1L); else (void)fprintf(stdout,"%*s%s[%ld] ",(idx == 0) ? prn_ndn : 0,spc_sng,dim[dmn_idx].nm,dmn_sbs_dsk[dmn_idx]);
            } /* !PRN_DMN_VAR_NM */
            continue;
          } /* end if */

          if(prn_flg->PRN_DMN_VAR_NM) (void)sprintf(dmn_sng,"%*s%%s[%%ld]=%s ",(idx == 0) ? prn_ndn : 0,spc_sng,nco_typ_fmt_sng(dim[dmn_idx].type)); else (void)sprintf(dmn_sng,"%*s%s ",(idx == 0) ? prn_ndn : 0,spc_sng,nco_typ_fmt_sng(dim[dmn_idx].type));
          dmn_sbs_prn=dmn_sbs_dsk[dmn_idx];

          if(prn_flg->FORTRAN_IDX_CNV){
            (void)sng_idx_dlm_c2f(dmn_sng);
            dmn_sbs_prn++;
          } /* end if */

          /* Account for hyperslab offset in coordinate values*/
          crd_idx_crr=dmn_sbs_ram[dmn_idx];
          if(prn_flg->PRN_DMN_VAR_NM){
            switch(dim[dmn_idx].type){
            case NC_FLOAT: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,dim[dmn_idx].val.fp[crd_idx_crr]); break;
            case NC_DOUBLE: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,dim[dmn_idx].val.dp[crd_idx_crr]); break;
            case NC_SHORT: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,dim[dmn_idx].val.sp[crd_idx_crr]); break;
            case NC_INT: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,dim[dmn_idx].val.ip[crd_idx_crr]); break;
            case NC_CHAR: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,dim[dmn_idx].val.cp[crd_idx_crr]); break;
            case NC_BYTE: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,(unsigned char)dim[dmn_idx].val.bp[crd_idx_crr]); break;
            case NC_UBYTE: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,dim[dmn_idx].val.ubp[crd_idx_crr]); break;
            case NC_USHORT: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,dim[dmn_idx].val.usp[crd_idx_crr]); break;
            case NC_UINT: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,dim[dmn_idx].val.uip[crd_idx_crr]); break;
            case NC_INT64: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,dim[dmn_idx].val.i64p[crd_idx_crr]); break;
            case NC_UINT64: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,dim[dmn_idx].val.ui64p[crd_idx_crr]); break;
            case NC_STRING: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,dim[dmn_idx].val.sngp[crd_idx_crr]); break;
            default: nco_dfl_case_nc_type_err(); break;
            } /* end switch */
          }else{ /* !PRN_DMN_VAR_NM */
            switch(dim[dmn_idx].type){
            case NC_FLOAT: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].val.fp[crd_idx_crr]); break;
            case NC_DOUBLE: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].val.dp[crd_idx_crr]); break;
            case NC_SHORT: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].val.sp[crd_idx_crr]); break;
            case NC_INT: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].val.ip[crd_idx_crr]); break;
            case NC_CHAR: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].val.cp[crd_idx_crr]); break;
            case NC_BYTE: (void)fprintf(stdout,dmn_sng,(unsigned char)dim[dmn_idx].val.bp[crd_idx_crr]); break;
            case NC_UBYTE: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].val.ubp[crd_idx_crr]); break;
            case NC_USHORT: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].val.usp[crd_idx_crr]); break;
            case NC_UINT: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].val.uip[crd_idx_crr]); break;
            case NC_INT64: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].val.i64p[crd_idx_crr]); break;
            case NC_UINT64: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].val.ui64p[crd_idx_crr]); break;
            case NC_STRING: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].val.sngp[crd_idx_crr]); break;
            default: nco_dfl_case_nc_type_err(); break;
            } /* end switch */
          } /* !PRN_DMN_VAR_NM */
        } /* end loop over dimensions */
      } /* end if PRN_DMN_IDX_CRD_VAL */

      /* Print all characters in last dimension each time penultimate dimension subscript changes to its start value */
lbl_chr_prn:

      if(var.type == NC_CHAR){
        static nco_bool NUL_CHR_IN_SLB;
        static char *prn_sng;
        static int chr_cnt;
        static long dmn_sz;
        static long var_dsk_srt;
        static long var_dsk_end;

        /* At beginning of character array */
        if(dmn_sbs_ram[var.nbr_dim-1] == 0L) {
          dmn_sz=lmt_msa[var.nbr_dim-1]->dmn_cnt;
          prn_sng=(char *)nco_malloc((size_t)dmn_sz+1UL);
          var_dsk_srt=var_dsk;
          var_dsk_end=var_dsk;
          chr_cnt=0;
          NUL_CHR_IN_SLB=False;
        } /* end if */

        /* In middle of array---save characters to prn_sng */
        prn_sng[chr_cnt++]=var.val.cp[lmn];
        if(var.val.cp[lmn] == '\0' && !NUL_CHR_IN_SLB){
          var_dsk_end=var_dsk;
          NUL_CHR_IN_SLB=True;
        } /* end if */

        /* At end of character array */
        if(dmn_sbs_ram[var.nbr_dim-1] == dmn_sz-1L){
          if(NUL_CHR_IN_SLB){
            (void)sprintf(var_sng,"%%s[%%ld--%%ld]=\"%%s\" %%s");
          }else{
            (void)sprintf(var_sng,"%%s[%%ld--%%ld]='%%s' %%s");
            prn_sng[chr_cnt]='\0';
            var_dsk_end=var_dsk;   
          } /* end if */
          if(prn_flg->FORTRAN_IDX_CNV){ 
            (void)sng_idx_dlm_c2f(var_sng);
            var_dsk_srt++; 
            var_dsk_end++; 
          } /* end if */
          (void)fprintf(stdout,var_sng,var_nm,var_dsk_srt,var_dsk_end,prn_sng,unit_sng);
          (void)fprintf(stdout,"\n");
          (void)fflush(stdout);
          (void)nco_free(prn_sng);
        } /* endif */
        continue;
      } /* end if NC_CHAR */

      /* Print variable name, index, and value */
      if(prn_flg->PRN_DMN_VAR_NM) (void)sprintf(var_sng,"%*s%%s[%%ld]=%s %%s\n",(var_trv->is_crd_var) ? prn_ndn : 0,spc_sng,nco_typ_fmt_sng(var.type)); else (void)sprintf(var_sng,"%*s%s\n",(var_trv->is_crd_var) ? prn_ndn : 0,spc_sng,nco_typ_fmt_sng(var.type));
      if(prn_flg->FORTRAN_IDX_CNV){
        (void)sng_idx_dlm_c2f(var_sng);
        var_dsk++;
      } /* end if FORTRAN_IDX_CNV */

      if(prn_flg->PRN_MSS_VAL_BLANK && is_mss_val){
        if(prn_flg->PRN_DMN_VAR_NM) (void)fprintf(stdout,"%*s%s[%ld]=%s %s\n",(var_trv->is_crd_var) ? prn_ndn : 0,spc_sng,var_nm,var_dsk,mss_val_sng,unit_sng); else (void)fprintf(stdout,"%*s%s\n",(var_trv->is_crd_var) ? prn_ndn : 0,spc_sng,mss_val_sng); 
      }else{ /* !is_mss_val */
        if(prn_flg->PRN_DMN_VAR_NM){
          switch(var.type){
          case NC_FLOAT: (void)fprintf(stdout,var_sng,var_nm,var_dsk,var.val.fp[lmn],unit_sng); break;
          case NC_DOUBLE: (void)fprintf(stdout,var_sng,var_nm,var_dsk,var.val.dp[lmn],unit_sng); break;
          case NC_SHORT: (void)fprintf(stdout,var_sng,var_nm,var_dsk,var.val.sp[lmn],unit_sng); break;
          case NC_INT: (void)fprintf(stdout,var_sng,var_nm,var_dsk,var.val.ip[lmn],unit_sng); break;
          case NC_CHAR: (void)fprintf(stdout,var_sng,var_nm,var_dsk,var.val.cp[lmn],unit_sng); break;
          case NC_BYTE: (void)fprintf(stdout,var_sng,var_nm,var_dsk,(unsigned char)var.val.bp[lmn],unit_sng); break;
          case NC_UBYTE: (void)fprintf(stdout,var_sng,var_nm,var_dsk,var.val.ubp[lmn],unit_sng); break;
          case NC_USHORT: (void)fprintf(stdout,var_sng,var_nm,var_dsk,var.val.usp[lmn],unit_sng); break;
          case NC_UINT: (void)fprintf(stdout,var_sng,var_nm,var_dsk,var.val.uip[lmn],unit_sng); break;
          case NC_INT64: (void)fprintf(stdout,var_sng,var_nm,var_dsk,var.val.i64p[lmn],unit_sng); break;
          case NC_UINT64: (void)fprintf(stdout,var_sng,var_nm,var_dsk,var.val.ui64p[lmn],unit_sng); break;
          case NC_STRING: (void)fprintf(stdout,var_sng,var_nm,var_dsk,var.val.sngp[lmn],unit_sng); break;
          default: nco_dfl_case_nc_type_err(); break;
          } /* end switch */
        }else{ /* !PRN_DMN_VAR_NM */
          switch(var.type){
          case NC_FLOAT: (void)fprintf(stdout,var_sng,var.val.fp[lmn],unit_sng); break;
          case NC_DOUBLE: (void)fprintf(stdout,var_sng,var.val.dp[lmn],unit_sng); break;
          case NC_SHORT: (void)fprintf(stdout,var_sng,var.val.sp[lmn],unit_sng); break;
          case NC_INT: (void)fprintf(stdout,var_sng,var.val.ip[lmn],unit_sng); break;
          case NC_CHAR: (void)fprintf(stdout,var_sng,var.val.cp[lmn],unit_sng); break;
          case NC_BYTE: (void)fprintf(stdout,var_sng,(unsigned char)var.val.bp[lmn],unit_sng); break;
          case NC_UBYTE: (void)fprintf(stdout,var_sng,var.val.ubp[lmn],unit_sng); break;
          case NC_USHORT: (void)fprintf(stdout,var_sng,var.val.usp[lmn],unit_sng); break;
          case NC_UINT: (void)fprintf(stdout,var_sng,var.val.uip[lmn],unit_sng); break;
          case NC_INT64: (void)fprintf(stdout,var_sng,var.val.i64p[lmn],unit_sng); break;
          case NC_UINT64: (void)fprintf(stdout,var_sng,var.val.ui64p[lmn],unit_sng); break;
          case NC_STRING: (void)fprintf(stdout,var_sng,var.val.sngp[lmn],unit_sng); break;
          default: nco_dfl_case_nc_type_err(); break;
          } /* end switch */
        } /* !PRN_DMN_VAR_NM */
      } /* !is_mss_val */
    } /* end loop over elements */

    (void)fflush(stdout);

    for(int idx=0;idx<var.nbr_dim;idx++) if(dim[idx].val.vp) dim[idx].val.vp=nco_free(dim[idx].val.vp);
  } /* end if variable has more than one dimension */

  /* Free value buffer */
  var.val.vp=nco_free(var.val.vp);
  var.mss_val.vp=nco_free(var.mss_val.vp);
  var.nm=(char *)nco_free(var.nm);

  if(MALLOC_UNITS_SNG) unit_sng=(char *)nco_free(unit_sng);

  if(dlm_sng) dlm_sng=(char *)nco_free(dlm_sng);

  if(prn_flg->nwl_pst_val) (void)fprintf(stdout,"\n");

  /* Free (allocated for non scalars only) */
  if(var.nbr_dim > 0){
    (void)nco_lmt_msa_free(var_trv->nbr_dmn,lmt_msa);
    lmt=(lmt_sct **)nco_free(lmt);
  } /* endif */

} /* end nco_prn_var_val_trv() */

int /* [rcd] Return code */
nco_grp_prn /* [fnc] Recursively print group contents */
(const int nc_id, /* I [id] netCDF file ID */
 const char * const grp_nm_fll, /* I [sng] Absolute group name (path) */
 prn_fmt_sct * const prn_flg, /* I/O [sct] Print-format information */
 const trv_tbl_sct * const trv_tbl) /* I [sct] Traversal table */
{
  /* Purpose: Recursively print group contents
     Assumptions: 
     1. Input group name is a valid group to be extracted (set in nco_xtr_dfn()). 
        Hence no need to check for group type, or if group is extracted.
     2. Input ID is netCDF file ID, not extracted group ID */

  /* Testing: 
     ncks -5 ~/nco/data/in_grp.nc
     ncks --cdl ~/nco/data/in_grp.nc */

  const char sls_sng[]="/";        /* [sng] Slash string */
  const char spc_sng[]="";        /* [sng] Space string */

  char grp_nm[NC_MAX_NAME+1L];      /* [sng] Group name */
  char var_nm[NC_MAX_NAME+1L];      /* [sng] Variable name */ 

  char *nm_cdl;
  char *var_nm_fll;                /* [sng] Full path for variable */

  int *grp_ids;                    /* [ID] Sub-group IDs array */  

  int dmn_idx_grp[NC_MAX_DIMS];    /* [ID] Dimension indices array for group */ 
  int grp_idx;                     /* [idx] Group index */  
  int grp_id;                      /* [id] netCDF group ID */
  int grp_dpt;                     /* [nbr] Depth of group (root = 0) */
  int nbr_att;                     /* [nbr] Number of attributes */
  int nbr_grp;                     /* [nbr] Number of sub-groups in this group */
  int nbr_var;                     /* [nbr] Number of variables */
  int prn_ndn=0;                   /* [nbr] Indentation for printing */
  int rcd=NC_NOERR;                /* [rcd] Return code */
  int var_id;                      /* [id] Variable ID */
  int var_idx;                     /* [idx] Variable index */
  int var_nbr_xtr;                 /* [nbr] Number of extracted variables */

  const nco_bool CDL=prn_flg->cdl; /* [flg] CDL output */
  const nco_bool XML=prn_flg->xml; /* [flg] XML output */
  const nco_bool TRD=prn_flg->trd; /* [flg] Traditional output */
  const nco_bool SRM=prn_flg->srm; /* [flg] Stream output */
  const nco_bool JSN=prn_flg->jsn; /* [flg] JSON output */
  const nco_bool CDL_OR_XML=prn_flg->cdl || prn_flg->xml; /* [flg] CDL or XML output */
  const nco_bool CDL_OR_TRD=prn_flg->cdl || prn_flg->trd; /* [flg] CDL or Traditional output */
  const nco_bool CDL_OR_JSN_OR_TRD=prn_flg->cdl || prn_flg->jsn || prn_flg->trd; /* [flg] CDL or JSON or Traditional output */
  const nco_bool CDL_OR_TRD_OR_XML=prn_flg->cdl || prn_flg->trd || prn_flg->xml; /* [flg] CDL or Traditional or XML output */

  nm_id_sct *dmn_lst; /* [sct] Dimension list */
  nm_id_sct *var_lst; /* [sct] Variable list */

  unsigned int dmn_idx; /* [idx] Index over dimensions */
  unsigned int dmn_nbr; /* [nbr] Number of dimensions defined in group */
  unsigned int obj_idx; /* [idx] Index over traversal table */
  unsigned int obj_idx_crr; /* [idx] Object index for this group */

  /* Initialize */
  dmn_nbr=0; /* [nbr] Number of dimensions defined in group */
  var_nbr_xtr=0; /* [nbr] Number of variables to be extracted in group */

  /* Find group in traversal table */
  for(obj_idx=0;obj_idx<trv_tbl->nbr;obj_idx++)
    if(trv_tbl->lst[obj_idx].nco_typ == nco_obj_typ_grp)
      if(!strcmp(trv_tbl->lst[obj_idx].grp_nm_fll,grp_nm_fll))
	break;
    
  /* Obtain group ID */
  (void)nco_inq_grp_full_ncid(nc_id,grp_nm_fll,&grp_id);

  /* Obtain group information */
  obj_idx_crr=obj_idx;
  grp_dpt=trv_tbl->lst[obj_idx].grp_dpt;
  nbr_att=trv_tbl->lst[obj_idx].nbr_att;
  nbr_var=trv_tbl->lst[obj_idx].nbr_var;
  nbr_grp=trv_tbl->lst[obj_idx].nbr_grp;

  /* Find dimension information for group */
  for(dmn_idx=0;dmn_idx<trv_tbl->nbr_dmn;dmn_idx++){
    /* Will dimension be extracted? */
    if(trv_tbl->lst_dmn[dmn_idx].flg_xtr){
      /* And was dimension defined in this group? */
      if(!strcmp(grp_nm_fll,trv_tbl->lst_dmn[dmn_idx].grp_nm_fll)){
	/* Add dimension to list of dimensions defined in group */
	dmn_idx_grp[dmn_nbr]=dmn_idx;
	dmn_nbr++;
      } /* end if */
    } /* end if flg_xtr */
  } /* end loop over dmn_idx */

  /* Create arrays of these dimensions */
  dmn_lst=(nm_id_sct *)nco_malloc(dmn_nbr*(sizeof(nm_id_sct)));
  for(dmn_idx=0;dmn_idx<dmn_nbr;dmn_idx++){
    /* NB: ID here is actually index into trv_tbl->lst_dmn. It is NOT an ID. 
       However, it is same type (int) as an ID so we can use nm_id infrastructure. */
    dmn_lst[dmn_idx].id=dmn_idx_grp[dmn_idx];
    dmn_lst[dmn_idx].nm=strdup(trv_tbl->lst_dmn[dmn_idx_grp[dmn_idx]].nm);
  } /* end loop over dmn_idx */

  /* Sort dimensions alphabetically */
  if(dmn_nbr > 1) dmn_lst=nco_lst_srt_nm_id(dmn_lst,dmn_nbr,prn_flg->ALPHA_BY_STUB_GROUP);

  if(XML){
    if(grp_dpt == 0){
      if(prn_flg->xml_lcn) (void)fprintf(stdout,"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<netcdf xmlns=\"http://www.unidata.ucar.edu/namespaces/netcdf/ncml-2.2\" location=\"%s\">\n",prn_flg->fl_in); else (void)fprintf(stdout,"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<netcdf xmlns=\"http://www.unidata.ucar.edu/namespaces/netcdf/ncml-2.2\">\n");
      if(prn_flg->nfo_xtr) (void)fprintf(stdout,"%*s<!-- %s -->\n",prn_flg->sxn_fst,spc_sng,prn_flg->smr_sng);
    }else{ /* grp_dpt != 0 */
      (void)fprintf(stdout,"%*s<group name=\"%s\">\n",grp_dpt*prn_flg->spc_per_lvl,spc_sng,trv_tbl->lst[obj_idx].nm);
    } /* grp_dpt != 0 */
  }else if(SRM){ /* !xml */
    /* https://www.unidata.ucar.edu/software/thredds/current/netcdf-java/stream/NcStream.html */
  }else if(CDL_OR_TRD){ /* !xml */
    nm_cdl=nm2sng_cdl(nco_gpe_evl_stb(prn_flg->gpe,trv_tbl->lst[obj_idx].nm_fll));
    if(grp_dpt == 0 && CDL) (void)fprintf(stdout,"netcdf %s {",prn_flg->fl_stb); else (void)fprintf(stdout,"%*sgroup: %s {",grp_dpt*prn_flg->spc_per_lvl,spc_sng,nm_cdl);
    nm_cdl=(char *)nco_free(nm_cdl);
    if(prn_flg->fll_pth) (void)fprintf(stdout," // fullname: %s\n",nco_gpe_evl(prn_flg->gpe,grp_nm_fll)); else (void)fprintf(stdout,"\n");
    if(grp_dpt == 0 && prn_flg->nfo_xtr && prn_flg->PRN_GLB_METADATA) (void)fprintf(stdout,"%*s// %s\n",prn_flg->sxn_fst,spc_sng,prn_flg->smr_sng);
    if(grp_dpt == 0 && prn_flg->nfo_xtr) (void)fprintf(stdout,"%*s// ncgen -k netCDF-4 -b -o %s.nc %s.cdl\n",prn_flg->sxn_fst,spc_sng,prn_flg->fl_stb,prn_flg->fl_stb);
  } /* !XML */

  /* Print dimension information for group */
  prn_ndn=prn_flg->ndn=prn_flg->sxn_fst+grp_dpt*prn_flg->spc_per_lvl;
  if(dmn_nbr > 0 && CDL_OR_TRD) (void)fprintf(stdout,"%*sdimensions:\n",prn_flg->ndn,spc_sng);
  if(CDL) prn_ndn+=prn_flg->var_fst;
  for(dmn_idx=0;dmn_idx<dmn_nbr;dmn_idx++){
    if(XML){
      (void)fprintf(stdout,"%*s<dimension name=\"%s\" length=\"%lu\" %s/>\n",prn_ndn,spc_sng,dmn_lst[dmn_idx].nm,(unsigned long)trv_tbl->lst_dmn[dmn_lst[dmn_idx].id].lmt_msa.dmn_cnt,trv_tbl->lst_dmn[dmn_lst[dmn_idx].id].is_rec_dmn ? "isUnlimited=\"true\" " : "");
    }else if(CDL_OR_TRD){ /* !XML */
      nm_cdl=nm2sng_cdl(dmn_lst[dmn_idx].nm);
      if(trv_tbl->lst_dmn[dmn_lst[dmn_idx].id].is_rec_dmn) (void)fprintf(stdout,"%*s%s = UNLIMITED%s// (%lu currently)\n",prn_ndn,spc_sng,nm_cdl,(CDL) ? " ; " : " ",(unsigned long)trv_tbl->lst_dmn[dmn_lst[dmn_idx].id].lmt_msa.dmn_cnt); else (void)fprintf(stdout,"%*s%s = %lu%s\n",prn_ndn,spc_sng,nm_cdl,(unsigned long)trv_tbl->lst_dmn[dmn_lst[dmn_idx].id].lmt_msa.dmn_cnt,(CDL) ? " ;" : "");
      nm_cdl=(char *)nco_free(nm_cdl);
    } /* !XML */
  } /* end loop over dimension */

  /* Dimension list no longer needed */
  dmn_lst=nco_nm_id_lst_free(dmn_lst,dmn_nbr);

  /* Variables */

  /* Create array to hold names and indices of extracted variables in this group */
  var_lst=(nm_id_sct *)nco_malloc(nbr_var*(sizeof(nm_id_sct)));

  for(var_idx=0;var_idx<nbr_var;var_idx++){
    /* Get variable name */
    rcd+=nco_inq_varname(grp_id,var_idx,var_nm);

    /* Allocate path buffer and include space for trailing NUL */ 
    var_nm_fll=(char *)nco_malloc(strlen(grp_nm_fll)+strlen(var_nm)+2L);

    /* Initialize path with current absolute group path */
    strcpy(var_nm_fll,grp_nm_fll);

    /* If not root group, concatenate separator */
    if(strcmp(grp_nm_fll,sls_sng)) strcat(var_nm_fll,sls_sng);

    /* Concatenate variable to absolute group path */
    strcat(var_nm_fll,var_nm);

    /* Find variable in traversal table */
    for(obj_idx=0;obj_idx<trv_tbl->nbr;obj_idx++)
      if(trv_tbl->lst[obj_idx].nco_typ == nco_obj_typ_var)
	if(!strcmp(trv_tbl->lst[obj_idx].nm_fll,var_nm_fll))
	  break;
    
    /* Is variable to be extracted? */
    if(trv_tbl->lst[obj_idx].flg_xtr){
      /* NB: ID here is actually index into trv_tbl->lst. It is NOT an ID. 
	 However, it is same type (int) as an ID so we re-use nm_id infrastructure */
      var_lst[var_nbr_xtr].id=obj_idx;
      var_lst[var_nbr_xtr].nm=strdup(var_nm);
      var_nbr_xtr++;
    } /* endif extracted */

    /* Free constructed name */
    var_nm_fll=(char *)nco_free(var_nm_fll);

  } /* end loop over variables */

  /* Compactify array to hold names and indices of extracted variables in this group */
  var_lst=(nm_id_sct *)nco_realloc(var_lst,var_nbr_xtr*(sizeof(nm_id_sct)));

  /* Sort variables alphabetically */
  if(var_nbr_xtr > 1) var_lst=nco_lst_srt_nm_id(var_lst,var_nbr_xtr,prn_flg->ALPHA_BY_STUB_GROUP);

  if(JSN) (void)fprintf(stdout,"{\n");

  /* Print variable information for group */
  if(var_nbr_xtr > 0 && CDL_OR_TRD) (void)fprintf(stdout,"\n%*svariables:\n",prn_flg->ndn,spc_sng);
  for(var_idx=0;var_idx<var_nbr_xtr;var_idx++){
    trv_sct var_trv=trv_tbl->lst[var_lst[var_idx].id];

    /* Print variable full name */
    if(var_trv.grp_dpt > 0 && prn_flg->fll_pth && TRD) (void)fprintf(stdout,"%*s%s\n",prn_flg->ndn,spc_sng,var_trv.nm_fll);

    /* Print variable metadata */ 
    if(prn_flg->PRN_VAR_METADATA || CDL_OR_XML) (void)nco_prn_var_dfn(nc_id,prn_flg,&var_trv);

    /* Obtain variable ID using group ID */
    (void)nco_inq_varid(grp_id,var_trv.nm,&var_id);

    /* Print variable attributes */
    if(JSN && prn_flg->PRN_VAR_METADATA) (void)fprintf(stdout,"\"attributes\": [");
    if(CDL_OR_TRD_OR_XML || (JSN && prn_flg->PRN_VAR_METADATA)) (void)nco_prn_att(grp_id,prn_flg,var_id);
    if(JSN && prn_flg->PRN_VAR_METADATA) (void)fprintf(stdout,"], \n");

    if(XML && prn_flg->PRN_VAR_DATA) (void)nco_prn_var_val_trv(nc_id,prn_flg,&trv_tbl->lst[var_lst[var_idx].id],trv_tbl);
    if(XML && (prn_flg->PRN_VAR_DATA || prn_flg->PRN_VAR_METADATA)) (void)fprintf(stdout,"%*s</variable>\n",prn_ndn,spc_sng);

    if(var_idx != var_nbr_xtr-1 && CDL_OR_TRD) (void)fprintf(stdout,"\n");
  } /* end loop over var_idx */

  /* Print attribute information for group */
  if((nbr_att > 0 || (prn_flg->hdn && grp_dpt == 0)) && prn_flg->PRN_GLB_METADATA && CDL_OR_TRD) (void)fprintf(stdout,"\n%*s%s%sattributes:\n",prn_flg->ndn,spc_sng,(CDL) ? "// " : "",(grp_dpt == 0) ? "global " : "group ");
  if((nbr_att > 0 || (prn_flg->hdn && grp_dpt == 0)) && prn_flg->PRN_GLB_METADATA) nco_prn_att(grp_id,prn_flg,NC_GLOBAL);

  /* Print data for group */
  if(CDL_OR_JSN_OR_TRD && var_nbr_xtr > 0 && prn_flg->PRN_VAR_DATA){
    if(CDL_OR_TRD) (void)fprintf(stdout,"\n%*sdata:\n",prn_flg->ndn,spc_sng);
    for(var_idx=0;var_idx<var_nbr_xtr;var_idx++) (void)nco_prn_var_val_trv(nc_id,prn_flg,&trv_tbl->lst[var_lst[var_idx].id],trv_tbl);
  } /* end if */

  /* Variable list no longer needed */
  var_lst=nco_nm_id_lst_free(var_lst,var_nbr_xtr);

  /* Get ready for sub-groups */ 
  grp_ids=(int *)nco_malloc(nbr_grp*sizeof(int)); 
  rcd+=nco_inq_grps(grp_id,(int *)NULL,grp_ids);

  if(JSN && prn_flg->PRN_GLB_METADATA) (void)fprintf(stdout,"\"nodes\": [");

  /* Call recursively for all extracted subgroups */
  for(grp_idx=0;grp_idx<nbr_grp;grp_idx++){
    char *sub_grp_nm_fll=NULL; /* [sng] Sub group path */
    int gid=grp_ids[grp_idx]; /* [id] Current group ID */  

    /* Get sub-group name */
    rcd+=nco_inq_grpname(gid,grp_nm);

    if(JSN) (void)fprintf(stdout,"{\"name\": \"%s\"}%s",grp_nm,(grp_idx == nbr_grp-1) ? "" : ", ");

    /* Allocate path buffer including space for trailing NUL */ 
    sub_grp_nm_fll=(char *)nco_malloc(strlen(grp_nm_fll)+strlen(grp_nm)+2L);

    /* Initialize path with current absolute group path */
    strcpy(sub_grp_nm_fll,grp_nm_fll);

    /* If not root group, concatenate separator */
    if(strcmp(grp_nm_fll,sls_sng)) strcat(sub_grp_nm_fll,sls_sng);

    /* Concatenate current group to absolute group path */
    strcat(sub_grp_nm_fll,grp_nm); 

    /* Find sub-group in traversal table */
    for(obj_idx=0;obj_idx<trv_tbl->nbr;obj_idx++)
      if(trv_tbl->lst[obj_idx].nco_typ == nco_obj_typ_grp)
	if(!strcmp(trv_tbl->lst[obj_idx].grp_nm_fll,sub_grp_nm_fll))
	  break;
    
    /* Is sub-group to be extracted? If so, recurse */
    if(!JSN && trv_tbl->lst[obj_idx].flg_xtr) rcd+=nco_grp_prn(nc_id,sub_grp_nm_fll,prn_flg,trv_tbl);

    /* Free constructed name */
    sub_grp_nm_fll=(char *)nco_free(sub_grp_nm_fll);
  } /* end loop over grp_idx */

  if(JSN && prn_flg->PRN_GLB_METADATA) (void)fprintf(stdout,"], \n"); /* End node-list */
  if(JSN) (void)fprintf(stdout,"\"w10n\": [{\"name\": \"spec\", \"value\": \"draft-20091228\"}, {\"name\": \"application\", \"value\": \"%s\"}, {\"name\": \"type\", \"value\": \"%s\"}, {\"name\": \"path\", \"value\": \"%s\"}, {\"name\": \"identifier\", \"value\": \"/\"}]}, \n",nco_prg_nm_get(),jsn_fmt_xtn_nm(nco_fmt_xtn_get()),prn_flg->fl_in);
  if(JSN && prn_flg->PRN_GLB_METADATA) (void)fprintf(stdout,"\"name\": \"%s\"\n",(grp_dpt == 0) ? "" : trv_tbl->lst[obj_idx_crr].nm);

  /* Mark end of output */
  if(CDL_OR_TRD) (void)fprintf(stdout,"%*s} // group %s\n",grp_dpt*prn_flg->spc_per_lvl,spc_sng,(grp_dpt == 0) ? grp_nm_fll : nm2sng_cdl(nco_gpe_evl(prn_flg->gpe,grp_nm_fll)));
  if(JSN) (void)fprintf(stdout,"}\n");
  if(XML && grp_dpt == 0) (void)fprintf(stdout,"</netcdf>\n"); 
  if(XML && grp_dpt != 0) (void)fprintf(stdout,"%*s</group>\n",grp_dpt*prn_flg->spc_per_lvl,spc_sng); 

  return rcd;
} /* end nco_grp_prn() */

nco_bool                            /* O [flg] Variable is compound */
nco_prn_cpd_chk                     /* [fnc] Check whether variable is compound */
(const trv_sct * const var_trv,     /* I [sct] Variable to check */
 const trv_tbl_sct * const trv_tbl) /* I [sct] GTT (Group Traversal Table) */ 
{
  /* Purpose: Check whether variable dimensionality is compound, i.e.,
     whether variable needs extra printed braces in CDL output.
     For purposes of this routine, a variable is compound iff it contains
     a record dimension as any but the leading dimension. */
  
  int dmn_idx;
  dmn_trv_sct *dmn_trv; /* [sct] Unique dimension object */
  
  if(var_trv->nbr_dmn <= 1) return False;
  
  for(dmn_idx=1;dmn_idx<var_trv->nbr_dmn;dmn_idx++){ /* NB: dimension index starts at 1 */
    dmn_trv=nco_dmn_trv_sct(var_trv->var_dmn[dmn_idx].dmn_id,trv_tbl); 
    if(dmn_trv->is_rec_dmn) break; /* fxm: change to var_dmn->is_rec_var */
  } /* end loop over dimensions */
  
  if(dmn_idx != var_trv->nbr_dmn) return True; else return False;
} /* end nco_prn_cpd_chk() */

nco_bool /* O [flg] Type requires hidden _Unsigned attribute string */
nco_xml_typ_rqr_nsg_att /* [fnc] Does type require hidden _Unsigned attribute for XML representation? */
(const nc_type nco_typ) /* I [enm] netCDF type */
{
  /* Purpose: Return boolean if netCDF type requires hidden _Unsigned attribute for XML representation
     Output of toolsui shows these attributes */
  switch(nco_typ){
  case NC_FLOAT:
  case NC_DOUBLE:
  case NC_INT:
  case NC_SHORT:
  case NC_CHAR:
  case NC_BYTE:
  case NC_INT64:
  case NC_STRING:
    return False;
  case NC_UBYTE:
  case NC_USHORT:
  case NC_UINT:
  case NC_UINT64:
    return True;
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */

  /* Some compilers, e.g., SGI cc, need return statement to end non-void functions */
  return False;
} /* end nco_xml_typ_rqr_nsg_att() */

nco_bool /* O [flg] Type requires hidden _FillValue attribute string */
nco_xml_typ_rqr_flv_att /* [fnc] Does type require hidden _FillValue attribute for XML representation? */
(const nc_type nco_typ) /* I [enm] netCDF type */
{
  /* Purpose: Return boolean if netCDF type requires hidden _FillValue attribute for XML representation
     Output of toolsui shows these attributes */
  switch(nco_typ){
  case NC_FLOAT:
  case NC_DOUBLE:
  case NC_INT:
  case NC_SHORT:
  case NC_CHAR:
  case NC_BYTE:
  case NC_INT64:
  case NC_STRING:
    return False;
  case NC_UBYTE:
  case NC_USHORT:
  case NC_UINT:
  case NC_UINT64:
    return True;
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */

  /* Some compilers, e.g., SGI cc, need return statement to end non-void functions */
  return False;
} /* end nco_xml_typ_rqr_flv_att() */

void
nco_dfl_case_fmt_xtn_err(void) /* [fnc] Print error and exit for illegal switch(nco_fmt_xtn) case */
{
  /* Purpose: Convenience routine for printing error and exiting when
     switch(nco_fmt_xtn) statement receives an illegal default case

     Placing this in its own routine also has the virtue of saving many lines
     of code since this function is used in many many switch() statements. */
  const char fnc_nm[]="nco_dfl_case_fmt_xtn_err()";
  (void)fprintf(stdout,"%s: ERROR switch(nco_fmt_xtn) statement fell through to default case, which is unsafe. This catch-all error handler ensures all switch(nco_fmt_xtn) statements are fully enumerated. Exiting...\n",fnc_nm);
  nco_err_exit(0,fnc_nm);
} /* end nco_dfl_case_fmt_xtn_err() */

const char * /* O [sng] JSON file type */
jsn_fmt_xtn_nm /* [fnc] Return string describing JSON filetype */
(const int fl_fmt_xtn) /* I [enm] Extended filetype */
{
  /* Purpose: Divine JSON filetype string from netCDF exteded filetype enum */
  switch(fl_fmt_xtn){
  case nco_fmt_xtn_nil: /* 0 NC_FORMAT_UNDEFINED Undefined (more precisely, not yet defined) */
    return "nil";
  case nco_fmt_xtn_nc3: /* 1 NC_FORMAT_NC3 netCDF3 */
    return "nc3";
  case nco_fmt_xtn_hdf5: /* 2 NC_FORMAT_NC_HDF5 HDF5 */
    return "hdf5";
  case nco_fmt_xtn_hdf4: /* 3 NC_FORMAT_NC_HDF4 HDF4 */
    return "hdf4";
  case nco_fmt_xtn_pnetcdf: /* 4 NC_FORMAT_PNETCDF PnetCDF */
    return "pnetcdf";
  case nco_fmt_xtn_dap2: /* 5 NC_FORMAT_DAP2 DAP2 */
    return "dap2";
  case nco_fmt_xtn_dap4: /* 6 NC_FORMAT_DAP4 DAP4 */
    return "dap4";
  default: nco_dfl_case_fmt_xtn_err(); break;
  } /* end switch */

  /* Some compilers, e.g., SGI cc, need return statement to end non-void functions */
  return (char *)NULL;
} /* end jsn_fmt_xtn_nm() */
