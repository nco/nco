/* $Header: /data/zender/nco_20150216/nco/src/nco/ncatted.c,v 1.158 2013-07-23 20:59:40 zender Exp $ */

/* ncatted -- netCDF attribute editor */

/* Purpose: Add, create, delete, or overwrite attributes in a netCDF file */

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

   Use C language escape sequences:
   ncatted -D 3 -h -a history,global,o,c,"String\tformatting\tfor\nDennis" ~/nco/data/in.nc ~/foo.nc
   ncatted -D 3 -h -a history,global,o,c,'\a\b\f\n\r\t\v\\\?\0' ~/nco/data/in.nc ~/foo.nc
   ncatted -D 3 -h -a history,global,o,c,'Characters which require protection by backslash:\nDouble quote: \"\nTwo consecutive double quotes: \"\"\nSingle quote: Beyond my shell abilities!\nBackslash: \\\nTwo consecutive backslashes: \\\\\nQuestion mark: \?\n' ~/nco/data/in.nc ~/foo.nc
   ncatted -D 3 -h -a history,global,o,c,'Characters which do not require protection by backslash:\nSingle backquote: `\nDollarsign: $\nLeft brace: {\nRight brace: }\nPipe: |\nAmpersand: &\nAt sign: @\nPercent: %\n\n' ~/nco/data/in.nc ~/foo.nc

   Append to existing string:
   ncatted -D 5 -O -a char_att,att_var,a,c,"and appended Sentence three." ~/nco/data/in.nc ~/foo.nc

   Append to existing string with internal delimiter characters (commas):
   ncatted -D 5 -O -a char_att,att_var,a,c,"appended a comma, and three more commas,,," ~/nco/data/in.nc ~/foo.nc

   Append to existing float:
   ncatted -D 5 -O -a float_att,att_var,a,f,74 ~/nco/data/in.nc ~/foo.nc
   ncatted -D 5 -O -a float_att,att_var,a,f,74,75,76 ~/nco/data/in.nc ~/foo.nc

   Create new float:
   ncatted -D 5 -O -a new_float_att,att_var,c,f,74 ~/nco/data/in.nc ~/foo.nc

   Create new string:
   ncatted -D 5 -O -a new_string_att,att_var,c,sng,"string" ~/nco/data/in_4.nc ~/foo.nc

   Create new list of strings:
   ncatted -D 5 -O -a new_string_att,att_var,c,sng,"list","of","strings" ~/nco/data/in_4.nc ~/foo.nc

   Create all new netCDF4 atomic type attributes:
   ncatted -D 5 -O -a new_ubyte_att,att_var,c,ub,0 -a new_ushort_att,att_var,c,us,74 -a new_uint_att,att_var,c,u,71,72,73,74 -a new_int64_att,att_var,c,ll,-74 -a new_uint64_att,att_var,c,ull,74,75 -a new_string_att,att_var,c,sng,"No man is an island" ~/nco/data/in_4.nc ~/foo.nc

   Delete attribute:
   ncatted -D 5 -O -a float_att,att_var,d,,, ~/nco/data/in.nc ~/foo.nc

   Delete all attributes for given var:
   ncatted -D 5 -O -a ,att_var,d,,, ~/nco/data/in.nc ~/foo.nc

   Modify existing float:
   ncatted -D 5 -O -a float_att,att_var,m,f,74 ~/nco/data/in.nc ~/foo.nc

   Modify existing missing value attribute:
   ncatted -D 5 -O -a missing_value,mss_val,m,f,74 ~/nco/data/in.nc ~/foo.nc
   
   Multiple attribute edits:
   ncatted -D 5 -O -a char_att,att_var,a,c,"and appended Sentence three." -a short_att,att_var,c,s,37,38,39 -a float_att,att_var,d,,, -a long_att,att_var,o,l,37 -a new_att,att_var,o,d,73,74,75 ~/nco/data/in.nc ~/foo.nc

   Create global attribute:
   ncatted -D 5 -O -a float_att,global,c,f,74 ~/nco/data/in.nc ~/foo.nc

   Test algorithm for all variables:
   Append to existing string for all variables:
   ncatted -D 5 -O -a char_att,,a,c,"and appended Sentence three." ~/nco/data/in.nc ~/foo.nc

   Append to existing float for all variables:
   ncatted -D 5 -O -a float_att,,a,f,74 ~/nco/data/in.nc ~/foo.nc
   ncatted -D 5 -O -a float_att,,a,f,74,75,76 ~/nco/data/in.nc ~/foo.nc

   Create new float for all variables:
   ncatted -D 5 -O -a float_att,,c,f,74 ~/nco/data/in.nc ~/foo.nc

   Delete attribute for all variables:
   ncatted -D 5 -O -a float_att,,d,,, ~/nco/data/in.nc ~/foo.nc

   Modify existing float for all variables:
   ncatted -D 5 -O -a float_att,,m,f,74 ~/nco/data/in.nc ~/foo.nc

   Verify results:
   ncks -C -h ~/foo.nc | m */

#if 0
#define USE_TRV_API
#endif

#ifdef HAVE_CONFIG_H
# include <config.h> /* Autotools tokens */
#endif /* !HAVE_CONFIG_H */

/* Standard C headers */
#include <math.h> /* sin cos cos sin 3.14159 */
#include <stdio.h> /* stderr, FILE, NULL, etc. */
#include <stdlib.h> /* atof, atoi, malloc, getopt */
#include <string.h> /* strcmp() */
#ifdef HAVE_STRINGS_H
# include <strings.h> /* strcasecmp() */
#endif /* !HAVE_STRINGS_H */
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
#include "nco_netcdf.h" /* NCO wrappers for netCDF C library */

/* #define MAIN_PROGRAM_FILE MUST precede #include libnco.h */
#define MAIN_PROGRAM_FILE
#include "nco.h" /* netCDF Operator (NCO) definitions */
#include "libnco.h" /* netCDF Operator (NCO) library */

int 
main(int argc,char **argv)
{
  aed_sct *aed_lst=NULL_CEWI;

  nco_bool FL_RTR_RMT_LCN;
  nco_bool FL_LST_IN_FROM_STDIN=False; /* [flg] fl_lst_in comes from stdin */
  nco_bool FORCE_APPEND=False; /* Option A */
  nco_bool FORCE_OVERWRITE=False; /* Option O */
  nco_bool HISTORY_APPEND=True; /* Option h */
  nco_bool FL_OUT_NEW=False;
  nco_bool RAM_OPEN=False; /* [flg] Open (netCDF3-only) file(s) in RAM */
  nco_bool RM_RMT_FL_PST_PRC=True; /* Option R */
  nco_bool flg_cln=False; /* [flg] Clean memory prior to exit */

  char **fl_lst_abb=NULL; /* Option n */
  char **fl_lst_in;
  char *aed_arg[NC_MAX_ATTRS];
  char *cmd_ln;
  char *fl_in=NULL;
  char *fl_out=NULL; /* Option o */
  char *fl_pth=NULL; /* Option p */
  char *fl_pth_lcl=NULL; /* Option l */
  char *opt_crr=NULL; /* [sng] String representation of current long-option name */
  char *sng_cnv_rcd=NULL_CEWI; /* [sng] strtol()/strtoul() return code */

  const char * const CVS_Id="$Id: ncatted.c,v 1.158 2013-07-23 20:59:40 zender Exp $"; 
  const char * const CVS_Revision="$Revision: 1.158 $";
  const char * const opt_sht_lst="Aa:D:hl:Oo:p:Rr-:";

#if defined(__cplusplus) || defined(PGI_CC)
  ddra_info_sct ddra_info;
  ddra_info.flg_ddra=False;
#else /* !__cplusplus */
  ddra_info_sct ddra_info={.flg_ddra=False};
#endif /* !__cplusplus */

  extern char *optarg;
  extern int optind;

  int abb_arg_nbr=0;
  int fl_nbr=0;
  int idx;
  int idx_var;
  int nbr_aed=0; /* Option a. NB: nbr_var_aed gets incremented */
  int nbr_var_fl;
  int nc_id;  
  int md_open; /* [enm] Mode flag for nc_open() call */
  int opt;
  int rcd=NC_NOERR; /* [rcd] Return code */

  size_t bfr_sz_hnt=NC_SIZEHINT_DEFAULT; /* [B] Buffer size hint */
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
    {"ram_all",no_argument,0,0}, /* [flg] Open (netCDF3) and create file(s) in RAM */
    {"create_ram",no_argument,0,0}, /* [flg] Create file in RAM */
    {"open_ram",no_argument,0,0}, /* [flg] Open (netCDF3) file(s) in RAM */
    {"diskless_all",no_argument,0,0}, /* [flg] Open (netCDF3) and create file(s) in RAM */
    {"version",no_argument,0,0},
    {"vrs",no_argument,0,0},
    /* Long options with argument, no short option counterpart */
    {"bfr_sz_hnt",required_argument,0,0}, /* [B] Buffer size hint */
    {"buffer_size_hint",required_argument,0,0}, /* [B] Buffer size hint */
    {"hdr_pad",required_argument,0,0},
    {"header_pad",required_argument,0,0},
    /* Long options with short counterparts */
    {"append",no_argument,0,'A'},
    {"attribute",required_argument,0,'a'},
    {"debug",required_argument,0,'D'},
    {"dbg_lvl",required_argument,0,'D'},
    {"history",no_argument,0,'h'},
    {"hst",no_argument,0,'h'},
    {"local",required_argument,0,'l'},
    {"lcl",required_argument,0,'l'},
    {"overwrite",no_argument,0,'O'},
    {"ovr",no_argument,0,'O'},
    {"output",required_argument,0,'o'},
    {"fl_out",required_argument,0,'o'},
    {"path",required_argument,0,'p'},
    {"retain",no_argument,0,'R'},
    {"rtn",no_argument,0,'R'},
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
      if(!strcmp(opt_crr,"cln") || !strcmp(opt_crr,"mmr_cln") || !strcmp(opt_crr,"clean")) flg_cln=True; /* [flg] Clean memory prior to exit */
      if(!strcmp(opt_crr,"drt") || !strcmp(opt_crr,"mmr_drt") || !strcmp(opt_crr,"dirty")) flg_cln=False; /* [flg] Clean memory prior to exit */
      if(!strcmp(opt_crr,"hdr_pad") || !strcmp(opt_crr,"header_pad")){
        hdr_pad=strtoul(optarg,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
        if(*sng_cnv_rcd) nco_sng_cnv_err(optarg,"strtoul",sng_cnv_rcd);
      } /* endif "hdr_pad" */
      if(!strcmp(opt_crr,"ram_all") || !strcmp(opt_crr,"open_ram") || !strcmp(opt_crr,"diskless_all")) RAM_OPEN=True; /* [flg] Create file in RAM */
      if(!strcmp(opt_crr,"vrs") || !strcmp(opt_crr,"version")){
        (void)nco_vrs_prn(CVS_Id,CVS_Revision);
        nco_exit(EXIT_SUCCESS);
      } /* endif "vrs" */
    } /* opt != 0 */
    /* Process short options */
    switch(opt){
    case 0: /* Long options have already been processed, return */
      break;
    case 'A': /* Toggle FORCE_APPEND */
      FORCE_APPEND=!FORCE_APPEND;
      break;
    case 'a': /* Copy argument for later processing */
      aed_arg[nbr_aed]=(char *)strdup(optarg);
      nbr_aed++;
      break;
    case 'D': /* Debugging level. Default is 0. */
      dbg_lvl=(unsigned short int)strtoul(optarg,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
      if(*sng_cnv_rcd) nco_sng_cnv_err(optarg,"strtoul",sng_cnv_rcd);
      break;
    case 'h': /* Toggle appending to history global attribute */
      HISTORY_APPEND=!HISTORY_APPEND;
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
      RM_RMT_FL_PST_PRC=!RM_RMT_FL_PST_PRC;
      break;
    case 'r': /* Print CVS program information and copyright notice */
      (void)nco_vrs_prn(CVS_Id,CVS_Revision);
      (void)nco_lbr_vrs_prn();
      (void)nco_cpy_prn();
      (void)nco_cnf_prn();
      nco_exit(EXIT_SUCCESS);
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
    } /* end switch */
    if(opt_crr) opt_crr=(char *)nco_free(opt_crr);
  } /* end while loop */

  /* Process positional arguments and fill in filenames */
  fl_lst_in=nco_fl_lst_mk(argv,argc,optind,&fl_nbr,&fl_out,&FL_LST_IN_FROM_STDIN);
  if(fl_out) FL_OUT_NEW=True; else fl_out=(char *)strdup(fl_lst_in[0]);

  if(nbr_aed == 0){
    (void)fprintf(stdout,"%s: ERROR must specify an attribute to edit\n",prg_nm);
    nco_usg_prn();
    nco_exit(EXIT_FAILURE);
  } /* end if */ 

  if(dbg_lvl_get() >= nco_dbg_var) (void)fprintf(stdout,"%s: DEBUG attribute assumed to hold missing data is named \"%s\"\n",prg_nm_get(),nco_mss_val_sng_get());

  /* Make uniform list of user-specified attribute edit structures */
  if(nbr_aed > 0) aed_lst=nco_prs_aed_lst(nbr_aed,aed_arg);

  /* We now have final list of attributes to edit */

  /* Parse filename */
  fl_in=nco_fl_nm_prs(fl_in,0,&fl_nbr,fl_lst_in,abb_arg_nbr,fl_lst_abb,fl_pth);
  /* Make sure file is on local system and is readable or die trying */
  fl_in=nco_fl_mk_lcl(fl_in,fl_pth_lcl,&FL_RTR_RMT_LCN);

  if(FL_OUT_NEW){
    /* Obtain user consent, if needed, to overwrite output file (or die trying) */
    if(!FORCE_OVERWRITE) nco_fl_overwrite_prm(fl_out);

    /* Copy input file to output file and then search through output, 
    changing names on the fly. This avoids possible XDR translation
    performance penalty of copying each variable with netCDF. */
    (void)nco_fl_cp(fl_in,fl_out);

    /* Ensure output file is user/owner-writable */
    (void)nco_fl_chmod(fl_out);
  } /* end if FL_OUT_NEW */

  /* Open file. Writing must be enabled and file should be in define mode for renaming */
  /*  if(dbg_lvl == 8) md_open|=NC_SHARE;*/
  if(RAM_OPEN) md_open=NC_WRITE|NC_DISKLESS; else md_open=NC_WRITE;
  rcd+=nco_fl_open(fl_out,md_open,&bfr_sz_hnt,&nc_id);
  (void)nco_redef(nc_id);

#ifndef USE_TRV_API

  /* Get number of variables in file */
  (void)nco_inq(nc_id,(int *)NULL,&nbr_var_fl,(int *)NULL,(int *)NULL);

  /* Timestamp end of metadata setup and disk layout */
  rcd+=nco_ddra((char *)NULL,(char *)NULL,&ddra_info);
  ddra_info.tmr_flg=nco_tmr_rgl;

  for(idx=0;idx<nbr_aed;idx++){
    if(aed_lst[idx].var_nm == NULL){
      /* Variable name is blank so edit same attribute for all variables ... */
      for(idx_var=0;idx_var<nbr_var_fl;idx_var++) (void)nco_aed_prc(nc_id,idx_var,aed_lst[idx]);
    }else if(strpbrk(aed_lst[idx].var_nm,".*^$\\[]()<>+?|{}")){
      /* Variable name contains a "regular expression" (rx) ... */
      int xtr_nbr=1;
      nm_id_sct *xtr_lst=NULL;
      xtr_lst=nco_var_lst_mk(nc_id,nbr_var_fl,&aed_lst[idx].var_nm,False,False,&xtr_nbr);
      /* Edit attribute for each matching variable */
      for(idx_var=0;idx_var<xtr_nbr;idx_var++) (void)nco_aed_prc(nc_id,xtr_lst[idx_var].id,aed_lst[idx]);
      /* Free Extraction list  */
      xtr_lst=nco_nm_id_lst_free(xtr_lst,xtr_nbr);
    }else if(!strcasecmp(aed_lst[idx].var_nm,"global")){
      /* Variable name indicates a global attribute ... */
      (void)nco_aed_prc(nc_id,NC_GLOBAL,aed_lst[idx]);
    }else{ 
      /* Variable is a normal variable ... */
      (void)nco_inq_varid(nc_id,aed_lst[idx].var_nm,&aed_lst[idx].id);
      /* Edit attribute */
      (void)nco_aed_prc(nc_id,aed_lst[idx].id,aed_lst[idx]);
    } /* end var_nm */
  } /* end loop over idx */

#else

  /* Initialize traversal table */
  (void)trv_tbl_init(&trv_tbl);

  /* Construct GTT, Group Traversal Table (groups,variables,dimensions, limits) */
  (void)nco_bld_trv_tbl(nc_id,"/",(nco_bool)False,(int)0,(lmt_sct**)NULL,(nco_bool)False,(int)0,(char **)NULL,trv_tbl);

  /* Process attributes  */
  (void)nco_aed_prc_trv(nc_id,aed_lst,nbr_aed,trv_tbl);


#endif /* USE_TRV_API */

  /* Catenate the timestamped command line to the "history" global attribute */
  if(HISTORY_APPEND) (void)nco_hst_att_cat(nc_id,cmd_ln);

  /* Take output file out of define mode */
  if(hdr_pad == 0UL){
    (void)nco_enddef(nc_id);
  }else{
    (void)nco__enddef(nc_id,hdr_pad);
    if(dbg_lvl >= nco_dbg_scl) (void)fprintf(stderr,"%s: INFO Padding header with %lu extra bytes\n",prg_nm_get(),(unsigned long)hdr_pad);
  } /* hdr_pad */

  /* Close the open netCDF file */
  nco_close(nc_id);

  /* Remove local copy of file */
  if(FL_RTR_RMT_LCN && RM_RMT_FL_PST_PRC) (void)nco_fl_rm(fl_in);

  /* Clean memory unless dirty memory allowed */
  if(flg_cln){
    /* ncatted-specific memory */
    for(idx=0;idx<nbr_aed;idx++) aed_arg[idx]=(char *)nco_free(aed_arg[idx]);
    for(idx=0;idx<nbr_aed;idx++){
      if(aed_lst[idx].att_nm) aed_lst[idx].att_nm=(char *)nco_free(aed_lst[idx].att_nm);
      if(aed_lst[idx].var_nm) aed_lst[idx].var_nm=(char *)nco_free(aed_lst[idx].var_nm);
      aed_lst[idx].val.vp=(void *)nco_free(aed_lst[idx].val.vp);
    } /* end for */
    if(nbr_aed > 0) aed_lst=(aed_sct *)nco_free(aed_lst);

    /* NCO-generic clean-up */
    /* Free individual strings/arrays */
    if(cmd_ln) cmd_ln=(char *)nco_free(cmd_ln);
    if(fl_in) fl_in=(char *)nco_free(fl_in);
    if(fl_out) fl_out=(char *)nco_free(fl_out);
    if(fl_pth) fl_pth=(char *)nco_free(fl_pth);
    if(fl_pth_lcl) fl_pth_lcl=(char *)nco_free(fl_pth_lcl);
    /* Free lists of strings */
    if(fl_lst_in && fl_lst_abb == NULL) fl_lst_in=nco_sng_lst_free(fl_lst_in,fl_nbr); 
    if(fl_lst_in && fl_lst_abb) fl_lst_in=nco_sng_lst_free(fl_lst_in,1);
    if(fl_lst_abb) fl_lst_abb=nco_sng_lst_free(fl_lst_abb,abb_arg_nbr);

    trv_tbl_free(trv_tbl);
  } /* !flg_cln */

  /* End timer */ 
  ddra_info.tmr_flg=nco_tmr_end; /* [enm] Timer flag */
  rcd+=nco_ddra((char *)NULL,(char *)NULL,&ddra_info);

  if(rcd != NC_NOERR) nco_err_exit(rcd,"main");
  nco_exit_gracefully();
  return EXIT_SUCCESS;
} /* end main() */
