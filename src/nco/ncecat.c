/* $Header: /data/zender/nco_20150216/nco/src/nco/ncecat.c,v 1.7 1999-05-13 03:06:03 zender Exp $ */

/* ncecat -- netCDF running averager */

/* (c) Copyright 1995--1999 University Corporation for Atmospheric Research 
   The file LICENSE contains the full copyright notice 
   Contact NSF/UCAR/NCAR/CGD/CMS for copyright assistance */

/* Purpose: Join variables across files by attaching them to a new record variable */

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

/* #define MAIN_PROGRAM_FILE MUST precede #include nc.h */
#define MAIN_PROGRAM_FILE
#include "nc.h"                 /* global definitions */

int 
main(int argc,char **argv)
{
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

  char **var_lst_in;
  char **fl_lst_abb=NULL; /* Option a */ 
  char **fl_lst_in;
  char *fl_in=NULL;
  char *fl_pth_lcl=NULL; /* Option l */ 
  char *lmt_arg[MAX_NC_DIMS];
  char *opt_sng;
  char *fl_out;
  char *fl_out_tmp;
  char *fl_pth=NULL; /* Option p */ 
  char *time_buf_srt;
  char *cmd_ln;
  char RCS_Id[]="$Id: ncecat.c,v 1.7 1999-05-13 03:06:03 zender Exp $"; 
  char RCS_Revision[]="$Revision: 1.7 $";
  
  dim_sct *rdim;
  dim_sct **dim;
  dim_sct **dim_out;
  
  extern char *optarg;
  extern int ncopts;
  extern int optind;

  int idx;
  int idx_fl;
  int in_id;  
  int out_id;  
  int nbr_abb_arg=0;
  int nbr_dim_fl;
  int nbr_lmt=0; /* Option d. NB: nbr_lmt gets incremented */
  int nbr_var_fl;
  int nbr_var_fix; /* nbr_var_fix gets incremented */ 
  int nbr_var_prc; /* nbr_var_prc gets incremented */ 
  int nbr_xtr=0; /* nbr_xtr won't otherwise be set for -c with no -v */ 
  int nbr_dim_xtr;
  int nbr_fl=0;
  int opt;
  int rec_dim_id=-1;
  
  lmt_sct *lmt;
  
  long idx_rec_out=0L; /* idx_rec_out gets incremented */ 
  
  nm_id_sct *dim_lst;
  nm_id_sct *xtr_lst=NULL; /* xtr_lst can get realloc()'d from NULL with -c option */ 
  
  time_t clock;
  
  var_sct **var;
  var_sct **var_fix;
  var_sct **var_fix_out;
  var_sct **var_out;
  var_sct **var_prc;
  var_sct **var_prc_out;
  
  /* Start the clock and save the command line */  
  cmd_ln=cmd_ln_sng(argc,argv);
  clock=time((time_t *)NULL);
  time_buf_srt=ctime(&clock);
  
  /* Get the program name and set the enum for the program (e.g., prg=ncra) */
  prg_nm=prg_prs(argv[0],&prg);

  /* Parse command line arguments */
  opt_sng="ACcD:d:Fhl:n:Op:rRv:x";
  while((opt = getopt(argc,argv,opt_sng)) != EOF){
    switch(opt){
    case 'A': /* Toggle FORCE_APPEND */
      FORCE_APPEND=!FORCE_APPEND;
      break;
    case 'C': /* Add to the extraction list any coordinates associated with variables to be extracted? */ 
      PROCESS_ASSOCIATED_COORDINATES=False;
      break;
    case 'c':
      PROCESS_ALL_COORDINATES=True;
      break;
    case 'D': /* The debugging level.  Default is 0. */
      dbg_lvl=atoi(optarg);
      break;
    case 'd': /* Copy the argument for later processing */ 
      lmt_arg[nbr_lmt]=(char *)strdup(optarg);
      nbr_lmt++;
      break;
    case 'F': /* Toggle the style of printing out arrays. Default is C-style. */
      FORTRAN_STYLE=!FORTRAN_STYLE;
      break;
    case 'h': /* Toggle appending to history global attribute */
      HISTORY_APPEND=!HISTORY_APPEND;
      break;
    case 'l': /* Get the local path prefix for storing files retrieved from the remote file system */
      fl_pth_lcl=optarg;
      break;
    case 'n': /* Get the NINTAP-style abbreviation of files to average */ 
      fl_lst_abb=lst_prs(optarg,",",&nbr_abb_arg);
      if(nbr_abb_arg < 1 || nbr_abb_arg > 5){
	(void)fprintf(stdout,"%s: ERROR Incorrect abbreviation for file list\n",prg_nm);
	(void)usg_prn();
	exit(EXIT_FAILURE);
      } /* end if */
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
    case 'r': /* Print the RCS program info and copyright notice */
      (void)copyright_prn(RCS_Id,RCS_Revision);
      (void)nc_lib_vrs_prn();
      exit(EXIT_SUCCESS);
      break;
    case 'v': /* Assemble the list of variables to extract/exclude */ 
      var_lst_in=lst_prs(optarg,",",&nbr_xtr);
      break;
    case 'x': /* Exclude rather than extract the variables specified with -v */
      EXCLUDE_INPUT_LIST=True;
      break;
    default: /* Print proper usage */
      (void)usg_prn();
      exit(EXIT_FAILURE);
      break;
    } /* end switch */
  } /* end while loop */
  
  /* Process the positional arguments and fill in the filenames */
  fl_lst_in=fl_lst_mk(argv,argc,optind,&nbr_fl,&fl_out);

  /* Make a uniform list of the user-specified dimension limits */ 
  lmt=lmt_prs(nbr_lmt,lmt_arg);
  
  /* Make netCDF errors fatal and print the diagnostic */   
  ncopts=NC_VERBOSE | NC_FATAL; 
  
  /* Parse the filename */ 
  fl_in=fl_nm_prs(fl_in,0,&nbr_fl,fl_lst_in,nbr_abb_arg,fl_lst_abb,fl_pth);
  /* Make sure the file is on the local system and is readable or die trying */ 
  fl_in=fl_mk_lcl(fl_in,fl_pth_lcl,&FILE_RETRIEVED_FROM_REMOTE_LOCATION);
  /* Open the file for reading */ 
  in_id=ncopen(fl_in,NC_NOWRITE);
  
  /* Get the number of variables, dimensions, and the record dimension ID for the file */
  (void)ncinquire(in_id,&nbr_dim_fl,&nbr_var_fl,(int *)NULL,&rec_dim_id);
  
  /* Form the initial extraction list from the user input */ 
  xtr_lst=var_lst_mk(in_id,nbr_var_fl,var_lst_in,PROCESS_ALL_COORDINATES,&nbr_xtr);

  /* Change the included variables to excluded variables */ 
  if(EXCLUDE_INPUT_LIST) xtr_lst=var_lst_xcl(in_id,nbr_var_fl,xtr_lst,&nbr_xtr);

  /* Add all the coordinate variables to the extraction list */ 
  if(PROCESS_ALL_COORDINATES) xtr_lst=var_lst_add_crd(in_id,nbr_var_fl,nbr_dim_fl,xtr_lst,&nbr_xtr);

  /* Make sure all coordinates associated with each of the variables to be extracted is also on the list */ 
  if(PROCESS_ASSOCIATED_COORDINATES) xtr_lst=var_lst_ass_crd_add(in_id,xtr_lst,&nbr_xtr);

  /* Remove the record coordinate, if any, from the extraction list */ 
  if(False) xtr_lst=var_lst_crd_xcl(in_id,rec_dim_id,xtr_lst,&nbr_xtr);

  /* Finally, heapsort the extraction list by variable ID for fastest I/O */ 
  if(nbr_xtr > 1) xtr_lst=lst_heapsort(xtr_lst,nbr_xtr,False);
    
  /* We now have the final list of variables to extract. Phew. */
  
  /* Find the coordinate/dimension values associated with the limits */ 
  for(idx=0;idx<nbr_lmt;idx++) (void)lmt_evl(in_id,lmt+idx,0L,FORTRAN_STYLE);
  
  /* Find all the dimensions associated with all variables to be extracted */ 
  dim_lst=dim_lst_ass_var(in_id,xtr_lst,nbr_xtr,&nbr_dim_xtr);

  /* Fill in the dimension structure for all the extracted dimensions */ 
  dim=(dim_sct **)malloc(nbr_dim_xtr*sizeof(dim_sct *));
  for(idx=0;idx<nbr_dim_xtr;idx++) dim[idx]=dim_fll(in_id,dim_lst[idx].id,dim_lst[idx].nm);
  
  /* Merge the hyperslab limit information into the dimension structures */ 
  if(nbr_lmt > 0) (void)dim_lmt_merge(dim,nbr_dim_xtr,lmt,nbr_lmt);

  /* Duplicate the input dimension structures for output dimension structures */ 
  dim_out=(dim_sct **)malloc(nbr_dim_xtr*sizeof(dim_sct *));
  for(idx=0;idx<nbr_dim_xtr;idx++){
    dim_out[idx]=dim_dup(dim[idx]);
    (void)dim_xrf(dim[idx],dim_out[idx]); 
  } /* end loop over idx */

  /* Is this an NCAR CSM-format history tape? */
  NCAR_CSM_FORMAT=ncar_csm_inq(in_id);

  /* Fill in the variable structure list for all the extracted variables */ 
  var=(var_sct **)malloc(nbr_xtr*sizeof(var_sct *));
  var_out=(var_sct **)malloc(nbr_xtr*sizeof(var_sct *));
  for(idx=0;idx<nbr_xtr;idx++){
    var[idx]=var_fll(in_id,xtr_lst[idx].id,xtr_lst[idx].nm,dim,nbr_dim_xtr);
    var_out[idx]=var_dup(var[idx]);
    (void)var_xrf(var[idx],var_out[idx]);
    (void)var_dim_xrf(var_out[idx]);
  } /* end loop over idx */

  /* Divide the variable lists into lists of fixed variables and variables to be processed */ 
  (void)var_lst_divide(var,var_out,nbr_xtr,NCAR_CSM_FORMAT,(dim_sct **)NULL,0,&var_fix,&var_fix_out,&nbr_var_fix,&var_prc,&var_prc_out,&nbr_var_prc);

  /* Open the output file */ 
  fl_out_tmp=fl_out_open(fl_out,FORCE_APPEND,FORCE_OVERWRITE,&out_id);

  /* Copy all the global attributes */ 
  (void)att_cpy(in_id,out_id,NC_GLOBAL,NC_GLOBAL);
  
  /* Catenate the time-stamped command line to the "history" global attribute */ 
  if(HISTORY_APPEND) (void)hst_att_cat(out_id,cmd_ln);

  /* ncecat-specific operations */ 
  if(True){
    /* Define the new record dimension */
    rdim=dim_dup(dim[0]);

    rdim->nm="record";
    rdim->id=-1;
    rdim->nc_id=-1;
    rdim->xrf=NULL;
    rdim->val.vp=NULL;
    rdim->is_crd_dim=False;
    rdim->is_crd_dim=True;
    rdim->sz=0L;
    rdim->cnt=0L;
    rdim->srt=0L;
    rdim->end=rdim->sz-1L;
    
    /* Change any existing record dimension to be a regular dimension */
    for(idx=0;idx<nbr_dim_xtr;idx++){
      /* Is any input dimension a record dimension? */
      if(dim_out[idx]->is_rec_dim){
	dim_out[idx]->is_rec_dim=False;
	break;
      } /* end if */
    } /* end loop over idx */

    /* Add the record dimension to the end of the dimension list */ 
    nbr_dim_xtr++;
    dim_out=(dim_sct **)realloc(dim_out,nbr_dim_xtr*sizeof(dim_sct **));
    dim_out[nbr_dim_xtr-1]=rdim;

  } /* end if */ 

  /* Define the dimensions in the output file */ 
  (void)dim_def(fl_out,out_id,dim_out,nbr_dim_xtr);

  if(True){
    /* Prepend the record dimension to the beginning of all the vectors for the processed variables */ 
    for(idx=0;idx<nbr_var_prc;idx++){
      var_prc_out[idx]->nbr_dim++;
      var_prc_out[idx]->is_rec_var=True;
      var_prc_out[idx]->sz_rec=var_prc_out[idx]->sz;
      
      /* Allocate space to hold the dimension IDs */ 
      var_prc_out[idx]->dim=(dim_sct **)realloc(var_prc_out[idx]->dim,var_prc_out[idx]->nbr_dim*sizeof(dim_sct *));
      var_prc_out[idx]->dim_id=(int *)realloc(var_prc_out[idx]->dim_id,var_prc_out[idx]->nbr_dim*sizeof(int));
      var_prc_out[idx]->cnt=(long *)realloc(var_prc_out[idx]->cnt,var_prc_out[idx]->nbr_dim*sizeof(long));
      var_prc_out[idx]->srt=(long *)realloc(var_prc_out[idx]->srt,var_prc_out[idx]->nbr_dim*sizeof(long));
      var_prc_out[idx]->end=(long *)realloc(var_prc_out[idx]->end,var_prc_out[idx]->nbr_dim*sizeof(long));
      
      /* Move the current array by one to make room for the new record dimension info */ 
      (void)memmove((void *)(var_prc_out[idx]->dim+1),(void *)(var_prc_out[idx]->dim),(var_prc_out[idx]->nbr_dim-1)*sizeof(dim_sct *));
      (void)memmove((void *)(var_prc_out[idx]->dim_id+1),(void *)(var_prc_out[idx]->dim_id),(var_prc_out[idx]->nbr_dim-1)*sizeof(int));
      (void)memmove((void *)(var_prc_out[idx]->cnt+1),(void *)(var_prc_out[idx]->cnt),(var_prc_out[idx]->nbr_dim-1)*sizeof(long));
      (void)memmove((void *)(var_prc_out[idx]->srt+1),(void *)(var_prc_out[idx]->srt),(var_prc_out[idx]->nbr_dim-1)*sizeof(long));
      (void)memmove((void *)(var_prc_out[idx]->end+1),(void *)(var_prc_out[idx]->end),(var_prc_out[idx]->nbr_dim-1)*sizeof(long));
      
      /* Insert the value for the new record dimension */ 
      var_prc_out[idx]->dim[0]=rdim;
      var_prc_out[idx]->dim_id[0]=rdim->id;
      var_prc_out[idx]->cnt[0]=1L;
      var_prc_out[idx]->srt[0]=-1L;
      var_prc_out[idx]->end[0]=-1L;
		    
    } /* end loop over idx */
    
  } /* end if */

  /* Define the variables in the output file, and copy their attributes */ 
  (void)var_def(in_id,fl_out,out_id,var_out,nbr_xtr,(dim_sct **)NULL,0);

  /* Turn off the default filling behavior to enhance efficiency */ 
#if ( ! defined SUN4 ) && ( ! defined SUN4SOL2 ) && ( ! defined SUNMP )
  (void)ncsetfill(out_id,NC_NOFILL);
#endif
  
  /* Take the output file out of define mode */ 
  (void)ncendef(out_id);
  
  /* Zero the start vectors for all the output variables */ 
  (void)var_srt_zero(var_out,nbr_xtr);

  /* Copy the variable data for the non-processed variables */ 
  (void)var_val_cpy(in_id,out_id,var_fix,nbr_var_fix);

  /* Close the first input netCDF file */ 
  (void)ncclose(in_id);
  
  /* Loop over the input files */ 
  for(idx_fl=0;idx_fl<nbr_fl;idx_fl++){
    /* Parse the filename */ 
    if(idx_fl != 0) fl_in=fl_nm_prs(fl_in,idx_fl,(int *)NULL,fl_lst_in,nbr_abb_arg,fl_lst_abb,fl_pth);
    if(dbg_lvl > 0) (void)fprintf(stderr,"\nInput file %d is %s; ",idx_fl,fl_in);
    /* Make sure the file is on the local system and is readable or die trying */ 
    if(idx_fl != 0) fl_in=fl_mk_lcl(fl_in,fl_pth_lcl,&FILE_RETRIEVED_FROM_REMOTE_LOCATION);
    if(dbg_lvl > 0) (void)fprintf(stderr,"local file %s:\n",fl_in);
    in_id=ncopen(fl_in,NC_NOWRITE);
    
    /* Perform error checking if there are any variables to be processed in this file */ 
    if(False) (void)fl_cmp_err_chk();

    /* Process all variables in the current file */ 
    for(idx=0;idx<nbr_var_prc;idx++){
      if(dbg_lvl > 1) (void)fprintf(stderr,"%s, ",var_prc[idx]->nm);
      if(dbg_lvl > 0) (void)fflush(stderr);
      (void)var_refresh(in_id,var_prc[idx]);
      /* Retrieve the variable values from disk into memory */ 
      (void)var_get(in_id,var_prc[idx]);
      /* The size of the record dimension is 1 in the output file */ 
      var_prc_out[idx]->cnt[0]=1L;
      var_prc_out[idx]->srt[0]=idx_rec_out;
      /* Write the variable into the current record in the output file */ 
      if(var_prc[idx]->nbr_dim == 0){
	(void)ncvarput1(out_id,var_prc_out[idx]->id,var_prc_out[idx]->srt,var_prc[idx]->val.vp);
      }else{ /* end if variable is a scalar */ 
	(void)ncvarput(out_id,var_prc_out[idx]->id,var_prc_out[idx]->srt,var_prc_out[idx]->cnt,var_prc[idx]->val.vp);
      } /* end if variable is an array */ 
      /* Free the current input buffer */
      (void)free(var_prc[idx]->val.vp); var_prc[idx]->val.vp=NULL;
    } /* end loop over idx */
    idx_rec_out++;
    
    if(dbg_lvl > 1) (void)fprintf(stderr,"\n");
    
    /* Close the input netCDF file */ 
    ncclose(in_id);
    
    /* Dispose of the local copy of the file */ 
    if(FILE_RETRIEVED_FROM_REMOTE_LOCATION && REMOVE_REMOTE_FILES_AFTER_PROCESSING) (void)fl_rm(fl_in);

  } /* end loop over idx_fl */
  
  /* Close the output file and move it from the temporary to the permanent location */ 
  (void)fl_out_close(fl_out,fl_out_tmp,out_id);
  
  Exit_gracefully();
  return EXIT_SUCCESS;
} /* end main() */
