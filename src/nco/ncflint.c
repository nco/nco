/* $Header: /data/zender/nco_20150216/nco/src/nco/ncflint.c,v 1.42 2002-06-16 05:12:03 zender Exp $ */

/* ncflint -- netCDF file interpolator */

/* Purpose: Linearly interpolate a third netCDF file from two input files */

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

/* Usage:
   ncflint -O -D 2 in.nc in.nc foo.nc
   ncflint -O -i lcl_time_hr,9.0 -v lcl_time_hr /data/zender/arese/crm/951030_0800_arese_crm.nc /data/zender/arese/crm/951030_1100_arese_crm.nc foo.nc; ncks -H foo.nc
   ncflint -O -w 0.66666,0.33333 -v lcl_time_hr /data/zender/arese/crm/951030_0800_arese_crm.nc /data/zender/arese/crm/951030_1100_arese_crm.nc foo.nc; ncks -H foo.nc
   ncflint -O -w 0.66666 -v lcl_time_hr /data/zender/arese/crm/951030_0800_arese_crm.nc /data/zender/arese/crm/951030_1100_arese_crm.nc foo.nc; ncks -H foo.nc

   ncdiff -O foo.nc /data/zender/arese/crm/951030_0900_arese_crm.nc foo2.nc;ncks -H foo2.nc | m
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
#include "nco_netcdf.h" /* netCDF 3.0 wrapper functions */
/* #define MAIN_PROGRAM_FILE MUST precede #include nco.h */
#define MAIN_PROGRAM_FILE
#include "nco.h" /* NCO definitions */
#include "libnco.h" /* netCDF operator library */

int 
main(int argc,char **argv)
{
  bool CMD_LN_NTP_VAR=False; /* Option i */
  bool CMD_LN_NTP_WGT=True; /* Option w */
  bool EXCLUDE_INPUT_LIST=False; /* Option c */
  bool FILE_1_RETRIEVED_FROM_REMOTE_LOCATION;
  bool FILE_2_RETRIEVED_FROM_REMOTE_LOCATION;
  bool FORCE_APPEND=False; /* Option A */
  bool FORCE_OVERWRITE=False; /* Option O */
  bool FORTRAN_STYLE=False; /* Option F */
  bool HISTORY_APPEND=True; /* Option h */
  bool MUST_CONFORM=False; /* Must nco_var_conform_dim() find truly conforming variables? */
  bool DO_CONFORM=False; /* Did nco_var_conform_dim() find truly conforming variables? */
  bool NCAR_CSM_FORMAT;
  bool PROCESS_ALL_COORDINATES=False; /* Option c */
  bool PROCESS_ASSOCIATED_COORDINATES=True; /* Option C */
  bool REMOVE_REMOTE_FILES_AFTER_PROCESSING=True; /* Option R */
  
  char **var_lst_in=NULL_CEWI;
  char **ntp_lst_in;
  char **fl_lst_abb=NULL; /* Option a */
  char **fl_lst_in;
  char *fl_in=NULL;
  char *fl_in_1;
  char *fl_in_2;
  char *fl_pth_lcl=NULL; /* Option l */
  char *lmt_arg[NC_MAX_DIMS];
  char *opt_sng;
  char *fl_out;
  char *fl_out_tmp;
  char *fl_pth=NULL; /* Option p */
  char *time_bfr_srt;
  char *cmd_ln;
  char *ntp_nm=NULL; /* Option i */
  char CVS_Id[]="$Id: ncflint.c,v 1.42 2002-06-16 05:12:03 zender Exp $"; 
  char CVS_Revision[]="$Revision: 1.42 $";
  
  dmn_sct **dim;
  dmn_sct **dmn_out;
  
  double ntp_val_out=double_CEWI; /* Option i */
  double wgt_val_1=0.5; /* Option w */
  double wgt_val_2=0.5; /* Option w */

  extern char *optarg;
  extern int optind;
  
  int fll_md_old; /* [enm] Old fill mode */
  int idx;
  int idx_fl;
  int in_id;  
  int in_id_1;  
  int in_id_2;  
  int out_id;  
  int nbr_abb_arg=0;
  int nbr_dmn_fl;
  int lmt_nbr=0; /* Option d. NB: lmt_nbr gets incremented */
  int nbr_ntp;
  int nbr_var_fl;
  int nbr_var_fix; /* nbr_var_fix gets incremented */
  int nbr_var_prc; /* nbr_var_prc gets incremented */
  int nbr_xtr=0; /* nbr_xtr won't otherwise be set for -c with no -v */
  int nbr_dmn_xtr;
  int nbr_fl=0;
  int opt;
  int rcd=NC_NOERR; /* [rcd] Return code */
    
  lmt_sct *lmt;
  
  nm_id_sct *dmn_lst;
  nm_id_sct *xtr_lst=NULL; /* xtr_lst can get realloc()'d from NULL with -c option */
  
  time_t clock;
  
  val_unn val_gnr_unn; /* Generic container for arrival point or weight */

  var_sct *wgt_1=NULL_CEWI;
  var_sct *wgt_2=NULL_CEWI;
  var_sct *wgt_out_1=NULL;
  var_sct *wgt_out_2=NULL;
  var_sct **var;
  var_sct **var_fix;
  var_sct **var_fix_out;
  var_sct **var_out;
  var_sct **var_prc_1;
  var_sct **var_prc_2;
  var_sct **var_prc_out;
  
  /* Start the clock and save the command line */ 
  cmd_ln=nco_cmd_ln_sng(argc,argv);
  clock=time((time_t *)NULL);
  time_bfr_srt=ctime(&clock); time_bfr_srt=time_bfr_srt; /* Avoid compiler warning until variable is used for something */
  
  /* Get program name and set program enum (e.g., prg=ncra) */
  prg_nm=prg_prs(argv[0],&prg);

  /* Parse command line arguments */
  opt_sng="ACcD:d:Fhi:l:Op:rRv:xw:";
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
    case 'D': /* The debugging level. Default is 0. */
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
    case 'i':
      /* Name of variable to guide interpolation. Default is none */
      ntp_lst_in=lst_prs(optarg,",",&nbr_ntp);
      if(nbr_ntp > 2){
	(void)fprintf(stdout,"%s: ERROR too many arguments to -i\n",prg_nm_get());
	(void)nco_usg_prn();
	nco_exit(EXIT_FAILURE);
      } /* end if */
      ntp_nm=ntp_lst_in[0];
      ntp_val_out=strtod(ntp_lst_in[1],(char **)NULL);
      CMD_LN_NTP_VAR=True;
      CMD_LN_NTP_WGT=False;
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
    case 'v': /* Variables to extract/exclude */
      var_lst_in=lst_prs(optarg,",",&nbr_xtr);
      break;
    case 'w':
      /* Weight(s) for interpolation.  Default is wgt_val_1=wgt_val2=0.5 */
      ntp_lst_in=lst_prs(optarg,",",&nbr_ntp);
      if(nbr_ntp > 2){
	(void)fprintf(stdout,"%s: ERROR too many arguments to -w\n",prg_nm_get());
	(void)nco_usg_prn();
	nco_exit(EXIT_FAILURE);
      }else if(nbr_ntp == 2){
	wgt_val_1=strtod(ntp_lst_in[0],(char **)NULL);
	wgt_val_2=strtod(ntp_lst_in[1],(char **)NULL);
      }else if(nbr_ntp == 1){
	wgt_val_1=strtod(ntp_lst_in[0],(char **)NULL);
	wgt_val_2=1.0-wgt_val_1;
      } /* end else */
      CMD_LN_NTP_WGT=True;
      break;
    case 'x': /* Exclude rather than extract variables specified with -v */
      EXCLUDE_INPUT_LIST=True;
      break;
    default: /* Print proper usage */
      (void)nco_usg_prn();
      nco_exit(EXIT_FAILURE);
      break;
    } /* end switch */
  } /* end while loop */
  
  if(CMD_LN_NTP_VAR && CMD_LN_NTP_WGT){
    (void)fprintf(stdout,"%s: ERROR interpolating variable (-i) and fixed weight(s) (-w) both set\n",prg_nm_get());
    nco_exit(EXIT_FAILURE);
  }else if(!CMD_LN_NTP_VAR && !CMD_LN_NTP_WGT){
    (void)fprintf(stdout,"%s: ERROR interpolating variable (-i) or fixed weight(s) (-w) must be set\n",prg_nm_get());
    nco_exit(EXIT_FAILURE);
  } /* end else */

  /* Process positional arguments and fill in filenames */
  fl_lst_in=nco_fl_lst_mk(argv,argc,optind,&nbr_fl,&fl_out);
  
  /* Make uniform list of user-specified dimension limits */
  lmt=nco_lmt_prs(lmt_nbr,lmt_arg);
    
  /* Parse filename */
  idx_fl=0;
  fl_in=nco_fl_nm_prs(fl_in,idx_fl,&nbr_fl,fl_lst_in,nbr_abb_arg,fl_lst_abb,fl_pth);
  if(dbg_lvl > 0) (void)fprintf(stderr,"\nInput file %d is %s; ",idx_fl,fl_in);
  /* Make sure file is on local system and is readable or die trying */
  fl_in=nco_fl_mk_lcl(fl_in,fl_pth_lcl,&FILE_1_RETRIEVED_FROM_REMOTE_LOCATION);
  if(dbg_lvl > 0) (void)fprintf(stderr,"local file %s:\n",fl_in);
  rcd=nco_open(fl_in,NC_NOWRITE,&in_id);
  
  /* Get number of variables and dimensions in file */
  (void)nco_inq(in_id,&nbr_dmn_fl,&nbr_var_fl,(int *)NULL,(int *)NULL);
  
  /* Form initial extraction list from user input */
  xtr_lst=nco_var_lst_mk(in_id,nbr_var_fl,var_lst_in,PROCESS_ALL_COORDINATES,&nbr_xtr);

  /* Change included variables to excluded variables */
  if(EXCLUDE_INPUT_LIST) xtr_lst=nco_var_lst_xcl(in_id,nbr_var_fl,xtr_lst,&nbr_xtr);

  /* Add all coordinate variables to extraction list */
  if(PROCESS_ALL_COORDINATES) xtr_lst=nco_var_lst_add_crd(in_id,nbr_var_fl,nbr_dmn_fl,xtr_lst,&nbr_xtr);

  /* Make sure coordinates associated extracted variables are also on extraction list */
  if(PROCESS_ASSOCIATED_COORDINATES) xtr_lst=nco_var_lst_ass_crd_add(in_id,xtr_lst,&nbr_xtr);

  /* Sort extraction list by variable ID for fastest I/O */
  if(nbr_xtr > 1) xtr_lst=nco_lst_srt(xtr_lst,nbr_xtr,False);

  /* We now have final list of variables to extract. Phew. */
  
  /* Find coordinate/dimension values associated with user-specified limits */
  for(idx=0;idx<lmt_nbr;idx++) (void)nco_lmt_evl(in_id,lmt+idx,0L,FORTRAN_STYLE);
  
  /* Find dimensions associated with variables to be extracted */
  dmn_lst=nco_dmn_lst_ass_var(in_id,xtr_lst,nbr_xtr,&nbr_dmn_xtr);

  /* Fill in dimension structure for all extracted dimensions */
  dim=(dmn_sct **)nco_malloc(nbr_dmn_xtr*sizeof(dmn_sct *));
  for(idx=0;idx<nbr_dmn_xtr;idx++) dim[idx]=nco_dmn_fll(in_id,dmn_lst[idx].id,dmn_lst[idx].nm);
  
  /* Merge hyperslab limit information into dimension structures */
  if(lmt_nbr > 0) (void)nco_dmn_lmt_mrg(dim,nbr_dmn_xtr,lmt,lmt_nbr);

  /* Duplicate input dimension structures for output dimension structures */
  dmn_out=(dmn_sct **)nco_malloc(nbr_dmn_xtr*sizeof(dmn_sct *));
  for(idx=0;idx<nbr_dmn_xtr;idx++){
    dmn_out[idx]=nco_dmn_dpl(dim[idx]);
    (void)nco_dmn_xrf(dim[idx],dmn_out[idx]); 
  } /* end loop over idx */

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
  (void)nco_var_lst_divide(var,var_out,nbr_xtr,NCAR_CSM_FORMAT,(dmn_sct **)NULL,0,&var_fix,&var_fix_out,&nbr_var_fix,&var_prc_1,&var_prc_out,&nbr_var_prc);

  /* Open output file */
  fl_out_tmp=nco_fl_out_open(fl_out,FORCE_APPEND,FORCE_OVERWRITE,&out_id);

  /* Copy global attributes */
  (void)nco_att_cpy(in_id,out_id,NC_GLOBAL,NC_GLOBAL);
  
  /* Catenate time-stamped command line to "history" global attribute */
  if(HISTORY_APPEND) (void)nco_hst_att_cat(out_id,cmd_ln);

  /* Define dimensions in output file */
  (void)nco_dmn_dfn(fl_out,out_id,dmn_out,nbr_dmn_xtr);

  /* Define variables in output file, and copy their attributes */
  (void)nco_var_dfn(in_id,fl_out,out_id,var_out,nbr_xtr,(dmn_sct **)NULL,0);

  /* Turn off default filling behavior to enhance efficiency */
  rcd=nco_set_fill(out_id,NC_NOFILL,&fll_md_old);
  
  /* Take output file out of define mode */
  (void)nco_enddef(out_id);
  
  /* Zero start vectors for all output variables */
  (void)nco_var_srt_zero(var_out,nbr_xtr);

  /* Copy variable data for non-processed variables */
  (void)nco_var_val_cpy(in_id,out_id,var_fix,nbr_var_fix);

  in_id_1=in_id;
  idx_fl=1;
  fl_in_1=(char *)strdup(fl_in);

  /* Parse filename */
  fl_in=nco_fl_nm_prs(fl_in,idx_fl,&nbr_fl,fl_lst_in,nbr_abb_arg,fl_lst_abb,fl_pth);
  if(dbg_lvl > 0) (void)fprintf(stderr,"\nInput file %d is %s; ",idx_fl,fl_in);
  /* Make sure file is on local system and is readable or die trying */
  fl_in=nco_fl_mk_lcl(fl_in,fl_pth_lcl,&FILE_2_RETRIEVED_FROM_REMOTE_LOCATION);
  if(dbg_lvl > 0) (void)fprintf(stderr,"local file %s:\n",fl_in);
  rcd=nco_open(fl_in,NC_NOWRITE,&in_id_2);
  fl_in_2=fl_in;
  
  /* Perform various error-checks on input file */
  if(False) (void)nco_fl_cmp_err_chk();

  /* ncflint-specific stuff: */
  /* Find the weighting variable in input file */
  if(CMD_LN_NTP_VAR){
    int ntp_id_1;
    int ntp_id_2;
    
    var_sct *ntp_1;
    var_sct *ntp_2;
    var_sct *ntp_var_out;

    /* Turn arrival point into pseudo-variable */
    val_gnr_unn.d=ntp_val_out; /* Generic container for arrival point or weight */
    ntp_var_out=scl_mk_var(val_gnr_unn,NC_DOUBLE);

    rcd=nco_inq_varid(in_id_1,ntp_nm,&ntp_id_1);
    rcd=nco_inq_varid(in_id_2,ntp_nm,&ntp_id_2);

    ntp_1=nco_var_fll(in_id_1,ntp_id_1,ntp_nm,dim,nbr_dmn_xtr);
    ntp_2=nco_var_fll(in_id_2,ntp_id_2,ntp_nm,dim,nbr_dmn_xtr);
    
    /* Currently, only support scalar variables */
    if(ntp_1->sz > 1 || ntp_2->sz > 1){
      (void)fprintf(stdout,"%s: ERROR interpolation variable %s must be scalar\n",prg_nm_get(),ntp_nm);
      nco_exit(EXIT_FAILURE);
    } /* end if */

    /* Retrieve the interpolation variable */
    (void)nco_var_get(in_id_1,ntp_1);
    (void)nco_var_get(in_id_2,ntp_2);

    /* Weights must be NC_DOUBLE */
    ntp_1=nco_var_conform_type((nc_type)NC_DOUBLE,ntp_1);
    ntp_2=nco_var_conform_type((nc_type)NC_DOUBLE,ntp_2);

    /* Check for degenerate case */
    if(ntp_1->val.dp[0] == ntp_2->val.dp[0]){
      /* fxm: This is an error because weights ... */
      (void)fprintf(stdout,"%s: ERROR %s is identical (%g) in input files\n",prg_nm_get(),ntp_nm,ntp_1->val.dp[0]);
      nco_exit(EXIT_FAILURE);
    } /* end if */

    /* Turn weights into pseudo-variables */
    wgt_1=nco_var_dpl(ntp_2);
    wgt_2=nco_var_dpl(ntp_var_out);

    /* Subtract to find interpolation distances */
    (void)nco_var_subtract(ntp_1->type,ntp_1->sz,ntp_1->has_mss_val,ntp_1->mss_val,ntp_var_out->val,wgt_1->val);
    (void)nco_var_subtract(ntp_1->type,ntp_1->sz,ntp_1->has_mss_val,ntp_1->mss_val,ntp_1->val,wgt_2->val);
    (void)nco_var_subtract(ntp_1->type,ntp_1->sz,ntp_1->has_mss_val,ntp_1->mss_val,ntp_1->val,ntp_2->val);

    /* Normalize to obtain final interpolation weights */
    (void)nco_var_divide(wgt_1->type,wgt_1->sz,wgt_1->has_mss_val,wgt_1->mss_val,ntp_2->val,wgt_1->val);
    (void)nco_var_divide(wgt_2->type,wgt_2->sz,wgt_2->has_mss_val,wgt_2->mss_val,ntp_2->val,wgt_2->val);

    if(ntp_1 != NULL) ntp_1=nco_var_free(ntp_1);
    if(ntp_2 != NULL) ntp_2=nco_var_free(ntp_2);
    if(ntp_var_out != NULL) ntp_var_out=nco_var_free(ntp_var_out);
  } /* end if CMD_LN_NTP_VAR */

  if(CMD_LN_NTP_WGT){
    val_gnr_unn.d=wgt_val_1; /* Generic container for arrival point or weight */
    wgt_1=scl_mk_var(val_gnr_unn,NC_DOUBLE);
    val_gnr_unn.d=wgt_val_2; /* Generic container for arrival point or weight */
    wgt_2=scl_mk_var(val_gnr_unn,NC_DOUBLE);
  } /* end if CMD_LN_NTP_WGT */

  if(dbg_lvl > 1) (void)fprintf(stderr,"wgt_1 = %g, wgt_2 = %g\n",wgt_1->val.dp[0],wgt_2->val.dp[0]);

  /* Create structure list for second file */
  var_prc_2=(var_sct **)nco_malloc(nbr_var_prc*sizeof(var_sct *));

  /* Loop over each interpolated variable */
  for(idx=0;idx<nbr_var_prc;idx++){
    if(dbg_lvl > 0) (void)fprintf(stderr,"%s, ",var_prc_1[idx]->nm);
    if(dbg_lvl > 0) (void)fflush(stderr);

    var_prc_2[idx]=nco_var_dpl(var_prc_1[idx]);
    (void)nco_var_refresh(in_id_2,var_prc_2[idx]);

    (void)nco_var_get(in_id_1,var_prc_1[idx]);
    (void)nco_var_get(in_id_2,var_prc_2[idx]);
    
    wgt_out_1=nco_var_conform_dim(var_prc_1[idx],wgt_1,wgt_out_1,MUST_CONFORM,&DO_CONFORM);
    wgt_out_2=nco_var_conform_dim(var_prc_2[idx],wgt_2,wgt_out_2,MUST_CONFORM,&DO_CONFORM);

    var_prc_1[idx]=nco_var_conform_type((nc_type)NC_DOUBLE,var_prc_1[idx]);
    var_prc_2[idx]=nco_var_conform_type((nc_type)NC_DOUBLE,var_prc_2[idx]);

    /* Allocate and, if necesssary, initialize space for processed variable */
    var_prc_out[idx]->sz=var_prc_1[idx]->sz;
    /* NB: must not try to free() same tally buffer twice */
    /*    var_prc_out[idx]->tally=var_prc_1[idx]->tally=(long *)nco_malloc(var_prc_out[idx]->sz*sizeof(long));*/
    var_prc_out[idx]->tally=(long *)nco_malloc(var_prc_out[idx]->sz*sizeof(long));
    (void)nco_zero_long(var_prc_out[idx]->sz,var_prc_out[idx]->tally);
  
    /* Weight variable by taking product of weight with variable */
    (void)nco_var_multiply(var_prc_1[idx]->type,var_prc_1[idx]->sz,var_prc_1[idx]->has_mss_val,var_prc_1[idx]->mss_val,wgt_out_1->val,var_prc_1[idx]->val);
    (void)nco_var_multiply(var_prc_2[idx]->type,var_prc_2[idx]->sz,var_prc_2[idx]->has_mss_val,var_prc_2[idx]->mss_val,wgt_out_2->val,var_prc_2[idx]->val);
    (void)nco_var_add(var_prc_1[idx]->type,var_prc_1[idx]->sz,var_prc_1[idx]->has_mss_val,var_prc_1[idx]->mss_val,var_prc_out[idx]->tally,var_prc_1[idx]->val,var_prc_2[idx]->val);
    
    /* Recast output variable to original type */
    var_prc_2[idx]=nco_var_conform_type(var_prc_out[idx]->type,var_prc_2[idx]);

    /* Copy interpolations to output file */
    if(var_prc_out[idx]->nbr_dim == 0){
      (void)nco_put_var1(out_id,var_prc_out[idx]->id,var_prc_out[idx]->srt,var_prc_2[idx]->val.vp,var_prc_out[idx]->type);
    }else{ /* end if variable is a scalar */
      (void)nco_put_vara(out_id,var_prc_out[idx]->id,var_prc_out[idx]->srt,var_prc_out[idx]->cnt,var_prc_2[idx]->val.vp,var_prc_out[idx]->type);
    } /* end else */

    /* Free dynamically allocated buffers */
    if(var_prc_1[idx] != NULL) var_prc_1[idx]=nco_var_free(var_prc_1[idx]);
    if(var_prc_2[idx] != NULL) var_prc_2[idx]=nco_var_free(var_prc_2[idx]);
    if(var_prc_out[idx] != NULL) var_prc_out[idx]=nco_var_free(var_prc_out[idx]);
    
  } /* end loop over idx */
  if(dbg_lvl > 0) (void)fprintf(stderr,"\n");
  
  /* Close input netCDF files */
  nco_close(in_id_1);
  nco_close(in_id_2);

  /* Close output file and move it from temporary to permanent location */
  (void)nco_fl_out_cls(fl_out,fl_out_tmp,out_id);
  
  /* Remove local copy of file */
  if(FILE_1_RETRIEVED_FROM_REMOTE_LOCATION && REMOVE_REMOTE_FILES_AFTER_PROCESSING) (void)nco_fl_rm(fl_in_1);
  if(FILE_2_RETRIEVED_FROM_REMOTE_LOCATION && REMOVE_REMOTE_FILES_AFTER_PROCESSING) (void)nco_fl_rm(fl_in_2);
  
  if(rcd != NC_NOERR) nco_err_exit(rcd,"main");
  nco_exit_gracefully();
  return EXIT_SUCCESS;
} /* end main() */
