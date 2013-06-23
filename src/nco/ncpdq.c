/* $Header: /data/zender/nco_20150216/nco/src/nco/ncpdq.c,v 1.318 2013-06-23 06:45:59 pvicente Exp $ */

/* ncpdq -- netCDF pack, re-dimension, query */

/* Purpose: Pack, re-dimension, query single netCDF file and output to a single file */

/* Copyright (C) 1995--2013 Charlie Zender

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
   ncpdq -O -D 3 -a lat,lev,lon -v three_dmn_var ~/nco/data/in.nc ~/foo.nc;ncks -P ~/foo.nc
   ncpdq -O -D 3 -a lon,lev,lat -v three_dmn_var ~/nco/data/in.nc ~/foo.nc;ncks -P ~/foo.nc
   ncpdq -O -D 3 -a lon,time -x -v three_double_dmn ~/nco/data/in.nc ~/foo.nc;ncks -P ~/foo.nc
   ncpdq -O -D 3 -P all_new ~/nco/data/in.nc ~/foo.nc
   ncpdq -O -D 3 -P all_xst ~/nco/data/in.nc ~/foo.nc
   ncpdq -O -D 3 -P xst_new ~/nco/data/in.nc ~/foo.nc
   ncpdq -O -D 3 -P upk ~/nco/data/in.nc ~/foo.nc */

#if 1
#define USE_TRV_API
#endif

#if 1
#define USE_RDR_NETCDF3 /* Use re-ordering functions for netCDF3 (original code inside GTT) */
#endif

#ifdef HAVE_CONFIG_H
# include <config.h> /* Autotools tokens */
#endif /* !HAVE_CONFIG_H */

/* Standard C headers */
#include <math.h> /* sin cos cos sin 3.14159 */
#include <stdio.h> /* stderr, FILE, NULL, etc. */
#include <stdlib.h> /* atof, atoi, malloc, getopt */
#include <string.h> /* strcmp() */
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

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions and C library */

/* #define MAIN_PROGRAM_FILE MUST precede #include libnco.h */
#define MAIN_PROGRAM_FILE
#include "libnco.h" /* netCDF Operator (NCO) library */

void dbg_var_dim_sct(const char* str, const int idx_var, var_sct *var);

int 
main(int argc,char **argv)
{
  aed_sct *aed_lst_add_fst=NULL_CEWI;
  aed_sct *aed_lst_scl_fct=NULL_CEWI;

  nco_bool **dmn_rvr_in=NULL; /* [flg] Reverse dimension */
  nco_bool *dmn_rvr_rdr=NULL; /* [flg] Reverse dimension */
  nco_bool CNV_CCM_CCSM_CF;
  nco_bool EXCLUDE_INPUT_LIST=False; /* Option c */
  nco_bool EXTRACT_ALL_COORDINATES=False; /* Option c */
  nco_bool EXTRACT_ASSOCIATED_COORDINATES=True; /* Option C */
  nco_bool FL_RTR_RMT_LCN;
  nco_bool FL_LST_IN_FROM_STDIN=False; /* [flg] fl_lst_in comes from stdin */
  nco_bool FORCE_APPEND=False; /* Option A */
  nco_bool FORCE_OVERWRITE=False; /* Option O */
  nco_bool FORTRAN_IDX_CNV=False; /* Option F */
  nco_bool GRP_VAR_UNN=False; /* [flg] Select union of specified groups and variables */
  nco_bool HISTORY_APPEND=True; /* Option h */
  nco_bool IS_REORDER=False; /* Re-order mode */
  nco_bool MSA_USR_RDR=False; /* [flg] Multi-Slab Algorithm returns hyperslabs in user-specified order*/
  nco_bool REDEFINED_RECORD_DIMENSION=False; /* [flg] Re-defined record dimension */
  nco_bool RAM_CREATE=False; /* [flg] Create file in RAM */
  nco_bool RAM_OPEN=False; /* [flg] Open (netCDF3-only) file(s) in RAM */
  nco_bool RM_RMT_FL_PST_PRC=True; /* Option R */
  nco_bool WRT_TMP_FL=True; /* [flg] Write output to temporary file */
  nco_bool flg_cln=False; /* [flg] Clean memory prior to exit */

  char **dmn_rdr_lst_in=NULL_CEWI; /* Option a */
  char **fl_lst_abb=NULL; /* Option n */
  char **fl_lst_in=NULL_CEWI;
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
  char *nco_pck_plc_sng=NULL_CEWI; /* [sng] Packing policy Option P */
  char *nco_pck_map_sng=NULL_CEWI; /* [sng] Packing map Option M */
  char *opt_crr=NULL; /* [sng] String representation of current long-option name */
  char *optarg_lcl=NULL; /* [sng] Local copy of system optarg */
  char *rec_dmn_nm_in=NULL; /* [sng] Record dimension name, original */
  char *rec_dmn_nm_out=NULL; /* [sng] Record dimension name, re-ordered */
  char *rec_dmn_nm_out_crr=NULL; /* [sng] Name of record dimension, if any, required by re-order */
  char *sng_cnv_rcd=NULL_CEWI; /* [sng] strtol()/strtoul() return code */

  char add_fst_sng[]="add_offset"; /* [sng] Unidata standard string for add offset */
  char scl_fct_sng[]="scale_factor"; /* [sng] Unidata standard string for scale factor */
  char trv_pth[]="/"; /* [sng] Root path of traversal tree */
  char *grp_out=NULL; /* [sng] Group name */

  const char * const CVS_Id="$Id: ncpdq.c,v 1.318 2013-06-23 06:45:59 pvicente Exp $"; 
  const char * const CVS_Revision="$Revision: 1.318 $";
  const char * const opt_sht_lst="346Aa:CcD:d:Fg:G:hL:l:M:Oo:P:p:Rrt:v:UxZ-:";

  cnk_sct **cnk=NULL_CEWI;

#if defined(__cplusplus) || defined(PGI_CC)
  ddra_info_sct ddra_info;
  ddra_info.flg_ddra=False;
#else /* !__cplusplus */
  ddra_info_sct ddra_info={.flg_ddra=False};
#endif /* !__cplusplus */

  dmn_sct **dim=NULL_CEWI;
  dmn_sct **dmn_out;
  dmn_sct **dmn_rdr=NULL; /* [sct] Dimension structures to be re-ordered */

  extern char *optarg;
  extern int optind;

  /* Using naked stdin/stdout/stderr in parallel region generates warning
  Copy appropriate filehandle to variable scoped shared in parallel clause */
  FILE * const fp_stderr=stderr; /* [fl] stderr filehandle CEWI */
  FILE * const fp_stdout=stdout; /* [fl] stdout filehandle CEWI */

  int **dmn_idx_out_in=NULL; /* [idx] Dimension correspondence, output->input CEWI */

  int *in_id_arr;

  int abb_arg_nbr=0;
  int aux_nbr=0; /* [nbr] Number of auxiliary coordinate hyperslabs specified */
  int cnk_map=nco_cnk_map_nil; /* [enm] Chunking map */
  int cnk_nbr=0; /* [nbr] Number of chunk sizes */
  int cnk_plc=nco_cnk_plc_nil; /* [enm] Chunking policy */
  int dfl_lvl=0; /* [enm] Deflate level */
  int dmn_out_idx; /* [idx] Index over output dimension list */
  int dmn_out_idx_rec_in=NCO_REC_DMN_UNDEFINED; /* [idx] Record dimension index in output dimension list, original */
  int dmn_rdr_nbr=0; /* [nbr] Number of dimension to re-order */
  int dmn_rdr_nbr_in=0; /* [nbr] Original number of dimension to re-order */
  int dmn_rdr_nbr_utl=0; /* [nbr] Number of dimension to re-order, utilized */
  int fl_idx=int_CEWI;
  int fl_nbr=0;
  int fl_in_fmt; /* [enm] Input file format */
  int fl_out_fmt=NCO_FORMAT_UNDEFINED; /* [enm] Output file format */
  int fll_md_old; /* [enm] Old fill mode */
  int idx=int_CEWI;
  int jdx=int_CEWI;
  int idx_rdr=int_CEWI;
  int in_id;  
  int lmt_nbr=0; /* Option d. NB: lmt_nbr gets incremented */
  int md_open; /* [enm] Mode flag for nc_open() call */
  int nbr_dmn_fl;
  int nbr_dmn_out;
  int nbr_dmn_xtr;
  int nbr_var_fix; /* nbr_var_fix gets incremented */
  int nbr_var_fl;
  int nbr_var_prc; /* nbr_var_prc gets incremented */
  int xtr_nbr=0; /* xtr_nbr won't otherwise be set for -c with no -v */
  int nco_pck_map=nco_pck_map_flt_sht; /* [enm] Packing map */
  int nco_pck_plc=nco_pck_plc_nil; /* [enm] Packing policy */
  int opt;
  int out_id;  
  int rcd=NC_NOERR; /* [rcd] Return code */
  int rec_dmn_id_in=NCO_REC_DMN_UNDEFINED; /* [id] Record dimension ID in input file */
  int thr_idx; /* [idx] Index of current thread */
  int thr_nbr=int_CEWI; /* [nbr] Thread number Option t */
  int var_lst_in_nbr=0;
  int grp_lst_in_nbr=0; /* [nbr] Number of groups explicitly specified by user */

  lmt_sct **aux=NULL_CEWI; /* Auxiliary coordinate limits */
  lmt_sct **lmt=NULL_CEWI;
  lmt_msa_sct **lmt_all_lst=NULL_CEWI; /* List of *lmt_all structures */

  nm_id_sct *dmn_lst;
  nm_id_sct *dmn_rdr_lst;
  nm_id_sct *xtr_lst=NULL; /* xtr_lst may be alloc()'d from NULL with -c option */

  size_t bfr_sz_hnt=NC_SIZEHINT_DEFAULT; /* [B] Buffer size hint */
  size_t cnk_sz_scl=0UL; /* [nbr] Chunk size scalar */
  size_t hdr_pad=0UL; /* [B] Pad at end of header section */
  size_t grp_out_lng; /* [nbr] Length of original, canonicalized GPE specification filename component */

  var_sct **var;
  var_sct **var_fix;
  var_sct **var_fix_out;
  var_sct **var_out;
  var_sct **var_prc;
  var_sct **var_prc_out;

  trv_tbl_sct *trv_tbl=NULL; /* [lst] Traversal table */

  gpe_sct *gpe=NULL; /* [sng] Group Path Editing (GPE) structure */

  static struct option opt_lng[]=
  { /* Structure ordered by short option key if possible */
    /* Long options with no argument, no short option counterpart */
    {"cln",no_argument,0,0}, /* [flg] Clean memory prior to exit */
    {"clean",no_argument,0,0}, /* [flg] Clean memory prior to exit */
    {"mmr_cln",no_argument,0,0}, /* [flg] Clean memory prior to exit */
    {"drt",no_argument,0,0}, /* [flg] Allow dirty memory on exit */
    {"dirty",no_argument,0,0}, /* [flg] Allow dirty memory on exit */
    {"mmr_drt",no_argument,0,0}, /* [flg] Allow dirty memory on exit */
    {"msa_usr_rdr",no_argument,0,0}, /* [flg] Multi-Slab Algorithm returns hyperslabs in user-specified order */
    {"ram_all",no_argument,0,0}, /* [flg] Open (netCDF3) and create file(s) in RAM */
    {"create_ram",no_argument,0,0}, /* [flg] Create file in RAM */
    {"open_ram",no_argument,0,0}, /* [flg] Open (netCDF3) file(s) in RAM */
    {"diskless_all",no_argument,0,0}, /* [flg] Open (netCDF3) and create file(s) in RAM */
    {"wrt_tmp_fl",no_argument,0,0}, /* [flg] Write output to temporary file */
    {"write_tmp_fl",no_argument,0,0}, /* [flg] Write output to temporary file */
    {"no_tmp_fl",no_argument,0,0}, /* [flg] Do not write output to temporary file */
    {"version",no_argument,0,0},
    {"vrs",no_argument,0,0},
    /* Long options with argument, no short option counterpart */
    {"bfr_sz_hnt",required_argument,0,0}, /* [B] Buffer size hint */
    {"buffer_size_hint",required_argument,0,0}, /* [B] Buffer size hint */
    {"chunk_map",required_argument,0,0}, /* [nbr] Chunking map */
    {"cnk_plc",required_argument,0,0}, /* [nbr] Chunking policy */
    {"chunk_policy",required_argument,0,0}, /* [nbr] Chunking policy */
    {"cnk_scl",required_argument,0,0}, /* [nbr] Chunk size scalar */
    {"chunk_scalar",required_argument,0,0}, /* [nbr] Chunk size scalar */
    {"cnk_dmn",required_argument,0,0}, /* [nbr] Chunk size */
    {"chunk_dimension",required_argument,0,0}, /* [nbr] Chunk size */
    {"fl_fmt",required_argument,0,0},
    {"hdr_pad",required_argument,0,0},
    {"header_pad",required_argument,0,0},
    /* Long options with short counterparts */
    {"3",no_argument,0,'3'},
    {"4",no_argument,0,'4'},
    {"64bit",no_argument,0,'4'},
    {"netcdf4",no_argument,0,'4'},
    {"append",no_argument,0,'A'},
    {"arrange",required_argument,0,'a'},
    {"permute",required_argument,0,'a'},
    {"reorder",required_argument,0,'a'},
    {"rdr",required_argument,0,'a'},
    {"no-coords",no_argument,0,'C'},
    {"no-crd",no_argument,0,'C'},
    {"coords",no_argument,0,'c'},
    {"crd",no_argument,0,'c'},
    {"debug",required_argument,0,'D'},
    {"dbg_lvl",required_argument,0,'D'},
    {"dimension",required_argument,0,'d'},
    {"dmn",required_argument,0,'d'},
    {"fortran",no_argument,0,'F'},
    {"ftn",no_argument,0,'F'},
    {"gpe",required_argument,0,'G'}, /* [sng] Group Path Edit (GPE) */
    {"grp",required_argument,0,'g'},
    {"group",required_argument,0,'g'},
    {"history",no_argument,0,'h'},
    {"hst",no_argument,0,'h'},
    {"dfl_lvl",required_argument,0,'L'}, /* [enm] Deflate level */
    {"deflate",required_argument,0,'L'}, /* [enm] Deflate level */
    {"local",required_argument,0,'l'},
    {"lcl",required_argument,0,'l'},
    {"pack_map",required_argument,0,'M'},
    {"pck_map",required_argument,0,'M'},
    {"map",required_argument,0,'M'},
    {"overwrite",no_argument,0,'O'},
    {"ovr",no_argument,0,'O'},
    {"output",required_argument,0,'o'},
    {"fl_out",required_argument,0,'o'},
    {"pack_policy",required_argument,0,'P'},
    {"pck_plc",required_argument,0,'P'},
    {"path",required_argument,0,'p'},
    {"retain",no_argument,0,'R'},
    {"rtn",no_argument,0,'R'},
    {"revision",no_argument,0,'r'},
    {"thr_nbr",required_argument,0,'t'},
    {"threads",required_argument,0,'t'},
    {"omp_num_threads",required_argument,0,'t'},
    {"unpack",no_argument,0,'U'},
    {"upk",no_argument,0,'U'},
    {"variable",required_argument,0,'v'},
    {"auxiliary",required_argument,0,'X'},
    {"exclude",no_argument,0,'x'},
    {"xcl",no_argument,0,'x'},
    {"help",no_argument,0,'?'},
    {"hlp",no_argument,0,'?'},
    {0,0,0,0}
  }; /* end opt_lng */
  int opt_idx=0; /* Index of current long option into opt_lng array */

  /* Initialize traversal table */ 
  trv_tbl_init(&trv_tbl);

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
      if(!strcmp(opt_crr,"bfr_sz_hnt") || !strcmp(opt_crr,"buffer_size_hint")){
        bfr_sz_hnt=strtoul(optarg,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
        if(*sng_cnv_rcd) nco_sng_cnv_err(optarg,"strtoul",sng_cnv_rcd);
      } /* endif cnk */
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
      if(!strcmp(opt_crr,"fl_fmt") || !strcmp(opt_crr,"file_format")) rcd=nco_create_mode_prs(optarg,&fl_out_fmt);
      if(!strcmp(opt_crr,"hdr_pad") || !strcmp(opt_crr,"header_pad")){
        hdr_pad=strtoul(optarg,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
        if(*sng_cnv_rcd) nco_sng_cnv_err(optarg,"strtoul",sng_cnv_rcd);
      } /* endif "hdr_pad" */
      if(!strcmp(opt_crr,"msa_usr_rdr")) MSA_USR_RDR=True; /* [flg] Multi-Slab Algorithm returns hyperslabs in user-specified order */
      if(!strcmp(opt_crr,"ram_all") || !strcmp(opt_crr,"create_ram") || !strcmp(opt_crr,"diskless_all")) RAM_CREATE=True; /* [flg] Open (netCDF3) file(s) in RAM */
      if(!strcmp(opt_crr,"ram_all") || !strcmp(opt_crr,"open_ram") || !strcmp(opt_crr,"diskless_all")) RAM_OPEN=True; /* [flg] Create file in RAM */
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
    case 'A': /* Toggle FORCE_APPEND */
      FORCE_APPEND=!FORCE_APPEND;
      break;
    case 'a': /* Re-order dimensions */
      dmn_rdr_lst_in=nco_lst_prs_2D(optarg,",",&dmn_rdr_nbr_in);
      dmn_rdr_nbr=dmn_rdr_nbr_in;
      break;
    case 'C': /* Extract all coordinates associated with extracted variables? */
      EXTRACT_ASSOCIATED_COORDINATES=False;
      break;
    case 'c':
      EXTRACT_ALL_COORDINATES=True;
      break;
    case 'D': /* Debugging level. Default is 0. */
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
    case 'G': /* Apply Group Path Editing (GPE) to output group */
      /* NB: GNU getopt() optional argument syntax is ugly (requires "=" sign) so avoid it
      http://stackoverflow.com/questions/1052746/getopt-does-not-parse-optional-arguments-to-parameters */
      gpe=nco_gpe_prs_arg(optarg);
      grp_out=(char *)strdup(gpe->nm_cnn); /* [sng] Group name */
      grp_out_lng=gpe->lng_cnn;
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
    case 'L': /* [enm] Deflate level. Default is 0. */
      dfl_lvl=(int)strtol(optarg,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
      if(*sng_cnv_rcd) nco_sng_cnv_err(optarg,"strtol",sng_cnv_rcd);
      break;
    case 'l': /* Local path prefix for files retrieved from remote file system */
      fl_pth_lcl=(char *)strdup(optarg);
      break;
    case 'M': /* Packing map */
      nco_pck_map_sng=(char *)strdup(optarg);
      nco_pck_map=nco_pck_map_get(nco_pck_map_sng);
      break;
    case 'O': /* Toggle FORCE_OVERWRITE */
      FORCE_OVERWRITE=!FORCE_OVERWRITE;
      break;
    case 'o': /* Name of output file */
      fl_out=(char *)strdup(optarg);
      break;
    case 'P': /* Packing policy */
      nco_pck_plc_sng=(char *)strdup(optarg);
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
    case 'U': /* Unpacking switch */
      nco_pck_plc_sng=(char *)strdup("upk");
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
    case '?': /* Print proper usage */
      (void)nco_usg_prn();
      nco_exit(EXIT_SUCCESS);
      break;
    case '-': /* Long options are not allowed */
      (void)fprintf(stderr,"%s: ERROR Long options are not available in this build. Use single letter options instead.\n",prg_nm_get());
      nco_exit(EXIT_FAILURE);
      break;
    default: /* Print proper usage */
      (void)fprintf(stdout,"%s ERROR in command-line syntax/options. Please reformulate command accordingly.\n",prg_nm_get());
      (void)nco_usg_prn();
      nco_exit(EXIT_FAILURE);
      break;
    } /* end switch */
    if(opt_crr) opt_crr=(char *)nco_free(opt_crr);
  } /* end while loop */

  /* Set re-order flag */
  if(dmn_rdr_nbr > 0 ) IS_REORDER=True; 

  /* No re-order dimensions specified implies packing request */
  if(dmn_rdr_nbr == 0){
    if(nco_pck_plc == nco_pck_plc_nil) nco_pck_plc=nco_pck_plc_get(nco_pck_plc_sng);
    if(dbg_lvl >= nco_dbg_scl) (void)fprintf(stderr,"%s: DEBUG Packing map is %s and packing policy is %s\n",prg_nm_get(),nco_pck_map_sng_get(nco_pck_map),nco_pck_plc_sng_get(nco_pck_plc));
  }

  /* From this point forward, assume ncpdq operator packs or re-orders, not both */
  if(dmn_rdr_nbr > 0 && nco_pck_plc != nco_pck_plc_nil){
    (void)fprintf(fp_stdout,"%s: ERROR %s does not support simultaneous dimension re-ordering  (-a switch) and packing (-P switch).\nHINT: Invoke %s twice, once to re-order (with -a), and once to pack (with -P).\n",prg_nm,prg_nm,prg_nm);
    nco_exit(EXIT_FAILURE);
  } 

  /* Create reversed dimension list */
  if(dmn_rdr_nbr > 0 ){
    /* Create reversed dimension list */
    dmn_rvr_rdr=(nco_bool *)nco_malloc(dmn_rdr_nbr*sizeof(nco_bool));
    for(idx_rdr=0;idx_rdr<dmn_rdr_nbr;idx_rdr++){
      if(dmn_rdr_lst_in[idx_rdr][0] == '-'){
        dmn_rvr_rdr[idx_rdr]=True;
        /* Copy string to new memory one past negative sign to avoid losing byte */
        optarg_lcl=dmn_rdr_lst_in[idx_rdr];
        dmn_rdr_lst_in[idx_rdr]=(char *)strdup(optarg_lcl+1);
        optarg_lcl=(char *)nco_free(optarg_lcl);
      }else{
        dmn_rvr_rdr[idx_rdr]=False;
      } /* end else */
    } /* end loop over idx_rdr */
  } /* Create reversed dimension list */


  /* Process positional arguments and fill in filenames */
  fl_lst_in=nco_fl_lst_mk(argv,argc,optind,&fl_nbr,&fl_out,&FL_LST_IN_FROM_STDIN);

  /* Make uniform list of user-specified chunksizes */
  if(cnk_nbr > 0) cnk=nco_cnk_prs(cnk_nbr,cnk_arg);

  /* Make uniform list of user-specified dimension limits */
  lmt=nco_lmt_prs(lmt_nbr,lmt_arg);

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

  /* Get file format */
  (void)nco_inq_format(in_id,&fl_in_fmt);

#ifndef USE_TRV_API

  /* Parse auxiliary coordinates */
  if(aux_nbr > 0){
    int aux_idx_nbr;
    aux=nco_aux_evl(in_id,aux_nbr,aux_arg,&aux_idx_nbr);
    if(aux_idx_nbr > 0){
      lmt=(lmt_sct **)nco_realloc(lmt,(lmt_nbr+aux_idx_nbr)*sizeof(lmt_sct *));
      int lmt_nbr_new=lmt_nbr+aux_idx_nbr;
      int aux_idx=0;
      for(int lmt_idx=lmt_nbr;lmt_idx<lmt_nbr_new;lmt_idx++) lmt[lmt_idx]=aux[aux_idx++];
      lmt_nbr=lmt_nbr_new;
    } /* endif aux */
  } /* endif aux_nbr */

  /* Get number of variables, dimensions, and record dimension ID of input file */
  (void)nco_inq(in_id,&nbr_dmn_fl,&nbr_var_fl,(int *)NULL,&rec_dmn_id_in);

  /* Form initial extraction list which may include extended regular expressions */
  xtr_lst=nco_var_lst_mk(in_id,nbr_var_fl,var_lst_in,EXCLUDE_INPUT_LIST,EXTRACT_ALL_COORDINATES,&xtr_nbr);

  /* Change included variables to excluded variables */
  if(EXCLUDE_INPUT_LIST) xtr_lst=nco_var_lst_xcl(in_id,nbr_var_fl,xtr_lst,&xtr_nbr);

  /* Is this a CCM/CCSM/CF-format history tape? */
  CNV_CCM_CCSM_CF=nco_cnv_ccm_ccsm_cf_inq(in_id);

  /* Add all coordinate variables to extraction list */
  if(EXTRACT_ALL_COORDINATES) xtr_lst=nco_var_lst_crd_add(in_id,nbr_dmn_fl,nbr_var_fl,xtr_lst,&xtr_nbr,CNV_CCM_CCSM_CF);

  /* Extract coordinates associated with extracted variables */
  if(EXTRACT_ASSOCIATED_COORDINATES) xtr_lst=nco_var_lst_crd_ass_add(in_id,xtr_lst,&xtr_nbr,CNV_CCM_CCSM_CF);

  /* Sort extraction list by variable ID for fastest I/O */
  if(xtr_nbr > 1) xtr_lst=nco_lst_srt_nm_id(xtr_lst,xtr_nbr,False);

  /* Find coordinate/dimension values associated with user-specified limits
  NB: nco_lmt_evl() with same nc_id contains OpenMP critical region */
  for(idx=0;idx<lmt_nbr;idx++) (void)nco_lmt_evl(in_id,lmt[idx],0L,FORTRAN_IDX_CNV);

  /* Place all dimensions in lmt_all_lst */
  lmt_all_lst=(lmt_msa_sct **)nco_malloc(nbr_dmn_fl*sizeof(lmt_msa_sct *));
  /* Initialize lmt_msa_sct's */ 
  (void)nco_msa_lmt_all_ntl(in_id,MSA_USR_RDR,lmt_all_lst,nbr_dmn_fl,lmt,lmt_nbr);

  /* Find dimensions associated with variables to be extracted */
  dmn_lst=nco_dmn_lst_ass_var(in_id,xtr_lst,xtr_nbr,&nbr_dmn_xtr);

  nbr_dmn_out=nbr_dmn_xtr;
  /* Fill-in dimension structure for all extracted dimensions */
  dim=(dmn_sct **)nco_malloc(nbr_dmn_xtr*sizeof(dmn_sct *));
  for(idx=0;idx<nbr_dmn_xtr;idx++) dim[idx]=nco_dmn_fll(in_id,dmn_lst[idx].id,dmn_lst[idx].nm);
  /* Dimension list no longer needed */
  dmn_lst=nco_nm_id_lst_free(dmn_lst,nbr_dmn_xtr);

  /* Duplicate input dimension structures for output dimension structures */
  dmn_out=(dmn_sct **)nco_malloc(nbr_dmn_xtr*sizeof(dmn_sct *));
  for(idx=0;idx<nbr_dmn_xtr;idx++){
    dmn_out[idx]=nco_dmn_dpl(dim[idx]);
    (void)nco_dmn_xrf(dim[idx],dmn_out[idx]);
  } /* end loop over idx */

  /* Merge hyperslab limit information into dimension structures */
  if(nbr_dmn_fl > 0) (void)nco_dmn_lmt_all_mrg(dmn_out,nbr_dmn_xtr,lmt_all_lst,nbr_dmn_fl); 

  if(dmn_rdr_nbr > 0 ){
    /* NB: Same logic as in ncwa, perhaps combine into single function, nco_dmn_avg_rdr_prp()? */
    /* Make list of user-specified dimension re-orders */

    /* Create structured list of re-ordering dimension names and IDs */
    dmn_rdr_lst=nco_dmn_lst_mk(in_id,dmn_rdr_lst_in,dmn_rdr_nbr);

    /* Form list of re-ordering dimensions from extracted input dimensions */
    dmn_rdr=(dmn_sct **)nco_malloc(dmn_rdr_nbr*sizeof(dmn_sct *));
    /* Loop over original number of re-order dimensions */
    for(idx_rdr=0;idx_rdr<dmn_rdr_nbr;idx_rdr++){
      for(idx=0;idx<nbr_dmn_xtr;idx++){
        if(!strcmp(dmn_rdr_lst[idx_rdr].nm,dim[idx]->nm)) break;
      } /* end loop over idx_rdr */
      if(idx != nbr_dmn_xtr) dmn_rdr[dmn_rdr_nbr_utl++]=dim[idx]; else if(dbg_lvl >= nco_dbg_std) (void)fprintf(stderr,"%s: WARNING re-ordering dimension \"%s\" is not contained in any variable in extraction list\n",prg_nm,dmn_rdr_lst[idx_rdr].nm);
    } /* end loop over idx_rdr */
    dmn_rdr_nbr=dmn_rdr_nbr_utl;
    /* Collapse extra dimension structure space to prevent accidentally using it */
    dmn_rdr=(dmn_sct **)nco_realloc(dmn_rdr,dmn_rdr_nbr*sizeof(dmn_sct *));
    /* Dimension list in name-ID format is no longer needed */
    dmn_rdr_lst=nco_nm_id_lst_free(dmn_rdr_lst,dmn_rdr_nbr);

    /* Make sure re-ordering dimensions are specified no more than once */
    for(idx=0;idx<dmn_rdr_nbr;idx++){
      for(idx_rdr=0;idx_rdr<dmn_rdr_nbr;idx_rdr++){
        if(idx_rdr != idx){
          if(dmn_rdr[idx]->id == dmn_rdr[idx_rdr]->id){
            (void)fprintf(fp_stdout,"%s: ERROR %s specified more than once in reducing list\n",prg_nm,dmn_rdr[idx]->nm);
            nco_exit(EXIT_FAILURE);
          } /* end if */
        } /* end if */
      } /* end loop over idx_rdr */
    } /* end loop over idx */

    /* 20121009: fxm users should be allowed to sloppily specify more re-order than extracted dimensions */
    if(dmn_rdr_nbr > nbr_dmn_xtr){
      (void)fprintf(fp_stdout,"%s: ERROR More re-ordering dimensions than extracted dimensions\n",prg_nm);
      nco_exit(EXIT_FAILURE);
    } /* end if */

  } /* dmn_rdr_nbr <= 0 */

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

  /* Refresh var_out with dim_out data */
  (void)nco_var_dmn_refresh(var_out,xtr_nbr);

  /* Divide variable lists into lists of fixed variables and variables to be processed */
  (void)nco_var_lst_dvd(var,var_out,xtr_nbr,CNV_CCM_CCSM_CF,True,nco_pck_map,nco_pck_plc,dmn_rdr,dmn_rdr_nbr,&var_fix,&var_fix_out,&nbr_var_fix,&var_prc,&var_prc_out,&nbr_var_prc);

  /* We now have final list of variables to extract. Phew. */
  if(dbg_lvl >= nco_dbg_var){
    for(idx=0;idx<xtr_nbr;idx++) (void)fprintf(stderr,"var[%d]->nm = %s, ->id=[%d]\n",idx,var[idx]->nm,var[idx]->id);
    for(idx=0;idx<nbr_var_fix;idx++) (void)fprintf(stderr,"var_fix[%d]->nm = %s, ->id=[%d]\n",idx,var_fix[idx]->nm,var_fix[idx]->id);
    for(idx=0;idx<nbr_var_prc;idx++) (void)fprintf(stderr,"var_prc[%d]->nm = %s, ->id=[%d]\n",idx,var_prc[idx]->nm,var_prc[idx]->id);
  } /* end if */

  /* Make output and input files consanguinous */
  if(fl_out_fmt == NCO_FORMAT_UNDEFINED) fl_out_fmt=fl_in_fmt;

  /* Verify output file format supports requested actions */
  (void)nco_fl_fmt_vet(fl_out_fmt,cnk_nbr,dfl_lvl);

  /* Open output file */
  fl_out_tmp=nco_fl_out_open(fl_out,FORCE_APPEND,FORCE_OVERWRITE,fl_out_fmt,&bfr_sz_hnt,RAM_CREATE,RAM_OPEN,WRT_TMP_FL,&out_id);
  if(dbg_lvl >= nco_dbg_sbr) (void)fprintf(stderr,"Input, output file IDs = %d, %d\n",in_id,out_id);

  /* Copy global attributes */
  (void)nco_att_cpy(in_id,out_id,NC_GLOBAL,NC_GLOBAL,(nco_bool)True);

  /* Catenate time-stamped command line to "history" global attribute */
  if(HISTORY_APPEND) (void)nco_hst_att_cat(out_id,cmd_ln);

  if(thr_nbr > 0 && HISTORY_APPEND) (void)nco_thr_att_cat(out_id,thr_nbr);

  /* If re-ordering, then in files with record dimension... */
  if(dmn_rdr_nbr > 0 && rec_dmn_id_in != NCO_REC_DMN_UNDEFINED){
    /* ...which, if any, output dimension structure currently holds record dimension? */
    for(dmn_out_idx=0;dmn_out_idx<nbr_dmn_out;dmn_out_idx++)
      if(dmn_out[dmn_out_idx]->is_rec_dmn){
        break;
      }
      if(dmn_out_idx != nbr_dmn_out){
        dmn_out_idx_rec_in=dmn_out_idx;
        /* Initialize output record dimension to input record dimension */
        rec_dmn_nm_in=rec_dmn_nm_out=dmn_out[dmn_out_idx_rec_in]->nm;
      }else{
        dmn_out_idx_rec_in=NCO_REC_DMN_UNDEFINED;
      } /* end else */
  } /* end if file contains record dimension */

  /* If re-ordering, determine and set new dimensionality in metadata of each re-ordered variable */
  if(dmn_rdr_nbr > 0){
    dmn_idx_out_in=(int **)nco_malloc(nbr_var_prc*sizeof(int *));
    dmn_rvr_in=(nco_bool **)nco_malloc(nbr_var_prc*sizeof(nco_bool *));
    for(idx=0;idx<nbr_var_prc;idx++){
      dmn_idx_out_in[idx]=(int *)nco_malloc(var_prc[idx]->nbr_dim*sizeof(int));
      dmn_rvr_in[idx]=(nco_bool *)nco_malloc(var_prc[idx]->nbr_dim*sizeof(nco_bool));
      /* nco_var_dmn_rdr_mtd() does re-order heavy lifting */
      rec_dmn_nm_out_crr=nco_var_dmn_rdr_mtd(var_prc[idx],var_prc_out[idx],dmn_rdr,dmn_rdr_nbr,dmn_idx_out_in[idx],dmn_rvr_rdr,dmn_rvr_in[idx]);
      /* If record dimension required by current variable re-order...
      ...and variable is multi-dimensional (one dimensional arrays cannot request record dimension changes)... */
      if(rec_dmn_nm_out_crr && var_prc_out[idx]->nbr_dim > 1){
        /* ...differs from input and current output record dimension(s)... */
        if(strcmp(rec_dmn_nm_out_crr,rec_dmn_nm_in) && strcmp(rec_dmn_nm_out_crr,rec_dmn_nm_out)){
          /* ...and current output record dimension already differs from input record dimension... */
          if(REDEFINED_RECORD_DIMENSION){
            /* ...then requested re-order requires multiple record dimensions... */
            if(dbg_lvl >= nco_dbg_std) (void)fprintf(fp_stdout,"%s: WARNING Re-order requests multiple record dimensions\n. Only first request will be honored (netCDF3 allows only one record dimension). Record dimensions involved [original,first change request (honored),latest change request (made by variable %s)]=[%s,%s,%s]\n",prg_nm,var_prc[idx]->nm,rec_dmn_nm_in,rec_dmn_nm_out,rec_dmn_nm_out_crr);
            break;
          }else{ /* !REDEFINED_RECORD_DIMENSION */
            /* ...otherwise, update output record dimension name... */
            rec_dmn_nm_out=rec_dmn_nm_out_crr;
            /* ...and set new and un-set old record dimensions... */
            var_prc_out[idx]->dim[0]->is_rec_dmn=True;
            dmn_out[dmn_out_idx_rec_in]->is_rec_dmn=False;
            /* ...and set flag that record dimension has been re-defined... */
            REDEFINED_RECORD_DIMENSION=True;
          } /* !REDEFINED_RECORD_DIMENSION */
        } /* endif new and old record dimensions differ */
      } /* endif current variable is record variable */
    } /* end loop over var_prc */
  } /* endif dmn_rdr_nbr > 0 */

  /* NB: Much of following logic is required by netCDF3 constraint that only
  one record variable is allowed per file. netCDF4 relaxes this constraint.
  Hence making following logic prettier or funcionalizing is not high priority.
  Logic may need to be simplified/re-written once netCDF4 is released. */
  if(REDEFINED_RECORD_DIMENSION){
    if(dbg_lvl >= nco_dbg_std) (void)fprintf(fp_stdout,"%s: INFO Requested re-order will change record dimension from %s to %s. netCDF3 allows only one record dimension. Hence %s will make %s record (i.e., least rapidly varying) dimension in all variables that contain it.\n",prg_nm,rec_dmn_nm_in,rec_dmn_nm_out,prg_nm,rec_dmn_nm_out);
    /* Changing record dimension may invalidate is_rec_var flag
    Updating is_rec_var flag to correct value, even if value is ignored,
    helps keep user appraised of unexpected dimension re-orders.
    is_rec_var may change both for "fixed" and "processed" variables
    When is_rec_var changes for processed variables, may also need to change
    ancillary information and to check for duplicate dimensions.
    Ancillary information (dmn_idx_out_in) is available only for var_prc!
    Hence must update is_rec_var flag for var_fix and var_prc separately */

    /*  Update is_rec_var flag for var_fix */
    for(idx=0;idx<nbr_var_fix;idx++){
      /* Search all dimensions in variable for new record dimension */
      for(dmn_out_idx=0;dmn_out_idx<var_fix[idx]->nbr_dim;dmn_out_idx++)
        if(!strcmp(var_fix[idx]->dim[dmn_out_idx]->nm,rec_dmn_nm_out)){
          break;
        }
        /* ...Will variable be record variable in output file?... */
        if(dmn_out_idx == var_fix[idx]->nbr_dim){
          /* ...No. Variable will be non-record---does this change its status?... */
          if(dbg_lvl >= nco_dbg_var) if(var_fix[idx]->is_rec_var == True) (void)fprintf(fp_stdout,"%s: INFO Requested re-order will change variable %s from record to non-record variable\n",prg_nm,var_fix[idx]->nm);
          /* Assign record flag dictated by re-order */
          var_fix[idx]->is_rec_var=False; 
        }else{ /* ...otherwise variable will be record variable... */
          /* ...Yes. Variable will be record... */
          /* ...Will becoming record variable change its status?... */
          if(var_fix[idx]->is_rec_var == False){
            if(dbg_lvl >= nco_dbg_var) (void)fprintf(fp_stdout,"%s: INFO Requested re-order will change variable %s from non-record to record variable\n",prg_nm,var_fix[idx]->nm);
            /* Change record flag to status dictated by re-order */
            var_fix[idx]->is_rec_var=True;
          } /* endif status changing from non-record to record */
        } /* endif variable will be record variable */
    } /* end loop over var_fix */

    /* Update is_rec_var flag for var_prc */
    for(idx=0;idx<nbr_var_prc;idx++){
      /* Search all dimensions in variable for new record dimension */
      for(dmn_out_idx=0;dmn_out_idx<var_prc_out[idx]->nbr_dim;dmn_out_idx++)
        if(!strcmp(var_prc_out[idx]->dim[dmn_out_idx]->nm,rec_dmn_nm_out)){
          break;
        }
        /* ...Will variable be record variable in output file?... */
        if(dmn_out_idx == var_prc_out[idx]->nbr_dim){
          /* ...No. Variable will be non-record---does this change its status?... */
          if(dbg_lvl >= nco_dbg_var) if(var_prc_out[idx]->is_rec_var == True) (void)fprintf(fp_stdout,"%s: INFO Requested re-order will change variable %s from record to non-record variable\n",prg_nm,var_prc_out[idx]->nm);
          /* Assign record flag dictated by re-order */
          var_prc_out[idx]->is_rec_var=False; 
        }else{ /* ...otherwise variable will be record variable... */
          /* ...Yes. Variable will be record... */
          /* ...must ensure new record dimension is not duplicate dimension... */
          if(var_prc_out[idx]->has_dpl_dmn){
            int dmn_dpl_idx;
            for(dmn_dpl_idx=1;dmn_dpl_idx<var_prc_out[idx]->nbr_dim;dmn_dpl_idx++){ /* NB: loop starts from 1 */
              if(var_prc_out[idx]->dmn_id[0] == var_prc_out[idx]->dmn_id[dmn_dpl_idx]){
                (void)fprintf(stdout,"%s: ERROR Requested re-order turns duplicate non-record dimension %s in variable %s into output record dimension. netCDF does not support duplicate record dimensions in a single variable.\n%s: HINT: Exclude variable %s from extraction list with \"-x -v %s\".\n",prg_nm_get(),rec_dmn_nm_out,var_prc_out[idx]->nm,prg_nm_get(),var_prc_out[idx]->nm,var_prc_out[idx]->nm);
                nco_exit(EXIT_FAILURE);
              } /* endif err */
            } /* end loop over dmn_out */
          } /* endif has_dpl_dmn */
          /* ...Will becoming record variable change its status?... */
          if(var_prc_out[idx]->is_rec_var == False){
            if(dbg_lvl >= nco_dbg_var) (void)fprintf(fp_stdout,"%s: INFO Requested re-order will change variable %s from non-record to record variable\n",prg_nm,var_prc_out[idx]->nm);
            /* Change record flag to status dictated by re-order */
            var_prc_out[idx]->is_rec_var=True;
            /* ...Swap dimension information for multi-dimensional variables... */
            if(var_prc_out[idx]->nbr_dim > 1){
              /* Swap dimension information when turning multi-dimensional 
              non-record variable into record variable. 
              Single dimensional non-record variables that turn into 
              record variables already have correct dimension information */
              dmn_sct *dmn_swp; /* [sct] Dimension structure for swapping */
              int dmn_idx_rec_in; /* [idx] Record dimension index in input variable */
              int dmn_idx_rec_out; /* [idx] Record dimension index in output variable */
              int dmn_idx_swp; /* [idx] Dimension index for swapping */
              /* If necessary, swap new record dimension to first position */
              /* Label indices with standard names */
              dmn_idx_rec_in=dmn_out_idx;
              dmn_idx_rec_out=0;
              /* Swap indices in map */
              dmn_idx_swp=dmn_idx_out_in[idx][dmn_idx_rec_out];
              dmn_idx_out_in[idx][dmn_idx_rec_out]=dmn_idx_rec_in;
              dmn_idx_out_in[idx][dmn_idx_rec_in]=dmn_idx_swp;
              /* Swap dimensions in list */
              dmn_swp=var_prc_out[idx]->dim[dmn_idx_rec_out];
              var_prc_out[idx]->dim[dmn_idx_rec_out]=var_prc_out[idx]->dim[dmn_idx_rec_in];
              var_prc_out[idx]->dim[dmn_idx_rec_in]=dmn_swp;
              /* NB: Change dmn_id,cnt,srt,end,srd together to minimize chances of forgetting one */
              /* Correct output variable structure copy of output record dimension information */
              var_prc_out[idx]->dmn_id[dmn_idx_rec_out]=var_prc_out[idx]->dim[dmn_idx_rec_out]->id;
              var_prc_out[idx]->cnt[dmn_idx_rec_out]=var_prc_out[idx]->dim[dmn_idx_rec_out]->cnt;
              var_prc_out[idx]->srt[dmn_idx_rec_out]=var_prc_out[idx]->dim[dmn_idx_rec_out]->srt;
              var_prc_out[idx]->end[dmn_idx_rec_out]=var_prc_out[idx]->dim[dmn_idx_rec_out]->end;
              var_prc_out[idx]->srd[dmn_idx_rec_out]=var_prc_out[idx]->dim[dmn_idx_rec_out]->srd;
              /* Correct output variable structure copy of input record dimension information */
              var_prc_out[idx]->dmn_id[dmn_idx_rec_in]=var_prc_out[idx]->dim[dmn_idx_rec_in]->id;
              var_prc_out[idx]->cnt[dmn_idx_rec_in]=var_prc_out[idx]->dim[dmn_idx_rec_in]->cnt;
              var_prc_out[idx]->srt[dmn_idx_rec_in]=var_prc_out[idx]->dim[dmn_idx_rec_in]->srt;
              var_prc_out[idx]->end[dmn_idx_rec_in]=var_prc_out[idx]->dim[dmn_idx_rec_in]->end;
              var_prc_out[idx]->srd[dmn_idx_rec_in]=var_prc_out[idx]->dim[dmn_idx_rec_in]->srd;
            } /* endif multi-dimensional */
          } /* endif status changing from non-record to record */
        } /* endif variable will be record variable */
    } /* end loop over var_prc */
  } /* !REDEFINED_RECORD_DIMENSION */

  /* Once new record dimension, if any, is known, define dimensions in output file */
  (void)nco_dmn_dfn(fl_out,out_id,dmn_out,nbr_dmn_out);

  /* Alter metadata for variables that will be packed */
  if(nco_pck_plc != nco_pck_plc_nil){
    if(nco_pck_plc != nco_pck_plc_upk){
      /* Allocate attribute list container for maximum number of entries */
      aed_lst_add_fst=(aed_sct *)nco_malloc(nbr_var_prc*sizeof(aed_sct));
      aed_lst_scl_fct=(aed_sct *)nco_malloc(nbr_var_prc*sizeof(aed_sct));
    } /* endif packing */
    for(idx=0;idx<nbr_var_prc;idx++){
      nco_pck_mtd(var_prc[idx],var_prc_out[idx],nco_pck_map,nco_pck_plc);
      if(nco_pck_plc != nco_pck_plc_upk){
        /* Use same copy of attribute name for all edits */
        aed_lst_add_fst[idx].att_nm=add_fst_sng;
        aed_lst_scl_fct[idx].att_nm=scl_fct_sng;
      } /* endif packing */
    } /* end loop over var_prc */
  } /* nco_pck_plc == nco_pck_plc_nil */

  /* Define variables in output file, copy their attributes */
  (void)nco_var_dfn(in_id,fl_out,out_id,var_out,xtr_nbr,(dmn_sct **)NULL,(int)0,nco_pck_map,nco_pck_plc,dfl_lvl);

  /* Set chunksize parameters */
  if(fl_out_fmt == NC_FORMAT_NETCDF4 || fl_out_fmt == NC_FORMAT_NETCDF4_CLASSIC) (void)nco_cnk_sz_set(out_id,lmt_all_lst,nbr_dmn_fl,&cnk_map,&cnk_plc,cnk_sz_scl,cnk,cnk_nbr);

  /* Turn off default filling behavior to enhance efficiency */
  nco_set_fill(out_id,NC_NOFILL,&fll_md_old);

  /* Take output file out of define mode */
  if(hdr_pad == 0UL){
    (void)nco_enddef(out_id);
  }else{
    (void)nco__enddef(out_id,hdr_pad);
    if(dbg_lvl >= nco_dbg_scl) (void)fprintf(stderr,"%s: INFO Padding header with %lu extra bytes\n",prg_nm_get(),(unsigned long)hdr_pad);
  } /* hdr_pad */

  /* Assign zero to start and unity to stride vectors in output variables */
  (void)nco_var_srd_srt_set(var_out,xtr_nbr);

  /* Copy variable data for non-processed variables */
  (void)nco_msa_var_val_cpy(in_id,out_id,var_fix,nbr_var_fix,lmt_all_lst,nbr_dmn_fl);

  /* Close first input netCDF file */
  nco_close(in_id);

  /* Refresh var_prc with dim_out data */
  for(idx=0;idx<nbr_var_prc;idx++){
    long sz;
    long sz_rec;
    sz=1;
    sz_rec=1;
    var_sct *var_tmp;
    (void)nco_xrf_dmn(var_prc[idx]);
    var_tmp=var_prc[idx];

    for(jdx=0;jdx<var_tmp->nbr_dim;jdx++){
      var_tmp->srt[jdx]=var_tmp->dim[jdx]->srt; 
      var_tmp->end[jdx]=var_tmp->dim[jdx]->end;
      var_tmp->cnt[jdx]=var_tmp->dim[jdx]->cnt;
      var_tmp->srd[jdx]=var_tmp->dim[jdx]->srd;
      sz*=var_tmp->dim[jdx]->cnt;
      if(jdx >0) sz_rec*=var_tmp->dim[jdx]->cnt;
    } /* end loop over jdx */
    var_tmp->sz=sz; 
    var_tmp->sz_rec=sz_rec;
  } /* end loop over idx */

  /* Loop over input files (not currently used, fl_nbr == 1) */
  for(fl_idx=0;fl_idx<fl_nbr;fl_idx++){
    /* Parse filename */
    if(fl_idx != 0) fl_in=nco_fl_nm_prs(fl_in,fl_idx,&fl_nbr,fl_lst_in,abb_arg_nbr,fl_lst_abb,fl_pth);
    if(dbg_lvl >= nco_dbg_fl) (void)fprintf(stderr,"%s: INFO Input file %d is %s",prg_nm_get(),fl_idx,fl_in);
    /* Make sure file is on local system and is readable or die trying */
    if(fl_idx != 0) fl_in=nco_fl_mk_lcl(fl_in,fl_pth_lcl,&FL_RTR_RMT_LCN);
    if(dbg_lvl >= nco_dbg_fl && FL_RTR_RMT_LCN) (void)fprintf(stderr,", local file is %s",fl_in);
    if(dbg_lvl >= nco_dbg_fl) (void)fprintf(stderr,"\n");

    /* Open file once per thread to improve caching */
    for(thr_idx=0;thr_idx<thr_nbr;thr_idx++) rcd+=nco_fl_open(fl_in,md_open,&bfr_sz_hnt,in_id_arr+thr_idx);

    /* Timestamp end of metadata setup and disk layout */
    rcd+=nco_ddra((char *)NULL,(char *)NULL,&ddra_info);
    ddra_info.tmr_flg=nco_tmr_rgl;

#ifdef _OPENMP
#pragma omp parallel for default(none) private(idx,in_id) shared(aed_lst_add_fst,aed_lst_scl_fct,dbg_lvl,dmn_idx_out_in,dmn_rdr_nbr,dmn_rvr_in,in_id_arr,nbr_var_prc,nco_pck_map,nco_pck_plc,out_id,prg_nm,rcd,var_prc,var_prc_out,lmt_all_lst,nbr_dmn_fl)
#endif /* !_OPENMP */
    for(idx=0;idx<nbr_var_prc;idx++){ /* Process all variables in current file */
      in_id=in_id_arr[omp_get_thread_num()];
      /* fxm TODO nco638 temporary fix? or leave permanantly? */
      var_prc[idx]->nc_id=in_id; 
      if(dbg_lvl >= nco_dbg_var) rcd+=nco_var_prc_crr_prn(idx,var_prc[idx]->nm);
      if(dbg_lvl >= nco_dbg_var) (void)fflush(fp_stderr);

      /* Retrieve variable from disk into memory */
      /* NB: nco_var_get() with same nc_id contains OpenMP critical region */
      (void)nco_msa_var_get(in_id,var_prc[idx],lmt_all_lst,nbr_dmn_fl);
      if(dmn_rdr_nbr > 0){
        if((var_prc_out[idx]->val.vp=(void *)nco_malloc_flg(var_prc_out[idx]->sz*nco_typ_lng(var_prc_out[idx]->type))) == NULL){
          (void)fprintf(fp_stdout,"%s: ERROR Unable to malloc() %ld*%lu bytes for value buffer for variable %s in main()\n",prg_nm_get(),var_prc_out[idx]->sz,(unsigned long)nco_typ_lng(var_prc_out[idx]->type),var_prc_out[idx]->nm);
          nco_exit(EXIT_FAILURE); 
        } /* endif err */
        /* Change dimensionionality of values */
        rcd=nco_var_dmn_rdr_val(var_prc[idx],var_prc_out[idx],dmn_idx_out_in[idx],dmn_rvr_in[idx]);
        /* Re-ordering required two value buffers, time to free input buffer */
        var_prc[idx]->val.vp=nco_free(var_prc[idx]->val.vp);
        /* Free current dimension correspondence */
        dmn_idx_out_in[idx]=(int *)nco_free(dmn_idx_out_in[idx]);
        dmn_rvr_in[idx]=(nco_bool *)nco_free(dmn_rvr_in[idx]);
      } /* endif dmn_rdr_nbr > 0 */

      if(nco_pck_plc != nco_pck_plc_nil){
        /* Copy input variable buffer to processed variable buffer */
        /* fxm: this is dangerous and leads to double free()'ing variable buffer */
        var_prc_out[idx]->val=var_prc[idx]->val;
        /* (Un-)Pack variable according to packing specification */
        nco_pck_val(var_prc[idx],var_prc_out[idx],nco_pck_map,nco_pck_plc,aed_lst_add_fst+idx,aed_lst_scl_fct+idx);
      } /* endif dmn_rdr_nbr > 0 */

#ifdef _OPENMP
#pragma omp critical
#endif /* _OPENMP */
      { /* begin OpenMP critical */
        /* Copy variable to output file then free value buffer */
        if(var_prc_out[idx]->nbr_dim == 0){
          (void)nco_put_var1(out_id,var_prc_out[idx]->id,var_prc_out[idx]->srt,var_prc_out[idx]->val.vp,var_prc_out[idx]->type);
        }else{ /* end if variable is scalar */
          (void)nco_put_vara(out_id,var_prc_out[idx]->id,var_prc_out[idx]->srt,var_prc_out[idx]->cnt,var_prc_out[idx]->val.vp,var_prc_out[idx]->type);
        } /* end if variable is array */
      } /* end OpenMP critical */
      /* Free current output buffer */
      var_prc_out[idx]->val.vp=nco_free(var_prc_out[idx]->val.vp);

    } /* end (OpenMP parallel for) loop over idx */

    if(dbg_lvl >= nco_dbg_fl) (void)fprintf(fp_stderr,"\n");

    /* Write/overwrite packing attributes for newly packed and re-packed variables 
    Logic here should nearly mimic logic in nco_var_dfn() */
    if(nco_pck_plc != nco_pck_plc_nil && nco_pck_plc != nco_pck_plc_upk){
      /* ...put file in define mode to allow metadata writing... */
      (void)nco_redef(out_id);
      /* ...loop through all variables that may have been packed... */
      for(idx=0;idx<nbr_var_prc;idx++){
        /* nco_var_dfn() pre-defined dummy packing attributes in output file 
        only for input variables considered "packable" */
        if(nco_pck_plc_typ_get(nco_pck_map,var_prc[idx]->typ_upk,(nc_type *)NULL)){
          /* Verify input variable was newly packed by this operator
          Writing pre-existing (non-re-packed) attributes here would fail because
          nco_pck_dsk_inq() never fills in var->scl_fct.vp and var->add_fst.vp
          Logic is same as in nco_var_dfn() (except var_prc[] instead of var[])
          If operator newly packed this particular variable... */
          if(
            /* ...either because operator newly packs all variables... */
            (nco_pck_plc == nco_pck_plc_all_new_att) ||
            /* ...or because operator newly packs un-packed variables like this one... */
            (nco_pck_plc == nco_pck_plc_all_xst_att && !var_prc[idx]->pck_ram) ||
            /* ...or because operator re-packs packed variables like this one... */
            (nco_pck_plc == nco_pck_plc_xst_new_att && var_prc[idx]->pck_ram)
            ){
              /* Replace dummy packing attributes with final values, or delete them */
              if(dbg_lvl >= nco_dbg_io) (void)fprintf(stderr,"%s: main() replacing dummy packing attribute values for variable %s\n",prg_nm,var_prc[idx]->nm);
              (void)nco_aed_prc(out_id,aed_lst_add_fst[idx].id,aed_lst_add_fst[idx]);
              (void)nco_aed_prc(out_id,aed_lst_scl_fct[idx].id,aed_lst_scl_fct[idx]);
          } /* endif variable is newly packed by this operator */
        } /* !nco_pck_plc_alw */
      } /* end loop over var_prc */
      (void)nco_enddef(out_id);
    } /* nco_pck_plc == nco_pck_plc_nil || nco_pck_plc == nco_pck_plc_upk */

    /* Close input netCDF file */
    for(thr_idx=0;thr_idx<thr_nbr;thr_idx++) nco_close(in_id_arr[thr_idx]);

    /* Remove local copy of file */
    if(FL_RTR_RMT_LCN && RM_RMT_FL_PST_PRC) (void)nco_fl_rm(fl_in);

  } /* end loop over fl_idx */

  /* Close output file and move it from temporary to permanent location */
  (void)nco_fl_out_cls(fl_out,fl_out_tmp,out_id);

#endif /* USE_TRV_API */


#ifdef USE_TRV_API

  /* Construct GTT, Group Traversal Table (groups,variables,dimensions, limits) */
  (void)nco_bld_trv_tbl(in_id,trv_pth,MSA_USR_RDR,lmt_nbr,lmt,FORTRAN_IDX_CNV,aux_nbr,aux_arg,trv_tbl);

  /* Get number of variables, dimensions, and global attributes in file, file format */
  (void)trv_tbl_inq((int *)NULL,(int *)NULL,(int *)NULL,&nbr_dmn_fl,(int *)NULL,(int *)NULL,(int *)NULL,(int *)NULL,&nbr_var_fl,trv_tbl);

  /* Check -v and -g input names and create extraction list */
  (void)nco_xtr_mk(grp_lst_in,grp_lst_in_nbr,var_lst_in,xtr_nbr,EXTRACT_ALL_COORDINATES,GRP_VAR_UNN,trv_tbl);

  /* Change included variables to excluded variables */
  if(EXCLUDE_INPUT_LIST) (void)nco_xtr_xcl(trv_tbl);

  /* Add all coordinate variables to extraction list */
  if(EXTRACT_ALL_COORDINATES) (void)nco_xtr_crd_add(trv_tbl);

  /* Extract coordinates associated with extracted variables */
  if(EXTRACT_ASSOCIATED_COORDINATES) (void)nco_xtr_crd_ass_add(in_id,trv_tbl);

  /* Is this a CCM/CCSM/CF-format history tape? */
  CNV_CCM_CCSM_CF=nco_cnv_ccm_ccsm_cf_inq(in_id);
  if(CNV_CCM_CCSM_CF && EXTRACT_ASSOCIATED_COORDINATES){
    /* Implement CF "coordinates" and "bounds" conventions */
    (void)nco_xtr_cf_add(in_id,"coordinates",trv_tbl);
    (void)nco_xtr_cf_add(in_id,"bounds",trv_tbl);
  } /* CNV_CCM_CCSM_CF */


  /* Allocate array of dimensions associated with variables to be extracted with maximum possible size */
  dim=(dmn_sct **)nco_malloc(nbr_dmn_fl*sizeof(dmn_sct *));

  /* Find dimensions associated with variables to be extracted */
  (void)nco_dmn_lst_ass_var_trv(in_id,trv_tbl,&nbr_dmn_xtr,&dim);

  /* Duplicate input dimension structures for output dimension structures */
  dmn_out=(dmn_sct **)nco_malloc(nbr_dmn_xtr*sizeof(dmn_sct *));
  for(idx=0;idx<nbr_dmn_xtr;idx++){
    dmn_out[idx]=nco_dmn_dpl(dim[idx]);
    (void)nco_dmn_xrf(dim[idx],dmn_out[idx]);
  } 

  /* If re-ordering */
  if(IS_REORDER){

    /* Create structured list of re-ordering dimension names and IDs */
    dmn_rdr_lst=nco_dmn_lst_mk(in_id,dmn_rdr_lst_in,dmn_rdr_nbr);

    /* Form list of re-ordering dimensions from extracted input dimensions */
    dmn_rdr=(dmn_sct **)nco_malloc(dmn_rdr_nbr*sizeof(dmn_sct *));
    /* Loop over original number of re-order dimensions */
    for(idx_rdr=0;idx_rdr<dmn_rdr_nbr;idx_rdr++){
      for(idx=0;idx<nbr_dmn_xtr;idx++){
        if(!strcmp(dmn_rdr_lst[idx_rdr].nm,dim[idx]->nm)){
          break;
        }
      } /* end loop over idx_rdr */
      if(idx != nbr_dmn_xtr){
        dmn_rdr[dmn_rdr_nbr_utl++]=dim[idx];
      }else {
        if(dbg_lvl >= nco_dbg_std) (void)fprintf(stderr,"%s: WARNING re-ordering dimension \"%s\" is not contained in any variable in extraction list\n",prg_nm,dmn_rdr_lst[idx_rdr].nm);
      }
    } /* end loop over idx_rdr */
    dmn_rdr_nbr=dmn_rdr_nbr_utl;

    /* Collapse extra dimension structure space to prevent accidentally using it */
    dmn_rdr=(dmn_sct **)nco_realloc(dmn_rdr,dmn_rdr_nbr*sizeof(dmn_sct *));

    /* Dimension list in name-ID format is no longer needed */
    dmn_rdr_lst=nco_nm_id_lst_free(dmn_rdr_lst,dmn_rdr_nbr);

    /* Make sure re-ordering dimensions are specified no more than once */
    for(idx=0;idx<dmn_rdr_nbr;idx++){
      for(idx_rdr=0;idx_rdr<dmn_rdr_nbr;idx_rdr++){
        if(idx_rdr != idx){
          if(dmn_rdr[idx]->id == dmn_rdr[idx_rdr]->id){
            (void)fprintf(fp_stdout,"%s: ERROR %s specified more than once in reducing list\n",prg_nm,dmn_rdr[idx]->nm);
            nco_exit(EXIT_FAILURE);
          } /* end if */
        } /* end if */
      } /* end loop over idx_rdr */
    } /* end loop over idx */

    /* 20121009: fxm users should be allowed to sloppily specify more re-order than extracted dimensions */
    if(dmn_rdr_nbr > nbr_dmn_xtr){
      (void)fprintf(fp_stdout,"%s: ERROR More re-ordering dimensions than extracted dimensions\n",prg_nm);
      nco_exit(EXIT_FAILURE);
    } /* end if */
  } /* If re-ordering */

  /* Fill-in variable structure list for all extracted variables. NOTE: Using GTT version */
  var=nco_fll_var_trv(in_id,&xtr_nbr,trv_tbl);

  /* Duplicate to output array */
  var_out=(var_sct **)nco_malloc(xtr_nbr*sizeof(var_sct *));
  for(idx=0;idx<xtr_nbr;idx++){
    var_out[idx]=nco_var_dpl(var[idx]);
    (void)nco_xrf_var(var[idx],var_out[idx]);
    (void)nco_xrf_dmn(var_out[idx]);
  }

  /* Refresh var_out with dim_out data */
  (void)nco_var_dmn_refresh(var_out,xtr_nbr);

  /* Divide variable lists into lists of fixed variables and variables to be processed */
  (void)nco_var_lst_dvd(var,var_out,xtr_nbr,CNV_CCM_CCSM_CF,True,nco_pck_map,nco_pck_plc,dmn_rdr,dmn_rdr_nbr,&var_fix,&var_fix_out,&nbr_var_fix,&var_prc,&var_prc_out,&nbr_var_prc);

  /* Store processed and fixed variables info into GTT */
  (void)nco_var_prc_fix_trv(nbr_var_prc,var_prc,nbr_var_fix,var_fix,trv_tbl);

  /* We now have final list of variables to extract. Phew. */

  /* Make output and input files consanguinous */
  if(fl_out_fmt == NCO_FORMAT_UNDEFINED) fl_out_fmt=fl_in_fmt;

  /* Verify output file format supports requested actions */
  (void)nco_fl_fmt_vet(fl_out_fmt,cnk_nbr,dfl_lvl);

  /* Open output file */
  fl_out_tmp=nco_fl_out_open(fl_out,FORCE_APPEND,FORCE_OVERWRITE,fl_out_fmt,&bfr_sz_hnt,RAM_CREATE,RAM_OPEN,WRT_TMP_FL,&out_id);
  if(dbg_lvl >= nco_dbg_sbr) (void)fprintf(stderr,"Input, output file IDs = %d, %d\n",in_id,out_id);

  /* Copy global attributes */
  (void)nco_att_cpy(in_id,out_id,NC_GLOBAL,NC_GLOBAL,(nco_bool)True);

  /* Catenate time-stamped command line to "history" global attribute */
  if(HISTORY_APPEND) (void)nco_hst_att_cat(out_id,cmd_ln);

  if(thr_nbr > 0 && HISTORY_APPEND) (void)nco_thr_att_cat(out_id,thr_nbr);


#ifdef USE_RDR_NETCDF3

  if (fl_in_fmt != NC_FORMAT_NETCDF4) {
    /* Get record dimension ID of input file */
    (void)nco_inq(in_id,(int *)NULL,(int *)NULL,(int *)NULL,&rec_dmn_id_in);
  }

  /* If re-ordering, then in files with record dimension... */
  if(IS_REORDER && rec_dmn_id_in != NCO_REC_DMN_UNDEFINED){
    nbr_dmn_out=nbr_dmn_xtr;
    /* ...which, if any, output dimension structure currently holds record dimension? */
    for(dmn_out_idx=0;dmn_out_idx<nbr_dmn_out;dmn_out_idx++)
      if(dmn_out[dmn_out_idx]->is_rec_dmn){
        break;
      }
      if(dmn_out_idx != nbr_dmn_out){
        dmn_out_idx_rec_in=dmn_out_idx;
        /* Initialize output record dimension to input record dimension */
        rec_dmn_nm_in=rec_dmn_nm_out=dmn_out[dmn_out_idx_rec_in]->nm;
      }else{
        dmn_out_idx_rec_in=NCO_REC_DMN_UNDEFINED;
      } /* end else */
  } /* end if file contains record dimension */


  /* If re-ordering, determine and set new dimensionality in metadata of each re-ordered variable */
  if(IS_REORDER){
    dmn_idx_out_in=(int **)nco_malloc(nbr_var_prc*sizeof(int *));
    dmn_rvr_in=(nco_bool **)nco_malloc(nbr_var_prc*sizeof(nco_bool *));
    for(idx=0;idx<nbr_var_prc;idx++){
      dmn_idx_out_in[idx]=(int *)nco_malloc(var_prc[idx]->nbr_dim*sizeof(int));
      dmn_rvr_in[idx]=(nco_bool *)nco_malloc(var_prc[idx]->nbr_dim*sizeof(nco_bool));
      /* nco_var_dmn_rdr_mtd() does re-order heavy lifting */
      rec_dmn_nm_out_crr=nco_var_dmn_rdr_mtd(var_prc[idx],var_prc_out[idx],dmn_rdr,dmn_rdr_nbr,dmn_idx_out_in[idx],dmn_rvr_rdr,dmn_rvr_in[idx]);
      /* If record dimension required by current variable re-order...
      ...and variable is multi-dimensional (one dimensional arrays cannot request record dimension changes)... */
      if(rec_dmn_nm_out_crr && var_prc_out[idx]->nbr_dim > 1){
        /* ...differs from input and current output record dimension(s)... */
        if(strcmp(rec_dmn_nm_out_crr,rec_dmn_nm_in) && strcmp(rec_dmn_nm_out_crr,rec_dmn_nm_out)){
          /* ...and current output record dimension already differs from input record dimension... */
          if(REDEFINED_RECORD_DIMENSION){
            /* ...then requested re-order requires multiple record dimensions... */
            if(dbg_lvl >= nco_dbg_std) (void)fprintf(fp_stdout,"%s: WARNING Re-order requests multiple record dimensions\n. Only first request will be honored (netCDF3 allows only one record dimension). Record dimensions involved [original,first change request (honored),latest change request (made by variable %s)]=[%s,%s,%s]\n",prg_nm,var_prc[idx]->nm,rec_dmn_nm_in,rec_dmn_nm_out,rec_dmn_nm_out_crr);
            break;
          }else{ /* !REDEFINED_RECORD_DIMENSION */
            /* ...otherwise, update output record dimension name... */
            rec_dmn_nm_out=rec_dmn_nm_out_crr;
            /* ...and set new and un-set old record dimensions... */
            var_prc_out[idx]->dim[0]->is_rec_dmn=True;
            dmn_out[dmn_out_idx_rec_in]->is_rec_dmn=False;
            /* ...and set flag that record dimension has been re-defined... */
            REDEFINED_RECORD_DIMENSION=True;
          } /* !REDEFINED_RECORD_DIMENSION */
        } /* endif new and old record dimensions differ */
      } /* endif current variable is record variable */
    } /* end loop over var_prc */
  } /* endif IS_REORDER */


  /* NB: Much of following logic is required by netCDF3 constraint that only
  one record variable is allowed per file. netCDF4 relaxes this constraint.
  Hence making following logic prettier or funcionalizing is not high priority.
  Logic may need to be simplified/re-written once netCDF4 is released. */
  if(REDEFINED_RECORD_DIMENSION){
    if(dbg_lvl >= nco_dbg_std) (void)fprintf(fp_stdout,"%s: INFO Requested re-order will change record dimension from %s to %s. netCDF3 allows only one record dimension. Hence %s will make %s record (i.e., least rapidly varying) dimension in all variables that contain it.\n",prg_nm,rec_dmn_nm_in,rec_dmn_nm_out,prg_nm,rec_dmn_nm_out);
    /* Changing record dimension may invalidate is_rec_var flag
    Updating is_rec_var flag to correct value, even if value is ignored,
    helps keep user appraised of unexpected dimension re-orders.
    is_rec_var may change both for "fixed" and "processed" variables
    When is_rec_var changes for processed variables, may also need to change
    ancillary information and to check for duplicate dimensions.
    Ancillary information (dmn_idx_out_in) is available only for var_prc!
    Hence must update is_rec_var flag for var_fix and var_prc separately */

    /*  Update is_rec_var flag for var_fix */
    for(idx=0;idx<nbr_var_fix;idx++){
      /* Search all dimensions in variable for new record dimension */
      for(dmn_out_idx=0;dmn_out_idx<var_fix[idx]->nbr_dim;dmn_out_idx++)
        if(!strcmp(var_fix[idx]->dim[dmn_out_idx]->nm,rec_dmn_nm_out)){
          break;
        }
        /* ...Will variable be record variable in output file?... */
        if(dmn_out_idx == var_fix[idx]->nbr_dim){
          /* ...No. Variable will be non-record---does this change its status?... */
          if(dbg_lvl >= nco_dbg_var) if(var_fix[idx]->is_rec_var == True) (void)fprintf(fp_stdout,"%s: INFO Requested re-order will change variable %s from record to non-record variable\n",prg_nm,var_fix[idx]->nm);
          /* Assign record flag dictated by re-order */
          var_fix[idx]->is_rec_var=False; 
        }else{ /* ...otherwise variable will be record variable... */
          /* ...Yes. Variable will be record... */
          /* ...Will becoming record variable change its status?... */
          if(var_fix[idx]->is_rec_var == False){
            if(dbg_lvl >= nco_dbg_var) (void)fprintf(fp_stdout,"%s: INFO Requested re-order will change variable %s from non-record to record variable\n",prg_nm,var_fix[idx]->nm);
            /* Change record flag to status dictated by re-order */
            var_fix[idx]->is_rec_var=True;
          } /* endif status changing from non-record to record */
        } /* endif variable will be record variable */
    } /* end loop over var_fix */

    /* Update is_rec_var flag for var_prc */
    for(idx=0;idx<nbr_var_prc;idx++){
      /* Search all dimensions in variable for new record dimension */
      for(dmn_out_idx=0;dmn_out_idx<var_prc_out[idx]->nbr_dim;dmn_out_idx++)
        if(!strcmp(var_prc_out[idx]->dim[dmn_out_idx]->nm,rec_dmn_nm_out)){
          break;
        }
        /* ...Will variable be record variable in output file?... */
        if(dmn_out_idx == var_prc_out[idx]->nbr_dim){
          /* ...No. Variable will be non-record---does this change its status?... */
          if(dbg_lvl >= nco_dbg_var) if(var_prc_out[idx]->is_rec_var == True) (void)fprintf(fp_stdout,"%s: INFO Requested re-order will change variable %s from record to non-record variable\n",prg_nm,var_prc_out[idx]->nm);
          /* Assign record flag dictated by re-order */
          var_prc_out[idx]->is_rec_var=False; 
        }else{ /* ...otherwise variable will be record variable... */
          /* ...Yes. Variable will be record... */
          /* ...must ensure new record dimension is not duplicate dimension... */
          if(var_prc_out[idx]->has_dpl_dmn){
            int dmn_dpl_idx;
            for(dmn_dpl_idx=1;dmn_dpl_idx<var_prc_out[idx]->nbr_dim;dmn_dpl_idx++){ /* NB: loop starts from 1 */
              if(var_prc_out[idx]->dmn_id[0] == var_prc_out[idx]->dmn_id[dmn_dpl_idx]){
                (void)fprintf(stdout,"%s: ERROR Requested re-order turns duplicate non-record dimension %s in variable %s into output record dimension. netCDF does not support duplicate record dimensions in a single variable.\n%s: HINT: Exclude variable %s from extraction list with \"-x -v %s\".\n",prg_nm_get(),rec_dmn_nm_out,var_prc_out[idx]->nm,prg_nm_get(),var_prc_out[idx]->nm,var_prc_out[idx]->nm);
                nco_exit(EXIT_FAILURE);
              } /* endif err */
            } /* end loop over dmn_out */
          } /* endif has_dpl_dmn */
          /* ...Will becoming record variable change its status?... */
          if(var_prc_out[idx]->is_rec_var == False){
            if(dbg_lvl >= nco_dbg_var) (void)fprintf(fp_stdout,"%s: INFO Requested re-order will change variable %s from non-record to record variable\n",prg_nm,var_prc_out[idx]->nm);
            /* Change record flag to status dictated by re-order */
            var_prc_out[idx]->is_rec_var=True;
            /* ...Swap dimension information for multi-dimensional variables... */
            if(var_prc_out[idx]->nbr_dim > 1){
              /* Swap dimension information when turning multi-dimensional 
              non-record variable into record variable. 
              Single dimensional non-record variables that turn into 
              record variables already have correct dimension information */
              dmn_sct *dmn_swp; /* [sct] Dimension structure for swapping */
              int dmn_idx_rec_in; /* [idx] Record dimension index in input variable */
              int dmn_idx_rec_out; /* [idx] Record dimension index in output variable */
              int dmn_idx_swp; /* [idx] Dimension index for swapping */
              /* If necessary, swap new record dimension to first position */
              /* Label indices with standard names */
              dmn_idx_rec_in=dmn_out_idx;
              dmn_idx_rec_out=0;
              /* Swap indices in map */
              dmn_idx_swp=dmn_idx_out_in[idx][dmn_idx_rec_out];
              dmn_idx_out_in[idx][dmn_idx_rec_out]=dmn_idx_rec_in;
              dmn_idx_out_in[idx][dmn_idx_rec_in]=dmn_idx_swp;
              /* Swap dimensions in list */
              dmn_swp=var_prc_out[idx]->dim[dmn_idx_rec_out];
              var_prc_out[idx]->dim[dmn_idx_rec_out]=var_prc_out[idx]->dim[dmn_idx_rec_in];
              var_prc_out[idx]->dim[dmn_idx_rec_in]=dmn_swp;
              /* NB: Change dmn_id,cnt,srt,end,srd together to minimize chances of forgetting one */
              /* Correct output variable structure copy of output record dimension information */
              var_prc_out[idx]->dmn_id[dmn_idx_rec_out]=var_prc_out[idx]->dim[dmn_idx_rec_out]->id;
              var_prc_out[idx]->cnt[dmn_idx_rec_out]=var_prc_out[idx]->dim[dmn_idx_rec_out]->cnt;
              var_prc_out[idx]->srt[dmn_idx_rec_out]=var_prc_out[idx]->dim[dmn_idx_rec_out]->srt;
              var_prc_out[idx]->end[dmn_idx_rec_out]=var_prc_out[idx]->dim[dmn_idx_rec_out]->end;
              var_prc_out[idx]->srd[dmn_idx_rec_out]=var_prc_out[idx]->dim[dmn_idx_rec_out]->srd;
              /* Correct output variable structure copy of input record dimension information */
              var_prc_out[idx]->dmn_id[dmn_idx_rec_in]=var_prc_out[idx]->dim[dmn_idx_rec_in]->id;
              var_prc_out[idx]->cnt[dmn_idx_rec_in]=var_prc_out[idx]->dim[dmn_idx_rec_in]->cnt;
              var_prc_out[idx]->srt[dmn_idx_rec_in]=var_prc_out[idx]->dim[dmn_idx_rec_in]->srt;
              var_prc_out[idx]->end[dmn_idx_rec_in]=var_prc_out[idx]->dim[dmn_idx_rec_in]->end;
              var_prc_out[idx]->srd[dmn_idx_rec_in]=var_prc_out[idx]->dim[dmn_idx_rec_in]->srd;
            } /* endif multi-dimensional */
          } /* endif status changing from non-record to record */
        } /* endif variable will be record variable */
    } /* end loop over var_prc */
  } /* !REDEFINED_RECORD_DIMENSION */

  /* Transfer dimension re-order structures (index map) into GTT */
  if(IS_REORDER) (void)nco_dmn_rdr_trv(dmn_idx_out_in,nbr_var_prc,var_prc_out,trv_tbl);

#else /* USE_RDR_NETCDF3 */

  /* If re-ordering */
  if(IS_REORDER){

    /* Determine and set new dimensionality in metadata of each re-ordered variable */
    (void)nco_var_dmn_rdr_mtd_trv(trv_tbl,nbr_var_prc,var_prc,var_prc_out,nbr_var_fix,var_fix,dmn_rdr,dmn_rdr_nbr,dmn_rvr_rdr);            

  } /* IS_REORDER */

#endif /* USE_RDR_NETCDF3 */

  /* Alter metadata for variables that will be packed */
  if(nco_pck_plc != nco_pck_plc_nil){
    if(nco_pck_plc != nco_pck_plc_upk){
      /* Allocate attribute list container for maximum number of entries */
      aed_lst_add_fst=(aed_sct *)nco_malloc(nbr_var_prc*sizeof(aed_sct));
      aed_lst_scl_fct=(aed_sct *)nco_malloc(nbr_var_prc*sizeof(aed_sct));
    } /* endif packing */
    for(idx=0;idx<nbr_var_prc;idx++){
      nco_pck_mtd(var_prc[idx],var_prc_out[idx],nco_pck_map,nco_pck_plc);
      if(nco_pck_plc != nco_pck_plc_upk){
        /* Use same copy of attribute name for all edits */
        aed_lst_add_fst[idx].att_nm=add_fst_sng;
        aed_lst_scl_fct[idx].att_nm=scl_fct_sng;
      } /* endif packing */
    } /* end loop over var_prc */

    /* Transfer variable type to table. NOTE: Using processed variables set with the new type. MUST be done befoe definition  */
    (void)nco_var_typ_trv(xtr_nbr,var_prc_out,trv_tbl);    

  } /* nco_pck_plc == nco_pck_plc_nil */

#ifdef USE_RDR_NETCDF3

  /* Define dimensions, extracted groups, variables, and attributes in output file */
  (void)nco_xtr_dfn(in_id,out_id,&cnk_map,&cnk_plc,cnk_sz_scl,cnk,cnk_nbr,dfl_lvl,gpe,True,True,rec_dmn_nm_out,trv_tbl); 

#else

  /* Define dimensions, extracted groups, variables, and attributes in output file. NOTE. record name is NULL */
  (void)nco_xtr_dfn(in_id,out_id,&cnk_map,&cnk_plc,cnk_sz_scl,cnk,cnk_nbr,dfl_lvl,gpe,True,True,(char *)NULL,trv_tbl); 

#endif

  /* Set chunksize parameters */
  if(fl_out_fmt == NC_FORMAT_NETCDF4 || fl_out_fmt == NC_FORMAT_NETCDF4_CLASSIC) (void)nco_cnk_sz_set(out_id,lmt_all_lst,nbr_dmn_fl,&cnk_map,&cnk_plc,cnk_sz_scl,cnk,cnk_nbr);

  /* Turn off default filling behavior to enhance efficiency */
  nco_set_fill(out_id,NC_NOFILL,&fll_md_old);

  /* Take output file out of define mode */
  if(hdr_pad == 0UL){
    (void)nco_enddef(out_id);
  }else{
    (void)nco__enddef(out_id,hdr_pad);
    if(dbg_lvl >= nco_dbg_scl) (void)fprintf(stderr,"%s: INFO Padding header with %lu extra bytes\n",prg_nm_get(),(unsigned long)hdr_pad);
  } /* hdr_pad */

  /* Assign zero to start and unity to stride vectors in output variables */
  (void)nco_var_srd_srt_set(var_out,xtr_nbr);

  /* Copy variable data for non-processed variables */
  (void)nco_cpy_fix_var_trv(in_id,out_id,trv_tbl);  

  /* Close first input netCDF file */
  nco_close(in_id);

  /* Loop over input files (not currently used, fl_nbr == 1) */
  for(fl_idx=0;fl_idx<fl_nbr;fl_idx++){

    /* Parse filename */
    if(fl_idx != 0) fl_in=nco_fl_nm_prs(fl_in,fl_idx,&fl_nbr,fl_lst_in,abb_arg_nbr,fl_lst_abb,fl_pth);
    if(dbg_lvl >= nco_dbg_fl) (void)fprintf(stderr,"%s: INFO Input file %d is %s",prg_nm_get(),fl_idx,fl_in);

    /* Make sure file is on local system and is readable or die trying */
    if(fl_idx != 0) fl_in=nco_fl_mk_lcl(fl_in,fl_pth_lcl,&FL_RTR_RMT_LCN);
    if(dbg_lvl >= nco_dbg_fl && FL_RTR_RMT_LCN) (void)fprintf(stderr,", local file is %s",fl_in);
    if(dbg_lvl >= nco_dbg_fl) (void)fprintf(stderr,"\n");

    /* Open file once per thread to improve caching */
    for(thr_idx=0;thr_idx<thr_nbr;thr_idx++) rcd+=nco_fl_open(fl_in,md_open,&bfr_sz_hnt,in_id_arr+thr_idx);

    /* Timestamp end of metadata setup and disk layout */
    rcd+=nco_ddra((char *)NULL,(char *)NULL,&ddra_info);
    ddra_info.tmr_flg=nco_tmr_rgl;

#ifdef _OPENMP
#pragma omp parallel for default(none) private(idx,in_id) shared(aed_lst_add_fst,aed_lst_scl_fct,dbg_lvl,dmn_idx_out_in,dmn_rdr_nbr,dmn_rvr_in,in_id_arr,nbr_var_prc,nco_pck_map,nco_pck_plc,out_id,prg_nm,rcd,var_prc,var_prc_out,lmt_all_lst,nbr_dmn_fl)
#endif /* !_OPENMP */

    /* Process all variables in current file */
    for(idx=0;idx<nbr_var_prc;idx++){ 

      int grp_id;        /* [ID] Group ID */
      int grp_out_id;    /* [ID] Group ID (output) */
      int var_out_id;    /* [ID] Variable ID (output) */
      trv_sct *var_trv;  /* [sct] Variable GTT object */

      in_id=in_id_arr[omp_get_thread_num()];
      var_prc[idx]->nc_id=in_id; 

      if(dbg_lvl >= nco_dbg_var) rcd+=nco_var_prc_crr_prn(idx,var_prc[idx]->nm);
      if(dbg_lvl >= nco_dbg_var) (void)fflush(fp_stderr);

      /* Obtain variable GTT object using full variable name */
      var_trv=trv_tbl_var_nm_fll(var_prc[idx]->nm_fll,trv_tbl);

      /* Obtain group ID using full group name */
      (void)nco_inq_grp_full_ncid(in_id,var_trv->grp_nm_fll,&grp_id);

      /* Retrieve variable from disk into memory */
      (void)nco_msa_var_get_trv(grp_id,var_prc[idx],var_trv);

      /* If re-ordering */
      if(IS_REORDER){
        if((var_prc_out[idx]->val.vp=(void *)nco_malloc_flg(var_prc_out[idx]->sz*nco_typ_lng(var_prc_out[idx]->type))) == NULL){
          (void)fprintf(fp_stdout,"%s: ERROR Unable to malloc() %ld*%lu bytes for value buffer for variable %s in main()\n",prg_nm_get(),var_prc_out[idx]->sz,(unsigned long)nco_typ_lng(var_prc_out[idx]->type),var_prc_out[idx]->nm);
          nco_exit(EXIT_FAILURE); 
        } /* endif err */

#ifdef USE_RDR_NETCDF3
        /* Change dimensionionality of values */
        (void)nco_var_dmn_rdr_val(var_prc[idx],var_prc_out[idx],dmn_idx_out_in[idx],dmn_rvr_in[idx]);

        /* Free current dimension correspondence */
        dmn_idx_out_in[idx]=(int *)nco_free(dmn_idx_out_in[idx]);
        dmn_rvr_in[idx]=(nco_bool *)nco_free(dmn_rvr_in[idx]);
#else
        (void)nco_var_dmn_rdr_val_trv(var_prc[idx],var_prc_out[idx],trv_tbl);
#endif

        /* Re-ordering required two value buffers, time to free input buffer */
        var_prc[idx]->val.vp=nco_free(var_prc[idx]->val.vp);

      } /* IS_REORDER */


      /* Obtain output group ID using full group name */
      (void)nco_inq_grp_full_ncid(out_id,var_trv->grp_nm_fll,&grp_out_id);

      /* Get variable ID */
      (void)nco_inq_varid(grp_out_id,var_trv->nm,&var_out_id);

      /* Store the output variable ID */
      var_prc_out[idx]->id=var_out_id;


      if(nco_pck_plc != nco_pck_plc_nil){
        /* Copy input variable buffer to processed variable buffer */
        /* fxm: this is dangerous and leads to double free()'ing variable buffer */
        var_prc_out[idx]->val=var_prc[idx]->val;
        /* (Un-)Pack variable according to packing specification */
        nco_pck_val(var_prc[idx],var_prc_out[idx],nco_pck_map,nco_pck_plc,aed_lst_add_fst+idx,aed_lst_scl_fct+idx);
      } /* endif nco_pck_plc != nco_pck_plc_nil */

#ifdef _OPENMP
#pragma omp critical
#endif /* _OPENMP */
      { /* begin OpenMP critical */
        /* Copy variable to output file then free value buffer */
        if(var_prc_out[idx]->nbr_dim == 0){
          (void)nco_put_var1(grp_out_id,var_prc_out[idx]->id,var_prc_out[idx]->srt,var_prc_out[idx]->val.vp,var_prc_out[idx]->type);
        }else{ /* end if variable is scalar */
          (void)nco_put_vara(grp_out_id,var_prc_out[idx]->id,var_prc_out[idx]->srt,var_prc_out[idx]->cnt,var_prc_out[idx]->val.vp,var_prc_out[idx]->type);
        } /* end if variable is array */
      } /* end OpenMP critical */
      /* Free current output buffer */
      var_prc_out[idx]->val.vp=nco_free(var_prc_out[idx]->val.vp);

    } /* end (OpenMP parallel for) loop over idx */

    if(dbg_lvl >= nco_dbg_fl) (void)fprintf(fp_stderr,"\n");

    /* Write/overwrite packing attributes for newly packed and re-packed variables 
    Logic here should nearly mimic logic in nco_var_dfn() */
    if(nco_pck_plc != nco_pck_plc_nil && nco_pck_plc != nco_pck_plc_upk){
      /* ...put file in define mode to allow metadata writing... */
      (void)nco_redef(out_id);
      /* ...loop through all variables that may have been packed... */
      for(idx=0;idx<nbr_var_prc;idx++){
        /* nco_var_dfn() pre-defined dummy packing attributes in output file 
        only for input variables considered "packable" */
        if(nco_pck_plc_typ_get(nco_pck_map,var_prc[idx]->typ_upk,(nc_type *)NULL)){
          /* Verify input variable was newly packed by this operator
          Writing pre-existing (non-re-packed) attributes here would fail because
          nco_pck_dsk_inq() never fills in var->scl_fct.vp and var->add_fst.vp
          Logic is same as in nco_var_dfn() (except var_prc[] instead of var[])
          If operator newly packed this particular variable... */
          if(
            /* ...either because operator newly packs all variables... */
            (nco_pck_plc == nco_pck_plc_all_new_att) ||
            /* ...or because operator newly packs un-packed variables like this one... */
            (nco_pck_plc == nco_pck_plc_all_xst_att && !var_prc[idx]->pck_ram) ||
            /* ...or because operator re-packs packed variables like this one... */
            (nco_pck_plc == nco_pck_plc_xst_new_att && var_prc[idx]->pck_ram)
            ){
              /* Replace dummy packing attributes with final values, or delete them */
              if(dbg_lvl >= nco_dbg_io) (void)fprintf(stderr,"%s: main() replacing dummy packing attribute values for variable %s\n",prg_nm,var_prc[idx]->nm);
              (void)nco_aed_prc(out_id,aed_lst_add_fst[idx].id,aed_lst_add_fst[idx]);
              (void)nco_aed_prc(out_id,aed_lst_scl_fct[idx].id,aed_lst_scl_fct[idx]);
          } /* endif variable is newly packed by this operator */
        } /* !nco_pck_plc_alw */
      } /* end loop over var_prc */
      (void)nco_enddef(out_id);
    } /* nco_pck_plc == nco_pck_plc_nil || nco_pck_plc == nco_pck_plc_upk */


    /* Close input netCDF file */
    for(thr_idx=0;thr_idx<thr_nbr;thr_idx++) nco_close(in_id_arr[thr_idx]);

    /* Remove local copy of file */
    if(FL_RTR_RMT_LCN && RM_RMT_FL_PST_PRC) (void)nco_fl_rm(fl_in);

  } /* end loop over fl_idx */

  /* Close output file and move it from temporary to permanent location */
  (void)nco_fl_out_cls(fl_out,fl_out_tmp,out_id);

#endif /* USE_TRV_API */

  /* Clean memory unless dirty memory allowed */
  if(flg_cln){
    /* ncpdq-specific memory cleanup */
    if(dmn_rdr_nbr > 0){
      /* Free dimension correspondence list */
      for(idx=0;idx<nbr_var_prc;idx++){
        dmn_idx_out_in[idx]=(int *)nco_free(dmn_idx_out_in[idx]);
        dmn_rvr_in[idx]=(nco_bool *)nco_free(dmn_rvr_in[idx]);
      } /* end loop over idx */
      if(dmn_idx_out_in) dmn_idx_out_in=(int **)nco_free(dmn_idx_out_in);
      if(dmn_rvr_in) dmn_rvr_in=(nco_bool **)nco_free(dmn_rvr_in);
      if(dmn_rvr_rdr) dmn_rvr_rdr=(nco_bool *)nco_free(dmn_rvr_rdr);
      if(dmn_rdr_nbr_in > 0) dmn_rdr_lst_in=nco_sng_lst_free(dmn_rdr_lst_in,dmn_rdr_nbr_in);
      /* Free dimension list pointers */
      dmn_rdr=(dmn_sct **)nco_free(dmn_rdr);
      /* Dimension structures in dmn_rdr are owned by dmn and dmn_out, free'd later */
    } /* endif dmn_rdr_nbr > 0 */
    if(nco_pck_plc != nco_pck_plc_nil){
      if(nco_pck_plc_sng) nco_pck_plc_sng=(char *)nco_free(nco_pck_plc_sng);
      if(nco_pck_map_sng) nco_pck_map_sng=(char *)nco_free(nco_pck_map_sng);
      if(nco_pck_plc != nco_pck_plc_upk){
        /* No need for loop over var_prc variables to free attribute values
        Variable structures and attribute edit lists share same attribute values
        Free them only once, and do it in nco_var_free() */
        aed_lst_add_fst=(aed_sct *)nco_free(aed_lst_add_fst);
        aed_lst_scl_fct=(aed_sct *)nco_free(aed_lst_scl_fct);
      } /* nco_pck_plc == nco_pck_plc_upk */
    } /* nco_pck_plc == nco_pck_plc_nil */

    /* NB: lmt now referenced within lmt_all_lst[idx]  */
    for(idx=0;idx<nbr_dmn_fl;idx++)
      for(jdx=0;jdx< lmt_all_lst[idx]->lmt_dmn_nbr;jdx++)
        lmt_all_lst[idx]->lmt_dmn[jdx]=nco_lmt_free(lmt_all_lst[idx]->lmt_dmn[jdx]);
    if(nbr_dmn_fl > 0) lmt_all_lst=nco_lmt_all_lst_free(lmt_all_lst,nbr_dmn_fl);   
    lmt=(lmt_sct**)nco_free(lmt); 

    /* NCO-generic clean-up */
    /* Free individual strings/arrays */
    if(cmd_ln) cmd_ln=(char *)nco_free(cmd_ln);
    if(cnk_map_sng) cnk_map_sng=(char *)strdup(cnk_map_sng);
    if(cnk_plc_sng) cnk_plc_sng=(char *)strdup(cnk_plc_sng);
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
    for(idx=0;idx<aux_nbr;idx++) aux_arg[idx]=(char *)nco_free(aux_arg[idx]);
    if(aux_nbr > 0) aux=(lmt_sct **)nco_free(aux);
    /* Free chunking information */
    for(idx=0;idx<cnk_nbr;idx++) cnk_arg[idx]=(char *)nco_free(cnk_arg[idx]);
    if(cnk_nbr > 0) cnk=nco_cnk_lst_free(cnk,cnk_nbr);
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

    /* Free traversal table */
    trv_tbl_free(trv_tbl); 
    if(gpe) gpe=(gpe_sct *)nco_gpe_free(gpe);
  } /* !flg_cln */

  /* End timer */ 
  ddra_info.tmr_flg=nco_tmr_end; /* [enm] Timer flag */
  rcd+=nco_ddra((char *)NULL,(char *)NULL,&ddra_info);

  if(rcd != NC_NOERR) nco_err_exit(rcd,"main");
  nco_exit_gracefully();
  return EXIT_SUCCESS;
} /* end main() */


void dbg_var_dim_sct(const char* str, const int idx_var, var_sct *var)
{
  (void)fprintf(stdout,"%s[%d]->nm=%s\n",str,idx_var,var->nm);
  for(int idx_dmn=0;idx_dmn<var->nbr_dim;idx_dmn++){
    dmn_sct *dim=var->dim[idx_dmn];
    (void)fprintf(stdout,"dim[%d]->nm=%s ID=%d cnt=%ld sz=%ld\n",idx_dmn,dim->nm,dim->id,dim->cnt,dim->sz);
  }
}
