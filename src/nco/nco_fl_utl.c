/* $Header$ */

/* Purpose: File manipulation */

/* Copyright (C) 1995--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

#include "nco_fl_utl.h" /* File manipulation */

int /* O [enm] Mode flag for nco_create() call */
nco_create_mode_mrg /* [fnc] Merge clobber mode with user-specified file format */
(const int md_clobber, /* I [enm] Clobber mode (NC_CLOBBER or NC_NOCLOBBER) */
 const int fl_out_fmt) /* I [enm] Output file format */
{
  /* Purpose: Merge clobber mode with flag determined by fl_out_fmt
     to produce output mode flag
     md_clobber: Either NC_CLOBBER or NC_NOCLOBBER
     md_create: md_clobber OR'd with (user-specified) file format mode */

  int md_create; /* O [enm] Mode flag for nco_create() call */

  if(md_clobber != NC_CLOBBER && md_clobber != NC_NOCLOBBER){
    (void)fprintf(stderr,"%s: ERROR nco_create_mode_mrg() received unknown clobber mode md_clobber\n",nco_prg_nm_get());
    nco_exit(EXIT_FAILURE);
  } /* endif */

  md_create=md_clobber;
  if(fl_out_fmt == NC_FORMAT_64BIT_OFFSET){
    md_create|=NC_64BIT_OFFSET;
  }else if(fl_out_fmt == NC_FORMAT_CDF5){
    md_create|=NC_64BIT_DATA;
  }else if(fl_out_fmt == NC_FORMAT_NETCDF4){
    md_create|=NC_NETCDF4;
  }else if(fl_out_fmt == NC_FORMAT_NETCDF4_CLASSIC){
    md_create|=NC_NETCDF4|NC_CLASSIC_MODEL;
  }else if(fl_out_fmt == NC_COMPRESS){ /* fxm: is NC_COMPRESS legal? */
    md_create|=NC_COMPRESS;
  }else if(fl_out_fmt != NC_FORMAT_CLASSIC){
    (void)fprintf(stderr,"%s: ERROR nco_create_mode_mrg() received unknown file format = %d\n",nco_prg_nm_get(),fl_out_fmt);
    nco_exit(EXIT_FAILURE);
  } /* end else fl_out_fmt */

#ifdef ENABLE_MPI
  md_create|=NC_NETCDF4|NC_MPIIO;
#endif /* !ENABLE_MPI */

  return md_create;
} /* end nco_create_mode_mrg() */

int /* [rcd] Return code */
nco_create_mode_prs /* [fnc] Parse user-specified file format */
(const char * const fl_fmt_sng, /* I [sng] User-specified file format string */
 int * const fl_fmt_enm) /* O [enm] Output file format */
{
  int rcd=NC_NOERR; /* [rcd] Return code */

  /* Careful! Some valid format strings are subsets of other valid format strings:
     "classic" is a sub-string of itself and of "netcdf4_classic"
     "netcdf4" is a sub-string of itself and of "netcdf4_classic" */
  if(strcasestr("classic",fl_fmt_sng) && !strcasestr(fl_fmt_sng,"netcdf4")){
    /* If "classic" contains string and string does not contain "netcdf4" */
    *fl_fmt_enm=NC_FORMAT_CLASSIC;
  }else if(strcasestr("64bit_offset",fl_fmt_sng)){
    /* If "64bit_offset" contains string */
    *fl_fmt_enm=NC_FORMAT_64BIT_OFFSET;
  }else if(strcasestr(fl_fmt_sng,"netcdf4")){
    /* If string contains "netcdf4" */
#ifdef ENABLE_NETCDF4
    if(strcasestr("netcdf4",fl_fmt_sng)){
      /* If "netcdf4" contains string */
      *fl_fmt_enm=NC_FORMAT_NETCDF4;
    }else if(strcasestr("netcdf4_classic",fl_fmt_sng)){
      /* If "netcdf4_classic" contains string */
      *fl_fmt_enm=NC_FORMAT_NETCDF4_CLASSIC;
    } /* endif NETCDF4 */
#else /* !ENABLE_NETCDF4 */
    (void)fprintf(stderr,"%s: ERROR This NCO was not built with netCDF4 and cannot create the requested netCDF4 file format. HINT: Re-try with netCDF3 file format, either by omitting the filetype specification, or by explicitly specifying the \"-3\", \"--fl_fmt=classic\", \"-6\",  or \"--fl_fmt=64bit_offset\" options.\n",nco_prg_nm_get());
    nco_exit(EXIT_FAILURE);
#endif /* !ENABLE_NETCDF4 */
  }else if(strcasestr("64bit_data",fl_fmt_sng) || strcasestr("pnetcdf",fl_fmt_sng) || strcasestr(fl_fmt_sng,"cdf5")){
    /* If "64bit_data" contains string or "pnetcdf" contains string or string contains "cdf5" */
    if(NC_LIB_VERSION >= 440){
      *fl_fmt_enm=NC_FORMAT_CDF5;
    }else{
      (void)fprintf(stderr,"%s: ERROR This NCO was not built with CDF5 (http://trac.mcs.anl.gov/projects/parallel-netcdf) capabilities and cannot create the requested CDF5 (aka PnetCDF) file format. CDF5 was introduced in the base netCDF library in version 4.4.0 in January, 2016. HINT: Re-try with after building NCO with the requisite netCDF library version or select a supported file format such as \"classic\" or \"64bit_offset\".\n",nco_prg_nm_get());
    } /* !NC_LIB_VERSION */
  }else{
    (void)fprintf(stderr,"%s: ERROR Unknown output file format \"%s\" requested. Valid formats are (unambiguous leading characters of) \"classic\", \"64bit_offset\",%s \"netcdf4\", and \"netcdf4_classic\".\n",nco_prg_nm_get(),fl_fmt_sng,(NC_LIB_VERSION >= 440) ? "\"64bit_data\"," : "");
    nco_exit(EXIT_FAILURE);
  } /* endif fl_fmt_enm */

  return rcd; /* [rcd] Return code */
} /* end nco_create_mode_prs() */

int /* O [enm] Return code */
nco_fl_dmm_mk /* Create dummy file */
(const char * const fl_out) /* I [sng] Dummy file */
{
  /* Purpose: Create dummy file for use by ncremap/ncks */

  const char fnc_nm[]="nco_fl_dmm_mk()"; /* [sng] Function name */

  char *fl_out_tmp=NULL_CEWI;

  int fl_out_fmt=NC_FORMAT_CLASSIC; /* [enm] Output file format */
  int dmn_id; /* [id] Dimension ID */
  int out_id; /* I [id] Output netCDF file ID */
  int rcd=NC_NOERR;

  nco_bool FORCE_APPEND=False; /* Option A */
  nco_bool FORCE_OVERWRITE=True; /* Option O */
  nco_bool RAM_CREATE=False; /* [flg] Create file in RAM */
  nco_bool RAM_OPEN=False; /* [flg] Open (netCDF3-only) file(s) in RAM */
  nco_bool SHARE_CREATE=False; /* [flg] Create (netCDF3-only) file(s) with unbuffered I/O */
  nco_bool SHARE_OPEN=False; /* [flg] Open (netCDF3-only) file(s) with unbuffered I/O */
  nco_bool WRT_TMP_FL=False; /* [flg] Write output to temporary file */

  size_t bfr_sz_hnt=NC_SIZEHINT_DEFAULT; /* [B] Buffer size hint */

  /* Open dummy file */
  fl_out_tmp=nco_fl_out_open(fl_out,&FORCE_APPEND,FORCE_OVERWRITE,fl_out_fmt,&bfr_sz_hnt,RAM_CREATE,RAM_OPEN,SHARE_CREATE,SHARE_OPEN,WRT_TMP_FL,&out_id);

  rcd=nco_def_dim(out_id,"dummy",1L,&dmn_id);
  if(rcd != NC_NOERR){
    (void)fprintf(stdout,"%s: ERROR %s unable to open dummy file\n",nco_prg_nm_get(),fnc_nm);
    nco_exit(EXIT_FAILURE);
  } /* !rcd */
  
  /* Close output file and move it from temporary to permanent location */
  (void)nco_fl_out_cls(fl_out,fl_out_tmp,out_id);

  return NCO_NOERR;

} /* !nco_fl_dmm_mk() */

void
nco_fl_sz_est /* [fnc] Estimate RAM size == uncompressed file size */
(char *smr_fl_sz_sng, /* I/O [sng] String describing estimated file size */
 const trv_tbl_sct * const trv_tbl) /* I [sct] Traversal table */
{
  /* Purpose: Estimate RAM size == uncompressed file size */
  const char fnc_nm[]="nco_fl_sz_est()"; /* [sng] Function name */

  size_t ram_sz_crr;
  size_t ram_sz_ttl=0L;
  size_t dmn_sz[NC_MAX_VAR_DIMS]; /* [nbr] Dimension sizes */
  
  for(unsigned idx_tbl=0;idx_tbl<trv_tbl->nbr;idx_tbl++){
    trv_sct var_trv=trv_tbl->lst[idx_tbl]; 
    if(var_trv.flg_xtr && var_trv.nco_typ == nco_obj_typ_var){
      ram_sz_crr=1L;
      for(unsigned int dmn_idx=0;dmn_idx<(unsigned int)var_trv.nbr_dmn;dmn_idx++){
	if(var_trv.var_dmn[dmn_idx].is_crd_var){
	  /* Get coordinate from table */
	  crd_sct *crd=var_trv.var_dmn[dmn_idx].crd;
	  /* Use hyperslabbed size */
	  dmn_sz[dmn_idx]=crd->lmt_msa.dmn_cnt;
	}else{
	  /* Get unique dimension */
	  dmn_trv_sct *dmn_trv=var_trv.var_dmn[dmn_idx].ncd;
	  /* Use hyperslabbed size */
	  dmn_sz[dmn_idx]=dmn_trv->lmt_msa.dmn_cnt;
	} /* !is_crd_var */
	ram_sz_crr*=dmn_sz[dmn_idx];
      } /* !dmn */
      ram_sz_crr*=nco_typ_lng(var_trv.var_typ);
      ram_sz_ttl+=ram_sz_crr;
    } /* !var */
  } /* end idx_tbl */

  (void)sprintf(smr_fl_sz_sng,"Size expected in RAM or uncompressed storage of all data (not metadata), accounting for subsets and hyperslabs, is %lu B ~ %lu kB, %lu kiB ~ %lu MB, %lu MiB ~ %lu GB, %lu GiB",(unsigned long)ram_sz_ttl,(unsigned long)round(1.0*ram_sz_ttl/NCO_BYT_PER_KB),(unsigned long)round(1.0*ram_sz_ttl/NCO_BYT_PER_KiB),(unsigned long)round(1.0*ram_sz_ttl/NCO_BYT_PER_MB),(unsigned long)round(1.0*ram_sz_ttl/NCO_BYT_PER_MiB),(unsigned long)round(1.0*ram_sz_ttl/NCO_BYT_PER_GB),(unsigned long)round(1.0*ram_sz_ttl/NCO_BYT_PER_GiB));
  if(nco_dbg_lvl_get() >= nco_dbg_scl) (void)fprintf(stdout,"%s: %s reports %s\n",nco_prg_nm_get(),fnc_nm,smr_fl_sz_sng);

  return;
} /* end nco_fl_sz_est() */

void
nco_fl_cmp_err_chk(void) /* [fnc] Perform error checking on file */
{
  /* Purpose: Perform error checking on file */
} /* end nco_fl_cmp_err_chk() */

void
nco_fl_fmt_vet /* [fnc] Verify output file format supports requested actions */
(const int fl_fmt, /* I [enm] Output file format */
 const int cnk_nbr, /* I [nbr] Number of chunksizes specified */
 const int dfl_lvl) /* I [enm] Deflate level [0..9] */
{
  /* Purpose: Verify output file format supports requested actions */
  if(cnk_nbr > 0 && !(fl_fmt == NC_FORMAT_NETCDF4 || fl_fmt == NC_FORMAT_NETCDF4_CLASSIC)) (void)fprintf(stdout,"%s: WARNING Attempt to chunk variables in output file which has netCDF format %s. Chunking is only supported by netCDF filetypes NC_FORMAT_NETCDF4 and NC_FORMAT_NETCDF4_CLASSIC. Command will attempt to complete but without chunking. HINT: re-run command and change output type to netCDF4 using \"-4\", \"--fl_fmt=netcdf4\", or \"--fl_fmt=netcdf4_classic\" option.\n",nco_prg_nm_get(),nco_fmt_sng(fl_fmt));
  if(dfl_lvl > 0 && !(fl_fmt == NC_FORMAT_NETCDF4 || fl_fmt == NC_FORMAT_NETCDF4_CLASSIC)) (void)fprintf(stdout,"%s: WARNING Attempt to deflate (compress) variables in output file which has netCDF format %s. Deflation is only supported by netCDF filetypes NC_FORMAT_NETCDF4 and NC_FORMAT_NETCDF4_CLASSIC. Command will attempt to complete but without deflation. HINT: re-run command and change output type to netCDF4 using \"-4\", (same as \"--fl_fmt=netcdf4\"), or \"-7\" (same as \"--fl_fmt=netcdf4_classic\") option.\n",nco_prg_nm_get(),nco_fmt_sng(fl_fmt));
} /* end nco_nco_fl_fmt_vet() */

void
nco_fl_overwrite_prm /* [fnc] Obtain user consent to overwrite output file */
(const char * const fl_nm) /* I [sng] Name of file */
{
  /* Purpose: Obtain user consent to overwrite output file (or die trying) */
  const char fnc_nm[]="nco_fl_overwrite_prm()"; /* [sng] Function name */

  int rcd_sys;
  
  struct stat stat_sct;
  
  /* Use stat() to determine if output file already exists */
  rcd_sys=stat(fl_nm,&stat_sct);
  
  /* When output file already exists, query user whether to overwrite */
  if(rcd_sys != -1){
    char garbage;
    char usr_rpl='z';
    short nbr_itr=0;
    
    /* fxm TODO nco199: Internationalize (i18n) NCO with gettext() */
    while(usr_rpl != 'n' && usr_rpl != 'y'){
      nbr_itr++;
      if(nbr_itr > NCO_MAX_NBR_USR_INPUT_RETRY){
	(void)fprintf(stdout,"\n%s: ERROR %s reports %d failed attempts to obtain valid interactive input. Assuming non-interactive shell and exiting.\n",nco_prg_nm_get(),fnc_nm,nbr_itr-1);
	nco_exit(EXIT_FAILURE);
      } /* end if */
      (void)fprintf(stdout,"%s: overwrite %s (y/n)? ",nco_prg_nm_get(),fl_nm);
      (void)fflush(stdout);
      usr_rpl=(char)fgetc(stdin);
      /* Allow one carriage return per response free of charge */
      if(usr_rpl == '\n') usr_rpl=(char)fgetc(stdin);
      /* Treat other charcacters after the last question/prompt, and before the next, as garbage */
      do{garbage=fgetc(stdin);} while(garbage != '\n' && garbage != EOF);

      /* (void)fflush(stdin);
	 comp.lang.c 20000212 and http://www.eskimo.com/~scs/C-faq/q12.18.html
	 C FAQ Author Steve Summit explains why not to use fflush(stdin)
	 and how best to manually clean stdin of unwanted residue */

    } /* end while */
    
    if(usr_rpl == 'n') nco_exit(EXIT_SUCCESS);

  } /* end if rcd_sys != -1 */
  
} /* end nco_fl_overwrite_prm() */

#ifdef _MSC_VER
void nco_fl_chmod(const char * const fl_nm){}
#else /* !_MSC_VER */
void
nco_fl_chmod /* [fnc] Ensure file is user/owner-writable */
(const char * const fl_nm) /* I [sng] Name of file */
{
  /* Purpose: Make file user/owner-writable
     Uses chmod() C-library call rather than chmod shell program
     Routine assumes that output file already exists, but is of unknown mode */

  const char fnc_nm[]="nco_fl_chmod()"; /* [sng] Function name */
  
  int rcd_sys;
  
  mode_t fl_md;
  mode_t fl_usr_md;
  mode_t fl_usr_wrt_md;
  
  struct stat stat_sct;
  
  /* Output file now guaranteed to exist. Perform stat() to check its permissions. */
  rcd_sys=stat(fl_nm,&stat_sct);
  
  /* 20120228 Ensure output file is writable even when input file is not 
     stat structure includes st_mode field which includes following flags:
     mode_t st_mode
     S_IRWXU    00700     mask for file owner permissions
     S_IWUSR    00200     owner has write permission
     Method of checking: 
     First  bit-wise "and" (& S_IRWXU) uses mask to strip full, multibyte, file mode flag of all but user/owner byte 
     Second bit-wise "and" (& S_IWUSR) is "true" (non-zero) iff owner write permission is set */
  fl_md=stat_sct.st_mode;
  fl_usr_md=fl_md & S_IRWXU;
  fl_usr_wrt_md=fl_usr_md & S_IWUSR;
  if(nco_dbg_lvl_get() >= nco_dbg_scl) (void)fprintf(stderr,"%s: %s reports permissions for file %s are (octal) = %lo\n",nco_prg_nm_get(),fnc_nm,fl_nm,(unsigned long)fl_md);
  if(!fl_usr_wrt_md){
    /* Set user-write bit of output file */
    fl_md=fl_md | S_IWUSR;
    rcd_sys=chmod(fl_nm,fl_md);
    if(rcd_sys == -1){
#ifndef __GNUG__
      (void)fprintf(stdout,"%s: %s reports chmod() returned error \"%s\"\n",nco_prg_nm_get(),fnc_nm,strerror(errno));
#endif /* __GNUG__ */
      (void)fprintf(stdout,"%s: ERROR Unable to make output file writable by user, exiting...\n",nco_prg_nm_get());
      nco_exit(EXIT_FAILURE);
    } /* endif rcd_sys */
  } /* end if chmod */
  
} /* end nco_fl_chmod() */
#endif /* !_MSC_VER */

void
nco_fl_cp /* [fnc] Copy first file to second */
(const char * const fl_src, /* I [sng] Name of source file to copy */
 const char * const fl_dst) /* I [sng] Name of destination file */
{
  /* Purpose: Copy first file to second */
  char *cmd_cp;
  char *fl_dst_cdl;
  char *fl_src_cdl;
#ifdef _MSC_VER
  const char cmd_cp_fmt[]="copy %s %s";
#else /* !_MSC_VER */
  const char cmd_cp_fmt[]="/bin/cp %s %s";
#endif /* !_MSC_VER */

  int rcd;
  const int fmt_chr_nbr=4;

  /* Perform system() call iff files are not identical */
  if(!strcmp(fl_src,fl_dst)){
    if(nco_dbg_lvl_get() >= nco_dbg_fl) (void)fprintf(stderr,"%s: INFO Temporary and final files %s are identical---no need to copy.\n",nco_prg_nm_get(),fl_src);
    return;
  } /* end if */

  /* 20131227 Allow for whitespace characters in fl_dst 
     Assume CDL translation results in acceptable name for shell commands */
  fl_src_cdl=nm2sng_fl(fl_src);
  fl_dst_cdl=nm2sng_fl(fl_dst);

  /* Construct and execute copy command */
  cmd_cp=(char *)nco_malloc((strlen(cmd_cp_fmt)+strlen(fl_src_cdl)+strlen(fl_dst_cdl)-fmt_chr_nbr+1UL)*sizeof(char));
  if(nco_dbg_lvl_get() >= nco_dbg_fl) (void)fprintf(stderr,"%s: Copying %s to %s...",nco_prg_nm_get(),fl_src_cdl,fl_dst_cdl);
  (void)sprintf(cmd_cp,cmd_cp_fmt,fl_src_cdl,fl_dst_cdl);
  rcd=system(cmd_cp);
  if(rcd == -1){
    (void)fprintf(stdout,"%s: ERROR nco_fl_cp() is unable to execute cp command \"%s\"\n",nco_prg_nm_get(),cmd_cp);
    nco_exit(EXIT_FAILURE);
  } /* end if */
  if(nco_dbg_lvl_get() >= nco_dbg_fl) (void)fprintf(stderr,"done\n");

  cmd_cp=(char *)nco_free(cmd_cp);
  if(fl_dst_cdl) fl_dst_cdl=(char *)nco_free(fl_dst_cdl);
  if(fl_src_cdl) fl_src_cdl=(char *)nco_free(fl_src_cdl);
} /* end nco_fl_cp() */

#ifdef _MSC_VER
char * nco_fl_info_get(const char *){return NULL;}
#else /* !_MSC_VER */
char * /* O [sng] Canonical file name*/
nco_fl_info_get /* [fnc] Determine canonical filename and properties */
(const char * const fl_nm_lcl) /* I [sng] Name of file */
{
  /* Purpose: Return canonical filename of by removing all symbolic links */
  int rcd;
  char *fl_nm_cnc=NULL; /* [sng] Canonical file name */
  struct stat stat_sct;

  /* Does file exist on local system? */
  rcd=stat(fl_nm_lcl,&stat_sct);
  if(rcd == -1) (void)fprintf(stderr,"%s: INFO File %s does not exist on local system\n",nco_prg_nm_get(),fl_nm_lcl);

  /* Is file a symbolic link? */
  rcd=lstat(fl_nm_lcl,&stat_sct);
#ifndef NECSX
  if(rcd != -1 && S_ISLNK(stat_sct.st_mode)){
#else /* NECSX */
  if(rcd != -1 && ((stat_sct.st_mode & S_IFMT) == S_IFLNK)){ /* fxm: This is BSD not POSIX */
#endif /* NECSX */
#if (defined HAVE_CANONICALIZE_FILE_NAME) && 0
    /* 20040619: Function prototype is not found (perhaps due to aggressive standard switches) by NCO, althouth configure.ac finds it */
    /* Remember to free() fl_nm_cnc after using it */
    fl_nm_cnc=(char *)canonicalize_file_name(fl_nm_lcl);
    if(fl_nm_cnc){
      (void)fprintf(stderr,"%s: INFO Local file %s is symbolic link to %s",nco_prg_nm_get(),fl_nm_lcl,fl_nm_cnc);
      fl_nm_cnc=(char *)nco_free(fl_nm_cnc);
    }else{
      (void)fprintf(stderr,"%s: INFO Local file %s is symbolic link but does not canonicalize",nco_prg_nm_get(),fl_nm_lcl);
    } /* endif link canonicalizes */
#else /* !HAVE_CANONICALIZE_FILE_NAME */
    (void)fprintf(stderr,"%s: INFO File %s is a symbolic link\n",nco_prg_nm_get(),fl_nm_lcl);
#endif /* !HAVE_CANONICALIZE_FILE_NAME */
  } /* endif symbolic link */

  return fl_nm_cnc;
} /* end nco_fl_info_get() */
#endif /* !_MSC_VER */ 

char ** /* O [sng] List of user-specified filenames */
nco_fl_lst_mk /* [fnc] Create file list from command line positional arguments */
(CST_X_PTR_CST_PTR_CST_Y(char,argv), /* I [sng] Argument list */
 const int argc, /* I [nbr] Argument count */
 int arg_crr, /* I [idx] Index of current argument */
 int * const fl_nbr, /* O [nbr] Number of files in input file list */
 char ** const fl_out, /* I/O [sng] Name of output file */
 nco_bool *FL_LST_IN_FROM_STDIN, /* O [flg] fl_lst_in comes from stdin */
 const nco_bool FORCE_OVERWRITE) /* I [flg] Overwrite existing file, if any */
{
  /* Purpose: Parse positional arguments on command line
     Name of calling program plays a role in this */

  /* Assume command-line switches have been digested already (e.g., by getopt())
     Assume argv[arg_crr] points to first positional argument (i.e., first argument
     following all switches and their arugments).
     fl_out is filled in if it was not specified as a command line switch
     Multi-file operators take input filenames from positional arguments, if any
     Otherwise, multi-file operators try to get input filenames from stdin */

  const char fnc_nm[]="nco_fl_lst_mk()"; /* [sng] Function name */

  nco_bool FL_OUT_FROM_PSN_ARG=True; /* [flg] fl_out comes from positional argument */

  char **fl_lst_in=NULL_CEWI; /* [sng] List of user-specified filenames */

  int idx;
  int fl_nm_sz_wrn=255;
  int psn_arg_fst=0; /* [nbr] Offset for expected number of positional arguments */
  int psn_arg_nbr; /* [nbr] Number of remaining positional arguments */

  int nco_prg_id; /* Program ID */
  nco_prg_id=nco_prg_id_get(); /* [enm] Program ID */
  psn_arg_nbr=argc-arg_crr; /* [nbr] Number of remaining positional arguments */

  /* Is output file already known from command line switch (i.e., -o fl_out)? */
  if(*fl_out){
    /* fl_out is already known so interpret all remaining positional arguments as input files */
    FL_OUT_FROM_PSN_ARG=False;
    /* Offset expected number of positional arguments by one to account for fl_out being known */
    psn_arg_fst=1;
  } /* end if */

  /* All operators except multi-file operators and possibly must have at least one positional argument */
  if(!nco_is_mfo(nco_prg_id) && FL_OUT_FROM_PSN_ARG && psn_arg_nbr == 0){
    (void)fprintf(stdout,"%s: ERROR received %d filenames; need at least one\n",nco_prg_nm_get(),psn_arg_nbr);
    (void)nco_usg_prn();
    nco_exit(EXIT_FAILURE);
  } /* end if */

  /* psn_arg_nbr can be 0 for ncap2 */
  if(nco_dbg_lvl_get() >= nco_dbg_std){
    if(psn_arg_nbr > 0){
      /* Might there be problems with any specified files? */
      for(idx=arg_crr;idx<argc;idx++){
	if((int)strlen(argv[idx]) >= fl_nm_sz_wrn) (void)fprintf(stderr,"%s: WARNING filename %s is long (%ld characters) and may not play well with older operating systems\n",nco_prg_nm_get(),argv[idx],(long int)strlen(argv[idx]));
      } /* !idx */
    } /* !psn_arg_nbr */
  } /* !dbg */
  
  /* ncap2 dummy file */
  const char tmp_sng_1[]="_tmp_dmm.nc.pid"; /* Extra string appended to temporary filenames */
  char *fl_dmm;
  int rcd_stt=NC_MIN_INT; /* [rcd] Return code from stat() */
  long fl_dmm_lng; /* [nbr] Length of dummy file name */
  pid_t pid; /* Process ID */
  struct stat stat_sct;
  
  switch(nco_prg_id) {
    /* 20190414 Given ncap2 its own block so it can have no input files 
       Previously, ncap2 used ncatted/ncks/ncrename code block below, but this often required supplying dummy filenames */

  case ncap:
    /* Operators with optional fl_in and required fl_out */
    if(psn_arg_nbr > 2-psn_arg_fst){
      if(FL_OUT_FROM_PSN_ARG) (void)fprintf(stdout,"%s: ERROR received %d filenames; need no more than two\nHINT: Eliminate extra whitespace, such as spaces in comma-separated lists, from command\n",nco_prg_nm_get(),psn_arg_nbr); else (void)fprintf(stdout,"%s: ERROR received %d input filenames; need no more than one (output file was specified with -o switch)\nHINT: Eliminate extra whitespace, such as spaces in comma-separated lists, from command\n",nco_prg_nm_get(),psn_arg_nbr);
      (void)nco_usg_prn();
      nco_exit(EXIT_FAILURE);
    } /* !psn_arg_nbr */
    
    /* ncap2 always has one input file, whether dummy or real */
    fl_lst_in=(char **)nco_malloc(sizeof(char *)); /* fxm: free() this memory sometime */

    /* two regular file arguments */
    if(FL_OUT_FROM_PSN_ARG && psn_arg_nbr == 2){
      fl_lst_in[(*fl_nbr)++]=(char *)strdup(argv[arg_crr++]);
      *fl_out=(char *)strdup(argv[arg_crr]);
    } /* !FL_OUT_FROM_PSN_ARG */

    /* two files, output file from --output option argument */
    if(!FL_OUT_FROM_PSN_ARG && psn_arg_nbr == 1){
      fl_lst_in[(*fl_nbr)++]=(char *)strdup(argv[arg_crr++]);
    } /* !FL_OUT_FROM_PSN_ARG */
    
    /* ncap2 was called with a single filename argument
       If that file exists, treat it as both input and output file
       Otherwise, treat it as output file and create dummy input file */
    if((!FL_OUT_FROM_PSN_ARG && psn_arg_nbr == 0) || (FL_OUT_FROM_PSN_ARG && psn_arg_nbr == 1)){

      rcd_stt = stat(argv[arg_crr], &stat_sct);

      if(rcd_stt == 0 && !FORCE_OVERWRITE){
	/* Single file exists, use it as input file */
	fl_lst_in[(*fl_nbr)++] = (char *) strdup(argv[arg_crr++]);
      }else if((rcd_stt == -1) || (rcd_stt == 0 && FORCE_OVERWRITE)){
	if((nco_dbg_lvl_get() >= nco_dbg_fl) && (rcd_stt == -1)) (void)fprintf(stderr,"\n%s: DEBUG stat() #1 failed: %s does not exist. Will assume %s will be brand-new output file and will create dummy input file...\n",nco_prg_nm_get(),argv[arg_crr],argv[arg_crr]);
	if((nco_dbg_lvl_get() >= nco_dbg_fl) && (rcd_stt == 0)) (void)fprintf(stderr,"\n%s: DEBUG stat() #1 succeeded: %s exists but FORCE_OVERWRITE is true so will overwrite existing %s and will create dummy input file...\n",nco_prg_nm_get(),argv[arg_crr],argv[arg_crr]);
	pid = getpid();
	/* ncap2 dummy file name is "ncap2" + tmp_sng_1 + PID + NUL */
	fl_dmm_lng=strlen(nco_prg_nm_get())+strlen(tmp_sng_1)+8UL+1UL;
	/* NB: Calling routine has responsibility to free() this memory */
	fl_dmm=(char *) nco_malloc(fl_dmm_lng*sizeof(char));
	(void)sprintf(fl_dmm,"%s%s%ld",nco_prg_nm_get(),tmp_sng_1,(long)pid);
	(void) nco_fl_dmm_mk(fl_dmm);
	fl_lst_in[(*fl_nbr)++] = fl_dmm;
      } /* !rcd_stt */

      if(FL_OUT_FROM_PSN_ARG && (rcd_stt != 0 || FORCE_OVERWRITE)){
	*fl_out = (char *) strdup(argv[arg_crr]);
	//*fl_out=nco_sng_sntz(*fl_out);
      } /* !FL_OUT_FROM_PSN_ARG */
    } /* !FL_OUT_FROM_PSN_ARG */

    if(nco_dbg_lvl_get() >= nco_dbg_scl) (void)fprintf(stdout,"%s: DEBUG %s reports psn_arg_nbr = %d, psn_arg_fst = %d, arg_crr = %d,argc = %d, fl_lst_in[0]=%s, *fl_nbr=%d, *fl_out = %s\n",nco_prg_nm_get(),fnc_nm,psn_arg_nbr,psn_arg_fst,arg_crr,argc,fl_lst_in[0],*fl_nbr,*fl_out);

    return fl_lst_in;
    /* break; *//* NB: break after return in case statement causes SGI cc warning */
  case ncatted:
  case ncks:
  case ncrename:
    /* Operators with single fl_in and optional fl_out */
    if(psn_arg_nbr > 2-psn_arg_fst){
      if(FL_OUT_FROM_PSN_ARG) (void)fprintf(stdout,"%s: ERROR received %d filenames; need no more than two\nHINT: Eliminate extra whitespace, such as spaces in comma-separated lists, from command\n",nco_prg_nm_get(),psn_arg_nbr); else (void)fprintf(stdout,"%s: ERROR received %d input filenames; need no more than one (output file was specified with -o switch)\nHINT: Eliminate extra whitespace, such as spaces in comma-separated lists, from command\n",nco_prg_nm_get(),psn_arg_nbr);
      (void)nco_usg_prn();
      nco_exit(EXIT_FAILURE);
    } /* end if */
    fl_lst_in=(char **)nco_malloc(sizeof(char *)); /* fxm: free() this memory sometime */
    fl_lst_in[(*fl_nbr)++]=(char *)strdup(argv[arg_crr++]);

    /* Sanitize input list from stdin and from positional arguments */
    //    for(int fl_idx=0;fl_idx<*fl_nbr;fl_idx++) (void)nco_sng_sntz(fl_lst_in[fl_idx]);

    /* Output file is optional for these operators */
    if(arg_crr == argc-1){
      *fl_out=(char *)strdup(argv[arg_crr]);
      //*fl_out=nco_sng_sntz(*fl_out);
    } /* !arg_crr */
    return fl_lst_in;
    /* break; *//* NB: break after return in case statement causes SGI cc warning */
  case ncbo:
  case ncflint:
    /* Operators with dual fl_in and required fl_out */
    if(psn_arg_nbr != 3-psn_arg_fst){
      if(FL_OUT_FROM_PSN_ARG) (void)fprintf(stdout,"%s: ERROR received %d filenames; need exactly three\n",nco_prg_nm_get(),psn_arg_nbr); else (void)fprintf(stdout,"%s: ERROR received %d input filenames; need exactly two (output file was specified with -o switch)\n",nco_prg_nm_get(),psn_arg_nbr);
      (void)nco_usg_prn();
      nco_exit(EXIT_FAILURE);
    } /* end if */
    break;
  case ncpdq:
  case ncwa:
    /* Operators with single fl_in and required fl_out */
    if(psn_arg_nbr != 2-psn_arg_fst){
      if(FL_OUT_FROM_PSN_ARG) (void)fprintf(stdout,"%s: ERROR received %d filenames; need exactly two\n",nco_prg_nm_get(),psn_arg_nbr); else (void)fprintf(stdout,"%s: ERROR received %d input filenames; need exactly one (output file was specified with -o switch)\n",nco_prg_nm_get(),psn_arg_nbr);
      (void)nco_usg_prn();
      nco_exit(EXIT_FAILURE);
    } /* end if */
    break;
  case ncra:
  case ncfe:
  case ncrcat:
  case ncecat:
  case ncge:
    /* Operators with multiple fl_in and required fl_out */
    if(psn_arg_nbr < 2-psn_arg_fst){

      /* If multi-file operator has no positional arguments for input files... */
      if(nco_is_mfo(nco_prg_id) && ((!FL_OUT_FROM_PSN_ARG && psn_arg_nbr == 0) || (FL_OUT_FROM_PSN_ARG && psn_arg_nbr == 1))){
	/* ...then try to obtain input files from stdin... */
	char *fl_in=NULL; /* [sng] Input file name */
	FILE *fp_in; /* [enm] Input file handle */
	char *bfr_in; /* [sng] Temporary buffer for stdin filenames */
	int cnv_nbr; /* [nbr] Number of scanf conversions performed this scan */
	long fl_lst_in_lng; /* [nbr] Number of characters in input file name list */
	char fmt_sng[10];
	size_t fl_nm_lng; /* [nbr] Filename length */

	if(nco_dbg_lvl_get() >= nco_dbg_scl) (void)fprintf(stderr,"%s: DEBUG nco_fl_lst_mk() reports input files not specified as positional arguments. Attempting to read from stdin instead...\n",nco_prg_nm_get());

	/* Initialize information to read stdin */
	fl_lst_in_lng=0L; /* [nbr] Number of characters in input file name list */

	if(fl_in == NULL){
	  fp_in=stdin; /* [enm] Input file handle */
	}else{
	  if((fp_in=fopen(fl_in,"r")) == NULL){
	    (void)fprintf(stderr,"%s: ERROR opening file containing input filename list %s\n",nco_prg_nm_get(),fl_in);
	    nco_exit(EXIT_FAILURE);
	  } /* endif err */
	} /* endelse */

	/* Allocate temporary space for input buffer */
#define FL_NM_IN_MAX_LNG 256 /* [nbr] Maximum length of single input file name */
#define FL_LST_IN_MAX_LNG 504576001 /* [nbr] Maximum length of input file list */
	/* 20121207: 10 MB per request of S. McGinnis 20121130
	   20140711: 50 MB per request of Christine Smit 20140711 for 30 years of hourly data files each 192 characters long */
	bfr_in=(char *)nco_malloc((FL_NM_IN_MAX_LNG+1L)*sizeof(char));
	(void)sprintf(fmt_sng,"%%%ds\n",FL_NM_IN_MAX_LNG);

	/* Assume filenames are whitespace-separated
	   Format string "%256s\n" tells scanf() to:
	   1. Skip any initial whitespace
	   2. Read first block of non-whitespace characters (up to 256 of them) into buffer
	   3. \n allows entries to be separated by carriage returns */
	while(((cnv_nbr=fscanf(fp_in,fmt_sng,bfr_in)) != EOF) && (fl_lst_in_lng < FL_LST_IN_MAX_LNG)){
	  if(cnv_nbr == 0){
	    (void)fprintf(stdout,"%s: ERROR stdin input not convertible to filename. HINT: Maximum length for input filenames is %d characters. HINT: Separate filenames with whitespace. Carriage returns are automatically stripped out.\n",nco_prg_nm_get(),FL_NM_IN_MAX_LNG);
	    nco_exit(EXIT_FAILURE);
	  } /* endif err */
	  fl_nm_lng=strlen(bfr_in);
	  fl_lst_in_lng+=fl_nm_lng;
	  (*fl_nbr)++;
	  if(nco_dbg_lvl_get() >= nco_dbg_scl) (void)fprintf(stderr,"%s: DEBUG input file #%d is \"%s\", filename length=%li\n",nco_prg_nm_get(),*fl_nbr,bfr_in,(long int)fl_nm_lng);
	  /* Increment file number */
	  fl_lst_in=(char **)nco_realloc(fl_lst_in,(*fl_nbr*sizeof(char *)));
	  fl_lst_in[(*fl_nbr)-1]=(char *)strdup(bfr_in);
	} /* end while */

	/* Finished reading list. Close file resource if one was opened. */
	if(fl_in != NULL && fp_in != NULL) (void)fclose(fp_in);

#if 0
	/* 20040621: Following flusher does no harm on Linux
	   However, AIX gets caught in an infinite loop here */
	/* Discard characters remainining in stdin */
	char chr_foo;
	while((chr_foo=getchar()) != '\n' && chr_foo != EOF){
	  if(nco_dbg_lvl_get() >= nco_dbg_scl) (void)fprintf(stderr,"%s: DEBUG Read and discarded \'%c\'\n",nco_prg_nm_get(),chr_foo);
	} /* end while */
#endif /* endif 0 */

	/* Free temporary buffer */
	bfr_in=(char *)nco_free(bfr_in);

	if(fl_lst_in_lng >= FL_LST_IN_MAX_LNG){
	  (void)fprintf(stdout,"%s: ERROR Total length of fl_lst_in from stdin exceeds %d characters. Possible misuse of feature. If your input file list is really this long, post request to developer's forum (http://sf.net/p/nco/discussion/9831) to expand FL_LST_IN_MAX_LNG\n",nco_prg_nm_get(),FL_LST_IN_MAX_LNG);
	  nco_exit(EXIT_FAILURE);
	} /* endif err */

	if(nco_dbg_lvl_get() >= nco_dbg_scl) (void)fprintf(stderr,"%s: DEBUG Read %d filenames in %li characters from stdin\n",nco_prg_nm_get(),*fl_nbr,(long)fl_lst_in_lng);
	if(*fl_nbr > 0) *FL_LST_IN_FROM_STDIN=True; else (void)fprintf(stderr,"%s: WARNING Tried and failed to get input filenames from stdin\n",nco_prg_nm_get());

      } /* endif multi-file operator without positional arguments for fl_in */

      if(!*FL_LST_IN_FROM_STDIN){
	if(FL_OUT_FROM_PSN_ARG) (void)fprintf(stdout,"%s: ERROR received %d filenames; need at least two\n",nco_prg_nm_get(),psn_arg_nbr); else (void)fprintf(stdout,"%s: ERROR received %d input filenames; need at least one (output file was specified with -o switch)\n",nco_prg_nm_get(),psn_arg_nbr);
	(void)nco_usg_prn();
	nco_exit(EXIT_FAILURE);
      } /* FL_LST_IN_FROM_STDIN */

    } /* end Operators with multiple fl_in and required fl_out */
    break;
  default: nco_dfl_case_prg_id_err(); break;
  } /* end switch */

  /* If input files are required but have not been obtained yet from stdin */
  if(!*FL_LST_IN_FROM_STDIN){
    /* Fill-in input file list from positional arguments */
    /* fxm: valgrind unfree'd memory in nco_fl_lst_mk() (nco_fl_utl.c:264) */
    fl_lst_in=(char **)nco_malloc((psn_arg_nbr-1+psn_arg_fst)*sizeof(char *));
    while(arg_crr < argc-1+psn_arg_fst) fl_lst_in[(*fl_nbr)++]=(char *)strdup(argv[arg_crr++]);
  } /* FL_LST_IN_FROM_STDIN */

  if(*fl_nbr == 0){
    (void)fprintf(stdout,"%s: ERROR Must specify input filename.\n",nco_prg_nm_get());
    (void)nco_usg_prn();
    nco_exit(EXIT_FAILURE);
  } /* end if */

  /* Sanitize input list from stdin and from positional arguments */
  //for(int fl_idx=0;fl_idx<*fl_nbr;fl_idx++) (void)nco_sng_sntz(fl_lst_in[fl_idx]);

  /* Assign output file from positional argument */
  if(FL_OUT_FROM_PSN_ARG){
    *fl_out=(char *)strdup(argv[argc-1]);
    //*fl_out=nco_sng_sntz(*fl_out);
  } /* !FL_OUT_FROM_PSN_ARG */
 
  return fl_lst_in;

} /* end nco_fl_lst_mk() */

char * /* O [sng] Filename of locally available file */
nco_fl_mk_lcl /* [fnc] Retrieve input file and return local filename */
(char *fl_nm, /* I/O [sng] Current filename, if any (destroyed) */
 const char * const fl_pth_lcl, /* I [sng] Local storage area for files retrieved from remote locations */
 const nco_bool HPSS_TRY, /* I [flg] Search HPSS for unfound files */
 nco_bool * const FL_RTR_RMT_LCN) /* O [flg] File was retrieved from remote location */
{
  /* Purpose: Locate input file, retrieve it from remote storage system if necessary,
     create local storage directory if neccessary, check file for read-access,
     return name of file on local system */

#ifdef _MSC_VER
  /* TODO nco1068 The stat function retuns -1 for large files; assume success */
  *FL_RTR_RMT_LCN=False;
  return fl_nm;
#endif

  FILE *fp_in;

  nco_bool DAP_URL=False; /* DAP handles netCDF API, no retrieval necessary */
  nco_bool FTP_URL=False; /* Retrieve remote file via FTP */
  nco_bool FTP_NETRC=False; /* Retrieve remote file via FTP with .netrc file */
  nco_bool FTP_OR_SFTP_URL; /* FTP or SFTP */
  nco_bool HTTP_URL=False; /* Retrieve remote file via wget */
  nco_bool SFTP_URL=False; /* Retrieve remote file via SFTP */

  char *cln_ptr; /* [ptr] Colon pointer */
  char *fl_nm_lcl;
  char *fl_nm_lcl_tmp;
  char *fl_nm_stub;
  char *fl_pth_lcl_tmp=NULL;

  const char fnc_nm[]="nco_fl_mk_lcl()"; /* [sng] Function name */
  const char dap4_url_sng[]="dap4://";
  const char ftp_url_sng[]="ftp://";
  const char http_url_sng[]="http://";
  const char https_url_sng[]="https://";
  const char sftp_url_sng[]="sftp://";

#ifdef ENABLE_DAP
  int rcd; /* [rcd] Return code */
#endif /* !ENABLE_DAP */
  int rcd_stt=0; /* [rcd] Return code from stat() */
  int rcd_sys; /* [rcd] Return code from system() */

  size_t url_sng_lng=0L; /* CEWI */

  struct stat stat_sct;

  /* Assume local filename is input filename */
  fl_nm_lcl=(char *)strdup(fl_nm);

  /* Remove any URL and machine-name components from local filename */
  if(strstr(fl_nm_lcl,sftp_url_sng) == fl_nm_lcl){
    SFTP_URL=True;
    url_sng_lng=strlen(sftp_url_sng);
  }else if(strstr(fl_nm_lcl,ftp_url_sng) == fl_nm_lcl){ /* !sftp */
    FTP_URL=True;
    url_sng_lng=strlen(ftp_url_sng);
  } /* !ftp */
  FTP_OR_SFTP_URL=FTP_URL || SFTP_URL;

  if(FTP_OR_SFTP_URL){
    /* If FTP rearrange fl_nm_lcl to remove ftp://hostname part, and
       if SFTP rearrange fl_nm_lcl to remove sftp://hostname: part,
       then search for file on local disk */
    fl_pth_lcl_tmp=strchr(fl_nm_lcl+url_sng_lng,'/');
    if(!fl_pth_lcl_tmp){
      (void)fprintf(stderr,"%s: ERROR %s unable to find valid filename component of SFTP path %s\n",nco_prg_nm_get(),fnc_nm,fl_nm_lcl);
      nco_exit(EXIT_FAILURE);
    } /* endif */
    fl_nm_lcl_tmp=fl_nm_lcl;
    fl_nm_lcl=(char *)nco_malloc(strlen(fl_pth_lcl_tmp)+1UL);
    (void)strcpy(fl_nm_lcl,fl_pth_lcl_tmp);
    fl_nm_lcl_tmp=(char *)nco_free(fl_nm_lcl_tmp);
  }else if((strstr(fl_nm_lcl,http_url_sng) == fl_nm_lcl) || (strstr(fl_nm_lcl,https_url_sng) == fl_nm_lcl) || (strstr(fl_nm_lcl,dap4_url_sng) == fl_nm_lcl)){
    /* Filename starts with "http://" or "https://" or "dap4://" so try DAP first (if available), then wget */

#ifdef ENABLE_DAP
    /* Filename has http:// prefix so try DAP access to unadulterated filename */
    int in_id; /* [id] Temporary input file ID */

    /* Attempt nc_open() on HTTP protocol files. Success means DAP found file.
       Do not worry about NC_DISKLESS here since any file will be immediately closed */
    rcd=nco_open_flg(fl_nm_lcl,NC_NOWRITE,&in_id);

    if(rcd == NC_NOERR){
      int nco_prg_id; /* Program ID */
      nco_prg_id=nco_prg_id_get(); /* [enm] Program ID */

      /* Fail gracefully if ncatted or ncrename try to use DAP */
      switch(nco_prg_id){
      case ncatted:
      case ncrename:
        (void)fprintf(stderr,"%s: ERROR %s reminds you that ncatted and ncrename must process truly local (i.e., not read via DAP) files (fxm TODO nco664)\n",nco_prg_nm_get(),fnc_nm);
        nco_exit(EXIT_FAILURE);
        break;
      default:
        /* All other operators work correctly with DAP */
        break;
      } /* end switch */

      /* Close file to prevent accumulating dangling open files on DAP server */
      rcd=nco_close(in_id);

      /* Great! DAP worked so file has earned DAP identification */
      DAP_URL=True; 

      if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stdout,"%s: INFO %s successfully accessed this file using the DAP protocol\n",nco_prg_nm_get(),fnc_nm);

      /* 20120728: Set rcd_stt=0 to mimic successful stat() return so rest of function treats file as local
	 (DAP treats HTTP protocol files as local files) */
      rcd_stt=0;

    }else{ /* DAP-access failed */
      (void)fprintf(stderr,"%s: INFO DAP-access to %s failed with error code %d. ",nco_prg_nm_get(),fl_nm_lcl,rcd);
      (void)fprintf(stderr,"Translation into English with nc_strerror(%d) is \"%s\"\n",rcd,nc_strerror(rcd));
      /* Error codes explained by Dennis Heimbigner in e-mail on 20110627 */
      if(rcd == NC_ECANTREAD) (void)fprintf(stderr,"%s: HINT DAP-access error code indicates that the URL does not exist. Is there a typo in the URL? Please verify that the file is accessible at the specified location on the DAP-server.\n",nco_prg_nm_get());
      if(rcd == NC_EDAPSVC) (void)fprintf(stderr,"%s: HINT DAP-access error code indicates that the URL _does_ exist, so error appears to be in the DAP server. Reasons for this could include 1. an aggregation server (e.g., GDS or THREDDS) that receives a URL that is valid according to the aggregation metadata but the implied back-end dataset may be temporarily unavailable. 2. The DAP server does not support the requested protocol, e.g., the server may support the DAP2 protocol but the dataset was requested with the DAP4 protocol.\n",nco_prg_nm_get());
      /* Error codes NC_EDDS, NC_EDAS, etc. are self-explanatory with nc_strerror() */
    } /* DAP-access failed */

#else /* !ENABLE_DAP */
    (void)fprintf(stderr,"%s: INFO Access to %s may (because filename starts with \"http://\") require DAP, but NCO was not enabled with DAP. Instead NCO will try to search for the file locally, and then will try wget.\n",nco_prg_nm_get(),fl_nm_lcl); 
    (void)fprintf(stderr,"%s: HINT: Obtain or build DAP-enabled NCO, e.g., with configure --enable-dap-netcdf ...\n",nco_prg_nm_get());
#endif /* !ENABLE_DAP */

    if(DAP_URL == False){ /* DAP access to http:// file failed */
      /* Attempt to retrieve URLs directly when DAP access fails. Tests:
	 ncks -D 2 -M http://dust.ess.uci.edu/nco/in.nc # wget
	 ncks -D 2 -M -l . http://dust.ess.uci.edu/nco/in.nc # wget
	 ncks -D 2 -M -l . -p http://dust.ess.uci.edu/nco in.nc # wget
	 ncks -D 2 -M -p http://dust.ess.uci.edu/cgi-bin/dods/nph-dods/dodsdata in.nc # DAP
	 ncks -O -D 2 -M -p http://dust.ess.uci.edu/cgi-bin/dods/nph-dods/dodsdata in.nc ~/foo.nc # DAP
	 ncks -O -v one -p http://thredds-test.ucar.edu/thredds/dodsC/testdods in.nc ~/foo.nc # DAP */
      if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stderr,"%s: INFO Will attempt to find file on local disk and, if unsuccessful, will then attempt retrieve remote file to local client using wget\n",nco_prg_nm_get());

      /* DAP cannot open file so leave DAP_URL=FALSE and set HTTP_URL=True
	 Later we will attempt to wget file to local system */
      HTTP_URL=True;
      url_sng_lng=strlen(http_url_sng);

      /* If HTTP then rearrange fl_nm_lcl to remove http://hostname part
      before searching for file on local disk */
      fl_pth_lcl_tmp=strchr(fl_nm_lcl+url_sng_lng,'/');
      if(!fl_pth_lcl_tmp){
	(void)fprintf(stderr,"%s: ERROR %s unable to find valid filename component of HTTP path %s\n",nco_prg_nm_get(),fnc_nm,fl_nm_lcl);
	nco_exit(EXIT_FAILURE);
      } /* endif */
      fl_nm_lcl_tmp=fl_nm_lcl;
      fl_nm_lcl=(char *)nco_malloc(strlen(fl_pth_lcl_tmp)+1UL);
      (void)strcpy(fl_nm_lcl,fl_pth_lcl_tmp);
      fl_nm_lcl_tmp=(char *)nco_free(fl_nm_lcl_tmp);

    } /* !DAP_URL, HTTP_URL */

  }else if((cln_ptr=strchr(fl_nm_lcl,':'))){
    /* 19990804
       Colon separates machine name from filename in rcp, scp, and sftp requests
       However, colon is also legal in _any_ UNIX filename
       Thus whether colon signifies rcp or scp request is somewhat ambiguous
       NCO treats names with more than one colon as regular filenames
       In order for colon to be interpreted as machine name delimiter,
       it must be preceded by period within three or four spaces, e.g., uci.edu: */
    if(((cln_ptr-4 >= fl_nm_lcl) && *(cln_ptr-4) == '.') ||
       ((cln_ptr-3 >= fl_nm_lcl) && *(cln_ptr-3) == '.')){
      /* Rearrange fl_nm_lcl to remove hostname: part */
      fl_pth_lcl_tmp=strchr(fl_nm_lcl+url_sng_lng,'/');
      if(!fl_pth_lcl_tmp){
	(void)fprintf(stderr,"%s: ERROR %s unable to find valid filename component of scp or rcp path %s\n",nco_prg_nm_get(),fnc_nm,fl_nm_lcl);
	  nco_exit(EXIT_FAILURE);
	} /* endif */
        fl_nm_lcl_tmp=fl_nm_lcl;
        fl_nm_lcl=(char *)nco_malloc(strlen(fl_pth_lcl_tmp)+1UL);
        (void)strcpy(fl_nm_lcl,fl_pth_lcl_tmp);
        fl_nm_lcl_tmp=(char *)nco_free(fl_nm_lcl_tmp);
    } /* endif period is three or four characters from colon */
  } /* end if */

  /* Does file exist on local system? */
  if(!DAP_URL) rcd_stt=stat(fl_nm_lcl,&stat_sct);
  if(rcd_stt == -1 && (nco_dbg_lvl_get() >= nco_dbg_fl)) (void)fprintf(stderr,"\n%s: INFO stat() #1 failed: %s does not exist\n",nco_prg_nm_get(),fl_nm_lcl);

  /* If not, does file exist on local system under same path interpreted relative to current working directory? */
  if(rcd_stt == -1){
    if(fl_nm_lcl[0] == '/'){
      rcd_stt=stat(fl_nm_lcl+1UL,&stat_sct);
      if(rcd_stt == -1 && (nco_dbg_lvl_get() >= nco_dbg_fl)) (void)fprintf(stderr,"%s: INFO stat() #2 failed: %s does not exist\n",nco_prg_nm_get(),fl_nm_lcl+1UL);
    } /* end if */
    if(rcd_stt == 0){
      /* NB: Adding one to filename pointer is like deleting initial slash on filename
	 Then free(fl_nm_lcl) would miss this initial byte (memory is lost)
	 Hence must copy new name into its own memory space */
      fl_nm_lcl_tmp=(char *)strdup(fl_nm_lcl+1UL);
      fl_nm_lcl=(char *)nco_free(fl_nm_lcl);
      fl_nm_lcl=fl_nm_lcl_tmp;
      (void)fprintf(stderr,"%s: WARNING not searching for %s on remote filesystem, using local file %s instead\n",nco_prg_nm_get(),fl_nm,fl_nm_lcl+1UL);
    } /* end if */
  } /* end if */

  /* Finally, check if file exists locally in directory for remotely retrieved files
     This occurs when previous program invocations have already retrieved some files */
  if(rcd_stt == -1){
    /* Where does filename stub begin? 
       NB: Assume local filename has a slash (because remote file system always has a slash) */
    fl_nm_stub=strrchr(fl_nm_lcl,'/')+1UL;

    /* Construct local filename from user-supplied local file path and existing file stub */
    if(fl_pth_lcl){
      fl_nm_lcl_tmp=fl_nm_lcl;
      /* Allocate enough room for the joining slash '/' and the terminating NUL */
      fl_nm_lcl=(char *)nco_malloc((strlen(fl_pth_lcl)+strlen(fl_nm_stub)+2)*sizeof(char));
      (void)strcpy(fl_nm_lcl,fl_pth_lcl);
      (void)strcat(fl_nm_lcl,"/");
      (void)strcat(fl_nm_lcl,fl_nm_stub);
      /* Free old filename space */
      fl_nm_lcl_tmp=(char *)nco_free(fl_nm_lcl_tmp);
    } /* end if */

    /* At last, check for file in local storage directory */
    rcd_stt=stat(fl_nm_lcl,&stat_sct);
    if(rcd_stt != -1) (void)fprintf(stderr,"%s: WARNING not searching for %s on remote filesystem, using local file %s instead\n",nco_prg_nm_get(),fl_nm,fl_nm_lcl);
    if(rcd_stt == -1 && (nco_dbg_lvl_get() >= nco_dbg_fl)) (void)fprintf(stderr,"%s: INFO stat() #3 failed: %s does not exist\n",nco_prg_nm_get(),fl_nm_lcl);
  } /* end if */

  /* File was not found locally and is not DAP-accessible, try to fetch file from remote filesystem */
  if(rcd_stt == -1){

    typedef struct{ /* [enm] Remote fetch command structure */
      const char *fmt; /* [] Format */
      const int fmt_chr_nbr; /* [nbr] Number of formatting characters */
      const int transfer_mode; /* [enm] Transfer mode */
      const int file_order; /* [enm] File order */
    } rmt_fch_cmd_sct;

    char *cmd_sys;
    char *fl_nm_rmt;
    char *fmt_ftp=NULL; /* [sng] Declare outside FTP block scope for easier freeing */

    /* fxm: use autoconf HAVE_MKDIR_M rather than SUN4 token TODO nco293 */
#ifndef SUN4
    const char cmd_mkdir[]="mkdir -m 777 -p";
#else /* SUN4 */
    const char cmd_mkdir[]="mkdir -p";
#endif /* SUN4 */

    enum{ /* [enm] Transfer mode */
      synchronous, /* Syncronous transfer */
      asynchronous}; /* Asynchronous transfer */

      enum{ /* [enm] File order */
        lcl_rmt, /* Local file argument before remote file argument */
        rmt_lcl}; /* Remote file argument before local file argument */

        size_t fl_pth_lcl_lng=size_t_CEWI; /* CEWI */

        rmt_fch_cmd_sct *rmt_cmd=NULL;
        /* fxm: Initialize structure contents as const */
        rmt_fch_cmd_sct hsiget={"hsi get %s : %s",4,synchronous,lcl_rmt};
        /* rmt_fch_cmd_sct msread={"msread -R %s %s",4,synchronous,lcl_rmt};*/ /* Deprecated 20110419 */
        /* rmt_fch_cmd_sct msrcp={"msrcp mss:%s %s",4,synchronous,rmt_lcl};*/ /* Deprecated 20110419 */
        /* rmt_fch_cmd_sct nrnet={"nrnet msget %s r flnm=%s l mail=FAIL",4,asynchronous,lcl_rmt};*/ /* Deprecated 20110419 */
        /* rmt_fch_cmd_sct rcp={"rcp -p %s %s",4,synchronous,rmt_lcl};*/ /* Deprecated ~2000 */
        /* wget -p: fxm (and enables clobber)
        wget -r: Turn-on recursive retrieving (and enables clobber)
        wget --tries: Set number of retries. Default is 20. */
        rmt_fch_cmd_sct http={"wget --tries=1 --output-document=%s %s",4,synchronous,lcl_rmt};
        rmt_fch_cmd_sct scp={"scp -p %s %s",4,synchronous,rmt_lcl};
        rmt_fch_cmd_sct sftp={"sftp %s %s",4,synchronous,rmt_lcl};
        /* Fill-in ftp structure fmt element dynamically later */
        rmt_fch_cmd_sct ftp={"",4,synchronous,rmt_lcl};

        /* Why did stat() command fail? */
        /* (void)perror(nco_prg_nm_get());*/

        /* Remote filename is input filename by definition */
        fl_nm_rmt=fl_nm;

        /* URL specifier in filename unambiguously signals to use FTP */
        if(!rmt_cmd){
          if(FTP_URL){
            /* fxm: use autoconf HAVE_XXX rather than WIN32 token TODO nco292 */
#ifdef WIN32
            /* #ifndef HAVE_NETWORK fxm */
            /* I have no idea how FTP networking calls work in MS Windows, so just exit */
            (void)fprintf(stdout,"%s: ERROR Networking required to obtain %s is not supported by this operating system\n",nco_prg_nm_get(),fl_nm_rmt);
            nco_exit(EXIT_FAILURE);
#else /* !WIN32 */
            char *usr_nm;
            char *host_nm_lcl;
            char *host_nm_rmt;
            char *usr_email=NULL; /* CEWI */
            char *fl_nm_netrc;

            const char fl_stb_netrc[]="/.netrc";
            const char ftp_cmd_anonymous[]="/usr/bin/ftp -i -p -n"; /* -n turns off using .netrc */
            const char ftp_cmd_netrc[]="/usr/bin/ftp -i -p"; /* Allow FTP to use .netrc */
            const char fmt_ftp_anonymous_tpl[]="%s %s << END\nuser anonymous %s\nbin\nget %s %s\nquit\nEND";
            const char fmt_ftp_netrc_tpl[]="%s %s << END\nbin\nget %s %s\nquit\nEND";

            struct passwd *usr_pwd;

            uid_t usr_uid;

            rmt_cmd=&ftp;

            /* Get UID to get password structure which contains home directory, login name
	       Home directory needed to search for .netrc
	       Login name used to construct e-mail address for anonymous FTP */
            usr_uid=getuid();
            usr_pwd=getpwuid(usr_uid);
            usr_nm=usr_pwd->pw_name;

            /* Construct remote hostname and filename now since:
	       1. .netrc, if any, will soon be searched for remote hostname
	       2. Remote hostname and filename always needed for remote retrieval */

            /* Remote hostname begins directly after "[s]ftp://" */
            host_nm_rmt=fl_nm_rmt+url_sng_lng;
            /* Filename begins after slash */
            fl_nm_rmt=strstr(fl_nm_rmt+url_sng_lng,"/")+1UL;
            /* NUL-terminate hostname by overwriting colon in hostname:/filename syntax */
            *(fl_nm_rmt-1)='\0';

            /* Use anonymous FTP unless .netrc file exists on local system */
            fl_nm_netrc=(char *)strdup(usr_pwd->pw_dir);
            /* Create space for full path to ${HOME}/.netrc */
            fl_nm_netrc=(char *)nco_realloc(fl_nm_netrc,(strlen(fl_nm_netrc)+strlen(fl_stb_netrc)+2UL)*sizeof(char));
            fl_nm_netrc=(char *)strcat(fl_nm_netrc,fl_stb_netrc);
            rcd_stt=stat(fl_nm_netrc,&stat_sct);
            if(!rcd_stt){
              /* Search .netrc file for remote host name */
              char *host_nm_rmt_psn;
              char *fl_netrc_bfr;
              FILE *fp_netrc; /* [fl] .netrc inpu file handle */
	      int rcd_frd; /* [rcd] Return code from fread() and fclose() */
              if((fp_netrc=fopen(fl_nm_netrc,"r")) == NULL){
                (void)fprintf(stderr,"%s: ERROR unable to open user's .netrc file %s\n",nco_prg_nm_get(),fl_nm_netrc);
                /* Why did fopen() command fail? */
                (void)perror(nco_prg_nm_get());
                nco_exit(EXIT_FAILURE);
              } /* end if */
              /* Add one for NUL-terminator */
              fl_netrc_bfr=(char *)nco_malloc((1UL+stat_sct.st_size)*sizeof(char));
              /* NUL-terminate buffer */
              fl_netrc_bfr[stat_sct.st_size]='\0';
              rcd_frd=fread((void *)fl_netrc_bfr,stat_sct.st_size,1,fp_netrc);
              if(rcd_frd <= 0){
                (void)fprintf(stderr,"%s: ERROR reading %s\n",nco_prg_nm_get(),fl_nm_netrc);
                /* Why did fread() command fail? */
                (void)perror(nco_prg_nm_get());
                /* Check for feof() vs. ferror() here? */
                rcd_frd=fclose(fp_netrc);
                nco_exit(EXIT_FAILURE);
              } /* end if */
              rcd_frd=fclose(fp_netrc);
              host_nm_rmt_psn=strstr(fl_netrc_bfr,host_nm_rmt);
              if(host_nm_rmt_psn){
                FTP_NETRC=True;
                if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stderr,"%s: INFO %s will use .netrc file at %s instead of anonymous FTP\n",nco_prg_nm_get(),fnc_nm,fl_nm_netrc);
              } /* endif host_nm_rmt_psn */
              fl_netrc_bfr=(char *)nco_free(fl_netrc_bfr);
            } /* endif rcd_stt */

            if(!FTP_NETRC){
              /* DEBUG: 256 should be replaced by MAXHOSTNAMELEN from <sys/param.h>, but 
		 Solaris does not have MAXHOSTNAMELEN in sys/param.h */
              host_nm_lcl=(char *)nco_malloc((256UL+1UL)*sizeof(char));
              (void)gethostname(host_nm_lcl,256UL+1UL);
              /* fxm: move to gethostbyname() next */
              if(!strchr(host_nm_lcl,'.')){
                /* #ifdef HAVE_RES_ */
                /* Returned hostname did not include fully qualified Internet domain name (FQDN) */
                (void)res_init();
                (void)strcat(host_nm_lcl,".");
                (void)strcat(host_nm_lcl,_res.defdname);
                /* #endif HAVE_RES_ */
              } /* end if */

              /* Add one for joining "@" and one for NUL byte */
              usr_email=(char *)nco_malloc((strlen(usr_nm)+1UL+strlen(host_nm_lcl)+1UL)*sizeof(char));
              (void)sprintf(usr_email,"%s@%s",usr_nm,host_nm_lcl);
              /* Free hostname space */
              host_nm_lcl=(char *)nco_free(host_nm_lcl);
            } /* FTP_NETRC */

            if(FTP_NETRC){
              /* Subtract four characters for two "percent s" formats replaced by new strings, add one for NUL byte */
              fmt_ftp=(char *)nco_malloc((strlen(fmt_ftp_netrc_tpl)+strlen(ftp_cmd_netrc)+strlen(host_nm_rmt)-4UL+1UL)*sizeof(char));
              (void)sprintf(fmt_ftp,fmt_ftp_netrc_tpl,ftp_cmd_netrc,host_nm_rmt,"%s","%s");
            }else{
              /* Subtract six characters for three "percent s" formats replaced by new strings, add one for NUL byte */
              fmt_ftp=(char *)nco_malloc((strlen(fmt_ftp_anonymous_tpl)+strlen(ftp_cmd_anonymous)+strlen(host_nm_rmt)+strlen(usr_email)-6UL+1UL)*sizeof(char));
              (void)sprintf(fmt_ftp,fmt_ftp_anonymous_tpl,ftp_cmd_anonymous,host_nm_rmt,usr_email,"%s","%s");
            } /* !FTP_NETRC */
            rmt_cmd->fmt=fmt_ftp;
            /* Free space, if any, holding user's E-mail address */
            if(!FTP_NETRC) usr_email=(char *)nco_free(usr_email);
            /* Always free .netrc filename space */
            fl_nm_netrc=(char *)nco_free(fl_nm_netrc);
#endif /* !WIN32 */
          } /* end if FTP_URL */
        } /* end if rmt_cmd */

        /* Currently, sftp transfers are indicated by FTP-style URLs
	   NB: Unlike FTP, SFTP does not have a recognized URL format
	   Hence actual transfer via SFTP uses scp syntax (for single files)
	   Multiple file transfer via SFTP can use FTP-like scripts, requires more work
	   NCO SFTP file specification must have colon separating hostname from filename */
        if(!rmt_cmd){
          if(SFTP_URL){
            /* Remote filename begins after URL but includes hostname */
            fl_nm_rmt+=url_sng_lng;
            /* Make sure that SFTP hostname looks like a hostname */
            if((cln_ptr=strchr(fl_nm_rmt,':'))){
              if(((cln_ptr-4 >= fl_nm_rmt) && *(cln_ptr-4) == '.') ||
                ((cln_ptr-3 >= fl_nm_rmt) && *(cln_ptr-3) == '.')){
                  rmt_cmd=&sftp;
              } /* end if */
	    } /* end if colon */
          } /* end if SFTP */
        } /* end if rmt_cmd */

        /* Attempt wget on files that contain http:// prefix and are not accessible via DAP */
        if(!rmt_cmd){
          if(HTTP_URL){
            rmt_cmd=&http;
            (void)fprintf(stderr,"%s: INFO Will now attempt wget on the full filepath. wget will fail if the file is \"hidden\" behind a DAP server. Unfortunately, failed wget attempts creates rather long pathnames in the current directory. fxm TODO nco970, nco971. On the other hand, wget should succeed if the file is stored in any publicly-accessible web location.\n",nco_prg_nm_get());
          } /* end if HTTP */
        } /* end if rmt_cmd */

        /* Otherwise, single colon preceded by period in filename signals rcp or scp
	   Determining whether to try scp instead of rcp is difficult
	   Ideally, NCO would test remote machine for rcp/scp priveleges with system command like, e.g., "ssh echo ok"
	   To start we use scp which has its own fall-through to rcp */
        if(!rmt_cmd){
          if((cln_ptr=strchr(fl_nm_rmt,':'))){
            if(((cln_ptr-4 >= fl_nm_rmt) && *(cln_ptr-4) == '.') ||
              ((cln_ptr-3 >= fl_nm_rmt) && *(cln_ptr-3) == '.')){
                rmt_cmd=&scp;
            } /* end if */
          } /* end if colon */
        } /* end if rmt_cmd */

#if 0
        /* NB: MSS commands deprecated 20110419 */
        if(!rmt_cmd){
          /* Does msrcp command exist on local system? */
          rcd_stt=stat("/usr/local/bin/msrcp",&stat_sct); /* SCD Dataproc, Ouray */
          if(rcd_stt != 0) rcd_stt=stat("/usr/bin/msrcp",&stat_sct); /* ACD Linux */
          if(rcd_stt != 0) rcd_stt=stat("/opt/local/bin/msrcp",&stat_sct); /* CGD */
          if(rcd_stt != 0) rcd_stt=stat("/usr/local/dcs/bin/msrcp",&stat_sct); /* ACD */
          if(rcd_stt == 0) rmt_cmd=&msrcp;
        } /* end if */

        if(!rmt_cmd){
          /* Does msread command exist on local system? */
          rcd_stt=stat("/usr/local/bin/msread",&stat_sct);
          if(rcd_stt == 0) rmt_cmd=&msread;
        } /* end if */

        if(!rmt_cmd){
          /* Does nrnet command exist on local system? */
          rcd_stt=stat("/usr/local/bin/nrnet",&stat_sct);
          if(rcd_stt == 0) rmt_cmd=&nrnet;
        } /* end if */

        /* Before we look for file on remote system, be sure
	   filename has correct syntax to exist on remote system */
        if(rmt_cmd == &msread || rmt_cmd == &nrnet || rmt_cmd == &msrcp){
          if (fl_nm_rmt[0] != '/' || fl_nm_rmt[1] < 'A' || fl_nm_rmt[1] > 'Z'){
            (void)fprintf(stderr,"%s: ERROR %s is not on local filesystem and is not a syntactically valid filename on remote file system\n",nco_prg_nm_get(),fl_nm_rmt);
            nco_exit(EXIT_FAILURE);
          } /* end if */
        } /* end if */
#endif /* endif False */

        /* NB: HPSS commands replaced MSS commands in NCO 4.0.8 in 201104 */
        if(!rmt_cmd){
	  if(HPSS_TRY){
	    /* Does hsi command exist on local system? */
	    rcd_stt=system("which hsi"); /* Generic location on user's PATH */
	    if(rcd_stt != 0) rcd_stt=stat("/usr/local/bin/hsi",&stat_sct); /* CISL Bluefire default */
	    if(rcd_stt != 0) rcd_stt=stat("/opt/hpss/bin/hsi",&stat_sct); /* CISL alternate */
	    if(rcd_stt != 0) rcd_stt=stat("/usr/common/mss/bin/hsi",&stat_sct); /* Cori/Edison */
	    if(rcd_stt != 0) rcd_stt=stat("/ncar/opt/hpss/hsi",&stat_sct); /* Yellowstone default added to NCO 4.3.2 in 201306 */
	    if(rcd_stt == 0) rmt_cmd=&hsiget;
	  } /* !HPSS_TRY */
        } /* end if */

        if(!rmt_cmd){
          (void)fprintf(stderr,"%s: ERROR file %s not found. It does not exist on the local filesystem, nor does it match remote filename patterns (e.g., http://foo or foo.bar.edu:file)%s\n",nco_prg_nm_get(),fl_nm_rmt,(HPSS_TRY) ? ", nor did NCO detect a remote High Performance Storage System (HPSS) accessible via the 'hsi' command." : ".");
	  (void)fprintf(stderr,"%s: HINT file-not-found errors usually arise from filename typos, incorrect paths, missing files, or capricious gods. Please verify spelling and location of requested file.%s\n",nco_prg_nm_get(),(HPSS_TRY) ? "" : " If the file resides on a High Performance Storage System (HPSS) accessible via the 'hsi' command, then add the --hpss option and re-try command.");
          nco_exit(EXIT_FAILURE);
        } /* end if */

        if(fl_pth_lcl == NULL){
          /* Derive path for storing local file from remote filename */
          (void)fprintf(stderr,"%s: INFO Unable to find file %s on local system. Found hsi command indicating presence of High Performance Storage System (HPSS). Will assume file is stored on HPSS. Received no local path information and so will try to derive suitable local filepath from given filename...\n",nco_prg_nm_get(),fl_nm_lcl);
	  /* Unlike old MSS, HPSS paths need not be absolute, i.e., begin with slash
	     HPSS is smart, HPSS:${HOME}/foo and foo are same file
	     Search backwards from end for last path separator */
	  fl_nm_stub=strrchr(fl_nm_lcl,'/');
	  if(fl_nm_stub){
	    /* Successful search points to slash, add one to point to stub */
	    fl_nm_stub++;
	  }else{
	    fl_nm_stub=fl_nm_lcl;
	  } /* !fl_nm_stb */
	  /* HPSS has no restrictions on filename syntax, following is deprecated:
	  (void)fprintf(stderr,"%s: ERROR %s unable to find path component of requested file %s. HPSS filenames must have a multi-component path structure (i.e., contain slashes).\n",nco_prg_nm_get(),fnc_nm,fl_nm_lcl);
	  (void)fprintf(stderr,"%s: HINT This error often occurs because of a simple filename typo or missing file. NCO calls exit() with a simpler error message when it cannot find a specified file on systems without HPSS clients. NCO just performed and failed a more elaborate search for the file because this system appears to have hsi (see http://nco.sf.net/nco.html#hsi). Please verify spelling/location of requested file.\n",nco_prg_nm_get());
	  nco_exit(EXIT_FAILURE); */
	  if(HTTP_URL){
            /* Strip leading slash from fl_nm_lcl for HTTP files so, e.g., 
	       http://dust.ess.uci.edu/nco/in.nc produces local path "nco" not "/nco" */
            fl_nm_lcl_tmp=(char *)strdup(fl_nm_lcl+1UL);
            fl_nm_lcl=(char *)nco_free(fl_nm_lcl);
            fl_nm_lcl=fl_nm_lcl_tmp;
          } /* !HTTP_URL */
          /* Construct local storage filepath name */
          if(fl_nm_stub != fl_nm_lcl) fl_pth_lcl_lng=strlen(fl_nm_lcl)-strlen(fl_nm_stub)-1UL; else fl_pth_lcl_lng=0L;
          /* Allocate enough room for terminating NUL */
          fl_pth_lcl_tmp=(char *)nco_malloc((fl_pth_lcl_lng+1UL)*sizeof(char));
          (void)strncpy(fl_pth_lcl_tmp,fl_nm_lcl,fl_pth_lcl_lng);
          fl_pth_lcl_tmp[fl_pth_lcl_lng]='\0';
          /* Tell user what local filepath was derived */
          (void)fprintf(stderr,"%s: INFO Retrieved files will be stored in derived directory ./%s\n",nco_prg_nm_get(),fl_pth_lcl_tmp);
        }else{
          /* Copy user-specified local path to unite following code in terms of fl_pth_lcl_tmp */
          fl_pth_lcl_tmp=(char *)strdup(fl_pth_lcl);
        } /* fl_pth_lcl */

        /* Does local filepath exist already on local system? */
        rcd_stt=stat(fl_pth_lcl_tmp,&stat_sct);
        /* If not, then create local filepath if one is needed */
        if(rcd_stt != 0 && fl_pth_lcl_lng > 0L){
          /* Allocate enough room for joining space ' ' and terminating NUL */
          cmd_sys=(char *)nco_malloc((strlen(cmd_mkdir)+fl_pth_lcl_lng+2UL)*sizeof(char));
          (void)strcpy(cmd_sys,cmd_mkdir);
          (void)strcat(cmd_sys," ");
          (void)strcat(cmd_sys,fl_pth_lcl_tmp);
          rcd_sys=system(cmd_sys);
          if(rcd_sys != 0){
            (void)fprintf(stderr,"%s: ERROR Unable to create local directory %s\n",nco_prg_nm_get(),fl_pth_lcl_tmp);
            if(fl_pth_lcl == NULL) (void)fprintf(stderr,"%s: HINT Use -l option\n",nco_prg_nm_get());
            nco_exit(EXIT_FAILURE);
          } /* end if */
          if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stderr,"%s: INFO Created local directory ./%s\n",nco_prg_nm_get(),fl_pth_lcl_tmp);
          /* Free local command space */
          cmd_sys=(char *)nco_free(cmd_sys);
        } /* end if */

        /* Free local path space, if any */
        fl_pth_lcl_tmp=(char *)nco_free(fl_pth_lcl_tmp);

        /* Allocate enough room for joining space ' ' and terminating NUL */
        cmd_sys=(char *)nco_malloc((strlen(rmt_cmd->fmt)-rmt_cmd->fmt_chr_nbr+strlen(fl_nm_lcl)+strlen(fl_nm_rmt)+2)*sizeof(char));
        if(rmt_cmd->file_order == lcl_rmt){
          (void)sprintf(cmd_sys,rmt_cmd->fmt,fl_nm_lcl,fl_nm_rmt);
        }else{
          (void)sprintf(cmd_sys,rmt_cmd->fmt,fl_nm_rmt,fl_nm_lcl);
        } /* end else */
        if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stderr,"%s: Retrieving file from remote location with command:\n%s\n",nco_prg_nm_get(),cmd_sys);
        (void)fflush(stderr);
        /* Fetch file from remote file system */
        rcd_sys=system(cmd_sys);
        /* Free local command space */
        cmd_sys=(char *)nco_free(cmd_sys);

        /* Free dynamically allocated FTP script memory */
        if(rmt_cmd == &ftp) fmt_ftp=(char *)nco_free(fmt_ftp);

        if(rmt_cmd->transfer_mode == synchronous){
          if(rcd_sys != 0){
            (void)fprintf(stderr,"%s: ERROR Synchronous fetch command failed\n",nco_prg_nm_get());
            nco_exit(EXIT_FAILURE);
          } /* end if */
        }else{
          /* This is appropriate place to insert invocation of shell command
	     to retrieve file asynchronously and then to return status to NCO synchronously. */

          int fl_sz_crr=-2;
          int fl_sz_ntl=-1;
          int tm_nbr=100; /* Maximum number of sleep periods before error exit */
          int tm_idx;
          int tm_sleep_scn=10; /* [s] Seconds per stat() check for successful return */
#ifdef _MSC_VER
          /* MSVC NB: Win32 Sleep() function measured in milliseconds, Linux sleep() function measured in seconds */
          int tm_sleep_ms=tm_sleep_scn*1000; /* [ms] Milliseconds per stat() check for successful return */
#endif /* !_MSC_VER */

          /* Asynchronous retrieval uses sleep-and-poll technique */
          for(tm_idx=0;tm_idx<tm_nbr;tm_idx++){
            rcd_stt=stat(fl_nm_lcl,&stat_sct);
            if(rcd_stt == 0){
              /* What is current size of file? */
              fl_sz_ntl=fl_sz_crr;
              fl_sz_crr=stat_sct.st_size;
              /* If file size does not change during entire sleep period, assume
              file is completely retrieved. */
              if(fl_sz_ntl == fl_sz_crr){
                break;
              } /* end if */
            } /* end if */
            /* Sleep for specified time */
#ifdef _MSC_VER
            (void)Sleep((unsigned)tm_sleep_ms);
#else /* !_MSC_VER */
            (void)sleep((unsigned)tm_sleep_scn);
#endif /* !_MSC_VER */
            if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stderr,".");
            (void)fflush(stderr);
          } /* end for */
          if(tm_idx == tm_nbr){
            (void)fprintf(stderr,"%s: ERROR Maximum time (%d seconds = %.1f minutes) for asynchronous file retrieval exceeded.\n",nco_prg_nm_get(),tm_nbr*tm_sleep_scn,tm_nbr*tm_sleep_scn/60.0);
            nco_exit(EXIT_FAILURE);
          } /* end if */
          if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stderr,"\n%s Retrieval successful after %d sleeps of %d seconds each = %.1f minutes\n",nco_prg_nm_get(),tm_idx,tm_sleep_scn,tm_idx*tm_sleep_scn/60.0);
        } /* end else transfer mode is asynchronous */
        *FL_RTR_RMT_LCN=True;
  }else{ /* end if input file did not exist locally */
    *FL_RTR_RMT_LCN=False;
  } /* end if file was already on the local system */

  if(nco_dbg_lvl_get() >= nco_dbg_fl)
    if(DAP_URL && fl_pth_lcl)
      (void)fprintf(stderr,"%s: INFO User-specified option \"-l %s\" was not used since input file was not retrieved from remote location\n",nco_prg_nm_get(),fl_pth_lcl);

  if(!DAP_URL){
    /* File is (now, anyway) truly local---does local system have read permission? */
    if((fp_in=fopen(fl_nm_lcl,"r")) == NULL){
      (void)fprintf(stderr,"%s: ERROR User does not have read permission for %s, or file does not exist\n",nco_prg_nm_get(),fl_nm_lcl);
      nco_exit(EXIT_FAILURE);
    }else{
      (void)fclose(fp_in);
    } /* end else */

    /* For local files, perform optional file diagnostics */
    if(nco_dbg_lvl_get() >= nco_dbg_std){
      char *fl_nm_cnc=NULL; /* [sng] Canonical file name */
      /* Determine canonical filename and properties */
      fl_nm_cnc=nco_fl_info_get(fl_nm_lcl);
      if(fl_nm_cnc) fl_nm_cnc=(char *)nco_free(fl_nm_cnc);
    } /* endif dbg */

  } /* end if file is truly local */

  /* Free input filename space */
  fl_nm=(char *)nco_free(fl_nm);

  /* Return local filename */
  return(fl_nm_lcl);

} /* end nco_fl_mk_lcl() */

void
nco_fl_mv /* [fnc] Move first file to second */
(const char * const fl_src, /* I [sng] Name of source file to move */
 const char * const fl_dst) /* I [sng] Name of destination file */
{
  /* Purpose: Move first file to second */
  char *cmd_mv;
  char *fl_dst_cdl;
  char *fl_src_cdl;

#ifdef _MSC_VER
  const char cmd_mv_fmt[]="move %s %s";
#else /* !_MSC_VER */
  const char cmd_mv_fmt[]="/bin/mv -f %s %s";
#endif /* !_MSC_VER */

  int rcd_sys; /* [rcd] Return code from system() */
  const int fmt_chr_nbr=4;

  /* Only bother to perform system() call if files are not identical */
  if(!strcmp(fl_src,fl_dst)){
    if(nco_dbg_lvl_get() >= nco_dbg_fl) (void)fprintf(stderr,"%s: INFO Temporary and final files %s are identical---no need to move.\n",nco_prg_nm_get(),fl_src);
    return;
  } /* end if */

  /* 20131227 Allow for whitespace characters in fl_dst 
     Assume CDL translation results in acceptable name for shell commands */
  fl_src_cdl=nm2sng_fl(fl_src);
  fl_dst_cdl=nm2sng_fl(fl_dst);

  /* Construct and execute move command */
  cmd_mv=(char *)nco_malloc((strlen(cmd_mv_fmt)+strlen(fl_src_cdl)+strlen(fl_dst_cdl)-fmt_chr_nbr+1UL)*sizeof(char));
  if(nco_dbg_lvl_get() >= nco_dbg_fl) (void)fprintf(stderr,"%s: INFO Moving %s to %s...",nco_prg_nm_get(),fl_src_cdl,fl_dst_cdl);
  (void)sprintf(cmd_mv,cmd_mv_fmt,fl_src_cdl,fl_dst_cdl);
  rcd_sys=system(cmd_mv);
  /* 20160802: Until today, failure was diagnosed iff rcd == -1
     Unclear what rcd == -1 actually means to systems, because rcd == 0 always indicates success and
     Linux rcd     > 0 indicates failure
     MacOS BSD rcd > 0 indicates failure */
  if(rcd_sys > 0){
    (void)fprintf(stdout,"%s: ERROR nco_fl_mv() unable to execute mv command \"%s\"\n",nco_prg_nm_get(),cmd_mv);
    nco_exit(EXIT_FAILURE);
  } /* end if */
  if(nco_dbg_lvl_get() >= nco_dbg_fl) (void)fprintf(stderr,"done\n");

  cmd_mv=(char *)nco_free(cmd_mv);
  if(fl_dst_cdl) fl_dst_cdl=(char *)nco_free(fl_dst_cdl);
  if(fl_src_cdl) fl_src_cdl=(char *)nco_free(fl_src_cdl);
} /* end nco_fl_mv() */

char * /* O [sng] Name of file to retrieve */
nco_fl_nm_prs /* [fnc] Construct file name from input arguments */
(char *fl_nm, /* I/O [sng] Current filename, if any */
 const int fl_idx, /* I [nbr] Ordinal index of file in input file list */
 int * const fl_nbr, /* I/O [nbr] Number of files to be processed */
 char * const * const fl_lst_in, /* I [sng] User-specified filenames */
 const int abb_arg_nbr, /* I [nbr] Number of abbreviation arguments */
 CST_X_PTR_CST_PTR_CST_Y(char,fl_lst_abb), /* I [sng] NINTAP-style arguments, if any */
 const char * const fl_pth) /* I [sng] Path prefix for files in fl_lst_in */
{
  /* Purpose: Construct file name from various input arguments and switches
     Routine implements NINTAP-style specification by using static
     memory to avoid repetition in construction of filename */

  char *sng_cnv_rcd=NULL_CEWI; /* [sng] strtol()/strtoul() return code */

  static char *fl_nm_1st_dgt;
  static char *fl_nm_nbr_sng;
  static char fl_nm_nbr_sng_fmt[10];

  static int fl_nm_nbr_crr;
  static int fl_nm_nbr_dgt;
  static int fl_nm_nbr_ncr;
  static int fl_nm_nbr_max;
  static int fl_nm_nbr_min;
  static int fl_nm_nbr_ttl;
  static int mm_crr;
  static int yyyy_crr;

  static nco_bool FIRST_INVOCATION=True;
  static nco_bool flg_yyyymm=False;

  /* Free any old filename space */
  fl_nm=(char *)nco_free(fl_nm);

  /* Construct filename from NINTAP-style arguments and input name */
  if(fl_lst_abb){
    if(FIRST_INVOCATION){
      int fl_nm_sfx_lng=0;

      /* Parse abbreviation list analogously to CCM Processor ICP "NINTAP" */
      if(fl_nbr){
	*fl_nbr=(int)strtol(fl_lst_abb[0],&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
	if(*sng_cnv_rcd) nco_sng_cnv_err(fl_lst_abb[0],"strtol",sng_cnv_rcd);
	fl_nm_nbr_ttl=*fl_nbr;
      } /* endif */

      if(abb_arg_nbr > 1){
	fl_nm_nbr_dgt=(int)strtol(fl_lst_abb[1],&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
	if(*sng_cnv_rcd) nco_sng_cnv_err(fl_lst_abb[1],"strtol",sng_cnv_rcd);
      }else{
	fl_nm_nbr_dgt=3;
      } /* end if */

      if(abb_arg_nbr > 2){
	fl_nm_nbr_ncr=(int)strtol(fl_lst_abb[2],&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
	if(*sng_cnv_rcd) nco_sng_cnv_err(fl_lst_abb[2],"strtol",sng_cnv_rcd);
      }else{
	fl_nm_nbr_ncr=1;
      } /* end if */

      if(abb_arg_nbr > 3){
	fl_nm_nbr_max=(int)strtol(fl_lst_abb[3],&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
	if(*sng_cnv_rcd) nco_sng_cnv_err(fl_lst_abb[3],"strtol",sng_cnv_rcd);
      }else{
	fl_nm_nbr_max=0;
      } /* end if */

      if(abb_arg_nbr > 4){
	fl_nm_nbr_min=(int)strtol(fl_lst_abb[4],&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
	if(*sng_cnv_rcd) nco_sng_cnv_err(fl_lst_abb[4],"strtol",sng_cnv_rcd);
      }else{
	fl_nm_nbr_min=1;
      } /* end if */

      if(abb_arg_nbr > 5){
	if(!strcmp(fl_lst_abb[5],"yyyymm")) flg_yyyymm=True;
      } /* end if */

      /* Is there a .nc, .h5, .cdf, .hdf, .hd5, or .he5 suffix? */
      if(!strncmp(fl_lst_in[0]+strlen(fl_lst_in[0])-3,".nc",3))
	fl_nm_sfx_lng=3;
      if(!strncmp(fl_lst_in[0]+strlen(fl_lst_in[0])-3,".h5",3))
	fl_nm_sfx_lng=3;
      else if(!strncmp(fl_lst_in[0]+strlen(fl_lst_in[0])-4,".cdf",4))
	fl_nm_sfx_lng=4;
      else if(!strncmp(fl_lst_in[0]+strlen(fl_lst_in[0])-4,".hdf",4))
	fl_nm_sfx_lng=4;
      else if(!strncmp(fl_lst_in[0]+strlen(fl_lst_in[0])-4,".hd5",4))
	fl_nm_sfx_lng=4;
      else if(!strncmp(fl_lst_in[0]+strlen(fl_lst_in[0])-4,".he5",4))
	fl_nm_sfx_lng=4;

      /* Initialize static information useful for future invocations */
      fl_nm_1st_dgt=fl_lst_in[0]+strlen(fl_lst_in[0])-fl_nm_nbr_dgt-fl_nm_sfx_lng;
      fl_nm_nbr_sng=(char *)nco_malloc((size_t)(fl_nm_nbr_dgt+1UL)*sizeof(char));
      fl_nm_nbr_sng=strncpy(fl_nm_nbr_sng,fl_nm_1st_dgt,(size_t)fl_nm_nbr_dgt);
      fl_nm_nbr_sng[fl_nm_nbr_dgt]='\0';
      fl_nm_nbr_crr=(int)strtol(fl_nm_nbr_sng,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
      if(*sng_cnv_rcd) nco_sng_cnv_err(fl_nm_nbr_sng,"strtol",sng_cnv_rcd);

      if(flg_yyyymm){
	yyyy_crr=fl_nm_nbr_crr/100;
	mm_crr=fl_nm_nbr_crr%12;
	mm_crr=fl_nm_nbr_crr-yyyy_crr*100;
	(void)sprintf(fl_nm_nbr_sng_fmt,"%%0%dd%%02d",fl_nm_nbr_dgt-2);
      }else{
	(void)sprintf(fl_nm_nbr_sng_fmt,"%%0%dd",fl_nm_nbr_dgt);
      } /* !flg_yyyymm */

      /* First filename is always specified on command line anyway... */
      fl_nm=(char *)strdup(fl_lst_in[0]);

      /* Set flag that this routine has already been invoked at least once */
      FIRST_INVOCATION=False;

    }else{ /* end if FIRST_INVOCATION */
      /* Create current filename from previous filename */
      fl_nm_nbr_crr+=fl_nm_nbr_ncr;
      if(fl_nm_nbr_max){
	if(flg_yyyymm){
	  /* String contains dates (months) in YYYYMM format so increment left-most four digits (year) by one when  
	     right-most two digits exceed fl_nm_nbr_max */
	  mm_crr+=fl_nm_nbr_ncr;
	  if(mm_crr > fl_nm_nbr_max){
	    mm_crr=fl_nm_nbr_min;
	    yyyy_crr++;
	  } /* !mm_crr */
	  (void)sprintf(fl_nm_nbr_sng,fl_nm_nbr_sng_fmt,yyyy_crr,mm_crr);
	}else{
	  if(fl_nm_nbr_crr > fl_nm_nbr_max) fl_nm_nbr_crr=fl_nm_nbr_min;
	  (void)sprintf(fl_nm_nbr_sng,fl_nm_nbr_sng_fmt,fl_nm_nbr_crr);
	} /* !flg_yyyymm */
      }else{ /* !fl_nm_nbr_max */
	(void)sprintf(fl_nm_nbr_sng,fl_nm_nbr_sng_fmt,fl_nm_nbr_crr);
      } /* !fl_nm_nbr_max */
      fl_nm=(char *)strdup(fl_lst_in[0]);
      (void)strncpy(fl_nm+(fl_nm_1st_dgt-fl_lst_in[0]),fl_nm_nbr_sng,(size_t)fl_nm_nbr_dgt);
      if(fl_idx == fl_nm_nbr_ttl-1) fl_nm_nbr_sng=(char *)nco_free(fl_nm_nbr_sng);
    } /* end if not FIRST_INVOCATION */
  }else{ /* end if abbreviation list */
    fl_nm=(char *)strdup(fl_lst_in[fl_idx]);
  } /* end if no abbreviation list */

  /* Prepend path prefix */
  if(fl_pth){
    char *fl_nm_stub;

    fl_nm_stub=fl_nm;
    /* Allocate enough room for joining slash '/' and terminating NUL */
    fl_nm=(char *)nco_malloc((strlen(fl_nm_stub)+strlen(fl_pth)+2)*sizeof(char));
    (void)strcpy(fl_nm,fl_pth);
#ifdef _MSC_VER 
    nco_bool is_url;
    char *sng;

    /* Remote access detection; this should be replaced with NC_testurl; DAP-URL = "http://" host [ ":" port ] [ abs-path ] */
    if(strlen(fl_pth) < 8UL) is_url=False; else{
      sng=(char *)nco_malloc(8);
      sng=strncpy(sng,fl_pth,8);
      sng[7]='\0';    
      if(!strcmp("http://",sng)) is_url=True; else is_url=False;
      sng=(char *)nco_free(sng);
    } /* end else */

    /* Windows uses backslash for path separator; escape that character */
    if(is_url) (void)strcat(fl_nm,"/"); else (void)strcat(fl_nm,"\\");
   
#else /* !_MSC_VER */
    (void)strcat(fl_nm,"/");
#endif /* !_MSC_VER */
    (void)strcat(fl_nm,fl_nm_stub);

    /* Free filestub space */
    fl_nm_stub=(char *)nco_free(fl_nm_stub);
  } /* end if */

  /* Return new filename */
  return(fl_nm);
} /* end nco_fl_nm_prs() */

int /* [rcd] Return code */
nco_fl_open /* [fnc] Open file using appropriate buffer size hints and verbosity */
(const char * const fl_nm, /* I [sng] Name of file to open */
 const int md_open, /* I [enm] Mode flag for nc_open() call */
 const size_t * const bfr_sz_hnt, /* I [B] Buffer size hint */
 int * const nc_id) /* O [id] File ID */
{
  /* Purpose: Open file using appropriate buffer size hints and verbosity
     ncks -O -D 3 --bfr_sz=8192 ~/nco/data/in.nc ~/foo.nc */
  const char fnc_nm[]="nco_fl_open()"; /* [sng] Function name */

  int rcd=NC_NOERR; /* [rcd] Return code */
  int fl_fmt_xtn_prv; /* I [enm] Previous extended file format */
  int fl_fmt_xtn_crr; /* I [enm] Current  extended file format */
  int mode; /* I [enm] Mode flag for nc_inq_format_extended() call */

  nco_bool flg_rqs_vrb_mpl; /* [flg] Sufficiently verbose implicit request */
  nco_bool flg_rqs_vrb_xpl; /* [flg] Sufficiently verbose explicit request */

  size_t bfr_sz_hnt_lcl; /* [B] Buffer size hint */

  static nco_bool FIRST_INFO=True;

  /* Initialize local buffer size hint with user-input value */
  bfr_sz_hnt_lcl= (bfr_sz_hnt) ? *bfr_sz_hnt : NC_SIZEHINT_DEFAULT; /* [B] Buffer size hint */

  /* Is request implicit and sufficiently verbose? */
  flg_rqs_vrb_mpl = ((bfr_sz_hnt == NULL || *bfr_sz_hnt == NC_SIZEHINT_DEFAULT) && nco_dbg_lvl_get() >= nco_dbg_var && FIRST_INFO) ? True : False;

  /* Is request explicit and sufficiently verbose? */
  flg_rqs_vrb_xpl = ((bfr_sz_hnt != NULL && *bfr_sz_hnt != NC_SIZEHINT_DEFAULT) && nco_dbg_lvl_get() >= nco_dbg_fl && FIRST_INFO) ? True : False;

  /* Print implicit or explicit buffer request depending on debugging level */
  if(flg_rqs_vrb_mpl) (void)fprintf(stderr,"%s: INFO %s reports nc__open() will request file buffer of default size\n",nco_prg_nm_get(),fnc_nm); 
  if(flg_rqs_vrb_xpl) (void)fprintf(stderr,"%s: INFO %s reports nc__open() will request file buffer size = %lu bytes\n",nco_prg_nm_get(),fnc_nm,(unsigned long)*bfr_sz_hnt); 

  /* Pass local copy of size hint otherwise user-specified value is overwritten on first call */
#ifdef ENABLE_MPI
  rcd=nco_open_par(fl_nm,md_open|NC_MPIIO,MPI_COMM_WORLD,MPI_INFO_NULL,nc_id);
#else /* !ENABLE_MPI */
  rcd=nco__open(fl_nm,md_open,&bfr_sz_hnt_lcl,nc_id);
#endif /* !ENABLE_MPI */

  /* Print results using same verbosity criteria
     NB: bfr_sz_hnt_lcl is never NULL because nco__open() always returns a valid size */
  if(flg_rqs_vrb_mpl || flg_rqs_vrb_xpl) (void)fprintf(stderr,"%s: INFO %s reports nc__open() opened file with buffer size = %lu bytes\n",nco_prg_nm_get(),fnc_nm,(unsigned long)bfr_sz_hnt_lcl);

  /* 20131222: Update underlying file-type using new nc_inq_format_extended() function */
  fl_fmt_xtn_prv=nco_fmt_xtn_get();
  rcd+=nco_inq_format_extended(*nc_id,&fl_fmt_xtn_crr,&mode);
  if(fl_fmt_xtn_prv != nco_fmt_xtn_nil){
    /* Complain if set value of extended type does not match current type */
    if(nco_dbg_lvl_get() >= nco_dbg_fl && fl_fmt_xtn_prv != fl_fmt_xtn_crr && FIRST_INFO) (void)fprintf(stderr,"%s: INFO %s reports current extended filetype = %s does not equal previous extended filetype = %s. This is expected when NCO is instructed to convert filetypes, i.e., to read from one type and write to another. And when NCO generates grids or templates of a type different from the input file type. It is also expected when multi-file operators receive files known to be of different types. However, it could also indicate an unexpected change in input dataset type of which the user should be cognizant.\n",nco_prg_nm_get(),fnc_nm,nco_fmt_xtn_sng(fl_fmt_xtn_crr),nco_fmt_xtn_sng(fl_fmt_xtn_prv));
  }else{
    /* Set undefined extended file type to actual extended filetype */
    nco_fmt_xtn_set(fl_fmt_xtn_crr);
  } /* endif */
  if(nco_dbg_lvl_get() >= nco_dbg_scl && FIRST_INFO) (void)fprintf(stderr,"%s: INFO %s reports extended filetype of %s is %s, mode = %o (oct) = %d (dec) = %04x (hex) \n",nco_prg_nm_get(),fnc_nm,fl_nm,nco_fmt_xtn_sng(fl_fmt_xtn_crr),mode,(unsigned)mode,(unsigned)mode);

  if(FIRST_INFO && nco_dbg_lvl_get() >= nco_dbg_fl){
    (void)fprintf(stderr,"%s: INFO %s will not print any more INFO messages if this file is opened again. (Many NCO operators open the same file multiple times when OpenMP is enabled, %s prints INFO messages only the first time because successive messages are usually redundant).\n",nco_prg_nm_get(),fnc_nm,fnc_nm);
    FIRST_INFO=False;
  } /* !FIRST_INFO */
    
  return rcd;
} /* end nco_fl_open() */

size_t /* [B] Blocksize */
nco_fl_blocksize /* [fnc] Find blocksize of filesystem will or does contain this file */
(const char * const fl_out) /* [sng] Filename */
{
  /* Purpose: Find blocksize of filesystem will or does contain this file */
  const char fnc_nm[]="nco_fl_blocksize()"; /* [sng] Function name */
  const char sls_chr='/';   /* [chr] Slash character */
  
  char *drc_out; /* [sng] Directory containing output file */
  char *sls_ptr; /* [sng] Pointer to slash */
  
  int rcd_stt=0; /* [rcd] Return code from stat() */
  
  size_t fl_sys_blk_sz=0UL; /* [nbr] File system blocksize for I/O */

  struct stat stat_sct;

  drc_out=(char *)strdup(fl_out);
  
  /* Find last occurence of '/' */
  sls_ptr=strrchr(drc_out,sls_chr);
  
  if(sls_ptr){
    /* Filename includes path component(s)
       NUL-terminate file name at last slash */
    *sls_ptr='\0';
  }else{
    /* Filename is relative to local directory
       Replace filename by local directory specification, i.e., by UNIX "."  */
    drc_out[0]='.'; 
    drc_out[1]='\0'; 
  } /* endif */

  /* Blocksize information in stat structure:
     blksize_t st_blksize blocksize for file system I/O
     20140105: Although blksize_t defined in stat(), there is actually no Linux type named blksize_t 
     Use size_t instead */
  rcd_stt=stat(drc_out,&stat_sct);
  if(rcd_stt == -1){
    (void)fprintf(stdout,"%s: ERROR %s reports output file directory %s does not exist, unable to stat()\n",nco_prg_nm_get(),fnc_nm,drc_out);
    nco_exit(EXIT_FAILURE);
  } /* end if */
#ifndef _MSC_VER
  fl_sys_blk_sz=(size_t)stat_sct.st_blksize;
#endif /* _MSC_VER */
  if(nco_dbg_lvl_get() >= nco_dbg_scl) (void)fprintf(stderr,"%s: INFO %s reports preferred output filesystem I/O block size: %ld bytes\n",nco_prg_nm_get(),fnc_nm,(long)fl_sys_blk_sz);
  
  if(drc_out) drc_out=(char *)nco_free(drc_out);

  return fl_sys_blk_sz;
} /* end nco_fl_blocksize() */

char * /* O [sng] Name of temporary file actually opened */
nco_fl_out_open /* [fnc] Open output file subject to availability and user input */
(const char * const fl_out, /* I [sng] Name of file to open */
 nco_bool * const FORCE_APPEND, /* I/O [flg] Append to existing file, if any */
 const nco_bool FORCE_OVERWRITE, /* I [flg] Overwrite existing file, if any */
 const int fl_out_fmt, /* I [enm] Output file format */
 const size_t * const bfr_sz_hnt, /* I [B] Buffer size hint */
 const nco_bool RAM_CREATE, /* I [flg] Create file in RAM */
 const nco_bool RAM_OPEN, /* I [flg] Open (netCDF3) file(s) in RAM */
 const nco_bool SHARE_CREATE, /* I [flg] Create (netCDF3-only) file(s) with unbuffered I/O */
 const nco_bool SHARE_OPEN, /* I [flg] Open (netCDF3-only) file(s) with unbuffered I/O */
 const int WRT_TMP_FL, /* I [flg] Write output to temporary file */
 int * const out_id) /* O [id] File ID */
{
  /* Purpose: Open output file subject to availability and user input
     In accord with conservative NCO/netCDF philosophy, normally open temporary file named according 
     to fl_out and process ID so that errors cannot infect intended output file.
     Calling routine has responsibility to close and free fl_out_tmp */

  char *fl_out_tmp;
  char *pid_sng; /* String containing decimal representation of PID */

  const char fnc_nm[]="nco_fl_out_open()"; /* [sng] Function name */
  const char tmp_sng_1[]="pid"; /* Extra string appended to temporary filenames */
  const char tmp_sng_2[]="tmp"; /* Extra string appended to temporary filenames */

  int md_create; /* [enm] Mode flag for nco_create() call */
  int rcd=NC_NOERR; /* [rcd] Return code */
  int rcd_stt; /* [rcd] Return code */

  long fl_out_tmp_lng; /* [nbr] Length of temporary file name */
  long pid_sng_lng; /* [nbr] Theoretical length of decimal representation of this PID */
  long pid_sng_lng_max; /* [nbr] Maximum length of decimal representation of any PID */

  pid_t pid; /* Process ID */

  size_t bfr_sz_hnt_lcl; /* [B] Buffer size hint */
  
  struct stat stat_sct;

  /* Make sure output is possible */
#ifndef ENABLE_NETCDF4
  if(fl_out_fmt == NC_FORMAT_NETCDF4 || fl_out_fmt == NC_FORMAT_NETCDF4_CLASSIC){
    (void)fprintf(stdout,"%s: ERROR Requested netCDF4-format output file but NCO was built without netCDF4 support\n",nco_prg_nm_get());
    (void)fprintf(stdout,"%s: HINT: Obtain or build a netCDF4-enabled version of NCO.  Try, e.g., ./configure --enable-netcdf4 ...;make;make install\n",nco_prg_nm_get());
    nco_exit(EXIT_FAILURE);
  } /* netCDF4 */
#endif /* ENABLE_NETCDF4 */

  if(!fl_out){
    (void)fprintf(stdout,"%s: ERROR %s received empty filename to open\n",nco_prg_nm_get(),fnc_nm);
    nco_exit(EXIT_FAILURE);
  } /* fl_out */

  /* Set default clobber mode then modify for specified file format */
  md_create=NC_CLOBBER; /* [enm] Mode flag for nco_create() call */
  /* [fnc] Merge clobber mode with user-specified file format */
  md_create=nco_create_mode_mrg(md_create,fl_out_fmt);
  if(RAM_CREATE) md_create|=NC_DISKLESS|NC_WRITE;
  if(SHARE_CREATE) md_create|=NC_SHARE;

  if(FORCE_OVERWRITE && *FORCE_APPEND){
    (void)fprintf(stdout,"%s: ERROR FORCE_OVERWRITE and FORCE_APPEND are both set\n",nco_prg_nm_get());
    (void)fprintf(stdout,"%s: HINT: Overwrite (-O) and Append (-A) options are mutually exclusive. Re-run your command, setting at most one of these switches.\n",nco_prg_nm_get());
    nco_exit(EXIT_FAILURE);
  } /* end if */

  /* Generate unique temporary file name
     System routines tempnam(), tmpname(), mktemp() perform a similar function, but are OS dependent
     Maximum length of PID depends on pid_t
     Until about 1995 most OSs set pid_t = short = 16 or 32 bits
     Now some OSs have /usr/include/sys/types.h set pid_t = long = 32 or 64 bits
     20000126: Use sizeof(pid_t) rather than hardcoded size to fix longstanding bug on SGIs */

  /* Maximum length of decimal representation of PID is number of bits in PID times log10(2) */
  pid_sng_lng_max=(long)ceil(8*sizeof(pid_t)*log10(2.0));
  pid_sng=(char *)nco_malloc((pid_sng_lng_max+1UL)*sizeof(char));
  pid=getpid();
  (void)sprintf(pid_sng,"%ld",(long)pid);
  /* Theoretical length of decimal representation of PID is 1+ceil(log10(PID)) where the 1 is required iff PID is exact power of 10 */
  pid_sng_lng=1L+(long)ceil(log10((double)pid));
  /* NCO temporary file name is user-specified file name + "." + tmp_sng_1 + PID + "." + nco_prg_nm + "." + tmp_sng_2 + NUL */
  fl_out_tmp_lng=strlen(fl_out)+1UL+strlen(tmp_sng_1)+strlen(pid_sng)+1UL+strlen(nco_prg_nm_get())+1UL+strlen(tmp_sng_2)+1UL;
  /* NB: Calling routine has responsibility to free() this memory */
  fl_out_tmp=(char *)nco_malloc(fl_out_tmp_lng*sizeof(char));
  (void)sprintf(fl_out_tmp,"%s.%s%s.%s.%s",fl_out,tmp_sng_1,pid_sng,nco_prg_nm_get(),tmp_sng_2);
  if(nco_dbg_lvl_get() >= nco_dbg_sbr) (void)fprintf(stdout,"%s: %s reports sizeof(pid_t) = %d bytes, pid = %ld, pid_sng_lng = %ld bytes, strlen(pid_sng) = %ld bytes, fl_out_tmp_lng = %ld bytes, strlen(fl_out_tmp) = %ld, fl_out_tmp = %s\n",nco_prg_nm_get(),fnc_nm,(int)sizeof(pid_t),(long)pid,pid_sng_lng,(long)strlen(pid_sng),fl_out_tmp_lng,(long)strlen(fl_out_tmp),fl_out_tmp);

  /* Free temporary memory */
  pid_sng=(char *)nco_free(pid_sng);

#ifndef _MSC_VER
  if(nco_dbg_lvl_get() == nco_dbg_vec){
  /* Use built-in system routines to generate temporary filename
     This allows file to be built in fast directory like /tmp rather than local
     directory which could be a slow, NFS-mounted directories like /fs/cgd

     There are many options:
     tmpnam() uses P_tmpdir, does not allow specfication of drc
     tempnam(const char *drc, const char *pfx) uses writable $TMPDIR, else drc, else P_tmpdir, else /tmp and prefixes returned name with up to five characters from pfx, if supplied
     mkstemp(char *tpl) generates a filename and creates file in mode 0600

     Many sysadmins do not make /tmp large enough for huge temporary data files
     tempnam(), however, allows $TMPDIR or drc to be set to override /tmp
     20001010 Tried tempnam() 20001010 but GCC 2.96 warns: "the use of `tempnam' is dangerous, better use `mkstemp'" */
    int fl_out_hnd; /* Temporary file */
    char *fl_out_tmp_sys; /* System-generated unique temporary filename */
    fl_out_tmp_sys=(char *)nco_malloc((strlen(fl_out)+7)*sizeof(char));
    fl_out_tmp_sys[0]='\0'; /* NUL-terminate */
    fl_out_tmp_sys=strcat(fl_out_tmp_sys,fl_out);
    fl_out_tmp_sys=strcat(fl_out_tmp_sys,"XXXXXX");
#ifdef HAVE_MKSTEMP
    fl_out_hnd=mkstemp(fl_out_tmp_sys);
#else /* !HAVE_MKSTEMP */
    /* 20020812: Cray OS does not support mkstemp() */
    fl_out_hnd=creat(mktemp(fl_out_tmp_sys),0600);
#endif /* !HAVE_MKSTEMP */
    fl_out_hnd=fl_out_hnd+0; /* CEWI Removes compiler warning on SGI */
    if(nco_dbg_lvl_get() >= nco_dbg_scl) (void)fprintf(stdout,"%s: %s reports strlen(fl_out_tmp_sys) = %ld, fl_out_tmp_sys = %s, \n",nco_prg_nm_get(),fnc_nm,(long)strlen(fl_out_tmp_sys),fl_out_tmp_sys);
    fl_out_tmp_sys=(char *)nco_free(fl_out_tmp_sys);
  } /* endif dbg */
#endif /* _MSC_VER */ 

  if(WRT_TMP_FL){
    /* If temporary file already exists, prompt user to remove temporary files and exit */
    rcd_stt=stat(fl_out_tmp,&stat_sct);
    if(rcd_stt != -1){
      (void)fprintf(stdout,"%s: ERROR temporary file %s already exists, remove and try again\n",nco_prg_nm_get(),fl_out_tmp);
      nco_exit(EXIT_FAILURE);
    } /* end if */
  }else{ /* !WRT_TMP_FL */
    /* Name "temporary output file" same as final output file et voilà, no temporary file! */
    (void)strcpy(fl_out_tmp,fl_out);
  } /* !WRT_TMP_FL */

  /* Initialize local buffer size hint with user-input value */
  bfr_sz_hnt_lcl= (bfr_sz_hnt) ? *bfr_sz_hnt : NC_SIZEHINT_DEFAULT; /* [B] Buffer size hint */

  if(FORCE_OVERWRITE){
#ifdef ENABLE_MPI
    rcd+=nco_create_par(fl_out_tmp,md_create,MPI_COMM_WORLD,MPI_INFO_NULL,out_id);
#else /* !ENABLE_MPI */
    rcd+=nco__create(fl_out_tmp,md_create,NC_SIZEHINT_DEFAULT,&bfr_sz_hnt_lcl,out_id);
#endif /* !ENABLE_MPI */
    return fl_out_tmp;
  } /* end if */

  /* Following code block could be potentially be used by ncrename and ncatted
     Doing so would align file I/O for these operators with rest of NCO
     However, this would also require abandoning their "special treatment"
     which both requires them to work on local (not remote or DAP) files, 
     and prevents them from creating intermediate files.
     Changing towards greater NCO-wide consistency would be a good thing? 
     In ~20 years since release, though, only one complaint (from Chris Lynnes)
     that ncatted/ncrename have this locality requirement */
  if(False){
    if(nco_prg_id_get() == ncrename || nco_prg_id_get() == ncatted){
      /* ncrename and ncatted allow single filename without question */
      /* Incur expense of copying current file to temporary file */
      int md_open; /* [enm] Mode flag for nc_open() call */
      (void)nco_fl_cp(fl_out,fl_out_tmp);
      if(RAM_OPEN) md_open=NC_WRITE|NC_DISKLESS; else md_open=NC_WRITE;
      if(SHARE_OPEN) md_open=md_open|NC_SHARE;
      rcd+=nco_fl_open(fl_out_tmp,md_open,&bfr_sz_hnt_lcl,out_id);
      (void)nco_redef(*out_id);
      return fl_out_tmp;
    } /* end if */
  } /* end if false */

  /* If permanent output file already exists, query user whether to overwrite, append, or exit */
  rcd_stt=stat(fl_out,&stat_sct);
  if(rcd_stt != -1){
    char *rcd_fgets=NULL; /* Return code from fgets */
    char usr_rpl[NCO_USR_RPL_MAX_LNG];
    int md_open; /* [enm] Mode flag for nc_open() call */
    int usr_rpl_int;
    short nbr_itr=0;
    size_t usr_rpl_lng;
    
    /* Initialize user reply string */
    usr_rpl[0]='z';
    usr_rpl[1]='\0';

    if(RAM_OPEN) md_open=NC_WRITE|NC_DISKLESS; else md_open=NC_WRITE;
    if(SHARE_OPEN) md_open=md_open|NC_SHARE;

    if(*FORCE_APPEND){
      /* Incur expense of copying current file to temporary file
	 This is a no-op when files are identical */
      (void)nco_fl_cp(fl_out,fl_out_tmp);
      rcd+=nco_fl_open(fl_out_tmp,md_open,&bfr_sz_hnt_lcl,out_id);
      (void)nco_redef(*out_id);
      return fl_out_tmp;
    } /* end if */

    /* Ensure one exit condition for each valid switch in following case statement */
    while(strcasecmp(usr_rpl,"o") && strcasecmp(usr_rpl,"a") && strcasecmp(usr_rpl,"e")){
      /* fxm: i18n necessary here */
      /* int cnv_nbr; *//* [nbr] Number of scanf conversions performed this scan */
      if(nbr_itr++ > NCO_MAX_NBR_USR_INPUT_RETRY){
	(void)fprintf(stdout,"\n%s: ERROR %d failed attempts to obtain valid interactive input. Assuming non-interactive shell and exiting.\n",nco_prg_nm_get(),nbr_itr-1);
	nco_exit(EXIT_FAILURE);
      } /* end if */
      if(nbr_itr > 1) (void)fprintf(stdout,"%s: ERROR Invalid response.\n",nco_prg_nm_get());
      (void)fprintf(stdout,"%s: %s exists---`e'xit, `o'verwrite (i.e., clobber existing file), or `a'ppend (i.e., replace duplicate variables in, and add metadata and new variables to, existing file) (e/o/a)? ",nco_prg_nm_get(),fl_out);
      (void)fflush(stdout);
      /*       fgets() reads (at most one less than NCO_USR_RPL_MAX_LNG) to first newline or EOF */
      rcd_fgets=fgets(usr_rpl,NCO_USR_RPL_MAX_LNG,stdin);
      /*       fscanf() reads ... */
      /*      while((cnv_nbr=fscanf(stdin,"%9s",usr_rpl)) != EOF) continue;*/
      /*      while((rcd_fgets=fgets(usr_rpl,NCO_USR_RPL_MAX_LNG,stdin)) == NULL){*/
	/*	if(nco_dbg_lvl_get() >= nco_dbg_scl) (void)fprintf(stderr,"%s: DEBUG Read \"%s\" while waiting for non-NULL on stdin...\n",nco_prg_nm_get(),(rcd_fgets == NULL) ? "NULL" : usr_rpl);*/
      /*      continue;}*/

      /* Ensure last character in input string is \n and replace that with \0 */
      usr_rpl_lng=strlen(usr_rpl);
      if(usr_rpl_lng >= 1)
	if(usr_rpl[usr_rpl_lng-1] == '\n')
	    usr_rpl[usr_rpl_lng-1]='\0';

      if(nco_dbg_lvl_get() == nco_dbg_scl) (void)fprintf(stdout,"%s: INFO %s reports that fgets() read \"%s\" (after removing trailing newline) from stdin\n",nco_prg_nm_get(),fnc_nm,(rcd_fgets == NULL) ? "NULL" : usr_rpl);
    } /* end while user reply is not yet "o", "a", or "e" */

    /* Ensure one case statement for each exit condition in preceding while loop */
    usr_rpl_int=(int)usr_rpl[0];
    switch(usr_rpl_int){
    case 'E':
    case 'e':
      nco_exit(EXIT_SUCCESS);
      break;
    case 'O':
    case 'o':
#ifdef ENABLE_MPI
    rcd+=nco_create_par(fl_out_tmp,md_create,MPI_COMM_WORLD,MPI_INFO_NULL,out_id);
#else /* !ENABLE_MPI */
    rcd+=nco__create(fl_out_tmp,md_create,NC_SIZEHINT_DEFAULT,&bfr_sz_hnt_lcl,out_id);
#endif /* !ENABLE_MPI */
       break;
    case 'A':
    case 'a':
      /* Incur expense of copying current file to temporary file */
      (void)nco_fl_cp(fl_out,fl_out_tmp);
      rcd+=nco_fl_open(fl_out_tmp,md_open,&bfr_sz_hnt_lcl,out_id);
      (void)nco_redef(*out_id);
      *FORCE_APPEND=True;
      break;
    default: nco_dfl_case_nc_type_err(); break;
    } /* end switch */

  }else{ /* Output file does not already exist */
    md_create=NC_NOCLOBBER;
    md_create=nco_create_mode_mrg(md_create,fl_out_fmt);
    if(RAM_CREATE) md_create|=NC_DISKLESS|NC_WRITE;
    if(SHARE_CREATE) md_create|=NC_SHARE;
#ifdef ENABLE_MPI
    rcd+=nco_create_par(fl_out_tmp,md_create,MPI_COMM_WORLD,MPI_INFO_NULL,out_id);
#else /* !ENABLE_MPI */
    rcd+=nco__create(fl_out_tmp,md_create,NC_SIZEHINT_DEFAULT,&bfr_sz_hnt_lcl,out_id);
#endif /* !ENABLE_MPI */
  } /* end if output file does not already exist */

  if(rcd != NC_NOERR) nco_err_exit(rcd,fnc_nm);

  return fl_out_tmp;

} /* end nco_fl_out_open() */

void
nco_fl_out_cls /* [fnc] Close temporary output file, move it to permanent output file */
(const char * const fl_out, /* I [sng] Name of permanent output file */
 const char * const fl_out_tmp, /* I [sng] Name of temporary output file to close and move to permanent output file */
 const int nc_id) /* I [id] File ID of fl_out_tmp */
{
  /* Purpose: Close temporary output file, move it to permanent output file */
  int rcd; /* [rcd] Return code */

  rcd=nco_close(nc_id);
  if(rcd != NC_NOERR){
    (void)fprintf(stdout,"%s: ERROR nco_fl_out_cls() is unable to nco_close() file %s\n",nco_prg_nm_get(),fl_out_tmp);
    nco_exit(EXIT_FAILURE);
  } /* end if */

  /* Only bother to perform system() call if files are not identical */
  if(!strcmp(fl_out_tmp,fl_out)){
    if(nco_dbg_lvl_get() >= nco_dbg_fl) (void)fprintf(stderr,"%s: INFO Temporary and final files %s are identical---no need to move.\n",nco_prg_nm_get(),fl_out);
    return;
  }else{
    (void)nco_fl_mv(fl_out_tmp,fl_out);
  } /* end if */

} /* end nco_fl_out_cls() */

void
nco_fl_rm /* [fnc] Remove file */
(char *fl_nm) /* I [sng] File to be removed */
{
  /* Purpose: Remove specified file from local system */
  int rcd;
  char *rm_cmd;
#ifdef _MSC_VER
  const char rm_cmd_sys_dep[]="del /F";
#else /* !_MSC_VER */
  const char rm_cmd_sys_dep[]="rm -f";
#endif /* !_MSC_VER */

  /* Remember to add one for the space and one for the terminating NUL character */
  rm_cmd=(char *)nco_malloc((strlen(rm_cmd_sys_dep)+1UL+strlen(fl_nm)+1UL)*sizeof(char));
  (void)sprintf(rm_cmd,"%s %s",rm_cmd_sys_dep,fl_nm);

  if(nco_dbg_lvl_get() >= nco_dbg_fl) (void)fprintf(stderr,"%s: DEBUG Removing %s with %s\n",nco_prg_nm_get(),fl_nm,rm_cmd);
  rcd=system(rm_cmd);
  if(rcd == -1) (void)fprintf(stderr,"%s: WARNING unable to remove %s, continuing anyway...\n",nco_prg_nm_get(),fl_nm);

  rm_cmd=(char *)nco_free(rm_cmd);
} /* end nco_fl_rm() */

