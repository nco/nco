/* $Header: /data/zender/nco_20150216/nco/src/nco/ncap.c,v 1.35 2001-11-29 16:08:37 hmb Exp $ */

/* ncap -- netCDF arithmetic processor */

/* Purpose: Compute user-defined derived fields using forward algebraic notation applied to netCDF files */

/* Copyright (C) 1995--2001 Charlie Zender

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
   ncap -O -D 1 -S ncap.in in.nc foo.nc
   ncap -O -D 1 -s a=b+c -s "b=c-d/2." -S ncap.in in.nc foo.nc
   ncap -O -D 1 -s two=one+two in.nc foo.nc
   */

/* fxm 20000730:
   I currently get four compiler warnings I do not understand on Linux
   Two are in bison.simple and occur when compiling ../src/nco/ncap.tab.c:
/usr/lib/bison.simple:432: warning: implicit declaration of function `yylex'
/usr/lib/bison.simple:458: warning: implicit declaration of function `yyprint'
   One occurs when compiling the lexer lex.yy.c = ncap_lex.c:
lex.yy.c:1060: warning: `yyunput' defined but not used
   Once occurs when compiling ncap.c which calls the parser:
../src/nco/ncap.c:423: warning: implicit declaration of function `yyparse'
 */

/* Standard C headers */
#include <math.h> /* sin cos cos sin 3.14159 */
#include <stdio.h> /* stderr, FILE, NULL, etc. */
#include <stdlib.h> /* atof, atoi, malloc, getopt */
#include <string.h> /* strcmp. . . */
#include <sys/stat.h> /* stat() */
#include <time.h> /* machine time */
#include <unistd.h> /* all sorts of POSIX stuff */
/* #include <assert.h> */ /* assert() debugging macro */
/* #include <errno.h> */ /* errno */
/* #include <malloc.h> */ /* malloc() stuff */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions */
#include "nco_netcdf.h"  /* neCDF wrapper functions */

/* #define MAIN_PROGRAM_FILE MUST precede #include nco.h */
#define MAIN_PROGRAM_FILE
#include "nco.h" /* NCO definitions */
#include "ncap.h" /* ncap-specific definitions */

#ifdef LINUX
#include <getopt.h> /* GNU getopt() is standard on Linux */
#else /* not LINUX */
#if ( !defined AIX ) && ( !defined CRAY ) && ( !defined NEC ) /* getopt() is in <unistd.h> or <stdlib.h> */
#include "getopt.h" /* GNU getopt() */
#endif /* not AIX */
#endif /* not LINUX */

long int line_number;    /* line number incremented in ncap.l */
char *fl_spt_global;         /* instructions file */

int 
main(int argc,char **argv)
{
  extern int yyparse (void *); /* Prototype here as in bison.simple to avoid compiler warning */


  extern FILE *yyin;

  bool EXCLUDE_INPUT_LIST=False; /* Option c */
  bool FILE_RETRIEVED_FROM_REMOTE_LOCATION;
  bool FORCE_APPEND=False; /* Option A */
  bool FORCE_OVERWRITE=False; /* Option O */
  bool FORTRAN_STYLE=False; /* Option F */
  bool HISTORY_APPEND=True; /* Option h */
  bool NCAR_CSM_FORMAT;
  bool PROCESS_ALL_COORDINATES=False; /* Option c */
  bool PROCESS_ASSOCIATED_COORDINATES=True; /* Option C */
  bool REMOVE_REMOTE_FILES_AFTER_PROCESSING=True; /* Option R */

  char **var_lst_in=NULL_CEWI;
  char **fl_lst_abb=NULL; /* Option n */
  char **fl_lst_in;
  char *fl_spt=NULL; /* Option s */
  char *fl_in=NULL;
  char *fl_pth_lcl=NULL; /* Option l */
  char *lmt_arg[NC_MAX_DIMS];
  char *spt_arg[73];
  char *spt_arg_cat=NULL;
  char *opt_sng;
  char *fl_out;
  char *fl_out_tmp;
  char *fl_pth=NULL; /* Option p */
  char *time_bfr_srt;
  char *cmd_ln;
  char CVS_Id[]="$Id: ncap.c,v 1.35 2001-11-29 16:08:37 hmb Exp $"; 
  char CVS_Revision[]="$Revision: 1.35 $";
  
  dmn_sct **dim;
  dmn_sct **dmn_out;
  
  extern char *optarg;
  extern int optind;
  
  
  int idx;
  int in_id;  
  int out_id;  
  int nbr_abb_arg=0;
  int nbr_dmn_fl;
  int lmt_nbr=0; /* Option d. NB: lmt_nbr gets incremented */
  int nbr_spt=0; /* Option s. NB: nbr_spt gets incremented */
  int nbr_var_fl;
  int nbr_var_fix; /* nbr_var_fix gets incremented */
  int nbr_var_prc; /* nbr_var_prc gets incremented */
  int nbr_xtr=0; /* nbr_xtr won't otherwise be set for -c with no -v */
  int nbr_xtr_2=0;
  int nbr_dmn_xtr;
  int nbr_fl=0;
  int nbr_lst_a=0;
  int nbr_lst_b=0;
  int nbr_lst_c=0;
  int opt;
  int rec_dmn_id=NCO_REC_DMN_UNDEFINED;
  int rcd=NC_NOERR; /* [rcd] Return code */
  int slen;
  int var_id;
  int spt_arg_len;
  int nbr_att=0;    /* Contains the size of att_lst */ 

  lmt_sct *lmt=NULL_CEWI;
  
  nm_id_sct *dmn_lst;
  nm_id_sct *xtr_lst=NULL;
  nm_id_sct *xtr_lst_2=NULL;
 
  nm_id_sct *xtr_lst_a=NULL;
  nm_id_sct *xtr_lst_b=NULL;
  nm_id_sct *xtr_lst_c=NULL;
  time_t clock;
  
  var_sct **var;
  var_sct **var_fix;
  var_sct **var_fix_out;
  var_sct **var_out;
  var_sct **var_prc;
  var_sct **var_prc_out;
  
  aed_sct *att_lst[500]; /* Structure filled out by yyparse , contains attributes to write to disk */
                         /* Can be realloced in yyparse */
  prs_sct prs_arg;

 
    /* Start the clock and save the command line */ 
  cmd_ln=cmd_ln_sng(argc,argv);
  clock=time((time_t *)NULL);
  time_bfr_srt=ctime(&clock); time_bfr_srt=time_bfr_srt; /* Avoid compiler warning until variable is used for something */

  /* Get program name and set program enum (e.g., prg=ncra) */
  prg_nm=prg_prs(argv[0],&prg);

  /* Parse command line arguments */

  opt_sng="ACcD:d:Fhl:n:Op:r:s:S:v:x";
  
 while( (opt=getopt(argc,argv,opt_sng))!= EOF){
   switch(opt){
    case 'A': /* Toggle FORCE_APPEND */
      FORCE_APPEND=!FORCE_APPEND;
      break;
    case 'C': /* Extraction list should include all coordinates associated with extracted variables? */
      PROCESS_ASSOCIATED_COORDINATES=False;
      break;
    case 'c':
      PROCESS_ALL_COORDINATES=True;
      break;
    case 'D': /* Debugging level. Default is 0. */
      dbg_lvl=(unsigned short)strtol(optarg,(char **)NULL,10);
      break;
    case 'd': /* Copy argument for later processing */
      lmt_arg[lmt_nbr]=(char *)strdup(optarg);
      lmt_nbr++;
      break;
    case 'F': /* Toggle index convention. Default is 0-based arrays (C-style). */
      FORTRAN_STYLE=!FORTRAN_STYLE;
      break;
    case 'h': /* Toggle appending to history global attribute */
      HISTORY_APPEND=!HISTORY_APPEND;
      break;
    case 'l': /* Local path prefix for files retrieved from remote file system */
      fl_pth_lcl=optarg;
      break;
    case 'n': /* Get the NINTAP-style abbreviation of files to average */
      fl_lst_abb=lst_prs(optarg,",",&nbr_abb_arg);
      if(nbr_abb_arg < 1 || nbr_abb_arg > 3){
	(void)fprintf(stdout,"%s: ERROR Incorrect abbreviation for file list\n",prg_nm);
	(void)usg_prn();
	exit(EXIT_FAILURE);
      } /* end if */
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
      (void)nco_lib_vrs_prn();
      exit(EXIT_SUCCESS);
      break;
    case 's': /* Copy command script for later processing */
      spt_arg[nbr_spt++]= strdup(optarg);
      break;
    case 'S': /* Read command script from file rather than from command line */
      fl_spt=optarg;
      break;
    case 'v': /* Variables to extract/exclude */
      var_lst_in=lst_prs(optarg,",",&nbr_xtr);
      break;
    case 'x': /* Exclude rather than extractvariables specified with -v */
      EXCLUDE_INPUT_LIST=True;
      break;
    default: /* Print proper usage */
      (void)usg_prn();
      exit(EXIT_FAILURE);
      break;
    } /* end switch */
  } /* end while loop */
  
  /* append the string  ";\n" to the command-sript args and then concatenate them */

  for(idx = 0 ; idx < nbr_spt ; idx++){
    slen = strlen (spt_arg[idx]);
    if(idx == 0){
      spt_arg_cat = nco_malloc(slen+3);
      strcpy(spt_arg_cat,spt_arg[idx]);
      strcat(spt_arg_cat,";\n");
      spt_arg_len = slen + 3;
      } else { 
      spt_arg_len+=slen+2;
      spt_arg_cat = nco_realloc(spt_arg_cat,spt_arg_len);
      strcat(spt_arg_cat,spt_arg[idx]);
      strcat(spt_arg_cat,";\n");
      } /* end else */
  } /* end if */    

  /* Process positional arguments and fill in filenames */
  fl_lst_in=fl_lst_mk(argv,argc,optind,&nbr_fl,&fl_out);

  /* Make uniform list of user-specified dimension limits */
  if(lmt_nbr > 0) lmt=lmt_prs(lmt_nbr,lmt_arg);
  
  /* Parse filename */
  fl_in=fl_nm_prs(fl_in,0,&nbr_fl,fl_lst_in,nbr_abb_arg,fl_lst_abb,fl_pth);
  /* Make sure file is on local system and is readable or die trying */
  fl_in=fl_mk_lcl(fl_in,fl_pth_lcl,&FILE_RETRIEVED_FROM_REMOTE_LOCATION);
  /* Open file for reading */
  rcd=nco_open(fl_in,NC_NOWRITE,&in_id);
  
  /* Create three lists of variables    */
  /* list a variables on the RHS of an = sign    */
  /* list b variables on the LHS                 */
  /* list c variables of attributes on the LHS   */
  /* All the variables are present in the input file */

  prs_arg.fl_in=fl_in;
  prs_arg.in_id=in_id;
  prs_arg.fl_out=NULL;
  prs_arg.out_id=-1;
  prs_arg.fl_spt = fl_spt;
  prs_arg.att_lst = att_lst;
  prs_arg.nbr_att =&nbr_att;
  prs_arg.dim=dim;
  prs_arg.nbr_dmn_xtr=nbr_dmn_xtr;
  prs_arg.inital_scan = True;
  
  (void)ncap_initial_scan(&prs_arg,spt_arg_cat,&xtr_lst_a,&nbr_lst_a,&xtr_lst_b,&nbr_lst_b,&xtr_lst_c, &nbr_lst_c);

  /* Get number of variables, dimensions, and record dimension ID of input file */
  rcd=nco_inq(in_id,&nbr_dmn_fl,&nbr_var_fl,(int *)NULL,&rec_dmn_id);
    
  /* Form initial extraction list from user input */
  xtr_lst=var_lst_mk(in_id,nbr_var_fl,var_lst_in,PROCESS_ALL_COORDINATES,&nbr_xtr);

  /* Change included variables to excluded variables */
  if(EXCLUDE_INPUT_LIST) xtr_lst=var_lst_xcl(in_id,nbr_var_fl,xtr_lst,&nbr_xtr);
  
  /* now add list c to the extraction list  */
  if( nbr_lst_c >0)  xtr_lst=var_lst_add(in_id,xtr_lst,&nbr_xtr,xtr_lst_c,nbr_lst_c);

  /* now add list a to the extraction list   */
  if( nbr_lst_a >0 ) xtr_lst=var_lst_add(in_id,xtr_lst,&nbr_xtr,xtr_lst_a,nbr_lst_a);

  /* Add all coordinate variables to extraction list */
  if(PROCESS_ALL_COORDINATES) xtr_lst=var_lst_add_crd(in_id,nbr_var_fl,nbr_dmn_fl,xtr_lst,&nbr_xtr);

  /* Make sure coordinates associated extracted variables are also on extraction list */
  if(PROCESS_ASSOCIATED_COORDINATES) xtr_lst=var_lst_ass_crd_add(in_id,xtr_lst,&nbr_xtr);

  /* Remove record coordinate, if any, from extraction list */
  if(False) xtr_lst=var_lst_crd_xcl(in_id,rec_dmn_id,xtr_lst,&nbr_xtr);

 /* Finally, heapsort extraction list by variable ID for fastest I/O */
  if(nbr_xtr > 1) xtr_lst=lst_heapsort(xtr_lst,nbr_xtr,False);


  /* Make a copy of the list for later */
  xtr_lst_2 = var_lst_copy(xtr_lst,nbr_xtr);
  nbr_xtr_2 = nbr_xtr;

  /* now subtract  list a from the copied list   */
  if( nbr_lst_a >0 ) xtr_lst_2=var_lst_sub(in_id,xtr_lst_2,&nbr_xtr_2,xtr_lst_a,nbr_lst_a);
  /* New subtract list b  */
  if( nbr_lst_b >0)  xtr_lst_2=var_lst_sub(in_id,xtr_lst_2,&nbr_xtr_2,xtr_lst_b,nbr_lst_b);
 
 /* Finally, heapsort extraction list by variable ID for fastest I/O */
  if(nbr_xtr_2 > 1) xtr_lst_2=lst_heapsort(xtr_lst_2,nbr_xtr_2,False);

     
  /* Find coordinate/dimension values associated with user-specified limits */
  for(idx=0;idx<lmt_nbr;idx++) (void)lmt_evl(in_id,lmt+idx,0L,FORTRAN_STYLE);
  
  /* Find dimensions associated with variables to be extracted */
  dmn_lst=dmn_lst_ass_var(in_id,xtr_lst,nbr_xtr,&nbr_dmn_xtr);

  /* Fill in dimension structure for all extracted dimensions */
  dim=(dmn_sct **)nco_malloc(nbr_dmn_xtr*sizeof(dmn_sct *));
  for(idx=0;idx<nbr_dmn_xtr;idx++) dim[idx]=dmn_fll(in_id,dmn_lst[idx].id,dmn_lst[idx].nm);
  
  /* Merge hyperslab limit information into dimension structures */
  if(lmt_nbr > 0) (void)dmn_lmt_mrg(dim,nbr_dmn_xtr,lmt,lmt_nbr);

  /* Duplicate input dimension structures for output dimension structures */
  dmn_out=(dmn_sct **)nco_malloc(nbr_dmn_xtr*sizeof(dmn_sct *));
  for(idx=0;idx<nbr_dmn_xtr;idx++){
    dmn_out[idx]=dmn_dpl(dim[idx]);
    (void)dmn_xrf(dim[idx],dmn_out[idx]); 
  } /* end loop over idx */

  /* Is this an NCAR CSM-format history tape? */
  NCAR_CSM_FORMAT=ncar_csm_inq(in_id);


  /* Fill in variable structure list for all extracted variables */
  var=(var_sct **)nco_malloc(nbr_xtr_2*sizeof(var_sct *));
  var_out=(var_sct **)nco_malloc(nbr_xtr_2*sizeof(var_sct *));
  for(idx=0;idx<nbr_xtr_2;idx++){
    var[idx]=var_fll(in_id,xtr_lst_2[idx].id,xtr_lst_2[idx].nm,dim,nbr_dmn_xtr);
    var_out[idx]=var_dpl(var[idx]);
    (void)var_xrf(var[idx],var_out[idx]);
    (void)var_dmn_xrf(var_out[idx]);
  } /* end loop over idx */

  /* NB: ncap is not suited for var_lst_divide() */
  /* Divide variable lists into lists of fixed variables and variables to be processed */
    (void)var_lst_divide(var,var_out,nbr_xtr_2,NCAR_CSM_FORMAT,(dmn_sct **)NULL,0,&var_fix,&var_fix_out,&nbr_var_fix,&var_prc,&var_prc_out,&nbr_var_prc);

  /* Open output file */
  fl_out_tmp=fl_out_open(fl_out,FORCE_APPEND,FORCE_OVERWRITE,&out_id);

  /* Copy global attributes */
  (void)att_cpy(in_id,out_id,NC_GLOBAL,NC_GLOBAL);
  
  /* Catenate time-stamped command line to "history" global attribute */
  if(HISTORY_APPEND) (void)hst_att_cat(out_id,cmd_ln);
  (void)dmn_dfn(fl_out,out_id,dmn_out,nbr_dmn_xtr);


  (void)var_dfn(in_id,fl_out,out_id,var_out,nbr_xtr_2,(dmn_sct **)NULL,0);
  //(void)var_dfn(in_id,fl_out,out_id,var_fix,nbr_var_fix,(dmn_sct **)NULL,0);
  
  /* Turn off default filling behavior to enhance efficiency */
  rcd=nc_set_fill(out_id,NC_NOFILL,(int *)NULL);
  
  /* Take output file out of define mode */
  rcd=nco_enddef(out_id);
  
  /* Copy variable data for non-processed variables */
  (void)var_val_cpy(in_id,out_id,var_fix,nbr_var_fix);
  //(void)var_val_cpy(in_id,out_id,var_out,nbr_xtr_2);

  /* Set arguments to parser */
  prs_arg.fl_in=fl_in;
  prs_arg.in_id=in_id;
  prs_arg.fl_out=fl_out;
  prs_arg.out_id=out_id;
  prs_arg.fl_spt = fl_spt;
  prs_arg.att_lst = att_lst;
  prs_arg.nbr_att =&nbr_att;
  prs_arg.dim=dmn_out;
  prs_arg.nbr_dmn_xtr=nbr_dmn_xtr;
  prs_arg.inital_scan = False;

  if(fl_spt == NULL){
    if(nbr_spt == 0){
      (void)fprintf(stderr,"%s: ERROR must supply derived field scripts\n",prg_nm_get());
      exit(EXIT_FAILURE);
    } /* end if */
   
    if(dbg_lvl > 0) 
       for(idx = 0 ; idx< nbr_spt ;idx ++)
       (void)fprintf(stderr,"spt_arg[%d] = %s\n",idx,spt_arg[idx]);
    fl_spt_global = "Arg";
    line_number = 1;
    yy_scan_string(spt_arg_cat);
    rcd=yyparse((void *)&prs_arg);
  }else{
    /* Open script file for reading */
    if((yyin=fopen(fl_spt,"r")) == NULL){
      (void)fprintf(stderr,"%s: ERROR Unable to open script file %s\n",prg_nm_get(),fl_spt);
      exit(EXIT_FAILURE);
    } /* end if */
    /* Invoke parser on script file */
    fl_spt_global = fl_spt;
    line_number = 1;
    rcd=yyparse((void *)&prs_arg);
  } /* end else */
  /* Define dimensions in output file */

   
  rcd = nco_redef(out_id);
  
  /* Copy new attributes overwriting old ones */
  for( idx =0 ; idx < nbr_att ; idx++) {
    rcd = nco_inq_varid_flg(out_id ,att_lst[idx]->var_nm,&var_id);
    if( rcd == NC_NOERR) {
      att_lst[idx]->mode = aed_overwrite;
      (void)aed_prc(out_id,var_id,*att_lst[idx]);
    } /* end if */
  } /* end for */
  
  rcd = nco_enddef(out_id);
  /* Close input netCDF file */
  rcd=nco_close(in_id);
  
  /* Remove local copy of file */
  if(FILE_RETRIEVED_FROM_REMOTE_LOCATION && REMOVE_REMOTE_FILES_AFTER_PROCESSING) (void)fl_rm(fl_in);

  /* Close output file and move it from temporary to permanent location */
  (void)fl_out_cls(fl_out,fl_out_tmp,out_id);

  Exit_gracefully();
  return EXIT_SUCCESS;

} /* end main() */





