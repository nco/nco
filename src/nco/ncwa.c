/* $Header: /data/zender/nco_20150216/nco/src/nco/ncwa.c,v 1.91 2002-12-13 23:31:49 rorik Exp $ */

/* ncwa -- netCDF weighted averager */

/* Purpose: Compute averages of specified hyperslabs of specfied variables
   in a single input netCDF file and output them to a single file. */

/* Copyright (C) 1995--2002 Charlie Zender

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

/* fxm: 19981202 -n and -W switches were deactivated but code left in place
   while I rethink the normalization switches */

/* Usage:
   ncwa -O -a lon /home/zender/nco/data/in.nc foo.nc
   ncwa -O -R -p /ZENDER/tmp -l /home/zender/nco/data in.nc foo.nc
   ncwa -O -a lat -w gw -d lev,17 -v T -p /fs/cgd/csm/input/atm SEP1.T42.0596.nc foo.nc
   ncwa -O -C -a lat,lon,time -w gw -v PS -p /fs/cgd/csm/input/atm SEP1.T42.0596.nc foo.nc;ncks -H foo.nc
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
/* #include <assert.h> */ /* assert() debugging macro */
/* #include <errno.h> */ /* errno */
/* #include <malloc.h> */ /* malloc() stuff */
#ifdef HAVE_GETOPT_H
#include <getopt.h>  /* getopt_long */
#endif  /* HAVE_GETOPT_H

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions */

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
  bool MUST_CONFORM=False; /* [flg] Must nco_var_cnf_dmn() find truly conforming variables? */
  bool DO_CONFORM_MSK; /* Did nco_var_cnf_dmn() find truly conforming mask? */
  bool DO_CONFORM_WGT=False; /* Did nco_var_cnf_dmn() find truly conforming weight? */
  bool NCAR_CSM_FORMAT;
  bool PROCESS_ALL_COORDINATES=False; /* Option c */
  bool PROCESS_ASSOCIATED_COORDINATES=True; /* Option C */
  bool REMOVE_REMOTE_FILES_AFTER_PROCESSING=True; /* Option R */
  bool NRM_BY_DNM=True; /* Option N Normalize by denominator */
  bool MULTIPLY_BY_TALLY=False; /* Not currently implemented */
  bool NORMALIZE_BY_TALLY=True; /* Not currently implemented */
  bool NORMALIZE_BY_WEIGHT=True; /* Not currently implemented */
  bool WGT_MSK_CRD_VAR=True; /* [flg] Weight and/or mask coordinate variables */
  bool opt_a_flg=False; /* Option a */

  char **dmn_avg_lst_in=NULL_CEWI; /* Option a */
  char **var_lst_in=NULL_CEWI;
  char **fl_lst_abb=NULL; /* Option n */
  char **fl_lst_in=NULL_CEWI;
  char *fl_in=NULL;
  char *fl_pth_lcl=NULL; /* Option l */
  char *lmt_arg[NC_MAX_DIMS];
  char *opt_sng;
  char *fl_out;
  char *fl_out_tmp=NULL_CEWI;
  char *fl_pth=NULL; /* Option p */
  char *time_bfr_srt;
  char *msk_nm=NULL;
  char *nco_op_typ_sng; /* Operation type */
  char *wgt_nm=NULL;
  char *cmd_ln;
  char CVS_Id[]="$Id: ncwa.c,v 1.91 2002-12-13 23:31:49 rorik Exp $"; 
  char CVS_Revision[]="$Revision: 1.91 $";
  
  dmn_sct **dim=NULL_CEWI;
  dmn_sct **dmn_out;
  dmn_sct **dmn_avg=NULL_CEWI;
  
  double msk_val=1.0; /* Option M */

  extern char *optarg;
  extern int optind;
  
  int fll_md_old; /* [enm] Old fill mode */
  int idx=int_CEWI;
  int idx_avg;
  int idx_fl=int_CEWI;
  int in_id=int_CEWI;  
  int out_id;  
  int nbr_abb_arg=0;
  int nbr_dmn_fl;
  int nbr_dmn_avg=0;
  int lmt_nbr=0; /* Option d. NB: lmt_nbr gets incremented */
  int nbr_var_fl;
  int nbr_var_fix; /* nbr_var_fix gets incremented */
  int nbr_var_prc; /* nbr_var_prc gets incremented */
  int nbr_xtr=0; /* nbr_xtr won't otherwise be set for -c with no -v */
  int nbr_dmn_out;
  int nbr_dmn_xtr;
  int nbr_fl=0;
  int nco_op_typ=nco_op_avg; /* Operation type */
  int opt;
  int op_typ_rlt=0; /* Option o */
  int rcd=NC_NOERR; /* [rcd] Return code */
  int rec_dmn_id=-1;
  
  lmt_sct *lmt;
  
  nm_id_sct *dmn_lst;
  nm_id_sct *xtr_lst=NULL; /* xtr_lst can get realloc()'d from NULL with -c option */
  nm_id_sct *dmn_avg_lst;
  
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
  
#ifdef HAVE_GETOPT_LONG
  static struct option long_options[] =
    {
      {"average", required_argument, 0, 'a'},
      {"append",  no_argument,  0,  'A'},
      {"coords", no_argument, 0, 'c'},
      {"nocoords", no_argument, 0, 'C'},
      {"debug", required_argument, 0, 'D'},
      {"dimension", required_argument, 0, 'd'},
      {"fortran", no_argument, 0, 'F'},
      {"history", no_argument, 0, 'h'},
      {"here", no_argument, 0, 'H'},
      {"midpoint", no_argument, 0, 'I'},
      {"local", no_argument, 0, 'l'},
      {"mask-variable", no_argument, 0, 'm'},
      {"Mask-value", no_argument, 0, 'M'},
      {"nintap", required_argument, 0, 'n'},
      {"numerator", no_argument, 0, 'N'},
      {"overwrite", no_argument, 0, 'O'},
      {"path", required_argument, 0, 'p'},
      {"keep", no_argument, 0, 'R'},
      {"revision", no_argument, 0, 'r'},
      {"variable", required_argument, 0, 'v'},
      {"version", no_argument, 0, 'r'},
      {"normalize-by-tally", no_argument, 0, 'W',},
      {"exclude", no_argument, 0, 'x'},
      {"math", required_argument, 0, 'y'},
      {"help", no_argument, 0, '?'},
      {0, 0, 0, 0}
    };
  int option_index = 0;  /* getopt_long stores the option index here. */
#endif  /* HAVE_GETOPT_LONG */

  /* Start the clock and save the command line */ 
  cmd_ln=nco_cmd_ln_sng(argc,argv);
  clock=time((time_t *)NULL);
  time_bfr_srt=ctime(&clock); time_bfr_srt=time_bfr_srt; /* Avoid compiler warning until variable is used for something */

  time_bfr_srt=time_bfr_srt; /* Avert compiler warning that variable is set but never used */
  NORMALIZE_BY_TALLY=NORMALIZE_BY_TALLY; /* Avert compiler warning that variable is set but never used */
  NORMALIZE_BY_WEIGHT=NORMALIZE_BY_WEIGHT; /* Avert compiler warning that variable is set but never used */
  
  /* Get program name and set program enum (e.g., prg=ncra) */
  prg_nm=prg_prs(argv[0],&prg);

  /* Parse command line arguments */
  opt_sng="Aa:CcD:d:FhIl:M:m:nNo:Op:rRv:xWw:y:-:";
#ifdef HAVE_GETOPT_LONG
  while((opt = getopt_long(argc,argv,opt_sng,long_options,&option_index))
            != EOF) {
#else  /* DO NOT HAVE GETOPT_LONG */
  while((opt = getopt(argc,argv,opt_sng)) != EOF){
#endif /* HAVE_GETOPT_LONG */
    switch(opt){
    case 'A': /* Toggle FORCE_APPEND */
      FORCE_APPEND=!FORCE_APPEND;
      break;
    case 'a': /* Dimensions over which to average hyperslab */
      if(opt_a_flg){
	(void)fprintf(stdout,"%s: ERROR Option -a appears more than once\n",prg_nm);
	(void)fprintf(stdout,"%s: HINT Use -a dim1,dim2,... not -a dim1 -a dim2 ...\n",prg_nm);
	(void)nco_usg_prn();
	nco_exit(EXIT_FAILURE);
      } /* endif */
      dmn_avg_lst_in=lst_prs(optarg,",",&nbr_dmn_avg);
      opt_a_flg=True;
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
    case 'I': /* [flg] Weight and/or mask coordinate variables */
      WGT_MSK_CRD_VAR=!WGT_MSK_CRD_VAR;
      break;
    case 'l': /* Local path prefix for files retrieved from remote file system */
      fl_pth_lcl=optarg;
      break;
    case 'm': /* Name of variable to use as mask in reducing.  Default is none */
      msk_nm=optarg;
      break;
    case 'M': /* Good data defined by relation to mask value. Default is 1. */
      msk_val=strtod(optarg,(char **)NULL);
      break;
    case 'N':
      NRM_BY_DNM=False;
      NORMALIZE_BY_TALLY=False;
      NORMALIZE_BY_WEIGHT=False;
      break;
    case 'n':
      NORMALIZE_BY_WEIGHT=False;
      (void)fprintf(stdout,"%s: ERROR This option has been disabled while I rethink its implementation\n",prg_nm);
      nco_exit(EXIT_FAILURE);
      break;
    case 'O': /* Toggle FORCE_OVERWRITE */
      FORCE_OVERWRITE=!FORCE_OVERWRITE;
      break;
    case 'o': /* Relational operator type.  Default is 0, eq, equality */
      op_typ_rlt=nco_op_prs_rlt(optarg);
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
    case 'v': /* Variables to extract/exclude */
      var_lst_in=lst_prs(optarg,",",&nbr_xtr);
      break;
    case 'W':
      NORMALIZE_BY_TALLY=False;
      (void)fprintf(stdout,"%s: ERROR This option has been disabled while I rethink its implementation\n",prg_nm);
      nco_exit(EXIT_FAILURE);
      break;
    case 'w': /* Variable to use as weight in reducing.  Default is none */
      wgt_nm=optarg;
      break;
    case 'x': /* Exclude rather than extract variables specified with -v */
      EXCLUDE_INPUT_LIST=True;
      break;
    case 'y': /* Operation type */
      nco_op_typ_sng=(char *)strdup(optarg);
      nco_op_typ=nco_op_typ_get(nco_op_typ_sng);
      break;
    case '?': /* Print proper usage */
      (void)nco_usg_prn();
      nco_exit(EXIT_FAILURE);
      break;
    case '-': /* notify that long options are not allowed */
      (void)printf("long options are not available in this build.\n");
      (void)printf("use single-letter options instead.\n");
      nco_exit(EXIT_FAILURE);
      break;
    default: /* Print proper usage */
      (void)nco_usg_prn();
      nco_exit(EXIT_FAILURE);
      break;
    } /* end switch */
  } /* end while loop */

  /* Ensure we do not attempt to normalize by non-existent weight */
  if(wgt_nm == NULL) NORMALIZE_BY_WEIGHT=False;

  /* Process positional arguments and fill in filenames */
  fl_lst_in=nco_fl_lst_mk(argv,argc,optind,&nbr_fl,&fl_out);

  /* Make uniform list of user-specified dimension limits */
  lmt=nco_lmt_prs(lmt_nbr,lmt_arg);
    
  /* Parse filename */
  fl_in=nco_fl_nm_prs(fl_in,0,&nbr_fl,fl_lst_in,nbr_abb_arg,fl_lst_abb,fl_pth);
  /* Make sure file is on local system and is readable or die trying */
  fl_in=nco_fl_mk_lcl(fl_in,fl_pth_lcl,&FILE_RETRIEVED_FROM_REMOTE_LOCATION);
  /* Open file for reading */
  rcd=nco_open(fl_in,NC_NOWRITE,&in_id);
  
  /* Get number of variables, dimensions, and record dimension ID of input file */
  (void)nco_inq(in_id,&nbr_dmn_fl,&nbr_var_fl,(int *)NULL,&rec_dmn_id);
  
  /* Form initial extraction list from user input */
  xtr_lst=nco_var_lst_mk(in_id,nbr_var_fl,var_lst_in,PROCESS_ALL_COORDINATES,&nbr_xtr);

  /* Change included variables to excluded variables */
  if(EXCLUDE_INPUT_LIST) xtr_lst=nco_var_lst_xcl(in_id,nbr_var_fl,xtr_lst,&nbr_xtr);

  /* Add all coordinate variables to extraction list */
  if(PROCESS_ALL_COORDINATES) xtr_lst=nco_var_lst_add_crd(in_id,nbr_var_fl,nbr_dmn_fl,xtr_lst,&nbr_xtr);

  /* Make sure coordinates associated extracted variables are also on extraction list */
  if(PROCESS_ASSOCIATED_COORDINATES) xtr_lst=nco_var_lst_ass_crd_add(in_id,xtr_lst,&nbr_xtr);

  /* Remove record coordinate, if any, from extraction list */
  if(False) xtr_lst=nco_var_lst_crd_xcl(in_id,rec_dmn_id,xtr_lst,&nbr_xtr);

  /* Sort extraction list by variable ID for fastest I/O */
  if(nbr_xtr > 1) xtr_lst=nco_lst_srt(xtr_lst,nbr_xtr,False);
    
  /* Find coordinate/dimension values associated with user-specified limits */
  for(idx=0;idx<lmt_nbr;idx++) (void)nco_lmt_evl(in_id,lmt+idx,0L,FORTRAN_STYLE);
  
  /* Find dimensions associated with variables to be extracted */
  dmn_lst=nco_dmn_lst_ass_var(in_id,xtr_lst,nbr_xtr,&nbr_dmn_xtr);

  /* Fill in dimension structure for all extracted dimensions */
  dim=(dmn_sct **)nco_malloc(nbr_dmn_xtr*sizeof(dmn_sct *));
  for(idx=0;idx<nbr_dmn_xtr;idx++){
    dim[idx]=nco_dmn_fll(in_id,dmn_lst[idx].id,dmn_lst[idx].nm);
  } /* end loop over idx */
  
  /* Merge hyperslab limit information into dimension structures */
  if(lmt_nbr > 0) (void)nco_dmn_lmt_mrg(dim,nbr_dmn_xtr,lmt,lmt_nbr);

  /* Not specifying any dimensions is interpreted as specifying all dimensions */
  if (nbr_dmn_avg == 0){
    nbr_dmn_avg=nbr_dmn_xtr;
    dmn_avg_lst_in=(char **)nco_malloc(nbr_dmn_avg*sizeof(char *));
    for(idx=0;idx<nbr_dmn_avg;idx++){
      dmn_avg_lst_in[idx]=(char *)strdup(dmn_lst[idx].nm);
    } /* end loop over idx */
    (void)fprintf(stderr,"%s: INFO No dimensions specified with -a, therefore reducing (averaging, taking minimum, etc.) over all dimensions\n",prg_nm);
  } /* end if nbr_dmn_avg == 0 */

  if (nbr_dmn_avg > 0){
    /* Form list of reducing dimensions */
    dmn_avg_lst=nco_dmn_lst_mk(in_id,dmn_avg_lst_in,nbr_dmn_avg);

    if(nbr_dmn_avg > nbr_dmn_xtr){
      (void)fprintf(stdout,"%s: ERROR More reducing dimensions than extracted dimensions\n",prg_nm);
      nco_exit(EXIT_FAILURE);
    } /* end if */

    /* Form list of reducing dimensions from extracted input dimensions */
    dmn_avg=(dmn_sct **)nco_malloc(nbr_dmn_avg*sizeof(dmn_sct *));
    for(idx_avg=0;idx_avg<nbr_dmn_avg;idx_avg++){
      for(idx=0;idx<nbr_dmn_xtr;idx++){
	if(!strcmp(dmn_avg_lst[idx_avg].nm,dim[idx]->nm)) break;
      } /* end loop over idx_avg */
      if(idx != nbr_dmn_xtr){
	dmn_avg[idx_avg]=dim[idx];
      }else{
	(void)fprintf(stderr,"%s: WARNING reducing dimension \"%s\" is not contained in any variable in extraction list\n",prg_nm,dmn_avg_lst[idx_avg].nm);
	/* Collapse dimension average list by omitting irrelevent dimension */
	(void)memmove(dmn_avg_lst,dmn_avg_lst,idx_avg*sizeof(nm_id_sct));
	(void)memmove(dmn_avg_lst+idx_avg*sizeof(nm_id_sct),dmn_avg_lst+(idx_avg+1)*sizeof(nm_id_sct),(nbr_dmn_avg-idx_avg+1)*sizeof(nm_id_sct));
	--nbr_dmn_avg;
	dmn_avg_lst=(nm_id_sct *)nco_realloc(dmn_avg_lst,nbr_dmn_avg*sizeof(nm_id_sct));
      } /* end else */
    } /* end loop over idx_avg */

    /* Make sure no reducing dimension is specified more than once */
    for(idx=0;idx<nbr_dmn_avg;idx++){
      for(idx_avg=0;idx_avg<nbr_dmn_avg;idx_avg++){
	if(idx_avg != idx){
	  if(dmn_avg[idx]->id == dmn_avg[idx_avg]->id){
	    (void)fprintf(stdout,"%s: ERROR %s specified more than once in reducing list\n",prg_nm,dmn_avg[idx]->nm);
	    nco_exit(EXIT_FAILURE);
	  } /* end if */
	} /* end if */
      } /* end loop over idx_avg */
    } /* end loop over idx */

    /* Dimensions to be averaged will not appear in output file */
    dmn_out=(dmn_sct **)nco_malloc((nbr_dmn_xtr-nbr_dmn_avg)*sizeof(dmn_sct *));
    nbr_dmn_out=0;
    for(idx=0;idx<nbr_dmn_xtr;idx++){
      for(idx_avg=0;idx_avg<nbr_dmn_avg;idx_avg++){
	if(!strcmp(dmn_avg_lst[idx_avg].nm,dim[idx]->nm)) break;
      } /* end loop over idx_avg */
      if(idx_avg == nbr_dmn_avg){
	dmn_out[nbr_dmn_out]=nco_dmn_dpl(dim[idx]);
	(void)nco_dmn_xrf(dim[idx],dmn_out[nbr_dmn_out]);
	nbr_dmn_out++;
      } /* end if */
    } /* end loop over idx_avg */

    if(nbr_dmn_out != nbr_dmn_xtr-nbr_dmn_avg){
      (void)fprintf(stdout,"%s: ERROR nbr_dmn_out != nbr_dmn_xtr-nbr_dmn_avg\n",prg_nm);
      nco_exit(EXIT_FAILURE);
    } /* end if */
    
  }else{

    /* Duplicate input dimension structures for output dimension structures */
    nbr_dmn_out=nbr_dmn_xtr;
    dmn_out=(dmn_sct **)nco_malloc(nbr_dmn_out*sizeof(dmn_sct *));
    for(idx=0;idx<nbr_dmn_out;idx++){
      dmn_out[idx]=nco_dmn_dpl(dim[idx]);
      (void)nco_dmn_xrf(dim[idx],dmn_out[idx]);
    } /* end loop over idx */

  } /* end if */

  /* Is this an NCAR CSM-format history tape? */
  NCAR_CSM_FORMAT=nco_ncar_csm_inq(in_id);

  /* Fill in variable structure list for all extracted variables */
  var=(var_sct **)nco_malloc(nbr_xtr*sizeof(var_sct *));
  var_out=(var_sct **)nco_malloc(nbr_xtr*sizeof(var_sct *));
  for(idx=0;idx<nbr_xtr;idx++){
    var[idx]=nco_var_fll(in_id,xtr_lst[idx].id,xtr_lst[idx].nm,dim,nbr_dmn_xtr);
    var_out[idx]=nco_var_dpl(var[idx]);
    (void)nco_xrf_var(var[idx],var_out[idx]);
    (void)nco_xrf_dmn(var_out[idx]);
  } /* end loop over idx */

  /* Divide variable lists into lists of fixed variables and variables to be processed */
  (void)nco_var_lst_dvd(var,var_out,nbr_xtr,NCAR_CSM_FORMAT,dmn_avg,nbr_dmn_avg,&var_fix,&var_fix_out,&nbr_var_fix,&var_prc,&var_prc_out,&nbr_var_prc);

  /* We now have final list of variables to extract. Phew. */
  if(dbg_lvl > 0){
    for(idx=0;idx<nbr_xtr;idx++) (void)fprintf(stderr,"var[%d]->nm = %s, ->id=[%d]\n",idx,var[idx]->nm,var[idx]->id);
    for(idx=0;idx<nbr_var_fix;idx++) (void)fprintf(stderr,"var_fix[%d]->nm = %s, ->id=[%d]\n",idx,var_fix[idx]->nm,var_fix[idx]->id);
    for(idx=0;idx<nbr_var_prc;idx++) (void)fprintf(stderr,"var_prc[%d]->nm = %s, ->id=[%d]\n",idx,var_prc[idx]->nm,var_prc[idx]->id);
  } /* end if */
  
  /* Open output file */
  fl_out_tmp=nco_fl_out_open(fl_out,FORCE_APPEND,FORCE_OVERWRITE,&out_id);
  if(dbg_lvl > 4) (void)fprintf(stderr,"Input, output file IDs = %d, %d\n",in_id,out_id);

  /* Copy all global attributes */
  (void)nco_att_cpy(in_id,out_id,NC_GLOBAL,NC_GLOBAL,True);
  
  /* Catenate time-stamped command line to "history" global attribute */
  if(HISTORY_APPEND) (void)nco_hst_att_cat(out_id,cmd_ln);

  /* Define dimensions in output file */
  (void)nco_dmn_dfn(fl_out,out_id,dmn_out,nbr_dmn_out);

  /* Define variables in output file, copy their attributes */
  (void)nco_var_dfn(in_id,fl_out,out_id,var_out,nbr_xtr,dmn_out,nbr_dmn_out);

  /* New missing values must be added to output file while in define mode */
  if(msk_nm != NULL){
    for(idx=0;idx<nbr_var_prc;idx++){
      /* Define for var_prc_out because mss_val for var_prc will be overwritten in nco_var_refresh */
      if(!var_prc_out[idx]->has_mss_val){
	var_prc_out[idx]->has_mss_val=True;
	var_prc_out[idx]->mss_val=nco_mss_val_mk(var_prc[idx]->type);
	(void)nco_put_att(out_id,var_prc_out[idx]->id,"missing_value",var_prc_out[idx]->type,1,var_prc_out[idx]->mss_val.vp);
      } /* end if */
    } /* end for */
  } /* end if */

  /* Turn off default filling behavior to enhance efficiency */
  rcd=nco_set_fill(out_id,NC_NOFILL,&fll_md_old);
  
  /* Take output file out of define mode */
  (void)nco_enddef(out_id);
  
  /* Zero start vectors for all output variables */
  (void)nco_var_srt_zero(var_out,nbr_xtr);

  /* Copy variable data for non-processed variables */
  (void)nco_var_val_cpy(in_id,out_id,var_fix,nbr_var_fix);

  /* Close first input netCDF file */
  nco_close(in_id);
  
  /* Loop over input files (not currently used, nbr_fl == 1) */
  for(idx_fl=0;idx_fl<nbr_fl;idx_fl++){
    /* Parse filename */
    if(idx_fl != 0) fl_in=nco_fl_nm_prs(fl_in,idx_fl,&nbr_fl,fl_lst_in,nbr_abb_arg,fl_lst_abb,fl_pth);
    if(dbg_lvl > 0) (void)fprintf(stderr,"\nInput file %d is %s; ",idx_fl,fl_in);
    /* Make sure file is on local system and is readable or die trying */
    if(idx_fl != 0) fl_in=nco_fl_mk_lcl(fl_in,fl_pth_lcl,&FILE_RETRIEVED_FROM_REMOTE_LOCATION);
    if(dbg_lvl > 0) (void)fprintf(stderr,"local file %s:\n",fl_in);
    rcd=nco_open(fl_in,NC_NOWRITE,&in_id);
    
    /* Perform various error-checks on input file */
    if(False) (void)nco_fl_cmp_err_chk();

    /* Find weighting variable in input file */
    if(wgt_nm != NULL){
      int wgt_id;
      
      rcd=nco_inq_varid(in_id,wgt_nm,&wgt_id);
      /* fxm: TODO #111 core dump if wgt has dimension not in extraction list */
      wgt=nco_var_fll(in_id,wgt_id,wgt_nm,dim,nbr_dmn_xtr);
      
      /* Retrieve weighting variable */
      (void)nco_var_get(in_id,wgt); /* Routine contains OpenMP critical regions */
      /* fxm: Perhaps should allocate default tally array for wgt here
       That way, when wgt conforms to the first var_prc_out and it therefore
       does not get a tally array copied by nco_var_dpl() in nco_var_cnf_dmn(), 
       it will at least have space for a tally array. TODO #114. */

    } /* end if */

    /* Find mask variable in input file */
    if(msk_nm != NULL){
      int msk_id;
      
      rcd=nco_inq_varid(in_id,msk_nm,&msk_id);
      /* fxm: TODO #111 core dump if msk has dimension not in extraction list */
      msk=nco_var_fll(in_id,msk_id,msk_nm,dim,nbr_dmn_xtr);
      
      /* Retrieve mask variable */
      (void)nco_var_get(in_id,msk); /* Routine contains OpenMP critical regions */
    } /* end if */

    /* Set up OpenMP multi-threading environment */
    rcd+=nco_openmp_ini();

  /* 
     cd ~/nco/bld;make OPTS=D;cd -
     cd ~/nco/data;ncwa -D 1 -O in.nc foo.nc 2>&1 | m;cd -

     cd ~/nco/data
     ncks -O -v one,two,three,four in.nc omp.nc
     ncks -O -v one one.nc omp.nc

     ncks -O -v one one.nc two.nc
     ncrename -v one,two two.nc
     ncflint -O -v two -w 2.0,0.0 two.nc two.nc two.nc
     ncks -A -C -v two two.nc omp.nc
     
     ncks -O -v one one.nc three.nc
     ncrename -v one,three three.nc
     ncflint -O -v three -w 3.0,0.0 three.nc three.nc three.nc
     ncks -A -C -v three three.nc omp.nc

     ncks -O -v one one.nc four.nc
     ncrename -v one,four four.nc
     ncflint -O -v four -w 4.0,0.0 four.nc four.nc four.nc
     ncks -A -C -v four four.nc omp.nc

     ncks -H -C -v one,two,three,four omp.nc | m
     ncwa -D 1 -O omp.nc foo.nc 2>&1 | m
     ncks -H -C -v one,two,three,four foo.nc | m
  */
#ifdef _OPENMP
/* Adding a default(none) clause causes a weird error: "Error: Variable __iob used without scope declaration in a parallel region with DEFAULT(NONE) scope". This appears to be a compiler bug. */
  /* OpenMP notes:
     firstprivate(): msk_out and wgt_out must be NULL on first call to nco_var_cnf_dmn()
     shared(): msk and wgt are not altered within loop
     private(): wgt_avg does not need initialization */
#pragma omp parallel for firstprivate(msk_out,wgt_out) private(idx,DO_CONFORM_MSK,DO_CONFORM_WGT,wgt_avg) shared(nbr_var_prc,dbg_lvl,var_prc,var_prc_out,in_id,nco_op_typ,msk_nm,WGT_MSK_CRD_VAR,MUST_CONFORM,msk_val,op_typ_rlt,wgt_nm,dmn_avg,nbr_dmn_avg,NRM_BY_DNM,out_id,wgt,msk,MULTIPLY_BY_TALLY,prg_nm,rcd)
#endif /* not _OPENMP */
    for(idx=0;idx<nbr_var_prc;idx++){ /* Process all variables in current file */
      if(dbg_lvl > 0) rcd+=nco_var_prc_crr_prn(idx,var_prc[idx]->nm);
      if(dbg_lvl > 0) (void)fflush(stderr);
      
      /* Allocate and, if necesssary, initialize accumulation space for all processed variables */
      var_prc_out[idx]->sz=var_prc[idx]->sz;
      if((var_prc_out[idx]->tally=var_prc[idx]->tally=(long *)malloc(var_prc_out[idx]->sz*sizeof(long))) == NULL){
	(void)fprintf(stdout,"%s: ERROR Unable to malloc() %ld*%ld bytes for tally buffer for variable %s in main()\n",prg_nm_get(),var_prc_out[idx]->sz,(long)sizeof(long),var_prc_out[idx]->nm);
	nco_exit(EXIT_FAILURE); 
      } /* end if */
      (void)nco_zero_long(var_prc_out[idx]->sz,var_prc_out[idx]->tally);
      if((var_prc_out[idx]->val.vp=(void *)malloc(var_prc_out[idx]->sz*nco_typ_lng(var_prc_out[idx]->type))) == NULL){
	(void)fprintf(stdout,"%s: ERROR Unable to malloc() %ld*%zu bytes for value buffer for variable %s in main()\n",prg_nm_get(),var_prc_out[idx]->sz,nco_typ_lng(var_prc_out[idx]->type),var_prc_out[idx]->nm);
	nco_exit(EXIT_FAILURE); 
      } /* end if */
      (void)nco_var_zero(var_prc_out[idx]->type,var_prc_out[idx]->sz,var_prc_out[idx]->val);
      
      (void)nco_var_refresh(in_id,var_prc[idx]); /* Routine contains OpenMP critical regions */
      /* Retrieve variable from disk into memory */
      (void)nco_var_get(in_id,var_prc[idx]); /* Routine contains OpenMP critical regions */
      
      /* Convert char, short, long, int types to doubles before arithmetic */
      var_prc[idx]=nco_typ_cnv_rth(var_prc[idx],nco_op_typ);
      var_prc_out[idx]=nco_typ_cnv_rth(var_prc_out[idx],nco_op_typ);
      
      if(msk_nm != NULL && (!var_prc[idx]->is_crd_var || WGT_MSK_CRD_VAR)){
	msk_out=nco_var_cnf_dmn(var_prc[idx],msk,msk_out,MUST_CONFORM,&DO_CONFORM_MSK);
	/* If msk and var did not conform then do not mask var! */
	if(DO_CONFORM_MSK){
	  msk_out=nco_var_cnf_typ(var_prc[idx]->type,msk_out);
	  
	  /* mss_val for var_prc has been overwritten in nco_var_refresh() */
	  if(!var_prc[idx]->has_mss_val){
	    var_prc[idx]->has_mss_val=True;
	    var_prc[idx]->mss_val=nco_mss_val_mk(var_prc[idx]->type);
	  } /* end if */
	  
	  /* Mask by changing variable to missing value where condition is false */
	  (void)nco_var_mask(var_prc[idx]->type,var_prc[idx]->sz,var_prc[idx]->has_mss_val,var_prc[idx]->mss_val,msk_val,op_typ_rlt,msk_out->val,var_prc[idx]->val);
	} /* end if */
      } /* end if */
      /* Perform non-linear transformations before weighting */
      switch(nco_op_typ){
      case nco_op_avgsqr: /* Square variable before weighting */
      case nco_op_rms: /* Square variable before weighting */
      case nco_op_rmssdn: /* Square variable before weighting */
	(void)nco_var_mlt(var_prc[idx]->type,var_prc[idx]->sz,var_prc[idx]->has_mss_val,var_prc[idx]->mss_val,var_prc[idx]->val,var_prc[idx]->val);
	break;
      default: /* All other operations are linear, do nothing to them yet */
	break;
      } /* end case */
      if(wgt_nm != NULL && (!var_prc[idx]->is_crd_var || WGT_MSK_CRD_VAR)){
	/* fxm: nco_var_cnf_dmn() has bug where it does not allocate tally array
	   for weights that do already conform to var_prc. TODO #114. */
	wgt_out=nco_var_cnf_dmn(var_prc[idx],wgt,wgt_out,MUST_CONFORM,&DO_CONFORM_WGT);
	if(DO_CONFORM_WGT){
	  wgt_out=nco_var_cnf_typ(var_prc[idx]->type,wgt_out);
	  /* Weight after any initial non-linear operation so, e.g., variable is squared but not weights */
	  /* Weight variable by taking product of weight and variable */
	  (void)nco_var_mlt(var_prc[idx]->type,var_prc[idx]->sz,var_prc[idx]->has_mss_val,var_prc[idx]->mss_val,wgt_out->val,var_prc[idx]->val);
	} /* end if weights conformed */
      } /* end if weight was specified and then tested for conformance */
      /* Copy (masked) (weighted) values from var_prc to var_prc_out */
      (void)memcpy((void *)(var_prc_out[idx]->val.vp),(void *)(var_prc[idx]->val.vp),var_prc_out[idx]->sz*nco_typ_lng(var_prc_out[idx]->type));
      /* Reduce variable over specified dimensions (tally array is set here) */
      var_prc_out[idx]=nco_var_avg(var_prc_out[idx],dmn_avg,nbr_dmn_avg,nco_op_typ);
      /* var_prc_out[idx]->val now holds numerator of averaging expression documented in NCO User's Guide
	 Denominator is also tricky due to sundry normalization options
	 These logical switches are VERY tricky---be careful modifying them */
      if(NRM_BY_DNM && DO_CONFORM_WGT && (!var_prc[idx]->is_crd_var || WGT_MSK_CRD_VAR)){
	/* Duplicate wgt_out as wgt_avg so that wgt_out is not contaminated by any
	   averaging operation and may be reused on next variable.
	   Free wgt_avg after each use but continue to reuse wgt_out */
	wgt_avg=nco_var_dpl(wgt_out);
	
	if(var_prc[idx]->has_mss_val){
	  double mss_val_dbl=double_CEWI;
	  /* Set denominator to missing value at all locations where variable is missing value
	     If this is accomplished by setting weight to missing value wherever variable is missing value
	     then weight must not be reused by next variable (which might conform but have missing values in different locations)
	     This is one good reason to copy wgt_out into disposable wgt_avg for each new variable */
	  /* First make sure wgt_avg has same missing value as variable */
	  (void)nco_mss_val_cp(var_prc[idx],wgt_avg);
	  /* Copy missing value into double precision variable */
	  switch(wgt_avg->type){
	  case NC_FLOAT: mss_val_dbl=wgt_avg->mss_val.fp[0]; break; 
	  case NC_DOUBLE: mss_val_dbl=wgt_avg->mss_val.dp[0]; break; 
	  case NC_INT: mss_val_dbl=wgt_avg->mss_val.lp[0]; break;
	  case NC_SHORT: mss_val_dbl=wgt_avg->mss_val.sp[0]; break;
	  case NC_CHAR: mss_val_dbl=wgt_avg->mss_val.cp[0]; break;
	  case NC_BYTE: mss_val_dbl=wgt_avg->mss_val.bp[0]; break;
	  default: nco_dfl_case_nc_type_err(); break;
	  } /* end switch */
	  /* Second mask wgt_avg where variable is missing value */
	  (void)nco_var_mask(wgt_avg->type,wgt_avg->sz,var_prc[idx]->has_mss_val,var_prc[idx]->mss_val,mss_val_dbl,nco_op_ne,var_prc[idx]->val,wgt_avg->val);
	} /* endif weight must be checked for missing values */
	
	if(msk_nm != NULL && DO_CONFORM_MSK){
	  /* Must mask weight in same fashion as variable was masked */
	  /* If msk and var did not conform then do not mask wgt */
	  /* Ensure wgt_avg has a missing value */
	  if(!wgt_avg->has_mss_val){
	    wgt_avg->has_mss_val=True;
	    wgt_avg->mss_val=nco_mss_val_mk(wgt_avg->type);
	  } /* end if */
	  /* Mask by changing weight to missing value where condition is false */
	  (void)nco_var_mask(wgt_avg->type,wgt_avg->sz,wgt_avg->has_mss_val,wgt_avg->mss_val,msk_val,op_typ_rlt,msk_out->val,wgt_avg->val);
	} /* endif weight must be masked */
	
	/* fxm: temporary kludge to make sure weight has tally space
	   wgt_avg may occasionally lack a valid tally array in ncwa because
	   it is created, sometimes, before the tally array for var_prc_out[idx] is 
	   created, and thus the nco_var_dpl() call in nco_var_cnf_dmn() does not copy
	   a tally array into wgt_avg. See related note about this above. TODO #114.*/
	if((wgt_avg->tally=(long *)nco_realloc(wgt_avg->tally,wgt_avg->sz*sizeof(long))) == NULL){
	  (void)fprintf(stdout,"%s: ERROR Unable to realloc() %ld*%ld bytes for tally buffer for weight %s in main()\n",prg_nm_get(),wgt_avg->sz,(long)sizeof(long),wgt_avg->nm);
	  nco_exit(EXIT_FAILURE); 
	} /* end if */
	/* Average weight over specified dimensions (tally array is set here) */
	wgt_avg=nco_var_avg(wgt_avg,dmn_avg,nbr_dmn_avg,nco_op_avg);
	if(MULTIPLY_BY_TALLY){
	  /* Currently this is not implemented */
	  /* Multiply numerator (weighted sum of variable) by tally 
	     We deviously accomplish this by dividing denominator by tally */
	  (void)nco_var_nrm(wgt_avg->type,wgt_avg->sz,wgt_avg->has_mss_val,wgt_avg->mss_val,wgt_avg->tally,wgt_avg->val);
	} /* endif */
	/* Divide numerator by denominator */
	/* Diagnose common PEBCAK before it causes core dump */
	if(var_prc_out[idx]->sz == 1L && var_prc_out[idx]->type == NC_INT && var_prc_out[idx]->val.lp[0] == 0){
	  (void)fprintf(stdout,"%s: ERROR Weight in denominator weight = 0.0, will cause SIGFPE\n%s: HINT Sum of masked, averaged weights must be non-zero\n%s: HINT A possible workaround is to remove variable \"%s\" from output file using \"%s -x -v %s ...\"\n%s: Expecting core dump...now!\n",prg_nm,prg_nm,prg_nm,var_prc_out[idx]->nm,prg_nm,var_prc_out[idx]->nm,prg_nm);
	} /* end if */
	/* Divide numerator by masked, averaged, weights */
	switch(nco_op_typ){
	case nco_op_avg: /* Normalize sum by weighted tally to create mean */
	case nco_op_sqravg: /* Normalize sum by weighted tally to create mean */
	case nco_op_avgsqr: /* Normalize sum of squares by weighted tally to create mean square */
	case nco_op_rms: /* Normalize sum of squares by weighted tally to create mean square */
	case nco_op_sqrt: /* Normalize sum by weighted tally to create mean */
	case nco_op_rmssdn: /* Normalize sum of squares by weighted tally-1 to create mean square for sdn */
	  (void)nco_var_dvd(var_prc_out[idx]->type,var_prc_out[idx]->sz,var_prc_out[idx]->has_mss_val,var_prc_out[idx]->mss_val,wgt_avg->val,var_prc_out[idx]->val);
	  break;
	case nco_op_min: /* Minimum is already in buffer, do nothing */
	case nco_op_max: /* Maximum is already in buffer, do nothing */	
	case nco_op_ttl: /* Total is already in buffer, do nothing */	
	  break;
	default:
	  (void)fprintf(stdout,"%s: ERROR Illegal nco_op_typ in weighted normalization\n",prg_nm);
	  break;
	} /* end switch */
	/* Free wgt_avg, but keep wgt_out, after each use */
	if(wgt_avg != NULL) wgt_avg=nco_var_free(wgt_avg);
	/* End of branch for normalization when weights were specified */
      }else if(NRM_BY_DNM){
	/* Branch for normalization when no weights were specified
	   Normalization is just due to tally */
	switch(nco_op_typ){
	case nco_op_avg: /* Normalize sum by tally to create mean */
	case nco_op_sqravg: /* Normalize sum by tally to create mean */
	case nco_op_avgsqr: /* Normalize sum of squares by tally to create mean square */
	case nco_op_rms: /* Normalize sum of squares by tally to create mean square */
	case nco_op_sqrt: /* Normalize sum by tally to create mean */
	  (void)nco_var_nrm(var_prc_out[idx]->type,var_prc_out[idx]->sz,var_prc_out[idx]->has_mss_val,var_prc_out[idx]->mss_val,var_prc_out[idx]->tally,var_prc_out[idx]->val);
	  break;
	case nco_op_rmssdn: /* Normalize sum of squares by tally-1 to create mean square for sdn */
	  (void)nco_var_nrm_sdn(var_prc_out[idx]->type,var_prc_out[idx]->sz,var_prc_out[idx]->has_mss_val,var_prc_out[idx]->mss_val,var_prc_out[idx]->tally,var_prc_out[idx]->val);
	  break;
	case nco_op_min: /* Minimum is already in buffer, do nothing */
	case nco_op_max: /* Maximum is already in buffer, do nothing */	
	case nco_op_ttl: /* Total is already in buffer, do nothing */	
	default:
	  break;
	} /* end switch */
      }else if(!NRM_BY_DNM){
	/* Normalization has been turned off by user, we are done */
	;
      }else{
	(void)fprintf(stdout,"%s: ERROR Unforeseen logical branch in main()\n",prg_nm);
	nco_exit(EXIT_FAILURE);
      } /* end if */
      /* Some non-linear operations require additional processing */
      switch(nco_op_typ){
      case nco_op_sqravg: /* Square mean to create square of the mean (for sdn) */
	(void)nco_var_mlt(var_prc_out[idx]->type,var_prc_out[idx]->sz,var_prc_out[idx]->has_mss_val,var_prc_out[idx]->mss_val,var_prc_out[idx]->val,var_prc_out[idx]->val);
	break;
      case nco_op_sqrt: /* Take root of mean to create root mean */
      case nco_op_rms: /* Take root of mean of sum of squares to create root mean square */
      case nco_op_rmssdn: /* Take root of sdn mean of sum of squares to create root mean square for sdn */
	(void)nco_var_sqrt(var_prc_out[idx]->type,var_prc_out[idx]->sz,var_prc_out[idx]->has_mss_val,var_prc_out[idx]->mss_val,var_prc_out[idx]->tally,var_prc_out[idx]->val,var_prc_out[idx]->val);  
	break;
      default:
	break;
      } /* end switch */
      /* Free tally buffer */
      var_prc_out[idx]->tally=(long *)nco_free(var_prc_out[idx]->tally);
      
      /* Revert any arithmetic promotion but leave unpacking (for now) */
      var_prc_out[idx]=nco_var_cnf_typ(var_prc_out[idx]->typ_upk,var_prc_out[idx]);
      
      /* Free current input buffer */
      var_prc[idx]->val.vp=nco_free(var_prc[idx]->val.vp);

#ifdef _OPENMP
#pragma omp critical
#endif /* _OPENMP */
      { /* begin OpenMP critical */
	/* Copy average to output file and free averaging buffer */
	if(var_prc_out[idx]->nbr_dim == 0){
	  (void)nco_put_var1(out_id,var_prc_out[idx]->id,var_prc_out[idx]->srt,var_prc_out[idx]->val.vp,var_prc_out[idx]->type);
	}else{ /* end if variable is scalar */
	  (void)nco_put_vara(out_id,var_prc_out[idx]->id,var_prc_out[idx]->srt,var_prc_out[idx]->cnt,var_prc_out[idx]->val.vp,var_prc_out[idx]->type);
	} /* end if variable is array */
      } /* end OpenMP critical */
      var_prc_out[idx]->val.vp=nco_free(var_prc_out[idx]->val.vp);
      
    } /* end (OpenMP parallel for) loop over idx */
    
    /* Free weights and masks */
    if(wgt != NULL) wgt=nco_var_free(wgt);
    if(wgt_out != NULL) wgt_out=nco_var_free(wgt_out);
    if(wgt_avg != NULL) wgt_avg=nco_var_free(wgt_avg);
    if(msk != NULL) msk=nco_var_free(msk);
    if(msk_out != NULL) msk_out=nco_var_free(msk_out);
    
    if(dbg_lvl > 0) (void)fprintf(stderr,"\n");
    
    /* Close input netCDF file */
    nco_close(in_id);
    
    /* Remove local copy of file */
    if(FILE_RETRIEVED_FROM_REMOTE_LOCATION && REMOVE_REMOTE_FILES_AFTER_PROCESSING) (void)nco_fl_rm(fl_in);
    
  } /* end loop over idx_fl */
  
  /* Close output file and move it from temporary to permanent location */
  (void)nco_fl_out_cls(fl_out,fl_out_tmp,out_id);
  
  nco_exit_gracefully();
  return EXIT_SUCCESS;
} /* end main() */
