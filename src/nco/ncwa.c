/* $Header: /data/zender/nco_20150216/nco/src/nco/ncwa.c,v 1.13 1999-01-07 00:59:17 zender Exp $ */

/* ncwa -- netCDF weighted averager */

/* (c) Copyright 1995--1999 University Corporation for Atmospheric Research 
   The file LICENSE contains the full copyright notice 
   Contact NSF/UCAR/NCAR/CGD/CMS for copyright assistance */

/* Purpose: Compute averages of specified hyperslabs of specfied variables
   in a single input netCDF file and output them to a single file. */

/* NB: As of 98/12/02, -n and -W switches were deactivated but code left in place
   while I rethink the normalization switches */ 

/* Usage:
   ncwa -O -a lon /home/zender/nc/nco/data/in.nc foo.nc
   ncwa -O -R -p /ZENDER/tmp -l /home/zender/nc/nco/data in.nc foo.nc
   ncwa -O -a lat -w gw -d lev,17 -v T -p /fs/cgd/csm/input/atm SEP1.T42.0596.nc foo.nc
   ncwa -O -C -a lat,lon,time -w gw -v PS -p /fs/cgd/csm/input/atm SEP1.T42.0596.nc foo.nc;ncks -H foo.nc
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

/* #define MAIN_PROGRAM_FILE MUST precede #include nc.h */
#define MAIN_PROGRAM_FILE
#include "nc.h"                 /* global definitions */

int 
main(int argc,char **argv)
{
  int FORTRAN_newdate(long *,long *);
  
  bool EXCLUDE_INPUT_LIST=False; /* Option c */ 
  bool FILE_RETRIEVED_FROM_REMOTE_LOCATION;
  bool FORCE_APPEND=False; /* Option A */ 
  bool FORCE_OVERWRITE=False; /* Option O */ 
  bool FORTRAN_STYLE=False; /* Option F */
  bool HISTORY_APPEND=True; /* Option h */
  bool MUST_CONFORM=False; /* Must var_conform_dim() find truly conforming variables? */ 
  bool DO_CONFORM_MSK; /* Did var_conform_dim() find truly conforming variables? */ 
  bool DO_CONFORM_WGT; /* Did var_conform_dim() find truly conforming variables? */ 
  bool NCAR_CSM_FORMAT;
  bool PROCESS_ALL_COORDINATES=False; /* Option c */
  bool PROCESS_ASSOCIATED_COORDINATES=True; /* Option C */
  bool REMOVE_REMOTE_FILES_AFTER_PROCESSING=True; /* Option R */ 
  bool NRM_BY_DNM=True; /* Option N */ 
  bool MULTIPLY_BY_TALLY=False; /* Not currently implemented */ 
  bool NORMALIZE_BY_TALLY=True; /* Not currently implemented */ 
  bool NORMALIZE_BY_WEIGHT=True; /* Not currently implemented */ 
  bool WGT_MSK_CRD_VAR=True; /* Option I */ 

  char **dim_avg_lst_in; /* Option a */ 
  char **var_lst_in;
  char **fl_lst_abb=NULL; /* Option n */ 
  char **fl_lst_in;
  char *fl_in=NULL;
  char *fl_pth_lcl=NULL; /* Option l */ 
  char *lim_arg[MAX_NC_DIMS];
  char *opt_sng;
  char *fl_out;
  char *fl_out_tmp;
  char *fl_pth=NULL; /* Option p */ 
  char *time_buf_srt;
  char *msk_nm=NULL;
  char *wgt_nm=NULL;
  char *cmd_ln;
  char rcs_Id[]="$Id: ncwa.c,v 1.13 1999-01-07 00:59:17 zender Exp $"; 
  char rcs_Revision[]="$Revision: 1.13 $";
  
  dim_sct **dim;
  dim_sct **dim_out;
  dim_sct **dim_avg;
  
  double msk_val=1.; /* Option M */ 

  extern char *optarg;
  extern int ncopts;
  extern int optind;
  
  int idx;
  int idx_avg;
  int idx_prc;
  int idx_fix;
  int idx_fl;
  int in_id;  
  int out_id;  
  int nbr_abb_arg=0;
  int nbr_dim_fl;
  int nbr_dim_avg=0;
  int nbr_lim=0; /* Option d. NB: nbr_lim gets incremented */
  int nbr_var_fl;
  int nbr_var_fix; /* nbr_var_fix gets incremented */ 
  int nbr_var_prc; /* nbr_var_prc gets incremented */ 
  int nbr_xtr=0; /* nbr_xtr won't otherwise be set for -c with no -v */ 
  int nbr_dim_out;
  int nbr_dim_xtr;
  int nbr_fl=0;
  int opt;
  int op_type=0; /* Option o */ 
  int rec_dim_id=-1;
  
  lim_sct *lim;
  lim_sct lim_rec;
  
  long idx_rec;
  
  nm_id_sct *dim_lst;
  nm_id_sct *xtr_lst=NULL; /* xtr_lst can get realloc()'d from NULL with -c option */ 
  nm_id_sct *dim_avg_lst;
  
  time_t clock;
  
  var_sct **var;
  var_sct **var_fix;
  var_sct **var_fix_out;
  var_sct **var_out;
  var_sct **var_prc;
  var_sct **var_prc_out;
  var_sct *msk=NULL;
  var_sct *msk_out=NULL;
  var_sct *wgt=NULL;
  var_sct *wgt_avg=NULL;
  var_sct *wgt_out=NULL;

  
  /* NB: assert causes inscrutable crashes (because it's a macro?)
     during compilation under sunos. */ 
  /*
     #ifndef SUN4
     (void)assert(argc > 0);
     (void)assert(argv != 0);
     #end if
     */
  
  /* Start the clock and save the command line */  
  cmd_ln=cmd_ln_sng(argc,argv);
  clock=time((time_t *)NULL);
  time_buf_srt=ctime(&clock);
  
  /* Get the program name and set the enum for the program (e.g., prg=ncra) */
  prg_nm=prg_prs(argv[0],&prg);

  /* Parse command line arguments */
  opt_sng="Aa:CcD:d:FhIl:M:m:nNo:Op:rRv:xWw:";
  while((opt = getopt(argc,argv,opt_sng)) != EOF){
    switch(opt){
    case 'A': /* Toggle FORCE_APPEND */
      FORCE_APPEND=!FORCE_APPEND;
      break;
    case 'a':
      /* Get the list dimensions over which to average the hyperslab */ 
      dim_avg_lst_in=lst_prs(optarg,",",&nbr_dim_avg);
      break;
    case 'C': /* Add to the extraction list any coordinates associated with variables to be extracted? */ 
      PROCESS_ASSOCIATED_COORDINATES=False;
      break;
    case 'c':
      PROCESS_ALL_COORDINATES=True;
      break;
    case 'D':
      /* The debugging level.  Default is 0. */
      dbg_lvl=atoi(optarg);
      break;
    case 'd':
      /* Copy the argument for later processing */ 
      lim_arg[nbr_lim]=(char *)strdup(optarg);
      nbr_lim++;
      break;
    case 'F':
      /* Toggle the style of printing out arrays. Default is C-style. */
      FORTRAN_STYLE=!FORTRAN_STYLE;
      break;
    case 'h': /* Toggle appending to history global attribute */
      HISTORY_APPEND=!HISTORY_APPEND;
      break;
    case 'I':
      WGT_MSK_CRD_VAR=!WGT_MSK_CRD_VAR;
      break;
    case 'l':
      /* Get the local path prefix for storing files retrieved from the remote file system */
      fl_pth_lcl=optarg;
      break;
    case 'm':
      /* The name of the variable to use as a mask in the averaging.  Default is none */
      msk_nm=optarg;
      break;
    case 'M':
      /* The only good data will be when the mask has this value. Default is 1. */
      msk_val=atof(optarg);
      break;
    case 'N':
      NRM_BY_DNM=False;
      NORMALIZE_BY_TALLY=False;
      NORMALIZE_BY_WEIGHT=False;
      break;
    case 'n':
      NORMALIZE_BY_WEIGHT=False;
      (void)fprintf(stdout,"%s: ERROR This option has been disabled while I rethink its implementation\n",prg_nm);
      exit(EXIT_FAILURE);
      break;
    case 'O': /* Toggle FORCE_OVERWRITE */
      FORCE_OVERWRITE=!FORCE_OVERWRITE;
      break;
    case 'o':
      /* The relational operator type.  Default is 0, eq, equality */
      op_type=op_prs(optarg);
      break;
    case 'p':
      /* Get the path prefix */
      fl_pth=optarg;
      break;
    case 'R':
      /* Toggle the removal of remotely-retrieved-files after processing. Default is True */
      REMOVE_REMOTE_FILES_AFTER_PROCESSING=!REMOVE_REMOTE_FILES_AFTER_PROCESSING;
      break;
    case 'r':
      /* Print the RCS program info and copyright notice */
      (void)copyright_prn(rcs_Id,rcs_Revision);
      (void)nc_lib_vrs_prn();
       exit(EXIT_SUCCESS);
      break;
    case 'v':
      /* Assemble the list of variables to extract/exclude */ 
      var_lst_in=lst_prs(optarg,",",&nbr_xtr);
      break;
    case 'W':
      NORMALIZE_BY_TALLY=False;
      (void)fprintf(stdout,"%s: ERROR This option has been disabled while I rethink its implementation\n",prg_nm);
      exit(EXIT_FAILURE);
      break;
    case 'w':
      /* The name of the variable to use as a weight in the averaging.  Default is none */
      wgt_nm=optarg;
      break;
    case 'x':
      /* Exclude rather than extract the variables specified with -v */
      EXCLUDE_INPUT_LIST=True;
      break;
    case '?':
      /* Print proper usage */
      (void)usg_prn();
      exit(EXIT_FAILURE);
      break;
    } /* end switch */
  } /* end while loop */
  
  /* Ensure we do not attempt to normalize by non-existent weight */ 
  if(wgt_nm == NULL) NORMALIZE_BY_WEIGHT=False;

  /* Process the positional arguments and fill in the filenames */
  fl_lst_in=fl_lst_mk(argv,argc,optind,&nbr_fl,&fl_out);

  /* Make a uniform list of the user-specified dimension limits */ 
  lim=lim_prs(nbr_lim,lim_arg);
  
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
  if(nbr_xtr > 1) xtr_lst=lst_heapsort(xtr_lst,nbr_xtr);
    
  /* Find the coordinate/dimension values associated with the limits */ 
  for(idx=0;idx<nbr_lim;idx++){
    (void)lim_evl(in_id,lim+idx,FORTRAN_STYLE);
  } /* end loop over idx */
  
  /* Find all the dimensions associated with all variables to be extracted */ 
  dim_lst=dim_lst_ass_var(in_id,xtr_lst,nbr_xtr,&nbr_dim_xtr);

  /* Fill in the dimension structure for all the extracted dimensions */ 
  dim=(dim_sct **)malloc(nbr_dim_xtr*sizeof(dim_sct *));
  for(idx=0;idx<nbr_dim_xtr;idx++){
    dim[idx]=dim_fll(in_id,dim_lst[idx].id,dim_lst[idx].nm);
  } /* end loop over idx */
  
  /* Merge the hyperslab limit information into the dimension structures */ 
  if(nbr_lim > 0) (void)dim_lim_merge(dim,nbr_dim_xtr,lim,nbr_lim);

  if (nbr_dim_avg > 0){
    /* Form a list of the averaging dimensions */ 
    dim_avg_lst=dim_lst_mk(in_id,dim_avg_lst_in,nbr_dim_avg);

    if(nbr_dim_avg > nbr_dim_xtr){
      (void)fprintf(stdout,"%s: ERROR More averaging dimensions than extracted dimensions\n",prg_nm);
      exit(EXIT_FAILURE);
    } /* end if */

    /* Form a list of the averaging dimensions from the extracted input dimensions */ 
    dim_avg=(dim_sct **)malloc(nbr_dim_avg*sizeof(dim_sct *));
    for(idx_avg=0;idx_avg<nbr_dim_avg;idx_avg++){
      for(idx=0;idx<nbr_dim_xtr;idx++){
	if(!strcmp(dim_avg_lst[idx_avg].nm,dim[idx]->nm)) break;
      } /* end loop over idx_avg */
      if(idx != nbr_dim_xtr){
	dim_avg[idx_avg]=dim[idx];
      }else{
	(void)fprintf(stderr,"%s: WARNING averaging dimension \"%s\" is not contained in any variable in extraction list\n",prg_nm,dim_avg_lst[idx_avg].nm);
	/* Collapse the dimension average list by omitting the irrelevent dimension */ 
	(void)memmove(dim_avg_lst,dim_avg_lst,idx_avg*sizeof(nm_id_sct));
	(void)memmove(dim_avg_lst+idx_avg*sizeof(nm_id_sct),dim_avg_lst+(idx_avg+1)*sizeof(nm_id_sct),(nbr_dim_avg-idx_avg+1)*sizeof(nm_id_sct));
	--nbr_dim_avg;
	dim_avg_lst=(nm_id_sct *)realloc(dim_avg_lst,nbr_dim_avg*sizeof(nm_id_sct));
      } /* end else */ 
    } /* end loop over idx_avg */

    /* Make sure no averaging dimension is specified more than once */ 
    for(idx=0;idx<nbr_dim_avg;idx++){
      for(idx_avg=0;idx_avg<nbr_dim_avg;idx_avg++){
	if(idx_avg != idx){
	  if(dim_avg[idx]->id == dim_avg[idx_avg]->id){
	    (void)fprintf(stdout,"%s: ERROR %s specified more than once in averaging list\n",prg_nm,dim_avg[idx]->nm);
	    exit(EXIT_FAILURE);
	  } /* end if */
	} /* end if */
      } /* end loop over idx_avg */
    } /* end loop over idx */

    /* Dimensions to be averaged will not appear in output file */ 
    dim_out=(dim_sct **)malloc((nbr_dim_xtr-nbr_dim_avg)*sizeof(dim_sct *));
    nbr_dim_out=0;
    for(idx=0;idx<nbr_dim_xtr;idx++){
      for(idx_avg=0;idx_avg<nbr_dim_avg;idx_avg++){
	if(!strcmp(dim_avg_lst[idx_avg].nm,dim[idx]->nm)) break;
      } /* end loop over idx_avg */
      if(idx_avg == nbr_dim_avg){
	dim_out[nbr_dim_out]=dim_dup(dim[idx]);
	(void)dim_xrf(dim[idx],dim_out[nbr_dim_out]);
	nbr_dim_out++;
      } /* end if */
    } /* end loop over idx_avg */

    if(nbr_dim_out != nbr_dim_xtr-nbr_dim_avg){
      (void)fprintf(stdout,"%s: ERROR nbr_dim_out != nbr_dim_xtr-nbr_dim_avg\n",prg_nm);
      exit(EXIT_FAILURE);
    } /* end if */
    
  }else{

    /* Duplicate input dimension structures for output dimension structures */ 
    nbr_dim_out=nbr_dim_xtr;
    dim_out=(dim_sct **)malloc(nbr_dim_out*sizeof(dim_sct *));
    for(idx=0;idx<nbr_dim_out;idx++){
      dim_out[idx]=dim_dup(dim[idx]);
      (void)dim_xrf(dim[idx],dim_out[idx]);
    } /* end loop over idx */

  } /* end if */ 

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

  /* Divide variable lists into lists of fixed variables and variables to be processed */ 
  (void)var_lst_divide(var,var_out,nbr_xtr,NCAR_CSM_FORMAT,dim_avg,nbr_dim_avg,&var_fix,&var_fix_out,&nbr_var_fix,&var_prc,&var_prc_out,&nbr_var_prc);

  /* We now have the final list of variables to extract. Phew. */
  if(dbg_lvl > 0){
    for(idx=0;idx<nbr_xtr;idx++) (void)fprintf(stderr,"var[%d]->nm = %s, ->id=[%d]\n",idx,var[idx]->nm,var[idx]->id);
    for(idx=0;idx<nbr_var_fix;idx++) (void)fprintf(stderr,"var_fix[%d]->nm = %s, ->id=[%d]\n",idx,var_fix[idx]->nm,var_fix[idx]->id);
    for(idx=0;idx<nbr_var_prc;idx++) (void)fprintf(stderr,"var_prc[%d]->nm = %s, ->id=[%d]\n",idx,var_prc[idx]->nm,var_prc[idx]->id);
  } /* end if */
  
  /* Open output file */ 
  fl_out_tmp=fl_out_open(fl_out,FORCE_APPEND,FORCE_OVERWRITE,&out_id);

  /* Copy all global attributes */ 
  (void)att_cpy(in_id,out_id,NC_GLOBAL,NC_GLOBAL);
  
  /* Catenate time-stamped command line to "history" global attribute */ 
  if(HISTORY_APPEND) (void)hst_att_cat(out_id,cmd_ln);

  /* Define dimensions in output file */ 
  (void)dim_def(fl_out,out_id,dim_out,nbr_dim_out);

  /* Define variables in output file, and copy their attributes */ 
  (void)var_def(in_id,fl_out,out_id,var_out,nbr_xtr,dim_out,nbr_dim_out);

  /* New missing values must be added to the output file in define mode */
  if(msk_nm != NULL){
    for(idx=0;idx<nbr_var_prc;idx++){
      /* Define for var_prc_out because mss_val for var_prc will be overwritten in var_refresh */ 
      if(!var_prc_out[idx]->has_mss_val){
	var_prc_out[idx]->has_mss_val=True;
	var_prc_out[idx]->mss_val=mss_val_mk(var_prc[idx]->type);
	(void)ncattput(out_id,var_prc_out[idx]->id,"missing_value",var_prc_out[idx]->type,1,var_prc_out[idx]->mss_val.vp);
      } /* end if */
    } /* end for */ 
  } /* end if */

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
  ncclose(in_id);
  
  /* Loop over the input files */ 
  for(idx_fl=0;idx_fl<nbr_fl;idx_fl++){
    /* Parse the filename */ 
    if(idx_fl != 0) fl_in=fl_nm_prs(fl_in,idx_fl,&nbr_fl,fl_lst_in,nbr_abb_arg,fl_lst_abb,fl_pth);
    if(dbg_lvl > 0) (void)fprintf(stderr,"\nInput file %d is %s; ",idx_fl,fl_in);
    /* Make sure the file is on the local system and is readable or die trying */ 
    if(idx_fl != 0) fl_in=fl_mk_lcl(fl_in,fl_pth_lcl,&FILE_RETRIEVED_FROM_REMOTE_LOCATION);
    if(dbg_lvl > 0) (void)fprintf(stderr,"local file %s:\n",fl_in);
    in_id=ncopen(fl_in,NC_NOWRITE);
    
    /* Perform error checking if there are any variables to be processed in this file */ 
    if(False) (void)fl_cmp_err_chk();

    /* Find the weighting variable in the input file */
    if(wgt_nm != NULL){
      int wgt_id;
      
      wgt_id=ncvarid(in_id,wgt_nm);
      wgt=var_fll(in_id,wgt_id,wgt_nm,dim,nbr_dim_fl);
      
      /* Retrieve the weighting variable */ 
      (void)var_get(in_id,wgt);
      /* DBG XXX Perhaps should allocate default tally array for wgt here.
       That way, when wgt conforms to the first var_prc_out and it therefore
       does not get a tally array copied by var_dup() in var_conform_dim(), 
       it will at least have space for a tally array. TODO #114. */ 

    } /* end if */

    /* Find the mask variable in the input file */
    if(msk_nm != NULL){
      int msk_id;
      
      msk_id=ncvarid(in_id,msk_nm);
      msk=var_fll(in_id,msk_id,msk_nm,dim,nbr_dim_fl);
      
      /* Retrieve the masking variable */ 
      (void)var_get(in_id,msk);
    } /* end if */

    /* Process all variables in the current file */ 
    for(idx=0;idx<nbr_var_prc;idx++){
      if(dbg_lvl > 0) (void)fprintf(stderr,"%s, ",var_prc[idx]->nm);
      if(dbg_lvl > 0) (void)fflush(stderr);

      /* Allocate and, if necesssary, initialize accumulation space for all processed variables */ 
      var_prc_out[idx]->sz=var_prc[idx]->sz;
      if((var_prc_out[idx]->tally=var_prc[idx]->tally=(long *)malloc(var_prc_out[idx]->sz*sizeof(long))) == NULL){
	(void)fprintf(stdout,"%s: ERROR Unable to malloc() %ld*%ld bytes for tally buffer for variable %s in main()\n",prg_nm_get(),var_prc_out[idx]->sz,(long)sizeof(long),var_prc_out[idx]->nm);
	exit(EXIT_FAILURE); 
      } /* end if */ 
      (void)zero_long(var_prc_out[idx]->sz,var_prc_out[idx]->tally);
      if((var_prc_out[idx]->val.vp=(void *)malloc(var_prc_out[idx]->sz*nctypelen(var_prc_out[idx]->type))) == NULL){
	(void)fprintf(stdout,"%s: ERROR Unable to malloc() %ld*%d bytes for value buffer for variable %s in main()\n",prg_nm_get(),var_prc_out[idx]->sz,nctypelen(var_prc_out[idx]->type),var_prc_out[idx]->nm);
	exit(EXIT_FAILURE); 
      } /* end if */ 
      (void)var_zero(var_prc_out[idx]->type,var_prc_out[idx]->sz,var_prc_out[idx]->val);
      
      (void)var_refresh(in_id,var_prc[idx]);
      /* Retrieve the variable from disk into memory */ 
      (void)var_get(in_id,var_prc[idx]);
      if(msk_nm != NULL && (!var_prc[idx]->is_crd_var || WGT_MSK_CRD_VAR)){
	msk_out=var_conform_dim(var_prc[idx],msk,msk_out,MUST_CONFORM,&DO_CONFORM_MSK);
	/* If msk and var did not conform then do not mask var! */ 
	if(DO_CONFORM_MSK){
	  msk_out=var_conform_type(var_prc[idx]->type,msk_out);
	  
	  /* mss_val for var_prc has been overwritten in var_refresh() */ 
	  if(!var_prc[idx]->has_mss_val){
	    var_prc[idx]->has_mss_val=True;
	    var_prc[idx]->mss_val=mss_val_mk(var_prc[idx]->type);
	  } /* end if */
	  
	  /* Mask by changing variable to missing value where condition is false */ 
	  (void)var_mask(var_prc[idx]->type,var_prc[idx]->sz,var_prc[idx]->has_mss_val,var_prc[idx]->mss_val,msk_val,op_type,msk_out->val,var_prc[idx]->val);
	} /* end if */
      } /* end if */
      if(wgt_nm != NULL && (!var_prc[idx]->is_crd_var || WGT_MSK_CRD_VAR)){
	/* DBG XXX var_conform_dim() has a bug where it does not allocate a tally array
	 for weights that do already conform to var_prc. TODO #114. */ 
	wgt_out=var_conform_dim(var_prc[idx],wgt,wgt_out,MUST_CONFORM,&DO_CONFORM_WGT);
	wgt_out=var_conform_type(var_prc[idx]->type,wgt_out);
	/* Weight variable by taking product of weight and variable */ 
	(void)var_multiply(var_prc[idx]->type,var_prc[idx]->sz,var_prc[idx]->has_mss_val,var_prc[idx]->mss_val,wgt_out->val,var_prc[idx]->val);
      } /* end if */
      /* Copy (masked) (weighted) values from var_prc to var_prc_out */ 
      (void)memcpy((void *)(var_prc_out[idx]->val.vp),(void *)(var_prc[idx]->val.vp),var_prc_out[idx]->sz*nctypelen(var_prc_out[idx]->type));
      /* Average variable over specified dimensions (tally array is set here) */
      var_prc_out[idx]=var_avg(var_prc_out[idx],dim_avg,nbr_dim_avg);
      /* var_prc_out[idx]->val holds numerator of averaging expression documented in NCO User's Guide 
	 Denominator is also tricky due to sundry normalization options 
	 These logical switches are VERY tricky---be careful modifying them */
      if(NRM_BY_DNM && wgt_nm != NULL && (!var_prc[idx]->is_crd_var || WGT_MSK_CRD_VAR)){
	if(msk_nm != NULL && DO_CONFORM_MSK){
	  /* Must mask weight in same fashion as variable was masked */ 
	  /* If msk and var did not conform then do not mask wgt */ 
	  /* Ensure wgt_out has a missing value */ 
	  if(!wgt_out->has_mss_val){
	    wgt_out->has_mss_val=True;
	    wgt_out->mss_val=mss_val_mk(wgt_out->type);
	  } /* end if */
	  /* Mask by changing weight to missing value where condition is false */ 
	  (void)var_mask(wgt_out->type,wgt_out->sz,wgt_out->has_mss_val,wgt_out->mss_val,msk_val,op_type,msk_out->val,wgt_out->val);
	} /* endif weight must be masked */ 
	/* DBG XXX temporary kludge to make sure weight has tally space.
	   wgt_out may occasionally lack a valid tally array in ncwa because
	   it is created, sometimes, before the tally array for var_prc_out[idx] is 
	   created, and thus the var_dup() call in var_conform_dim() does not copy
	   a tally array into wgt_out. See related note about this above. TODO #114.*/ 
	if((wgt_out->tally=(long *)realloc(wgt_out->tally,wgt_out->sz*sizeof(long))) == NULL){
	  (void)fprintf(stdout,"%s: ERROR Unable to realloc() %ld*%ld bytes for tally buffer for weight %s in main()\n",prg_nm_get(),wgt_out->sz,(long)sizeof(long),wgt_out->nm);
	  exit(EXIT_FAILURE); 
	} /* end if */ 
	/* Average weight over specified dimensions (tally array is set here) */ 
	wgt_out=var_avg(wgt_out,dim_avg,nbr_dim_avg);
	if(MULTIPLY_BY_TALLY){
	  /* Currently this is not implemented */ 
	  /* Multiply numerator (weighted sum of variable) by tally 
	     We deviously accomplish this by dividing the denominator by tally */ 
	  (void)var_normalize(wgt_out->type,wgt_out->sz,wgt_out->has_mss_val,wgt_out->mss_val,wgt_out->tally,wgt_out->val);
	} /* endif */ 
	/* Divide by denominator */ 
	/* This constructs the default weighted average */ 
	(void)var_divide(var_prc_out[idx]->type,var_prc_out[idx]->sz,var_prc_out[idx]->has_mss_val,var_prc_out[idx]->mss_val,wgt_out->val,var_prc_out[idx]->val);
      }else if(NRM_BY_DNM){
	/* Normalize by tally only and forget about weights */ 
	(void)var_normalize(var_prc_out[idx]->type,var_prc_out[idx]->sz,var_prc_out[idx]->has_mss_val,var_prc_out[idx]->mss_val,var_prc_out[idx]->tally,var_prc_out[idx]->val);
      }else if(!NRM_BY_DNM){
	/* No normalization required, we are done */ 
	;
      }else{
	(void)fprintf(stdout,"%s: ERROR Unforeseen logical branch in main()\n",prg_nm);
	exit(EXIT_FAILURE);
      } /* end if */
      /* Free tallying buffer */
      (void)free(var_prc_out[idx]->tally); var_prc_out[idx]->tally=NULL;
      /* Free current input buffer */
      (void)free(var_prc[idx]->val.vp); var_prc[idx]->val.vp=NULL;

      /* Copy average to output file and free averaging buffer */ 
      if(var_prc_out[idx]->nbr_dim == 0){
	(void)ncvarput1(out_id,var_prc_out[idx]->id,var_prc_out[idx]->srt,var_prc_out[idx]->val.vp);
      }else{ /* end if variable is scalar */ 
	(void)ncvarput(out_id,var_prc_out[idx]->id,var_prc_out[idx]->srt,var_prc_out[idx]->cnt,var_prc_out[idx]->val.vp);
      } /* end if variable is array */ 
      (void)free(var_prc_out[idx]->val.vp);
      var_prc_out[idx]->val.vp=NULL;
      
    } /* end loop over idx */
    
    /* Free weights and masks */ 
    if(wgt != NULL) wgt=var_free(wgt);
    if(wgt_out != NULL) wgt_out=var_free(wgt_out);
    if(wgt_avg != NULL) wgt_avg=var_free(wgt_avg);
    if(msk != NULL) msk=var_free(msk);
    if(msk_out != NULL) msk_out=var_free(msk_out);

    if(dbg_lvl > 0) (void)fprintf(stderr,"\n");
    
    /* Close the input netCDF file */ 
    ncclose(in_id);
    
    /* Dispose of the local copy of the file */ 
    if(FILE_RETRIEVED_FROM_REMOTE_LOCATION && REMOVE_REMOTE_FILES_AFTER_PROCESSING) (void)fl_rm(fl_in);

  } /* end loop over idx_fl */
  
  /* Close the output file and move it from the temporary to the permanent location */ 
  (void)fl_out_close(fl_out,fl_out_tmp,out_id);
  
  Exit_gracefully();
} /* end main() */
