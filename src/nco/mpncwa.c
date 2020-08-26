/* $Header$ */

/* mpncwa -- netCDF weighted averager */

/* Purpose: Compute averages of specified hyperslabs of specfied variables
   in a single input netCDF file and output them to a single file. */

/* Copyright (C) 1995--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License.
   You are permitted to link NCO with the HDF, netCDF, OPeNDAP, and UDUnits
   libraries and to distribute the resulting executables under the terms 
   of the BSD, but in addition obeying the extra stipulations of the 
   HDF, netCDF, OPeNDAP, and UDUnits licenses.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
   See the 3-Clause BSD License for more details.
   
   The original author of this software, Charlie Zender, seeks to improve
   it with your suggestions, contributions, bug-reports, and patches.
   Please contact the NCO project at http://nco.sf.net or write to
   Charlie Zender
   Department of Earth System Science
   University of California, Irvine
   Irvine, CA 92697-3100 */

/* fxm: 19981202 Deactivated -n and -W switches while I rethink the normalization switches */

/* Usage:
   ncwa -O -a lon ~/nco/data/in.nc ~/foo.nc
   ncwa -O -R -p /ZENDER/tmp -l ~/nco/data in.nc ~/foo.nc
   ncwa -O -C -a lat,lon,time -w gw -v PS -p /fs/cgd/csm/input/atm SEP1.T42.0596.nc ~/foo.nc;ncks -H foo.nc
   scp ~/nco/src/nco/ncwa.c esmf.ess.uci.edu:nco/src/nco */

#ifdef HAVE_CONFIG_H
# include <config.h> /* Autotools tokens */
#endif /* !HAVE_CONFIG_H */

/* Standard C headers */
#include <math.h> /* sin cos cos sin 3.14159 */
#include <stdio.h> /* stderr, FILE, NULL, etc. */
#include <stdlib.h> /* atof, atoi, malloc, getopt */
#include <string.h> /* strcmp() */
#include <sys/stat.h> /* stat() */
#include <time.h> /* machine time */
#include <unistd.h> /* POSIX stuff */
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
#include <mpi.h> /* MPI definitions */
#include "nco_mpi.h" /* MPI utilities */
#endif /* !ENABLE_MPI */

/* Personal headers */
/* #define MAIN_PROGRAM_FILE MUST precede #include libnco.h */
#define MAIN_PROGRAM_FILE
#ifdef HAVE_BISON_FLEX
  # include "ncap_utl.h" /* netCDF arithmetic processor-specific definitions (symbol table, ...) */
#endif /* !HAVE_BISON_FLEX */
#include "libnco.h" /* netCDF Operator (NCO) library */

/* Global variables (keep consistent with global variables declared in ncap.c) */ 
size_t ncap_ncl_dpt_crr=0UL; /* [nbr] Depth of current #include file (incremented in ncap.l) */
size_t *ncap_ln_nbr_crr; /* [cnt] Line number (incremented in ncap.l) */
char **ncap_fl_spt_glb; /* [fl] Script file */

int 
main(int argc,char **argv)
{
  char **dmn_avg_lst_in=NULL_CEWI; /* Option a */
  char **fl_lst_abb=NULL; /* Option n */
  char **fl_lst_in=NULL_CEWI;
  char **var_lst_in=NULL_CEWI;
  char *cmd_ln;
  char *cnk_arg[NC_MAX_DIMS];
  char *cnk_map_sng=NULL_CEWI; /* [sng] Chunking map */
  char *cnk_plc_sng=NULL_CEWI; /* [sng] Chunking policy */
  char *fl_in=NULL;
  char *fl_out=NULL; /* Option o */
  char *fl_out_tmp=NULL_CEWI;
  char *fl_pth=NULL; /* Option p */
  char *fl_pth_lcl=NULL; /* Option l */
  char *lmt_arg[NC_MAX_DIMS];
  char *msk_nm=NULL;
  char *msk_cnd_sng=NULL; /* Mask string to be "parsed" and values given to msk_nm, msk_val, op_typ_rlt */
  char *nco_op_typ_sng; /* Operation type */
  char *opt_crr=NULL; /* [sng] String representation of current long-option name */
  char *optarg_lcl=NULL; /* [sng] Local copy of system optarg */
  char *sng_cnv_rcd=NULL_CEWI; /* [sng] strtol()/strtoul() return code */
  char *wgt_nm=NULL;

  const char * const CVS_Id="$Id$"; 
  const char * const CVS_Revision="$Revision$";
  const char * const opt_sht_lst="34567Aa:B:bCcD:d:FhIL:l:M:m:nNOo:p:rRST:t:v:Ww:xy:-:";
  
  cnk_dmn_sct **cnk_dmn=NULL_CEWI;
  
#if defined(__cplusplus) || defined(PGI_CC)
  ddra_info_sct ddra_info;
  ddra_info.flg_ddra=False;
#else /* !__cplusplus */
  ddra_info_sct ddra_info={.MRV_flg=False,.flg_ddra=False,.lmn_nbr=0LL,.lmn_nbr_avg=0LL,.lmn_nbr_wgt=0LL,.nco_op_typ=nco_op_nil,.rnk_avg=0,.rnk_var=0,.rnk_wgt=0,.tmr_flg=nco_tmr_srt,.var_idx=0,.wgt_brd_flg=False,.wrd_sz=0};
#endif /* !__cplusplus */
  
  dmn_sct **dim=NULL_CEWI;
  dmn_sct **dmn_out=NULL_CEWI;
  dmn_sct **dmn_avg=NULL_CEWI;
  
  double msk_val=1.0; /* Option M */
  
  extern char *optarg;
  extern int optind;
  
  /* Using naked stdin/stdout/stderr in parallel region generates warning
     Copy appropriate filehandle to variable scoped shared in parallel clause */
  FILE * const fp_stderr=stderr; /* [fl] stderr filehandle CEWI */
  FILE * const fp_stdout=stdout; /* [fl] stdout filehandle CEWI */
  
  int *in_id_arr;
  
  int abb_arg_nbr=0;
  int cnk_map=nco_cnk_map_nil; /* [enm] Chunking map */
  int cnk_nbr=0; /* [nbr] Number of chunk sizes */
  int cnk_plc=nco_cnk_plc_nil; /* [enm] Chunking policy */
  int dfl_lvl=NCO_DFL_LVL_UNDEFINED; /* [enm] Deflate level */
  int dmn_avg_nbr=0;
  int fl_idx=int_CEWI;
  int fl_nbr=0;
  int fl_in_fmt; /* [enm] Input file format */
  int fl_out_fmt=NCO_FORMAT_UNDEFINED; /* [enm] Output file format */
  int fll_md_old; /* [enm] Old fill mode */
  int gaa_nbr=0; /* [nbr] Number of global attributes to add */
  int idx=int_CEWI;
  int idx_avg;
  int in_id;
  int lmt_nbr=0; /* Option d. NB: lmt_nbr gets incremented */
  int log_lvl=0; /* [enm] netCDF library debugging verbosity [0..5] */
  int md_open; /* [enm] Mode flag for nc_open() call */
  int nbr_dmn_fl;
  int nbr_dmn_out=0;
  int nbr_dmn_xtr;
  int nbr_var_fix; /* nbr_var_fix gets incremented */
  int nbr_var_fl;
  int nbr_var_prc; /* nbr_var_prc gets incremented */
  int xtr_nbr=0; /* xtr_nbr won't otherwise be set for -c with no -v */
  int nco_op_typ=nco_op_avg; /* Operation type */
  int op_typ_rlt=0; /* Option o */
  int opt;
  int out_id;  
  int rcd=NC_NOERR; /* [rcd] Return code */
  int rec_dmn_id=NCO_REC_DMN_UNDEFINED; /* [id] Record dimension ID in input file */
  int thr_idx; /* [idx] Index of current thread */
  int thr_nbr=int_CEWI; /* [nbr] Thread number Option t */
  int var_lst_in_nbr=0;
  
  lmt_sct **lmt;
  
  cnv_sct *cnv; /* [sct] Convention structure */
  nco_bool DO_CONFORM_MSK=False; /* Did nco_var_cnf_dmn() find truly conforming mask? */
  nco_bool DO_CONFORM_WGT=False; /* Did nco_var_cnf_dmn() find truly conforming weight? */
  nco_bool EXCLUDE_INPUT_LIST=False; /* Option c */
  nco_bool EXTRACT_ALL_COORDINATES=False; /* Option c */
  nco_bool EXTRACT_ASSOCIATED_COORDINATES=True; /* Option C */
  nco_bool FL_RTR_RMT_LCN;
  nco_bool FL_LST_IN_FROM_STDIN=False; /* [flg] fl_lst_in comes from stdin */
  nco_bool FORCE_APPEND=False; /* Option A */
  nco_bool FORCE_OVERWRITE=False; /* Option O */
  nco_bool FORTRAN_IDX_CNV=False; /* Option F */
  nco_bool HISTORY_APPEND=True; /* Option h */
  nco_bool HPSS_TRY=False; /* [flg] Search HPSS for unfound files */
  nco_bool MULTIPLY_BY_TALLY=False; /* Not currently implemented */
  nco_bool MUST_CONFORM=False; /* [flg] Must nco_var_cnf_dmn() find truly conforming variables? */
  nco_bool NORMALIZE_BY_TALLY=True; /* Not currently implemented */
  nco_bool NORMALIZE_BY_WEIGHT=True; /* Not currently implemented */
  nco_bool NRM_BY_DNM=True; /* Option N Normalize by denominator */
  nco_bool RAM_CREATE=False; /* [flg] Create file in RAM */
  nco_bool RAM_OPEN=False; /* [flg] Open (netCDF3-only) file(s) in RAM */
  nco_bool SHARE_CREATE=False; /* [flg] Create (netCDF3-only) file(s) with unbuffered I/O */
  nco_bool SHARE_OPEN=False; /* [flg] Open (netCDF3-only) file(s) with unbuffered I/O */
  nco_bool RM_RMT_FL_PST_PRC=True; /* Option R */
  nco_bool WGT_MSK_CRD_VAR=True; /* [flg] Weight and/or mask coordinate variables */
  nco_bool WRT_TMP_FL=True; /* [flg] Write output to temporary file */
  nco_bool flg_mmr_cln=False; /* [flg] Clean memory prior to exit */
  nco_bool flg_ddra=False; /* [flg] DDRA diagnostics */
  nco_bool flg_opt_a=False; /* Option a */
  nco_bool flg_rdd=False; /* [flg] Retain degenerate dimensions */
  
  nm_id_sct *dmn_lst;
  nm_id_sct *xtr_lst=NULL; /* xtr_lst may be alloc()'d from NULL with -c option */
  nm_id_sct *dmn_avg_lst;
  
  prs_sct prs_arg;  /* I/O [sct] Global information required in ncwa parser */
  
  size_t bfr_sz_hnt=NC_SIZEHINT_DEFAULT; /* [B] Buffer size hint */
    size_t cnk_csh_byt=NCO_CNK_CSH_BYT_DFL; /* [B] Chunk cache size */
  size_t cnk_min_byt=NCO_CNK_SZ_MIN_BYT_DFL; /* [B] Minimize size of variable to chunk */
  size_t cnk_sz_byt=0UL; /* [B] Chunk size in bytes */
  size_t cnk_sz_scl=0UL; /* [nbr] Chunk size scalar */
  size_t hdr_pad=0UL; /* [B] Pad at end of header section */
  
  var_sct **var;
  var_sct **var_fix;
  var_sct **var_fix_out;
  var_sct **var_out;
  var_sct **var_prc;
  var_sct **var_prc_out;
  var_sct *msk=NULL;
  var_sct *msk_out=NULL;
  var_sct *wgt=NULL;
  var_sct *wgt_avg=NULL;
  var_sct *wgt_out=NULL;
  
#ifdef ENABLE_MPI
  /* Declare all MPI-specific variables here */
  MPI_Status mpi_stt; /* [enm] Status check to decode msg_tag_typ */
  
  nco_bool TKN_WRT_FREE=True; /* [flg] Write-access to output file is available */
  
  int fl_nm_lng; /* [nbr] Output file name length */
  int msg_bfr[msg_bfr_lng]; /* [bfr] Buffer containing var, idx, tkn_wrt_rsp */
  int msg_tag_typ; /* [enm] MPI message tag type */
  int prc_rnk; /* [idx] Process rank */
  int prc_nbr=0; /* [nbr] Number of MPI processes */
  int tkn_wrt_rsp; /* [enm] Response to request for write token */
  int var_wrt_nbr=0; /* [nbr] Variables written to output file until now */
  int rnk_wrk; /* [idx] Worker rank */
  int wrk_id_bfr[wrk_id_bfr_lng]; /* [bfr] Buffer for rnk_wrk */
#endif /* !ENABLE_MPI */
  
  static struct option opt_lng[]={ /* Structure ordered by short option key if possible */
    /* Long options with no argument, no short option counterpart */
    {"clean",no_argument,0,0}, /* [flg] Clean memory prior to exit */
    {"mmr_cln",no_argument,0,0}, /* [flg] Clean memory prior to exit */
    {"drt",no_argument,0,0}, /* [flg] Allow dirty memory on exit */
    {"dirty",no_argument,0,0}, /* [flg] Allow dirty memory on exit */
    {"mmr_drt",no_argument,0,0}, /* [flg] Allow dirty memory on exit */
    {"ddra",no_argument,0,0}, /* [flg] DDRA diagnostics */
    {"mdl_cmp",no_argument,0,0}, /* [flg] DDRA diagnostics */
    {"ram_all",no_argument,0,0}, /* [flg] Open and create (netCDF3) file(s) in RAM */
    {"create_ram",no_argument,0,0}, /* [flg] Create file in RAM */
    {"open_ram",no_argument,0,0}, /* [flg] Open (netCDF3) file(s) in RAM */
    {"diskless_all",no_argument,0,0}, /* [flg] Open and create (netCDF3) file(s) in RAM */
    {"share_all",no_argument,0,0}, /* [flg] Open and create (netCDF3) file(s) with unbuffered I/O */
    {"create_share",no_argument,0,0}, /* [flg] Create (netCDF3) file(s) with unbuffered I/O */
    {"open_share",no_argument,0,0}, /* [flg] Open (netCDF3) file(s) with unbuffered I/O */
    {"unbuffered_io",no_argument,0,0}, /* [flg] Open and create (netCDF3) file(s) with unbuffered I/O */
    {"uio",no_argument,0,0}, /* [flg] Open and create (netCDF3) file(s) with unbuffered I/O */
    {"wrt_tmp_fl",no_argument,0,0}, /* [flg] Write output to temporary file */
    {"write_tmp_fl",no_argument,0,0}, /* [flg] Write output to temporary file */
    {"no_tmp_fl",no_argument,0,0}, /* [flg] Do not write output to temporary file */
    {"version",no_argument,0,0},
    {"vrs",no_argument,0,0},
    /* Long options with argument, no short option counterpart */
    {"bfr_sz_hnt",required_argument,0,0}, /* [B] Buffer size hint */
    {"buffer_size_hint",required_argument,0,0}, /* [B] Buffer size hint */
    {"cnk_byt",required_argument,0,0}, /* [B] Chunk size in bytes */
    {"chunk_byte",required_argument,0,0}, /* [B] Chunk size in bytes */
    {"cnk_dmn",required_argument,0,0}, /* [nbr] Chunk size */
    {"chunk_dimension",required_argument,0,0}, /* [nbr] Chunk size */
    {"cnk_map",required_argument,0,0}, /* [nbr] Chunking map */
    {"chunk_map",required_argument,0,0}, /* [nbr] Chunking map */
    {"cnk_min",required_argument,0,0}, /* [B] Minimize size of variable to chunk */
    {"chunk_min",required_argument,0,0}, /* [B] Minimize size of variable to chunk */
    {"cnk_plc",required_argument,0,0}, /* [nbr] Chunking policy */
    {"chunk_policy",required_argument,0,0}, /* [nbr] Chunking policy */
    {"cnk_scl",required_argument,0,0}, /* [nbr] Chunk size scalar */
    {"chunk_scalar",required_argument,0,0}, /* [nbr] Chunk size scalar */
    {"fl_fmt",required_argument,0,0},
    {"file_format",required_argument,0,0},
    {"gaa",required_argument,0,0}, /* [sng] Global attribute add */
    {"glb_att_add",required_argument,0,0}, /* [sng] Global attribute add */
    {"hdr_pad",required_argument,0,0},
    {"header_pad",required_argument,0,0},
    {"log_lvl",required_argument,0,0}, /* [enm] netCDF library debugging verbosity [0..5] */
    {"log_level",required_argument,0,0}, /* [enm] netCDF library debugging verbosity [0..5] */
    /* Long options with short counterparts */
    {"3",no_argument,0,'3'},
    {"4",no_argument,0,'4'},
    {"netcdf4",no_argument,0,'4'},
    {"5",no_argument,0,'5'},
    {"64bit_data",no_argument,0,'5'},
    {"cdf5",no_argument,0,'5'},
    {"pnetcdf",no_argument,0,'5'},
    {"64bit_offset",no_argument,0,'6'},
    {"7",no_argument,0,'7'},
    {"average",required_argument,0,'a'},
    {"avg",required_argument,0,'a'},
    {"append",no_argument,0,'A'},
    {"mask_condition",required_argument,0,'B'},
    {"msk_cnd_sng",required_argument,0,'B'},
    {"retain-degenerate-dimensions",no_argument,0,'b'}, /* [flg] Retain degenerate dimensions */
    {"rdd",no_argument,0,'b'}, /* [flg] Retain degenerate dimensions */
    {"xtr_ass_var",no_argument,0,'c'},
    {"xcl_ass_var",no_argument,0,'C'},
    {"no_coords",no_argument,0,'C'},
    {"no_crd",no_argument,0,'C'},
    {"coords",no_argument,0,'c'},
    {"crd",no_argument,0,'c'},
    {"dbg_lvl",required_argument,0,'D'},
    {"debug",required_argument,0,'D'},
    {"nco_dbg_lvl",required_argument,0,'D'},
    {"dimension",required_argument,0,'d'},
    {"dmn",required_argument,0,'d'},
    {"fortran",no_argument,0,'F'},
    {"ftn",no_argument,0,'F'},
    {"history",no_argument,0,'h'},
    {"hst",no_argument,0,'h'},
    {"wgt_msk_crd_var",no_argument,0,'I'},
    {"dfl_lvl",required_argument,0,'L'}, /* [enm] Deflate level */
    {"deflate",required_argument,0,'L'}, /* [enm] Deflate level */
    {"local",required_argument,0,'l'},
    {"lcl",required_argument,0,'l'},
    {"mask-variable",required_argument,0,'m'},
    {"mask_variable",required_argument,0,'m'},
    {"mask",required_argument,0,'m'},
    {"msk_var",required_argument,0,'m'},
    {"msk_nm",required_argument,0,'m'},
    {"mask-value",required_argument,0,'M'},
    {"mask_value",required_argument,0,'M'},
    {"msk_val",required_argument,0,'M'},
    {"nintap",required_argument,0,'n'},
    {"nmr",no_argument,0,'N'},
    {"numerator",no_argument,0,'N'},
    {"overwrite",no_argument,0,'O'},
    {"ovr",no_argument,0,'O'},
    {"output",required_argument,0,'o'},
    {"fl_out",required_argument,0,'o'},
    {"path",required_argument,0,'p'},
    {"retain",no_argument,0,'R'},
    {"rtn",no_argument,0,'R'},
    {"revision",no_argument,0,'r'},
    {"suspend", no_argument,0,'S'},
    {"mask_comparator",required_argument,0,'T'},
    {"msk_cmp_typ",required_argument,0,'T'},
    {"op_rlt",required_argument,0,'T'},
    {"thr_nbr",required_argument,0,'t'},
    {"threads",required_argument,0,'t'},
    {"omp_num_threads",required_argument,0,'t'},
    {"variable",required_argument,0,'v'},
    {"normalize-by-tally",no_argument,0,'W',},
    {"exclude",no_argument,0,'x'},
    {"xcl",no_argument,0,'x'},
    {"weight",no_argument,0,'w'},
    {"wgt",no_argument,0,'w'},
    {"wgt_var",no_argument,0,'w'},
    {"operation",required_argument,0,'y'},
    {"op_typ",required_argument,0,'y'},
    {"help",no_argument,0,'?'},
    {"hlp",no_argument,0,'?'},
    {0,0,0,0}
  }; /* end opt_lng */
  int opt_idx=0; /* Index of current long option into opt_lng array */
  
#ifdef ENABLE_MPI
  /* MPI Initialization */
  MPI_Init(&argc,&argv);
  MPI_Comm_size(MPI_COMM_WORLD,&prc_nbr);
  MPI_Comm_rank(MPI_COMM_WORLD,&prc_rnk);
#endif /* !ENABLE_MPI */
  
  /* Start timer and save command line */ 
  ddra_info.tmr_flg=nco_tmr_srt;
  rcd+=nco_ddra((char *)NULL,(char *)NULL,&ddra_info);
  ddra_info.tmr_flg=nco_tmr_mtd;
  cmd_ln=nco_cmd_ln_sng(argc,argv);
  
  NORMALIZE_BY_TALLY=NORMALIZE_BY_TALLY; /* CEWI: Avert compiler warning that variable is set but never used */
  NORMALIZE_BY_WEIGHT=NORMALIZE_BY_WEIGHT; /* CEWI: Avert compiler warning that variable is set but never used */
  
  /* Get program name and set program enum (e.g., nco_prg_id=ncra) */
  nco_prg_nm=nco_prg_prs(argv[0],&nco_prg_id);
  
  /* Parse command line arguments */
  while(1){
    /* getopt_long_only() allows one dash to prefix long options */
    opt=getopt_long(argc,argv,opt_sht_lst,opt_lng,&opt_idx);
    /* NB: access to opt_crr is only valid when long_opt is detected */
    if(opt == EOF) break; /* Parse positional arguments once getopt_long() returns EOF */
    opt_crr=(char *)strdup(opt_lng[opt_idx].name);
    
    /* Process long options without short option counterparts */
    if(opt == 0){
      if(!strcmp(opt_crr,"bfr_sz_hnt") || !strcmp(opt_crr,"buffer_size_hint")){
	bfr_sz_hnt=strtoul(optarg,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
	if(*sng_cnv_rcd) nco_sng_cnv_err(optarg,"strtoul",sng_cnv_rcd);
      } /* endif cnk */
      if(!strcmp(opt_crr,"cnk_byt") || !strcmp(opt_crr,"chunk_byte")){
        cnk_sz_byt=strtoul(optarg,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
        if(*sng_cnv_rcd) nco_sng_cnv_err(optarg,"strtoul",sng_cnv_rcd);
      } /* endif cnk_byt */
      if(!strcmp(opt_crr,"cnk_min") || !strcmp(opt_crr,"chunk_min")){
        cnk_min_byt=strtoul(optarg,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
        if(*sng_cnv_rcd) nco_sng_cnv_err(optarg,"strtoul",sng_cnv_rcd);
      } /* endif cnk_min */
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
      if(!strcmp(opt_crr,"mmr_cln") || !strcmp(opt_crr,"clean")) flg_mmr_cln=True; /* [flg] Clean memory prior to exit */
      if(!strcmp(opt_crr,"drt") || !strcmp(opt_crr,"mmr_drt") || !strcmp(opt_crr,"dirty")) flg_mmr_cln=False; /* [flg] Clean memory prior to exit */
      if(!strcmp(opt_crr,"ddra") || !strcmp(opt_crr,"mdl_cmp")) ddra_info.flg_ddra=flg_ddra=True; /* [flg] DDRA diagnostics */
      if(!strcmp(opt_crr,"fl_fmt") || !strcmp(opt_crr,"file_format")) rcd=nco_create_mode_prs(optarg,&fl_out_fmt);
      if(!strcmp(opt_crr,"gaa") || !strcmp(opt_crr,"glb_att_add")){
        gaa_arg=(char **)nco_realloc(gaa_arg,(gaa_nbr+1)*sizeof(char *));
        gaa_arg[gaa_nbr++]=(char *)strdup(optarg);
      } /* endif gaa */
      if(!strcmp(opt_crr,"hdr_pad") || !strcmp(opt_crr,"header_pad")){
        hdr_pad=strtoul(optarg,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
        if(*sng_cnv_rcd) nco_sng_cnv_err(optarg,"strtoul",sng_cnv_rcd);
      } /* endif "hdr_pad" */
      if(!strcmp(opt_crr,"log_lvl") || !strcmp(opt_crr,"log_level")){
	log_lvl=(int)strtol(optarg,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
	if(*sng_cnv_rcd) nco_sng_cnv_err(optarg,"strtol",sng_cnv_rcd);
	nc_set_log_level(log_lvl);
      } /* !log_lvl */
      if(!strcmp(opt_crr,"ram_all") || !strcmp(opt_crr,"create_ram") || !strcmp(opt_crr,"diskless_all")) RAM_CREATE=True; /* [flg] Create (netCDF3) file(s) in RAM */
      if(!strcmp(opt_crr,"ram_all") || !strcmp(opt_crr,"open_ram") || !strcmp(opt_crr,"diskless_all")) RAM_OPEN=True; /* [flg] Open (netCDF3) file(s) in RAM */
      if(!strcmp(opt_crr,"share_all") || !strcmp(opt_crr,"unbuffered_io") || !strcmp(opt_crr,"uio") || !strcmp(opt_crr,"create_share")) SHARE_CREATE=True; /* [flg] Create (netCDF3) file(s) with unbuffered I/O */
      if(!strcmp(opt_crr,"share_all") || !strcmp(opt_crr,"unbuffered_io") || !strcmp(opt_crr,"uio") || !strcmp(opt_crr,"open_share")) SHARE_OPEN=True; /* [flg] Open (netCDF3) file(s) with unbuffered I/O */
      if(!strcmp(opt_crr,"vrs") || !strcmp(opt_crr,"version")){
	(void)nco_vrs_prn(CVS_Id,CVS_Revision);
	nco_exit(EXIT_SUCCESS);
      } /* endif "vrs" */
      if(!strcmp(opt_crr,"wrt_tmp_fl") || !strcmp(opt_crr,"write_tmp_fl")) WRT_TMP_FL=True;
      if(!strcmp(opt_crr,"no_tmp_fl")) WRT_TMP_FL=False;
    } /* opt != 0 */
    /* Process short options */
    switch(opt){
    case 0: /* Long options have already been processed, return */
      break;
    case '3': /* Request netCDF3 output storage format */
      fl_out_fmt=NC_FORMAT_CLASSIC;
      break;
    case '4': /* Request netCDF4 output storage format */
      fl_out_fmt=NC_FORMAT_NETCDF4; 
      break;
    case '5': /* Request netCDF3 64-bit offset+data storage (i.e., pnetCDF) format */
      fl_out_fmt=NC_FORMAT_CDF5;
      break;
    case '6': /* Request netCDF3 64-bit offset output storage format */
      fl_out_fmt=NC_FORMAT_64BIT;
      break;
    case '7': /* Request netCDF4-classic output storage format */
      fl_out_fmt=NC_FORMAT_NETCDF4_CLASSIC;
      break;
    case 'A': /* Toggle FORCE_APPEND */
      FORCE_APPEND=!FORCE_APPEND;
      break;
    case 'a': /* Dimensions over which to average hyperslab */
      if(flg_opt_a){
	(void)fprintf(fp_stdout,"%s: ERROR Option -a appears more than once\n",nco_prg_nm);
	(void)fprintf(fp_stdout,"%s: HINT Use -a dim1,dim2,... not -a dim1 -a dim2 ...\n",nco_prg_nm);
	(void)nco_usg_prn();
	nco_exit(EXIT_FAILURE);
      } /* endif */
      dmn_avg_lst_in=nco_lst_prs_2D(optarg,",",&dmn_avg_nbr);
      flg_opt_a=True;
      break;
    case 'B': /* Mask string to be parsed */
      msk_cnd_sng=(char *)strdup(optarg);
      break;
    case 'b': /* [flg] Retain degenerate dimensions */
      flg_rdd=True;
      break;
    case 'C': /* Extract all coordinates associated with extracted variables? */
      EXTRACT_ASSOCIATED_COORDINATES=False;
      break;
    case 'c':
      EXTRACT_ALL_COORDINATES=True;
      break;
    case 'D': /* Debugging level. Default is 0. */
      nco_dbg_lvl=(unsigned short int)strtoul(optarg,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
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
    case 'I': /* [flg] Weight and/or mask coordinate variables */
      WGT_MSK_CRD_VAR=!WGT_MSK_CRD_VAR;
      break;
    case 'L': /* [enm] Deflate level. Default is 0. */
      dfl_lvl=(int)strtol(optarg,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
      if(*sng_cnv_rcd) nco_sng_cnv_err(optarg,"strtol",sng_cnv_rcd);
      break;
    case 'l': /* Local path prefix for files retrieved from remote file system */
      fl_pth_lcl=(char *)strdup(optarg);
      break;
    case 'm': /* Name of variable to use as mask in reducing. Default is none */
      msk_nm=(char *)strdup(optarg);
      break;
    case 'M': /* Good data defined by relation to mask value. Default is 1. */
      msk_val=strtod(optarg,&sng_cnv_rcd);
      if(*sng_cnv_rcd) nco_sng_cnv_err(optarg,"strtod",sng_cnv_rcd);
      break;
    case 'N':
      NRM_BY_DNM=False;
      NORMALIZE_BY_TALLY=False;
      NORMALIZE_BY_WEIGHT=False;
      break;
    case 'n':
      NORMALIZE_BY_WEIGHT=False;
      (void)fprintf(fp_stdout,"%s: ERROR This option has been disabled while I rethink its implementation\n",nco_prg_nm);
      nco_exit(EXIT_FAILURE);
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
      RM_RMT_FL_PST_PRC=!RM_RMT_FL_PST_PRC;
      break;
    case 'r': /* Print CVS program information and copyright notice */
      (void)nco_vrs_prn(CVS_Id,CVS_Revision);
      (void)nco_lbr_vrs_prn();
      (void)nco_cpy_prn();
      (void)nco_cnf_prn();
      nco_exit(EXIT_SUCCESS);
      break;
#ifdef ENABLE_MPI
    case 'S': /* Suspend with signal handler to facilitate debugging */
      if(signal(SIGUSR1,nco_cnt_run) == SIG_ERR) (void)fprintf(fp_stdout,"%s: ERROR Could not install suspend handler.\n",nco_prg_nm);
      while(!nco_spn_lck_brk) usleep(nco_spn_lck_us); /* Spinlock. fxm: should probably insert a sched_yield */
      break;
#endif /* !ENABLE_MPI */
    case 'T': /* Relational operator type. Default is 0, eq, equality */
      op_typ_rlt=nco_op_prs_rlt(optarg);
      break;
    case 't': /* Thread number */
      thr_nbr=(int)strtol(optarg,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
      if(*sng_cnv_rcd) nco_sng_cnv_err(optarg,"strtol",sng_cnv_rcd);
      break;
    case 'v': /* Variables to extract/exclude */
      /* Replace commas with hashes when within braces (convert back later) */
      optarg_lcl=(char *)strdup(optarg);
      (void)nco_rx_comma2hash(optarg_lcl);
      var_lst_in=nco_lst_prs_2D(optarg_lcl,",",&var_lst_in_nbr);
      optarg_lcl=(char *)nco_free(optarg_lcl);
      xtr_nbr=var_lst_in_nbr;
      break;
    case 'W':
      NORMALIZE_BY_TALLY=False;
      (void)fprintf(fp_stdout,"%s: ERROR This option has been disabled while I rethink its implementation\n",nco_prg_nm);
      nco_exit(EXIT_FAILURE);
      break;
    case 'w': /* Variable to use as weight in reducing.  Default is none */
      wgt_nm=(char *)strdup(optarg);
      break;
    case 'x': /* Exclude rather than extract variables specified with -v */
      EXCLUDE_INPUT_LIST=True;
      break;
    case 'y': /* Operation type */
      nco_op_typ_sng=(char *)strdup(optarg);
      nco_op_typ=nco_op_typ_get(nco_op_typ_sng);
      break;
    case '?': /* Print proper usage */
      (void)nco_usg_prn();
      nco_exit(EXIT_SUCCESS);
      break;
    case '-': /* Long options are not allowed */
      (void)fprintf(stderr,"%s: ERROR Long options are not available in this build. Use single letter options instead.\n",nco_prg_nm_get());
      nco_exit(EXIT_FAILURE);
      break;
    default: /* Print proper usage */
      (void)fprintf(stdout,"%s ERROR in command-line syntax/options. Please reformulate command accordingly.\n",nco_prg_nm_get());
      (void)nco_usg_prn();
      nco_exit(EXIT_FAILURE);
      break;
    } /* end switch */
    if(opt_crr) opt_crr=(char *)nco_free(opt_crr);
  } /* end while loop */
  
  /* Parse mask string */
  if(msk_cnd_sng){
    int cst_zero=0;
    /* Set arguments for scan */
    prs_arg.fl_in=NULL; /* [sng] Input data file */
    prs_arg.in_id=0; /* [id] Input data file ID */
    prs_arg.fl_out=NULL; /* [sng] Output data file */
    prs_arg.out_id=0; /* [id] Output data file ID */
    prs_arg.att_lst=NULL; /* [sct] Attributes in script */
    prs_arg.nbr_att=&cst_zero; /* [nbr] Number of attributes in script */
    prs_arg.dmn_in=NULL; /* [dmn_in] List of all dimensions in input */
    prs_arg.nbr_dmn_in=0; /* [nbr] Number of  dimensions in input */
    prs_arg.dmn_out=NULL; /* [sct] Pointer to output dimension list */
    prs_arg.nbr_dmn_out=&cst_zero; /* [nbr] Number of dimensions in output list */
    prs_arg.sym_tbl=NULL; /* [fnc] Symbol table for functions */
    prs_arg.sym_tbl_nbr=0; /* [nbr] Number of functions in table */
    prs_arg.ntl_scn=False; /* [flg] Initial scan of script */
    prs_arg.var_LHS=NULL; /* [var] LHS cast variable */
    prs_arg.nco_op_typ=nco_op_nil; /* [enm] Operation type */
    
    /* Initialize line counter */
    ncap_ln_nbr_crr=(size_t *)nco_realloc(ncap_ln_nbr_crr,ncap_ncl_dpt_crr+1UL); 
    ncap_ln_nbr_crr[ncap_ncl_dpt_crr]=1UL; /* [cnt] Line number incremented in ncap.l */
    if(ncap_ncwa_scn(&prs_arg,msk_cnd_sng,&msk_nm,&msk_val,&op_typ_rlt) == 0) nco_exit(EXIT_FAILURE); 
  } /* endif msk_cnd_sng */
  
  /* Ensure we do not attempt to normalize by non-existent weight */
  if(wgt_nm == NULL) NORMALIZE_BY_WEIGHT=False;
  
  /* Process positional arguments and fill-in filenames */
  fl_lst_in=nco_fl_lst_mk(argv,argc,optind,&fl_nbr,&fl_out,&FL_LST_IN_FROM_STDIN,FORCE_OVERWRITE);
  
  /* Make uniform list of user-specified chunksizes */
  if(cnk_nbr > 0) cnk_dmn=nco_cnk_prs(cnk_nbr,cnk_arg);
  
  /* Make uniform list of user-specified dimension limits */
  lmt=nco_lmt_prs(lmt_nbr,lmt_arg);
  
  /* Initialize thread information */
  thr_nbr=nco_openmp_ini(thr_nbr);
  in_id_arr=(int *)nco_malloc(thr_nbr*sizeof(int));
  
  /* Parse filename */
  fl_in=nco_fl_nm_prs(fl_in,0,&fl_nbr,fl_lst_in,abb_arg_nbr,fl_lst_abb,fl_pth);
  /* Make sure file is on local system and is readable or die trying */
  fl_in=nco_fl_mk_lcl(fl_in,fl_pth_lcl,HPSS_TRY,&FL_RTR_RMT_LCN);
  /* Open file using appropriate buffer size hints and verbosity */
  if(RAM_OPEN) md_open=NC_NOWRITE|NC_DISKLESS; else md_open=NC_NOWRITE;
  if(SHARE_OPEN) md_open=md_open|NC_SHARE;
  rcd+=nco_fl_open(fl_in,md_open,&bfr_sz_hnt,&in_id);
  
  /* Get number of variables, dimensions, and record dimension ID of input file */
  (void)nco_inq(in_id,&nbr_dmn_fl,&nbr_var_fl,(int *)NULL,&rec_dmn_id);
  (void)nco_inq_format(in_id,&fl_in_fmt);
  
  /* Form initial extraction list which may include extended regular expressions */
  xtr_lst=nco_var_lst_mk(in_id,nbr_var_fl,var_lst_in,EXCLUDE_INPUT_LIST,EXTRACT_ALL_COORDINATES,&xtr_nbr);
  
  /* Change included variables to excluded variables */
  if(EXCLUDE_INPUT_LIST) xtr_lst=nco_var_lst_xcl(in_id,nbr_var_fl,xtr_lst,&xtr_nbr);
  
  /* Determine conventions (ARM/CCM/CCSM/CF/MPAS) for treating file */
  cnv=nco_cnv_ini(in_id);
  
  /* Add all coordinate variables to extraction list */
  if(EXTRACT_ALL_COORDINATES) xtr_lst=nco_var_lst_crd_add(in_id,nbr_dmn_fl,nbr_var_fl,xtr_lst,&xtr_nbr,cnv);
  
  /* Extract coordinates associated with extracted variables */
  if(EXTRACT_ASSOCIATED_COORDINATES) xtr_lst=nco_var_lst_crd_ass_add(in_id,xtr_lst,&xtr_nbr,cnv);
  
  /* Sort extraction list by variable ID for fastest I/O */
  if(xtr_nbr > 1) xtr_lst=nco_lst_srt_nm_id(xtr_lst,xtr_nbr,False);
  
  /* Find coordinate/dimension values associated with user-specified limits
     NB: nco_lmt_evl() with same nc_id contains OpenMP critical region */
  for(idx=0;idx<lmt_nbr;idx++) (void)nco_lmt_evl(in_id,lmt[idx],0L,FORTRAN_IDX_CNV);
  
  /* Find dimensions associated with variables to be extracted */
  dmn_lst=nco_dmn_lst_ass_var(in_id,xtr_lst,xtr_nbr,&nbr_dmn_xtr);
  
  /* Fill-in dimension structure for all extracted dimensions */
  dim=(dmn_sct **)nco_malloc(nbr_dmn_xtr*sizeof(dmn_sct *));
  for(idx=0;idx<nbr_dmn_xtr;idx++) dim[idx]=nco_dmn_fll(in_id,dmn_lst[idx].id,dmn_lst[idx].nm);
  
  /* Merge hyperslab limit information into dimension structures */
  if(lmt_nbr > 0) (void)nco_dmn_lmt_mrg(dim,nbr_dmn_xtr,lmt,lmt_nbr);
  
  /* Not specifying any dimensions is interpreted as specifying all dimensions */
  if(dmn_avg_nbr == 0){
    dmn_avg_nbr=nbr_dmn_xtr;
    dmn_avg_lst_in=(char **)nco_malloc(dmn_avg_nbr*sizeof(char *));
    for(idx=0;idx<dmn_avg_nbr;idx++){
      dmn_avg_lst_in[idx]=(char *)strdup(dmn_lst[idx].nm);
    } /* end loop over idx */
    (void)fprintf(stderr,"%s: INFO No dimensions specified with -a, therefore reducing (averaging, taking minimum, etc.) over all dimensions\n",nco_prg_nm);
  } /* end if dmn_avg_nbr == 0 */
  /* Dimension list no longer needed */
  dmn_lst=nco_nm_id_lst_free(dmn_lst,nbr_dmn_xtr);
  
  if(dmn_avg_nbr > 0){
    if(dmn_avg_nbr > nbr_dmn_xtr){
      (void)fprintf(fp_stdout,"%s: ERROR More reducing dimensions than extracted dimensions\n",nco_prg_nm);
      nco_exit(EXIT_FAILURE);
    } /* end if */
    
    /* Create structured list of reducing dimension names and IDs */
    dmn_avg_lst=nco_dmn_lst_mk(in_id,dmn_avg_lst_in,dmn_avg_nbr);
    /* Dimension average list no longer needed */
    if(dmn_avg_nbr > 0) dmn_avg_lst_in=nco_sng_lst_free(dmn_avg_lst_in,dmn_avg_nbr);
    
    /* Form list of reducing dimensions from extracted input dimensions */
    dmn_avg=(dmn_sct **)nco_malloc(dmn_avg_nbr*sizeof(dmn_sct *));
    for(idx_avg=0;idx_avg<dmn_avg_nbr;idx_avg++){
      for(idx=0;idx<nbr_dmn_xtr;idx++){
	if(!strcmp(dmn_avg_lst[idx_avg].nm,dim[idx]->nm)) break;
      } /* end loop over idx_avg */
      if(idx != nbr_dmn_xtr){
	dmn_avg[idx_avg]=dim[idx];
      }else{
	(void)fprintf(stderr,"%s: WARNING reducing dimension \"%s\" is not contained in any variable in extraction list\n",nco_prg_nm,dmn_avg_lst[idx_avg].nm);
	/* Collapse dimension average list by omitting irrelevent dimension */
	(void)memmove(dmn_avg_lst+idx_avg*sizeof(nm_id_sct),dmn_avg_lst+(idx_avg+1)*sizeof(nm_id_sct),(dmn_avg_nbr-idx_avg-1)*sizeof(nm_id_sct));
	--dmn_avg_nbr;
	dmn_avg_lst=(nm_id_sct *)nco_realloc(dmn_avg_lst,dmn_avg_nbr*sizeof(nm_id_sct));
	dmn_avg=(dmn_sct **)nco_realloc(dmn_avg,dmn_avg_nbr*sizeof(dmn_sct *)); 
      } /* end else */
    } /* end loop over idx_avg */
    
    /* Make sure no reducing dimension is specified more than once */
    for(idx=0;idx<dmn_avg_nbr;idx++){
      for(idx_avg=0;idx_avg<dmn_avg_nbr;idx_avg++){
	if(idx_avg != idx){
	  if(dmn_avg[idx]->id == dmn_avg[idx_avg]->id){
	    (void)fprintf(fp_stdout,"%s: ERROR %s specified more than once in reducing list\n",nco_prg_nm,dmn_avg[idx]->nm);
	    nco_exit(EXIT_FAILURE);
	  } /* end if */
	} /* end if */
      } /* end loop over idx_avg */
    } /* end loop over idx */
    
    /* Averaged dimensions appear in output file iff flg_rdd is set */
    dmn_out=(dmn_sct **)nco_malloc((flg_rdd ? nbr_dmn_xtr : nbr_dmn_xtr-dmn_avg_nbr)*sizeof(dmn_sct *));
    nbr_dmn_out=0;
    for(idx=0;idx<nbr_dmn_xtr;idx++){
      for(idx_avg=0;idx_avg<dmn_avg_nbr;idx_avg++){
	if(!strcmp(dmn_avg_lst[idx_avg].nm,dim[idx]->nm)) break;
      } /* end loop over idx_avg */
      if(idx_avg == dmn_avg_nbr || flg_rdd){
	/* Output list comprises non-averaged and, if specified, degenerate dimensions */
	dmn_out[nbr_dmn_out]=nco_dmn_dpl(dim[idx]);
	(void)nco_dmn_xrf(dim[idx],dmn_out[nbr_dmn_out]);
	if(idx_avg != dmn_avg_nbr && flg_rdd){
	  /* Cut degenerate dimensions down to size */
	  dmn_out[nbr_dmn_out]->cnt=1L;
	  dmn_out[nbr_dmn_out]->srt=dmn_out[nbr_dmn_out]->end=0L;
	} /* !flg_rdd */
	nbr_dmn_out++;
      } /* end if idx_avg */
    } /* end loop over idx_xtr */
    /* Dimension average list no longer needed */
    dmn_avg_lst=nco_nm_id_lst_free(dmn_avg_lst,dmn_avg_nbr);
    
    if(nbr_dmn_out != (flg_rdd ? nbr_dmn_xtr : nbr_dmn_xtr-dmn_avg_nbr)){
      (void)fprintf(fp_stdout,"%s: ERROR nbr_dmn_out != %s\n",nco_prg_nm,(flg_rdd) ? "nbr_dmn_xtr" : "nbr_dmn_xtr-dmn_avg_nbr");
      nco_exit(EXIT_FAILURE);
    } /* end if */
    
  } /* dmn_avg_nbr <= 0 */
  
  /* Fill-in variable structure list for all extracted variables */
  var=(var_sct **)nco_malloc(xtr_nbr*sizeof(var_sct *));
  var_out=(var_sct **)nco_malloc(xtr_nbr*sizeof(var_sct *));
  for(idx=0;idx<xtr_nbr;idx++){
    var[idx]=nco_var_fll(in_id,xtr_lst[idx].id,xtr_lst[idx].nm,dim,nbr_dmn_xtr);
    var_out[idx]=nco_var_dpl(var[idx]);
    (void)nco_xrf_var(var[idx],var_out[idx]);
    (void)nco_xrf_dmn(var_out[idx]);
  } /* end loop over idx */
  /* Extraction list no longer needed */
  xtr_lst=nco_nm_id_lst_free(xtr_lst,xtr_nbr);
  
  /* Divide variable lists into lists of fixed variables and variables to be processed */
  (void)nco_var_lst_dvd(var,var_out,xtr_nbr,cnv,True,nco_pck_plc_nil,nco_pck_map_nil,dmn_avg,dmn_avg_nbr,&var_fix,&var_fix_out,&nbr_var_fix,&var_prc,&var_prc_out,&nbr_var_prc);
  
  /* We now have final list of variables to extract. Phew. */
  if(nco_dbg_lvl >= nco_dbg_var){
    for(idx=0;idx<xtr_nbr;idx++) (void)fprintf(stderr,"var[%d]->nm = %s, ->id=[%d]\n",idx,var[idx]->nm,var[idx]->id);
    for(idx=0;idx<nbr_var_fix;idx++) (void)fprintf(stderr,"var_fix[%d]->nm = %s, ->id=[%d]\n",idx,var_fix[idx]->nm,var_fix[idx]->id);
    for(idx=0;idx<nbr_var_prc;idx++) (void)fprintf(stderr,"var_prc[%d]->nm = %s, ->id=[%d]\n",idx,var_prc[idx]->nm,var_prc[idx]->id);
  } /* end if */
  
#ifdef ENABLE_MPI
  if(prc_rnk == rnk_mgr){ /* MPI manager code */
#endif /* !ENABLE_MPI */
    /* Make output and input files consanguinous */
    if(fl_out_fmt == NCO_FORMAT_UNDEFINED) fl_out_fmt=fl_in_fmt;
    
    /* Verify output file format supports requested actions */
    (void)nco_fl_fmt_vet(fl_out_fmt,cnk_nbr,dfl_lvl);
    
    /* Open output file */
    fl_out_tmp=nco_fl_out_open(fl_out,&FORCE_APPEND,FORCE_OVERWRITE,fl_out_fmt,&bfr_sz_hnt,RAM_CREATE,RAM_OPEN,SHARE_CREATE,SHARE_OPEN,WRT_TMP_FL,&out_id);
    if(nco_dbg_lvl >= nco_dbg_sbr) (void)fprintf(stderr,"Input, output file IDs = %d, %d\n",in_id,out_id);
    
    /* Copy all global attributes */
    (void)nco_att_cpy(in_id,out_id,NC_GLOBAL,NC_GLOBAL,(nco_bool)True);
    
    /* Catenate time-stamped command line to "history" global attribute */
    if(HISTORY_APPEND) (void)nco_hst_att_cat(out_id,cmd_ln);
    if(HISTORY_APPEND && FORCE_APPEND) (void)nco_prv_att_cat(fl_in,in_id,out_id);
    if(gaa_nbr > 0) (void)nco_glb_att_add(out_id,gaa_arg,gaa_nbr);
    if(HISTORY_APPEND) (void)nco_vrs_att_cat(out_id);
    
    if(thr_nbr > 1 && HISTORY_APPEND) (void)nco_thr_att_cat(out_id,thr_nbr);
    
#ifdef ENABLE_MPI
    /* Initialize MPI task information */
    if(prc_nbr > 0 && HISTORY_APPEND) (void)nco_mpi_att_cat(out_id,prc_nbr);
#endif /* !ENABLE_MPI */
    
    /* Define dimensions in output file */
    (void)nco_dmn_dfn(fl_out,out_id,dmn_out,nbr_dmn_out);
    
    /* Define variables in output file, copy their attributes */
    (void)nco_var_dfn(in_id,fl_out,out_id,var_out,xtr_nbr,dmn_out,nbr_dmn_out,nco_pck_plc_nil,nco_pck_map_nil,dfl_lvl);
    
#ifdef ENABLE_MPI
  } /* prc_rnk != rnk_mgr */
#endif /* !ENABLE_MPI */
  
  /* Add new missing values to output file while in define mode */
  if(msk_nm){
    for(idx=0;idx<nbr_var_prc;idx++){
      /* Define for var_prc_out because mss_val for var_prc will be overwritten in nco_var_mtd_refresh */
      if(!var_prc_out[idx]->has_mss_val){
	var_prc_out[idx]->has_mss_val=True;
	var_prc_out[idx]->mss_val=nco_mss_val_mk(var_prc[idx]->type);
#ifdef ENABLE_MPI
	if(prc_rnk == rnk_mgr)
#endif /* !ENABLE_MPI */
	  (void)nco_put_att(out_id,var_prc_out[idx]->id,nco_mss_val_sng_get(),var_prc_out[idx]->type,1,var_prc_out[idx]->mss_val.vp);
      } /* end if */
    } /* end for */
  } /* end if */
  
#ifdef ENABLE_MPI
  if(prc_rnk == rnk_mgr){ /* MPI manager code */
#endif /* !ENABLE_MPI */
    
    /* Set chunksize parameters */
    if(fl_out_fmt == NC_FORMAT_NETCDF4 || fl_out_fmt == NC_FORMAT_NETCDF4_CLASSIC) (void)nco_cnk_sz_set(out_id,lmt_all_lst,nbr_dmn_fl,&cnk_map,&cnk_plc,cnk_sz_scl,cnk_dmn,cnk_nbr);
    
    /* Turn-off default filling behavior to enhance efficiency */
    nco_set_fill(out_id,NC_NOFILL,&fll_md_old);
    
    /* Take output file out of define mode */
    if(hdr_pad == 0UL){
      (void)nco_enddef(out_id);
    }else{
      (void)nco__enddef(out_id,hdr_pad);
      if(nco_dbg_lvl >= nco_dbg_scl) (void)fprintf(stderr,"%s: INFO Padding header with %lu extra bytes\n",nco_prg_nm_get(),(unsigned long)hdr_pad);
    } /* hdr_pad */
    
#ifdef ENABLE_MPI
  } /* prc_rnk != rnk_mgr */
  
  /* Manager obtains output filename and broadcasts to workers */ 
  if(prc_rnk == rnk_mgr) fl_nm_lng=(int)strlen(fl_out_tmp); 
  MPI_Bcast(&fl_nm_lng,1,MPI_INT,0,MPI_COMM_WORLD);
  if(prc_rnk != rnk_mgr) fl_out_tmp=(char *)nco_malloc((fl_nm_lng+1)*sizeof(char));
  MPI_Bcast(fl_out_tmp,fl_nm_lng+1,MPI_CHAR,0,MPI_COMM_WORLD);
  
  if(prc_rnk == rnk_mgr){ /* MPI manager code */
    TKN_WRT_FREE=False;
#endif /* !ENABLE_MPI */
    /* Copy variable data for non-processed variables */
    (void)nco_var_val_cpy(in_id,out_id,var_fix,nbr_var_fix);
#ifdef ENABLE_MPI
    /* Close output file so workers can open it */
    nco_close(out_id);
    TKN_WRT_FREE=True;
  } /* prc_rnk != rnk_mgr */
#endif /* !ENABLE_MPI */
  
  /* Assign zero to start and unity to stride vectors in output variables */
  (void)nco_var_srd_srt_set(var_out,xtr_nbr);
  
  /* Close first input netCDF file */
  nco_close(in_id);
  
  /* Loop over input files (not currently used, fl_nbr == 1) */
  for(fl_idx=0;fl_idx<fl_nbr;fl_idx++){
    /* Parse filename */
    if(fl_idx != 0) fl_in=nco_fl_nm_prs(fl_in,fl_idx,&fl_nbr,fl_lst_in,abb_arg_nbr,fl_lst_abb,fl_pth);
    if(nco_dbg_lvl >= nco_dbg_fl) (void)fprintf(stderr,"\nInput file %d is %s; ",fl_idx,fl_in);
    /* Make sure file is on local system and is readable or die trying */
    if(fl_idx != 0) fl_in=nco_fl_mk_lcl(fl_in,fl_pth_lcl,HPSS_TRY,&FL_RTR_RMT_LCN);
    if(nco_dbg_lvl >= nco_dbg_fl) (void)fprintf(stderr,"local file %s:\n",fl_in);
    
    /* Open file once per thread to improve caching */
    for(thr_idx=0;thr_idx<thr_nbr;thr_idx++) rcd=nco_fl_open(fl_in,md_open,&bfr_sz_hnt,in_id_arr+thr_idx);
    in_id=in_id_arr[0];
    
    /* Perform various error-checks on input file */
    if(False) (void)nco_fl_cmp_err_chk();
    
    /* Find weighting variable in input file */
    if(wgt_nm){
      int wgt_id;
      
      rcd=nco_inq_varid(in_id,wgt_nm,&wgt_id);
      /* fxm: TODO #111 core dump if wgt has dimension not in extraction list */
      wgt=nco_var_fll(in_id,wgt_id,wgt_nm,dim,nbr_dmn_xtr);
      
      /* Retrieve weighting variable */
      /* NB: nco_var_get() with same nc_id contains OpenMP critical region */
      (void)nco_var_get(in_id,wgt);
      /* fxm: Perhaps should allocate default tally array for wgt here
	 That way, when wgt conforms to the first var_prc_out and it therefore
	 does not get a tally array copied by nco_var_dpl() in nco_var_cnf_dmn(), 
	 it will at least have space for a tally array. TODO #114. */
      
    } /* end if */
    
    /* Find mask variable in input file */
    if(msk_nm){
      int msk_id;
      
      rcd=nco_inq_varid(in_id,msk_nm,&msk_id);
      /* fxm: TODO #111 core dump if msk has dimension not in extraction list */
      msk=nco_var_fll(in_id,msk_id,msk_nm,dim,nbr_dmn_xtr);
      
      /* Retrieve mask variable */
      /* NB: nco_var_get() with same nc_id contains OpenMP critical region */
      (void)nco_var_get(in_id,msk);
    } /* end if */
    
    /* Timestamp end of metadata setup and disk layout */
    rcd+=nco_ddra((char *)NULL,(char *)NULL,&ddra_info);
    ddra_info.tmr_flg=nco_tmr_rgl;
    
#ifdef ENABLE_MPI
    if(prc_rnk == rnk_mgr){ /* MPI manager code */
      /* Compensate for incrementing on each worker's first message */
      var_wrt_nbr=-prc_nbr+1;
      idx=0;
      /* While variables remain to be processed or written... */
      while(var_wrt_nbr < nbr_var_prc){
	/* Receive message from any worker */
	MPI_Recv(wrk_id_bfr,wrk_id_bfr_lng,MPI_INT,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&mpi_stt);
	/* Obtain MPI message tag type */
	msg_tag_typ=mpi_stt.MPI_TAG;
	/* Get sender's prc_rnk */
	rnk_wrk=wrk_id_bfr[0];
	
	/* Allocate next variable, if any, to worker */
	if(msg_tag_typ == msg_tag_wrk_rqs){
	  var_wrt_nbr++; /* [nbr] Number of variables written */
	  /* Workers close output file before sending next msg_tag_wrk_rqs */
	  /* fxm csz 20050924: Safe? Can't other process have write token here? */
	  TKN_WRT_FREE=True;
	  
	  if(idx > nbr_var_prc-1){
	    msg_bfr[0]=idx_all_wrk_ass; /* [enm] All variables already assigned */
	    msg_bfr[1]=out_id; /* Output file ID */
	  }else{
	    /* Tell requesting worker to allocate space for next variable */
	    msg_bfr[0]=idx; /* [idx] Variable to be processed */
	    msg_bfr[1]=out_id; /* Output file ID */
	    msg_bfr[2]=var_prc_out[idx]->id; /* [id] Variable ID in output file */
	    /* Point to next variable on list */
	    idx++;
	  } /* endif idx */
	  MPI_Send(msg_bfr,msg_bfr_lng,MPI_INT,rnk_wrk,msg_tag_wrk_rsp,MPI_COMM_WORLD);
	  /* msg_tag_typ != msg_tag_wrk_rqs */
	}else if(msg_tag_typ == msg_tag_tkn_wrt_rqs){
	  /* Allocate token if free, else ask worker to try later */
	  if(TKN_WRT_FREE){
	    TKN_WRT_FREE=False;
	    msg_bfr[0]=tkn_wrt_rqs_xcp; /* Accept request for write token */
	  }else{
	    msg_bfr[0]=tkn_wrt_rqs_dny; /* Deny request for write token */
	  } /* !TKN_WRT_FREE */
	  MPI_Send(msg_bfr,msg_bfr_lng,MPI_INT,rnk_wrk,msg_tag_tkn_wrt_rsp,MPI_COMM_WORLD);
	} /* msg_tag_typ != msg_tag_tkn_wrt_rqs */
      } /* end while var_wrt_nbr < nbr_var_prc */
    }else{ /* prc_rnk != rnk_mgr, end Manager code begin Worker code */
      wrk_id_bfr[0]=prc_rnk;
      while(1){ /* While work remains... */
	/* Send msg_tag_wrk_rqs */
	wrk_id_bfr[0]=prc_rnk;
	MPI_Send(wrk_id_bfr,wrk_id_bfr_lng,MPI_INT,rnk_mgr,msg_tag_wrk_rqs,MPI_COMM_WORLD);
	/* Receive msg_tag_wrk_rsp */
	MPI_Recv(msg_bfr,msg_bfr_lng,MPI_INT,0,msg_tag_wrk_rsp,MPI_COMM_WORLD,&mpi_stt);
	idx=msg_bfr[0];
	out_id=msg_bfr[1];
	if(idx == idx_all_wrk_ass) break;
	else{
	  var_prc_out[idx]->id=msg_bfr[2];
	  /* Process this variable same as UP code */
#else /* !ENABLE_MPI */
#ifdef _OPENMP
	  /* OpenMP notes:
	     firstprivate(): msk_out and wgt_out must be NULL on first call to nco_var_cnf_dmn()
	     shared(): msk and wgt are not altered within loop
	     private(): wgt_avg does not need initialization */
#pragma omp parallel for default(none) firstprivate(DO_CONFORM_MSK,DO_CONFORM_WGT,ddra_info,msk_out,wgt_out) private(idx,in_id,wgt_avg) shared(MULTIPLY_BY_TALLY,MUST_CONFORM,NRM_BY_DNM,WGT_MSK_CRD_VAR,nco_dbg_lvl,dmn_avg,dmn_avg_nbr,flg_ddra,flg_rdd,in_id_arr,msk,msk_nm,msk_val,nbr_var_prc,nco_op_typ,op_typ_rlt,out_id,nco_prg_nm,rcd,var_prc,var_prc_out,wgt,wgt_nm)
#endif /* !_OPENMP */
	  /* UP and SMP codes main loop over variables */
	  for(idx=0;idx<nbr_var_prc;idx++){ /* Process all variables in current file */
#endif /* !ENABLE_MPI */
	    in_id=in_id_arr[omp_get_thread_num()];
	    if(nco_dbg_lvl >= nco_dbg_var) rcd+=nco_var_prc_crr_prn(idx,var_prc[idx]->nm);
	    if(nco_dbg_lvl >= nco_dbg_var) (void)fflush(fp_stderr);
	    
	    /* Allocate and, if necessary, initialize accumulation space for all processed variables */
	    var_prc_out[idx]->sz=var_prc[idx]->sz;
	    /*      if((var_prc_out[idx]->tally=var_prc[idx]->tally=(long *)nco_malloc_flg(var_prc_out[idx]->sz*sizeof(long int))) == NULL){*/
	    /* fxm: verify that var_prc->tally is not needed */
	    if((var_prc_out[idx]->tally=(long *)nco_malloc_flg(var_prc_out[idx]->sz*sizeof(long int))) == NULL){
	      (void)fprintf(fp_stdout,"%s: ERROR Unable to malloc() %ld*%ld bytes for tally buffer for variable %s in main()\n",nco_prg_nm_get(),var_prc_out[idx]->sz,(long int)sizeof(long int),var_prc_out[idx]->nm);
	      nco_exit(EXIT_FAILURE); 
	    } /* end if err */
	    (void)nco_zero_long(var_prc_out[idx]->sz,var_prc_out[idx]->tally);
	    if((var_prc_out[idx]->val.vp=(void *)nco_malloc_flg(var_prc_out[idx]->sz*nco_typ_lng(var_prc_out[idx]->type))) == NULL){
	      (void)fprintf(fp_stdout,"%s: ERROR Unable to malloc() %ld*%lu bytes for value buffer for variable %s in main()\n",nco_prg_nm_get(),var_prc_out[idx]->sz,(unsigned long)nco_typ_lng(var_prc_out[idx]->type),var_prc_out[idx]->nm);
	      nco_exit(EXIT_FAILURE); 
	    } /* end if err */
	    (void)nco_var_zero(var_prc_out[idx]->type,var_prc_out[idx]->sz,var_prc_out[idx]->val);
	    
	    (void)nco_var_mtd_refresh(in_id,var_prc[idx]);
	    /* Retrieve variable from disk into memory */
	    if(False) (void)fprintf(fp_stdout,"%s: DEBUG: fxm TODO nco354 About to nco_var_get() %s\n",nco_prg_nm,var_prc[idx]->nm);
	    /* NB: nco_var_get() with same nc_id contains OpenMP critical region */
	    (void)nco_var_get(in_id,var_prc[idx]);
	    if(False) (void)fprintf(fp_stdout,"%s: DEBUG: fxm TODO nco354 Finished nco_var_get() %s\n",nco_prg_nm,var_prc[idx]->nm);
	    
	    /* Convert char, short, long, int, and float types to doubles before arithmetic */
	    var_prc[idx]=nco_typ_cnv_rth(var_prc[idx],nco_op_typ);
	    var_prc_out[idx]=nco_typ_cnv_rth(var_prc_out[idx],nco_op_typ);
	    
	    if(msk_nm && (!var_prc[idx]->is_crd_var || WGT_MSK_CRD_VAR)){
	      msk_out=nco_var_cnf_dmn(var_prc[idx],msk,msk_out,MUST_CONFORM,&DO_CONFORM_MSK);
	      /* If msk and var did not conform then do not mask var! */
	      if(DO_CONFORM_MSK){
		msk_out=nco_var_cnf_typ(var_prc[idx]->type,msk_out);
		
		/* mss_val for var_prc has been overwritten in nco_var_mtd_refresh() */
		if(!var_prc[idx]->has_mss_val){
		  var_prc[idx]->has_mss_val=True;
		  var_prc[idx]->mss_val=nco_mss_val_mk(var_prc[idx]->type);
		} /* end if */
		
		/* Mask by changing variable to missing value where condition is false */
		(void)nco_var_msk(var_prc[idx]->type,var_prc[idx]->sz,var_prc[idx]->has_mss_val,var_prc[idx]->mss_val,msk_val,op_typ_rlt,msk_out->val,var_prc[idx]->val);
	      } /* end if */
	    } /* end if */
	    /* Perform non-linear transformations before weighting */
	    if(!var_prc[idx]->is_crd_var){
	      switch(nco_op_typ){
	      case nco_op_avgsqr: /* Square variable before weighting */
	      case nco_op_rms: /* Square variable before weighting */
	      case nco_op_rmssdn: /* Square variable before weighting */
		(void)nco_var_mlt(var_prc[idx]->type,var_prc[idx]->sz,var_prc[idx]->has_mss_val,var_prc[idx]->mss_val,var_prc[idx]->val,var_prc[idx]->val);
		break;
	      default: /* All other operations are linear, do nothing to them yet */
		break;
	      } /* end case */
	    } /* var_prc[idx]->is_crd_var */
	    if(wgt_nm && (!var_prc[idx]->is_crd_var || WGT_MSK_CRD_VAR)){
	      /* fxm: nco_var_cnf_dmn() has bug where it does not allocate tally array
		 for weights that do already conform to var_prc. TODO #114. */
	      wgt_out=nco_var_cnf_dmn(var_prc[idx],wgt,wgt_out,MUST_CONFORM,&DO_CONFORM_WGT);
	      if(DO_CONFORM_WGT){
		wgt_out=nco_var_cnf_typ(var_prc[idx]->type,wgt_out);
		/* Weight after any initial non-linear operation so, e.g., variable is squared but not weights */
		/* Weight variable by taking product of weight and variable */
		(void)nco_var_mlt(var_prc[idx]->type,var_prc[idx]->sz,var_prc[idx]->has_mss_val,var_prc[idx]->mss_val,wgt_out->val,var_prc[idx]->val);
	      } /* end if weights conformed */
	    } /* end if weight was specified and then tested for conformance */
	    /* Copy (masked) (weighted) values from var_prc to var_prc_out */
	    (void)memcpy((void *)(var_prc_out[idx]->val.vp),(void *)(var_prc[idx]->val.vp),var_prc_out[idx]->sz*nco_typ_lng(var_prc_out[idx]->type));
	    /* 20050516: fxm: destruction of var_prc_out in nco_var_avg() leaves dangling pointers in var_out? */
	    /* Reduce variable over specified dimensions (tally array is set here)
	       NB: var_prc_out[idx] is new, so corresponding var_out[idx] is dangling */
	    var_prc_out[idx]=nco_var_avg(var_prc_out[idx],dmn_avg,dmn_avg_nbr,nco_op_typ,flg_rdd,&ddra_info);
	    /* var_prc_out[idx]->val now holds numerator of averaging expression documented in NCO Users Guide
	       Denominator is also tricky due to sundry normalization options
	       These logical switches are VERY tricky---be careful modifying them */
	    if(NRM_BY_DNM && DO_CONFORM_WGT && (!var_prc[idx]->is_crd_var || WGT_MSK_CRD_VAR)){
	      /* Duplicate wgt_out as wgt_avg so that wgt_out is not contaminated by any
		 averaging operation and may be reused on next variable.
		 Free wgt_avg after each use but continue to reuse wgt_out */
	      wgt_avg=nco_var_dpl(wgt_out);
	      
	      if(var_prc[idx]->has_mss_val){
		double mss_val_dbl=double_CEWI;
		/* Set denominator to missing value at all locations where variable is missing value
		   If this is accomplished by setting weight to missing value wherever variable is missing value
		   then weight must not be re-used by next variable (which might conform but have missing values in different locations)
		   This is one good reason to copy wgt_out into disposable wgt_avg for each new variable */
		/* First, make sure wgt_avg has same missing value as variable */
		(void)nco_mss_val_cp(var_prc[idx],wgt_avg);
		/* Copy missing value into double precision variable */
		switch(wgt_avg->type){
		case NC_FLOAT: mss_val_dbl=wgt_avg->mss_val.fp[0]; break; 
		case NC_DOUBLE: mss_val_dbl=wgt_avg->mss_val.dp[0]; break; 
		case NC_INT: mss_val_dbl=wgt_avg->mss_val.ip[0]; break;
		case NC_SHORT: mss_val_dbl=wgt_avg->mss_val.sp[0]; break;
		case NC_USHORT: mss_val_dbl=wgt_avg->mss_val.usp[0]; break;
		case NC_UINT: mss_val_dbl=wgt_avg->mss_val.uip[0]; break;
		case NC_INT64: mss_val_dbl=wgt_avg->mss_val.i64p[0]; break;
		case NC_UINT64: mss_val_dbl=wgt_avg->mss_val.ui64p[0]; break;
		case NC_BYTE: mss_val_dbl=wgt_avg->mss_val.bp[0]; break;
		case NC_UBYTE: mss_val_dbl=wgt_avg->mss_val.cp[0]; break;
		case NC_CHAR: mss_val_dbl=wgt_avg->mss_val.cp[0]; break;
		case NC_STRING: break; /* Do nothing */
		default: nco_dfl_case_nc_type_err(); break;
		} /* end switch */
		/* Second, mask wgt_avg where variable is missing value */
		(void)nco_var_msk(wgt_avg->type,wgt_avg->sz,var_prc[idx]->has_mss_val,var_prc[idx]->mss_val,mss_val_dbl,nco_op_ne,var_prc[idx]->val,wgt_avg->val);
	      } /* endif weight must be checked for missing values */
	      
	      /* Free current input buffer */
	      var_prc[idx]->val.vp=nco_free(var_prc[idx]->val.vp);
	      
	      if(msk_nm && DO_CONFORM_MSK){
		/* Must mask weight in same fashion as variable was masked
		   If msk and var did not conform then do not mask wgt
		   Ensure wgt_avg has a missing value */
		if(!wgt_avg->has_mss_val){
		  wgt_avg->has_mss_val=True;
		  wgt_avg->mss_val=nco_mss_val_mk(wgt_avg->type);
		} /* end if */
		/* Mask by changing weight to missing value where condition is false */
		(void)nco_var_msk(wgt_avg->type,wgt_avg->sz,wgt_avg->has_mss_val,wgt_avg->mss_val,msk_val,op_typ_rlt,msk_out->val,wgt_avg->val);
	      } /* endif weight must be masked */
	      
	      /* fxm: temporary kludge to make sure weight has tally space
		 wgt_avg may lack valid tally array in ncwa because wgt_avg is created, 
		 sometimes, before the tally array for var_prc_out[idx] is created. 
		 When this occurs the nco_var_dpl() call in nco_var_cnf_dmn() does not copy
		 tally array into wgt_avg. See related note about this above. TODO #114.*/
	      if(wgt_avg->sz > 0)
		if((wgt_avg->tally=(long *)nco_realloc(wgt_avg->tally,wgt_avg->sz*sizeof(long int))) == NULL){
		  (void)fprintf(fp_stdout,"%s: ERROR Unable to realloc() %ld*%ld bytes for tally buffer for weight %s in main()\n",nco_prg_nm_get(),wgt_avg->sz,(long int)sizeof(long int),wgt_avg->nm);
		  nco_exit(EXIT_FAILURE); 
		} /* end if */
	      /* Average weight over specified dimensions (tally array is set here) */
	      wgt_avg=nco_var_avg(wgt_avg,dmn_avg,dmn_avg_nbr,nco_op_avg,flg_rdd,&ddra_info);
	      if(MULTIPLY_BY_TALLY){
		/* NB: Currently this is not implemented */
		/* Multiply numerator (weighted sum of variable) by tally 
		   We deviously accomplish this by dividing denominator by tally */
		(void)nco_var_nrm(wgt_avg->type,wgt_avg->sz,wgt_avg->has_mss_val,wgt_avg->mss_val,wgt_avg->tally,wgt_avg->val);
	      } /* endif */
	      /* Rather complex conditional statement is shorter than switch() */
	      if( /* Normalize by weighted tally if ....  */
		 (nco_op_typ != nco_op_min) && /* ...operation is not min() and... */
		 (nco_op_typ != nco_op_max) && /* ...operation is not max() and... */
		 (nco_op_typ != nco_op_ttl || /* ...operation is not ttl() or... */
		  var_prc[idx]->is_crd_var) /* ...variable is a coordinate */
		  ){ /* Divide numerator by masked, averaged, weights */
		(void)nco_var_dvd(var_prc_out[idx]->type,var_prc_out[idx]->sz,var_prc_out[idx]->has_mss_val,var_prc_out[idx]->mss_val,wgt_avg->val,var_prc_out[idx]->val);
	      } /* endif */
	      /* Free wgt_avg, but keep wgt_out, after each use */
	      if(wgt_avg) wgt_avg=nco_var_free(wgt_avg);
	      /* End of branch for normalization when weights were specified */
	    }else if(NRM_BY_DNM){
	      /* Branch for normalization when no weights were specified
		 Normalization is just due to tally */
	      if(var_prc[idx]->is_crd_var){
		/* Return linear averages of coordinates unless computing extrema
		   Prevent coordinate variables from encountering nco_var_nrm_sdn() */
		if((nco_op_typ != nco_op_min) && (nco_op_typ != nco_op_max)) (void)nco_var_nrm(var_prc_out[idx]->type,var_prc_out[idx]->sz,var_prc_out[idx]->has_mss_val,var_prc_out[idx]->mss_val,var_prc_out[idx]->tally,var_prc_out[idx]->val);
	      }else{ /* !var_prc[idx]->is_crd_var */
		switch(nco_op_typ){
		case nco_op_avg: /* Normalize sum by tally to create mean */
		case nco_op_sqravg: /* Normalize sum by tally to create mean */
		case nco_op_avgsqr: /* Normalize sum of squares by tally to create mean square */
		case nco_op_rms: /* Normalize sum of squares by tally to create mean square */
		case nco_op_sqrt: /* Normalize sum by tally to create mean */
		  (void)nco_var_nrm(var_prc_out[idx]->type,var_prc_out[idx]->sz,var_prc_out[idx]->has_mss_val,var_prc_out[idx]->mss_val,var_prc_out[idx]->tally,var_prc_out[idx]->val);
		  break;
		case nco_op_rmssdn: /* Normalize sum of squares by tally-1 to create mean square for sdn */
		  (void)nco_var_nrm_sdn(var_prc_out[idx]->type,var_prc_out[idx]->sz,var_prc_out[idx]->has_mss_val,var_prc_out[idx]->mss_val,var_prc_out[idx]->tally,var_prc_out[idx]->val);
		  break;
		case nco_op_min: /* Minimum is already in buffer, do nothing */
		case nco_op_max: /* Maximum is already in buffer, do nothing */	
		case nco_op_ttl: /* Total is already in buffer, do nothing */	
		  break;
		default:
		  (void)fprintf(fp_stdout,"%s: ERROR Illegal nco_op_typ in non-weighted normalization\n",nco_prg_nm);
		  nco_exit(EXIT_FAILURE);
		  break;
		} /* end switch */
	      } /* !var_prc[idx]->is_crd_var */
	    }else if(!NRM_BY_DNM){
	      /* Normalization has been turned off by user, we are done */
	      ;
	    }else{
	      (void)fprintf(fp_stdout,"%s: ERROR Unforeseen logical branch in main()\n",nco_prg_nm);
	      nco_exit(EXIT_FAILURE);
	    } /* end if */
	    /* Some non-linear operations require additional processing */
	    if(!var_prc[idx]->is_crd_var){
	      switch(nco_op_typ){
	      case nco_op_sqravg: /* Square mean to create square of the mean (for sdn) */
		(void)nco_var_mlt(var_prc_out[idx]->type,var_prc_out[idx]->sz,var_prc_out[idx]->has_mss_val,var_prc_out[idx]->mss_val,var_prc_out[idx]->val,var_prc_out[idx]->val);
		break;
	      case nco_op_sqrt: /* Take root of mean to create root mean */
	      case nco_op_rms: /* Take root of mean of sum of squares to create root mean square */
	      case nco_op_rmssdn: /* Take root of sdn mean of sum of squares to create root mean square for sdn */
		(void)nco_var_sqrt(var_prc_out[idx]->type,var_prc_out[idx]->sz,var_prc_out[idx]->has_mss_val,var_prc_out[idx]->mss_val,var_prc_out[idx]->tally,var_prc_out[idx]->val,var_prc_out[idx]->val);  
		break;
	      default:
		break;
	      } /* end switch */
	    } /* var_prc[idx]->is_crd_var */
	    /* Free tally buffer */
	    var_prc_out[idx]->tally=(long *)nco_free(var_prc_out[idx]->tally);
	    
	    /* Revert any arithmetic promotion but leave unpacked (for now) */
	    var_prc_out[idx]=nco_var_cnf_typ(var_prc_out[idx]->typ_upk,var_prc_out[idx]);
	    
#ifdef ENABLE_MPI
	    /* Obtain token and prepare to write */
	    while(1){ /* Send msg_tag_tkn_wrt_rqs repeatedly until token obtained */
	      wrk_id_bfr[0]=prc_rnk;
	      MPI_Send(wrk_id_bfr,wrk_id_bfr_lng,MPI_INT,rnk_mgr,msg_tag_tkn_wrt_rqs,MPI_COMM_WORLD);
	      MPI_Recv(msg_bfr,msg_bfr_lng,MPI_INT,rnk_mgr,msg_tag_tkn_wrt_rsp,MPI_COMM_WORLD,&mpi_stt);
	      tkn_wrt_rsp=msg_bfr[0];
	      /* Wait then re-send request */
	      if(tkn_wrt_rsp == tkn_wrt_rqs_dny) sleep(tkn_wrt_rqs_ntv); else break;
	    } /* end while loop waiting for write token */
	    
	    /* Worker has token---prepare to write */
	    if(tkn_wrt_rsp == tkn_wrt_rqs_xcp){
	      if(RAM_OPEN) md_open=NC_WRITE|NC_SHARE|NC_DISKLESS; else md_open=NC_WRITE|NC_SHARE;
	      rcd=nco_fl_open(fl_out_tmp,md_open,&bfr_sz_hnt,&out_id);
	      /* Set chunksize parameters */
	      if(fl_out_fmt == NC_FORMAT_NETCDF4 || fl_out_fmt == NC_FORMAT_NETCDF4_CLASSIC) (void)nco_cnk_sz_set(out_id,lmt_all_lst,nbr_dmn_fl,&cnk_map,&cnk_plc,cnk_sz_scl,cnk_dmn,cnk_nbr);
	      
	      /* Turn-off default filling behavior to enhance efficiency */
	      nco_set_fill(out_id,NC_NOFILL,&fll_md_old);
#endif /* !ENABLE_MPI */
	      
#ifdef _OPENMP
#pragma omp critical
#endif /* _OPENMP */
	      /* Common code for UP, SMP, and MPI */
	      { /* begin OpenMP critical */
		/* Copy average to output file then free averaging buffer */
		if(var_prc_out[idx]->nbr_dim == 0){
		  (void)nco_put_var1(out_id,var_prc_out[idx]->id,var_prc_out[idx]->srt,var_prc_out[idx]->val.vp,var_prc_out[idx]->type);
		}else{ /* end if variable is scalar */
		  (void)nco_put_vara(out_id,var_prc_out[idx]->id,var_prc_out[idx]->srt,var_prc_out[idx]->cnt,var_prc_out[idx]->val.vp,var_prc_out[idx]->type);
		} /* end if variable is array */
	      } /* end OpenMP critical */
	      
	      /* fxm: TODO nco722 DDRA diagnostics should work in MPI mode */
	      if(flg_ddra){
		/* DDRA diagnostics
		   Usage:
		   mpncwa -O -C --mdl -a lat,lon,time -w lat ~/nco/data/in.nc ~/foo.nc
		   mpncwa -O -C --mdl -a lat,lon -w lat ${DATA}/nco_bm/stl_5km.nc ~/foo.nc
		   mpncwa -O -C --mdl -a lat,lon,time -w lat ${DATA}/nco_bm/gcm_T85.nc ~/foo.nc */
		
		/* Assign remaining input for DDRA diagnostics */
		ddra_info.lmn_nbr=var_prc[idx]->sz; /* [nbr] Variable size */
		if(wgt) ddra_info.lmn_nbr_wgt=wgt->sz; /* [nbr] Weight size */
		ddra_info.nco_op_typ=nco_op_typ; /* [enm] Operation type */
		ddra_info.rnk_var=var_prc[idx]->nbr_dim; /* I [nbr] Variable rank (in input file) */
		if(wgt) ddra_info.rnk_wgt=wgt->nbr_dim; /* [nbr] Rank of weight */
		ddra_info.var_idx=idx; /* [enm] Index */
		ddra_info.wrd_sz=nco_typ_lng(var_prc[idx]->type); /* [B] Bytes per element */
		
		/* DDRA diagnostics */
		rcd+=nco_ddra /* [fnc] Count operations */
		  (var_prc[idx]->nm, /* I [sng] Variable name */
		   wgt_nm, /* I [sng] Weight name */
		   &ddra_info); /* I [sct] DDRA information */
		
	      } /* !flg_ddra */
	      
	      /* Free current output buffer */
	      var_prc_out[idx]->val.vp=nco_free(var_prc_out[idx]->val.vp);
	      
#ifdef ENABLE_MPI
	      /* Close output file and increment written counter */
	      nco_close(out_id);
	      var_wrt_nbr++;
	    } /* endif tkn_wrt_rqs_xcp */
	  } /* end else !idx_all_wrk_ass */
	} /* end while loop requesting work/token */
      } /* endif Worker */
#else /* !ENABLE_MPI */
    }  /* end (OpenMP parallel for) loop over idx */
#endif /* !ENABLE_MPI */
    
    if(nco_dbg_lvl >= nco_dbg_fl) (void)fprintf(stderr,"\n");
    
    /* Close input netCDF file */
    for(thr_idx=0;thr_idx<thr_nbr;thr_idx++) nco_close(in_id_arr[thr_idx]);
    
#ifdef ENABLE_MPI
    /* Manager moves output file (closed by workers) from temporary to permanent location */
    if(prc_rnk == rnk_mgr) (void)nco_fl_mv(fl_out_tmp,fl_out);
    if(prc_rnk == rnk_mgr) /* This if statement conditions nco_fl_rm() below */
#else /* !ENABLE_MPI */
      /* Close output file and move it from temporary to permanent location */
      (void)nco_fl_out_cls(fl_out,fl_out_tmp,out_id);
#endif /* end !ENABLE_MPI */
    /* Remove local copy of file */
    if(FL_RTR_RMT_LCN && RM_RMT_FL_PST_PRC) (void)nco_fl_rm(fl_in);
    
  } /* end loop over fl_idx */
  
  /* Clean memory unless dirty memory allowed */
  if(flg_mmr_cln){
    /* ncwa-specific memory */
    if(dmn_avg_nbr > 0) dmn_avg=(dmn_sct **)nco_free(dmn_avg);
    if(msk) msk=nco_var_free(msk);
    if(msk_nm) msk_nm=(char *)nco_free(msk_nm);
    if(msk_out) msk_out=nco_var_free(msk_out);
    if(msk_cnd_sng) msk_cnd_sng=(char *)nco_free(msk_cnd_sng);
    if(wgt) wgt=nco_var_free(wgt);
    if(wgt_avg) wgt_avg=nco_var_free(wgt_avg);
    if(wgt_nm) wgt_nm=(char *)nco_free(wgt_nm);
    if(wgt_out) wgt_out=nco_var_free(wgt_out);
    
    /* NCO-generic clean-up */
    /* Free individual strings/arrays */
    if(cmd_ln) cmd_ln=(char *)nco_free(cmd_ln);
    if(cnk_map_sng) cnk_map_sng=(char *)nco_free(cnk_map_sng);
    if(cnk_plc_sng) cnk_plc_sng=(char *)nco_free(cnk_plc_sng);
    if(fl_in) fl_in=(char *)nco_free(fl_in);
    if(fl_out) fl_out=(char *)nco_free(fl_out);
    if(fl_out_tmp) fl_out_tmp=(char *)nco_free(fl_out_tmp);
    if(fl_pth) fl_pth=(char *)nco_free(fl_pth);
    if(fl_pth_lcl) fl_pth_lcl=(char *)nco_free(fl_pth_lcl);
    if(in_id_arr) in_id_arr=(int *)nco_free(in_id_arr);
    /* Free lists of strings */
    if(fl_lst_in && fl_lst_abb == NULL) fl_lst_in=nco_sng_lst_free(fl_lst_in,fl_nbr); 
    if(fl_lst_in && fl_lst_abb) fl_lst_in=nco_sng_lst_free(fl_lst_in,1);
    if(fl_lst_abb) fl_lst_abb=nco_sng_lst_free(fl_lst_abb,abb_arg_nbr);
    if(gaa_nbr > 0) gaa_arg=nco_sng_lst_free(gaa_arg,gaa_nbr);
    if(var_lst_in_nbr > 0) var_lst_in=nco_sng_lst_free(var_lst_in,var_lst_in_nbr);
    /* Free limits */
    for(idx=0;idx<lmt_nbr;idx++) lmt_arg[idx]=(char *)nco_free(lmt_arg[idx]);
    if(lmt_nbr > 0) lmt=nco_lmt_lst_free(lmt,lmt_nbr);
    /* Free chunking information */
    for(idx=0;idx<cnk_nbr;idx++) cnk_arg[idx]=(char *)nco_free(cnk_arg[idx]);
    if(cnk_nbr > 0) cnk_dmn=nco_cnk_lst_free(cnk_dmn,cnk_nbr);
    /* Free dimension lists */
    if(nbr_dmn_xtr > 0) dim=nco_dmn_lst_free(dim,nbr_dmn_xtr);
    if(nbr_dmn_out > 0) dmn_out=nco_dmn_lst_free(dmn_out,nbr_dmn_out);
    /* Free variable lists */
    if(xtr_nbr > 0) var=nco_var_lst_free(var,xtr_nbr); 
    /* ncwa uses nco_var_lst_free() on var_prc_out because var_out has dangling pointers */
    if(nbr_var_fix > 0) var_fix_out=nco_var_lst_free(var_fix_out,nbr_var_fix);
    if(nbr_var_prc > 0) var_prc_out=nco_var_lst_free(var_prc_out,nbr_var_prc);
    var_prc=(var_sct **)nco_free(var_prc);
    var_fix=(var_sct **)nco_free(var_fix);
    var_out=(var_sct **)nco_free(var_out);
  } /* !flg_mmr_cln */
  
#ifdef ENABLE_MPI
  MPI_Finalize();
#endif /* !ENABLE_MPI */
  
  /* End timer */ 
  ddra_info.tmr_flg=nco_tmr_end; /* [enm] Timer flag */
  rcd+=nco_ddra((char *)NULL,(char *)NULL,&ddra_info);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"main");
  
  nco_exit_gracefully();
  return EXIT_SUCCESS;
} /* end main() */
