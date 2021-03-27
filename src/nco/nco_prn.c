/* $Header$ */

/* Purpose: Print variables, attributes, metadata */

/* Copyright (C) 1995--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

#include "nco_prn.h" /* Print variables, attributes, metadata */

int 
nco_att_nbr        /* [fnc] Return number of attributes in variable or group attributes in group */ 
(const int grp_id, /* I [id] netCDF group ID */
 const int var_id) /* I [id] netCDF input variable ID */
{
  int att_nbr;

  if(var_id == NC_GLOBAL){
    /* Get number of global attributes in group */
    (void)nco_inq(grp_id,(int *)NULL,(int *)NULL,&att_nbr,(int *)NULL);
  }else{
    /* Get name and number of attributes for variable */
    (void)nco_inq_var(grp_id,var_id,(char*)NULL,(int*)NULL,(int*)NULL,(int *)NULL,&att_nbr);
  } /* end else */

  return att_nbr; 
} /* !nco_att_nbr() */

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

  const char fnc_nm[]="nco_prn_att()"; /* [sng] Function name */
  const char spc_sng[]=""; /* [sng] Space string */

  char *nm_cdl;
  char *nm_jsn;
  char *sng_val_sng=NULL_CEWI; /* [sng] String of NC_CHAR */
  char *sng_val_sng_cpy; /* [sng] Copy of sng_val_sng to avoid cppcheck error about using sng_val_sng as both parameter and destination in sprintf(). NB: free() only one of these two pointers. */
  char *spr_sng=NULL; /* [sng] Output separator string */

  char cma_sng[]=", "; /* [sng] Comma string */
  char spr_xml_chr[]="*"; /* [sng] Default XML separator for character types */
  char spr_xml_chr_bck[]="*|*"; /* [sng] Backup default XML separator for character types */
  char spr_xml_nmr[]=" "; /* [sng] Default XML separator for numeric types */

  char chr_val; /* [sng] Current character */

  char att_nm[NC_MAX_NAME];
  char att_sng_dlm[NCO_MAX_LEN_FMT_SNG];
  char att_sng_pln[NCO_MAX_LEN_FMT_SNG];
  char src_sng[NC_MAX_NAME];
  char val_sng[NCO_ATM_SNG_LNG];
  char var_nm[NC_MAX_NAME+1L];

  double val_dbl;

  float val_flt;

  FILE *fp_out=prn_flg->fp_out; /* [fl] Formatted text output file handle */

  int att_nbr_ttl;
  int att_nbr_vsb;
  int dmn_nbr=0;
  int fl_fmt; /* I [enm] File format */
  int fl_fmt_xtn; /* I [enm] Extended file format */
  int grp_id_prn;
  int idx;
  int prn_ndn=0; /* [nbr] Indentation for printing */
  int rcd=NC_NOERR; /* [rcd] Return code */
  int rcd_prn=0; /* [rcd] Return code from snprintf() function */

  long att_sz;
  long att_szm1;
  long chr_idx;
  long lmn;
  long sng_lng=long_CEWI; /* [nbr] Length of NC_CHAR string */
  long sng_lngm1=long_CEWI; /* [nbr] Length minus one of NC_CHAR string */
  
  nc_type att_typ;
  nc_type var_typ;

  nco_bool flg_glb=False; /* [flg] Printing attributes for root-level group */
  
  bool jsn_obj=False; /* if true then print att in own object */
  const nco_bool CDL=prn_flg->cdl; /* [flg] CDL output */
  const nco_bool XML=prn_flg->xml; /* [flg] XML output */
  const nco_bool TRD=prn_flg->trd; /* [flg] Traditional output */
  const nco_bool JSN=prn_flg->jsn; /* [flg] JSON output */
  const nco_bool CDL_OR_JSN=prn_flg->cdl || prn_flg->jsn; /* [flg] CDL or JSON output */
  const nco_bool CDL_OR_TRD=prn_flg->cdl || prn_flg->trd; /* [flg] CDL or Traditional output */
  const nco_bool CDL_OR_JSN_OR_XML=prn_flg->cdl || prn_flg->jsn || prn_flg->xml; /* [flg] CDL or JSON or XML output */

  nco_string sng_val; /* [sng] Current string */

  prn_ndn=prn_flg->ndn;
  if(var_id == NC_GLOBAL){
    /* Get number of global attributes in group */
    (void)nco_inq(grp_id,(int *)NULL,(int *)NULL,&att_nbr_vsb,(int *)NULL);
    /* Which group is this? */
    rcd=nco_inq_grp_parent_flg(grp_id,&grp_id_prn);
    if(rcd == NC_ENOGRP) flg_glb=True;
    if(flg_glb) (void)strcpy(src_sng,(CDL) ? "" : "Global"); else (void)strcpy(src_sng,(CDL) ? "" : "Group");
    if(CDL) prn_ndn+=prn_flg->sxn_fst;
  }else{
    /* Get name and number of attributes for variable */
    (void)nco_inq_var(grp_id,var_id,src_sng,&var_typ,&dmn_nbr,(int *)NULL,&att_nbr_vsb);
    if(CDL) prn_ndn+=2*prn_flg->var_fst;
    if(XML) prn_ndn+=prn_flg->sxn_fst;
    if(JSN) prn_ndn+=prn_flg->sxn_fst;
    if(prn_flg->new_fmt && TRD) prn_ndn+=prn_flg->var_fst;
  } /* end else */

  /* Allocate space for attribute names and types */
  att_nbr_ttl=att_nbr_vsb;
  assert(att_nbr_ttl >= 0);
  if(att_nbr_ttl > 0) att=(att_sct *)nco_malloc(att_nbr_ttl*sizeof(att_sct));
    
  rcd=nco_inq_format(grp_id,&fl_fmt);
  rcd=nco_inq_format_extended(grp_id,&fl_fmt_xtn,(int *)NULL);

  if(prn_flg->hdn && ((fl_fmt == NC_FORMAT_NETCDF4) || (fl_fmt == NC_FORMAT_NETCDF4_CLASSIC))){
    char *val_hdn_sng=NULL;
    int chk_typ; /* [enm] Checksum type [0..9] */
    int deflate; /* [flg] Deflation is on */
    int dfl_lvl; /* [enm] Deflate level [0..9] */
    int flg_ndn=NC_ENDIAN_NATIVE; /* [enm] _Endianness */
    int fll_nil; /* [flg] NO_FILL */
    int shuffle; /* [flg] Shuffling is on */
    int srg_typ; /* [enm] Storage type */
    size_t cnk_sz[NC_MAX_VAR_DIMS]; /* [nbr] Chunk sizes */
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
	  /* _SOURCE_FORMAT only and always printed for root group */
	  idx=att_nbr_ttl++;
	  att=(att_sct *)nco_realloc(att,att_nbr_ttl*sizeof(att_sct));
	  att[idx].nm=(char *)strdup("_SOURCE_FORMAT");
	  att[idx].type=NC_CHAR;
	  val_hdn_sng=strdup(nco_fmt_xtn_sng(fl_fmt_xtn));
	  att_sz=att[idx].sz=strlen(val_hdn_sng);
	  att[idx].val.vp=(void *)nco_malloc(att_sz*nco_typ_lng(att[idx].type));
	  strncpy(att[idx].val.cp,val_hdn_sng,att_sz);
	  if(val_hdn_sng) val_hdn_sng=(char *)nco_free(val_hdn_sng);
	  /* _NCProperties, _IsNetcdf4, _SuperblockVersion only printed for root group
	     _NCProperties is persistent, added at file creation 
	     _IsNetcdf4 and _SuperblockVersion are computed by traversing file with HDF5 API, looking for clues
	     All were introduced in 4.4.1-rc2 on 20160513 */
	  if(nco_fmt_xtn_get() != nco_fmt_xtn_hdf4 && NC_LIB_VERSION >= 441){
	    /* 20160514: nc_inq_att() for "_NCProperties" returns type==NC_NAT or random integer, and att_sz is random for files without _NCProperties */
	    /* 20160514: nc_inq_att() for "_IsNetcdf4" returns random type and size too */
	    /* 20160719: Above issues were fixed in netCDF 4.4.1 final release */
	    strcpy(att_nm,"_NCProperties");
	    rcd=nco_inq_att_flg(grp_id,var_id,att_nm,&att_typ,&att_sz);
	    if(rcd == NC_NOERR){
	      idx=att_nbr_ttl++;
	      att=(att_sct *)nco_realloc(att,att_nbr_ttl*sizeof(att_sct));
	      att[idx].nm=(char *)strdup(att_nm);
	      att[idx].type=att_typ;
	      att[idx].sz=att_sz;
	      att[idx].val.vp=(void *)nco_malloc(att_sz*nco_typ_lng(att[idx].type));
	      rcd=nco_get_att(grp_id,var_id,att[idx].nm,att[idx].val.vp,att[idx].type);
	    } /* !rcd */
	    /* _IsNetcdf4 */
	    strcpy(att_nm,"_IsNetcdf4");
	    rcd=nco_inq_att_flg(grp_id,var_id,att_nm,&att_typ,&att_sz);
	    if(rcd == NC_NOERR){
	      idx=att_nbr_ttl++;
	      att=(att_sct *)nco_realloc(att,att_nbr_ttl*sizeof(att_sct));
	      att[idx].nm=(char *)strdup(att_nm);
	      att[idx].type=att_typ;
	      att[idx].sz=att_sz;
	      att[idx].val.vp=(void *)nco_malloc(att_sz*nco_typ_lng(att[idx].type));
	      rcd=nco_get_att(grp_id,var_id,att[idx].nm,att[idx].val.vp,att[idx].type);
	    } /* !rcd */
	    /* _SuperblockVersion */
	    strcpy(att_nm,"_SuperblockVersion");
	    rcd=nco_inq_att_flg(grp_id,var_id,att_nm,&att_typ,&att_sz);
	    if(rcd == NC_NOERR){
	      idx=att_nbr_ttl++;
	      att=(att_sct *)nco_realloc(att,att_nbr_ttl*sizeof(att_sct));
	      att[idx].nm=(char *)strdup(att_nm);
	      att[idx].type=att_typ;
	      att[idx].sz=att_sz;
	      att[idx].val.vp=(void *)nco_malloc(att_sz*nco_typ_lng(att[idx].type));
	      rcd=nco_get_att(grp_id,var_id,att[idx].nm,att[idx].val.vp,att[idx].type);
	    } /* !rcd */
	  } /* !441 */
	} /* !rcd */	
      } /* !xml */
    }else{
     if(nco_fmt_xtn_get() != nco_fmt_xtn_hdf4 || NC_LIB_VERSION >= 433){
	/* _NOFILL */
	rcd=nco_inq_var_fill(grp_id,var_id,&fll_nil,(int *)NULL);
	if(fll_nil){
	  /* Print _NOFILL for variables that are not pre-filled
	     20190217: netCDF 4.4+ ncdump prints _NoFill = "true" not _NOFILL = 1 like ncks
	     Behavior has changed since first implemented, yet seem backwards compatible 
	     Nevertheless should emulate newer behavior... */
	  idx=att_nbr_ttl++;
	  att=(att_sct *)nco_realloc(att,att_nbr_ttl*sizeof(att_sct));
	  // att[idx].nm=(char *)strdup("_NOFILL"); /* Deprecated 20190217 */
	  att[idx].nm=(char *)strdup("_NoFill");
	  att[idx].type=NC_CHAR;
	  val_hdn_sng= (fll_nil == 1) ? (char *)strdup("true") : (char *)strdup("false");
	  att_sz=att[idx].sz=strlen(val_hdn_sng);
	  att[idx].val.vp=(void *)nco_malloc(att_sz*nco_typ_lng(att[idx].type));
	  strncpy(att[idx].val.cp,val_hdn_sng,att_sz);
	  if(val_hdn_sng) val_hdn_sng=(char *)nco_free(val_hdn_sng);
	} /* !fll_nil */
	/* _Storage */
	rcd=nco_inq_var_chunking(grp_id,var_id,&srg_typ,cnk_sz);
	if(!XML){
	  if(dmn_nbr > 0){
	    /* Print _Storage for all arrays (not for any scalars) */
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
	/* _Filter */
	if(NC_LIB_VERSION >= 460){
	  if(!XML){
	    if(srg_typ == NC_CHUNKED){
	      /* Support dynamic compression filters as of netCDF 4.6.0 (January, 2018)
		 Unfortunately, I do not yet have an actual file or dynamic filter library for testing:
		 https://www.unidata.ucar.edu/software/netcdf/docs/filters_8md_source.html */
	      unsigned int flt_id=NC_MAX_UINT;
	      size_t flt_nbr;
	      size_t prm_idx;
	      size_t prm_nbr;
	      unsigned int *prm_lst=NULL;
	      char sng_foo[12]; /* nbr] Maximum printed size of unsigned integer (4294967295) + 1 (for comma) + 1 (for trailing NUL) */
	      /* 20200418 netCDF 4.7.4 nc_inq_var_filter() is backwards incompatible
		 https://github.com/Unidata/netcdf-c/issues/1693
		 As of 4.7.4, nc_inq_var_filter() called on a chunked and shuffled 
		 though not compressed variable in a netCDF4 dataset returns error -136:
		 "NetCDF: Filter error: filter not defined for variable"
		 Prior to 4.7.4, nc_inq_var_filter() returns NC_NOERR on such variables, and sets the filter ID to 0
		 Workaround for netCDF 4.7.4 is to proceed only if first nc_inq_var_filter_flg() returns NC_NOERR */
	      // 20200701 Dennis Heimbigner reverted the 4.7.4 change in 4.8.0-develop
	      //if(NC_LIB_VERSION == 474 || NC_LIB_VERSION == 480){
	      if(NC_LIB_VERSION == 474){
		rcd=nco_inq_var_filter_flg(grp_id,var_id,&flt_id,&prm_nbr,NULL);
		if(rcd == NC_ENOFILTER){
		  rcd=NC_NOERR;
		  flt_id=0;
		  prm_nbr=0;
		} /* !rcd */
	      }else{ /* !netCDF < 4.7.4 || >= 4.8.0 */
		rcd=nco_inq_var_filter(grp_id,var_id,&flt_id,&prm_nbr,NULL);
	      } /* !netCDF 4.7.4 */
	      //if(nco_dbg_lvl_get() >= nco_dbg_var) (void)fprintf(stdout,"%s: DEBUG %s reports flt_id = %u, prm_nbr = %lu\n",nco_prg_nm_get(),fnc_nm,flt_id,(unsigned long)prm_nbr);
	      if(flt_id != NC_MAX_UINT && flt_id != 0){
		rcd=nco_inq_var_filter_ids(grp_id,var_id,&flt_nbr,NULL);
		if(flt_nbr > 1){
		  nco_inq_varname(grp_id,var_id,var_nm);
		  (void)fprintf(stdout,"%s: WARNING %s reports variable %s has %lu filters applied. The proper library functions to correctly handle multiple filters are not present in netCDF version 4.7.4. Stay tuned.\n",nco_prg_nm_get(),fnc_nm,var_nm,(unsigned long)flt_nbr);
		} /* !flt_nbr */
		/* Print _Filter for (first filter of) filtered variables */
		prm_lst=(unsigned int *)nco_malloc(prm_nbr*sizeof(unsigned int));
		rcd=nco_inq_var_filter(grp_id,var_id,NULL,NULL,prm_lst);
		idx=att_nbr_ttl++;
		att=(att_sct *)nco_realloc(att,att_nbr_ttl*sizeof(att_sct));
		att[idx].nm=(char *)strdup("_Filter");
		att[idx].type=NC_CHAR;
		val_hdn_sng=(char *)nco_malloc(100L*sizeof(char));
		sprintf(val_hdn_sng,"%u,",flt_id);
		for(prm_idx=0;prm_idx<prm_nbr;prm_idx++){
		  //if(flt_id == 37373 && nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stdout,"Filter parameter #%lu of %lu = %u\n",prm_idx,prm_nbr,prm_lst[prm_idx]);
		  (void)sprintf(sng_foo,"%u%s",prm_lst[prm_idx],(prm_idx == prm_nbr-1) ? "" : ",");
		  strcat(val_hdn_sng,sng_foo);
		} /* !prm_idx */
		att_sz=att[idx].sz=strlen(val_hdn_sng);
		att[idx].val.vp=(void *)nco_malloc(att_sz*nco_typ_lng(att[idx].type));
		strncpy(att[idx].val.cp,val_hdn_sng,att_sz);
		if(val_hdn_sng) val_hdn_sng=(char *)nco_free(val_hdn_sng);
		if(prm_lst) prm_lst=(unsigned int *)nco_free(prm_lst);
	      } /* !flt_id */
	    } /* srg_typ != NC_CHUNKED */
	  } /* !xml */
	} /* !4.6.0 */
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
	  if((var_typ != NC_CHAR) && (var_typ != NC_UBYTE) && (var_typ != NC_BYTE) && (var_typ != NC_STRING)){
	    /* ncdump has always printed _Endianness variable attribute for integer types
	       ncdump started printing _Endianness for floating point types at some unknown date < 2019 */
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

  if(JSN && att_nbr_ttl > 0){
    (void)fprintf(fp_out,"%*s\"attributes\": {\n",prn_ndn,spc_sng);
    prn_ndn+=prn_flg->sxn_fst;  
  } /* !JSN */
 
  /* Get attributes' names, types, lengths, and values */
  for(idx=0;idx<att_nbr_ttl;idx++){

    char mbr_nm[NC_MAX_NAME+1L]; /* [sng] Member name */
    long long mbr_val;

    double *vln_val_dp;
    float *vln_val_fp;
    nco_int *vln_val_ip;
    nco_short *vln_val_sp;
    nco_byte *vln_val_bp;
    nco_ubyte *vln_val_ubp;
    nco_ushort *vln_val_usp;
    nco_uint *vln_val_uip;
    nco_int64 *vln_val_i64p;
    nco_uint64 *vln_val_ui64p;
    nco_vlen vln_val;
    size_t fld_nbr;
    size_t vln_idx;
    size_t vln_lng;
    size_t vln_lngm1;
    
    nc_type bs_typ;
    nc_type cls_typ;
    
    if(idx <= att_nbr_vsb-1){
      /* Visible attributes get standard treatment */
      att[idx].nm=(char *)nco_malloc(NC_MAX_NAME*sizeof(char));
      (void)nco_inq_attname(grp_id,var_id,idx,att[idx].nm);
      (void)nco_inq_att(grp_id,var_id,att[idx].nm,&att[idx].type,&att[idx].sz);

      /* Allocate enough space to hold attribute */
      att[idx].val.vp=(void *)nco_malloc(att[idx].sz*nco_typ_lng_udt(grp_id,att[idx].type));
      (void)nco_get_att(grp_id,var_id,att[idx].nm,att[idx].val.vp,att[idx].type);

      /* NC_CHAR can have zero length size maybe others? Create with a single FILL value */
      if(att[idx].sz == 0L){
        att[idx].val=nco_mss_val_mk(att[idx].type);
        att[idx].sz=1;
      } /* !att.sz */
    } /* idx == att_nbr */
    
    bs_typ=cls_typ=att[idx].type;
    if(att[idx].type > NC_MAX_ATOMIC_TYPE) rcd=nco_inq_user_type(grp_id,att[idx].type,NULL,NULL,&bs_typ,&fld_nbr,&cls_typ);

    /* Copy value to avoid indirection in loop over att_sz */
    att_sz=att[idx].sz;
    att_szm1=att_sz-1L;

    if(CDL){
      char *typ_nm;
      nm_cdl=nm2sng_cdl(att[idx].nm);
      typ_nm=cdl_typ_nm_udt(grp_id,att[idx].type);
      if(cls_typ >= NC_MAX_ATOMIC_TYPE) (void)fprintf(fp_out,"%*s%s %s:%s = ",prn_ndn,spc_sng,typ_nm,src_sng,nm_cdl); else (void)fprintf(fp_out,"%*s%s:%s = ",prn_ndn,spc_sng,src_sng,nm_cdl); 
      if(cls_typ > NC_MAX_ATOMIC_TYPE) typ_nm=(char *)nco_free(typ_nm);
      nm_cdl=(char *)nco_free(nm_cdl);
    } /* !cdl */

    if(JSN){ 
      nm_jsn=nm2sng_jsn(att[idx].nm);

      jsn_obj=False;         
      if(prn_flg->jsn_att_fmt == 2 ||
	 (prn_flg->jsn_att_fmt == 1 && (att[idx].type != NC_FLOAT && att[idx].type != NC_INT && att[idx].type != NC_STRING && att[idx].type != NC_CHAR)))
        jsn_obj=True;   
 
      if(jsn_obj) (void)fprintf(fp_out,"%*s\"%s\": { \"type\": \"%s\", \"data\": ",prn_ndn,spc_sng,nm_jsn,jsn_typ_nm(att[idx].type)); else (void)fprintf(fp_out,"%*s\"%s\": ",prn_ndn,spc_sng,nm_jsn); 
 
      /* Multi-element so print array open */ 
      if(att_sz > 1L && att[idx].type != NC_CHAR) (void)fprintf(fp_out,"["); 
      nm_jsn=(char *)nco_free(nm_jsn);    
    } /* !JSN */
    if(TRD) (void)fprintf(fp_out,"%*s%s attribute %i: %s, size = %li %s, value = ",prn_ndn,spc_sng,src_sng,idx,att[idx].nm,att_sz,nco_typ_sng(att[idx].type));

    spr_sng=cma_sng; /* [sng] Output separator string */
    if(XML){
      /* Official NcML XML Schema is here:
	 http://www.unidata.ucar.edu/software/thredds/current/netcdf-java/ncml/AnnotatedSchema4.html // 2015
	 http://www.unidata.ucar.edu/software/thredds/current/netcdf-java/ncml/v2.2/AnnotatedSchema4.html // older
	 http://www.unidata.ucar.edu/schemas/netcdf/ncml-2.2.xsd */

      char *typ_nm;
      char *xml_att_nm;   
      if(att[idx].type <=NC_MAX_ATOMIC_TYPE)
	typ_nm=strdup(xml_typ_nm(att[idx].type));
      else
        typ_nm=cdl_typ_nm_udt(grp_id,att[idx].type);  			     

      xml_att_nm=sng2sng_sf(att[idx].nm,2);
      
      (void)fprintf(fp_out,"%*s<ncml:attribute name=\"%s\"",prn_ndn,spc_sng,xml_att_nm);

      /* User may override default separator string for XML only */
      if(att[idx].type == NC_STRING || att[idx].type == NC_CHAR) spr_sng= (prn_flg->spr_chr) ? prn_flg->spr_chr : spr_xml_chr; else spr_sng= (prn_flg->spr_nmr) ? prn_flg->spr_nmr : spr_xml_nmr;

      /* Print type of non-string variables
	 NB: Take or lose this opportunity to distinguish char from string?
	 Until ~2014 NcML did not preserve unsigned types, so why not turn char attributes into strings? 
	 Can turn char into string here just by omitting "type=char" attribute
	 toolsui NcML does not print "type=char" for for char attributes
	 Hence neither does ncks */
      if(att[idx].type != NC_STRING && att[idx].type != NC_CHAR){
	(void)fprintf(fp_out," type=\"%s\"",typ_nm);
	/* Print hidden attributes */
	/* Until ~2014 toolsui showed no way to indicate unsigned types for attributes
	   20151207 Aleksander Jelenak and Ed Armstrong request "_Unsigned" attributes to denote unsigned attribute types */
	if(nco_xml_typ_rqr_nsg_att(att[idx].type)) (void)fprintf(fp_out," isUnsigned=\"true\"");
      } /* endif */

      if(att[idx].type == NC_VLEN)
	(void)fprintf(fp_out," shape=\"*\"" );
      
      /* Print separator element for non-whitespace separators */
      if((att[idx].sz == 1L && att[idx].type == NC_STRING) || att[idx].sz > 1L){ 
	/* Ensure string attribute value does not contain separator string */
	if(att[idx].type == NC_CHAR)
	  if(strstr(att[idx].val.cp,spr_sng)) spr_sng=spr_xml_chr_bck;
	if(att[idx].type == NC_STRING){
	  for(lmn=0;lmn<att_sz;lmn++){
	    if(att[idx].val.sngp[lmn] && strstr(att[idx].val.sngp[lmn],spr_sng)){
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
	if(spr_sng_idx < spr_sng_lng) (void)fprintf(fp_out," separator=\"%s\"",spr_sng);
      } /* att[idx].sz */
      (void)fprintf(fp_out," value=\"");

      typ_nm=(char*)nco_free(typ_nm);
      xml_att_nm=(char*)nco_free(xml_att_nm);
      
      /* XML-mode if dataset defines its own _FillValue for this variable? */
      // if(!(int)strcasecmp(att[idx].nm,nco_mss_val_sng_get())) has_fll_val=True;
    } /* !xml */
    
    /* Typecast pointer to values before access */
    (void)cast_void_nctype(cls_typ,&att[idx].val);
    
    if(CDL){
      	/* CDL attribute values of many types must have type-specific suffixes (e.g., s, u, ull) unless they are user-defined types that, like variables, can always be disambiguated by their explicit (not implicit) type specifier that resolves to a base atomic-type */
      	(void)sprintf(att_sng_pln,"%s",cls_typ <= NC_MAX_ATOMIC_TYPE ? nco_typ_fmt_sng_att_cdl(bs_typ) : nco_typ_fmt_sng_var_cdl(bs_typ));
      	(void)sprintf(att_sng_dlm,"%s%%s",cls_typ <= NC_MAX_ATOMIC_TYPE ? nco_typ_fmt_sng_att_cdl(bs_typ) : nco_typ_fmt_sng_var_cdl(bs_typ));
    }else if(XML) {
    	(void)sprintf(att_sng_pln,"%s", nco_typ_fmt_sng_att_xml(bs_typ) );
      	(void)sprintf(att_sng_dlm,"%s%%s", nco_typ_fmt_sng_att_xml(bs_typ) );
    }else if(JSN) {
    	(void)sprintf(att_sng_pln,"%s", nco_typ_fmt_sng_att_jsn(bs_typ) );
		(void)sprintf(att_sng_dlm,"%s%%s", nco_typ_fmt_sng_att_jsn(bs_typ) );
    }else{
		(void)sprintf(att_sng_pln,"%s", nco_typ_fmt_sng(bs_typ));
		(void)sprintf(att_sng_dlm,"%s%%s", nco_typ_fmt_sng(bs_typ));
    }
    switch(cls_typ){
    case NC_FLOAT:
      for(lmn=0;lmn<att_sz;lmn++){
	val_flt=att[idx].val.fp[lmn];
	if(isfinite(val_flt)){
	  rcd_prn=snprintf(val_sng,(size_t)NCO_ATM_SNG_LNG,att_sng_pln,val_flt);
	  (void)sng_trm_trl_zro(val_sng,prn_flg->nbr_zro);
	}else{
          (void)nco_prn_nonfinite_flt(val_sng,prn_flg,val_flt);
	} /* endelse */
	(void)fprintf(fp_out,"%s%s",val_sng,(lmn != att_szm1) ? spr_sng : "");
      } /* end loop */
      break;
    case NC_DOUBLE:
      for(lmn=0;lmn<att_sz;lmn++){
	val_dbl=att[idx].val.dp[lmn];
	if(isfinite(val_dbl)){
	  rcd_prn=snprintf(val_sng,(size_t)NCO_ATM_SNG_LNG,att_sng_pln,val_dbl);
	  (void)sng_trm_trl_zro(val_sng,prn_flg->nbr_zro);
	}else{
          (void)nco_prn_nonfinite_dbl(val_sng,prn_flg,val_dbl);
	} /* endelse */
	(void)fprintf(fp_out,"%s%s",val_sng,(lmn != att_szm1) ? spr_sng : "");
      } /* end loop */
      break;
    case NC_INT: for(lmn=0;lmn<att_sz;lmn++) (void)fprintf(fp_out,att_sng_dlm,(long)att[idx].val.ip[lmn],(lmn != att_szm1) ? spr_sng : ""); break;
    case NC_SHORT: for(lmn=0;lmn<att_sz;lmn++) (void)fprintf(fp_out,att_sng_dlm,att[idx].val.sp[lmn],(lmn != att_szm1) ? spr_sng : ""); break;
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
	    if(CDL_OR_JSN) (void)fprintf(fp_out,"\"");
	    sng_val_sng[0]='\0';
	  } /* endif new string */
	  (void)strcat(sng_val_sng,(CDL ? chr2sng_cdl(chr_val,val_sng) : XML ? chr2sng_xml(chr_val,val_sng) : chr2sng_jsn(chr_val,val_sng)));
	  if(chr_val == '\n' && lmn != att_szm1 && CDL) (void)sprintf(sng_val_sng,"%s\",\n%*s\"",sng_val_sng_cpy,prn_ndn+prn_flg->var_fst,spc_sng);
	  if(lmn%sng_lng == sng_lngm1){
	    (void)fprintf(fp_out,"%s%s",sng_val_sng,(CDL_OR_JSN) ? "\"" : "");
	    /* Print separator after non-final string */
	    if(lmn != att_szm1) (void)fprintf(fp_out,"%s",spr_sng);
	  } /* endif string end */
	  if(lmn == att_szm1) sng_val_sng=(char *)nco_free(sng_val_sng);
	}else{ /* Traditional */
	  /* Assume \0 is string terminator and do not print it */
	  if(chr_val != '\0') (void)fprintf(fp_out,"%c",chr_val);
	} /* endelse CDL, XML, Traditional */
      } /* end loop over element */
      break;
    case NC_BYTE: for(lmn=0;lmn<att_sz;lmn++) (void)fprintf(fp_out,att_sng_dlm,att[idx].val.bp[lmn],(lmn != att_szm1) ? spr_sng : ""); break;
    case NC_UBYTE: for(lmn=0;lmn<att_sz;lmn++) (void)fprintf(fp_out,att_sng_dlm,att[idx].val.ubp[lmn],(lmn != att_szm1) ? spr_sng : ""); break;
    case NC_USHORT: for(lmn=0;lmn<att_sz;lmn++) (void)fprintf(fp_out,att_sng_dlm,att[idx].val.usp[lmn],(lmn != att_szm1) ? spr_sng : ""); break;
    case NC_UINT: for(lmn=0;lmn<att_sz;lmn++) (void)fprintf(fp_out,att_sng_dlm,att[idx].val.uip[lmn],(lmn != att_szm1) ? spr_sng : ""); break;
    case NC_INT64: for(lmn=0;lmn<att_sz;lmn++) (void)fprintf(fp_out,att_sng_dlm,att[idx].val.i64p[lmn],(lmn != att_szm1) ? spr_sng : ""); break;
    case NC_UINT64: for(lmn=0;lmn<att_sz;lmn++) (void)fprintf(fp_out,att_sng_dlm,att[idx].val.ui64p[lmn],(lmn != att_szm1) ? spr_sng : ""); break;
    case NC_STRING:
      for(lmn=0;lmn<att_sz;lmn++){
	sng_val=att[idx].val.sngp[lmn];
	/* In strict CDL an NC_STRING null is indictaed by NIL, for now we output an empty string */
	if(sng_val) sng_lng=strlen(sng_val); else sng_lng=0L;
	sng_lngm1=sng_lng-1UL;
	if(CDL||XML||JSN){
	  /* Worst case is printable strings are six or four times longer than unformatted, i.e., '\"' == "&quot;" or '\\' == "\\\\" */
	  sng_val_sng=(char *)nco_malloc(6*sng_lng+1UL);
	  if(CDL||JSN) (void)fprintf(fp_out,"\"");
	  sng_val_sng[0]='\0';
	  for(chr_idx=0;chr_idx<sng_lng;chr_idx++){
	    val_sng[0]='\0';
	    chr_val=sng_val[chr_idx];
	    /* (void)strcat(sng_val_sng,(CDL) ? chr2sng_cdl(chr_val,val_sng) : chr2sng_xml(chr_val,val_sng)); */
	    (void)strcat(sng_val_sng,(CDL ? chr2sng_cdl(chr_val,val_sng) : XML ? chr2sng_xml(chr_val,val_sng) : chr2sng_jsn(chr_val,val_sng)));
	  } /* end loop over character */
	  (void)fprintf(fp_out,"%s%s",sng_val_sng,(XML) ? "" : "\"");
	  /* Print separator after non-final string */
	  if(lmn != att_szm1) (void)fprintf(fp_out,"%s",spr_sng);
	  sng_val_sng=(char *)nco_free(sng_val_sng);
	}else if(TRD){ /* Traditional */
	  (void)fprintf(fp_out,att_sng_dlm,att[idx].val.sngp[lmn],(lmn != att_szm1) ? spr_sng : "");
	} /* endelse CDL, XML, Traditional */
      } /* end loop over element */
      break;
    case NC_VLEN:

      for(lmn=0;lmn<att_sz;lmn++){
	vln_val=att[idx].val.vlnp[lmn];
	vln_lng=vln_val.len;
	vln_lngm1=vln_lng-1UL;

	vln_val_fp=(float *)vln_val.p;
	vln_val_dp=(double *)vln_val.p;
	vln_val_ip=(nco_int *)vln_val.p;
	vln_val_sp=(nco_short *)vln_val.p;
	vln_val_bp=(nco_byte *)vln_val.p;
	vln_val_ubp=(nco_ubyte *)vln_val.p;
	vln_val_usp=(nco_ushort *)vln_val.p;
	vln_val_uip=(nco_uint *)vln_val.p;
	vln_val_i64p=(nco_int64 *)vln_val.p;
	vln_val_ui64p=(nco_uint64 *)vln_val.p;

	if(CDL)
	  (void)fprintf(fp_out,"{");
	else if(JSN)
	  (void)fprintf(fp_out,"[");
	else if(XML)
	  (void)fprintf(fp_out,",");
	
	switch(bs_typ){
	case NC_FLOAT:
	  for(vln_idx=0;vln_idx<vln_lng;vln_idx++){
	    val_flt=vln_val_fp[vln_idx];
	    if(isfinite(val_flt)){
	      rcd_prn=snprintf(val_sng,(size_t)NCO_ATM_SNG_LNG,att_sng_pln,val_flt);
	      (void)sng_trm_trl_zro(val_sng,prn_flg->nbr_zro);
	    }else{
              (void)nco_prn_nonfinite_flt(val_sng,prn_flg,val_flt);
            }
	    
	    (void)fprintf(fp_out,"%s%s",val_sng,(vln_idx != vln_lngm1) ? spr_sng : "");
	  } /* !vln_idx */
	  break;
	case NC_DOUBLE:
	  for(vln_idx=0;vln_idx<vln_lng;vln_idx++){
	    val_dbl=vln_val_dp[vln_idx];
	    if(isfinite(val_dbl)){
	      rcd_prn=snprintf(val_sng,(size_t)NCO_ATM_SNG_LNG,att_sng_pln,val_dbl);
	      (void)sng_trm_trl_zro(val_sng,prn_flg->nbr_zro);
	    }else{
              (void)nco_prn_nonfinite_dbl(val_sng,prn_flg,val_dbl);
	    } /* endelse */
	    (void)fprintf(fp_out,"%s%s",val_sng,(vln_idx != vln_lngm1) ? spr_sng : "");
	  } /* !vln_idx */
	  break;
	case NC_INT: for(vln_idx=0;vln_idx<vln_lng;vln_idx++) (void)fprintf(fp_out,att_sng_dlm,(long)vln_val_ip[vln_idx],(vln_idx != vln_lngm1) ? spr_sng : ""); break;
	case NC_SHORT: for(vln_idx=0;vln_idx<vln_lng;vln_idx++) (void)fprintf(fp_out,att_sng_dlm,vln_val_sp[vln_idx],(vln_idx != vln_lngm1) ? spr_sng : ""); break;
	case NC_CHAR:
	  break;
	case NC_BYTE: for(vln_idx=0;vln_idx<vln_lng;vln_idx++) (void)fprintf(fp_out,att_sng_dlm,vln_val_bp[vln_idx],(vln_idx != vln_lngm1) ? spr_sng : ""); break;
	case NC_UBYTE: for(vln_idx=0;vln_idx<vln_lng;vln_idx++) (void)fprintf(fp_out,att_sng_dlm,vln_val_ubp[vln_idx],(vln_idx != vln_lngm1) ? spr_sng : ""); break;
	case NC_USHORT: for(vln_idx=0;vln_idx<vln_lng;vln_idx++) (void)fprintf(fp_out,att_sng_dlm,vln_val_usp[vln_idx],(vln_idx != vln_lngm1) ? spr_sng : ""); break;
	case NC_UINT: for(vln_idx=0;vln_idx<vln_lng;vln_idx++) (void)fprintf(fp_out,att_sng_dlm,vln_val_uip[vln_idx],(vln_idx != vln_lngm1) ? spr_sng : ""); break;
	case NC_INT64: for(vln_idx=0;vln_idx<vln_lng;vln_idx++) (void)fprintf(fp_out,att_sng_dlm,vln_val_i64p[vln_idx],(vln_idx != vln_lngm1) ? spr_sng : ""); break;
	case NC_UINT64: for(vln_idx=0;vln_idx<vln_lng;vln_idx++) (void)fprintf(fp_out,att_sng_dlm,vln_val_ui64p[vln_idx],(vln_idx != vln_lngm1) ? spr_sng : ""); break;
	case NC_STRING:
	  break;
	default: nco_dfl_case_nc_type_err(); break;
	} /* !bs_typ switch */
	if(CDL) (void)fprintf(fp_out,"}%s",(lmn != att_szm1) ? spr_sng : "");
	if(JSN) (void)fprintf(fp_out,"]%s",(lmn != att_szm1) ? spr_sng : "");
	/* if(XML && lmn != att_szm1 ) (void)fprintf(fp_out," "); */
      } /* !lmn */
      rcd=nco_free_vlens(att_sz,att[idx].val.vlnp);
      break; /* !NC_VLEN */
    case NC_ENUM:
	(void)cast_void_nctype(bs_typ,&att[idx].val);
	for(lmn=0L;lmn<att_sz;lmn++){
	  switch(bs_typ){
	  case NC_BYTE: mbr_val=(long long)att[idx].val.bp[lmn]; break;
	  case NC_UBYTE: mbr_val=(long long)att[idx].val.ubp[lmn]; break;
	  case NC_SHORT: mbr_val=(long long)att[idx].val.sp[lmn]; break;
	  case NC_USHORT: mbr_val=(long long)att[idx].val.usp[lmn]; break;
	  case NC_INT: mbr_val=(long long)att[idx].val.ip[lmn]; break;
	  case NC_UINT: mbr_val=(long long)att[idx].val.uip[lmn]; break;
	  case NC_INT64: mbr_val=(long long)att[idx].val.i64p[lmn]; break;
	  case NC_UINT64: mbr_val=(long long)att[idx].val.ui64p[lmn]; break;
	  default: nco_dfl_case_nc_type_err(); break;
	  } /* !bs_typ switch */
	  rcd=nco_inq_enum_ident(grp_id,att[idx].type,mbr_val,mbr_nm);
	  (void)fprintf(fp_out,"%s%s",mbr_nm,(lmn != att_szm1) ? spr_sng : "");
	} /* !lmn */
      break; /* !NC_ENUM */
    case NC_COMPOUND:
    case NC_OPAQUE:
    default: nco_dfl_case_nc_type_err();
      break;
    } /* !cls_typ switch */
    if(CDL){
      if(nco_dbg_lvl_get() >= nco_dbg_std){
	/* 20161129: Add netCDF attribute type as comment after semi-colon. Yes, "string" is redundant. */
	(void)fprintf(fp_out," ; // %s\n",cdl_typ_nm(bs_typ));
      }else{ /* !dbg */
	(void)fprintf(fp_out," ;\n");
      } /* !dbg */
    } /* !CDL */
    if(TRD) (void)fprintf(fp_out,"\n");
    if(XML) (void)fprintf(fp_out,"\" />\n");
    if(JSN){ 
      /* Close list for multi-element attributes */ 
      if(att_sz > 1L && att[idx].type != NC_CHAR) (void)fprintf(fp_out,"]");          
      /* Close JSON object tag */  
      if(jsn_obj) (void)fprintf(fp_out,"%s",(idx < att_nbr_ttl-1) ? "},\n" : "}"); else (void)fprintf(fp_out,"%s",(idx < att_nbr_ttl-1) ? ",\n" : "");
    } /* !JSN */

    rcd_prn+=0; /* CEWI */
  } /* !idx */

  /* Omit comma and carriage-return for final attribute */
  if(JSN && att_nbr_ttl>0){
    prn_ndn-=prn_flg->sxn_fst;  
    (void)fprintf(fp_out,"\n%*s}",prn_ndn,spc_sng);
  } /* !JSN */

  /* Print extra line after global attributes */
  if(CDL && flg_glb) (void)fprintf(fp_out,"\n");
  if(!prn_flg->new_fmt && CDL_OR_TRD) (void)fprintf(fp_out,"\n");
  (void)fflush(fp_out);
  
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
    if(var_type<=NC_MAX_ATOMIC_TYPE &&  nco_xml_typ_rqr_nsg_att(var_type)) (void)fprintf(fp_out,"%*s<ncml:attribute name=\"_Unsigned\" value=\"true\" />\n",prn_ndn,spc_sng);
    /* 20131231: Emulate toolsUI 4.3 _FillValue behavior for unsigned types (present in NCO 4.3.7-4.3.9, deprecated in 4.4.0 */
    // if(nco_xml_typ_rqr_flv_att(var_type) && !has_fll_val) (void)fprintf(fp_out,"%*s<ncml:attribute name=\"_FillValue\" type=\"%s\" value=\"%d\" />\n",prn_ndn,spc_sng,xml_typ_nm(var_type),(var_type == NC_UINT64) ? -2 : -1);
  } /* !xml */

  /* Free space holding attribute values */
  for(idx=0;idx<att_nbr_ttl;idx++){
    if(att[idx].type <= NC_MAX_ATOMIC_TYPE) att[idx].val.vp=nco_free(att[idx].val.vp);
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
     Unidata formats shown in netcdf-c/ncdump/ncdump.c pr_att_valgs() near line 593
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
     Unidata formats shown in netcdf-c/ncdump/ncdump.c pr_att_valgs() near line 593
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
  static const char fmt_NC_UINT[]="%uul"; /*  */
  static const char fmt_NC_INT64[]="%llill"; /*  */
  static const char fmt_NC_UINT64[]="%lluull"; /*  */
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
     Unidata formats shown in netcdf-c/ncdump/ncdump.c pr_att_valgs() near line 593
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

const char * /* O [sng] sprintf() format string for JSON attribute type typ */
nco_typ_fmt_sng_att_jsn /* [fnc] Provide sprintf() format string for specified attribute type in XML */
(const nc_type typ) /* I [enm] netCDF attribute type to provide XML format string for */
{
  /* Purpose: Provide sprintf() format string for specified type attribute
     Unidata formats shown in netcdf-c/ncdump/ncdump.c pr_att_valgs() near line 593
     Float formats called float_att_fmt, double_att_fmt are in dumplib.c,
     and are user-configurable with -p float_digits,double_digits.
     These default to 7 and 15, respectively
     Use these formats for JSON attributes AND variables */
  
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
} /* !nco_typ_fmt_sng_att_jsn() */

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
nco_prn_var_val_cmt /* Print variable values as CDL comment (delimited by comma) */
(var_sct *var, /* I [sct] variable to print */
 const prn_fmt_sct * const prn_flg) /* I [sct] Print-format information */
{
  const char *mss_val_sng="_";

  char *fmt_sng_mss_val=NULL;

  char dlm_sng[NCO_MAX_LEN_FMT_SNG]={0};

  FILE *fp_out=prn_flg->fp_out; /* [fl] Formatted text output file handle */

  nco_bool is_mss_val=False;

  long lmn;
  long sz;

  size_t val_sz_byt;

  /* Print each element with user-supplied formatting code */
  /* Replace C language '\X' escape codes with ASCII bytes */
  sz=var->sz;
  sprintf(dlm_sng,"%s",nco_typ_fmt_sng_var_cdl(var->type));

  (void)sng_ascii_trn(dlm_sng);

  if(var->has_mss_val) val_sz_byt=nco_typ_lng(var->type);

  /* Assume -s argument (dlm_sng) formats entire string
     Otherwise, one could assume that field will be printed with format nco_typ_fmt_sng(var->type),
     and that user is only allowed to affect text between fields. 
     This would be accomplished with:
     (void)sprintf(var_sng,"%s%s",nco_typ_fmt_sng(var->type),dlm_sng);*/

  /* Find replacement format string at most once, then re-use */
  #ifdef NCO_HAVE_REGEX_FUNCTIONALITY
    /* Replace printf()-format statements with format for missing values */
      fmt_sng_mss_val=nco_fmt_sng_printf_subst(dlm_sng);
  #endif /* !NCO_HAVE_REGEX_FUNCTIONALITY */

  /* Print type in English in prefix text */
  if(var->type == NC_STRING) (void)fprintf(fp_out,"calendar format: "); else (void)fprintf(fp_out,"%s value%s: ",cdl_typ_nm(var->type),(var->sz > 1 ? "s":""));

  for(lmn=0;lmn<sz;lmn++){

    if(prn_flg->PRN_MSS_VAL_BLANK && var->has_mss_val){
      /* memcmp() triggers pedantic warning unless pointer arithmetic is cast to type char */
      if(var->type == NC_STRING) is_mss_val=!strcmp(var->val.sngp[lmn],var->mss_val.sngp[0]); else is_mss_val=!memcmp((char *)var->val.vp+lmn*val_sz_byt,var->mss_val.vp,(size_t)val_sz_byt);
    } /* !PRN_MSS_VAL_BLANK */
      
    if(prn_flg->PRN_MSS_VAL_BLANK && var->has_mss_val && is_mss_val){
      if(strcmp(dlm_sng,fmt_sng_mss_val)){
	(void)fprintf(fp_out,fmt_sng_mss_val,mss_val_sng);
      }else{
	(void)fprintf(fp_out,"%s",mss_val_sng);
      } /* !strcmp() */
    }else{ /* !is_mss_val */
        switch(var->type){
          case NC_FLOAT: (void)fprintf(fp_out,dlm_sng,var->val.fp[lmn]); break;
          case NC_DOUBLE: (void)fprintf(fp_out,dlm_sng,var->val.dp[lmn]); break;
          case NC_SHORT: (void)fprintf(fp_out,dlm_sng,var->val.sp[lmn]); break;
          case NC_INT: (void)fprintf(fp_out,dlm_sng,var->val.ip[lmn]); break;
          case NC_CHAR: (void)fprintf(fp_out,dlm_sng,var->val.cp[lmn]); break;
          case NC_BYTE: (void)fprintf(fp_out,dlm_sng,var->val.bp[lmn]); break;
          case NC_UBYTE: (void)fprintf(fp_out,dlm_sng,var->val.ubp[lmn]); break;
          case NC_USHORT: (void)fprintf(fp_out,dlm_sng,var->val.usp[lmn]); break;
          case NC_UINT: (void)fprintf(fp_out,dlm_sng,var->val.uip[lmn]); break;
          case NC_INT64: (void)fprintf(fp_out,dlm_sng,var->val.i64p[lmn]); break;
          case NC_UINT64: (void)fprintf(fp_out,dlm_sng,var->val.ui64p[lmn]); break;
          case NC_STRING: (void)fprintf(fp_out,dlm_sng,var->val.sngp[lmn]); break;
          default: nco_dfl_case_nc_type_err(); break;
       } /* end switch */

    } /* !is_mss_val */

    if(lmn<sz-1L) (void)fprintf(fp_out,", ");

  } /* end loop over element */

  (void)fprintf(fp_out,"\n");

  if(fmt_sng_mss_val) fmt_sng_mss_val=(char *)nco_free(fmt_sng_mss_val);

}/* end nco_prn_var_val_cmt() */

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

  char nul_chr='\0';
  char var_sng[NCO_MAX_LEN_FMT_SNG];
  char *unit_sng=NULL;
  
  dmn_sct *dim=NULL_CEWI;

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
  
  nco_bool SRD=False; /* Stride is non-unity */
  nco_bool WRP=False; /* Coordinate is wrapped */

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
	if(dmn_srd[idx] == 1L) (void)nco_get_vara(in_id,dim[idx].cid,dmn_srt+idx,dmn_cnt+idx,dim[idx].val.vp,dim[idx].type); else nco_get_vars(in_id,dim[idx].cid,dmn_srt+idx,dmn_cnt+idx,dmn_srd+idx,dim[idx].val.vp,dim[idx].type);
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
      nco_get_vars(in_id,var.id,dmn_srt,dmn_cnt,dmn_srd,var.val.vp,var.type);
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

  if(var.nbr_dim == 0 && !dlm_sng){ 
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

  if(var.nbr_dim > 0 && !dlm_sng){ 
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
	if(nco_dbg_lvl_get() >= nco_dbg_var)(void)fprintf(stdout,"DEBUG: format string used for chars is dmn_sng = %s, var_sng = %s\n",dmn_sng,var_sng); 
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
nco_prn_var_dfn /* [fnc] Print variable metadata */
(const int nc_id, /* I [id] netCDF file ID */
 const prn_fmt_sct * const prn_flg, /* I [sct] Print-format information */
 const trv_sct * const var_trv) /* I [sct] Object to print (variable) */
{
  /* Purpose: Print variable metadata */
  //const char fnc_nm[]="nco_prn_var_dfn()";
  const char spc_sng[]=""; /* [sng] Space string */

  char *dmn_sng=NULL;
  char *nm_cdl;
  char *nm_jsn;

  char sng_foo[NC_MAX_NAME+10]; /* Add 10 for extra formatting characters */
  char sz_sng[100];

  FILE *fp_out=prn_flg->fp_out; /* [fl] Formatted text output file handle */

  float vln_lng_avg; /* [nbr] Mean vlen length */

  int deflate; /* [flg] Deflation is on */
  int dfl_lvl; /* [enm] Deflate level [0..9] */
  int dmn_idx;
  int grp_id;
  int nbr_att;
  int dmn_nbr;
  int packing; /* [flg] Variable is packed */
  int prn_ndn=0; /* [nbr] Indentation for printing */
  int shuffle; /* [flg] Shuffling is on */
  int srg_typ; /* [enm] Storage type */
  int var_id;

  long var_sz=1L;

  nc_type bs_typ;
  nc_type cls_typ;
  nc_type var_typ;

  nco_bool CRR_DMN_IS_REC_IN_INPUT[NC_MAX_DIMS]; /* [flg] Is record dimension */

  size_t cnk_sz[NC_MAX_VAR_DIMS]; /* [nbr] Chunk sizes */
  size_t dmn_sz[NC_MAX_VAR_DIMS]; /* [nbr] Dimension sizes */

  size_t ram_sz_crr;
  static size_t ram_sz_ttl=0L;

  /* Obtain group ID */
  (void)nco_inq_grp_full_ncid(nc_id,var_trv->grp_nm_fll,&grp_id);

  /* Obtain variable ID */
  (void)nco_inq_varid(grp_id,var_trv->nm,&var_id);

  /* Get number of dimensions, type, and number of attributes for variable */
  (void)nco_inq_var(grp_id,var_id,(char *)NULL,&var_typ,&dmn_nbr,(int *)NULL,&nbr_att);
  bs_typ=cls_typ=var_typ;
  if(var_typ > NC_MAX_ATOMIC_TYPE) nco_inq_user_type(nc_id,var_typ,NULL,NULL,&bs_typ,NULL,&cls_typ);
  
  /* Storage properties */
  if(nco_fmt_xtn_get() != nco_fmt_xtn_hdf4 || (NC_LIB_VERSION >= 433 && NC_LIB_VERSION != 474)){
    (void)nco_inq_var_chunking(grp_id,var_id,&srg_typ,cnk_sz);
    (void)nco_inq_var_deflate(grp_id,var_id,&shuffle,&deflate,&dfl_lvl);
  } /* endif */
  (void)nco_inq_var_packing(grp_id,var_id,&packing);

  for(dmn_idx=0;dmn_idx<dmn_nbr;dmn_idx++){

    //    if(nco_dbg_lvl_get() >= nco_dbg_std && var_trv->nco_typ == nco_obj_typ_nonatomic_var) (void)fprintf(stdout,"%s: DEBUG %s reports non-atomic printing got to quark1\n",nco_prg_nm_get(),fnc_nm);

      if(var_trv->var_dmn[dmn_idx].is_crd_var){
      /* Dimension has coordinate variable */
      /* Get coordinate from table */
      crd_sct *crd=var_trv->var_dmn[dmn_idx].crd;
      /* Use hyperslabbed size */
      dmn_sz[dmn_idx]=crd->lmt_msa.dmn_cnt;
      CRR_DMN_IS_REC_IN_INPUT[dmn_idx]=crd->is_rec_dmn;
    }else{
      /* Dimension does not have associated coordinate variable */
      /* Get unique dimension */
      dmn_trv_sct *dmn_trv=var_trv->var_dmn[dmn_idx].ncd;
      /* Use hyperslabbed size */
      dmn_sz[dmn_idx]=dmn_trv->lmt_msa.dmn_cnt;
      CRR_DMN_IS_REC_IN_INPUT[dmn_idx]=dmn_trv->is_rec_dmn;
    } /* end else */
  } /* !dmn_idx */
  for(dmn_idx=0;dmn_idx<dmn_nbr;dmn_idx++) var_sz*=dmn_sz[dmn_idx];

  if(cls_typ == NC_VLEN){
    ptr_unn val;
    long *dmn_srt; /* [nbr] Dimension start index */
    long *dmn_cnt; /* [nbr] Dimension counts */
    size_t vln_lng; /* [nbr] vlen length */
    size_t vln_lng_ttl; /* [nbr] Total vlen length */
    long lmn;
    nco_vlen vln_val;
    dmn_srt=(long *)nco_malloc(dmn_nbr*sizeof(long));
    dmn_cnt=(long *)nco_malloc(dmn_nbr*sizeof(long));
    for(dmn_idx=0;dmn_idx<dmn_nbr;dmn_idx++){
      /* No single start vector exists for variables retrieved through MSA
	 Count vector is accurate even for variables retrieved through MSA
	 Since MSA invocation is rare, approximate size by assuming start vector is zero */
      if(var_trv->var_dmn[dmn_idx].is_crd_var){
	dmn_srt[dmn_idx]=0L;
	dmn_cnt[dmn_idx]=var_trv->var_dmn[dmn_idx].crd->lmt_msa.dmn_cnt;
      }else{
	dmn_srt[dmn_idx]=0L;
	dmn_cnt[dmn_idx]=var_trv->var_dmn[dmn_idx].ncd->lmt_msa.dmn_cnt;
      } /* !crd */
	//if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(fp_out,"%s: %s reports dmn_srt[%d]=%ld, dmn_cnt[%d]=%ld\n",nco_prg_nm_get(),fnc_nm,dmn_idx,dmn_srt[dmn_idx],dmn_idx,dmn_cnt[dmn_idx]);
    } /* !dmn_idx */
    val.vp=nco_malloc(var_sz*nco_typ_lng_udt(nc_id,var_typ));
    nco_get_vara(nc_id,var_id,dmn_srt,dmn_cnt,val.vp,var_typ);
    vln_lng_ttl=0L;
    /* Acquire and print mean vlen dimension size? */
    for(lmn=0;lmn<var_sz;lmn++){
      vln_val=val.vlnp[lmn];
      vln_lng=vln_val.len;
      vln_lng_ttl+=vln_lng;
      //if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(fp_out,"%s: %s reports len(vlen[%ld])=%ld\n",nco_prg_nm_get(),fnc_nm,lmn,vln_lng);
    } /* !lmn */
    vln_lng_avg=vln_lng_ttl/var_sz;
    //if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(fp_out,"%s: %s reports mean vlen size is %g\n",nco_prg_nm_get(),fnc_nm,vln_lng_avg);
    nco_free_vlens(var_sz,val.vlnp);
    if(dmn_srt) dmn_srt=(long *)nco_free(dmn_srt);
    if(dmn_cnt) dmn_cnt=(long *)nco_free(dmn_cnt);
    if(val.vp) val.vp=(void *)nco_free(val.vp);
  } /* !vlen */

  if(cls_typ == NC_VLEN) ram_sz_crr=var_sz*vln_lng_avg*nco_typ_lng_udt(nc_id,bs_typ); else ram_sz_crr=var_sz*nco_typ_lng_udt(nc_id,var_typ);
  ram_sz_ttl+=ram_sz_crr;
  
  /* Print header for variable */
  if(prn_flg->new_fmt && !prn_flg->xml && !prn_flg->jsn) prn_ndn=prn_flg->sxn_fst+prn_flg->var_fst+var_trv->grp_dpt*prn_flg->spc_per_lvl;
  if(prn_flg->xml) prn_ndn=prn_flg->sxn_fst+var_trv->grp_dpt*prn_flg->spc_per_lvl;
  if(prn_flg->jsn) prn_ndn=prn_flg->ndn;

  if(prn_flg->trd){
    if(nco_fmt_xtn_get() != nco_fmt_xtn_hdf4 || (NC_LIB_VERSION >= 433 && NC_LIB_VERSION != 474)) (void)fprintf(fp_out,"%*s%s: type %s, %i dimension%s, %i attribute%s, compressed? %s, chunked? %s, packed? %s\n",prn_ndn,spc_sng,var_trv->nm,nco_typ_sng(var_typ),dmn_nbr,(dmn_nbr == 1) ? "" : "s",nbr_att,(nbr_att == 1) ? "" : "s",(deflate) ? "yes" : "no",(srg_typ == NC_CHUNKED) ? "yes" : "no",(packing) ? "yes" : "no"); else (void)fprintf(fp_out,"%*s%s: type %s, %i dimension%s, %i attribute%s, compressed? HDF4_UNKNOWN, chunked? HDF4_UNKNOWN, packed? %s\n",prn_ndn,spc_sng,var_trv->nm,nco_typ_sng(var_typ),dmn_nbr,(dmn_nbr == 1) ? "" : "s",nbr_att,(nbr_att == 1) ? "" : "s",(packing) ? "yes" : "no");
    /* 20170913: Typically users not interested in variable ID. However, ID helps diagnose susceptibility to CDF5 bug */
    if(nco_dbg_lvl_get() >= nco_dbg_var) (void)fprintf(fp_out,"%*s%s ID = netCDF define order = %d\n",prn_ndn,spc_sng,var_trv->nm,var_id);
  } /* !trd */
  if(prn_flg->xml){
    int bs_sz;
    char *typ_nm=(char*)NULL;
    
    typ_nm=cdl_typ_nm_udt(grp_id,var_typ);
    
    (void)fprintf(fp_out,"%*s<ncml:variable name=\"%s\" ",prn_ndn,spc_sng,var_trv->nm);
    
    if(cls_typ <= NC_MAX_ATOMIC_TYPE){
      (void)fprintf(fp_out,"type=\"%s\"",xml_typ_nm(var_typ));    
    }else if(cls_typ == NC_ENUM){

      bs_sz=(int)nco_typ_lng(bs_typ);
      bs_sz=(bs_sz > 4 ? 4 : bs_sz); // 20180608: NcML 2.2 lacks enum8 type https://github.com/Unidata/thredds/issues/1098
      (void)fprintf(fp_out,"type=\"enum%d\" typedef=\"%s\"",bs_sz,typ_nm);
    }else if(cls_typ == NC_VLEN ){ /* ncml schema lacks a "type" relating to a VLEN so we will use the typedef instead. */ 
      (void)fprintf(fp_out,"typedef=\"%s\"",typ_nm);
    } else{
      (void)fprintf(fp_out,"typedef=\"%s\"",typ_nm); /* for future use eg compound, opaque,  sequence */      
    }
      
    if(typ_nm)
      typ_nm=(char*)NULL;    
  } /* !XML */

  if(prn_flg->jsn) (void)fprintf(fp_out,"%*s\"%s\": {\n",prn_ndn,spc_sng,var_trv->nm);

  /* Print type, shape, and total size of variable */
  /* Use nbr_dmn+1 in malloc() to handle case when dmn_nbr == 0 and allow for formatting characters */
  dmn_sng=(char *)nco_malloc((dmn_nbr+1)*NC_MAX_NAME*sizeof(char));
  dmn_sng[0]='\0';
  sz_sng[0]='\0';

  if(dmn_nbr == 0){
    if(prn_flg->trd){
      if(cls_typ == NC_VLEN) (void)fprintf(fp_out,"%*s%s size (RAM) = %ld*mean_length(NC_VLEN)*sizeof(%s) = %ld*%g*%lu = %lu bytes\n",prn_ndn,spc_sng,var_trv->nm,var_sz,nco_typ_sng(bs_typ),var_sz,vln_lng_avg,(unsigned long)nco_typ_lng_udt(nc_id,bs_typ),(unsigned long)ram_sz_crr); else (void)fprintf(fp_out,"%*s%s size (RAM) = %ld*sizeof(%s) = %ld*%lu = %lu bytes\n",prn_ndn,spc_sng,var_trv->nm,var_sz,nco_typ_sng(cls_typ),var_sz,(unsigned long)nco_typ_lng_udt(nc_id,bs_typ),(unsigned long)ram_sz_crr);
    } /* !trd */
    /* 20131122: Implement ugly NcML requirement that scalars have shape="" attribute */
    if(prn_flg->xml) (void)sprintf(dmn_sng," shape=\"%s\"",(cls_typ == NC_VLEN ? "*" : "" ));
    (void)sprintf(sng_foo,"1*");
    (void)strcat(sz_sng,sng_foo);
  }else{
    for(dmn_idx=0;dmn_idx<dmn_nbr;dmn_idx++){
      if(prn_flg->xml){
	(void)sprintf(sng_foo,"%s%s%s",(dmn_idx == 0) ? " shape=\"" : "",var_trv->var_dmn[dmn_idx].dmn_nm,(dmn_idx < dmn_nbr-1) ? " " : (cls_typ == NC_VLEN ? " *\""  : "\"")); 
      }else if(prn_flg->jsn){
        /* Indent content */ 
        nm_jsn=nm2sng_jsn(var_trv->var_dmn[dmn_idx].dmn_nm);
        if(dmn_idx==0) (void)sprintf(dmn_sng,"%*s\"shape\": [", prn_ndn+prn_flg->sxn_fst,spc_sng); 
	(void)sprintf(sng_foo,"\"%s\"%s",nm_jsn,(dmn_idx < dmn_nbr-1) ? ", " : "],"); 
	nm_jsn=(char *)nco_free(nm_jsn);
      }else{
	nm_cdl=nm2sng_cdl(var_trv->var_dmn[dmn_idx].dmn_nm);
	(void)sprintf(sng_foo,"%s%s%s",(dmn_idx == 0) ? "(" : "",nm_cdl,(dmn_idx < dmn_nbr-1) ? "," : ")");
	nm_cdl=(char *)nco_free(nm_cdl);
      } /* !xml */
      (void)strcat(dmn_sng,sng_foo);
    } /* end loop over dim */

    for(dmn_idx=0;dmn_idx<dmn_nbr;dmn_idx++){
      (void)sprintf(sng_foo,"%li*",(long)dmn_sz[dmn_idx]);
      (void)strcat(sz_sng,sng_foo);
    } /* end loop over dim */

    if(nco_fmt_xtn_get() != nco_fmt_xtn_hdf4 || (NC_LIB_VERSION >= 433 && NC_LIB_VERSION != 474)) (void)nco_inq_var_deflate(grp_id,var_id,&shuffle,&deflate,&dfl_lvl);

  } /* !scalar */

  if(cls_typ == NC_VLEN){
    (void)sprintf(sng_foo,"mean_length(NC_VLEN)*");
    (void)strcat(sz_sng,sng_foo);
  } /* !cls_typ */
  (void)sprintf(sng_foo,"sizeof(%s)",nco_typ_sng(bs_typ));
  (void)strcat(sz_sng,sng_foo);
  
  if(dmn_nbr > 0 && prn_flg->trd){
    if((nco_fmt_xtn_get() != nco_fmt_xtn_hdf4 || (NC_LIB_VERSION >= 433 && NC_LIB_VERSION != 474)) && deflate) (void)fprintf(fp_out,"%*s%s compression (Lempel-Ziv %s shuffling) level = %d\n",prn_ndn,spc_sng,var_trv->nm,(shuffle) ? "with" : "without",dfl_lvl);
    if(nco_fmt_xtn_get() == nco_fmt_xtn_hdf4 && (NC_LIB_VERSION < 433 || NC_LIB_VERSION == 474)) (void)fprintf(fp_out,"%*s%s compression and shuffling characteristics are HDF4_UNKNOWN\n",prn_ndn,spc_sng,var_trv->nm);
    if(cls_typ == NC_VLEN) (void)fprintf(fp_out,"%*s%s size (RAM) = %s = %li*%g*%lu = %lu bytes\n",prn_ndn,spc_sng,var_trv->nm,sz_sng,var_sz,vln_lng_avg,(unsigned long)nco_typ_lng_udt(nc_id,bs_typ),(unsigned long)ram_sz_crr); else (void)fprintf(fp_out,"%*s%s size (RAM) = %s = %li*%lu = %lu bytes\n",prn_ndn,spc_sng,var_trv->nm,sz_sng,var_sz,(unsigned long)nco_typ_lng_udt(nc_id,bs_typ),(unsigned long)ram_sz_crr);
  } /* !prn_flg->trd */

  if(prn_flg->cdl){
    char *typ_nm;
    typ_nm=cdl_typ_nm_udt(nc_id,var_typ);
    nm_cdl=nm2sng_cdl(var_trv->nm);
    (void)fprintf(fp_out,"%*s%s %s%s ;",prn_ndn,spc_sng,typ_nm,nm_cdl,dmn_sng);
    if(var_typ > NC_MAX_ATOMIC_TYPE) typ_nm=(char *)nco_free(typ_nm);
    if(nco_dbg_lvl_get() >= nco_dbg_std){
      if(cls_typ == NC_VLEN) (void)fprintf(fp_out," // RAM size = %s = %li*%g*%lu = %lu bytes",sz_sng,var_sz,vln_lng_avg,(unsigned long)nco_typ_lng_udt(nc_id,bs_typ),(unsigned long)ram_sz_crr); else (void)fprintf(fp_out," // RAM size = %s = %li*%lu = %lu bytes",sz_sng,var_sz,(unsigned long)nco_typ_lng_udt(nc_id,bs_typ),(unsigned long)ram_sz_crr);
    } /* !dbg */
    /* 20170913: Typically users not interested in variable ID. However, ID helps diagnose susceptibility to CDF5 bug */
    if(nco_dbg_lvl_get() >= nco_dbg_fl) (void)fprintf(fp_out,", ID = %d",var_id);
    (void)fprintf(fp_out,"\n");
    nm_cdl=(char *)nco_free(nm_cdl);
  } /* !cdl */
  if(prn_flg->xml){
    if(prn_flg->PRN_VAR_DATA || prn_flg->PRN_VAR_METADATA) (void)fprintf(fp_out,"%s>\n",dmn_sng); else (void)fprintf(fp_out,"%s />\n",dmn_sng);
  } /* !xml */

  /* Add comma as next in queue are the attributes NB: DO NOT LIKE THIS */
  if(prn_flg->jsn && prn_flg->jsn_var_fmt >= 1){
    char *typ_nm;
    
    typ_nm=cdl_typ_nm_udt(nc_id,var_typ);
    if(dmn_nbr > 0) (void)fprintf(fp_out,"%s\n",dmn_sng); 
    /* Print netCDF type with same names as XML */
    
    // (void)fprintf(fp_out,"%*s\"type\": \"%s\"",prn_ndn+prn_flg->sxn_fst,spc_sng,jsn_typ_nm(var_typ));
    (void)fprintf(fp_out,"%*s\"type\": \"%s\"",prn_ndn+prn_flg->sxn_fst,spc_sng,typ_nm);
    if(var_typ > NC_MAX_ATOMIC_TYPE) typ_nm=(char *)nco_free(typ_nm);
  } /* !xml */

  if(dmn_sng) dmn_sng=(char *)nco_free(dmn_sng);

  /* Print dimension sizes and names */
  if(prn_flg->trd){
    /* Loop over dimensions for object (variable) */
    for(dmn_idx=0;dmn_idx<var_trv->nbr_dmn;dmn_idx++){

      /* Casting from "size_t" to "unsigned long" necessary since %zu format only available in C99 */
      /* Coordinate dimension */
      if(var_trv->var_dmn[dmn_idx].is_crd_var){

        crd_sct *crd=var_trv->var_dmn[dmn_idx].crd;

        /* NOTE: Use hyperslabbed sizes for dimension size */
        if(srg_typ == NC_CHUNKED) (void)fprintf(fp_out,"%*s%s dimension %i: %s, size = %li %s, chunksize = %lu (",prn_ndn,spc_sng,var_trv->nm,dmn_idx,(!strcmp(crd->dmn_grp_nm_fll,var_trv->grp_nm_fll)) ? crd->nm : crd->dmn_nm_fll,crd->lmt_msa.dmn_cnt,nco_typ_sng(crd->var_typ),(unsigned long)cnk_sz[dmn_idx]); else (void)fprintf(fp_out,"%*s%s dimension %i: %s, size = %li %s (",prn_ndn,spc_sng,var_trv->nm,dmn_idx,(!strcmp(crd->dmn_grp_nm_fll,var_trv->grp_nm_fll)) ? crd->nm : crd->dmn_nm_fll,crd->lmt_msa.dmn_cnt,nco_typ_sng(crd->var_typ));

        (void)fprintf(fp_out,"%soordinate is %s)",(CRR_DMN_IS_REC_IN_INPUT[dmn_idx]) ? "Record c" : "C",(!strcmp(crd->crd_grp_nm_fll,var_trv->grp_nm_fll)) ? crd->nm : crd->crd_nm_fll);

        /* Non-coordinate dimension */
      }else if(!var_trv->var_dmn[dmn_idx].is_crd_var){

        dmn_trv_sct *dmn_trv=var_trv->var_dmn[dmn_idx].ncd;

        /* NOTE: Use hyperslabbed sizes for dimension size */
        if(srg_typ == NC_CHUNKED) (void)fprintf(fp_out,"%*s%s dimension %i: %s, size = %li, chunksize = %lu (",prn_ndn,spc_sng,var_trv->nm,dmn_idx,(!strcmp(dmn_trv->grp_nm_fll,var_trv->grp_nm_fll)) ? dmn_trv->nm : dmn_trv->nm_fll,dmn_trv->lmt_msa.dmn_cnt,(unsigned long)cnk_sz[dmn_idx]); else (void)fprintf(fp_out,"%*s%s dimension %i: %s, size = %li (",prn_ndn,spc_sng,var_trv->nm,dmn_idx,(!strcmp(dmn_trv->grp_nm_fll,var_trv->grp_nm_fll)) ? dmn_trv->nm : dmn_trv->nm_fll,dmn_trv->lmt_msa.dmn_cnt);
        (void)fprintf(fp_out,"%son-coordinate dimension)",(CRR_DMN_IS_REC_IN_INPUT[dmn_idx]) ? "Record n" : "N");

      } /* end non-coordinate dimension */
      (void)fprintf(fp_out,"\n"); 
    } /* end loop over dimensions */

    /* Caveat user */
    if((nc_type)var_typ == NC_STRING) (void)fprintf(fp_out,"%*s%s size (RAM) above is space required for pointers only, full size of strings is unknown until data are read\n",prn_ndn,spc_sng,var_trv->nm);
  } /* !prn_flg->trd */

  (void)fflush(fp_out);

} /* end nco_prn_var_dfn() */

void
nco_prn_var_val_trv /* [fnc] Print variable data (GTT version) */
(const int nc_id, /* I [ID] netCDF file ID */
 const prn_fmt_sct * const prn_flg, /* I [sct] Print-format information */
 const trv_sct * const var_trv, /* I [sct] Object to print (variable) */
 const trv_tbl_sct * const trv_tbl) /* I [sct] GTT (Group Traversal Table) */
{
  /* Purpose: Print variable data
     
     Descended from nco_prn_var_val_lmt(), and uses limit information contained in GTT 
     Modulo arrays: Changing subscript of first (least rapidly varying) dimension by one moves most quickly through 
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

  char *dlm_sng=NULL; /* [sng] User-specified delimiter string, if any */
  char *fmt_val=NULL; /* [sng] Format string for variable values */
  char *nm_cdl;
  char *sng_val_sng=NULL_CEWI; /* [sng] String of NC_CHAR */
  char *sng_val_sng_cpy; /* [sng] Copy of sng_val_sng to avoid cppcheck error about using sng_val_sng as both parameter and desitnation in sprintf(). NB: free() only one of these two pointers. */
  char *spr_sng=NULL; /* [sng] Output separator string */
  char *unit_sng_var=NULL_CEWI; /* [sng] Units string for variable */
  char *unit_sng_crd=NULL_CEWI; /* [sng] Units string for coordinate */ 

  char chr_val; /* [sng] Current character */

  char val_sng[NCO_ATM_SNG_LNG];
  char var_nm[NC_MAX_NAME+1]; /* [sng] Variable name (used for validation only) */ 
  char var_sng[NCO_MAX_LEN_FMT_SNG]; /* [sng] Variable string */

  dmn_sct *dim=NULL_CEWI; /* [sct] Dimension structure */

  double val_dbl;

  FILE *fp_out=prn_flg->fp_out; /* [fl] Formatted text output file handle */

  float val_flt;

  int dmn_idx; /* [idx] Counter over dimensions */
  int grp_id; /* [ID] Group ID where variable resides (passed to MSA) */

  int rcd_prn=0; /* [rcd] Return code from *printf() function */
  int prn_ndn=0; /* [nbr] Indentation for printing */
  int val_sz_byt=int_CEWI; /* [nbr] Type size */

  lmt_msa_sct **lmt_msa=NULL_CEWI; /* [sct] MSA Limits for only for variable dimensions  */          
  lmt_sct **lmt=NULL_CEWI; /* [sct] Auxiliary Limit used in MSA */

  long *dmn_sbs_dsk=NULL_CEWI; /* [nbr] Indices of hyperslab relative to original on disk */
  long *dmn_sbs_ram=NULL_CEWI; /* [nbr] Indices in hyperslab */
  long *mod_map_cnt=NULL_CEWI; /* [nbr] MSA modulo array */
  long *mod_map_rv_cnt=NULL_CEWI; /* [nbr] MSA modulo array reverse multiply */
  long *mod_map_in=NULL_CEWI; /* [nbr] MSA modulo array */
  long lmn; /* [nbr] Index to print variable data */
  long sng_lng=long_CEWI; /* [nbr] Length of NC_CHAR string */
  long sng_lngm1=long_CEWI; /* [nbr] Length minus one of NC_CHAR string */
  long var_dsk; /* [nbr] Variable index relative to disk */
  long var_szm1;

  nco_cln_typ cln_typ = cln_std;   /* calendar type - for time@units */

  const nco_bool CDL=prn_flg->cdl; /* [flg] CDL output */
  const nco_bool XML=prn_flg->xml; /* [flg] XML output */
  const nco_bool TRD=prn_flg->trd; /* [flg] Traditional output */
  const nco_bool JSN=prn_flg->jsn; /* [flg] JSON output */

  nco_bool is_mss_val=False; /* [flg] Current value is missing value */
  nco_bool flg_malloc_unit_crd=False; /* [flg] Allocated memory for coordinate units string */
  nco_bool flg_malloc_unit_var=False; /* [flg] Allocated memory for variable units string */
  nco_bool unit_cln_crd=False; /* [flg] Coordinate has calendar units */
  
  nco_string sng_val; /* [sng] Current string */

  var_sct *var=NULL_CEWI; /* [sct] Variable structure */
  var_sct *var_aux=NULL_CEWI; /* Holds variable data printed as CDL comment AFTER regular variable data */

  nc_type bs_typ;
  nc_type cls_typ;
  
  if(prn_flg->new_fmt && (CDL||TRD||JSN)) prn_ndn=prn_flg->ndn+prn_flg->var_fst;
  if(XML) prn_ndn=prn_flg->ndn;

  /* Obtain group ID where variable is located */
  (void)nco_inq_grp_full_ncid(nc_id,var_trv->grp_nm_fll,&grp_id);

  /* malloc space */
  var=(var_sct *)nco_malloc(sizeof(var_sct));

  /* Set defaults */
  (void)var_dfl_set(var);

  /* Initialize units string, overwrite later if necessary */
  unit_sng_var=&nul_chr;

  /* Get ID for requested variable */
  var->nm=(char *)strdup(var_trv->nm);
  var->nc_id=grp_id;
  (void)nco_inq_varid(grp_id,var_trv->nm,&var->id);

  /* Get type of variable (get also name and number of dimensions for validation against parameter object) */
  (void)nco_inq_var(grp_id,var->id,var_nm,&var->type,&var->nbr_dim,(int *)NULL,(int *)NULL);

  /* Ensure we have correct variable */
  //assert(var_trv->nco_typ == nco_obj_typ_var);
  assert(var->nbr_dim == var_trv->nbr_dmn);
  assert(!strcmp(var_nm,var_trv->nm));

  /* Scalars */
  if(var->nbr_dim == 0){
    var->sz=1L;
    var->val.vp=nco_malloc(nco_typ_lng_udt(nc_id,var->type));
    /* Block is critical/thread-safe for identical/distinct grp_id's */
    { /* begin potential OpenMP critical */
      (void)nco_get_var1(grp_id,var->id,0L,var->val.vp,var->type);
    } /* end potential OpenMP critical */
  }else{ /* ! Scalars */
    /* Allocate local MSA */
    lmt_msa=(lmt_msa_sct **)nco_malloc(var_trv->nbr_dmn*sizeof(lmt_msa_sct *));
    lmt=(lmt_sct **)nco_malloc(var_trv->nbr_dmn*sizeof(lmt_sct *));

    /* Copy from table to local MSA */
    (void)nco_cpy_msa_lmt(var_trv,&lmt_msa);

    /* Call super-dooper recursive routine */
    var->val.vp=nco_msa_rcr_clc((int)0,var->nbr_dim,lmt,lmt_msa,var);
  } /* ! Scalars */

  /* Refresh missing value attribute, if any */
  var->has_mss_val=nco_mss_val_get(var->nc_id,var);

  /* Only TRD and CDL need units at this stage and might have this flag set */
  if(prn_flg->PRN_DMN_UNITS) {
    int cf_var_id;
    char *cln_sng=NULL;
    var_sct *var_tmp=NULL_CEWI;
    var_sct *var_swp=NULL_CEWI;

    if(!nco_is_spc_in_cf_att(grp_id, "bounds", var->id, &cf_var_id) &&
       !nco_is_spc_in_cf_att(grp_id, "climatology", var->id, &cf_var_id))
      cf_var_id = var->id;

    unit_sng_var=nco_lmt_get_udu_att(grp_id,cf_var_id,"units");
    if(unit_sng_var && strlen(unit_sng_var)){
      flg_malloc_unit_var=True;
      cln_sng=nco_lmt_get_udu_att(grp_id, cf_var_id,"calendar");
      if(cln_sng) cln_typ=nco_cln_get_cln_typ(cln_sng); else cln_typ=cln_std;
      if(cln_sng) cln_sng=(char *)nco_free(cln_sng);
    } /* !unit_sng_var */

    /* Set default for TRD */
    if(TRD && flg_malloc_unit_var == False){
      unit_sng_var=strdup("(no units)");
      flg_malloc_unit_var=True;
    }else if(prn_flg->cdl_fmt_dt != fmt_dt_nil && flg_malloc_unit_var){

        var_tmp=nco_var_dpl(var);
        var_aux=nco_var_dpl(var);
        var_aux->val.vp=nco_free(var_aux->val.vp);

        if(var_aux->has_mss_val){
	  var_aux->mss_val.vp=nco_free(var_aux->mss_val.vp);
	  var_aux->has_mss_val=False;
	}

        nco_var_cnf_typ(NC_STRING,var_aux);

        /* NB: nco_cln_var_prs() modifies var_tmp and var_aux */
	if(nco_cln_var_prs(unit_sng_var,cln_typ,prn_flg->cdl_fmt_dt,var_tmp,var_aux) == NCO_NOERR){
	  var_swp=var;var=var_aux;var_aux=var_swp;
	}else{
	  var_aux=nco_var_free(var_aux);
	}
        if(var_tmp) var_tmp=(var_sct*)nco_var_free(var_tmp);
    }
  }else{
    flg_malloc_unit_var=False;
    unit_sng_var=&nul_chr;
  } /* !TRD */

  bs_typ=cls_typ=var->type;
  if(var->type > NC_MAX_ATOMIC_TYPE) nco_inq_user_type(nc_id,var->type,NULL,NULL,&bs_typ,NULL,&cls_typ);

  if(var->has_mss_val) val_sz_byt=nco_typ_lng_udt(nc_id,bs_typ);

  if(var->nbr_dim){ 
    /* Allocate space for dimension information */
    dim=(dmn_sct *)nco_malloc(var->nbr_dim*sizeof(dmn_sct));
    /* Ensure val.vp is NULL-initialized (and thus not inadvertently free'd) when PRN_DMN_IDX_CRD_VAL is False */
    for(int idx=0;idx<var->nbr_dim;idx++) dim[idx].val.vp=NULL;
    dmn_sbs_ram=(long *)nco_malloc(var->nbr_dim*sizeof(long));
    dmn_sbs_dsk=(long *)nco_malloc(var->nbr_dim*sizeof(long));
    mod_map_cnt=(long *)nco_malloc(var->nbr_dim*sizeof(long));
    mod_map_rv_cnt=(long *)nco_malloc(var->nbr_dim*sizeof(long));
    mod_map_in=(long *)nco_malloc(var->nbr_dim*sizeof(long));

    /* Create mod_map_in */
    for(int idx=0;idx<var->nbr_dim;idx++) mod_map_in[idx]=1L;
    for(int idx=0;idx<var->nbr_dim;idx++)
      for(int jdx=idx+1;jdx<var->nbr_dim;jdx++)
        mod_map_in[idx]*=lmt_msa[jdx]->dmn_sz_org;

    /* Create mod_map_cnt */
    for(int idx=0;idx<var->nbr_dim;idx++) mod_map_cnt[idx]=1L;
    for(int idx=0;idx<var->nbr_dim;idx++)
      for(int jdx=idx;jdx<var->nbr_dim;jdx++)
        mod_map_cnt[idx]*=lmt_msa[jdx]->dmn_cnt;

    /* Create mod_map_rv_cnt */ 
    long rsz=1L;
    for(int jdx=var->nbr_dim-1;jdx>=0;jdx--)
      mod_map_rv_cnt[jdx]=rsz*=lmt_msa[jdx]->dmn_cnt;
  } /* !var->nbr_dim */

  /* Call also initializes var.sz with final size */
  if(prn_flg->md5)
    if(prn_flg->md5->dgs) (void)nco_md5_chk(prn_flg->md5,var_nm,var->sz*nco_typ_lng_udt(nc_id,var->type),grp_id,(long *)NULL,(long *)NULL,var->val.vp);

  /* Warn if variable is packed */
  if(nco_dbg_lvl_get() > 0)
    if(nco_pck_dsk_inq(grp_id,var))
      (void)fprintf(stderr,"%s: WARNING will print packed values of variable \"%s\". Unpack first (with ncpdq -U) to see actual values.\n",nco_prg_nm_get(),var_nm);

  if(prn_flg->dlm_sng) dlm_sng=strdup(prn_flg->dlm_sng); /* [sng] User-specified delimiter string, if any */
  if(prn_flg->fmt_val) fmt_val=strdup(prn_flg->fmt_val); /* [sng] Format string for variable values */

  if(dlm_sng && TRD){
    /* Print variable with user-supplied dlm_sng (includes nbr_dmn == 0) */
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

    for(lmn=0;lmn<var->sz;lmn++){

      /* memcmp() triggers pedantic warning unless pointer arithmetic is cast to type char * */
      if(prn_flg->PRN_MSS_VAL_BLANK && var->has_mss_val){
        if(var->type == NC_STRING) is_mss_val=!strcmp(var->val.sngp[lmn],var->mss_val.sngp[0]); else is_mss_val=!memcmp((char *)var->val.vp+lmn*val_sz_byt,var->mss_val.vp,(size_t)val_sz_byt);
      } /* !PRN_MSS_VAL_BLANK */
	
      if(prn_flg->PRN_MSS_VAL_BLANK && var->has_mss_val && is_mss_val){
        if(strcmp(dlm_sng,fmt_sng_mss_val)) (void)fprintf(fp_out,fmt_sng_mss_val,mss_val_sng); else (void)fprintf(fp_out,"%s, ",mss_val_sng);
      }else{ /* !is_mss_val */
        switch(var->type){
        case NC_FLOAT: (void)fprintf(fp_out,dlm_sng,var->val.fp[lmn]); break;
        case NC_DOUBLE: (void)fprintf(fp_out,dlm_sng,var->val.dp[lmn]); break;
        case NC_SHORT: (void)fprintf(fp_out,dlm_sng,var->val.sp[lmn]); break;
        case NC_INT: (void)fprintf(fp_out,dlm_sng,var->val.ip[lmn]); break;
        case NC_CHAR: (void)fprintf(fp_out,dlm_sng,var->val.cp[lmn]); break;
        case NC_BYTE: (void)fprintf(fp_out,dlm_sng,var->val.bp[lmn]); break;
        case NC_UBYTE: (void)fprintf(fp_out,dlm_sng,var->val.ubp[lmn]); break;
        case NC_USHORT: (void)fprintf(fp_out,dlm_sng,var->val.usp[lmn]); break;
        case NC_UINT: (void)fprintf(fp_out,dlm_sng,var->val.uip[lmn]); break;
        case NC_INT64: (void)fprintf(fp_out,dlm_sng,var->val.i64p[lmn]); break;
        case NC_UINT64: (void)fprintf(fp_out,dlm_sng,var->val.ui64p[lmn]); break;
        case NC_STRING: (void)fprintf(fp_out,dlm_sng,var->val.sngp[lmn]); break;
        default: nco_dfl_case_nc_type_err(); break;
        } /* end switch */
      } /* !is_mss_val */
    } /* end loop over element */
    (void)fprintf(fp_out,"\n");

    if(fmt_sng_mss_val) fmt_sng_mss_val=(char *)nco_free(fmt_sng_mss_val);
  } /* !dlm_sng */

  spr_sng=cma_sng; /* [sng] Output separator string */
  if(CDL || JSN || XML){
    char * (*chr2sng_sf)(const char chr_val, /* I [chr] Character to process */
			 char * const val_sng); /* I/O [sng] String to stuff printable result into */
    char fmt_sng[NCO_MAX_LEN_FMT_SNG];
    dmn_trv_sct *dmn_trv; /* [sct] Unique dimension object */
    long chr_idx;

    if(CDL){     
      chr2sng_sf=chr2sng_cdl;
      if(fmt_val && (bs_typ == NC_FLOAT || bs_typ == NC_DOUBLE)) (void)sprintf(fmt_sng,"%s",fmt_val); else (void)sprintf(fmt_sng,"%s",nco_typ_fmt_sng_var_cdl(bs_typ));
    } /* !CDL */
    if(XML){     
      chr2sng_sf=chr2sng_xml;
      if(fmt_val && (bs_typ == NC_FLOAT || bs_typ == NC_DOUBLE)) (void)sprintf(fmt_sng,"%s",fmt_val); else (void)sprintf(fmt_sng,"%s",nco_typ_fmt_sng_att_xml(bs_typ));
    } /* !XML */
    if(JSN){
      chr2sng_sf=chr2sng_jsn;
      if(fmt_val && (bs_typ == NC_FLOAT || bs_typ == NC_DOUBLE)) (void)sprintf(fmt_sng,"%s",fmt_val); else (void)sprintf(fmt_sng,"%s",nco_typ_fmt_sng_att_jsn(bs_typ));
   
      (void)fprintf(fp_out,"%*s\"data\": ",prn_ndn,spc_sng);

      /* If false then print only one set of braces even when nbr_dim > 1 */
      if(prn_flg->jsn_data_brk == False)
        for(dmn_idx=1;dmn_idx<var->nbr_dim;dmn_idx++)
	  mod_map_rv_cnt[dmn_idx]=0L;

      /* Switch-off final level braces for NC_CHAR as string is already quoted */      
      if(var->nbr_dim >= 1 && bs_typ == NC_CHAR) mod_map_rv_cnt[var->nbr_dim-1]=0L;
    } /* !JSN */

    nm_cdl=nm2sng_cdl(var_nm);

    if(XML){
      /* User may override default separator string for XML only */
      if(var->type == NC_STRING || var->type == NC_CHAR) spr_sng= (prn_flg->spr_chr) ? prn_flg->spr_chr : spr_xml_chr; else spr_sng= (prn_flg->spr_nmr) ? prn_flg->spr_nmr : spr_xml_nmr;

      (void)fprintf(fp_out,"%*s<ncml:values",prn_ndn+prn_flg->var_fst,spc_sng);
      /* Print non-whitespace separators between elements */
      if((var->sz == 1L && var->type == NC_STRING) || var->sz > 1L){
	/* Ensure string variable value does not contain separator string */
	if(var->type == NC_CHAR)
	  if(strstr(var->val.cp,spr_sng)) spr_sng=spr_xml_chr_bck;
	if(var->type == NC_STRING){
	  for(lmn=0;lmn<var->sz;lmn++){
	    if(strstr(var->val.sngp[lmn],spr_sng)){
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
	if(spr_sng_idx < spr_sng_lng) (void)fprintf(fp_out," separator=\"%s\"",spr_sng);
	if(var->type == NC_CHAR && var->nbr_dim > 1 && FIRST_WARNING && nco_dbg_lvl_get() > 0){
	  /* 20131122: Warnings about XML ambiguities caused by spr_sng appearing in val_sng would go here
	     New procedure to pre-check strings above obviates need for this in all all cases except one:
	     Multi-dimensional NC_CHAR variables may have embedded NULs that prevent strstr(val,spr_sng)
	     appearances of spr_sng after first NUL. Could use GNU-specific memmem() instead?
	     Balance of simplicity and readability suggests warning at most once */
	  (void)fprintf(stderr,"%s: WARNING %s converting to NcML multi-dimensional variable %s, presumably an array of strings of type %s, with NcML separator \"%s\". NCO performs precautionary checks with strstr(val,spr) to identify presence of separator string (spr) in data (val) and, if it detects a match, automatically switches to a backup separator string (\"%s\"). However limitations of strstr() may lead to false negatives when separator string occurs in data beyond the first string in multi-dimensional NC_CHAR arrays. Hence, result may be ambiguous to NcML parsers. HINT: If problems arise, use --xml_spr_chr to specify a multi-character separator that 1. does not appear in the string array and 2. does not include an NcML formatting characters (e.g., commas, angles, quotes). This warning is printed at most once per file.\n",nco_prg_nm_get(),fnc_nm,var->nm,nco_typ_sng(var->type),spr_sng,spr_xml_chr_bck);
	  FIRST_WARNING=False;
	} /* endif WARNING */
      } /* var.sz */
      (void)fprintf(fp_out,">");
    } /* !xml */
    if(CDL){
      (void)fprintf(fp_out,"%*s%s = ",prn_ndn,spc_sng,nm_cdl);
      /* Pretty printing */
      if(var->nbr_dim >= 2) (void)fprintf(fp_out,"\n%*s",prn_ndn,spc_sng);
    } /* !CDL */
    nm_cdl=(char *)nco_free(nm_cdl);
    var_szm1=var->sz-1L;

    /* Pre-compute elements that need brace punctuation */
    if(CDL && var->nbr_dim >= 1){
      mod_map_rv_cnt[0]=0L;
      /* Create brace list here we simply modify mod_map_rv_cnt[idx]
	 If dimension is NOT unlimited we set mod_map_rv_cnt[idx] to zero */
      for(dmn_idx=1;dmn_idx<var->nbr_dim;dmn_idx++){ /* NB: dimension index starts at 1 */
        dmn_trv=nco_dmn_trv_sct(var_trv->var_dmn[dmn_idx].dmn_id,trv_tbl);
        if(dmn_trv->is_rec_dmn == False) mod_map_rv_cnt[dmn_idx]=0L;
      } /* !dmn_idx */
    } /* !CDL */

    char *mbr_nm_cdl;
    char mbr_nm[NC_MAX_NAME+1L]; /* [sng] Member name */
    long long mbr_val;

    double *vln_val_dp;
    float *vln_val_fp;
    nco_int *vln_val_ip;
    nco_short *vln_val_sp;
    nco_byte *vln_val_bp;
    nco_ubyte *vln_val_ubp;
    nco_ushort *vln_val_usp;
    nco_uint *vln_val_uip;
    nco_int64 *vln_val_i64p;
    nco_uint64 *vln_val_ui64p;
    nco_vlen vln_val;
    size_t vln_idx;
    size_t vln_lng;
    size_t vln_lngm1;

    /* 20180604 Catenate %s to VLEN types that do not yet check for missing values */
    if((cls_typ == NC_VLEN) && (bs_typ != NC_FLOAT) && (bs_typ != NC_DOUBLE) && (bs_typ != NC_INT)) strcat(fmt_sng,"%s");

    for(lmn=0;lmn<var->sz;lmn++){

      if(cls_typ == NC_VLEN){
	vln_val=var->val.vlnp[lmn];
    	vln_lng=vln_val.len;
	vln_lngm1=vln_lng-1UL;
	
	vln_val_fp=(float *)vln_val.p;
	vln_val_dp=(double *)vln_val.p;
	vln_val_ip=(nco_int *)vln_val.p;
	vln_val_sp=(nco_short *)vln_val.p;
	vln_val_bp=(nco_byte *)vln_val.p;
	vln_val_ubp=(nco_ubyte *)vln_val.p;
	vln_val_usp=(nco_ushort *)vln_val.p;
	vln_val_uip=(nco_uint *)vln_val.p;
	vln_val_i64p=(nco_int64 *)vln_val.p;
	vln_val_ui64p=(nco_uint64 *)vln_val.p;
      } /* !cls_typ */
      
      /* Bracket data if specified */
      if(JSN || CDL)
        for(int bdz=0;bdz<var->nbr_dim;bdz++)
          if(mod_map_rv_cnt[bdz] && lmn % mod_map_rv_cnt[bdz] == 0)
	    (void)fprintf(fp_out,"%c",(JSN ? '[' : '{' ));

      is_mss_val=False;
      if(prn_flg->PRN_MSS_VAL_BLANK && var->has_mss_val){
        if(var->type == NC_STRING) is_mss_val=!strcmp(var->val.sngp[lmn],var->mss_val.sngp[0]);
	  /* In regular CDL format for NC_CHAR if _FillValue is NOT '\0' then the char is printed as is */
	else if(var->type == NC_CHAR) is_mss_val=False;
	/* memcmp() triggers pedantic warning unless pointer arithmetic is cast to type char * */
	else is_mss_val=!memcmp((char *)var->val.vp+lmn*val_sz_byt,var->mss_val.vp,(size_t)val_sz_byt);
	/* fxm: 20180604: NC_ENUM missing value test fails with above */
	/* fxm: 20180604: NC_VLEN missing value test must be moved here (into a vln loop) so we can avoid printing braces around underscore */
      } /* !PRN_MSS_VAL_BLANK */
	
      if(is_mss_val){
        (void)sprintf(val_sng,"%s",mss_val_sng);
      }else{ /* !is_mss_val */
        switch(cls_typ){
        case NC_FLOAT: 
          val_flt=var->val.fp[lmn];
          if(isfinite(val_flt)){
            rcd_prn=snprintf(val_sng,(size_t)NCO_ATM_SNG_LNG,fmt_sng,val_flt);
            (void)sng_trm_trl_zro(val_sng,prn_flg->nbr_zro);
          }else{
            (void)nco_prn_nonfinite_flt(val_sng,prn_flg,val_flt);
          } /* !isfinite() */
          break;
        case NC_DOUBLE:
          val_dbl=var->val.dp[lmn];
          if(isfinite(val_dbl)){
            rcd_prn=snprintf(val_sng,(size_t)NCO_ATM_SNG_LNG,fmt_sng,val_dbl);
            (void)sng_trm_trl_zro(val_sng,prn_flg->nbr_zro);
          }else{
            (void)nco_prn_nonfinite_dbl(val_sng,prn_flg,val_dbl);
          } /* !isfinite() */
          break;
        case NC_SHORT: (void)sprintf(val_sng,fmt_sng,var->val.sp[lmn]); break;
        case NC_INT: (void)sprintf(val_sng,fmt_sng,var->val.ip[lmn]); break;
        case NC_CHAR: 
          chr_val=var->val.cp[lmn];
          if(var->nbr_dim == 0){
            if(CDL||TRD||JSN) (void)fprintf(fp_out,"\"");
	    /* ncdump prints NUL as "0" but NCO prints NUL as "" for all formats to disambiguate from zero */
            if(chr_val != 0) (void)fprintf(fp_out,"%s",(*chr2sng_sf)(chr_val,val_sng));
            if(CDL||TRD||JSN) (void)fprintf(fp_out,"\"");
            val_sng[0]='\0'; /* Re-initialize with NUL byte to be safe */
          }else{ /* var.nbr_dim > 0 */
            /* Multi-dimensional string arrays of NC_CHAR */
            val_sng[0]='\0';
            if(lmn == 0L){
              sng_lng=lmt_msa[var->nbr_dim-1]->dmn_cnt;
              sng_lngm1=sng_lng-1UL;
              /* Worst case is printable strings are four times longer than unformatted, i.e. '\\' == "\\\\" */
              sng_val_sng_cpy=sng_val_sng=(char *)nco_malloc(4*sng_lng+1UL);
            } /* endif first element of string array */
            /* New string begins each element where penultimate dimension changes */
            if(lmn%sng_lng == 0L){
              if(CDL||JSN) (void)fprintf(fp_out,"\"");
              sng_val_sng[0]='\0';
            } /* endif new string */
            if(chr_val != '\0') (void)fprintf(fp_out,"%s",(*chr2sng_sf)(chr_val,val_sng));
            if(chr_val == '\n' && lmn != var_szm1) (void)sprintf(sng_val_sng,"%s\",\n%*s\"",sng_val_sng_cpy,prn_ndn+prn_flg->var_fst,spc_sng);
            if(lmn%sng_lng == sng_lngm1) (void)fprintf(fp_out,"%s%s",sng_val_sng,(CDL||JSN) ? "\"" : "");
            if(lmn == var_szm1) sng_val_sng=(char *)nco_free(sng_val_sng);
          } /* var.nbr_dim > 0 */
          break;
        case NC_BYTE: (void)sprintf(val_sng,fmt_sng,var->val.bp[lmn]); break;
        case NC_UBYTE: (void)sprintf(val_sng,fmt_sng,var->val.ubp[lmn]); break;
        case NC_USHORT: (void)sprintf(val_sng,fmt_sng,var->val.usp[lmn]); break;
        case NC_UINT: (void)sprintf(val_sng,fmt_sng,var->val.uip[lmn]); break;
        case NC_INT64: (void)sprintf(val_sng,fmt_sng,var->val.i64p[lmn]); break;
        case NC_UINT64: (void)sprintf(val_sng,fmt_sng,var->val.ui64p[lmn]); break;
        case NC_STRING: 
          sng_val=var->val.sngp[lmn];
          sng_lng=strlen(sng_val);
          sng_lngm1=sng_lng-1UL;
          /* Worst case is printable strings are six or four times longer than unformatted, i.e., '\"' == "&quot;" or '\\' == "\\\\" */
          sng_val_sng=(char *)nco_malloc(6*sng_lng+1UL);
          if(CDL||JSN) (void)fprintf(fp_out,"\"");
          sng_val_sng[0]='\0';
          for(chr_idx=0;chr_idx<sng_lng;chr_idx++){
            val_sng[0]='\0';
            chr_val=sng_val[chr_idx];
	    (void)strcat(sng_val_sng,(*chr2sng_sf)(chr_val,val_sng));
          } /* end loop over character */
          (void)fprintf(fp_out,"%s%s",sng_val_sng,(XML) ? "" : "\"");
          sng_val_sng=(char *)nco_free(sng_val_sng);
          break;
	case NC_VLEN:
	  if(CDL) (void)fprintf(fp_out,"{");
	  else if(JSN) (void)fprintf(fp_out,"[");
	  else if(XML) (void)fprintf(fp_out,",");

	  switch(bs_typ){
	  case NC_FLOAT:
	    for(vln_idx=0;vln_idx<vln_lng;vln_idx++){
	      is_mss_val=False;
	      if(prn_flg->PRN_MSS_VAL_BLANK && var->has_mss_val){
		// (void)fprintf(fp_out,"quark2: val_sz_byt = %d, vln_val_fp+vln_idx*val_sz_byt = %g, var->mss_val.fp = %g\n",val_sz_byt,vln_val_fp[vln_idx],var->mss_val.fp[0]);
		/* memcmp() triggers pedantic warning unless pointer arithmetic is cast to type char * */
		is_mss_val=!memcmp((char *)vln_val_fp+vln_idx*val_sz_byt,var->mss_val.vp,(size_t)val_sz_byt);
	      } /* !prn_flg */
	      if(is_mss_val){
		(void)sprintf(val_sng,"%s",mss_val_sng);
	      }else{
		val_flt=vln_val_fp[vln_idx];
		if(isfinite(val_flt)){
		  rcd_prn=snprintf(val_sng,(size_t)NCO_ATM_SNG_LNG,fmt_sng,val_flt);
		  (void)sng_trm_trl_zro(val_sng,prn_flg->nbr_zro);
		}else{
                  (void)nco_prn_nonfinite_flt(val_sng,prn_flg, val_flt);
		} /* endelse */
	      } /* !is_mss_val */
	      (void)fprintf(fp_out,"%s%s",val_sng,(vln_idx != vln_lngm1) ? spr_sng : "");
	    } /* !vln_idx */
	    break;
	  case NC_DOUBLE:
	    for(vln_idx=0;vln_idx<vln_lng;vln_idx++){
	      val_dbl=vln_val_dp[vln_idx];
	      if(isfinite(val_dbl)){
		rcd_prn=snprintf(val_sng,(size_t)NCO_ATM_SNG_LNG,fmt_sng,val_dbl);
		(void)sng_trm_trl_zro(val_sng,prn_flg->nbr_zro);
	      }else{
                (void)nco_prn_nonfinite_dbl(val_sng, prn_flg, val_dbl);
	      } /* endelse */
	      (void)fprintf(fp_out,"%s%s",val_sng,(vln_idx != vln_lngm1) ? spr_sng : "");
	    } /* !vln_idx */
	    break;
	    //	  case NC_INT: for(vln_idx=0;vln_idx<vln_lng;vln_idx++) (void)fprintf(fp_out,fmt_sng,(long)vln_val_ip[vln_idx],(vln_idx != vln_lngm1) ? spr_sng : ""); break;
	    // 20180603: Handle VLEN NC_INT _FillValue:
	  case NC_INT:
	    for(vln_idx=0;vln_idx<vln_lng;vln_idx++){
	      is_mss_val=False;
	      if(prn_flg->PRN_MSS_VAL_BLANK && var->has_mss_val) is_mss_val=!memcmp((char *)vln_val_ip+vln_idx*val_sz_byt,var->mss_val.vp,(size_t)val_sz_byt);
	      if(is_mss_val) (void)sprintf(val_sng,"%s",mss_val_sng); else rcd_prn=snprintf(val_sng,(size_t)NCO_ATM_SNG_LNG,fmt_sng,(long)vln_val_ip[vln_idx]);
	      (void)fprintf(fp_out,"%s%s",val_sng,(vln_idx != vln_lngm1) ? spr_sng : "");
	    } /* !vln_idx */
	    break;
	  case NC_SHORT: for(vln_idx=0;vln_idx<vln_lng;vln_idx++) (void)fprintf(fp_out,fmt_sng,vln_val_sp[vln_idx],(vln_idx != vln_lngm1) ? spr_sng : ""); break;
	  case NC_CHAR:
	    break;
	  case NC_BYTE: for(vln_idx=0;vln_idx<vln_lng;vln_idx++) (void)fprintf(fp_out,fmt_sng,vln_val_bp[vln_idx],(vln_idx != vln_lngm1) ? spr_sng : ""); break;
	  case NC_UBYTE: for(vln_idx=0;vln_idx<vln_lng;vln_idx++) (void)fprintf(fp_out,fmt_sng,vln_val_ubp[vln_idx],(vln_idx != vln_lngm1) ? spr_sng : ""); break;
	  case NC_USHORT: for(vln_idx=0;vln_idx<vln_lng;vln_idx++) (void)fprintf(fp_out,fmt_sng,vln_val_usp[vln_idx],(vln_idx != vln_lngm1) ? spr_sng : ""); break;
	  case NC_UINT: for(vln_idx=0;vln_idx<vln_lng;vln_idx++) (void)fprintf(fp_out,fmt_sng,vln_val_uip[vln_idx],(vln_idx != vln_lngm1) ? spr_sng : ""); break;
	  case NC_INT64: for(vln_idx=0;vln_idx<vln_lng;vln_idx++) (void)fprintf(fp_out,fmt_sng,vln_val_i64p[vln_idx],(vln_idx != vln_lngm1) ? spr_sng : ""); break;
	  case NC_UINT64: for(vln_idx=0;vln_idx<vln_lng;vln_idx++) (void)fprintf(fp_out,fmt_sng,vln_val_ui64p[vln_idx],(vln_idx != vln_lngm1) ? spr_sng : ""); break;
	  case NC_STRING:
	    break;
	  default: nco_dfl_case_nc_type_err(); break;
	  } /* !bs_typ switch */
	  
          if(CDL) (void)fprintf(fp_out,"}");
	  else if(JSN) (void)fprintf(fp_out,"]");
	  
	  break; /* !NC_VLEN */
	case NC_ENUM:
	  switch(bs_typ){
	  case NC_BYTE: mbr_val=(long long)var->val.bp[lmn]; break;
	  case NC_UBYTE: mbr_val=(long long)var->val.ubp[lmn]; break;
	  case NC_SHORT: mbr_val=(long long)var->val.sp[lmn]; break;
	  case NC_USHORT: mbr_val=(long long)var->val.usp[lmn]; break;
	  case NC_INT: mbr_val=(long long)var->val.ip[lmn]; break;
	  case NC_UINT: mbr_val=(long long)var->val.uip[lmn]; break;
	  case NC_INT64: mbr_val=(long long)var->val.i64p[lmn]; break;
	  case NC_UINT64: mbr_val=(long long)var->val.ui64p[lmn]; break;
	  default: nco_dfl_case_nc_type_err(); break;
	  } /* !bs_typ switch */
	  nco_inq_enum_ident(nc_id,var->type,mbr_val,mbr_nm);
	  mbr_nm_cdl=nm2sng_cdl(mbr_nm);
	  if(JSN) (void)fprintf(fp_out,"\"%s\"",mbr_nm_cdl); else (void)fprintf(fp_out,"%s",mbr_nm_cdl);
	  mbr_nm_cdl=(char *)nco_free(mbr_nm_cdl);
	  break; /* !NC_ENUM */
	case NC_COMPOUND:
	case NC_OPAQUE:
	default: nco_dfl_case_nc_type_err();
	  break;
	} /* !cls_typ switch */
      } /* !is_mss_val */
      
      if((cls_typ <= NC_MAX_ATOMIC_TYPE && var->type != NC_CHAR && var->type != NC_STRING) || (var->type == NC_STRING && is_mss_val) || (cls_typ == NC_ENUM && is_mss_val)) (void)fprintf(fp_out,"%s",val_sng);

      /* Bracket data if specified */
      if(JSN || CDL)
        for(int bdz=0;bdz<var->nbr_dim;bdz++)
          if(mod_map_rv_cnt[bdz] && (lmn+1) % mod_map_rv_cnt[bdz] == 0)
	    (void)fprintf(fp_out,"%c",(JSN ? ']' : '}'));

      /* Print separator after non-final string */
      if(lmn != var_szm1)
	if((var->type == NC_CHAR && lmn%sng_lng == sng_lngm1) || (var->type != NC_CHAR ))
          (void)fprintf(fp_out,"%s",spr_sng);

      /* Pretty printing */
      if(CDL && var->nbr_dim && lmn< var_szm1 && (lmn+1L) % lmt_msa[var->nbr_dim-1]->dmn_cnt == 0) (void)fprintf(fp_out,"\n%*s",prn_ndn,spc_sng);
    } /* !lmn */
    if(cls_typ == NC_VLEN) nco_free_vlens(var->sz,var->val.vlnp);

    rcd_prn+=0; /* CEWI */

    if(CDL){
      char tmp_sng[100]={0};
      if(nco_dbg_lvl_get() >= nco_dbg_std && flg_malloc_unit_var) (void)sprintf(tmp_sng,"units=\"%s\"",unit_sng_var);
      if(nco_dbg_lvl_get() == nco_dbg_std && var_aux){
        (void)fprintf(fp_out,"; // %s  ",tmp_sng);
        /* Print values as CDL text comment */
        nco_prn_var_val_cmt(var_aux,prn_flg);
      }else if(tmp_sng[0]){
        (void)fprintf(fp_out," ; // %s\n",tmp_sng);
      }else{
        (void)fprintf(fp_out," ;\n");
      } /* !dbg */
    } /* !CDL */

    if(XML) (void)fprintf(fp_out,"</ncml:values>\n");
  } /* end if CDL_OR_JSN_OR_XML */

  if(var->nbr_dim == 0 && !dlm_sng && TRD){
    /* Variable is scalar, byte, or character */
    lmn=0L;
    if(prn_flg->PRN_MSS_VAL_BLANK) is_mss_val = var->has_mss_val ? !memcmp(var->val.vp,var->mss_val.vp,(size_t)val_sz_byt) : False;
    if(prn_flg->PRN_DMN_VAR_NM) (void)sprintf(var_sng,"%*s%%s = %s %%s\n",prn_ndn,spc_sng,nco_typ_fmt_sng(var->type)); else (void)sprintf(var_sng,"%*s%s\n",prn_ndn,spc_sng,nco_typ_fmt_sng(var->type));
    if(prn_flg->PRN_MSS_VAL_BLANK && is_mss_val){
      if(prn_flg->PRN_DMN_VAR_NM) (void)fprintf(fp_out,"%*s%s = %s %s\n",prn_ndn,spc_sng,var_nm,mss_val_sng,unit_sng_var); else (void)fprintf(fp_out,"%*s%s\n",prn_ndn,spc_sng,mss_val_sng);
    }else{ /* !is_mss_val */
      if(prn_flg->PRN_DMN_VAR_NM){
        switch(var->type){
        case NC_FLOAT: (void)fprintf(fp_out,var_sng,var_nm,var->val.fp[lmn],unit_sng_var); break;
        case NC_DOUBLE: (void)fprintf(fp_out,var_sng,var_nm,var->val.dp[lmn],unit_sng_var); break;
        case NC_SHORT: (void)fprintf(fp_out,var_sng,var_nm,var->val.sp[lmn],unit_sng_var); break;
        case NC_INT: (void)fprintf(fp_out,var_sng,var_nm,var->val.ip[lmn],unit_sng_var); break;
        case NC_CHAR:
          if(var->val.cp[lmn] != '\0'){
            (void)sprintf(var_sng,"%*s%%s = '%s' %%s\n",prn_ndn,spc_sng,nco_typ_fmt_sng(var->type));
            (void)fprintf(fp_out,var_sng,var_nm,var->val.cp[lmn],unit_sng_var);
          }else{ /* 20171220 Deal with NUL character here, otherwise NUL will print as '0' (which ncdump does, though we think that is a bug) */
            (void)fprintf(fp_out,"%*s%s = \"\" %s\n",prn_ndn,spc_sng,var_nm,unit_sng_var);
          } /* end if */
          break;
        case NC_BYTE: (void)fprintf(fp_out,var_sng,var_nm,(unsigned char)var->val.bp[lmn],unit_sng_var); break;
        case NC_UBYTE: (void)fprintf(fp_out,var_sng,var_nm,var->val.ubp[lmn],unit_sng_var); break;
        case NC_USHORT: (void)fprintf(fp_out,var_sng,var_nm,var->val.usp[lmn],unit_sng_var); break;
        case NC_UINT: (void)fprintf(fp_out,var_sng,var_nm,var->val.uip[lmn],unit_sng_var); break;
        case NC_INT64: (void)fprintf(fp_out,var_sng,var_nm,var->val.i64p[lmn],unit_sng_var); break;
        case NC_UINT64: (void)fprintf(fp_out,var_sng,var_nm,var->val.ui64p[lmn],unit_sng_var); break;
        case NC_STRING: (void)fprintf(fp_out,var_sng,var_nm,var->val.sngp[lmn],unit_sng_var); break;
        case NC_VLEN: (void)fprintf(fp_out,var_sng,var_nm,var->val.ip[lmn],unit_sng_var); break;
        default: nco_dfl_case_nc_type_err(); break;
        } /* end switch */
      }else{ /* !PRN_DMN_VAR_NM */
        switch(var->type){
        case NC_FLOAT: (void)fprintf(fp_out,var_sng,var->val.fp[lmn]); break;
        case NC_DOUBLE: (void)fprintf(fp_out,var_sng,var->val.dp[lmn]); break;
        case NC_SHORT: (void)fprintf(fp_out,var_sng,var->val.sp[lmn]); break;
        case NC_INT: (void)fprintf(fp_out,var_sng,var->val.ip[lmn]); break;
        case NC_CHAR:
          if(var->val.cp[lmn] != '\0'){
            (void)sprintf(var_sng,"'%s'\n",nco_typ_fmt_sng(var->type));
            (void)fprintf(fp_out,var_sng,var->val.cp[lmn]);
          }else{ /* 20171220 Deal with NUL character here, otherwise NUL will print as '0' (which ncdump does, though we think that is a bug) */
            (void)fprintf(fp_out, "\"\"\n");
          } /* end if */
          break;
        case NC_BYTE: (void)fprintf(fp_out,var_sng,(unsigned char)var->val.bp[lmn]); break;
        case NC_UBYTE: (void)fprintf(fp_out,var_sng,var->val.ubp[lmn]); break;
        case NC_USHORT: (void)fprintf(fp_out,var_sng,var->val.usp[lmn]); break;
        case NC_UINT: (void)fprintf(fp_out,var_sng,var->val.uip[lmn]); break;
        case NC_INT64: (void)fprintf(fp_out,var_sng,var->val.i64p[lmn]); break;
        case NC_UINT64: (void)fprintf(fp_out,var_sng,var->val.ui64p[lmn]); break;
        case NC_STRING: (void)fprintf(fp_out,var_sng,var->val.sngp[lmn]); break;
        case NC_VLEN: (void)fprintf(fp_out,var_sng,var->val.ip[lmn]); break;
        default: nco_dfl_case_nc_type_err(); break;
        } /* end switch */
      } /* !PRN_DMN_VAR_NM */
    } /* !is_mss_val */
  } /* end if variable is scalar, byte, or character */

  if(var->nbr_dim > 0 && !dlm_sng && TRD){

    /* Read coordinate dimensions if required */
    if(prn_flg->PRN_DMN_IDX_CRD_VAL){

      for(int idx=0;idx<var_trv->nbr_dmn;idx++){

        assert(!strcmp(lmt_msa[idx]->dmn_nm,var_trv->var_dmn[idx].dmn_nm));

        if(nco_dbg_lvl_get() == nco_dbg_old) (void)fprintf(stdout,"%s: DEBUG %s reading %s dimension %d, %s",nco_prg_nm_get(),fnc_nm,var_trv->nm_fll,idx,var_trv->var_dmn[idx].dmn_nm_fll);

	dim[idx].val.vp=NULL;
        dim[idx].nm=lmt_msa[idx]->dmn_nm;

        /* Dimension does not have coordinate variable, do not read... */
        if(!var_trv->var_dmn[idx].is_crd_var){
          dim[idx].is_crd_dmn=False;
          dim[idx].cid=-1;
          continue;
        }else if(var_trv->var_dmn[idx].is_crd_var){
          /* Dimension is a coordinate */
          var_sct var_crd;
          /* Get coordinate from table */
          crd_sct *crd=var_trv->var_dmn[idx].crd;

          /* Obtain group ID */
          (void)nco_inq_grp_full_ncid(nc_id,crd->crd_grp_nm_fll,&var_crd.nc_id);

          /* Obtain variable ID and name */
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

	  /* Always check to see if coordinate is calendar date */
	  if(True){
	    const char units_nm[]="units"; /* [sng] Name of units attribute */
	    int rcd_lcl; /* [rcd] Return code */
	    int att_id; /* [id] Attribute ID */
	    long att_sz;
	    nc_type att_typ;
	    
	    /* Does coordinate have character attribute named units_nm? */
	    rcd_lcl=nco_inq_attid_flg(grp_id,var->id,units_nm,&att_id);
	    if(rcd_lcl == NC_NOERR){
	      (void)nco_inq_att(grp_id,var->id,units_nm,&att_typ,&att_sz);
	      if(att_typ == NC_CHAR){
		unit_sng_crd=(char *)nco_malloc((att_sz+1L)*nco_typ_lng(att_typ));
		(void)nco_get_att(grp_id,var->id,units_nm,unit_sng_crd,att_typ);
		unit_sng_crd[(att_sz+1L)*nco_typ_lng(att_typ)-1L]='\0';
		flg_malloc_unit_crd=True;
		
		/* Are units those of a calendar? */
		unit_cln_crd=nco_cln_chk_tm(unit_sng_crd);
		if(nco_dbg_lvl_get() == nco_dbg_crr) (void)fprintf(stdout,"%s: INFO %s reports units string \"%s\" is %sa calendar string\n",nco_prg_nm_get(),fnc_nm,unit_sng_crd,unit_cln_crd ? "" : "not " );
		if(unit_cln_crd){
#ifdef HAVE_UDUNITS2_H
		/* 20160418: Even when UDUnits is available, the lgb software produces poor formatting and needs work so turn-off for now */
		  unit_cln_crd=False;
#else /* !HAVE_UDUNITS2 */
		  if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stdout,"%s: INFO %s reports units string \"%s\" is a calendar string, but UDUNITS2 not built-in, so will not attempt to print calendar dates legibly\n",nco_prg_nm_get(),fnc_nm,unit_sng_crd);
		  unit_cln_crd=False;
#endif /* !HAVE_UDUNITS2 */
                } /* !unit_cln_crd */
	      } /* end if att_typ */
	    } /* end if rcd_lcl */
	  } /* end if True */
	  
        } /* end if is_crd_var */

        /* Typecast pointer before use */  
        (void)cast_void_nctype(dim[idx].type,&dim[idx].val);
      } /* end for */
    } /* end if */

    for(lmn=0;lmn<var->sz;lmn++){

      /* memcmp() triggers pedantic warning unless pointer arithmetic is cast to type char * */
      if(prn_flg->PRN_MSS_VAL_BLANK) is_mss_val = var->has_mss_val ? !memcmp((char *)var->val.vp+lmn*val_sz_byt,var->mss_val.vp,(size_t)val_sz_byt) : False;

      /* Calculate RAM indices from current limit */
      for(int idx=0;idx<var->nbr_dim;idx++)
        dmn_sbs_ram[idx]=(lmn%mod_map_cnt[idx])/(idx == var->nbr_dim-1 ? 1L : mod_map_cnt[idx+1]);

      /* Calculate disk indices from RAM indices */
      (void)nco_msa_ram_2_dsk(dmn_sbs_ram,lmt_msa,var->nbr_dim,dmn_sbs_dsk,(lmn == var->sz-1L));

      /* Find variable index relative to disk */
      var_dsk=0;
      for(int idx=0;idx<var->nbr_dim;idx++) var_dsk+=dmn_sbs_dsk[idx]*mod_map_in[idx];

      /* Skip rest of loop unless element is first in string */
      if(var->type == NC_CHAR && dmn_sbs_ram[var->nbr_dim-1] > 0) goto lbl_chr_prn;

      /* Print dimensions with indices along with values if they are coordinate variables */
      if(prn_flg->PRN_DMN_IDX_CRD_VAL){
        long dmn_sbs_prn;
        long crd_idx_crr;
        char dmn_sng[NCO_MAX_LEN_FMT_SNG];

        /* Loop over dimensions whose coordinates are to be printed */
        for(int idx=0;idx<var_trv->nbr_dmn;idx++){

          /* Reverse dimension ordering for Fortran convention */
          if(prn_flg->FORTRAN_IDX_CNV) dmn_idx=var->nbr_dim-1-idx; else dmn_idx=idx;

          /* Format and print dimension part of output string for non-coordinate variables */

          /* If variable is a coordinate then skip printing until later */
          if(var_trv->is_crd_var) continue;

          if(!dim[dmn_idx].is_crd_dmn){ /* If dimension is not a coordinate... */
            if(prn_flg->PRN_DMN_VAR_NM){
              if(prn_flg->FORTRAN_IDX_CNV) (void)fprintf(fp_out,"%*s%s(%ld) ",(idx == 0) ? prn_ndn : 0,spc_sng,dim[dmn_idx].nm,dmn_sbs_dsk[dmn_idx]+1L); else (void)fprintf(fp_out,"%*s%s[%ld] ",(idx == 0) ? prn_ndn : 0,spc_sng,dim[dmn_idx].nm,dmn_sbs_dsk[dmn_idx]);
            } /* !PRN_DMN_VAR_NM */
            continue;
          } /* end if */

          if(prn_flg->PRN_DMN_VAR_NM) (void)sprintf(dmn_sng,"%*s%%s%s%%ld%s=%s ",(idx == 0) ? prn_ndn : 0,spc_sng,prn_flg->FORTRAN_IDX_CNV ? "(" : "[",prn_flg->FORTRAN_IDX_CNV ? ")" : "]",nco_typ_fmt_sng(dim[dmn_idx].type)); else (void)sprintf(dmn_sng,"%*s%s ",(idx == 0) ? prn_ndn : 0,spc_sng,nco_typ_fmt_sng(dim[dmn_idx].type));
          dmn_sbs_prn=dmn_sbs_dsk[dmn_idx];

          if(prn_flg->FORTRAN_IDX_CNV){
            (void)sng_idx_dlm_c2f(dmn_sng);
            dmn_sbs_prn++;
          } /* end if */

          /* Account for hyperslab offset in coordinate values*/
          crd_idx_crr=dmn_sbs_ram[dmn_idx];
	  char crd_sng_lgb[NCO_MAX_LEN_FMT_SNG];
	  if(unit_cln_crd){
	    crd_sng_lgb[0]='\0';
	    (void)nco_cln_sng_rbs(dim[dmn_idx].val,crd_idx_crr,dim[dmn_idx].type,unit_sng_crd,crd_sng_lgb);
	    dim[dmn_idx].type=NC_CHAR;
	  } /* !unit_cln_crd */
          if(prn_flg->PRN_DMN_VAR_NM){
            switch(dim[dmn_idx].type){
            case NC_FLOAT: (void)fprintf(fp_out,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,dim[dmn_idx].val.fp[crd_idx_crr]); break;
            case NC_DOUBLE: (void)fprintf(fp_out,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,dim[dmn_idx].val.dp[crd_idx_crr]); break;
            case NC_SHORT: (void)fprintf(fp_out,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,dim[dmn_idx].val.sp[crd_idx_crr]); break;
            case NC_INT: (void)fprintf(fp_out,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,dim[dmn_idx].val.ip[crd_idx_crr]); break;
            case NC_CHAR:
	      if(unit_cln_crd) (void)fprintf(fp_out,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,crd_sng_lgb); else (void)fprintf(fp_out,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,dim[dmn_idx].val.cp[crd_idx_crr]);
	      break;
	    case NC_BYTE: (void)fprintf(fp_out,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,(unsigned char)dim[dmn_idx].val.bp[crd_idx_crr]); break;
            case NC_UBYTE: (void)fprintf(fp_out,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,dim[dmn_idx].val.ubp[crd_idx_crr]); break;
            case NC_USHORT: (void)fprintf(fp_out,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,dim[dmn_idx].val.usp[crd_idx_crr]); break;
            case NC_UINT: (void)fprintf(fp_out,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,dim[dmn_idx].val.uip[crd_idx_crr]); break;
            case NC_INT64: (void)fprintf(fp_out,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,dim[dmn_idx].val.i64p[crd_idx_crr]); break;
            case NC_UINT64: (void)fprintf(fp_out,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,dim[dmn_idx].val.ui64p[crd_idx_crr]); break;
            case NC_STRING: (void)fprintf(fp_out,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,dim[dmn_idx].val.sngp[crd_idx_crr]); break;
            case NC_VLEN: (void)fprintf(fp_out,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,dim[dmn_idx].val.ip[crd_idx_crr]); break;
            default: nco_dfl_case_nc_type_err(); break;
            } /* end switch */
          }else{ /* !PRN_DMN_VAR_NM */
            switch(dim[dmn_idx].type){
            case NC_FLOAT: (void)fprintf(fp_out,dmn_sng,dim[dmn_idx].val.fp[crd_idx_crr]); break;
            case NC_DOUBLE: (void)fprintf(fp_out,dmn_sng,dim[dmn_idx].val.dp[crd_idx_crr]); break;
            case NC_SHORT: (void)fprintf(fp_out,dmn_sng,dim[dmn_idx].val.sp[crd_idx_crr]); break;
            case NC_INT: (void)fprintf(fp_out,dmn_sng,dim[dmn_idx].val.ip[crd_idx_crr]); break;
            case NC_CHAR:
	      if(unit_cln_crd) (void)fprintf(fp_out,dmn_sng,crd_sng_lgb); else (void)fprintf(fp_out,dmn_sng,dim[dmn_idx].val.cp[crd_idx_crr]);
	      break;
            case NC_BYTE: (void)fprintf(fp_out,dmn_sng,(unsigned char)dim[dmn_idx].val.bp[crd_idx_crr]); break;
            case NC_UBYTE: (void)fprintf(fp_out,dmn_sng,dim[dmn_idx].val.ubp[crd_idx_crr]); break;
            case NC_USHORT: (void)fprintf(fp_out,dmn_sng,dim[dmn_idx].val.usp[crd_idx_crr]); break;
            case NC_UINT: (void)fprintf(fp_out,dmn_sng,dim[dmn_idx].val.uip[crd_idx_crr]); break;
            case NC_INT64: (void)fprintf(fp_out,dmn_sng,dim[dmn_idx].val.i64p[crd_idx_crr]); break;
            case NC_UINT64: (void)fprintf(fp_out,dmn_sng,dim[dmn_idx].val.ui64p[crd_idx_crr]); break;
            case NC_STRING: (void)fprintf(fp_out,dmn_sng,dim[dmn_idx].val.sngp[crd_idx_crr]); break;
            case NC_VLEN: (void)fprintf(fp_out,dmn_sng,dim[dmn_idx].val.ip[crd_idx_crr]); break;
            default: nco_dfl_case_nc_type_err(); break;
            } /* end switch */
          } /* !PRN_DMN_VAR_NM */
        } /* end loop over dimensions */
      } /* end if PRN_DMN_IDX_CRD_VAL */

      /* Print all characters in last dimension each time penultimate dimension subscript changes to its start value */
lbl_chr_prn:

      if(var->type == NC_CHAR){
        static nco_bool NUL_CHR_IN_SLB;
        static char *prn_sng;
        static int chr_cnt;
        static long dmn_sz;
        static long var_dsk_srt;
        static long var_dsk_end;

        /* At beginning of character array */
        if(dmn_sbs_ram[var->nbr_dim-1] == 0L) {
          dmn_sz=lmt_msa[var->nbr_dim-1]->dmn_cnt;
          prn_sng=(char *)nco_malloc((size_t)dmn_sz+1UL);
          var_dsk_srt=var_dsk;
          var_dsk_end=var_dsk;
          chr_cnt=0;
          NUL_CHR_IN_SLB=False;
        } /* end if */

        /* In middle of array---save characters to prn_sng */
        prn_sng[chr_cnt++]=var->val.cp[lmn];
        if(var->val.cp[lmn] == '\0' && !NUL_CHR_IN_SLB){
          var_dsk_end=var_dsk;
          NUL_CHR_IN_SLB=True;
        } /* end if */

        /* At end of character array */
        if(dmn_sbs_ram[var->nbr_dim-1] == dmn_sz-1L){
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
          (void)fprintf(fp_out,var_sng,var_nm,var_dsk_srt,var_dsk_end,prn_sng,unit_sng_var);
          (void)fprintf(fp_out,"\n");
          (void)fflush(fp_out);
          (void)nco_free(prn_sng);
        } /* endif */
        continue;
      } /* end if NC_CHAR */

      /* Print variable name, index, and value */
      if(prn_flg->PRN_DMN_VAR_NM) (void)sprintf(var_sng,"%*s%%s%s%%ld%s=%s %%s\n",(var_trv->is_crd_var) ? prn_ndn : 0,spc_sng,prn_flg->FORTRAN_IDX_CNV ? "(" : "[",prn_flg->FORTRAN_IDX_CNV ? ")" : "]",nco_typ_fmt_sng(var->type)); else (void)sprintf(var_sng,"%*s%s\n",(var_trv->is_crd_var) ? prn_ndn : 0,spc_sng,nco_typ_fmt_sng(var->type));
      if(prn_flg->FORTRAN_IDX_CNV){
        (void)sng_idx_dlm_c2f(var_sng);
        var_dsk++;
      } /* end if FORTRAN_IDX_CNV */

      if(prn_flg->PRN_MSS_VAL_BLANK && is_mss_val){
        if(prn_flg->PRN_DMN_VAR_NM) (void)fprintf(fp_out,"%*s%s%s%ld%s=%s %s\n",(var_trv->is_crd_var) ? prn_ndn : 0,spc_sng,var_nm,prn_flg->FORTRAN_IDX_CNV ? "(" : "[",var_dsk,prn_flg->FORTRAN_IDX_CNV ? ")" : "]",mss_val_sng,unit_sng_var); else (void)fprintf(fp_out,"%*s%s\n",(var_trv->is_crd_var) ? prn_ndn : 0,spc_sng,mss_val_sng); 
      }else{ /* !is_mss_val */
        if(prn_flg->PRN_DMN_VAR_NM){
          switch(var->type){
          case NC_FLOAT: (void)fprintf(fp_out,var_sng,var_nm,var_dsk,var->val.fp[lmn],unit_sng_var); break;
          case NC_DOUBLE: (void)fprintf(fp_out,var_sng,var_nm,var_dsk,var->val.dp[lmn],unit_sng_var); break;
          case NC_SHORT: (void)fprintf(fp_out,var_sng,var_nm,var_dsk,var->val.sp[lmn],unit_sng_var); break;
          case NC_INT: (void)fprintf(fp_out,var_sng,var_nm,var_dsk,var->val.ip[lmn],unit_sng_var); break;
          case NC_CHAR: (void)fprintf(fp_out,var_sng,var_nm,var_dsk,var->val.cp[lmn],unit_sng_var); break;
          case NC_BYTE: (void)fprintf(fp_out,var_sng,var_nm,var_dsk,(unsigned char)var->val.bp[lmn],unit_sng_var); break;
          case NC_UBYTE: (void)fprintf(fp_out,var_sng,var_nm,var_dsk,var->val.ubp[lmn],unit_sng_var); break;
          case NC_USHORT: (void)fprintf(fp_out,var_sng,var_nm,var_dsk,var->val.usp[lmn],unit_sng_var); break;
          case NC_UINT: (void)fprintf(fp_out,var_sng,var_nm,var_dsk,var->val.uip[lmn],unit_sng_var); break;
          case NC_INT64: (void)fprintf(fp_out,var_sng,var_nm,var_dsk,var->val.i64p[lmn],unit_sng_var); break;
          case NC_UINT64: (void)fprintf(fp_out,var_sng,var_nm,var_dsk,var->val.ui64p[lmn],unit_sng_var); break;
          case NC_STRING: (void)fprintf(fp_out,var_sng,var_nm,var_dsk,var->val.sngp[lmn],unit_sng_var); break;
          case NC_VLEN: (void)fprintf(fp_out,var_sng,var_nm,var_dsk,var->val.ip[lmn],unit_sng_var); break;
          default: nco_dfl_case_nc_type_err(); break;
          } /* end switch */
        }else{ /* !PRN_DMN_VAR_NM */
          switch(var->type){
          case NC_FLOAT: (void)fprintf(fp_out,var_sng,var->val.fp[lmn],unit_sng_var); break;
          case NC_DOUBLE: (void)fprintf(fp_out,var_sng,var->val.dp[lmn],unit_sng_var); break;
          case NC_SHORT: (void)fprintf(fp_out,var_sng,var->val.sp[lmn],unit_sng_var); break;
          case NC_INT: (void)fprintf(fp_out,var_sng,var->val.ip[lmn],unit_sng_var); break;
          case NC_CHAR: (void)fprintf(fp_out,var_sng,var->val.cp[lmn],unit_sng_var); break;
          case NC_BYTE: (void)fprintf(fp_out,var_sng,(unsigned char)var->val.bp[lmn],unit_sng_var); break;
          case NC_UBYTE: (void)fprintf(fp_out,var_sng,var->val.ubp[lmn],unit_sng_var); break;
          case NC_USHORT: (void)fprintf(fp_out,var_sng,var->val.usp[lmn],unit_sng_var); break;
          case NC_UINT: (void)fprintf(fp_out,var_sng,var->val.uip[lmn],unit_sng_var); break;
          case NC_INT64: (void)fprintf(fp_out,var_sng,var->val.i64p[lmn],unit_sng_var); break;
          case NC_UINT64: (void)fprintf(fp_out,var_sng,var->val.ui64p[lmn],unit_sng_var); break;
          case NC_STRING: (void)fprintf(fp_out,var_sng,var->val.sngp[lmn],unit_sng_var); break;
          case NC_VLEN: (void)fprintf(fp_out,var_sng,var->val.ip[lmn],unit_sng_var); break;
          default: nco_dfl_case_nc_type_err(); break;
          } /* end switch */
        } /* !PRN_DMN_VAR_NM */
      } /* !is_mss_val */
    } /* end loop over elements */

    (void)fflush(fp_out);

    /* Clean-up information for multi-dimensional arrays */
    for(int idx=0;idx<var->nbr_dim;idx++) if(dim[idx].val.vp) dim[idx].val.vp=nco_free(dim[idx].val.vp);
    if(dim) dim=(dmn_sct *)nco_free(dim);
    if(dmn_sbs_ram) dmn_sbs_ram=(long *)nco_free(dmn_sbs_ram);
    if(dmn_sbs_dsk) dmn_sbs_dsk=(long *)nco_free(dmn_sbs_dsk);
    if(mod_map_cnt) mod_map_cnt=(long *)nco_free(mod_map_cnt);
    if(mod_map_rv_cnt) mod_map_rv_cnt=(long *)nco_free(mod_map_rv_cnt);
    if(mod_map_in) mod_map_in=(long *)nco_free(mod_map_in);

  } /* end if variable has more than one dimension */

  /* Free value buffer
  if(var->type == NC_STRING)
     // nco_string_lst_free(var.val.sngp,var.sz);
      for(lmn=0;lmn<var->sz;lmn++)
	    if(var->val.sngp[lmn]) var->val.sngp[lmn]=(nco_string)nco_free(var->val.sngp[lmn]);

  var->val.vp=nco_free(var->val.vp);
  var->mss_val.vp=nco_free(var->mss_val.vp);
  var->nm=(char *)nco_free(var->nm); */

  if(flg_malloc_unit_crd) unit_sng_crd=(char *)nco_free(unit_sng_crd);
  if(flg_malloc_unit_var) unit_sng_var=(char *)nco_free(unit_sng_var);
  if(dlm_sng) dlm_sng=(char *)nco_free(dlm_sng);
  if(prn_flg->nwl_pst_val) (void)fprintf(fp_out,"\n");

  /* Free (allocated for non scalars only) */
  if(var->nbr_dim > 0){
    (void)nco_lmt_msa_free(var_trv->nbr_dmn,lmt_msa);
    lmt=(lmt_sct **)nco_free(lmt);
  } /* endif */

  var=nco_var_free(var);
  if(var_aux) var_aux=nco_var_free(var_aux);

} /* end nco_prn_var_val_trv() */

int /* [rcd] Return code */
nco_grp_prn /* [fnc] Recursively print group contents */
(const int nc_id, /* I [id] netCDF file ID */
 const char * const grp_nm_fll, /* I [sng] Absolute group name (path) */
 prn_fmt_sct * const prn_flg, /* I/O [sct] Print-format information */
 const trv_tbl_sct * const trv_tbl) /* I [sct] Traversal table */
{
  /* NB: Deprecated--Do Not Modify!!!
     This is the original hierarchical, recursive, multi-plexed print routine used until ~2017
     Henry de-multi-plexed it into routines nco_prn_jsn(), nco_prn_xml(), nco_prn_cdl_trd() */

  /* Purpose: Recursively print group contents
     Assumptions: 
     1. Input is a valid group name on extraction list (set in nco_xtr_dfn())
        Hence no need to check for group type, or if group is extracted
     2. Input ID is netCDF file ID, not extracted group ID */

  /* Testing: 
     ncks ~/nco/data/in_grp.nc */

  const char sls_sng[]="/";        /* [sng] Slash string */
  const char spc_sng[]="";        /* [sng] Space string */

  char grp_nm[NC_MAX_NAME+1L];      /* [sng] Group name */
  char var_nm[NC_MAX_NAME+1L];      /* [sng] Variable name */ 

  char *nm_cdl;
  char *nm_jsn;
  char *var_nm_fll;                /* [sng] Full path for variable */

  FILE *fp_out=prn_flg->fp_out; /* [fl] Formatted text output file handle */

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
  
  nco_bool JSN_BLOCK=False;         /* turns true is we have output a jsnblock -need so we add commas where needed */
  const nco_bool CDL=prn_flg->cdl; /* [flg] CDL output */
  const nco_bool XML=prn_flg->xml; /* [flg] XML output */
  const nco_bool TRD=prn_flg->trd; /* [flg] Traditional output */
  const nco_bool SRM=prn_flg->srm; /* [flg] Stream output */
  const nco_bool JSN=prn_flg->jsn; /* [flg] JSON output */
  const nco_bool CDL_OR_TRD=prn_flg->cdl || prn_flg->trd; /* [flg] CDL or Traditional output */

  nm_id_sct *dmn_lst; /* [sct] Dimension list */
  nm_id_sct *var_lst; /* [sct] Variable list */

  unsigned int dmn_idx; /* [idx] Index over dimensions */
  unsigned int dmn_nbr; /* [nbr] Number of dimensions defined in group */
  unsigned int obj_idx; /* [idx] Index over traversal table */

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
  grp_dpt=trv_tbl->lst[obj_idx].grp_dpt;
  nbr_att=trv_tbl->lst[obj_idx].nbr_att;
  nbr_var=trv_tbl->lst[obj_idx].nbr_var;
  nbr_grp=trv_tbl->lst[obj_idx].nbr_grp;

  /* Find dimension information for group */
  for(dmn_idx=0;dmn_idx<trv_tbl->nbr_dmn;dmn_idx++){
    /* Will dimension be extracted? (or are we printing all dimensions?) */
    if(trv_tbl->lst_dmn[dmn_idx].flg_xtr || prn_flg->rad){
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
      if(prn_flg->xml_lcn) (void)fprintf(fp_out,"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<ncml:netcdf xmlns:ncml=\"http://www.unidata.ucar.edu/namespaces/netcdf/ncml-2.2\" location=\"file:%s\">\n",prn_flg->fl_in); else (void)fprintf(fp_out,"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<ncml:netcdf xmlns:ncml=\"http://www.unidata.ucar.edu/namespaces/netcdf/ncml-2.2\">\n");
      if(prn_flg->nfo_xtr) (void)fprintf(fp_out,"%*s<!-- %s -->\n",prn_flg->sxn_fst,spc_sng,prn_flg->smr_sng);
    }else{ /* grp_dpt != 0 */
      (void)fprintf(fp_out,"%*s<ncml:group name=\"%s\">\n",grp_dpt*prn_flg->spc_per_lvl,spc_sng,trv_tbl->lst[obj_idx].nm);
    } /* grp_dpt != 0 */
  }else if(SRM){ /* !xml */
    /* https://www.unidata.ucar.edu/software/thredds/current/netcdf-java/stream/NcStream.html */
  }else if(CDL_OR_TRD){ /* !xml */
    nm_cdl=nm2sng_cdl(nco_gpe_evl_stb(prn_flg->gpe,trv_tbl->lst[obj_idx].nm_fll));
    if(grp_dpt == 0 && CDL) (void)fprintf(fp_out,"netcdf %s {",prn_flg->fl_stb); else (void)fprintf(fp_out,"%*sgroup: %s {",grp_dpt*prn_flg->spc_per_lvl,spc_sng,nm_cdl);
    nm_cdl=(char *)nco_free(nm_cdl);
    if(prn_flg->fll_pth) (void)fprintf(fp_out," // fullname: %s\n",nco_gpe_evl(prn_flg->gpe,grp_nm_fll)); else (void)fprintf(fp_out,"\n");
    if(grp_dpt == 0 && prn_flg->nfo_xtr && prn_flg->PRN_GLB_METADATA) (void)fprintf(fp_out,"%*s// %s\n",prn_flg->sxn_fst,spc_sng,prn_flg->smr_sng);
    if(grp_dpt == 0 && prn_flg->nfo_xtr) (void)fprintf(fp_out,"%*s// %s\n",prn_flg->sxn_fst,spc_sng,prn_flg->smr_fl_sz_sng);
    if(grp_dpt == 0 && prn_flg->nfo_xtr) (void)fprintf(fp_out,"%*s// %sncgen -k %s -b -o %s.nc %s.cdl\n",prn_flg->sxn_fst,spc_sng,prn_flg->PRN_GLB_METADATA ? "Generate binary file: " : "",nco_fmt_hdn_sng(prn_flg->fl_out_fmt),prn_flg->fl_stb,prn_flg->fl_stb);
  }else if(JSN){
    nm_jsn=nm2sng_jsn(nco_gpe_evl_stb(prn_flg->gpe,trv_tbl->lst[obj_idx].nm_fll));
    /* JSN print main opening brace */
    if(grp_dpt==0) (void)fprintf(fp_out,"{\n"); else (void)fprintf(fp_out,"%*s\"%s\": {\n",prn_flg->sxn_fst+grp_dpt*prn_flg->spc_per_lvl,spc_sng,nm_jsn);
    nm_jsn=(char *)nco_free(nm_jsn);  
  } /* !JSN */
  
  /* Print dimension information for group */
  prn_ndn=prn_flg->ndn=prn_flg->sxn_fst+grp_dpt*prn_flg->spc_per_lvl;
  if(dmn_nbr > 0 && CDL_OR_TRD) (void)fprintf(fp_out,"%*sdimensions:\n",prn_flg->ndn,spc_sng); 
  if(dmn_nbr > 0 && JSN) (void)fprintf(fp_out,"%*s\"dimensions\": {\n",prn_flg->ndn,spc_sng);
  if(CDL||JSN) prn_ndn+=prn_flg->var_fst;
  for(dmn_idx=0;dmn_idx<dmn_nbr;dmn_idx++){
    if(XML){
      (void)fprintf(fp_out,"%*s<ncml:dimension name=\"%s\" length=\"%lu\" %s/>\n",prn_ndn,spc_sng,dmn_lst[dmn_idx].nm,(unsigned long)trv_tbl->lst_dmn[dmn_lst[dmn_idx].id].lmt_msa.dmn_cnt,trv_tbl->lst_dmn[dmn_lst[dmn_idx].id].is_rec_dmn ? "isUnlimited=\"true\" " : "");
    }else if(CDL_OR_TRD){ /* !XML */
      nm_cdl=nm2sng_cdl(dmn_lst[dmn_idx].nm);
      if(trv_tbl->lst_dmn[dmn_lst[dmn_idx].id].is_rec_dmn) (void)fprintf(fp_out,"%*s%s = UNLIMITED%s// (%lu currently)\n",prn_ndn,spc_sng,nm_cdl,(CDL) ? " ; " : " ",(unsigned long)trv_tbl->lst_dmn[dmn_lst[dmn_idx].id].lmt_msa.dmn_cnt); else (void)fprintf(fp_out,"%*s%s = %lu%s\n",prn_ndn,spc_sng,nm_cdl,(unsigned long)trv_tbl->lst_dmn[dmn_lst[dmn_idx].id].lmt_msa.dmn_cnt,(CDL) ? " ;" : "");
      nm_cdl=(char *)nco_free(nm_cdl);
    } /* !XML */
    if(JSN){ 
      nm_jsn=nm2sng_jsn(dmn_lst[dmn_idx].nm);
      (void)fprintf(fp_out,"%*s\"%s\": %lu",prn_ndn,spc_sng,nm_jsn,(unsigned long)trv_tbl->lst_dmn[dmn_lst[dmn_idx].id].lmt_msa.dmn_cnt);   
      /* Add comma and carriage-return unless last element */
      if(dmn_idx<dmn_nbr-1) (void)fprintf(fp_out,",\n");
      else{  
        prn_ndn-=prn_flg->var_fst;
	(void)fprintf(fp_out,"\n%*s}",prn_ndn,spc_sng);         
      } /* !dmn_idx */
       
      nm_jsn=(char *)nco_free(nm_jsn);   
      JSN_BLOCK=True;    
    } /* !JSN */
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

  // if(JSN) (void)fprintf(fp_out,"{\n");

  /* Print variable information for group */
  if(var_nbr_xtr > 0 && CDL_OR_TRD) (void)fprintf(fp_out,"\n%*svariables:\n",prn_flg->ndn,spc_sng);

  for(var_idx=0;var_idx<var_nbr_xtr;var_idx++){
    trv_sct var_trv=trv_tbl->lst[var_lst[var_idx].id];
    /* Obtain variable ID */
    (void)nco_inq_varid(grp_id,var_trv.nm,&var_id);

    /* Print variable full name */
    if(var_trv.grp_dpt > 0 && prn_flg->fll_pth && TRD) (void)fprintf(fp_out,"%*s%s\n",prn_flg->ndn,spc_sng,var_trv.nm_fll);

    /* NB: CDL and TRD place data in seperate tag, while XML and JSN place data in same tag as metadata */  
   
    /* Following IFs are mutually exclusive */
    if(CDL || TRD){
      if(CDL || (TRD && prn_flg->PRN_VAR_METADATA)) (void)nco_prn_var_dfn(nc_id,prn_flg,&var_trv);

      /* nco_prn_att() prints final brace but no return, as we may need to add a comma */ 
      if(prn_flg->PRN_VAR_METADATA) (void)nco_prn_att(grp_id,prn_flg,var_id);
           
      if((CDL || TRD) && var_idx != var_nbr_xtr-1) (void)fprintf(fp_out,"\n"); 
    } /* !CDL_OR_TRD */

    /* Options can toggle printing of data. Only JSN and XML place data "inside" variable markup */ 
    if(XML){
      (void)nco_prn_var_dfn(nc_id,prn_flg,&var_trv); 
      
      if(prn_flg->PRN_VAR_METADATA) (void)nco_prn_att(grp_id,prn_flg,var_id);
   
      if(prn_flg->PRN_VAR_DATA) (void)nco_prn_var_val_trv(nc_id,prn_flg,&trv_tbl->lst[var_lst[var_idx].id],trv_tbl);
      /* XML close variable tag */
      (void)fprintf(fp_out,"%*s</ncml:variable>\n",prn_ndn,spc_sng);
    } /* !XML */

    if(JSN){ 
      /* Deal with first iteration */   
      if(var_idx == 0){ 
        if(JSN_BLOCK) (void)fprintf(fp_out,",\n"); else JSN_BLOCK=True;              
        (void)fprintf(fp_out,"%*s\"variables\": {\n",prn_flg->ndn,spc_sng);
      } /* !var_idx */
        
      /* Variable definition section does not print a final CR since as we may need to insert a comma before the CR */
      (void)nco_prn_var_dfn(nc_id,prn_flg,&var_trv);
      prn_flg->ndn+=prn_flg->sxn_fst;  

      /* nco_prn_att() prints final brace though not return, as we may need to insert a comma before the CR */ 
      if(prn_flg->PRN_VAR_METADATA  && nco_att_nbr(grp_id,var_id) > 0){
	/* Print comma+CR preceding "attributes" section */
	(void)fprintf(fp_out,",\n");            
        (void)nco_prn_att(grp_id,prn_flg,var_id);
      } /* !nco_att_nbr */
        
      if(prn_flg->PRN_VAR_DATA){  
	/* Print comma+CR preceding "data" section */
        if(prn_flg->jsn_var_fmt >= 1) (void)fprintf(fp_out,",\n");
        (void)nco_prn_var_val_trv(nc_id,prn_flg,&trv_tbl->lst[var_lst[var_idx].id],trv_tbl);      
      }else{
        (void)fprintf(fp_out,"\n");        
      } /* !PRN_VAR_DATA */

      /* Close JSON object tag, do not add return as we may need to add comma later */
      (void)fprintf(fp_out,"%*s}%s",prn_flg->ndn,spc_sng,(var_idx<var_nbr_xtr-1) ? ",\n" : "\n");
      /* special indents for jsn */
      prn_flg->ndn-=prn_flg->sxn_fst;  
    }
   } /* end loop over var_idx */

  /* close out json variable tag */
  if(JSN && var_nbr_xtr > 0) (void)fprintf(fp_out,"%*s}",prn_flg->ndn,spc_sng);

  /* Print attribute information for group 
  if((nbr_att > 0 || (prn_flg->hdn && grp_dpt == 0)) && prn_flg->PRN_GLB_METADATA && CDL_OR_TRD) (void)fprintf(fp_out,"\n%*s%s%sattributes:\n",prn_flg->ndn,spc_sng,(CDL) ? "// " : "",(grp_dpt == 0) ? "global " : "group ");
  if((nbr_att > 0 || (prn_flg->hdn && grp_dpt == 0)) && prn_flg->PRN_GLB_METADATA){ 
     (void)fprintf(fp_out,",\n");          
     nco_prn_att(grp_id,prn_flg,NC_GLOBAL);
  }else{
     (void)fprintf(fp_out,"\n");           
     } */

  if((nbr_att > 0 || (prn_flg->hdn && grp_dpt == 0)) && prn_flg->PRN_GLB_METADATA){
    if(CDL || TRD){ 
      (void)fprintf(fp_out,"\n%*s%s%sattributes:\n",prn_flg->ndn,spc_sng,(CDL) ? "// " : "",(grp_dpt == 0) ? "global " : "group ");
      nco_prn_att(grp_id,prn_flg,NC_GLOBAL);
    }
    if(XML) nco_prn_att(grp_id,prn_flg,NC_GLOBAL);
    if(JSN && nco_att_nbr(grp_id,NC_GLOBAL) > 0){
      if(JSN_BLOCK) (void)fprintf(fp_out,",\n"); else JSN_BLOCK=True;              
      nco_prn_att(grp_id,prn_flg,NC_GLOBAL);
    }
  }  

  /* Print data for group only CDL and TRD have a separate data block*/
  if( (CDL||TRD) && var_nbr_xtr > 0 && prn_flg->PRN_VAR_DATA){
    (void)fprintf(fp_out,"\n%*sdata:\n",prn_flg->ndn,spc_sng);
    for(var_idx=0;var_idx<var_nbr_xtr;var_idx++) (void)nco_prn_var_val_trv(nc_id,prn_flg,&trv_tbl->lst[var_lst[var_idx].id],trv_tbl);
  } /* end if */

  /* Variable list no longer needed */
  var_lst=nco_nm_id_lst_free(var_lst,var_nbr_xtr);

  /* Get ready for sub-groups */ 
  grp_ids=(int *)nco_malloc(nbr_grp*sizeof(int)); 
  rcd+=nco_inq_grps(grp_id,(int *)NULL,grp_ids);

  //if(JSN && prn_flg->PRN_GLB_METADATA) (void)fprintf(fp_out,"\"nodes\": [");

  /* Recursive block for evrything else */
  if(!JSN){
    /* Call recursively for all extracted subgroups */
    for(grp_idx=0;grp_idx<nbr_grp;grp_idx++){
      char *sub_grp_nm_fll=NULL; /* [sng] Sub group path */
      int gid=grp_ids[grp_idx]; /* [id] Current group ID */  

      /* Get sub-group name */
      rcd+=nco_inq_grpname(gid,grp_nm);

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
      if(trv_tbl->lst[obj_idx].flg_xtr) rcd+=nco_grp_prn(nc_id,sub_grp_nm_fll,prn_flg,trv_tbl);

      /* Free constructed name */
      sub_grp_nm_fll=(char *)nco_free(sub_grp_nm_fll);
    } /* end loop over grp_idx */

    /* Recurse block for JSN */
  }else{
    int nbr_grp_xtr=0;   
    if(nbr_grp > 0){  
      if(JSN_BLOCK) (void)fprintf(fp_out,",\n"); 
      (void)fprintf(fp_out,"%*s\"groups\": {\n",prn_ndn,spc_sng);       
      JSN_BLOCK=True; 
    }

    /* Call recursively for all extracted subgroups */
    for(grp_idx=0;grp_idx<nbr_grp;grp_idx++){
      char *sub_grp_nm_fll=NULL; /* [sng] Sub group path */
      int gid=grp_ids[grp_idx]; /* [id] Current group ID */  

      /* Get sub-group name */
      rcd+=nco_inq_grpname(gid,grp_nm);

      /* if(grp_idx>0) */
      /* 	  (void)fprintf(fp_out,"\n");  */
      /* else */
      /* 	(void)fprintf(fp_out,"%*s\"%s\":{\n",prn_ndn,spc_sng,grp_nm); */

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
      if(trv_tbl->lst[obj_idx].flg_xtr){ 
        if(nbr_grp_xtr++ > 0) (void)fprintf(fp_out,",\n"); 
        rcd+=nco_grp_prn(nc_id,sub_grp_nm_fll,prn_flg,trv_tbl);
      } 

      /* Free constructed name */
      sub_grp_nm_fll=(char *)nco_free(sub_grp_nm_fll);
    } /* end loop over grp_idx */
  }

  /* if JSN then print closing tag to group */
  if(JSN && nbr_grp > 0) (void)fprintf(fp_out,"\n%*s}",prn_ndn,spc_sng);

  /* Mark end of output */
  if(CDL_OR_TRD) (void)fprintf(fp_out,"%*s} // group %s\n",grp_dpt*prn_flg->spc_per_lvl,spc_sng,(grp_dpt == 0) ? grp_nm_fll : nm2sng_cdl(nco_gpe_evl(prn_flg->gpe,grp_nm_fll)));
  if(JSN && grp_dpt ==0) (void)fprintf(fp_out,"\n}\n"); 
  // if(JSN && grp_dpt >0) (void)fprintf(fp_out,"\n%*s}",prn_ndn,spc_sng); 
  if(JSN && grp_dpt >0) (void)fprintf(fp_out,"\n%*s}", prn_flg->sxn_fst+grp_dpt*prn_flg->spc_per_lvl,spc_sng);
  if(XML && grp_dpt == 0) (void)fprintf(fp_out,"</netcdf>\n"); 
  if(XML && grp_dpt != 0) (void)fprintf(fp_out,"%*s</ncml:group>\n",grp_dpt*prn_flg->spc_per_lvl,spc_sng); 

  return rcd;
} /* end nco_grp_prn() */

int /* [rcd] Return code */
nco_prn_cdl_trd /* [fnc] Recursively print group contents */
(const int nc_id, /* I [id] netCDF file ID */
 const char * const grp_nm_fll, /* I [sng] Absolute group name (path) */
 prn_fmt_sct * const prn_flg, /* I/O [sct] Print-format information */
 const trv_tbl_sct * const trv_tbl) /* I [sct] Traversal table */
{
  /* This and other per-Language backend routines replaced nco_grp_prn() in 2017 */

  /* Purpose: Recursively print group contents
     Assumptions: 
     1. Input is a valid group name on extraction list (set in nco_xtr_dfn())
        Hence no need to check for group type, or if group is extracted
     2. Input ID is netCDF file ID, not extracted group ID */

  /* Testing: 
     ncks --cdl ~/nco/data/buggy.nc
     ncks --udt --cdl ~/nco/data/buggy.nc
     ncks -D 1 --udt ~/nco/data/vlen.nc
     ncks -D 1 --udt ~/nco/data/enum.nc
     ncks --cdl ~/nco/data/in_grp.nc
     ncks --trd ~/nco/data/in_grp.nc */

  const char fnc_nm[]="nco_prn_cdl_trd()"; /* [sng] Function name */

  const char sls_sng[]="/";        /* [sng] Slash string */
  const char spc_sng[]="";        /* [sng] Space string */

  char grp_nm[NC_MAX_NAME+1L];      /* [sng] Group name */
  char var_nm[NC_MAX_NAME+1L];      /* [sng] Variable name */ 

  char *nm_cdl;
  char *var_nm_fll;                /* [sng] Full path for variable */

  FILE *fp_out=prn_flg->fp_out; /* [fl] Formatted text output file handle */

  int *grp_ids;                    /* [ID] Sub-group IDs array */  

  int dmn_idx_grp[NC_MAX_DIMS];    /* [ID] Dimension indices array for group */ 
  int grp_idx;                     /* [idx] Group index */  
  int grp_id;                      /* [id] netCDF group ID */
  int grp_dpt;                     /* [nbr] Depth of group (root = 0) */
  int nbr_att;                     /* [nbr] Number of attributes */
  int nbr_grp;                     /* [nbr] Number of sub-groups in this group */
  int nbr_var;                     /* [nbr] Number of variables */
  int nbr_typ;                     /* [nbr] Number of types defined in group */
  int prn_ndn=0;                   /* [nbr] Indentation for printing */
  int rcd=NC_NOERR;                /* [rcd] Return code */
  int var_id;                      /* [id] Variable ID */
  int var_idx;                     /* [idx] Variable index */
  int var_nbr_xtr;                 /* [nbr] Number of extracted variables */
  
  const nco_bool CDL=prn_flg->cdl; /* [flg] CDL output */
  const nco_bool TRD=prn_flg->trd; /* [flg] CDL output */

  nm_id_sct *dmn_lst; /* [sct] Dimension list */
  nm_id_sct *var_lst; /* [sct] Variable list */

  unsigned int dmn_idx; /* [idx] Index over dimensions */
  unsigned int dmn_nbr; /* [nbr] Number of dimensions defined in group */
  unsigned int obj_idx; /* [idx] Index over traversal table */

  /* Initialize */
  nbr_typ=0; /* [nbr] Number of types defined in group */
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
  grp_dpt=trv_tbl->lst[obj_idx].grp_dpt;
  nbr_att=trv_tbl->lst[obj_idx].nbr_att;
  nbr_var=trv_tbl->lst[obj_idx].nbr_var;
  nbr_grp=trv_tbl->lst[obj_idx].nbr_grp;
  nbr_typ=trv_tbl->lst[obj_idx].nbr_typ;

  /* Find dimension information for group */
  for(dmn_idx=0;dmn_idx<trv_tbl->nbr_dmn;dmn_idx++){
    /* Will dimension be extracted? (or are we printing all dimensions?) */
    if(trv_tbl->lst_dmn[dmn_idx].flg_xtr || prn_flg->rad){
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

  nm_cdl=nm2sng_cdl(nco_gpe_evl_stb(prn_flg->gpe,trv_tbl->lst[obj_idx].nm_fll));
  if(grp_dpt == 0 && CDL) (void)fprintf(fp_out,"netcdf %s {",prn_flg->fl_stb); else (void)fprintf(fp_out,"%*sgroup: %s {",grp_dpt*prn_flg->spc_per_lvl,spc_sng,nm_cdl);
  nm_cdl=(char *)nco_free(nm_cdl);
  if(prn_flg->fll_pth) (void)fprintf(fp_out," // fullname: %s\n",nco_gpe_evl(prn_flg->gpe,grp_nm_fll)); else (void)fprintf(fp_out,"\n");
  if(grp_dpt == 0 && prn_flg->nfo_xtr && prn_flg->PRN_GLB_METADATA) (void)fprintf(fp_out,"%*s// %s\n",prn_flg->sxn_fst,spc_sng,prn_flg->smr_sng);
  if(grp_dpt == 0 && prn_flg->nfo_xtr && prn_flg->PRN_GLB_METADATA && nco_dbg_lvl_get() > nco_dbg_std) (void)fprintf(fp_out,"%*s// %s\n",prn_flg->sxn_fst,spc_sng,prn_flg->smr_fl_sz_sng);
  if(grp_dpt == 0 && prn_flg->nfo_xtr) (void)fprintf(fp_out,"%*s// %sncgen -k %s -b -o %s.nc %s.cdl\n",prn_flg->sxn_fst,spc_sng,prn_flg->PRN_GLB_METADATA ? "Generate binary file from this CDL: " : "",nco_fmt_hdn_sng(prn_flg->fl_out_fmt),prn_flg->fl_stb,prn_flg->fl_stb);
  
  /* Print type information for group */
  if(nbr_typ > 0){
    char *bs_cdl;
    char *mbr_nm_cdl;
    char *typ_cdl;
    char bs_nm[NC_MAX_NAME+1L]; /* [sng] Base name */
    char mbr_nm[NC_MAX_NAME+1L]; /* [sng] Member name */
    char typ_nm[NC_MAX_NAME+1L]; /* [sng] Type name */
    int cls_typ;
    nc_type *typ_ids;
    nc_type bs_typ;
    size_t fld_nbr;
    prn_ndn=prn_flg->ndn=prn_flg->sxn_fst+grp_dpt*prn_flg->spc_per_lvl;
    (void)fprintf(fp_out,"%*stypes:\n",prn_flg->ndn,spc_sng); 
    if(CDL) prn_ndn+=prn_flg->var_fst;
    typ_ids=(nc_type *)nco_malloc(nbr_typ*(sizeof(nc_type)));
    rcd+=nco_inq_typeids(grp_id,NULL,typ_ids);
    for(int typ_idx=0;typ_idx<nbr_typ;typ_idx++){
      rcd=nco_inq_user_type(grp_id,typ_ids[typ_idx],typ_nm,NULL,&bs_typ,&fld_nbr,&cls_typ);
      rcd=nco_inq_type(grp_id,bs_typ,bs_nm,NULL);
      bs_cdl=nm2sng_cdl(bs_nm);
      typ_cdl=nm2sng_cdl(typ_nm);
      if(cls_typ == NC_VLEN) (void)fprintf(fp_out,"%*s%s(*) %s ;\n",prn_ndn,spc_sng,bs_cdl,typ_cdl);
      if(cls_typ == NC_ENUM){
	char enm_fmt[NCO_MAX_LEN_FMT_SNG];
	size_t mbr_nbr;
	size_t mbr_nbrm1;
	val_unn enm_val;
	(void)sprintf(enm_fmt,"%%s = %s",nco_typ_fmt_sng_var_cdl(bs_typ));
	(void)fprintf(fp_out,"%*s%s enum %s {",prn_ndn,spc_sng,bs_cdl,typ_cdl);
	mbr_nbr=fld_nbr;
	mbr_nbrm1=mbr_nbr-1L;
	for(unsigned int mbr_idx=0;mbr_idx<mbr_nbr;mbr_idx++){
	  rcd=nco_inq_enum_member(grp_id,typ_ids[typ_idx],mbr_idx,mbr_nm,(void *)&enm_val);
	  mbr_nm_cdl=nm2sng_cdl(mbr_nm);
	  switch(bs_typ){
	  case NC_BYTE: (void)fprintf(fp_out,enm_fmt,mbr_nm_cdl,enm_val.b); break;
	  case NC_UBYTE: (void)fprintf(fp_out,enm_fmt,mbr_nm_cdl,enm_val.ub); break;
	  case NC_SHORT: (void)fprintf(fp_out,enm_fmt,mbr_nm_cdl,enm_val.s); break;
	  case NC_USHORT: (void)fprintf(fp_out,enm_fmt,mbr_nm_cdl,enm_val.us); break;
	  case NC_INT: (void)fprintf(fp_out,enm_fmt,mbr_nm_cdl,enm_val.i); break;
	  case NC_UINT: (void)fprintf(fp_out,enm_fmt,mbr_nm_cdl,enm_val.ui); break;
	  case NC_INT64: (void)fprintf(fp_out,enm_fmt,mbr_nm_cdl,enm_val.i64); break;
	  case NC_UINT64: (void)fprintf(fp_out,enm_fmt,mbr_nm_cdl,enm_val.ui64); break;
	  default: nco_dfl_case_nc_type_err(); break;
	  } /* !bs_typ switch */
	  mbr_nm_cdl=(char *)nco_free(mbr_nm_cdl);
	  if(mbr_idx < mbr_nbrm1) (void)fprintf(fp_out,", "); else (void)fprintf(fp_out,"} ;\n");
	} /* !mbr_idx */
      } /* !enm */
      bs_cdl=(char *)nco_free(bs_cdl);
      typ_cdl=(char *)nco_free(typ_cdl);
    } /* !typ_idx */
    typ_ids=(nc_type *)nco_free(typ_ids);
    (void)fprintf(fp_out,"\n"); 
  } /* !nbr_typ */
  
  /* Print dimension information for group */
  prn_ndn=prn_flg->ndn=prn_flg->sxn_fst+grp_dpt*prn_flg->spc_per_lvl;
  if(dmn_nbr > 0) (void)fprintf(fp_out,"%*sdimensions:\n",prn_flg->ndn,spc_sng); 
  if(CDL) prn_ndn+=prn_flg->var_fst;

  for(dmn_idx=0;dmn_idx<dmn_nbr;dmn_idx++){
    nm_cdl=nm2sng_cdl(dmn_lst[dmn_idx].nm);
    if(trv_tbl->lst_dmn[dmn_lst[dmn_idx].id].is_rec_dmn) (void)fprintf(fp_out,"%*s%s = UNLIMITED%s// (%lu currently)",prn_ndn,spc_sng,nm_cdl,(CDL) ? " ; " : " ",(unsigned long)trv_tbl->lst_dmn[dmn_lst[dmn_idx].id].lmt_msa.dmn_cnt); else (void)fprintf(fp_out,"%*s%s = %lu%s",prn_ndn,spc_sng,nm_cdl,(unsigned long)trv_tbl->lst_dmn[dmn_lst[dmn_idx].id].lmt_msa.dmn_cnt,(CDL) ? " ;" : "");
    nm_cdl=(char *)nco_free(nm_cdl);
    /* 20180605: Add dimension IDs */
    if((nco_dbg_lvl_get() >= nco_dbg_fl) && CDL){
      int dmn_id;
      rcd+=nco_inq_dimid(grp_id,dmn_lst[dmn_idx].nm,&dmn_id);
      (void)fprintf(fp_out," %sID = %d\n",(trv_tbl->lst_dmn[dmn_lst[dmn_idx].id].is_rec_dmn) ? "" : "// ",dmn_id);
    }else{ /* !dbg */
      (void)fprintf(fp_out,"\n");
    } /* !dbg */
  } /* !dmn_idx */

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
    if(!prn_flg->udt){
      /* Normal variable */
      for(obj_idx=0;obj_idx<trv_tbl->nbr;obj_idx++)
	if(trv_tbl->lst[obj_idx].nco_typ == nco_obj_typ_var)
	  if(!strcmp(trv_tbl->lst[obj_idx].nm_fll,var_nm_fll))
	    break;
    }else{
      for(obj_idx=0;obj_idx<trv_tbl->nbr;obj_idx++)
	if(trv_tbl->lst[obj_idx].nco_typ != nco_obj_typ_grp)
	  if(!strcmp(trv_tbl->lst[obj_idx].nm_fll,var_nm_fll))
	    break;
      
      if((obj_idx != trv_tbl->nbr) && (trv_tbl->lst[obj_idx].nco_typ == nco_obj_typ_nonatomic_var)){
	/* Variable is non-atomic, print helpful information */
	if(nco_dbg_lvl_get() >= nco_dbg_var){
	  (void)fprintf(stdout,"%s: DEBUG %s reports grp_nm_fll = %s, grp_dpt = %d, nbr_var = %d\n",nco_prg_nm_get(),fnc_nm,grp_nm_fll,grp_dpt,nbr_var);
	  (void)fprintf(stdout,"%s: DEBUG %s reports %s is non-atomic (e.g., compound, enum, opaque, vlen, or user-defined) variable type. Support is minimal.\n",nco_prg_nm_get(),fnc_nm,var_nm);
	  trv_sct var_trv=trv_tbl->lst[obj_idx];
	  (void)fprintf(stdout,"%s: DEBUG %s reports obj_nm = %s, var_nm = %s, var_typ = %d = %s, flg_xtr = %d\n",nco_prg_nm_get(),fnc_nm,var_trv.nm_fll,var_nm_fll,var_trv.var_typ,nco_typ_sng(var_trv.var_typ),var_trv.flg_xtr);
	  (void)fprintf(stdout,"%s: DEBUG %s reports %s type %d = %s, typ_nm = %s, typ_sz = %lu, bs_typ = %d = %s, fld_nbr = %lu, cls_typ = %d = %s\n",nco_prg_nm_get(),fnc_nm,var_trv.nm_fll,var_trv.var_typ,nco_typ_sng(var_trv.var_typ),var_trv.typ_nm,(unsigned long)var_trv.typ_sz,var_trv.bs_typ,nco_typ_sng(var_trv.bs_typ),(unsigned long)var_trv.fld_nbr,var_trv.cls_typ,nco_typ_sng(var_trv.cls_typ));
	  (void)nco_prn_var_dfn(nc_id,prn_flg,&var_trv);
	} /* !dbg */
      } /* !udt */

    } /* udt */
    
    /* Is variable to be extracted? */
    if(obj_idx<trv_tbl->nbr && trv_tbl->lst[obj_idx].flg_xtr){
      /* NB: ID here is actually index into trv_tbl->lst. It is NOT an ID. 
	 However, it is same type (int) as an ID so we re-use nm_id infrastructure */
      var_lst[var_nbr_xtr].id=obj_idx;
      var_lst[var_nbr_xtr].nm=strdup(var_nm);
      var_nbr_xtr++;
    } /* endif extracted */

    /* Free constructed name */
    var_nm_fll=(char *)nco_free(var_nm_fll);

  } /* !var_idx */

  /* Compactify array to hold names and indices of extracted variables in this group */
  var_lst=(nm_id_sct *)nco_realloc(var_lst,var_nbr_xtr*(sizeof(nm_id_sct)));

  /* Sort variables alphabetically */
  if(var_nbr_xtr > 1) var_lst=nco_lst_srt_nm_id(var_lst,var_nbr_xtr,prn_flg->ALPHA_BY_STUB_GROUP);

  /* Print variable information for group */
  if(var_nbr_xtr > 0) (void)fprintf(fp_out,"%s%*svariables:\n",(dmn_nbr > 0) ? "\n" : "",prn_flg->ndn,spc_sng);

  for(var_idx=0;var_idx<var_nbr_xtr;var_idx++){
    trv_sct var_trv=trv_tbl->lst[var_lst[var_idx].id];
    /* Obtain variable ID */
    (void)nco_inq_varid(grp_id,var_trv.nm,&var_id);

    /* Print variable full name */
    if(var_trv.grp_dpt > 0 && prn_flg->fll_pth && TRD) (void)fprintf(fp_out,"%*s%s\n",prn_flg->ndn,spc_sng,var_trv.nm_fll);

    /* NB: CDL and TRD place data in seperate tag, while XML and JSN place data in same tag as metadata */  
    if(CDL || (TRD && prn_flg->PRN_VAR_METADATA)) (void)nco_prn_var_dfn(nc_id,prn_flg,&var_trv);

    /* nco_prn_att() prints final brace but no return, as we may need to add a comma */ 
    if(prn_flg->PRN_VAR_METADATA) (void)nco_prn_att(grp_id,prn_flg,var_id);
           
    if(var_idx != var_nbr_xtr-1) (void)fprintf(fp_out,"\n"); 
  } /* end loop over var_idx */

  if((nbr_att > 0 || (prn_flg->hdn && grp_dpt == 0)) && prn_flg->PRN_GLB_METADATA){
    (void)fprintf(fp_out,"\n%*s%s%sattributes:\n",prn_flg->ndn,spc_sng,(CDL) ? "// " : "",(grp_dpt == 0) ? "global " : "group ");
    nco_prn_att(grp_id,prn_flg,NC_GLOBAL);
  } /* !nbr_att */

  /* Print data for group. Only CDL and TRD have a separate data block */
  if(var_nbr_xtr > 0 && prn_flg->PRN_VAR_DATA){
    (void)fprintf(fp_out,"\n%*sdata:\n",prn_flg->ndn,spc_sng);
    for(var_idx=0;var_idx<var_nbr_xtr;var_idx++) (void)nco_prn_var_val_trv(nc_id,prn_flg,&trv_tbl->lst[var_lst[var_idx].id],trv_tbl);
  } /* end if */

  /* Variable list no longer needed */
  var_lst=nco_nm_id_lst_free(var_lst,var_nbr_xtr);

  /* Get ready for sub-groups */ 
  grp_ids=(int *)nco_malloc(nbr_grp*sizeof(int)); 
  rcd+=nco_inq_grps(grp_id,(int *)NULL,grp_ids);

  /* Call recursively for all extracted subgroups */
  for(grp_idx=0;grp_idx<nbr_grp;grp_idx++){
    char *sub_grp_nm_fll=NULL; /* [sng] Sub group path */
    int gid=grp_ids[grp_idx]; /* [id] Current group ID */  

    /* Get sub-group name */
    rcd+=nco_inq_grpname(gid,grp_nm);

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
    if(trv_tbl->lst[obj_idx].flg_xtr) rcd+=nco_prn_cdl_trd(nc_id,sub_grp_nm_fll,prn_flg,trv_tbl);

    /* Free constructed name */
    sub_grp_nm_fll=(char *)nco_free(sub_grp_nm_fll);
  } /* end loop over grp_idx */
  
  /* Mark end of output */
  (void)fprintf(fp_out,"%*s} // group %s\n",grp_dpt*prn_flg->spc_per_lvl,spc_sng,(grp_dpt == 0) ? grp_nm_fll : nm2sng_cdl(nco_gpe_evl(prn_flg->gpe,grp_nm_fll)));
  return rcd;
} /* end nco_prn_cdl_trd() */

int /* [rcd] Return code */
nco_prn_xml /* [fnc] Recursively print group contents */
(const int nc_id, /* I [id] netCDF file ID */
 const char * const grp_nm_fll, /* I [sng] Absolute group name (path) */
 prn_fmt_sct * const prn_flg, /* I/O [sct] Print-format information */
 const trv_tbl_sct * const trv_tbl) /* I [sct] Traversal table */
{
  /* Purpose: Recursively print group contents
     Assumptions: 
     1. Input is a valid group name on extraction list (set in nco_xtr_dfn())
        Hence no need to check for group type, or if group is extracted
     2. Input ID is netCDF file ID, not extracted group ID */

  /* Testing: 
     ncks --xml ~/nco/data/in_grp.nc */

  const char sls_sng[]="/";        /* [sng] Slash string */
  const char spc_sng[]="";        /* [sng] Space string */

  char grp_nm[NC_MAX_NAME+1L];      /* [sng] Group name */
  char var_nm[NC_MAX_NAME+1L];      /* [sng] Variable name */ 

  char *var_nm_fll;                /* [sng] Full path for variable */

  FILE *fp_out=prn_flg->fp_out; /* [fl] Formatted text output file handle */

  int *grp_ids;                    /* [ID] Sub-group IDs array */  

  int dmn_idx_grp[NC_MAX_DIMS];    /* [ID] Dimension indices array for group */ 
  int grp_idx;                     /* [idx] Group index */  
  int grp_id;                      /* [id] netCDF group ID */
  int grp_dpt;                     /* [nbr] Depth of group (root = 0) */
  int nbr_att;                     /* [nbr] Number of attributes */
  int nbr_grp;                     /* [nbr] Number of sub-groups in this group */
  int nbr_var;                     /* [nbr] Number of variables */
  int nbr_typ;                     /* [nbr] Number of types */
  int prn_ndn=0;                   /* [nbr] Indentation for printing */
  int rcd=NC_NOERR;                /* [rcd] Return code */
  int var_id;                      /* [id] Variable ID */
  int var_idx;                     /* [idx] Variable index */
  int var_nbr_xtr;                 /* [nbr] Number of extracted variables */
  
  nm_id_sct *dmn_lst; /* [sct] Dimension list */
  nm_id_sct *var_lst; /* [sct] Variable list */

  unsigned int dmn_idx; /* [idx] Index over dimensions */
  unsigned int dmn_nbr; /* [nbr] Number of dimensions defined in group */
  unsigned int obj_idx; /* [idx] Index over traversal table */

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
  grp_dpt=trv_tbl->lst[obj_idx].grp_dpt;
  nbr_att=trv_tbl->lst[obj_idx].nbr_att;
  nbr_var=trv_tbl->lst[obj_idx].nbr_var;
  nbr_grp=trv_tbl->lst[obj_idx].nbr_grp;
  nbr_typ=trv_tbl->lst[obj_idx].nbr_typ;

  /* Find dimension information for group */
  for(dmn_idx=0;dmn_idx<trv_tbl->nbr_dmn;dmn_idx++){
    /* Will dimension be extracted? (or are we printing all dimensions?)*/
    if(trv_tbl->lst_dmn[dmn_idx].flg_xtr || prn_flg->rad){
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

  if(grp_dpt == 0){
    if(prn_flg->xml_lcn) (void)fprintf(fp_out,"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<ncml:netcdf xmlns:ncml=\"http://www.unidata.ucar.edu/namespaces/netcdf/ncml-2.2\" location=\"file:%s\">\n",prn_flg->fl_in); else (void)fprintf(fp_out,"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<ncml:netcdf xmlns:ncml=\"http://www.unidata.ucar.edu/namespaces/netcdf/ncml-2.2\">\n");
    if(prn_flg->nfo_xtr) (void)fprintf(fp_out,"%*s<!-- %s -->\n",prn_flg->sxn_fst,spc_sng,prn_flg->smr_sng);
  }else{ /* grp_dpt != 0 */
    (void)fprintf(fp_out,"%*s<ncml:group name=\"%s\">\n",grp_dpt*prn_flg->spc_per_lvl,spc_sng,trv_tbl->lst[obj_idx].nm);
  } /* grp_dpt != 0 */

  /* Print dimension information for group */
  prn_ndn=prn_flg->ndn=prn_flg->sxn_fst+grp_dpt*prn_flg->spc_per_lvl;

  /* Print type information for group */
  if(nbr_typ > 0){
    char *typ_cdl;
    char *bs_cdl;
    char bs_nm[NC_MAX_NAME+1L]; /* [sng] Base name */
    char mbr_nm[NC_MAX_NAME+1L]; /* [sng] Member name */
    char typ_nm[NC_MAX_NAME+1L]; /* [sng] Type name */
    int cls_typ;
    nc_type *typ_ids;
    nc_type bs_typ;
    size_t fld_nbr;

    typ_ids=(nc_type *)nco_malloc(nbr_typ*(sizeof(nc_type)));
    rcd+=nco_inq_typeids(grp_id,NULL,typ_ids);
    for(int typ_idx=0;typ_idx<nbr_typ;typ_idx++){
      rcd=nco_inq_user_type(grp_id,typ_ids[typ_idx],typ_nm,NULL,&bs_typ,&fld_nbr,&cls_typ);
      rcd=nco_inq_type(grp_id,bs_typ,bs_nm,NULL);
      bs_cdl=nm2sng_cdl(bs_nm);
      typ_cdl=nm2sng_cdl(typ_nm);
      if(cls_typ == NC_VLEN) (void)fprintf(fp_out,"%*s<type name=\"%s\" value=\"%s(*)\" />\n", prn_ndn, spc_sng, typ_cdl, bs_cdl);
      if(cls_typ == NC_ENUM){
	char enm_fmt[NCO_MAX_LEN_FMT_SNG];
	int bs_sz;
	size_t mbr_nbr;
	val_unn enm_val;
        bs_sz=(int)nco_typ_lng(bs_typ);
	bs_sz=(bs_sz > 4 ? 4 : bs_sz); // 20180608: NcML 2.2 lacks enum8 type https://github.com/Unidata/thredds/issues/1098
	(void)fprintf(fp_out,"%*s<ncml:enumTypedef name=\"%s\" type=\"enum%d\" >\n",prn_ndn,spc_sng,typ_cdl,bs_sz);
	(void)sprintf(enm_fmt,"%*s<ncml:enum key=\"%s\">%%s</ncml:enum>\n",prn_ndn+prn_flg->spc_per_lvl,spc_sng,nco_typ_fmt_sng_var_cdl(bs_typ));
	mbr_nbr=fld_nbr;
	for(unsigned int mbr_idx=0;mbr_idx<mbr_nbr;mbr_idx++){
	  rcd=nco_inq_enum_member(grp_id,typ_ids[typ_idx],mbr_idx,mbr_nm,(void *)&enm_val);
	  switch(bs_typ){
	  case NC_BYTE: (void)fprintf(fp_out,enm_fmt,enm_val.b,mbr_nm); break;
	  case NC_UBYTE: (void)fprintf(fp_out,enm_fmt,enm_val.ub,mbr_nm); break;
	  case NC_SHORT: (void)fprintf(fp_out,enm_fmt,enm_val.s,mbr_nm); break;
	  case NC_USHORT: (void)fprintf(fp_out,enm_fmt,enm_val.us,mbr_nm); break;
	  case NC_INT: (void)fprintf(fp_out,enm_fmt,enm_val.i,mbr_nm); break;
	  case NC_UINT: (void)fprintf(fp_out,enm_fmt,enm_val.ui,mbr_nm); break;
	  case NC_INT64: (void)fprintf(fp_out,enm_fmt,enm_val.i64,mbr_nm); break;
	  case NC_UINT64: (void)fprintf(fp_out,enm_fmt,enm_val.ui64,mbr_nm); break;
	  default: nco_dfl_case_nc_type_err(); break;
	  } /* !bs_typ switch */
	} /* !mbr_idx */
        (void)fprintf(fp_out,"%*s</ncml:enumTypedef>\n",prn_ndn,spc_sng);
      } /* !enm */
      /* close bracket  */
      bs_cdl=(char *)nco_free(bs_cdl);
      typ_cdl=(char *)nco_free(typ_cdl);
    } /* !typ_idx */
    typ_ids=(nc_type *)nco_free(typ_ids);
  } /* !nbr_typ */
  
  for(dmn_idx=0;dmn_idx<dmn_nbr;dmn_idx++) (void)fprintf(fp_out,"%*s<ncml:dimension name=\"%s\" length=\"%lu\" %s/>\n",prn_ndn,spc_sng,dmn_lst[dmn_idx].nm,(unsigned long)trv_tbl->lst_dmn[dmn_lst[dmn_idx].id].lmt_msa.dmn_cnt,trv_tbl->lst_dmn[dmn_lst[dmn_idx].id].is_rec_dmn ? "isUnlimited=\"true\" " : "");
  
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
      if(trv_tbl->lst[obj_idx].nco_typ == nco_obj_typ_var || trv_tbl->lst[obj_idx].nco_typ == nco_obj_typ_nonatomic_var )
	if(!strcmp(trv_tbl->lst[obj_idx].nm_fll,var_nm_fll))
	  break;
    
    /* Is variable to be extracted? */
    if(obj_idx<trv_tbl->nbr && trv_tbl->lst[obj_idx].flg_xtr){
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

  for(var_idx=0;var_idx<var_nbr_xtr;var_idx++){
    trv_sct var_trv=trv_tbl->lst[var_lst[var_idx].id];
    /* Obtain variable ID */
    (void)nco_inq_varid(grp_id,var_trv.nm,&var_id);

    /* Print variable full name */
    // if(var_trv.grp_dpt > 0 && prn_flg->fll_pth && TRD) (void)fprintf(fp_out,"%*s%s\n",prn_flg->ndn,spc_sng,var_trv.nm_fll);

    (void)nco_prn_var_dfn(nc_id,prn_flg,&var_trv); 
      
    if(prn_flg->PRN_VAR_METADATA) (void)nco_prn_att(grp_id,prn_flg,var_id);
   
    if(prn_flg->PRN_VAR_DATA) (void)nco_prn_var_val_trv(nc_id,prn_flg,&trv_tbl->lst[var_lst[var_idx].id],trv_tbl);

    /* XML close variable tag */
    (void)fprintf(fp_out,"%*s</ncml:variable>\n",prn_ndn,spc_sng);
  } /* end loop over var_idx */

  /* Print global metatdata */
  if((nbr_att > 0 || (prn_flg->hdn && grp_dpt == 0)) && prn_flg->PRN_GLB_METADATA) nco_prn_att(grp_id,prn_flg,NC_GLOBAL);

  /* Variable list no longer needed */
  var_lst=nco_nm_id_lst_free(var_lst,var_nbr_xtr);

  /* Get ready for sub-groups */ 
  grp_ids=(int *)nco_malloc(nbr_grp*sizeof(int)); 
  rcd+=nco_inq_grps(grp_id,(int *)NULL,grp_ids);

  /* Call recursively for all extracted subgroups */
  for(grp_idx=0;grp_idx<nbr_grp;grp_idx++){
    char *sub_grp_nm_fll=NULL; /* [sng] Sub group path */
    int gid=grp_ids[grp_idx]; /* [id] Current group ID */  

    /* Get sub-group name */
    rcd+=nco_inq_grpname(gid,grp_nm);

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
    if(trv_tbl->lst[obj_idx].flg_xtr) rcd+=nco_prn_xml(nc_id,sub_grp_nm_fll,prn_flg,trv_tbl);

    /* Free constructed name */
    sub_grp_nm_fll=(char *)nco_free(sub_grp_nm_fll);
  } /* end loop over grp_idx */

  /* Mark end of output */
  if(grp_dpt == 0) (void)fprintf(fp_out,"</ncml:netcdf>\n"); 
  if(grp_dpt > 0) (void)fprintf(fp_out,"%*s</ncml:group>\n",grp_dpt*prn_flg->spc_per_lvl,spc_sng); 

  return rcd;
} /* end nco_grp_prn_xml() */

int /* [rcd] Return code */
nco_prn_jsn /* [fnc] Recursively print group contents */
(const int nc_id, /* I [id] netCDF file ID */
 const char * const grp_nm_fll, /* I [sng] Absolute group name (path) */
 prn_fmt_sct * const prn_flg, /* I/O [sct] Print-format information */
 const trv_tbl_sct * const trv_tbl) /* I [sct] Traversal table */
{
  /* Purpose: Recursively print group contents
     Assumptions: 
     1. Input is a valid group name on extraction list (set in nco_xtr_dfn())
        Hence no need to check for group type, or if group is extracted
     2. Input ID is netCDF file ID, not extracted group ID */

  /* Testing: 
     ncks --jsn ~/nco/data/in_grp.nc */

  const char sls_sng[]="/";        /* [sng] Slash string */
  const char spc_sng[]="";        /* [sng] Space string */

  char grp_nm[NC_MAX_NAME+1L];      /* [sng] Group name */
  char var_nm[NC_MAX_NAME+1L];      /* [sng] Variable name */ 

  char *nm_jsn;
  char *var_nm_fll;                /* [sng] Full path for variable */

  FILE *fp_out=prn_flg->fp_out; /* [fl] Formatted text output file handle */

  int *grp_ids;                    /* [ID] Sub-group IDs array */  

  int dmn_idx_grp[NC_MAX_DIMS];    /* [ID] Dimension indices array for group */ 
  int grp_idx;                     /* [idx] Group index */  
  int grp_id;                      /* [id] netCDF group ID */
  int grp_dpt;                     /* [nbr] Depth of group (root = 0) */
  int nbr_att;                     /* [nbr] Number of attributes */
  int nbr_grp;                     /* [nbr] Number of sub-groups in this group */
  int nbr_var;                     /* [nbr] Number of variables */
  int nbr_typ;                     /* [nbr] Number of types defined in group */
  int prn_ndn=0;                   /* [nbr] Indentation for printing */
  int rcd=NC_NOERR;                /* [rcd] Return code */
  int var_id;                      /* [id] Variable ID */
  int var_idx;                     /* [idx] Variable index */
  int var_nbr_xtr;                 /* [nbr] Number of extracted variables */
  int nbr_grp_xtr=0;               /* number of groups currently extracted */  
  nco_bool JSN_BLOCK=False;         /* turns true is we have output a jsnblock -need so we add commas where needed */
  
  nm_id_sct *dmn_lst; /* [sct] Dimension list */
  nm_id_sct *var_lst; /* [sct] Variable list */

  unsigned int dmn_idx; /* [idx] Index over dimensions */
  unsigned int dmn_nbr; /* [nbr] Number of dimensions defined in group */
  unsigned int obj_idx; /* [idx] Index over traversal table */

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
  grp_dpt=trv_tbl->lst[obj_idx].grp_dpt;
  nbr_att=trv_tbl->lst[obj_idx].nbr_att;
  nbr_var=trv_tbl->lst[obj_idx].nbr_var;
  nbr_grp=trv_tbl->lst[obj_idx].nbr_grp;
  nbr_typ=trv_tbl->lst[obj_idx].nbr_typ;
  
  /* Find dimension information for group */
  for(dmn_idx=0;dmn_idx<trv_tbl->nbr_dmn;dmn_idx++){
    /* Will dimension be extracted? (or are we printing all dimensions?)*/
    if(trv_tbl->lst_dmn[dmn_idx].flg_xtr || prn_flg->rad){
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

  /* prn_ndn is indentation of group ID tag; offsets for vars/atts/groups tag are from this */
  if(grp_dpt == 0) prn_ndn=0; else prn_ndn=2*grp_dpt*prn_flg->spc_per_lvl;
     
  nm_jsn=nm2sng_jsn(nco_gpe_evl_stb(prn_flg->gpe,trv_tbl->lst[obj_idx].nm_fll));
 
  if(grp_dpt==0) (void)fprintf(fp_out,"{\n"); else (void)fprintf(fp_out,"%*s\"%s\": {\n",prn_ndn,spc_sng,nm_jsn);
  nm_jsn=(char *)nco_free(nm_jsn);  

  /* Print type information for group */
  if(nbr_typ > 0){
    char *typ_cdl;
    char *bs_cdl;
    char bs_nm[NC_MAX_NAME+1L]; /* [sng] Base name */
    char mbr_nm[NC_MAX_NAME+1L]; /* [sng] Member name */
    char typ_nm[NC_MAX_NAME+1L]; /* [sng] Type name */
    int cls_typ;
    nc_type *typ_ids;
    nc_type bs_typ;
    size_t fld_nbr;
    // prn_ndn=prn_flg->ndn=prn_flg->sxn_fst+grp_dpt*prn_flg->spc_per_lvl;
    (void)fprintf(fp_out,"%*s\"types\": {\n",prn_ndn+prn_flg->spc_per_lvl,spc_sng);
    //if(CDL) prn_ndn+=prn_flg->var_fst;
    typ_ids=(nc_type *)nco_malloc(nbr_typ*(sizeof(nc_type)));
    rcd+=nco_inq_typeids(grp_id,NULL,typ_ids);
    for(int typ_idx=0;typ_idx<nbr_typ;typ_idx++){
      rcd=nco_inq_user_type(grp_id,typ_ids[typ_idx],typ_nm,NULL,&bs_typ,&fld_nbr,&cls_typ);
      rcd=nco_inq_type(grp_id,bs_typ,bs_nm,NULL);
      bs_cdl=nm2sng_jsn(bs_nm);
      typ_cdl=nm2sng_cdl(typ_nm);
      if(cls_typ == NC_VLEN) (void)fprintf(fp_out,"%*s\"%s(*)\" : \"%s\"",prn_ndn+2*prn_flg->spc_per_lvl,spc_sng,bs_cdl,typ_cdl);
      if(cls_typ == NC_ENUM){
	char enm_fmt[NCO_MAX_LEN_FMT_SNG];
	size_t mbr_nbr;
	size_t mbr_nbrm1;
	val_unn enm_val;
	(void)sprintf(enm_fmt,"\"%%s\":%s",nco_typ_fmt_sng_var_cdl(bs_typ));
	(void)fprintf(fp_out,"%*s\"%s\": [ ",prn_ndn+2*prn_flg->spc_per_lvl,spc_sng,typ_cdl);
	mbr_nbr=fld_nbr;
	mbr_nbrm1=mbr_nbr-1L;
	for(unsigned int mbr_idx=0;mbr_idx<mbr_nbr;mbr_idx++){
	  rcd=nco_inq_enum_member(grp_id,typ_ids[typ_idx],mbr_idx,mbr_nm,(void *)&enm_val);
	  switch(bs_typ){
	  case NC_BYTE: (void)fprintf(fp_out,enm_fmt,mbr_nm,enm_val.b); break;
	  case NC_UBYTE: (void)fprintf(fp_out,enm_fmt,mbr_nm,enm_val.ub); break;
	  case NC_SHORT: (void)fprintf(fp_out,enm_fmt,mbr_nm,enm_val.s); break;
	  case NC_USHORT: (void)fprintf(fp_out,enm_fmt,mbr_nm,enm_val.us); break;
	  case NC_INT: (void)fprintf(fp_out,enm_fmt,mbr_nm,enm_val.i); break;
	  case NC_UINT: (void)fprintf(fp_out,enm_fmt,mbr_nm,enm_val.ui); break;
	  case NC_INT64: (void)fprintf(fp_out,enm_fmt,mbr_nm,enm_val.i64); break;
	  case NC_UINT64: (void)fprintf(fp_out,enm_fmt,mbr_nm,enm_val.ui64); break;
	  default: nco_dfl_case_nc_type_err(); break;
	  } /* !bs_typ switch */
	  if(mbr_idx < mbr_nbrm1) (void)fprintf(fp_out,", "); 
	} /* !mbr_idx */
        (void)fprintf(fp_out," ]");
      } /* !enm */
      /* close bracket  */
      bs_cdl=(char *)nco_free(bs_cdl);
      typ_cdl=(char *)nco_free(typ_cdl);
      if(typ_idx < nbr_typ-1) fprintf(fp_out,",");      
      (void)fprintf(fp_out,"\n");
    } /* !typ_idx */
    typ_ids=(nc_type *)nco_free(typ_ids);
    (void)fprintf(fp_out,"%*s},\n",prn_ndn+prn_flg->spc_per_lvl,spc_sng); 
  } /* !nbr_typ */
  
  if(dmn_nbr > 0) (void)fprintf(fp_out,"%*s\"dimensions\": {\n",prn_ndn+prn_flg->spc_per_lvl,spc_sng);
 
  for(dmn_idx=0;dmn_idx<dmn_nbr;dmn_idx++){
    nm_jsn=nm2sng_jsn(dmn_lst[dmn_idx].nm);
    (void)fprintf(fp_out,"%*s\"%s\": %lu",prn_ndn+2*prn_flg->spc_per_lvl,spc_sng,nm_jsn,(unsigned long)trv_tbl->lst_dmn[dmn_lst[dmn_idx].id].lmt_msa.dmn_cnt);   
    /* Add comma and carriage-return unless last element */
    if(dmn_idx<dmn_nbr-1) (void)fprintf(fp_out,",\n"); else (void)fprintf(fp_out,"\n%*s}",prn_ndn+prn_flg->spc_per_lvl,spc_sng);         
    nm_jsn=(char *)nco_free(nm_jsn);   
    JSN_BLOCK=True;    
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
      if(trv_tbl->lst[obj_idx].nco_typ == nco_obj_typ_var || trv_tbl->lst[obj_idx].nco_typ == nco_obj_typ_nonatomic_var )
	if(!strcmp(trv_tbl->lst[obj_idx].nm_fll,var_nm_fll))
	  break;
    
    /* Is variable to be extracted? */
    if(obj_idx<trv_tbl->nbr && trv_tbl->lst[obj_idx].flg_xtr){
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

  for(var_idx=0;var_idx<var_nbr_xtr;var_idx++){
    trv_sct var_trv=trv_tbl->lst[var_lst[var_idx].id];
    /* Obtain variable ID */
    (void)nco_inq_varid(grp_id,var_trv.nm,&var_id);
   
    /* Deal with first iteration */   
    if(var_idx == 0){ 
      if(JSN_BLOCK) (void)fprintf(fp_out,",\n"); else JSN_BLOCK=True;              
      (void)fprintf(fp_out,"%*s\"variables\": {\n",prn_ndn+prn_flg->spc_per_lvl,spc_sng);   
    } /* !var_idx */
        
    /* DOES NOT include a return as we may wanna add a COMMA */
    prn_flg->ndn=prn_ndn+2*prn_flg->spc_per_lvl; 
    (void)nco_prn_var_dfn(nc_id,prn_flg,&var_trv);

    /* nco_prn_att() prints final brace without carriage return since we may need to add a comma */
    if(prn_flg->PRN_VAR_METADATA && nco_att_nbr(grp_id,var_id) > 0){     
      (void)fprintf(fp_out,",\n");            
      //prn_flg->ndn=prn_ndn+2*prn_flg->spc_per_lvl;  
      (void)nco_prn_att(grp_id,prn_flg,var_id);
    } /* !prn_flg */
        
    if(prn_flg->PRN_VAR_DATA){  
      (void)fprintf(fp_out,",\n");  
      // prn_flg->ndn=prn_ndn+2*prn_flg->spc_per_lvl;  
      (void)nco_prn_var_val_trv(nc_id,prn_flg,&trv_tbl->lst[var_lst[var_idx].id],trv_tbl);      
    }else{
      (void)fprintf(fp_out,"\n");        
    } /* !PRN_VAR_DATA */
    /* Close JSON tag without carriage return since we may need to add comma later */
    (void)fprintf(fp_out,"%*s}%s",prn_flg->ndn,spc_sng, (var_idx<var_nbr_xtr-1 ?",\n":"\n"));
    
  } /* end loop over var_idx */

  /* Close-out JSON variable tag */
  if(var_nbr_xtr>0) (void)fprintf(fp_out,"%*s}",prn_ndn+prn_flg->spc_per_lvl,spc_sng);   

  if((nbr_att > 0 || (prn_flg->hdn && grp_dpt == 0)) && prn_flg->PRN_GLB_METADATA){
    if(JSN_BLOCK) (void)fprintf(fp_out,",\n"); else JSN_BLOCK=True;              
    prn_flg->ndn=prn_ndn+prn_flg->spc_per_lvl;  
    nco_prn_att(grp_id,prn_flg,NC_GLOBAL);
  }  

  /* Variable list no longer needed */
  var_lst=nco_nm_id_lst_free(var_lst,var_nbr_xtr);

  /* Get ready for sub-groups */ 
  grp_ids=(int *)nco_malloc(nbr_grp*sizeof(int)); 
  rcd+=nco_inq_grps(grp_id,(int *)NULL,grp_ids);

  /*
  if( nbr_grp > 0) 
  {  
     if(JSN_BLOCK) (void)fprintf(fp_out,",\n"); 
     (void)fprintf(fp_out,"%*s\"groups\": {\n",prn_ndn+prn_flg->spc_per_lvl,spc_sng);       
     JSN_BLOCK=True; 
  }
  */
  
  /* Call recursively for all extracted subgroups */
  for(grp_idx=0;grp_idx<nbr_grp;grp_idx++){
    char *sub_grp_nm_fll=NULL; /* [sng] Sub group path */
    int gid=grp_ids[grp_idx]; /* [id] Current group ID */  

    /* Get sub-group name */
    rcd+=nco_inq_grpname(gid,grp_nm);

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
    if(trv_tbl->lst[obj_idx].flg_xtr){  
      /* Print groups tag for first extracted group */ 
      if(nbr_grp_xtr++ == 0){   
	if(JSN_BLOCK) (void)fprintf(fp_out,",\n"); 
	(void)fprintf(fp_out,"%*s\"groups\": {\n",prn_ndn+prn_flg->spc_per_lvl,spc_sng);       
      }else{
	(void)fprintf(fp_out,",\n"); 
      } /* !nbr_grp_xtr */
      rcd+=nco_prn_jsn(nc_id,sub_grp_nm_fll,prn_flg,trv_tbl);        
    } /* !flg_xtr */

    /* Free constructed name */
    sub_grp_nm_fll=(char *)nco_free(sub_grp_nm_fll);
  } /* end loop over grp_idx */

  /* Print closing tag for group tag */
  if(nbr_grp > 0 && nbr_grp_xtr > 0) (void)fprintf(fp_out,"\n%*s}",prn_ndn+prn_flg->spc_per_lvl,spc_sng);         
  if(grp_dpt ==0) (void)fprintf(fp_out,"\n}\n"); 
  if(grp_dpt >0) (void)fprintf(fp_out,"\n%*s}",prn_ndn,spc_sng);
  
  return rcd;
} /* end nco_prn_jsn() */

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

  if(nco_typ>NC_MAX_ATOMIC_TYPE)
    return False;

  
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

     Placing this in its own routine saves many lines
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

void
nco_prn_nonfinite_flt
(char * const val_sng,		  
 const prn_fmt_sct * const prn_flg, /* I [sct] Print-format information */
 float val_flt)
{
  if(isnan(val_flt)) (void)sprintf(val_sng,(prn_flg->jsn) ? "null" : "NaN");
  else if(isinf(val_flt)) (void)sprintf(val_sng,"%s",(prn_flg->jsn) ? "null" : (val_flt < 0.0f) ? "-Infinity" : "Infinity");    

  /* Add trailing "f" for just CDL */
  if(prn_flg->cdl && !prn_flg->xml) strcat(val_sng, "f");

  return; 
} /* !nco_prn_nonfinite_flt() */

void
nco_prn_nonfinite_dbl
(char * const val_sng,		  
 const prn_fmt_sct * const prn_flg, /* I [sct] Print-format information */
 double val_dbl)
{

  if(isnan(val_dbl)) (void)sprintf(val_sng,(prn_flg->jsn) ? "null" : "NaN");
  else if(isinf(val_dbl)) (void)sprintf(val_sng,"%s",(prn_flg->jsn) ? "null" : (val_dbl < 0.0) ? "-Infinity" : "Infinity");    

  return; 
} /* !nco_prn_nonfinite_dbl() */
