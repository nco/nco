/* $Header: /data/zender/nco_20150216/nco/src/nco/ncrename.c,v 1.8 1999-10-04 05:13:36 zender Exp $ */

/* ncrename -- netCDF renaming operator */

/* (c) Copyright 1995--1999 University Corporation for Atmospheric Research 
   The file LICENSE contains the full copyright notice 
   Contact NSF/UCAR/NCAR/CGD/CMS for copyright assistance */

/* Purpose: Rename dimensions, variables, and attributes from a netCDF file. */

/* Usage:
   ncrename -d old_dim1,new_dim1 -v old_var1,new_var1 -v old_var2,new_var2 -a old_att1,new_att1 in.nc foo.nc
   ncrename -d lon,new_lon -v scalar_var,new_scalar_var -a long_name,new_long_name in.nc foo.nc
   */ 

/* Standard header files */
#include <math.h>               /* sin cos cos sin 3.14159 */
#include <netcdf.h>             /* netCDF def'ns */
#include <stdio.h>              /* stderr, FILE, NULL, etc. */
#include <stdlib.h>             /* atof, atoi, malloc, getopt */ 
#include <string.h>             /* strcmp. . . */
#include <sys/stat.h>           /* stat() */
#include <time.h>               /* machine time */
#include <unistd.h>             /* all sorts of POSIX stuff */ 
/* #include <assert.h> */            /* assert() debugging macro */ 
/* #include <errno.h> */             /* errno */
/* #include <malloc.h>    */         /* malloc() stuff */
/*#include <curses.h>*/

/* #define MAIN_PROGRAM_FILE MUST precede #include nc.h */
#define MAIN_PROGRAM_FILE
#include "nc.h"                 /* global definitions */

int 
main(int argc,char **argv)
{
/*  int initscr(void);*/
/*  int cbreak(void);*/

  rnm_sct *prs_rnm_lst(int,char **);

  bool OUTPUT_TO_NEW_NETCDF_FILE=False;
  bool FORCE_APPEND=False; /* Option A */ 
  bool FORCE_OVERWRITE=False; /* Option O */ 
  bool FILE_RETRIEVED_FROM_REMOTE_LOCATION;
  bool HISTORY_APPEND=True; /* Option h */
  bool REMOVE_REMOTE_FILES_AFTER_PROCESSING=True; /* Option R */ 
  
  char **fl_lst_abb=NULL; /* Option a */ 
  char **fl_lst_in;
  char *fl_in=NULL;
  char *dim_rnm_arg[MAX_NC_DIMS];
  char *var_rnm_arg[MAX_NC_VARS];
  char *att_rnm_arg[MAX_NC_ATTRS];
  char *opt_sng;
  char *fl_out;
  char *fl_pth_lcl=NULL; /* Option l */ 
  char *fl_pth=NULL; /* Option p */ 
  char *time_buf_srt;
  char *cmd_ln;
  char CVS_Id[]="$Id: ncrename.c,v 1.8 1999-10-04 05:13:36 zender Exp $"; 
  char CVS_Revision[]="$Revision: 1.8 $";
  
  rnm_sct *var_rnm_lst=NULL_CEWI;
  rnm_sct *dim_rnm_lst=NULL_CEWI;
  rnm_sct *att_rnm_lst=NULL_CEWI;

  extern char *optarg;
  extern int ncopts;
  extern int optind;
  
  int idx;
  int nc_id;  
  int nbr_abb_arg=0;
  int nbr_var_rnm=0; /* Option v. NB: nbr_var_rnm gets incremented */
  int nbr_att_rnm=0; /* Option a. NB: nbr_var_rnm gets incremented */
  int nbr_dim_rnm=0; /* Option d. NB: nbr_var_rnm gets incremented */
  int nbr_fl=0;
  int opt;
  
  time_t clock;

  /* Start the clock and save the command line */  
  cmd_ln=cmd_ln_sng(argc,argv);
  clock=time((time_t *)NULL);
  time_buf_srt=ctime(&clock);
  
  /* Get the program name and set the enum for the program (e.g., prg=ncra) */
  prg_nm=prg_prs(argv[0],&prg);

  /* Parse command line arguments */
  opt_sng="a:D:d:hl:Op:rv:";
  while((opt = getopt(argc,argv,opt_sng)) != EOF){
    switch(opt){
    case 'A': /* Toggle FORCE_APPEND */
      FORCE_APPEND=!FORCE_APPEND;
      break;
    case 'a': /* Copy the argument for later processing */ 
      att_rnm_arg[nbr_att_rnm]=optarg;
      nbr_att_rnm++;
      break;
    case 'D': /* The debugging level.  Default is 0. */
      dbg_lvl=atoi(optarg);
      break;
    case 'd': /* Copy the argument for later processing */ 
      dim_rnm_arg[nbr_dim_rnm]=optarg;
      nbr_dim_rnm++;
      break;
    case 'h': /* Toggle appending to history global attribute */
      HISTORY_APPEND=!HISTORY_APPEND;
      break;
    case 'l': /* Get the local path prefix for storing files retrieved from the remote file system */
      fl_pth_lcl=optarg;
      break;
    case 'O': /* Toggle FORCE_OVERWRITE */
      FORCE_OVERWRITE=!FORCE_OVERWRITE;
      break;
    case 'p': /* Get the path prefix */
      fl_pth=optarg;
      break;
    case 'R': /* Toggle the removal of remotely-retrieved-files after processing. Default is True */
      REMOVE_REMOTE_FILES_AFTER_PROCESSING=!REMOVE_REMOTE_FILES_AFTER_PROCESSING;
      break;
    case 'r': /* Print the CVS program info and copyright notice */
      (void)copyright_prn(CVS_Id,CVS_Revision);
      (void)nc_lib_vrs_prn();
      exit(EXIT_SUCCESS);
      break;
    case 'v': /* Copy the argument for later processing */ 
      var_rnm_arg[nbr_var_rnm]=optarg;
      nbr_var_rnm++;
      break;
    default: /* Print proper usage */
      (void)usg_prn();
      exit(EXIT_FAILURE);
    } /* end switch */
  } /* end while loop */
  
  /* Process the positional arguments and fill in the filenames */
  fl_lst_in=fl_lst_mk(argv,argc,optind,&nbr_fl,&fl_out);
  if(fl_out != NULL) OUTPUT_TO_NEW_NETCDF_FILE=True; else fl_out=fl_lst_in[0];

  if(nbr_var_rnm == 0 && nbr_att_rnm == 0 && nbr_dim_rnm == 0){
    (void)fprintf(stdout,"%s: ERROR must specify something to rename\n",prg_nm);
    usg_prn();
    exit(EXIT_FAILURE);
  } /* end if */  

  /* Make a uniform list of the user-specified rename structures */ 
  if(nbr_var_rnm > 0) var_rnm_lst=prs_rnm_lst(nbr_var_rnm,var_rnm_arg);
  if(nbr_dim_rnm > 0) dim_rnm_lst=prs_rnm_lst(nbr_dim_rnm,dim_rnm_arg);
  if(nbr_att_rnm > 0) att_rnm_lst=prs_rnm_lst(nbr_att_rnm,att_rnm_arg);

  /* We now have the final list of variables, dimensions, and attributes to rename. */
  
  /* Parse the filename */ 
  fl_in=fl_nm_prs(fl_in,0,&nbr_fl,fl_lst_in,nbr_abb_arg,fl_lst_abb,fl_pth);
  /* Make sure the file is on the local system and is readable or die trying */ 
  fl_in=fl_mk_lcl(fl_in,fl_pth_lcl,&FILE_RETRIEVED_FROM_REMOTE_LOCATION);

  if(OUTPUT_TO_NEW_NETCDF_FILE){
/*    printf( "Overwrite ? [yn] " );
    inpt = '\0';
    if ( ( inpt = getch( ) ) == EOF ) {
        fprintf( stdout, "EOF found on stdin\n" );
    } else {
        switch ( inpt ) {
        case  'y':
        case  'Y':
            printf( "\nfile will be overwritten\n" );
            break;
        default:
            printf( "\nfile will *NOT* be overwritten\n" );
            break;
        }
    } */

    if(!FORCE_OVERWRITE){
      int rcd;

      struct stat stat_sct;
      
      rcd=stat(fl_out,&stat_sct);

      /* If the file already exists, then query the user whether to overwrite */
      if(rcd != -1){
        char usr_reply;
        
        usr_reply='z';
        while(usr_reply != 'n' && usr_reply != 'y'){
          (void)fprintf(stdout,"ncrename: overwrite %s (y/n)? ",fl_out);
	  (void)fflush(stdout);
/*          (void)setvbuf(stdin,NULL,_IONBF,0);*/
/*          usr_reply=(char)getc(stdin);*/
          usr_reply=(char)fgetc(stdin);
/*          usr_reply=(char)getchar();*/
/*          (void)read((int)stdin,&usr_reply,1);*/
        } /* end while */ 
        
        if(usr_reply == 'n'){
          exit(EXIT_SUCCESS);
        } /* end if */
      } /* end if */
    } /* end if */
    
    /* Copy the input file to the output file and then search through
       the output, changing names as you go. This avoids the possible XDR translation
       performance penalty of copying each variable with netCDF. */
    (void)fl_cp(fl_in,fl_out);

  } /* end if */ 

  /* Make netCDF errors fatal and print the diagnostic */   
  ncopts=NC_VERBOSE | NC_FATAL; 

  /* Open the file. Writing must be enabled and the file should be in define mode for renaming */ 
  nc_id=ncopen(fl_out,NC_WRITE);
  (void)ncredef(nc_id);

  /* Without further ado, change the names */ 
  for(idx=0;idx<nbr_var_rnm;idx++){
    if(var_rnm_lst[idx].old_nm[0] == '.'){
      ncopts=0;
      var_rnm_lst[idx].id=ncvarid(nc_id,var_rnm_lst[idx].old_nm+1);
      if(var_rnm_lst[idx].id != -1){
	(void)ncvarrename(nc_id,var_rnm_lst[idx].id,var_rnm_lst[idx].new_nm);
	if(dbg_lvl > 0) (void)fprintf(stderr,"Renamed variable \"%s\" to \"%s\"\n",var_rnm_lst[idx].old_nm+1,var_rnm_lst[idx].new_nm);
      }else{
	(void)fprintf(stderr,"%s: WARNING Variable \"%s\" not present in %s, skipping it.\n",prg_nm,var_rnm_lst[idx].old_nm+1,fl_in);
      } /* end if */ 
      ncopts=NC_VERBOSE | NC_FATAL; 
    }else{
      var_rnm_lst[idx].id=ncvarid(nc_id,var_rnm_lst[idx].old_nm);
      (void)ncvarrename(nc_id,var_rnm_lst[idx].id,var_rnm_lst[idx].new_nm);
      if(dbg_lvl > 0) (void)fprintf(stderr,"Renamed variable \"%s\" to \"%s\"\n",var_rnm_lst[idx].old_nm,var_rnm_lst[idx].new_nm);
    } /* end else */ 
  } /* end loop over idx */

  for(idx=0;idx<nbr_dim_rnm;idx++){
    if(dim_rnm_lst[idx].old_nm[0] == '.'){
      ncopts=0;
      dim_rnm_lst[idx].id=ncdimid(nc_id,dim_rnm_lst[idx].old_nm+1);
      if(dim_rnm_lst[idx].id != -1){
	(void)ncdimrename(nc_id,dim_rnm_lst[idx].id,dim_rnm_lst[idx].new_nm);
	if(dbg_lvl > 0) (void)fprintf(stderr,"Renamed dimension \"%s\" to \"%s\"\n",dim_rnm_lst[idx].old_nm+1,dim_rnm_lst[idx].new_nm);
      }else{
	(void)fprintf(stderr,"%s: WARNING Dimension \"%s\" not present in %s, skipping it.\n",prg_nm,dim_rnm_lst[idx].old_nm+1,fl_in);
      } /* end if */ 
      ncopts=NC_VERBOSE | NC_FATAL; 
    }else{
      dim_rnm_lst[idx].id=ncdimid(nc_id,dim_rnm_lst[idx].old_nm);
      (void)ncdimrename(nc_id,dim_rnm_lst[idx].id,dim_rnm_lst[idx].new_nm);
      if(dbg_lvl > 0) (void)fprintf(stderr,"Renamed dimension \"%s\" to \"%s\"\n",dim_rnm_lst[idx].old_nm,dim_rnm_lst[idx].new_nm);
    } /* end else */ 
  } /* end loop over idx */

  if(nbr_att_rnm > 0){
    int nbr_var_fl;
    
    /* Get the number of variables in the file */
    (void)ncinquire(nc_id,(int *)NULL,&nbr_var_fl,(int *)NULL,(int *)NULL);

    ncopts=0;
    for(idx=0;idx<nbr_att_rnm;idx++){
      int var_id;
      int nbr_rnm=0;
      
      for(var_id=-1;var_id<nbr_var_fl;var_id++){
	if(att_rnm_lst[idx].old_nm[0] == '.'){

	  att_rnm_lst[idx].id=ncattinq(nc_id,var_id,att_rnm_lst[idx].old_nm+1,(nc_type *)NULL,(int *)NULL);
	  if(att_rnm_lst[idx].id != -1){
	    (void)ncattrename(nc_id,var_id,att_rnm_lst[idx].old_nm+1,att_rnm_lst[idx].new_nm);
	    nbr_rnm++;

	    /* There can be many attributes of the same name in a file, so
	       tell the user what was actually changed */
	    if(var_id > -1){
	      char var_nm[MAX_NC_NAME];
	    
	      (void)ncvarinq(nc_id,var_id,var_nm,(nc_type *)NULL,(int *)NULL,(int *)NULL,(int *)NULL);
	      if(dbg_lvl > 0) (void)fprintf(stderr,"Renamed attribute \"%s\" to \"%s\" for variable \"%s\"\n",att_rnm_lst[idx].old_nm+1,att_rnm_lst[idx].new_nm,var_nm);
	    }else{
	      if(dbg_lvl > 0) (void)fprintf(stderr,"Renamed global attribute \"%s\" to \"%s\"\n",att_rnm_lst[idx].old_nm+1,att_rnm_lst[idx].new_nm);
	    } /* end else */
	  } /* end if */ 
	}else{
	  att_rnm_lst[idx].id=ncattinq(nc_id,var_id,att_rnm_lst[idx].old_nm,(nc_type *)NULL,(int *)NULL);
	  if(att_rnm_lst[idx].id != -1){
	    (void)ncattrename(nc_id,var_id,att_rnm_lst[idx].old_nm,att_rnm_lst[idx].new_nm);
	    nbr_rnm++;

	    /* There can be many attributes of the same name in a file, so
	       tell the user what was actually changed */
	    if(var_id > -1){
	      char var_nm[MAX_NC_NAME];
	    
	      (void)ncvarinq(nc_id,var_id,var_nm,(nc_type *)NULL,(int *)NULL,(int *)NULL,(int *)NULL);
	      if(dbg_lvl > 0) (void)fprintf(stderr,"Renamed attribute \"%s\" to \"%s\" for variable \"%s\"\n",att_rnm_lst[idx].old_nm,att_rnm_lst[idx].new_nm,var_nm);
	    }else{
	      if(dbg_lvl > 0) (void)fprintf(stderr,"Renamed global attribute \"%s\" to \"%s\"\n",att_rnm_lst[idx].old_nm,att_rnm_lst[idx].new_nm);
	    } /* end else */
	  } /* end if */ 
	} /* end else */ 
      } /* end loop over var_id */

      /* See to it that any mandatory renaming was performed, else abort. */ 
      if(nbr_rnm == 0){
	if(att_rnm_lst[idx].old_nm[0] == '.'){
	  (void)fprintf(stderr,"%s: WARNING Attribute \"%s\" not present in %s, skipping it.\n",prg_nm,att_rnm_lst[idx].old_nm+1,fl_in);
	}else{
	  (void)fprintf(stdout,"%s: ERROR Attribute \"%s\" not present in %s, aborting.\n",prg_nm,att_rnm_lst[idx].old_nm,fl_in);
	  exit(EXIT_FAILURE);
	} /* end else */
      } /* end if */

    } /* end loop over idx */
    ncopts=NC_VERBOSE | NC_FATAL; 
  }/* end if */ 
  
  /* Catenate the timestamped command line to the "history" global attribute */ 
  if(HISTORY_APPEND) (void)hst_att_cat(nc_id,cmd_ln);
  
  /* Take the file out of define mode */ 
  (void)ncendef(nc_id);
    
  /* Close the open netCDF file */ 
  ncclose(nc_id);
  
  /* Dispose of the local copy of the file */ 
  if(FILE_RETRIEVED_FROM_REMOTE_LOCATION && REMOVE_REMOTE_FILES_AFTER_PROCESSING) (void)fl_rm(fl_in);

  Exit_gracefully();
  return EXIT_SUCCESS;
} /* end main() */

rnm_sct *
prs_rnm_lst(int nbr_rnm,char **rnm_arg)
{
  /* Routine to set the old_nm, new_nm elements of 
     a rename structure. This routine merely fills the 
     rename structure and does not attempt to validate 
     the presence of the variables in the input netCDF file. */

  void usg_prn(void);

  rnm_sct *rnm_lst;

  int idx;
  int len_arg_1;
  int len_arg_2;

  rnm_lst=(rnm_sct *)malloc(nbr_rnm*sizeof(rnm_sct));

  for(idx=0;idx<nbr_rnm;idx++){
    char *comma_1_cp;

    /* Find the positions of the commas and the number of characters
       between (non-inclusive) them */ 
    comma_1_cp=strchr(rnm_arg[idx],',');
    
    /* Before doing any pointer arithmetic, make sure the pointers 
       are valid. */ 
    if(comma_1_cp == NULL){
      (void)usg_prn();
      exit(EXIT_FAILURE);
    } /* end if */
    
    len_arg_1=comma_1_cp-rnm_arg[idx]; 
    len_arg_2=rnm_arg[idx]+strlen(rnm_arg[idx])-comma_1_cp-1; 
    
    /* If the length of either of the arguments is zero then exit */ 
    if(len_arg_1 <= 0 || len_arg_2 <= 0){
      (void)usg_prn();
      exit(EXIT_FAILURE);
    } /* end if */ 
    
    /* Assign the pointers to the member of the rnm_lst */ 
    rnm_lst[idx].old_nm=rnm_arg[idx];
    rnm_lst[idx].new_nm=comma_1_cp+1;

    /* Manually null-terminate the arguments */ 
    rnm_lst[idx].old_nm[len_arg_1]='\0';
    rnm_lst[idx].new_nm[len_arg_2]='\0';
    
  } /* End loop over rnm_lst */

  if(dbg_lvl == 5){
    for(idx=0;idx<nbr_rnm;idx++){
      (void)fprintf(stderr,"%s\n",rnm_lst[idx].old_nm);
      (void)fprintf(stderr,"%s\n",rnm_lst[idx].new_nm);
    } /* end loop over idx */
  } /* end debug */

  return rnm_lst;
} /* end prs_rnm_lst() */ 
