/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_cnk.c,v 1.2 2009-05-23 00:04:41 zender Exp $ */

/* Purpose: NCO utilities for chunking */

/* Copyright (C) 1995--2009 Charlie Zender
   License: GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */

#include "nco_cnk.h" /* Chunking */

const char * /* O [sng] Chunking map string */
nco_cnk_map_sng_get /* [fnc] Convert chunking map enum to string */
(const int nco_cnk_map) /* I [enm] Chunking map */
{
  /* Purpose: Convert chunking map enum to string */
  switch(nco_cnk_map){
  case nco_cnk_map_nil:
    return "nil";
  case nco_cnk_map_dmn:
    return "dmn";
  case nco_cnk_map_rcd_one:
    return "rcd_one";
  case nco_cnk_map_scl:
    return "scl";
  default: nco_dfl_case_cnk_map_err(); break;
  } /* end switch */
  /* Some compilers, e.g., SGI cc, need return statement to end non-void functions */
  return (char *)NULL;
} /* end nco_cnk_map_sng_get() */

const char * /* O [sng] Chunking policy string */
nco_cnk_plc_sng_get /* [fnc] Convert chunking policy enum to string */
(const int nco_cnk_plc) /* I [enm] Chunking policy */
{
  /* Purpose: Convert chunking policy enum to string */
  switch(nco_cnk_plc){
  case nco_cnk_plc_nil:
    return "nil";
  case nco_cnk_plc_all:
    return "all";
  case nco_cnk_plc_g2d:
    return "g2d";
  case nco_cnk_plc_g3d: 
    return "g3d";
  case nco_cnk_plc_uck:
    return "uck";
  default: nco_dfl_case_cnk_plc_err(); break;
  } /* end switch */
  /* Some compilers, e.g., SGI cc, need return statement to end non-void functions */
  return (char *)NULL;
} /* end nco_cnk_plc_sng_get() */

void 
nco_dfl_case_cnk_map_err(void) /* [fnc] Print error and exit for illegal switch(cnk_map) case */
{
  /* Purpose: Convenience routine for printing error and exiting when
     switch(cnk_map) statement receives an illegal default case

     Placing this in its own routine also has the virtue of saving many lines 
     of code since this function is used in many many switch() statements. */
  const char fnc_nm[]="nco_dfl_case_cnk_map_err()";
  (void)fprintf(stdout,"%s: ERROR switch(cnk_map) statement fell through to default case, which is unsafe. This catch-all error handler ensures all switch(cnk_map) statements are fully enumerated. Exiting...\n",fnc_nm);
  nco_err_exit(0,fnc_nm);
} /* end nco_dfl_case_cnk_map_err() */

void 
nco_dfl_case_cnk_plc_err(void) /* [fnc] Print error and exit for illegal switch(cnk_plc) case */
{
  /* Purpose: Convenience routine for printing error and exiting when
     switch(cnk_plc) statement receives an illegal default case

     Placing this in its own routine also has the virtue of saving many lines 
     of code since this function is used in many many switch() statements. */
  const char fnc_nm[]="nco_dfl_case_cnk_plc_err()";
  (void)fprintf(stdout,"%s: ERROR switch(cnk_plc) statement fell through to default case, which is unsafe. This catch-all error handler ensures all switch(cnk_plc) statements are fully enumerated. Exiting...\n",fnc_nm);
  nco_err_exit(0,fnc_nm);
} /* end nco_dfl_case_cnk_plc_err() */

cnk_sct ** /* O [sct] Structure list with user-specified chunking information */
nco_cnk_prs /* [fnc] Create chunking structures with name and chunksize elements */
(const int cnk_nbr, /* I [nbr] Number of chunksizes specified */
 CST_X_PTR_CST_PTR_CST_Y(char,cnk_arg)) /* I [sng] List of user-specified chunksizes */
{
  /* Purpose: Determine name and chunksize elements from user arguments
     Routine merely evaluates syntax of input expressions and does not 
     attempt to validate dimensions or chunksizes against input file.
     Routine based on nco_lmt_prs() */
  
  /* Valid syntax adheres to nm,cnk_sz */
  
  void nco_usg_prn(void);
  
  char **arg_lst;
  
  const char dlm_sng[]=",";
  
  cnk_sct **cnk=NULL_CEWI;
  
  int idx;
  int arg_nbr;
  
  if(cnk_nbr > 0) cnk=(cnk_sct **)nco_malloc(cnk_nbr*sizeof(cnk_sct *));
  for(idx=0;idx<cnk_nbr;idx++){
    /* Process chunksize specifications as normal text list */
    /* fxm: probably need to free arg_lst sometime... */
    arg_lst=nco_lst_prs_2D(cnk_arg[idx],dlm_sng,&arg_nbr);
    
    /* Check syntax */
    if(
       arg_nbr < 2 || /* Need more than just dimension name */
       arg_nbr > 2 || /* Too much information */
       arg_lst[0] == NULL || /* Dimension name not specified */
       False){
      (void)fprintf(stdout,"%s: ERROR in chunksize specification for dimension %s\n%s: HINT Conform request to chunksize documentation at http://nco.sf.net/nco.html#cnk\n",prg_nm_get(),cnk_arg[idx],prg_nm_get());
      nco_exit(EXIT_FAILURE);
    } /* end if */
    
    /* Initialize structure */
    /* cnk strings that are not explicitly set by user remain NULL, i.e., 
       specifying default setting will appear as if nothing at all was set.
       Hopefully, in routines that follow, branch followed when dimension has
       all default settings specified (e.g.,"-d foo,,,,") yields same answer
       as branch for which no hyperslab along that dimension was set. */
    cnk[idx]=(cnk_sct *)nco_malloc(sizeof(cnk_sct));
    cnk[idx]->nm=NULL;
    cnk[idx]->is_usr_spc_cnk=True; /* True if any part of limit is user-specified, else False */
    
    /* Fill in structure */
    cnk[idx]->nm=arg_lst[0];
    cnk[idx]->sz=strtoul(arg_lst[1],(char **)NULL,10);
    
    /* Free current pointer array to strings
       Strings themselves are untouched and will be free()'d with chunk structures 
       in nco_cnk_lst_free() */
    arg_lst=(char **)nco_free(arg_lst);
  } /* end loop over cnk structure list */
  
  return cnk;
} /* end nco_cnk_prs() */

cnk_sct ** /* O [sct] Pointer to free'd structure list */
nco_cnk_lst_free /* [fnc] Free memory associated with chunking structure list */
(cnk_sct **cnk_lst, /* I/O [sct] Chunking structure list to free */
 const int cnk_nbr) /* I [nbr] Number of chunking structures in list */
{
  /* Threads: Routine is thread safe and calls no unsafe routines */
  /* Purpose: Free all memory associated with dynamically allocated chunking structure list */
  int idx;

  for(idx=0;idx<cnk_nbr;idx++){
    cnk_lst[idx]=nco_cnk_free(cnk_lst[idx]);
  } /* end loop over idx */

  /* Free structure pointer last */
  cnk_lst=(cnk_sct **)nco_free(cnk_lst);

  return cnk_lst;
} /* end nco_cnk_lst_free() */

cnk_sct * /* O [sct] Pointer to free'd chunking structure */
nco_cnk_free /* [fnc] Free all memory associated with chunking structure */
(cnk_sct *cnk) /* I/O [sct] Chunking structure to free */
{
  /* Threads: Routine is thread safe and calls no unsafe routines */
  /* Purpose: Free all memory associated with a dynamically allocated chunking structure */
  cnk->nm=(char *)nco_free(cnk->nm);
  /* Free structure pointer last */
  cnk=(cnk_sct *)nco_free(cnk);

  return NULL;
} /* end nco_cnk_free() */

#if 0
nco_bool /* O [flg] NCO will attempt to chunk variable */
nco_is_chunkable /* [fnc] Will NCO attempt to chunk variable? */
(const nc_type nc_typ_in) /* I [enm] Type of input variable */
{
  /* Purpose: Determine whether NCO should attempt to chunk a given type
     Chunking certain variable types is not recommended, e.g., chunking NC_CHAR
     and NC_BYTE makes no sense, because precision would needlessly be lost.
     Routine should be consistent with nco_cnk_plc_typ_get()
     NB: Routine is deprecated in favor of more flexible nco_cnk_plc_typ_get() */
  const char fnc_nm[]="nco_is_chunkable()"; /* [sng] Function name */

  (void)fprintf(stdout,"%s: ERROR deprecated routine %s should not be called\n",prg_nm_get(),fnc_nm);
  nco_exit(EXIT_FAILURE);

  switch(nc_typ_in){ 
  case NC_FLOAT: 
  case NC_DOUBLE: 
  case NC_INT64: 
  case NC_UINT64: 
  case NC_INT: 
  case NC_UINT: 
    return True;
    break;
  case NC_SHORT: 
  case NC_USHORT: 
  case NC_CHAR: 
  case NC_BYTE: 
  case NC_UBYTE: 
  case NC_STRING:
    return False;
    break;
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */ 

  /* Some compilers, e.g., SGI cc, need return statement to end non-void functions */
  return False;
} /* end nco_is_chunkable() */

int /* O [enm] Chunking map */
nco_cnk_map_get /* [fnc] Convert user-specified chunking map to key */
(const char *nco_cnk_map_sng) /* [sng] User-specified chunking map */
{
  /* Purpose: Process ncpdq '-P' command line argument
     Convert user-specified string to chunking map
     Return nco_cnk_map_nil by default */
  const char fnc_nm[]="nco_cnk_map_get()"; /* [sng] Function name */
  char *prg_nm; /* [sng] Program name */
  prg_nm=prg_nm_get(); /* [sng] Program name */

  if(nco_cnk_map_sng == NULL){ 
    (void)fprintf(stderr,"%s: ERROR %s reports empty user-specified chunking map string %s\n",prg_nm,fnc_nm,nco_cnk_map_sng);
    nco_exit(EXIT_FAILURE);
  } /* endif */

  if(!strcmp(nco_cnk_map_sng,"hgh_sht")) return nco_cnk_map_hgh_sht;
  if(!strcmp(nco_cnk_map_sng,"cnk_map_hgh_sht")) return nco_cnk_map_hgh_sht;
  if(!strcmp(nco_cnk_map_sng,"hgh_chr")) return nco_cnk_map_hgh_chr;
  if(!strcmp(nco_cnk_map_sng,"cnk_map_hgh_chr")) return nco_cnk_map_hgh_chr;
  if(!strcmp(nco_cnk_map_sng,"hgh_byt")) return nco_cnk_map_hgh_byt;
  if(!strcmp(nco_cnk_map_sng,"cnk_map_hgh_byt")) return nco_cnk_map_hgh_byt;
  if(!strcmp(nco_cnk_map_sng,"nxt_lsr")) return nco_cnk_map_nxt_lsr;
  if(!strcmp(nco_cnk_map_sng,"cnk_map_nxt_lsr")) return nco_cnk_map_nxt_lsr;
  if(!strcmp(nco_cnk_map_sng,"flt_sht")) return nco_cnk_map_flt_sht;
  if(!strcmp(nco_cnk_map_sng,"cnk_map_flt_sht")) return nco_cnk_map_flt_sht;
  if(!strcmp(nco_cnk_map_sng,"flt_chr")) return nco_cnk_map_flt_chr;
  if(!strcmp(nco_cnk_map_sng,"cnk_map_flt_chr")) return nco_cnk_map_flt_chr;
  if(!strcmp(nco_cnk_map_sng,"flt_byt")) return nco_cnk_map_flt_byt;
  if(!strcmp(nco_cnk_map_sng,"cnk_map_flt_byt")) return nco_cnk_map_flt_byt;

  (void)fprintf(stderr,"%s: ERROR %s reports unknown user-specified chunking policy %s\n",prg_nm_get(),fnc_nm,nco_cnk_map_sng);
  nco_exit(EXIT_FAILURE);
  return nco_cnk_map_nil; /* Statement should not be reached */
} /* end nco_cnk_map_get() */

int /* O [enm] Chunking policy */
nco_cnk_plc_get /* [fnc] Convert user-specified chunking policy to key */
(const char *nco_cnk_plc_sng) /* [sng] User-specified chunking policy */
{
  /* Purpose: Process ncpdq '-P' command line argument
     Convert user-specified string to chunking operation type 
     Return nco_cnk_plc_nil by default */
  const char fnc_nm[]="nco_cnk_plc_get()"; /* [sng] Function name */
  char *prg_nm; /* [sng] Program name */
  prg_nm=prg_nm_get(); /* [sng] Program name */

  if(nco_cnk_plc_sng == NULL){
    if(strstr(prg_nm,"ncpdq")){
      if(dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stdout,"%s: INFO %s reports %s invoked without explicit chunking or dimension permutation options. Defaulting to chunking policy \"all_new\".\n",prg_nm,fnc_nm,prg_nm);
      return nco_cnk_plc_all_new_att;
    } /* endif */
    if(strstr(prg_nm,"ncchunk")) return nco_cnk_plc_all_new_att;
    if(strstr(prg_nm,"ncunchunk")) return nco_cnk_plc_uck;
    (void)fprintf(stderr,"%s: ERROR %s reports empty user-specified chunking string in conjunction with unknown or ambiguous executable name %s\n",prg_nm,fnc_nm,prg_nm);
    nco_exit(EXIT_FAILURE);
  } /* endif */

  if(!strcmp(nco_cnk_plc_sng,"all_xst")) return nco_cnk_plc_all_xst_att;
  if(!strcmp(nco_cnk_plc_sng,"cnk_all_xst_att")) return nco_cnk_plc_all_xst_att;
  if(!strcmp(nco_cnk_plc_sng,"all_new")) return nco_cnk_plc_all_new_att;
  if(!strcmp(nco_cnk_plc_sng,"cnk_all_new_att")) return nco_cnk_plc_all_new_att;
  if(!strcmp(nco_cnk_plc_sng,"xst_new")) return nco_cnk_plc_xst_new_att;
  if(!strcmp(nco_cnk_plc_sng,"cnk_xst_new_att")) return nco_cnk_plc_xst_new_att;
  if(!strcmp(nco_cnk_plc_sng,"uck")) return nco_cnk_plc_uck;
  if(!strcmp(nco_cnk_plc_sng,"unchunk")) return nco_cnk_plc_uck;
  if(!strcmp(nco_cnk_plc_sng,"cnk_uck")) return nco_cnk_plc_uck;

  (void)fprintf(stderr,"%s: ERROR %s reports unknown user-specified chunking policy %s\n",prg_nm_get(),fnc_nm,nco_cnk_plc_sng);
  nco_exit(EXIT_FAILURE);
  return nco_cnk_plc_nil; /* Statement should not be reached */
} /* end nco_cnk_plc_get() */

nco_bool /* O [flg] Chunking policy allows chunking nc_typ_in */
nco_cnk_plc_typ_get /* [fnc] Determine type, if any, to chunk input type to */
(const int nco_cnk_map,  /* I [enm] Chunking map */
 const nc_type nc_typ_in, /* I [enm] Type of input variable */
 nc_type *nc_typ_cnk_out) /* O [enm] Type to chunk variable to */
{
  /* Purpose: Determine type, if any, to chunk input type to
     Routine enforces policy specified by nco_cnk_map
     Replacement for simple deprecated routine nco_is_chunkable()
     There are two cases:
     1. nco_cnk_map allows chunking nc_typ_in:
        Routine returns true and sets nc_typ_cnk_out accordingly
     2. nco_cnk_map does not allow chunking nc_typ_in:
        Routine returns false and sets nc_typ_cnk_out=nc_typ_in
     In both cases, nc_typ_cnk_out is only set if it is non-NULL */

  const char fnc_nm[]="nco_cnk_plc_typ_get()"; /* [sng] Function name */
  nco_bool nco_cnk_plc_alw; /* O [flg] Chunking policy allows chunking nc_typ_in */
  nc_type nc_typ_cnk_out_tmp; /* O [enm] Type to chunk variable to */

  /* Initialize output type to NAT and chunk allow to False to help catch errors */
  nc_typ_cnk_out_tmp=NC_NAT;
  nco_cnk_plc_alw=False;
  switch(nco_cnk_map){ 
  case nco_cnk_map_nil:
    nc_typ_cnk_out_tmp=nc_typ_in; nco_cnk_plc_alw=False; break;
  case nco_cnk_map_hgh_sht:
    switch(nc_typ_in){ 
    case NC_DOUBLE: 
    case NC_FLOAT: 
    case NC_INT64: 
    case NC_UINT64: 
    case NC_INT: 
    case NC_UINT: 
      nc_typ_cnk_out_tmp=NC_SHORT; nco_cnk_plc_alw=True; break;
    case NC_SHORT: 
    case NC_USHORT: 
    case NC_BYTE: 
    case NC_UBYTE: 
    case NC_CHAR: 
    case NC_STRING: 
      nc_typ_cnk_out_tmp=nc_typ_in; nco_cnk_plc_alw=False; break;
    default: nco_dfl_case_nc_type_err(); break;
    } /* end nc_type switch */ 
    break;
  case nco_cnk_map_hgh_chr:
    switch(nc_typ_in){ 
    case NC_DOUBLE: 
    case NC_FLOAT: 
    case NC_INT64: 
    case NC_UINT64: 
    case NC_INT: 
    case NC_UINT: 
    case NC_SHORT: 
    case NC_USHORT: 
      nc_typ_cnk_out_tmp=NC_CHAR; nco_cnk_plc_alw=True; break;
    case NC_BYTE: 
    case NC_UBYTE: 
    case NC_CHAR: 
    case NC_STRING: 
      nc_typ_cnk_out_tmp=nc_typ_in; nco_cnk_plc_alw=False; break;
    default: nco_dfl_case_nc_type_err(); break;
    } /* end nc_type switch */ 
    break;
  case nco_cnk_map_hgh_byt:
    switch(nc_typ_in){ 
    case NC_DOUBLE: 
    case NC_FLOAT: 
    case NC_INT64: 
    case NC_UINT64: 
    case NC_INT: 
    case NC_UINT: 
    case NC_SHORT: 
    case NC_USHORT: 
      nc_typ_cnk_out_tmp=NC_BYTE; nco_cnk_plc_alw=True; break;
    case NC_BYTE: 
    case NC_UBYTE: 
    case NC_CHAR: 
    case NC_STRING: 
      nc_typ_cnk_out_tmp=nc_typ_in; nco_cnk_plc_alw=False; break;
    default: nco_dfl_case_nc_type_err(); break;
    } /* end nc_type switch */ 
    break;
  case nco_cnk_map_nxt_lsr:
    switch(nc_typ_in){ 
    case NC_DOUBLE: 
    case NC_INT64: 
    case NC_UINT64: 
      nc_typ_cnk_out_tmp=NC_INT; nco_cnk_plc_alw=True; break; 
    case NC_FLOAT: 
    case NC_INT: 
    case NC_UINT: 
      nc_typ_cnk_out_tmp=NC_SHORT; nco_cnk_plc_alw=True; break;
    case NC_SHORT: 
    case NC_USHORT: 
      nc_typ_cnk_out_tmp=NC_BYTE; nco_cnk_plc_alw=True; break;
    case NC_BYTE: 
    case NC_UBYTE: 
    case NC_CHAR: 
    case NC_STRING: 
      nc_typ_cnk_out_tmp=nc_typ_in; nco_cnk_plc_alw=False; break;
    default: nco_dfl_case_nc_type_err(); break;
    } /* end nc_type switch */ 
    break;
  case nco_cnk_map_flt_sht:
    switch(nc_typ_in){ 
    case NC_DOUBLE: 
    case NC_FLOAT: 
      nc_typ_cnk_out_tmp=NC_SHORT; nco_cnk_plc_alw=True; break;
    case NC_INT64: 
    case NC_UINT64: 
    case NC_INT: 
    case NC_UINT: 
    case NC_SHORT: 
    case NC_USHORT: 
    case NC_BYTE: 
    case NC_UBYTE: 
    case NC_CHAR: 
    case NC_STRING: 
      nc_typ_cnk_out_tmp=nc_typ_in; nco_cnk_plc_alw=False; break;
    default: nco_dfl_case_nc_type_err(); break;
    } /* end nc_type switch */ 
    break;
  case nco_cnk_map_flt_chr:
    switch(nc_typ_in){ 
    case NC_DOUBLE: 
    case NC_FLOAT: 
      nc_typ_cnk_out_tmp=NC_CHAR; nco_cnk_plc_alw=True; break;
    case NC_INT64: 
    case NC_UINT64: 
    case NC_INT: 
    case NC_UINT: 
    case NC_SHORT: 
    case NC_USHORT: 
    case NC_BYTE: 
    case NC_UBYTE: 
    case NC_CHAR: 
    case NC_STRING: 
      nc_typ_cnk_out_tmp=nc_typ_in; nco_cnk_plc_alw=False; break;
    default: nco_dfl_case_nc_type_err(); break;
    } /* end nc_type switch */ 
    break;
  case nco_cnk_map_flt_byt:
    switch(nc_typ_in){ 
    case NC_DOUBLE: 
    case NC_FLOAT: 
      nc_typ_cnk_out_tmp=NC_BYTE; nco_cnk_plc_alw=True; break;
    case NC_INT64: 
    case NC_UINT64: 
    case NC_INT: 
    case NC_UINT: 
    case NC_SHORT: 
    case NC_USHORT: 
    case NC_BYTE: 
    case NC_UBYTE: 
    case NC_CHAR: 
    case NC_STRING: 
      nc_typ_cnk_out_tmp=nc_typ_in; nco_cnk_plc_alw=False; break;
    default: nco_dfl_case_nc_type_err(); break;
    } /* end nc_type switch */ 
    break;
  default: 
    (void)fprintf(stdout,"%s: ERROR %s reports switch(nco_cnk_map) statement fell through to default case\n",prg_nm_get(),fnc_nm);
    nco_err_exit(0,fnc_nm);
    break;
  } /* end nco_cnk_map switch */ 
  
  /* Only fill in nc_typ_cnk_out if it is non-NULL */
  if(nc_typ_cnk_out != NULL) *nc_typ_cnk_out=nc_typ_cnk_out_tmp;
  
  return nco_cnk_plc_alw; /* O [flg] Chunking policy allows chunking nc_typ_in */
} /* end nco_cnk_plc_typ_get() */

nco_bool /* O [flg] Variable is chunked on disk */
nco_cnk_dsk_inq /* [fnc] Check whether variable is chunked on disk */
(const int nc_id, /* I [idx] netCDF file ID */
 var_sct * const var) /* I/O [sct] Variable */
{
  /* Purpose: Check whether variable is chunked on disk and set variable members 
     cnk_dsk, has_scl_fct, has_add_fst, and typ_uck accordingly
     nco_cnk_dsk_inq() should be called early in application, e.g., in nco_var_fll() 
     Call nco_cnk_dsk_inq() before copying input list to output list 
     Multi-file operators which handle chunking must call this routine prior
     to each read of a variable, in case that variable has been unchunked. */
  /* ncea -O -D 3 -v cnk ~/nco/data/in.nc ~/nco/data/foo.nc */
  
  const char add_fst_sng[]="add_offset"; /* [sng] Unidata standard string for add offset */
  const char scl_fct_sng[]="scale_factor"; /* [sng] Unidata standard string for scale factor */
  
  int rcd; /* [rcd] Return success code */
  
  long add_fst_lng; /* [idx] Number of elements in add_offset attribute */
  long scl_fct_lng; /* [idx] Number of elements in scale_factor attribute */

  nc_type add_fst_typ; /* [idx] Type of add_offset attribute */
  nc_type scl_fct_typ; /* [idx] Type of scale_factor attribute */

  /* Set some defaults in variable structure for safety in case of early return
     Flags for variables without valid scaling information should appear 
     same as flags for variables with _no_ scaling information
     Set has_scl_fct, has_add_fst in var_dfl_set()
     typ_uck:
     1. is required by ncra nco_cnv_mss_val_typ() 
     2. depends on var->type and so should not be set in var_dfl_set()
     3. is therefore set to default here */
  var->typ_uck=var->type; /* [enm] Type of variable when unchunked (expanded) (in memory) */

  /* Vet scale_factor */
  rcd=nco_inq_att_flg(nc_id,var->id,scl_fct_sng,&scl_fct_typ,&scl_fct_lng);
  if(rcd != NC_ENOTATT){
    if(scl_fct_typ == NC_BYTE || scl_fct_typ == NC_CHAR){
      if(dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stdout,"%s: WARNING nco_cnk_dsk_inq() reports scale_factor for %s is NC_BYTE or NC_CHAR. Will not attempt to unchunk using scale_factor.\n",prg_nm_get(),var->nm); 
      return False;
    } /* endif */
    if(scl_fct_lng != 1){
      if(dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stdout,"%s: WARNING nco_cnk_dsk_inq() reports %s has scale_factor of length %li. Will not attempt to unchunk using scale_factor\n",prg_nm_get(),var->nm,scl_fct_lng); 
      return False;
    } /* endif */
    var->has_scl_fct=True; /* [flg] Valid scale_factor attribute exists */
    var->typ_uck=scl_fct_typ; /* [enm] Type of variable when unchunked (expanded) (in memory) */
  } /* endif */

  /* Vet add_offset */
  rcd=nco_inq_att_flg(nc_id,var->id,add_fst_sng,&add_fst_typ,&add_fst_lng);
  if(rcd != NC_ENOTATT){
    if(add_fst_typ == NC_BYTE || add_fst_typ == NC_CHAR){
      if(dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stdout,"%s: WARNING nco_cnk_dsk_inq() reports add_offset for %s is NC_BYTE or NC_CHAR. Will not attempt to unchunk using add_offset.\n",prg_nm_get(),var->nm); 
      return False;
    } /* endif */
    if(add_fst_lng != 1){
      if(dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stdout,"%s: WARNING nco_cnk_dsk_inq() reports %s has add_offset of length %li. Will not attempt to unchunk.\n",prg_nm_get(),var->nm,add_fst_lng); 
      return False;
    } /* endif */
    var->has_add_fst=True; /* [flg] Valid add_offset attribute exists */
    var->typ_uck=add_fst_typ; /* [enm] Type of variable when unchunked (expanded) (in memory) */
  } /* endif */

  if(var->has_scl_fct && var->has_add_fst){
    if(scl_fct_typ != add_fst_typ){
      if(dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stdout,"%s: WARNING nco_cnk_dsk_inq() reports type of scale_factor does not equal type of add_offset. Will not attempt to unchunk.\n",prg_nm_get());
      return False;
    } /* endif */
  } /* endif */

  if(var->has_scl_fct || var->has_add_fst){
    /* Variable is considered chunked iff either or both valid scale_factor or add_offset exist */
    var->cnk_dsk=True; /* [flg] Variable is chunked on disk */
    /* If variable is chunked on disk and is in memory then variable is chunked in memory */
    var->cnk_ram=True; /* [flg] Variable is chunked in memory */
    var->typ_uck=(var->has_scl_fct) ? scl_fct_typ : add_fst_typ; /* [enm] Type of variable when unchunked (expanded) (in memory) */
    if(nco_is_rth_opr(prg_get()) && dbg_lvl_get() >= nco_dbg_var){
      (void)fprintf(stdout,"%s: CHUNKING Variable %s is type %s chunked into type %s\n",prg_nm_get(),var->nm,nco_typ_sng(var->typ_uck),nco_typ_sng(var->typ_dsk));
      (void)fprintf(stdout,"%s: DEBUG Chunked variables processed by all arithmetic operators are unchunked automatically, and then stored unchunked in the output file. If you wish to rechunk them in the output file, use, e.g., ncap -O -s \"foo=chunk(foo);\" out.nc out.nc. If you wish to chunk all variables in a file, use, e.g., ncpdq -P all_new in.nc out.nc.\n",prg_nm_get());
    } /* endif print chunking information */
  }else{
    /* Variable is not chunked since neither scale factor nor add_offset exist
       Insert hooks which depend on variable not being chunked here
       Currently this is no-op */
    ;
  } /* end else */

  return var->cnk_dsk; /* [flg] Variable is chunked on disk (valid scale_factor, add_offset, or both attributes exist) */
  
} /* end nco_cnk_dsk_inq() */

void
nco_cnk_mtd /* [fnc] Alter metadata according to chunking specification */
(const var_sct * const var_in, /* I [ptr] Variable in original disk state */
 var_sct * const var_out, /* I/O [ptr] Variable whose metadata will be altered */
 const int nco_cnk_map,  /* I [enm] Chunking map */
 const int nco_cnk_plc)  /* I [enm] Chunking policy */
{
  /* Purpose: Alter metadata according to chunking specification */
  const char fnc_nm[]="nco_cnk_mtd()"; /* [sng] Function name */
  nc_type nc_typ_cnk_out; /* [enm] Type to chunk to */
  
  switch(nco_cnk_plc){
  case nco_cnk_plc_all_xst_att:
    /* If variable is already chunked do nothing otherwise chunk to default type */
    if(var_in->cnk_ram){
      if(dbg_lvl_get() >= nco_dbg_var) (void)fprintf(stdout,"%s: DEBUG %s keeping existing chunking parameters and type (%s) for %s\n",prg_nm_get(),fnc_nm,nco_typ_sng(var_in->type),var_in->nm);
    }else{
      goto var_uck_try_to_cnk;
    } /* endif */
    break;
  case nco_cnk_plc_xst_new_att:
    /* If variable is already chunked then re-chunk otherwise do nothing */
    if(var_in->cnk_ram){
      goto var_cnk_try_to_rpk;
    }else{
      /* Variable is not chunked so do nothing */
      if(dbg_lvl_get() >= nco_dbg_var) (void)fprintf(stdout,"%s: INFO %s leaving variable %s of type %s as unchunked\n",prg_nm_get(),fnc_nm,var_in->nm,nco_typ_sng(var_out->typ_uck));
    } /* endelse */
    break;
  case nco_cnk_plc_all_new_att:
    if(var_in->cnk_ram){
      goto var_cnk_try_to_rpk;
    }else{
      goto var_uck_try_to_cnk;
    } /* endif */
    break;
  case nco_cnk_plc_uck:
    var_out->type=var_in->typ_uck;
    if(dbg_lvl_get() >= nco_dbg_sbr){
      if(var_in->cnk_ram) (void)fprintf(stdout,"%s: DEBUG %s will unchunk variable %s from %s to %s\n",prg_nm_get(),fnc_nm,var_in->nm,nco_typ_sng(var_in->type),nco_typ_sng(var_out->type)); else (void)fprintf(stdout,"%s: DEBUG %s variable %s is already unchunked and of type %s\n",prg_nm_get(),fnc_nm,var_in->nm,nco_typ_sng(var_in->type)); 
    } /* endif dbg */
    break;
  case nco_cnk_plc_nil:
  default: nco_dfl_case_cnk_plc_err(); break;
  } /* end case */

  /* Return after finishing switch() statement and before falling through
     to code-saving goto branches */
  return;

 var_uck_try_to_cnk: /* end goto */
  /* Variable is not yet chunked---try to chunk it */
  if(nco_cnk_plc_typ_get(nco_cnk_map,var_in->type,&nc_typ_cnk_out)){
    var_out->type=nc_typ_cnk_out;
    if(dbg_lvl_get() >= nco_dbg_sbr) (void)fprintf(stdout,"%s: DEBUG %s will chunk variable %s from %s to %s\n",prg_nm_get(),fnc_nm,var_in->nm,nco_typ_sng(var_in->type),nco_typ_sng(var_out->type));
  }else{
    if(dbg_lvl_get() >= nco_dbg_var) (void)fprintf(stdout,"%s: INFO %s chunking policy %s with chunking map %s does not allow chunking variable %s of type %s, skipping...\n",prg_nm_get(),fnc_nm,nco_cnk_plc_sng_get(nco_cnk_plc),nco_cnk_map_sng_get(nco_cnk_map),var_in->nm,nco_typ_sng(var_in->type));
  } /* !nco_cnk_plc_alw */
  return;
  
 var_cnk_try_to_rpk: /* end goto */
  /* Variable is already chunked---try to re-chunk it 
     Final chunked variable type may differ from original */
  if(nco_cnk_plc_typ_get(nco_cnk_map,var_in->typ_uck,&nc_typ_cnk_out)){
    var_out->type=nc_typ_cnk_out;
    if(dbg_lvl_get() >= nco_dbg_sbr) (void)fprintf(stdout,"%s: DEBUG %s will re-chunk variable %s of expanded type %s from current chunking (type %s) into new chunking of type %s\n",prg_nm_get(),fnc_nm,var_in->nm,nco_typ_sng(var_in->typ_uck),nco_typ_sng(var_in->type),nco_typ_sng(var_out->type));
  }else{
    if(dbg_lvl_get() >= nco_dbg_var) (void)fprintf(stdout,"%s: WARNING %s variable %s of expanded type %s is already chunked into type %s and re-chunking is requested but chunking policy %s and chunking map %s does not allow re-chunking variables of type %s\n",prg_nm_get(),fnc_nm,var_in->nm,nco_typ_sng(var_in->typ_uck),nco_typ_sng(var_in->type),nco_cnk_plc_sng_get(nco_cnk_plc),nco_cnk_map_sng_get(nco_cnk_map),nco_typ_sng(var_in->typ_uck));
  } /* !nco_cnk_plc_alw */
  return;

} /* end nco_cnk_mtd() */

void
nco_cnk_val /* [fnc] Chunk variable according to chunking specification */
(var_sct * const var_in, /* I [ptr] Variable in original disk state */
 var_sct * var_out, /* I/O [ptr] Variable after chunking/unchunking operation */
 const int nco_cnk_map,  /* I [enm] Chunking map */
 const int nco_cnk_plc,  /* I [enm] Chunking policy */
 aed_sct * const aed_lst_add_fst, /* O [enm] Attribute edit structure, add_offset */
 aed_sct * const aed_lst_scl_fct) /* O [enm] Attribute edit structure, scale_factor */
{
  /* Purpose: Alter metadata according to chunking specification */
  const char fnc_nm[]="nco_cnk_val()"; /* [sng] Function name */
  nco_bool CNK_VAR_WITH_NEW_CNK_ATT=False; /* [flg] Insert new scale_factor and add_offset into lists */
  nc_type typ_out; /* [enm] Type in output file */
  
  /* typ_out contains type of variable defined in output file
     as defined by var_out->type which was set in var_cnk_mtd() 
     We will temporarily set var_out->type to RAM type of variable
     Chunking routine will re-set var_out->type to typ_out if necessary */
  typ_out=var_out->type; /* [enm] Type in output file */
  
  switch(nco_cnk_plc){
  case nco_cnk_plc_all_xst_att:
    /* nco_var_cnk() expects to alter var_out->type itself, if necessary */
    var_out->type=var_in->typ_dsk;
    if(var_in->cnk_ram){
      if(dbg_lvl_get() >= nco_dbg_sbr) (void)fprintf(stdout,"%s: INFO %s keeping existing chunking attributes for variable %s\n",prg_nm_get(),fnc_nm,var_in->nm);
      /* Warn if chunking attribute values are in memory for pre-chunked variables */
      if(var_out->scl_fct.vp != NULL || var_out->add_fst.vp != NULL) (void)fprintf(stdout,"%s: WARNING %s reports variable %s has chunking attribute values in memory. This is not supposed to happen through known code paths, but is not necessarily dangerous.\n",prg_nm_get(),fnc_nm,var_in->nm);
      /* Remove dangling pointer, see explanation below */
      var_in->val.vp=NULL; 
    }else{
      goto var_uck_try_to_cnk; /* end goto */
    } /* endif input variable was not chunked */
    break;
  case nco_cnk_plc_xst_new_att:
    if(var_in->cnk_ram){
      nco_var_uck_swp(var_in,var_out);
      goto var_uck_try_to_cnk;
    }else{
      /* Remove dangling pointer, see explanation below */
      var_in->val.vp=NULL; 
    } /* endif */
    break;
  case nco_cnk_plc_all_new_att:
    if(var_in->cnk_ram){
      /* Variable is already chunked---unchunk it before re-chunking it */
      nco_var_uck_swp(var_in,var_out);
    }else{
      /* nco_var_cnk() expects to alter var_out->type itself, if necessary */
      var_out->type=var_in->typ_dsk;
    } /* endif */
    goto var_uck_try_to_cnk; 
    break;
  case nco_cnk_plc_uck:
    /* Unchunk if possible, otherwise remove dangling pointer (explanation below) */
    if(var_in->cnk_ram) nco_var_uck_swp(var_in,var_out); else var_in->val.vp=NULL; 
    break;
  case nco_cnk_plc_nil:
  default: nco_dfl_case_cnk_plc_err(); break;
  } /* end case */

  /* Ensure code goes to final block before falling through to next goto */
  goto put_new_cnk_att_in_lst;

 var_uck_try_to_cnk: /* end goto */
  /* Variable is not yet chunked---try to chunk it */
  if(nco_cnk_plc_typ_get(nco_cnk_map,var_out->type,(nc_type *)NULL)){
    if(dbg_lvl_get() >= nco_dbg_sbr) (void)fprintf(stdout,"%s: INFO %s chunking variable %s values from %s to %s\n",prg_nm_get(),fnc_nm,var_in->nm,nco_typ_sng(var_out->typ_uck),nco_typ_sng(typ_out));
    var_out=nco_var_cnk(var_out,typ_out,&CNK_VAR_WITH_NEW_CNK_ATT);
  }else{
    if(dbg_lvl_get() >= nco_dbg_var) (void)fprintf(stdout,"%s: INFO %s chunking policy %s with chunking map %s does not allow chunking variable %s of type %s, skipping...\n",prg_nm_get(),fnc_nm,nco_cnk_plc_sng_get(nco_cnk_plc),nco_cnk_map_sng_get(nco_cnk_map),var_in->nm,nco_typ_sng(var_out->typ_uck));
  } /* !nco_cnk_plc_alw */ 
  /* Chunking function nco_var_cnk() usually free()'s var_out->val.vp 
     Hence var_in->val.vp is left with a dangling pointer
     In ncpdq, var_in->val.vp and var_out->val.vp point to same buffer 
     This reduces peak memory consumption by ~50%, but is dangerous */
  var_in->val.vp=NULL; 
  /* Ensure code goes to final block before falling through to next goto */
  goto put_new_cnk_att_in_lst;
  
 put_new_cnk_att_in_lst: /* end goto */
  /* Fill attribute edit structures
     Use values directly from variable structures rather than copying
     Attribute structure dynamic memory will be free()'d in nco_var_free() call */
  if(CNK_VAR_WITH_NEW_CNK_ATT){
    aed_lst_add_fst->var_nm=aed_lst_scl_fct->var_nm=var_out->nm;
    aed_lst_add_fst->id=aed_lst_scl_fct->id=var_out->id;
    aed_lst_add_fst->sz=aed_lst_scl_fct->sz=1L;
    aed_lst_add_fst->type=aed_lst_scl_fct->type=var_out->typ_uck;
    /* Chunking generates at least one of scale_factor or add_offset,
       but not necessarily both.
       Delete pre-defined attributes for those which were not created */
    if(var_out->has_add_fst) aed_lst_add_fst->mode=aed_overwrite; else aed_lst_add_fst->mode=aed_delete;
    if(var_out->has_scl_fct) aed_lst_scl_fct->mode=aed_overwrite; else aed_lst_scl_fct->mode=aed_delete;
    /* Insert values into attribute structures */
    aed_lst_add_fst->val=var_out->add_fst;
    aed_lst_scl_fct->val=var_out->scl_fct;
  } /* endif */

} /* end nco_cnk_val() */

var_sct * /* O [sct] Chunked variable */
nco_put_var_cnk /* [fnc] Chunk variable in memory and write chunking attributes to disk */
(const int out_id, /* I [id] netCDF output file ID */
 var_sct *var, /* I/O [sct] Variable to be chunked */
 const int nco_cnk_plc) /* [enm] Chunking operation type */
{
  /* Purpose: Chunk variable in memory and write chunking attributes to disk
     NB: Routine is not complete, debugged, or currently used 
     ncpdq breaks up writing chunked variables into multiple tasks, i.e.,
     ncpdq separates variable value writes from chunking attribute value writes.
     This routine is intended to write a chunked variable in one routine */
  nco_bool CNK_VAR_WITH_NEW_CNK_ATT=False; /* [flg] Insert new scale_factor and add_offset into lists */
  
  switch(nco_cnk_plc){
  case nco_cnk_plc_all_xst_att:
    break;
  case nco_cnk_plc_xst_new_att:
    break;
  case nco_cnk_plc_all_new_att:
    break;
  case nco_cnk_plc_uck:
    break;
  case nco_cnk_plc_nil:
  default: nco_dfl_case_cnk_plc_err(); break;
  } /* end switch */

  /* Chunk variable */
  if(var->xrf->cnk_dsk && !var->xrf->cnk_ram) var=nco_var_cnk(var,var->typ_cnk,&CNK_VAR_WITH_NEW_CNK_ATT);

  /* Write/overwrite scale_factor and add_offset attributes */
  if(var->cnk_ram){ /* [flg] Variable is chunked in memory */
    if(var->has_scl_fct){ /* [flg] Valid scale_factor attribute exists */
      (void)nco_put_att(out_id,var->id,"scale_factor",var->typ_uck,1,var->scl_fct.vp);
    } /* endif has_scl_fct */
    if(var->has_add_fst){ /* [flg] Valid add_offset attribute exists */
      (void)nco_put_att(out_id,var->id,"add_offset",var->typ_uck,1,var->add_fst.vp);
    } /* endif has_add_fst */
  } /* endif cnk_ram */
  
  return var;
  
} /* end nco_put_var_cnk() */

var_sct * /* O [sct] Chunked variable */
nco_var_cnk /* [fnc] Chunk variable in memory */
(var_sct *var, /* I/O [sct] Variable to be chunked */
 const nc_type nc_typ_cnk, /* I [enm] Type of variable when chunked (on disk). This should be same as typ_dsk except in cases where variable is chunked in input file and unchunked in output file. */
 nco_bool *CNK_VAR_WITH_NEW_CNK_ATT) /* O [flg] Routine generated new scale_factor/add_offset */
{
  /* Purpose: Chunk variable 
     Routine is inverse of nco_var_uck(): nco_var_cnk[nco_var_uck(var)]=var 
     Currently routine outputs same variable structure as given on input
     In other words, output structure may be neglected as all changes are made 
     to input structure.
     NB: Value buffer var->val.vp is usually free()'d here
     Variables in calling routine which point to var->val.vp will be left dangling */

  nco_bool PURE_MSS_VAL_FLD=False; /* [flg] Field is pure missing_value, i.e., no valid values */
  const char fnc_nm[]="nco_var_cnk()"; /* [sng] Function name */
  double scl_fct_dbl=double_CEWI; /* [sct] Double precision value of scale_factor */
  double add_fst_dbl=double_CEWI; /* [sct] Double precision value of add_offset */

  /* Set flag true once new scale_factor/add_offset generated */
  *CNK_VAR_WITH_NEW_CNK_ATT=False;

  /* Return if variable in memory is currently chunked and should not be re-chunked */
  if(var->cnk_ram) return var;

  /* Routine should be called with variable already in memory */
  if(var->val.vp == NULL) (void)fprintf(stdout,"%s: ERROR %s called with empty var->val.vp\n",prg_nm_get(),fnc_nm);
  
  /* Chunked type must be NC_BYTE, NC_CHAR, NC_SHORT, or NC_INT */
  if(nc_typ_cnk == NC_FLOAT || nc_typ_cnk == NC_DOUBLE || 
     nc_typ_cnk == NC_UINT  || nc_typ_cnk == NC_USHORT || 
     nc_typ_cnk == NC_UBYTE || nc_typ_cnk == NC_STRING){
    (void)fprintf(stdout,"%s: ERROR %s called to chunk variable %s with invalid chunked type nc_typ_cnk = %s\n",prg_nm_get(),fnc_nm,var->nm,nco_typ_sng(nc_typ_cnk));
    nco_exit(EXIT_FAILURE);
  } /* endif */

  /* Variable must be chunkable (usually NC_INT, NC_FLOAT, or NC_DOUBLE)
     Definition of "chunkable" determined by nco_cnk_plc_typ_get()
     Prefer not to make nco_var_cnk() rely directly on nco_cnk_plc_typ_get()
     However, certain types are never chunkable */
  if(var->type == NC_BYTE || var->type == NC_UBYTE || var->type == NC_CHAR || var->type == NC_STRING){
    (void)fprintf(stdout,"%s: ERROR %s is asked to chunk variable %s of type %s\n",prg_nm_get(),fnc_nm,var->nm,nco_typ_sng(var->type));
    nco_exit(EXIT_FAILURE);
  } /* endif */

  if(True){ /* Keep in own scope for eventual functionalization of core chunking algorithm */
    /* Compute chunking parameters to apply to var

       Linear chunking in a nutshell:
       scale_factor = (max-min)/ndrv <--> (max-min)/scale_factor = ndrv <--> scale_factor*ndrv = max-min
       add_offset = 0.5*(min+max)
       cnk = (uck-add_offset)/scale_factor = (uck-0.5*(min+max))*ndrv/(max-min)
       uck = scale_factor*cnk + add_offset = (max-min)*cnk/ndrv + 0.5*(min+max) 

       where 

       ndrv = number of discrete representable values for given type of chunked variable and
       ndrv = 256 iff var->typ_cnk == NC_CHAR
       ndrv = 256*256 iff var->typ_cnk == NC_SHORT
       ndrv = 256*256*256*256 = 2^32 iff var->typ_cnk == NC_INT */

    const double max_mns_min_dbl_wrn=1.0e10; /* [frc] Threshold value for warning */
    double ndrv_dbl=double_CEWI; /* [frc] Double precision value of number of discrete representable values */
    double max_mns_min_dbl; /* [frc] Maximum value minus minimum value */

    ptr_unn ptr_unn_min; /* [ptr] Pointer union to minimum value of variable */
    ptr_unn ptr_unn_max; /* [ptr] Pointer union to maximum value of variable */
    ptr_unn ptr_unn_mss_val_dbl; /* [ptr] Pointer union to missing value of variable */
    
    var_sct *min_var; /* [sct] Minimum value of variable */
    var_sct *max_var; /* [sct] Maximum value of variable */
    var_sct *max_var_dpl; /* [sct] Copy of Maximum value of variable */
    var_sct *hlf_var; /* [sct] NCO variable for value 0.5 */
    var_sct *zero_var; /* [sct] NCO variable for value 0.0 */
    var_sct *ndrv_var; /* [sct] NCO variable for number of discrete representable values */
    
    val_unn hlf_unn; /* [frc] Generic container for value 0.5 */
    val_unn zero_unn; /* [frc] Generic container for value 0.0 */
    val_unn ndrv_unn; /* [nbr] Generic container for number of discrete representable values */

    /* Initialize data */
    hlf_unn.d=0.5; /* Generic container for value 0.5 */
    zero_unn.d=0.0; /* Generic container for value 0.0 */

    /* Derive scalar values for scale_factor and add_offset */
    var->scl_fct.vp=nco_free(var->scl_fct.vp);
    var->add_fst.vp=nco_free(var->add_fst.vp);
    var->scl_fct.vp=(void *)nco_malloc(nco_typ_lng(var->type));
    var->add_fst.vp=(void *)nco_malloc(nco_typ_lng(var->type));
    ptr_unn_min.vp=(void *)nco_malloc(nco_typ_lng(var->type));
    ptr_unn_max.vp=(void *)nco_malloc(nco_typ_lng(var->type));

    /* Create double precision missing_value for use in min/max arithmetic */
    if(var->has_mss_val){
      ptr_unn_mss_val_dbl.vp=(void *)nco_malloc(nco_typ_lng((nc_type)NC_DOUBLE));
      (void)nco_val_cnf_typ(var->type,var->mss_val,(nc_type)NC_DOUBLE,ptr_unn_mss_val_dbl);
    } /* endif has_mss_val */

    /* Find minimum and maximum values in data */
    (void)nco_var_avg_rdc_max(var->type,var->sz,1L,var->has_mss_val,var->mss_val,var->val,ptr_unn_min);
    (void)nco_var_avg_rdc_min(var->type,var->sz,1L,var->has_mss_val,var->mss_val,var->val,ptr_unn_max);

    /* Convert to NC_DOUBLE before 0.5*(min+max) operation */
    min_var=scl_ptr_mk_var(ptr_unn_min,var->type);
    min_var=nco_var_cnf_typ((nc_type)NC_DOUBLE,min_var);
    max_var=scl_ptr_mk_var(ptr_unn_max,var->type);
    max_var=nco_var_cnf_typ((nc_type)NC_DOUBLE,max_var);
    /* Copy max_var for use in scale_factor computation */
    max_var_dpl=nco_var_dpl(max_var);
    hlf_var=scl_mk_var(hlf_unn,NC_DOUBLE); /* [sct] NCO variable for value one half */

    /* Field is pure missing_value iff either extrema is missing_value */
    if(var->has_mss_val)
      if(min_var->val.dp[0] == ptr_unn_mss_val_dbl.dp[0]) 
	PURE_MSS_VAL_FLD=True;

    /* Change value of missing value iff necessary to fit inside chunked type */
    if(var->has_mss_val && !PURE_MSS_VAL_FLD){
      double mss_val_dfl_dbl=0.0; /* CEWI */
      switch(nc_typ_cnk){ 
      case NC_FLOAT: mss_val_dfl_dbl=NC_FILL_FLOAT; break; 
      case NC_DOUBLE: mss_val_dfl_dbl=NC_FILL_DOUBLE; break; 
      case NC_INT: mss_val_dfl_dbl=NC_FILL_INT; break;
      case NC_SHORT: mss_val_dfl_dbl=NC_FILL_SHORT; break;
      case NC_USHORT: mss_val_dfl_dbl=NC_FILL_USHORT; break;
      case NC_UINT: mss_val_dfl_dbl=NC_FILL_UINT; break;
      case NC_INT64: mss_val_dfl_dbl=NC_FILL_INT64; break;
      case NC_UINT64: mss_val_dfl_dbl=NC_FILL_UINT64; break;
      case NC_BYTE: mss_val_dfl_dbl=NC_FILL_BYTE; break;
      case NC_UBYTE: mss_val_dfl_dbl=NC_FILL_UBYTE; break;
      case NC_CHAR: mss_val_dfl_dbl=NC_FILL_CHAR; break;
      case NC_STRING: break; /* Do nothing */
      default: nco_dfl_case_nc_type_err(); break;
      } /* end switch */ 
      if(dbg_lvl_get() >= nco_dbg_io) (void)fprintf(stdout,"%s: %s mss_val_dfl = %g\n",prg_nm_get(),fnc_nm,mss_val_dfl_dbl);
    } /* endif */

    if(dbg_lvl_get() >= nco_dbg_io) (void)fprintf(stdout,"%s: %s: min_var = %g, max_var = %g\n",prg_nm_get(),var->nm,min_var->val.dp[0],max_var->val.dp[0]);

    /* add_offset is 0.5*(min+max) */
    /* max_var->val is overridden with add_offset answers, no longer valid as max_var */
    (void)nco_var_add((nc_type)NC_DOUBLE,1L,var->has_mss_val,ptr_unn_mss_val_dbl,min_var->val,max_var->val);
    (void)nco_var_mlt((nc_type)NC_DOUBLE,1L,var->has_mss_val,ptr_unn_mss_val_dbl,hlf_var->val,max_var->val);
    /* Contents of max_var are actually add_offset */
    (void)nco_val_cnf_typ((nc_type)NC_DOUBLE,max_var->val,var->type,var->add_fst);

    /* ndrv is 2^{bits per chunked value} where bppv = 8 for NC_CHAR and bppv = 16 for NC_SHORT
       Subtract one to leave slop for rounding errors
       Subtract two to leave room for missing_value? */
    if(nc_typ_cnk == NC_BYTE || nc_typ_cnk == NC_CHAR){
      ndrv_dbl=256.0-1.0; /* [sct] Double precision value of number of discrete representable values */
    }else if(nc_typ_cnk == NC_SHORT){
      ndrv_dbl=65536.0-1.0; /* [sct] Double precision value of number of discrete representable values */
    }else if(nc_typ_cnk == NC_INT){
      ndrv_dbl=4294967295.0-1.0; /* [sct] Double precision value of number of discrete representable values */
    } /* end else */
    ndrv_unn.d=ndrv_dbl; /* Generic container for number of discrete representable values */
    ndrv_var=scl_mk_var(ndrv_unn,NC_DOUBLE); /* [sct] Variable structure for number of discrete representable values */

    /* scale_factor is (max-min)/ndrv
       If max-min = 0 then variable is constant value so scale_factor=0.0 and add_offset=var
       If max-min > ndrv then precision is worse than 1.0
       If max-min < ndrv then precision is better than 1.0 */
    (void)nco_var_sbt((nc_type)NC_DOUBLE,1L,var->has_mss_val,ptr_unn_mss_val_dbl,min_var->val,max_var_dpl->val);
    /* max-min is currently stored in max_var_dpl */
    max_mns_min_dbl=ptr_unn_2_scl_dbl(max_var_dpl->val,max_var_dpl->type); 

    /* Manually set max-min=0.0 for pure missing_value fields to set add_offset correctly */
    if(PURE_MSS_VAL_FLD) max_mns_min_dbl=0.0;

    if(max_mns_min_dbl != 0.0){
      (void)nco_var_dvd((nc_type)NC_DOUBLE,1L,var->has_mss_val,ptr_unn_mss_val_dbl,ndrv_var->val,max_var_dpl->val);
      /* Contents of max_var_dpl are actually scale_factor */
      (void)nco_val_cnf_typ((nc_type)NC_DOUBLE,max_var_dpl->val,var->type,var->scl_fct);
    }else{
      /* Variable is constant, i.e., equal values everywhere */
      zero_var=scl_mk_var(zero_unn,var->type); /* [sct] NCO variable for value 0.0 */
      /* Set scale_factor to 0.0 */
      (void)memcpy(var->scl_fct.vp,zero_var->val.vp,nco_typ_lng(var->type));
      if(zero_var != NULL) zero_var=nco_var_free(zero_var);
      /* Set add_offset to first variable value 
	 Variable is constant everywhere so particular value copied is unimportant */
      (void)memcpy(var->add_fst.vp,var->val.vp,nco_typ_lng(var->type));
    } /* end else */

    if(max_mns_min_dbl > max_mns_min_dbl_wrn){
      if(dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stdout,"%s: WARNING %s reports data range of variable %s is = %g. The linear data chunking technique defined by netCDF's chunking convention and implemented by NCO result in significant precision loss over such a great range.\n",prg_nm_get(),fnc_nm,var->nm,max_mns_min_dbl);
      if(dbg_lvl_get() >= nco_dbg_std) if(var->has_mss_val) (void)fprintf(stdout,"%s: HINT variable %s has %s = %g. Consider specifying new %s to reduce range of data needing chunking. See http://nco.sf.net/nco.html#ncatted for examples of how to change the %s attribute.\n",prg_nm_get(),var->nm,nco_mss_val_sng_get(),ptr_unn_mss_val_dbl.dp[0],nco_mss_val_sng_get(),nco_mss_val_sng_get());
    } /* endif large data range */

    /* Free minimum and maximum values */
    ptr_unn_min.vp=nco_free(ptr_unn_min.vp);
    ptr_unn_max.vp=nco_free(ptr_unn_max.vp);

    /* Free temporary double missing_value */
    if(var->has_mss_val) ptr_unn_mss_val_dbl.vp=nco_free(ptr_unn_mss_val_dbl.vp);

    /* Free variables */
    if(min_var != NULL) min_var=nco_var_free(min_var);
    if(max_var != NULL) max_var=nco_var_free(max_var);
    if(max_var_dpl != NULL) max_var_dpl=nco_var_free(max_var_dpl);
    if(hlf_var != NULL) hlf_var=nco_var_free(hlf_var);
    if(ndrv_var != NULL) ndrv_var=nco_var_free(ndrv_var);

    /* Do not bother creating superfluous scale_factor (0.0 or 1.0) or add_offset (0.0) */
    scl_fct_dbl=ptr_unn_2_scl_dbl(var->scl_fct,var->type); 
    add_fst_dbl=ptr_unn_2_scl_dbl(var->add_fst,var->type);

    if(scl_fct_dbl != 0.0 && scl_fct_dbl != 1.0) var->has_scl_fct=True; /* [flg] Valid scale_factor attribute exists */
    if(add_fst_dbl != 0.0) var->has_add_fst=True; /* [flg] Valid add_offset attribute exists */

    /* However, must create either scale_factor or add_offset
       Otherwise, routine fails to chunk field uniformly equal to zero (0.0)
       In zero corner case, create add_offset (avoids division by zero problems) */
    if(scl_fct_dbl == 0.0 && add_fst_dbl == 0.0) var->has_add_fst=True; 

  } /* endif True */

  /* Create double precision value of scale_factor for diagnostics */
  if(var->has_scl_fct){ /* [flg] Valid scale_factor attribute exists */
    scl_fct_dbl=ptr_unn_2_scl_dbl(var->scl_fct,var->type); 
    if(scl_fct_dbl == 0.0 && dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stdout,"%s: WARNING %s reports scl_fct_dbl = 0.0\n",prg_nm_get(),fnc_nm);
  } /* endif */
  
  /* Create double precision value of add_offset for diagnostics */
  if(var->has_add_fst){ /* [flg] Valid add_offset attribute exists */
    add_fst_dbl=ptr_unn_2_scl_dbl(var->add_fst,var->type);
  } /* endif */
  
  if(dbg_lvl_get() >= nco_dbg_io) (void)fprintf(stdout,"%s: %s reports variable %s has scl_fct_dbl = %g, add_fst_dbl = %g\n",prg_nm_get(),fnc_nm,var->nm,scl_fct_dbl,add_fst_dbl);
  
  /* Chunking attributes now exist and are same type as variable in memory */

  /* Apply scale_factor and add_offset to reduce variable size
     add_offset and scale_factor are always scalars so use var_scv_* functions
     var_scv_[sub,multiply] functions avoid cost of broadcasting attributes and doing element-by-element operations 
     Using var_scv_[sub,multiply] instead of ncap_var_scv_[sub,multiply] avoids cost of deep copies
     Moreover, this keeps variable structure from changing (because ncap_var_scv_* functions all do deep copies before operations) and thus complicating memory management */
  if(var->has_add_fst){ /* [flg] Valid add_offset attribute exists */
    nco_bool has_mss_val_tmp; /* [flg] Temporary missing_value flag */

    /* Subtract add_offset from var */
    scv_sct add_fst_scv;
    add_fst_scv.type=NC_DOUBLE;
    add_fst_scv.val.d=add_fst_dbl;
    (void)nco_scv_cnf_typ(var->type,&add_fst_scv);
    /* Pass temporary missing_value flag to accomodate pure missing_value fields */
    has_mss_val_tmp=var->has_mss_val;
    /* Dupe var_scv_sub() into subtracting missing values when all values are missing */
    if(PURE_MSS_VAL_FLD){
      has_mss_val_tmp=False;
      if(dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stdout,"%s: INFO %s reports variable %s is filled completely with %s = %g. Why do you store variables with no valid values?\n",prg_nm_get(),fnc_nm,var->nm,nco_mss_val_sng_get(),add_fst_dbl);
    } /* !PURE_MSS_VAL_FLD */
    (void)var_scv_sub(var->type,var->sz,has_mss_val_tmp,var->mss_val,var->val,&add_fst_scv);
  } /* endif */

  if(var->has_scl_fct){ /* [flg] Valid scale_factor attribute exists */
    /* Divide var by scale_factor */
    scv_sct scl_fct_scv;
    scl_fct_scv.type=NC_DOUBLE;
    scl_fct_scv.val.d=scl_fct_dbl;
    (void)nco_scv_cnf_typ(var->type,&scl_fct_scv);
    if(scl_fct_dbl != 0.0) (void)var_scv_dvd(var->type,var->sz,var->has_mss_val,var->mss_val,var->val,&scl_fct_scv);
  } /* endif */

  if(!var->has_scl_fct && !var->has_add_fst){
    (void)fprintf(stderr,"%s: ERROR Reached end of %s without chunking variable\n",prg_nm_get(),fnc_nm);
    nco_exit(EXIT_FAILURE);
  }else{
    *CNK_VAR_WITH_NEW_CNK_ATT=True; /* O [flg] Routine generated new scale_factor/add_offset */
  } /* endif */

  /* Tell the world we chunked the variable
     This is true if input variable satisfied nco_cnk_plc_typ_get() criteria
     Variables that fail nco_cnk_plc_typ_get() (e.g., type == NC_CHAR) are not chunked 
     and should not have their chunking attributes set */
  var->cnk_ram=True; /* [flg] Variable is chunked in memory */
  var->typ_cnk=nc_typ_cnk; /* [enm] Type of variable when chunked (on disk). This should be same as typ_dsk except in cases where variable is chunked in input file and unchunked in output file. */
  var->typ_uck=var->type; /* [enm] Type of variable when unchunked (expanded) (in memory) */

  /* Convert variable to user-specified chunked type
     This is where var->type is changed from original to chunked type */
  var=nco_var_cnf_typ(nc_typ_cnk,var);

  if(dbg_lvl_get() >= nco_dbg_sbr) (void)fprintf(stdout,"%s: CHUNKING %s chunked %s into %s\n",prg_nm_get(),fnc_nm,var->nm,nco_typ_sng(var->type));

  return var;
} /* end nco_var_cnk() */

var_sct * /* O [sct] Unchunked variable */
nco_var_uck /* [fnc] Unchunk variable in memory */
(var_sct *var) /* I/O [sct] Variable to be unchunked */
{
  /* Threads: Routine is thread-safe */
  /* Purpose: Unchunk variable
     Routine is inverse of nco_var_cnk(): nco_var_uck[nco_var_cnk(var)]=var
     Routine handles missing_value's implicitly:
     nco_var_cnf_typ() automatically converts missing_value, if any, to unchunked type
     This may need to change when nco427 is addressed */

  const char fnc_nm[]="nco_var_uck()";
  const char scl_fct_sng[]="scale_factor"; /* [sng] Unidata standard string for scale factor */
  const char add_fst_sng[]="add_offset"; /* [sng] Unidata standard string for add offset */

  /* Return if variable in memory is not currently chunked */
  if(!var->cnk_ram) return var;

  /* Routine should be called with variable already in memory */
  if(var->val.vp == NULL){
    (void)fprintf(stdout,"%s: ERROR nco_var_uck() called with empty var->val.vp\n",prg_nm_get());
    nco_exit(EXIT_FAILURE);
  } /* endif */

  /* Chunked variables are not guaranteed to have both scale_factor and add_offset
     scale_factor is guaranteed to be of type NC_FLOAT or NC_DOUBLE and of size 1 (a scalar) */

  /* Create scalar value structures from values of scale_factor, add_offset */
  if(var->has_scl_fct){ /* [flg] Valid scale_factor attribute exists */
    scv_sct scl_fct_scv;
    var->scl_fct.vp=(void *)nco_malloc(nco_typ_lng(var->typ_uck));
    (void)nco_get_att(var->nc_id,var->id,scl_fct_sng,var->scl_fct.vp,var->typ_uck);
    scl_fct_scv=ptr_unn_2_scv(var->typ_uck,var->scl_fct);
    /* Convert var to type of scale_factor for expansion */
    var=nco_var_cnf_typ(scl_fct_scv.type,var);
    /* Multiply var by scale_factor */
    (void)var_scv_mlt(var->type,var->sz,var->has_mss_val,var->mss_val,var->val,&scl_fct_scv);
  } /* endif has_scl_fct */

  if(var->has_add_fst){ /* [flg] Valid add_offset attribute exists */
    scv_sct add_fst_scv;
    var->add_fst.vp=(void *)nco_malloc(nco_typ_lng(var->typ_uck));
    /* fxm TODO nco638 */
    (void)nco_get_att(var->nc_id,var->id,add_fst_sng,var->add_fst.vp,var->typ_uck);
    add_fst_scv=ptr_unn_2_scv(var->typ_uck,var->add_fst);
    /* Convert var to type of scale_factor for expansion */
    var=nco_var_cnf_typ(add_fst_scv.type,var);
    /* Add add_offset to var */
    (void)var_scv_add(var->type,var->sz,var->has_mss_val,var->mss_val,var->val,&add_fst_scv);
  } /* endif has_add_fst */

  if(var->has_mss_val) var=nco_cnv_mss_val_typ(var,var->type);

  /* Tell the world */  
  var->cnk_ram=False;

  /* Clean up tell-tale signs that variable was ever chunked */
  var->has_scl_fct=False; /* [flg] Valid scale_factor attribute exists */
  var->has_add_fst=False; /* [flg] Valid add_offset attribute exists */
  var->scl_fct.vp=nco_free(var->scl_fct.vp);
  var->add_fst.vp=nco_free(var->add_fst.vp);

  if(dbg_lvl_get() >= nco_dbg_var) (void)fprintf(stdout,"%s: CHUNKING %s unchunked %s into %s\n",prg_nm_get(),fnc_nm,var->nm,nco_typ_sng(var->type));

  return var;
} /* end nco_var_uck() */

void
nco_var_uck_swp /* [fnc] Unchunk var_in into var_out */
(var_sct * const var_in, /* I/O [sct] Variable to unchunk */
 var_sct * const var_out) /* I/O [sct] Variable to unchunk into */
{
  /* Purpose: Unchunk var_in into var_out
     Information flow in ncpdq prevents ncpdq from calling nco_var_uck()
     directly with either var_in or var_out.
     Need combination of var_in (for correct file and variable IDs) 
     and var_out (so unchunked variable ends up in correct place)
     Accomplish this by unchunking into temporary variable and copying 
     needed information from temporary (swap) variable to var_out.
     Routine hides gory details of swapped unchunking
     var_in is untouched except var_in->val buffer is free()'d

     nco_cnk_val() uses this routine for two purposes:
     1. Unchunk already chunked variable prior to re-chunking them 
     2. Unchunk already chunked variables permanently */
  const char fnc_nm[]="nco_var_uck_swp()";
  var_sct *var_tmp; /* [sct] Temporary variable to be unchunked */
  
  if(var_in->cnk_ram){
    if(dbg_lvl_get() >= nco_dbg_io) (void)fprintf(stdout,"%s: DEBUG %s unchunking variable %s values from %s to %s\n",prg_nm_get(),fnc_nm,var_in->nm,nco_typ_sng(var_out->typ_cnk),nco_typ_sng(var_out->typ_uck));
  }else{
    (void)fprintf(stderr,"%s: ERROR %s variable %s is already unchunked\n",prg_nm_get(),fnc_nm,var_in->nm);
    nco_exit(EXIT_FAILURE);
  } /* endif not already chunked */

  /* Output file does not contain chunking attributes yet 
     Hence unchunking var_out directly is impossible 
     Instead, make var_tmp a copy of var_in and unchunk var_tmp 
     Then copy needed elements of var_tmp to var_out 
     Then delete the rest of var_tmp 
     Fields modified in nco_var_uck() must be explicitly updated in var_out */
  var_tmp=nco_var_dpl(var_in);
  /* Free current input buffer */
  var_in->val.vp=nco_free(var_in->val.vp);
  /* Unchunk temporary variable */
  var_tmp=nco_var_uck(var_tmp); 
  /* Save relevent parts of temporary variable into output variable */
  var_out->type=var_tmp->type;
  var_out->val=var_tmp->val;
  var_out->cnk_ram=var_tmp->cnk_ram;
  /* nco_var_cnf_typ() in nco_var_uck() automatically converts missing_value, if any, to unchunked type */
  if(var_out->has_mss_val){
    /* Free current missing value before obtaining new one */
    var_out->mss_val.vp=(void *)nco_free(var_out->mss_val.vp);
    var_out->mss_val=var_tmp->mss_val;
    /* var_out now owns mss_val, make sure nco_var_free(var_tmp) ignores it */
    var_tmp->mss_val.vp=NULL;
  } /* endif */
  var_out->has_scl_fct=var_tmp->has_scl_fct;
  var_out->has_add_fst=var_tmp->has_add_fst;
  var_out->scl_fct.vp=nco_free(var_out->scl_fct.vp);
  var_out->add_fst.vp=nco_free(var_out->add_fst.vp);
  /* var_tmp->val buffer now doubles as var_out->val buffer
     Prevent nco_var_free() from free()'ing var_tmp->val
     Setting var_tmp->val.vp=NULL accomplishes this
     Use var_out->val.vp to free() this buffer later with nco_var_free() */
  var_tmp->val.vp=NULL;
  if(var_tmp != NULL) var_tmp=nco_var_free(var_tmp);
} /* end nco_var_uck_swp() */

#endif /* endif 0 */
