/* $Header: /data/zender/nco_20150216/nco/src/nco/ncks.c,v 1.71 2002-12-16 16:42:00 rorik Exp $ */

/* ncks -- netCDF Kitchen Sink */

/* Purpose: Extract (subsets of) variables from a netCDF file 
   Print them to screen, or copy them to a new file, or both */

/* Copyright (C) 1995--2002 Charlie Zender

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.
   
   As a special exception to the terms of the GPL, you are permitted 
   to link the NCO source code with the NetCDF and HDF libraries 
   and distribute the resulting executables under the terms of the GPL, 
   but in addition obeying the extra stipulations of the netCDF and 
   HDF library licenses.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

   The file LICENSE contains the GNU General Public License, version 2
   It may be viewed interactively by typing, e.g., ncks -L

   The author of this software, Charlie Zender, would like to receive
   your suggestions, improvements, bug-reports, and patches for NCO.
   Please contact the project at http://sourceforge.net/projects/nco or by writing

   Charlie Zender
   Department of Earth System Science
   University of California at Irvine
   Irvine, CA 92697-3100
 */

/* Usage:
   ncks in.nc 
   ncks -v one in.nc 
   ncks in.nc foo.nc
   ncks -v one in.nc foo.nc
   ncks -p /ZENDER/tmp -l /data/zender/tmp h0001.nc foo.nc
   ncks -s "%+16.10f\n" -H -C -v three_dmn_var in.nc
   ncks -H -v fl_nm,fl_nm_arr ~/nco/data/in.nc
   ncks -H -d fl_dim,1 -d char_dim,6,12 -v fl_nm,fl_nm_arr ~/nco/data/in.nc
   ncks -H -m -v char_var_nul,char_var_space,char_var_multinul ~/nco/data/in.nc
   ncks -H -C -v three_dmn_rec_var -d time,,,2 in.nc
   ncks -H -C -v lon -d lon,3,1 in.nc
*/

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
#ifdef HAVE_GETOPT_H
#include <getopt.h>  /* getopt_long() */
#endif /* HAVE_GETOPT_H

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions */
#include "nco_netcdf.h" /* Wrappers for netCDF 3.X C-library */

/* #define MAIN_PROGRAM_FILE MUST precede #include nco.h */
#define MAIN_PROGRAM_FILE
#include "nco.h" /* NCO definitions */
#include "libnco.h" /* netCDF operator library */

int 
main(int argc,char **argv)
{
  bool ALPHABETIZE_OUTPUT=True; /* Option a */
  bool EXCLUDE_INPUT_LIST=False; /* Option c */
  bool FILE_RETRIEVED_FROM_REMOTE_LOCATION;
  bool FORCE_APPEND=False; /* Option A */
  bool FORCE_OVERWRITE=False; /* Option O */
  bool FORTRAN_STYLE=False; /* Option F */
  bool HISTORY_APPEND=True; /* Option h */
  bool NCO_BNR_WRT=False; /* [flg] Write binary file */
  bool OUTPUT_DATA=False; /* Option H */
  bool OUTPUT_VARIABLE_METADATA=False; /* Option m */
  bool OUTPUT_GLOBAL_METADATA=False; /* Option M */
  bool PRINT_DIMENSIONAL_UNITS=False; /* Option u */
  bool PRN_DMN_IDX_CRD_VAL=True; /* Option q Print leading dimension/coordinate indices/values */
  bool PROCESS_ALL_COORDINATES=False; /* Option c */
  bool PROCESS_ASSOCIATED_COORDINATES=True; /* Option C */
  bool REMOVE_REMOTE_FILES_AFTER_PROCESSING=True; /* Option R */

  char **var_lst_in=NULL_CEWI;
  char **fl_lst_abb=NULL; /* Option a */
  char **fl_lst_in;
  char *dlm_sng=NULL;
  char *fl_in=NULL;
  char *fl_pth_lcl=NULL; /* Option l */
  char *lmt_arg[NC_MAX_DIMS];
  char *opt_sng;
  char *fl_bnr=NULL; /* [sng] Unformatted binary output file */
  char *fl_out;
  char *fl_out_tmp;
  char *fl_pth=NULL; /* Option p */
  char *time_bfr_srt;
  char *cmd_ln;
  char CVS_Id[]="$Id: ncks.c,v 1.71 2002-12-16 16:42:00 rorik Exp $"; 
  char CVS_Revision[]="$Revision: 1.71 $";
  
  extern char *optarg;
  
  extern int optind;
  
  FILE *fp_bnr=NULL_CEWI; /* [fl] Unformatted binary output file handle */

  int fll_md_old; /* [enm] Old fill mode */
  int idx;
  int in_id;  
  int nbr_abb_arg=0;
  int nbr_dmn_fl;
  int lmt_nbr=0; /* Option d. NB: lmt_nbr gets incremented */
  int nbr_glb_att;
  int nbr_var_fl;
  int nbr_xtr=0; /* nbr_xtr won't otherwise be set for -c with no -v */
  int nbr_fl=0;
  int opt;
  int rec_dmn_id=NCO_REC_DMN_UNDEFINED;
    
  lmt_sct *lmt;

  nm_id_sct *xtr_lst=NULL; /* xtr_lst can get realloc()'d from NULL with -c option */

  time_t clock;

#ifdef HAVE_GETOPT_LONG
  static struct option opt_lng[]=
    {
      {"alphabetize",no_argument,0,'a'},
      {"append",no_argument,0,'A'},
      {"binary-file",required_argument,0,'b'},
      {"binary",no_argument,0,'B'},
      {"coords",no_argument,0,'c'},
      {"nocoords",no_argument,0,'C'},
      {"debug",required_argument,0,'D'},
      {"dbg_lvl",required_argument,0,'D'},
      {"dimension",required_argument,0,'d'},
      {"dmn",required_argument,0,'d'},
      {"fortran",no_argument,0,'F'},
      {"ftn",no_argument,0,'F'},
      {"history",no_argument,0,'h'},
      {"hst",no_argument,0,'h'},
      {"here",no_argument,0,'H'},
      {"local",no_argument,0,'l'},
      {"meta",no_argument,0,'m'},
      {"Meta",no_argument,0,'M'},
      {"overwrite",no_argument,0,'O'},
      {"path",required_argument,0,'p'},
      {"quiet",no_argument,0,'q'},
      {"retain",no_argument,0,'R'},
      {"revision",no_argument,0,'r'},
      {"string",required_argument,0,'s'},
      {"units",no_argument,0,'u'},
      {"variable",required_argument,0,'v'},
      {"version",no_argument,0,'r'},
      {"exclude",no_argument,0,'x'},
      {"help",no_argument,0,'?'},
      {0,0,0,0}
    }; /* end opt_lng */
  int opt_idx=0; /* Index of current long option into opt_lng array */
#endif /* HAVE_GETOPT_LONG */

  /* Start clock and save command line */ 
  cmd_ln=nco_cmd_ln_sng(argc,argv);
  clock=time((time_t *)NULL);
  time_bfr_srt=ctime(&clock); time_bfr_srt=time_bfr_srt; /* Avoid compiler warning until variable is used for something */
  fl_bnr=(char *)strdup("ncks.bnr");
  
  /* Get program name and set program enum (e.g., prg=ncra) */
  prg_nm=prg_prs(argv[0],&prg);

  /* Parse command line arguments */
  opt_sng="aABb:CcD:d:FHhl:MmOp:qrRs:uv:x-:";
#ifdef HAVE_GETOPT_LONG
  while((opt = getopt_long(argc,argv,opt_sng,opt_lng,&opt_idx)) != EOF){
#else  /* DO NOT HAVE GETOPT_LONG */
  while((opt = getopt(argc,argv,opt_sng)) != EOF){
#endif /* HAVE_GETOPT_LONG */
    switch(opt){
    case 'a': /* Toggle ALPHABETIZE_OUTPUT */
      ALPHABETIZE_OUTPUT=!ALPHABETIZE_OUTPUT;
      break;
    case 'A': /* Toggle FORCE_APPEND */
      FORCE_APPEND=!FORCE_APPEND;
      break;
    case 'B': /* Set NCO_BNR_WRT */
      NCO_BNR_WRT=True;
      break;
    case 'b': /* Set file for binary output */
      NCO_BNR_WRT=True;
      fl_bnr=(char *)strdup(optarg);
      break;
    case 'C': /* Extract all coordinates associated with extracted variables? */
      PROCESS_ASSOCIATED_COORDINATES=False;
      break;
    case 'c': /* Add all coordinates to extraction list? */
      PROCESS_ALL_COORDINATES=True;
      break;
    case 'D': /* Debugging level. Default is 0. */
      dbg_lvl=(unsigned short)strtol(optarg,(char **)NULL,10);
      break;
    case 'd': /* Copy argument for later processing */
      lmt_arg[lmt_nbr]=(char *)strdup(optarg);
      lmt_nbr++;
      break;
    case 'H': /* Print data to screen */
      OUTPUT_DATA=True;
      break;
    case 'h': /* Toggle appending to history global attribute */
      HISTORY_APPEND=!HISTORY_APPEND;
      break;
    case 'l': /* Local path prefix for files retrieved from remote file system */
      fl_pth_lcl=optarg;
      break;
    case 'm': /* Print variable metadata to screen */
      OUTPUT_VARIABLE_METADATA=True;
      break;
    case 'M': /* Print global metadata to screen */
      OUTPUT_GLOBAL_METADATA=True;
      break;
    case 'O': /* Toggle FORCE_OVERWRITE */
      FORCE_OVERWRITE=!FORCE_OVERWRITE;
      break;
    case 'F': /* Toggle index convention. Default is 0-based arrays (C-style). */
      FORTRAN_STYLE=!FORTRAN_STYLE;
      break;
    case 'p': /* Common file path */
      fl_pth=optarg;
      break;
    case 'q': /* Turn off printing of dimension indices and coordinate values */
      PRN_DMN_IDX_CRD_VAL=!PRN_DMN_IDX_CRD_VAL;
      break;
    case 'R': /* Toggle removal of remotely-retrieved-files. Default is True. */
      REMOVE_REMOTE_FILES_AFTER_PROCESSING=!REMOVE_REMOTE_FILES_AFTER_PROCESSING;
      break;
    case 'r': /* Print CVS program information and copyright notice */
      (void)copyright_prn(CVS_Id,CVS_Revision);
      (void)nco_lib_vrs_prn();
      nco_exit(EXIT_SUCCESS);
      break;
    case 's': /* User specified delimiter string for printed output */
      dlm_sng=optarg;
      break;
    case 'u': /* Turn on the printing of dimensional units. */
      PRINT_DIMENSIONAL_UNITS=!PRINT_DIMENSIONAL_UNITS;
      break;
    case 'v': /* Variables to extract/exclude */
      var_lst_in=lst_prs(optarg,",",&nbr_xtr);
      break;
    case 'x': /* Exclude rather than extract variables specified with -v */
      EXCLUDE_INPUT_LIST=True;
      break;
    case '?': /* Print proper usage */
      (void)nco_usg_prn();
      nco_exit(EXIT_FAILURE);
      break;
    case '-': /* Long options are not allowed */
      (void)printf("long options are not available in this build.\n");
      (void)printf("use single-letter options instead.\n");
      nco_exit(EXIT_FAILURE);
      break;
    default: /* Print proper usage */
      (void)nco_usg_prn();
      nco_exit(EXIT_FAILURE);
      break;
    } /* end switch */
  } /* end while loop */
  
  /* Process positional arguments and fill in filenames */
  fl_lst_in=nco_fl_lst_mk(argv,argc,optind,&nbr_fl,&fl_out);
  
  /* Make uniform list of user-specified dimension limits */
  lmt=nco_lmt_prs(lmt_nbr,lmt_arg);
  
  /* Parse filename */
  fl_in=nco_fl_nm_prs(fl_in,0,&nbr_fl,fl_lst_in,nbr_abb_arg,fl_lst_abb,fl_pth);
  /* Make sure file is on local system and is readable or die trying */
  fl_in=nco_fl_mk_lcl(fl_in,fl_pth_lcl,&FILE_RETRIEVED_FROM_REMOTE_LOCATION);
  /* Open file for reading */
  nco_open(fl_in,NC_NOWRITE,&in_id);
  
  /* Get number of variables, dimensions, and global attributes in file */
  (void)nco_inq(in_id,&nbr_dmn_fl,&nbr_var_fl,&nbr_glb_att,&rec_dmn_id);
  
  /* Form initial extraction list from user input */
  xtr_lst=nco_var_lst_mk(in_id,nbr_var_fl,var_lst_in,PROCESS_ALL_COORDINATES,&nbr_xtr);

  /* Change included variables to excluded variables */
  if(EXCLUDE_INPUT_LIST) xtr_lst=nco_var_lst_xcl(in_id,nbr_var_fl,xtr_lst,&nbr_xtr);

  /* Add all coordinate variables to extraction list */
  if(PROCESS_ALL_COORDINATES) xtr_lst=nco_var_lst_add_crd(in_id,nbr_var_fl,nbr_dmn_fl,xtr_lst,&nbr_xtr);

  /* Make sure coordinates associated extracted variables are also on extraction list */
  if(PROCESS_ASSOCIATED_COORDINATES) xtr_lst=nco_var_lst_ass_crd_add(in_id,xtr_lst,&nbr_xtr);

  /* Sort extraction list alphabetically or by variable ID */
  if(nbr_xtr > 1) xtr_lst=nco_lst_srt(xtr_lst,nbr_xtr,ALPHABETIZE_OUTPUT);
    
  /* We now have final list of variables to extract. Phew. */
  
  /* Find coordinate/dimension values associated with user-specified limits */
  for(idx=0;idx<lmt_nbr;idx++) (void)nco_lmt_evl(in_id,lmt+idx,0L,FORTRAN_STYLE);
  
  if(fl_out != NULL){
    int out_id;  

    /* Open output file */
    fl_out_tmp=nco_fl_out_open(fl_out,FORCE_APPEND,FORCE_OVERWRITE,&out_id);
    
    /* Copy global attributes */
    (void)nco_att_cpy(in_id,out_id,NC_GLOBAL,NC_GLOBAL,True);
    
    /* Catenate timestamped command line to "history" global attribute */
    if(HISTORY_APPEND) (void)nco_hst_att_cat(out_id,cmd_ln);

    for(idx=0;idx<nbr_xtr;idx++){
      int var_out_id;
      
      /* Define variable in output file */
      if(lmt_nbr > 0) var_out_id=nco_cpy_var_dfn_lmt(in_id,out_id,rec_dmn_id,xtr_lst[idx].nm,lmt,lmt_nbr); else var_out_id=nco_cpy_var_dfn(in_id,out_id,rec_dmn_id,xtr_lst[idx].nm);
      /* Copy variable's attributes */
      (void)nco_att_cpy(in_id,out_id,xtr_lst[idx].id,var_out_id,True);
    } /* end loop over idx */

    /* Turn off default filling behavior to enhance efficiency */
    nco_set_fill(out_id,NC_NOFILL,&fll_md_old);
  
    /* Take output file out of define mode */
    (void)nco_enddef(out_id);
    
    /* [fnc] Open unformatted binary data file for writing */
    if(NCO_BNR_WRT) fp_bnr=nco_bnr_open(fl_bnr);

    /* Copy variable data */
    for(idx=0;idx<nbr_xtr;idx++){
      if(dbg_lvl > 2 && !NCO_BNR_WRT) (void)fprintf(stderr,"%s, ",xtr_lst[idx].nm);
      if(dbg_lvl > 0) (void)fflush(stderr);
      if(lmt_nbr > 0) (void)nco_cpy_var_val_lmt(in_id,out_id,fp_bnr,NCO_BNR_WRT,xtr_lst[idx].nm,lmt,lmt_nbr); else (void)nco_cpy_var_val(in_id,out_id,fp_bnr,NCO_BNR_WRT,xtr_lst[idx].nm);
    } /* end loop over idx */
    
    /* [fnc] Close unformatted binary data file */
    if(NCO_BNR_WRT) (void)nco_bnr_close(fp_bnr,fl_bnr);

    /* Close output file and move it from temporary to permanent location */
    (void)nco_fl_out_cls(fl_out,fl_out_tmp,out_id);

  } /* end if fl_out != NULL */
  
  if(OUTPUT_GLOBAL_METADATA){
    (void)fprintf(stdout,"Opened file %s: dimensions = %i, variables = %i, global atts. = %i, id = %i\n",fl_in,nbr_dmn_fl,nbr_var_fl,nbr_glb_att,in_id);
    if(rec_dmn_id != NCO_REC_DMN_UNDEFINED){
      char rec_dmn_nm[NC_MAX_NAME];
      long rec_dmn_sz;
      
      (void)nco_inq_dim(in_id,rec_dmn_id,rec_dmn_nm,&rec_dmn_sz);
      (void)fprintf(stdout,"Record dimension: name = %s, size = %li\n\n",rec_dmn_nm,rec_dmn_sz);
    } /* end if */
    
    /* Print all global attributes */
    (void)nco_prn_att(in_id,NC_GLOBAL);
  } /* endif OUTPUT_GLOBAL_METADATA */
  
  if(OUTPUT_VARIABLE_METADATA){
    for(idx=0;idx<nbr_xtr;idx++){
      /* Print variable's definition */
      (void)nco_prn_var_dfn(in_id,xtr_lst[idx].nm);
      /* Print variable's attributes */
      (void)nco_prn_att(in_id,xtr_lst[idx].id);
    } /* end loop over idx */
  } /* end if OUTPUT_VARIABLE_METADATA */
  
  if(OUTPUT_DATA){
    for(idx=0;idx<nbr_xtr;idx++) (void)nco_prn_var_val_lmt(in_id,xtr_lst[idx].nm,lmt,lmt_nbr,dlm_sng,FORTRAN_STYLE,PRINT_DIMENSIONAL_UNITS,PRN_DMN_IDX_CRD_VAL);
  } /* end if OUTPUT_DATA */
  
  /* Close input netCDF file */
  nco_close(in_id);
  
  /* Remove local copy of file */
  if(FILE_RETRIEVED_FROM_REMOTE_LOCATION && REMOVE_REMOTE_FILES_AFTER_PROCESSING) (void)nco_fl_rm(fl_in);

  nco_exit_gracefully();
  return EXIT_SUCCESS;
} /* end main() */
