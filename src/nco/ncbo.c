/* $Header: /data/zender/nco_20150216/nco/src/nco/ncbo.c,v 1.172 2011-07-25 03:38:41 zender Exp $ */

/* ncbo -- netCDF binary operator */

/* Purpose: Compute sum, difference, product, or ratio of specified hyperslabs of specfied variables
   from two input netCDF files and output them to a single file. */

/* Copyright (C) 1995--2011 Charlie Zender
   
   License: GNU General Public License (GPL) Version 3
   The full license text is at http://www.gnu.org/copyleft/gpl.html 
   and in the file nco/doc/LICENSE in the NCO source distribution.

   As a special exception to the terms of the GPL, you are permitted 
   to link the NCO source code with the HDF, netCDF, OPeNDAP, and UDUnits
   libraries and to distribute the resulting executables under the terms 
   of the GPL, but in addition obeying the extra stipulations of the 
   HDF, netCDF, OPeNDAP, and UDUnits licenses.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
   See the GNU General Public License for more details.
   
   The original author of this software, Charlie Zender, seeks to improve
   it with your suggestions, contributions, bug-reports, and patches.
   Please contact the NCO project at http://nco.sf.net or write to
   Charlie Zender
   Department of Earth System Science
   University of California, Irvine
   Irvine, CA 92697-3100 */

/* Usage:
   ncbo -O -p ~/nco/data in.nc in.nc ~/foo.nc
   ncbo -O -v mss_val -p ~/nco/data in.nc in.nc ~/foo.nc
   ncbo -p /data/zender/tmp h0001.nc ~/foo.nc
   ncbo -p /data/zender/tmp -l /data/zender/tmp/rmt h0001.nc h0002.nc ~/foo.nc
   ncbo -p /ZENDER/tmp -l /data/zender/tmp/rmt h0001.nc h0002.nc ~/foo.nc
   ncbo -p /ZENDER/tmp -l /usr/tmp/zender h0001.nc h0002.nc ~/foo.nc
   
   Test type conversion:
   ncks -O -C -v float_var in.nc foo1.nc
   ncrename -v float_var,double_var foo1.nc
   ncks -O -C -v double_var in.nc foo2.nc
   ncbo -O -C -v double_var foo1.nc foo2.nc foo3.nc
   ncbo -O -C -v double_var foo2.nc foo1.nc foo4.nc
   ncks -H -m foo1.nc
   ncks -H -m foo2.nc
   ncks -H -m foo3.nc
   ncks -H -m foo4.nc
   
   Test nco_var_cnf_dmn:
   ncks -O -v scalar_var in.nc ~/foo.nc ; ncrename -v scalar_var,four_dmn_rec_var foo.nc ; ncbo -O -v four_dmn_rec_var in.nc ~/foo.nc foo2.nc */
#ifdef HAVE_CONFIG_H
# include <config.h> /* Autotools tokens */
#endif /* !HAVE_CONFIG_H */

/* Standard C headers */
#include <math.h> /* sin cos cos sin 3.14159 */
#include <stdio.h> /* stderr, FILE, NULL, etc. */
#include <stdlib.h> /* atof, atoi, malloc, getopt */
#include <string.h> /* strcmp. . . */
#include <sys/stat.h> /* stat() */
#include <time.h> /* machine time */
#include <unistd.h> /* all sorts of POSIX stuff */
#ifndef HAVE_GETOPT_LONG
# include "nco_getopt.h"
#else /* HAVE_GETOPT_LONG */ 
# ifdef HAVE_GETOPT_H
#  include <getopt.h>
# endif /* !HAVE_GETOPT_H */ 
#endif /* HAVE_GETOPT_LONG */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions and C library */
#ifdef ENABLE_MPI
# include <mpi.h> /* MPI definitions */
# include "nco_mpi.h" /* MPI utilities */
#endif /* !ENABLE_MPI */

/* Personal headers */
/* #define MAIN_PROGRAM_FILE MUST precede #include libnco.h */
#define MAIN_PROGRAM_FILE
#include "libnco.h" /* netCDF Operator (NCO) library */

int 
main(int argc,char **argv)
{
  nco_bool CNV_CCM_CCSM_CF;
  nco_bool EXCLUDE_INPUT_LIST=False; /* Option c */
  nco_bool EXTRACT_ALL_COORDINATES=False; /* Option c */
  nco_bool EXTRACT_ASSOCIATED_COORDINATES=True; /* Option C */
  nco_bool FILE_1_RETRIEVED_FROM_REMOTE_LOCATION;
  nco_bool FILE_2_RETRIEVED_FROM_REMOTE_LOCATION;
  nco_bool FL_LST_IN_FROM_STDIN=False; /* [flg] fl_lst_in comes from stdin */
  nco_bool FORCE_APPEND=False; /* Option A */
  nco_bool FORCE_OVERWRITE=False; /* Option O */
  nco_bool FORTRAN_IDX_CNV=False; /* Option F */
  nco_bool HISTORY_APPEND=True; /* Option h */
  nco_bool MSA_USR_RDR=False; /* [flg] Multi-slabbing algorithm leaves hyperslabs in */
  nco_bool REMOVE_REMOTE_FILES_AFTER_PROCESSING=True; /* Option R */
  nco_bool flg_cln=False; /* [flg] Clean memory prior to exit */
  nco_bool flg_ddra=False; /* [flg] DDRA diagnostics */
  
  char **fl_lst_abb=NULL; /* Option a */
  char **fl_lst_in;
  char **var_lst_in=NULL_CEWI;
  char *aux_arg[NC_MAX_DIMS];
  char *cmd_ln;
  char *cnk_arg[NC_MAX_DIMS];
  char *cnk_map_sng=NULL_CEWI; /* [sng] Chunking map */
  char *cnk_plc_sng=NULL_CEWI; /* [sng] Chunking policy */
  char *fl_in_1=NULL; /* fl_in_1 is nco_realloc'd when not NULL */
  char *fl_in_2=NULL; /* fl_in_2 is nco_realloc'd when not NULL */
  char *fl_out=NULL; /* Option o */
  char *fl_out_tmp;
  char *fl_pth=NULL; /* Option p */
  char *fl_pth_lcl=NULL; /* Option l */
  char *lmt_arg[NC_MAX_DIMS];
  char *nco_op_typ_sng=NULL; /* [sng] Operation type */
  char *opt_crr=NULL; /* [sng] String representation of current long-option name */
  char *optarg_lcl=NULL; /* [sng] Local copy of system optarg */
  
  char *sng_cnv_rcd=char_CEWI; /* [sng] strtol()/strtoul() return code */

  const char * const CVS_Id="$Id: ncbo.c,v 1.172 2011-07-25 03:38:41 zender Exp $"; 
  const char * const CVS_Revision="$Revision: 1.172 $";
  const char * const opt_sht_lst="346ACcD:d:FhL:l:Oo:p:rRt:v:X:xy:-:";
  
  cnk_sct **cnk=NULL_CEWI;

#if defined(__cplusplus) || defined(PGI_CC)
  ddra_info_sct ddra_info;
  ddra_info.flg_ddra=False;
#else /* !__cplusplus */
  ddra_info_sct ddra_info={.MRV_flg=False,.flg_ddra=False,.lmn_nbr=0LL,.lmn_nbr_avg=0LL,.lmn_nbr_wgt=0LL,.nco_op_typ=nco_op_nil,.rnk_avg=0,.rnk_var=0,.rnk_wgt=0,.tmr_flg=nco_tmr_srt,.var_idx=0,.wgt_brd_flg=False,.wrd_sz=0};
#endif /* !__cplusplus */

  dmn_sct **dim_1;
  dmn_sct **dim_2;
  dmn_sct **dmn_out;
  
  extern char *optarg;
  extern int optind;
  
  /* Using naked stdin/stdout/stderr in parallel region generates warning
     Copy appropriate filehandle to variable scoped shared in parallel clause */
  FILE * const fp_stderr=stderr; /* [fl] stderr filehandle CEWI */
  FILE * const fp_stdout=stdout; /* [fl] stdout filehandle CEWI */

  int *in_id_1_arr;
  int *in_id_2_arr;

  int abb_arg_nbr=0;
  int aux_nbr=0; /* [nbr] Number of auxiliary coordinate hyperslabs specified */
  int cnk_map=nco_cnk_map_nil; /* [enm] Chunking map */
  int cnk_nbr=0; /* [nbr] Number of chunk sizes */
  int cnk_plc=nco_cnk_plc_nil; /* [enm] Chunking policy */
  int dfl_lvl=0; /* [enm] Deflate level */
  int fl_idx;
  int fl_nbr=0;
  int fl_in_fmt_1; /* [enm] Input file format */
  int fl_in_fmt_2; /* [enm] Input file format */
  int fl_out_fmt=NCO_FORMAT_UNDEFINED; /* [enm] Output file format */
  int fll_md_old; /* [enm] Old fill mode */
  int idx;
  int jdx;
  int dmn_idx;
  int dmn_jdx;
  int in_id_1;  
  int in_id_2;  
  int lmt_nbr=0; /* Option d. NB: lmt_nbr gets incremented */
  int nbr_dmn_fl_1;
  int nbr_dmn_fl_2;
  int nbr_dmn_xtr_1;
  int nbr_dmn_xtr_2;
  int nbr_var_fix_1; /* nbr_var_fix_1 gets incremented */
  int nbr_var_fix_2; /* nbr_var_fix_2 gets incremented */
  int nbr_var_fl_1;
  int nbr_var_fl_2;
  int nbr_var_prc_1; /* nbr_var_prc_1 gets incremented */
  int nbr_var_prc_2; /* nbr_var_prc_2 gets incremented */
  int nbr_xtr_1=0; /* nbr_xtr_1 won't otherwise be set for -c with no -v */
  int nbr_xtr_2=0; /* nbr_xtr_2 won't otherwise be set for -c with no -v */
  int nco_op_typ=nco_op_nil; /* [enm] Operation type */
  int opt;
  int out_id;  
  int rcd=NC_NOERR; /* [rcd] Return code */
  int thr_idx; /* [idx] Index of current thread */
  int thr_nbr=int_CEWI; /* [nbr] Thread number Option t */
  int var_lst_in_nbr=0;
  
  lmt_sct **aux=NULL_CEWI; /* Auxiliary coordinate limits */
  lmt_sct **lmt=NULL_CEWI;
  lmt_all_sct **lmt_all_lst=NULL_CEWI; /* List of *lmt_all structures */
  
  nm_id_sct *dmn_lst_1;
  nm_id_sct *dmn_lst_2;
  nm_id_sct *xtr_lst_1=NULL; /* xtr_lst_1 may be alloc()'d from NULL with -c option */
  nm_id_sct *xtr_lst_2=NULL; /* xtr_lst_2 may be alloc()'d from NULL with -c option */
  
  size_t cnk_sz_scl=0UL; /* [nbr] Chunk size scalar */

  var_sct **var_1;
  var_sct **var_2;
  var_sct **var_fix_1;
  var_sct **var_fix_2;
  var_sct **var_fix_out;
  var_sct **var_out;
  var_sct **var_prc_1;
  var_sct **var_prc_2;
  var_sct **var_prc_out;
  
  static struct option opt_lng[]=
    { /* Structure ordered by short option key if possible */
      /* Long options with no argument, no short option counterpart */
      {"cln",no_argument,0,0}, /* [flg] Clean memory prior to exit */
      {"clean",no_argument,0,0}, /* [flg] Clean memory prior to exit */
      {"mmr_cln",no_argument,0,0}, /* [flg] Clean memory prior to exit */
      {"drt",no_argument,0,0}, /* [flg] Allow dirty memory on exit */
      {"dirty",no_argument,0,0}, /* [flg] Allow dirty memory on exit */
      {"mmr_drt",no_argument,0,0}, /* [flg] Allow dirty memory on exit */
      {"ddra",no_argument,0,0}, /* [flg] DDRA diagnostics */
      {"mdl_cmp",no_argument,0,0}, /* [flg] DDRA diagnostics */
      {"msa_usr_rdr",no_argument,0,0}, /* [flg] Multi-slabbing algorithm leaves hyperslabs in user order */	  
      {"version",no_argument,0,0},
      {"vrs",no_argument,0,0},
      /* Long options with argument, no short option counterpart */
      {"cnk_map",required_argument,0,0}, /* [nbr] Chunking map */
      {"chunk_map",required_argument,0,0}, /* [nbr] Chunking map */
      {"cnk_plc",required_argument,0,0}, /* [nbr] Chunking policy */
      {"chunk_policy",required_argument,0,0}, /* [nbr] Chunking policy */
      {"cnk_scl",required_argument,0,0}, /* [nbr] Chunk size scalar */
      {"chunk_scalar",required_argument,0,0}, /* [nbr] Chunk size scalar */
      {"cnk_dmn",required_argument,0,0}, /* [nbr] Chunk size */
      {"chunk_dimension",required_argument,0,0}, /* [nbr] Chunk size */
      {"file_format",required_argument,0,0},
      /* Long options with short counterparts */
      {"3",no_argument,0,'3'},
      {"4",no_argument,0,'4'},
      {"64bit",no_argument,0,'4'},
      {"netcdf4",no_argument,0,'4'},
      {"append",no_argument,0,'A'},
      {"coords",no_argument,0,'c'},
      {"crd",no_argument,0,'c'},
      {"no-coords",no_argument,0,'C'},
      {"no-crd",no_argument,0,'C'},
      {"debug",required_argument,0,'D'},
      {"dbg_lvl",required_argument,0,'D'},
      {"dimension",required_argument,0,'d'},
      {"dmn",required_argument,0,'d'},
      {"fortran",no_argument,0,'F'},
      {"ftn",no_argument,0,'F'},
      {"history",no_argument,0,'h'},
      {"hst",no_argument,0,'h'},
      {"dfl_lvl",required_argument,0,'L'}, /* [enm] Deflate level */
      {"deflate",required_argument,0,'L'}, /* [enm] Deflate level */
      {"local",required_argument,0,'l'},
      {"lcl",required_argument,0,'l'},
      {"overwrite",no_argument,0,'O'},
      {"ovr",no_argument,0,'O'},
      {"path",required_argument,0,'p'},
      {"retain",no_argument,0,'R'},
      {"rtn",no_argument,0,'R'},
      {"revision",no_argument,0,'r'},
      {"thr_nbr",required_argument,0,'t'},
      {"threads",required_argument,0,'t'},
      {"omp_num_threads",required_argument,0,'t'},
      {"variable",required_argument,0,'v'},
      {"auxiliary",required_argument,0,'X'},
      {"exclude",no_argument,0,'x'},
      {"xcl",no_argument,0,'x'},
      {"operation",required_argument,0,'y'},
      {"op_typ",required_argument,0,'y'},
      {"help",no_argument,0,'?'},
      {0,0,0,0}
    }; /* end opt_lng */
  int opt_idx=0; /* Index of current long option into opt_lng array */
  
  /* Start timer and save command line */ 
  ddra_info.tmr_flg=nco_tmr_srt;
  rcd+=nco_ddra((char *)NULL,(char *)NULL,&ddra_info);
  ddra_info.tmr_flg=nco_tmr_mtd;
  cmd_ln=nco_cmd_ln_sng(argc,argv);
  
  /* Get program name and set program enum (e.g., prg=ncra) */
  prg_nm=prg_prs(argv[0],&prg);
  
  /* Parse command line arguments */
  while(1){
    /* getopt_long_only() allows one dash to prefix long options */
    opt=getopt_long(argc,argv,opt_sht_lst,opt_lng,&opt_idx);
    /* NB: access to opt_crr is only valid when long_opt is detected */
    if(opt == EOF) break; /* Parse positional arguments once getopt_long() returns EOF */
    opt_crr=(char *)strdup(opt_lng[opt_idx].name);

    /* Process long options without short option counterparts */
    if(opt == 0){
      if(!strcmp(opt_crr,"cnk_dmn") || !strcmp(opt_crr,"chunk_dimension")){
	/* Copy limit argument for later processing */
	cnk_arg[cnk_nbr]=(char *)strdup(optarg);
	cnk_nbr++;
      } /* endif cnk */
      if(!strcmp(opt_crr,"cnk_scl") || !strcmp(opt_crr,"chunk_scalar")){
	cnk_sz_scl=strtoul(optarg,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
	if(*sng_cnv_rcd) nco_sng_cnv_err(optarg,"strtoul",sng_cnv_rcd);
      } /* endif cnk */
      if(!strcmp(opt_crr,"cnk_map") || !strcmp(opt_crr,"chunk_map")){
	/* Chunking map */
	cnk_map_sng=(char *)strdup(optarg);
	cnk_map=nco_cnk_map_get(cnk_map_sng);
      } /* endif cnk */
      if(!strcmp(opt_crr,"cnk_plc") || !strcmp(opt_crr,"chunk_policy")){
	/* Chunking policy */
	cnk_plc_sng=(char *)strdup(optarg);
	cnk_plc=nco_cnk_plc_get(cnk_plc_sng);
      } /* endif cnk */
      if(!strcmp(opt_crr,"cln") || !strcmp(opt_crr,"mmr_cln") || !strcmp(opt_crr,"clean")) flg_cln=True; /* [flg] Clean memory prior to exit */
      if(!strcmp(opt_crr,"drt") || !strcmp(opt_crr,"mmr_drt") || !strcmp(opt_crr,"dirty")) flg_cln=False; /* [flg] Clean memory prior to exit */
      if(!strcmp(opt_crr,"ddra") || !strcmp(opt_crr,"mdl_cmp")) ddra_info.flg_ddra=flg_ddra=True; /* [flg] DDRA diagnostics */
      if(!strcmp(opt_crr,"fl_fmt") || !strcmp(opt_crr,"file_format")) rcd=nco_create_mode_prs(optarg,&fl_out_fmt);
      if(!strcmp(opt_crr,"msa_usr_rdr")) MSA_USR_RDR=True; /* [flg] Multi-slabbing algorithm leaves hyperslabs in user order */
      if(!strcmp(opt_crr,"vrs") || !strcmp(opt_crr,"version")){
	(void)nco_vrs_prn(CVS_Id,CVS_Revision);
	nco_exit(EXIT_SUCCESS);
      } /* endif "vrs" */
    } /* opt != 0 */
    /* Process short options */
    switch(opt){
    case 0: /* Long options have already been processed, return */
      break;
    case '3': /* Request netCDF3 output storage format */
      fl_out_fmt=NC_FORMAT_CLASSIC;
      break;
    case '4': /* Catch-all to prescribe output storage format */
      if(!strcmp(opt_crr,"64bit")) fl_out_fmt=NC_FORMAT_64BIT; else fl_out_fmt=NC_FORMAT_NETCDF4; 
      break;
    case '6': /* Request netCDF3 64-bit offset output storage format */
      fl_out_fmt=NC_FORMAT_64BIT;
      break;
    case 'A': /* Toggle FORCE_APPEND */
      FORCE_APPEND=!FORCE_APPEND;
      break;
    case 'C': /* Extract all coordinates associated with extracted variables? */
      EXTRACT_ASSOCIATED_COORDINATES=False;
      break;
    case 'c':
      EXTRACT_ALL_COORDINATES=True;
      break;
    case 'D': /* The debugging level. Default is 0. */
      dbg_lvl=(unsigned short int)strtoul(optarg,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
      if(*sng_cnv_rcd) nco_sng_cnv_err(optarg,"strtoul",sng_cnv_rcd);
      break;
    case 'd': /* Copy limit argument for later processing */
      lmt_arg[lmt_nbr]=(char *)strdup(optarg);
      lmt_nbr++;
      break;
    case 'F': /* Toggle index convention. Default is 0-based arrays (C-style). */
      FORTRAN_IDX_CNV=!FORTRAN_IDX_CNV;
      break;
    case 'h': /* Toggle appending to history global attribute */
      HISTORY_APPEND=!HISTORY_APPEND;
      break;
    case 'L': /* [enm] Deflate level. Default is 0. */
      dfl_lvl=(int)strtol(optarg,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
      if(*sng_cnv_rcd) nco_sng_cnv_err(optarg,"strtol",sng_cnv_rcd);
      break;
    case 'l': /* Local path prefix for files retrieved from remote file system */
      fl_pth_lcl=(char *)strdup(optarg);
      break;
    case 'O': /* Toggle FORCE_OVERWRITE */
      FORCE_OVERWRITE=!FORCE_OVERWRITE;
      break;
    case 'o': /* Name of output file */
      fl_out=(char *)strdup(optarg);
      break;
    case 'p': /* Common file path */
      fl_pth=(char *)strdup(optarg);
      break;
    case 'R': /* Toggle removal of remotely-retrieved-files. Default is True. */
      REMOVE_REMOTE_FILES_AFTER_PROCESSING=!REMOVE_REMOTE_FILES_AFTER_PROCESSING;
      break;
    case 'r': /* Print CVS program information and copyright notice */
      (void)nco_vrs_prn(CVS_Id,CVS_Revision);
      (void)nco_lbr_vrs_prn();
      (void)nco_cpy_prn();
      (void)nco_cnf_prn();
      nco_exit(EXIT_SUCCESS);
      break;
    case 't': /* Thread number */
      thr_nbr=(int)strtol(optarg,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
      if(*sng_cnv_rcd) nco_sng_cnv_err(optarg,"strtol",sng_cnv_rcd);
      break;
    case 'v': /* Variables to extract/exclude */
      /* Replace commas with hashes when within braces (convert back later) */
      optarg_lcl=(char *)strdup(optarg);
      (void)nco_lst_comma2hash(optarg_lcl);
      var_lst_in=nco_lst_prs_2D(optarg_lcl,",",&var_lst_in_nbr);
      optarg_lcl=(char *)nco_free(optarg_lcl);
      nbr_xtr_1=nbr_xtr_2=var_lst_in_nbr;
      break;
    case 'X': /* Copy auxiliary coordinate argument for later processing */
      aux_arg[aux_nbr]=(char *)strdup(optarg);
      aux_nbr++;
      MSA_USR_RDR=True; /* [flg] Multi-slabbing algorithm leaves hyperslabs in user order */      
      break;
    case 'x': /* Exclude rather than extract variables specified with -v */
      EXCLUDE_INPUT_LIST=True;
      break;
    case 'y': /* User-specified operation type overrides invocation default */
      nco_op_typ_sng=(char *)strdup(optarg);
      nco_op_typ=nco_op_typ_get(nco_op_typ_sng);
      break;
    case '?': /* Print proper usage */
      (void)nco_usg_prn();
      nco_exit(EXIT_SUCCESS);
      break;
    case '-': /* Long options are not allowed */
      (void)fprintf(stderr,"%s: ERROR Long options are not available in this build. Use single letter options instead.\n",prg_nm_get());
      nco_exit(EXIT_FAILURE);
      break;
    default: /* Print proper usage */
      (void)nco_usg_prn();
      nco_exit(EXIT_FAILURE);
      break;
    } /* end switch */
    if(opt_crr) opt_crr=(char *)nco_free(opt_crr);
  } /* end while loop */
  
  /* Process positional arguments and fill in filenames */
  fl_lst_in=nco_fl_lst_mk(argv,argc,optind,&fl_nbr,&fl_out,&FL_LST_IN_FROM_STDIN);
  
  /* Make uniform list of user-specified chunksizes */
  if(cnk_nbr > 0) cnk=nco_cnk_prs(cnk_nbr,cnk_arg);

  /* Make uniform list of user-specified dimension limits */
  lmt=nco_lmt_prs(lmt_nbr,lmt_arg);
    
  /* Initialize thread information */
  thr_nbr=nco_openmp_ini(thr_nbr);
  in_id_1_arr=(int *)nco_malloc(thr_nbr*sizeof(int));
  in_id_2_arr=(int *)nco_malloc(thr_nbr*sizeof(int));

  /* Parse filenames */
  fl_idx=0; /* Input file _1 */
  fl_in_1=nco_fl_nm_prs(fl_in_1,fl_idx,&fl_nbr,fl_lst_in,abb_arg_nbr,fl_lst_abb,fl_pth);
  if(dbg_lvl >= nco_dbg_fl) (void)fprintf(stderr,"%s: INFO Input file %d is %s",prg_nm_get(),fl_idx,fl_in_1);
  /* Make sure file is on local system and is readable or die trying */
  fl_in_1=nco_fl_mk_lcl(fl_in_1,fl_pth_lcl,&FILE_1_RETRIEVED_FROM_REMOTE_LOCATION);
  if(dbg_lvl >= nco_dbg_fl && FILE_1_RETRIEVED_FROM_REMOTE_LOCATION) (void)fprintf(stderr,", local file is %s",fl_in_1);
  if(dbg_lvl >= nco_dbg_fl) (void)fprintf(stderr,"\n");
  /* Open file once per thread to improve caching */
  for(thr_idx=0;thr_idx<thr_nbr;thr_idx++) rcd=nco_open(fl_in_1,NC_NOWRITE,in_id_1_arr+thr_idx);
  in_id_1=in_id_1_arr[0];

  fl_idx=1; /* Input file _2 */
  fl_in_2=nco_fl_nm_prs(fl_in_2,fl_idx,&fl_nbr,fl_lst_in,abb_arg_nbr,fl_lst_abb,fl_pth);
  if(dbg_lvl >= nco_dbg_fl) (void)fprintf(stderr,"%s: INFO Input file %d is %s",prg_nm_get(),fl_idx,fl_in_2);
  /* Make sure file is on local system and is readable or die trying */
  fl_in_2=nco_fl_mk_lcl(fl_in_2,fl_pth_lcl,&FILE_2_RETRIEVED_FROM_REMOTE_LOCATION);
  if(dbg_lvl >= nco_dbg_fl && FILE_2_RETRIEVED_FROM_REMOTE_LOCATION) (void)fprintf(stderr,", local file is %s",fl_in_2);
  if(dbg_lvl >= nco_dbg_fl) (void)fprintf(stderr,"\n");
  /* Open file once per thread to improve caching */
  for(thr_idx=0;thr_idx<thr_nbr;thr_idx++) rcd=nco_open(fl_in_2,NC_NOWRITE,in_id_2_arr+thr_idx);
  in_id_2=in_id_2_arr[0];
  
  /* Parse auxiliary coordinates */
  if(aux_nbr > 0){
     int aux_idx_nbr;
     aux=nco_aux_evl(in_id_1,aux_nbr,aux_arg,&aux_idx_nbr);
     if(aux_idx_nbr > 0){
        lmt=(lmt_sct **)nco_realloc(lmt,(lmt_nbr+aux_idx_nbr)*sizeof(lmt_sct *));
        int lmt_nbr_new=lmt_nbr+aux_idx_nbr;
        int aux_idx=0;
        for(int lmt_idx=lmt_nbr;lmt_idx<lmt_nbr_new;lmt_idx++) lmt[lmt_idx]=aux[aux_idx++];
        lmt_nbr=lmt_nbr_new;
     } /* endif aux */
  } /* endif aux_nbr */
  
  /* Get number of variables and dimensions in file */
  (void)nco_inq(in_id_1,&nbr_dmn_fl_1,&nbr_var_fl_1,(int *)NULL,(int *)NULL);
  (void)nco_inq(in_id_2,&nbr_dmn_fl_2,&nbr_var_fl_2,(int *)NULL,(int *)NULL);
  (void)nco_inq_format(in_id_1,&fl_in_fmt_1);
  (void)nco_inq_format(in_id_2,&fl_in_fmt_2);
  
  /* Form initial extraction list which may include extended regular expressions */
  xtr_lst_1=nco_var_lst_mk(in_id_1,nbr_var_fl_1,var_lst_in,EXCLUDE_INPUT_LIST,EXTRACT_ALL_COORDINATES,&nbr_xtr_1);
  xtr_lst_2=nco_var_lst_mk(in_id_2,nbr_var_fl_2,var_lst_in,EXCLUDE_INPUT_LIST,EXTRACT_ALL_COORDINATES,&nbr_xtr_2);
  
  /* Change included variables to excluded variables */
  if(EXCLUDE_INPUT_LIST) xtr_lst_1=nco_var_lst_xcl(in_id_1,nbr_var_fl_1,xtr_lst_1,&nbr_xtr_1);
  if(EXCLUDE_INPUT_LIST) xtr_lst_2=nco_var_lst_xcl(in_id_2,nbr_var_fl_2,xtr_lst_2,&nbr_xtr_2);
  
  /* Is this a CCM/CCSM/CF-format history tape? */
  CNV_CCM_CCSM_CF=nco_cnv_ccm_ccsm_cf_inq(in_id_1);

  /* Add all coordinate variables to extraction list */
  if(EXTRACT_ALL_COORDINATES) xtr_lst_1=nco_var_lst_crd_add(in_id_1,nbr_dmn_fl_1,nbr_var_fl_1,xtr_lst_1,&nbr_xtr_1,CNV_CCM_CCSM_CF);
  if(EXTRACT_ALL_COORDINATES) xtr_lst_2=nco_var_lst_crd_add(in_id_2,nbr_dmn_fl_2,nbr_var_fl_2,xtr_lst_2,&nbr_xtr_2,CNV_CCM_CCSM_CF);
  
  /* Extract coordinates associated with extracted variables */
  if(EXTRACT_ASSOCIATED_COORDINATES) xtr_lst_1=nco_var_lst_crd_ass_add(in_id_1,xtr_lst_1,&nbr_xtr_1,CNV_CCM_CCSM_CF);
  if(EXTRACT_ASSOCIATED_COORDINATES) xtr_lst_2=nco_var_lst_crd_ass_add(in_id_2,xtr_lst_2,&nbr_xtr_2,CNV_CCM_CCSM_CF);
  
  /* With fully symmetric 1<->2 ordering, may occasionally find nbr_xtr_2 > nbr_xtr_1 
     This occurs, e.g., when fl_in_1 contains reduced variables and full coordinates
     are only in fl_in_2 and so will not appear xtr_lst_1 */
  
  /* Sort extraction list by variable ID for fastest I/O */
  if(nbr_xtr_1 > 1) xtr_lst_1=nco_lst_srt_nm_id(xtr_lst_1,nbr_xtr_1,False);
  if(nbr_xtr_2 > 1) xtr_lst_2=nco_lst_srt_nm_id(xtr_lst_2,nbr_xtr_2,False);
  
  /* We now have final list of variables to extract. Phew. */
  
  /* Find coordinate/dimension values associated with user-specified limits
     NB: nco_lmt_evl() with same nc_id contains OpenMP critical region */
  for(idx=0;idx<lmt_nbr;idx++) (void)nco_lmt_evl(in_id_1,lmt[idx],0L,FORTRAN_IDX_CNV);

  /* Place all dimensions in lmt_all_lst */
  lmt_all_lst=(lmt_all_sct **)nco_malloc(nbr_dmn_fl_1*sizeof(lmt_all_sct *));
  /* Initialize lmt_all_sct's */ 
  (void)nco_msa_lmt_all_int(in_id_1,MSA_USR_RDR,lmt_all_lst,nbr_dmn_fl_1,lmt,lmt_nbr);

  /* Find dimensions associated with variables to be extracted */
  dmn_lst_1=nco_dmn_lst_ass_var(in_id_1,xtr_lst_1,nbr_xtr_1,&nbr_dmn_xtr_1);
  dmn_lst_2=nco_dmn_lst_ass_var(in_id_2,xtr_lst_2,nbr_xtr_2,&nbr_dmn_xtr_2);
  
  /* Fill-in dimension structure for all extracted dimensions */
  dim_1=(dmn_sct **)nco_malloc(nbr_dmn_xtr_1*sizeof(dmn_sct *));
  dim_2=(dmn_sct **)nco_malloc(nbr_dmn_xtr_2*sizeof(dmn_sct *));
  for(idx=0;idx<nbr_dmn_xtr_1;idx++) dim_1[idx]=nco_dmn_fll(in_id_1,dmn_lst_1[idx].id,dmn_lst_1[idx].nm);
  for(idx=0;idx<nbr_dmn_xtr_2;idx++) dim_2[idx]=nco_dmn_fll(in_id_2,dmn_lst_2[idx].id,dmn_lst_2[idx].nm);
  /* Dimension lists no longer needed */
  dmn_lst_1=nco_nm_id_lst_free(dmn_lst_1,nbr_dmn_xtr_1);
  dmn_lst_2=nco_nm_id_lst_free(dmn_lst_2,nbr_dmn_xtr_2);

  /* Check that dims in list 2 are a subset of list 1 and that they are the same size */
  (void)nco_dmn_sct_cmp(dim_1,nbr_dmn_xtr_1,dim_2,nbr_dmn_xtr_2,fl_in_1,fl_in_2);    
	  
  /* Duplicate input dimension structures for output dimension structures */
  dmn_out=(dmn_sct **)nco_malloc(nbr_dmn_xtr_1*sizeof(dmn_sct *));
  for(idx=0;idx<nbr_dmn_xtr_1;idx++){ 
    dmn_out[idx]=nco_dmn_dpl(dim_1[idx]);
    (void)nco_dmn_xrf(dim_1[idx],dmn_out[idx]);
  }

  /* Merge hyperslab limit information into dimension structures */
  if(nbr_dmn_fl_1 > 0) (void)nco_dmn_lmt_all_mrg(dmn_out,nbr_dmn_xtr_1,lmt_all_lst,nbr_dmn_fl_1); 

  if(dbg_lvl >= nco_dbg_sbr){
    for(idx=0;idx<nbr_xtr_1;idx++) (void)fprintf(stderr,"xtr_lst_1[%d].nm = %s, .id= %d\n",idx,xtr_lst_1[idx].nm,xtr_lst_1[idx].id);
  } /* end if */
  
  /* Fill-in variable structure list for all extracted variables */
  var_1=(var_sct **)nco_malloc(nbr_xtr_1*sizeof(var_sct *));
  var_2=(var_sct **)nco_malloc(nbr_xtr_2*sizeof(var_sct *));
  var_out=(var_sct **)nco_malloc(nbr_xtr_1*sizeof(var_sct *));
  for(idx=0;idx<nbr_xtr_1;idx++){
    var_1[idx]=nco_var_fll(in_id_1,xtr_lst_1[idx].id,xtr_lst_1[idx].nm,dim_1,nbr_dmn_xtr_1);
    var_out[idx]=nco_var_dpl(var_1[idx]);
    (void)nco_xrf_var(var_1[idx],var_out[idx]);
    (void)nco_xrf_dmn(var_out[idx]);
  } /* end loop over idx */
  for(idx=0;idx<nbr_xtr_2;idx++) var_2[idx]=nco_var_fll(in_id_2,xtr_lst_2[idx].id,xtr_lst_2[idx].nm,dim_2,nbr_dmn_xtr_2);

  /* Extraction lists no longer needed */
  xtr_lst_1=nco_nm_id_lst_free(xtr_lst_1,nbr_xtr_1);
  xtr_lst_2=nco_nm_id_lst_free(xtr_lst_2,nbr_xtr_2);
  
  /* Die gracefully on unsupported features... */
  if(nbr_xtr_1 < nbr_xtr_2){
    (void)fprintf(fp_stdout,"%s: WARNING First file has fewer extracted variables than second file (%d < %d). This desired feature is TODO nco581.\n",prg_nm,nbr_xtr_1,nbr_xtr_2);
      nco_exit(EXIT_FAILURE);
  } /* endif */

  /* Refresh var_out with dim_out data */
  (void)nco_var_dmn_refresh(var_out,nbr_xtr_1);

  /* Change dimensions in dim_2 to dim_out */
  for(idx=0;idx<nbr_dmn_xtr_2;idx++){
    for(jdx=0;jdx<nbr_dmn_xtr_1;jdx++)  
      if(!strcmp(dim_2[idx]->nm,dmn_out[jdx]->nm)){  
        /* NB: Copy new dim data but do NOT free original as dimension element is aliased in var_2 array */ 
	(void)nco_dmn_cpy(dim_2[idx],dmn_out[jdx]);  
        break;   
      } /* endif */
    /* Dimension not found so die gracefully */
    if(jdx==nbr_dmn_xtr_1){
      (void)fprintf(fp_stdout,"%s: ERROR dimension \"%s\" in second file %s is not present in first file %s\n",prg_nm,dim_2[idx]->nm,fl_in_2,fl_in_1);
      nco_exit(EXIT_FAILURE);
    } /* endif dimension not found */
  } /* end loop over dimensions */
       
  /* Refresh var_2 with the new dim_2 data */
  (void)nco_var_dmn_refresh(var_2,nbr_xtr_2);
   
  /* Divide variable lists into lists of fixed variables and variables to be processed
     Create lists from file_1 last so those values remain in *_out arrays */
  (void)nco_var_lst_dvd(var_2,var_out,nbr_xtr_2,CNV_CCM_CCSM_CF,nco_pck_plc_nil,nco_pck_map_nil,(dmn_sct **)NULL,0,&var_fix_2,&var_fix_out,&nbr_var_fix_2,&var_prc_2,&var_prc_out,&nbr_var_prc_2);
  /* Avoid double-free() condition */
  var_fix_out=(var_sct **)nco_free(var_fix_out);
  var_prc_out=(var_sct **)nco_free(var_prc_out);
  (void)nco_var_lst_dvd(var_1,var_out,nbr_xtr_1,CNV_CCM_CCSM_CF,nco_pck_plc_nil,nco_pck_map_nil,(dmn_sct **)NULL,0,&var_fix_1,&var_fix_out,&nbr_var_fix_1,&var_prc_1,&var_prc_out,&nbr_var_prc_1);

  /* Die gracefully on unsupported features... */
  if(nbr_var_fix_1 < nbr_var_fix_2){
    (void)fprintf(fp_stdout,"%s: ERROR First file has fewer fixed variables than second file (%d < %d). This feature is TODO nco581.\n",prg_nm,nbr_var_fix_1,nbr_var_fix_2);
      nco_exit(EXIT_FAILURE);
  } /* endif */

  /* Merge two variable lists into same order */
  rcd=nco_var_lst_mrg(&var_prc_1,&var_prc_2,&nbr_var_prc_1,&nbr_var_prc_2); 

  /* Make output and input files consanguinous */
  if(fl_out_fmt == NCO_FORMAT_UNDEFINED) fl_out_fmt=fl_in_fmt_1;

  /* Verify output file format supports requested actions */
  (void)nco_fl_fmt_vet(fl_out_fmt,cnk_nbr,dfl_lvl);

  /* Open output file */
  fl_out_tmp=nco_fl_out_open(fl_out,FORCE_APPEND,FORCE_OVERWRITE,fl_out_fmt,&out_id);
  
  /* Copy global attributes */
  (void)nco_att_cpy(in_id_1,out_id,NC_GLOBAL,NC_GLOBAL,(nco_bool)True);
  
  /* Catenate time-stamped command line to "history" global attribute */
  if(HISTORY_APPEND) (void)nco_hst_att_cat(out_id,cmd_ln);
  
  if(thr_nbr > 0 && HISTORY_APPEND) (void)nco_thr_att_cat(out_id,thr_nbr);
  
  /* Define dimensions in output file */
  (void)nco_dmn_dfn(fl_out,out_id,dmn_out,nbr_dmn_xtr_1);
  
  /* fxm: TODO 550 put max_dim_sz/list(var_1,var_2) into var_def(var_out) */
  /* Define variables in output file, copy their attributes */
  (void)nco_var_dfn(in_id_1,fl_out,out_id,var_out,nbr_xtr_1,(dmn_sct **)NULL,(int)0,nco_pck_plc_nil,nco_pck_map_nil,dfl_lvl);
  
  /* Set chunksize parameters */
  if(fl_out_fmt == NC_FORMAT_NETCDF4 || fl_out_fmt == NC_FORMAT_NETCDF4_CLASSIC) (void)nco_cnk_sz_set(out_id,lmt_all_lst,nbr_dmn_fl_1,&cnk_map,&cnk_plc,cnk_sz_scl,cnk,cnk_nbr);

  /* Turn off default filling behavior to enhance efficiency */
  nco_set_fill(out_id,NC_NOFILL,&fll_md_old);
  
  /* Take output file out of define mode */
  (void)nco_enddef(out_id);
  
  /* Assign zero-start and unity-stride vectors to output variables */
  (void)nco_var_srd_srt_set(var_out,nbr_xtr_1);
  
  /* Copy variable data for non-processed variables */
  (void)nco_msa_var_val_cpy(in_id_1,out_id,var_fix_1,nbr_var_fix_1,lmt_all_lst,nbr_dmn_fl_1);
  
  /* ncbo() code has been similar to ncea() (and ncra()) wherever possible
     Major differences occur where performance would otherwise suffer
     From now on, however, binary-file and binary-operation nature of ncbo()
     is too different from ncea() paradigm to justify following ncea() style.
     Instead, we adopt symmetric nomenclature (e.g., file_1, file_2), and 
     perform differences variable-by-variable so peak memory usage goes as
     Order(2*maximum variable size) rather than Order(3*maximum record size) or
     Order(3*file size) */

  /* Perform various error-checks on input file */
  if(False) (void)nco_fl_cmp_err_chk();
  
  /* Default operation depends on invocation name */
  if(nco_op_typ_sng == NULL) nco_op_typ=nco_op_typ_get(nco_op_typ_sng);
    
  /* Timestamp end of metadata setup and disk layout */
  rcd+=nco_ddra((char *)NULL,(char *)NULL,&ddra_info);
  ddra_info.tmr_flg=nco_tmr_rgl;

  /* Loop over variables */
#ifdef _OPENMP
  /* OpenMP notes:
     shared(): msk and wgt are not altered within loop
     private(): wgt_avg does not need initialization */
#pragma omp parallel for default(none) firstprivate(ddra_info) private(idx,in_id_1,in_id_2,dmn_idx,dmn_jdx) shared(dbg_lvl,dim_1,fl_in_1,fl_in_2,fl_out,flg_ddra,in_id_1_arr,in_id_2_arr,nbr_dmn_xtr_1,nbr_var_prc_1,nbr_var_prc_2,nco_op_typ,out_id,prg_nm,rcd,var_prc_1,var_prc_2,var_prc_out,lmt_all_lst,nbr_dmn_fl_1)
#endif /* !_OPENMP */
  for(idx=0;idx<nbr_var_prc_1;idx++){
    int has_mss_val=False;
    ptr_unn mss_val;

    if(dbg_lvl >= nco_dbg_var && dbg_lvl < nco_dbg_nbr) (void)fprintf(fp_stderr,"%s, ",var_prc_1[idx]->nm);
    if(dbg_lvl >= nco_dbg_var && dbg_lvl < nco_dbg_nbr) (void)fflush(fp_stderr);
    
    in_id_1=in_id_1_arr[omp_get_thread_num()];
    in_id_2=in_id_2_arr[omp_get_thread_num()];

    (void)nco_var_mtd_refresh(in_id_1,var_prc_1[idx]);
    has_mss_val=var_prc_1[idx]->has_mss_val; 
    (void)nco_msa_var_get(in_id_1,var_prc_1[idx],lmt_all_lst,nbr_dmn_fl_1);
    
    /* Find and set variable dmn_nbr, ID, mss_val, type in second file */
    (void)nco_var_mtd_refresh(in_id_2,var_prc_2[idx]);
    
    /* Read hyperslab from second file */
    (void)nco_msa_var_get(in_id_2,var_prc_2[idx],lmt_all_lst,nbr_dmn_fl_1);
       
    /* Check that all dims in var_prc_2 are in var_prc_1 */
    for(dmn_idx=0;dmn_idx<var_prc_2[idx]->nbr_dim;dmn_idx++){
      for(dmn_jdx=0;dmn_jdx<var_prc_1[idx]->nbr_dim;dmn_jdx++)  
	if(!strcmp(var_prc_2[idx]->dim[dmn_idx]->nm,var_prc_1[idx]->dim[dmn_jdx]->nm))
	  break;
      if(dmn_jdx==var_prc_1[idx]->nbr_dim){
	(void)fprintf(fp_stdout,"%s: ERROR Variables do not conform:\nFile %s variable %s has dimension %s not present in file %s variable %s\n",prg_nm,fl_in_2,var_prc_2[idx]->nm, var_prc_2[idx]->dim[dmn_idx]->nm,fl_in_1,var_prc_1[idx]->nm);
	nco_exit(EXIT_FAILURE);
      } /* endif error */
    } /* end loop over idx */
	    		
    /* Die gracefully on unsupported features... */
    if(var_prc_1[idx]->nbr_dim < var_prc_2[idx]->nbr_dim){
      (void)fprintf(fp_stdout,"%s: ERROR Variable %s has lesser rank in first file than in second file (%d < %d). This feature is NCO TODO 552.\n",prg_nm,var_prc_1[idx]->nm,var_prc_1[idx]->nbr_dim,var_prc_2[idx]->nbr_dim);
      nco_exit(EXIT_FAILURE);
    } /* endif */
    
    if(var_prc_1[idx]->nbr_dim > var_prc_2[idx]->nbr_dim) (void)ncap_var_cnf_dmn(&var_prc_out[idx],&var_prc_2[idx]);
    
    /* var2 now conforms in size to var1, and is in memory */
    
    /* fxm: TODO 268 allow var1 or var2 to typecast */
    /* Make sure var2 conforms to type of var1 */
    if(var_prc_1[idx]->type != var_prc_2[idx]->type){
      if(dbg_lvl >= nco_dbg_std) (void)fprintf(fp_stderr,"%s: INFO Input variables do not conform in type:\nFile 1 = %s variable %s has type %s\nFile 2 = %s variable %s has type %s\nFile 3 = %s variable %s will have type %s\n",prg_nm,fl_in_1,var_prc_1[idx]->nm,nco_typ_sng(var_prc_1[idx]->type),fl_in_2,var_prc_2[idx]->nm,nco_typ_sng(var_prc_2[idx]->type),fl_out,var_prc_1[idx]->nm,nco_typ_sng(var_prc_1[idx]->type));
    }  /* endif different type */
    var_prc_2[idx]=nco_var_cnf_typ(var_prc_1[idx]->type,var_prc_2[idx]);
    
    /* Change missing_value of var_prc_2, if any, to missing_value of var_prc_1, if any */
    has_mss_val=nco_mss_val_cnf(var_prc_1[idx],var_prc_2[idx]);
    
    /* mss_val in fl_1, if any, overrides mss_val in fl_2 */
    if(has_mss_val) mss_val=var_prc_1[idx]->mss_val;
    
    /* Perform specified binary operation */
    switch(nco_op_typ){
    case nco_op_add: /* [enm] Add file_1 to file_2 */
      (void)nco_var_add(var_prc_1[idx]->type,var_prc_1[idx]->sz,has_mss_val,mss_val,var_prc_2[idx]->val,var_prc_1[idx]->val); break;
    case nco_op_mlt: /* [enm] Multiply file_1 by file_2 */
      (void)nco_var_mlt(var_prc_1[idx]->type,var_prc_1[idx]->sz,has_mss_val,mss_val,var_prc_2[idx]->val,var_prc_1[idx]->val); break;
    case nco_op_dvd: /* [enm] Divide file_1 by file_2 */
      (void)nco_var_dvd(var_prc_1[idx]->type,var_prc_1[idx]->sz,has_mss_val,mss_val,var_prc_2[idx]->val,var_prc_1[idx]->val); break;
    case nco_op_sbt: /* [enm] Subtract file_2 from file_1 */
      (void)nco_var_sbt(var_prc_1[idx]->type,var_prc_1[idx]->sz,has_mss_val,mss_val,var_prc_2[idx]->val,var_prc_1[idx]->val); break;
    default: /* Other defined nco_op_typ values are valid for ncra(), ncrcat(), ncwa(), not ncbo() */
      (void)fprintf(fp_stdout,"%s: ERROR Illegal nco_op_typ in binary operation\n",prg_nm);
      nco_exit(EXIT_FAILURE);
      break;
    } /* end case */
    
    var_prc_2[idx]->val.vp=nco_free(var_prc_2[idx]->val.vp);

#ifdef _OPENMP
#pragma omp critical
#endif /* _OPENMP */
    { /* begin OpenMP critical */
      /* Copy result to output file and free workspace buffer */
      if(var_prc_1[idx]->nbr_dim == 0){
	(void)nco_put_var1(out_id,var_prc_out[idx]->id,var_prc_out[idx]->srt,var_prc_1[idx]->val.vp,var_prc_1[idx]->type);
      }else{ /* end if variable is scalar */
	(void)nco_put_vara(out_id,var_prc_out[idx]->id,var_prc_out[idx]->srt,var_prc_out[idx]->cnt,var_prc_1[idx]->val.vp,var_prc_1[idx]->type);
      } /* end else */
    } /* end OpenMP critical */
    var_prc_1[idx]->val.vp=nco_free(var_prc_1[idx]->val.vp);
    
    if(flg_ddra){
      /* DDRA diagnostics
	 Usage:
	 ncbo -O -C --mdl -p ~/nco/data in.nc in.nc ~/foo.nc
	 ncbo -O -C --mdl -p ${DATA}/nco_bm stl_5km.nc stl_5km.nc ~/foo.nc
	 ncbo -O -C --mdl -p ${DATA}/nco_bm gcm_T85.nc gcm_T85.nc ~/foo.nc */
      
      /* Assign remaining input for DDRA diagnostics */
      ddra_info.lmn_nbr=var_prc_1[idx]->sz; /* [nbr] Variable size */
      ddra_info.nco_op_typ=nco_op_typ; /* [enm] Operation type */
      ddra_info.rnk_var=var_prc_1[idx]->nbr_dim; /* I [nbr] Variable rank (in input file) */
      ddra_info.var_idx=idx; /* [enm] Index */
      ddra_info.wrd_sz=nco_typ_lng(var_prc_1[idx]->type); /* [B] Bytes per element */
      
      /* DDRA diagnostics */
      rcd+=nco_ddra /* [fnc] Count operations */
	(var_prc_1[idx]->nm, /* I [sng] Variable name */
	 (char *)NULL, /* I [sng] Weight name */
	 &ddra_info); /* I [sct] DDRA information */
      
    } /* !flg_ddra */
    
  } /* end (OpenMP parallel for) loop over idx */
  if(dbg_lvl >= nco_dbg_fl) (void)fprintf(stderr,"\n");
  
  /* Close input netCDF files */
  for(thr_idx=0;thr_idx<thr_nbr;thr_idx++) nco_close(in_id_1_arr[thr_idx]);
  for(thr_idx=0;thr_idx<thr_nbr;thr_idx++) nco_close(in_id_2_arr[thr_idx]);
  
  /* Close output file and move it from temporary to permanent location */
  (void)nco_fl_out_cls(fl_out,fl_out_tmp,out_id);
  
  /* Remove local copy of file */
  if(FILE_1_RETRIEVED_FROM_REMOTE_LOCATION && REMOVE_REMOTE_FILES_AFTER_PROCESSING) (void)nco_fl_rm(fl_in_1);
  if(FILE_2_RETRIEVED_FROM_REMOTE_LOCATION && REMOVE_REMOTE_FILES_AFTER_PROCESSING) (void)nco_fl_rm(fl_in_2);
  
  /* Clean memory unless dirty memory allowed */
  if(flg_cln){
    /* ncbo-specific memory */
    if(fl_in_1) fl_in_1=(char *)nco_free(fl_in_1);
    if(fl_in_2) fl_in_2=(char *)nco_free(fl_in_2);
    
    /* NCO-generic clean-up */
    /* Free individual strings/arrays */
    /* NB: free lmt[] is now referenced within lmt_all_lst[idx] */
    for(idx=0;idx<nbr_dmn_fl_1;idx++)
      for(jdx=0;jdx<lmt_all_lst[idx]->lmt_dmn_nbr;jdx++)
         lmt_all_lst[idx]->lmt_dmn[jdx]=nco_lmt_free(lmt_all_lst[idx]->lmt_dmn[jdx]);

    if(nbr_dmn_fl_1 > 0) lmt_all_lst=nco_lmt_all_lst_free(lmt_all_lst,nbr_dmn_fl_1);   
    lmt=(lmt_sct**)nco_free(lmt); 

    if(cmd_ln) cmd_ln=(char *)nco_free(cmd_ln);
    if(cnk_map_sng) cnk_map_sng=(char *)strdup(cnk_map_sng);
    if(cnk_plc_sng) cnk_plc_sng=(char *)strdup(cnk_plc_sng);
    if(fl_out) fl_out=(char *)nco_free(fl_out);
    if(fl_out_tmp) fl_out_tmp=(char *)nco_free(fl_out_tmp);
    if(fl_pth) fl_pth=(char *)nco_free(fl_pth);
    if(fl_pth_lcl) fl_pth_lcl=(char *)nco_free(fl_pth_lcl);
    if(in_id_1_arr) in_id_1_arr=(int *)nco_free(in_id_1_arr);
    if(in_id_2_arr) in_id_2_arr=(int *)nco_free(in_id_2_arr);
    /* Free lists of strings */
    if(fl_lst_in && fl_lst_abb == NULL) fl_lst_in=nco_sng_lst_free(fl_lst_in,fl_nbr); 
    if(fl_lst_in && fl_lst_abb) fl_lst_in=nco_sng_lst_free(fl_lst_in,1);
    if(fl_lst_abb) fl_lst_abb=nco_sng_lst_free(fl_lst_abb,abb_arg_nbr);
    if(var_lst_in_nbr > 0) var_lst_in=nco_sng_lst_free(var_lst_in,var_lst_in_nbr);
    /* Free limits */
    for(idx=0;idx<lmt_nbr;idx++) lmt_arg[idx]=(char *)nco_free(lmt_arg[idx]);
    for(idx=0;idx<aux_nbr;idx++) aux_arg[idx]=(char *)nco_free(aux_arg[idx]);
    if(aux_nbr > 0) aux=(lmt_sct **)nco_free(aux);
    /* Free chunking information */
    for(idx=0;idx<cnk_nbr;idx++) cnk_arg[idx]=(char *)nco_free(cnk_arg[idx]);
    if(cnk_nbr > 0) cnk=nco_cnk_lst_free(cnk,cnk_nbr);
    /* Free dimension lists */
    if(nbr_dmn_xtr_1 > 0) dim_1=nco_dmn_lst_free(dim_1,nbr_dmn_xtr_1);
    if(nbr_dmn_xtr_1 > 0) dmn_out=nco_dmn_lst_free(dmn_out,nbr_dmn_xtr_1);
    if(nbr_dmn_xtr_2 > 0) dim_2=nco_dmn_lst_free(dim_2,nbr_dmn_xtr_2);

    /* Free variable lists 
       Using nco_var_lst_free() to free main var_1 and var_2 lists would fail
       if ncap_var_prc_dmn() had to broadcast any variables because pointer
       var_1 and var_2 still contain dangling pointer to old variable.
       Hence, use nco_var_lst_free() to free prc and fix lists then
       use nco_free() to free main var_1 and var_2 lists.
       Dangling pointers in var_1 and var_2 are unsafe: fxm TODO 578 */
    if(nbr_var_prc_1 > 0) var_prc_1=nco_var_lst_free(var_prc_1,nbr_var_prc_1);
    if(nbr_var_fix_1 > 0) var_fix_1=nco_var_lst_free(var_fix_1,nbr_var_fix_1);
    if(nbr_var_prc_2 > 0) var_prc_2=nco_var_lst_free(var_prc_2,nbr_var_prc_2);
    if(nbr_var_fix_2 > 0) var_fix_2=nco_var_lst_free(var_fix_2,nbr_var_fix_2);
    var_1=(var_sct **)nco_free(var_1);
    var_2=(var_sct **)nco_free(var_2);
    if(nbr_xtr_1 > 0) var_out=nco_var_lst_free(var_out,nbr_xtr_1);
    var_prc_out=(var_sct **)nco_free(var_prc_out);
    var_fix_out=(var_sct **)nco_free(var_fix_out);
  } /* !flg_cln */

  /* End timer */ 
  ddra_info.tmr_flg=nco_tmr_end; /* [enm] Timer flag */
  rcd+=nco_ddra((char *)NULL,(char *)NULL,&ddra_info);

  if(rcd != NC_NOERR) nco_err_exit(rcd,"main");
  nco_exit_gracefully();
  return EXIT_SUCCESS;
} /* end main() */
