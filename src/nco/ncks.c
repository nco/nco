/* $Header: /data/zender/nco_20150216/nco/src/nco/ncks.c,v 1.251 2010-03-30 04:08:06 zender Exp $ */

/* ncks -- netCDF Kitchen Sink */

/* Purpose: Extract (subsets of) variables from a netCDF file 
   Print them to screen, or copy them to a new file, or both */

/* Copyright (C) 1995--2010 Charlie Zender

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
   ncks in.nc 
   ncks -v one in.nc 
   ncks in.nc ~/foo.nc
   ncks -O --fl_fmt=netcdf4 ~/nco/data/in.nc ~/foo.nc
   ncks -v one in.nc ~/foo.nc
   ncks -p /ZENDER/tmp -l /data/zender/tmp h0001.nc ~/foo.nc
   ncks -s "%+16.10f\n" -H -C -v three_dmn_var in.nc
   ncks -H -v fl_nm,fl_nm_arr ~/nco/data/in.nc
   ncks -H -C -u -d wvl,'0.4 micron','0.7 micron' -v wvl ~/nco/data/in.nc
   ncks -H -d fl_dim,1 -d char_dim,6,12 -v fl_nm,fl_nm_arr ~/nco/data/in.nc
   ncks -H -m -v char_var_nul,char_var_space,char_var_multinul ~/nco/data/in.nc
   ncks -H -C -v three_dmn_rec_var -d time,,,2 in.nc
   ncks -H -C -v lon -d lon,3,1 in.nc */

#ifdef HAVE_CONFIG_H
#include <config.h> /* Autotools tokens */
#endif /* !HAVE_CONFIG_H */

/* Standard C headers */
#include <stdio.h> /* stderr, FILE, NULL, etc. */
#include <stdlib.h> /* atof, atoi, malloc, getopt */
#include <string.h> /* strcmp. . . */
#include <sys/stat.h> /* stat() */
#include <time.h> /* machine time */
#include <unistd.h> /* all sorts of POSIX stuff */
#ifndef HAVE_GETOPT_LONG
#include "nco_getopt.h"
#else /* !NEED_GETOPT_LONG */ 
#ifdef HAVE_GETOPT_H
#include <getopt.h>
#endif /* !HAVE_GETOPT_H */ 
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
  nco_bool EXCLUDE_INPUT_LIST=False; /* Option c */
  nco_bool EXTRACT_ALL_COORDINATES=False; /* Option c */
  nco_bool EXTRACT_ASSOCIATED_COORDINATES=True; /* Option C */
  nco_bool FILE_RETRIEVED_FROM_REMOTE_LOCATION;
  nco_bool FIX_REC_DMN=False; /* [flg] Fix record dimension */
  nco_bool FL_LST_IN_FROM_STDIN=False; /* [flg] fl_lst_in comes from stdin */
  nco_bool FORCE_APPEND=False; /* Option A */
  nco_bool FORCE_NOCLOBBER=False; /* Option no-clobber */
  nco_bool FORCE_OVERWRITE=False; /* Option O */
  nco_bool FORTRAN_IDX_CNV=False; /* Option F */
  nco_bool HISTORY_APPEND=True; /* Option h */
  nco_bool MSA_USR_RDR=False; /* [flg] Multi-slabbing algorithm leaves hyperslabs in user order */
  nco_bool NCO_BNR_WRT=False; /* [flg] Write binary file */
  nco_bool PRN_DMN_IDX_CRD_VAL=True; /* [flg] Print leading dimension/coordinate indices/values Option Q */
  nco_bool PRN_DMN_UNITS=False; /* [flg] Print dimensional units Option u */
  nco_bool PRN_DMN_VAR_NM=True; /* [flg] Print dimension/variable names */
  nco_bool PRN_DMN_UNITS_TGL=False; /* [flg] Toggle print dimensional units Option u */
  nco_bool PRN_GLB_METADATA=False; /* [flg] Print global metadata */
  nco_bool PRN_GLB_METADATA_TGL=False; /* [flg] Toggle print global metadata Option M */
  nco_bool PRN_QUIET=False; /* [flg] Turn off all printing to screen */
  nco_bool PRN_VAR_DATA=False; /* [flg] Print variable data */
  nco_bool PRN_VAR_DATA_TGL=False; /* [flg] Toggle print variable data Option H */
  nco_bool PRN_VAR_METADATA=False; /* [flg] Print variable metadata */
  nco_bool PRN_VAR_METADATA_TGL=False; /* [flg] Toggle print variable metadata Option m */
  nco_bool PRN_VRB=False; /* [flg] Print data and metadata by default */
  nco_bool REMOVE_REMOTE_FILES_AFTER_PROCESSING=True; /* Option R */
  nco_bool flg_cln=False; /* [flg] Clean memory prior to exit */

  char **fl_lst_abb=NULL; /* Option a */
  char **fl_lst_in;
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

  const char * const CVS_Id="$Id: ncks.c,v 1.251 2010-03-30 04:08:06 zender Exp $"; 
  const char * const CVS_Revision="$Revision: 1.251 $";
  const char * const opt_sht_lst="346aABb:CcD:d:FHhL:l:MmOo:Pp:qQrRs:uv:X:x-:";

  cnk_sct **cnk=NULL_CEWI;

#if defined(__cplusplus) || defined(PGI_CC)
  ddra_info_sct ddra_info;
  ddra_info.flg_ddra=False;
#else /* !__cplusplus */
  ddra_info_sct ddra_info={.flg_ddra=False};
#endif /* !__cplusplus */

  extern char *optarg;
  extern int optind;
  
  FILE *fp_bnr=NULL_CEWI; /* [fl] Unformatted binary output file handle */

  int abb_arg_nbr=0;
  int aux_nbr=0; /* [nbr] Number of auxiliary coordinate hyperslabs specified */
  int cnk_map=nco_cnk_map_nil; /* [enm] Chunking map */
  int cnk_nbr=0; /* [nbr] Number of chunk sizes */
  int cnk_plc=nco_cnk_plc_nil; /* [enm] Chunking policy */
  int dfl_lvl=0; /* [enm] Deflate level */
  int fl_nbr=0;
  int fl_in_fmt; /* [enm] Input file format */
  int fl_out_fmt=NCO_FORMAT_UNDEFINED; /* [enm] Output file format */
  int fll_md_old; /* [enm] Old fill mode */
  int glb_att_nbr;
  int idx;
  int in_id;  
  int jdx;
  int lmt_nbr=0; /* Option d. NB: lmt_nbr gets incremented */
  int nbr_dmn_fl;
  int nbr_var_fl;
  int nbr_xtr=0; /* nbr_xtr will not otherwise be set for -c with no -v */
  int opt;
  int rcd=NC_NOERR; /* [rcd] Return code */
  int rec_dmn_id=NCO_REC_DMN_UNDEFINED;
  int var_lst_in_nbr=0;
    
  lmt_sct **aux=NULL_CEWI; /* Auxiliary coordinate limits */
  lmt_sct **lmt=NULL_CEWI;
  lmt_all_sct **lmt_all_lst; /* List of *lmt_all structures */

  nm_id_sct *xtr_lst=NULL; /* xtr_lst may be alloc()'d from NULL with -c option */

  size_t cnk_sz_scl=0UL; /* [nbr] Chunk size scalar */
  size_t hdr_pad=0UL; /* [B] Pad at end of header section */

  static struct option opt_lng[]=
    { /* Structure ordered by short option key if possible */
      /* Long options with no argument, no short option counterpart */
      {"cln",no_argument,0,0}, /* [flg] Clean memory prior to exit */
      {"clean",no_argument,0,0}, /* [flg] Clean memory prior to exit */
      {"mmr_cln",no_argument,0,0}, /* [flg] Clean memory prior to exit */
      {"drt",no_argument,0,0}, /* [flg] Allow dirty memory on exit */
      {"dirty",no_argument,0,0}, /* [flg] Allow dirty memory on exit */
      {"mmr_drt",no_argument,0,0}, /* [flg] Allow dirty memory on exit */
      {"cmp",no_argument,0,0},
      {"compiler",no_argument,0,0},
      {"fix_rec_dmn",no_argument,0,0}, /* [flg] Fix record dimension */
      {"no_rec_dmn",no_argument,0,0}, /* [flg] Fix record dimension */
      {"lbr",no_argument,0,0},
      {"library",no_argument,0,0},
      {"mpi_implementation",no_argument,0,0},
      {"msa_usr_rdr",no_argument,0,0}, /* [flg] Multi-slabbing algorithm leaves hyperslabs in user order */
      {"no_clb",no_argument,0,0},
      {"noclobber",no_argument,0,0},
      {"no-clobber",no_argument,0,0},
      {"no_clobber",no_argument,0,0},
      {"no_dmn_var_nm",no_argument,0,0}, /* [flg] Print dimension/variable names */
      {"no_nm_prn",no_argument,0,0}, /* [flg] Print dimension/variable names */
      {"secret",no_argument,0,0},
      {"shh",no_argument,0,0},
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
      {"bnr",no_argument,0,'B'},
      {"binary",no_argument,0,'B'},
      {"binary-file",required_argument,0,'b'},
      {"fl_bnr",required_argument,0,'b'},
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

  fl_bnr=(char *)strdup("ncks.bnr");
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
	cnk_sz_scl=strtoul(optarg,(char **)NULL,10);
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
      if(!strcmp(opt_crr,"hdr_pad") || !strcmp(opt_crr,"header_pad")) hdr_pad=strtoul(optarg,(char **)NULL,10);
      if(!strcmp(opt_crr,"lbr") || !strcmp(opt_crr,"library")){
	(void)nco_lbr_vrs_prn();
	nco_exit(EXIT_SUCCESS);
      } /* endif "lbr" */
      if(!strcmp(opt_crr,"mpi_implementation")){
	(void)fprintf(stdout,"%s\n",nco_mpi_get());
	nco_exit(EXIT_SUCCESS);
      } /* endif "mpi" */
      if(!strcmp(opt_crr,"msa_usr_rdr")) MSA_USR_RDR=True; /* [flg] Multi-slabbing algorithm leaves hyperslabs in user order */
      if(!strcmp(opt_crr,"no_clb") || !strcmp(opt_crr,"no-clobber") || !strcmp(opt_crr,"no_clobber") || !strcmp(opt_crr,"noclobber")) FORCE_NOCLOBBER=!FORCE_NOCLOBBER;
      if(!strcmp(opt_crr,"no_dmn_var_nm") || !strcmp(opt_crr,"no_nm_prn")) PRN_DMN_VAR_NM=False;
      if(!strcmp(opt_crr,"tst_udunits")){ 
	char *cp;
        char **args;
        double crr_val;
       
        #ifndef ENABLE_UDUNITS
        (void)fprintf(stdout,"%s: To use this option nco must be built with udunits support\n",prg_nm_get());
        nco_exit(EXIT_SUCCESS);
        #endif 
               
        cp=strdup(optarg); 
        args=nco_lst_prs_1D(cp,",",&lmt_nbr);         
        
        nco_cln_clc_org(args[0],args[1],(lmt_nbr>2 ? nco_cln_get_cal_typ(args[2]):cal_void) ,&crr_val);        
        (void)fprintf(stdout,"units in=%s units out=%s difference=%f\n",args[0],args[1],crr_val);

        nco_free(cp);
        nco_exit(EXIT_SUCCESS);

      } /* endif "tst_udunits" */
      if(!strcmp(opt_crr,"secret") || !strcmp(opt_crr,"scr") || !strcmp(opt_crr,"shh")){
	(void)fprintf(stdout,"Hidden/unsupported NCO options:\nCompiler used\t\t--cmp, --compiler\nHidden functions\t--scr, --ssh, --secret\nLibrary used\t\t--lbr, --library\nMemory clean\t\t--mmr_cln, --cln, --clean\nMemory dirty\t\t--mmr_drt, --drt, --dirty\nMPI implementation\t--mpi_implementation\nMSA user order\t\t--msa_usr_rdr\nNameless printing\t--no_nm_prn, --no_dmn_var_nm\nNo-clobber files\t--no_clb, --no-clobber\nTest udunits\t\t--tst_udunits,'units_in','units_out','cal_sng'? \nVersion\t\t\t--vrs, --version\n\n");
	nco_exit(EXIT_SUCCESS);
      } /* endif "shh" */
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
    case 'a': /* Toggle ALPHABETIZE_OUTPUT */
      ALPHABETIZE_OUTPUT=!ALPHABETIZE_OUTPUT;
      break;
    case 'B': /* Set NCO_BNR_WRT */
      NCO_BNR_WRT=True;
      break;
    case 'b': /* Set file for binary output */
      NCO_BNR_WRT=True;
      fl_bnr=(char *)strdup(optarg);
      break;
    case 'C': /* Extract all coordinates associated with extracted variables? */
      EXTRACT_ASSOCIATED_COORDINATES=!EXTRACT_ASSOCIATED_COORDINATES;
      break;
    case 'c': /* Add all coordinates to extraction list? */
      EXTRACT_ALL_COORDINATES=True;
      break;
    case 'D': /* Debugging level. Default is 0. */
      dbg_lvl=(unsigned short)strtol(optarg,(char **)NULL,10);
      break;
    case 'd': /* Copy limit argument for later processing */
      lmt_arg[lmt_nbr]=(char *)strdup(optarg);
      lmt_nbr++;
      break;
    case 'F': /* Toggle index convention. Default is 0-based arrays (C-style). */
      FORTRAN_IDX_CNV=!FORTRAN_IDX_CNV;
      break;
    case 'H': /* Toggle printing data to screen */
      PRN_VAR_DATA_TGL=True;
      break;
    case 'h': /* Toggle appending to history global attribute */
      HISTORY_APPEND=!HISTORY_APPEND;
      break;
    case 'L': /* [enm] Deflate level. Default is 0. */
      dfl_lvl=(int)strtol(optarg,(char **)NULL,10);
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
      REMOVE_REMOTE_FILES_AFTER_PROCESSING=!REMOVE_REMOTE_FILES_AFTER_PROCESSING;
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
      (void)nco_lst_comma2hash(optarg_lcl);
      var_lst_in=nco_lst_prs_2D(optarg_lcl,",",&var_lst_in_nbr);
      optarg_lcl=(char *)nco_free(optarg_lcl);
      nbr_xtr=var_lst_in_nbr;
       break;
    case 'X': /* Copy auxiliary coordinate argument for later processing */
      aux_arg[aux_nbr]=(char *)strdup(optarg);
      aux_nbr++;
      MSA_USR_RDR=True; /* [flg] Multi-slabbing algorithm leaves hyperslabs in user order */      
      break;
    case 'x': /* Exclude rather than extract variables specified with -v */
      EXCLUDE_INPUT_LIST=True;
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
  
  /* Parse filename */
  fl_in=nco_fl_nm_prs(fl_in,0,&fl_nbr,fl_lst_in,abb_arg_nbr,fl_lst_abb,fl_pth);
  /* Make sure file is on local system and is readable or die trying */
  fl_in=nco_fl_mk_lcl(fl_in,fl_pth_lcl,&FILE_RETRIEVED_FROM_REMOTE_LOCATION);
  /* Open file for reading */
  rcd=nco_open(fl_in,NC_NOWRITE,&in_id);

  /* Process auxiliary coordinates */
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
  
  /* Get number of variables, dimensions, and global attributes in file */
  (void)nco_inq(in_id,&nbr_dmn_fl,&nbr_var_fl,&glb_att_nbr,&rec_dmn_id);
  (void)nco_inq_format(in_id,&fl_in_fmt);

  /* Get record dimension name name if not already defined with --mk_rec_dmn (and --fix_rec_dmn is false) */
  if(!FIX_REC_DMN && !rec_dmn_nm && (rec_dmn_id != NCO_REC_DMN_UNDEFINED)){ 
    rec_dmn_nm=(char *)nco_malloc(NC_MAX_NAME*(sizeof(char)));
    (void)nco_inq_dimname(in_id,rec_dmn_id,rec_dmn_nm);
  } /* endif rec_dmn_nm */

  /* Form initial extraction list which may include extended regular expressions */
  xtr_lst=nco_var_lst_mk(in_id,nbr_var_fl,var_lst_in,EXCLUDE_INPUT_LIST,EXTRACT_ALL_COORDINATES,&nbr_xtr);

  /* Change included variables to excluded variables */
  if(EXCLUDE_INPUT_LIST) xtr_lst=nco_var_lst_xcl(in_id,nbr_var_fl,xtr_lst,&nbr_xtr);

  /* Is this an CCM/CCSM/CF-format history tape? */
  CNV_CCM_CCSM_CF=nco_cnv_ccm_ccsm_cf_inq(in_id);

  /* Add all coordinate variables to extraction list */
  if(EXTRACT_ALL_COORDINATES) xtr_lst=nco_var_lst_crd_add(in_id,nbr_dmn_fl,nbr_var_fl,xtr_lst,&nbr_xtr,CNV_CCM_CCSM_CF);

  /* Make sure coordinates associated extracted variables are also on extraction list */
  if(EXTRACT_ASSOCIATED_COORDINATES) xtr_lst=nco_var_lst_crd_ass_add(in_id,xtr_lst,&nbr_xtr,CNV_CCM_CCSM_CF);

  /* Sort extraction list alphabetically or by variable ID */
  if(nbr_xtr > 1) xtr_lst=nco_lst_srt_nm_id(xtr_lst,nbr_xtr,ALPHABETIZE_OUTPUT);
    
  /* We now have final list of variables to extract. Phew. */

  /* Find coordinate/dimension values associated with user-specified limits
     NB: nco_lmt_evl() with same nc_id contains OpenMP critical region */
  for(idx=0;idx<lmt_nbr;idx++) (void)nco_lmt_evl(in_id,lmt[idx],0L,FORTRAN_IDX_CNV);

  /* Place all dimensions in lmt_all_lst */
  lmt_all_lst=(lmt_all_sct **)nco_malloc(nbr_dmn_fl*sizeof(lmt_all_sct *));
  
  /* Initialize lmt_all_sct's */ 
  (void)nco_msa_lmt_all_int(in_id,MSA_USR_RDR,lmt_all_lst,nbr_dmn_fl,lmt,lmt_nbr);
  
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
      
      /* PRN_QUIET turns off all printing to screen */
      if(PRN_QUIET) PRN_VAR_DATA=PRN_VAR_METADATA=PRN_GLB_METADATA=False;
      
    } /* !PRN_VRB */  
  } /* !fl_out */  

  if(NCO_BNR_WRT && !fl_out){
    /* Native binary files depend on writing netCDF file to enter generic I/O logic */
    (void)fprintf(stdout,"%s: ERROR Native binary files cannot be written unless netCDF output filename also specified. HINT: Repeat command with dummy netCDF file specified for output file (e.g., -o foo.nc)\n",prg_nm_get());
    nco_exit(EXIT_FAILURE);
  } /* endif NCO_BNR_WRT */
    
  if(fl_out){
    /* Output file was specified so PRN_ tokens refer to (meta)data copying */
    int out_id;  
    
    /* Make output and input files consanguinous */
    if(fl_out_fmt == NCO_FORMAT_UNDEFINED) fl_out_fmt=fl_in_fmt;

    /* Open output file */
    fl_out_tmp=nco_fl_out_open(fl_out,FORCE_APPEND,FORCE_OVERWRITE,fl_out_fmt,&out_id);
    
    /* Copy global attributes */
    if(PRN_GLB_METADATA) (void)nco_att_cpy(in_id,out_id,NC_GLOBAL,NC_GLOBAL,(nco_bool)True);
    
    /* Catenate timestamped command line to "history" global attribute */
    if(HISTORY_APPEND) (void)nco_hst_att_cat(out_id,cmd_ln);
    if(True) (void)nco_vrs_att_cat(out_id);
    
    for(idx=0;idx<nbr_xtr;idx++){
      int var_out_id;

      /* Define variable in output file */
      if(lmt_nbr > 0) var_out_id=nco_cpy_var_dfn_lmt(in_id,out_id,rec_dmn_nm,xtr_lst[idx].nm,lmt_all_lst,nbr_dmn_fl,dfl_lvl); else var_out_id=nco_cpy_var_dfn(in_id,out_id,rec_dmn_nm,xtr_lst[idx].nm,dfl_lvl);
      /* Copy variable's attributes */
      if(PRN_VAR_METADATA) (void)nco_att_cpy(in_id,out_id,xtr_lst[idx].id,var_out_id,(nco_bool)True);
    } /* end loop over idx */
    /* Set chunksize parameters */
    if(fl_out_fmt == NC_FORMAT_NETCDF4) (void)nco_cnk_sz_set(out_id,lmt_all_lst,nbr_dmn_fl,&cnk_map,&cnk_plc,cnk_sz_scl,cnk,cnk_nbr);
    
    /* Turn off default filling behavior to enhance efficiency */
    rcd=nco_set_fill(out_id,NC_NOFILL,&fll_md_old);
    
    /* Take output file out of define mode */
    if(hdr_pad == 0UL){
      (void)nco_enddef(out_id);
    }else{
      (void)nco__enddef(out_id,hdr_pad);
      if(dbg_lvl >= nco_dbg_scl) (void)fprintf(stderr,"%s: INFO Padding header with %lu extra bytes \n",prg_nm_get(),(unsigned long)hdr_pad);
    } /* hdr_pad */
    
    /* [fnc] Open unformatted binary data file for writing */
    if(NCO_BNR_WRT) fp_bnr=nco_bnr_open(fl_bnr);
    
    /* Timestamp end of metadata setup and disk layout */
    rcd+=nco_ddra((char *)NULL,(char *)NULL,&ddra_info);
    ddra_info.tmr_flg=nco_tmr_rgl;
    
    /* Copy variable data */
    for(idx=0;idx<nbr_xtr;idx++){
      if(dbg_lvl >= nco_dbg_var && !NCO_BNR_WRT) (void)fprintf(stderr,"%s, ",xtr_lst[idx].nm);
      if(dbg_lvl >= nco_dbg_var) (void)fflush(stderr);
      /* Old hyperslab routines */
      /* NB: nco_cpy_var_val_lmt() contains OpenMP critical region */
      /* if(lmt_nbr > 0) (void)nco_cpy_var_val_lmt(in_id,out_id,fp_bnr,NCO_BNR_WRT,xtr_lst[idx].nm,lmt,lmt_nbr); else (void)nco_cpy_var_val(in_id,out_id,fp_bnr,NCO_BNR_WRT,xtr_lst[idx].nm); */
      /* Multi-slab routines */
      /* NB: nco_cpy_var_val_mlt_lmt() contains OpenMP critical region */
      if(lmt_nbr > 0) (void)nco_cpy_var_val_mlt_lmt(in_id,out_id,fp_bnr,NCO_BNR_WRT,xtr_lst[idx].nm,lmt_all_lst,nbr_dmn_fl); else (void)nco_cpy_var_val(in_id,out_id,fp_bnr,NCO_BNR_WRT,xtr_lst[idx].nm);
    } /* end loop over idx */
    
    /* [fnc] Close unformatted binary data file */
    if(NCO_BNR_WRT) (void)nco_bnr_close(fp_bnr,fl_bnr);
    
    /* Close output file and move it from temporary to permanent location */
    (void)nco_fl_out_cls(fl_out,fl_out_tmp,out_id);
    
  }else{ /* !fl_out */
    /* No output file was specified so PRN_ tokens refer to screen printing */
    if(PRN_GLB_METADATA){
      (void)fprintf(stdout,"Opened file %s: dimensions = %i, variables = %i, global atts. = %i, ID = %i, type = %s\n",fl_in,nbr_dmn_fl,nbr_var_fl,glb_att_nbr,in_id,nco_fmt_sng(fl_in_fmt));
      if(rec_dmn_id != NCO_REC_DMN_UNDEFINED){
	char dmn_nm[NC_MAX_NAME]; 
	long rec_dmn_sz;
	
	(void)nco_inq_dim(in_id,rec_dmn_id,dmn_nm,&rec_dmn_sz);
	(void)fprintf(stdout,"Record dimension: name = %s, size = %li\n\n",dmn_nm,rec_dmn_sz);
      } /* end if */
      
      /* Print global attributes */
      (void)nco_prn_att(in_id,NC_GLOBAL);
    } /* endif PRN_GLB_METADATA */
    
    if(PRN_VAR_METADATA){
      for(idx=0;idx<nbr_xtr;idx++){
	/* Print variable's definition */
	(void)nco_prn_var_dfn(in_id,xtr_lst[idx].nm);
	/* Print variable's attributes */
	(void)nco_prn_att(in_id,xtr_lst[idx].id);
      } /* end loop over idx */
    } /* end if PRN_VAR_METADATA */
    
    if(PRN_VAR_DATA){
      /* NB: nco_msa_prn_var_val() with same nc_id contains OpenMP critical region */
      for(idx=0;idx<nbr_xtr;idx++) (void)nco_msa_prn_var_val(in_id,xtr_lst[idx].nm,lmt_all_lst,nbr_dmn_fl,dlm_sng,FORTRAN_IDX_CNV,PRN_DMN_UNITS,PRN_DMN_IDX_CRD_VAL,PRN_DMN_VAR_NM);
    } /* end if PRN_VAR_DATA */
    
  } /* !fl_out */
  
  /* Extraction list no longer needed */
  xtr_lst=nco_nm_id_lst_free(xtr_lst,nbr_xtr);
  
  /* Close input netCDF file */
  nco_close(in_id);
  
  /* Remove local copy of file */
  if(FILE_RETRIEVED_FROM_REMOTE_LOCATION && REMOVE_REMOTE_FILES_AFTER_PROCESSING) (void)nco_fl_rm(fl_in);
  
  /* Clean memory unless dirty memory allowed */
  if(flg_cln){
    /* ncks-specific memory */
    if(fl_bnr) fl_bnr=(char *)nco_free(fl_bnr);
    if(rec_dmn_nm) rec_dmn_nm=(char *)nco_free(rec_dmn_nm);

    /* free lmt[] NB: is now referenced within lmt_all_lst[idx] */
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
    /* Free lists of strings */
    if(fl_lst_in && fl_lst_abb == NULL) fl_lst_in=nco_sng_lst_free(fl_lst_in,fl_nbr); 
    if(fl_lst_in && fl_lst_abb) fl_lst_in=nco_sng_lst_free(fl_lst_in,1);
    if(fl_lst_abb) fl_lst_abb=nco_sng_lst_free(fl_lst_abb,abb_arg_nbr);
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
  
  /* End timer */ 
  ddra_info.tmr_flg=nco_tmr_end; /* [enm] Timer flag */
  rcd+=nco_ddra((char *)NULL,(char *)NULL,&ddra_info);
  
  if(rcd != NC_NOERR) nco_err_exit(rcd,"main");
  nco_exit_gracefully();
  return EXIT_SUCCESS;

} /* end main() */
