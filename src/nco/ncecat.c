/* $Header: /data/zender/nco_20150216/nco/src/nco/ncecat.c,v 1.273 2013-03-29 00:19:04 zender Exp $ */

/* ncecat -- netCDF ensemble concatenator */

/* Purpose: Join variables across files with new record variable or aggregate files as groups */

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

/* URL: http://nco.cvs.sf.net/nco/nco/src/nco/ncecat.c

   Usage:
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

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions and C library */

/* Personal headers */
/* #define MAIN_PROGRAM_FILE MUST precede #include libnco.h */
#define MAIN_PROGRAM_FILE
#include "libnco.h" /* netCDF Operator (NCO) library */

int 
main(int argc,char **argv)
{
  nco_bool CNV_CCM_CCSM_CF;
  nco_bool CPY_GLB_METADATA=True; /* Option M */
  nco_bool EXCLUDE_INPUT_LIST=False; /* Option c */
  nco_bool EXTRACT_ALL_COORDINATES=False; /* Option c */
  nco_bool EXTRACT_ASSOCIATED_COORDINATES=True; /* Option C */
  nco_bool FL_RTR_RMT_LCN;
  nco_bool FL_LST_IN_APPEND=True; /* Option H */
  nco_bool FL_LST_IN_FROM_STDIN=False; /* [flg] fl_lst_in comes from stdin */
  nco_bool FORCE_APPEND=False; /* Option A */
  nco_bool FORCE_OVERWRITE=False; /* Option O */
  nco_bool FORTRAN_IDX_CNV=False; /* Option F */
  nco_bool GROUP_AGGREGATE=False; /* Option G */
  nco_bool GRP_VAR_UNN=False; /* [flg] Select union of specified groups and variables */
  nco_bool HISTORY_APPEND=True; /* Option h */
  nco_bool HAVE_LIMITS=False; /* [flg] Are there user limits? (-d) */
  nco_bool MD5_DIGEST=False; /* [flg] Perform MD5 digests */
  nco_bool MSA_USR_RDR=False; /* [flg] Multi-Slab Algorithm returns hyperslabs in user-specified order */
  nco_bool RAM_CREATE=False; /* [flg] Create file in RAM */
  nco_bool RAM_OPEN=False; /* [flg] Open (netCDF3-only) file(s) in RAM */
  nco_bool RM_RMT_FL_PST_PRC=True; /* Option R */
  nco_bool USE_MM3_WORKAROUND=False; /* [flg] Faster copy on Multi-record Multi-variable netCDF3 files */
  nco_bool WRT_TMP_FL=True; /* [flg] Write output to temporary file */
  nco_bool RECORD_AGGREGATE=True; /* Option G */
  nco_bool flg_cln=False; /* [flg] Clean memory prior to exit */

  char **fl_lst_abb=NULL; /* Option a */
  char **fl_lst_in;
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
  char *rec_dmn_nm=NULL; /* [sng] New record dimension name */
  char *sng_cnv_rcd=NULL_CEWI; /* [sng] strtol()/strtoul() return code */

  /* NCO_GRP_OUT_SFX_LNG is number of consecutive numeric digits automatically generated as group name suffixes */
#define	NCO_GRP_OUT_SFX_LNG 2
  char grp_out_sfx[NCO_GRP_OUT_SFX_LNG+1L];
  char trv_pth[]="/"; /* [sng] Root path of traversal tree */

  const char * const CVS_Id="$Id: ncecat.c,v 1.273 2013-03-29 00:19:04 zender Exp $"; 
  const char * const CVS_Revision="$Revision: 1.273 $";
  const char * const opt_sht_lst="346ACcD:d:Fg:G:HhL:l:Mn:Oo:p:rRt:u:v:X:x-:";

  cnk_sct **cnk=NULL_CEWI;

#if defined(__cplusplus) || defined(PGI_CC)
  ddra_info_sct ddra_info;
  ddra_info.flg_ddra=False;
#else /* !__cplusplus */
  ddra_info_sct ddra_info={.flg_ddra=False};
#endif /* !__cplusplus */

  dmn_sct **dim;
  dmn_sct **dmn_out;
  dmn_sct *rec_dmn;
  
  extern char *optarg;
  extern int optind;
  
  /* Using naked stdin/stdout/stderr in parallel region generates warning
     Copy appropriate filehandle to variable scoped shared in parallel clause */
  FILE * const fp_stderr=stderr; /* [fl] stderr filehandle CEWI */
  FILE *fp_bnr=NULL; /* [fl] Unformatted binary output file handle */

  gpe_sct *gpe=NULL; /* [sng] Group Path Editing (GPE) structure */

  int *in_id_arr;

  int abb_arg_nbr=0;
  int aux_nbr=0; /* [nbr] Number of auxiliary coordinate hyperslabs specified */
  int cnk_map=nco_cnk_map_nil; /* [enm] Chunking map */
  int cnk_nbr=0; /* [nbr] Number of chunk sizes */
  int cnk_plc=nco_cnk_plc_nil; /* [enm] Chunking policy */
  int dfl_lvl=0; /* [enm] Deflate level */
  int fl_idx;
  int fl_nbr=0;
  int fl_in_fmt; /* [enm] Input file format */
  int fl_out_fmt=NCO_FORMAT_UNDEFINED; /* [enm] Output file format */
  int fll_md_old; /* [enm] Old fill mode */
  int gpe_id; /* [id] Group ID of GPE path (diagnostic only) */
  int grp_lst_in_nbr=0; /* [nbr] Number of groups explicitly specified by user */
  int grp_nbr=0; /* [nbr] Number of groups to extract */
  int idx;
  int jdx;
  int in_id;  
  int lmt_nbr=0; /* Option d. NB: lmt_nbr gets incremented */
  int lmt_nbr_rgn=0; /* Option d. Original limit number for GTT initialization */
  int md_open; /* [enm] Mode flag for nc_open() call */
  int nbr_dmn_fl;
  int nbr_dmn_xtr;
  int nbr_var_fix; /* nbr_var_fix gets incremented */
  int nbr_var_fl;
  int nbr_var_prc; /* nbr_var_prc gets incremented */
  int xtr_nbr=0; /* xtr_nbr won't otherwise be set for -c with no -v */
  int opt;
  int out_id;  
  int rcd=NC_NOERR; /* [rcd] Return code */
  int rec_dmn_id=NCO_REC_DMN_UNDEFINED;
  int thr_idx; /* [idx] Index of current thread */
  int thr_nbr=int_CEWI; /* [nbr] Thread number Option t */
  int var_lst_in_nbr=0;

  lmt_sct **aux=NULL_CEWI; /* Auxiliary coordinate limits */
  lmt_sct **lmt;
  lmt_msa_sct **lmt_all_lst; /* List of *lmt_all structures */

  long idx_rec_out=0L; /* idx_rec_out gets incremented */
  
  nm_id_sct *dmn_lst;
  nm_id_sct *xtr_lst=NULL; /* xtr_lst may be alloc()'d from NULL with -c option */  

  size_t bfr_sz_hnt=NC_SIZEHINT_DEFAULT; /* [B] Buffer size hint */
  size_t cnk_sz_scl=0UL; /* [nbr] Chunk size scalar */
  size_t grp_out_lng; /* [nbr] Length of original, canonicalized GPE specification filename component */
  size_t hdr_pad=0UL; /* [B] Pad at end of header section */

  trv_tbl_sct *trv_tbl=NULL; /* [lst] Traversal table */

  var_sct **var;
  var_sct **var_fix;
  var_sct **var_fix_out;
  var_sct **var_out;
  var_sct **var_prc;
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
      {"gag",no_argument,0,0}, /* [flg] Group aggregation */
      {"aggregate_group",no_argument,0,0}, /* [flg] Group aggregation */
      {"md5_digest",no_argument,0,0}, /* [flg] Print MD5 digest */
      {"msa_usr_rdr",no_argument,0,0}, /* [flg] Multi-Slab Algorithm returns hyperslabs in user-specified order */
      {"ram_all",no_argument,0,0}, /* [flg] Open (netCDF3) and create file(s) in RAM */
      {"create_ram",no_argument,0,0}, /* [flg] Create file in RAM */
      {"open_ram",no_argument,0,0}, /* [flg] Open (netCDF3) file(s) in RAM */
      {"diskless_all",no_argument,0,0}, /* [flg] Open (netCDF3) and create file(s) in RAM */
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
      {"glb_mtd_spr",no_argument,0,'M'},
      {"global_metadata_suppress",no_argument,0,'M'},
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
      {"help",no_argument,0,'?'},
      {"hlp",no_argument,0,'?'},
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
      if(!strcmp(opt_crr,"gag") || !strcmp(opt_crr,"aggregate_group")) GROUP_AGGREGATE=True; /* [flg] Aggregate files into groups not records */
      if(!strcmp(opt_crr,"hdr_pad") || !strcmp(opt_crr,"header_pad")){
        hdr_pad=strtoul(optarg,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
        if(*sng_cnv_rcd) nco_sng_cnv_err(optarg,"strtoul",sng_cnv_rcd);
      } /* endif "hdr_pad" */
      if(!strcmp(opt_crr,"md5_digest")){
        MD5_DIGEST=True;
        if(dbg_lvl >= nco_dbg_std) (void)fprintf(stderr,"%s: INFO Will perform MD5 digests of input and output hyperslabs\n",prg_nm_get());
      } /* endif "md5" */
      if(!strcmp(opt_crr,"msa_usr_rdr")) MSA_USR_RDR=True; /* [flg] Multi-Slab Algorithm returns hyperslabs in user-specified order */
      if(!strcmp(opt_crr,"ram_all") || !strcmp(opt_crr,"create_ram") || !strcmp(opt_crr,"diskless_all")) RAM_CREATE=True; /* [flg] Open (netCDF3) file(s) in RAM */
      if(!strcmp(opt_crr,"ram_all") || !strcmp(opt_crr,"open_ram") || !strcmp(opt_crr,"diskless_all")) RAM_OPEN=True; /* [flg] Create file in RAM */
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
    case 'D': /* Debugging level. Default is 0. */
      dbg_lvl=(unsigned short int)strtoul(optarg,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
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
      grp_nbr=grp_lst_in_nbr;
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
      if(abb_arg_nbr < 1 || abb_arg_nbr > 5){
        (void)fprintf(stdout,"%s: ERROR Incorrect abbreviation for file list\n",prg_nm);
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

  if(GROUP_AGGREGATE){
    RECORD_AGGREGATE=!GROUP_AGGREGATE;
    fl_out_fmt=NC_FORMAT_NETCDF4; 
  } /* !GROUP_AGGREGATE */

  /* Process positional arguments and fill in filenames */
  fl_lst_in=nco_fl_lst_mk(argv,argc,optind,&fl_nbr,&fl_out,&FL_LST_IN_FROM_STDIN);

  /* Make uniform list of user-specified chunksizes */
  if(cnk_nbr > 0) cnk=nco_cnk_prs(cnk_nbr,cnk_arg);

  /* Make uniform list of user-specified dimension limits */
  lmt=nco_lmt_prs(lmt_nbr,lmt_arg);

  /* Parse filename */
  fl_in=nco_fl_nm_prs(fl_in,0,&fl_nbr,fl_lst_in,abb_arg_nbr,fl_lst_abb,fl_pth);
  /* Make sure file is on local system and is readable or die trying */
  fl_in=nco_fl_mk_lcl(fl_in,fl_pth_lcl,&FL_RTR_RMT_LCN);

  if(RECORD_AGGREGATE){

    /* Initialize thread information */
    thr_nbr=nco_openmp_ini(thr_nbr);
    in_id_arr=(int *)nco_malloc(thr_nbr*sizeof(int));

    /* Open file using appropriate buffer size hints and verbosity */
    if(RAM_OPEN) md_open=NC_NOWRITE|NC_DISKLESS; else md_open=NC_NOWRITE;
    rcd+=nco_fl_open(fl_in,md_open,&bfr_sz_hnt,&in_id);

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
    (void)nco_inq(in_id,&nbr_dmn_fl,&nbr_var_fl,(int *)NULL,&rec_dmn_id);
    (void)nco_inq_format(in_id,&fl_in_fmt); /* fxm: not sure why this line must be _here_ rather than at "consanguinous", but it must */

    /* Begin API4 */
    /* Construct traversal table */
    trv_tbl_init(&trv_tbl);

    /* Construct GTT, Group Traversal Table (groups,variables,dimensions, limits) */
    (void)nco_bld_trv_tbl(in_id,trv_pth,MSA_USR_RDR,lmt_nbr_rgn,lmt,FORTRAN_IDX_CNV,trv_tbl);

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
    /* End API4 */

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

    /* We now have final list of variables to extract. Phew. */

    /* Find coordinate/dimension values associated with user-specified limits
    NB: nco_lmt_evl() with same nc_id contains OpenMP critical region */
    for(idx=0;idx<lmt_nbr;idx++) (void)nco_lmt_evl(in_id,lmt[idx],0L,FORTRAN_IDX_CNV);

    /* Place all dimensions in lmt_all_lst */
    lmt_all_lst=(lmt_msa_sct **)nco_malloc(nbr_dmn_fl*sizeof(lmt_msa_sct *));
    /* Initialize lmt_msa_sct's */ 
    (void)nco_msa_lmt_all_ntl(in_id,MSA_USR_RDR,lmt_all_lst,nbr_dmn_fl,lmt,lmt_nbr);

    /* Find dimensions associated with variables to be extracted */
    dmn_lst=nco_dmn_lst_ass_var(in_id,xtr_lst,xtr_nbr,&nbr_dmn_xtr);

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
    } /* end loop over dimensions */

    /* Merge hyperslab limit information into dimension structures */
    if(nbr_dmn_fl > 0) (void)nco_dmn_lmt_all_mrg(dmn_out,nbr_dmn_xtr,lmt_all_lst,nbr_dmn_fl); 

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
    (void)nco_var_lst_dvd(var,var_out,xtr_nbr,CNV_CCM_CCSM_CF,True,nco_pck_plc_nil,nco_pck_map_nil,(dmn_sct **)NULL,0,&var_fix,&var_fix_out,&nbr_var_fix,&var_prc,&var_prc_out,&nbr_var_prc);

  } /* !RECORD_AGGREGATE */

  /* Make output and input files consanguinous */
  if(fl_out_fmt == NCO_FORMAT_UNDEFINED) fl_out_fmt=fl_in_fmt;

  /* Verify output file format supports requested actions */
  (void)nco_fl_fmt_vet(fl_out_fmt,cnk_nbr,dfl_lvl);

  /* Open output file */
  fl_out_tmp=nco_fl_out_open(fl_out,FORCE_APPEND,FORCE_OVERWRITE,fl_out_fmt,&bfr_sz_hnt,RAM_CREATE,RAM_OPEN,WRT_TMP_FL,&out_id);

  /* ncecat-specific operations */
  if(RECORD_AGGREGATE){

    /* Copy global attributes */
    if(CPY_GLB_METADATA) (void)nco_att_cpy(in_id,out_id,NC_GLOBAL,NC_GLOBAL,(nco_bool)True);

    /* Catenate time-stamped command line to "history" global attribute */
    if(HISTORY_APPEND) (void)nco_hst_att_cat(out_id,cmd_ln);

    /* Add input file list global attribute */
    if(FL_LST_IN_APPEND && HISTORY_APPEND && FL_LST_IN_FROM_STDIN) (void)nco_fl_lst_att_cat(out_id,fl_lst_in,fl_nbr);

    /* Always construct new "record" dimension from scratch */
    rec_dmn=(dmn_sct *)nco_malloc(sizeof(dmn_sct));
    if(rec_dmn_nm == NULL) rec_dmn->nm=rec_dmn_nm=(char *)strdup("record"); else rec_dmn->nm=rec_dmn_nm;
    rec_dmn->id=-1;
    rec_dmn->nc_id=-1;
    rec_dmn->xrf=NULL;
    rec_dmn->val.vp=NULL;
    rec_dmn->is_crd_dmn=False;
    rec_dmn->is_rec_dmn=True;
    rec_dmn->sz=0L;
    rec_dmn->cnt=0L;
    rec_dmn->srd=0L;
    rec_dmn->srt=0L;
    rec_dmn->end=rec_dmn->sz-1L;

    /* Change existing record dimension, if any, to regular dimension */
    for(idx=0;idx<nbr_dmn_xtr;idx++){
      /* Is any input dimension a record dimension? */
      if(dmn_out[idx]->is_rec_dmn){
        dmn_out[idx]->is_rec_dmn=False;
        break;
      } /* end if */
    } /* end loop over idx */

    /* Add record dimension to end of dimension list */
    nbr_dmn_xtr++;
    dmn_out=(dmn_sct **)nco_realloc(dmn_out,nbr_dmn_xtr*sizeof(dmn_sct **));
    dmn_out[nbr_dmn_xtr-1]=rec_dmn;

    if(thr_nbr > 0 && HISTORY_APPEND) (void)nco_thr_att_cat(out_id,thr_nbr);

    /* Define dimensions in output file */
    (void)nco_dmn_dfn(fl_out,out_id,dmn_out,nbr_dmn_xtr);

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
      var_prc_out[idx]->dim[0]=rec_dmn;
      var_prc_out[idx]->dmn_id[0]=rec_dmn->id;
      var_prc_out[idx]->cnt[0]=1L;
      var_prc_out[idx]->end[0]=-1L;
      var_prc_out[idx]->srd[0]=-1L;
      var_prc_out[idx]->srt[0]=-1L;
    } /* end loop over idx */

    /* Define variables in output file, copy their attributes */
    (void)nco_var_dfn(in_id,fl_out,out_id,var_out,xtr_nbr,(dmn_sct **)NULL,(int)0,nco_pck_plc_nil,nco_pck_map_nil,dfl_lvl);

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
    (void)nco_close(in_id);

    /* Timestamp end of metadata setup and disk layout */
    rcd+=nco_ddra((char *)NULL,(char *)NULL,&ddra_info);
    ddra_info.tmr_flg=nco_tmr_rgl;
  } /* !RECORD_AGGREGATE */

  if(GROUP_AGGREGATE){
#ifndef ENABLE_NETCDF4
    (void)fprintf(stderr,"%s: ERROR GAG requires netCDF4 capabilities. HINT: Rebuild NCO with netCDF4 enabled.\n");
    nco_exit(EXIT_FAILURE);
#endif /* ENABLE_NETCDF4 */
    if(dbg_lvl >= nco_dbg_fl) (void)fprintf(stderr,"%s: INFO Group Aggregation (GAG) feature enabled\n",prg_nm_get());
    if(fl_out_fmt != NC_FORMAT_NETCDF4){
      (void)fprintf(stderr,"%s: ERROR Group Aggregation requires requires netCDF4 output format but user explicitly requested format = %s\n",prg_nm_get(),nco_fmt_sng(fl_out_fmt));
      nco_exit(EXIT_FAILURE);
    } /* endif err */
  } /* !GROUP_AGGREGATE */

  /* Loop over input files */
  for(fl_idx=0;fl_idx<fl_nbr;fl_idx++){
    /* Parse filename */
    if(fl_idx) fl_in=nco_fl_nm_prs(fl_in,fl_idx,(int *)NULL,fl_lst_in,abb_arg_nbr,fl_lst_abb,fl_pth);
    if(dbg_lvl >= nco_dbg_fl) (void)fprintf(stderr,"%s: INFO Input file %d is %s",prg_nm_get(),fl_idx,fl_in);
    /* Make sure file is on local system and is readable or die trying */
    if(fl_idx) fl_in=nco_fl_mk_lcl(fl_in,fl_pth_lcl,&FL_RTR_RMT_LCN);
    if(dbg_lvl >= nco_dbg_fl && FL_RTR_RMT_LCN) (void)fprintf(stderr,", local file is %s",fl_in);
    if(dbg_lvl >= nco_dbg_fl) (void)fprintf(stderr,"\n");

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

        /* Is there a .nc, .cdf, .nc3, or .nc4 suffix? */
        fl_in_lng=strlen(fl_in);
        sfx_lng=3L;
        sfx_fst=fl_in_lng-sfx_lng;
	/* Sample data found in, e.g., ftp://ftp.unidata.ucar.edu/pub/netcdf/sample_data and http://hdfeos.org/zoo
	   Two versions of HDF-EOS exist: HDF-EOS2 for HDF4, and HDF-EOS5 for HDF5 */
        if(strncmp(fl_in+sfx_fst,".nc",sfx_lng) && /* netCDF standard suffix */
	   strncmp(fl_in+sfx_fst,".H5",sfx_lng) && /* HDF5 suffix used by ICESat GLAS, e.g., GLAH05_633_2103_001_1107_3_01_0001.H5 */
	   strncmp(fl_in+sfx_fst,".h5",sfx_lng)){ /* HDF5 suffix used by BUV, SBUV, e.g., BUV-Nimbus04_L3zm_v01-00-2012m0203t144121.h5 */
          sfx_lng=4L;
          sfx_fst=fl_in_lng-sfx_lng;
          if(strncmp(fl_in+sfx_fst,".cdf",sfx_lng) && /* netCDF old-fashioned suffix */
	     strncmp(fl_in+sfx_fst,".hdf",sfx_lng) && /* HDF-EOS2 (HDF4) suffix used by AIRS, AMSR-E, MODIS, e.g., AIRS.2002.08.01.L3.RetStd_H031.v4.0.21.0.G06104133732.hdf, MSR_E_L2_Rain_V10_200905312326_A.hdf, MOD10CM.A2007001.005.2007108111758.hdf */
	     strncmp(fl_in+sfx_fst,".HDF",sfx_lng) && /* HDF-EOS2 (HDF4) suffix used by TRMM, e.g., 3B43.070901.6A.HDF */
	     strncmp(fl_in+sfx_fst,".he5",sfx_lng) && /* HDF-EOS5 (HDF5) suffix used by HIRDLS, e.g., HIRDLS-Aura_L3ZAD_v06-00-00-c02_2005d022-2008d077.he5 */
	     strncmp(fl_in+sfx_fst,".nc3",sfx_lng) && /* netCDF3 variant suffix */
	     strncmp(fl_in+sfx_fst,".nc4",sfx_lng)){ /* netCDF4 variant suffix */
              (void)fprintf(stderr,"%s: WARNING GAG filename suffix is unusual---using whole filename as group name\n",prg_nm_get());
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
	if(rcd == NC_NOERR) (void)fprintf(stderr,"%s: WARNING GAG path \"%s\" automatically derived from stub of filename %s conflicts with existing path in output file. Any input data with same absolute path names as contents of a previous input file will be overwritten. Is the same input file specified multiple times? Is this intentional?\nHINT: To distribute copies of a single input file into different groups, use GPE to generate distinct output group names, e.g., %s -G copy in.nc in.nc out.nc\n",prg_nm_get(),gpe_arg,fl_in,prg_nm_get());
      } /* !grp_out */
      /* Free old structure, if any, before re-use */
      if(gpe) gpe=(gpe_sct *)nco_gpe_free(gpe);
      gpe=nco_gpe_prs_arg(gpe_arg);
      gpe_arg=(char *)nco_free(gpe_arg);
      if(dbg_lvl >= nco_dbg_scl) (void)fprintf(stderr,"%s: INFO GAG current file has gpe->arg=%s\n",prg_nm_get(),gpe->arg);

      /* Open file using appropriate buffer size hints and verbosity */
      if(RAM_OPEN) md_open=NC_NOWRITE|NC_DISKLESS; else md_open=NC_NOWRITE;
      rcd=nco_fl_open(fl_in,md_open,&bfr_sz_hnt,&in_id);

      if(fl_idx == 0){
        /* fxm: Copy global attributes of first file to root of output file to preserve history attribute */
        /* Copy global attributes */
        if(CPY_GLB_METADATA) (void)nco_att_cpy(in_id,out_id,NC_GLOBAL,NC_GLOBAL,(nco_bool)True);

        /* Catenate time-stamped command line to "history" global attribute */
        if(HISTORY_APPEND) (void)nco_hst_att_cat(out_id,cmd_ln);

        /* Add input file list global attribute */
        if(FL_LST_IN_APPEND && HISTORY_APPEND && FL_LST_IN_FROM_STDIN) (void)nco_fl_lst_att_cat(out_id,fl_lst_in,fl_nbr);
      } /* endif first file */

      /* Construct traversal table */
      trv_tbl_init(&trv_tbl);

      /* Construct GTT, Group Traversal Table (groups,variables,dimensions, limits) */
      (void)nco_bld_trv_tbl(in_id,trv_pth,MSA_USR_RDR,lmt_nbr_rgn,lmt,FORTRAN_IDX_CNV,trv_tbl);

      /* Get number of variables, dimensions, and global attributes in file, file format */
      (void)trv_tbl_inq((int *)NULL,(int *)NULL,(int *)NULL,&nbr_dmn_fl,(int *)NULL,(int *)NULL,(int *)NULL,(int *)NULL,&nbr_var_fl,trv_tbl);
      (void)nco_inq_format(in_id,&fl_in_fmt);

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

      /* Check -v and -g input names and create extraction list */
      nco_xtr_mk(grp_lst_in,grp_nbr,var_lst_in,xtr_nbr,EXTRACT_ALL_COORDINATES,GRP_VAR_UNN,trv_tbl);

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

      /* We now have final list of variables to extract. Phew. */

      /* Define extracted groups, variables, and attributes in output file */
      (void)nco_xtr_dfn(in_id,out_id,&cnk_map,&cnk_plc,cnk_sz_scl,cnk,cnk_nbr,dfl_lvl,gpe,CPY_GLB_METADATA,(nco_bool)True,rec_dmn_nm,trv_tbl);

      /* Turn off default filling behavior to enhance efficiency */
      nco_set_fill(out_id,NC_NOFILL,&fll_md_old);

      /* Write extracted data to output file */
      (void)nco_xtr_wrt(in_id,out_id,fp_bnr,MD5_DIGEST,HAVE_LIMITS,trv_tbl);

      /* Close input netCDF file */
      (void)nco_close(in_id);

      /* Free traversal table */
      trv_tbl_free(trv_tbl);

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
#pragma omp parallel for default(none) private(idx,in_id) shared(dbg_lvl,fl_nbr,idx_rec_out,in_id_arr,nbr_var_prc,out_id,var_prc,var_prc_out,lmt_all_lst,nbr_dmn_fl,jdx,MD5_DIGEST)
#endif /* !_OPENMP */
      /* Process all variables in current file */
      for(idx=0;idx<nbr_var_prc;idx++){
        in_id=in_id_arr[omp_get_thread_num()];
        if(dbg_lvl >= nco_dbg_var) (void)fprintf(fp_stderr,"%s, ",var_prc[idx]->nm);
        if(dbg_lvl >= nco_dbg_var) (void)fflush(fp_stderr);
        /* Variables may have different ID, missing_value, type, in each file */
        (void)nco_var_mtd_refresh(in_id,var_prc[idx]);
        /* Retrieve variable from disk into memory */
        /* NB: nco_var_get() with same nc_id contains OpenMP critical region */
        (void)nco_msa_var_get(in_id,var_prc[idx],lmt_all_lst,nbr_dmn_fl);
        /* Size of record dimension is 1 in output file */
        var_prc_out[idx]->cnt[0]=1L;
        var_prc_out[idx]->srt[0]=idx_rec_out;
        /* Write variable into current record in output file */
#ifdef _OPENMP
#pragma omp critical
#endif /* _OPENMP */
        { /* begin OpenMP critical */
          if(var_prc[idx]->nbr_dim == 0){
            (void)nco_put_var1(out_id,var_prc_out[idx]->id,var_prc_out[idx]->srt,var_prc[idx]->val.vp,var_prc[idx]->type);
          }else{ /* end if variable is scalar */
            (void)nco_put_vara(out_id,var_prc_out[idx]->id,var_prc_out[idx]->srt,var_prc_out[idx]->cnt,var_prc[idx]->val.vp,var_prc[idx]->type);
          } /* end if variable is array */
          /* Perform MD5 digest of input and output data if requested */
          if(MD5_DIGEST) (void)nco_md5_chk(var_prc_out[idx]->nm,var_prc_out[idx]->sz*nco_typ_lng(var_prc[idx]->type),out_id,var_prc_out[idx]->srt,var_prc_out[idx]->cnt,var_prc[idx]->val.vp);
          /* Free current input buffer */
          var_prc[idx]->val.vp=nco_free(var_prc[idx]->val.vp);
        } /* end OpenMP critical */

      } /* end (OpenMP parallel for) loop over idx */
      if(dbg_lvl >= nco_dbg_fl) (void)fprintf(stderr,"\n");

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
  if(flg_cln){
    /* ncecat-specific memory cleanup */
    if(rec_dmn_nm) rec_dmn_nm=(char *)nco_free(rec_dmn_nm);

    /* NB: free lmt[] is now referenced within lmt_all_lst[idx] */
    for(idx=0;idx<nbr_dmn_fl;idx++)
      for(jdx=0;jdx<lmt_all_lst[idx]->lmt_dmn_nbr;jdx++)
        lmt_all_lst[idx]->lmt_dmn[jdx]=nco_lmt_free(lmt_all_lst[idx]->lmt_dmn[jdx]);

    lmt=(lmt_sct**)nco_free(lmt); 
    if(nbr_dmn_fl > 0) lmt_all_lst=nco_lmt_all_lst_free(lmt_all_lst,nbr_dmn_fl);

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
    if(nbr_dmn_xtr > 0) dim=nco_dmn_lst_free(dim,nbr_dmn_xtr-1); /* NB: ncecat has one fewer input than output dimension */
    if(nbr_dmn_xtr > 0) dmn_out=nco_dmn_lst_free(dmn_out,nbr_dmn_xtr); 
    /* Free variable lists */
    if(xtr_nbr > 0) var=nco_var_lst_free(var,xtr_nbr);
    if(xtr_nbr > 0) var_out=nco_var_lst_free(var_out,xtr_nbr);
    var_prc=(var_sct **)nco_free(var_prc);
    var_prc_out=(var_sct **)nco_free(var_prc_out);
    var_fix=(var_sct **)nco_free(var_fix);
    var_fix_out=(var_sct **)nco_free(var_fix_out);
    if(gpe) gpe=(gpe_sct *)nco_gpe_free(gpe);
  } /* !flg_cln */

  /* End timer */ 
  ddra_info.tmr_flg=nco_tmr_end; /* [enm] Timer flag */
  rcd+=nco_ddra((char *)NULL,(char *)NULL,&ddra_info);

  if(rcd != NC_NOERR) nco_err_exit(rcd,"main");
  nco_exit_gracefully();
  return EXIT_SUCCESS;
} /* end main() */
