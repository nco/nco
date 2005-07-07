/* $Header: /data/zender/nco_20150216/nco/src/nco/ncbo.c,v 1.55 2005-07-07 23:52:13 zender Exp $ */

/* ncbo -- netCDF binary operator */

/* Purpose: Compute sum, difference, product, or ratio of specified hyperslabs of specfied variables
   from two input netCDF files and output them to a single file. */

/* Copyright (C) 1995--2005 Charlie Zender
   
   This software may be modified and/or re-distributed under the terms of the GNU General Public License (GPL) Version 2
   The full license text is at http://www.gnu.ai.mit.edu/copyleft/gpl.html 
   and in the file nco/doc/LICENSE in the NCO source distribution.

   As a special exception to the terms of the GPL, you are permitted 
   to link the NCO source code with the HDF, netCDF, OPeNDAP, and UDUnits
   libraries and to distribute the resulting executables under the terms 
   of the GPL, but in addition obeying the extra stipulations of the 
   HDF, netCDF, OPeNDAP, and UDUnits licenses.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
   See the GNU General Public License for more details.
   
   The original author of this software, Charlie Zender, wants to improve it
   with the help of your suggestions, improvements, bug-reports, and patches.
   Please contact the NCO project at http://nco.sf.net or by writing
   Charlie Zender
   Department of Earth System Science
   University of California at Irvine
   Irvine, CA 92697-3100 */

/* Usage:
   ncbo -O in.nc in.nc foo.nc
   ncbo -O -v mss_val in.nc in.nc foo.nc
   ncbo -p /data/zender/tmp h0001.nc foo.nc
   ncbo -p /data/zender/tmp -l /data/zender/tmp/rmt h0001.nc h0002.nc foo.nc
   ncbo -p /ZENDER/tmp -l /data/zender/tmp/rmt h0001.nc h0002.nc foo.nc
   ncbo -p /ZENDER/tmp -l /usr/tmp/zender h0001.nc h0002.nc foo.nc
   
   Test type conversion:
   ncks -O -C -v float_var in.nc foo1.nc
   ncrename -v float_var,double_var foo1.nc
   ncks -O -C -v double_var in.nc foo2.nc
   ncbo -O -C -v double_var foo1.nc foo2.nc foo3.nc
   ncbo -O -C -v double_var foo2.nc foo1.nc foo4.nc
   ncks -H -m foo1.nc
   ncks -H -m foo2.nc
   ncks -H -m foo3.nc
   ncks -H -m foo4.nc
   
   Test nco_var_cnf_dmn:
   ncks -O -v scalar_var in.nc foo.nc ; ncrename -v scalar_var,four_dmn_rec_var foo.nc ; ncbo -O -v four_dmn_rec_var in.nc foo.nc foo2.nc */
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
#ifndef HAVE_GETOPT_LONG
#include "nco_getopt.h"
#else /* !NEED_GETOPT_LONG */ 
#ifdef HAVE_GETOPT_H
#include <getopt.h>
#endif /* !HAVE_GETOPT_H */ 
#endif /* HAVE_GETOPT_LONG */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions and C library */

/* Personal headers */
/* #define MAIN_PROGRAM_FILE MUST precede #include libnco.h */
#define MAIN_PROGRAM_FILE
#include "libnco.h" /* netCDF Operator (NCO) library */

int 
main(int argc,char **argv)
{
  bool DO_CONFORM; /* Did nco_var_cnf_dmn() find truly conforming variables? */
  bool EXCLUDE_INPUT_LIST=False; /* Option c */
  bool EXTRACT_ALL_COORDINATES=False; /* Option c */
  bool EXTRACT_ASSOCIATED_COORDINATES=True; /* Option C */
  bool FILE_1_RETRIEVED_FROM_REMOTE_LOCATION;
  bool FILE_2_RETRIEVED_FROM_REMOTE_LOCATION;
  bool FL_LST_IN_FROM_STDIN=False; /* [flg] fl_lst_in comes from stdin */
  bool FORCE_64BIT_OFFSET=False; /* Option Z */
  bool FORCE_APPEND=False; /* Option A */
  bool FORCE_OVERWRITE=False; /* Option O */
  bool FORTRAN_IDX_CNV=False; /* Option F */
  bool HISTORY_APPEND=True; /* Option h */
  bool MUST_CONFORM=True; /* Must nco_var_cnf_dmn() find truly conforming variables? */
  bool NCAR_CCSM_FORMAT;
  bool REMOVE_REMOTE_FILES_AFTER_PROCESSING=True; /* Option R */
  
  char **fl_lst_abb=NULL; /* Option a */
  char **fl_lst_in;
  char **var_lst_in=NULL_CEWI;
  char *cmd_ln;
  char *fl_in_1=NULL; /* fl_in_1 is nco_realloc'd when not NULL */;
  char *fl_in_2=NULL; /* fl_in_2 is nco_realloc'd when not NULL */;
  char *fl_out=NULL; /* Option o */
  char *fl_out_tmp;
  char *fl_pth=NULL; /* Option p */
  char *fl_pth_lcl=NULL; /* Option l */
  char *lmt_arg[NC_MAX_DIMS];
  char *nco_op_typ_sng=NULL; /* [sng] Operation type */
  char *optarg_lcl=NULL; /* [sng] Local copy of system optarg */
  char *time_bfr_srt;
  
  const char * const CVS_Id="$Id: ncbo.c,v 1.55 2005-07-07 23:52:13 zender Exp $"; 
  const char * const CVS_Revision="$Revision: 1.55 $";
  const char * const opt_sht_lst="ACcD:d:Fhl:Oo:p:rRt:v:xy:Z-:";
  
  dmn_sct **dim_1;
  dmn_sct **dim_2;
  dmn_sct **dmn_out;
  
  extern char *optarg;
  extern int optind;
  
  /* Using naked stdin/stdout/stderr in parallel region generates warning
     Copy appropriate filehandle to variable scoped shared in parallel clause */
  FILE * const fp_stderr=stderr; /* [fl] stderr filehandle CEWI */
  FILE * const fp_stdout=stdout; /* [fl] stdout filehandle CEWI */

  int abb_arg_nbr=0;
  int fl_idx;
  int fl_nbr=0;
  int fll_md_old; /* [enm] Old fill mode */
  int idx;
  int in_id_1;  
  int in_id_2;  
  int lmt_nbr=0; /* Option d. NB: lmt_nbr gets incremented */
  int nbr_dmn_fl_1;
  int nbr_dmn_fl_2;
  int nbr_dmn_xtr_1;
  int nbr_dmn_xtr_2;
  int nbr_var_fix_1; /* nbr_var_fix_1 gets incremented */
  int nbr_var_fl_1;
  int nbr_var_fl_2;
  int nbr_var_prc_1; /* nbr_var_prc_1 gets incremented */
  int nbr_xtr_1=0; /* nbr_xtr_1 won't otherwise be set for -c with no -v */
  int nbr_xtr_2=0; /* nbr_xtr_2 won't otherwise be set for -c with no -v */
  int nco_op_typ=nco_op_nil; /* [enm] Operation type */
  int opt;
  int out_id;  
  int rcd=NC_NOERR; /* [rcd] Return code */
  int thr_nbr=0; /* [nbr] Thread number Option t */
  int var_lst_in_nbr=0;
  
  lmt_sct **lmt;
  
  nm_id_sct *dmn_lst_1;
  nm_id_sct *dmn_lst_2;
  nm_id_sct *xtr_lst_1=NULL; /* xtr_lst_1 may be alloc()'d from NULL with -c option */
  nm_id_sct *xtr_lst_2=NULL; /* xtr_lst_2 may be alloc()'d from NULL with -c option */
  
  time_t time_crr_time_t;
  
  var_sct **var_1;
  var_sct **var_2;
  var_sct **var_fix_1;
  var_sct **var_fix_out;
  var_sct **var_out;
  var_sct **var_prc_1;
  var_sct **var_prc_out;
  
  static struct option opt_lng[]=
    { /* Structure ordered by short option key if possible */
      {"append",no_argument,0,'A'},
      {"coords",no_argument,0,'c'},
      {"crd",no_argument,0,'c'},
      {"no-coords",no_argument,0,'C'},
      {"no-crd",no_argument,0,'C'},
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
      {"thr_nbr",required_argument,0,'t'},
      {"exclude",no_argument,0,'x'},
      {"xcl",no_argument,0,'x'},
      {"operation",required_argument,0,'y'},
      {"op_typ",required_argument,0,'y'},
      {"64-bit-offset",no_argument,0,'Z'},
      {"help",no_argument,0,'?'},
      {0,0,0,0}
    }; /* end opt_lng */
  int opt_idx=0; /* Index of current long option into opt_lng array */
  
  /* Start clock and save command line */ 
  cmd_ln=nco_cmd_ln_sng(argc,argv);
  time_crr_time_t=time((time_t *)NULL);
  time_bfr_srt=ctime(&time_crr_time_t); time_bfr_srt=time_bfr_srt; /* Avoid compiler warning until variable is used for something */
  
  /* Get program name and set program enum (e.g., prg=ncra) */
  prg_nm=prg_prs(argv[0],&prg);
  
  /* Parse command line arguments */
  while((opt = getopt_long(argc,argv,opt_sht_lst,opt_lng,&opt_idx)) != EOF){
    switch(opt){
    case 'A': /* Toggle FORCE_APPEND */
      FORCE_APPEND=!FORCE_APPEND;
      break;
    case 'C': /* Extract all coordinates associated with extracted variables? */
      EXTRACT_ASSOCIATED_COORDINATES=False;
      break;
    case 'c':
      EXTRACT_ALL_COORDINATES=True;
      break;
    case 'D': /* The debugging level. Default is 0. */
      dbg_lvl=(unsigned short)strtol(optarg,(char **)NULL,10);
      break;
    case 'd': /* Copy argument for later processing */
      lmt_arg[lmt_nbr]=(char *)strdup(optarg);
      lmt_nbr++;
      break;
    case 'F': /* Toggle index convention. Default is 0-based arrays (C-style). */
      FORTRAN_IDX_CNV=!FORTRAN_IDX_CNV;
      break;
    case 'h': /* Toggle appending to history global attribute */
      HISTORY_APPEND=!HISTORY_APPEND;
      break;
    case 'l': /* Local path prefix for files retrieved from remote file system */
      fl_pth_lcl=(char *)strdup(optarg);
      break;
    case 'O': /* Toggle FORCE_OVERWRITE */
      FORCE_OVERWRITE=!FORCE_OVERWRITE;
      break;
    case 'o': /* Name of output file */
      fl_out=(char *)strdup(optarg);
      break;
    case 'p': /* Common file path */
      fl_pth=(char *)strdup(optarg);
      break;
    case 'R': /* Toggle removal of remotely-retrieved-files. Default is True. */
      REMOVE_REMOTE_FILES_AFTER_PROCESSING=!REMOVE_REMOTE_FILES_AFTER_PROCESSING;
      break;
    case 'r': /* Print CVS program information and copyright notice */
      (void)copyright_prn(CVS_Id,CVS_Revision);
      (void)nco_lbr_vrs_prn();
      nco_exit(EXIT_SUCCESS);
      break;
    case 't': /* Thread number */
      thr_nbr=(int)strtol(optarg,(char **)NULL,10);
      break;
    case 'v': /* Variables to extract/exclude */
      /* Replace commas with hashes when within braces (convert back later) */
      optarg_lcl=(char *)strdup(optarg);
      (void)nco_lst_comma2hash(optarg_lcl);
      var_lst_in=lst_prs_2D(optarg_lcl,",",&var_lst_in_nbr);
      optarg_lcl=(char *)nco_free(optarg_lcl);
      nbr_xtr_1=var_lst_in_nbr;
      break;
    case 'x': /* Exclude rather than extract variables specified with -v */
      EXCLUDE_INPUT_LIST=True;
      break;
    case 'y': /* User-specified operation type overrides invocation default */
      nco_op_typ_sng=(char *)strdup(optarg);
      nco_op_typ=nco_op_typ_get(nco_op_typ_sng);
      break;
    case 'Z': /* [flg] Create output file with 64-bit offsets */
      FORCE_64BIT_OFFSET=True;
      break;
    case '?': /* Print proper usage */
      (void)nco_usg_prn();
      nco_exit(EXIT_SUCCESS);
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
  fl_lst_in=nco_fl_lst_mk(argv,argc,optind,&fl_nbr,&fl_out,&FL_LST_IN_FROM_STDIN);
  
  /* Make uniform list of user-specified dimension limits */
  lmt=nco_lmt_prs(lmt_nbr,lmt_arg);
  
  /* Parse filenames */
  fl_idx=0; /* Input file _1 */
  fl_in_1=nco_fl_nm_prs(fl_in_1,fl_idx,&fl_nbr,fl_lst_in,abb_arg_nbr,fl_lst_abb,fl_pth);
  if(dbg_lvl > 0) (void)fprintf(stderr,"\nInput file %d is %s; ",fl_idx,fl_in_1);
  /* Make sure file is on local system and is readable or die trying */
  fl_in_1=nco_fl_mk_lcl(fl_in_1,fl_pth_lcl,&FILE_1_RETRIEVED_FROM_REMOTE_LOCATION);
  if(dbg_lvl > 0) (void)fprintf(stderr,"local file %s:\n",fl_in_1);
  rcd=nco_open(fl_in_1,NC_NOWRITE,&in_id_1);

  fl_idx=1; /* Input file _2 */
  fl_in_2=nco_fl_nm_prs(fl_in_2,fl_idx,&fl_nbr,fl_lst_in,abb_arg_nbr,fl_lst_abb,fl_pth);
  if(dbg_lvl > 0) (void)fprintf(stderr,"\nInput file %d is %s; ",fl_idx,fl_in_2);
  /* Make sure file is on local system and is readable or die trying */
  fl_in_2=nco_fl_mk_lcl(fl_in_2,fl_pth_lcl,&FILE_2_RETRIEVED_FROM_REMOTE_LOCATION);
  if(dbg_lvl > 0) (void)fprintf(stderr,"local file %s:\n",fl_in_2);
  rcd=nco_open(fl_in_2,NC_NOWRITE,&in_id_2);
  
  /* Get number of variables and dimensions in file */
  (void)nco_inq(in_id_1,&nbr_dmn_fl_1,&nbr_var_fl_1,(int *)NULL,(int *)NULL);
  (void)nco_inq(in_id_2,&nbr_dmn_fl_2,&nbr_var_fl_2,(int *)NULL,(int *)NULL);
  
  /* Form initial extraction list which may include extended regular expressions */
  xtr_lst_1=nco_var_lst_mk(in_id_1,nbr_var_fl_1,var_lst_in,EXTRACT_ALL_COORDINATES,&nbr_xtr_1);
  xtr_lst_2=nco_var_lst_mk(in_id_2,nbr_var_fl_2,var_lst_in,EXTRACT_ALL_COORDINATES,&nbr_xtr_2);
  
  /* Change included variables to excluded variables */
  if(EXCLUDE_INPUT_LIST) xtr_lst_1=nco_var_lst_xcl(in_id_1,nbr_var_fl_1,xtr_lst_1,&nbr_xtr_1);
  if(EXCLUDE_INPUT_LIST) xtr_lst_2=nco_var_lst_xcl(in_id_2,nbr_var_fl_2,xtr_lst_2,&nbr_xtr_2);
  
  /* Add all coordinate variables to extraction list */
  if(EXTRACT_ALL_COORDINATES) xtr_lst_1=nco_var_lst_add_crd(in_id_1,nbr_dmn_fl_1,xtr_lst_1,&nbr_xtr_1);
  if(EXTRACT_ALL_COORDINATES) xtr_lst_2=nco_var_lst_add_crd(in_id_2,nbr_dmn_fl_2,xtr_lst_2,&nbr_xtr_2);
  
  /* Make sure coordinates associated extracted variables are also on extraction list */
  if(EXTRACT_ASSOCIATED_COORDINATES) xtr_lst_1=nco_var_lst_ass_crd_add(in_id_1,xtr_lst_1,&nbr_xtr_1);
  if(EXTRACT_ASSOCIATED_COORDINATES) xtr_lst_2=nco_var_lst_ass_crd_add(in_id_2,xtr_lst_2,&nbr_xtr_2);
  
  /* With fully symmetric 1<->2 ordering, may occasionally find nbr_xtr_2 > nbr_xtr_1 
     This occurs, e.g., when fl_in_1 contains reduced variables and full coordinates
     are only in fl_in_2 and so will not appear xtr_lst_1 */
  
  /* Sort extraction list by variable ID for fastest I/O */
  if(nbr_xtr_1 > 1) xtr_lst_1=nco_lst_srt_nm_id(xtr_lst_1,nbr_xtr_1,False);
  if(nbr_xtr_2 > 1) xtr_lst_2=nco_lst_srt_nm_id(xtr_lst_2,nbr_xtr_2,False);
  
  /* We now have final list of variables to extract. Phew. */
  
  /* Find coordinate/dimension values associated with user-specified limits */
  for(idx=0;idx<lmt_nbr;idx++) (void)nco_lmt_evl(in_id_1,lmt[idx],0L,FORTRAN_IDX_CNV);

  /* Find dimensions associated with variables to be extracted */
  dmn_lst_1=nco_dmn_lst_ass_var(in_id_1,xtr_lst_1,nbr_xtr_1,&nbr_dmn_xtr_1);
  dmn_lst_2=nco_dmn_lst_ass_var(in_id_2,xtr_lst_2,nbr_xtr_2,&nbr_dmn_xtr_2);
  
  /* Fill in dimension structure for all extracted dimensions */
  dim_1=(dmn_sct **)nco_malloc(nbr_dmn_xtr_1*sizeof(dmn_sct *));
  dim_2=(dmn_sct **)nco_malloc(nbr_dmn_xtr_2*sizeof(dmn_sct *));
  for(idx=0;idx<nbr_dmn_xtr_1;idx++) dim_1[idx]=nco_dmn_fll(in_id_1,dmn_lst_1[idx].id,dmn_lst_1[idx].nm);
  for(idx=0;idx<nbr_dmn_xtr_2;idx++) dim_2[idx]=nco_dmn_fll(in_id_2,dmn_lst_2[idx].id,dmn_lst_2[idx].nm);
  /* Dimension lists no longer needed */
  dmn_lst_1=nco_nm_id_lst_free(dmn_lst_1,nbr_dmn_xtr_1);
  dmn_lst_2=nco_nm_id_lst_free(dmn_lst_2,nbr_dmn_xtr_2);
  
  /* Merge hyperslab limit information into dimension structures */
  if(lmt_nbr > 0) (void)nco_dmn_lmt_mrg(dim_1,nbr_dmn_xtr_1,lmt,lmt_nbr);
  if(lmt_nbr > 0) (void)nco_dmn_lmt_mrg(dim_2,nbr_dmn_xtr_2,lmt,lmt_nbr);
  
  /* Duplicate input dimension structures for output dimension structures */
  dmn_out=(dmn_sct **)nco_malloc(nbr_dmn_xtr_1*sizeof(dmn_sct *));
  for(idx=0;idx<nbr_dmn_xtr_1;idx++){
    dmn_out[idx]=nco_dmn_dpl(dim_1[idx]);
    (void)nco_dmn_xrf(dim_1[idx],dmn_out[idx]); 
  } /* end loop over idx */
  
  if(dbg_lvl > 3){
    for(idx=0;idx<nbr_xtr_1;idx++) (void)fprintf(stderr,"xtr_lst_1[%d].nm = %s, .id= %d\n",idx,xtr_lst_1[idx].nm,xtr_lst_1[idx].id);
  } /* end if */
  
  /* Is this an NCAR CCSM-format history tape? */
  NCAR_CCSM_FORMAT=nco_ncar_csm_inq(in_id_1);
  
  /* Fill in variable structure list for all extracted variables */
  var_1=(var_sct **)nco_malloc(nbr_xtr_1*sizeof(var_sct *));
  var_2=(var_sct **)nco_malloc(nbr_xtr_2*sizeof(var_sct *));
  var_out=(var_sct **)nco_malloc(nbr_xtr_1*sizeof(var_sct *));
  for(idx=0;idx<nbr_xtr_1;idx++){
    var_1[idx]=nco_var_fll(in_id_1,xtr_lst_1[idx].id,xtr_lst_1[idx].nm,dim_1,nbr_dmn_xtr_1);
    var_2[idx]=nco_var_fll(in_id_2,xtr_lst_2[idx].id,xtr_lst_2[idx].nm,dim_2,nbr_dmn_xtr_2);
    var_out[idx]=nco_var_dpl(var_1[idx]);
    (void)nco_xrf_var(var_1[idx],var_out[idx]);
    (void)nco_xrf_dmn(var_out[idx]);
  } /* end loop over idx */
  /* Extraction list no longer needed */
  xtr_lst_1=nco_nm_id_lst_free(xtr_lst_1,nbr_xtr_1);
  xtr_lst_2=nco_nm_id_lst_free(xtr_lst_2,nbr_xtr_2);
  
  /* Divide variable lists into lists of fixed variables and variables to be processed */
  (void)nco_var_lst_dvd(var_1,var_out,nbr_xtr_1,NCAR_CCSM_FORMAT,nco_pck_plc_nil,nco_pck_map_nil,(dmn_sct **)NULL,0,&var_fix_1,&var_fix_out,&nbr_var_fix_1,&var_prc_1,&var_prc_out,&nbr_var_prc_1);
  
  /* fxm: Make sure lists are in same order */

  /* Open output file */
  fl_out_tmp=nco_fl_out_open(fl_out,FORCE_APPEND,FORCE_OVERWRITE,FORCE_64BIT_OFFSET,&out_id);
  
  /* Copy global attributes */
  (void)nco_att_cpy(in_id_1,out_id,NC_GLOBAL,NC_GLOBAL,True);
  
  /* Catenate time-stamped command line to "history" global attribute */
  if(HISTORY_APPEND) (void)nco_hst_att_cat(out_id,cmd_ln);
  cmd_ln=(char *)nco_free(cmd_ln);
  
  /* Initialize thread information */
  thr_nbr=nco_openmp_ini(thr_nbr);
  if(thr_nbr > 0 && HISTORY_APPEND) (void)nco_thr_att_cat(out_id,thr_nbr);
  
  /* Define dimensions in output file */
  (void)nco_dmn_dfn(fl_out,out_id,dmn_out,nbr_dmn_xtr_1);
  
  /* Define variables in output file, copy their attributes */
  (void)nco_var_dfn(in_id_1,fl_out,out_id,var_out,nbr_xtr_1,(dmn_sct **)NULL,(int)0,nco_pck_plc_nil,nco_pck_map_nil);
  
  /* Turn off default filling behavior to enhance efficiency */
  rcd=nco_set_fill(out_id,NC_NOFILL,&fll_md_old);
  
  /* Take output file out of define mode */
  (void)nco_enddef(out_id);
  
  /* Zero start vectors for all output variables */
  (void)nco_var_srt_zero(var_out,nbr_xtr_1);
  
  /* Copy variable data for non-processed variables */
  (void)nco_var_val_cpy(in_id_1,out_id,var_fix_1,nbr_var_fix_1);
  
  /* The code for ncbo() has been similar to ncea() (and ncra()) wherever possible
     The major differences occur where performance would otherwise suffer
     From now on, however, the binary-file and binary-operation nature of ncbo()
     is too different from ncea() paradigm to justify following ncea() style.
     Instead, symmetric nomenclature of file_1, file_2 is adopted, and 
     differences are performed variable by variable so peak memory usage goes as
     Order(2*maximum variable size) rather than Order(3*maximum record size) or
     Order(3*file size) 
     
     ncbo tries hard (probably too hard) to save memory
     ncbo is a three file operator (input, input, output) but manages to get by 
     with only two lists of variable structures at any given time.
     The resulting is fairly convoluted and hard to maintain
     A re-write would be beneficial
     ncbo overwrites variable structure from file_2 with structure for file_3 (output file)
     but, at the same time, it writes value array for file_3 into value part of file_1
     variable structures. */
  
  /* Perform various error-checks on input file */
  if(False) (void)nco_fl_cmp_err_chk();
  
  /* Default operation type depends on invocation name */
  if(nco_op_typ_sng == NULL) nco_op_typ=nco_op_typ_get(nco_op_typ_sng);
    
  /* Loop over each differenced variable */
#ifdef _OPENMP
  /* OpenMP notes:
     shared(): msk and wgt are not altered within loop
     private(): wgt_avg does not need initialization */
#pragma omp parallel for default(none) private(DO_CONFORM,MUST_CONFORM,idx) shared(dbg_lvl,dim_1,fl_in_1,fl_in_2,fl_out,fp_stderr,in_id_1,in_id_2,nbr_dmn_xtr_1,nbr_var_prc_1,nco_op_typ,out_id,prg_nm,var_prc_1,var_prc_out)
#endif /* !_OPENMP */
  for(idx=0;idx<nbr_var_prc_1;idx++){
    int var_id; /* [id] Variable ID */
    int has_mss_val=False;
    ptr_unn mss_val;
    if(dbg_lvl > 0) (void)fprintf(fp_stderr,"%s, ",var_prc_1[idx]->nm);
    if(dbg_lvl > 0) (void)fflush(fp_stderr);
    
    (void)nco_var_mtd_refresh(in_id_1,var_prc_1[idx]); /* Routine contains OpenMP critical regions */
    has_mss_val=var_prc_1[idx]->has_mss_val; 
    (void)nco_var_get(in_id_1,var_prc_1[idx]); /* Routine contains OpenMP critical regions */
    
    /* Save output variable ID from being overwritten in refresh call */
    var_prc_1[idx]->id=var_prc_out[idx]->id;
    /* Find and set variable dmn_nbr, ID, mss_val, type in second file */
    (void)nco_var_mtd_refresh(in_id_2,var_prc_out[idx]);
    
    /* Determine whether var1 and var2 conform */
    if(var_prc_out[idx]->nbr_dim == var_prc_1[idx]->nbr_dim){
      int dmn_idx;
      long *lp;
      
      /* Do all dimensions match in sequence? */
      for(dmn_idx=0;dmn_idx<var_prc_1[idx]->nbr_dim;dmn_idx++){
	if(
	   /* fxm: TODO nco551: compare var_1 vs. var_2 dim's here, not var_1 vs. var_out */
	   strcmp(var_prc_out[idx]->dim[dmn_idx]->nm,var_prc_1[idx]->dim[dmn_idx]->nm) || /* Dimension names do not match */
	   (var_prc_out[idx]->dim[dmn_idx]->cnt != var_prc_1[idx]->dim[dmn_idx]->cnt) || /* Dimension sizes do not match */
	   False){
	  (void)fprintf(fp_stdout,"%s: ERROR Variables do not conform:\nFile %s variable %s dimension %d is %s with size %li and count %li\nFile %s variable %s dimension %d is %s with size %li and count %li\n",prg_nm,fl_in_1,var_prc_1[idx]->nm,dmn_idx,var_prc_1[idx]->dim[dmn_idx]->nm,var_prc_1[idx]->dim[dmn_idx]->sz,var_prc_1[idx]->dim[dmn_idx]->cnt,fl_in_2,var_prc_out[idx]->nm,dmn_idx,var_prc_out[idx]->dim[dmn_idx]->nm,var_prc_out[idx]->dim[dmn_idx]->sz,var_prc_out[idx]->dim[dmn_idx]->cnt);
	  nco_exit(EXIT_FAILURE);
	} /* endif */
      } /* end loop over dmn_idx */
      
      /* Temporarily set srt vector to account for hyperslabs while reading */
      lp=var_prc_out[idx]->srt;
      var_prc_out[idx]->srt=var_prc_1[idx]->srt;
      (void)nco_var_get(in_id_2,var_prc_out[idx]);
      /* Reset srt vector to zeros for eventual output */
      var_prc_out[idx]->srt=lp;
    }else{ /* var_prc_out[idx]->nbr_dim != var_prc_1[idx]->nbr_dim) */
      /* Number of dimensions do not match, attempt to broadcast variables */
      var_sct *var_tmp=NULL;
      
      /* fxm: TODO 268 allow var1 or var2 to be broadcast */
      /* var1 and var2 have differing numbers of dimensions so make var2 conform to var1 */
      var_prc_out[idx]=nco_var_free(var_prc_out[idx]);
      (void)nco_inq_varid(in_id_2,var_prc_1[idx]->nm,&var_id);
      var_prc_out[idx]=nco_var_fll(in_id_2,var_id,var_prc_1[idx]->nm,dim_1,nbr_dmn_xtr_1);
      (void)nco_var_get(in_id_2,var_prc_out[idx]);
      
      /* Pass dummy pointer so we do not lose track of original */
      var_tmp=nco_var_cnf_dmn(var_prc_1[idx],var_prc_out[idx],var_tmp,MUST_CONFORM,&DO_CONFORM);
      var_prc_out[idx]=nco_var_free(var_prc_out[idx]);
      var_prc_out[idx]=var_tmp;
    } /* end else */
    
    /* var2 now conforms in size to var1, and is in memory */
    
    /* fxm: TODO 268 allow var1 or var2 to typecast */
    /* Make sure var2 conforms to type of var1 */
    if(var_prc_out[idx]->type != var_prc_1[idx]->type){
      (void)fprintf(fp_stderr,"%s: WARNING Input variables do not conform in type:\nFile 1 = %s variable %s has type %s\nFile 2 = %s variable %s has type %s\nFile 3 = %s variable %s will have type %s\n",prg_nm,fl_in_1,var_prc_1[idx]->nm,nco_typ_sng(var_prc_1[idx]->type),fl_in_2,var_prc_out[idx]->nm,nco_typ_sng(var_prc_out[idx]->type),fl_out,var_prc_1[idx]->nm,nco_typ_sng(var_prc_1[idx]->type));
    }  /* endif different type */
    var_prc_out[idx]=nco_var_cnf_typ(var_prc_1[idx]->type,var_prc_out[idx]);
    
    /* Change missing_value of var_prc_out, if any, to missing_value of var_prc_1, if any */
    has_mss_val=nco_mss_val_cnf(var_prc_1[idx],var_prc_out[idx]);
    
    /* mss_val in fl_1, if any, overrides mss_val in fl_2 */
    if(has_mss_val) mss_val=var_prc_1[idx]->mss_val;
    
    /* Perform specified binary operation */
    switch(nco_op_typ){
    case nco_op_add: /* [enm] Add file_1 to file_2 */
      (void)nco_var_add(var_prc_1[idx]->type,var_prc_1[idx]->sz,has_mss_val,mss_val,var_prc_out[idx]->val,var_prc_1[idx]->val); break;
    case nco_op_mlt: /* [enm] Multiply file_1 by file_2 */
      (void)nco_var_mlt(var_prc_1[idx]->type,var_prc_1[idx]->sz,has_mss_val,mss_val,var_prc_out[idx]->val,var_prc_1[idx]->val); break;
    case nco_op_dvd: /* [enm] Divide file_1 by file_2 */
      (void)nco_var_dvd(var_prc_1[idx]->type,var_prc_1[idx]->sz,has_mss_val,mss_val,var_prc_out[idx]->val,var_prc_1[idx]->val); break;
    case nco_op_sbt: /* [enm] Subtract file_2 from file_1 */
      (void)nco_var_sbt(var_prc_1[idx]->type,var_prc_1[idx]->sz,has_mss_val,mss_val,var_prc_out[idx]->val,var_prc_1[idx]->val); break;
    default: /* Other defined nco_op_typ values are valid for ncra(), ncrcat(), ncwa(), not ncbo() */
      (void)fprintf(fp_stdout,"%s: ERROR Illegal nco_op_typ in binary operation\n",prg_nm);
      nco_exit(EXIT_FAILURE);
      break;
    } /* end case */
    
    var_prc_out[idx]->val.vp=nco_free(var_prc_out[idx]->val.vp);

#ifdef _OPENMP
#pragma omp critical
#endif /* _OPENMP */
    { /* begin OpenMP critical */
      /* Copy result to output file and free workspace buffer */
      if(var_prc_1[idx]->nbr_dim == 0){
	(void)nco_put_var1(out_id,var_prc_1[idx]->id,var_prc_out[idx]->srt,var_prc_1[idx]->val.vp,var_prc_1[idx]->type);
      }else{ /* end if variable is scalar */
	(void)nco_put_vara(out_id,var_prc_1[idx]->id,var_prc_out[idx]->srt,var_prc_out[idx]->cnt,var_prc_1[idx]->val.vp,var_prc_1[idx]->type);
      } /* end else */
    } /* end OpenMP critical */
    var_prc_1[idx]->val.vp=nco_free(var_prc_1[idx]->val.vp);
    
  } /* end (OpenMP parallel for) loop over idx */
  if(dbg_lvl > 0) (void)fprintf(stderr,"\n");
  
  /* Close input netCDF files */
  nco_close(in_id_1);
  nco_close(in_id_2);
  
  /* Close output file and move it from temporary to permanent location */
  (void)nco_fl_out_cls(fl_out,fl_out_tmp,out_id);
  
  /* Remove local copy of file */
  if(FILE_1_RETRIEVED_FROM_REMOTE_LOCATION && REMOVE_REMOTE_FILES_AFTER_PROCESSING) (void)nco_fl_rm(fl_in_1);
  if(FILE_2_RETRIEVED_FROM_REMOTE_LOCATION && REMOVE_REMOTE_FILES_AFTER_PROCESSING) (void)nco_fl_rm(fl_in_2);
  
  /* ncbo-unique memory */
  if(fl_in_1 != NULL) fl_in_1=(char *)nco_free(fl_in_1);
  if(fl_in_2 != NULL) fl_in_2=(char *)nco_free(fl_in_2);

  /* NCO-generic clean-up */
  /* Free individual strings */
  if(fl_out != NULL) fl_out=(char *)nco_free(fl_out);
  if(fl_out_tmp != NULL) fl_out_tmp=(char *)nco_free(fl_out_tmp);
  if(fl_pth != NULL) fl_pth=(char *)nco_free(fl_pth);
  if(fl_pth_lcl != NULL) fl_pth_lcl=(char *)nco_free(fl_pth_lcl);
  /* Free lists of strings */
  if(fl_lst_in != NULL && fl_lst_abb == NULL) fl_lst_in=nco_sng_lst_free(fl_lst_in,fl_nbr); 
  if(fl_lst_in != NULL && fl_lst_abb != NULL) fl_lst_in=nco_sng_lst_free(fl_lst_in,1);
  if(fl_lst_abb != NULL) fl_lst_abb=nco_sng_lst_free(fl_lst_abb,abb_arg_nbr);
  if(var_lst_in_nbr > 0) var_lst_in=nco_sng_lst_free(var_lst_in,var_lst_in_nbr);
  /* Free limits */
  for(idx=0;idx<lmt_nbr;idx++) lmt_arg[idx]=(char *)nco_free(lmt_arg[idx]);
  if(lmt_nbr > 0) lmt=nco_lmt_lst_free(lmt,lmt_nbr);
  /* Free dimension lists */
  if(nbr_dmn_xtr_1 > 0) dim_1=nco_dmn_lst_free(dim_1,nbr_dmn_xtr_1);
  if(nbr_dmn_xtr_1 > 0) dmn_out=nco_dmn_lst_free(dmn_out,nbr_dmn_xtr_1);
  /* Free variable lists */
  if(nbr_xtr_1 > 0) var_1=nco_var_lst_free(var_1,nbr_xtr_1);
  if(nbr_xtr_1 > 0) var_out=nco_var_lst_free(var_out,nbr_xtr_1);
  var_prc_1=(var_sct **)nco_free(var_prc_1);
  var_prc_out=(var_sct **)nco_free(var_prc_out);
  var_fix_1=(var_sct **)nco_free(var_fix_1);
  var_fix_out=(var_sct **)nco_free(var_fix_out);

  if(rcd != NC_NOERR) nco_err_exit(rcd,"main");
  nco_exit_gracefully();
  return EXIT_SUCCESS;
} /* end main() */
