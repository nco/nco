/* $Header$ */

/* ncflint -- netCDF file interpolator */

/* Purpose: Linearly interpolate a third netCDF file from two input files */

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

/* Usage:
   ncflint -O -D 2 in.nc in.nc ~/foo.nc
   ncflint -O -i lcl_time_hr,9.0 -v lcl_time_hr ${DATA}/arese/clm/951030_0800_arese_clm.nc ${DATA}/arese/clm/951030_1100_arese_clm.nc ~/foo.nc; ncks -H ~/foo.nc
   ncflint -O -w 0.66666,0.33333 -v lcl_time_hr ${DATA}/arese/clm/951030_0800_arese_clm.nc ${DATA}/arese/clm/951030_1100_arese_clm.nc ~/foo.nc; ncks -H ~/foo.nc
   ncflint -O -w 0.66666 -v lcl_time_hr ${DATA}/arese/clm/951030_0800_arese_clm.nc ${DATA}/arese/clm/951030_1100_arese_clm.nc ~/foo.nc; ncks -H ~/foo.nc

   ncdiff -O ~/foo.nc ${DATA}/arese/clm/951030_0900_arese_clm.nc foo2.nc;ncks -H foo2.nc | m */

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
# include <unistd.h> /* POSIX stuff */
#endif
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
#include "libnco.h" /* netCDF Operator (NCO) library */

int 
main(int argc,char **argv)
{
  cnv_sct *cnv; /* [sct] Convention structure */
  nco_bool CMD_LN_NTP_VAR=False; /* Option i */
  nco_bool CMD_LN_NTP_WGT=True; /* Option w */
  nco_bool DO_CONFORM=False; /* Did nco_var_cnf_dmn() find truly conforming variables? */
  nco_bool EXCLUDE_INPUT_LIST=False; /* Option c */
  nco_bool EXTRACT_ALL_COORDINATES=False; /* Option c */
  nco_bool EXTRACT_ASSOCIATED_COORDINATES=True; /* Option C */
  nco_bool EXTRACT_CLL_MSR=True; /* [flg] Extract cell_measures variables */
  nco_bool EXTRACT_FRM_TRM=True; /* [flg] Extract formula_terms variables */
  nco_bool FILE_1_RETRIEVED_FROM_REMOTE_LOCATION;
  nco_bool FILE_2_RETRIEVED_FROM_REMOTE_LOCATION;
  nco_bool FIX_REC_CRD=False; /* [flg] Do not interpolate/multiply record coordinate variables */
  nco_bool FL_LST_IN_FROM_STDIN=False; /* [flg] fl_lst_in comes from stdin */
  nco_bool FORCE_APPEND=False; /* Option A */
  nco_bool FORCE_OVERWRITE=False; /* Option O */
  nco_bool FORTRAN_IDX_CNV=False; /* Option F */
  nco_bool GRP_VAR_UNN=False; /* [flg] Select union of specified groups and variables */
  nco_bool HISTORY_APPEND=True; /* Option h */
  nco_bool HPSS_TRY=False; /* [flg] Search HPSS for unfound files */
  nco_bool MSA_USR_RDR=False; /* [flg] Multi-Slab Algorithm returns hyperslabs in user-specified order*/
  nco_bool MUST_CONFORM=False; /* Must nco_var_cnf_dmn() find truly conforming variables? */
  nco_bool NRM_WGT=False; /* [flg] Normalize input weights so w1:=w1/(w1+w2), w2:=w2/(w1+w2) */
  nco_bool RAM_CREATE=False; /* [flg] Create file in RAM */
  nco_bool RAM_OPEN=False; /* [flg] Open (netCDF3-only) file(s) in RAM */
  nco_bool SHARE_CREATE=False; /* [flg] Create (netCDF3-only) file(s) with unbuffered I/O */
  nco_bool SHARE_OPEN=False; /* [flg] Open (netCDF3-only) file(s) with unbuffered I/O */
  nco_bool RM_RMT_FL_PST_PRC=True; /* Option R */
  nco_bool WRT_TMP_FL=True; /* [flg] Write output to temporary file */
  nco_bool flg_mmr_cln=True; /* [flg] Clean memory prior to exit */

  char **fl_lst_abb=NULL; /* Option a */
  char **fl_lst_in;
  char **gaa_arg=NULL; /* [sng] Global attribute arguments */
  char **ntp_lst_in;
  char **grp_lst_in=NULL_CEWI;
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
  char *ntp_nm=NULL; /* Option i */
  char *opt_crr=NULL; /* [sng] String representation of current long-option name */
  char *optarg_lcl=NULL; /* [sng] Local copy of system optarg */
  char *ppc_arg[NC_MAX_VARS]; /* [sng] PPC arguments */
  char *sng_cnv_rcd=NULL_CEWI; /* [sng] strtol()/strtoul() return code */
  char trv_pth[]="/"; /* [sng] Root path of traversal tree */

  const char * const CVS_Id="$Id$"; 
  const char * const CVS_Revision="$Revision$";
  const char * const opt_sht_lst="34567ACcD:d:Fg:G:hi:L:l:NOo:p:rRt:v:X:xw:-:";

  cnk_sct cnk; /* [sct] Chunking structure */

#if defined(__cplusplus) || defined(PGI_CC)
  ddra_info_sct ddra_info;
  ddra_info.flg_ddra=False;
#else /* !__cplusplus */
  ddra_info_sct ddra_info={.flg_ddra=False};
#endif /* !__cplusplus */

  double ntp_val_out=double_CEWI; /* Option i */
  double wgt_val_1=0.5; /* Option w */
  double wgt_val_2=0.5; /* Option w */

  extern char *optarg;
  extern int optind;

  /* Using naked stdin/stdout/stderr in parallel region generates warning
     Copy appropriate filehandle to variable scoped shared in parallel clause */
  FILE * const fp_stderr=stderr; /* [fl] stderr filehandle CEWI */
  FILE * const fp_stdout=stdout; /* [fl] stdout filehandle CEWI */

  gpe_sct *gpe=NULL; /* [sng] Group Path Editing (GPE) structure */

  int *in_id_1_arr;
  int *in_id_2_arr;

  int abb_arg_nbr=0;
  int aux_nbr=0; /* [nbr] Number of auxiliary coordinate hyperslabs specified */
  int cnk_map=nco_cnk_map_nil; /* [enm] Chunking map */
  int cnk_nbr=0; /* [nbr] Number of chunk sizes */
  int cnk_plc=nco_cnk_plc_nil; /* [enm] Chunking policy */
  int dfl_lvl=NCO_DFL_LVL_UNDEFINED; /* [enm] Deflate level */
  int fl_idx;
  int fl_nbr=0;
  int fl_in_fmt_1; /* [enm] Input file format */
  int fl_in_fmt_2; /* [enm] Input file format */
  int fl_out_fmt=NCO_FORMAT_UNDEFINED; /* [enm] Output file format */
  int fll_md_old; /* [enm] Old fill mode */
  int gaa_nbr=0; /* [nbr] Number of global attributes to add */
  int has_mss_val=False;
  int idx;
  int in_id_1;  
  int in_id_2;  
  int lmt_nbr=0; /* Option d. NB: lmt_nbr gets incremented */
  int log_lvl=0; /* [enm] netCDF library debugging verbosity [0..5] */
  int md_open; /* [enm] Mode flag for nc_open() call */
  int nbr_dmn_fl;
  int nbr_ntp;
  int nbr_var_fix; /* nbr_var_fix gets incremented */
  int nbr_var_fl;
  int nbr_var_prc; /* nbr_var_prc gets incremented */
  int xtr_nbr=0; /* xtr_nbr won't otherwise be set for -c with no -v */
  int opt;
  int out_id;  
  int ppc_nbr=0; /* [nbr] Number of PPC arguments */
  int rcd=NC_NOERR; /* [rcd] Return code */
  int thr_idx; /* [idx] Index of current thread */
  int thr_nbr=int_CEWI; /* [nbr] Thread number Option t */
  int var_lst_in_nbr=0;
  int grp_lst_in_nbr=0; /* [nbr] Number of groups explicitly specified by user */

  md5_sct *md5=NULL; /* [sct] MD5 configuration */

  size_t bfr_sz_hnt=NC_SIZEHINT_DEFAULT; /* [B] Buffer size hint */
  size_t cnk_csh_byt=NCO_CNK_CSH_BYT_DFL; /* [B] Chunk cache size */
  size_t cnk_min_byt=NCO_CNK_SZ_MIN_BYT_DFL; /* [B] Minimize size of variable to chunk */
  size_t cnk_sz_byt=0UL; /* [B] Chunk size in bytes */
  size_t cnk_sz_scl=0UL; /* [nbr] Chunk size scalar */
  size_t hdr_pad=0UL; /* [B] Pad at end of header section */

  val_unn val_gnr_unn; /* Generic container for arrival point or weight */

  var_sct *wgt_1=NULL_CEWI;
  var_sct *wgt_2=NULL_CEWI;
  var_sct *wgt_out_1=NULL;
  var_sct *wgt_out_2=NULL;
  var_sct **var;
  var_sct **var_fix;
  var_sct **var_fix_out;
  var_sct **var_out;
  var_sct **var_prc_1;
  var_sct **var_prc_2;
  var_sct **var_prc_out;

  trv_tbl_sct *trv_tbl=NULL; /* [lst] Traversal table */

  nco_dmn_dne_t *flg_dne=NULL; /* [lst] Flag to check if input dimension -d "does not exist" */

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
    {"clean",no_argument,0,0}, /* [flg] Clean memory prior to exit */
    {"mmr_cln",no_argument,0,0}, /* [flg] Clean memory prior to exit */
    {"drt",no_argument,0,0}, /* [flg] Allow dirty memory on exit */
    {"dirty",no_argument,0,0}, /* [flg] Allow dirty memory on exit */
    {"mmr_drt",no_argument,0,0}, /* [flg] Allow dirty memory on exit */
    {"hdf4",no_argument,0,0}, /* [flg] Treat file as HDF4 */
    {"hdf_upk",no_argument,0,0}, /* [flg] HDF unpack convention: unpacked=scale_factor*(packed-add_offset) */
    {"hdf_unpack",no_argument,0,0}, /* [flg] HDF unpack convention: unpacked=scale_factor*(packed-add_offset) */
    {"help",no_argument,0,0},
    {"hlp",no_argument,0,0},
    {"hpss_try",no_argument,0,0}, /* [flg] Search HPSS for unfound files */
    {"msa_usr_rdr",no_argument,0,0}, /* [flg] Multi-Slab Algorithm returns hyperslabs in user-specified order */
    {"msa_user_order",no_argument,0,0}, /* [flg] Multi-Slab Algorithm returns hyperslabs in user-specified order */
    {"fix_rec_crd",no_argument,0,0}, /* [flg] Do not interpolate/multiply record coordinate variables */
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
    {"intersection",no_argument,0,0}, /* [flg] Select intersection of specified groups and variables */
    {"nsx",no_argument,0,0}, /* [flg] Select intersection of specified groups and variables */
    {"union",no_argument,0,0}, /* [flg] Select union of specified groups and variables */
    {"unn",no_argument,0,0}, /* [flg] Select union of specified groups and variables */
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
    {"coords",no_argument,0,'c'},
    {"crd",no_argument,0,'c'},
    {"xtr_ass_var",no_argument,0,'c'},
    {"xcl_ass_var",no_argument,0,'C'},
    {"no_coords",no_argument,0,'C'},
    {"no_crd",no_argument,0,'C'},
    {"dbg_lvl",required_argument,0,'D'},
    {"debug",required_argument,0,'D'},
    {"nco_dbg_lvl",required_argument,0,'D'},
    {"dimension",required_argument,0,'d'},
    {"dmn",required_argument,0,'d'},
    {"fortran",no_argument,0,'F'},
    {"ftn",no_argument,0,'F'},
    {"gpe",required_argument,0,'G'}, /* [sng] Group Path Edit (GPE) */
    {"grp",required_argument,0,'g'},
    {"group",required_argument,0,'g'},
    {"history",no_argument,0,'h'},
    {"hst",no_argument,0,'h'},
    {"interpolate",required_argument,0,'i'},
    {"ntp",required_argument,0,'i'},
    {"dfl_lvl",required_argument,0,'L'}, /* [enm] Deflate level */
    {"deflate",required_argument,0,'L'}, /* [enm] Deflate level */
    {"local",required_argument,0,'l'},
    {"lcl",required_argument,0,'l'},
    {"nrm",no_argument,0,'N'}, /* [flg] Normalize input weights so w1:=w1/(w1+w2), w2:=w2/(w1+w2) */
    {"normalize",no_argument,0,'N'}, /* [flg] Normalize input weights so w1:=w1/(w1+w2), w2:=w2/(w1+w2) */
    {"overwrite",no_argument,0,'O'},
    {"ovr",no_argument,0,'O'},
    {"output",required_argument,0,'o'},
    {"fl_out",required_argument,0,'o'},
    {"path",required_argument,0,'p'},
    {"retain",no_argument,0,'R'},
    {"rtn",no_argument,0,'R'},
    {"revision",no_argument,0,'r'},
    {"thr_nbr",required_argument,0,'t'},
    {"threads",required_argument,0,'t'},
    {"omp_num_threads",required_argument,0,'t'},
    {"variable",required_argument,0,'v'},
    {"weight",required_argument,0,'w'},
    {"wgt_var",required_argument,0,'w'},
    {"auxiliary",required_argument,0,'X'},
    {"exclude",no_argument,0,'x'},
    {"xcl",no_argument,0,'x'},
    {0,0,0,0}
  }; /* end opt_lng */
  int opt_idx=0; /* Index of current long option into opt_lng array */
  
  /* Start timer and save command line */ 
  ddra_info.tmr_flg=nco_tmr_srt;
  rcd+=nco_ddra((char *)NULL,(char *)NULL,&ddra_info);
  ddra_info.tmr_flg=nco_tmr_mtd;
  cmd_ln=nco_cmd_ln_sng(argc,argv);
  
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
      } /* endif cnk */
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
      if(!strcmp(opt_crr,"cll_msr") || !strcmp(opt_crr,"cell_measures")) EXTRACT_CLL_MSR=True; /* [flg] Extract cell_measures variables */
      if(!strcmp(opt_crr,"no_cll_msr") || !strcmp(opt_crr,"no_cell_measures")) EXTRACT_CLL_MSR=False; /* [flg] Do not extract cell_measures variables */
      if(!strcmp(opt_crr,"frm_trm") || !strcmp(opt_crr,"formula_terms")) EXTRACT_FRM_TRM=True; /* [flg] Extract formula_terms variables */
      if(!strcmp(opt_crr,"no_frm_trm") || !strcmp(opt_crr,"no_formula_terms")) EXTRACT_FRM_TRM=False; /* [flg] Do not extract formula_terms variables */
      if(!strcmp(opt_crr,"mmr_cln") || !strcmp(opt_crr,"clean")) flg_mmr_cln=True; /* [flg] Clean memory prior to exit */
      if(!strcmp(opt_crr,"drt") || !strcmp(opt_crr,"mmr_drt") || !strcmp(opt_crr,"dirty")) flg_mmr_cln=False; /* [flg] Clean memory prior to exit */
      if(!strcmp(opt_crr,"fix_rec_crd")) FIX_REC_CRD=True; /* [flg] Do not interpolate/multiply record coordinate variables */
      if(!strcmp(opt_crr,"fl_fmt") || !strcmp(opt_crr,"file_format")) rcd=nco_create_mode_prs(optarg,&fl_out_fmt);
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
      if(!strcmp(opt_crr,"msa_usr_rdr") || !strcmp(opt_crr,"msa_user_order")) MSA_USR_RDR=True; /* [flg] Multi-Slab Algorithm returns hyperslabs in user-specified order */
      if(!strcmp(opt_crr,"ppc") || !strcmp(opt_crr,"precision_preserving_compression") || !strcmp(opt_crr,"quantize")){
        ppc_arg[ppc_nbr]=(char *)strdup(optarg);
        ppc_nbr++;
      } /* endif "ppc" */
      if(!strcmp(opt_crr,"ram_all") || !strcmp(opt_crr,"create_ram") || !strcmp(opt_crr,"diskless_all")) RAM_CREATE=True; /* [flg] Create (netCDF3) file(s) in RAM */
      if(!strcmp(opt_crr,"ram_all") || !strcmp(opt_crr,"open_ram") || !strcmp(opt_crr,"diskless_all")) RAM_OPEN=True; /* [flg] Open (netCDF3) file(s) in RAM */
      if(!strcmp(opt_crr,"share_all") || !strcmp(opt_crr,"unbuffered_io") || !strcmp(opt_crr,"uio") || !strcmp(opt_crr,"create_share")) SHARE_CREATE=True; /* [flg] Create (netCDF3) file(s) with unbuffered I/O */
      if(!strcmp(opt_crr,"share_all") || !strcmp(opt_crr,"unbuffered_io") || !strcmp(opt_crr,"uio") || !strcmp(opt_crr,"open_share")) SHARE_OPEN=True; /* [flg] Open (netCDF3) file(s) with unbuffered I/O */
      if(!strcmp(opt_crr,"unn") || !strcmp(opt_crr,"union")) GRP_VAR_UNN=True;
      if(!strcmp(opt_crr,"nsx") || !strcmp(opt_crr,"intersection")) GRP_VAR_UNN=False;
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
    case 'C': /* Extract all coordinates associated with extracted variables? */
      EXTRACT_ASSOCIATED_COORDINATES=False;
      break;
    case 'c':
      EXTRACT_ALL_COORDINATES=True;
      break;
    case 'D': /* The debugging level. Default is 0. */
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
    case 'i':
      /* Name of variable to guide interpolation. Default is none */
      ntp_lst_in=nco_lst_prs_2D(optarg,",",&nbr_ntp);
      if(nbr_ntp > 2){
        (void)fprintf(stdout,"%s: ERROR too many arguments to -i\n",nco_prg_nm_get());
        nco_exit(EXIT_FAILURE);
      } /* end if */
      ntp_nm=ntp_lst_in[0];
      ntp_val_out=strtod(ntp_lst_in[1],&sng_cnv_rcd);
      if(*sng_cnv_rcd) nco_sng_cnv_err(ntp_lst_in[1],"strtod",sng_cnv_rcd);
      CMD_LN_NTP_VAR=True;
      CMD_LN_NTP_WGT=False;
      break;
    case 'L': /* [enm] Deflate level. Default is 0. */
      dfl_lvl=(int)strtol(optarg,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
      if(*sng_cnv_rcd) nco_sng_cnv_err(optarg,"strtol",sng_cnv_rcd);
      break;
    case 'l': /* Local path prefix for files retrieved from remote file system */
      fl_pth_lcl=(char *)strdup(optarg);
      break;
    case 'N': /* Toggle NRM_WGT */
      NRM_WGT=!NRM_WGT;
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
    case 'w':
      /* Weight(s) for interpolation.  Default is wgt_val_1=wgt_val_2=0.5 */
      ntp_lst_in=nco_lst_prs_2D(optarg,",",&nbr_ntp);
      if(nbr_ntp > 2){
        (void)fprintf(stdout,"%s: ERROR too many arguments to -w\n",nco_prg_nm_get());
        nco_exit(EXIT_FAILURE);
      }else if(nbr_ntp == 2){
        wgt_val_1=strtod(ntp_lst_in[0],&sng_cnv_rcd);
        if(*sng_cnv_rcd) nco_sng_cnv_err(ntp_lst_in[0],"strtod",sng_cnv_rcd);
        wgt_val_2=strtod(ntp_lst_in[1],&sng_cnv_rcd);
        if(*sng_cnv_rcd) nco_sng_cnv_err(ntp_lst_in[1],"strtod",sng_cnv_rcd);
      }else if(nbr_ntp == 1){
        wgt_val_1=strtod(ntp_lst_in[0],&sng_cnv_rcd);
        if(*sng_cnv_rcd) nco_sng_cnv_err(ntp_lst_in[0],"strtod",sng_cnv_rcd);
        wgt_val_2=1.0-wgt_val_1;
      } /* end else */
      CMD_LN_NTP_WGT=True;
      break;
    case 'X': /* Copy auxiliary coordinate argument for later processing */
      aux_arg[aux_nbr]=(char *)strdup(optarg);
      aux_nbr++;
      MSA_USR_RDR=True; /* [flg] Multi-Slab Algorithm returns hyperslabs in user-specified order */      
      break;
    case 'x': /* Exclude rather than extract variables specified with -v */
      EXCLUDE_INPUT_LIST=True;
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

  if(CMD_LN_NTP_VAR && CMD_LN_NTP_WGT){
    (void)fprintf(stdout,"%s: ERROR interpolating variable (-i) and fixed weight(s) (-w) both set\n",nco_prg_nm_get());
    nco_exit(EXIT_FAILURE);
  }else if(!CMD_LN_NTP_VAR && !CMD_LN_NTP_WGT){
    (void)fprintf(stdout,"%s: ERROR interpolating variable (-i) or fixed weight(s) (-w) must be set\n",nco_prg_nm_get());
    nco_exit(EXIT_FAILURE);
  } /* end else */

  /* Process positional arguments and fill-in filenames */
  fl_lst_in=nco_fl_lst_mk(argv,argc,optind,&fl_nbr,&fl_out,&FL_LST_IN_FROM_STDIN,FORCE_OVERWRITE);

  /* Initialize thread information */
  thr_nbr=nco_openmp_ini(thr_nbr);
  in_id_1_arr=(int *)nco_malloc(thr_nbr*sizeof(int));
  in_id_2_arr=(int *)nco_malloc(thr_nbr*sizeof(int));

  /* Parse filenames */
  fl_idx=0; /* Input file _1 */
  fl_in_1=nco_fl_nm_prs(fl_in_1,fl_idx,&fl_nbr,fl_lst_in,abb_arg_nbr,fl_lst_abb,fl_pth);
  if(nco_dbg_lvl >= nco_dbg_fl) (void)fprintf(stderr,"%s: INFO Input file %d is %s",nco_prg_nm_get(),fl_idx,fl_in_1);
  /* Make sure file is on local system and is readable or die trying */
  fl_in_1=nco_fl_mk_lcl(fl_in_1,fl_pth_lcl,HPSS_TRY,&FILE_1_RETRIEVED_FROM_REMOTE_LOCATION);
  if(nco_dbg_lvl >= nco_dbg_fl && FILE_1_RETRIEVED_FROM_REMOTE_LOCATION) (void)fprintf(stderr,", local file is %s",fl_in_1);
  if(nco_dbg_lvl >= nco_dbg_fl) (void)fprintf(stderr,"\n");
  /* Open file once per thread to improve caching */
  if(RAM_OPEN) md_open=NC_NOWRITE|NC_DISKLESS; else md_open=NC_NOWRITE;
  if(SHARE_OPEN) md_open=md_open|NC_SHARE;
  for(thr_idx=0;thr_idx<thr_nbr;thr_idx++) rcd+=nco_fl_open(fl_in_1,md_open,&bfr_sz_hnt,in_id_1_arr+thr_idx);
  in_id_1=in_id_1_arr[0];

  fl_idx=1; /* Input file _2 */
  fl_in_2=nco_fl_nm_prs(fl_in_2,fl_idx,&fl_nbr,fl_lst_in,abb_arg_nbr,fl_lst_abb,fl_pth);
  if(nco_dbg_lvl >= nco_dbg_fl) (void)fprintf(stderr,"%s: INFO Input file %d is %s",nco_prg_nm_get(),fl_idx,fl_in_2);
  /* Make sure file is on local system and is readable or die trying */
  fl_in_2=nco_fl_mk_lcl(fl_in_2,fl_pth_lcl,HPSS_TRY,&FILE_2_RETRIEVED_FROM_REMOTE_LOCATION);
  if(nco_dbg_lvl >= nco_dbg_fl && FILE_2_RETRIEVED_FROM_REMOTE_LOCATION) (void)fprintf(stderr,", local file is %s",fl_in_2);
  if(nco_dbg_lvl >= nco_dbg_fl) (void)fprintf(stderr,"\n");
  /* Open file once per thread to improve caching */
  if(RAM_OPEN) md_open=NC_NOWRITE|NC_DISKLESS; else md_open=NC_NOWRITE;
  if(SHARE_OPEN) md_open=md_open|NC_SHARE;
  for(thr_idx=0;thr_idx<thr_nbr;thr_idx++) rcd+=nco_fl_open(fl_in_2,md_open,&bfr_sz_hnt,in_id_2_arr+thr_idx);
  in_id_2=in_id_2_arr[0];

  (void)nco_inq_format(in_id_1,&fl_in_fmt_1);
  (void)nco_inq_format(in_id_2,&fl_in_fmt_2);
 
  trv_tbl_init(&trv_tbl);

  /* Construct GTT, Group Traversal Table (groups,variables,dimensions, limits) */
  (void)nco_bld_trv_tbl(in_id_1,trv_pth,lmt_nbr,lmt_arg,aux_nbr,aux_arg,MSA_USR_RDR,FORTRAN_IDX_CNV,grp_lst_in,grp_lst_in_nbr,var_lst_in,xtr_nbr,EXTRACT_ALL_COORDINATES,GRP_VAR_UNN,False,EXCLUDE_INPUT_LIST,EXTRACT_ASSOCIATED_COORDINATES,EXTRACT_CLL_MSR,EXTRACT_FRM_TRM,nco_pck_plc_nil,&flg_dne,trv_tbl);

  /* Were all user-specified dimensions found? */ 
  (void)nco_chk_dmn(lmt_nbr,flg_dne);     

  /* Get number of variables, dimensions, and global attributes in file, file format */
  (void)trv_tbl_inq((int *)NULL,(int *)NULL,(int *)NULL,&nbr_dmn_fl,(int *)NULL,(int *)NULL,(int *)NULL,(int *)NULL,&nbr_var_fl,trv_tbl);

  /* Fill-in variable structure list for all extracted variables */
  var=nco_fll_var_trv(in_id_1,&xtr_nbr,trv_tbl);

  var_out=(var_sct **)nco_malloc(xtr_nbr*sizeof(var_sct *));
  for(int var_idx=0;var_idx<xtr_nbr;var_idx++)
    var_out[var_idx]=nco_var_dpl(var[var_idx]);

  /* Determine conventions (ARM/CCM/CCSM/CF/MPAS) for treating file */
  cnv=nco_cnv_ini(in_id_1);

  /* Divide variable lists into lists of fixed variables and variables to be processed */
  (void)nco_var_lst_dvd(var,var_out,xtr_nbr,cnv,FIX_REC_CRD,nco_pck_plc_nil,nco_pck_map_nil,(dmn_sct **)NULL,0,&var_fix,&var_fix_out,&nbr_var_fix,&var_prc_1,&var_prc_out,&nbr_var_prc,trv_tbl);

  /* Store processed and fixed variables info into GTT */
  (void)nco_var_prc_fix_trv(nbr_var_prc,var_prc_1,nbr_var_fix,var_fix,trv_tbl);

  /* Make output and input files consanguinous */
  if(fl_out_fmt == NCO_FORMAT_UNDEFINED) fl_out_fmt=fl_in_fmt_1;

  /* Initialize, decode, and set PPC information */
  if(ppc_nbr > 0) nco_ppc_ini(in_id_1,&dfl_lvl,fl_out_fmt,ppc_arg,ppc_nbr,trv_tbl);

  /* Verify output file format supports requested actions */
  (void)nco_fl_fmt_vet(fl_out_fmt,cnk_nbr,dfl_lvl);

  /* Open output file */
  fl_out_tmp=nco_fl_out_open(fl_out,&FORCE_APPEND,FORCE_OVERWRITE,fl_out_fmt,&bfr_sz_hnt,RAM_CREATE,RAM_OPEN,SHARE_CREATE,SHARE_OPEN,WRT_TMP_FL,&out_id);

  /* Initialize chunking from user-specified inputs */
  if(fl_out_fmt == NC_FORMAT_NETCDF4 || fl_out_fmt == NC_FORMAT_NETCDF4_CLASSIC) rcd+=nco_cnk_ini(in_id_1,fl_out,cnk_arg,cnk_nbr,cnk_map,cnk_plc,cnk_csh_byt,cnk_min_byt,cnk_sz_byt,cnk_sz_scl,&cnk);

  /* Transfer variable type to table. NOTE: Using var/xtr_nbr containing all variables (processed, fixed) */
  (void)nco_var_typ_trv(xtr_nbr,var,trv_tbl);         

  /* Define dimensions, extracted groups, variables, and attributes in output file */
  (void)nco_xtr_dfn(in_id_1,out_id,&cnk,dfl_lvl,gpe,md5,!FORCE_APPEND,True,False,nco_pck_plc_nil,(char *)NULL,trv_tbl);

  /* Catenate time-stamped command line to "history" global attribute */
  if(HISTORY_APPEND) (void)nco_hst_att_cat(out_id,cmd_ln);
  if(HISTORY_APPEND && FORCE_APPEND) (void)nco_prv_att_cat(fl_in_1,in_id_1,out_id);
  if(gaa_nbr > 0) (void)nco_glb_att_add(out_id,gaa_arg,gaa_nbr);
  if(HISTORY_APPEND) (void)nco_vrs_att_cat(out_id);
  if(thr_nbr > 1 && HISTORY_APPEND) (void)nco_thr_att_cat(out_id,thr_nbr);

  /* Turn-off default filling behavior to enhance efficiency */
  nco_set_fill(out_id,NC_NOFILL,&fll_md_old);

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
  (void)nco_cpy_fix_var_trv(in_id_1,out_id,gpe,trv_tbl);  

  /* Perform various error-checks on input file */
  if(False) (void)nco_fl_cmp_err_chk();

  /* ncflint-specific stuff: */
  /* Find the weighting variable in input file */
  if(CMD_LN_NTP_VAR){
    var_sct *ntp_1=NULL;
    var_sct *ntp_2=NULL;
    var_sct *ntp_var_out;

    /* Turn arrival point into pseudo-variable */
    val_gnr_unn.d=ntp_val_out; /* Generic container for arrival point or weight */
    ntp_var_out=scl_mk_var(val_gnr_unn,NC_DOUBLE);

    int xtr_nbr_ntp_1;
    int xtr_nbr_ntp_2;

    var_sct ** var_ntp_1;
    var_sct ** var_ntp_2; 

    /* Fill-in variable structure list for all variables named "ntp_nm" NOTE: using table from file 1 */
    var_ntp_1=nco_var_trv(in_id_1,ntp_nm,&xtr_nbr_ntp_1,trv_tbl);
    var_ntp_2=nco_var_trv(in_id_2,ntp_nm,&xtr_nbr_ntp_2,trv_tbl);

    if (xtr_nbr_ntp_1) ntp_1=var_ntp_1[0];
    if (xtr_nbr_ntp_2) ntp_2=var_ntp_2[0];

    if(!xtr_nbr_ntp_1 || !xtr_nbr_ntp_2){
      (void)fprintf(fp_stdout,"%s: ERROR Variable %s is not present in input file. ncflint assumes same file structure for both input files\n",nco_prg_nm_get(),ntp_nm);
      nco_exit(EXIT_FAILURE);
    } /* endif */
    
    /* Read */
    (void)nco_msa_var_get_trv(in_id_1,ntp_1,trv_tbl);
    (void)nco_msa_var_get_trv(in_id_2,ntp_2,trv_tbl);

    /* Currently, only support scalar variables */
    if(ntp_1->sz > 1 || ntp_2->sz > 1){
      (void)fprintf(stdout,"%s: ERROR interpolation variable %s must be scalar\n",nco_prg_nm_get(),ntp_nm);
      nco_exit(EXIT_FAILURE);
    } /* end if */

    /* Weights must be NC_DOUBLE */
    ntp_1=nco_var_cnf_typ((nc_type)NC_DOUBLE,ntp_1);
    ntp_2=nco_var_cnf_typ((nc_type)NC_DOUBLE,ntp_2);

    /* Check for degenerate case */
    if(ntp_1->val.dp[0] == ntp_2->val.dp[0]){
      (void)fprintf(stdout,"%s: ERROR Interpolation variable %s is identical (%g) in input files, therefore unable to interpolate.\n",nco_prg_nm_get(),ntp_nm,ntp_1->val.dp[0]);
      nco_exit(EXIT_FAILURE);
    } /* end if */

    /* Turn weights into pseudo-variables */
    wgt_1=nco_var_dpl(ntp_2);
    wgt_2=nco_var_dpl(ntp_var_out);

    /* Subtract to find interpolation distances */
    (void)nco_var_sbt(ntp_1->type,ntp_1->sz,ntp_1->has_mss_val,ntp_1->mss_val,ntp_var_out->val,wgt_1->val);
    (void)nco_var_sbt(ntp_1->type,ntp_1->sz,ntp_1->has_mss_val,ntp_1->mss_val,ntp_1->val,wgt_2->val);
    (void)nco_var_sbt(ntp_1->type,ntp_1->sz,ntp_1->has_mss_val,ntp_1->mss_val,ntp_1->val,ntp_2->val);

    /* Normalize to obtain final interpolation weights */
    (void)nco_var_dvd(wgt_1->type,wgt_1->sz,wgt_1->has_mss_val,wgt_1->mss_val,ntp_2->val,wgt_1->val);
    (void)nco_var_dvd(wgt_2->type,wgt_2->sz,wgt_2->has_mss_val,wgt_2->mss_val,ntp_2->val,wgt_2->val);

    for(idx=0;idx<xtr_nbr_ntp_1;idx++) var_ntp_1[idx]=nco_var_free(var_ntp_1[idx]);
    for(idx=0;idx<xtr_nbr_ntp_2;idx++) var_ntp_2[idx]=nco_var_free(var_ntp_2[idx]);
    var_ntp_1=(var_sct **)nco_free(var_ntp_1);
    var_ntp_2=(var_sct **)nco_free(var_ntp_2);

    if(ntp_var_out) ntp_var_out=nco_var_free(ntp_var_out);
  } /* end if CMD_LN_NTP_VAR */

  if(CMD_LN_NTP_WGT){
    if(NRM_WGT){
      /* Normalize input weights so w1:=w1/(w1+w2), w2:=w2/(w1+w2) */
      wgt_val_1=wgt_val_1/(wgt_val_1+wgt_val_2);
      wgt_val_2=wgt_val_2/(wgt_val_1+wgt_val_2);
    } /* !NRM_WGT */
    val_gnr_unn.d=wgt_val_1; /* Generic container for arrival point or weight */
    wgt_1=scl_mk_var(val_gnr_unn,NC_DOUBLE);
    val_gnr_unn.d=wgt_val_2; /* Generic container for arrival point or weight */
    wgt_2=scl_mk_var(val_gnr_unn,NC_DOUBLE);
  } /* end if CMD_LN_NTP_WGT */

  if(nco_dbg_lvl >= nco_dbg_scl) (void)fprintf(stderr,"wgt_1 = %g, wgt_2 = %g\n",wgt_1->val.dp[0],wgt_2->val.dp[0]);

  /* Create structure list for second file */
  var_prc_2=(var_sct **)nco_malloc(nbr_var_prc*sizeof(var_sct *));

  /* Timestamp end of metadata setup and disk layout */
  rcd+=nco_ddra((char *)NULL,(char *)NULL,&ddra_info);
  ddra_info.tmr_flg=nco_tmr_rgl;

  /* Loop over each interpolated variable */
#ifdef _OPENMP
  /* OpenMP notes:
     shared(): msk and wgt are not altered within loop
     private(): wgt_avg does not need initialization */
# pragma omp parallel for firstprivate(wgt_1,wgt_2,wgt_out_1,wgt_out_2) private(DO_CONFORM,idx,in_id_1,in_id_2,has_mss_val) shared(MUST_CONFORM,nco_dbg_lvl,fl_in_1,fl_in_2,fl_out,gpe,in_id_1_arr,in_id_2_arr,nbr_var_prc,out_id,nco_prg_nm,var_prc_1,var_prc_2,var_prc_out,nbr_dmn_fl,trv_tbl)
#endif /* !_OPENMP */
  for(idx=0;idx<nbr_var_prc;idx++){
    
    /* Note: Using object 2 from table 1, only one table built, assumes same structure for processed objects in both files */

    char *grp_out_fll=NULL; /* [sng] Group name */

    int grp_id_1;      /* [ID] Group ID */
    int grp_id_2;      /* [ID] Group ID */
    int grp_out_id;    /* [ID] Group ID (output) */
    int var_out_id;    /* [ID] Variable ID (output) */

    trv_sct *var_trv_1;/* [sct] Variable GTT object */
    trv_sct *var_trv_2;/* [sct] Variable GTT object */

    if(nco_dbg_lvl >= nco_dbg_var) (void)fprintf(fp_stderr,"%s, ",var_prc_1[idx]->nm);
    if(nco_dbg_lvl >= nco_dbg_var) (void)fflush(fp_stderr);

    in_id_1=in_id_1_arr[omp_get_thread_num()];
    in_id_2=in_id_2_arr[omp_get_thread_num()];

    var_prc_2[idx]=nco_var_dpl(var_prc_1[idx]);

    /* Obtain variable GTT object using full variable name */
    var_trv_1=trv_tbl_var_nm_fll(var_prc_1[idx]->nm_fll,trv_tbl);
    var_trv_2=trv_tbl_var_nm_fll(var_prc_2[idx]->nm_fll,trv_tbl);

    if(!var_trv_2){
      (void)fprintf(fp_stdout,"%s: ERROR Variable %s is not present in second input file. ncflint assumes same structure for processed objects in both files\n",nco_prg_nm_get(),var_trv_1->nm_fll);
      nco_exit(EXIT_FAILURE);
    } /* endif */

    /* Obtain group ID */
    (void)nco_inq_grp_full_ncid(in_id_1,var_trv_1->grp_nm_fll,&grp_id_1);
    (void)nco_inq_grp_full_ncid(in_id_2,var_trv_2->grp_nm_fll,&grp_id_2);

    (void)nco_var_mtd_refresh(grp_id_2,var_prc_2[idx]);

    /* Read */
    (void)nco_msa_var_get_trv(in_id_1,var_prc_1[idx],trv_tbl);
    (void)nco_msa_var_get_trv(in_id_2,var_prc_2[idx],trv_tbl);

    /* Set var_prc_1 and var_prc_2 to correct size */
    var_prc_1[idx]->sz=var_prc_out[idx]->sz;       
    var_prc_2[idx]->sz=var_prc_out[idx]->sz;  

    /* Stretch second variable to match dimensions of first variable */
    wgt_out_1=nco_var_cnf_dmn(var_prc_out[idx],wgt_1,wgt_out_1,MUST_CONFORM,&DO_CONFORM);
    wgt_out_2=nco_var_cnf_dmn(var_prc_out[idx],wgt_2,wgt_out_2,MUST_CONFORM,&DO_CONFORM);

    var_prc_1[idx]=nco_var_cnf_typ((nc_type)NC_DOUBLE,var_prc_1[idx]);
    var_prc_2[idx]=nco_var_cnf_typ((nc_type)NC_DOUBLE,var_prc_2[idx]);

    /* Allocate and, if necesssary, initialize space for processed variable */
    var_prc_out[idx]->sz=var_prc_1[idx]->sz;

    /* NB: must not try to free() same tally buffer twice */
    /* var_prc_out[idx]->tally=var_prc_1[idx]->tally=(long *)nco_malloc(var_prc_out[idx]->sz*sizeof(long int));*/
    var_prc_out[idx]->tally=(long *)nco_malloc(var_prc_out[idx]->sz*sizeof(long int));
    (void)nco_zero_long(var_prc_out[idx]->sz,var_prc_out[idx]->tally);

    /* Weight variable by taking product of weight with variable */
    (void)nco_var_mlt(var_prc_1[idx]->type,var_prc_1[idx]->sz,var_prc_1[idx]->has_mss_val,var_prc_1[idx]->mss_val,wgt_out_1->val,var_prc_1[idx]->val);
    (void)nco_var_mlt(var_prc_2[idx]->type,var_prc_2[idx]->sz,var_prc_2[idx]->has_mss_val,var_prc_2[idx]->mss_val,wgt_out_2->val,var_prc_2[idx]->val);

    /* Change missing_value of var_prc_2, if any, to missing_value of var_prc_1, if any */
    has_mss_val=nco_mss_val_cnf(var_prc_1[idx],var_prc_2[idx]);

    /* NB: fxm: use tally to determine when to "unweight" answer? TODO */
    (void)nco_var_add_tll_ncflint(var_prc_1[idx]->type,var_prc_1[idx]->sz,has_mss_val,var_prc_1[idx]->mss_val,var_prc_out[idx]->tally,var_prc_1[idx]->val,var_prc_2[idx]->val);

    /* Re-cast output variable to original type */
    var_prc_2[idx]=nco_var_cnf_typ(var_prc_out[idx]->type,var_prc_2[idx]);

    /* Edit group name for output */
    if(gpe) grp_out_fll=nco_gpe_evl(gpe,var_trv_1->grp_nm_fll); else grp_out_fll=(char *)strdup(var_trv_1->grp_nm_fll);

    /* Obtain output group ID */
    (void)nco_inq_grp_full_ncid(out_id,grp_out_fll,&grp_out_id);

    /* Memory management after current extracted group */
    if(grp_out_fll) grp_out_fll=(char *)nco_free(grp_out_fll);

    /* Get variable ID */
    (void)nco_inq_varid(grp_out_id,var_trv_1->nm,&var_out_id);

    /* Store the output variable ID */
    var_prc_out[idx]->id=var_out_id;

    if(var_trv_1->ppc != NC_MAX_INT){
    if(var_trv_1->flg_nsd) (void)nco_ppc_bitmask(var_trv_1->ppc,var_prc_2[idx]->type,var_prc_out[idx]->sz,var_prc_out[idx]->has_mss_val,var_prc_out[idx]->mss_val,var_prc_2[idx]->val); else (void)nco_ppc_around(var_trv_1->ppc,var_prc_2[idx]->type,var_prc_out[idx]->sz,var_prc_out[idx]->has_mss_val,var_prc_out[idx]->mss_val,var_prc_2[idx]->val);
    } /* endif ppc */
    if(nco_is_xcp(var_trv_1->nm)) nco_xcp_prc(var_trv_1->nm,var_prc_2[idx]->type,var_prc_out[idx]->sz,(char *)var_prc_2[idx]->val.vp);
#ifdef _OPENMP
# pragma omp critical
#endif /* _OPENMP */
    { /* begin OpenMP critical */
      /* Copy interpolations to output file */
      if(!var_prc_out[idx]->nbr_dim) (void)nco_put_var1(grp_out_id,var_prc_out[idx]->id,var_prc_out[idx]->srt,var_prc_2[idx]->val.vp,var_prc_2[idx]->type); else (void)nco_put_vara(grp_out_id,var_prc_out[idx]->id,var_prc_out[idx]->srt,var_prc_out[idx]->cnt,var_prc_2[idx]->val.vp,var_prc_2[idx]->type);
    } /* end OpenMP critical */

    /* Free dynamically allocated buffers */
    if(var_prc_1[idx]) var_prc_1[idx]=nco_var_free(var_prc_1[idx]);
    if(var_prc_2[idx]) var_prc_2[idx]=nco_var_free(var_prc_2[idx]);
    if(var_prc_out[idx]) var_prc_out[idx]=nco_var_free(var_prc_out[idx]);

  } /* end (OpenMP parallel for) loop over idx */
  if(nco_dbg_lvl >= nco_dbg_var) (void)fprintf(stderr,"\n");

  /* Close input netCDF files */
  for(thr_idx=0;thr_idx<thr_nbr;thr_idx++) nco_close(in_id_1_arr[thr_idx]);
  for(thr_idx=0;thr_idx<thr_nbr;thr_idx++) nco_close(in_id_2_arr[thr_idx]);

  /* Close output file and move it from temporary to permanent location */
  (void)nco_fl_out_cls(fl_out,fl_out_tmp,out_id);

  /* Remove local copy of file */
  if(FILE_1_RETRIEVED_FROM_REMOTE_LOCATION && RM_RMT_FL_PST_PRC) (void)nco_fl_rm(fl_in_1);
  if(FILE_2_RETRIEVED_FROM_REMOTE_LOCATION && RM_RMT_FL_PST_PRC) (void)nco_fl_rm(fl_in_2);

  /* Clean memory unless dirty memory allowed */
  if(flg_mmr_cln){
    /* ncflint-specific memory */
    if(fl_in_1) fl_in_1=(char *)nco_free(fl_in_1);
    if(fl_in_2) fl_in_2=(char *)nco_free(fl_in_2);
    var_prc_1=(var_sct **)nco_free(var_prc_1);
    var_prc_2=(var_sct **)nco_free(var_prc_2);
    if(wgt_1) wgt_1=(var_sct *)nco_var_free(wgt_1);
    if(wgt_2) wgt_2=(var_sct *)nco_var_free(wgt_2);
    if(wgt_out_1) wgt_out_1=(var_sct *)nco_var_free(wgt_out_1);
    if(wgt_out_2) wgt_out_2=(var_sct *)nco_var_free(wgt_out_2);

    /* NCO-generic clean-up */
    /* Free individual strings/arrays */
    if(cmd_ln) cmd_ln=(char *)nco_free(cmd_ln);
    if(cnk_map_sng) cnk_map_sng=(char *)nco_free(cnk_map_sng);
    if(cnk_plc_sng) cnk_plc_sng=(char *)nco_free(cnk_plc_sng);
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
    if(gaa_nbr > 0) gaa_arg=nco_sng_lst_free(gaa_arg,gaa_nbr);
    if(var_lst_in_nbr > 0) var_lst_in=nco_sng_lst_free(var_lst_in,var_lst_in_nbr);
    /* Free limits */
    for(idx=0;idx<aux_nbr;idx++) aux_arg[idx]=(char *)nco_free(aux_arg[idx]);
    for(idx=0;idx<lmt_nbr;idx++) lmt_arg[idx]=(char *)nco_free(lmt_arg[idx]);
    for(idx=0;idx<ppc_nbr;idx++) ppc_arg[idx]=(char *)nco_free(ppc_arg[idx]);
    /* Free chunking information */
    for(idx=0;idx<cnk_nbr;idx++) cnk_arg[idx]=(char *)nco_free(cnk_arg[idx]);
    if(cnk_nbr > 0 && (fl_out_fmt == NC_FORMAT_NETCDF4 || fl_out_fmt == NC_FORMAT_NETCDF4_CLASSIC)) cnk.cnk_dmn=(cnk_dmn_sct **)nco_cnk_lst_free(cnk.cnk_dmn,cnk_nbr);
    var=(var_sct **)nco_free(var);
    var_out=(var_sct **)nco_free(var_out);
    var_prc_out=(var_sct **)nco_free(var_prc_out);
    if(nbr_var_fix > 0) var_fix=nco_var_lst_free(var_fix,nbr_var_fix);
    if(nbr_var_fix > 0) var_fix_out=nco_var_lst_free(var_fix_out,nbr_var_fix);
    trv_tbl_free(trv_tbl); 
    for(idx=0;idx<lmt_nbr;idx++) flg_dne[idx].dim_nm=(char *)nco_free(flg_dne[idx].dim_nm);
    if(flg_dne) flg_dne=(nco_dmn_dne_t *)nco_free(flg_dne);
    if(gpe) gpe=(gpe_sct *)nco_gpe_free(gpe);
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
