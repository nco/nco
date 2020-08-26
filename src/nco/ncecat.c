/* $Header$ */

/* ncecat -- netCDF ensemble concatenator */

/* Purpose: Join variables across files with new record variable or aggregate files as groups */

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

/* URL: https://github.com/nco/nco/tree/master/src/nco/ncecat.c

   Usage:
   ncecat -O -D 1 -p ${HOME}/nco/data in_grp.nc in_grp.nc ~/foo.nc
   ncecat -O -D 1 -G ensemble:-1 -p ${HOME}/nco/data in_grp.nc in_grp.nc ~/foo.nc
   ncecat -O -D 1 -G ensemble:1 -p ${HOME}/nco/data in_grp.nc in_grp.nc ~/foo.nc
   ncecat -O -D 1 -G ensemble -p ${HOME}/nco/data in_grp.nc in_grp.nc ~/foo.nc
   ncecat -O -D 1 --gag -p ${HOME}/nco/data h0001.nc h0002.nc ~/foo.nc
   ncecat -O -n 3,4,1 -p ${HOME}/nco/data h0001.nc ~/foo.nc
   ncecat -O -n 3,4,1 -p /ZENDER/tmp -l ${HOME} h0001.nc ~/foo.nc */

#ifdef HAVE_CONFIG_H
# include <config.h> /* Autotools tokens */
#endif /* !HAVE_CONFIG_H */

/* Standard header files */
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

/* Personal headers */
/* #define MAIN_PROGRAM_FILE MUST precede #include libnco.h */
#define MAIN_PROGRAM_FILE
#include "libnco.h" /* netCDF Operator (NCO) library */

int 
main(int argc,char **argv)
{
  cnv_sct *cnv; /* [sct] Convention structure */
  nco_bool CPY_GLB_METADATA=True; /* Option M */
  nco_bool EXCLUDE_INPUT_LIST=False; /* Option c */
  nco_bool EXTRACT_ALL_COORDINATES=False; /* Option c */
  nco_bool EXTRACT_ASSOCIATED_COORDINATES=True; /* Option C */
  nco_bool EXTRACT_CLL_MSR=True; /* [flg] Extract cell_measures variables */
  nco_bool EXTRACT_FRM_TRM=True; /* [flg] Extract formula_terms variables */
  nco_bool FL_RTR_RMT_LCN;
  nco_bool FL_LST_IN_APPEND=True; /* Option H */
  nco_bool FL_LST_IN_FROM_STDIN=False; /* [flg] fl_lst_in comes from stdin */
  nco_bool FORCE_APPEND=False; /* Option A */
  nco_bool FORCE_OVERWRITE=False; /* Option O */
  nco_bool FORTRAN_IDX_CNV=False; /* Option F */
  nco_bool GROUP_AGGREGATE=False; /* Option G */
  nco_bool GRP_VAR_UNN=False; /* [flg] Select union of specified groups and variables */
  nco_bool HISTORY_APPEND=True; /* Option h */
  nco_bool HPSS_TRY=False; /* [flg] Search HPSS for unfound files */
  nco_bool HAVE_LIMITS=False; /* [flg] Are there user limits? (-d) */
  nco_bool MSA_USR_RDR=False; /* [flg] Multi-Slab Algorithm returns hyperslabs in user-specified order */
  nco_bool RAM_CREATE=False; /* [flg] Create file in RAM */
  nco_bool RAM_OPEN=False; /* [flg] Open (netCDF3-only) file(s) in RAM */
  nco_bool SHARE_CREATE=False; /* [flg] Create (netCDF3-only) file(s) with unbuffered I/O */
  nco_bool SHARE_OPEN=False; /* [flg] Open (netCDF3-only) file(s) with unbuffered I/O */
  nco_bool RM_RMT_FL_PST_PRC=True; /* Option R */
  nco_bool USE_MM3_WORKAROUND=False; /* [flg] Faster copy on Multi-record Multi-variable netCDF3 files */
  nco_bool WRT_TMP_FL=True; /* [flg] Write output to temporary file */
  nco_bool RECORD_AGGREGATE=True; /* Option G */
  nco_bool flg_mmr_cln=True; /* [flg] Clean memory prior to exit */

  char **fl_lst_abb=NULL; /* Option a */
  char **fl_lst_in;
  char **gaa_arg=NULL; /* [sng] Global attribute arguments */
  char **grp_lst_in=NULL;
  char **var_lst_in=NULL_CEWI;
  char *aux_arg[NC_MAX_DIMS];
  char *cmd_ln;
  char *cnk_arg[NC_MAX_DIMS];
  char *cnk_map_sng=NULL_CEWI; /* [sng] Chunking map */
  char *cnk_plc_sng=NULL_CEWI; /* [sng] Chunking policy */
  char *fl_in=NULL;
  char *fl_out=NULL; /* Option o */
  char *fl_out_tmp;
  char *fl_pth=NULL; /* Option p */
  char *fl_pth_lcl=NULL; /* Option l */
  char *gpe_arg=NULL; /* [sng] GPE argument */
  char *grp_out=NULL; /* [sng] Group name */
  char *lmt_arg[NC_MAX_DIMS];
  char *opt_crr=NULL; /* [sng] String representation of current long-option name */
  char *optarg_lcl=NULL; /* [sng] Local copy of system optarg */
  char *ppc_arg[NC_MAX_VARS]; /* [sng] PPC arguments */
  char *rec_dmn_nm=NULL; /* [sng] New record dimension name */
  char *sng_cnv_rcd=NULL_CEWI; /* [sng] strtol()/strtoul() return code */

  /* NCO_GRP_OUT_SFX_LNG is number of consecutive numeric digits automatically generated as group name suffixes */
#define	NCO_GRP_OUT_SFX_LNG 2
  char grp_out_sfx[NCO_GRP_OUT_SFX_LNG+1L];
  char trv_pth[]="/"; /* [sng] Root path of traversal tree */

  const char * const CVS_Id="$Id$"; 
  const char * const CVS_Revision="$Revision$";
  const char * const opt_sht_lst="34567ACcD:d:Fg:G:HhL:l:Mn:Oo:p:rRt:u:v:X:x-:";

  cnk_sct cnk; /* [sct] Chunking structure */

#if defined(__cplusplus) || defined(PGI_CC)
  ddra_info_sct ddra_info;
  ddra_info.flg_ddra=False;
#else /* !__cplusplus */
  ddra_info_sct ddra_info={.flg_ddra=False};
#endif /* !__cplusplus */

  extern char *optarg;
  extern int optind;

  /* Using naked stdin/stdout/stderr in parallel region generates warning
  Copy appropriate filehandle to variable scoped shared in parallel clause */
  FILE * const fp_stderr=stderr; /* [fl] stderr filehandle CEWI */
  FILE *fp_bnr=NULL; /* [fl] Unformatted binary output file handle */

  gpe_sct *gpe=NULL; /* [sng] Group Path Editing (GPE) structure */

  int *in_id_arr=NULL;

  int abb_arg_nbr=0;
  int aux_nbr=0; /* [nbr] Number of auxiliary coordinate hyperslabs specified */
  int cnk_map=nco_cnk_map_nil; /* [enm] Chunking map */
  int cnk_nbr=0; /* [nbr] Number of chunk sizes */
  int cnk_plc=nco_cnk_plc_nil; /* [enm] Chunking policy */
  int dfl_lvl=NCO_DFL_LVL_UNDEFINED; /* [enm] Deflate level */
  int fl_idx;
  int fl_nbr=0;
  int fl_in_fmt; /* [enm] Input file format */
  int fl_out_fmt=NCO_FORMAT_UNDEFINED; /* [enm] Output file format */
  int fll_md_old; /* [enm] Old fill mode */
  int gaa_nbr=0; /* [nbr] Number of global attributes to add */
  int gpe_id; /* [id] Group ID of GPE path (diagnostic only) */
  int grp_lst_in_nbr=0; /* [nbr] Number of groups explicitly specified by user */
  int idx;
  int in_id;  
  int lmt_nbr=0; /* Option d. NB: lmt_nbr gets incremented */
  int lmt_nbr_rgn=0; /* Option d. Original limit number for GTT initialization */
  int log_lvl=0; /* [enm] netCDF library debugging verbosity [0..5] */
  int md_open; /* [enm] Mode flag for nc_open() call */
  int nbr_dmn_fl;
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
  int var_idx;
  int var_lst_in_nbr=0;

  long idx_rec_out=0L; /* idx_rec_out gets incremented */

  md5_sct *md5=NULL; /* [sct] MD5 configuration */

  size_t bfr_sz_hnt=NC_SIZEHINT_DEFAULT; /* [B] Buffer size hint */
  size_t cnk_csh_byt=NCO_CNK_CSH_BYT_DFL; /* [B] Chunk cache size */
  size_t cnk_min_byt=NCO_CNK_SZ_MIN_BYT_DFL; /* [B] Minimize size of variable to chunk */
  size_t cnk_sz_byt=0UL; /* [B] Chunk size in bytes */
  size_t cnk_sz_scl=0UL; /* [nbr] Chunk size scalar */
  size_t grp_out_lng; /* [nbr] Length of original, canonicalized GPE specification filename component */
  size_t hdr_pad=0UL; /* [B] Pad at end of header section */

  var_sct **var=NULL;
  var_sct **var_fix;
  var_sct **var_fix_out;
  var_sct **var_out=NULL;
  var_sct **var_prc;
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
    {"gag",no_argument,0,0}, /* [flg] Group aggregation */
    {"aggregate_group",no_argument,0,0}, /* [flg] Group aggregation */
    {"hdf4",no_argument,0,0}, /* [flg] Treat file as HDF4 */
    {"help",no_argument,0,0},
    {"hlp",no_argument,0,0},
    {"hpss_try",no_argument,0,0}, /* [flg] Search HPSS for unfound files */
    {"md5_dgs",no_argument,0,0}, /* [flg] Perform MD5 digests */
    {"md5_digest",no_argument,0,0}, /* [flg] Perform MD5 digests */
    {"mrd",no_argument,0,0}, /* [enm] Multiple Record Dimension convention */
    {"multiple_record_dimension",no_argument,0,0}, /* [enm] Multiple Record Dimension convention */
    {"msa_usr_rdr",no_argument,0,0}, /* [flg] Multi-Slab Algorithm returns hyperslabs in user-specified order */
    {"msa_user_order",no_argument,0,0}, /* [flg] Multi-Slab Algorithm returns hyperslabs in user-specified order */
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
    {"group",required_argument,0,'g'},
    {"grp",required_argument,0,'g'},
    {"fl_lst_in",no_argument,0,'H'},
    {"file_list",no_argument,0,'H'},
    {"history",no_argument,0,'h'},
    {"hst",no_argument,0,'h'},
    {"dfl_lvl",required_argument,0,'L'}, /* [enm] Deflate level */
    {"deflate",required_argument,0,'L'}, /* [enm] Deflate level */
    {"local",required_argument,0,'l'},
    {"lcl",required_argument,0,'l'},
    {"no_glb_mtd",no_argument,0,'M'},
    {"suppress_global_metadata",no_argument,0,'M'},
    {"nintap",required_argument,0,'n'},
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
    {"ulm_nm",required_argument,0,'u'},
    {"rcd_nm",required_argument,0,'u'},
    {"variable",required_argument,0,'v'},
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
      if(!strcmp(opt_crr,"fl_fmt") || !strcmp(opt_crr,"file_format")) rcd=nco_create_mode_prs(optarg,&fl_out_fmt);
      if(!strcmp(opt_crr,"gaa") || !strcmp(opt_crr,"glb_att_add")){
        gaa_arg=(char **)nco_realloc(gaa_arg,(gaa_nbr+1)*sizeof(char *));
        gaa_arg[gaa_nbr++]=(char *)strdup(optarg);
      } /* endif gaa */
      if(!strcmp(opt_crr,"gag") || !strcmp(opt_crr,"aggregate_group")) GROUP_AGGREGATE=True; /* [flg] Aggregate files into groups not records */
      if(!strcmp(opt_crr,"hdf4")) nco_fmt_xtn=nco_fmt_xtn_hdf4; /* [enm] Treat file as HDF4 */
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
      if(!strcmp(opt_crr,"md5_dgs") || !strcmp(opt_crr,"md5_digest")){
        if(!md5) md5=nco_md5_ini();
	md5->dgs=True;
        if(nco_dbg_lvl >= nco_dbg_std) (void)fprintf(stderr,"%s: INFO Will perform MD5 digests of input and output hyperslabs\n",nco_prg_nm_get());
      } /* endif "md5_dgs" */
      if(!strcmp(opt_crr,"mrd") || !strcmp(opt_crr,"multiple_record_dimension")) nco_mrd_cnv=nco_mrd_allow; /* [enm] Multiple Record Dimension convention */
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
    case 'D': /* Debugging level. Default is 0. */
      nco_dbg_lvl=(unsigned short int)strtoul(optarg,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
      if(*sng_cnv_rcd) nco_sng_cnv_err(optarg,"strtoul",sng_cnv_rcd);
      break;
    case 'd': /* Copy limit argument for later processing */
      lmt_arg[lmt_nbr]=(char *)strdup(optarg);
      lmt_nbr++;
      lmt_nbr_rgn=lmt_nbr;
      HAVE_LIMITS=True;
      break;
    case 'F': /* Toggle index convention. Default is 0-based arrays (C-style). */
      FORTRAN_IDX_CNV=!FORTRAN_IDX_CNV;
      break;
    case 'G': /* Apply Group Path Editing (GPE) to output group */
      /* NB: GNU getopt() optional argument syntax is ugly (requires "=" sign) so avoid it
      http://stackoverflow.com/questions/1052746/getopt-does-not-parse-optional-arguments-to-parameters */
      gpe=nco_gpe_prs_arg(optarg);
      grp_out=(char *)strdup(gpe->nm_cnn); /* [sng] Group name */
      grp_out_lng=gpe->lng_cnn;
      GROUP_AGGREGATE=True;
      break;
    case 'g': /* Copy group argument for later processing */
      /* Replace commas with hashes when within braces (convert back later) */
      optarg_lcl=(char *)strdup(optarg);
      (void)nco_rx_comma2hash(optarg_lcl);
      grp_lst_in=nco_lst_prs_2D(optarg_lcl,",",&grp_lst_in_nbr);
      optarg_lcl=(char *)nco_free(optarg_lcl);
      break;
    case 'H': /* Toggle writing input file list attribute */
      FL_LST_IN_APPEND=!FL_LST_IN_APPEND;
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
    case 'M': /* Toggle copying global metadata */
      CPY_GLB_METADATA=!CPY_GLB_METADATA;
      break;
    case 'n': /* NINTAP-style abbreviation of files to process */
      fl_lst_abb=nco_lst_prs_2D(optarg,",",&abb_arg_nbr);
      if(abb_arg_nbr < 1 || abb_arg_nbr > 6){
        (void)fprintf(stdout,"%s: ERROR Incorrect abbreviation for file list\n",nco_prg_nm);
        (void)nco_usg_prn();
        nco_exit(EXIT_FAILURE);
      } /* end if */
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
    case 'u': /* New record dimension name */
      rec_dmn_nm=(char *)strdup(optarg);
      break;
    case 'v': /* Variables to extract/exclude */
      /* Replace commas with hashes when within braces (convert back later) */
      optarg_lcl=(char *)strdup(optarg);
      (void)nco_rx_comma2hash(optarg_lcl);
      var_lst_in=nco_lst_prs_2D(optarg_lcl,",",&var_lst_in_nbr);
      optarg_lcl=(char *)nco_free(optarg_lcl);
      xtr_nbr=var_lst_in_nbr;
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

  trv_tbl_init(&trv_tbl);

  if(GROUP_AGGREGATE){
    RECORD_AGGREGATE=!GROUP_AGGREGATE;
    fl_out_fmt=NC_FORMAT_NETCDF4; 
  } /* !GROUP_AGGREGATE */

  /* Process positional arguments and fill-in filenames */
  fl_lst_in=nco_fl_lst_mk(argv,argc,optind,&fl_nbr,&fl_out,&FL_LST_IN_FROM_STDIN,FORCE_OVERWRITE);

  /* Parse filename */
  fl_in=nco_fl_nm_prs(fl_in,0,&fl_nbr,fl_lst_in,abb_arg_nbr,fl_lst_abb,fl_pth);
  /* Make sure file is on local system and is readable or die trying */
  fl_in=nco_fl_mk_lcl(fl_in,fl_pth_lcl,HPSS_TRY,&FL_RTR_RMT_LCN);
  /* Open file using appropriate buffer size hints and verbosity */
  if(RAM_OPEN) md_open=NC_NOWRITE|NC_DISKLESS; else md_open=NC_NOWRITE;
  if(SHARE_OPEN) md_open=md_open|NC_SHARE;
  rcd+=nco_fl_open(fl_in,md_open,&bfr_sz_hnt,&in_id);

  (void)nco_inq_format(in_id,&fl_in_fmt); 

  if(RECORD_AGGREGATE){

    /* Initialize thread information */
    thr_nbr=nco_openmp_ini(thr_nbr);
    in_id_arr=(int *)nco_malloc(thr_nbr*sizeof(int));

    /* Construct GTT, Group Traversal Table (groups,variables,dimensions, limits) */
    (void)nco_bld_trv_tbl(in_id,trv_pth,lmt_nbr,lmt_arg,aux_nbr,aux_arg,MSA_USR_RDR,FORTRAN_IDX_CNV,grp_lst_in,grp_lst_in_nbr,var_lst_in,xtr_nbr,EXTRACT_ALL_COORDINATES,GRP_VAR_UNN,False,EXCLUDE_INPUT_LIST,EXTRACT_ASSOCIATED_COORDINATES,EXTRACT_CLL_MSR,EXTRACT_FRM_TRM,nco_pck_plc_nil,&flg_dne,trv_tbl);

    /* Were all user-specified dimensions found? */ 
    (void)nco_chk_dmn(lmt_nbr,flg_dne);     

    /* Get number of variables, dimensions, and global attributes in file, file format */
    (void)trv_tbl_inq((int *)NULL,(int *)NULL,(int *)NULL,&nbr_dmn_fl,(int *)NULL,(int *)NULL,(int *)NULL,(int *)NULL,&nbr_var_fl,trv_tbl);

    /* Fill-in variable structure list for all extracted variables */
    var=nco_fll_var_trv(in_id,&xtr_nbr,trv_tbl);

    var_out=(var_sct **)nco_malloc(xtr_nbr*sizeof(var_sct *));
    for(var_idx=0;var_idx<xtr_nbr;var_idx++) var_out[var_idx]=nco_var_dpl(var[var_idx]);

    /* Determine conventions (ARM/CCM/CCSM/CF/MPAS) for treating file */
    cnv=nco_cnv_ini(in_id);

    /* Divide variable lists into lists of fixed variables and variables to be processed */
    (void)nco_var_lst_dvd(var,var_out,xtr_nbr,cnv,True,nco_pck_plc_nil,nco_pck_map_nil,(dmn_sct **)NULL,0,&var_fix,&var_fix_out,&nbr_var_fix,&var_prc,&var_prc_out,&nbr_var_prc,trv_tbl);

    /* Store processed and fixed variables info into GTT */
    (void)nco_var_prc_fix_trv(nbr_var_prc,var_prc,nbr_var_fix,var_fix,trv_tbl);
  
  } /* !RECORD_AGGREGATE */

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

  /* ncecat-specific operations */
  if(RECORD_AGGREGATE){

    if(!rec_dmn_nm) rec_dmn_nm=(char *)strdup("record"); 

    /* Prepend record dimension to beginning of all vectors for processed variables */
    for(idx=0;idx<nbr_var_prc;idx++){
      var_prc_out[idx]->nbr_dim++;
      var_prc_out[idx]->is_rec_var=True;
      var_prc_out[idx]->sz_rec=var_prc_out[idx]->sz;

      /* Allocate space to hold dimension IDs */
      var_prc_out[idx]->dim=(dmn_sct **)nco_realloc(var_prc_out[idx]->dim,var_prc_out[idx]->nbr_dim*sizeof(dmn_sct *));
      var_prc_out[idx]->dmn_id=(int *)nco_realloc(var_prc_out[idx]->dmn_id,var_prc_out[idx]->nbr_dim*sizeof(int));
      var_prc_out[idx]->cnt=(long *)nco_realloc(var_prc_out[idx]->cnt,var_prc_out[idx]->nbr_dim*sizeof(long int));
      var_prc_out[idx]->end=(long *)nco_realloc(var_prc_out[idx]->end,var_prc_out[idx]->nbr_dim*sizeof(long int));
      var_prc_out[idx]->srd=(long *)nco_realloc(var_prc_out[idx]->srd,var_prc_out[idx]->nbr_dim*sizeof(long int));
      var_prc_out[idx]->srt=(long *)nco_realloc(var_prc_out[idx]->srt,var_prc_out[idx]->nbr_dim*sizeof(long int));

      /* Moves current array by one to make room for new record dimension info */
      (void)memmove((void *)(var_prc_out[idx]->dim+1),(void *)(var_prc_out[idx]->dim),(var_prc_out[idx]->nbr_dim-1)*sizeof(dmn_sct *));
      (void)memmove((void *)(var_prc_out[idx]->dmn_id+1),(void *)(var_prc_out[idx]->dmn_id),(var_prc_out[idx]->nbr_dim-1)*sizeof(int));
      (void)memmove((void *)(var_prc_out[idx]->cnt+1L),(void *)(var_prc_out[idx]->cnt),(var_prc_out[idx]->nbr_dim-1)*sizeof(long int));
      (void)memmove((void *)(var_prc_out[idx]->end+1L),(void *)(var_prc_out[idx]->end),(var_prc_out[idx]->nbr_dim-1)*sizeof(long int));
      (void)memmove((void *)(var_prc_out[idx]->srd+1L),(void *)(var_prc_out[idx]->srd),(var_prc_out[idx]->nbr_dim-1)*sizeof(long int));
      (void)memmove((void *)(var_prc_out[idx]->srt+1L),(void *)(var_prc_out[idx]->srt),(var_prc_out[idx]->nbr_dim-1)*sizeof(long int));

      /* Insert value for new record dimension */
      var_prc_out[idx]->cnt[0]=1L;
      var_prc_out[idx]->end[0]=-1L;
      var_prc_out[idx]->srd[0]=-1L;
      var_prc_out[idx]->srt[0]=-1L;
    } /* end loop over idx */

    /* Define dimensions, extracted groups, variables, and attributes in output file */
    (void)nco_xtr_dfn(in_id,out_id,&cnk,dfl_lvl,gpe,md5,!FORCE_APPEND,True,False,nco_pck_plc_nil,rec_dmn_nm,trv_tbl);

    /* Catenate time-stamped command line to "history" global attribute */
    if(HISTORY_APPEND) (void)nco_hst_att_cat(out_id,cmd_ln);
    if(HISTORY_APPEND && FORCE_APPEND) (void)nco_prv_att_cat(fl_in,in_id,out_id);
    if(gaa_nbr > 0) (void)nco_glb_att_add(out_id,gaa_arg,gaa_nbr);
    if(HISTORY_APPEND) (void)nco_vrs_att_cat(out_id);
    /* Add input file list global attribute */
    if(FL_LST_IN_APPEND && HISTORY_APPEND && FL_LST_IN_FROM_STDIN) (void)nco_fl_lst_att_cat(out_id,fl_lst_in,fl_nbr);
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
    (void)nco_cpy_fix_var_trv(in_id,out_id,gpe,trv_tbl);  

    /* Close first input netCDF file */
    (void)nco_close(in_id);

    /* Timestamp end of metadata setup and disk layout */
    rcd+=nco_ddra((char *)NULL,(char *)NULL,&ddra_info);
    ddra_info.tmr_flg=nco_tmr_rgl;
  } /* !RECORD_AGGREGATE */

  if(GROUP_AGGREGATE){
#ifndef ENABLE_NETCDF4
    (void)fprintf(stderr,"%s: ERROR GAG requires netCDF4 capabilities. HINT: Rebuild NCO with netCDF4 enabled.\n",nco_prg_nm_get());
    nco_exit(EXIT_FAILURE);
#endif /* ENABLE_NETCDF4 */
    if(nco_dbg_lvl >= nco_dbg_fl) (void)fprintf(stderr,"%s: INFO Group Aggregation (GAG) feature enabled\n",nco_prg_nm_get());
    if(fl_out_fmt != NC_FORMAT_NETCDF4){
      (void)fprintf(stderr,"%s: ERROR Group Aggregation requires netCDF4 output format but user explicitly requested format = %s\n",nco_prg_nm_get(),nco_fmt_sng(fl_out_fmt));
      nco_exit(EXIT_FAILURE);
    } /* endif err */
  } /* !GROUP_AGGREGATE */

  /* Loop over input files */
  for(fl_idx=0;fl_idx<fl_nbr;fl_idx++){
    /* Parse filename */
    if(fl_idx) fl_in=nco_fl_nm_prs(fl_in,fl_idx,(int *)NULL,fl_lst_in,abb_arg_nbr,fl_lst_abb,fl_pth);
    if(nco_dbg_lvl >= nco_dbg_fl) (void)fprintf(stderr,"%s: INFO Input file %d is %s",nco_prg_nm_get(),fl_idx,fl_in);
    /* Make sure file is on local system and is readable or die trying */
    if(fl_idx) fl_in=nco_fl_mk_lcl(fl_in,fl_pth_lcl,HPSS_TRY,&FL_RTR_RMT_LCN);
    if(nco_dbg_lvl >= nco_dbg_fl && FL_RTR_RMT_LCN) (void)fprintf(stderr,", local file is %s",fl_in);
    if(nco_dbg_lvl >= nco_dbg_fl) (void)fprintf(stderr,"\n");

    if(GROUP_AGGREGATE){
      size_t gpe_arg_lng; /* [nbr] Length of group specification */
      if(grp_out){
        /* Append enumerated counter to end of user-specified root group path */
        sprintf(grp_out_sfx,"%02d",fl_idx);
        gpe_arg_lng=grp_out_lng+NCO_GRP_OUT_SFX_LNG+gpe->lng_edt;
        gpe_arg=(char *)nco_malloc((gpe_arg_lng+1L)*sizeof(char));
        (void)strcpy(gpe_arg,grp_out);
        (void)strcat(gpe_arg,grp_out_sfx);
        if(gpe->edt) (void)strcat(gpe_arg,gpe->edt);
      }else{
        /* Use filename stub as group name */
        char *stb_srt_psn; /* [sng] Starting position of "stub", i.e., last component of filename */
        size_t fl_in_lng; /* [nbr] Length of filename */
        size_t sfx_fst; /* [nbr] Offset of suffix from start of string */
        size_t sfx_lng; /* [nbr] Suffix has this many characters */

        /* Is there a .nc, .cdf, .nc3, .nc4, .nc5, .nc6, or .nc7 suffix? or an .hdf, .HDF, .h5, .H5, or .he5 suffix? */
        fl_in_lng=strlen(fl_in);
        sfx_lng=3L;
        sfx_fst=fl_in_lng-sfx_lng;
        /* Sample data found in, e.g., ftp://ftp.unidata.ucar.edu/pub/netcdf/sample_data and http://hdfeos.org/zoo
	   Two versions of HDF-EOS exist: HDF-EOS2 for HDF4, and HDF-EOS5 for HDF5 */
        if(strncmp(fl_in+sfx_fst,".nc",sfx_lng) && /* netCDF standard suffix */
	   strncmp(fl_in+sfx_fst,".H5",sfx_lng) && /* HDF5 suffix used by ICESat GLAS, e.g., GLAH05_633_2103_001_1107_3_01_0001.H5 */
	   strncmp(fl_in+sfx_fst,".h5",sfx_lng)){ /* HDF5 suffix used by BUV, ICESat GLAS, MABEL, MEaSUREs Ozone, SBUV, e.g., BUV-Nimbus04_L3zm_v01-00-2012m0203t144121.h5, GLAH13_633_2103_001_1317_0_01_0001.h5, mabel_l2_20130927t201800_008_1.h5, MSO3L3zm5_v01-02-2013m0907t142428.h5 */
	  sfx_lng=4L;
	  sfx_fst=fl_in_lng-sfx_lng;
	  if(strncmp(fl_in+sfx_fst,".cdf",sfx_lng) && /* netCDF old-fashioned suffix */
	     strncmp(fl_in+sfx_fst,".hdf",sfx_lng) && /* HDF-EOS2 (HDF4) suffix used by AIRS, AMSR-E, MODIS, MOPPITT, SeaWiFS, e.g., AIRS.2002.08.01.L3.RetStd_H031.v4.0.21.0.G06104133732.hdf, MSR_E_L2_Rain_V10_200905312326_A.hdf, MOD10CM.A2007001.005.2007108111758.hdf, MOP01-20121231-L1V3.34.10.hdf, S1999001.L3m_DAY_CDOM_cdom_index_9km.hdf */
	     strncmp(fl_in+sfx_fst,".HDF",sfx_lng) && /* HDF-EOS2 (HDF4) suffix used by TRMM, e.g., 3B43.070901.6A.HDF */
	     strncmp(fl_in+sfx_fst,".he5",sfx_lng) && /* HDF-EOS5 (HDF5) suffix used by HIRDLS, OMI Aerosols, e.g., HIRDLS-Aura_L3ZAD_v06-00-00-c02_2005d022-2008d077.he5, OMI-Aura_L2-OMAERUV_2013m1004t2338-o49057_v003-2013m1005t053932.he5 */
	     strncmp(fl_in+sfx_fst,".nc3",sfx_lng) && /* netCDF3 variant suffix */
	     strncmp(fl_in+sfx_fst,".nc4",sfx_lng) && /* netCDF4 variant suffix */
	     strncmp(fl_in+sfx_fst,".nc5",sfx_lng) && /* netCDF CDF/PnetCDF variant suffix */
	     strncmp(fl_in+sfx_fst,".nc6",sfx_lng) && /* netCDF3 64bit_offset variant suffix */
	     strncmp(fl_in+sfx_fst,".nc7",sfx_lng)){ /* netCDF4 classic variant suffix */
	    (void)fprintf(stderr,"%s: WARNING GAG filename suffix is unusual---using whole filename as group name\n",nco_prg_nm_get());
	    sfx_lng=0;
	  } /* endif */
        } /* endif */

        stb_srt_psn=strrchr(fl_in,'/');
        if(!stb_srt_psn) stb_srt_psn=fl_in; else stb_srt_psn++;
        gpe_arg_lng=strlen(stb_srt_psn)-sfx_lng;
        gpe_arg=(char *)nco_malloc((gpe_arg_lng+1L)*sizeof(char));
        gpe_arg=strncpy(gpe_arg,stb_srt_psn,strlen(stb_srt_psn)-sfx_lng);
        gpe_arg[gpe_arg_lng]='\0';

        /* GPE arguments derived from filenames check for existence of path in output file */
        rcd=nco_inq_grp_full_ncid_flg(out_id,gpe_arg,&gpe_id);
        /* Existence implies current file may overwrite contents of previous file */
        if(rcd == NC_NOERR) (void)fprintf(stderr,"%s: WARNING GAG path \"%s\" automatically derived from stub of filename %s conflicts with existing path in output file. Any input data with same absolute path names as contents of a previous input file will be overwritten. Is the same input file specified multiple times? Is this intentional?\nHINT: To distribute copies of a single input file into different groups, use GPE to generate distinct output group names, e.g., %s -G copy in.nc in.nc out.nc\n",nco_prg_nm_get(),gpe_arg,fl_in,nco_prg_nm_get()); else rcd=NC_NOERR;
      } /* !grp_out */
      /* Free old structure, if any, before re-use */
      if(gpe) gpe=(gpe_sct *)nco_gpe_free(gpe);
      gpe=nco_gpe_prs_arg(gpe_arg);
      gpe_arg=(char *)nco_free(gpe_arg);
      if(nco_dbg_lvl >= nco_dbg_scl) (void)fprintf(stderr,"%s: INFO GAG current file has gpe->arg=%s\n",nco_prg_nm_get(),gpe->arg);

      /* Open file using appropriate buffer size hints and verbosity */
      if(fl_idx) rcd=nco_fl_open(fl_in,md_open,&bfr_sz_hnt,&in_id);

      if(fl_idx == 0){
        /* fxm: Copy global attributes of first file to root of output file to preserve history attribute */
        /* Copy global attributes */
        if(CPY_GLB_METADATA) (void)nco_att_cpy(in_id,out_id,NC_GLOBAL,NC_GLOBAL,(nco_bool)True);

        /* Catenate time-stamped command line to "history" global attribute */
        if(HISTORY_APPEND) (void)nco_hst_att_cat(out_id,cmd_ln);
    if(gaa_nbr > 0) (void)nco_glb_att_add(out_id,gaa_arg,gaa_nbr);
  if(HISTORY_APPEND) (void)nco_vrs_att_cat(out_id);

        /* Add input file list global attribute */
        if(FL_LST_IN_APPEND && HISTORY_APPEND && FL_LST_IN_FROM_STDIN) (void)nco_fl_lst_att_cat(out_id,fl_lst_in,fl_nbr);
      } /* endif first file */

      trv_tbl_sct *trv_tbl_gpr=NULL; /* [lst] Traversal table for GROUP_AGGREGATE only */

      /* Initialize traversal table */
      trv_tbl_init(&trv_tbl_gpr);

      /* Construct GTT, Group Traversal Table (groups,variables,dimensions, limits) */
      (void)nco_bld_trv_tbl(in_id,trv_pth,lmt_nbr_rgn,lmt_arg,aux_nbr,aux_arg,MSA_USR_RDR,FORTRAN_IDX_CNV,grp_lst_in,grp_lst_in_nbr,var_lst_in,xtr_nbr,EXTRACT_ALL_COORDINATES,GRP_VAR_UNN,False,EXCLUDE_INPUT_LIST,EXTRACT_ASSOCIATED_COORDINATES,EXTRACT_CLL_MSR,EXTRACT_FRM_TRM,nco_pck_plc_nil,&flg_dne,trv_tbl_gpr);

      /* Get number of variables, dimensions, and global attributes in file, file format */
      (void)trv_tbl_inq((int *)NULL,(int *)NULL,(int *)NULL,&nbr_dmn_fl,(int *)NULL,(int *)NULL,(int *)NULL,(int *)NULL,&nbr_var_fl,trv_tbl_gpr);
      (void)nco_inq_format(in_id,&fl_in_fmt);

      /* We now have final list of variables to extract. Phew. */

      /* Define extracted groups, variables, and attributes in output file */
      (void)nco_xtr_dfn(in_id,out_id,&cnk,dfl_lvl,gpe,md5,CPY_GLB_METADATA,(nco_bool)True,(nco_bool)False,nco_pck_plc_nil,rec_dmn_nm,trv_tbl_gpr);

      /* Turn-off default filling behavior to enhance efficiency */
      nco_set_fill(out_id,NC_NOFILL,&fll_md_old);

      /* Write extracted data to output file */
      (void)nco_xtr_wrt(in_id,out_id,gpe,fp_bnr,md5,HAVE_LIMITS,trv_tbl_gpr);

      /* Close input netCDF file */
      (void)nco_close(in_id);

      /* Free traversal table */
      trv_tbl_free(trv_tbl_gpr);

    } /* !GROUP_AGGREGATE */

    if(RECORD_AGGREGATE){

      /* Open file once per thread to improve caching */
      for(thr_idx=0;thr_idx<thr_nbr;thr_idx++) rcd=nco_fl_open(fl_in,md_open,&bfr_sz_hnt,in_id_arr+thr_idx);

      /* Perform various error-checks on input file */
      if(False) (void)nco_fl_cmp_err_chk();

      /* 20120309 Special case to improve copy speed on large blocksize filesystems (MM3s) */
      USE_MM3_WORKAROUND=nco_use_mm3_workaround(in_id_arr[0],fl_out_fmt);
      if(lmt_nbr > 0) USE_MM3_WORKAROUND=False; 
      if(!USE_MM3_WORKAROUND){  
        /* Copy all data variable-by-variable */
        ;
      }else{
        /* MM3 workaround algorithm */
        /* ncecat-specific MM3 characteristics:
        Only coordinates are "fixed" (non-processed) variables
        All other variables are "processed" 
        These variables may be fixed or
        record on input yet are all record on output 
        Makes sense to always use MM3? */
        ;
      } /* endif MM3 workaround */

      /* OpenMP with threading over variables, not files */
#ifdef _OPENMP
# pragma omp parallel for private(in_id) shared(nco_dbg_lvl,fl_nbr,idx_rec_out,in_id_arr,nbr_var_prc,out_id,var_prc,var_prc_out,nbr_dmn_fl,md5,trv_tbl)
#endif /* !_OPENMP */
      /* Process all variables in current file */
      for(idx=0;idx<nbr_var_prc;idx++){
	int grp_id; /* [ID] Group ID (input) */
        int grp_out_id; /* [ID] Group ID (output) */
        int var_out_id; /* [ID] Variable ID (output) */
        trv_sct *var_trv; /* [sct] Variable GTT object */

        in_id=in_id_arr[omp_get_thread_num()];
        if(nco_dbg_lvl >= nco_dbg_var) (void)fprintf(fp_stderr,"%s, ",var_prc[idx]->nm);
        if(nco_dbg_lvl >= nco_dbg_var) (void)fflush(fp_stderr);

        /* Obtain variable GTT object using full variable name */
        var_trv=trv_tbl_var_nm_fll(var_prc[idx]->nm_fll,trv_tbl);
	/* Variables may have different ID, missing_value, type, in each file */
	(void)nco_inq_grp_full_ncid(in_id,var_trv->grp_nm_fll,&grp_id);
	(void)nco_var_mtd_refresh(grp_id,var_prc[idx]);

        /* Read */
        (void)nco_msa_var_get_trv(in_id,var_prc[idx],trv_tbl);

        /* Size of record dimension is 1 in output file */
        var_prc_out[idx]->cnt[0]=1L;
        var_prc_out[idx]->srt[0]=idx_rec_out;

        /* Write variable into current record in output file */

        /* Obtain output group ID */
        (void)nco_inq_grp_full_ncid(out_id,var_trv->grp_nm_fll,&grp_out_id);

        /* Get variable ID */
        (void)nco_inq_varid(grp_out_id,var_trv->nm,&var_out_id);

        /* Store the output variable ID */
        var_prc_out[idx]->id=var_out_id;

	if(var_trv->ppc != NC_MAX_INT){
	  if(var_trv->flg_nsd) (void)nco_ppc_bitmask(var_trv->ppc,var_prc[idx]->type,var_prc[idx]->sz,var_prc[idx]->has_mss_val,var_prc[idx]->mss_val,var_prc[idx]->val); else (void)nco_ppc_around(var_trv->ppc,var_prc[idx]->type,var_prc[idx]->sz,var_prc[idx]->has_mss_val,var_prc[idx]->mss_val,var_prc[idx]->val);
	} /* endif ppc */
	if(nco_is_xcp(var_trv->nm)) nco_xcp_prc(var_trv->nm,var_prc[idx]->type,var_prc[idx]->sz,(char *)var_prc[idx]->val.vp);
 
#ifdef _OPENMP
# pragma omp critical
#endif /* _OPENMP */
        { /* begin OpenMP critical */
          if(var_prc[idx]->nbr_dim == 0){
            (void)nco_put_var1(grp_out_id,var_prc_out[idx]->id,var_prc_out[idx]->srt,var_prc[idx]->val.vp,var_prc[idx]->type);
          }else{ /* end if variable is scalar */
            (void)nco_put_vara(grp_out_id,var_prc_out[idx]->id,var_prc_out[idx]->srt,var_prc_out[idx]->cnt,var_prc[idx]->val.vp,var_prc[idx]->type);
          } /* end if variable is array */
          /* Perform MD5 digest of input and output data if requested */
          if(md5) (void)nco_md5_chk(md5,var_prc_out[idx]->nm,var_prc_out[idx]->sz*nco_typ_lng(var_prc[idx]->type),out_id,var_prc_out[idx]->srt,var_prc_out[idx]->cnt,var_prc[idx]->val.vp);
          /* Free current input buffer */
          var_prc[idx]->val.vp=nco_free(var_prc[idx]->val.vp);
        } /* end OpenMP critical */

      } /* end (OpenMP parallel for) loop over idx */
      if(nco_dbg_lvl >= nco_dbg_fl) (void)fprintf(stderr,"\n");

      idx_rec_out++; /* [idx] Index of current record in output file (0 is first, ...) */

      /* Close input netCDF file */
      for(thr_idx=0;thr_idx<thr_nbr;thr_idx++) nco_close(in_id_arr[thr_idx]);

    } /* !RECORD_AGGREGATE */

    /* Remove local copy of file */
    if(FL_RTR_RMT_LCN && RM_RMT_FL_PST_PRC) (void)nco_fl_rm(fl_in);

  } /* end loop over fl_idx */

  /* Close output file and move it from temporary to permanent location */
  (void)nco_fl_out_cls(fl_out,fl_out_tmp,out_id);

  /* Clean memory unless dirty memory allowed */
  if(flg_mmr_cln){

    /* NCO-generic clean-up */
    /* Free individual strings/arrays */
    if(cmd_ln) cmd_ln=(char *)nco_free(cmd_ln);
    if(cnk_map_sng) cnk_map_sng=(char *)nco_free(cnk_map_sng);
    if(cnk_plc_sng) cnk_plc_sng=(char *)nco_free(cnk_plc_sng);
    if(grp_out) grp_out=(char *)nco_free(grp_out);
    if(fl_in) fl_in=(char *)nco_free(fl_in);
    if(fl_out) fl_out=(char *)nco_free(fl_out);
    if(fl_out_tmp) fl_out_tmp=(char *)nco_free(fl_out_tmp);
    if(fl_pth) fl_pth=(char *)nco_free(fl_pth);
    if(fl_pth_lcl) fl_pth_lcl=(char *)nco_free(fl_pth_lcl);
    if(in_id_arr) in_id_arr=(int *)nco_free(in_id_arr);
    /* Free lists of strings */
    if(fl_lst_in && !fl_lst_abb) fl_lst_in=nco_sng_lst_free(fl_lst_in,fl_nbr); 
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
    if(RECORD_AGGREGATE){
      /* Free dimension lists */
      /* ncecat-specific memory cleanup */
      if(rec_dmn_nm) rec_dmn_nm=(char *)nco_free(rec_dmn_nm);
      /* Free variable lists */
      if(xtr_nbr > 0) var=nco_var_lst_free(var,xtr_nbr);
      if(xtr_nbr > 0) var_out=nco_var_lst_free(var_out,xtr_nbr);
      var_prc=(var_sct **)nco_free(var_prc);
      var_prc_out=(var_sct **)nco_free(var_prc_out);
      var_fix=(var_sct **)nco_free(var_fix);
      var_fix_out=(var_sct **)nco_free(var_fix_out);
      if(gpe) gpe=(gpe_sct *)nco_gpe_free(gpe);
      if(md5) md5=(md5_sct *)nco_md5_free(md5);
      /* Free traversal table */
      trv_tbl_free(trv_tbl);
      for(idx=0;idx<lmt_nbr;idx++) flg_dne[idx].dim_nm=(char *)nco_free(flg_dne[idx].dim_nm);
      if(flg_dne) flg_dne=(nco_dmn_dne_t *)nco_free(flg_dne);
    } /* RECORD_AGGREGATE */
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
