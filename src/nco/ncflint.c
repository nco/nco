/* $Header: /data/zender/nco_20150216/nco/src/nco/ncflint.c,v 1.6 1999-04-05 00:37:36 zender Exp $ */

/* ncflint -- netCDF file interpolator */

/* (c) Copyright 1995--1999 University Corporation for Atmospheric Research 
   The file LICENSE contains the full copyright notice 
   Contact NSF/UCAR/NCAR/CGD/CMS for copyright assistance */

/* Purpose: Linearly interpolate a third netCDF file from two input files */

/* Usage:
   ncflint -O -D 2 in.nc in.nc foo.nc
   ncflint -O -i lcl_time_hr,9. -v lcl_time_hr /data/zender/arese/crm/951030_0800_arese_crm.nc /data/zender/arese/crm/951030_1100_arese_crm.nc foo.nc; ncks -H foo.nc
   ncflint -O -w .66666,.33333 -v lcl_time_hr /data/zender/arese/crm/951030_0800_arese_crm.nc /data/zender/arese/crm/951030_1100_arese_crm.nc foo.nc; ncks -H foo.nc
   ncflint -O -w .66666 -v lcl_time_hr /data/zender/arese/crm/951030_0800_arese_crm.nc /data/zender/arese/crm/951030_1100_arese_crm.nc foo.nc; ncks -H foo.nc

   ncdiff -O foo.nc /data/zender/arese/crm/951030_0900_arese_crm.nc foo2.nc;ncks -H foo2.nc | m
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
  extern var_sct *scl_dbl_mk_var(double);

  bool CMD_LN_NTP_VAR=False; /* Option i */ 
  bool CMD_LN_NTP_WGT=True; /* Option w */ 
  bool EXCLUDE_INPUT_LIST=False; /* Option c */ 
  bool FILE_1_RETRIEVED_FROM_REMOTE_LOCATION;
  bool FILE_2_RETRIEVED_FROM_REMOTE_LOCATION;
  bool FORCE_APPEND=False; /* Option A */ 
  bool FORCE_OVERWRITE=False; /* Option O */ 
  bool FORTRAN_STYLE=False; /* Option F */
  bool HISTORY_APPEND=True; /* Option h */
  bool MUST_CONFORM=False; /* Must var_conform_dim() find truly conforming variables? */ 
  bool DO_CONFORM=False; /* Did var_conform_dim() find truly conforming variables? */ 
  bool NCAR_CSM_FORMAT;
  bool PROCESS_ALL_COORDINATES=False; /* Option c */
  bool PROCESS_ASSOCIATED_COORDINATES=True; /* Option C */
  bool REMOVE_REMOTE_FILES_AFTER_PROCESSING=True; /* Option R */ 
  
  char **var_lst_in;
  char **ntp_lst_in;
  char **fl_lst_abb=NULL; /* Option a */ 
  char **fl_lst_in;
  char *fl_in=NULL;
  char *fl_in_1;
  char *fl_in_2;
  char *fl_pth_lcl=NULL; /* Option l */ 
  char *lim_arg[MAX_NC_DIMS];
  char *opt_sng;
  char *fl_out;
  char *fl_out_tmp;
  char *fl_pth=NULL; /* Option p */ 
  char *time_buf_srt;
  char *cmd_ln;
  char *ntp_nm=NULL; /* Option i */ 
  char rcs_Id[]="$Id: ncflint.c,v 1.6 1999-04-05 00:37:36 zender Exp $"; 
  char rcs_Revision[]="$Revision: 1.6 $";
  
  dim_sct **dim;
  dim_sct **dim_out;
  
  double ntp_val_out; /* Option i */ 
  double wgt_val_1=.5; /* Option w */ 
  double wgt_val_2=.5; /* Option w */ 

  extern char *optarg;
  extern int ncopts;
  extern int optind;
  
  int has_mss_val=False;
  int idx;
  int idx_fl;
  int in_id;  
  int in_id_1;  
  int in_id_2;  
  int out_id;  
  int nbr_abb_arg=0;
  int nbr_dim_fl;
  int nbr_lim=0; /* Option d. NB: nbr_lim gets incremented */
  int nbr_ntp;
  int nbr_var_fl;
  int nbr_var_fix; /* nbr_var_fix gets incremented */ 
  int nbr_var_prc; /* nbr_var_prc gets incremented */ 
  int nbr_xtr=0; /* nbr_xtr won't otherwise be set for -c with no -v */ 
  int nbr_dim_xtr;
  int nbr_fl=0;
  int opt;
  
  lim_sct *lim;
  
  nm_id_sct *dim_lst;
  nm_id_sct *xtr_lst=NULL; /* xtr_lst can get realloc()'d from NULL with -c option */ 
  
  time_t clock;
  
  var_sct *wgt_1;
  var_sct *wgt_2;
  var_sct *wgt_out_1=NULL;
  var_sct *wgt_out_2=NULL;
  var_sct **var;
  var_sct **var_fix;
  var_sct **var_fix_out;
  var_sct **var_out;
  var_sct **var_prc_1;
  var_sct **var_prc_2;
  var_sct **var_prc_out;
  
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
  opt_sng="ACcD:d:Fhi:l:Op:rRv:xw:";
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
    case 'D': /* The debugging level. Default is 0. */
      dbg_lvl=atoi(optarg);
      break;
    case 'd': /* Copy the argument for later processing */ 
      lim_arg[nbr_lim]=(char *)strdup(optarg);
      nbr_lim++;
      break;
    case 'F': /* Toggle the style of printing out arrays. Default is C-style. */
      FORTRAN_STYLE=!FORTRAN_STYLE;
      break;
    case 'h': /* Toggle appending to history global attribute */
      HISTORY_APPEND=!HISTORY_APPEND;
      break;
    case 'i':
      /* The name of the variable to guide interpolation. Default is none */
      ntp_lst_in=lst_prs(optarg,",",&nbr_ntp);
      if(nbr_ntp > 2){
	(void)fprintf(stdout,"%s: ERROR too many arguments to -i\n",prg_nm_get());
	(void)usg_prn();
	exit(EXIT_FAILURE);
      } /* end if */ 
      ntp_nm=ntp_lst_in[0];
      ntp_val_out=strtod(ntp_lst_in[1],(char **)NULL);
      CMD_LN_NTP_VAR=True;
      CMD_LN_NTP_WGT=False;
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
    case 'r': /* Print the RCS program info and copyright notice */
      (void)copyright_prn(rcs_Id,rcs_Revision);
      (void)nc_lib_vrs_prn();
      exit(EXIT_SUCCESS);
      break;
    case 'v': /* Assemble the list of variables to extract/exclude */ 
      var_lst_in=lst_prs(optarg,",",&nbr_xtr);
      break;
    case 'w':
      /* Weight(s) for interpolation.  Default is wgt_val_1=wgt_val2=.5 */
      ntp_lst_in=lst_prs(optarg,",",&nbr_ntp);
      if(nbr_ntp > 2){
	(void)fprintf(stdout,"%s: ERROR too many arguments to -w\n",prg_nm_get());
	(void)usg_prn();
	exit(EXIT_FAILURE);
      }else if(nbr_ntp == 2){
	wgt_val_1=strtod(ntp_lst_in[0],(char **)NULL);
	wgt_val_2=strtod(ntp_lst_in[1],(char **)NULL);
      }else if(nbr_ntp == 1){
	wgt_val_1=strtod(ntp_lst_in[0],(char **)NULL);
	wgt_val_2=1.-wgt_val_1;
      } /* end else */ 
      CMD_LN_NTP_WGT=True;
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
  
  if(CMD_LN_NTP_VAR && CMD_LN_NTP_WGT){
    (void)fprintf(stdout,"%s: ERROR interpolating variable (-i) and fixed weight(s) (-w) both set\n",prg_nm_get());
    exit(EXIT_FAILURE);
  }else if(!CMD_LN_NTP_VAR && !CMD_LN_NTP_WGT){
    (void)fprintf(stdout,"%s: ERROR interpolating variable (-i) or fixed weight(s) (-w) must be set\n",prg_nm_get());
    exit(EXIT_FAILURE);
  } /* end else */

  /* Process the positional arguments and fill in the filenames */
  fl_lst_in=fl_lst_mk(argv,argc,optind,&nbr_fl,&fl_out);
  
  /* Make a uniform list of the user-specified dimension limits */ 
  lim=lim_prs(nbr_lim,lim_arg);
  
  /* Make netCDF errors fatal and print the diagnostic */   
  ncopts=NC_VERBOSE | NC_FATAL; 
  
  /* Parse the filename */ 
  idx_fl=0;
  fl_in=fl_nm_prs(fl_in,idx_fl,&nbr_fl,fl_lst_in,nbr_abb_arg,fl_lst_abb,fl_pth);
  if(dbg_lvl > 0) (void)fprintf(stderr,"\nInput file %d is %s; ",idx_fl,fl_in);
  /* Make sure the file is on the local system and is readable or die trying */ 
  fl_in=fl_mk_lcl(fl_in,fl_pth_lcl,&FILE_1_RETRIEVED_FROM_REMOTE_LOCATION);
  if(dbg_lvl > 0) (void)fprintf(stderr,"local file %s:\n",fl_in);
  in_id=ncopen(fl_in,NC_NOWRITE);
  
  /* Get the number of variables and dimensions in the file */
  (void)ncinquire(in_id,&nbr_dim_fl,&nbr_var_fl,(int *)NULL,(int *)NULL);
  
  /* Form the initial extraction list from the user input */ 
  xtr_lst=var_lst_mk(in_id,nbr_var_fl,var_lst_in,PROCESS_ALL_COORDINATES,&nbr_xtr);

  /* Change the included variables to excluded variables */ 
  if(EXCLUDE_INPUT_LIST) xtr_lst=var_lst_xcl(in_id,nbr_var_fl,xtr_lst,&nbr_xtr);

  /* Add all the coordinate variables to the extraction list */ 
  if(PROCESS_ALL_COORDINATES) xtr_lst=var_lst_add_crd(in_id,nbr_var_fl,nbr_dim_fl,xtr_lst,&nbr_xtr);

  /* Make sure all coordinates associated with each of the variables to be extracted is also on the list */ 
  if(PROCESS_ASSOCIATED_COORDINATES) xtr_lst=var_lst_ass_crd_add(in_id,xtr_lst,&nbr_xtr);

  /* Finally, heapsort the extraction list by variable ID for fastest I/O */ 
  if(nbr_xtr > 1) xtr_lst=lst_heapsort(xtr_lst,nbr_xtr,False);
    
  /* We now have the final list of variables to extract. Phew. */
  
  /* Find the coordinate/dimension values associated with the limits */ 
  for(idx=0;idx<nbr_lim;idx++) (void)lim_evl(in_id,lim+idx,FORTRAN_STYLE);
  
  /* Find all the dimensions associated with all variables to be extracted */ 
  dim_lst=dim_lst_ass_var(in_id,xtr_lst,nbr_xtr,&nbr_dim_xtr);

  /* Fill in the dimension structure for all the extracted dimensions */ 
  dim=(dim_sct **)malloc(nbr_dim_xtr*sizeof(dim_sct *));
  for(idx=0;idx<nbr_dim_xtr;idx++) dim[idx]=dim_fll(in_id,dim_lst[idx].id,dim_lst[idx].nm);
  
  /* Merge the hyperslab limit information into the dimension structures */ 
  if(nbr_lim > 0) (void)dim_lim_merge(dim,nbr_dim_xtr,lim,nbr_lim);

  /* Duplicate the input dimension structures for output dimension structures */ 
  dim_out=(dim_sct **)malloc(nbr_dim_xtr*sizeof(dim_sct *));
  for(idx=0;idx<nbr_dim_xtr;idx++){
    dim_out[idx]=dim_dup(dim[idx]);
    (void)dim_xrf(dim[idx],dim_out[idx]); 
  } /* end loop over idx */

  if(dbg_lvl > 0){
    for(idx=0;idx<nbr_xtr;idx++) (void)fprintf(stderr,"xtr_lst[%d].nm = %s, .id= %d\n",idx,xtr_lst[idx].nm,xtr_lst[idx].id);
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

  /* Divide the variable lists into lists of fixed variables and variables to be processed */ 
  (void)var_lst_divide(var,var_out,nbr_xtr,NCAR_CSM_FORMAT,(dim_sct **)NULL,0,&var_fix,&var_fix_out,&nbr_var_fix,&var_prc_1,&var_prc_out,&nbr_var_prc);

  /* Open the output file */ 
  fl_out_tmp=fl_out_open(fl_out,FORCE_APPEND,FORCE_OVERWRITE,&out_id);

  /* Copy all the global attributes */ 
  (void)att_cpy(in_id,out_id,NC_GLOBAL,NC_GLOBAL);
  
  /* Catenate the time-stamped command line to the "history" global attribute */ 
  if(HISTORY_APPEND) (void)hst_att_cat(out_id,cmd_ln);

  /* Define the dimensions in the output file */ 
  (void)dim_def(fl_out,out_id,dim_out,nbr_dim_xtr);

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

  in_id_1=in_id;
  idx_fl=1;
  fl_in_1=(char *)strdup(fl_in);

  /* Parse the filename */ 
  fl_in=fl_nm_prs(fl_in,idx_fl,&nbr_fl,fl_lst_in,nbr_abb_arg,fl_lst_abb,fl_pth);
  if(dbg_lvl > 0) (void)fprintf(stderr,"\nInput file %d is %s; ",idx_fl,fl_in);
  /* Make sure the file is on the local system and is readable or die trying */ 
  fl_in=fl_mk_lcl(fl_in,fl_pth_lcl,&FILE_2_RETRIEVED_FROM_REMOTE_LOCATION);
  if(dbg_lvl > 0) (void)fprintf(stderr,"local file %s:\n",fl_in);
  in_id_2=ncopen(fl_in,NC_NOWRITE);
  fl_in_2=fl_in;
  
  /* Perform error checking if there are any variables to be processed in this file */ 
  if(False) (void)fl_cmp_err_chk();

  /* ncflint-specific stuff: */
  /* Find the weighting variable in the input file */
  if(CMD_LN_NTP_VAR){
    int ntp_id_1;
    int ntp_id_2;
    
    var_sct *ntp_1;
    var_sct *ntp_2;
    var_sct *ntp_var_out;

    /* Turn arrival point into pseudo-variable */
    ntp_var_out=scl_dbl_mk_var(ntp_val_out);

    ntp_id_1=ncvarid(in_id_1,ntp_nm);
    ntp_id_2=ncvarid(in_id_2,ntp_nm);

    ntp_1=var_fll(in_id_1,ntp_id_1,ntp_nm,dim,nbr_dim_xtr);
    ntp_2=var_fll(in_id_2,ntp_id_2,ntp_nm,dim,nbr_dim_xtr);
    
    /* Currently, only support scalar variables */ 
    if(ntp_1->sz > 1 || ntp_2->sz > 1){
      (void)fprintf(stdout,"%s: ERROR interpolation variable %s must be scalar\n",prg_nm_get(),ntp_nm);
      exit(EXIT_FAILURE);
    } /* end if */

    /* Retrieve the interpolation variable */ 
    (void)var_get(in_id_1,ntp_1);
    (void)var_get(in_id_2,ntp_2);

    /* Weights must be NC_DOUBLE */ 
    ntp_1=var_conform_type(NC_DOUBLE,ntp_1);
    ntp_2=var_conform_type(NC_DOUBLE,ntp_2);

    /* Check for degenerate case */ 
    if(ntp_1->val.dp[0] == ntp_2->val.dp[0]){
      (void)fprintf(stdout,"%s: ERROR %s is identical (%g) in input files\n",prg_nm_get(),ntp_nm,ntp_1->val.dp[0]);
      exit(EXIT_FAILURE);
    } /* end if */ 

    /* Turn weights into pseudo-variables */
    wgt_1=var_dup(ntp_2);
    wgt_2=var_dup(ntp_var_out);

    /* Subtract to find interpolation distances */ 
    (void)var_subtract(ntp_1->type,ntp_1->sz,ntp_1->has_mss_val,ntp_1->mss_val,ntp_var_out->val,wgt_1->val);
    (void)var_subtract(ntp_1->type,ntp_1->sz,ntp_1->has_mss_val,ntp_1->mss_val,ntp_1->val,wgt_2->val);
    (void)var_subtract(ntp_1->type,ntp_1->sz,ntp_1->has_mss_val,ntp_1->mss_val,ntp_1->val,ntp_2->val);

    /* Normalize to obtain final interpolation weights */ 
    (void)var_divide(wgt_1->type,wgt_1->sz,wgt_1->has_mss_val,wgt_1->mss_val,ntp_2->val,wgt_1->val);
    (void)var_divide(wgt_2->type,wgt_2->sz,wgt_2->has_mss_val,wgt_2->mss_val,ntp_2->val,wgt_2->val);

    if(ntp_1 != NULL) ntp_1=var_free(ntp_1);
    if(ntp_2 != NULL) ntp_2=var_free(ntp_2);
    if(ntp_var_out != NULL) ntp_var_out=var_free(ntp_var_out);
  } /* end if CMD_LN_NTP_VAR */

  if(CMD_LN_NTP_WGT){
    wgt_1=scl_dbl_mk_var(wgt_val_1);
    wgt_2=scl_dbl_mk_var(wgt_val_2);
  } /* end if CMD_LN_NTP_WGT */ 

  if(dbg_lvl > 1) (void)fprintf(stderr,"wgt_1 = %g, wgt_2 = %g\n",wgt_1->val.dp[0],wgt_2->val.dp[0]);

    /* Create a structure list for the second file */ 
  var_prc_2=(var_sct **)malloc(nbr_var_prc*sizeof(var_sct *));

  /* Loop over each interpolated variable */ 
  for(idx=0;idx<nbr_var_prc;idx++){
    if(dbg_lvl > 0) (void)fprintf(stderr,"%s, ",var_prc_1[idx]->nm);
    if(dbg_lvl > 0) (void)fflush(stderr);

    var_prc_2[idx]=var_dup(var_prc_1[idx]);
    (void)var_refresh(in_id_2,var_prc_2[idx]);

    (void)var_get(in_id_1,var_prc_1[idx]);
    (void)var_get(in_id_2,var_prc_2[idx]);
    
    wgt_out_1=var_conform_dim(var_prc_1[idx],wgt_1,wgt_out_1,MUST_CONFORM,&DO_CONFORM);
    wgt_out_2=var_conform_dim(var_prc_2[idx],wgt_2,wgt_out_2,MUST_CONFORM,&DO_CONFORM);

    var_prc_1[idx]=var_conform_type(NC_DOUBLE,var_prc_1[idx]);
    var_prc_2[idx]=var_conform_type(NC_DOUBLE,var_prc_2[idx]);

    /* Allocate and, if necesssary, initialize space for processed variable */ 
    var_prc_out[idx]->sz=var_prc_1[idx]->sz;
    /* NB: must not try to free() same tally buffer twice */ 
    /*    var_prc_out[idx]->tally=var_prc_1[idx]->tally=(long *)malloc(var_prc_out[idx]->sz*sizeof(long));*/
    var_prc_out[idx]->tally=(long *)malloc(var_prc_out[idx]->sz*sizeof(long));
    (void)zero_long(var_prc_out[idx]->sz,var_prc_out[idx]->tally);
  
    /* Weight the variable by taking the product of the weight and the variable */ 
    (void)var_multiply(var_prc_1[idx]->type,var_prc_1[idx]->sz,var_prc_1[idx]->has_mss_val,var_prc_1[idx]->mss_val,wgt_out_1->val,var_prc_1[idx]->val);
    (void)var_multiply(var_prc_2[idx]->type,var_prc_2[idx]->sz,var_prc_2[idx]->has_mss_val,var_prc_2[idx]->mss_val,wgt_out_2->val,var_prc_2[idx]->val);
    (void)var_add(var_prc_1[idx]->type,var_prc_1[idx]->sz,var_prc_1[idx]->has_mss_val,var_prc_1[idx]->mss_val,var_prc_out[idx]->tally,var_prc_1[idx]->val,var_prc_2[idx]->val);
    
    /* Recast output variable to original type */ 
    var_prc_2[idx]=var_conform_type(var_prc_out[idx]->type,var_prc_2[idx]);

    /* Copy interpolations to output file */ 
    if(var_prc_out[idx]->nbr_dim == 0){
      (void)ncvarput1(out_id,var_prc_out[idx]->id,var_prc_out[idx]->srt,var_prc_2[idx]->val.vp);
    }else{ /* end if variable is a scalar */ 
      (void)ncvarput(out_id,var_prc_out[idx]->id,var_prc_out[idx]->srt,var_prc_out[idx]->cnt,var_prc_2[idx]->val.vp);
    } /* end else */ 

    /* Free dynamically allocated buffers */ 
    if(var_prc_1[idx] != NULL) var_prc_1[idx]=var_free(var_prc_1[idx]);
    if(var_prc_2[idx] != NULL) var_prc_2[idx]=var_free(var_prc_2[idx]);
    if(var_prc_out[idx] != NULL) var_prc_out[idx]=var_free(var_prc_out[idx]);
    
  } /* end loop over idx */
  if(dbg_lvl > 0) (void)fprintf(stderr,"\n");
  
  /* Close the input netCDF files */ 
  ncclose(in_id_1);
  ncclose(in_id_2);

  /* Close the output file and move it from the temporary to the permanent location */ 
  (void)fl_out_close(fl_out,fl_out_tmp,out_id);
  
  /* Dispose of the local copy of the file */ 
  if(FILE_1_RETRIEVED_FROM_REMOTE_LOCATION && REMOVE_REMOTE_FILES_AFTER_PROCESSING) (void)fl_rm(fl_in_1);
  if(FILE_2_RETRIEVED_FROM_REMOTE_LOCATION && REMOVE_REMOTE_FILES_AFTER_PROCESSING) (void)fl_rm(fl_in_2);
  
  Exit_gracefully();
} /* end main() */

var_sct *
scl_dbl_mk_var(double val)
/* 
   double val: input double precision value to turn into netCDF variable
   scl_dbl_mk_var: output netCDF variable structure representing val
 */ 
{
  /* Routine to turn a scalar double into a netCDF variable */ 

  static char *var_nm="Internally generated variable";

  var_sct *var;

  var=(var_sct *)malloc(sizeof(var_sct));

  var->id=-1;
  var->nc_id=-1;
  var->nbr_dim=0;
  var->nbr_att=-1;

  var->type=NC_DOUBLE;
  var->val.vp=(void *)malloc(nctypelen(var->type));
  (void)memcpy((void *)var->val.vp,(void *)(&val),nctypelen(var->type));

  /* Set defaults */ 
  var->is_rec_var=False;
  var->is_crd_var=False;
  var->sz=1L;
  var->sz_rec=1L;
  var->cid=-1;
  var->has_mss_val=False;

  /* Fill in pointer values */ 
  var->nm=(char *)strdup(var_nm);
  var->mss_val.vp=NULL;
  var->tally=NULL;
  var->dim_id=NULL;
  var->dim=NULL;
  var->srt=NULL;
  var->end=NULL;
  var->cnt=NULL;
  var->xrf=NULL;

  return var;
} /* end scl_dbl_mk_var */ 

