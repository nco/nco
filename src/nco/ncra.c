/* $Header: /data/zender/nco_20150216/nco/src/nco/ncra.c,v 1.67 2002-05-12 01:01:36 zender Exp $ */

/* ncra -- netCDF running averager */

/* Purpose: Compute averages or extract series of specified hyperslabs of 
   specfied variables of multiple input netCDF files and output them 
   to a single file. */

/* Copyright (C) 1995--2002 Charlie Zender
   This software is distributed under the terms of the GNU General Public License
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

/* Usage:
   ncra -n 3,4,1 -p /data/zender/tmp h0001.nc foo.nc
   ncra -n 3,4,1 -p /data/zender/tmp -l /data/zender/tmp/rmt h0001.nc foo.nc
   ncra -n 3,4,1 -p /ZENDER/tmp -l /data/zender/tmp h0001.nc foo.nc
   ncra -n 3,4,1 -p /ZENDER/tmp -l /usr/tmp/zender h0001.nc foo.nc

   ncea in.nc in.nc foo.nc
   ncea -n 3,4,1 -p /data/zender/tmp h0001.nc foo.nc
   ncea -n 3,4,1 -p /data/zender/tmp -l /data/zender/tmp/rmt h0001.nc foo.nc
   ncea -n 3,4,1 -p /ZENDER/tmp -l /data/zender/tmp/rmt h0001.nc foo.nc
   ncea -n 3,4,1 -p /ZENDER/tmp -l /usr/tmp/zender h0001.nc foo.nc
 */

/* Standard C headers */
#include <math.h> /* sin cos cos sin 3.14159 */
#include <stdio.h> /* stderr, FILE, NULL, etc. */
#include <stdlib.h> /* atof, atoi, malloc, getopt */
#include <string.h> /* strcmp. . . */
#include <sys/stat.h> /* stat() */
#include <time.h> /* machine time */
#include <unistd.h> /* all sorts of POSIX stuff */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions */
#ifdef _OPENMP
#include <omp.h> /* OpenMP pragmas */
#endif /* not _OPENMP */

/* Internationalization i18n */
#ifdef I18N
#include <libintl.h> /* Internationalization i18n */
#include <locale.h> /* Locale setlocale() */
#endif /* I18N */
#ifndef _LIBINTL_H
# define gettext(foo) foo
#endif /* _LIBINTL_H */

/* Personal headers */
/* #define MAIN_PROGRAM_FILE MUST precede #include nco.h */
#define MAIN_PROGRAM_FILE
#include "libnco.h" /* netCDF operator library */

int 
main(int argc,char **argv)
{
  bool EXCLUDE_INPUT_LIST=False; /* Option c */
  bool FILE_RETRIEVED_FROM_REMOTE_LOCATION;
  bool FORCE_APPEND=False; /* Option A */
  bool FORCE_OVERWRITE=False; /* Option O */
  bool FORTRAN_STYLE=False; /* Option F */
  bool HISTORY_APPEND=True; /* Option h */
  bool ARM_FORMAT=int_CEWI;
  bool NCAR_CSM_FORMAT=int_CEWI;
  bool PROCESS_ALL_COORDINATES=False; /* Option c */
  bool PROCESS_ASSOCIATED_COORDINATES=True; /* Option C */
  bool REMOVE_REMOTE_FILES_AFTER_PROCESSING=True; /* Option R */

  char **var_lst_in=NULL_CEWI;
  char **fl_lst_abb=NULL; /* Option n */
  char **fl_lst_in;
  char *fl_in=NULL;
  char *fl_pth_lcl=NULL; /* Option l */
  char *lmt_arg[NC_MAX_DIMS];
  char *opt_sng;
  char *fl_out;
  char *fl_out_tmp=NULL_CEWI;
  char *fl_pth=NULL; /* Option p */
  char *time_bfr_srt;
  char *cmd_ln;
  char CVS_Id[]="$Id: ncra.c,v 1.67 2002-05-12 01:01:36 zender Exp $"; 
  char CVS_Revision[]="$Revision: 1.67 $";
  char *nco_op_typ_sng=NULL_CEWI; /* [sng] Operation type */
  char *nco_pck_typ_sng=NULL_CEWI; /* [sng] Packing type */
  
  dmn_sct **dim;
  dmn_sct **dmn_out;
  
  extern char *optarg;
  extern int optind;

  int fll_md_old; /* [enm] Old fill mode */
  int idx=int_CEWI;
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
  int rcd=NC_NOERR; /* [rcd] Return code */
  int rec_dmn_id=NCO_REC_DMN_UNDEFINED;
  int nco_op_typ=nco_op_avg; /* [enm] Default operation is averaging */
  int nco_pck_typ=nco_pck_nil; /* [enm] Default packing is none */
  
  lmt_sct *lmt=NULL_CEWI;
  lmt_sct lmt_rec;
  
  long idx_rec; /* [idx] Index of current record in current input file */
  long idx_rec_out=0L; /* [idx] Index of current record in output file (0 is first, ...) */
  
  nco_long base_time_srt=nco_long_CEWI;
  nco_long base_time_crr=nco_long_CEWI;

  nm_id_sct *dmn_lst;
  nm_id_sct *xtr_lst=NULL; /* xtr_lst can get realloc()'d from NULL with -c option */
  
  time_t clock;
  
  var_sct **var;
  var_sct **var_fix;
  var_sct **var_fix_out;
  var_sct **var_out=NULL_CEWI;
  var_sct **var_prc;
  var_sct **var_prc_out;
  
#ifdef _LIBINTL_H
  setlocale(LC_ALL,""); /* LC_ALL sets all localization tokens to same value */
  bindtextdomain("nco","/home/zender/share/locale"); // LOCALEDIR is e.g., /usr/share/locale
  // MO files should be in $LOCALEDIR/es/LC_MESSAGES
  textdomain("nco"); // PACKAGE is name of program
#endif /* not _LIBINTL_H */

  /* Start the clock and save the command line */
  cmd_ln=cmd_ln_sng(argc,argv);
  clock=time((time_t *)NULL);
  time_bfr_srt=ctime(&clock); time_bfr_srt=time_bfr_srt; /* Avoid compiler warning until variable is used for something */

  /* Get program name and set program enum (e.g., prg=ncra) */
  prg_nm=prg_prs(argv[0],&prg);

  /* Parse command line arguments */
  opt_sng="ACcD:d:Fhl:n:Op:P:rRv:xy:";
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
    case 'n': /* NINTAP-style abbreviation of files to average */
      fl_lst_abb=lst_prs(optarg,",",&nbr_abb_arg);
      if(nbr_abb_arg < 1 || nbr_abb_arg > 5){
	(void)fprintf(stdout,gettext("%s: ERROR Incorrect abbreviation for file list\n"),prg_nm);
	(void)nco_usg_prn();
	nco_exit(EXIT_FAILURE);
      } /* end if */
      break;
    case 'O': /* Toggle FORCE_OVERWRITE */
      FORCE_OVERWRITE=!FORCE_OVERWRITE;
      break;
    case 'p': /* Common file path */
      fl_pth=optarg;
      break;
    case 'P': /* Packing type */
      nco_pck_typ_sng=(char *)strdup(optarg);
      nco_pck_typ=nco_pck_typ_get(nco_pck_typ_sng);
      break;
    case 'R': /* Toggle removal of remotely-retrieved-files. Default is True. */
      REMOVE_REMOTE_FILES_AFTER_PROCESSING=!REMOVE_REMOTE_FILES_AFTER_PROCESSING;
      break;
    case 'r': /* Print CVS program information and copyright notice */
      (void)copyright_prn(CVS_Id,CVS_Revision);
      (void)nco_lib_vrs_prn();
      nco_exit(EXIT_SUCCESS);
      break;
    case 'v': /* Variables to extract/exclude */
      var_lst_in=lst_prs(optarg,",",&nbr_xtr);
      break;
    case 'x': /* Exclude rather than extract variables specified with -v */
      EXCLUDE_INPUT_LIST=True;
      break;
    case 'y': /* Operation type */
      nco_op_typ_sng=(char *)strdup(optarg);
      if(prg == ncra || prg == ncea ) nco_op_typ=nco_op_typ_get(nco_op_typ_sng);
      break;
    default: /* Print proper usage */
      (void)nco_usg_prn();
      nco_exit(EXIT_FAILURE);
      break;
    } /* end switch */
  } /* end while loop */
  
  /* Process positional arguments and fill in filenames */
  fl_lst_in=fl_lst_mk(argv,argc,optind,&nbr_fl,&fl_out);

  /* Make uniform list of user-specified dimension limits */
  if(lmt_nbr > 0) lmt=lmt_prs(lmt_nbr,lmt_arg);
  
  /* Make netCDF errors fatal and print the diagnostic */ 
      
  /* Parse filename */
  fl_in=fl_nm_prs(fl_in,0,&nbr_fl,fl_lst_in,nbr_abb_arg,fl_lst_abb,fl_pth);
  /* Make sure file is on local system and is readable or die trying */
  fl_in=fl_mk_lcl(fl_in,fl_pth_lcl,&FILE_RETRIEVED_FROM_REMOTE_LOCATION);
  /* Open file for reading */
  rcd=nco_open(fl_in,NC_NOWRITE,&in_id);
  
  /* Get number of variables, dimensions, and record dimension ID of input file */
  (void)nco_inq(in_id,&nbr_dmn_fl,&nbr_var_fl,(int *)NULL,&rec_dmn_id);
  
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

  /* Finally, heapsort extraction list by variable ID for fastest I/O */
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

  /* Create stand-alone limit structure just for record dimension */
  if(prg == ncra || prg == ncrcat){
    if(rec_dmn_id == NCO_REC_DMN_UNDEFINED){
      (void)fprintf(stdout,gettext("%s: ERROR input file %s lacks a record dimension\n"),prg_nm_get(),fl_in);
      if(nbr_fl == 1)(void)fprintf(stdout,gettext("%s: HINT Use ncks instead of %s\n"),prg_nm_get(),prg_nm_get());
      nco_exit(EXIT_FAILURE);
    } /* endif */
    lmt_rec=lmt_sct_mk(in_id,rec_dmn_id,lmt,lmt_nbr,FORTRAN_STYLE);
  } /* endif */

  /* Is this an NCAR CSM-format history tape? */
  NCAR_CSM_FORMAT=ncar_csm_inq(in_id);

  /* Is this an ARM-format data file? */
  ARM_FORMAT=arm_inq(in_id);
  if(ARM_FORMAT) base_time_srt=arm_base_time_get(in_id);

  /* Fill in variable structure list for all extracted variables */
  var=(var_sct **)nco_malloc(nbr_xtr*sizeof(var_sct *));
  var_out=(var_sct **)nco_malloc(nbr_xtr*sizeof(var_sct *));
  for(idx=0;idx<nbr_xtr;idx++){
    var[idx]=var_fll(in_id,xtr_lst[idx].id,xtr_lst[idx].nm,dim,nbr_dmn_xtr);
    var_out[idx]=var_dpl(var[idx]);
    (void)nco_xrf_var(var[idx],var_out[idx]);
    (void)nco_xrf_dmn(var_out[idx]);
  } /* end loop over idx */

  /* Divide variable lists into lists of fixed variables and variables to be processed */
  (void)var_lst_divide(var,var_out,nbr_xtr,NCAR_CSM_FORMAT,NULL,0,&var_fix,&var_fix_out,&nbr_var_fix,&var_prc,&var_prc_out,&nbr_var_prc);

  /* Open output file */
  fl_out_tmp=fl_out_open(fl_out,FORCE_APPEND,FORCE_OVERWRITE,&out_id);

  /* Copy global attributes */
  (void)att_cpy(in_id,out_id,NC_GLOBAL,NC_GLOBAL);
  
  /* Catenate time-stamped command line to "history" global attribute */
  if(HISTORY_APPEND) (void)hst_att_cat(out_id,cmd_ln);

  /* Define dimensions in output file */
  (void)dmn_dfn(fl_out,out_id,dmn_out,nbr_dmn_xtr);

  /* Define variables in output file, and copy their attributes */
  (void)var_dfn(in_id,fl_out,out_id,var_out,nbr_xtr,NULL,0);

  /* Turn off default filling behavior to enhance efficiency */
  (void)nco_set_fill(out_id,NC_NOFILL,&fll_md_old);
  
  /* Take output file out of define mode */
  (void)nco_enddef(out_id);
  
  /* Zero start vectors for all output variables */
  (void)var_srt_zero(var_out,nbr_xtr);

  /* Copy variable data for non-processed variables */
  (void)var_val_cpy(in_id,out_id,var_fix,nbr_var_fix);

  /* Close first input netCDF file */
  (void)nco_close(in_id);
  
  /* Allocate and, if necesssary, initialize accumulation space for processed variables */
  for(idx=0;idx<nbr_var_prc;idx++){
    if(prg == ncra || prg == ncrcat){
      /* Only allocate space for one record */
      var_prc_out[idx]->sz=var_prc[idx]->sz=var_prc[idx]->sz_rec;
    } /* endif */
    if(prg == ncra || prg == ncea){
      var_prc_out[idx]->tally=var_prc[idx]->tally=(long *)nco_malloc(var_prc_out[idx]->sz*sizeof(long));
      (void)zero_long(var_prc_out[idx]->sz,var_prc_out[idx]->tally);
      var_prc_out[idx]->val.vp=(void *)nco_malloc(var_prc_out[idx]->sz*nco_typ_lng(var_prc_out[idx]->type));
      (void)var_zero(var_prc_out[idx]->type,var_prc_out[idx]->sz,var_prc_out[idx]->val);
    } /* end if */
  } /* end loop over idx */
  
  /* Print introductory thread information */
  if(dbg_lvl > 0) rcd+=nco_openmp_ini();
  
  /* Loop over input files */
  for(idx_fl=0;idx_fl<nbr_fl;idx_fl++){
    /* Parse filename */
    if(idx_fl != 0) fl_in=fl_nm_prs(fl_in,idx_fl,(int *)NULL,fl_lst_in,nbr_abb_arg,fl_lst_abb,fl_pth);
    if(dbg_lvl > 0) (void)fprintf(stderr,gettext("\nInput file %d is %s; "),idx_fl,fl_in);
    /* Make sure file is on local system and is readable or die trying */
    if(idx_fl != 0) fl_in=fl_mk_lcl(fl_in,fl_pth_lcl,&FILE_RETRIEVED_FROM_REMOTE_LOCATION);
    if(dbg_lvl > 0) (void)fprintf(stderr,gettext("local file %s:\n"),fl_in);
    rcd=nco_open(fl_in,NC_NOWRITE,&in_id);
    
    /* Variables may have different IDs and missing_values in each file */
    for(idx=0;idx<nbr_var_prc;idx++) (void)var_refresh(in_id,var_prc[idx]); /* Routine contains OpenMP critical regions */

    /* Each file can have a different number of records to process */
    if(prg == ncra || prg == ncrcat) (void)lmt_evl(in_id,&lmt_rec,idx_rec_out,FORTRAN_STYLE); /* Routine is thread-unsafe */
    
    /* Is this an ARM-format data file? */
    if(ARM_FORMAT) base_time_crr=arm_base_time_get(in_id); /* Routine is thread-unsafe */

    /* Perform various error-checks on input file */
    if(False) (void)fl_cmp_err_chk();

    if(prg == ncra || prg == ncrcat){ /* ncea jumps to else branch */
      /* Loop over each record in current file */
      if(lmt_rec.srt > lmt_rec.end) (void)fprintf(stdout,gettext("%s: WARNING %s (input file %d) is superfluous\n"),prg_nm_get(),fl_in,idx_fl);
      for(idx_rec=lmt_rec.srt;idx_rec<=lmt_rec.end;idx_rec+=lmt_rec.srd){
	/* Process all variables in current record */
	if(dbg_lvl > 1) (void)fprintf(stderr,gettext("Record %ld of %s is input record %ld\n"),idx_rec,fl_in,idx_rec_out);
#ifdef _OPENMP
#pragma omp parallel for private(idx) shared(nbr_var_prc,dbg_lvl,var_prc,idx_rec,in_id,prg,nco_op_typ,var_prc_out,idx_rec_out,rcd,ARM_FORMAT,out_id,fl_in,fl_out)
#endif /* not _OPENMP */
	for(idx=0;idx<nbr_var_prc;idx++){
	  if(dbg_lvl > 2) rcd+=nco_var_prc_crr_prn(idx,var_prc[idx]->nm);
	  if(dbg_lvl > 0) (void)fflush(stderr);
	  /* Update hyperslab start indices to current record for each variable */
	  var_prc[idx]->srt[0]=idx_rec;
	  var_prc[idx]->end[0]=idx_rec;
	  var_prc[idx]->cnt[0]=1L;
	  /* Retrieve variable from disk into memory */
	  (void)var_get(in_id,var_prc[idx]); /* Routine contains OpenMP critical regions */
	  
	  if(prg == ncra){
	    /* Convert char, short, long, int types to doubles before arithmetic */
	    var_prc[idx]=nco_typ_cnv_rth(var_prc[idx],nco_op_typ);
	    /* Output variable type is "sticky" so only convert on first record */
	    if(idx_rec_out == 0) var_prc_out[idx]=nco_typ_cnv_rth(var_prc_out[idx],nco_op_typ);
	    /* Convert var_prc to type of var_prc_out in case type of variable on disk has changed */
	    var_prc[idx]=var_conform_type(var_prc_out[idx]->type,var_prc[idx]);
	    /* Perform arithmetic operations: avg, min, max, ttl, ... */
	    nco_opr_drv(idx_rec_out,nco_op_typ,var_prc_out[idx],var_prc[idx]);
	  } /* end if ncra */
	  	  	  
	  /* Append current record to output file */
	  if(prg == ncrcat){
	    var_prc_out[idx]->srt[0]=var_prc_out[idx]->end[0]=idx_rec_out;
	    var_prc_out[idx]->cnt[0]=1L;
	    /* Replace this time_offset value with time_offset from initial file base_time */
	    if(ARM_FORMAT && !strcmp(var_prc[idx]->nm,"time_offset")) var_prc[idx]->val.dp[0]+=(base_time_crr-base_time_srt);
#ifdef _OPENMP
#pragma omp critical
#endif /* _OPENMP */
	    if(var_prc_out[idx]->sz_rec > 1) (void)nco_put_vara(out_id,var_prc_out[idx]->id,var_prc_out[idx]->srt,var_prc_out[idx]->cnt,var_prc[idx]->val.vp,var_prc_out[idx]->type); else (void)nco_put_var1(out_id,var_prc_out[idx]->id,var_prc_out[idx]->srt,var_prc[idx]->val.vp,var_prc_out[idx]->type);
	  } /* end if ncrcat */

	  /* Make sure record coordinate, if any, is monotonic */
	  if(prg == ncrcat && var_prc[idx]->is_crd_var) (void)rec_crd_chk(var_prc[idx],fl_in,fl_out,idx_rec,idx_rec_out);
	  /* Free current input buffer */
	  var_prc[idx]->val.vp=nco_free(var_prc[idx]->val.vp);
	} /* end (OpenMP parallel for) loop over variables */
	idx_rec_out++; /* [idx] Index of current record in output file (0 is first, ...) */
	if(dbg_lvl > 2) (void)fprintf(stderr,"\n");
      } /* end loop over idx_rec */
      /* Warn if fewer than number of requested records were read and final file has been processed */
      if(lmt_rec.lmt_typ == lmt_dmn_idx && lmt_rec.is_usr_spc_min && lmt_rec.is_usr_spc_max){
	long rec_nbr_rqs; /* Number of records user requested */
	rec_nbr_rqs=1L+(lmt_rec.max_idx-lmt_rec.min_idx)/lmt_rec.srd;
	if(idx_fl == nbr_fl-1 && rec_nbr_rqs != idx_rec_out) (void)fprintf(stdout,gettext("%s: WARNING User requested %li records but only %li were found\n"),prg_nm_get(),rec_nbr_rqs,idx_rec_out);
      } /* end if */
      /* Error if no records were read and final file has been processed */
      if(idx_rec_out <= 0 && idx_fl == nbr_fl-1){
	(void)fprintf(stdout,gettext("%s: ERROR No records lay within specified hyperslab\n"),prg_nm_get());
	nco_exit(EXIT_FAILURE);
      } /* end if */
      /* End of ncra, ncrcat section */
    }else{ /* ncea */
#ifdef _OPENMP
#pragma omp parallel for private(idx) shared(nbr_var_prc,dbg_lvl,var_prc,in_id,nco_op_typ,var_prc_out,idx_fl,rcd)
#endif /* not _OPENMP */
      for(idx=0;idx<nbr_var_prc;idx++){ /* Process all variables in current file */
	if(dbg_lvl > 0) rcd+=nco_var_prc_crr_prn(idx,var_prc[idx]->nm);
	if(dbg_lvl > 0) (void)fflush(stderr);
	/* Retrieve variable from disk into memory */
	(void)var_get(in_id,var_prc[idx]); /* Routine contains OpenMP critical regions */
	
	/* Convert char, short, long, int types to doubles before arithmetic */
	var_prc[idx]=nco_typ_cnv_rth(var_prc[idx],nco_op_typ);
	/* Output variable type is "sticky" so only convert on first record */
	if(idx_fl == 0) var_prc_out[idx]=nco_typ_cnv_rth(var_prc_out[idx],nco_op_typ);
	/* Convert var_prc to type of var_prc_out in case type of variable on disk has changed */
	var_prc[idx]=var_conform_type(var_prc_out[idx]->type,var_prc[idx]);
	/* Perform arithmetic operations: avg, min, max, ttl, ... */ /* Note: idx_fl not idx_rec_out! */
	nco_opr_drv(idx_fl,nco_op_typ,var_prc_out[idx],var_prc[idx]);
	
	/* Free current input buffer */
	var_prc[idx]->val.vp=nco_free(var_prc[idx]->val.vp);
      } /* end (OpenMP parallel for) loop over idx */
    } /* end else */

    if(dbg_lvl > 1) (void)fprintf(stderr,"\n");
      
    /* Close input netCDF file */
    nco_close(in_id);
    
    /* Dispose local copy of file */
    if(FILE_RETRIEVED_FROM_REMOTE_LOCATION && REMOVE_REMOTE_FILES_AFTER_PROCESSING) (void)fl_rm(fl_in);

  } /* end loop over idx_fl */
  
  /* Normalize, multiply, etc where necessary */
  if(prg == ncra || prg == ncea){
#ifdef _OPENMP
#pragma omp parallel for private(idx) shared(nbr_var_prc,nco_op_typ,var_prc_out,var_prc)
#endif /* not _OPENMP */
    for(idx=0;idx<nbr_var_prc;idx++){
      switch(nco_op_typ) {
      case nco_op_avg: /* Normalize sum by tally to create mean */
      case nco_op_sqrt: /* Normalize sum by tally to create mean */
      case nco_op_sqravg: /* Normalize sum by tally to create mean */
      case nco_op_rms: /* Normalize sum of squares by tally to create mean square */
      case nco_op_avgsqr: /* Normalize sum of squares by tally to create mean square */
	(void)var_normalize(var_prc_out[idx]->type,var_prc_out[idx]->sz,var_prc[idx]->has_mss_val,var_prc[idx]->mss_val,var_prc[idx]->tally,var_prc_out[idx]->val);
	break;
      case nco_op_rmssdn: /* Normalize sum of squares by tally-1 to create mean square for sdn */
	(void)var_normalize_sdn(var_prc_out[idx]->type,var_prc_out[idx]->sz,var_prc[idx]->has_mss_val,var_prc[idx]->mss_val,var_prc[idx]->tally,var_prc_out[idx]->val);
	break;
      case nco_op_min: /* Minimum is already in buffer, do nothing */
      case nco_op_max: /* Maximum is already in buffer, do nothing */
      case nco_op_ttl: /* Total is already in buffer, do nothing */
      default:
	break;
      } /* end switch */
      /* Some operations require additional processing */
      switch(nco_op_typ) {
      case nco_op_rms: /* Take root of mean of sum of squares to create root mean square */
      case nco_op_rmssdn: /* Take root of sdn mean of sum of squares to create root mean square for sdn */
      case nco_op_sqrt: /* Take root of mean to create root mean */
	(void)var_sqrt(var_prc_out[idx]->type,var_prc_out[idx]->sz,var_prc[idx]->has_mss_val,var_prc[idx]->mss_val,var_prc[idx]->tally,var_prc_out[idx]->val,var_prc_out[idx]->val);
	break;
      case nco_op_sqravg: /* Square mean to create square of the mean (for sdn) */
	(void)var_multiply(var_prc_out[idx]->type,var_prc_out[idx]->sz,var_prc_out[idx]->has_mss_val,var_prc_out[idx]->mss_val,var_prc_out[idx]->val,var_prc_out[idx]->val);
	break;
      default:
	break;
      } /* end switch */
      var_prc[idx]->tally=nco_free(var_prc[idx]->tally);
    } /* end (OpenMP parallel for) loop over variables */
  } /* end if */
  
  /* Manually fix YYMMDD date which was mangled by averaging */
  if(NCAR_CSM_FORMAT && prg == ncra) (void)ncar_csm_date(out_id,var_out,nbr_xtr);
  
  /* Add time variable to output file */
  if(ARM_FORMAT && prg == ncrcat) (void)arm_time_install(out_id,base_time_srt);
  
  /* Copy averages to output file and free averaging buffers */
  if(prg == ncra || prg == ncea){
    for(idx=0;idx<nbr_var_prc;idx++){
      /* Revert to original type if required */
      var_prc_out[idx]=nco_cnv_var_typ_dsk(var_prc_out[idx]);
      /* Packing/Unpacking */
      if(nco_pck_typ == nco_pck_all_new_att) var_prc_out[idx]=nco_put_var_pck(out_id,var_prc_out[idx],nco_pck_typ);
      if(var_prc_out[idx]->nbr_dim == 0){
	(void)nco_put_var1(out_id,var_prc_out[idx]->id,var_prc_out[idx]->srt,var_prc_out[idx]->val.vp,var_prc_out[idx]->type);
      }else{ /* end if variable is a scalar */
	/* Size of record dimension is 1 in output file */
	if(prg == ncra) var_prc_out[idx]->cnt[0]=1L;
	(void)nco_put_vara(out_id,var_prc_out[idx]->id,var_prc_out[idx]->srt,var_prc_out[idx]->cnt,var_prc_out[idx]->val.vp,var_prc_out[idx]->type);
      } /* end if variable is an array */
      var_prc_out[idx]->val.vp=nco_free(var_prc_out[idx]->val.vp);
    } /* end loop over idx */
  } /* end if */
  
  /* Close output file and move it from temporary to permanent location */
  (void)fl_out_cls(fl_out,fl_out_tmp,out_id);
  
  Exit_gracefully();
  return EXIT_SUCCESS;
} /* end main() */
