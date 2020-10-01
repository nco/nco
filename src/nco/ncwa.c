/* $Header$ */

/* ncwa -- netCDF weighted averager */

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

/* fxm: 19981202 deactivated -n and -W switches and code left in place to rethink normalization switches */

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
#ifndef _MSC_VER
# if !defined(HAVE_BISON_FLEX)
#  define HAVE_BISON_FLEX /* 21070906 pvn add this definition to automake, currently in CMake */
# endif /* HAVE_BISON_FLEX */
# include <unistd.h> /* POSIX stuff */
#endif /* _MSC_VER */
#ifndef HAVE_GETOPT_LONG
# include "nco_getopt.h"
#else /* HAVE_GETOPT_LONG */ 
# ifdef HAVE_GETOPT_H
#  include <getopt.h>
# endif /* !HAVE_GETOPT_H */ 
#endif /* HAVE_GETOPT_LONG */

#ifdef I18N
# include <langinfo.h> /* nl_langinfo() */
# include <libintl.h> /* Internationalization i18n */
# include <locale.h> /* Locale setlocale() */
# define _(sng) gettext (sng)
# define gettext_noop(sng) (sng)
# define N_(sng) gettext_noop(sng)
#endif /* I18N */
/* Supply stub gettext() function in case i18n failed */
#ifndef _LIBINTL_H
# define gettext(foo) foo
#endif /* _LIBINTL_H */

/* 3rd party vendors */	 
#include <netcdf.h> /* netCDF definitions and C library */	 
#ifdef ENABLE_MPI
# include <mpi.h> /* MPI definitions */
# include <netcdf_par.h> /* Parallel netCDF definitions */
# include "nco_mpi.h" /* MPI utilities */
#endif /* !ENABLE_MPI */

/* #define MAIN_PROGRAM_FILE MUST precede #include libnco.h */
#define MAIN_PROGRAM_FILE
#ifdef HAVE_BISON_FLEX
  # include "ncap_utl.h" /* netCDF arithmetic processor-specific definitions (symbol table, ...) */
#endif /* !HAVE_BISON_FLEX */
#include "libnco.h" /* netCDF Operator (NCO) library */

#ifdef HAVE_BISON_FLEX
/* Global variables (keep consistent with global variables declared in ncap.c) */ 
size_t ncap_ncl_dpt_crr=0UL; /* [nbr] Depth of current #include file (incremented in ncap.l) */
size_t *ncap_ln_nbr_crr; /* [cnt] Line number (incremented in ncap.l) */
char **ncap_fl_spt_glb; /* [fl] Script file */
#endif /* !HAVE_BISON_FLEX */

int 
main(int argc,char **argv)
{
  char **dmn_avg_lst_in=NULL_CEWI; /* Option a */
  char **fl_lst_abb=NULL; /* Option n */
  char **fl_lst_in=NULL_CEWI;
  char **gaa_arg=NULL; /* [sng] Global attribute arguments */
  char **var_lst_in=NULL_CEWI;
  char **grp_lst_in=NULL_CEWI;
  char *aux_arg[NC_MAX_DIMS];
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
  char *ppc_arg[NC_MAX_VARS]; /* [sng] PPC arguments */
  char *sng_cnv_rcd=NULL_CEWI; /* [sng] strtol()/strtoul() return code */
  char *wgt_nm=NULL;
  char trv_pth[]="/"; /* [sng] Root path of traversal tree */

  const char * const CVS_Id="$Id$"; 
  const char * const CVS_Revision="$Revision$";
  const char * const opt_sht_lst="34567Aa:B:bCcD:d:Fg:G:hIL:l:M:m:nNOo:p:rRT:t:v:Ww:xy:-:";

  cnk_sct cnk; /* [sct] Chunking structure */

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

  int aux_nbr=0; /* [nbr] Number of auxiliary coordinate hyperslabs specified */
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
  int grp_lst_in_nbr=0; /* [nbr] Number of groups explicitly specified by user */
  int idx=int_CEWI;
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
  int ppc_nbr=0; /* [nbr] Number of PPC arguments */
  int rcd=NC_NOERR; /* [rcd] Return code */
  int thr_idx; /* [idx] Index of current thread */
  int thr_nbr=int_CEWI; /* [nbr] Thread number Option t */
  int var_lst_in_nbr=0;

  md5_sct *md5=NULL; /* [sct] MD5 configuration */

  cnv_sct *cnv; /* [sct] Convention structure */
  nco_bool DO_CONFORM_MSK=False; /* Did nco_var_cnf_dmn() find truly conforming mask? */
  nco_bool DO_CONFORM_WGT=False; /* Did nco_var_cnf_dmn() find truly conforming weight? */
  nco_bool EXCLUDE_INPUT_LIST=False; /* Option c */
  nco_bool EXTRACT_ALL_COORDINATES=False; /* Option c */
  nco_bool EXTRACT_ASSOCIATED_COORDINATES=True; /* Option C */
  nco_bool EXTRACT_CLL_MSR=True; /* [flg] Extract cell_measures variables */
  nco_bool EXTRACT_FRM_TRM=True; /* [flg] Extract formula_terms variables */
  nco_bool FL_RTR_RMT_LCN;
  nco_bool FL_LST_IN_FROM_STDIN=False; /* [flg] fl_lst_in comes from stdin */
  nco_bool FORCE_APPEND=False; /* Option A */
  nco_bool FORCE_OVERWRITE=False; /* Option O */
  nco_bool FORTRAN_IDX_CNV=False; /* Option F */
  nco_bool GRP_VAR_UNN=False; /* [flg] Select union of specified groups and variables */
  nco_bool HISTORY_APPEND=True; /* Option h */
  nco_bool HPSS_TRY=False; /* [flg] Search HPSS for unfound files */
  nco_bool MSA_USR_RDR=False; /* [flg] Multi-Slab Algorithm returns hyperslabs in user-specified order */
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
  nco_bool flg_cll_mth=True; /* [flg] Add/modify cell_methods attributes */
  nco_bool flg_mmr_cln=True; /* [flg] Clean memory prior to exit */
  nco_bool flg_dmn_prc_usr_spc=False; /* [flg] Processed dimensions specified on command line */
  nco_bool flg_ddra=False; /* [flg] DDRA diagnostics */
  nco_bool flg_rdd=False; /* [flg] Retain degenerate dimensions */
  
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
  var_sct *wgt_avg=NULL;

  trv_tbl_sct *trv_tbl=NULL; /* [lst] Traversal table */

  nco_dmn_dne_t *flg_dne=NULL; /* [lst] Flag to check if input dimension -d "does not exist" */

  gpe_sct *gpe=NULL; /* [sng] Group Path Editing (GPE) structure */

#ifdef HAVE_BISON_FLEX
  prs_sct prs_arg;  /* I/O [sct] Global information required in ncwa parser */
#endif /* !HAVE_BISON_FLEX */

#ifdef ENABLE_MPI
  /* Declare all MPI-specific variables here */
  MPI_Comm mpi_cmm=MPI_COMM_WORLD; /* [prc] Communicator */
  int prc_rnk; /* [idx] Process rank */
  int prc_nbr=0; /* [nbr] Number of MPI processes */
#endif /* !ENABLE_MPI */
  
  static struct option opt_lng[]={ /* Structure ordered by short option key if possible */
    /* Long options with no argument, no short option counterpart */
    {"cll_msr",no_argument,0,0}, /* [flg] Extract cell_measures variables */
    {"cell_measures",no_argument,0,0}, /* [flg] Extract cell_measures variables */
    {"no_cll_msr",no_argument,0,0}, /* [flg] Do not extract cell_measures variables */
    {"no_cell_measures",no_argument,0,0}, /* [flg] Do not extract cell_measures variables */
    {"frm_trm",no_argument,0,0}, /* [flg] Extract formula_terms variables */
    {"formula_terms",no_argument,0,0}, /* [flg] Extract formula_terms variables */
    {"no_frm_trm",no_argument,0,0}, /* [flg] Do not extract formula_terms variables */
    {"no_formula_terms",no_argument,0,0}, /* [flg] Do not extract formula_terms variables */
    {"cll_mth",no_argument,0,0}, /* [flg] Add/modify cell_methods attributes */
    {"cell_methods",no_argument,0,0}, /* [flg] Add/modify cell_methods attributes */
    {"no_cll_mth",no_argument,0,0}, /* [flg] Do not add/modify cell_methods attributes */
    {"no_cell_methods",no_argument,0,0}, /* [flg] Do not add/modify cell_methods attributes */
    {"clean",no_argument,0,0}, /* [flg] Clean memory prior to exit */
    {"mmr_cln",no_argument,0,0}, /* [flg] Clean memory prior to exit */
    {"drt",no_argument,0,0}, /* [flg] Allow dirty memory on exit */
    {"dirty",no_argument,0,0}, /* [flg] Allow dirty memory on exit */
    {"mmr_drt",no_argument,0,0}, /* [flg] Allow dirty memory on exit */
    {"ddra",no_argument,0,0}, /* [flg] DDRA diagnostics */
    {"mdl_cmp",no_argument,0,0}, /* [flg] DDRA diagnostics */
    {"dbl",no_argument,0,0}, /* [flg] Arithmetic convention: promote float to double */
    {"flt",no_argument,0,0}, /* [flg] Arithmetic convention: keep single-precision */
    {"rth_dbl",no_argument,0,0}, /* [flg] Arithmetic convention: promote float to double */
    {"rth_flt",no_argument,0,0}, /* [flg] Arithmetic convention: keep single-precision */
    {"hdf4",no_argument,0,0}, /* [flg] Treat file as HDF4 */
    {"hdf_upk",no_argument,0,0}, /* [flg] HDF unpack convention: unpacked=scale_factor*(packed-add_offset) */
    {"hdf_unpack",no_argument,0,0}, /* [flg] HDF unpack convention: unpacked=scale_factor*(packed-add_offset) */
    {"help",no_argument,0,0},
    {"hlp",no_argument,0,0},
    {"hpss_try",no_argument,0,0}, /* [flg] Search HPSS for unfound files */
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
    {"cnk_csh",required_argument,0,0}, /* [B] Chunk cache size in bytes */
    {"chunk_cache",required_argument,0,0}, /* [B] Chunk cache size in bytes */
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
    {"ppc",required_argument,0,0}, /* [nbr] Precision-preserving compression, i.e., number of total or decimal significant digits */
    {"precision_preserving_compression",required_argument,0,0}, /* [nbr] Precision-preserving compression, i.e., number of total or decimal significant digits */
    {"quantize",required_argument,0,0}, /* [nbr] Precision-preserving compression, i.e., number of total or decimal significant digits */
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
    {"append",no_argument,0,'A'},
    {"average",required_argument,0,'a'},
    {"avg",required_argument,0,'a'},
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
    {"weight",required_argument,0,'w'},
    {"wgt",required_argument,0,'w'},
    {"wgt_var",required_argument,0,'w'},
    {"operation",required_argument,0,'y'},
    {"op_typ",required_argument,0,'y'},
    {0,0,0,0}
  }; /* end opt_lng */
  int opt_idx=0; /* Index of current long option into opt_lng array */

  /* Start timer and save command line */ 
  ddra_info.tmr_flg=nco_tmr_srt;
  rcd+=nco_ddra((char *)NULL,(char *)NULL,&ddra_info);
  ddra_info.tmr_flg=nco_tmr_mtd;
  cmd_ln=nco_cmd_ln_sng(argc,argv);

  NORMALIZE_BY_TALLY=NORMALIZE_BY_TALLY+0; /* CEWI: Avert compiler warning that variable is set but never used */
  NORMALIZE_BY_WEIGHT=NORMALIZE_BY_WEIGHT+0; /* CEWI: Avert compiler warning that variable is set but never used */

  /* Get program name and set program enum (e.g., nco_prg_id=ncra) */
  nco_prg_nm=nco_prg_prs(argv[0],&nco_prg_id);

#ifdef ENABLE_MPI
  /* MPI Initialization */
  if(False) (void)fprintf(stdout,gettext("%s: WARNING Compiled with MPI\n"),nco_prg_nm);
  MPI_Init(&argc,&argv);
  MPI_Comm_size(mpi_cmm,&prc_nbr);
  MPI_Comm_rank(mpi_cmm,&prc_rnk);
#endif /* !ENABLE_MPI */
  
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
      } /* endif bfr_sz */
      if(!strcmp(opt_crr,"cnk_byt") || !strcmp(opt_crr,"chunk_byte")){
        cnk_sz_byt=strtoul(optarg,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
        if(*sng_cnv_rcd) nco_sng_cnv_err(optarg,"strtoul",sng_cnv_rcd);
      } /* endif cnk_byt */
      if(!strcmp(opt_crr,"cnk_csh") || !strcmp(opt_crr,"chunk_cache")){
        cnk_csh_byt=strtoul(optarg,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
        if(*sng_cnv_rcd) nco_sng_cnv_err(optarg,"strtoul",sng_cnv_rcd);
      } /* endif cnk_csh_byt */
      if(!strcmp(opt_crr,"cnk_min") || !strcmp(opt_crr,"chunk_min")){
        cnk_min_byt=strtoul(optarg,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
        if(*sng_cnv_rcd) nco_sng_cnv_err(optarg,"strtoul",sng_cnv_rcd);
      } /* endif cnk_min */
      if(!strcmp(opt_crr,"cnk_dmn") || !strcmp(opt_crr,"chunk_dimension")){
        /* Copy limit argument for later processing */
        cnk_arg[cnk_nbr]=(char *)strdup(optarg);
        cnk_nbr++;
      } /* endif cnk_dmn */
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
      if(!strcmp(opt_crr,"cll_msr") || !strcmp(opt_crr,"cell_measures")) EXTRACT_CLL_MSR=True; /* [flg] Extract cell_measures variables */
      if(!strcmp(opt_crr,"no_cll_msr") || !strcmp(opt_crr,"no_cell_measures")) EXTRACT_CLL_MSR=False; /* [flg] Do not extract cell_measures variables */
      if(!strcmp(opt_crr,"frm_trm") || !strcmp(opt_crr,"formula_terms")) EXTRACT_FRM_TRM=True; /* [flg] Extract formula_terms variables */
      if(!strcmp(opt_crr,"no_frm_trm") || !strcmp(opt_crr,"no_formula_terms")) EXTRACT_FRM_TRM=False; /* [flg] Do not extract formula_terms variables */
      if(!strcmp(opt_crr,"cll_mth") || !strcmp(opt_crr,"cell_methods")) flg_cll_mth=True; /* [flg] Add/modify cell_methods attributes */
      if(!strcmp(opt_crr,"no_cll_mth") || !strcmp(opt_crr,"no_cell_methods")) flg_cll_mth=False; /* [flg] Add/modify cell_methods attributes */
      if(!strcmp(opt_crr,"mmr_cln") || !strcmp(opt_crr,"clean")) flg_mmr_cln=True; /* [flg] Clean memory prior to exit */
      if(!strcmp(opt_crr,"drt") || !strcmp(opt_crr,"mmr_drt") || !strcmp(opt_crr,"dirty")) flg_mmr_cln=False; /* [flg] Clean memory prior to exit */
      if(!strcmp(opt_crr,"ddra") || !strcmp(opt_crr,"mdl_cmp")) ddra_info.flg_ddra=flg_ddra=True; /* [flg] DDRA diagnostics */
      if(!strcmp(opt_crr,"fl_fmt") || !strcmp(opt_crr,"file_format")) rcd=nco_create_mode_prs(optarg,&fl_out_fmt);
      if(!strcmp(opt_crr,"dbl") || !strcmp(opt_crr,"rth_dbl")) nco_rth_cnv=nco_rth_flt_dbl; /* [flg] Arithmetic convention: promote float to double */
      if(!strcmp(opt_crr,"flt") || !strcmp(opt_crr,"rth_flt")) nco_rth_cnv=nco_rth_flt_flt; /* [flg] Arithmetic convention: keep single-precision */
      if(!strcmp(opt_crr,"gaa") || !strcmp(opt_crr,"glb_att_add")){
        gaa_arg=(char **)nco_realloc(gaa_arg,(gaa_nbr+1)*sizeof(char *));
        gaa_arg[gaa_nbr++]=(char *)strdup(optarg);
      } /* endif gaa */
      if(!strcmp(opt_crr,"hdf4")) nco_fmt_xtn=nco_fmt_xtn_hdf4; /* [enm] Treat file as HDF4 */
      if(!strcmp(opt_crr,"hdf_upk") || !strcmp(opt_crr,"hdf_unpack")) nco_upk_cnv=nco_upk_HDF_MOD10; /* [flg] HDF unpack convention: unpacked=scale_factor*(packed-add_offset) */
      if(!strcmp(opt_crr,"hdr_pad") || !strcmp(opt_crr,"header_pad")){
        hdr_pad=strtoul(optarg,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
        if(*sng_cnv_rcd) nco_sng_cnv_err(optarg,"strtoul",sng_cnv_rcd);
      } /* endif "hdr_pad" */
      if(!strcmp(opt_crr,"help") || !strcmp(opt_crr,"hlp")){
	(void)nco_usg_prn();
	nco_exit(EXIT_SUCCESS);
      } /* endif "help" */
      if(!strcmp(opt_crr,"hpss_try")) HPSS_TRY=True; /* [flg] Search HPSS for unfound files */
      if(!strcmp(opt_crr,"log_lvl") || !strcmp(opt_crr,"log_level")){
	log_lvl=(int)strtol(optarg,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
	if(*sng_cnv_rcd) nco_sng_cnv_err(optarg,"strtol",sng_cnv_rcd);
	nc_set_log_level(log_lvl);
      } /* !log_lvl */
      if(!strcmp(opt_crr,"ppc") || !strcmp(opt_crr,"precision_preserving_compression") || !strcmp(opt_crr,"quantize")){
        ppc_arg[ppc_nbr]=(char *)strdup(optarg);
        ppc_nbr++;
      } /* endif "ppc" */
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
      fl_out_fmt=NC_FORMAT_64BIT_OFFSET;
      break;
    case '7': /* Request netCDF4-classic output storage format */
      fl_out_fmt=NC_FORMAT_NETCDF4_CLASSIC;
      break;
    case 'A': /* Toggle FORCE_APPEND */
      FORCE_APPEND=!FORCE_APPEND;
      break;
    case 'a': /* Dimensions over which to average hyperslab */
      flg_dmn_prc_usr_spc=True;
      if(dmn_avg_lst_in){
        (void)fprintf(fp_stdout,"%s: ERROR Option -a appears more than once\n",nco_prg_nm);
        (void)fprintf(fp_stdout,"%s: HINT Use -a dim1,dim2,... not -a dim1 -a dim2 ...\n",nco_prg_nm);
        (void)nco_usg_prn();
        nco_exit(EXIT_FAILURE);
      } /* endif */
      dmn_avg_lst_in=nco_lst_prs_2D(optarg,",",&dmn_avg_nbr);
      break;
    case 'B': /* Mask string to be parsed */
      msk_cnd_sng=(char *)strdup(optarg);
#ifndef HAVE_BISON_FLEX
      (void)fprintf(fp_stdout,"%s: ERROR -B and --mask_condition options unsupported because configuration could not find a parser (e.g., Bison) and lexer (e.g., Flex). HINT: Break condition into component -m -T -M switches, e.g., use -m ORO -T lt -M 1.0 instead of -B \"ORO < 1\"\n",nco_prg_nm);
      nco_exit(EXIT_FAILURE);
#endif /* HAVE_BISON_FLEX */
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
    case 'G': /* Apply Group Path Editing (GPE) to output group */
      /* NB: GNU getopt() optional argument syntax is ugly (requires "=" sign) so avoid it
	 http://stackoverflow.com/questions/1052746/getopt-does-not-parse-optional-arguments-to-parameters */
      gpe=nco_gpe_prs_arg(optarg);
      fl_out_fmt=NC_FORMAT_NETCDF4; 
      break;
    case 'g': /* Copy group argument for later processing */
      /* Replace commas with hashes when within braces (convert back later) */
      optarg_lcl=(char *)strdup(optarg);
      (void)nco_rx_comma2hash(optarg_lcl);
      grp_lst_in=nco_lst_prs_2D(optarg_lcl,",",&grp_lst_in_nbr);
      optarg_lcl=(char *)nco_free(optarg_lcl);
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
    case 'M': /* Good data defined by relation to mask value. Default is 1.0 */
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
      (void)fprintf(fp_stdout,"%s: ERROR This option has been disabled while I re-think its implementation. Sincerely, Charlie\n",nco_prg_nm);
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
    case '?': /* Question mark means unrecognized option, print proper usage then EXIT_FAILURE */
      (void)fprintf(stdout,"%s: ERROR in command-line syntax/options. Missing or unrecognized option. Please reformulate command accordingly.\n",nco_prg_nm_get());
      (void)nco_usg_prn();
      nco_exit(EXIT_FAILURE);
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

  /* Set/report global chunk cache */
  rcd+=nco_cnk_csh_ini(cnk_csh_byt);

  /* Initialize traversal table */ 
  trv_tbl_init(&trv_tbl);

  /* Parse mask string */
#ifdef HAVE_BISON_FLEX
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
    if(ncap_ncwa_scn(&prs_arg,msk_cnd_sng,&msk_nm,&msk_val,&op_typ_rlt) != NCO_NOERR) nco_exit(EXIT_FAILURE); 
  } /* endif msk_cnd_sng */ 
#endif /* !HAVE_BISON_FLEX */

  /* Ensure we do not attempt to normalize by non-existent weight */
  if(!wgt_nm) NORMALIZE_BY_WEIGHT=False;

  /* Process positional arguments and fill-in filenames */
  fl_lst_in=nco_fl_lst_mk(argv,argc,optind,&fl_nbr,&fl_out,&FL_LST_IN_FROM_STDIN,FORCE_OVERWRITE);

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

  (void)nco_inq_format(in_id,&fl_in_fmt);

  /* Construct GTT, Group Traversal Table (groups,variables,dimensions, limits) */
  (void)nco_bld_trv_tbl(in_id,trv_pth,lmt_nbr,lmt_arg,aux_nbr,aux_arg,MSA_USR_RDR,FORTRAN_IDX_CNV,grp_lst_in,grp_lst_in_nbr,var_lst_in,xtr_nbr,EXTRACT_ALL_COORDINATES,GRP_VAR_UNN,False,EXCLUDE_INPUT_LIST,EXTRACT_ASSOCIATED_COORDINATES,EXTRACT_CLL_MSR,EXTRACT_FRM_TRM,nco_pck_plc_nil,&flg_dne,trv_tbl);

  /* Get number of variables, dimensions, and global attributes in file, file format */
  (void)trv_tbl_inq((int *)NULL,(int *)NULL,(int *)NULL,&nbr_dmn_fl,(int *)NULL,(int *)NULL,(int *)NULL,(int *)NULL,&nbr_var_fl,trv_tbl);

  /* Allocate array of dimensions associated with variables to be extracted with maximum possible size */
  dim=(dmn_sct **)nco_malloc(nbr_dmn_fl*sizeof(dmn_sct *));

  /* Find dimensions associated with variables to be extracted */
  (void)nco_dmn_lst_ass_var_trv(in_id,trv_tbl,&nbr_dmn_xtr,&dim);

  /* Not specifying any dimensions is interpreted as specifying all dimensions */
  if(dmn_avg_nbr == 0){
    dmn_avg_nbr=nbr_dmn_xtr;
    dmn_avg_lst_in=(char **)nco_malloc(dmn_avg_nbr*sizeof(char *));
    for(idx=0;idx<dmn_avg_nbr;idx++) dmn_avg_lst_in[idx]=(char *)strdup(dim[idx]->nm);
    if(nco_dbg_lvl >= nco_dbg_std) (void)fprintf(stderr,"%s: INFO No dimensions specified with -a, therefore reducing (averaging, taking minimum, etc.) over all dimensions\n",nco_prg_nm);
  } /* end if dmn_avg_nbr == 0 */

  /* Allocate array of dimensions to average with maximum possible size */
  dmn_avg=(dmn_sct **)nco_malloc(nbr_dmn_fl*sizeof(dmn_sct *));

  /* Allocate array of dimensions to keep on output with maximum possible size */
  dmn_out=(dmn_sct **)nco_malloc(nbr_dmn_fl*sizeof(dmn_sct *));

  /* Create list of dimensions to average */
  (void)nco_dmn_avg_mk(in_id,dmn_avg_lst_in,dmn_avg_nbr,flg_dmn_prc_usr_spc,flg_rdd,trv_tbl,&dmn_avg,&dmn_avg_nbr);

  /* Create list of dimensions to keep on output */
  (void)nco_dmn_out_mk(dim,nbr_dmn_xtr,trv_tbl,&dmn_out,&nbr_dmn_out);

  dmn_avg=(dmn_sct **)nco_realloc(dmn_avg,dmn_avg_nbr*sizeof(dmn_sct *));
  dmn_out=(dmn_sct **)nco_realloc(dmn_out,nbr_dmn_out*sizeof(dmn_sct *));

  /* Transfer degenerated dimensions information into GTT  */
  (void)nco_dmn_dgn_tbl(dmn_out,nbr_dmn_out,trv_tbl);

  /* Fill-in variable structure list for all extracted variables */
  var=nco_fll_var_trv(in_id,&xtr_nbr,trv_tbl);

  /* Duplicate to output array */
  var_out=(var_sct **)nco_malloc(xtr_nbr*sizeof(var_sct *));
  for(idx=0;idx<xtr_nbr;idx++){
    var_out[idx]=nco_var_dpl(var[idx]);
    (void)nco_xrf_var(var[idx],var_out[idx]);
    (void)nco_xrf_dmn(var_out[idx]);
  } /* end loop over var */

  /* Determine conventions (ARM/CCM/CCSM/CF/MPAS) for treating file */
  cnv=nco_cnv_ini(in_id);

  /* Divide variable lists into lists of fixed variables and variables to be processed */
  (void)nco_var_lst_dvd(var,var_out,xtr_nbr,cnv,True,nco_pck_plc_nil,nco_pck_map_nil,dmn_avg,dmn_avg_nbr,&var_fix,&var_fix_out,&nbr_var_fix,&var_prc,&var_prc_out,&nbr_var_prc,trv_tbl);

  /* Store processed and fixed variables info into GTT */
  (void)nco_var_prc_fix_trv(nbr_var_prc,var_prc,nbr_var_fix,var_fix,trv_tbl);

  /* We now have final list of variables to extract. Phew. */
  if(nco_dbg_lvl >= nco_dbg_var && nco_dbg_lvl != nco_dbg_dev){
    for(idx=0;idx<xtr_nbr;idx++) (void)fprintf(stderr,"var[%d]->nm = %s, ->id=[%d]\n",idx,var[idx]->nm,var[idx]->id);
    for(idx=0;idx<nbr_var_fix;idx++) (void)fprintf(stderr,"var_fix[%d]->nm = %s, ->id=[%d]\n",idx,var_fix[idx]->nm,var_fix[idx]->id);
    for(idx=0;idx<nbr_var_prc;idx++) (void)fprintf(stderr,"var_prc[%d]->nm = %s, ->id=[%d]\n",idx,var_prc[idx]->nm,var_prc[idx]->id);
  } /* end if */

  /* Make output and input files consanguinous */
  if(fl_out_fmt == NCO_FORMAT_UNDEFINED) fl_out_fmt=fl_in_fmt;

  /* Initialize, decode, and set PPC information */
  if(ppc_nbr > 0) nco_ppc_ini(in_id,&dfl_lvl,fl_out_fmt,ppc_arg,ppc_nbr,trv_tbl);

  /* Verify output file format supports requested actions */
  (void)nco_fl_fmt_vet(fl_out_fmt,cnk_nbr,dfl_lvl);

  /* Open output file */
  fl_out_tmp=nco_fl_out_open(fl_out,&FORCE_APPEND,FORCE_OVERWRITE,fl_out_fmt,&bfr_sz_hnt,RAM_CREATE,RAM_OPEN,SHARE_CREATE,SHARE_OPEN,WRT_TMP_FL,&out_id);

  /* Initialize chunking from user-specified inputs */
  if(fl_out_fmt == NC_FORMAT_NETCDF4 || fl_out_fmt == NC_FORMAT_NETCDF4_CLASSIC) rcd+=nco_cnk_ini(in_id,fl_out,cnk_arg,cnk_nbr,cnk_map,cnk_plc,cnk_csh_byt,cnk_min_byt,cnk_sz_byt,cnk_sz_scl,&cnk);

  /* Define dimensions, extracted groups, variables, and attributes in output file.  */
  (void)nco_xtr_dfn(in_id,out_id,&cnk,dfl_lvl,gpe,md5,!FORCE_APPEND,True,False,nco_pck_plc_nil,(char *)NULL,trv_tbl);

  /* Catenate time-stamped command line to "history" global attribute */
  if(HISTORY_APPEND) (void)nco_hst_att_cat(out_id,cmd_ln);
  if(HISTORY_APPEND && FORCE_APPEND) (void)nco_prv_att_cat(fl_in,in_id,out_id);
  if(gaa_nbr > 0) (void)nco_glb_att_add(out_id,gaa_arg,gaa_nbr);
  if(HISTORY_APPEND) (void)nco_vrs_att_cat(out_id);
  if(thr_nbr > 1 && HISTORY_APPEND) (void)nco_thr_att_cat(out_id,thr_nbr);

  /* Add new missing values to output file while in define mode */
  if(msk_nm){
    for(idx=0;idx<nbr_var_prc;idx++){
      char *grp_out_fll=NULL; /* [sng] Group name */
      int grp_out_id; /* [ID] Group ID (output) */
      int var_out_id; /* [ID] Variable ID (output) */
      trv_sct *var_trv; /* [sct] Variable GTT object */

      /* Obtain variable GTT object using full variable name */
      var_trv=trv_tbl_var_nm_fll(var_prc[idx]->nm_fll,trv_tbl);

      /* Edit group name for output */
      if(gpe) grp_out_fll=nco_gpe_evl(gpe,var_trv->grp_nm_fll); else grp_out_fll=(char *)strdup(var_trv->grp_nm_fll);

      /* Obtain output group ID */
      (void)nco_inq_grp_full_ncid(out_id,grp_out_fll,&grp_out_id);

      /* Memory management after current extracted group */
      if(grp_out_fll) grp_out_fll=(char *)nco_free(grp_out_fll);

      /* Get variable ID */
      (void)nco_inq_varid(grp_out_id,var_trv->nm,&var_out_id);

      /* Store the output variable ID */
      var_prc_out[idx]->id=var_out_id;

      /* Define for var_prc_out because mss_val for var_prc will be overwritten in nco_var_mtd_refresh() */
      if(!var_prc_out[idx]->has_mss_val){
        var_prc_out[idx]->has_mss_val=True;
        var_prc_out[idx]->mss_val=nco_mss_val_mk(var_prc[idx]->type);
        (void)nco_put_att(grp_out_id,var_prc_out[idx]->id,nco_mss_val_sng_get(),var_prc_out[idx]->type,(long)1,var_prc_out[idx]->mss_val.vp);
      } /* end if */
    } /* end for */
  } /* end if */

  /* Turn-off default filling behavior to enhance efficiency */
  nco_set_fill(out_id,NC_NOFILL,&fll_md_old);

  /* Add cell_methods attributes (before exiting define mode) */
  if(flg_cll_mth) rcd+=nco_cnv_cf_cll_mth_add(out_id,var_prc_out,nbr_var_prc,dmn_avg,dmn_avg_nbr,nco_op_typ,gpe,(clm_bnd_sct *)NULL,trv_tbl);

  /* Take output file out of define mode */
  if(hdr_pad == 0UL){
    (void)nco_enddef(out_id);
  }else{
    (void)nco__enddef(out_id,hdr_pad);
    if(nco_dbg_lvl >= nco_dbg_scl) (void)fprintf(stderr,"%s: INFO Padding header with %lu extra bytes\n",nco_prg_nm_get(),(unsigned long)hdr_pad);
  } /* hdr_pad */

  /* Assign zero to start and unity to stride vectors in output variables */
  (void)nco_var_srd_srt_set(var_out,xtr_nbr);

  /* Copy variable data for non-processed variables */
  (void)nco_cpy_fix_var_trv(in_id,out_id,gpe,trv_tbl);  

  /* Close first input netCDF file */
  nco_close(in_id);

  /* Loop over input files (not currently used, fl_nbr == 1) */
  for(fl_idx=0;fl_idx<fl_nbr;fl_idx++){
    /* Parse filename */
    if(fl_idx != 0) fl_in=nco_fl_nm_prs(fl_in,fl_idx,&fl_nbr,fl_lst_in,abb_arg_nbr,fl_lst_abb,fl_pth);
    if(nco_dbg_lvl >= nco_dbg_fl) (void)fprintf(stderr,"%s: INFO Input file %d is %s",nco_prg_nm_get(),fl_idx,fl_in);
    /* Make sure file is on local system and is readable or die trying */
    if(fl_idx != 0) fl_in=nco_fl_mk_lcl(fl_in,fl_pth_lcl,HPSS_TRY,&FL_RTR_RMT_LCN);
    if(nco_dbg_lvl >= nco_dbg_fl && FL_RTR_RMT_LCN) (void)fprintf(stderr,", local file is %s",fl_in);
    if(nco_dbg_lvl >= nco_dbg_fl) (void)fprintf(stderr,"\n");

    /* Open file once per thread to improve caching */
    for(thr_idx=0;thr_idx<thr_nbr;thr_idx++) rcd=nco_fl_open(fl_in,md_open,&bfr_sz_hnt,in_id_arr+thr_idx);
    in_id=in_id_arr[0];

    /* Perform various error-checks on input file */
    if(False) (void)nco_fl_cmp_err_chk();

    /* Timestamp end of metadata setup and disk layout */
    rcd+=nco_ddra((char *)NULL,(char *)NULL,&ddra_info);
    ddra_info.tmr_flg=nco_tmr_rgl;

#ifdef _OPENMP
    /* OpenMP notes:
       firstprivate(): rcd gets incremented, so keep initial value
       lastprivate(): retain rcd value from last thread
       private(): wgt_avg does not need initialization
       shared(): msk, wgt and lmt_nbr are not altered within loop */
#pragma omp parallel for firstprivate(DO_CONFORM_MSK,DO_CONFORM_WGT,ddra_info,rcd) lastprivate(rcd) private(idx,in_id,wgt_avg) shared(MULTIPLY_BY_TALLY,MUST_CONFORM,NRM_BY_DNM,WGT_MSK_CRD_VAR,dmn_avg,dmn_avg_nbr,flg_ddra,flg_rdd,gpe,in_id_arr,msk_nm,msk_val,nbr_var_prc,nco_dbg_lvl,nco_op_typ,nco_prg_nm,op_typ_rlt,out_id,trv_tbl,var_prc,var_prc_out,wgt_nm,lmt_nbr,lmt_arg,FORTRAN_IDX_CNV,MSA_USR_RDR)
#endif /* !_OPENMP */
    for(idx=0;idx<nbr_var_prc;idx++){ /* Process all variables in current file */
      char *grp_out_fll=NULL; /* [sng] Group name */
      int grp_id;        /* [ID] Group ID */
      int grp_out_id;    /* [ID] Group ID (output) */
      int var_out_id;    /* [ID] Variable ID (output) */
      trv_sct *var_trv;  /* [sct] Variable GTT object */
      var_sct *wgt=NULL;
      var_sct *wgt_out=NULL;
      var_sct *msk=NULL;
      var_sct *msk_out=NULL;

      in_id=in_id_arr[omp_get_thread_num()];

      /* Obtain variable GTT object using full variable name */
      var_trv=trv_tbl_var_nm_fll(var_prc[idx]->nm_fll,trv_tbl);

      /* Obtain group ID */
      (void)nco_inq_grp_full_ncid(in_id,var_trv->grp_nm_fll,&grp_id);

      if(nco_dbg_lvl >= nco_dbg_var && nco_dbg_lvl < nco_dbg_nbr) rcd+=nco_var_prc_crr_prn(idx,var_prc[idx]->nm);
      if(nco_dbg_lvl >= nco_dbg_var && nco_dbg_lvl < nco_dbg_nbr) (void)fflush(fp_stderr);

      /* Allocate and, if necessary, initialize accumulation space for all processed variables */
      var_prc_out[idx]->sz=var_prc[idx]->sz;
     
      /* fxm: verify that var_prc->tally is not needed */
      if(!(var_prc_out[idx]->tally=(long *)nco_malloc_flg(var_prc_out[idx]->sz*sizeof(long)))){
        (void)fprintf(fp_stdout,"%s: ERROR Unable to malloc() %ld*%ld bytes for tally buffer for variable %s in main()\n",nco_prg_nm_get(),var_prc_out[idx]->sz,(long)sizeof(long),var_prc_out[idx]->nm);
        nco_exit(EXIT_FAILURE); 
      } /* end if err */
      (void)nco_zero_long(var_prc_out[idx]->sz,var_prc_out[idx]->tally);
      if(!(var_prc_out[idx]->val.vp=(void *)nco_malloc_flg(var_prc_out[idx]->sz*nco_typ_lng(var_prc_out[idx]->type)))){
        (void)fprintf(fp_stdout,"%s: ERROR Unable to malloc() %ld*%lu bytes for value buffer for variable %s in main()\n",nco_prg_nm_get(),var_prc_out[idx]->sz,(unsigned long)nco_typ_lng(var_prc_out[idx]->type),var_prc_out[idx]->nm);
        nco_exit(EXIT_FAILURE); 
      } /* end if err */
      (void)nco_var_zero(var_prc_out[idx]->type,var_prc_out[idx]->sz,var_prc_out[idx]->val);

      (void)nco_var_mtd_refresh(grp_id,var_prc[idx]);

      /* Find weighting variable that matches current variable */
      if(wgt_nm) wgt=nco_var_get_wgt_trv(in_id,lmt_nbr,lmt_arg,MSA_USR_RDR,FORTRAN_IDX_CNV,wgt_nm,var_prc[idx],trv_tbl);

      /* Find mask variable that matches current variable */
      if(msk_nm) msk=nco_var_get_wgt_trv(in_id,lmt_nbr,lmt_arg,MSA_USR_RDR,FORTRAN_IDX_CNV,msk_nm,var_prc[idx],trv_tbl);

      /* Retrieve variable from disk into memory */
      (void)nco_msa_var_get_trv(in_id,var_prc[idx],trv_tbl);
      /* var_prc_out still has type = packed type for packed variables
	 nco_typ_cnv_rth() fixes that for most operations, though not for minimization or maximization
	 Following line is necessary only for packed variables subject to minimization or maximization */
      if(var_prc[idx]->typ_dsk != var_prc[idx]->type && var_prc[idx]->typ_upk == var_prc[idx]->type) var_prc_out[idx]=nco_var_cnf_typ(var_prc[idx]->type,var_prc_out[idx]);

      /* Convert char, short, long, int, and float types to doubles before arithmetic */
      var_prc[idx]=nco_typ_cnv_rth(var_prc[idx],nco_op_typ);
      var_prc_out[idx]=nco_typ_cnv_rth(var_prc_out[idx],nco_op_typ);

      /* Check mask found for this variable, using msk */
      if(msk && (!var_prc[idx]->is_crd_var || WGT_MSK_CRD_VAR)){
        msk_out=nco_var_cnf_dmn(var_prc[idx],msk,msk_out,MUST_CONFORM,&DO_CONFORM_MSK);
        /* Mask variable iff msk and var conform */
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
        case nco_op_mabs: /* Absolute value variable before weighting */
        case nco_op_mebs: /* Absolute value variable before weighting */
        case nco_op_mibs: /* Absolute value variable before weighting */
	  (void)nco_var_abs(var_prc[idx]->type,var_prc[idx]->sz,var_prc[idx]->has_mss_val,var_prc[idx]->mss_val,var_prc[idx]->val);
          break;
        case nco_op_avgsqr: /* Square variable before weighting */
        case nco_op_rms: /* Square variable before weighting */
        case nco_op_rmssdn: /* Square variable before weighting */
          (void)nco_var_mlt(var_prc[idx]->type,var_prc[idx]->sz,var_prc[idx]->has_mss_val,var_prc[idx]->mss_val,var_prc[idx]->val,var_prc[idx]->val);
          break;
        default: /* All other operations are linear, do nothing to them yet */
          break;
        } /* end case */
      } /* var_prc[idx]->is_crd_var */

      /* Check weight found for this variable, using wgt */
      if(wgt && (!var_prc[idx]->is_crd_var || WGT_MSK_CRD_VAR)){
        /* fxm: nco_var_cnf_dmn() has bug where it does not allocate tally array
	   for weights that do already conform to var_prc. TODO #114. */
        wgt_out=nco_var_cnf_dmn(var_prc[idx],wgt,wgt_out,MUST_CONFORM,&DO_CONFORM_WGT);
        if(DO_CONFORM_WGT){
          wgt_out=nco_var_cnf_typ(var_prc[idx]->type,wgt_out);
          /* Weight variable after any initial non-linear operation so, e.g., variable (not weights) is squared */
          (void)nco_var_mlt(var_prc[idx]->type,var_prc[idx]->sz,var_prc[idx]->has_mss_val,var_prc[idx]->mss_val,wgt_out->val,var_prc[idx]->val);
        } /* end if weights conformed */
      } /* end if weight was specified and then tested for conformance */
      /* Copy (masked) (weighted) values from var_prc to var_prc_out */
      (void)memcpy((void *)(var_prc_out[idx]->val.vp),(void *)(var_prc[idx]->val.vp),var_prc_out[idx]->sz*nco_typ_lng(var_prc_out[idx]->type));
      /* 20050516: fxm: destruction of var_prc_out in nco_var_avg() leaves dangling pointers in var_out? */
      /* Reduce variable over specified dimensions (tally array is set here)
	 NB: var_prc_out[idx] is new, so corresponding var_out[idx] is dangling
	 nco_var_avg() will perform nco_op_typ on all variables except coordinate variables
	 nco_var_avg() always averages coordinate variables */
      var_prc_out[idx]=nco_var_avg(var_prc_out[idx],dmn_avg,dmn_avg_nbr,nco_op_typ,flg_rdd,&ddra_info);
      /* var_prc_out[idx]->val now holds numerator of averaging expression documented in NCO Users Guide
	 Denominator is also tricky due to sundry normalization options
	 These logical switches are tricky---modify them with care */
      if(NRM_BY_DNM && DO_CONFORM_WGT && (!var_prc[idx]->is_crd_var || WGT_MSK_CRD_VAR)){
        /* Duplicate wgt_out as wgt_avg so that wgt_out is not contaminated by any
	   averaging operation and may be re-used on next variable.
	   Free wgt_avg after each use but continue to re-use wgt_out */
        wgt_avg=nco_var_dpl(wgt_out);

        if(var_prc[idx]->has_mss_val){
          double mss_val_dbl=double_CEWI;
          /* Set denominator to missing value at all locations where variable is missing value
	     If this is accomplished by setting weight to missing value wherever variable is missing value
	     then weight must not be re-used by next variable (which may conform but have missing values in different locations)
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
          case NC_INT64: mss_val_dbl=(double)wgt_avg->mss_val.i64p[0]; break; /* CEWI for MSVC */
          case NC_UINT64: mss_val_dbl=(double)wgt_avg->mss_val.ui64p[0]; break; /* CEWI for MSVC */
          case NC_BYTE: mss_val_dbl=wgt_avg->mss_val.bp[0]; break;
          case NC_UBYTE: mss_val_dbl=wgt_avg->mss_val.ubp[0]; break;
          case NC_CHAR: mss_val_dbl=wgt_avg->mss_val.cp[0]; break;
          case NC_STRING: break; /* Do nothing */
          default: nco_dfl_case_nc_type_err(); break;
          } /* end switch */
          /* Second, mask wgt_avg where variable is missing value */
          (void)nco_var_msk(wgt_avg->type,wgt_avg->sz,var_prc[idx]->has_mss_val,var_prc[idx]->mss_val,mss_val_dbl,nco_op_ne,var_prc[idx]->val,wgt_avg->val);
        } /* endif weight must be checked for missing values */

        /* Free current input buffer */
        var_prc[idx]->val.vp=nco_free(var_prc[idx]->val.vp);

        if(msk && DO_CONFORM_MSK){
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
        if(wgt_avg->sz > 0){
          if(!(wgt_avg->tally=(long *)nco_realloc(wgt_avg->tally,wgt_avg->sz*sizeof(long)))){
            (void)fprintf(fp_stdout,"%s: ERROR Unable to realloc() %ld*%ld bytes for tally buffer for weight %s in main()\n",nco_prg_nm_get(),wgt_avg->sz,(long)sizeof(long),wgt_avg->nm);
            nco_exit(EXIT_FAILURE); 
          } /* end if */
	} /* wgt_avg->sz */
        /* Average weight over specified dimensions (tally array is set here) */
	wgt_avg=nco_var_avg(wgt_avg,dmn_avg,dmn_avg_nbr,nco_op_avg,flg_rdd,&ddra_info);

	if(MULTIPLY_BY_TALLY){
	  /* NB: Currently this is not implemented */
	  /* Multiply numerator (weighted sum of variable) by tally 
	     We deviously accomplish this by dividing denominator by tally */
	  (void)nco_var_nrm(wgt_avg->type,wgt_avg->sz,wgt_avg->has_mss_val,wgt_avg->mss_val,wgt_avg->tally,wgt_avg->val);
	} /* endif */
          /* Divide numerator by denominator */
          /* Diagnose common PEBCAK before it causes core dump */
	if(var_prc_out[idx]->sz == 1L && var_prc_out[idx]->type == NC_INT && var_prc_out[idx]->val.ip[0] == 0){
	  (void)fprintf(fp_stdout,"%s: ERROR Weight in denominator weight = 0.0, will cause SIGFPE\n%s: HINT Sum of masked, averaged weights must be non-zero\n%s: HINT A possible workaround is to remove variable \"%s\" from output file using \"%s -x -v %s ...\"\n%s: Expecting core dump...now!\n",nco_prg_nm,nco_prg_nm,nco_prg_nm,var_prc_out[idx]->nm,nco_prg_nm,var_prc_out[idx]->nm,nco_prg_nm);
	} /* end if */
          /* Rather complex conditional statement is shorter than switch() */
	if( /* Normalize by weighted tally if ....  */
	   var_prc[idx]->is_crd_var || /* ...variable is a coordinate or ...*/
	   ((nco_op_typ != nco_op_min) && /* ...operation is not min() and... */
	    (nco_op_typ != nco_op_max) && /* ...operation is not max() and... */
	    (nco_op_typ != nco_op_mabs) && /* ...operation is not mabs() and... */
	    (nco_op_typ != nco_op_mibs) && /* ...operation is not mibs() and... */
	    (nco_op_typ != nco_op_tabs) && /* ...operation is not tabs() and... */
	    (nco_op_typ != nco_op_ttl)) /* ...operation is not ttl() and... */
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
          /* Always return averages (never extrema or other statistics) of coordinates
	     Prevent coordinate variables from encountering nco_var_nrm_sdn() */
          (void)nco_var_nrm(var_prc_out[idx]->type,var_prc_out[idx]->sz,var_prc_out[idx]->has_mss_val,var_prc_out[idx]->mss_val,var_prc_out[idx]->tally,var_prc_out[idx]->val);
        }else{ /* !var_prc[idx]->is_crd_var */
          switch(nco_op_typ){
          case nco_op_mebs: /* Normalize sum by tally to create mean */
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
          case nco_op_mabs: /* Maximum absolute value is already in buffer, do nothing */	
          case nco_op_mibs: /* Minimum absolute value is already in buffer, do nothing */	
          case nco_op_tabs: /* Total absolute value is already in buffer, do nothing */	
          case nco_op_ttl: /* Total is already in buffer, do nothing */	
            break;
          default:
            (void)fprintf(fp_stdout,"%s: ERROR Illegal nco_op_typ in non-weighted normalization\n",nco_prg_nm);
            nco_exit(EXIT_FAILURE);
            break;
          } /* end switch */
        } /* !var_prc[idx]->is_crd_var */
      }else if(!NRM_BY_DNM){
        /* User turned off normalization so we are done */
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

      /* Edit group name for output */
      if(gpe) grp_out_fll=nco_gpe_evl(gpe,var_trv->grp_nm_fll); else grp_out_fll=(char *)strdup(var_trv->grp_nm_fll);

      /* Obtain output group ID */
      (void)nco_inq_grp_full_ncid(out_id,grp_out_fll,&grp_out_id);

      /* Memory management after current extracted group */
      if(grp_out_fll) grp_out_fll=(char *)nco_free(grp_out_fll);

      /* Get variable ID */
      (void)nco_inq_varid(grp_out_id,var_trv->nm,&var_out_id);

      /* Store the output variable ID */
      var_prc_out[idx]->id=var_out_id;

      if(var_trv->ppc != NC_MAX_INT){
	if(var_trv->flg_nsd) (void)nco_ppc_bitmask(var_trv->ppc,var_prc_out[idx]->type,var_prc_out[idx]->sz,var_prc_out[idx]->has_mss_val,var_prc_out[idx]->mss_val,var_prc_out[idx]->val); else (void)nco_ppc_around(var_trv->ppc,var_prc_out[idx]->type,var_prc_out[idx]->sz,var_prc_out[idx]->has_mss_val,var_prc_out[idx]->mss_val,var_prc_out[idx]->val);
      } /* endif ppc */
      if(nco_is_xcp(var_trv->nm)) nco_xcp_prc(var_trv->nm,var_prc_out[idx]->type,var_prc_out[idx]->sz,(char *)var_prc_out[idx]->val.vp);
	
#ifdef _OPENMP
#pragma omp critical
#endif /* _OPENMP */
      { /* begin OpenMP critical */
        /* Copy average to output file then free averaging buffer */
        if(var_prc_out[idx]->nbr_dim == 0){
          (void)nco_put_var1(grp_out_id,var_prc_out[idx]->id,var_prc_out[idx]->srt,var_prc_out[idx]->val.vp,var_prc_out[idx]->type);
        }else{ /* end if variable is scalar */
          (void)nco_put_vara(grp_out_id,var_prc_out[idx]->id,var_prc_out[idx]->srt,var_prc_out[idx]->cnt,var_prc_out[idx]->val.vp,var_prc_out[idx]->type);
        } /* end if variable is array */
      } /* end OpenMP critical */

      if(flg_ddra){
        /* DDRA diagnostics
	   Usage:
	   ncwa -O -C --mdl -a lat,lon,time -w lat ~/nco/data/in.nc ~/foo.nc
	   ncwa -O -C --mdl -a lat,lon -w lat ${DATA}/nco_bm/stl_5km.nc ~/foo.nc
	   ncwa -O -C --mdl -a lat,lon,time -w lat ${DATA}/nco_bm/gcm_T85.nc ~/foo.nc */
	
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

      /* Free possible weight/mask found */
      if(wgt) wgt=nco_var_free(wgt);
      if(wgt_out) wgt_out=nco_var_free(wgt_out);
      if(msk) msk=nco_var_free(msk);
      if(msk_out) msk_out=nco_var_free(msk_out);

    } /* end (OpenMP parallel for) loop over idx */

    if(nco_dbg_lvl >= nco_dbg_var) (void)fprintf(stderr,"\n");

    /* Close input netCDF file */
    for(thr_idx=0;thr_idx<thr_nbr;thr_idx++) nco_close(in_id_arr[thr_idx]);

    /* Remove local copy of file */
    if(FL_RTR_RMT_LCN && RM_RMT_FL_PST_PRC) (void)nco_fl_rm(fl_in);

  } /* end loop over fl_idx */

  /* Close output file and move it from temporary to permanent location */
  (void)nco_fl_out_cls(fl_out,fl_out_tmp,out_id);

  /* Clean memory unless dirty memory allowed */
  if(flg_mmr_cln){
    /* ncwa-specific memory */
    if(dmn_avg_nbr > 0) dmn_avg=(dmn_sct **)nco_free(dmn_avg);
    if(msk_nm) msk_nm=(char *)nco_free(msk_nm);
    if(msk_cnd_sng) msk_cnd_sng=(char *)nco_free(msk_cnd_sng);
    if(wgt_avg) wgt_avg=nco_var_free(wgt_avg);
    if(wgt_nm) wgt_nm=(char *)nco_free(wgt_nm);
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
    for(idx=0;idx<aux_nbr;idx++) aux_arg[idx]=(char *)nco_free(aux_arg[idx]);
    for(idx=0;idx<lmt_nbr;idx++) lmt_arg[idx]=(char *)nco_free(lmt_arg[idx]);
    for(idx=0;idx<ppc_nbr;idx++) ppc_arg[idx]=(char *)nco_free(ppc_arg[idx]);
    /* Free chunking information */
    for(idx=0;idx<cnk_nbr;idx++) cnk_arg[idx]=(char *)nco_free(cnk_arg[idx]);
    if(cnk_nbr > 0 && (fl_out_fmt == NC_FORMAT_NETCDF4 || fl_out_fmt == NC_FORMAT_NETCDF4_CLASSIC)) cnk.cnk_dmn=(cnk_dmn_sct **)nco_cnk_lst_free(cnk.cnk_dmn,cnk_nbr);
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
    for(idx=0;idx<lmt_nbr;idx++) flg_dne[idx].dim_nm=(char *)nco_free(flg_dne[idx].dim_nm);
    if(flg_dne) flg_dne=(nco_dmn_dne_t *)nco_free(flg_dne);
    /* Free traversal table */
    trv_tbl_free(trv_tbl); 
  } /* !flg_mmr_cln */

  /* End timer */ 
  ddra_info.tmr_flg=nco_tmr_end; /* [enm] Timer flag */
  rcd+=nco_ddra((char *)NULL,(char *)NULL,&ddra_info);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"main");

#ifdef ENABLE_MPI
  MPI_Finalize();
#endif /* !ENABLE_MPI */
  
  nco_exit_gracefully();
  return EXIT_SUCCESS;
} /* end main() */
