/* $Header: /data/zender/nco_20150216/nco/src/nco/ncdiff.c,v 1.66 2003-04-07 02:46:39 zender Exp $ */

/* ncdiff -- netCDF differencer */

/* Purpose: Compute differences of specified hyperslabs of specfied variables
   from two input netCDF files and output them to a single file. */

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
   ncdiff -O in.nc in.nc foo.nc
   ncdiff -O -v mss_val in.nc in.nc foo.nc
   ncdiff -p /data/zender/tmp h0001.nc foo.nc
   ncdiff -p /data/zender/tmp -l /data/zender/tmp/rmt h0001.nc h0002.nc foo.nc
   ncdiff -p /ZENDER/tmp -l /data/zender/tmp/rmt h0001.nc h0002.nc foo.nc
   ncdiff -p /ZENDER/tmp -l /usr/tmp/zender h0001.nc h0002.nc foo.nc

   Test type conversion:
   ncks -O -C -v float_var in.nc foo1.nc
   ncrename -v float_var,double_var foo1.nc
   ncks -O -C -v double_var in.nc foo2.nc
   ncdiff -O -C -v double_var foo1.nc foo2.nc foo3.nc
   ncdiff -O -C -v double_var foo2.nc foo1.nc foo4.nc
   ncks -H -m foo1.nc
   ncks -H -m foo2.nc
   ncks -H -m foo3.nc
   ncks -H -m foo4.nc

   Test nco_var_cnf_dmn:
   ncks -O -v scalar_var in.nc foo.nc ; ncrename -v scalar_var,four_dmn_rec_var foo.nc ; ncdiff -O -v four_dmn_rec_var in.nc foo.nc foo2.nc
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
#endif /* !HAVE_GETOPT_H */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions */

/* Personal headers */
/* #define MAIN_PROGRAM_FILE MUST precede #include nco.h */
#define MAIN_PROGRAM_FILE
#include "libnco.h" /* netCDF operator library */

int 
main(int argc,char **argv)
{
  bool EXCLUDE_INPUT_LIST=False; /* Option c */
  bool FILE_1_RETRIEVED_FROM_REMOTE_LOCATION;
  bool FILE_2_RETRIEVED_FROM_REMOTE_LOCATION;
  bool FORCE_APPEND=False; /* Option A */
  bool FORCE_OVERWRITE=False; /* Option O */
  bool FORTRAN_STYLE=False; /* Option F */
  bool HISTORY_APPEND=True; /* Option h */
  bool MUST_CONFORM=True; /* Must nco_var_cnf_dmn() find truly conforming variables? */
  bool DO_CONFORM; /* Did nco_var_cnf_dmn() find truly conforming variables? */
  bool NCAR_CSM_FORMAT;
  bool PROCESS_ALL_COORDINATES=False; /* Option c */
  bool PROCESS_ASSOCIATED_COORDINATES=True; /* Option C */
  bool REMOVE_REMOTE_FILES_AFTER_PROCESSING=True; /* Option R */
  
  char **var_lst_in=NULL_CEWI;
  char **fl_lst_abb=NULL; /* Option a */
  char **fl_lst_in;
  char *fl_in=NULL; /* fl_in is nco_realloc'd when not NULL */
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
  char CVS_Id[]="$Id: ncdiff.c,v 1.66 2003-04-07 02:46:39 zender Exp $"; 
  char CVS_Revision[]="$Revision: 1.66 $";
  
  dmn_sct **dim;
  dmn_sct **dmn_out;
  
  extern char *optarg;
  extern int optind;
  
  int fll_md_old; /* [enm] Old fill mode */
  int has_mss_val=False;
  int idx;
  int idx_dim;
  int idx_fl;
  int in_id;  
  int in_id_1;  
  int in_id_2;  
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
    
  lmt_sct *lmt;
  
  long *lp;

  nm_id_sct *dmn_lst;
  nm_id_sct *xtr_lst=NULL; /* xtr_lst can get realloc()'d from NULL with -c option */
  
  ptr_unn mss_val;

  time_t clock;
  
  var_sct **var;
  var_sct **var_fix;
  var_sct **var_fix_out;
  var_sct **var_out;
  var_sct **var_prc;
  var_sct **var_prc_out;
  
 static struct option opt_lng[]=
    { /* Structure ordered by short option key if possible */
      {"append",no_argument,0,'A'},
      {"coords",no_argument,0,'c'},
      {"crd",no_argument,0,'c'},
      {"nocoords",no_argument,0,'C'},
      {"debug",required_argument,0,'D'},
      {"dbg_lvl",required_argument,0,'D'},
      {"dimension",required_argument,0,'d'},
      {"dmn",required_argument,0,'d'},
      {"fortran",no_argument,0,'F'},
      {"ftn",no_argument,0,'F'},
      {"history",no_argument,0,'h'},
      {"hst",no_argument,0,'h'},
      {"local",required_argument,0,'l'},
      {"lcl",required_argument,0,'l'},
      {"overwrite",no_argument,0,'O'},
      {"ovr",no_argument,0,'O'},
      {"path",required_argument,0,'p'},
      {"retain",no_argument,0,'R'},
      {"rtn",no_argument,0,'R'},
      {"revision",no_argument,0,'r'},
      {"variable",required_argument,0,'v'},
      {"version",no_argument,0,'r'},
      {"vrs",no_argument,0,'r'},
      {"exclude",no_argument,0,'x'},
      {"xcl",no_argument,0,'x'},
      {"help",no_argument,0,'?'},
      {0,0,0,0}
    }; /* end opt_lng */
  int opt_idx=0; /* Index of current long option into opt_lng array */

  /* Start clock and save command line */ 
  cmd_ln=nco_cmd_ln_sng(argc,argv);
  clock=time((time_t *)NULL);
  time_bfr_srt=ctime(&clock); time_bfr_srt=time_bfr_srt; /* Avoid compiler warning until variable is used for something */
  
  /* Get program name and set program enum (e.g., prg=ncra) */
  prg_nm=prg_prs(argv[0],&prg);

  /* Parse command line arguments */
  opt_sng="ACcD:d:Fhl:Op:rRv:x-:";
  while((opt = getopt_long(argc,argv,opt_sng,opt_lng,&opt_idx)) != EOF){
    switch(opt){
    case 'A': /* Toggle FORCE_APPEND */
      FORCE_APPEND=!FORCE_APPEND;
      break;
    case 'C': /* Extract all coordinates associated with extracted variables? */
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
    case 'x': /* Exclude rather than extract variables specified with -v */
      EXCLUDE_INPUT_LIST=True;
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
      break;
    } /* end switch */
  } /* end while loop */
  
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

  if(dbg_lvl > 3){
    for(idx=0;idx<nbr_xtr;idx++) (void)fprintf(stderr,"xtr_lst[%d].nm = %s, .id= %d\n",idx,xtr_lst[idx].nm,xtr_lst[idx].id);
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
  (void)nco_var_lst_dvd(var,var_out,nbr_xtr,NCAR_CSM_FORMAT,(dmn_sct **)NULL,0,&var_fix,&var_fix_out,&nbr_var_fix,&var_prc,&var_prc_out,&nbr_var_prc);

  /* Open output file */
  fl_out_tmp=nco_fl_out_open(fl_out,FORCE_APPEND,FORCE_OVERWRITE,&out_id);

  /* Copy global attributes */
  (void)nco_att_cpy(in_id,out_id,NC_GLOBAL,NC_GLOBAL,True);
  
  /* Catenate time-stamped command line to "history" global attribute */
  if(HISTORY_APPEND) (void)nco_hst_att_cat(out_id,cmd_ln);

  /* Define dimensions in output file */
  (void)nco_dmn_dfn(fl_out,out_id,dmn_out,nbr_dmn_xtr);

  /* Define variables in output file, copy their attributes */
  (void)nco_var_dfn(in_id,fl_out,out_id,var_out,nbr_xtr,(dmn_sct **)NULL,(int)0);

  /* Turn off default filling behavior to enhance efficiency */
  rcd=nco_set_fill(out_id,NC_NOFILL,&fll_md_old);
  
  /* Take output file out of define mode */
  (void)nco_enddef(out_id);
  
  /* Zero start vectors for all output variables */
  (void)nco_var_srt_zero(var_out,nbr_xtr);

  /* Copy variable data for non-processed variables */
  (void)nco_var_val_cpy(in_id,out_id,var_fix,nbr_var_fix);

  /* The code for ncdiff() has been similar to ncea() (and ncra()) wherever possible
     The major differences occur where performance would otherwise suffer
     From now on, however, the binary-file and binary-operation nature of ncdiff()
     is too different from ncea() paradigm to justify following ncea() style.
     Instead, symmetric nomenclature of file_1, file_2 is adopted, and 
     differences are performed variable by variable so peak memory usage goes as
     Order(2*maximum variable size) rather than Order(3*maximum record size) or
     Order(3*file size) 
     
     The other unique "feature" to know about ncdiff is that it tries too hard to save memory
     For example, ncdiff is a three file operator (input, input, output) but manages to get by 
     with having only two lists of variable structures at any given time.
     As a result, the logic it employs is fairly convoluted
     ncdiff overwrites variable structure from file_2 with the structure for file_3 (output file)
     but, at the same time, it writes value array for file_3 into value part of file_1
     variable structures. 
  */

  in_id_1=in_id;
  fl_in_1=(char *)strdup(fl_in);
  idx_fl=1;

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

  /* Loop over each differenced variable */
  for(idx=0;idx<nbr_var_prc;idx++){
    if(dbg_lvl > 0) (void)fprintf(stderr,"%s, ",var_prc[idx]->nm);
    if(dbg_lvl > 0) (void)fflush(stderr);

    /* Initialize mss_val flag */
    has_mss_val=False;

    (void)nco_var_refresh(in_id_1,var_prc[idx]);
    has_mss_val=has_mss_val || var_prc[idx]->has_mss_val; 
    (void)nco_var_get(in_id_1,var_prc[idx]);
    
    /* Save output variable ID from being overwritten in refresh call */
    var_prc[idx]->id=var_prc_out[idx]->id;
    (void)nco_var_refresh(in_id_2,var_prc_out[idx]);

    /* Assume var1 and var2 conform if they are same rank */
    if(var_prc_out[idx]->nbr_dim == var_prc[idx]->nbr_dim){
      nc_type var_type;
      int var_id;
      /* Routine nco_var_refresh() does not set type for var_prc_out, do so manually */
      (void)nco_inq_varid(in_id_2,var_prc_out[idx]->nm,&var_id);
      (void)nco_inq_vartype(in_id_2,var_id,&var_type);
      var_prc_out[idx]->type=var_type;

      /* Test whether all dimensions match in sequence */
      for(idx_dim=0;idx_dim<var_prc[idx]->nbr_dim;idx_dim++){
	if(
	   !strstr(var_prc_out[idx]->dim[idx_dim]->nm,var_prc[idx]->dim[idx_dim]->nm) || /* Dimension names do not match */
	   (var_prc_out[idx]->dim[idx_dim]->cnt != var_prc[idx]->dim[idx_dim]->cnt) || /* Dimension sizes do not match */
	   False){
	  (void)fprintf(stdout,"%s: ERROR Variables do not conform:\nFile %s variable %s dimension %d is %s with size %li and count %li\nFile %s variable %s dimension %d is %s with size %li and count %li\n",prg_nm,fl_in_1,var_prc[idx]->nm,idx_dim,var_prc[idx]->dim[idx_dim]->nm,var_prc[idx]->dim[idx_dim]->sz,var_prc[idx]->dim[idx_dim]->cnt,fl_in_2,var_prc_out[idx]->nm,idx_dim,var_prc_out[idx]->dim[idx_dim]->nm,var_prc_out[idx]->dim[idx_dim]->sz,var_prc_out[idx]->dim[idx_dim]->cnt);
	  nco_exit(EXIT_FAILURE);
	} /* endif */
      } /* end loop over idx_dim */

      /* Temporarily set srt vector to account for hyperslabs while reading */
      lp=var_prc_out[idx]->srt;
      var_prc_out[idx]->srt=var_prc[idx]->srt;
      (void)nco_var_get(in_id_2,var_prc_out[idx]);
      /* Reset srt vector to zeros for eventual output */
      var_prc_out[idx]->srt=lp;
    }else{
      var_sct *var_tmp=NULL;
      int var_id; /* [id] Variable ID */
      
      /* var1 and var2 have differing numbers of dimensions so make var2 conform to var1 */
      var_prc_out[idx]=nco_var_free(var_prc_out[idx]);
      (void)nco_inq_varid(in_id_2,var_prc[idx]->nm,&var_id);
      var_prc_out[idx]=nco_var_fll(in_id_2,var_id,var_prc[idx]->nm,dim,nbr_dmn_xtr);
      (void)nco_var_get(in_id_2,var_prc_out[idx]);
      
      /* Pass dummy pointer so we do not lose track of original */
      var_tmp=nco_var_cnf_dmn(var_prc[idx],var_prc_out[idx],var_tmp,MUST_CONFORM,&DO_CONFORM);
      var_prc_out[idx]=nco_var_free(var_prc_out[idx]);
      var_prc_out[idx]=var_tmp;
    } /* end else */
    
    /* var2 now conforms in size to var1, and is in memory */

    /* Make sure var2 conforms to type of var1 */
    if(var_prc_out[idx]->type != var_prc[idx]->type){
      (void)fprintf(stderr,"%s: WARNING Input variables do not conform in type:\nFile 1 = %s variable %s has type %s\nFile 2 = %s variable %s has type %s\nFile 3 = %s variable %s will have type %s\n",prg_nm,fl_in_1,var_prc[idx]->nm,nco_typ_sng(var_prc[idx]->type),fl_in_2,var_prc_out[idx]->nm,nco_typ_sng(var_prc_out[idx]->type),fl_out,var_prc[idx]->nm,nco_typ_sng(var_prc[idx]->type));
    }  /* endif different type */
    var_prc_out[idx]=nco_var_cnf_typ(var_prc[idx]->type,var_prc_out[idx]);

    /* The mss_val in fl_1, if any, overrides mss_val in fl_2 */
    if(has_mss_val) mss_val=var_prc[idx]->mss_val; else mss_val=var_prc_out[idx]->mss_val;
    has_mss_val=has_mss_val || var_prc_out[idx]->has_mss_val; 
    
    (void)nco_var_sbt(var_prc[idx]->type,var_prc[idx]->sz,has_mss_val,mss_val,var_prc_out[idx]->val,var_prc[idx]->val);
    
    var_prc_out[idx]->val.vp=nco_free(var_prc_out[idx]->val.vp);
    
    /* Copy differences to output file and free differencing buffer */
    if(var_prc[idx]->nbr_dim == 0){
      (void)nco_put_var1(out_id,var_prc[idx]->id,var_prc_out[idx]->srt,var_prc[idx]->val.vp,var_prc[idx]->type);
    }else{ /* end if variable is a scalar */
      (void)nco_put_vara(out_id,var_prc[idx]->id,var_prc_out[idx]->srt,var_prc_out[idx]->cnt,var_prc[idx]->val.vp,var_prc[idx]->type);
    } /* end else */
    var_prc[idx]->val.vp=nco_free(var_prc[idx]->val.vp);
    
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
