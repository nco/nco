/* $Header: /data/zender/nco_20150216/nco/src/nco/ncks.c,v 1.11 1999-08-30 07:07:23 zender Exp $ */

/* ncks -- netCDF Kitchen Sink */

/* (c) Copyright 1995--1999 University Corporation for Atmospheric Research 
   The file LICENSE contains the full copyright notice 
   Contact NSF/UCAR/NCAR/CGD/CMS for copyright assistance */

/* Purpose: Extract (subsets of) variables from a netCDF file. Print them to screen,
   or copy them to a new file, or both. */

/* Usage:
   ncks in.nc 
   ncks -v var1 in.nc 
   ncks in.nc foo.nc
   ncks -v var1 in.nc foo.nc
   ncks -H -C -v three_dim_var -d lon,3,0 in.nc
   ncks -p /ZENDER/tmp -l /data/zender/tmp h0001.nc foo.nc
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
  int cpy_var_def(int,int,int,char *);
  int cpy_var_def_lmt(int,int,int,char *,lmt_sct *,int);
  void cpy_var_val(int,int,char *);
  void cpy_var_val_lmt(int,int,char *,lmt_sct *,int);
  void prn_att(int,int);
  void prn_var_def(int,char *);
  void prn_var_val_lmt(int,char *,lmt_sct *,int,char *,bool,bool);
   
  bool ALPHABETIZE_OUTPUT=True; /* Option a */ 
  bool EXCLUDE_INPUT_LIST=False; /* Option c */ 
  bool FILE_RETRIEVED_FROM_REMOTE_LOCATION;
  bool FORCE_APPEND=False; /* Option A */ 
  bool FORCE_OVERWRITE=False; /* Option O */ 
  bool FORTRAN_STYLE=False; /* Option F */
  bool HISTORY_APPEND=True; /* Option h */
  bool OUTPUT_DATA=False; /* Option H */ 
  bool OUTPUT_VARIABLE_METADATA=False; /* Option m */ 
  bool OUTPUT_GLOBAL_METADATA=False; /* Option M */ 
  bool PRINT_DIMENSIONAL_UNITS=False; /* Option u */
  bool PROCESS_ALL_COORDINATES=False; /* Option c */
  bool PROCESS_ASSOCIATED_COORDINATES=True; /* Option C */
  bool REMOVE_REMOTE_FILES_AFTER_PROCESSING=True; /* Option R */ 

  char **var_lst_in;
  char **fl_lst_abb=NULL; /* Option a */ 
  char **fl_lst_in;
  char *dlm_sng=NULL;
  char *fl_in=NULL;
  char *fl_pth_lcl=NULL; /* Option l */ 
  char *lmt_arg[MAX_NC_DIMS];
  char *opt_sng;
  char *fl_out;
  char *fl_out_tmp;
  char *fl_pth=NULL; /* Option p */ 
  char *time_buf_srt;
  char *cmd_ln;
  char CVS_Id[]="$Id: ncks.c,v 1.11 1999-08-30 07:07:23 zender Exp $"; 
  char CVS_Revision[]="$Revision: 1.11 $";
  
  extern char *optarg;
  extern int ncopts;
  extern int optind;
  
  int idx;
  int in_id;  
  int nbr_abb_arg=0;
  int nbr_dim_fl;
  int nbr_lmt=0; /* Option d. NB: nbr_lmt gets incremented */
  int nbr_glb_att;
  int nbr_var_fl;
  int nbr_xtr=0; /* nbr_xtr won't otherwise be set for -c with no -v */ 
  int nbr_fl=0;
  int opt;
  int rec_dim_id;
  
  lmt_sct *lmt;

  nm_id_sct *xtr_lst=NULL; /* xtr_lst can get realloc()'d from NULL with -c option */ 

  time_t clock;

  /* Start the clock and save the command line */  
  cmd_ln=cmd_ln_sng(argc,argv);
  clock=time((time_t *)NULL);
  time_buf_srt=ctime(&clock);
  
  /* Get the program name and set the enum for the program (e.g., prg=ncra) */
  prg_nm=prg_prs(argv[0],&prg);

  /* Parse command line arguments */
  opt_sng="aACcD:d:FHhl:MmOp:rRs:uv:x";
  while((opt = getopt(argc,argv,opt_sng)) != EOF){
    switch(opt){
    case 'a': /* Toggle ALPHABETIZE_OUTPUT */
      ALPHABETIZE_OUTPUT=!ALPHABETIZE_OUTPUT;
      break;
    case 'A': /* Toggle FORCE_APPEND */
      FORCE_APPEND=!FORCE_APPEND;
      break;
    case 'C': /* Add to the extraction list any coordinates associated with variables to be extracted? */ 
      PROCESS_ASSOCIATED_COORDINATES=False;
      break;
    case 'c': /* Add all coordinates to the extraction list? */ 
      PROCESS_ALL_COORDINATES=True;
      break;
    case 'D': /* The debugging level.  Default is 0. */
      dbg_lvl=atoi(optarg);
      break;
    case 'd': /* Copy the argument for later processing */ 
      lmt_arg[nbr_lmt]=(char *)strdup(optarg);
      nbr_lmt++;
      break;
    case 'H': /* Print data to screen */
      OUTPUT_DATA=True;
      break;
    case 'h': /* Toggle appending to history global attribute */
      HISTORY_APPEND=!HISTORY_APPEND;
      break;
    case 'l': /* Get local path prefix for storing files retrieved from remote file system */
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
    case 'F': /* Toggle the style of printing out arrays. Default is C-style. */
      FORTRAN_STYLE=!FORTRAN_STYLE;
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
    case 's': /* User specified delimiter string for printed output */ 
      dlm_sng=optarg;
      break;
    case 'u': /* Turn on the printing of dimensional units. */
      PRINT_DIMENSIONAL_UNITS=!PRINT_DIMENSIONAL_UNITS;
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
  
  /* Get the number of variables, dimensions, and global attributes in the file */
  (void)ncinquire(in_id,&nbr_dim_fl,&nbr_var_fl,&nbr_glb_att,&rec_dim_id);
  
  /* Form the initial extraction list from the user input */ 
  xtr_lst=var_lst_mk(in_id,nbr_var_fl,var_lst_in,PROCESS_ALL_COORDINATES,&nbr_xtr);

  /* Change the included variables to excluded variables */ 
  if(EXCLUDE_INPUT_LIST) xtr_lst=var_lst_xcl(in_id,nbr_var_fl,xtr_lst,&nbr_xtr);

  /* Add all the coordinate variables to the extraction list */ 
  if(PROCESS_ALL_COORDINATES) xtr_lst=var_lst_add_crd(in_id,nbr_var_fl,nbr_dim_fl,xtr_lst,&nbr_xtr);

  /* Make sure all coordinates associated with each of the variables to be extracted is also on the list */ 
  if(PROCESS_ASSOCIATED_COORDINATES) xtr_lst=var_lst_ass_crd_add(in_id,xtr_lst,&nbr_xtr);

  /* Heapsort extraction list alphabetically or by variable ID */ 
  if(nbr_xtr > 1) xtr_lst=lst_heapsort(xtr_lst,nbr_xtr,ALPHABETIZE_OUTPUT);
    
  /* We now have the final list of variables to extract. Phew. */
  
  /* Find the coordinate/dimension values associated with the limits */ 
  for(idx=0;idx<nbr_lmt;idx++) (void)lmt_evl(in_id,lmt+idx,0L,FORTRAN_STYLE);
  
  if(fl_out != NULL){
    int out_id;  

    /* Open the output file */ 
    fl_out_tmp=fl_out_open(fl_out,FORCE_APPEND,FORCE_OVERWRITE,&out_id);
    
    /* Copy all the global attributes */ 
    (void)att_cpy(in_id,out_id,NC_GLOBAL,NC_GLOBAL);
    
    /* Catenate the timestamped command line to the "history" global attribute */ 
    if(HISTORY_APPEND) (void)hst_att_cat(out_id,cmd_ln);

    for(idx=0;idx<nbr_xtr;idx++){
      int var_out_id;
      
      /* Define the variable in the output file */ 
      if(nbr_lmt > 0) var_out_id=cpy_var_def_lmt(in_id,out_id,rec_dim_id,xtr_lst[idx].nm,lmt,nbr_lmt); else var_out_id=cpy_var_def(in_id,out_id,rec_dim_id,xtr_lst[idx].nm);
      /* Copy the variable's attributes */ 
      (void)att_cpy(in_id,out_id,xtr_lst[idx].id,var_out_id);
    } /* end loop over idx */

  /* Turn off the default filling behavior to enhance efficiency */ 
#if ( ! defined SUN4 ) && ( ! defined SUN4SOL2 ) && ( ! defined SUNMP )
    (void)ncsetfill(out_id,NC_NOFILL);
#endif
  
    /* Take the output file out of define mode */ 
    (void)ncendef(out_id);
    
    /* Copy the variable data */ 
    for(idx=0;idx<nbr_xtr;idx++){
      if(dbg_lvl > 2) (void)fprintf(stderr,"%s, ",xtr_lst[idx].nm);
      if(dbg_lvl > 0) (void)fflush(stderr);
      if(nbr_lmt > 0) (void)cpy_var_val_lmt(in_id,out_id,xtr_lst[idx].nm,lmt,nbr_lmt); else (void)cpy_var_val(in_id,out_id,xtr_lst[idx].nm);
    } /* end loop over idx */
    
    /* Close the output file and move it from the temporary to the permanent location */ 
    (void)fl_out_close(fl_out,fl_out_tmp,out_id);

  } /* end if fl_out != NULL */ 
  
  if(OUTPUT_GLOBAL_METADATA){
    (void)fprintf(stdout,"Opened file %s: dimensions = %i, variables = %i, global atts. = %i, id = %i\n",fl_in,nbr_dim_fl,nbr_var_fl,nbr_glb_att,in_id);
    if(rec_dim_id != -1){
      char rec_dim_nm[MAX_NC_NAME];
      int nbr_rec_var;
      long rec_dim_sz;
      
      (void)ncdiminq(in_id,rec_dim_id,rec_dim_nm,&rec_dim_sz);
      (void)ncrecinq(in_id,&nbr_rec_var,(int *)NULL,(long *)NULL);
      (void)fprintf(stdout,"Record dimension: name = %s, size = %li, record variables = %i\n\n",rec_dim_nm,rec_dim_sz,nbr_rec_var);
    } /* end if */ 
    
    /* Print all global attributes */ 
    (void)prn_att(in_id,NC_GLOBAL);
  } /* endif OUTPUT_GLOBAL_METADATA */
  
  if(OUTPUT_VARIABLE_METADATA){
    for(idx=0;idx<nbr_xtr;idx++){
      /* Print the variable's definition */ 
      (void)prn_var_def(in_id,xtr_lst[idx].nm);
      /* Print the variable's attributes */ 
      (void)prn_att(in_id,xtr_lst[idx].id);
    } /* end loop over idx */
  } /* end if OUTPUT_VARIABLE_METADATA */
  
  if(OUTPUT_DATA){
    for(idx=0;idx<nbr_xtr;idx++) (void)prn_var_val_lmt(in_id,xtr_lst[idx].nm,lmt,nbr_lmt,dlm_sng,FORTRAN_STYLE,PRINT_DIMENSIONAL_UNITS);
  } /* end if OUTPUT_DATA */
  
  /* Close the input netCDF file */ 
  ncclose(in_id);
  
  /* Dispose of the local copy of the file */ 
  if(FILE_RETRIEVED_FROM_REMOTE_LOCATION && REMOVE_REMOTE_FILES_AFTER_PROCESSING) (void)fl_rm(fl_in);

  Exit_gracefully();
  return EXIT_SUCCESS;
} /* end main() */

char *
type_fmt_sng(nc_type type)
/*  
   nc_type type: input netCDF type
   char *nc_type_nm(): output sprintf() format string for type
*/ 
{
  switch (type) {
  case NC_FLOAT:
    return "%g";
  case NC_DOUBLE:
    return "%g";
  case NC_LONG:
    return "%li";
  case NC_SHORT:
    return "%i";
  case NC_CHAR:
    return "%c";
  case NC_BYTE:
    return "%c";
  } /* end switch */ 

  /* Some C compilers, e.g., SGI cc, need a return statement at the end of non-void functions */ 
  return (char *)NULL;
} /* end type_fmt_sng() */ 

/* The other printout method must use a recursive call to step
   through the dimensions, because the number of dimensions,
   and thus of loops, is not known in advance. */
void recursive_prn
(int *min_dim_idx, /* input var: array of minimum dimension values */
 int *max_dim_idx, /* input var: array of maximum dimension values */
 int idx, /* input var: current dimension to process */ 
 int nbr_dim, /* input var: total number of dimensions in variable */ 
 int lmn) /* output var: index into the 1D variable array */ 
{
  int tmp_idx;

  /* Check for the recursion exiting condition first */ 
  if(idx > nbr_dim-1){
    ;
  }else{
    /* Recursively move to the next dimension */ 
    for(tmp_idx=min_dim_idx[idx];
	tmp_idx<=max_dim_idx[idx];
	tmp_idx++){
      (void)recursive_prn(min_dim_idx,max_dim_idx,idx,nbr_dim,lmn);
    } /* end loop over tmp_idx */
  } /* end else */ 
} /* end recursive_prn() */ 

void 
prn_att(int in_id,int var_id)
/* 
   int in_id: input netCDF input-file ID
   int var_id: input netCDF input-variable ID
 */ 
{
  /* Routine to print all the global attributes in a netCDF file,
     or all the attributes for a particular netCDF variable. 
     If var_id == NC_GLOBAL ( = -1) the global attributes are printed,
     otherwise the variable's attributes are printed. */

  char dlm_sng[3];
  char src_sng[MAX_NC_NAME];

  int att_lmn;
  int idx;
  int nbr_att;

  att_sct *att;

  if(var_id == NC_GLOBAL){
    /* Get the number of global attributes for the file */
    (void)ncinquire(in_id,(int *)NULL,(int *)NULL,&nbr_att,(int *)NULL);
    (void)strcpy(src_sng,"Global");
  }else{
    /* Get the name and number of attributes for the variable */
    (void)ncvarinq(in_id,var_id,src_sng,(nc_type *)NULL,(int *)NULL,(int *)NULL,&nbr_att);
  } /* end else */ 

  /* Allocate space for the attribute names and types */
  if(nbr_att > 0) att=(att_sct *)malloc(nbr_att*sizeof(att_sct));
    
  /* Get the attributes' names, types, lengths, and values */
  for(idx=0;idx<nbr_att;idx++){
    
    att[idx].nm=(char *)malloc(MAX_NC_NAME*sizeof(char));
    (void)ncattname(in_id,var_id,idx,att[idx].nm);
    (void)ncattinq(in_id,var_id,att[idx].nm,&att[idx].type,&att[idx].sz);

    /* Allocate enough space to hold the attribute */ 
    att[idx].val.vp=(void *)malloc(att[idx].sz*nctypelen(att[idx].type));
    (void)ncattget(in_id,var_id,att[idx].nm,att[idx].val.vp);
    (void)fprintf(stdout,"%s attribute %i: %s, size = %i %s, value = ",src_sng,idx,att[idx].nm,att[idx].sz,nc_type_nm(att[idx].type));
    
    /* Typecast the pointer to the values before access */ 
    (void)cast_void_nctype(att[idx].type,&att[idx].val);

    if(att[idx].sz > 1L) (void)strcpy(dlm_sng,", "); else (void)strcpy(dlm_sng,"");
    switch(att[idx].type){
    case NC_FLOAT:
      for(att_lmn=0;att_lmn<att[idx].sz;att_lmn++) (void)fprintf(stdout,"%g%s",att[idx].val.fp[att_lmn],dlm_sng);
      break;
    case NC_DOUBLE:
      for(att_lmn=0;att_lmn<att[idx].sz;att_lmn++) (void)fprintf(stdout,"%g%s",att[idx].val.dp[att_lmn],dlm_sng);
      break;
    case NC_SHORT:
      for(att_lmn=0;att_lmn<att[idx].sz;att_lmn++) (void)fprintf(stdout,"%hi%s",att[idx].val.sp[att_lmn],dlm_sng);
      break;
    case NC_LONG:
      for(att_lmn=0;att_lmn<att[idx].sz;att_lmn++) (void)fprintf(stdout,"%li%s",(long)att[idx].val.lp[att_lmn],dlm_sng);
      break;
    case NC_CHAR:
      for(att_lmn=0;att_lmn<att[idx].sz;att_lmn++){
        char char_foo;

	/* Assume \0 is a string terminator and don't print it out. */
	if((char_foo=att[idx].val.cp[att_lmn]) != '\0') (void)fprintf(stdout,"%c",char_foo);
      } /* end loop over element */ 
      break;
    case NC_BYTE:
      for(att_lmn=0;att_lmn<att[idx].sz;att_lmn++) (void)fprintf(stdout,"%c",att[idx].val.bp[att_lmn]);
      break;
    } /* end switch */ 
    (void)fprintf(stdout,"\n");
    
  } /* end loop over attributes */
  (void)fprintf(stdout,"\n");
  (void)fflush(stdout);
  
  /* Free the space holding the attribute values */
  for(idx=0;idx<nbr_att;idx++){
    (void)free(att[idx].val.vp);
    (void)free(att[idx].nm);
  } /* end loop over attributes */

  /* Free the rest of the space allocated for attribute info. */
  if(nbr_att > 0) (void)free(att);

} /* end prn_att() */ 

int 
cpy_var_def(int in_id,int out_id,int rec_dim_id,char *var_nm)
/* 
   int in_id: input netCDF input-file ID
   int out_id: input netCDF output-file ID
   int rec_dim_id: input input-file record dimension ID
   char *var_nm: input variable name
   int cpy_var_def(): output output-file variable ID
 */ 
{
  /* Routine to copy the variable metadata from an input netCDF file
     to an output netCDF file. This routine does not take into 
     account any user-specified limits, it just copies what it finds. */

  int *dim_in_id;
  int *dim_out_id;
  int idx;
  int nbr_dim;
  int var_in_id;
  int var_out_id;

  extern int ncopts;
  
  nc_type var_type;

  ncopts=0; 
  /* See if the requested variable is already in the output file. */
  var_out_id=ncvarid(out_id,var_nm);
  if(var_out_id != -1) return var_out_id;
  /* See if the requested variable is in the input file. */
  var_in_id=ncvarid(in_id,var_nm);
  if(var_in_id == -1) (void)fprintf(stdout,"%s: ERROR unable to find variable \"%s\"\n",prg_nm_get(),var_nm);
  ncopts=NC_VERBOSE | NC_FATAL; 
  
  /* Get the type of the variable and the number of dimensions. */
  (void)ncvarinq(in_id,var_in_id,(char *)NULL,&var_type,&nbr_dim,(int *)NULL,(int *)NULL);
  
  /* Recall:
     1. The dimensions must be defined before the variable.
     2. The variable must be defined before the attributes. */

  /* Allocate space to hold the dimension IDs */ 
  dim_in_id=(int *)malloc(nbr_dim*sizeof(int));
  dim_out_id=(int *)malloc(nbr_dim*sizeof(int));
  
  /* Get the dimension IDs */
  (void)ncvarinq(in_id,var_in_id,(char *)NULL,(nc_type *)NULL,(int *)NULL,dim_in_id,(int *)NULL);
  
  /* Get the dimension sizes and names */
  for(idx=0;idx<nbr_dim;idx++){
    char dim_nm[MAX_NC_NAME];
    long dim_sz;
    
    (void)ncdiminq(in_id,dim_in_id[idx],dim_nm,&dim_sz);
    
    /* See if the dimension has already been defined */ 
    ncopts=0; 
    dim_out_id[idx]=ncdimid(out_id,dim_nm);
    ncopts=NC_VERBOSE | NC_FATAL; 
    
    /* If the dimension hasn't been defined, copy it */
    if(dim_out_id[idx] == -1){
      if(dim_in_id[idx] != rec_dim_id){
	dim_out_id[idx]=ncdimdef(out_id,dim_nm,dim_sz);
      }else{
	dim_out_id[idx]=ncdimdef(out_id,dim_nm,NC_UNLIMITED);
      } /* end else */
    } /* end if */
  } /* end loop over dim */
  
  /* Define the variable in the output file */ 
  var_out_id=ncvardef(out_id,var_nm,var_type,nbr_dim,dim_out_id);
  
  /* Free the space holding the dimension IDs */ 
  (void)free(dim_in_id);
  (void)free(dim_out_id);
  
  return var_out_id;
} /* end cpy_var_def() */ 

int 
cpy_var_def_lmt(int in_id,int out_id,int rec_dim_id,char *var_nm,lmt_sct *lmt,int nbr_lmt)
/* 
   int in_id: input netCDF input-file ID
   int out_id: input netCDF output-file ID
   int rec_dim_id: input input-file record dimension ID
   char *var_nm: input variable name
   lmt_sct *lmt: input structure from lmt_evl() holding dimension limit info.
   int nbr_lmt: input number of dimensions with user-specified limits
   int cpy_var_def_lmt(): output output-file variable ID
 */ 
{
  /* Routine to copy the variable metadata from an input netCDF file
     to an output netCDF file. This routine truncates the dimensions
     in the variable definition in the output file according to any
     user-specified limits. */

  int *dim_in_id;
  int *dim_out_id;
  int idx;
  int nbr_dim;
  int var_in_id;
  int var_out_id;

  extern int ncopts;
  
  nc_type var_type;

  /* See if requested variable is already in output file. */
  ncopts=0; 
  var_out_id=ncvarid(out_id,var_nm);
  if(var_out_id != -1) return var_out_id;
  /* See if requested variable is in input file. */
  var_in_id=ncvarid(in_id,var_nm);
  if(var_in_id == -1) (void)fprintf(stdout,"%s: ERROR unable to find variable \"%s\"\n",prg_nm_get(),var_nm);
  ncopts=NC_VERBOSE | NC_FATAL; 
  
  /* Get type of variable and number of dimensions. */
  (void)ncvarinq(in_id,var_in_id,(char *)NULL,&var_type,&nbr_dim,(int *)NULL,(int *)NULL);
  
  /* Recall:
     1. Dimensions must be defined before variable.
     2. Variable must be defined before attributes. */
     
  /* Allocate space to hold dimension IDs */ 
  dim_in_id=(int *)malloc(nbr_dim*sizeof(int));
  dim_out_id=(int *)malloc(nbr_dim*sizeof(int));
  
  /* Get dimension IDs */
  (void)ncvarinq(in_id,var_in_id,(char *)NULL,(nc_type *)NULL,(int *)NULL,dim_in_id,(int *)NULL);
  
  /* Get dimension sizes and names */
  for(idx=0;idx<nbr_dim;idx++){
    char dim_nm[MAX_NC_NAME];
    long dim_sz;
    
    (void)ncdiminq(in_id,dim_in_id[idx],dim_nm,&dim_sz);
    
    /* See if dimension has already been defined */ 
    ncopts=0; 
    dim_out_id[idx]=ncdimid(out_id,dim_nm);
    ncopts=NC_VERBOSE | NC_FATAL; 
    
    /* If dimension hasn't been defined, copy it */
    if(dim_out_id[idx] == -1){
      if(dim_in_id[idx] != rec_dim_id){
	int lmt_idx;

	/* Decide whether this dimension has any user-specified limits */ 
	for(lmt_idx=0;lmt_idx<nbr_lmt;lmt_idx++){
	  if(lmt[lmt_idx].id == dim_in_id[idx]){
	    dim_sz=lmt[lmt_idx].cnt;
	    break;
	  } /* end if */
	} /* end loop over lmt_idx */
	
	dim_out_id[idx]=ncdimdef(out_id,dim_nm,dim_sz);
      }else{
	dim_out_id[idx]=ncdimdef(out_id,dim_nm,NC_UNLIMITED);
      } /* end else */
    } /* end if */
  } /* end loop over dim */
  
  /* Define variable in output file */ 
  var_out_id=ncvardef(out_id,var_nm,var_type,nbr_dim,dim_out_id);
  
  /* Free space holding dimension IDs */ 
  (void)free(dim_in_id);
  (void)free(dim_out_id);
  
  return var_out_id;
} /* end cpy_var_def_lmt() */ 

void 
cpy_var_val(int in_id,int out_id,char *var_nm)
/* 
   int in_id: input netCDF input-file ID
   int out_id: input netCDF output-file ID
   char *var_nm: input variable name
 */ 
{
  /* Routine to copy the variable data from an input netCDF file
     to an output netCDF file. This routine does not take into 
     account any user-specified limits, it just copies what it finds. */

  int *dim_id;
  int idx;
  int nbr_dim;
  int nbr_dim_in;
  int nbr_dim_out;
  int var_in_id;
  int var_out_id;

  extern int ncopts;
  
  long *dim_cnt;
  long *dim_sz;
  long *dim_srt;
  long var_sz=1L;

  nc_type var_type;

  void *void_ptr;

  /* Get the var_id for the requested variable from both files. */
  var_in_id=ncvarid(in_id,var_nm);
  var_out_id=ncvarid(out_id,var_nm);
  
  /* Get number of dimensions for variable. */
  (void)ncvarinq(out_id,var_out_id,(char *)NULL,&var_type,&nbr_dim_out,(int *)NULL,(int *)NULL);
  (void)ncvarinq(in_id,var_in_id,(char *)NULL,&var_type,&nbr_dim_in,(int *)NULL,(int *)NULL);
  if(nbr_dim_out != nbr_dim_in){
    (void)fprintf(stderr,"%s: ERROR attempt to write %d dimensional input variable %s to %d dimensional space in output file. \nHINT: When using -A (append) option, all appended variables must be the same rank in the input file as in the output file. ncwa operator is useful at ridding variables of extraneous (size = 1) dimensions. Read the manual to see how.\n",prg_nm_get(),nbr_dim_in,var_nm,nbr_dim_out);
    exit(EXIT_FAILURE);
  } /* endif */ 
  nbr_dim=nbr_dim_out;
  
  /* Allocate space to hold the dimension IDs */ 
  dim_cnt=(long *)malloc(nbr_dim*sizeof(long));
  dim_id=(int *)malloc(nbr_dim*sizeof(int));
  dim_sz=(long *)malloc(nbr_dim*sizeof(long));
  dim_srt=(long *)malloc(nbr_dim*sizeof(long));
  
  /* Get the dimension IDs from the input file */
  (void)ncvarinq(in_id,var_in_id,(char *)NULL,(nc_type *)NULL,(int *)NULL,dim_id,(int *)NULL);
  
  /* Get the dimension sizes from the input file */
  for(idx=0;idx<nbr_dim;idx++){
  /* NB: For the record dimension, ncdiminq() returns the maximum 
     value used so far in writing data for that dimension.
     Thus if you read the dimension sizes from the output file, then
     the ncdiminq() returns dim_sz=0 for the record dimension
     until a variable has been written with that dimension. This is
     the reason for always reading the input file for the dimension
     sizes. */
    (void)ncdiminq(in_id,dim_id[idx],(char *)NULL,dim_cnt+idx);

    /* Initialize the indicial offset and stride arrays */
    dim_srt[idx]=0L;
    var_sz*=dim_cnt[idx];
  } /* end loop over dim */
      
  /* Allocate enough space to hold the variable */ 
  void_ptr=(void *)malloc(var_sz*nctypelen(var_type));
  if(void_ptr == NULL){
    (void)fprintf(stderr,"%s: ERROR unable to malloc() %ld bytes for %s\n",prg_nm_get(),var_sz*nctypelen(var_type),var_nm);
    exit(EXIT_FAILURE);
  } /* end if */ 

  /* Get the variable */
  if(nbr_dim==0){
    ncvarget1(in_id,var_in_id,0L,void_ptr);
    ncvarput1(out_id,var_out_id,0L,void_ptr);
  }else{ /* end if variable is a scalar */ 
    ncvarget(in_id,var_in_id,dim_srt,dim_cnt,void_ptr);
    ncvarput(out_id,var_out_id,dim_srt,dim_cnt,void_ptr);
  } /* end if variable is an array */ 

  /* Free the space that held the dimension IDs */ 
  (void)free(dim_cnt);
  (void)free(dim_id);
  (void)free(dim_sz);
  (void)free(dim_srt);

  /* Free the space that held the variable */ 
  (void)free(void_ptr);

} /* end cpy_var_val() */ 

void 
cpy_var_val_lmt(int in_id,int out_id,char *var_nm,lmt_sct *lmt,int nbr_lmt)
/* 
   int in_id: input netCDF input-file ID
   int out_id: input netCDF output-file ID
   char *var_nm: input variable name
   lmt_sct *lmt: input structure from lmt_evl() holding dimension limit info.
   int nbr_lmt: input number of dimensions with user-specified limits
 */ 
{
  /* Routine to copy variable data from input netCDF file to output netCDF file. 
     Routine truncates dimensions in variable definition in output file according to user-specified limits. */

  bool SRD=False;
  bool WRP=False;

  int *dim_id;

  int dim_idx;
  int lmt_idx;
  int nbr_dim;
  int nbr_dim_in;
  int nbr_dim_out;
  int var_in_id;
  int var_out_id;

  extern int ncopts;
  
  /* For regular data */ 
  long *dim_map;
  long *dim_srd;
  long *dim_cnt;
  long *dim_in_srt;
  long *dim_out_srt;
  long *dim_sz;

  long var_sz=1L;

  nc_type var_type;

  void *void_ptr;

  /* Get var_id for requested variable from both files. */
  var_in_id=ncvarid(in_id,var_nm);
  var_out_id=ncvarid(out_id,var_nm);
  
  /* Get number of dimensions for variable. */
  (void)ncvarinq(out_id,var_out_id,(char *)NULL,&var_type,&nbr_dim_out,(int *)NULL,(int *)NULL);
  (void)ncvarinq(in_id,var_in_id,(char *)NULL,&var_type,&nbr_dim_in,(int *)NULL,(int *)NULL);
  if(nbr_dim_out != nbr_dim_in){
    (void)fprintf(stderr,"%s: ERROR attempt to write %d dimensional input variable %s to %d dimensional space in output file\n",prg_nm_get(),nbr_dim_in,var_nm,nbr_dim_out);
    exit(EXIT_FAILURE);
  } /* endif */ 
  nbr_dim=nbr_dim_out;
  
  /* Allocate space to hold dimension IDs */ 
  dim_map=(long *)malloc(nbr_dim*sizeof(long));
  dim_srd=(long *)malloc(nbr_dim*sizeof(long));
  dim_cnt=(long *)malloc(nbr_dim*sizeof(long));
  dim_id=(int *)malloc(nbr_dim*sizeof(int));
  dim_in_srt=(long *)malloc(nbr_dim*sizeof(long));
  dim_out_srt=(long *)malloc(nbr_dim*sizeof(long));
  dim_sz=(long *)malloc(nbr_dim*sizeof(long));
  
  /* Get dimension IDs from input file */
  (void)ncvarinq(in_id,var_in_id,(char *)NULL,(nc_type *)NULL,(int *)NULL,dim_id,(int *)NULL);
  
  /* Get dimension sizes from input file */
  for(dim_idx=0;dim_idx<nbr_dim;dim_idx++){
    
    /* NB: For record dimension, ncdiminq() returns maximum 
       value used so far in writing data for that dimension.
       Thus if you read dimension sizes from output file, then
       ncdiminq() returns dim_sz=0 for the record dimension
       until a variable has been written with that dimension. This is
       the reason for always reading the input file for dimension
       sizes. */

    /* dim_cnt may be overwritten by user-specified limits */ 
    (void)ncdiminq(in_id,dim_id[dim_idx],(char *)NULL,dim_sz+dim_idx);

    /* Set default start vectors: dim_in_srt may be overwritten by user-specified limits */
    dim_cnt[dim_idx]=dim_sz[dim_idx];
    dim_in_srt[dim_idx]=0L;
    dim_out_srt[dim_idx]=0L;
    dim_srd[dim_idx]=1L;
    dim_map[dim_idx]=1L;

    /* Decide whether this dimension has any user-specified limits */ 
    for(lmt_idx=0;lmt_idx<nbr_lmt;lmt_idx++){
      if(lmt[lmt_idx].id == dim_id[dim_idx]){
	dim_cnt[dim_idx]=lmt[lmt_idx].cnt;
	dim_in_srt[dim_idx]=lmt[lmt_idx].srt;
	dim_srd[dim_idx]=lmt[lmt_idx].srd;
	if(lmt[lmt_idx].srt > lmt[lmt_idx].end) WRP=True;
	if(lmt[lmt_idx].srd != 1L) SRD=True;
	break;
      } /* end if */
    } /* end loop over lmt_idx */

    var_sz*=dim_cnt[dim_idx];
  } /* end loop over dim */
      
  /* Allocate enough space to hold variable */ 
  void_ptr=(void *)malloc(var_sz*nctypelen(var_type));
  if(void_ptr == NULL){
    (void)fprintf(stderr,"%s: ERROR unable to malloc() %ld bytes for %s\n",prg_nm_get(),var_sz*nctypelen(var_type),var_nm);
    exit(EXIT_FAILURE);
  } /* end if */ 

  /* Copy variable */
  if(nbr_dim==0){ /* copy scalar */ 
    ncvarget1(in_id,var_in_id,0L,void_ptr);
    ncvarput1(out_id,var_out_id,0L,void_ptr);
  }else if(!WRP){ /* copy contiguous array */ 
    if(!SRD) ncvarget(in_id,var_in_id,dim_in_srt,dim_cnt,void_ptr); else ncvargetg(in_id,var_in_id,dim_in_srt,dim_cnt,dim_srd,(long *)NULL,void_ptr);
    ncvarput(out_id,var_out_id,dim_out_srt,dim_cnt,void_ptr);
  }else if(WRP){ /* copy wrapped array */ 
    int dim_idx;
    int lmt_idx;
    
    /* For wrapped data */ 
    long *dim_in_srt_1=NULL;
    long *dim_in_srt_2=NULL;
    long *dim_out_srt_1=NULL;
    long *dim_out_srt_2=NULL;
    long *dim_cnt_1=NULL;
    long *dim_cnt_2=NULL;
    
    dim_in_srt_1=(long *)malloc(nbr_dim*sizeof(long));
    dim_in_srt_2=(long *)malloc(nbr_dim*sizeof(long));
    dim_out_srt_1=(long *)malloc(nbr_dim*sizeof(long));
    dim_out_srt_2=(long *)malloc(nbr_dim*sizeof(long));
    dim_cnt_1=(long *)malloc(nbr_dim*sizeof(long));
    dim_cnt_2=(long *)malloc(nbr_dim*sizeof(long));
    
    /* Variable contains a wrapped dimension, requires two reads */ 
    /* For each dimension in the input variable */ 
    for(dim_idx=0;dim_idx<nbr_dim;dim_idx++){
      
      /* dim_cnt may be overwritten by user-specified limits */ 
      (void)ncdiminq(in_id,dim_id[dim_idx],(char *)NULL,dim_sz+dim_idx);
      
      /* Set default vectors */
      dim_cnt[dim_idx]=dim_cnt_1[dim_idx]=dim_cnt_2[dim_idx]=dim_sz[dim_idx];
      dim_in_srt[dim_idx]=dim_in_srt_1[dim_idx]=dim_in_srt_2[dim_idx]=0L;
      dim_out_srt[dim_idx]=dim_out_srt_1[dim_idx]=dim_out_srt_2[dim_idx]=0L;
      dim_srd[dim_idx]=1L;
      dim_map[dim_idx]=1L;
      
      /* Is there a limit specified for this dimension? */ 
      for(lmt_idx=0;lmt_idx<nbr_lmt;lmt_idx++){
	if(lmt[lmt_idx].id == dim_id[dim_idx]){ /* Yes, there is a limit on this dimension */ 
	  dim_cnt[dim_idx]=dim_cnt_1[dim_idx]=dim_cnt_2[dim_idx]=lmt[lmt_idx].cnt;
	  dim_in_srt[dim_idx]=dim_in_srt_1[dim_idx]=dim_in_srt_2[dim_idx]=lmt[lmt_idx].srt;
	  dim_srd[dim_idx]=lmt[lmt_idx].srd;
	  if(lmt[lmt_idx].srd != 1L) SRD=True;
	  if(lmt[lmt_idx].srt > lmt[lmt_idx].end){ /* WRP true for this dimension */ 
	    WRP=True;
	    if(lmt[lmt_idx].srd != 1L){ /* SRD true for this dimension */ 
	      long greatest_srd_multiplier_1st_hyp_slb; /* greatest integer m such that srt+m*srd < dim_sz */
	      long last_good_idx_1st_hyp_slb; /* C index of last valid member of 1st hyperslab (= srt+m*srd) */ 
	      long left_over_idx_1st_hyp_slb; /* # elements from first hyperslab to count towards current stride */ 
	      long first_good_idx_2nd_hyp_slb; /* C index of first valid member of 2nd hyperslab, if any */ 

	      /* NB: Perform these operations with integer arithmatic or else! */ 
	      dim_cnt_1[dim_idx]=1L+(dim_sz[dim_idx]-lmt[lmt_idx].srt-1L)/lmt[lmt_idx].srd; 
	      /* Wrapped dimensions with a stride may not start at idx 0 on second read */ 
	      greatest_srd_multiplier_1st_hyp_slb=(dim_sz[dim_idx]-lmt[lmt_idx].srt-1L)/lmt[lmt_idx].srd;
	      last_good_idx_1st_hyp_slb=lmt[lmt_idx].srt+lmt[lmt_idx].srd*greatest_srd_multiplier_1st_hyp_slb;
	      left_over_idx_1st_hyp_slb=dim_sz[dim_idx]-last_good_idx_1st_hyp_slb-1L;
	      first_good_idx_2nd_hyp_slb=(last_good_idx_1st_hyp_slb+lmt[lmt_idx].srd)%dim_sz[dim_idx];
	      dim_in_srt_2[dim_idx]=lmt[lmt_idx].srd-left_over_idx_1st_hyp_slb-1L;
	    }else{ /* !SRD */ 
	      dim_in_srt_2[dim_idx]=0L;
	      dim_cnt_1[dim_idx]=dim_sz[dim_idx]-lmt[lmt_idx].srt;
	    } /* end else */ 
	    dim_cnt_2[dim_idx]=dim_cnt[dim_idx]-dim_cnt_1[dim_idx];
	    dim_out_srt_2[dim_idx]=dim_cnt_1[dim_idx];
	  } /* end if WRP */ 
	  break; /* Move on to next dimension in variable */ 
	} /* end if */
      } /* end loop over lmt */
    } /* end loop over dim */
    
    if(dbg_lvl == 5){
      (void)fprintf(stderr,"\nvar = %s\n",var_nm);
      (void)fprintf(stderr,"dim\tcnt\tsrtin1\tcnt1\tsrtout1\tsrtin2\tcnt2\tsrtout2\n");
      for(dim_idx=0;dim_idx<nbr_dim;dim_idx++) (void)fprintf(stderr,"%d\t%ld\t%ld\t%ld\t%ld\t%ld\t%ld\t%ld\t\n",dim_idx,dim_cnt[dim_idx],dim_in_srt_1[dim_idx],dim_cnt_1[dim_idx],dim_out_srt_1[dim_idx],dim_in_srt_2[dim_idx],dim_cnt_2[dim_idx],dim_out_srt_2[dim_idx]);
      (void)fflush(stderr);
    } /* end if dbg */ 

    if(False){
      /* If this is a coordinate variable, perform a monotonicity check */
      bool CRD=False;
      bool MNT=False;

      double val_dbl;
      double wrp_spn;
      double wrp_max;
      double wrp_min;

      long idx;

      if(nbr_dim == 1){
	char dim_nm[MAX_NC_NAME];
	
	(void)ncdiminq(in_id,dim_id[0],dim_nm,(long *)NULL);
	if(!strcmp(dim_nm,var_nm)) CRD=True; else CRD=False;
      } /* end if */       
      
      if(CRD && MNT){ /* If this is a wrapped coordinate then apply monotonicity filter if requested */ 
	ncvarget(in_id,var_in_id,dim_in_srt_1,dim_cnt_1,void_ptr);
	/* Convert coordinate to double */ 
	for(idx=0;idx<var_sz;idx++){
	  switch(var_type){
	  case NC_FLOAT: /* val_dbl=void_ptr.fp[idx]; */ break; 
	  case NC_DOUBLE:
	  case NC_LONG:
	  case NC_SHORT:
	  case NC_CHAR:
	  case NC_BYTE:
	  default:
	    (void)fprintf(stdout,"%s: ERROR Unknown nc_type %d in cpy_var_val_lmt()\n",prg_nm_get(),var_type);
	    exit(EXIT_FAILURE);
	    break;
	  } /* end switch */ 
	  
	  /* Ensure val_dbl is between specified bounds */ 
	  wrp_spn=wrp_max-wrp_min;
	  if(val_dbl < wrp_min) val_dbl+=wrp_spn;
	  if(val_dbl > wrp_max) val_dbl-=wrp_spn;
	} /* end loop over idx */ 
      } /* endif CRD && MNT */
    } /* endif False */ 
    
    if(!SRD){
      ncvarget(in_id,var_in_id,dim_in_srt_1,dim_cnt_1,void_ptr);
      ncvarput(out_id,var_out_id,dim_out_srt_1,dim_cnt_1,void_ptr);
      ncvarget(in_id,var_in_id,dim_in_srt_2,dim_cnt_2,void_ptr);
      ncvarput(out_id,var_out_id,dim_out_srt_2,dim_cnt_2,void_ptr);
    }else{
      ncvargetg(in_id,var_in_id,dim_in_srt_1,dim_cnt_1,dim_srd,(long *)NULL,void_ptr);
      ncvarput(out_id,var_out_id,dim_out_srt_1,dim_cnt_1,void_ptr);
      ncvargetg(in_id,var_in_id,dim_in_srt_2,dim_cnt_2,dim_srd,(long *)NULL,void_ptr);
      ncvarput(out_id,var_out_id,dim_out_srt_2,dim_cnt_2,void_ptr);
    } /* end else */ 
    
    (void)free(dim_in_srt_1);
    (void)free(dim_in_srt_2);
    (void)free(dim_out_srt_1);
    (void)free(dim_out_srt_2);
    (void)free(dim_cnt_1);
    (void)free(dim_cnt_2);

  } /* end if WRP */ 

  /* Free space that held dimension IDs */ 
  (void)free(dim_map);
  (void)free(dim_srd);
  (void)free(dim_cnt);
  (void)free(dim_id);
  (void)free(dim_in_srt);
  (void)free(dim_out_srt);
  (void)free(dim_sz);

  /* Free space that held variable */ 
  (void)free(void_ptr);

} /* end cpy_var_val_lmt() */ 

void
prn_var_def(int in_id,char *var_nm)
/* 
   int in_id: input netCDF input-file ID
   char *var_nm: input variable name
 */ 
{
/* Routine to print the variable metadata. This routine does not take into 
   account any user-specified limits, it just prints what it finds. */

  extern int ncopts;

  int *dim_id;
  int idx;
  int nbr_dim;
  int nbr_att;
  int rec_dim_id;
  int var_id;
  
  nc_type var_type;
  
  dim_sct *dim;

  /* See if the requested variable is in the input file. */
  var_id=ncvarid(in_id,var_nm);

  /* Get the number of dimensions, type, and number of attributes for the variable. */
  (void)ncvarinq(in_id,var_id,(char *)NULL,&var_type,&nbr_dim,(int *)NULL,&nbr_att);

  /* Get the ID of the record dimension, if any */
  (void)ncinquire(in_id,(int *)NULL,(int *)NULL,(int *)NULL,&rec_dim_id);

  /* Print the header for the variable */ 
  (void)fprintf(stdout,"%s: # dim. = %i, %s, # att. = %i, ID = %i\n",var_nm,nbr_dim,nc_type_nm(var_type),nbr_att,var_id);

  if(nbr_dim > 0){
    /* Allocate space for dimension info */
    dim=(dim_sct *)malloc(nbr_dim*sizeof(dim_sct));
    dim_id=(int *)malloc(nbr_dim*sizeof(int));
  } /* end if nbr_dim > 0 */
  
  /* Get dimension IDs */
  (void)ncvarinq(in_id,var_id,(char *)NULL,(nc_type *)NULL,(int *)NULL,dim_id,(int *)NULL);
  
  /* Get dimension sizes and names */
  for(idx=0;idx<nbr_dim;idx++){
    
    dim[idx].nm=(char *)malloc(MAX_NC_NAME*sizeof(char));
    dim[idx].id=dim_id[idx];
    (void)ncdiminq(in_id,dim[idx].id,dim[idx].nm,&dim[idx].sz);
    
    /* Is dimension a coordinate, i.e., stored as a variable? */ 
    ncopts=0; 
    dim[idx].cid=ncvarid(in_id,dim[idx].nm);
    ncopts=NC_VERBOSE | NC_FATAL; 
    
    if(dim[idx].cid != -1){
      /* Find out what type of variable the coordinate is */ 
      (void)ncvarinq(in_id,dim[idx].cid,(char *)NULL,&dim[idx].type,(int *)NULL,(int *)NULL,(int *)NULL);
      (void)fprintf(stdout,"%s dimension %i: %s, size = %li %s, dim. ID = %d (CRD)",var_nm,idx,dim[idx].nm,dim[idx].sz,nc_type_nm(dim[idx].type),dim[idx].id);
    }else{
      (void)fprintf(stdout,"%s dimension %i: %s, size = %li, dim. ID = %d",var_nm,idx,dim[idx].nm,dim[idx].sz,dim[idx].id);
    } /* end else */ 
    if(dim[idx].id == rec_dim_id) (void)fprintf(stdout,"(REC)"); 
    (void)fprintf(stdout,"\n"); 
    
  } /* end loop over dim */
  
  /* Find the total size of the variable array */ 
  if(nbr_dim>0){
    long var_sz=1L;
    char sz_sng[100];
    char sng_foo[200];

    for(idx=0;idx<nbr_dim;idx++) var_sz*=dim[idx].sz;
    sz_sng[0]='\0';
    for(idx=0;idx<nbr_dim-1;idx++){
      (void)sprintf(sng_foo,"%li*",dim[idx].sz);
      (void)strcat(sz_sng,sng_foo);
    } /* end loop over dim */
    (void)sprintf(sng_foo,"%li*nctypelen(%s)",dim[idx].sz,nc_type_nm(var_type));
    (void)strcat(sz_sng,sng_foo);
    (void)fprintf(stdout,"%s mem. size is %s = %li*%i = %li bytes\n",var_nm,sz_sng,var_sz,nctypelen(var_type),var_sz*nctypelen(var_type));
  }else{
    long var_sz=1L;

    (void)fprintf(stdout,"%s mem. size is %li*nctypelen(%s) = %li*%i = %li bytes\n",var_nm,var_sz,nc_type_nm(var_type),var_sz,nctypelen(var_type),var_sz*nctypelen(var_type));
  } /* end if variable is a scalar */ 
  (void)fflush(stdout);
  
  /* Free the space allocated for dimension info. */
  for(idx=0;idx<nbr_dim;idx++) (void)free(dim[idx].nm);
  if(nbr_dim > 0){
    (void)free(dim);
    (void)free(dim_id);
  } /* end if nbr_dim > 0*/

} /* end prn_var_def() */ 

void 
prn_var_val_lmt(int in_id,char *var_nm,lmt_sct *lmt,int nbr_lmt,char *dlm_sng,bool FORTRAN_STYLE,bool PRINT_DIMENSIONAL_UNITS)
/* 
   int in_id: input netCDF input-file ID
   char *var_nm: input variable name
   lmt_sct *lmt: input structure from lmt_evl() holding dimension limit info.
   int nbr_lmt: input number of dimensions with user-specified limits
   char *dlm_sng: input user-specified delimiter string, if any
   bool FORTRAN_STYLE: input switch to determine syntactical interpretation of dimensional indices
   bool PRINT_DIMENSIONAL_UNITS: input switch for printing units attribute, if any.
 */ 
{
  /* Routine to print variable data. This routine truncates
     dimensions of printed output variable
     in accordance with any user-specified limits.  */

  bool SRD=False;

  char *type_fmt_sng(nc_type);
  char *unit_sng="";
#define MAX_LEN_FMT_SNG 25
  char var_sng[MAX_LEN_FMT_SNG];

  extern int ncopts;

  int *dim_id;
  int idx;
  
  long *dim_map;
  long *dim_srd;
  long *dim_cnt;
  long *dim_srt;
  long *dim_ss;
  long *dim_mod;
  long *hyp_mod;
  long lmn;  
  
  dim_sct *dim;
  var_sct var;

  /* Copy name into var structure for aesthetics. Unfortunately,
     Solaris machines can overwrite var.nm with next malloc(), so
     continue to use var_nm for output just to be safe. */ 
  var.nm=(char *)strdup(var_nm);

  /* See if requested variable is in input file. */
  var.id=ncvarid(in_id,var_nm);

  /* Get number of dimensions and type for variable. */
  (void)ncvarinq(in_id,var.id,(char *)NULL,&var.type,&var.nbr_dim,(int *)NULL,(int *)NULL);

  if(var.nbr_dim > 0){
    /* Allocate space for dimension info */
    dim=(dim_sct *)malloc(var.nbr_dim*sizeof(dim_sct));
    dim_id=(int *)malloc(var.nbr_dim*sizeof(int));
    /* BUG: Solaris will SIGBUS here on three_dim_var with ncks -P -d lev,100.,101. in.nc */ 
    dim_map=(long *)malloc(var.nbr_dim*sizeof(long));
    dim_srd=(long *)malloc(var.nbr_dim*sizeof(long));
    dim_cnt=(long *)malloc(var.nbr_dim*sizeof(long));
    dim_srt=(long *)malloc(var.nbr_dim*sizeof(long));

    /* Allocate space for related arrays */ 
    dim_ss=(long *)malloc(var.nbr_dim*sizeof(long));
    dim_mod=(long *)malloc(var.nbr_dim*sizeof(long));
    hyp_mod=(long *)malloc(var.nbr_dim*sizeof(long));
  } /* end if var.nbr_dim > 0 */
  
  /* Get dimension IDs */
  (void)ncvarinq(in_id,var.id,(char *)NULL,(nc_type *)NULL,(int *)NULL,dim_id,(int *)NULL);
  
  /* Get dimension sizes and names */
  for(idx=0;idx<var.nbr_dim;idx++){
    int lmt_idx;

    dim[idx].nm=(char *)malloc(MAX_NC_NAME*sizeof(char));
    dim[idx].id=dim_id[idx];
    (void)ncdiminq(in_id,dim[idx].id,dim[idx].nm,&dim[idx].sz);
    
    /* Initialize indicial offset and stride arrays */
    dim_srt[idx]=0L;
    dim_cnt[idx]=dim[idx].sz;
    dim_srd[idx]=1L;
    dim_map[idx]=1L;

    /* Decide whether this dimension has any user-specified limits */ 
    for(lmt_idx=0;lmt_idx<nbr_lmt;lmt_idx++){
      if(lmt[lmt_idx].id == dim[idx].id){
	dim_srt[idx]=lmt[lmt_idx].srt;
	dim_cnt[idx]=lmt[lmt_idx].cnt;
	dim_srd[idx]=lmt[lmt_idx].srd;
	if(lmt[lmt_idx].srd != 1L) SRD=True;
	break;
      } /* end if */
    } /* end loop over lmt_idx */

    /* Is dimension a coordinate, i.e., stored as a variable? */ 
    dim[idx].val.vp=NULL;
    ncopts=0; 
    dim[idx].cid=ncvarid(in_id,dim[idx].nm);
    ncopts=NC_VERBOSE | NC_FATAL; 
    
    /* Read in coordinate dimensions */ 
    if(dim[idx].cid != -1){
      /* Find out what type of variable coordinate is */ 
      (void)ncvarinq(in_id,dim[idx].cid,(char *)NULL,&dim[idx].type,(int *)NULL,(int *)NULL,(int *)NULL);
      
      /* Allocate enough space to hold coordinate */ 
      dim[idx].val.vp=(void *)malloc(dim_cnt[idx]*nctypelen(dim[idx].type));
      
      /* Retrieve this coordinate */ 
      if(dim_srd[idx] == 1L) (void)ncvarget(in_id,dim[idx].cid,dim_srt+idx,dim_cnt+idx,dim[idx].val.vp); else ncvargetg(in_id,dim[idx].cid,dim_srt+idx,dim_cnt+idx,dim_srd+idx,(long *)NULL,dim[idx].val.vp);

      /* Typecast pointer to values before access */ 
      (void)cast_void_nctype(dim[idx].type,&dim[idx].val);

    } /* end if dimension is coordinate */      
    
  } /* end loop over dim */
  
  /* Find total size of variable array */ 
  var.sz=1L;
  for(idx=0;idx<var.nbr_dim;idx++) var.sz*=dim_cnt[idx];

  /* Allocate enough space to hold variable */ 
  var.val.vp=(void *)malloc(var.sz*nctypelen(var.type));
  if(var.val.vp == NULL){
    (void)fprintf(stderr,"%s: ERROR unable to malloc() %ld bytes for %s\n",prg_nm_get(),var.sz*nctypelen(var.type),var.nm);
    exit(EXIT_FAILURE);
  } /* end if */ 

  /* Get variable */
  if(var.nbr_dim==0){
    ncvarget1(in_id,var.id,0L,var.val.vp); 
  }else if(!SRD){
    ncvarget(in_id,var.id,dim_srt,dim_cnt,var.val.vp);
  }else if(SRD){
    ncvargetg(in_id,var.id,dim_srt,dim_cnt,dim_srd,(long *)NULL,var.val.vp);
  } /* end else */ 
  
  /* Typecast pointer to values before access */ 
  (void)cast_void_nctype(var.type,&var.val);

  if(PRINT_DIMENSIONAL_UNITS){
    int att_sz;
    int status;
    nc_type att_typ;

    /* Find if this variable has an attribute named "units" */ 
    ncopts=0;
    status=ncattinq(in_id,var.id,"units",&att_typ,&att_sz);
    ncopts=NC_VERBOSE | NC_FATAL; 
    if(status != -1){
      if(att_typ == NC_CHAR){
	unit_sng=(char *)malloc((att_sz+1)*nctypelen(att_typ));
	(void)ncattget(in_id,var.id,"units",unit_sng);
	unit_sng[(att_sz+1)*nctypelen(att_typ)-1]='\0';
      } /* end if */
    } /* end if */
  } /* end if */

  /* Print out what we've learned */ 
  if(dlm_sng != NULL){
    char *dlm_sng_tmp;
    char *backslash_ptr;

    dlm_sng_tmp=(char *)strdup(dlm_sng);
    backslash_ptr=strchr(dlm_sng_tmp,'\\');
    if(backslash_ptr != NULL){
      /* Replace backslash character by corresponding control code */ 
      switch(*(backslash_ptr+1)){
      case 'a':	*backslash_ptr='\a'; break;
      case 'b':	*backslash_ptr='\b'; break;
      case 'f':	*backslash_ptr='\f'; break;
      case 'n':	*backslash_ptr='\n'; break;
      case 'r':	*backslash_ptr='\r'; break;
      case 't':	*backslash_ptr='\t'; break;
      case 'v':	*backslash_ptr='\v'; break;
      case '\\': *backslash_ptr='\\'; break;
      case '\?': *backslash_ptr='\?'; break;
      case '\'': *backslash_ptr='\''; break;
      case '\"': *backslash_ptr='\"'; break;
      default: 
	(void)fprintf(stderr,"%s: WARNING %2s not supported in a delimiter string\n",prg_nm_get(),backslash_ptr);
	break;
      } /* end switch */ 
      /* Get rid of character after backslash character */ 
      (void)memmove(dlm_sng_tmp,dlm_sng_tmp,(backslash_ptr-dlm_sng_tmp)*sizeof(char));
      (void)memmove(backslash_ptr+1,backslash_ptr+2,(strlen(backslash_ptr+2)+1)*sizeof(char));
    } /* end if */ 
    /* Assign local copy of dlm_sng to dlm_sng_tmp */ 
    dlm_sng=dlm_sng_tmp;

    /* We assume the -s argument (dlm_sng) formats the entire string.
       Otherwise, one could assume the field will be printed with format type_fmt_sng(var.type),
       and the user is only allowed to affect the text in between the fields. 
       This would be accomplished with the following: */ 
    /*    (void)sprintf(var_sng,"%s%s",type_fmt_sng(var.type),dlm_sng);*/

    for(lmn=0;lmn<var.sz;lmn++){
      switch(var.type){
      case NC_FLOAT: (void)fprintf(stdout,dlm_sng,var.val.fp[lmn]); break;
      case NC_DOUBLE: (void)fprintf(stdout,dlm_sng,var.val.dp[lmn]); break;
      case NC_SHORT: (void)fprintf(stdout,dlm_sng,var.val.sp[lmn]); break;
      case NC_LONG: (void)fprintf(stdout,dlm_sng,var.val.lp[lmn]); break;
      case NC_CHAR: (void)fprintf(stdout,dlm_sng,var.val.cp[lmn]); break;
      case NC_BYTE: (void)fprintf(stdout,dlm_sng,var.val.bp[lmn]); break;
      } /* end switch */ 
    } /* end loop over element */

    /* Free local copy of input dlm_sng */
    (void)free(dlm_sng);
  } /* end if */

  if(var.nbr_dim == 0 && dlm_sng == NULL){ 
    /* Variable is a scalar, byte, or character */ 
    lmn=0;
    (void)sprintf(var_sng,"%%s = %s %%s\n",type_fmt_sng(var.type));
    switch(var.type){
    case NC_FLOAT: (void)fprintf(stdout,var_sng,var_nm,var.val.fp[lmn],unit_sng); break;
    case NC_DOUBLE: (void)fprintf(stdout,var_sng,var_nm,var.val.dp[lmn],unit_sng); break;
    case NC_SHORT: (void)fprintf(stdout,var_sng,var_nm,var.val.sp[lmn],unit_sng); break;
    case NC_LONG: (void)fprintf(stdout,var_sng,var_nm,var.val.lp[lmn],unit_sng); break;
    case NC_CHAR: (void)fprintf(stdout,var_sng,var_nm,var.val.cp[lmn],unit_sng); break;
    case NC_BYTE: (void)fprintf(stdout,var_sng,var_nm,var.val.bp[lmn],unit_sng); break;
    } /* end switch */ 
  } /* end if variable is a scalar, byte, or character */ 

  if(var.nbr_dim > 0 && dlm_sng == NULL){ 
    char arr_lft_dlm;
    char arr_rgt_dlm;
    char dim_sng[25];

    int dim_idx;
    int mod_idx;

    long hyp_srt=0L;

    short f77_idx;

    /* Variable is an array */ 
   
    /* Figure out modulo masks for each index */ 
    for(idx=0;idx<var.nbr_dim;idx++) dim_mod[idx]=1L;
    for(idx=0;idx<var.nbr_dim-1;idx++)
      for(mod_idx=idx+1;mod_idx<var.nbr_dim;mod_idx++) 
	dim_mod[idx]*=dim_cnt[mod_idx];

    /* Compute offset of hyperslab buffer from origin */ 
    if(nbr_lmt > 0){
      for(idx=0;idx<var.nbr_dim;idx++) hyp_mod[idx]=1L;
      for(idx=0;idx<var.nbr_dim-1;idx++)
	for(mod_idx=idx+1;mod_idx<var.nbr_dim;mod_idx++) 
	  hyp_mod[idx]*=dim[mod_idx].sz;

      for(idx=0;idx<var.nbr_dim;idx++) hyp_srt+=dim_srt[idx]*hyp_mod[idx];
    } /* end if */

    if(FORTRAN_STYLE){
      f77_idx=1;
      arr_lft_dlm='(';
      arr_rgt_dlm=')';
    }else{
      f77_idx=0;
      arr_lft_dlm='[';
      arr_rgt_dlm=']';
    } /* end else */
    
    if(var.type == NC_CHAR && var.nbr_dim == 1){
      /* Common sense: print fields that look like strings, as strings */ 
      (void)fprintf(stdout,"%s = %s\n",var_nm,var.val.cp);
    }else{ /* Field is not a simple string */ 
      for(lmn=0;lmn<var.sz;lmn++){
	dim_ss[var.nbr_dim-1]=lmn%dim_cnt[var.nbr_dim-1];
	for(idx=0;idx<var.nbr_dim-1;idx++){
	  dim_ss[idx]=(long)(lmn/dim_mod[idx]);
	  dim_ss[idx]%=dim_cnt[idx];
	} /* end loop over dimensions */
	
	for(idx=0;idx<var.nbr_dim;idx++) dim_ss[idx]+=dim_srt[idx];
	
	for(idx=0;idx<var.nbr_dim;idx++){
	  
	  if(FORTRAN_STYLE) dim_idx=var.nbr_dim-1-idx; else dim_idx=idx;
	  
	  /* Format and print dimension part of output string for non-coordinate variables */ 
	  if(dim[dim_idx].cid != var.id){
	    if(dim[dim_idx].cid != -1){
	      
	      (void)sprintf(dim_sng,"%%s%c%%li%c=%s ",arr_lft_dlm,arr_rgt_dlm,type_fmt_sng(dim[dim_idx].type));
	      switch(dim[dim_idx].type){
	      case NC_FLOAT: (void)fprintf(stdout,dim_sng,dim[dim_idx].nm,dim_ss[dim_idx]+f77_idx,dim[dim_idx].val.fp[dim_ss[dim_idx]-dim_srt[dim_idx]]); break;
	      case NC_DOUBLE: (void)fprintf(stdout,dim_sng,dim[dim_idx].nm,dim_ss[dim_idx]+f77_idx,dim[dim_idx].val.dp[dim_ss[dim_idx]-dim_srt[dim_idx]]); break;
	      case NC_SHORT: (void)fprintf(stdout,dim_sng,dim[dim_idx].nm,dim_ss[dim_idx]+f77_idx,dim[dim_idx].val.sp[dim_ss[dim_idx]-dim_srt[dim_idx]]); break;
	      case NC_LONG: (void)fprintf(stdout,dim_sng,dim[dim_idx].nm,dim_ss[dim_idx]+f77_idx,dim[dim_idx].val.lp[dim_ss[dim_idx]-dim_srt[dim_idx]]); break;
	      case NC_CHAR: (void)fprintf(stdout,dim_sng,dim[dim_idx].nm,dim_ss[dim_idx]+f77_idx,dim[dim_idx].val.cp[dim_ss[dim_idx]-dim_srt[dim_idx]]); break;
	      case NC_BYTE: (void)fprintf(stdout,dim_sng,dim[dim_idx].nm,dim_ss[dim_idx]+f77_idx,dim[dim_idx].val.bp[dim_ss[dim_idx]-dim_srt[dim_idx]]); break;
	      } /* end switch */ 
	    }else{ /* This dimension is not a coordinate dimension */ 
	      (void)sprintf(dim_sng,"%%s%c%%li%c ",arr_lft_dlm,arr_rgt_dlm);
	      (void)fprintf(stdout,dim_sng,dim[dim_idx].nm,dim_ss[dim_idx]+f77_idx);
	    } /* end else */
	  } /* end if */
	} /* end loop over dimensions */
	
	(void)sprintf(var_sng,"%%s%c%%li%c=%s %%s\n",arr_lft_dlm,arr_rgt_dlm,type_fmt_sng(var.type));
	switch(var.type){
	case NC_FLOAT: (void)fprintf(stdout,var_sng,var_nm,lmn+hyp_srt+f77_idx,var.val.fp[lmn],unit_sng); break;
	case NC_DOUBLE: (void)fprintf(stdout,var_sng,var_nm,lmn+hyp_srt+f77_idx,var.val.dp[lmn],unit_sng); break;
	case NC_SHORT: (void)fprintf(stdout,var_sng,var_nm,lmn+hyp_srt+f77_idx,var.val.sp[lmn],unit_sng); break;
	case NC_LONG: (void)fprintf(stdout,var_sng,var_nm,lmn+hyp_srt+f77_idx,var.val.lp[lmn],unit_sng); break;
	case NC_CHAR: (void)fprintf(stdout,var_sng,var_nm,lmn+hyp_srt+f77_idx,var.val.cp[lmn],unit_sng); break;
	case NC_BYTE: (void)fprintf(stdout,var_sng,var_nm,lmn+hyp_srt+f77_idx,var.val.bp[lmn],unit_sng); break;
	} /* end switch */ 
      } /* end loop over element */
    } /* end if variable is an array */   
  } /* end else */
  (void)fprintf(stdout,"\n");
  (void)fflush(stdout);
  
  /* Free space allocated for dimension */
  for(idx=0;idx<var.nbr_dim;idx++){
    (void)free(dim[idx].nm);
    if(dim[idx].val.vp != NULL) (void)free(dim[idx].val.vp);
  } /* end loop over dimensions */
  if(var.nbr_dim > 0){
    (void)free(dim);
    (void)free(dim_id);
    (void)free(dim_srd);
    (void)free(dim_map);
    (void)free(dim_cnt);
    (void)free(dim_srt);
    (void)free(dim_ss);
    (void)free(dim_mod);
    (void)free(hyp_mod);
  } /* end if nbr_dim > 0*/

  /* Free space allocated for variable */
  (void)free(var.val.vp);
  (void)free(var.nm);
  if(strlen(unit_sng) > 0) (void)free(unit_sng);
 
} /* end prn_var_val_lmt() */ 
