/* $Header: /data/zender/nco_20150216/nco/src/nco/ncecat.c,v 1.25 2000-09-20 18:03:51 zender Exp $ */

/* ncecat -- netCDF running averager */

/* Purpose: Join variables across files by attaching them to a new record variable */

/* Copyright (C) 1995--2000 Charlie Zender

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

/* Standard header files */
#include <math.h>               /* sin cos cos sin 3.14159 */
#include <netcdf.h>             /* netCDF definitions */
#include <stdio.h>              /* stderr, FILE, NULL, etc. */
#include <stdlib.h>             /* atof, atoi, malloc, getopt */
#include <string.h>             /* strcmp. . . */
#include <sys/stat.h>           /* stat() */
#include <time.h>               /* machine time */
#include <unistd.h>             /* POSIX stuff */
/* #include <assert.h> */           /* assert() debugging macro */
/* #include <errno.h> */            /* errno */
/* #include <malloc.h>    */        /* malloc() stuff */

/* #define MAIN_PROGRAM_FILE MUST precede #include nc.h */
#define MAIN_PROGRAM_FILE
#include "nc.h"                 /* NCO definitions */

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

  char **var_lst_in=NULL_CEWI;
  char **fl_lst_abb=NULL; /* Option a */
  char **fl_lst_in;
  char *fl_in=NULL;
  char *fl_pth_lcl=NULL; /* Option l */
  char *lmt_arg[MAX_NC_DIMS];
  char *opt_sng;
  char *fl_out;
  char *fl_out_tmp;
  char *fl_pth=NULL; /* Option p */
  char *time_bfr_srt;
  char *cmd_ln;
  char CVS_Id[]="$Id: ncecat.c,v 1.25 2000-09-20 18:03:51 zender Exp $"; 
  char CVS_Revision[]="$Revision: 1.25 $";
  
  dmn_sct *rdim;
  dmn_sct **dim;
  dmn_sct **dmn_out;
  
  extern char *optarg;
  extern int ncopts;
  extern int optind;

  int idx;
  int idx_fl;
  int in_id;  
  int out_id;  
  int nbr_abb_arg=0;
  int nbr_dmn_fl;
  int lmt_nbr=0; /* Option d. NB: lmt_nbr gets incremented */
  int nbr_var_fl;
  int nbr_var_fix; /* nbr_var_fix gets incremented */
  int nbr_var_prc; /* nbr_var_prc gets incremented */
  int nbr_xtr=0; /* nbr_xtr won't otherwise be set for -c with no -v */
  int nbr_dmn_xtr;
  int nbr_fl=0;
  int opt;
  int rec_dmn_id=-1;
  
  lmt_sct *lmt;
  
  long idx_rec_out=0L; /* idx_rec_out gets incremented */
  
  nm_id_sct *dmn_lst;
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
  time_bfr_srt=ctime(&clock); time_bfr_srt=time_bfr_srt; /* Avoid compiler warning until variable is used for something */
  
  /* Get program name and set program enum (e.g., prg=ncra) */
  prg_nm=prg_prs(argv[0],&prg);

  /* Parse command line arguments */
  opt_sng="ACcD:d:Fhl:n:Op:rRv:x";
  while((opt = getopt(argc,argv,opt_sng)) != EOF){
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
      if(nbr_abb_arg < 1 || nbr_abb_arg > 5){
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
      (void)nc_lib_vrs_prn();
      exit(EXIT_SUCCESS);
      break;
    case 'v': /* Variables to extract/exclude */
      var_lst_in=lst_prs(optarg,",",&nbr_xtr);
      break;
    case 'x': /* Exclude rather than extract variables specified with -v */
      EXCLUDE_INPUT_LIST=True;
      break;
    default: /* Print proper usage */
      (void)usg_prn();
      exit(EXIT_FAILURE);
      break;
    } /* end switch */
  } /* end while loop */
  
  /* Process positional arguments and fill in filenames */
  fl_lst_in=fl_lst_mk(argv,argc,optind,&nbr_fl,&fl_out);

  /* Make uniform list of user-specified dimension limits */
  lmt=lmt_prs(lmt_nbr,lmt_arg);
  
  /* Make netCDF errors fatal and print the diagnostic */  
  ncopts=NC_VERBOSE | NC_FATAL; 
  
  /* Parse filename */
  fl_in=fl_nm_prs(fl_in,0,&nbr_fl,fl_lst_in,nbr_abb_arg,fl_lst_abb,fl_pth);
  /* Make sure file is on local system and is readable or die trying */
  fl_in=fl_mk_lcl(fl_in,fl_pth_lcl,&FILE_RETRIEVED_FROM_REMOTE_LOCATION);
  /* Open the file for reading */
  in_id=ncopen(fl_in,NC_NOWRITE);
  
  /* Get number of variables, dimensions, and record dimension ID of input file */
  (void)ncinquire(in_id,&nbr_dmn_fl,&nbr_var_fl,(int *)NULL,&rec_dmn_id);
  
  /* Form initial extraction list from user input */
  xtr_lst=var_lst_mk(in_id,nbr_var_fl,var_lst_in,PROCESS_ALL_COORDINATES,&nbr_xtr);

  /* Change included variables to excluded variables */
  if(EXCLUDE_INPUT_LIST) xtr_lst=var_lst_xcl(in_id,nbr_var_fl,xtr_lst,&nbr_xtr);

  /* Add all coordinate variables to extraction list */
  if(PROCESS_ALL_COORDINATES) xtr_lst=var_lst_add_crd(in_id,nbr_var_fl,nbr_dmn_fl,xtr_lst,&nbr_xtr);

  /* Make sure coordinates associated extracted variables are also on extraction list */
  if(PROCESS_ASSOCIATED_COORDINATES) xtr_lst=var_lst_ass_crd_add(in_id,xtr_lst,&nbr_xtr);

  /* Remove record coordinate, if any, from extraction list */
  if(False) xtr_lst=var_lst_crd_xcl(in_id,rec_dmn_id,xtr_lst,&nbr_xtr);

  /* Finally, heapsort the extraction list by variable ID for fastest I/O */
  if(nbr_xtr > 1) xtr_lst=lst_heapsort(xtr_lst,nbr_xtr,False);
    
  /* We now have final list of variables to extract. Phew. */
  
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
  var=(var_sct **)nco_malloc(nbr_xtr*sizeof(var_sct *));
  var_out=(var_sct **)nco_malloc(nbr_xtr*sizeof(var_sct *));
  for(idx=0;idx<nbr_xtr;idx++){
    var[idx]=var_fll(in_id,xtr_lst[idx].id,xtr_lst[idx].nm,dim,nbr_dmn_xtr);
    var_out[idx]=var_dpl(var[idx]);
    (void)var_xrf(var[idx],var_out[idx]);
    (void)var_dmn_xrf(var_out[idx]);
  } /* end loop over idx */

  /* Divide variable lists into lists of fixed variables and variables to be processed */
  (void)var_lst_divide(var,var_out,nbr_xtr,NCAR_CSM_FORMAT,(dmn_sct **)NULL,0,&var_fix,&var_fix_out,&nbr_var_fix,&var_prc,&var_prc_out,&nbr_var_prc);

  /* Open output file */
  fl_out_tmp=fl_out_open(fl_out,FORCE_APPEND,FORCE_OVERWRITE,&out_id);

  /* Copy global attributes */
  (void)att_cpy(in_id,out_id,NC_GLOBAL,NC_GLOBAL);
  
  /* Catenate time-stamped command line to "history" global attribute */
  if(HISTORY_APPEND) (void)hst_att_cat(out_id,cmd_ln);

  /* ncecat-specific operations */
  if(True){
    /* Define the new record dimension */
    rdim=dmn_dpl(dim[0]);

    rdim->nm="record";
    rdim->id=-1;
    rdim->nc_id=-1;
    rdim->xrf=NULL;
    rdim->val.vp=NULL;
    rdim->is_crd_dmn=False;
    rdim->is_crd_dmn=True;
    rdim->sz=0L;
    rdim->cnt=0L;
    rdim->srt=0L;
    rdim->end=rdim->sz-1L;
    
    /* Change any existing record dimension to be a regular dimension */
    for(idx=0;idx<nbr_dmn_xtr;idx++){
      /* Is any input dimension a record dimension? */
      if(dmn_out[idx]->is_rec_dmn){
	dmn_out[idx]->is_rec_dmn=False;
	break;
      } /* end if */
    } /* end loop over idx */

    /* Add the record dimension to the end of the dimension list */
    nbr_dmn_xtr++;
    dmn_out=(dmn_sct **)nco_realloc(dmn_out,nbr_dmn_xtr*sizeof(dmn_sct **));
    dmn_out[nbr_dmn_xtr-1]=rdim;

  } /* end if */

  /* Define dimensions in output file */
  (void)dmn_dfn(fl_out,out_id,dmn_out,nbr_dmn_xtr);

  if(True){
    /* Prepend the record dimension to the beginning of all the vectors for the processed variables */
    for(idx=0;idx<nbr_var_prc;idx++){
      var_prc_out[idx]->nbr_dim++;
      var_prc_out[idx]->is_rec_var=True;
      var_prc_out[idx]->sz_rec=var_prc_out[idx]->sz;
      
      /* Allocate space to hold the dimension IDs */
      var_prc_out[idx]->dim=(dmn_sct **)nco_realloc(var_prc_out[idx]->dim,var_prc_out[idx]->nbr_dim*sizeof(dmn_sct *));
      var_prc_out[idx]->dmn_id=(int *)nco_realloc(var_prc_out[idx]->dmn_id,var_prc_out[idx]->nbr_dim*sizeof(int));
      var_prc_out[idx]->cnt=(long *)nco_realloc(var_prc_out[idx]->cnt,var_prc_out[idx]->nbr_dim*sizeof(long));
      var_prc_out[idx]->srt=(long *)nco_realloc(var_prc_out[idx]->srt,var_prc_out[idx]->nbr_dim*sizeof(long));
      var_prc_out[idx]->end=(long *)nco_realloc(var_prc_out[idx]->end,var_prc_out[idx]->nbr_dim*sizeof(long));
      
      /* Move the current array by one to make room for the new record dimension info */
      (void)memmove((void *)(var_prc_out[idx]->dim+1),(void *)(var_prc_out[idx]->dim),(var_prc_out[idx]->nbr_dim-1)*sizeof(dmn_sct *));
      (void)memmove((void *)(var_prc_out[idx]->dmn_id+1),(void *)(var_prc_out[idx]->dmn_id),(var_prc_out[idx]->nbr_dim-1)*sizeof(int));
      (void)memmove((void *)(var_prc_out[idx]->cnt+1),(void *)(var_prc_out[idx]->cnt),(var_prc_out[idx]->nbr_dim-1)*sizeof(long));
      (void)memmove((void *)(var_prc_out[idx]->srt+1),(void *)(var_prc_out[idx]->srt),(var_prc_out[idx]->nbr_dim-1)*sizeof(long));
      (void)memmove((void *)(var_prc_out[idx]->end+1),(void *)(var_prc_out[idx]->end),(var_prc_out[idx]->nbr_dim-1)*sizeof(long));
      
      /* Insert the value for the new record dimension */
      var_prc_out[idx]->dim[0]=rdim;
      var_prc_out[idx]->dmn_id[0]=rdim->id;
      var_prc_out[idx]->cnt[0]=1L;
      var_prc_out[idx]->srt[0]=-1L;
      var_prc_out[idx]->end[0]=-1L;
		    
    } /* end loop over idx */
    
  } /* end if */

  /* Define variables in output file, and copy their attributes */
  (void)var_dfn(in_id,fl_out,out_id,var_out,nbr_xtr,(dmn_sct **)NULL,0);

  /* Turn off default filling behavior to enhance efficiency */
#if ( ! defined SUN4 ) && ( ! defined SUN4SOL2 ) && ( ! defined SUNMP )
  (void)ncsetfill(out_id,NC_NOFILL);
#endif
  
  /* Take output file out of define mode */
  (void)ncendef(out_id);
  
  /* Zero start vectors for all output variables */
  (void)var_srt_zero(var_out,nbr_xtr);

  /* Copy variable data for non-processed variables */
  (void)var_val_cpy(in_id,out_id,var_fix,nbr_var_fix);

  /* Close first input netCDF file */
  (void)ncclose(in_id);
  
  /* Loop over input files */
  for(idx_fl=0;idx_fl<nbr_fl;idx_fl++){
    /* Parse filename */
    if(idx_fl != 0) fl_in=fl_nm_prs(fl_in,idx_fl,(int *)NULL,fl_lst_in,nbr_abb_arg,fl_lst_abb,fl_pth);
    if(dbg_lvl > 0) (void)fprintf(stderr,"\nInput file %d is %s; ",idx_fl,fl_in);
    /* Make sure file is on local system and is readable or die trying */
    if(idx_fl != 0) fl_in=fl_mk_lcl(fl_in,fl_pth_lcl,&FILE_RETRIEVED_FROM_REMOTE_LOCATION);
    if(dbg_lvl > 0) (void)fprintf(stderr,"local file %s:\n",fl_in);
    in_id=ncopen(fl_in,NC_NOWRITE);
    
    /* Perform various error-checks on input file */
    if(False) (void)fl_cmp_err_chk();

    /* Process all variables in current file */
    for(idx=0;idx<nbr_var_prc;idx++){
      if(dbg_lvl > 1) (void)fprintf(stderr,"%s, ",var_prc[idx]->nm);
      if(dbg_lvl > 0) (void)fflush(stderr);
      /* Variables may have different IDs and missing_values in each file */
      (void)var_refresh(in_id,var_prc[idx]);
      /* Retrieve variable from disk into memory */
      (void)var_get(in_id,var_prc[idx]);
      /* Size of record dimension is 1 in output file */
      var_prc_out[idx]->cnt[0]=1L;
      var_prc_out[idx]->srt[0]=idx_rec_out;
      /* Write variable into current record in output file */
      if(var_prc[idx]->nbr_dim == 0){
	(void)ncvarput1(out_id,var_prc_out[idx]->id,var_prc_out[idx]->srt,var_prc[idx]->val.vp);
      }else{ /* end if variable is a scalar */
	(void)ncvarput(out_id,var_prc_out[idx]->id,var_prc_out[idx]->srt,var_prc_out[idx]->cnt,var_prc[idx]->val.vp);
      } /* end if variable is array */
      /* Free current input buffer */
      (void)free(var_prc[idx]->val.vp); var_prc[idx]->val.vp=NULL;
    } /* end loop over idx */
    idx_rec_out++; /* [idx] Index of current record in output file (0 is first, ...) */
    
    if(dbg_lvl > 1) (void)fprintf(stderr,"\n");
    
    /* Close input netCDF file */
    ncclose(in_id);
    
    /* Remove local copy of file */
    if(FILE_RETRIEVED_FROM_REMOTE_LOCATION && REMOVE_REMOTE_FILES_AFTER_PROCESSING) (void)fl_rm(fl_in);

  } /* end loop over idx_fl */
  
  /* Close output file and move it from temporary to permanent location */
  (void)fl_out_cls(fl_out,fl_out_tmp,out_id);
  
  Exit_gracefully();
  return EXIT_SUCCESS;
} /* end main() */
