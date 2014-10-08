/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_cnk.c,v 1.130 2014-10-08 21:00:46 zender Exp $ */

/* Purpose: NCO utilities for chunking */

/* Copyright (C) 1995--2014 Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You can redistribute and/or modify NCO under the terms of the 
   GNU General Public License (GPL) Version 3 with exceptions described in the LICENSE file */

/* Usage:
   ncks -O -4 -D 4 --cnk_scl=8 ~/nco/data/in.nc ~/foo.nc
   ncks -O -4 -D 4 --cnk_scl=8 ${DATA}/dstmch90/dstmch90_clm.nc ~/foo.nc
   ncks -O -4 -D 4 --cnk_dmn time,10 ~/nco/data/in.nc ~/foo.nc
   ncks -O -4 -D 4 --cnk_dmn time,10 --cnk_map=dmn ~/nco/data/in.nc ~/foo.nc
   ncks -O -4 -D 4 --cnk_map=dmn -d time,0,3 ~/nco/data/in.nc ~/foo.nc
   ncks -O -4 -D 4 --cnk_dmn lat,64 --cnk_dmn lon,128 ${DATA}/dstmch90/dstmch90_clm.nc ~/foo.nc 
   ncks -O -4 -D 4 --cnk_plc=uck ~/foo.nc ~/foo.nc
   ncks -O -4 -D 4 --cnk_plc=g2d --cnk_map=rd1 --cnk_dmn lat,64 --cnk_dmn lon,128 ${DATA}/dstmch90/dstmch90_clm.nc ~/foo.nc

   ncks -O -4 -D 4 --cnk_plc=all ~/nco/data/in.nc ~/foo.nc # Chunk unchunked data
   ncks -O -4 -D 4 --cnk_plc=uck ~/foo.nc ~/foo.nc # Unchunk chunked data
   ncks -O -4 -D 4 --cnk_plc=all ~/foo.nc ~/foo.nc # Chunk chunked data
   ncks -O -4 -D 4 --cnk_plc=uck ~/nco/data/in.nc ~/foo.nc # Unchunk unchunked data

   ncks -O -C --cnk_plc=xst --cnk_map=xst -g g13 -v one,two,three,four -p ~/nco/data in_grp.nc ~/foo.nc
   ncks -O -4 -C --cnk_plc=all --cnk_map=lfp ~/nco/data/in.nc ~/foo.nc
   ncks -C -m --hdn --cdl ~/foo.nc

   ncecat testing:
   ncecat -O -4 -D 4 --cnk_plc=all -p ~/nco/data in.nc in.nc ~/foo.nc
   ncecat -O -4 -D 4 --cnk_plc=g2d --cnk_map=rd1 --cnk_dmn lat,64 --cnk_dmn lon,128 -p ${DATA}/dstmch90 dstmch90_clm.nc dstmch90_clm.nc ~/foo.nc

   ncwa testing:
   ncwa -O -4 -D 4 -a time --cnk_plc=g2d --cnk_map=rd1 --cnk_dmn lat,32 --cnk_dmn lon,128 -p ${DATA}/dstmch90 dstmch90_clm_0112.nc ~/foo.nc

   Advanced Chunking:
   http://hdfeos.org/workshops/ws13/presentations/day1/HDF5-EOSXIII-Advanced-Chunking.ppt
   http://www.hdfgroup.org/HDF5/doc/H5.user/Chunking.html
   http://www.unidata.ucar.edu/blogs/developer/en/entry/chunking_data_why_it_matters
   http://www.unidata.ucar.edu/blogs/developer/en/entry/chunking_data_choosing_shapes

   http://www.unidata.ucar.edu/software/netcdf/docs/default_chunking_4_0_1.html
   http://www.unidata.ucar.edu/software/netcdf/docs_rc/default_chunking_4_1.html */

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
  case nco_cnk_map_rd1:
    return "rd1";
  case nco_cnk_map_scl:
    return "scl";
  case nco_cnk_map_prd:
    return "prd";
  case nco_cnk_map_lfp:
    return "lfp";
  case nco_cnk_map_xst:
    return "xst";
  case nco_cnk_map_rew:
    return "rew";
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
  case nco_cnk_plc_r1d:
    return "r1d";
  case nco_cnk_plc_xpl: 
    return "xpl";
  case nco_cnk_plc_xst:
    return "xst";
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

int /* [rcd] Return code */
nco_cnk_ini /* [fnc] Create structure with all chunking information */
(const char * const fl_out, /* I [sng] Output filename */
 CST_X_PTR_CST_PTR_CST_Y(char,cnk_arg), /* I [sng] List of user-specified chunksizes */
 const int cnk_nbr, /* I [nbr] Number of chunksizes specified */
 const int cnk_map, /* I [enm] Chunking map */
 const int cnk_plc, /* I [enm] Chunking policy */
 const size_t cnk_sz_byt, /* I [B] Chunk size in bytes */
 const size_t cnk_sz_scl, /* I [nbr] Chunk size scalar */
 cnk_sct * const cnk) /* O [sct] Chunking structure */
{
  /* Purpose: Create structure with all chunking information */
  int rcd=0; /* [enm] Return code  */

  size_t fl_sys_blk_sz=0UL; /* [nbr] File system blocksize for I/O */

  /* Initialize */
  cnk->flg_usr_rqs=False;
  cnk->cnk_dmn=NULL_CEWI;
  cnk->cnk_nbr=cnk_nbr;
  cnk->cnk_map=cnk_map;
  cnk->cnk_plc=cnk_plc;
  cnk->cnk_sz_scl=cnk_sz_scl;
  cnk->cnk_sz_byt=cnk_sz_byt; /* For now, let cnk_sz_scl play double duty */

  /* Did user explicitly request chunking? */
  if(cnk_nbr > 0 || cnk_sz_byt > 0UL || cnk_sz_scl > 0UL || cnk_map != nco_cnk_map_nil || cnk_plc != nco_cnk_plc_nil) cnk->flg_usr_rqs=True;

  /* Chunks are atomic unit of HDF5 read/write
     Variables are compressed and check-summed one chunk at-a-time
     Set NCO_CNK_SZ_BYT_DFL to system blocksize, if known
     Setting chunksize equal to system blocksize thought to minimize disk head movement, be most efficient 
     Blocksize is related to default buffer; check this with
     ncks -O -4 -D 6 -v one ~/nco/data/in.nc ~/foo.nc
     Buffer is at least twice and sometimes sixteen times Blocksize:

                         Blocksize Buffer
     Linux default            4096 8192
     Yellowstone /home       32768 524288
     Yellowstong /glade     131072 524288
     Yellowstong /tmp         4096 524288 <-- probably hardcoded default not actual blocksize reported */

  /* Discover blocksize if possible */
  fl_sys_blk_sz=nco_fl_blocksize(fl_out);

  if(cnk_sz_byt > 0ULL){
    /* Use user-specified chunk size if available */
    cnk->cnk_sz_byt=cnk_sz_byt;
  }else{
    /* Otherwise use filesystem blocksize if valid, otherwise use Linux default */
    cnk->cnk_sz_byt= (fl_sys_blk_sz > 0ULL) ? fl_sys_blk_sz : NCO_CNK_SZ_BYT_DFL;
  } /* end else */
    
  if(cnk_sz_byt <= 0ULL) cnk->cnk_sz_byt=NCO_CNK_SZ_BYT_DFL;

  /* Make uniform list of user-specified per-dimension chunksizes */
  if(cnk->cnk_nbr > 0) cnk->cnk_dmn=nco_cnk_prs(cnk_nbr,cnk_arg);

  /* Set actual chunk policy and map to defaults as necessary
     20140821: Until today, defaults for map and policy were set independently
     Now we consider four distinct cases: 
     1. Neither map nor policy was explicitly set, and something else, e.g., size, was
     2. User set map not policy
     3. User set policy not map */
  if(cnk->flg_usr_rqs && (cnk_map == nco_cnk_map_nil) && (cnk_plc == nco_cnk_plc_nil)){
    cnk->cnk_map=nco_cnk_map_rd1;
    cnk->cnk_plc=nco_cnk_plc_g2d;
  } /* endif */
  if(cnk_map == nco_cnk_map_nil && cnk_plc != nco_cnk_plc_nil) cnk->cnk_map=nco_cnk_map_rd1;
  if(cnk_plc == nco_cnk_plc_nil && cnk_map != nco_cnk_map_nil) cnk->cnk_plc=nco_cnk_plc_g2d;

  return rcd;
} /* end nco_cnk_ini() */

cnk_dmn_sct ** /* O [sct] Structure list with user-specified per-dimension chunking information */
nco_cnk_prs /* [fnc] Create chunking structures with name and chunksize elements */
(const int cnk_nbr, /* I [nbr] Number of chunksizes specified */
 CST_X_PTR_CST_PTR_CST_Y(char,cnk_arg)) /* I [sng] List of user-specified chunksizes */
{
  /* Purpose: Determine name and per-dimension chunksize elements from user arguments
     Routine merely evaluates syntax of input expressions and does not 
     attempt to validate dimensions or chunksizes against input file.
     Routine based on nco_lmt_prs() */
  
  /* Valid syntax adheres to nm,cnk_sz */
  
  void nco_usg_prn(void);
  
  char **arg_lst;
  char *sng_cnv_rcd=NULL_CEWI; /* [sng] strtol()/strtoul() return code */
  
  const char dlm_sng[]=",";
  
  cnk_dmn_sct **cnk_dmn=NULL_CEWI;
  
  int idx;
  int arg_nbr;
  
  if(cnk_nbr > 0) cnk_dmn=(cnk_dmn_sct **)nco_malloc(cnk_nbr*sizeof(cnk_dmn_sct *));
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
      (void)fprintf(stdout,"%s: ERROR in chunksize specification for dimension %s\n%s: HINT Conform request to chunksize documentation at http://nco.sf.net/nco.html#cnk\n",nco_prg_nm_get(),cnk_arg[idx],nco_prg_nm_get());
      nco_exit(EXIT_FAILURE);
    } /* end if */
    
    /* Initialize structure */
    /* cnk strings that are not explicitly set by user remain NULL, i.e., 
       specifying default setting will appear as if nothing at all was set.
       Hopefully, in routines that follow, branch followed when dimension has
       all default settings specified (e.g.,"-d foo,,,,") yields same answer
       as branch for which no hyperslab along that dimension was set. */
    cnk_dmn[idx]=(cnk_dmn_sct *)nco_malloc(sizeof(cnk_dmn_sct));
    cnk_dmn[idx]->nm=NULL;
    cnk_dmn[idx]->nm_fll=NULL;
    cnk_dmn[idx]->is_usr_spc_cnk=True; /* True if any part of limit is user-specified, else False */

    /* Parse input name into a temporary string and inquire if name is absolute or relative */
    char *sng_tmp=arg_lst[0];
    nco_bool is_fll_pth=nco_is_fll_pth(sng_tmp);
    if(is_fll_pth) cnk_dmn[idx]->nm_fll=(char *)strdup(sng_tmp); else cnk_dmn[idx]->nm=(char *)strdup(sng_tmp);

    /* 20130711: Debian Mayhem project bug #716602 shows unsanitized input can cause core-dump _inside_ strtoul() */
    cnk_dmn[idx]->sz=strtoul(arg_lst[1],&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
    if(*sng_cnv_rcd) nco_sng_cnv_err(arg_lst[1],"strtoul",sng_cnv_rcd);   

    /* Free current pointer array to strings
       Strings themselves are untouched and will be free()'d with chunk structures 
       in nco_cnk_lst_free() */
    arg_lst=(char **)nco_free(arg_lst);
  } /* end loop over cnk structure list */
  
  return cnk_dmn;
} /* end nco_cnk_prs() */

cnk_dmn_sct ** /* O [sct] Pointer to free'd structure list */
nco_cnk_lst_free /* [fnc] Free memory associated with chunking structure list */
(cnk_dmn_sct **cnk_lst, /* I/O [sct] Chunking structure list to free */
 const int cnk_nbr) /* I [nbr] Number of chunking structures in list */
{
  /* Threads: Routine is thread safe and calls no unsafe routines */
  /* Purpose: Free all memory associated with dynamically allocated chunking structure list */
  int idx;

  for(idx=0;idx<cnk_nbr;idx++) 
    cnk_lst[idx]=nco_cnk_dmn_free(cnk_lst[idx]);

  /* Free structure pointer last */
  cnk_lst=(cnk_dmn_sct **)nco_free(cnk_lst);

  return cnk_lst;
} /* end nco_cnk_lst_free() */

cnk_dmn_sct * /* O [sct] Pointer to free'd chunking structure */
nco_cnk_dmn_free /* [fnc] Free all memory associated with chunking structure */
(cnk_dmn_sct *cnk_dmn) /* I/O [sct] Chunking structure to free */
{
  /* Threads: Routine is thread safe and calls no unsafe routines */
  /* Purpose: Free all memory associated with a dynamically allocated chunking structure */
  if(cnk_dmn->nm) cnk_dmn->nm=(char *)nco_free(cnk_dmn->nm);
  if(cnk_dmn->nm_fll) cnk_dmn->nm_fll=(char *)nco_free(cnk_dmn->nm_fll);
  /* Free structure pointer last */
  if(cnk_dmn) cnk_dmn=(cnk_dmn_sct *)nco_free(cnk_dmn);

  return NULL;
} /* end nco_cnk_dmn_free() */

int /* O [enm] Chunking map */
nco_cnk_map_get /* [fnc] Convert user-specified chunking map to key */
(const char *nco_cnk_map_sng) /* [sng] User-specified chunking map */
{
  /* Purpose: Convert user-specified string to chunking map
     Return nco_cnk_map_xst by default */
  const char fnc_nm[]="nco_cnk_map_get()"; /* [sng] Function name */
  char *nco_prg_nm; /* [sng] Program name */
  nco_prg_nm=nco_prg_nm_get(); /* [sng] Program name */

  if(nco_cnk_map_sng == NULL){
    /* 20140815: Change default from rd1 to xst */
    if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stdout,"%s: INFO %s reports %s invoked without explicit chunking map. Defaulting to chunking map \"xst\".\n",nco_prg_nm,fnc_nm,nco_prg_nm);
    return nco_cnk_map_xst;
  } /* endif */

  if(!strcmp(nco_cnk_map_sng,"nil")) return nco_cnk_map_nil;
  if(!strcmp(nco_cnk_map_sng,"cnk_map_nil")) return nco_cnk_map_nil;
  if(!strcmp(nco_cnk_map_sng,"dmn")) return nco_cnk_map_dmn;
  if(!strcmp(nco_cnk_map_sng,"cnk_map_dmn")) return nco_cnk_map_dmn;
  if(!strcmp(nco_cnk_map_sng,"rd1")) return nco_cnk_map_rd1;
  if(!strcmp(nco_cnk_map_sng,"cnk_map_rd1")) return nco_cnk_map_rd1;
  if(!strcmp(nco_cnk_map_sng,"scl")) return nco_cnk_map_scl;
  if(!strcmp(nco_cnk_map_sng,"cnk_map_scl")) return nco_cnk_map_scl;
  if(!strcmp(nco_cnk_map_sng,"prd")) return nco_cnk_map_prd;
  if(!strcmp(nco_cnk_map_sng,"cnk_map_prd")) return nco_cnk_map_prd;
  if(!strcmp(nco_cnk_map_sng,"lfp")) return nco_cnk_map_lfp;
  if(!strcmp(nco_cnk_map_sng,"cnk_map_lfp")) return nco_cnk_map_lfp;
  if(!strcmp(nco_cnk_map_sng,"xst")) return nco_cnk_map_xst;
  if(!strcmp(nco_cnk_map_sng,"cnk_map_xst")) return nco_cnk_map_xst;
  if(!strcmp(nco_cnk_map_sng,"rew")) return nco_cnk_map_rew;
  if(!strcmp(nco_cnk_map_sng,"cnk_map_rew")) return nco_cnk_map_rew;

  (void)fprintf(stderr,"%s: ERROR %s reports unknown user-specified chunking map %s\n",nco_prg_nm_get(),fnc_nm,nco_cnk_map_sng);
  nco_exit(EXIT_FAILURE);
  return nco_cnk_map_nil; /* Statement should not be reached */
} /* end nco_cnk_map_get() */

int /* O [enm] Chunking policy */
nco_cnk_plc_get /* [fnc] Convert user-specified chunking policy to key */
(const char *nco_cnk_plc_sng) /* [sng] User-specified chunking policy */
{
  /* Purpose: Convert user-specified string to chunking operation type 
     Return nco_cnk_plc_xst by default */
  const char fnc_nm[]="nco_cnk_plc_get()"; /* [sng] Function name */
  char *nco_prg_nm; /* [sng] Program name */
  nco_prg_nm=nco_prg_nm_get(); /* [sng] Program name */

  if(nco_cnk_plc_sng == NULL){
    /* 20140815: Change default from g2d to xst */
    if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stdout,"%s: INFO %s reports %s invoked without explicit chunking policy. Defaulting to chunking policy \"xst\".\n",nco_prg_nm,fnc_nm,nco_prg_nm);
    return nco_cnk_plc_xst;
  } /* endif */

  if(!strcmp(nco_cnk_plc_sng,"nil")) return nco_cnk_plc_nil;
  if(!strcmp(nco_cnk_plc_sng,"cnk_nil")) return nco_cnk_plc_nil;
  if(!strcmp(nco_cnk_plc_sng,"plc_nil")) return nco_cnk_plc_nil;
  if(!strcmp(nco_cnk_plc_sng,"all")) return nco_cnk_plc_all;
  if(!strcmp(nco_cnk_plc_sng,"cnk_all")) return nco_cnk_plc_all;
  if(!strcmp(nco_cnk_plc_sng,"plc_all")) return nco_cnk_plc_all;
  if(!strcmp(nco_cnk_plc_sng,"g2d")) return nco_cnk_plc_g2d;
  if(!strcmp(nco_cnk_plc_sng,"cnk_g2d")) return nco_cnk_plc_g2d;
  if(!strcmp(nco_cnk_plc_sng,"plc_g2d")) return nco_cnk_plc_g2d;
  if(!strcmp(nco_cnk_plc_sng,"g3d")) return nco_cnk_plc_g3d;
  if(!strcmp(nco_cnk_plc_sng,"cnk_g3d")) return nco_cnk_plc_g3d;
  if(!strcmp(nco_cnk_plc_sng,"plc_g3d")) return nco_cnk_plc_g3d;
  if(!strcmp(nco_cnk_plc_sng,"xpl")) return nco_cnk_plc_xpl;
  if(!strcmp(nco_cnk_plc_sng,"r1d")) return nco_cnk_plc_r1d;
  if(!strcmp(nco_cnk_plc_sng,"cnk_r1d")) return nco_cnk_plc_r1d;
  if(!strcmp(nco_cnk_plc_sng,"plc_r1d")) return nco_cnk_plc_r1d;
  if(!strcmp(nco_cnk_plc_sng,"cnk_xpl")) return nco_cnk_plc_xpl;
  if(!strcmp(nco_cnk_plc_sng,"plc_xpl")) return nco_cnk_plc_xpl;
  if(!strcmp(nco_cnk_plc_sng,"xst")) return nco_cnk_plc_xst;
  if(!strcmp(nco_cnk_plc_sng,"cnk_xst")) return nco_cnk_plc_xst;
  if(!strcmp(nco_cnk_plc_sng,"plc_xst")) return nco_cnk_plc_xst;
  if(!strcmp(nco_cnk_plc_sng,"uck")) return nco_cnk_plc_uck;
  if(!strcmp(nco_cnk_plc_sng,"cnk_uck")) return nco_cnk_plc_uck;
  if(!strcmp(nco_cnk_plc_sng,"plc_uck")) return nco_cnk_plc_uck;
  if(!strcmp(nco_cnk_plc_sng,"unchunk")) return nco_cnk_plc_uck;

  (void)fprintf(stderr,"%s: ERROR %s reports unknown user-specified chunking policy %s\n",nco_prg_nm_get(),fnc_nm,nco_cnk_plc_sng);
  nco_exit(EXIT_FAILURE);
  return nco_cnk_plc_nil; /* Statement should not be reached */
} /* end nco_cnk_plc_get() */

nco_bool /* O [flg] Variable is chunked on disk */
nco_cnk_dsk_inq /* [fnc] Check whether variable is chunked on disk */
(const int nc_id, /* I [idx] netCDF file ID */
 const int var_id) /* I [id] Variable ID */
{
  /* Purpose: Check whether variable is chunked on disk */
  /* nces -O -D 3 -v cnk ~/nco/data/in.nc ~/nco/data/foo.nc */
  
  int srg_typ; /* [enm] Storage type */
  
  (void)nco_inq_var_chunking(nc_id,var_id,&srg_typ,(size_t *)NULL);

  if(srg_typ == NC_CONTIGUOUS) return False; else return True;
  
} /* end nco_cnk_dsk_inq() */

#if 0
/* NB: Following routines are placeholders, currently not used */
size_t * /* O [nbr] Chunksize array for variable */
nco_cnk_sz_get /* [fnc] Determine chunksize array */
(const int nc_id, /* I [id] netCDF file ID */
 const char * const var_nm, /* I [sng] Variable name */
 const int cnk_map, /* I [enm] Chunking map */
 const int cnk_plc, /* I [enm] Chunking policy */
 const size_t cnk_sz_scl, /* I [nbr] Chunk size scalar */
 CST_X_PTR_CST_PTR_CST_Y(cnk_dmn_sct,cnk_dmn), /* I [sct] Chunking information */
 const int cnk_nbr) /* I [nbr] Number of dimensions with user-specified chunking */
{
  /* Purpose: Use chunking map and policy to determine chunksize list */
  int dmn_nbr; /* [nbr] Number of dimensions in variable */
  int dmn_idx;
  int idx;
  int rec_dmn_id;
  
  size_t *cnk_sz; /* [nbr] Chunksize list */
  
  /* Get record dimension ID */
  (void)nco_inq(nc_id,(int *)NULL,(int *)NULL,(int *)NULL,&rec_dmn_id);

  /* Get type and number of dimensions and attributes for variable */

  cnk_sz=(size_t *)nco_malloc(dmn_nbr*sizeof(size_t));
  
  return cnk_sz;
} /* end nco_cnk_sz_get() */

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

  (void)fprintf(stdout,"%s: ERROR deprecated routine %s should not be called\n",nco_prg_nm_get(),fnc_nm);
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

#endif /* endif 0 */

void
nco_cnk_sz_set /* [fnc] Set chunksize parameters */
(const int nc_id, /* I [id] netCDF file ID */
 CST_X_PTR_CST_PTR_CST_Y(lmt_msa_sct,lmt_all_lst), /* I [sct] Hyperslab limits */
 const int lmt_all_lst_nbr, /* I [nbr] Number of hyperslab limits */
 int * const cnk_map_ptr, /* I/O [enm] Chunking map */
 int * const cnk_plc_ptr, /* I/O [enm] Chunking policy */
 const size_t cnk_sz_scl, /* I [nbr] Chunk size scalar */
 CST_X_PTR_CST_PTR_CST_Y(cnk_dmn_sct,cnk_dmn), /* I [sct] Chunking information */
 const int cnk_nbr) /* I [nbr] Number of dimensions with user-specified chunking */
{
  /* Purpose: Use chunking map and policy to determine chunksize list */
  const char fnc_nm[]="nco_cnk_sz_set()"; /* [sng] Function name */

  char dmn_nm[NC_MAX_NAME+1L];
  char var_nm[NC_MAX_NAME+1L];

  int *dmn_id;

  int cnk_idx;
  int dmn_idx;
  int cnk_map; /* [enm] Chunking map */
  int cnk_plc; /* [enm] Chunking policy */
  int chk_typ; /* [enm] Checksum type */
  int deflate; /* [enm] Deflate filter is on */
  int dmn_nbr; /* [nbr] Number of dimensions in variable */
  int fl_fmt; /* [enm] Input file format */
  int lmt_idx;
  int lmt_idx_rec=int_CEWI;
  int rcd_dmn_id;
  int srg_typ; /* [enm] Storage type */
  int var_idx;
  int var_nbr;

  long dmn_sz;

  nco_bool flg_cnk=False; /* [flg] Chunking requested */
  nco_bool is_rec_var; /* [flg] Record variable */
  nco_bool is_chk_var; /* [flg] Check-summed variable */
  nco_bool is_cmp_var; /* [flg] Compressed variable */
  nco_bool is_chunked; /* [flg] Chunked variable */
  nco_bool must_be_chunked; /* [flg] Variable must be chunked */

  nc_type var_typ_dsk;

  size_t *cnk_sz; /* [nbr] Chunksize list */
  size_t cnk_sz_dfl; /* [nbr] Chunksize default */

  /* Did user explicitly request chunking? */
  if(cnk_nbr > 0 || cnk_sz_scl > 0UL || *cnk_map_ptr != nco_cnk_map_nil || *cnk_plc_ptr != nco_cnk_plc_nil) flg_cnk=True;

  if(!flg_cnk) return;

  /* Set actual chunk policy and map to defaults as necessary
     This rather arcane procedure saves a few lines of code in calling program
     (because defaults not set there) while maintaining correctness of arguments */
  if(*cnk_map_ptr == nco_cnk_map_nil) *cnk_map_ptr=nco_cnk_map_get((char *)NULL);
  if(*cnk_plc_ptr == nco_cnk_plc_nil) *cnk_plc_ptr=nco_cnk_plc_get((char *)NULL);
  cnk_map=*cnk_map_ptr;
  cnk_plc=*cnk_plc_ptr;

  /* Bail on unsupported options */
  if(cnk_plc == nco_cnk_plc_xpl){
    (void)fprintf(stderr,"%s: ERROR cnk_plc = %s not yet supported\n",nco_prg_nm_get(),nco_cnk_plc_sng_get(cnk_plc));
    nco_exit(EXIT_FAILURE);
  } /* endif */

  /* Does output file support chunking? */
  (void)nco_inq_format(nc_id,&fl_fmt);
  if(fl_fmt != NC_FORMAT_NETCDF4 && fl_fmt != NC_FORMAT_NETCDF4_CLASSIC){
    (void)fprintf(stderr,"%s: WARNING Output file format is %s so chunking request will be ignored\n",nco_prg_nm_get(),nco_fmt_sng(fl_fmt));
    return;
  } /* endif dbg */

  /* Vet input */
  if(cnk_map == nco_cnk_map_scl && cnk_sz_scl <= 0){
    (void)fprintf(stderr,"%s: ERROR cnk_sz_scl = %lu must be greater than 0\n",nco_prg_nm_get(),(unsigned long)cnk_sz_scl);
    nco_exit(EXIT_FAILURE);
  } /* endif cnk_sz_scl */

  if(nco_dbg_lvl_get() >= nco_dbg_fl) (void)fprintf(stderr,"%s: INFO Requested chunking or unchunking\n",nco_prg_nm_get());
  if(nco_dbg_lvl_get() >= nco_dbg_scl){
    (void)fprintf(stderr,"cnk_plc: %s\n",nco_cnk_plc_sng_get(cnk_plc));
    (void)fprintf(stderr,"cnk_map: %s\n",nco_cnk_map_sng_get(cnk_map));
    (void)fprintf(stderr,"cnk_sz_scl: %lu\n",(unsigned long)cnk_sz_scl);
    if(cnk_nbr > 0){
      (void)fprintf(stderr,"idx dmn_nm\tcnk_sz:\n");
      for(cnk_idx=0;cnk_idx<cnk_nbr;cnk_idx++) (void)fprintf(stderr,"%2d %s\t%lu\n",cnk_idx,cnk_dmn[cnk_idx]->nm,(unsigned long)cnk_dmn[cnk_idx]->sz);
    } /* cnk_nbr == 0 */
  } /* endif dbg */

  /* Get record dimension ID */
  (void)nco_inq(nc_id,(int *)NULL,&var_nbr,(int *)NULL,&rcd_dmn_id);

  /* Find record dimension, if any, in limit structure list first
  This information may be needed below */
  if(rcd_dmn_id != NCO_REC_DMN_UNDEFINED){
    (void)nco_inq_dimname(nc_id,rcd_dmn_id,dmn_nm);
    for(lmt_idx=0;lmt_idx<lmt_all_lst_nbr;lmt_idx++){
      if(!strcmp(dmn_nm,lmt_all_lst[lmt_idx]->dmn_nm)){
        lmt_idx_rec=lmt_idx;
        break;
      } /* end if */
    } /* end loop over limit */
  } /* NCO_REC_DMN_UNDEFINED */

  /* NB: Assumes variable IDs range from [0..var_nbr-1] */
  for(var_idx=0;var_idx<var_nbr;var_idx++){

    /* Initialize storage type for this variable */
    srg_typ=NC_CONTIGUOUS; /* [enm] Storage type */
    cnk_sz=(size_t *)NULL; /* [nbr] Chunksize list */
    is_rec_var=False; /* [flg] Record variable */
    is_chk_var=False; /* [flg] Check-summed variable */
    is_cmp_var=False; /* [flg] Compressed variable */
    is_chunked=False; /* [flg] Chunked variable */

    /* Get type and number of dimensions for variable */
    (void)nco_inq_var(nc_id,var_idx,var_nm,&var_typ_dsk,&dmn_nbr,(int *)NULL,(int *)NULL);

    if(dmn_nbr == 0) continue; /* Skip chunking calls for scalars */

    /* Allocate space to hold dimension IDs */
    dmn_id=(int *)nco_malloc(dmn_nbr*sizeof(int));
    /* Get dimension IDs */
    (void)nco_inq_vardimid(nc_id,var_idx,dmn_id);

    /* Is this a record variable? */
    if(rcd_dmn_id != NCO_REC_DMN_UNDEFINED){
      for(dmn_idx=0;dmn_idx<dmn_nbr;dmn_idx++){
        /* Is this the record dimension? */
        if(dmn_id[dmn_idx] == rcd_dmn_id) break; /* ...then search no further */
      } /* end loop over dmn */
      if(dmn_idx < dmn_nbr) is_rec_var=True; /* [flg] Record variable */
    } /* NCO_REC_DMN_UNDEFINED */

    /* Is variable compressed? */
    (void)nco_inq_var_deflate(nc_id,var_idx,NULL,&deflate,NULL);
    if(deflate) is_cmp_var=True; 

    /* Is variable check-summed? */
    (void)nco_inq_var_fletcher32(nc_id,var_idx,&chk_typ);
    if(chk_typ != NC_NOCHECKSUM) is_chk_var=True;

    /* Must variable be chunked? */
    if(is_rec_var || is_chk_var || is_cmp_var) must_be_chunked=True; else must_be_chunked=False;

    /* Is variable currently chunked? */
    is_chunked=nco_cnk_dsk_inq(nc_id,var_idx);

    /* Explicitly turn-off chunking for arrays that are... */
    if((cnk_plc == nco_cnk_plc_g2d && dmn_nbr < 2) || /* ...much too small... */
       (cnk_plc == nco_cnk_plc_g3d && dmn_nbr < 3) || /* ...too small... */
       (cnk_plc == nco_cnk_plc_uck) || /* ...intentionally unchunked... */
       False){
      /* If variable is chunked */
      if(is_chunked){
	if(must_be_chunked){
	  if(nco_dbg_lvl_get() >= nco_dbg_var) (void)fprintf(stderr,"%s: INFO %s %s must be chunked (record, compressed, or check-summed variable)\n",nco_prg_nm_get(),fnc_nm,var_nm);
	}else{
	  /* Turn-off chunking for this variable */
	  if(nco_dbg_lvl_get() >= nco_dbg_var) (void)fprintf(stderr,"%s: INFO %s unchunking %s\n",nco_prg_nm_get(),fnc_nm,var_nm);
	  (void)nco_def_var_chunking(nc_id,var_idx,srg_typ,cnk_sz);
	} /* !must_be_chunked */
      }else{ /* !chunked */
	if(nco_dbg_lvl_get() >= nco_dbg_var) (void)fprintf(stderr,"%s: INFO %s not unchunking %s because it is not chunked\n",nco_prg_nm_get(),fnc_nm,var_nm);
      } /* !chunked */
        /* Free space holding dimension IDs before skipping to next variable */
      dmn_id=(int *)nco_free(dmn_id);
      /* Skip to next variable in loop */
      continue;
    } /* end if */

    /* Variable will definitely be chunked */
    srg_typ=NC_CHUNKED; /* [enm] Storage type */
    if(nco_dbg_lvl_get() >= nco_dbg_var) (void)fprintf(stderr,"%s: INFO %s %schunking %s\n",nco_prg_nm_get(),fnc_nm,(is_chunked ? "re-" : "" ),var_nm);

    /* Allocate space to hold chunksizes */
    cnk_sz=(size_t *)nco_malloc(dmn_nbr*sizeof(size_t));

    /* Default "equal" chunksize for each dimension */
    cnk_sz_dfl=cnk_sz_scl;
    if(cnk_map == nco_cnk_map_prd){
      double cnk_sz_prd_dbl; /* [nbr] Chunksize product, double precision */
      double cnk_sz_eql_dbl; /* [nbr] Chunksize equal, double precision */
      double cnk_sz_dfl_dbl; /* [nbr] Chunksize default, double precision */
      cnk_sz_prd_dbl=cnk_sz_scl;
      cnk_sz_eql_dbl=pow(cnk_sz_prd_dbl,1.0/dmn_nbr);
      cnk_sz_dfl_dbl=ceil(cnk_sz_eql_dbl);
      cnk_sz_dfl=(size_t)cnk_sz_dfl_dbl;
    } /* endif map_prd */

    for(dmn_idx=0;dmn_idx<dmn_nbr;dmn_idx++){

      /* Get dimension name and size */
      (void)nco_inq_dim(nc_id,dmn_id[dmn_idx],dmn_nm,&dmn_sz);

      /* Is this the record dimension? */
      if(dmn_id[dmn_idx] == rcd_dmn_id){
        /* Does policy specify record dimension treatment? */
        if(cnk_map == nco_cnk_map_rd1){
          cnk_sz[dmn_idx]=1UL;
          /* This may still be over-ridden by explicitly specified chunksize */
          goto cnk_xpl_override;
        } /* !nco_cnk_map_rd1 */
        /* Record dimension size in output file is zero until first write
	   Obtain record dimension size from lmt_all structure */
        if(lmt_all_lst[lmt_idx_rec]->NON_HYP_DMN){
          /* When not hyperslabbed, use input record dimension size ... */
          cnk_sz[dmn_idx]=lmt_all_lst[lmt_idx_rec]->dmn_sz_org;
        }else{ /* !NON_HYP_DMN */
          /* ... and when hyperslabbed, use user-specified count */
          cnk_sz[dmn_idx]=lmt_all_lst[lmt_idx_rec]->dmn_cnt;
        } /* !NON_HYP_DMN */
      }else{ /* !record dimension */
        /* Set non-record dimensions to default, possibly over-ride later */
        cnk_sz[dmn_idx]=dmn_sz;
        if(dmn_sz == 0L){
          (void)fprintf(stderr,"%s: ERROR %s reports variable %s has dim_sz == 0L for non-record dimension %s. This should not occur and it will cause chunking to fail...\n",nco_prg_nm_get(),fnc_nm,var_nm,dmn_nm);
        } /* endif err */
      } /* !record dimension */

      /* Propagate scalar chunksize, if specified */
      if(cnk_sz_dfl > 0UL){
        if(dmn_id[dmn_idx] == rcd_dmn_id){
          if(lmt_all_lst[lmt_idx_rec]->NON_HYP_DMN){
            /* When not hyperslabbed, use input record dimension size ... */
            cnk_sz[dmn_idx]=(cnk_sz_dfl <= (size_t)lmt_all_lst[lmt_idx_rec]->dmn_sz_org) ? cnk_sz_dfl : (size_t)lmt_all_lst[lmt_idx_rec]->dmn_sz_org;
          }else{ /* !NON_HYP_DMN */
            /* ... and when hyperslabbed, use user-specified count */
            cnk_sz[dmn_idx]=(cnk_sz_dfl <= (size_t)lmt_all_lst[lmt_idx_rec]->dmn_cnt) ? cnk_sz_dfl : (size_t)lmt_all_lst[lmt_idx_rec]->dmn_cnt;
          } /* !NON_HYP_DMN */
        }else{ /* !rcd_dmn_id */
          /* Non-record sizes default to cnk_sz_dfl or to dimension size */
          cnk_sz[dmn_idx]=(cnk_sz_dfl <= (size_t)dmn_sz) ? cnk_sz_dfl : (size_t)dmn_sz;
        } /* !rcd_dmn_id */
      } /* !cnk_sz_dfl */

cnk_xpl_override: /* end goto */

      /* Explicit chunk specifications override all else */
      for(cnk_idx=0;cnk_idx<cnk_nbr;cnk_idx++){
        /* Match on name not ID */
        if(!strcmp(cnk_dmn[cnk_idx]->nm,dmn_nm)){
          cnk_sz[dmn_idx]=cnk_dmn[cnk_idx]->sz;
          if(dmn_id[dmn_idx] == rcd_dmn_id){
            if(lmt_all_lst[lmt_idx_rec]->NON_HYP_DMN){
              if(cnk_sz[dmn_idx] > (size_t)lmt_all_lst[lmt_idx_rec]->dmn_sz_org){
                (void)fprintf(stderr,"%s: WARNING %s allowing user-specified record dimension chunksize = %lu for %s to exceed record dimension size in input file = %lu. May fail if output file is not concatenated from multiple inputs.\n",nco_prg_nm_get(),fnc_nm,(unsigned long)cnk_dmn[cnk_idx]->sz,dmn_nm,lmt_all_lst[lmt_idx_rec]->dmn_sz_org);
              } /* endif too big */
            }else{ /* !NON_HYP_DMN */
              if(cnk_sz[dmn_idx] > (size_t)lmt_all_lst[lmt_idx_rec]->dmn_cnt){
                (void)fprintf(stderr,"%s: WARNING %s allowing user-specified record dimension chunksize = %lu for %s to exceed user-specified record dimension hyperslab size in input file = %lu. May fail if output file is not concatenated from multiple inputs.\n",nco_prg_nm_get(),fnc_nm,(unsigned long)cnk_dmn[cnk_idx]->sz,dmn_nm,lmt_all_lst[lmt_idx_rec]->dmn_cnt);
              } /* endif too big */
            } /* !NON_HYP_DMN */
          }else{ /* !rcd_dmn_id */
            if(cnk_sz[dmn_idx] > (size_t)dmn_sz){
              /* dmn_sz of record dimension may (will) be zero in output file
		 Non-record dimensions, though, must have cnk_sz <= dmn_sz */
              (void)fprintf(stderr,"%s: WARNING %s trimming user-specified chunksize = %lu to %s size = %lu\n",nco_prg_nm_get(),fnc_nm,(unsigned long)cnk_dmn[cnk_idx]->sz,dmn_nm,dmn_sz);
              /* Trim else out-of-bounds sizes will fail in HDF library in nc_enddef() */
              cnk_sz[dmn_idx]=(size_t)dmn_sz;
            } /* endif */
          } /* !rcd_dmn_id */
          break;
        } /* cnk_nm != dmn_nm */
      } /* end loop over cnk */

    } /* end loop over dmn */

    if(nco_dbg_lvl_get() >= nco_dbg_scl){
      (void)fprintf(stderr,"idx nm\tdmn_sz\tcnk_sz for %s:\n",var_nm);
      for(dmn_idx=0;dmn_idx<dmn_nbr;dmn_idx++){
        (void)nco_inq_dimlen(nc_id,dmn_id[dmn_idx],&dmn_sz);
        (void)nco_inq_dimname(nc_id,dmn_id[dmn_idx],dmn_nm);
        (void)fprintf(stderr,"%2d %s\t%lu\t%lu\n",dmn_idx,dmn_nm,dmn_sz,(unsigned long)cnk_sz[dmn_idx]);
      } /* end loop over dmn */
    } /* endif dbg */

    /* Turn-on chunking for this variable */
    (void)nco_def_var_chunking(nc_id,var_idx,srg_typ,cnk_sz);

    /* Free space holding dimension IDs and chunksizes */
    dmn_id=(int *)nco_free(dmn_id);
    cnk_sz=(size_t *)nco_free(cnk_sz);

  } /* end loop over var */

  return;
} /* end nco_cnk_sz_set() */

void
nco_cnk_sz_set_trv /* [fnc] Set chunksize parameters (GTT version of nco_cnk_sz_set() ) */
(const int grp_id_in, /* I [id] netCDF group ID in input file */
 const int grp_id_out, /* I [id] netCDF group ID in output file */
 const cnk_sct * const cnk, /* I [sct] Chunking structure */
 const char * const var_nm,  /* I [sng] Name of variable */
 const dmn_cmn_sct * const dmn_cmn) /* I [sct] Dimension structure in output file */
{
  /* Purpose: Use chunking map and policy to determine chunksize list
     Adapted from nco_cnk_sz_set() to GTT:
     1) Instead of loop for all variables, this functions chunks one variable
     2) In dimension loop, dimension object is obtained from variable object

     Unidata defaults set in libsrc4/nc4hdf.c, nc4var.c

     netCDF 4.3.2 and later:
     DEFAULT_CHUNK_SIZE=4 MB
     Default chunk size for 1-D record variables is DEFAULT_CHUNK_SIZE/type_size
     = 1048576 for sizeof(type)=4 

     h5dump prints chunking and compression information for netCDF4/HDF5 files:
     h5dump -H -p ~/nco/data/hdn.nc
     Grep for "COMPRESSION" */

  const char fnc_nm[]="nco_cnk_sz_set_trv()"; /* [sng] Function name */

  cnk_dmn_sct **cnk_dmn;

  int chk_typ; /* [enm] Checksum type */
  int cnk_idx; /* [idx] Chunk dimension index */
  int cnk_map; /* [enm] Chunking map */
  int cnk_nbr; /* [nbr] Number of chunk sizes */
  int cnk_plc; /* [enm] Chunking policy */
  int deflate; /* [enm] Deflate filter is on */
  int dmn_idx; /* [idx] Dimension index */
  int dmn_nbr; /* [nbr] Number of dimensions for variable */
  int fl_fmt; /* [enm] Input file format */
  int shuffle; /* [enm] Shuffle filter is on */
  int srg_typ; /* [enm] Storage type */
  int var_id_in; /* [ID] Variable ID in input file */
  int var_id_out; /* [ID] Variable ID in output file */

  nc_type var_typ_dsk; /* [nbr] Variable type */

  nco_bool *flg_mch; /* [flg] Name match (absolute or relative) between chunking structure 'cnk_sct' and dimension 'dmn_cmn' */
  nco_bool flg_ovr; /* [flg] Override default chunking with user-specified chunking for this dimension */
  nco_bool flg_usr_rqs; /* [flg] User requested checking */
  nco_bool is_rec_var; /* [flg] Record variable */
  nco_bool is_chk_var; /* [flg] Check-summed variable */
  nco_bool is_cmp_var; /* [flg] Compressed variable */
  nco_bool is_chunked; /* [flg] Chunked variable */
  nco_bool is_xpl_cnk; /* [flg] Explicitly chunked variable */
  nco_bool must_be_chunked; /* [flg] Variable must be chunked */

  size_t *cnk_sz=NULL; /* [nbr] Chunksize list */
  size_t cnk_sz_dfl; /* [nbr] Chunksize default */
  size_t cnk_sz_scl; /* [nbr] Chunk size scalar */
  size_t typ_sz; /* [B] Bytes per value */

  static short FIRST_CALL=True;

  unsigned long long cnk_sz_byt; /* [B] Desired bytes per chunk (e.g., system blocksize) */

  /* Initialize local convenience variables */
  flg_usr_rqs=cnk->flg_usr_rqs;
  cnk_nbr=cnk->cnk_nbr;
  cnk_map=cnk->cnk_map;
  cnk_plc=cnk->cnk_plc;
  cnk_sz_scl=cnk->cnk_sz_scl;
  cnk_sz_byt=cnk->cnk_sz_byt;
  cnk_dmn=cnk->cnk_dmn;

  /* Only use NCO chunking when user explicitly sets a chunking option */
  if(!flg_usr_rqs) return;

  /* Bail on unsupported options */
  if(cnk_map == nco_cnk_map_nil){
    (void)fprintf(stderr,"%s: ERROR cnk_map = %s not yet supported\n",nco_prg_nm_get(),nco_cnk_map_sng_get(cnk_map));
    nco_exit(EXIT_FAILURE);
  } /* endif */

  /* Does output file support chunking? */
  (void)nco_inq_format(grp_id_out,&fl_fmt);
  if(fl_fmt != NC_FORMAT_NETCDF4 && fl_fmt != NC_FORMAT_NETCDF4_CLASSIC){
    (void)fprintf(stderr,"%s: WARNING Output file format is %s so chunking request will be ignored\n",nco_prg_nm_get(),nco_fmt_sng(fl_fmt));
    return;
  } /* endif dbg */

  /* Vet input */
  if(cnk_map == nco_cnk_map_scl && cnk_sz_scl <= 0UL){
    (void)fprintf(stderr,"%s: ERROR cnk_sz_scl = %lu must be greater than 0\n",nco_prg_nm_get(),(unsigned long)cnk_sz_scl);
    nco_exit(EXIT_FAILURE);
  } /* endif cnk_sz_scl */

  if(FIRST_CALL && nco_dbg_lvl_get() >= nco_dbg_fl && nco_dbg_lvl_get() != nco_dbg_dev){
    (void)fprintf(stdout,"%s: INFO User requested chunking or unchunking\n",nco_prg_nm_get());
    if(nco_dbg_lvl_get() >= nco_dbg_scl  && nco_dbg_lvl_get() != nco_dbg_dev){
      (void)fprintf(stdout,"cnk_plc, cnk_map: %s, %s\n",nco_cnk_plc_sng_get(cnk_plc),nco_cnk_map_sng_get(cnk_map));
      (void)fprintf(stdout,"cnk_sz_scl, cnk_sz_byt: %lu, %lu\n",(unsigned long)cnk_sz_scl,(unsigned long)cnk_sz_byt);
      if(cnk_nbr > 0){
        (void)fprintf(stdout,"idx dmn_nm\tcnk_sz:\n");
        for(cnk_idx=0;cnk_idx<cnk_nbr;cnk_idx++)
	  (void)fprintf(stdout,"%2d %s\t%lu\n",cnk_idx,cnk_dmn[cnk_idx]->nm,(unsigned long)cnk_dmn[cnk_idx]->sz);
      } /* cnk_nbr == 0 */
    } /* endif dbg */
  } /* endif dbg */
  FIRST_CALL=False;

  /* Initialize storage type for this variable */
  srg_typ=NC_CONTIGUOUS; /* [enm] Storage type */
  is_rec_var=False; /* [flg] Record variable */
  is_chk_var=False; /* [flg] Check-summed variable */
  is_cmp_var=False; /* [flg] Compressed variable */
  is_chunked=False; /* [flg] Chunked variable */
  is_xpl_cnk=False; /* [flg] Explicitly chunked variable */

  /* Obtain variable ID */
  (void)nco_inq_varid(grp_id_out,var_nm,&var_id_out);
  (void)nco_inq_varid(grp_id_in,var_nm,&var_id_in);

  /* Get type and number of dimensions for variable */
  (void)nco_inq_var(grp_id_out,var_id_out,(char *)NULL,&var_typ_dsk,&dmn_nbr,(int *)NULL,(int *)NULL);
  typ_sz=nco_typ_lng(var_typ_dsk);

  if(dmn_nbr == 0){
    if(nco_dbg_lvl_get() == nco_dbg_old) (void)fprintf(stdout,"%s: INFO %s skipping scalar...\n",nco_prg_nm_get(),fnc_nm);
    return; 
  } /* dmn_nbr */

  /* Does variable contain a record dimension? */
  for(dmn_idx=0;dmn_idx<dmn_nbr;dmn_idx++)
    if(dmn_cmn[dmn_idx].is_rec_dmn) is_rec_var=True;

  /* Is variable compressed? */
  (void)nco_inq_var_deflate(grp_id_out,var_id_out,&shuffle,&deflate,(int *)NULL);
  if(deflate) is_cmp_var=True; 

  /* Is variable check-summed? */
  (void)nco_inq_var_fletcher32(grp_id_out,var_id_out,&chk_typ);
  if(chk_typ != NC_NOCHECKSUM) is_chk_var=True;

  /* Must variable be chunked? */
  if(is_rec_var || is_chk_var || is_cmp_var) must_be_chunked=True; else must_be_chunked=False;

  /* Is variable currently chunked? */
  if(nco_fmt_xtn_get() != nco_fmt_xtn_hdf4 || NC_LIB_VERSION >= 433) is_chunked=nco_cnk_dsk_inq(grp_id_in,var_id_in);

  /* Does this variable have any user-chunked dimensions? */
  if(cnk_plc == nco_cnk_plc_xpl){
    for(dmn_idx=0;dmn_idx<dmn_nbr;dmn_idx++)
      for(cnk_idx=0;cnk_idx<cnk_nbr;cnk_idx++){
        if(cnk_dmn[cnk_idx]->nm_fll){
          if(!strcmp(cnk_dmn[cnk_idx]->nm_fll,dmn_cmn[dmn_idx].nm_fll)) break;
        }else{
          if(!strcmp(cnk_dmn[cnk_idx]->nm,dmn_cmn[dmn_idx].nm)) break;
        } /* end else */
      } /* end loop over cnk_idx */
    if(dmn_idx != dmn_nbr) is_xpl_cnk=True;
  } /* end plc_xpl */

  if(must_be_chunked){
    /* Some variables simply must be chunked */
    if(nco_dbg_lvl_get() >= nco_dbg_var && nco_dbg_lvl_get() != nco_dbg_dev) (void)fprintf(stdout,"%s: INFO %s %s must be chunked (record, compressed, or check-summed variable)\n",nco_prg_nm_get(),fnc_nm,var_nm);
  }else{
    /* Explicitly exit this chunking function now (and thus implement netCDF4-default chunking behavior) for variables that are... */
    if((cnk_plc == nco_cnk_plc_xpl && !is_xpl_cnk) || /* ...not explicitly chunked... */
       (cnk_plc == nco_cnk_plc_xst && !is_chunked) || /* ...not already chunked... */
       (cnk_plc == nco_cnk_plc_g2d && dmn_nbr < 2) || /* ...much too small... */
       (cnk_plc == nco_cnk_plc_g3d && dmn_nbr < 3) || /* ...too small... */
       (cnk_plc == nco_cnk_plc_uck) || /* ...intentionally unchunked... */
       False){
      /* If variable is chunked */
      if(is_chunked){
	/* Turn-off chunking for this variable */
	if(nco_dbg_lvl_get() >= nco_dbg_var && nco_dbg_lvl_get() != nco_dbg_dev) (void)fprintf(stdout,"%s: INFO %s unchunking %s\n",nco_prg_nm_get(),fnc_nm,var_nm);
	if(shuffle) (void)fprintf(stdout,"%s: WARNING %s reports variable %s has shuffle flag set before unchunking. Expect the worst.",nco_prg_nm_get(),fnc_nm,var_nm);
	(void)nco_def_var_chunking(grp_id_out,var_id_out,srg_typ,cnk_sz);
      }else{ /* !chunked */
	if(nco_dbg_lvl_get() >= nco_dbg_var && nco_dbg_lvl_get() != nco_dbg_dev) (void)fprintf(stdout,"%s: INFO %s not unchunking %s because it is not chunked\n",nco_prg_nm_get(),fnc_nm,var_nm);
      } /* !chunked */
      /* Return control to calling routine
	 NB: Here is where loop in original nco_cnk_sz_set() continues to next variable */
      return;
    } /* !turn-off chunking */
  } /* !must_be_chunked */

  /* Since routine has not returned yet, this variable will definitely be chunked */
  srg_typ=NC_CHUNKED; /* [enm] Storage type */
  if(nco_dbg_lvl_get() >= nco_dbg_var && nco_dbg_lvl_get() != nco_dbg_dev) (void)fprintf(stdout,"%s: INFO %s %schunking %s\n",nco_prg_nm_get(),fnc_nm,(is_chunked ? "re-" : "" ),var_nm);

  /* Allocate space to hold chunksizes */
  cnk_sz=(size_t *)nco_malloc(dmn_nbr*sizeof(size_t));    

  if(cnk_map == nco_cnk_map_xst){
    /* Set chunksizes to existing sizes for this variable */
    if(is_chunked){
      (void)nco_inq_var_chunking(grp_id_in,var_id_in,(int *)NULL,cnk_sz);
      /* Allow existing chunksizes to be over-ridden by explicitly specified chunksizes */
      goto cnk_xpl_override;
    } /* !is_chunked */
  } /* !nco_cnk_map_xst */

  /* Default "equal" chunksize for each dimension */
  cnk_sz_dfl=cnk_sz_scl;
  if(cnk_map == nco_cnk_map_prd){
    double cnk_sz_prd_dbl; /* [nbr] Chunksize product, double precision */
    double cnk_sz_eql_dbl; /* [nbr] Chunksize equal, double precision */
    double cnk_sz_dfl_dbl; /* [nbr] Chunksize default, double precision */
    cnk_sz_prd_dbl=cnk_sz_scl;
    cnk_sz_eql_dbl=pow(cnk_sz_prd_dbl,1.0/dmn_nbr);
    cnk_sz_dfl_dbl=ceil(cnk_sz_eql_dbl);
    cnk_sz_dfl=(size_t)cnk_sz_dfl_dbl;
  } /* endif map_prd */

  /* Set "reasonable" defaults */
  for(dmn_idx=0;dmn_idx<dmn_nbr;dmn_idx++){

    /* Is this a record dimension? */
    if(dmn_cmn[dmn_idx].is_rec_dmn){
      /* Does policy specify record dimension treatment? */
      if(cnk_map == nco_cnk_map_rd1){
        cnk_sz[dmn_idx]=1UL;
        /* Record chunksize has beeen set and may still be over-ridden by explicitly specified record chunksize */
        continue;
      } /* !nco_cnk_map_rd1 */
      /* Record dimension size in output file is zero until first write
	 Obtain record dimension size from lmt_all structure (for nco_cnk_sz_set()) */
      if(dmn_cmn[dmn_idx].NON_HYP_DMN){
        /* When not hyperslabbed, use input record dimension size ... */
        cnk_sz[dmn_idx]=dmn_cmn[dmn_idx].sz;
	/* 20140518: As of netCDF 4.3.2, employ smarter defaults for record dimension */
	if(dmn_nbr == 1) cnk_sz[dmn_idx]=NCO_CNK_SZ_BYT_R1D_DFL/typ_sz;
      }else{ /* !NON_HYP_DMN */
        /* ... and when hyperslabbed, use user-specified count */
        cnk_sz[dmn_idx]=dmn_cmn[dmn_idx].dmn_cnt;
      } /* !NON_HYP_DMN */
    }else{ /* !record dimension */
      /* Set non-record dimensions to default, possibly override later */
      cnk_sz[dmn_idx]=dmn_cmn[dmn_idx].sz;
      if(dmn_cmn[dmn_idx].sz == 0L) (void)fprintf(stdout,"%s: ERROR %s reports variable %s has dim_sz == 0L for non-record dimension %s. This should not occur and it will cause chunking to fail...\n",nco_prg_nm_get(),fnc_nm,var_nm,dmn_cmn[dmn_idx].nm);
    } /* !record dimension */

  } /* end loop over dimensions */

  if(cnk_map == nco_cnk_map_lfp){
    /* Set "Lefter Product" chunksizes
       Reasonable defaults have been set for all dimensions, now adjust lefter dimensions to match overall size */
    int dmn_idx_1st_rec;
    int dmn_nbr_lft;
    double cnk_sz_prd_rgt=1.0;
    double cnk_sz_prd_lft;
    double cnk_val_nbr;
    double cnk_sz_lft_dbl;
    size_t cnk_sz_lft;

    cnk_val_nbr=cnk_sz_byt/(double)typ_sz;

    /* Use default sizes on all righter dimensions than first record dimension */
    for(dmn_idx=dmn_nbr-1;dmn_idx>=0;dmn_idx--) /* NB: reverse loop */
      if(dmn_cmn[dmn_idx].is_rec_dmn) break;
    dmn_idx_1st_rec=dmn_idx;

    /* Find product of all righter chunksizes */
    for(dmn_idx=dmn_nbr-1;dmn_idx>=dmn_idx_1st_rec+1;dmn_idx--) cnk_sz_prd_rgt*=cnk_sz[dmn_idx];
    /* Derive product of all lefter chunksizes */
    cnk_sz_prd_lft=cnk_val_nbr/cnk_sz_prd_rgt;
    /* How many lefter chunksizes are there? */
    dmn_nbr_lft=dmn_idx_1st_rec+1;
    cnk_sz_lft_dbl=pow(cnk_sz_prd_lft,1.0/dmn_nbr_lft);
    cnk_sz_lft=(size_t)cnk_sz_lft_dbl;
    /* Assign remaining chunkspace equally amongst lefter dimensions */
    for(dmn_idx=0;dmn_idx<=dmn_idx_1st_rec;dmn_idx++) cnk_sz[dmn_idx]=cnk_sz_lft;

    /* Allow existing chunksizes to be over-ridden by explicitly specified chunksizes */
    goto cnk_xpl_override;
  } /* !nco_cnk_map_lfp */

  if(cnk_map == nco_cnk_map_rew){
    /* Described by Russ Rew at http://www.unidata.ucar.edu/staff/russ/public/chunk_shape_3D.py
       ncks -O -C -4 -D 6 -v four_dmn_rec_var --cnk_map=rew -p ~/nco/data in.nc ~/foo.nc */
    double cnk_val_nbr;
    double dmn_sz_prd=1.0; /* [nbr] Number of elements in output variable */
    double cnk_nbr_xct; /* [nbr] Exact number of ideal chunks needed to store variable */
    double cnk_nbr_2D_axs; /* [nbr] Exact number of ideal chunks along each 2D axis */
    double fct_ncr=1.0; /* [frc] Factor by which to increase chunk size */
    size_t *var_shp; /* [nbr] Shape of variable */

    /* Allocate space to hold variable shape */
    var_shp=(size_t *)nco_malloc(dmn_nbr*sizeof(size_t));    

    cnk_val_nbr=cnk_sz_byt/(double)typ_sz;
    for(dmn_idx=0;dmn_idx<=dmn_nbr-1;dmn_idx++){
      /* Store shape for future use */
      /* 20140131: fxm this gets wrong value for input record dimension size unless hyperslabbed. Why? */
      if(dmn_cmn[dmn_idx].is_rec_dmn) var_shp[dmn_idx]= (dmn_cmn[dmn_idx].NON_HYP_DMN) ? dmn_cmn[dmn_idx].dmn_cnt : dmn_cmn[dmn_idx].sz; else var_shp[dmn_idx]= (dmn_cmn[dmn_idx].NON_HYP_DMN) ? dmn_cmn[dmn_idx].sz : dmn_cmn[dmn_idx].dmn_cnt;
      assert(var_shp[dmn_idx] > 0L);
      /* Determine hyperslab size */
      dmn_sz_prd*=var_shp[dmn_idx];
    } /* end loop over dmn_idx */
    
    cnk_nbr_xct=dmn_sz_prd/cnk_val_nbr;
    cnk_nbr_2D_axs=pow(cnk_nbr_xct,0.25);

    if(var_shp[0]/(cnk_nbr_2D_axs*cnk_nbr_2D_axs) < 1.0){
      cnk_sz[0]=1L;
      cnk_nbr_2D_axs=1.0/sqrt((double)var_shp[0]);
    }else{
      cnk_sz[0]=var_shp[0]/(cnk_nbr_2D_axs*cnk_nbr_2D_axs);
    } /* endif */

    for(dmn_idx=1;dmn_idx<=dmn_nbr-1;dmn_idx++) /* NB: Start at 1 */
      if(var_shp[dmn_idx]/cnk_nbr_2D_axs < 1.0) fct_ncr*=cnk_nbr_2D_axs/var_shp[dmn_idx];
    for(dmn_idx=1;dmn_idx<=dmn_nbr-1;dmn_idx++) /* NB: Start at 1 */
      if(var_shp[dmn_idx]/cnk_nbr_2D_axs < 1.0) cnk_sz[dmn_idx]=1L; else cnk_sz[dmn_idx]=fct_ncr*var_shp[dmn_idx]/cnk_nbr_2D_axs;

    if(nco_dbg_lvl_get() >= nco_dbg_var){
      (void)fprintf(stdout,"map rew debugging:\n");
      (void)fprintf(stdout,"cnk_nbr_xct = %g, cnk_val_nbr = %g, dmn_sz_prd = %g\n",cnk_nbr_xct,cnk_val_nbr,dmn_sz_prd);
      (void)fprintf(stdout,"idx dmn_nm\tdmn_sz\tvar_shp\tcnk_sz:\n");
      for(dmn_idx=0;dmn_idx<dmn_nbr;dmn_idx++) (void)fprintf(stdout,"%2d %s\t\t%lu\t%lu\t%lu\n",dmn_idx,dmn_cmn[dmn_idx].nm,(unsigned long)dmn_cmn[dmn_idx].sz,(unsigned long)var_shp[dmn_idx],(unsigned long)cnk_sz[dmn_idx]);
    } /* endif dbg */

    /* Free shape space */
    if(var_shp) var_shp=(size_t *)nco_free(var_shp);    
  } /* !nco_cnk_map_rew */

  /* Override "reasonable" defaults with explicitly set uniform scalar chunksize, if any */
  if(cnk_sz_dfl > 0UL){
    for(dmn_idx=0;dmn_idx<dmn_nbr;dmn_idx++){
      /* Is this a record dimension? */
      if(dmn_cmn[dmn_idx].is_rec_dmn){
        if(dmn_cmn[dmn_idx].NON_HYP_DMN){
          /* When not hyperslabbed, use input record dimension size ... */
          cnk_sz[dmn_idx]=(cnk_sz_dfl <= (size_t)dmn_cmn[dmn_idx].sz) ? cnk_sz_dfl : (size_t)dmn_cmn[dmn_idx].sz;
        }else{ /* !NON_HYP_DMN */
          /* ... and when hyperslabbed, use user-specified count */
          cnk_sz[dmn_idx]=(cnk_sz_dfl <= (size_t)dmn_cmn[dmn_idx].dmn_cnt) ? cnk_sz_dfl : (size_t)dmn_cmn[dmn_idx].dmn_cnt;
        } /* !NON_HYP_DMN */
      }else{ /* !rec_dmn_id */
        /* Non-record sizes default to cnk_sz_dfl or to dimension size */
	//        cnk_sz[dmn_idx]=(cnk_sz_dfl <= (size_t)dmn_cmn[dmn_idx].sz) ? cnk_sz_dfl : (size_t)dmn_cmn[dmn_idx].sz;
        cnk_sz[dmn_idx]=(cnk_sz_dfl <= (size_t)dmn_cmn[dmn_idx].sz) ? cnk_sz_dfl : (size_t)dmn_cmn[dmn_idx].dmn_cnt;
      } /* !is_rec_dmn */
    } /* end loop over dimensions */
  } /* !cnk_sz_dfl */

cnk_xpl_override: /* end goto */

  /* Override "reasonable" defaults with explicitly set per-dimension sizes, if any */
  flg_mch=(nco_bool *)nco_malloc(dmn_nbr*sizeof(nco_bool));
  /* Loop over all dimensions in this variable */
  for(dmn_idx=0;dmn_idx<dmn_nbr;dmn_idx++){

    /* Initialize to false then override */
    flg_mch[dmn_idx]=False;

    /* Loop over all user-specified chunk-sizes */
    for(cnk_idx=0;cnk_idx<cnk_nbr;cnk_idx++){

      if(cnk_dmn[cnk_idx]->nm_fll){
        if(!strcmp(cnk_dmn[cnk_idx]->nm_fll,dmn_cmn[dmn_idx].nm_fll)) flg_mch[dmn_idx]=True;
      }else{
        if(!strcmp(cnk_dmn[cnk_idx]->nm,dmn_cmn[dmn_idx].nm)) flg_mch[dmn_idx]=True;
      } /* end else */

      /* User-specified name matches current dimension name */
      if(flg_mch[dmn_idx]){
	flg_ovr=False;

	/* Override default chunking with user-specified chunking for this dimension if... */
	if(
	   /* ... Policy is anything but r1d or ...*/
	   (cnk_plc != nco_cnk_plc_r1d) ||
	   /* ... Policy is r1d and this is the matching r1d coordinate or ... */
	   (cnk_plc == nco_cnk_plc_r1d && (dmn_nbr == 1 && is_rec_var)) ||
	   False){
	  /* Perform override */
	  cnk_sz[dmn_idx]=cnk_dmn[cnk_idx]->sz;
	  flg_ovr=True;
	} /* endif */

	/* Sanity checks for dimensions that were over-ridden by user-specified chunksizes */
	if(flg_ovr){
	  /* Is this a record dimension? */
	  if(dmn_cmn[dmn_idx].is_rec_dmn){
	    /* dmn_sz of record dimension can/will be zero in output file
	       Allow (though warn) when cnk_sz > dmn_sz in such cases */
	    if(dmn_cmn[dmn_idx].NON_HYP_DMN){
	      if(dmn_cmn[dmn_idx].sz > 0 && /* Warn only after records have been written */
		 cnk_sz[dmn_idx] > (size_t)dmn_cmn[dmn_idx].sz){
		(void)fprintf(stderr,"%s: WARNING %s allowing user-specified record dimension %s chunksize %lu which exceeds current output file record dimension size = %lu. May fail if output file is not concatenated from multiple inputs.\n",nco_prg_nm_get(),fnc_nm,dmn_cmn[dmn_idx].nm,(unsigned long)cnk_dmn[cnk_idx]->sz,(unsigned long)dmn_cmn[dmn_idx].sz);
	      } /* endif too big */
	    }else{ /* !NON_HYP_DMN */
	      if(cnk_sz[dmn_idx] > (size_t)dmn_cmn[dmn_idx].dmn_cnt){
		(void)fprintf(stderr,"%s: WARNING %s allowing user-specified record dimension %s chunksize = %lu which exceeds user-specified record dimension input hyperslab size = %lu. May fail if output file is not concatenated from multiple inputs.\n",nco_prg_nm_get(),fnc_nm,dmn_cmn[dmn_idx].nm,(unsigned long)cnk_dmn[cnk_idx]->sz,(unsigned long)dmn_cmn[dmn_idx].dmn_cnt);
	      } /* endif too big */
	    } /* !NON_HYP_DMN */
	  }else{ /* !rcd_dmn_id */
	    if(cnk_sz[dmn_idx] > (size_t)dmn_cmn[dmn_idx].sz){
	      /* Unlike record dimensions, non-record dimensions must have cnk_sz <= dmn_sz */
	      (void)fprintf(stderr,"%s: WARNING %s trimming user-specified fixed dimension %s chunksize from %lu to %lu\n",nco_prg_nm_get(),fnc_nm,dmn_cmn[dmn_idx].nm,(unsigned long)cnk_dmn[cnk_idx]->sz,(unsigned long)dmn_cmn[dmn_idx].sz);
	      /* Trim else out-of-bounds sizes will fail in HDF library in nc_enddef() */
	      cnk_sz[dmn_idx]=(size_t)dmn_cmn[dmn_idx].sz;
	    } /* endif */
	  } /* !rcd_dmn_id */
	} /* !flg_ovr */
	/* Dimension matched this chunk name, no need to search other chunk names */
	break;
      } /* cnk_nm != dmn_nm */
    } /* end loop over dimensions */
  } /* end loop over cnk */
  if(flg_mch) flg_mch=(nco_bool *)nco_free(flg_mch);

  /* Status: Previous block implemented per-dimension checks on user-requested chunksizes only
     Block below implements similar final safety check for ALL dimensions and ALL chunking maps
     Check trims fixed (not record) dimension chunksize to never be larger than dimension size */
  for(dmn_idx=0;dmn_idx<dmn_nbr;dmn_idx++){
    if(cnk_sz[dmn_idx] > (size_t)dmn_cmn[dmn_idx].sz){
      if(!dmn_cmn[dmn_idx].is_rec_dmn){
        /* Unlike record dimensions, non-record dimensions must have cnk_sz <= dmn_sz */
        (void)fprintf(stderr,"%s: WARNING %s final check trimming %s chunksize from %lu to %lu\n",nco_prg_nm_get(),fnc_nm,dmn_cmn[dmn_idx].nm,(unsigned long)cnk_sz[dmn_idx],(unsigned long)dmn_cmn[dmn_idx].sz);
        /* Trim else out-of-bounds sizes will fail in HDF library in nc_enddef() */
        cnk_sz[dmn_idx]=(size_t)dmn_cmn[dmn_idx].sz;
      } /* rcd_dmn_id */
    } /* end if */
  } /* end loop over dmn */

  if(nco_dbg_lvl_get() >= nco_dbg_var && nco_dbg_lvl_get() != nco_dbg_dev){
    /* Dimensions and chunksizes used by variable in output file */
    (void)fprintf(stdout,"idx dmn_nm\tdmn_sz\tcnk_sz:\n");
    for(dmn_idx=0;dmn_idx<dmn_nbr;dmn_idx++) (void)fprintf(stdout,"%2d %s \t%lu\t%lu\n",dmn_idx,dmn_cmn[dmn_idx].nm_fll,(unsigned long)dmn_cmn[dmn_idx].sz,(unsigned long)cnk_sz[dmn_idx]);
  } /* endif dbg */

  /* Set storage (chunked or contiguous) for this variable */
  (void)nco_def_var_chunking(grp_id_out,var_id_out,srg_typ,cnk_sz);

  /* Free space holding dimension IDs and chunksizes */
  if(cnk_sz) cnk_sz=(size_t *)nco_free(cnk_sz);

  return;
} /* nco_cnk_sz_set_trv() */
