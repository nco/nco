/* $Header: /data/zender/nco_20150216/nco/src/nco/ncatted.c,v 1.65 2004-01-10 04:30:28 zender Exp $ */

/* ncatted -- netCDF attribute editor */

/* Purpose: Add, create, delete, or overwrite attributes in a netCDF file */

/* Copyright (C) 1995--2004 Charlie Zender

   This software may be modified and/or re-distributed under the terms of the GNU General Public License (GPL) Version 2
   The full license text is at http://www.gnu.ai.mit.edu/copyleft/gpl.html 
   and in the file nco/doc/LICENSE in the NCO source distribution.
   
   As a special exception to the terms of the GPL, you are permitted 
   to link the NCO source code with the DODS, HDF, netCDF, and UDUnits
   libraries and to distribute the resulting executables under the terms 
   of the GPL, but in addition obeying the extra stipulations of the 
   DODS, HDF, netCDF, and UDUnits licenses.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
   See the GNU General Public License for more details.
   
   The original author of this software, Charlie Zender, wants to improve it
   with the help of your suggestions, improvements, bug-reports, and patches.
   Please contact the NCO project at http://nco.sf.net or by writing
   Charlie Zender
   Department of Earth System Science
   University of California at Irvine
   Irvine, CA 92697-3100 */

/* Usage:

   Use C language escape sequences:
   ncatted -D 3 -h -a history,global,o,c,"String\tformatting\tfor\nDennis" in.nc ; ncks -M in.nc
   ncatted -D 3 -h -a history,global,o,c,'\a\b\f\n\r\t\v\\\?\0' in.nc ; ncks -M in.nc
   ncatted -D 3 -h -a history,global,o,c,'Characters which require protection by backslash:\nDouble quote: \"\nTwo consecutive double quotes: \"\"\nSingle quote: Beyond my shell abilities!\nBackslash: \\\nTwo consecutive backslashes: \\\\\nQuestion mark: \?\n' in.nc ; ncks -M in.nc
   ncatted -D 3 -h -a history,global,o,c,'Characters which do not require protection by backslash:\nSingle backquote: `\nDollarsign: $\nLeft brace: {\nRight brace: }\nPipe: |\nAmpersand: &\nAt sign: @\nPercent: %\n\n' in.nc ; ncks -M in.nc

   Append to existing string:
   ncatted -D 5 -O -a char_att,att_var,a,c,"and appended Sentence three." in.nc foo.nc

   Append to existing string with internal delimiter characters (commas):
   ncatted -D 5 -O -a char_att,att_var,a,c,"appended a comma, and three more commas,,," in.nc foo.nc

   Append to existing float:
   ncatted -D 5 -O -a float_att,att_var,a,f,74 in.nc foo.nc
   ncatted -D 5 -O -a float_att,att_var,a,f,74,75,76 in.nc foo.nc

   Create new float:
   ncatted -D 5 -O -a new_float_att,att_var,c,f,74 in.nc foo.nc

   Delete attribute:
   ncatted -D 5 -O -a float_att,att_var,d,,, in.nc foo.nc

   Delete all attributes for given var:
   ncatted -D 5 -O -a ,att_var,d,,, in.nc foo.nc

   Modify existing float:
   ncatted -D 5 -O -a float_att,att_var,m,f,74 in.nc foo.nc

   Modify existing missing value attribute:
   ncatted -D 5 -O -a missing_value,mss_val,m,f,74 in.nc foo.nc
   
   Multiple attribute edits:
   ncatted -D 5 -O -a char_att,att_var,a,c,"and appended Sentence three." -a short_att,att_var,c,s,37,38,39 -a float_att,att_var,d,,, -a long_att,att_var,o,l,37 -a new_att,att_var,o,d,73,74,75 in.nc foo.nc

   Create global attribute:
   ncatted -D 5 -O -a float_att,global,c,f,74 in.nc foo.nc

   Verify results:
   ncks -C -H -v att_var foo.nc

   Test algorithm for all variables:
   Append to existing string for all variables:
   ncatted -D 5 -O -a char_att,,a,c,"and appended Sentence three." in.nc foo.nc

   Append to existing float for all variables:
   ncatted -D 5 -O -a float_att,,a,f,74 in.nc foo.nc
   ncatted -D 5 -O -a float_att,,a,f,74,75,76 in.nc foo.nc

   Create new float for all variables:
   ncatted -D 5 -O -a float_att,,c,f,74 in.nc foo.nc

   Delete attribute for all variables:
   ncatted -D 5 -O -a float_att,,d,,, in.nc foo.nc

   Modify existing float for all variables:
   ncatted -D 5 -O -a float_att,,m,f,74 in.nc foo.nc

   Verify results:
   ncks -C -h foo.nc | m */

#ifdef HAVE_CONFIG_H
#include <config.h> /* Autotools tokens */
#endif /* !HAVE_CONFIG_H */

/* Standard C headers */
#include <math.h> /* sin cos cos sin 3.14159 */
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
#include <netcdf.h> /* netCDF definitions */
#include "nco_netcdf.h" /* NCO wrappers for netCDF C library */

/* #define MAIN_PROGRAM_FILE MUST precede #include libnco.h */
#define MAIN_PROGRAM_FILE
#include "nco.h" /* NCO definitions */
#include "libnco.h" /* netCDF Operator (NCO) library */

int 
main(int argc,char **argv)
{
  aed_sct *aed_lst=NULL_CEWI;

  bool OUTPUT_TO_NEW_NETCDF_FILE=False;
  bool FORCE_APPEND=False; /* Option A */
  bool FORCE_OVERWRITE=False; /* Option O */
  bool FILE_RETRIEVED_FROM_REMOTE_LOCATION;
  bool HISTORY_APPEND=True; /* Option h */
  bool REMOVE_REMOTE_FILES_AFTER_PROCESSING=True; /* Option R */
  
  char **fl_lst_abb=NULL; /* Option n */
  char **fl_lst_in;
  char *fl_in=NULL;
  char *aed_arg[NC_MAX_ATTRS];
  char *fl_out;
  char *fl_pth_lcl=NULL; /* Option l */
  char *fl_pth=NULL; /* Option p */
  char *time_bfr_srt;
  char *cmd_ln;

  const char * const CVS_Id="$Id: ncatted.c,v 1.65 2004-01-10 04:30:28 zender Exp $"; 
  const char * const CVS_Revision="$Revision: 1.65 $";
  const char * const opt_sng="Aa:D:hl:Op:Rr-:";
  
  extern char *optarg;
  extern int optind;
  
  int idx;
  int idx_var;
  int nc_id;  
  int abb_arg_nbr=0;
  int nbr_var_fl;
  int nbr_aed=0; /* Option a. NB: nbr_var_aed gets incremented */
  int nbr_fl=0;
  int opt;
  int rcd=NC_NOERR; /* [rcd] Return code */

  time_t time_crr_time_t;

  static struct option opt_lng[]=
    { /* Structure ordered by short option key if possible */
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
      {"path",required_argument,0,'p'},
      {"retain",no_argument,0,'R'},
      {"rtn",no_argument,0,'R'},
      {"version",no_argument,0,'r'},
      {"vrs",no_argument,0,'r'},
      {"help",no_argument,0,'?'},
      {0,0,0,0}
    }; /* end opt_lng */
  int opt_idx=0; /* Index of current long option into opt_lng array */

  /* Start clock and save command line */ 
  cmd_ln=nco_cmd_ln_sng(argc,argv);
  time_crr_time_t=time((time_t *)NULL);
  time_bfr_srt=ctime(&time_crr_time_t); time_bfr_srt=time_bfr_srt; /* Avoid compiler warning until variable is used for something */
  
  /* Get program name and set program enum (e.g., prg=ncra) */
  prg_nm=prg_prs(argv[0],&prg);

  /* Parse command line arguments */
  while((opt = getopt_long(argc,argv,opt_sng,opt_lng,&opt_idx)) != EOF){
    switch(opt){
    case 'A': /* Toggle FORCE_APPEND */
      FORCE_APPEND=!FORCE_APPEND;
      break;
    case 'a': /* Copy argument for later processing */
      aed_arg[nbr_aed]=optarg;
      nbr_aed++;
      break;
    case 'D': /* Debugging level. Default is 0. */
      dbg_lvl=(unsigned short)strtol(optarg,(char **)NULL,10);
      break;
    case 'h': /* Toggle appending to history global attribute */
      HISTORY_APPEND=!HISTORY_APPEND;
      break;
    case 'l': /* Local path prefix for files retrieved from remote file system */
      fl_pth_lcl=optarg;
      break;
    case 'O': /* Toggle FORCE_OVERWRITE */
      FORCE_OVERWRITE=!FORCE_OVERWRITE;
      break;
    case 'p': /* Common file path */
      fl_pth=optarg;
      break;
    case 'R': /* Toggle removal of remotely-retrieved-files. Default is True. */
      REMOVE_REMOTE_FILES_AFTER_PROCESSING=!REMOVE_REMOTE_FILES_AFTER_PROCESSING;
      break;
    case 'r': /* Print CVS program information and copyright notice */
      (void)copyright_prn(CVS_Id,CVS_Revision);
      (void)nco_lbr_vrs_prn();
      nco_exit(EXIT_SUCCESS);
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
    } /* end switch */
  } /* end while loop */
  
  /* Process positional arguments and fill in filenames */
  fl_lst_in=nco_fl_lst_mk(argv,argc,optind,&nbr_fl,&fl_out);
  if(fl_out != NULL) OUTPUT_TO_NEW_NETCDF_FILE=True; else fl_out=fl_lst_in[0];

  if(nbr_aed == 0){
    (void)fprintf(stdout,"%s: ERROR must specify an attribute to edit\n",prg_nm);
    nco_usg_prn();
    nco_exit(EXIT_FAILURE);
  } /* end if */ 

  /* Make uniform list of user-specified attribute edit structures */
  if(nbr_aed > 0) aed_lst=nco_prs_aed_lst(nbr_aed,aed_arg);

  /* We now have final list of attributes to edit */
  
  /* Parse filename */
  fl_in=nco_fl_nm_prs(fl_in,0,&nbr_fl,fl_lst_in,abb_arg_nbr,fl_lst_abb,fl_pth);
  /* Make sure file is on local system and is readable or die trying */
  fl_in=nco_fl_mk_lcl(fl_in,fl_pth_lcl,&FILE_RETRIEVED_FROM_REMOTE_LOCATION);

  if(OUTPUT_TO_NEW_NETCDF_FILE){

    if(!FORCE_OVERWRITE){
      int rcd_lcl;

      struct stat stat_sct;
      
      rcd_lcl=stat(fl_out,&stat_sct);

      /* If file already exists, then query the user whether to overwrite */
      if(rcd_lcl != -1){
        char usr_reply;
        
        usr_reply='z';
        while(usr_reply != 'n' && usr_reply != 'y'){
          (void)fprintf(stdout,"ncrename: overwrite %s (y/n)? ",fl_out);
	  (void)fflush(stdout);
          usr_reply=(char)fgetc(stdin);
        } /* end while */
        
        if(usr_reply == 'n'){
          nco_exit(EXIT_SUCCESS);
        } /* end if */
      } /* end if */
    } /* end if */
    
    /* Copy input file to output file, then search through output, editing attributes along the way
       This avoids possible XDR translation performance penalty of copying each variable with netCDF */
    (void)nco_fl_cp(fl_in,fl_out);

  } /* end if */

  
  /* Open file. Writing must be enabled and file should be in define mode for renaming */
  rcd=nco_open(fl_out,NC_WRITE,&nc_id);
  (void)nco_redef(nc_id);

  /* Get number of variables in file */
  (void)nco_inq(nc_id,(int *)NULL,&nbr_var_fl,(int *)NULL,(int *)NULL);

  for(idx=0;idx<nbr_aed;idx++){

    if(aed_lst[idx].var_nm != NULL){

      /* Is this a global attribute? */
      if(!strcmp(aed_lst[idx].var_nm,"global")) aed_lst[idx].id=NC_GLOBAL; else (void)nco_inq_varid(nc_id,aed_lst[idx].var_nm,&aed_lst[idx].id);

      /* Edit attribute */
      (void)nco_aed_prc(nc_id,aed_lst[idx].id,aed_lst[idx]);

    }else{ /* var_nm == NULL */
      /* Perform operation for every variable for which it makes sense */
      
      /* Edit attribute for every variable */
      for(idx_var=0;idx_var<nbr_var_fl;idx_var++) (void)nco_aed_prc(nc_id,idx_var,aed_lst[idx]);

    } /* end else var_nm == NULL */

  } /* end loop over idx */
  
  /* Catenate the timestamped command line to the "history" global attribute */
  if(HISTORY_APPEND) (void)nco_hst_att_cat(nc_id,cmd_ln);
  
  /* Take file out of define mode */
  (void)nco_enddef(nc_id);
    
  /* Close the open netCDF file */
  nco_close(nc_id);
  
  /* Remove local copy of file */
  if(FILE_RETRIEVED_FROM_REMOTE_LOCATION && REMOVE_REMOTE_FILES_AFTER_PROCESSING) (void)nco_fl_rm(fl_in);

  if(rcd != NC_NOERR) nco_err_exit(rcd,"main");
  nco_exit_gracefully();
  return EXIT_SUCCESS;
} /* end main() */
