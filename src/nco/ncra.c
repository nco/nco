/* $Header: /data/zender/nco_20150216/nco/src/nco/ncra.c,v 1.544 2014-07-11 20:55:45 zender Exp $ */

/* This single source file compiles into three separate executables:
   ncra -- netCDF record averager
   nces -- netCDF ensemble statistics
   ncrcat -- netCDF record concatenator */

/* Purpose: Compute averages or extract series of specified hyperslabs of 
   specfied variables of multiple input netCDF files and output them 
   to a single file. */

/* Copyright (C) 1995--2014 Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You can redistribute and/or modify NCO under the terms of the 
   GNU General Public License (GPL) Version 3.
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

/* URL: http://nco.cvs.sf.net/nco/nco/src/nco/ncra.c
   
   Usage:
   ncra -O -n 3,4,1 -p ${HOME}/nco/data h0001.nc ~/foo.nc
   ncra -O -n 3,4,1 -p ${HOME}/nco/data -l ${HOME} h0001.nc ~/foo.nc
   ncra -O -n 3,4,1 -p /ZENDER/tmp -l ${HOME}/nco/data h0001.nc ~/foo.nc
   ncrcat -O -C -d time,0,5,4,2 -v time -p ~/nco/data in.nc ~/foo.nc
   ncra -O -C -d time,0,5,4,2 -v time -p ~/nco/data in.nc ~/foo.nc
   ncra -O -C --mro -d time,0,5,4,2 -v time -p ~/nco/data in.nc ~/foo.nc
   
   scp ~/nco/src/nco/ncra.c esmf.ess.uci.edu:nco/src/nco
   
   nces in.nc in.nc ~/foo.nc
   nces -O -n 3,4,1 -p ${HOME}/nco/data h0001.nc ~/foo.nc
   nces -O -n 3,4,1 -p ${HOME}/nco/data -l ${HOME} h0001.nc ~/foo.nc
   nces -O -n 3,4,1 -p /ZENDER/tmp -l ${HOME} h0001.nc ~/foo.nc

   ncra -Y ncge -O -p ~/nco/data mdl_1.nc ~/foo.nc
   ncra -Y ncge -O --nsm_sfx=_avg -p ~/nco/data mdl_1.nc ~/foo.nc */

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

/* Personal headers */
/* #define MAIN_PROGRAM_FILE MUST precede #include libnco.h */
#define MAIN_PROGRAM_FILE
#include "nco.h" /* netCDF Operator (NCO) definitions */
#include "libnco.h" /* netCDF Operator (NCO) library */

/* Define inline'd functions in header so source is visible to calling files
   C99 only: Declare prototype in exactly one header
   http://www.drdobbs.com/the-new-c-inline-functions/184401540 */
extern int min_int(int a, int b);
extern int max_int(int a, int b);
inline int min_int(int a, int b){return (a < b) ? a : b;}
inline int max_int(int a, int b){return (a > b) ? a : b;}
extern long min_lng(long a, long b);
extern long max_lng(long a, long b);
inline long min_lng(long a, long b){return (a < b) ? a : b;}
inline long max_lng(long a, long b){return (a > b) ? a : b;}

int
main(int argc,char **argv)
{
  char **fl_lst_abb=NULL; /* Option n */
  char **fl_lst_in;
  char **grp_lst_in=NULL_CEWI;
  char **var_lst_in=NULL_CEWI;
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
  char *grp_out_fll=NULL; /* [sng] Group name */
  char *lmt_arg[NC_MAX_DIMS];
  char *nco_op_typ_sng=NULL_CEWI; /* [sng] Operation type Option y */
  char *nco_pck_plc_sng=NULL_CEWI; /* [sng] Packing policy Option P */
  char *nsm_sfx=NULL; /* [sng] Ensemble suffix */
  char *opt_crr=NULL; /* [sng] String representation of current long-option name */
  char *optarg_lcl=NULL; /* [sng] Local copy of system optarg */
  char *sng_cnv_rcd=NULL_CEWI; /* [sng] strtol()/strtoul() return code */
  char trv_pth[]="/"; /* [sng] Root path of traversal tree */

  const char * const CVS_Id="$Id: ncra.c,v 1.544 2014-07-11 20:55:45 zender Exp $"; 
  const char * const CVS_Revision="$Revision: 1.544 $";
  const char * const opt_sht_lst="3467ACcD:d:FG:g:HhL:l:n:Oo:p:P:rRt:v:X:xY:y:-:";

  cnk_sct cnk; /* [sct] Chunking structure */

#if defined(__cplusplus) || defined(PGI_CC)
  ddra_info_sct ddra_info;
  ddra_info.flg_ddra=False;
#else /* !__cplusplus */
  ddra_info_sct ddra_info={.flg_ddra=False};
#endif /* !__cplusplus */

  dmn_sct **dim=NULL; /* CEWI */
  dmn_sct **dmn_out=NULL; /* CEWI */

  extern char *optarg;
  extern int optind;

  /* Using naked stdin/stdout/stderr in parallel region generates warning
  Copy appropriate filehandle to variable scoped shared in parallel clause */
  FILE * const fp_stderr=stderr; /* [fl] stderr filehandle CEWI */
  FILE * const fp_stdout=stdout; /* [fl] stdout filehandle CEWI */

  gpe_sct *gpe=NULL; /* [sng] Group Path Editing (GPE) structure */

  int *in_id_arr;

  int abb_arg_nbr=0;
  int aux_nbr=0; /* [nbr] Number of auxiliary coordinate hyperslabs specified */
  int cnk_map=nco_cnk_map_nil; /* [enm] Chunking map */
  int cnk_nbr=0; /* [nbr] Number of chunk sizes */
  int cnk_plc=nco_cnk_plc_nil; /* [enm] Chunking policy */
  int dfl_lvl=NCO_DFL_LVL_UNDEFINED; /* [enm] Deflate level */
  int fl_idx;
  int fl_in_fmt; /* [enm] Input file format */
  int fl_nbr=0;
  int fl_out_fmt=NCO_FORMAT_UNDEFINED; /* [enm] Output file format */
  int flg_input_complete_nbr=0; /* [nbr] Number of record dimensions completed */
  int fll_md_old; /* [enm] Old fill mode */
  int grp_lst_in_nbr=0; /* [nbr] Number of groups explicitly specified by user */
  int idx=int_CEWI;
  int in_id;
  int lmt_nbr=0; /* Option d. NB: lmt_nbr gets incremented */
  int md_open; /* [enm] Mode flag for nc_open() call */
  int nbr_dmn_fl;
  int nbr_dmn_xtr=0;
  int nbr_var_fix; /* nbr_var_fix gets incremented */
  int nbr_var_fl;
  int nbr_var_prc; /* nbr_var_prc gets incremented */
  int nbr_rec; /* [nbr] (ncra) Number of record dimensions  */
  int dmn_rec_fl;
  int nco_op_typ=nco_op_avg; /* [enm] Default operation is averaging */
  int nco_pck_plc=nco_pck_plc_nil; /* [enm] Default packing is none */
  int opt;
  int out_id;  
  int rcd=NC_NOERR; /* [rcd] Return code */
  int thr_idx; /* [idx] Index of current thread */
  int thr_nbr=int_CEWI; /* [nbr] Thread number Option t */
  int var_lst_in_nbr=0;
  int xtr_nbr=0; /* xtr_nbr won't otherwise be set for -c with no -v */
  int idx_rec=0;
  int grp_id;        /* [ID] Group ID */
  int grp_out_id;    /* [ID] Group ID (output) */
  int var_out_id;    /* [ID] Variable ID (output) */

  long idx_rec_crr_in; /* [idx] Index of current record in current input file */
  long *idx_rec_out=NULL; /* [idx] Index of current record in output file (0 is first, ...) */
  long *rec_in_cml=NULL;  /* [nbr] Number of records, read or not, in all processed files */
  long *rec_usd_cml=NULL; /* [nbr] Cumulative number of input records used (catenated by ncrcat or operated on by ncra) */
  long rec_dmn_sz=0L; /* [idx] Size of record dimension, if any, in current file (increments by srd) */
  long rec_rmn_prv_ssc=0L; /* [idx] Records remaining to be read in current subcycle group */

  md5_sct *md5=NULL; /* [sct] MD5 configuration */

  nco_bool *REC_LST_DSR=NULL; /* [flg] Record is last desired from all input files */
  nco_bool *flg_input_complete=NULL; /* [flg] All requested records in record dimension have been read */

  nco_bool CNV_ARM;
  nco_bool CNV_CCM_CCSM_CF;
  nco_bool EXCLUDE_INPUT_LIST=False; /* Option c */
  nco_bool EXTRACT_ALL_COORDINATES=False; /* Option c */
  nco_bool EXTRACT_ASSOCIATED_COORDINATES=True; /* Option C */
  nco_bool FLG_BFR_NRM=False; /* [flg] Current output buffers need normalization */
  nco_bool FLG_MRO=False; /* [flg] Multi-Record Output */
  nco_bool FL_LST_IN_APPEND=True; /* Option H */
  nco_bool FL_LST_IN_FROM_STDIN=False; /* [flg] fl_lst_in comes from stdin */
  nco_bool FL_RTR_RMT_LCN;
  nco_bool FORCE_APPEND=False; /* Option A */
  nco_bool FORCE_OVERWRITE=False; /* Option O */
  nco_bool FORTRAN_IDX_CNV=False; /* Option F */
  nco_bool GRP_VAR_UNN=False; /* [flg] Select union of specified groups and variables */
  nco_bool HISTORY_APPEND=True; /* Option h */
  nco_bool MSA_USR_RDR=False; /* [flg] Multi-Slab Algorithm returns hyperslabs in user-specified order */
  nco_bool RAM_CREATE=False; /* [flg] Create file in RAM */
  nco_bool RAM_OPEN=False; /* [flg] Open (netCDF3-only) file(s) in RAM */
  nco_bool REC_APN=False; /* [flg] Append records directly to output file */
  nco_bool REC_FRS_GRP=False; /* [flg] Record is first in current group */
  nco_bool REC_LST_GRP=False; /* [flg] Record is last in current group */
  nco_bool REC_SRD_LST=False; /* [flg] Record belongs to last stride of current file */
  nco_bool RM_RMT_FL_PST_PRC=True; /* Option R */
  nco_bool WRT_TMP_FL=True; /* [flg] Write output to temporary file */
  nco_bool flg_cll_mth=True; /* [flg] Add/modify cell_methods attributes */
  nco_bool flg_cln=True; /* [flg] Clean memory prior to exit */
  nco_bool flg_skp1; /* [flg] Current record is not dimension of this variable */
  nco_bool flg_skp2; /* [flg] Current record is not dimension of this variable */

  nco_int base_time_srt=nco_int_CEWI;
  nco_int base_time_crr=nco_int_CEWI;

  size_t bfr_sz_hnt=NC_SIZEHINT_DEFAULT; /* [B] Buffer size hint */
  size_t cnk_sz_byt=0UL; /* [B] Chunk size in bytes */
  size_t cnk_sz_scl=0UL; /* [nbr] Chunk size scalar */
  size_t hdr_pad=0UL; /* [B] Pad at end of header section */

  var_sct **var;
  var_sct **var_fix;
  var_sct **var_fix_out;
  var_sct **var_out=NULL_CEWI;
  var_sct **var_prc;
  var_sct **var_prc_out;

  trv_sct *var_trv;        /* [sct] Variable GTT object */

  trv_tbl_sct *trv_tbl;    /* [lst] Traversal table */

  nco_dmn_dne_t *flg_dne=NULL; /* [lst] Flag to check if input dimension -d "does not exist" */

  lmt_sct **lmt_rec=NULL; /* [lst] (ncra) Record dimensions */

#ifdef ENABLE_MPI
  /* Declare all MPI-specific variables here */
  MPI_Comm mpi_cmm=MPI_COMM_WORLD; /* [prc] Communicator */
  int prc_rnk; /* [idx] Process rank */
  int prc_nbr=0; /* [nbr] Number of MPI processes */
#endif /* !ENABLE_MPI */
  
  static struct option opt_lng[]=
  { /* Structure ordered by short option key if possible */
    /* Long options with no argument, no short option counterpart */
    {"cll_mth",no_argument,0,0}, /* [flg] Add/modify cell_methods attributes */
    {"cell_methods",no_argument,0,0}, /* [flg] Add/modify cell_methods attributes */
    {"no_cll_mth",no_argument,0,0}, /* [flg] Do not add/modify cell_methods attributes */
    {"cln",no_argument,0,0}, /* [flg] Clean memory prior to exit */
    {"clean",no_argument,0,0}, /* [flg] Clean memory prior to exit */
    {"mmr_cln",no_argument,0,0}, /* [flg] Clean memory prior to exit */
    {"drt",no_argument,0,0}, /* [flg] Allow dirty memory on exit */
    {"dirty",no_argument,0,0}, /* [flg] Allow dirty memory on exit */
    {"mmr_drt",no_argument,0,0}, /* [flg] Allow dirty memory on exit */
    {"dbl",no_argument,0,0}, /* [flg] Arithmetic convention: promote float to double */
    {"flt",no_argument,0,0}, /* [flg] Arithmetic convention: keep single-precision */
    {"rth_dbl",no_argument,0,0}, /* [flg] Arithmetic convention: promote float to double */
    {"rth_flt",no_argument,0,0}, /* [flg] Arithmetic convention: keep single-precision */
    {"hdf4",no_argument,0,0}, /* [flg] Treat file as HDF4 */
    {"hdf_upk",no_argument,0,0}, /* [flg] HDF unpack convention: unpacked=scale_factor*(packed-add_offset) */
    {"hdf_unpack",no_argument,0,0}, /* [flg] HDF unpack convention: unpacked=scale_factor*(packed-add_offset) */
    {"md5_dgs",no_argument,0,0}, /* [flg] Perform MD5 digests */
    {"md5_digest",no_argument,0,0}, /* [flg] Perform MD5 digests */
    {"mro",no_argument,0,0}, /* [flg] Multi-Record Output */
    {"multi_record_output",no_argument,0,0}, /* [flg] Multi-Record Output */
    {"msa_usr_rdr",no_argument,0,0}, /* [flg] Multi-Slab Algorithm returns hyperslabs in user-specified order */
    {"msa_user_order",no_argument,0,0}, /* [flg] Multi-Slab Algorithm returns hyperslabs in user-specified order */
    {"nsm_fl",no_argument,0,0},
    {"nsm_grp",no_argument,0,0},
    {"ram_all",no_argument,0,0}, /* [flg] Open (netCDF3) and create file(s) in RAM */
    {"create_ram",no_argument,0,0}, /* [flg] Create file in RAM */
    {"open_ram",no_argument,0,0}, /* [flg] Open (netCDF3) file(s) in RAM */
    {"diskless_all",no_argument,0,0}, /* [flg] Open (netCDF3) and create file(s) in RAM */
    {"rec_apn",no_argument,0,0}, /* [flg] Append records directly to output file */
    {"record_append",no_argument,0,0}, /* [flg] Append records directly to output file */
    {"wrt_tmp_fl",no_argument,0,0}, /* [flg] Write output to temporary file */
    {"write_tmp_fl",no_argument,0,0}, /* [flg] Write output to temporary file */
    {"no_tmp_fl",no_argument,0,0}, /* [flg] Do not write output to temporary file */
    {"version",no_argument,0,0},
    {"vrs",no_argument,0,0},
    /* Long options with argument, no short option counterpart */
    {"bfr_sz_hnt",required_argument,0,0}, /* [B] Buffer size hint */
    {"buffer_size_hint",required_argument,0,0}, /* [B] Buffer size hint */
    {"cnk_map",required_argument,0,0}, /* [nbr] Chunking map */
    {"chunk_map",required_argument,0,0}, /* [nbr] Chunking map */
    {"cnk_plc",required_argument,0,0}, /* [nbr] Chunking policy */
    {"chunk_policy",required_argument,0,0}, /* [nbr] Chunking policy */
    {"cnk_byt",required_argument,0,0}, /* [B] Chunk size in bytes */
    {"chunk_byte",required_argument,0,0}, /* [B] Chunk size in bytes */
    {"cnk_scl",required_argument,0,0}, /* [nbr] Chunk size scalar */
    {"chunk_scalar",required_argument,0,0}, /* [nbr] Chunk size scalar */
    {"cnk_dmn",required_argument,0,0}, /* [nbr] Chunk size */
    {"chunk_dimension",required_argument,0,0}, /* [nbr] Chunk size */
    {"fl_fmt",required_argument,0,0},
    {"hdr_pad",required_argument,0,0},
    {"header_pad",required_argument,0,0},
    {"nsm_sfx",required_argument,0,0},
    {"ensemble_suffix",required_argument,0,0},
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
    {"nco_dbg_lvl",required_argument,0,'D'},
    {"dimension",required_argument,0,'d'},
    {"dmn",required_argument,0,'d'},
    {"fortran",no_argument,0,'F'},
    {"ftn",no_argument,0,'F'},
    {"fl_lst_in",no_argument,0,'H'},
    {"file_list",no_argument,0,'H'},
    {"history",no_argument,0,'h'},
    {"hst",no_argument,0,'h'},
    {"dfl_lvl",required_argument,0,'L'}, /* [enm] Deflate level */
    {"deflate",required_argument,0,'L'}, /* [enm] Deflate level */
    {"local",required_argument,0,'l'},
    {"lcl",required_argument,0,'l'},
    {"nintap",required_argument,0,'n'},
    {"overwrite",no_argument,0,'O'},
    {"ovr",no_argument,0,'O'},
    {"output",required_argument,0,'o'},
    {"fl_out",required_argument,0,'o'},
    {"path",required_argument,0,'p'},
    {"pack",required_argument,0,'P'},
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
    {"pseudonym",required_argument,0,'Y'},
    {"program",required_argument,0,'Y'},
    {"prg_nm",required_argument,0,'Y'},
    {"math",required_argument,0,'y'},
    {"operation",required_argument,0,'y'},
    {"op_typ",required_argument,0,'y'},
    {"help",no_argument,0,'?'},
    {"hlp",no_argument,0,'?'},
    {0,0,0,0}
  }; /* end opt_lng */
  int opt_idx=0; /* Index of current long option into opt_lng array */

#ifdef _LIBINTL_H
  setlocale(LC_ALL,""); /* LC_ALL sets all localization tokens to same value */
  bindtextdomain("nco","/home/zender/share/locale"); /* ${LOCALEDIR} is e.g., /usr/share/locale */
  /* MO files should be in ${LOCALEDIR}/es/LC_MESSAGES */
  textdomain("nco"); /* PACKAGE is name of program or library */
#endif /* not _LIBINTL_H */

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
      if(!strcmp(opt_crr,"cll_mth") || !strcmp(opt_crr,"cell_methods")) flg_cll_mth=True; /* [flg] Add/modify cell_methods attributes */
      if(!strcmp(opt_crr,"no_cll_mth") || !strcmp(opt_crr,"no_cell_methods")) flg_cll_mth=False; /* [flg] Add/modify cell_methods attributes */
      if(!strcmp(opt_crr,"cln") || !strcmp(opt_crr,"mmr_cln") || !strcmp(opt_crr,"clean")) flg_cln=True; /* [flg] Clean memory prior to exit */
      if(!strcmp(opt_crr,"drt") || !strcmp(opt_crr,"mmr_drt") || !strcmp(opt_crr,"dirty")) flg_cln=False; /* [flg] Clean memory prior to exit */
      if(!strcmp(opt_crr,"fl_fmt") || !strcmp(opt_crr,"file_format")) rcd=nco_create_mode_prs(optarg,&fl_out_fmt);
      if(!strcmp(opt_crr,"dbl") || !strcmp(opt_crr,"rth_dbl")) nco_rth_cnv=nco_rth_flt_dbl; /* [flg] Arithmetic convention: promote float to double */
      if(!strcmp(opt_crr,"flt") || !strcmp(opt_crr,"rth_flt")) nco_rth_cnv=nco_rth_flt_flt; /* [flg] Arithmetic convention: keep single-precision */
      if(!strcmp(opt_crr,"hdf4")) nco_fmt_xtn=nco_fmt_xtn_hdf4; /* [enm] Treat file as HDF4 */
      if(!strcmp(opt_crr,"hdf_upk") || !strcmp(opt_crr,"hdf_unpack")) nco_upk_cnv=nco_upk_HDF; /* [flg] HDF unpack convention: unpacked=scale_factor*(packed-add_offset) */
      if(!strcmp(opt_crr,"hdr_pad") || !strcmp(opt_crr,"header_pad")){
        hdr_pad=strtoul(optarg,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
        if(*sng_cnv_rcd) nco_sng_cnv_err(optarg,"strtoul",sng_cnv_rcd);
      } /* endif "hdr_pad" */
      if(!strcmp(opt_crr,"md5_dgs") || !strcmp(opt_crr,"md5_digest")){
        if(!md5) md5=nco_md5_ini();
        md5->dgs=True;
        if(nco_dbg_lvl >= nco_dbg_std) (void)fprintf(stderr,"%s: INFO Will perform MD5 digests of input and output hyperslabs\n",nco_prg_nm_get());
      } /* endif "md5_dgs" */
      if(!strcmp(opt_crr,"mro") || !strcmp(opt_crr,"multi_record_output")) FLG_MRO=True; /* [flg] Multi-Record Output */
      if(!strcmp(opt_crr,"msa_usr_rdr") || !strcmp(opt_crr,"msa_user_order")) MSA_USR_RDR=True; /* [flg] Multi-Slab Algorithm returns hyperslabs in user-specified order */
      if(!strcmp(opt_crr,"nsm_fl") || !strcmp(opt_crr,"nsm_file") || !strcmp(opt_crr,"ensemble_file")){
	if(nco_prg_nm) nco_prg_nm=(char *)nco_free(nco_prg_nm);
	nco_prg_nm=nco_prg_prs("ncfe",&nco_prg_id);
      } /* endif nsm_fl */
      if(!strcmp(opt_crr,"nsm_grp") || !strcmp(opt_crr,"nsm_group") || !strcmp(opt_crr,"ensemble_group")){
	if(nco_prg_nm) nco_prg_nm=(char *)nco_free(nco_prg_nm);
	nco_prg_nm=nco_prg_prs("ncge",&nco_prg_id);
      } /* endif nsm_grp */
      if(!strcmp(opt_crr,"nsm_sfx") || !strcmp(opt_crr,"ensemble_suffix")) nsm_sfx=(char *)strdup(optarg);
      if(!strcmp(opt_crr,"ram_all") || !strcmp(opt_crr,"create_ram") || !strcmp(opt_crr,"diskless_all")) RAM_CREATE=True; /* [flg] Open (netCDF3) file(s) in RAM */
      if(!strcmp(opt_crr,"ram_all") || !strcmp(opt_crr,"open_ram") || !strcmp(opt_crr,"diskless_all")) RAM_OPEN=True; /* [flg] Create file in RAM */
      if(!strcmp(opt_crr,"rec_apn") || !strcmp(opt_crr,"record_append")){
        REC_APN=True; /* [flg] Append records directly to output file */
        FORCE_APPEND=True;
      } /* endif "rec_apn" */
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
    case '4': /* Catch-all to prescribe output storage format */
      if(!strcmp(opt_crr,"64bit")) fl_out_fmt=NC_FORMAT_64BIT; else fl_out_fmt=NC_FORMAT_NETCDF4; 
      break;
    case '6': /* Request netCDF3 64-bit offset output storage format */
      fl_out_fmt=NC_FORMAT_64BIT;
      break;
    case '7': /* Request netCDF4-classic output storage format */
      fl_out_fmt=NC_FORMAT_NETCDF4_CLASSIC;
      break;
    case 'A': /* Toggle FORCE_APPEND */
      FORCE_APPEND=True;
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
      nc_set_log_level(nco_dbg_lvl);
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
    case 'n': /* NINTAP-style abbreviation of files to average */
      fl_lst_abb=nco_lst_prs_2D(optarg,",",&abb_arg_nbr);
      if(abb_arg_nbr < 1 || abb_arg_nbr > 5){
        (void)fprintf(stdout,gettext("%s: ERROR Incorrect abbreviation for file list\n"),nco_prg_nm_get());
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
    case 'P': /* Packing policy */
      nco_pck_plc_sng=(char *)strdup(optarg);
      nco_pck_plc=nco_pck_plc_get(nco_pck_plc_sng);
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
    case 'X': /* Copy auxiliary coordinate argument for later processing */
      aux_arg[aux_nbr]=(char *)strdup(optarg);
      aux_nbr++;
      MSA_USR_RDR=True; /* [flg] Multi-Slab Algorithm returns hyperslabs in user-specified order */      
      break;
    case 'x': /* Exclude rather than extract variables specified with -v */
      EXCLUDE_INPUT_LIST=True;
      break;
    case 'Y': /* Pseudonym */
      /* Call nco_prg_prs() to reset pseudonym */
      optarg_lcl=(char *)strdup(optarg);
      if(nco_prg_nm) nco_prg_nm=(char *)nco_free(nco_prg_nm);
      nco_prg_nm=nco_prg_prs(optarg_lcl,&nco_prg_id);
      optarg_lcl=(char *)nco_free(optarg_lcl);
      break;
    case 'y': /* Operation type */
      nco_op_typ_sng=(char *)strdup(optarg);
      if(nco_prg_id == ncra || nco_prg_id == ncfe) nco_op_typ=nco_op_typ_get(nco_op_typ_sng);
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

  /* Process positional arguments and fill in filenames */
  fl_lst_in=nco_fl_lst_mk(argv,argc,optind,&fl_nbr,&fl_out,&FL_LST_IN_FROM_STDIN);

  /* Initialize thread information */
  thr_nbr=nco_openmp_ini(thr_nbr);
  in_id_arr=(int *)nco_malloc(thr_nbr*sizeof(int));

  /* Parse filename */
  fl_in=nco_fl_nm_prs(fl_in,0,&fl_nbr,fl_lst_in,abb_arg_nbr,fl_lst_abb,fl_pth);
  /* Make sure file is on local system and is readable or die trying */
  fl_in=nco_fl_mk_lcl(fl_in,fl_pth_lcl,&FL_RTR_RMT_LCN);
  /* Open file using appropriate buffer size hints and verbosity */
  if(RAM_OPEN) md_open=NC_NOWRITE|NC_DISKLESS; else md_open=NC_NOWRITE;
  rcd+=nco_fl_open(fl_in,md_open,&bfr_sz_hnt,&in_id);

  (void)nco_inq_format(in_id,&fl_in_fmt);

  /* Initialize traversal table */ 
  trv_tbl_init(&trv_tbl); 

  /* Construct GTT, Group Traversal Table (groups,variables,dimensions, limits) */
  (void)nco_bld_trv_tbl(in_id,trv_pth,lmt_nbr,lmt_arg,aux_nbr,aux_arg,MSA_USR_RDR,FORTRAN_IDX_CNV,grp_lst_in,grp_lst_in_nbr,var_lst_in,var_lst_in_nbr,EXTRACT_ALL_COORDINATES,GRP_VAR_UNN,False,EXCLUDE_INPUT_LIST,EXTRACT_ASSOCIATED_COORDINATES,nco_pck_plc_nil,&flg_dne,trv_tbl);

  /* Were all user-specified dimensions found? */ 
  (void)nco_chk_dmn(lmt_nbr,flg_dne);     

  /* Store ncge ensemble suffix in table */
  if(nco_prg_id == ncge && nsm_sfx) trv_tbl->nsm_sfx=nsm_sfx;

  /* Get number of variables, dimensions, and global attributes in file, file format */
  (void)trv_tbl_inq((int *)NULL,(int *)NULL,(int *)NULL,&nbr_dmn_fl,&dmn_rec_fl,(int *)NULL,(int *)NULL,(int *)NULL,&nbr_var_fl,trv_tbl);

  /* Record handling operators only */
  if(nco_prg_id == ncra || nco_prg_id == ncrcat){

    /* Build record dimensions array */
    (void)nco_bld_rec_dmn(in_id,FORTRAN_IDX_CNV,&lmt_rec,&nbr_rec,trv_tbl);  

    /* Allocate arrays for multi-records cases */
    flg_input_complete=(nco_bool *)nco_malloc(nbr_rec*sizeof(nco_bool));
    idx_rec_out=(long *)nco_malloc(nbr_rec*sizeof(long));
    rec_in_cml=(long *)nco_malloc(nbr_rec*sizeof(long));
    rec_usd_cml=(long *)nco_malloc(nbr_rec*sizeof(long));
    REC_LST_DSR=(nco_bool *)nco_malloc(nbr_rec*sizeof(nco_bool));

    /* Initialize arrays for multi-records cases */
    for(idx_rec=0;idx_rec<nbr_rec;idx_rec++){
      flg_input_complete[idx_rec]=False;
      idx_rec_out[idx_rec]=0L;
      rec_in_cml[idx_rec]=0L;
      rec_usd_cml[idx_rec]=0L;
      REC_LST_DSR[idx_rec]=False;
    } /* Initialize arrays */

  } /* Record handling operators only */

  /* Is this an ARM-format data file? */
  CNV_ARM=nco_cnv_arm_inq(in_id);
  /* NB: nco_cnv_arm_base_time_get() with same nc_id contains OpenMP critical region */
  if(CNV_ARM) base_time_srt=nco_cnv_arm_base_time_get(in_id);

  /* Fill-in variable structure list for all extracted variables */
  var=nco_fll_var_trv(in_id,&xtr_nbr,trv_tbl);

  /* Duplicate to output array */
  var_out=(var_sct **)nco_malloc(xtr_nbr*sizeof(var_sct *));
  for(idx=0;idx<xtr_nbr;idx++){
    var_out[idx]=nco_var_dpl(var[idx]);
    (void)nco_xrf_var(var[idx],var_out[idx]);
    (void)nco_xrf_dmn(var_out[idx]);
  } /* end loop over xtr */

  /* Refresh var_out with dim_out data */
  (void)nco_var_dmn_refresh(var_out,xtr_nbr);

  /* Is this a CCM/CCSM/CF-format history tape? */
  CNV_CCM_CCSM_CF=nco_cnv_ccm_ccsm_cf_inq(in_id);

  /* Divide variable lists into lists of fixed variables and variables to be processed */
  (void)nco_var_lst_dvd(var,var_out,xtr_nbr,CNV_CCM_CCSM_CF,True,nco_pck_plc_nil,nco_pck_map_nil,(dmn_sct **)NULL,0,&var_fix,&var_fix_out,&nbr_var_fix,&var_prc,&var_prc_out,&nbr_var_prc,trv_tbl);

  /* Store processed and fixed variables info into GTT */
  (void)nco_var_prc_fix_trv(nbr_var_prc,var_prc,nbr_var_fix,var_fix,trv_tbl);

  /* Make output and input files consanguinous */
  if(fl_out_fmt == NCO_FORMAT_UNDEFINED) fl_out_fmt=fl_in_fmt;

  /* Verify output file format supports requested actions */
  (void)nco_fl_fmt_vet(fl_out_fmt,cnk_nbr,dfl_lvl);

  /* Open output file */
  fl_out_tmp=nco_fl_out_open(fl_out,FORCE_APPEND,FORCE_OVERWRITE,fl_out_fmt,&bfr_sz_hnt,RAM_CREATE,RAM_OPEN,WRT_TMP_FL,&out_id);

  /* Create structure with all chunking information */
  if(fl_out_fmt == NC_FORMAT_NETCDF4 || fl_out_fmt == NC_FORMAT_NETCDF4_CLASSIC) rcd+=nco_cnk_ini(fl_out,cnk_arg,cnk_nbr,cnk_map,cnk_plc,cnk_sz_byt,cnk_sz_scl,&cnk);

  /* Define dimensions, extracted groups, variables, and attributes in output file */
  (void)nco_xtr_dfn(in_id,out_id,&cnk,dfl_lvl,gpe,md5,True,True,False,nco_pck_plc_nil,(char *)NULL,trv_tbl);

  /* Define ensemble fixed variables (True parameter) */
  if(nco_prg_id_get() == ncge) (void)nco_nsm_dfn_wrt(in_id,out_id,&cnk,dfl_lvl,gpe,True,trv_tbl); 

  /* Catenate time-stamped command line to "history" global attribute */
  if(HISTORY_APPEND) (void)nco_hst_att_cat(out_id,cmd_ln);
  if(thr_nbr > 0 && HISTORY_APPEND) (void)nco_thr_att_cat(out_id,thr_nbr);

  /* Add input file list global attribute */
  if(FL_LST_IN_APPEND && HISTORY_APPEND && FL_LST_IN_FROM_STDIN) (void)nco_fl_lst_att_cat(out_id,fl_lst_in,fl_nbr);

  /* Turn off default filling behavior to enhance efficiency */
  (void)nco_set_fill(out_id,NC_NOFILL,&fll_md_old);

  /* Add cell_methods attributes (before exiting define mode) */
  if(nco_prg_id == ncra){
    dmn_sct **dmn=NULL_CEWI;
    int nbr_dmn=nbr_rec;
    /* Allocate  */
    dmn=(dmn_sct **)nco_malloc(nbr_dmn*sizeof(dmn_sct *));
    /* Make dimension array from limit records array */
    (void)nco_dmn_lmt(lmt_rec,nbr_dmn,&dmn);
    /* Add cell_methods attributes (pass as dimension argument a records-only array) */
    if(flg_cll_mth){
      if(nco_dbg_lvl_get() >= nco_dbg_dev){
        (void)fprintf(stdout,"%s: DEBUG dimension list to nco_cnv_cf_cll_mth_add()\n",nco_prg_nm_get());
        for(idx=0;idx<nbr_dmn;idx++) (void)fprintf(stdout,"%s: DEBUG %s\n",nco_prg_nm_get(),dmn[idx]->nm);
        for(idx=0;idx<nbr_var_prc;idx++) (void)fprintf(stdout,"%s: DEBUG %s\n",nco_prg_nm_get(),var_prc_out[idx]->nm_fll);
      } /* endif dbg */
      rcd+=nco_cnv_cf_cll_mth_add(out_id,var_prc_out,nbr_var_prc,dmn,nbr_dmn,nco_op_typ,gpe,trv_tbl); 
    } /* !flg_cll_mth */
    if(nbr_dmn > 0) dmn=nco_dmn_lst_free(dmn,nbr_dmn);
  } /* !ncra */

  /* Take output file out of define mode */
  if(hdr_pad == 0UL){
    (void)nco_enddef(out_id);
  }else{
    (void)nco__enddef(out_id,hdr_pad);
    if(nco_dbg_lvl >= nco_dbg_scl) (void)fprintf(stderr,"%s: INFO Padding header with %lu extra bytes\n",nco_prg_nm_get(),(unsigned long)hdr_pad);
  } /* hdr_pad */

  /* Zero start and stride vectors for all output variables */
  (void)nco_var_srd_srt_set(var_out,xtr_nbr);

  /* Copy variable data for non-processed variables */
  (void)nco_cpy_fix_var_trv(in_id,out_id,gpe,trv_tbl); 

  /* Write ensemble fixed variables (False parameter) */
  if(nco_prg_id_get() == ncge) (void)nco_nsm_dfn_wrt(in_id,out_id,&cnk,dfl_lvl,gpe,False,trv_tbl); 

  /* Close first input netCDF file */
  nco_close(in_id);

  /* Allocate and, if necesssary, initialize accumulation space for processed variables */
  for(idx=0;idx<nbr_var_prc;idx++){
    if(nco_prg_id == ncra || nco_prg_id == ncrcat){
      /* Allocate space for only one record */
      var_prc[idx]->sz=var_prc[idx]->sz_rec=var_prc_out[idx]->sz=var_prc_out[idx]->sz_rec;
    } /* endif */
    if(nco_prg_id == ncra || nco_prg_id == ncfe || nco_prg_id == ncge){
      var_prc_out[idx]->tally=var_prc[idx]->tally=(long *)nco_malloc(var_prc_out[idx]->sz*sizeof(long));
      var_prc_out[idx]->val.vp=(void *)nco_malloc(var_prc_out[idx]->sz*nco_typ_lng(var_prc_out[idx]->type));
      if(nco_prg_id == ncfe || nco_prg_id == ncge){
        (void)nco_zero_long(var_prc_out[idx]->sz,var_prc_out[idx]->tally);
        (void)nco_var_zero(var_prc_out[idx]->type,var_prc_out[idx]->sz,var_prc_out[idx]->val);
      } /* end if ncfe || ncge */
    } /* end if */
  } /* end loop over idx */

  /* Timestamp end of metadata setup and disk layout */
  rcd+=nco_ddra((char *)NULL,(char *)NULL,&ddra_info);
  ddra_info.tmr_flg=nco_tmr_rgl;

  /* Loop over input files */
  for(fl_idx=0;fl_idx<fl_nbr;fl_idx++){

    /* Parse filename */
    if(fl_idx != 0) fl_in=nco_fl_nm_prs(fl_in,fl_idx,(int *)NULL,fl_lst_in,abb_arg_nbr,fl_lst_abb,fl_pth);
    if(nco_dbg_lvl >= nco_dbg_fl) (void)fprintf(stderr,gettext("%s: INFO Input file %d is %s"),nco_prg_nm_get(),fl_idx,fl_in);
    /* Make sure file is on local system and is readable or die trying */
    if(fl_idx != 0) fl_in=nco_fl_mk_lcl(fl_in,fl_pth_lcl,&FL_RTR_RMT_LCN);
    if(nco_dbg_lvl >= nco_dbg_fl && FL_RTR_RMT_LCN) (void)fprintf(stderr,gettext(", local file is %s"),fl_in);
    if(nco_dbg_lvl >= nco_dbg_fl) (void)fprintf(stderr,"\n");

    /* Open file once per thread to improve caching */
    for(thr_idx=0;thr_idx<thr_nbr;thr_idx++) rcd+=nco_fl_open(fl_in,NC_NOWRITE,&bfr_sz_hnt,in_id_arr+thr_idx);
    in_id=in_id_arr[0];

    /* Do ncge ensemble refresh */
    if(nco_prg_id == ncge){
      /* Refresh ensembles */
      if(fl_idx > 0) (void)nco_nsm_ncr(in_id,trv_tbl);
   
      /* Check if ensembles are valid  */       
      (void)nco_chk_nsm(in_id,fl_idx,trv_tbl); 

    }else{ /* ! ncge */
      /* Variables may have different ID, missing_value, type, in each file */
      for(idx=0;idx<nbr_var_prc;idx++){
        /* Obtain variable GTT object using full variable name */
        trv_sct *trv=trv_tbl_var_nm_fll(var_prc[idx]->nm_fll,trv_tbl);
        /* Obtain group ID using full group name */
        (void)nco_inq_grp_full_ncid(in_id,trv->grp_nm_fll,&grp_id);
        (void)nco_var_mtd_refresh(grp_id,var_prc[idx]);
      } /* end loop over variables */
    } /* ! ncge */

    if(nco_prg_id == ncra || nco_prg_id == ncrcat){ /* ncfe and ncge jump to else branch */

      /* Loop over number of different record variables in file */
      for(idx_rec=0;idx_rec<nbr_rec;idx_rec++){

        /* Obtain group ID using full group name */
        (void)nco_inq_grp_full_ncid(in_id,lmt_rec[idx_rec]->grp_nm_fll,&grp_id);

        /* Fill record array */
        (void)nco_lmt_evl(grp_id,lmt_rec[idx_rec],rec_usd_cml[idx_rec],FORTRAN_IDX_CNV);

        if(REC_APN){
          /* Append records directly to output file */
          int rec_dmn_out_id=NCO_REC_DMN_UNDEFINED;
          /* Get group ID using record group full name */
          (void)nco_inq_grp_full_ncid(out_id,lmt_rec[idx_rec]->grp_nm_fll,&grp_out_id);
          /* Get the dimension ID (rec_dmn_out_id) of the current record, from its name */
          (void)nco_inq_dimid(grp_out_id,lmt_rec[idx_rec]->nm,&rec_dmn_out_id);
          /* Get the size of the record  */
          (void)nco_inq_dimlen(grp_out_id,rec_dmn_out_id,&idx_rec_out[idx_rec]);
        } /* !REC_APN */

        if(nco_dbg_lvl_get() == nco_dbg_old) (void)fprintf(fp_stdout,"%s: DEBUG record [%d] #%d<%s>(%ld)\n",nco_prg_nm_get(),idx_rec,lmt_rec[idx_rec]->id,lmt_rec[idx_rec]->nm_fll,lmt_rec[idx_rec]->rec_dmn_sz);                    
        /* Two distinct ways to specify MRO are --mro and -d dmn,a,b,c,d,[m,M] */
        if(FLG_MRO) lmt_rec[idx_rec]->flg_mro=True;
        if(lmt_rec[idx_rec]->flg_mro) FLG_MRO=True;

        /* NB: nco_cnv_arm_base_time_get() with same nc_id contains OpenMP critical region */
        if(CNV_ARM) base_time_crr=nco_cnv_arm_base_time_get(in_id);

        /* Perform various error-checks on input file */
        if(False) (void)nco_fl_cmp_err_chk();

        /* This file may be superfluous though valid data will be found in upcoming files */
        if(nco_dbg_lvl >= nco_dbg_std)
          if((lmt_rec[idx_rec]->srt > lmt_rec[idx_rec]->end) && (lmt_rec[idx_rec]->rec_rmn_prv_ssc == 0L))
            (void)fprintf(fp_stdout,"%s: INFO %s (input file %d) is superfluous\n",nco_prg_nm_get(),fl_in,fl_idx);

        rec_dmn_sz=lmt_rec[idx_rec]->rec_dmn_sz;
        rec_rmn_prv_ssc=lmt_rec[idx_rec]->rec_rmn_prv_ssc; /* Local copy may be decremented later */
        idx_rec_crr_in= (rec_rmn_prv_ssc > 0L) ? 0L : lmt_rec[idx_rec]->srt;

        /* Master while loop over records in current file */
        while(idx_rec_crr_in >= 0L && idx_rec_crr_in < rec_dmn_sz){
          /* Following logic/assumptions built-in to this loop:
	     idx_rec_crr_in points to valid record before loop is entered
	     Loop is never entered if this file has no valid records
	     Much conditional logic needed to prescribe group position and next record

	     Index juggling:
	     idx_rec_crr_in: Index of current record in current input file (increments by 1 for ssc then srd-ssc ...)
	     idx_rec_out: Index of record in output file
	     lmt_rec->rec_rmn_prv_ssc: Structure member, at start of this while loop, contains records remaining-to-be-read to complete subcycle group from previous file. Structure member remains constant until next file is read.
	     rec_in_cml: Cumulative number of records, read or not, in all files opened so far. Similar to lmt_rec->rec_in_cml but augmented at end of record loop, rather than prior to record loop.
	     rec_rmn_prv_ssc: Local copy initialized from lmt_rec structure member begins with above, and then is set to and tracks number of records remaining remaining in current group. This means it is decremented from ssc_nbr->0 for each group contained in current file.
	     rec_usd_cml: Cumulative number of input records used (catenated by ncrcat or operated on by ncra)

	     Flag juggling:
	     REC_LST_DSR is "sloppy"---it is only set in last input file. If last file(s) is/are superfluous, REC_LST_DSR is never set and final normalization is done outside file and record loops (along with nces normalization). FLG_BFR_NRM indicates these situations and allow us to be "sloppy" in setting REC_LST_DSR. */

          /* Last stride in file has distinct index-augmenting behavior */
          if(idx_rec_crr_in >= lmt_rec[idx_rec]->end) REC_SRD_LST=True; else REC_SRD_LST=False;
          /* Even strides commence group beginnings */
          if(rec_rmn_prv_ssc == 0L) REC_FRS_GRP=True; else REC_FRS_GRP=False;
          /* Each group comprises SSC records */
          if(REC_FRS_GRP) rec_rmn_prv_ssc=lmt_rec[idx_rec]->ssc;
          /* Final record triggers normalization regardless of its location within group */
          if(fl_idx == fl_nbr-1 && idx_rec_crr_in == min_int(lmt_rec[idx_rec]->end+lmt_rec[idx_rec]->ssc-1L,rec_dmn_sz-1L)) REC_LST_DSR[idx_rec]=True;
          /* ncra normalization/writing code must know last record in current group (LRCG) for both MRO and non-MRO */
          if(rec_rmn_prv_ssc == 1L) REC_LST_GRP=True; else REC_LST_GRP=False;

          /* Process all variables in current record */
          if(nco_dbg_lvl >= nco_dbg_scl) (void)fprintf(fp_stdout,"%s: INFO Record %ld of %s contributes to output record %ld\n",nco_prg_nm_get(),idx_rec_crr_in,fl_in,idx_rec_out[idx_rec]);

#ifdef _OPENMP
#pragma omp parallel for default(none) private(idx,in_id) shared(CNV_ARM,base_time_crr,base_time_srt,nco_dbg_lvl,fl_in,fl_out,idx_rec_crr_in,idx_rec_out,rec_usd_cml,in_id_arr,REC_FRS_GRP,REC_LST_DSR,md5,nbr_var_prc,nco_op_typ,FLG_BFR_NRM,FLG_MRO,out_id,nco_prg_id,rcd,var_prc,var_prc_out,nbr_dmn_fl,trv_tbl,var_trv,grp_id,gpe,grp_out_fll,grp_out_id,var_out_id,idx_rec,flg_skp1,flg_skp2,lmt_rec,nbr_rec)
#endif /* !_OPENMP */
          for(idx=0;idx<nbr_var_prc;idx++){

            /* Skip variable if does not relate to current record */
            flg_skp1=nco_skp_var(var_prc[idx],lmt_rec[idx_rec]->nm_fll,trv_tbl);
            if(flg_skp1) continue;

            in_id=in_id_arr[omp_get_thread_num()];
            if(nco_dbg_lvl >= nco_dbg_var) rcd+=nco_var_prc_crr_prn(idx,var_prc[idx]->nm_fll);
            if(nco_dbg_lvl >= nco_dbg_var) (void)fflush(fp_stderr);

            /* Obtain variable GTT object using full variable name */
            var_trv=trv_tbl_var_nm_fll(var_prc[idx]->nm_fll,trv_tbl);
            /* Obtain group ID using full group name */
            (void)nco_inq_grp_full_ncid(in_id,var_trv->grp_nm_fll,&grp_id);
            /* Edit group name for output */
            grp_out_fll=NULL;
            if(gpe) grp_out_fll=nco_gpe_evl(gpe,var_trv->grp_nm_fll); else grp_out_fll=var_trv->grp_nm_fll;
            /* Obtain output group ID using full group name */
            (void)nco_inq_grp_full_ncid(out_id,grp_out_fll,&grp_out_id);
            /* Get variable ID */
            (void)nco_inq_varid(grp_out_id,var_trv->nm,&var_out_id);
            /* Memory management after current extracted group */
            if(gpe && grp_out_fll) grp_out_fll=(char *)nco_free(grp_out_fll);

            /* Store the output variable ID */
            var_prc_out[idx]->id=var_out_id;

            /* Retrieve variable from disk into memory. NB: Using version that updates hyperslab start indices with idx_rec_crr_in */
            (void)nco_msa_var_get_lmn_trv(in_id,var_prc[idx],lmt_rec[idx_rec]->nm_fll,idx_rec_crr_in,trv_tbl);

            if(nco_prg_id == ncra) FLG_BFR_NRM=True; /* [flg] Current output buffers need normalization */

            /* Re-base record coordinate and bounds if necessary (e.g., time, time_bnds) */
            if(lmt_rec[idx_rec]->origin != 0.0 && (var_prc[idx]->is_crd_var || nco_is_spc_in_bnd_att(grp_id,var_prc[idx]->id))){
              var_sct *var_crd;
              scv_sct scv;
              /* De-reference */
              var_crd=var_prc[idx];
              scv.val.d=lmt_rec[idx_rec]->origin;              
              scv.type=NC_DOUBLE;  
              /* Convert scalar to variable type */
              nco_scv_cnf_typ(var_crd->type,&scv);
              (void)var_scv_add(var_crd->type,var_crd->sz,var_crd->has_mss_val,var_crd->mss_val,var_crd->val,&scv);
            } /* end re-basing */

            if(nco_prg_id == ncra){
              nco_bool flg_rth_ntl;
              if(!rec_usd_cml[idx_rec] || (FLG_MRO && REC_FRS_GRP)) flg_rth_ntl=True; else flg_rth_ntl=False;
              /* Initialize tally and accumulation arrays when appropriate */
              if(flg_rth_ntl){
                (void)nco_zero_long(var_prc_out[idx]->sz,var_prc_out[idx]->tally);
                (void)nco_var_zero(var_prc_out[idx]->type,var_prc_out[idx]->sz,var_prc_out[idx]->val);
              } /* end if flg_rth_ntl */

              if(var_prc[idx]->type == NC_CHAR || var_prc[idx]->type == NC_STRING){
		/* Do not promote un-averagable types (NC_CHAR, NC_STRING)
		   Stuff their first record into output buffer regardless of nco_op_typ, and ignore later records (rec_usd_cml > 1)
		   Temporarily fixes TODO nco941 */
                if(flg_rth_ntl) nco_opr_drv((long)0L,nco_op_min,var_prc[idx],var_prc_out[idx]);
              }else{
                /* Convert char, short, long, int types to doubles before arithmetic
		   Output variable type is "sticky" so only convert on first record */
                if(flg_rth_ntl) var_prc_out[idx]=nco_typ_cnv_rth(var_prc_out[idx],nco_op_typ);
                var_prc[idx]=nco_var_cnf_typ(var_prc_out[idx]->type,var_prc[idx]);
                /* Perform arithmetic operations: avg, min, max, ttl, ... */
                if(flg_rth_ntl) nco_opr_drv((long)0L,nco_op_typ,var_prc[idx],var_prc_out[idx]); else nco_opr_drv((long)1L,nco_op_typ,var_prc[idx],var_prc_out[idx]);
              } /* end else */ 
            } /* end if ncra */

            /* All processed variables contain record dimension and both ncrcat and ncra write records singly */
            var_prc_out[idx]->srt[0]=var_prc_out[idx]->end[0]=idx_rec_out[idx_rec];
            var_prc_out[idx]->cnt[0]=1L;

            /* Append current record to output file */
            if(nco_prg_id == ncrcat){
              /* Replace this time_offset value with time_offset from initial file base_time */
              if(CNV_ARM && !strcmp(var_prc[idx]->nm,"time_offset")) var_prc[idx]->val.dp[0]+=(base_time_crr-base_time_srt);
#ifdef _OPENMP
#pragma omp critical
#endif /* _OPENMP */
              if(var_prc_out[idx]->sz_rec > 1L) (void)nco_put_vara(grp_out_id,var_prc_out[idx]->id,var_prc_out[idx]->srt,var_prc_out[idx]->cnt,var_prc[idx]->val.vp,var_prc_out[idx]->type); else (void)nco_put_var1(grp_out_id,var_prc_out[idx]->id,var_prc_out[idx]->srt,var_prc[idx]->val.vp,var_prc_out[idx]->type);
              /* Perform MD5 digest of input and output data if requested */
              if(md5) (void)nco_md5_chk(md5,var_prc_out[idx]->nm,var_prc_out[idx]->sz*nco_typ_lng(var_prc_out[idx]->type),grp_out_id,var_prc_out[idx]->srt,var_prc_out[idx]->cnt,var_prc[idx]->val.vp);
            } /* end if ncrcat */

            /* Warn if record coordinate, if any, is not monotonic */
            if(nco_prg_id == ncrcat && var_prc[idx]->is_crd_var) (void)rec_crd_chk(var_prc[idx],fl_in,fl_out,idx_rec_crr_in,idx_rec_out[idx_rec]);
            /* Convert missing_value, if any, back to unpacked type
	       Otherwise missing_value will be double-promoted when next record read
	       Do not convert after last record otherwise normalization fails
	       due to wrong missing_value type (needs promoted type, not unpacked type) */
            if(var_prc[idx]->has_mss_val && var_prc[idx]->type != var_prc[idx]->typ_upk && !REC_LST_DSR[idx_rec]) var_prc[idx]=nco_cnv_mss_val_typ(var_prc[idx],var_prc[idx]->typ_upk);
            /* Free current input buffer */
            var_prc[idx]->val.vp=nco_free(var_prc[idx]->val.vp);
          } /* end (OpenMP parallel for) loop over variables */

          if(nco_prg_id == ncra && ((FLG_MRO && REC_LST_GRP) || REC_LST_DSR[idx_rec])){
            /* Normalize, multiply, etc where necessary: ncra and nces normalization blocks are identical, 
	       except ncra normalizes after every ssc records, while nces normalizes once, after files loop. 
	       20131210: nco_cnv_mss_val_typ() can cause type of var_prc to be out-of-sync with var_prc_out
	       nco_cnv_mss_val_typ() above works correctly for case of packing/unpacking, not for rth_dbl
	       Options:
	       1. Avoid nco_cnv_mss_val_typ() above if rth_dbl is invoked. Keep it for packing.
	       2. In nco_opr_nrm() below, use mss_val from var_prc_out not var_prc
	       Problem is var_prc[idx]->mss_val is typ_upk while var_prc_out is type, so normalization
	       sets missing var_prc_out value to var_prc[idx]->mss_val read as type */
            (void)nco_opr_nrm(nco_op_typ,nbr_var_prc,var_prc,var_prc_out,lmt_rec[idx_rec]->nm_fll,trv_tbl);
            FLG_BFR_NRM=False; /* [flg] Current output buffers need normalization */

            /* Copy averages to output file */
            for(idx=0;idx<nbr_var_prc;idx++){

              /* Skip variables that do not contain current record dimension */
              flg_skp2=nco_skp_var(var_prc[idx],lmt_rec[idx_rec]->nm_fll,trv_tbl);
              if(flg_skp2) continue;

              /* Obtain variable GTT object using full variable name */
              var_trv=trv_tbl_var_nm_fll(var_prc_out[idx]->nm_fll,trv_tbl);
              /* Edit group name for output */
              if(gpe) grp_out_fll=nco_gpe_evl(gpe,var_trv->grp_nm_fll); else grp_out_fll=(char *)strdup(var_trv->grp_nm_fll);
              /* Obtain output group ID using full group name */
              (void)nco_inq_grp_full_ncid(out_id,grp_out_fll,&grp_out_id);
              /* Memory management after current extracted group */
              if(grp_out_fll) grp_out_fll=(char *)nco_free(grp_out_fll);

              var_prc_out[idx]=nco_var_cnf_typ(var_prc_out[idx]->typ_upk,var_prc_out[idx]);
              /* Packing/Unpacking */
              if(nco_pck_plc == nco_pck_plc_all_new_att) var_prc_out[idx]=nco_put_var_pck(grp_out_id,var_prc_out[idx],nco_pck_plc);
              if(var_prc_out[idx]->nbr_dim == 0) (void)nco_put_var1(grp_out_id,var_prc_out[idx]->id,var_prc_out[idx]->srt,var_prc_out[idx]->val.vp,var_prc_out[idx]->type); else (void)nco_put_vara(grp_out_id,var_prc_out[idx]->id,var_prc_out[idx]->srt,var_prc_out[idx]->cnt,var_prc_out[idx]->val.vp,var_prc_out[idx]->type);
            } /* end loop over idx */
            idx_rec_out[idx_rec]++; /* [idx] Index of current record in output file (0 is first, ...) */
          } /* end if normalize and write */

          /* Prepare indices and flags for next iteration */
          if(nco_prg_id == ncrcat) idx_rec_out[idx_rec]++; /* [idx] Index of current record in output file (0 is first, ...) */
          rec_usd_cml[idx_rec]++; /* [nbr] Cumulative number of input records used (catenated by ncrcat or operated on by ncra) */
          if(nco_dbg_lvl >= nco_dbg_var) (void)fprintf(fp_stderr,"\n");

          /* Finally, set index for next record or get outta' Dodge */
          if(REC_SRD_LST){
            /* Last index depends on whether user-specified end was exact, sloppy, or caused truncation */
            long end_max_crr;
            end_max_crr=min_lng(lmt_rec[idx_rec]->idx_end_max_abs-rec_in_cml[idx_rec],min_lng(lmt_rec[idx_rec]->end+lmt_rec[idx_rec]->ssc-1L,rec_dmn_sz-1L));
            if(--rec_rmn_prv_ssc > 0L && idx_rec_crr_in < end_max_crr) idx_rec_crr_in++; else break;
          }else{ /* !REC_SRD_LST */
            if(--rec_rmn_prv_ssc > 0L) idx_rec_crr_in++; else idx_rec_crr_in+=lmt_rec[idx_rec]->srd-lmt_rec[idx_rec]->ssc+1L;
          } /* !REC_SRD_LST */

        } /* end idx_rec_crr_in master while loop over records in current file */

        rec_in_cml[idx_rec]+=rec_dmn_sz; /* [nbr] Cumulative number of records in all files opened so far */
        lmt_rec[idx_rec]->rec_rmn_prv_ssc=rec_rmn_prv_ssc;

        if(fl_idx == fl_nbr-1){
          /* Warn if other than number of requested records were read */
          if(lmt_rec[idx_rec]->lmt_typ == lmt_dmn_idx && lmt_rec[idx_rec]->is_usr_spc_min && lmt_rec[idx_rec]->is_usr_spc_max){
            long ssc_grp_nbr_max; /* [nbr] Subcycle groups that start within range */
            long rec_nbr_rqs; /* Number of records user requested */
            long rec_nbr_rqs_max; /* [nbr] Records that would be used by ssc_grp_nbr_max groups */
            long rec_nbr_spn_act; /* [nbr] Records available within user-specified range */
            long rec_nbr_spn_max; /* [nbr] Minimum record number spanned by ssc_grp_nbr_max groups */
            long rec_nbr_trn; /* [nbr] Records truncated in last group */
            long srd_nbr_flr; /* [nbr] Whole strides that fit within specified range */
            /* Number of whole strides that fit within specified range */
            srd_nbr_flr=(lmt_rec[idx_rec]->max_idx-lmt_rec[idx_rec]->min_idx)/lmt_rec[idx_rec]->srd;
            ssc_grp_nbr_max=1L+srd_nbr_flr;
            /* Number of records that would be used by N groups */
            rec_nbr_rqs_max=ssc_grp_nbr_max*lmt_rec[idx_rec]->ssc;
            /* Minimum record number spanned by N groups of size D is N-1 strides, plus D-1 trailing members of last group */
            rec_nbr_spn_max=lmt_rec[idx_rec]->srd*(ssc_grp_nbr_max-1L)+lmt_rec[idx_rec]->ssc;
            /* Actual number of records available within range */
            rec_nbr_spn_act=1L+lmt_rec[idx_rec]->max_idx-lmt_rec[idx_rec]->min_idx;
            /* Number truncated in last group */
            rec_nbr_trn=max_int(rec_nbr_spn_max-rec_nbr_spn_act,0L);
            /* Records requested is maximum minus any truncated in last group */
            rec_nbr_rqs=rec_nbr_rqs_max-rec_nbr_trn;
            if(rec_nbr_rqs != rec_usd_cml[idx_rec]) (void)fprintf(fp_stdout,"%s: WARNING User requested %li records but only %li were found and used\n",nco_prg_nm_get(),rec_nbr_rqs,rec_usd_cml[idx_rec]);
          } /* end if */
          /* ... and die if no records were read ... */
          if(rec_usd_cml[idx_rec] <= 0){
            (void)fprintf(fp_stdout,"%s: ERROR No records lay within specified hyperslab\n",nco_prg_nm_get());
            nco_exit(EXIT_FAILURE);
          } /* end if */
        } /* end if */

      } /* end idx_rec loop over different record variables to process */

      /* End ncra, ncrcat section */
    }else if(nco_prg_id == ncfe){ /* ncfe */

#ifdef _OPENMP
#pragma omp parallel for default(none) private(idx,in_id) shared(nco_dbg_lvl,fl_idx,FLG_BFR_NRM,in_id_arr,nbr_var_prc,nco_op_typ,rcd,var_prc,var_prc_out,nbr_dmn_fl,trv_tbl,var_trv,grp_id,gpe,grp_out_fll,grp_out_id,out_id,var_out_id)
#endif /* !_OPENMP */
      for(idx=0;idx<nbr_var_prc;idx++){ /* Process all variables in current file */

        in_id=in_id_arr[omp_get_thread_num()];
        if(nco_dbg_lvl >= nco_dbg_var) rcd+=nco_var_prc_crr_prn(idx,var_prc[idx]->nm);
        if(nco_dbg_lvl >= nco_dbg_var) (void)fflush(fp_stderr);

        /* Obtain variable GTT object using full variable name */
        var_trv=trv_tbl_var_nm_fll(var_prc[idx]->nm_fll,trv_tbl);
        /* Obtain group ID using full group name */
        (void)nco_inq_grp_full_ncid(in_id,var_trv->grp_nm_fll,&grp_id);
        /* Edit group name for output */
        if(gpe) grp_out_fll=nco_gpe_evl(gpe,var_trv->grp_nm_fll); else grp_out_fll=(char *)strdup(var_trv->grp_nm_fll);
        /* Obtain output group ID using full group name */
        (void)nco_inq_grp_full_ncid(out_id,grp_out_fll,&grp_out_id);
        /* Memory management after current extracted group */
        if(grp_out_fll) grp_out_fll=(char *)nco_free(grp_out_fll);
        /* Get variable ID */
        (void)nco_inq_varid(grp_out_id,var_trv->nm,&var_out_id);

        /* Store the output variable ID */
        var_prc_out[idx]->id=var_out_id;

        /* Retrieve variable from disk into memory */
        (void)nco_msa_var_get_trv(in_id,var_prc[idx],trv_tbl);

        /* Convert char, short, long, int types to doubles before arithmetic
        Output variable type is "sticky" so only convert on first record */
        if(fl_idx == 0) var_prc_out[idx]=nco_typ_cnv_rth(var_prc_out[idx],nco_op_typ);
        var_prc[idx]=nco_var_cnf_typ(var_prc_out[idx]->type,var_prc[idx]);
        /* Perform arithmetic operations: avg, min, max, ttl, ... */ /* Note: fl_idx not rec_usd_cml! */
        nco_opr_drv(fl_idx,nco_op_typ,var_prc[idx],var_prc_out[idx]);
        FLG_BFR_NRM=True; /* [flg] Current output buffers need normalization */

        /* Free current input buffer */
        var_prc[idx]->val.vp=nco_free(var_prc[idx]->val.vp);
      } /* end (OpenMP parallel for) loop over idx */
      /* End ncfe section */
    }else if(nco_prg_id == ncge){ /* ncge */

      trv_tbl_sct *trv_tbl1;    /* [lst] Traversal table (needed for multi-file cases)  */

      /* Initialize traversal table */ 
      trv_tbl_init(&trv_tbl1); 

      /* Construct GTT using current file ID */
      (void)nco_bld_trv_tbl(in_id,trv_pth,lmt_nbr,lmt_arg,aux_nbr,aux_arg,MSA_USR_RDR,FORTRAN_IDX_CNV,grp_lst_in,grp_lst_in_nbr,var_lst_in,var_lst_in_nbr,EXTRACT_ALL_COORDINATES,GRP_VAR_UNN,False,EXCLUDE_INPUT_LIST,EXTRACT_ASSOCIATED_COORDINATES,nco_pck_plc_nil,&flg_dne,trv_tbl1);

      /* Were all user-specified dimensions found? */ 
      (void)nco_chk_dmn(lmt_nbr,flg_dne);  

      /* Loop over ensembles in current file */
      for(int idx_nsm=0;idx_nsm<trv_tbl->nsm_nbr;idx_nsm++){ 

        if(nco_dbg_lvl > nco_dbg_std) (void)fprintf(stdout,"%s: ensemble %d: %s\n",nco_prg_nm_get(),idx_nsm,trv_tbl->nsm[idx_nsm].grp_nm_fll_prn);

        int mbr_srt=trv_tbl->nsm[idx_nsm].mbr_srt;
        int mbr_end=trv_tbl->nsm[idx_nsm].mbr_end;

        /* Loop over members of current ensemble (use start and end members, multi file cases) */
        for(int idx_mbr=mbr_srt;idx_mbr<mbr_end;idx_mbr++){

          /* Loop over all variables */
          for(int idx_prc=0;idx_prc<nbr_var_prc;idx_prc++){ 

            /* Obtain variable GTT object for the member *variable* in ensemble (the ones to average) */
            var_trv=trv_tbl_var_nm_fll(var_prc[idx_prc]->nm_fll,trv_tbl);
            assert(var_trv);

            /* Skip if from different ensembles */
            if(strcmp(var_trv->nsm_nm,trv_tbl->nsm[idx_nsm].grp_nm_fll_prn)) continue;

            /* Build new variable name */
            char *grp_nm_fll=trv_tbl->nsm[idx_nsm].mbr[idx_mbr].mbr_nm_fll; 
            char *var_nm_fll=nco_bld_nm_fll(grp_nm_fll,var_prc[idx_prc]->nm);;
            char *nm_fll=strdup(var_prc[idx_prc]->nm_fll);

            var_prc[idx_prc]->nm_fll=(char *)nco_free(var_prc[idx_prc]->nm_fll);
            var_prc[idx_prc]->nm_fll=nco_bld_nm_fll(grp_nm_fll,var_prc[idx_prc]->nm);
            if(nco_dbg_lvl > nco_dbg_std) (void)fprintf(fp_stdout,"%s:\t variable <%s>\n",nco_prg_nm_get(),var_prc[idx_prc]->nm_fll); 

            /* Obtain group ID using full group name */
            (void)nco_inq_grp_full_ncid(in_id,grp_nm_fll,&grp_id);
            (void)nco_var_mtd_refresh(grp_id,var_prc[idx_prc]);

            /* Retrieve variable from disk into memory. NB: Using table in file loop */
            (void)nco_msa_var_get_trv(in_id,var_prc[idx_prc],trv_tbl1);

            /* Convert char, short, long, int types to doubles before arithmetic
            Output variable type is "sticky" so only convert on first member */
            if(fl_idx == 0 && idx_mbr == 0) var_prc_out[idx_prc]=nco_typ_cnv_rth(var_prc_out[idx_prc],nco_op_typ);
            var_prc[idx_prc]=nco_var_cnf_typ(var_prc_out[idx_prc]->type,var_prc[idx_prc]);
            /* Perform arithmetic operations: avg, min, max, ttl, ... */
            nco_opr_drv(fl_idx+idx_mbr,nco_op_typ,var_prc[idx_prc],var_prc_out[idx_prc]);
            FLG_BFR_NRM=True; /* [flg] Current output buffers need normalization */

            /* Put old name back */
            var_prc[idx_prc]->nm_fll=(char *)nco_free(var_prc[idx_prc]->nm_fll);
            var_prc[idx_prc]->nm_fll=strdup(nm_fll);

            /* Free current input buffer */
            var_prc[idx_prc]->val.vp=nco_free(var_prc[idx_prc]->val.vp);

            /* Free built variable name */
            var_nm_fll=(char *)nco_free(var_nm_fll);
            nm_fll=(char *)nco_free(nm_fll);

          } /* end loop over var_prc */
        } /* end loop over mbr */

      } /* Loop over ensembles in current file */

      (void)trv_tbl_free(trv_tbl1);

    } /* End ncge section */

    /* For ncge, save helpful metadata for later handling by ncbo */
    if(nco_prg_id == ncge && fl_idx == 0) (void)nco_nsm_wrt_att(in_id,out_id,gpe,trv_tbl);   

    if(nco_dbg_lvl >= nco_dbg_scl) (void)fprintf(fp_stderr,"\n");

    /* Close input netCDF file */
    for(thr_idx=0;thr_idx<thr_nbr;thr_idx++) nco_close(in_id_arr[thr_idx]);

    /* Dispose local copy of file */
    if(FL_RTR_RMT_LCN && RM_RMT_FL_PST_PRC) (void)nco_fl_rm(fl_in);

    /* Are all our data tanks already full? */
    if(nco_prg_id == ncra || nco_prg_id == ncrcat){
      for(idx_rec=0;idx_rec<nbr_rec;idx_rec++){
	if(!flg_input_complete[idx_rec]){
	  if((flg_input_complete[idx_rec]=lmt_rec[idx_rec]->flg_input_complete)){
	    /* NB: TODO nco1066 move input_complete break to precede record loop but remember to close open filehandles */
	    /* 20131209: Rewritten so file skipped only once all record dimensions have flg_input_complete
	       Warnings about superfluous files printed only once per dimension
	       fxm: use flg_input_complete[idx_rec] to skip completed entries in main record dimension loop above */
	    if(nco_dbg_lvl >= nco_dbg_std) (void)fprintf(fp_stderr,"%s: INFO All requested records for record dimension #%d (%s) were found within the first %d input file%s, next file was opened then skipped, and remaining %d input file%s need not be opened\n",nco_prg_nm_get(),idx_rec,lmt_rec[idx_rec]->nm_fll,fl_idx,(fl_idx == 1) ? "" : "s",fl_nbr-fl_idx-1,(fl_nbr-fl_idx-1 == 1) ? "" : "s");
	    flg_input_complete_nbr++;
	  } /* endif superfluous */
	} /* endif not already known to be complete */
      } /* end loop over record dimensions */
      /* Once all record dimensions are complete, break-out of file loop */
      if(flg_input_complete_nbr == nbr_rec) break;
    } /* endif ncra || ncrcat */

  } /* end loop over fl_idx */

  /* Subcycle argument warning */
  if(nco_prg_id == ncra || nco_prg_id == ncrcat){ /* fxm: Remove this or make DBG when crd_val SSC/MRO is predictable? */
    /* Loop records */
    for(idx_rec=0;idx_rec<nbr_rec;idx_rec++){
      /* Check subcycle for each record */
      if(lmt_rec[idx_rec]->ssc != 1L && (lmt_rec[idx_rec]->lmt_typ == lmt_crd_val || lmt_rec[idx_rec]->lmt_typ == lmt_udu_sng)){
        (void)fprintf(stderr,"\n%s: WARNING Subcycle argument SSC used in hyperslab specification for %s which will be determined based on coordinate values rather than dimension indices. The behavior of the subcycle hyperslab argument is ambiguous for coordinate-based hyperslabs---it could mean select the first SSC elements that are within the min and max coordinate values beginning with each strided point, or it could mean always select the first _consecutive_ SSC elements beginning with each strided point (regardless of their values relative to min and max). For such hyperslabs, NCO adopts the latter definition and always selects the group of SSC records beginning with each strided point. Strided points are guaranteed to be within the min and max coordinates, but the subsequent members of each group are not, though this is only the case if the record coordinate is not monotonic. The record coordinate is almost always monotonic, so surprises are only expected in a corner case unlikely to affect the vast majority of users. You have been warned. Use at your own risk.\n",nco_prg_nm_get(),lmt_rec[idx_rec]->nm);
      } /* Check subcycle for each record */
    } /* Loop records */
  } /* Subcycle argument warning */

  /* Normalize, multiply, etc where necessary: ncra and nces normalization blocks are identical, 
     except ncra normalizes after every SSC records, while nces normalizes once, after all files.
     Occassionally last input file(s) is/are superfluous so REC_LST_DSR never set
     In such cases FLG_BFR_NRM is still true, indicating ncra still needs normalization
     FLG_BFR_NRM is always true here for ncfe and ncge */
  if(FLG_BFR_NRM) (void)nco_opr_nrm(nco_op_typ,nbr_var_prc,var_prc,var_prc_out,(char *)NULL,(trv_tbl_sct *)NULL);

  /* Manually fix YYMMDD date which was mangled by averaging */
  if(CNV_CCM_CCSM_CF && nco_prg_id == ncra) (void)nco_cnv_ccm_ccsm_cf_date(grp_out_id,var_out,xtr_nbr);

  /* Add time variable to output file
     NB: nco_cnv_arm_time_install() contains OpenMP critical region */
  if(CNV_ARM && nco_prg_id == ncrcat) (void)nco_cnv_arm_time_install(grp_out_id,base_time_srt,dfl_lvl);

  /* Copy averages to output file for ncfe and ncge always and for ncra when trailing file(s) was/were superfluous */
  if(FLG_BFR_NRM){
    for(idx=0;idx<nbr_var_prc;idx++){

      /* Obtain variable GTT object using full variable name */
      var_trv=trv_tbl_var_nm_fll(var_prc_out[idx]->nm_fll,trv_tbl);

      /* For ncge, group to save is ensemble parent group */
      if(nco_prg_id == ncge){

        /* Check if suffix needed. Appends to default name */
        if(trv_tbl->nsm_sfx){
          /* Define (append) then use and forget new name */
          char *nm_fll_sfx=nco_bld_nsm_sfx(var_trv->grp_nm_fll_prn,trv_tbl);
          /* Use new name */
          if(gpe) grp_out_fll=nco_gpe_evl(gpe,nm_fll_sfx); else grp_out_fll=(char *)strdup(nm_fll_sfx);
          nm_fll_sfx=(char *)nco_free(nm_fll_sfx);
        }else{ /* Non suffix case */
          if(gpe) grp_out_fll=nco_gpe_evl(gpe,var_trv->nsm_nm); else grp_out_fll=(char *)strdup(var_trv->nsm_nm);
        } /* !trv_tbl->nsm_sfx */

      }else if(nco_prg_id == ncfe){
        /* Edit group name for output */
        if(gpe) grp_out_fll=nco_gpe_evl(gpe,var_trv->grp_nm_fll); else grp_out_fll=(char *)strdup(var_trv->grp_nm_fll);
      } /* end else */

      /* Obtain output group ID using full group name */
      (void)nco_inq_grp_full_ncid(out_id,grp_out_fll,&grp_out_id);
      /* Get output variable ID */
      (void)nco_inq_varid(grp_out_id,var_prc_out[idx]->nm,&var_out_id);   
      /* Store the output variable ID */
      var_prc_out[idx]->id=var_out_id;

      var_prc_out[idx]=nco_var_cnf_typ(var_prc_out[idx]->typ_upk,var_prc_out[idx]);
      /* Packing/Unpacking */
      if(nco_pck_plc == nco_pck_plc_all_new_att) var_prc_out[idx]=nco_put_var_pck(grp_out_id,var_prc_out[idx],nco_pck_plc);
      if(var_prc_out[idx]->nbr_dim == 0) (void)nco_put_var1(grp_out_id,var_prc_out[idx]->id,var_prc_out[idx]->srt,var_prc_out[idx]->val.vp,var_prc_out[idx]->type); else (void)nco_put_vara(grp_out_id,var_prc_out[idx]->id,var_prc_out[idx]->srt,var_prc_out[idx]->cnt,var_prc_out[idx]->val.vp,var_prc_out[idx]->type);

    } /* end loop over idx */
  } /* end if ncfe and ncge */


  /* Free averaging and tally buffers */
  if(nco_prg_id == ncra || nco_prg_id == ncfe || nco_prg_id == ncge){
#ifdef _OPENMP
#pragma omp parallel for default(none) private(idx) shared(nbr_var_prc,nco_op_typ,var_prc,var_prc_out)
#endif /* !_OPENMP */
    for(idx=0;idx<nbr_var_prc;idx++){
      var_prc_out[idx]->tally=var_prc[idx]->tally=(long *)nco_free(var_prc[idx]->tally);
      var_prc_out[idx]->val.vp=nco_free(var_prc_out[idx]->val.vp);
    } /* end loop over idx */
  } /* endif ncra || nces */

  /* Close output file and move it from temporary to permanent location */
  (void)nco_fl_out_cls(fl_out,fl_out_tmp,out_id);

  /* Clean memory unless dirty memory allowed */
  if(flg_cln){
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
    if(var_lst_in_nbr > 0) var_lst_in=nco_sng_lst_free(var_lst_in,var_lst_in_nbr);
    /* Free limits */
    for(idx=0;idx<lmt_nbr;idx++) lmt_arg[idx]=(char *)nco_free(lmt_arg[idx]);
    /* NB: lmt[idx] was free()'d earlier */
    for(idx=0;idx<aux_nbr;idx++) aux_arg[idx]=(char *)nco_free(aux_arg[idx]);
    /* Free chunking information */
    for(idx=0;idx<cnk_nbr;idx++) cnk_arg[idx]=(char *)nco_free(cnk_arg[idx]);
    if(cnk_nbr > 0) cnk.cnk_dmn=(cnk_dmn_sct **)nco_cnk_lst_free(cnk.cnk_dmn,cnk_nbr);
    /* Free dimension lists */
    if(nbr_dmn_xtr > 0) dim=nco_dmn_lst_free(dim,nbr_dmn_xtr);
    if(nbr_dmn_xtr > 0) dmn_out=nco_dmn_lst_free(dmn_out,nbr_dmn_xtr);
    /* Free variable lists */
    if(xtr_nbr > 0) var=nco_var_lst_free(var,xtr_nbr);
    if(xtr_nbr > 0) var_out=nco_var_lst_free(var_out,xtr_nbr);
    var_prc=(var_sct **)nco_free(var_prc);
    var_prc_out=(var_sct **)nco_free(var_prc_out);
    var_fix=(var_sct **)nco_free(var_fix);
    var_fix_out=(var_sct **)nco_free(var_fix_out);
    if(md5) md5=(md5_sct *)nco_md5_free(md5);

    (void)trv_tbl_free(trv_tbl);
    for(idx=0;idx<lmt_nbr;idx++) flg_dne[idx].dim_nm=(char *)nco_free(flg_dne[idx].dim_nm);
    if (flg_dne) flg_dne=(nco_dmn_dne_t *)nco_free(flg_dne);
    if(flg_input_complete) flg_input_complete=(nco_bool *)nco_free(flg_input_complete);
    if(idx_rec_out) idx_rec_out=(long *)nco_free(idx_rec_out);
    if(rec_in_cml) rec_in_cml=(long *)nco_free(rec_in_cml);
    if(rec_usd_cml) rec_usd_cml=(long *)nco_free(rec_usd_cml);
    if(REC_LST_DSR) REC_LST_DSR=(nco_bool *)nco_free(REC_LST_DSR);

  } /* !flg_cln */

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
