/* $Header: /data/zender/nco_20150216/nco/src/nco/ncrename.c,v 1.54 2003-04-07 02:46:40 zender Exp $ */

/* ncrename -- netCDF renaming operator */

/* Purpose: Rename dimensions, variables, and attributes of a netCDF file */

/* Copyright (C) 1995--2003 Charlie Zender

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
   ncrename -d old_dim1,new_dim1 -v old_var1,new_var1 -v old_var2,new_var2 -a old_att1,new_att1 in.nc foo.nc
   ncrename -d lon,new_lon -v scalar_var,new_scalar_var -a long_name,new_long_name in.nc foo.nc
   */

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
#ifdef HAVE_GETOPT_H
#include <getopt.h>  /* getopt_long() */
#else
#include "nco_getopt.h"
#endif /* HAVE_GETOPT_H */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions */
#include "nco_netcdf.h" /* netCDF 3.0 wrapper functions */

/* #define MAIN_PROGRAM_FILE MUST precede #include nco.h */
#define MAIN_PROGRAM_FILE
#include "libnco.h" /* netCDF operator library */

int 
main(int argc,char **argv)
{
  bool OUTPUT_TO_NEW_NETCDF_FILE=False;
  bool FORCE_APPEND=False; /* Option A */
  bool FORCE_OVERWRITE=False; /* Option O */
  bool FILE_RETRIEVED_FROM_REMOTE_LOCATION;
  bool HISTORY_APPEND=True; /* Option h */
  bool REMOVE_REMOTE_FILES_AFTER_PROCESSING=True; /* Option R */
  
  char **fl_lst_abb=NULL; /* Option a */
  char **fl_lst_in;
  char *fl_in=NULL;
  char *dmn_rnm_arg[NC_MAX_DIMS];
  char *var_rnm_arg[NC_MAX_VARS];
  char *att_rnm_arg[NC_MAX_ATTRS];
  char *opt_sng;
  char *fl_out;
  char *fl_pth_lcl=NULL; /* Option l */
  char *fl_pth=NULL; /* Option p */
  char *time_bfr_srt;
  char *cmd_ln;
  char CVS_Id[]="$Id: ncrename.c,v 1.54 2003-04-07 02:46:40 zender Exp $"; 
  char CVS_Revision[]="$Revision: 1.54 $";
  
  extern char *optarg;
  
  extern int optind;
  
  int idx;
  int nc_id;  
  int nbr_abb_arg=0;
  int nbr_var_rnm=0; /* Option v. NB: nbr_var_rnm gets incremented */
  int nbr_att_rnm=0; /* Option a. NB: nbr_att_rnm gets incremented */
  int nbr_dmn_rnm=0; /* Option d. NB: nbr_dmn_rnm gets incremented */
  int nbr_fl=0;
  int opt;
  int rcd=NC_NOERR; /* [rcd] Return code */
  
  rnm_sct *var_rnm_lst=NULL_CEWI;
  rnm_sct *dmn_rnm_lst=NULL_CEWI;
  rnm_sct *att_rnm_lst=NULL_CEWI;

  time_t clock;

  static struct option opt_lng[] =
    { /* Structure ordered by short option key if possible */
      {"attribute",required_argument,0,'a'},
      {"debug",required_argument,0,'D'},
      {"dbg_lvl",required_argument,0,'D'},
      {"dimension",required_argument,0,'d'},
      {"dmn",required_argument,0,'d'},
      {"history",no_argument,0,'h'},
      {"hst",no_argument,0,'h'},
      {"local",required_argument,0,'l'},
      {"lcl",required_argument,0,'l'},
      {"overwrite",no_argument,0,'O'},
      {"ovr",no_argument,0,'O'},
      {"path",required_argument,0,'p'},
      {"revision",no_argument,0,'r'},
      {"variable",required_argument,0,'v'},
      {"version",no_argument,0,'r'},
      {"vrs",no_argument,0,'r'},
      {"help",no_argument,0,'?'},
      {0,0,0,0}
    };
  int opt_idx=0;  /* Index of current long option into opt_lng array */

  /* Start clock and save command line */ 
  cmd_ln=nco_cmd_ln_sng(argc,argv);
  clock=time((time_t *)NULL);
  time_bfr_srt=ctime(&clock); time_bfr_srt=time_bfr_srt; /* Avoid compiler warning until variable is used for something */
  
  /* Get program name and set program enum (e.g., prg=ncra) */
  prg_nm=prg_prs(argv[0],&prg);

  /* Parse command line arguments */
  opt_sng="a:D:d:hl:Op:rv:-:";
  while((opt = getopt_long(argc,argv,opt_sng,opt_lng,&opt_idx)) != EOF){
    switch(opt){
    case 'A': /* Toggle FORCE_APPEND */
      FORCE_APPEND=!FORCE_APPEND;
      break;
    case 'a': /* Copy argument for later processing */
      att_rnm_arg[nbr_att_rnm]=optarg;
      nbr_att_rnm++;
      break;
    case 'D': /* Debugging level. Default is 0. */
      dbg_lvl=(unsigned short)strtol(optarg,(char **)NULL,10);
      break;
    case 'd': /* Copy argument for later processing */
      dmn_rnm_arg[nbr_dmn_rnm]=optarg;
      nbr_dmn_rnm++;
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
      (void)nco_lib_vrs_prn();
      nco_exit(EXIT_SUCCESS);
      break;
    case 'v': /* Copy argument for later processing */
      var_rnm_arg[nbr_var_rnm]=optarg;
      nbr_var_rnm++;
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

  if(nbr_var_rnm == 0 && nbr_att_rnm == 0 && nbr_dmn_rnm == 0){
    (void)fprintf(stdout,"%s: ERROR must specify something to rename\n",prg_nm);
    nco_usg_prn();
    nco_exit(EXIT_FAILURE);
  } /* end if */ 

  /* Make uniform list of user-specified rename structures */
  if(nbr_var_rnm > 0) var_rnm_lst=nco_prs_rnm_lst(nbr_var_rnm,var_rnm_arg);
  if(nbr_dmn_rnm > 0) dmn_rnm_lst=nco_prs_rnm_lst(nbr_dmn_rnm,dmn_rnm_arg);
  if(nbr_att_rnm > 0) att_rnm_lst=nco_prs_rnm_lst(nbr_att_rnm,att_rnm_arg);

  /* We have final list of variables, dimensions, and attributes to rename */
  
  /* Parse filename */
  fl_in=nco_fl_nm_prs(fl_in,0,&nbr_fl,fl_lst_in,nbr_abb_arg,fl_lst_abb,fl_pth);
  /* Make sure file is on local system and is readable or die trying */
  fl_in=nco_fl_mk_lcl(fl_in,fl_pth_lcl,&FILE_RETRIEVED_FROM_REMOTE_LOCATION);

  if(OUTPUT_TO_NEW_NETCDF_FILE){

    if(!FORCE_OVERWRITE){
      int rcd;

      struct stat stat_sct;
      
      rcd=stat(fl_out,&stat_sct);

      /* If file already exists, then query user whether to overwrite */
      if(rcd != -1){
        char usr_reply='z';
	short nbr_itr=0;

        while(usr_reply != 'n' && usr_reply != 'y'){
	  nbr_itr++;
	  if(nbr_itr > 10){
	    (void)fprintf(stdout,"\n%s: ERROR %hd failed attempts to obtain valid interactive input. Assuming non-interactive shell and exiting.\n",prg_nm_get(),nbr_itr-1);
	    nco_exit(EXIT_FAILURE);
	  } /* end if */
          (void)fprintf(stdout,"ncrename: overwrite %s (y/n)? ",fl_out);
	  (void)fflush(stdout);
          usr_reply=(char)fgetc(stdin);
	  /* Allow one carriage return per response free of charge */
	  if(usr_reply == '\n') usr_reply=(char)fgetc(stdin);
	  (void)fflush(stdin);
        } /* end while */
        
        if(usr_reply == 'n'){
          nco_exit(EXIT_SUCCESS);
        } /* end if */
      } /* end if */
    } /* end if */
    
    /* Copy input file to output file and then search through output, 
       changing names on the fly. This avoids possible XDR translation
       performance penalty of copying each variable with netCDF. */
    (void)nco_fl_cp(fl_in,fl_out);

  } /* end if */
  
  /* Open file. Writing must be enabled and file should be in define mode for renaming */
  rcd=nco_open(fl_out,NC_WRITE,&nc_id);
  (void)nco_redef(nc_id);

  /* Without further ado, change names */
  for(idx=0;idx<nbr_var_rnm;idx++){
    if(var_rnm_lst[idx].old_nm[0] == '.'){
      rcd=nco_inq_varid_flg(nc_id,var_rnm_lst[idx].old_nm+1,&var_rnm_lst[idx].id);
      if(rcd == NC_NOERR){
	(void)nco_rename_var(nc_id,var_rnm_lst[idx].id,var_rnm_lst[idx].new_nm);
	if(dbg_lvl > 0) (void)fprintf(stderr,"Renamed variable \"%s\" to \"%s\"\n",var_rnm_lst[idx].old_nm+1,var_rnm_lst[idx].new_nm);
      }else{
	(void)fprintf(stderr,"%s: WARNING Variable \"%s\" not present in %s, skipping it.\n",prg_nm,var_rnm_lst[idx].old_nm+1,fl_in);
      } /* end if */
    }else{ /* Variable name does not contain '.' so variable presence is required */
      rcd=nco_inq_varid(nc_id,var_rnm_lst[idx].old_nm,&var_rnm_lst[idx].id);
      (void)nco_rename_var(nc_id,var_rnm_lst[idx].id,var_rnm_lst[idx].new_nm);
      if(dbg_lvl > 0) (void)fprintf(stderr,"Renamed variable \"%s\" to \"%s\"\n",var_rnm_lst[idx].old_nm,var_rnm_lst[idx].new_nm);
    } /* end else */
  } /* end loop over idx */

  for(idx=0;idx<nbr_dmn_rnm;idx++){
    if(dmn_rnm_lst[idx].old_nm[0] == '.'){
      
      rcd=nco_inq_dimid_flg(nc_id,dmn_rnm_lst[idx].old_nm+1,&dmn_rnm_lst[idx].id);
      if(rcd == NC_NOERR){
	(void)nco_rename_dim(nc_id,dmn_rnm_lst[idx].id,dmn_rnm_lst[idx].new_nm);
	if(dbg_lvl > 0) (void)fprintf(stderr,"Renamed dimension \"%s\" to \"%s\"\n",dmn_rnm_lst[idx].old_nm+1,dmn_rnm_lst[idx].new_nm);
      }else{
	(void)fprintf(stderr,"%s: WARNING Dimension \"%s\" not present in %s, skipping it.\n",prg_nm,dmn_rnm_lst[idx].old_nm+1,fl_in);
      } /* end if */
      
    }else{
      rcd=nco_inq_dimid(nc_id,dmn_rnm_lst[idx].old_nm,&dmn_rnm_lst[idx].id);
      (void)nco_rename_dim(nc_id,dmn_rnm_lst[idx].id,dmn_rnm_lst[idx].new_nm);
      if(dbg_lvl > 0) (void)fprintf(stderr,"Renamed dimension \"%s\" to \"%s\"\n",dmn_rnm_lst[idx].old_nm,dmn_rnm_lst[idx].new_nm);
    } /* end else */
  } /* end loop over idx */

  if(nbr_att_rnm > 0){
    int nbr_var_fl;
    
    /* Get number of variables in file */
    (void)nco_inq(nc_id,(int *)NULL,&nbr_var_fl,(int *)NULL,(int *)NULL);

    for(idx=0;idx<nbr_att_rnm;idx++){
      int var_id;
      int nbr_rnm=0;
      
      /* Rename attribute of single variable... */
      if(strchr(att_rnm_lst[idx].old_nm,'@')){
	/* Get variable name from old name */
	char var_nm[NC_MAX_NAME];
	if(nco_prs_att((att_rnm_lst+idx),var_nm)){
	  /* Get var_id of variable */
	  if(var_nm[0] == '.'){
	    rcd=nco_inq_varid_flg(nc_id,var_nm+1,&var_id);
	    if(rcd != NC_NOERR){
	      (void)fprintf(stderr,"%s: WARNING Variable \"%s\" not present in %s, skipping it.\n",prg_nm,var_nm+1,fl_in);
	      continue;
	    } /* end if */
	  }else{ /* Variable name does not contain '.' so variable presence is required */
	    rcd=nco_inq_varid(nc_id,var_nm,&var_id);
	  } /* end if */
	  if(rcd == NC_NOERR){
	    if(att_rnm_lst[idx].old_nm[0] == '.'){
	      /* Preceding '.' means attribute may not be present */
	      rcd=nco_inq_attid_flg(nc_id,var_id,att_rnm_lst[idx].old_nm+1,&att_rnm_lst[idx].id);	  
	      if(rcd == NC_NOERR){
		(void)nco_rename_att(nc_id,var_id,att_rnm_lst[idx].old_nm+1,att_rnm_lst[idx].new_nm);
		nbr_rnm++;
		if(dbg_lvl > 0) (void)fprintf(stderr,"Renamed attribute \"%s\" to \"%s\" for variable \"%s\"\n",att_rnm_lst[idx].old_nm+1,att_rnm_lst[idx].new_nm,(var_nm[0] == '.' ? var_nm+1 : var_nm));
	      }else{
		(void)fprintf(stderr,"%s: WARNING Attribute \"%s\" not present in variable \"%s\"\n",prg_nm,att_rnm_lst[idx].old_nm+1,(var_nm[0] == '.' ? var_nm+1 : var_nm));
	      } /* endelse */
	    }else{ 
	      (void)fprintf(stderr,"DEBUGGING TODO 240. fxm 1: %d, %s\n",var_id,att_rnm_lst[idx].old_nm);
	      rcd=nco_inq_attid(nc_id,var_id,att_rnm_lst[idx].old_nm,&att_rnm_lst[idx].id);
	      if(rcd == NC_NOERR){
		(void)nco_rename_att(nc_id,var_id,att_rnm_lst[idx].old_nm,att_rnm_lst[idx].new_nm);
		nbr_rnm++;
		if(dbg_lvl > 0) (void)fprintf(stderr,"Renamed attribute \"%s\" to \"%s\" for variable \"%s\"\n",att_rnm_lst[idx].old_nm,att_rnm_lst[idx].new_nm,(var_nm[0] == '.' ? var_nm+1 : var_nm));
	      } /* endif attribute is present */
	    } /* endelse attribute must be present */  
	  }else{
	    (void)fprintf(stderr,"%s: WARNING variable \"%s\" not present in %s\n",prg_nm,var_nm,fl_in);
	  } /* endelse variable is present */
	} /* end if renaming single variable */
      }else{ /* ...or rename attribute for all variables... */
	for(var_id=-1;var_id<nbr_var_fl;var_id++){ /* Start loop at -1 for global attributes */
	  if(att_rnm_lst[idx].old_nm[0] == '.'){
	    /* Rename attribute if variable contains attribute else do nothing */
	    rcd=nco_inq_attid_flg(nc_id,var_id,att_rnm_lst[idx].old_nm+1,&att_rnm_lst[idx].id);
	    if(rcd == NC_NOERR){
	      (void)nco_rename_att(nc_id,var_id,att_rnm_lst[idx].old_nm+1,att_rnm_lst[idx].new_nm);
	      nbr_rnm++;
	      /* Inform user which variable had attribute renamed */
	      if(var_id > -1){
		char var_nm[NC_MAX_NAME];
		
		(void)nco_inq_varname(nc_id,var_id,var_nm);
		if(dbg_lvl > 0) (void)fprintf(stderr,"Renamed attribute \"%s\" to \"%s\" for variable \"%s\"\n",att_rnm_lst[idx].old_nm+1,att_rnm_lst[idx].new_nm,var_nm);
	      }else{
		if(dbg_lvl > 0) (void)fprintf(stderr,"Renamed global attribute \"%s\" to \"%s\"\n",att_rnm_lst[idx].old_nm+1,att_rnm_lst[idx].new_nm);
	      } /* end else */
	    } /* end if */
	    
	  }else{
	    /* Rename attribute or die trying */
/*	    (void)fprintf(stderr,"DEBUGGING TODO 240. fxm 2: %d, %s\n",var_id,att_rnm_lst[idx].old_nm); */
	    rcd=nco_inq_attid_flg(nc_id,var_id,att_rnm_lst[idx].old_nm,&att_rnm_lst[idx].id);
	    if(rcd == NC_NOERR){
	      (void)nco_rename_att(nc_id,var_id,att_rnm_lst[idx].old_nm,att_rnm_lst[idx].new_nm);
	      nbr_rnm++;
	      /* Inform user which variable had attribute renamed */
	      if(var_id > -1){
		char var_nm[NC_MAX_NAME];
		
		(void)nco_inq_varname(nc_id,var_id,var_nm);
		if(dbg_lvl > 0) (void)fprintf(stderr,"Renamed attribute \"%s\" to \"%s\" for variable \"%s\"\n",att_rnm_lst[idx].old_nm,att_rnm_lst[idx].new_nm,var_nm);
	      }else{
		if(dbg_lvl > 0) (void)fprintf(stderr,"Renamed global attribute \"%s\" to \"%s\"\n",att_rnm_lst[idx].old_nm,att_rnm_lst[idx].new_nm);
	      } /* end else */
	    } /* end if */
	  } /* end else */
	} /* end loop over var_id */
      } /* end if renaming attribute for all variables */
      /* See to it that any mandatory renaming was performed, else abort */
      if(nbr_rnm == 0){
	if(att_rnm_lst[idx].old_nm[0] == '.'){
	  (void)fprintf(stderr,"%s: WARNING Attribute \"%s\" not renamed because not found in searched variable(s)\n",prg_nm,att_rnm_lst[idx].old_nm+1);
	}else{
	  (void)fprintf(stdout,"%s: ERROR Attribute \"%s\" not present in %s, aborting.\n",prg_nm,att_rnm_lst[idx].old_nm,fl_in);
	  nco_exit(EXIT_FAILURE);
	} /* end else */
      } /* end if */
      
    } /* end loop over attributes to rename */
     
  } /* end if renaming attributes */
  
  /* Catenate timestamped command line to "history" global attribute */
  if(HISTORY_APPEND) (void)nco_hst_att_cat(nc_id,cmd_ln);
  
  /* Take file out of define mode */
  (void)nco_enddef(nc_id);
    
  /* Close the open netCDF file */
  nco_close(nc_id);
  
  /* Remove local copy of file */
  if(FILE_RETRIEVED_FROM_REMOTE_LOCATION && REMOVE_REMOTE_FILES_AFTER_PROCESSING) (void)nco_fl_rm(fl_in);

  nco_exit_gracefully();
  return EXIT_SUCCESS;
} /* end main() */

