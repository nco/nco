/* $Header: /data/zender/nco_20150216/nco/src/nco/ncks.c,v 1.551 2013-01-19 04:29:16 zender Exp $ */

/* ncks -- netCDF Kitchen Sink */

/* Purpose: Extract (subsets of) variables from a netCDF file 
   Print them to screen, or copy them to a new file, or both */

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

/* URL: http://nco.cvs.sf.net/nco/nco/src/nco/ncks.c

   Usage:
   ncks ~/nco/data/in.nc 
   ncks -v one ~/nco/data/in.nc
   ncks ~/nco/data/in.nc ~/foo.nc
   ncks -O -4 ~/nco/data/in.nc ~/foo.nc
   ncks -v one ~/nco/data/in.nc ~/foo.nc
   ncks -p /ZENDER/tmp -l /data/zender/tmp h0001.nc ~/foo.nc
   ncks -s "%+16.10f\n" -H -C -v three_dmn_var ~/nco/data/in.nc
   ncks -H -v fl_nm,fl_nm_arr ~/nco/data/in.nc
   ncks -H -C -u -d wvl,'0.4 micron','0.7 micron' -v wvl ~/nco/data/in.nc
   ncks -H -d fl_dim,1 -d char_dim,6,12 -v fl_nm,fl_nm_arr ~/nco/data/in.nc
   ncks -H -m -v char_var_nul,char_var_space,char_var_multinul ~/nco/data/in.nc
   ncks -H -C -v three_dmn_rec_var -d time,,,2 ~/nco/data/in.nc
   ncks -H -C -v lon -d lon,3,1 ~/nco/data/in.nc 
   ncks -M -p http://motherlode.ucar.edu:8080/thredds/dodsC/testdods in.nc
   ncks -O -v one -p http://motherlode.ucar.edu:8080/thredds/dodsC/testdods in.nc ~/foo.nc
   ncks -O -G foo ~/nco/data/in.nc ~/foo.nc
   ncks -O -G :-5 -v v7 ~/nco/data/in_grp.nc ~/foo.nc
   ncks -O -G level3name:-5 -v v7 ~/nco/data/in_grp.nc ~/foo.nc
   ncks -O -v time ~/in_grp.nc ~/foo.nc */

#ifdef HAVE_CONFIG_H
# include <config.h> /* Autotools tokens */
#endif /* !HAVE_CONFIG_H */

/* Standard C headers */
#include <assert.h> /* assert() */
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
/* #define MAIN_PROGRAM_FILE MUST precede #include libnco.h */
#define MAIN_PROGRAM_FILE

#include "libnco.h" /* netCDF Operator (NCO) library */

int 
main(int argc,char **argv)
{
  nco_bool ALPHABETIZE_OUTPUT=True; /* Option a */
  nco_bool CNV_CCM_CCSM_CF;
  nco_bool EXCLUDE_INPUT_LIST=False; /* Option x */
  nco_bool EXTRACT_ALL_COORDINATES=False; /* Option c */
  nco_bool EXTRACT_ASSOCIATED_COORDINATES=True; /* Option C */
  nco_bool FL_RTR_RMT_LCN;
  nco_bool FIX_REC_DMN=False; /* [flg] Fix record dimension */
  nco_bool FL_LST_IN_FROM_STDIN=False; /* [flg] fl_lst_in comes from stdin */
  nco_bool FORCE_APPEND=False; /* Option A */
  nco_bool FORCE_NOCLOBBER=False; /* Option no-clobber */
  nco_bool FORCE_OVERWRITE=False; /* Option O */
  nco_bool FORTRAN_IDX_CNV=False; /* Option F */
  nco_bool GET_GRP_INFO=False; /* [flg] Iterate file, get group extended information */
  nco_bool GET_FILE_INFO=False; /* [flg] Get file information (#groups, #dimensions, #attributes, #variables) */
  nco_bool GET_PRG_INFO=False; /* [flg] Get compiled program information (e.g libraries) */
  nco_bool GET_LIST=False; /* [flg] Iterate file, print variables and exit */
  nco_bool GRP_VAR_UNN=False; /* [flg] Select union of specified groups and variables */
  nco_bool HISTORY_APPEND=True; /* Option h */
  nco_bool MD5_DIGEST=False; /* [flg] Perform MD5 digests */
  nco_bool MSA_USR_RDR=False; /* [flg] Multi-Slab Algorithm returns hyperslabs in user-specified order */
  nco_bool PRN_DMN_IDX_CRD_VAL=True; /* [flg] Print leading dimension/coordinate indices/values Option Q */
  nco_bool PRN_DMN_UNITS=False; /* [flg] Print dimensional units Option u */
  nco_bool PRN_DMN_VAR_NM=True; /* [flg] Print dimension/variable names */
  nco_bool PRN_DMN_UNITS_TGL=False; /* [flg] Toggle print dimensional units Option u */
  nco_bool PRN_GLB_METADATA=False; /* [flg] Print global metadata */
  nco_bool PRN_GLB_METADATA_TGL=False; /* [flg] Toggle print global metadata Option M */
  nco_bool PRN_MSS_VAL_BLANK=True; /* [flg] Print missing values as blanks */
  nco_bool PRN_QUIET=False; /* [flg] Turn off all printing to screen */
  nco_bool PRN_VAR_DATA=False; /* [flg] Print variable data */
  nco_bool PRN_VAR_DATA_TGL=False; /* [flg] Toggle print variable data Option H */
  nco_bool PRN_VAR_METADATA=False; /* [flg] Print variable metadata */
  nco_bool PRN_VAR_METADATA_TGL=False; /* [flg] Toggle print variable metadata Option m */
  nco_bool PRN_VRB=False; /* [flg] Print data and metadata by default */
  nco_bool RAM_CREATE=False; /* [flg] Create file in RAM */
  nco_bool RAM_OPEN=False; /* [flg] Open (netCDF3-only) file(s) in RAM */
  nco_bool RM_RMT_FL_PST_PRC=True; /* Option R */
  nco_bool WRT_TMP_FL=True; /* [flg] Write output to temporary file */
  nco_bool flg_cln=False; /* [flg] Clean memory prior to exit */

  char **fl_lst_abb=NULL; /* Option a */
  char **fl_lst_in;
  char **grp_lst_in=NULL;
  char **var_lst_in=NULL;
  char *aux_arg[NC_MAX_DIMS];
  char *cmd_ln;
  char *cnk_arg[NC_MAX_DIMS];
  char *cnk_map_sng=NULL_CEWI; /* [sng] Chunking map */
  char *cnk_plc_sng=NULL_CEWI; /* [sng] Chunking policy */
  char *dlm_sng=NULL;
  char *fl_bnr=NULL; /* [sng] Unformatted binary output file */
  char *fl_in=NULL;
  char *fl_out=NULL; /* Option o */
  char *fl_out_tmp=NULL_CEWI;
  char *fl_pth=NULL; /* Option p */
  char *fl_pth_lcl=NULL; /* Option l */
  char *lmt_arg[NC_MAX_DIMS];
  char *opt_crr=NULL; /* [sng] String representation of current long-option name */
  char *optarg_lcl=NULL; /* [sng] Local copy of system optarg */
  char *rec_dmn_nm=NULL; /* [sng] Record dimension name */
  char *sng_cnv_rcd=NULL_CEWI; /* [sng] strtol()/strtoul() return code */

  char rth[]="/"; /* [sng] Group path */

  const char * const CVS_Id="$Id: ncks.c,v 1.551 2013-01-19 04:29:16 zender Exp $"; 
  const char * const CVS_Revision="$Revision: 1.551 $";
  const char * const opt_sht_lst="346aABb:CcD:d:FG:g:HhL:l:MmOo:Pp:qQrRs:uv:X:xz-:";

  cnk_sct **cnk=NULL_CEWI;

#if defined(__cplusplus) || defined(PGI_CC)
  ddra_info_sct ddra_info;
  ddra_info.flg_ddra=False;
#else /* !__cplusplus */
  ddra_info_sct ddra_info={.flg_ddra=False};
#endif /* !__cplusplus */

  extern char *optarg;
  extern int optind;

  FILE *fp_bnr=NULL; /* [fl] Unformatted binary output file handle */

  gpe_sct *gpe=NULL; /* [sng] Group Path Editing (GPE) structure */

  int abb_arg_nbr=0;
  int aux_nbr=0; /* [nbr] Number of auxiliary coordinate hyperslabs specified */
  int cnk_map=nco_cnk_map_nil; /* [enm] Chunking map */
  int cnk_nbr=0; /* [nbr] Number of chunk sizes */
  int cnk_plc=nco_cnk_plc_nil; /* [enm] Chunking policy */
  int dfl_lvl=0; /* [enm] Deflate level */
  int fl_in_fmt=NCO_FORMAT_UNDEFINED; /* [enm] Input file format */
  int fl_nbr=0;
  int fl_out_fmt=NCO_FORMAT_UNDEFINED; /* [enm] Output file format */
  int fll_md_old; /* [enm] Old fill mode */
  int grp_lst_in_nbr=0; /* [nbr] Number of groups explicitly specified by user */
  int grp_nbr=0; /* [nbr] Number of groups to extract */
  int idx;
  int in_id;  
  int jdx;
  int lmt_nbr=0; /* Option d. NB: lmt_nbr gets incremented */
  int md_open; /* [enm] Mode flag for nc_open() call */
  int nbr_dmn_fl;
  int nbr_glb_att;
  int nbr_grp_fl;
  int nbr_var_fl;
  int opt;
  int rcd=NC_NOERR; /* [rcd] Return code */
  int rec_dmn_id=NCO_REC_DMN_UNDEFINED;
  int var_lst_in_nbr=0;
  int xtr_nbr=0; /* xtr_nbr will not otherwise be set for -c with no -v */

  lmt_sct **aux=NULL_CEWI; /* Auxiliary coordinate limits */
  lmt_sct **lmt=NULL_CEWI;

  lmt_all_sct **lmt_all_lst=NULL_CEWI; /* List of *lmt_all structures */

  size_t bfr_sz_hnt=NC_SIZEHINT_DEFAULT; /* [B] Buffer size hint */
  size_t cnk_sz_scl=0UL; /* [nbr] Chunk size scalar */
  size_t hdr_pad=0UL; /* [B] Pad at end of header section */

  trv_tbl_sct *trv_tbl=NULL; /* [lst] Traversal table */

  static struct option opt_lng[]=
    { /* Structure ordered by short option key if possible */
      /* Long options with no argument, no short option counterpart */
      {"cln",no_argument,0,0}, /* [flg] Clean memory prior to exit */
      {"clean",no_argument,0,0}, /* [flg] Clean memory prior to exit */
      {"mmr_cln",no_argument,0,0}, /* [flg] Clean memory prior to exit */
      {"drt",no_argument,0,0}, /* [flg] Allow dirty memory on exit */
      {"dirty",no_argument,0,0}, /* [flg] Allow dirty memory on exit */
      {"mmr_drt",no_argument,0,0}, /* [flg] Allow dirty memory on exit */
      {"md5_digest",no_argument,0,0}, /* [flg] Perform MD5 digests */
      {"cmp",no_argument,0,0},
      {"compiler",no_argument,0,0},
      {"fix_rec_dmn",no_argument,0,0}, /* [flg] Fix record dimension */
      {"no_rec_dmn",no_argument,0,0}, /* [flg] Fix record dimension */
      {"id",no_argument,0,0}, /* [flg] Print normally hidden information, like file, group, and variable IDs */
      {"lbr",no_argument,0,0},
      {"library",no_argument,0,0},
      {"mpi_implementation",no_argument,0,0},
      {"msa_usr_rdr",no_argument,0,0}, /* [flg] Multi-Slab Algorithm returns hyperslabs in user-specified order */
      {"no_blank",no_argument,0,0}, /* [flg] Print numeric missing values */
      {"no-blank",no_argument,0,0}, /* [flg] Print numeric missing values */
      {"noblank",no_argument,0,0}, /* [flg] Print numeric missing values */
      {"no_clb",no_argument,0,0},
      {"noclobber",no_argument,0,0},
      {"no-clobber",no_argument,0,0},
      {"no_clobber",no_argument,0,0},
      {"no_dmn_var_nm",no_argument,0,0}, /* [flg] Print dimension/variable names */
      {"no_nm_prn",no_argument,0,0}, /* [flg] Print dimension/variable names */
      {"ram_all",no_argument,0,0}, /* [flg] Open (netCDF3) and create file(s) in RAM */
      {"create_ram",no_argument,0,0}, /* [flg] Create file in RAM */
      {"open_ram",no_argument,0,0}, /* [flg] Open (netCDF3) file(s) in RAM */
      {"diskless_all",no_argument,0,0}, /* [flg] Open (netCDF3) and create file(s) in RAM */
      {"secret",no_argument,0,0},
      {"shh",no_argument,0,0},
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
      {"cnk_map",required_argument,0,0}, /* [nbr] Chunking map */
      {"chunk_map",required_argument,0,0}, /* [nbr] Chunking map */
      {"cnk_plc",required_argument,0,0}, /* [nbr] Chunking policy */
      {"chunk_policy",required_argument,0,0}, /* [nbr] Chunking policy */
      {"cnk_scl",required_argument,0,0}, /* [nbr] Chunk size scalar */
      {"chunk_scalar",required_argument,0,0}, /* [nbr] Chunk size scalar */
      {"cnk_dmn",required_argument,0,0}, /* [nbr] Chunk size */
      {"chunk_dimension",required_argument,0,0}, /* [nbr] Chunk size */
      {"fl_fmt",required_argument,0,0},
      {"file_format",required_argument,0,0},
      {"hdr_pad",required_argument,0,0},
      {"header_pad",required_argument,0,0},
      {"mk_rec_dmn",required_argument,0,0}, /* [sng] Name of record dimension in output */
      {"tst_udunits",required_argument,0,0},
      /* Long options with short counterparts */
      {"3",no_argument,0,'3'},
      {"4",no_argument,0,'4'},
      {"64bit",no_argument,0,'4'},
      {"netcdf4",no_argument,0,'4'},
      {"abc",no_argument,0,'a'},
      {"alphabetize",no_argument,0,'a'},
      {"append",no_argument,0,'A'},
      {"apn",no_argument,0,'A'},
      {"bnr",required_argument,0,'b'},
      {"binary",required_argument,0,'b'},
      {"binary-file",required_argument,0,'b'},
      {"fl_bnr",required_argument,0,'b'},
      {"coords",no_argument,0,'c'},
      {"crd",no_argument,0,'c'},
      {"no-coords",no_argument,0,'C'},
      {"no-crd",no_argument,0,'C'},
      {"data",required_argument,0,'H'},
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
      {"hieronymus",no_argument,0,'H'}, /* fxm: need better mnemonic for -H */
      {"dfl_lvl",required_argument,0,'L'}, /* [enm] Deflate level */
      {"deflate",required_argument,0,'L'}, /* [enm] Deflate level */
      {"lcl",required_argument,0,'l'},
      {"local",required_argument,0,'l'},
      {"metadata",no_argument,0,'m'},
      {"mtd",no_argument,0,'m'},
      {"Metadata",no_argument,0,'M'},
      {"Mtd",no_argument,0,'M'},
      {"overwrite",no_argument,0,'O'},
      {"ovr",no_argument,0,'O'},
      {"output",required_argument,0,'o'},
      {"fl_out",required_argument,0,'o'},
      {"print",required_argument,0,'P'},
      {"prn",required_argument,0,'P'},
      {"path",required_argument,0,'p'},
      {"quiet",no_argument,0,'q'},
      {"retain",no_argument,0,'R'},
      {"rtn",no_argument,0,'R'},
      {"revision",no_argument,0,'r'},
      {"sng_fmt",required_argument,0,'s'},
      {"string",required_argument,0,'s'},
      {"units",no_argument,0,'u'},
      {"variable",required_argument,0,'v'},
      {"auxiliary",required_argument,0,'X'},
      {"exclude",no_argument,0,'x'},
      {"xcl",no_argument,0,'x'},
      {"help",no_argument,0,'?'},
      {"hlp",no_argument,0,'?'},
      {"get_grp_info",no_argument,0,0},
      {"get_prg_info",no_argument,0,0},
      {"get_file_info",no_argument,0,0},
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
      if(!strcmp(opt_crr,"mk_rec_dmn")){
        rec_dmn_nm=strdup(optarg);
      } /* endif mk_rec_dmn */
      if(!strcmp(opt_crr,"cmp") || !strcmp(opt_crr,"compiler")){
        (void)fprintf(stdout,"%s\n",nco_cmp_get());
        nco_exit(EXIT_SUCCESS);
      } /* endif "cmp" */
      if(!strcmp(opt_crr,"cln") || !strcmp(opt_crr,"mmr_cln") || !strcmp(opt_crr,"clean")) flg_cln=True; /* [flg] Clean memory prior to exit */
      if(!strcmp(opt_crr,"drt") || !strcmp(opt_crr,"mmr_drt") || !strcmp(opt_crr,"dirty")) flg_cln=False; /* [flg] Clean memory prior to exit */
      if(!strcmp(opt_crr,"fix_rec_dmn") || !strcmp(opt_crr,"no_rec_dmn")) FIX_REC_DMN=True; /* [flg] Fix record dimension */
      if(!strcmp(opt_crr,"fl_fmt") || !strcmp(opt_crr,"file_format")) rcd=nco_create_mode_prs(optarg,&fl_out_fmt);
      if(!strcmp(opt_crr,"get_grp_info") || !strcmp(opt_crr,"grp_info_get")){
        GET_GRP_INFO=True;
      } /* endif "get_grp_info" */
      if(!strcmp(opt_crr,"get_file_info")){
        GET_FILE_INFO=True;
      } /* endif "get_file_info" */
       if(!strcmp(opt_crr,"get_prg_info")){
        GET_PRG_INFO=True;
      } /* endif "get_prg_info" */
      if(!strcmp(opt_crr,"hdr_pad") || !strcmp(opt_crr,"header_pad")){
        hdr_pad=strtoul(optarg,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
        if(*sng_cnv_rcd) nco_sng_cnv_err(optarg,"strtoul",sng_cnv_rcd);
      } /* endif "hdr_pad" */
      if(!strcmp(opt_crr,"lbr") || !strcmp(opt_crr,"library")){
        (void)nco_lbr_vrs_prn();
        nco_exit(EXIT_SUCCESS);
      } /* endif "lbr" */
      if(!strcmp(opt_crr,"mpi_implementation")){
        (void)fprintf(stdout,"%s\n",nco_mpi_get());
        nco_exit(EXIT_SUCCESS);
      } /* endif "mpi" */
      if(!strcmp(opt_crr,"md5_digest")){
        MD5_DIGEST=True;
        if(dbg_lvl >= nco_dbg_std) (void)fprintf(stderr,"%s: INFO Will perform MD5 digests of input and output hyperslabs\n",prg_nm_get());
      } /* endif "md5" */
      if(!strcmp(opt_crr,"msa_usr_rdr")) MSA_USR_RDR=True; /* [flg] Multi-Slab Algorithm returns hyperslabs in user-specified order */
      if(!strcmp(opt_crr,"no_blank") || !strcmp(opt_crr,"no-blank") || !strcmp(opt_crr,"noblank")) PRN_MSS_VAL_BLANK=!PRN_MSS_VAL_BLANK;
      if(!strcmp(opt_crr,"no_clb") || !strcmp(opt_crr,"no-clobber") || !strcmp(opt_crr,"no_clobber") || !strcmp(opt_crr,"noclobber")) FORCE_NOCLOBBER=!FORCE_NOCLOBBER;
      if(!strcmp(opt_crr,"no_dmn_var_nm") || !strcmp(opt_crr,"no_nm_prn")) PRN_DMN_VAR_NM=False;
      if(!strcmp(opt_crr,"tst_udunits")){ 
        char *cp;
        char **args;
        double crr_val;
#ifndef ENABLE_UDUNITS
        (void)fprintf(stdout,"%s: Build NCO with UDUnits support to use this option\n",prg_nm_get());
        nco_exit(EXIT_FAILURE);
#endif /* !ENABLE_UDUNITS */
        cp=strdup(optarg); 
        args=nco_lst_prs_1D(cp,",",&lmt_nbr);         
        nco_cln_clc_org(args[0],args[1],(lmt_nbr > 2 ? nco_cln_get_cln_typ(args[2]) : cln_nil),&crr_val);        
        (void)fprintf(stdout,"Units in=%s, units out=%s, difference=%f\n",args[0],args[1],crr_val);
        if(cp) cp=(char *)nco_free(cp);
        nco_exit(EXIT_SUCCESS);
      } /* endif "tst_udunits" */
      if(!strcmp(opt_crr,"ram_all") || !strcmp(opt_crr,"create_ram") || !strcmp(opt_crr,"diskless_all")) RAM_CREATE=True; /* [flg] Open (netCDF3) file(s) in RAM */
      if(!strcmp(opt_crr,"ram_all") || !strcmp(opt_crr,"open_ram") || !strcmp(opt_crr,"diskless_all")) RAM_OPEN=True; /* [flg] Create file in RAM */
      if(!strcmp(opt_crr,"secret") || !strcmp(opt_crr,"scr") || !strcmp(opt_crr,"shh")){
        (void)fprintf(stdout,"Hidden/unsupported NCO options:\nCompiler used\t\t--cmp, --compiler\nHidden functions\t--scr, --ssh, --secret\nLibrary used\t\t--lbr, --library\nMemory clean\t\t--mmr_cln, --cln, --clean\nMemory dirty\t\t--mmr_drt, --drt, --dirty\nGroup sub-setting\t-g\nMD5 digests\t\t--md5_digest\nMPI implementation\t--mpi_implementation\nMSA user order\t\t--msa_usr_rdr\nNameless printing\t--no_nm_prn, --no_dmn_var_nm\nNo-clobber files\t--no_clb, --no-clobber\nPseudonym\t\t--pseudonym, -Y (ncra only)\nTest UDUnits\t\t--tst_udunits,'units_in','units_out','cln_sng'? \nVersion\t\t\t--vrs, --version\n\n");
        nco_exit(EXIT_SUCCESS);
      } /* endif "shh" */
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
    case 'a': /* Toggle ALPHABETIZE_OUTPUT */
      ALPHABETIZE_OUTPUT=!ALPHABETIZE_OUTPUT;
      break;
    case 'b': /* Set file for binary output */
      fl_bnr=(char *)strdup(optarg);
      break;
    case 'C': /* Extract all coordinates associated with extracted variables? */
      EXTRACT_ASSOCIATED_COORDINATES=!EXTRACT_ASSOCIATED_COORDINATES;
      break;
    case 'c': /* Add all coordinates to extraction list? */
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
      gpe=nco_gpe_prs_arg(optarg);
      fl_out_fmt=NC_FORMAT_NETCDF4; 
      break;
    case 'g': /* Copy group argument for later processing */
      /* Replace commas with hashes when within braces (convert back later) */
      optarg_lcl=(char *)strdup(optarg);
      (void)nco_rx_comma2hash(optarg_lcl);
      grp_lst_in=nco_lst_prs_2D(optarg_lcl,",",&grp_lst_in_nbr);
      optarg_lcl=(char *)nco_free(optarg_lcl);
      grp_nbr=grp_lst_in_nbr;
      break;
    case 'H': /* Toggle printing data to screen */
      PRN_VAR_DATA_TGL=True;
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
    case 'm': /* Toggle printing variable metadata to screen */
      PRN_VAR_METADATA_TGL=True;
      break;
    case 'M': /* Toggle printing global metadata to screen */
      PRN_GLB_METADATA_TGL=True;
      break;
    case 'O': /* Toggle FORCE_OVERWRITE */
      FORCE_OVERWRITE=!FORCE_OVERWRITE;
      break;
    case 'o': /* Name of output file */
      fl_out=(char *)strdup(optarg);
      break;
    case 'P': /* Print data to screen, maximal verbosity */
      PRN_VRB=True;
      EXTRACT_ASSOCIATED_COORDINATES=!EXTRACT_ASSOCIATED_COORDINATES;
      break;
    case 'p': /* Common file path */
      fl_pth=(char *)strdup(optarg);
      break;
    case 'q': /* [flg] Turn off all printing to screen */
      PRN_QUIET=True; /* [flg] Turn off all printing to screen */
      break;
    case 'Q': /* Turn off printing of dimension indices and coordinate values */
      PRN_DMN_IDX_CRD_VAL=!PRN_DMN_IDX_CRD_VAL;
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
    case 's': /* User specified delimiter string for printed output */
      dlm_sng=(char *)strdup(optarg);
      break;
    case 'u': /* Toggle printing dimensional units */
      PRN_DMN_UNITS_TGL=True;
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
    case 'x': /* Exclude rather than extract groups and variables specified with -v */
      EXCLUDE_INPUT_LIST=True;
      break;
    case 'z': /* Print absolute path of all input variables then exit */
      GET_LIST=True;
      break;
    case '?': /* Print proper usage */
      (void)nco_usg_prn();
      nco_exit(EXIT_FAILURE);
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

  /* Get program info for regressions tests */
  if(GET_PRG_INFO) nco_get_prg_info();

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
  /* Open file using appropriate buffer size hints and verbosity */
  if(RAM_OPEN) md_open=NC_NOWRITE|NC_DISKLESS; else md_open=NC_NOWRITE;
  rcd+=nco_fl_open(fl_in,md_open,&bfr_sz_hnt,&in_id);

  /* Get file format */
  (void)nco_inq_format(in_id,&fl_in_fmt);

  /* Get objects in file */
  trv_tbl_init(&trv_tbl);
  rcd+=nco_grp_itr(in_id,rth,trv_tbl);

   /* Get number of variables, dimensions, and global attributes in file, file format */
  (void)trv_tbl_inq(&nbr_glb_att,&nbr_dmn_fl,&nbr_var_fl,&nbr_grp_fl,trv_tbl);
  (void)nco_inq(in_id,NULL,NULL,NULL,&rec_dmn_id);

  /* Basic checks for ENABLE_NETCDF4 */
  /* Make output and input files consanguinous */
   if(fl_out && fl_out_fmt == NCO_FORMAT_UNDEFINED) fl_out_fmt=fl_in_fmt;
#ifndef ENABLE_NETCDF4
  if(fl_out_fmt == NC_FORMAT_NETCDF4 || fl_out_fmt == NC_FORMAT_NETCDF4_CLASSIC ){
    (void)fprintf(stdout,"%s: ERROR Requested netCDF4-format output file but NCO was not built with netCDF4 support\n",prg_nm_get());
    (void)fprintf(stdout,"%s: HINT: Obtain or build a netCDF4-enabled version of NCO.  Try, e.g., ./configure --enable-netcdf4 ...;make;make install\n",prg_nm_get());
    nco_exit(EXIT_FAILURE);
  } /* netCDF4 */
#endif /* ENABLE_NETCDF4 */

  /* Check -v and -g input names and create extraction list */
  nco_mk_xtr(grp_lst_in,grp_nbr,var_lst_in,xtr_nbr,EXTRACT_ALL_COORDINATES,GRP_VAR_UNN,trv_tbl);

  /* Process -z option if requested */ 
  if(GET_LIST){ 
    if(ALPHABETIZE_OUTPUT) trv_tbl_srt(trv_tbl);
    trv_tbl_prn(trv_tbl);
    goto close_and_free; 
  } /* end GET_LIST */ 

  /* Process --get_grp_info option if requested */ 
  if(GET_GRP_INFO){ 
    nco_prt_grp_trv(in_id,trv_tbl);
    goto close_and_free; 
  } /* end GET_GRP_INFO */

  /* Process --get_file_info option if requested */ 
  if(GET_FILE_INFO){ 
    (void)fprintf(stderr,"%s: INFO reports file information\n",prg_nm_get());
    (void)fprintf(stdout,"%d subgroups, %d dimensions, %d attributes, %d variables\n",nbr_grp_fl,nbr_dmn_fl,nbr_glb_att,nbr_var_fl); 
    goto close_and_free; 
  } /* end GET_FILE_INFO */

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
  
  /* Get record dimension name if not already defined with --mk_rec_dmn (and --fix_rec_dmn is false) */
  if(!FIX_REC_DMN && !rec_dmn_nm && (rec_dmn_id != NCO_REC_DMN_UNDEFINED)){ 
    rec_dmn_nm=(char *)nco_malloc(NC_MAX_NAME*(sizeof(char)));
    (void)nco_inq_dimname(in_id,rec_dmn_id,rec_dmn_nm);
  } /* endif rec_dmn_nm */

  /* Print extraction list in verbose mode */
  if(dbg_lvl_get() == nco_dbg_crr) (void)trv_tbl_prn_xtr(trv_tbl);

  /* Change included variables to excluded variables */
  if(EXCLUDE_INPUT_LIST) (void)nco_var_lst_xcl_trv2(trv_tbl);

  /* Add all coordinate variables to extraction list */
  if(EXTRACT_ALL_COORDINATES) (void)nco_xtr_crd_add(in_id,trv_tbl);

  /* Extract coordinates associated with extracted variables */
  /* fxm: fix netCDF4 code path bug */
  if(EXTRACT_ASSOCIATED_COORDINATES) (void)nco_xtr_crd_ass_add_trv(in_id,trv_tbl);

  /* Is this a CCM/CCSM/CF-format history tape? */
  CNV_CCM_CCSM_CF=nco_cnv_ccm_ccsm_cf_inq(in_id);
  if(CNV_CCM_CCSM_CF && EXTRACT_ASSOCIATED_COORDINATES){
    /* Implement CF "coordinates" and "bounds" conventions */
    (void)nco_xtr_cf_trv(in_id,"coordinates",trv_tbl);
    (void)nco_xtr_cf_trv(in_id,"bounds",trv_tbl);
  } /* CNV_CCM_CCSM_CF */

   if(ALPHABETIZE_OUTPUT) trv_tbl_srt(trv_tbl);

   /* We now have final list of variables to extract. Phew. */

  /* Find coordinate/dimension values associated with user-specified limits
     NB: nco_lmt_evl() with same nc_id contains OpenMP critical region */
  if(lmt_nbr) (void)nco_lmt_evl_trv(in_id,lmt_nbr,lmt,FORTRAN_IDX_CNV,trv_tbl);    

  /* Place all dimensions in lmt_all_lst */
  lmt_all_lst=(lmt_all_sct **)nco_malloc(nbr_dmn_fl*sizeof(lmt_all_sct *));

  /* Initialize lmt_all_sct's */ 
  (void)nco_msa_lmt_all_int_trv(in_id,MSA_USR_RDR,lmt_all_lst,nbr_dmn_fl,lmt,lmt_nbr,trv_tbl);
  
  if(fl_out){
    /* Copy everything (all data and metadata) to output file by default */
    if(PRN_VAR_DATA_TGL) PRN_VAR_DATA=False; else PRN_VAR_DATA=True;
    if(PRN_VAR_METADATA_TGL) PRN_VAR_METADATA=False; else PRN_VAR_METADATA=True;
    if(PRN_GLB_METADATA_TGL) PRN_GLB_METADATA=False; else PRN_GLB_METADATA=True;
    if(FORCE_APPEND){
      /* When appending, do not copy global metadata by default */
      if(var_lst_in) PRN_GLB_METADATA=False; else PRN_GLB_METADATA=True;
      if(PRN_GLB_METADATA_TGL) PRN_GLB_METADATA=!PRN_GLB_METADATA;
    } /* !FORCE_APPEND */
  }else{ /* !fl_out */
    /* Only input file is specified, so some printing should occur */
    if(PRN_VRB || (!PRN_VAR_DATA_TGL && !PRN_VAR_METADATA_TGL && !PRN_GLB_METADATA_TGL)){
      /* Verbose printing simply means assume user wants deluxe frills by default */
      if(PRN_DMN_UNITS_TGL) PRN_DMN_UNITS=False; else PRN_DMN_UNITS=True;
      if(PRN_VAR_DATA_TGL) PRN_VAR_DATA=False; else PRN_VAR_DATA=True;
      if(PRN_VAR_METADATA_TGL) PRN_VAR_METADATA=False; else PRN_VAR_METADATA=True;
      /* Assume user wants global metadata unless variable extraction is invoked */
      if(var_lst_in == NULL) PRN_GLB_METADATA=True;
      if(PRN_GLB_METADATA_TGL) PRN_GLB_METADATA=!PRN_GLB_METADATA;
    }else{ /* end if PRN_VRB */
      /* Default is to print data and metadata to screen if output file is not specified */
      if(PRN_DMN_UNITS_TGL) PRN_DMN_UNITS=True; else PRN_DMN_UNITS=False;
      if(PRN_VAR_DATA_TGL) PRN_VAR_DATA=True; else PRN_VAR_DATA=False;
      if(PRN_VAR_METADATA_TGL) PRN_VAR_METADATA=True; else PRN_VAR_METADATA=False;
      if(PRN_GLB_METADATA_TGL) PRN_GLB_METADATA=True; else PRN_GLB_METADATA=False;
    } /* !PRN_VRB */  
    /* PRN_QUIET turns off all printing to screen */
    if(PRN_QUIET) PRN_VAR_DATA=PRN_VAR_METADATA=PRN_GLB_METADATA=False;
  } /* !fl_out */  

  if(fl_bnr && !fl_out){
    /* Native binary files depend on writing netCDF file to enter generic I/O logic */
    (void)fprintf(stdout,"%s: ERROR Native binary files cannot be written unless netCDF output filename also specified. HINT: Repeat command with dummy netCDF file specified for output file (e.g., -o foo.nc)\n",prg_nm_get());
    nco_exit(EXIT_FAILURE);
  } /* endif fl_bnr */
    
  if(fl_out){
    /* Output file was specified so PRN_ tokens refer to (meta)data copying */
    int out_id;
    
    /* Make output and input files consanguinous */
    if(fl_out_fmt == NCO_FORMAT_UNDEFINED) fl_out_fmt=fl_in_fmt;

    /* Verify output file format supports requested actions */
    (void)nco_fl_fmt_vet(fl_out_fmt,cnk_nbr,dfl_lvl);

    /* Open output file */
    fl_out_tmp=nco_fl_out_open(fl_out,FORCE_APPEND,FORCE_OVERWRITE,fl_out_fmt,&bfr_sz_hnt,RAM_CREATE,RAM_OPEN,WRT_TMP_FL,&out_id);
    
    /* Copy global attributes */
    if(PRN_GLB_METADATA)(void)nco_att_cpy(in_id,out_id,NC_GLOBAL,NC_GLOBAL,(nco_bool)True);

    if(gpe){
#ifndef ENABLE_NETCDF4
      (void)fprintf(stderr,"ERROR Group Path Edit requires netCDF4 capabilities. HINT: Rebuild NCO with netCDF4 enabled.\n");
      nco_exit(EXIT_FAILURE);
#endif /* ENABLE_NETCDF4 */
      if(dbg_lvl >= nco_dbg_fl) (void)fprintf(stderr,"%s: INFO Group Path Edit (GPE) feature enabled\n",prg_nm_get());
      if(fl_out_fmt != NC_FORMAT_NETCDF4){
        (void)fprintf(stderr,"%s: ERROR Group Path Edit requires requires netCDF4 output format but user explicitly requested format = %s\n",prg_nm_get(),nco_fmt_sng(fl_out_fmt));
        nco_exit(EXIT_FAILURE);
      } /* endif err */
    } /* !gpe */

    /* Define extracted groups, variables, and attributes in output file */
    nco_xtr_dfn(in_id,out_id,&cnk_map,&cnk_plc,cnk_sz_scl,cnk,cnk_nbr,dfl_lvl,gpe,lmt_nbr,lmt_all_lst,nbr_dmn_fl,PRN_VAR_METADATA,trv_tbl);

    /* Catenate timestamped command line to "history" global attribute */
    if(HISTORY_APPEND) (void)nco_hst_att_cat(out_id,cmd_ln);
    if(HISTORY_APPEND) (void)nco_vrs_att_cat(out_id);

    /* Turn off default filling behavior to enhance efficiency */
    nco_set_fill(out_id,NC_NOFILL,&fll_md_old);

    /* Take output file out of define mode */
    if(hdr_pad == 0UL){
      (void)nco_enddef(out_id);
    }else{
      (void)nco__enddef(out_id,hdr_pad);
      if(dbg_lvl >= nco_dbg_scl) (void)fprintf(stderr,"%s: INFO Padding header with %lu extra bytes\n",prg_nm_get(),(unsigned long)hdr_pad);
    } /* hdr_pad */

    /* [fnc] Open unformatted binary data file for writing */
    if(fl_bnr) fp_bnr=nco_bnr_open(fl_bnr);

    /* Timestamp end of metadata setup and disk layout */
    rcd+=nco_ddra((char *)NULL,(char *)NULL,&ddra_info);
    ddra_info.tmr_flg=nco_tmr_rgl;

    /* Write extracted data to output file */
    nco_xtr_wrt(in_id,out_id,lmt_nbr,lmt_all_lst,nbr_dmn_fl,fp_bnr,MD5_DIGEST,trv_tbl);

    /* [fnc] Close unformatted binary data file */
    if(fp_bnr) (void)nco_bnr_close(fp_bnr,fl_bnr);

    /* Close output file and move it from temporary to permanent location */
    (void)nco_fl_out_cls(fl_out,fl_out_tmp,out_id);
    
  }else{ /* !fl_out */
    /* No output file was specified so PRN_ tokens refer to screen printing */
    if(PRN_GLB_METADATA){
      (void)fprintf(stdout,"Opened file %s: dimensions = %i, variables = %i, global atts. = %i, type = %s\n",fl_in,nbr_dmn_fl,nbr_var_fl,nbr_glb_att,nco_fmt_sng(fl_in_fmt));
      if(fl_in_fmt == NC_FORMAT_NETCDF4 || fl_in_fmt == NC_FORMAT_NETCDF4_CLASSIC){
        /* Print global attributes recursive */
        (void)nco_prn_att_trv(in_id,trv_tbl);
      }else{
        if(rec_dmn_id != NCO_REC_DMN_UNDEFINED){
          char dmn_nm[NC_MAX_NAME]; 
          long rec_dmn_sz;
          (void)nco_inq_dim(in_id,rec_dmn_id,dmn_nm,&rec_dmn_sz);
          (void)fprintf(stdout,"Record dimension: name = %s, size = %li\n\n",dmn_nm,rec_dmn_sz);
        }else{ /* NCO_REC_DMN_UNDEFINED */
          (void)fprintf(stdout,"Record dimension: None\n\n");
        } /* NCO_REC_DMN_UNDEFINED */
        /* Print global attributes */
        (void)nco_prn_att(in_id,in_id,NC_GLOBAL);
      } /* IS_NETCDF4 */
    } /* endif PRN_GLB_METADATA */
    
    if(PRN_VAR_METADATA) (void)nco_prn_var_def_trv2(in_id,trv_tbl);

    if(PRN_VAR_DATA) (void)nco_prn_var_val_trv2(in_id,lmt_all_lst,nbr_dmn_fl,dlm_sng,FORTRAN_IDX_CNV,MD5_DIGEST,PRN_DMN_UNITS,PRN_DMN_IDX_CRD_VAL,PRN_DMN_VAR_NM,PRN_MSS_VAL_BLANK,trv_tbl);
  } /* !fl_out */
  
 close_and_free: /* goto close_and_free */

 /* Close input netCDF file */
 nco_close(in_id);

 /* Remove local copy of file */
 if(FL_RTR_RMT_LCN && RM_RMT_FL_PST_PRC) (void)nco_fl_rm(fl_in);

 /* Clean memory unless dirty memory allowed */
 if(flg_cln){
   /* ncks-specific memory */
   if(fl_bnr) fl_bnr=(char *)nco_free(fl_bnr);
   if(rec_dmn_nm) rec_dmn_nm=(char *)nco_free(rec_dmn_nm);

   /* free lmt[] NB: is now referenced within lmt_all_lst[idx] */
   for(idx=0;idx<nbr_dmn_fl;idx++)
     for(jdx=0;jdx<lmt_all_lst[idx]->lmt_dmn_nbr;jdx++)
       lmt_all_lst[idx]->lmt_dmn[jdx]=nco_lmt_free(lmt_all_lst[idx]->lmt_dmn[jdx]);

   lmt=(lmt_sct **)nco_free(lmt); 
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
   /* Free lists of strings */
   if(fl_lst_in && fl_lst_abb == NULL) fl_lst_in=nco_sng_lst_free(fl_lst_in,fl_nbr); 
   if(fl_lst_in && fl_lst_abb) fl_lst_in=nco_sng_lst_free(fl_lst_in,1);
   if(fl_lst_abb) fl_lst_abb=nco_sng_lst_free(fl_lst_abb,abb_arg_nbr);
   if(grp_lst_in_nbr > 0) grp_lst_in=nco_sng_lst_free(grp_lst_in,grp_lst_in_nbr);
   if(var_lst_in_nbr > 0) var_lst_in=nco_sng_lst_free(var_lst_in,var_lst_in_nbr);
   /* Free limits */
   for(idx=0;idx<lmt_nbr;idx++) lmt_arg[idx]=(char *)nco_free(lmt_arg[idx]);
   /* NB: lmt[idx] was free()'d earlier
   if(lmt_nbr > 0) lmt=nco_lmt_lst_free(lmt,lmt_nbr); */
   for(idx=0;idx<aux_nbr;idx++) aux_arg[idx]=(char *)nco_free(aux_arg[idx]);
   if(aux_nbr > 0) aux=(lmt_sct **)nco_free(aux);
   /* Free chunking information */
   for(idx=0;idx<cnk_nbr;idx++) cnk_arg[idx]=(char *)nco_free(cnk_arg[idx]);
   if(cnk_nbr > 0) cnk=nco_cnk_lst_free(cnk,cnk_nbr);
 } /* !flg_cln */

 trv_tbl_free(trv_tbl);
 if(gpe) gpe=(gpe_sct *)nco_gpe_free(gpe);

 /* End timer */ 
 ddra_info.tmr_flg=nco_tmr_end; /* [enm] Timer flag */
 rcd+=nco_ddra((char *)NULL,(char *)NULL,&ddra_info);
 if(rcd != NC_NOERR) nco_err_exit(rcd,"main");
 nco_exit_gracefully();
 return EXIT_SUCCESS;

} /* end main() */
