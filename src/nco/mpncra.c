/* $Header: /data/zender/nco_20150216/nco/src/nco/mpncra.c,v 1.16 2005-09-18 01:13:51 zender Exp $ */

/* ncra -- netCDF running averager */

/* Purpose: Compute averages or extract series of specified hyperslabs of 
   specfied variables of multiple input netCDF files and output them 
   to a single file. */

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
   Please contact the NCO project at http://nco.sf.net or write to
   Charlie Zender
   Department of Earth System Science
   University of California at Irvine
   Irvine, CA 92697-3100 */

/* Usage:
   ncra -n 3,4,1 -p ${HOME}/nco/data h0001.nc foo.nc
   ncra -n 3,4,1 -p ${HOME}/nco/data -l ${HOME} h0001.nc foo.nc
   ncra -n 3,4,1 -p /ZENDER/tmp -l ${HOME}/nco/data h0001.nc foo.nc
   scp ~/nco/src/nco/ncra.c esmf.ess.uci.edu:nco/src/nco
   
   ncea in.nc in.nc foo.nc
   ncea -n 3,4,1 -p ${HOME}/nco/data h0001.nc foo.nc
   ncea -n 3,4,1 -p ${HOME}/nco/data -l ${HOME} h0001.nc foo.nc
   ncea -n 3,4,1 -p /ZENDER/tmp -l ${HOME} h0001.nc foo.nc */

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

/* Internationalization i18n, Linux Journal 200211 p. 57--59 */
#ifdef I18N
#include <libintl.h> /* Internationalization i18n */
#include <locale.h> /* Locale setlocale() */
#define _(sng) gettext (sng)
#define gettext_noop(sng) (sng)
#define N_(sng) gettext_noop(sng)
#endif /* I18N */
#ifndef _LIBINTL_H
# define gettext(foo) foo
#endif /* _LIBINTL_H */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions and C library */
#ifdef ENABLE_MPI
#include <mpi.h> /* MPI definitions */
#define WORK_DONE 600 /* Move this to nco.h if its required */
#endif /* !ENABLE_MPI */

/* Personal headers */
/* #define MAIN_PROGRAM_FILE MUST precede #include libnco.h */
#define MAIN_PROGRAM_FILE
#include "libnco.h" /* netCDF Operator (NCO) library */

int 
main(int argc,char **argv)
{
  bool CNV_ARM=int_CEWI;
  bool CNV_CCM_CCSM_CF=int_CEWI;
  bool EXCLUDE_INPUT_LIST=False; /* Option c */
  bool EXTRACT_ALL_COORDINATES=False; /* Option c */
  bool EXTRACT_ASSOCIATED_COORDINATES=True; /* Option C */
  bool FILE_RETRIEVED_FROM_REMOTE_LOCATION;
  bool FL_LST_IN_APPEND=True; /* Option H */
  bool FL_LST_IN_FROM_STDIN=False; /* [flg] fl_lst_in comes from stdin */
  bool FMT_64BIT=False; /* Option Z */
  bool FORCE_APPEND=False; /* Option A */
  bool FORCE_OVERWRITE=False; /* Option O */
  bool FORTRAN_IDX_CNV=False; /* Option F */
  bool HISTORY_APPEND=True; /* Option h */
  bool LAST_RECORD=False;
  bool REMOVE_REMOTE_FILES_AFTER_PROCESSING=True; /* Option R */
  
  char **fl_lst_abb=NULL; /* Option n */
  char **fl_lst_in;
  char **var_lst_in=NULL_CEWI;
  char *cmd_ln;
  char *fl_in=NULL;
  char *fl_out=NULL; /* Option o */
  char *fl_out_tmp=NULL_CEWI;
  char *fl_pth=NULL; /* Option p */
  char *fl_pth_lcl=NULL; /* Option l */
  char *lmt_arg[NC_MAX_DIMS];
  char *nco_op_typ_sng=NULL_CEWI; /* [sng] Operation type Option y */
  char *nco_pck_plc_sng=NULL_CEWI; /* [sng] Packing policy Option P */
  char *optarg_lcl=NULL; /* [sng] Local copy of system optarg */
  char *time_bfr_srt;
  
  const char * const CVS_Id="$Id: mpncra.c,v 1.16 2005-09-18 01:13:51 zender Exp $"; 
  const char * const CVS_Revision="$Revision: 1.16 $";
  const char * const opt_sht_lst="ACcD:d:FHhl:n:Oo:p:P:rRt:v:xY:y:Z-:";
  
  dmn_sct **dim;
  dmn_sct **dmn_out;
  
  extern char *optarg;
  extern int optind;
  
  /* Using naked stdin/stdout/stderr in parallel region generates warning
     Copy appropriate filehandle to variable scoped shared in parallel clause */
  FILE * const fp_stderr=stderr; /* [fl] stderr filehandle CEWI */
  
  int abb_arg_nbr=0;
  int fl_idx;
  int fl_nbr=0;
  int fll_md_old; /* [enm] Old fill mode */
  int idx=int_CEWI;
  int in_id;  
  int lmt_nbr=0; /* Option d. NB: lmt_nbr gets incremented */
  int nbr_dmn_fl;
  int nbr_dmn_xtr;
  int nbr_var_fix; /* nbr_var_fix gets incremented */
  int nbr_var_fl;
  int nbr_var_prc; /* nbr_var_prc gets incremented */
  int nbr_xtr=0; /* nbr_xtr won't otherwise be set for -c with no -v */
  int nco_op_typ=nco_op_avg; /* [enm] Default operation is averaging */
  int nco_pck_plc=nco_pck_plc_nil; /* [enm] Default packing is none */
  int opt;
  int out_id;  
  int rcd=NC_NOERR; /* [rcd] Return code */
  int rec_dmn_id=NCO_REC_DMN_UNDEFINED;
  int thr_nbr=0; /* [nbr] Thread number Option t */
  int var_lst_in_nbr=0;
  
  lmt_sct **lmt=NULL_CEWI;
  lmt_sct *lmt_rec=NULL_CEWI;
  
  long idx_rec; /* [idx] Index of current record in current input file */
  long idx_rec_out=0L; /* [idx] Index of current record in output file (0 is first, ...) */
  
#ifdef ENABLE_MPI
  /* Declare all MPI-specific variables here */
  MPI_Status mpi_stt; /* [enm] Status check to decode msg_typ */
  
  bool TOKEN_FREE=True; /* [flg] Allow MPI workers write-access to output file */
  
  const double sleep_tm=0.04; /* [s] Token request interval */
  
  const int info_bfr_lng=3; /* [nbr] Number of elements in info_bfr */
  const int wrk_id_bfr_lng=1; /* [nbr] Number of elements in wrk_id_bfr */
  
  int fl_nm_lng; /* [nbr] Output file name length */
  int info_bfr[3]; /* [bfr] Buffer containing var, idx, tkn_rsp */
  int jdx=0; /* [idx] MPI index for local variables */
  int lcl_idx_lst[60]; /* [arr] Array containing indices of variables processed at each Worker */
  int lcl_nbr_var=0; /* [nbr] Count of variables processes at each Worker */
  int msg_typ; /* [enm] MPI message type */
  int proc_id; /* [id] Process ID */
  int proc_nbr=0; /* [nbr] Number of MPI processes */
  int tkn_alloc_id=0; /* [id] To track write permission for ncrcat */
  int tkn_rsp; /* [enm] Mangager response [0,1] = [Wait,Allow] */
  int var_wrt_nbr=0; /* [nbr] Variables written to output file until now */
  int wrk_id; /* [id] Sender node ID */
  int wrk_id_bfr[1]; /* [bfr] Buffer for wrk_id */
  
#endif /* !ENABLE_MPI */
  
  nco_int base_time_srt=nco_int_CEWI;
  nco_int base_time_crr=nco_int_CEWI;
  
  nm_id_sct *dmn_lst;
  nm_id_sct *xtr_lst=NULL; /* xtr_lst may be alloc()'d from NULL with -c option */
  
  time_t time_crr_time_t;
  
  var_sct **var;
  var_sct **var_fix;
  var_sct **var_fix_out;
  var_sct **var_out=NULL_CEWI;
  var_sct **var_prc;
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
      {"fl_lst_in",no_argument,0,'H'},
      {"file_list",no_argument,0,'H'},
      {"history",no_argument,0,'h'},
      {"hst",no_argument,0,'h'},
      {"local",required_argument,0,'l'},
      {"lcl",required_argument,0,'l'},
      {"nintap",required_argument,0,'n'},
      {"overwrite",no_argument,0,'O'},
      {"ovr",no_argument,0,'O'},
      {"output",required_argument,0,'o'},
      {"fl_out",required_argument,0,'o'},
      {"path",required_argument,0,'p'},
      {"pack",required_argument,0,'P'},
      {"retain",no_argument,0,'R'},
      {"rtn",no_argument,0,'R'},
      {"revision",no_argument,0,'r'},
      {"thr_nbr",required_argument,0,'t'},
      {"threads",required_argument,0,'t'},
      {"omp_num_threads",required_argument,0,'t'},
      {"variable",required_argument,0,'v'},
      {"version",no_argument,0,'r'},
      {"vrs",no_argument,0,'r'},
      {"exclude",no_argument,0,'x'},
      {"xcl",no_argument,0,'x'},
      {"pseudonym",required_argument,0,'Y'},
      {"program",required_argument,0,'Y'},
      {"prg_nm",required_argument,0,'Y'},
      {"math",required_argument,0,'y'},
      {"64-bit-offset",no_argument,0,'Z'},
      {"help",no_argument,0,'?'},
      {0,0,0,0}
    }; /* end opt_lng */
  int opt_idx=0; /* Index of current long option into opt_lng array */
  
#ifdef _LIBINTL_H
  setlocale(LC_ALL,""); /* LC_ALL sets all localization tokens to same value */
  bindtextdomain("nco","/home/zender/share/locale"); /* ${LOCALEDIR} is e.g., /usr/share/locale */
  /* MO files should be in ${LOCALEDIR}/es/LC_MESSAGES */
  textdomain("nco"); /* PACKAGE is name of program */
#endif /* not _LIBINTL_H */
  
#ifdef ENABLE_MPI
  /* MPI Initialization */
  MPI_Init(&argc,&argv);
  MPI_Comm_size(MPI_COMM_WORLD,&proc_nbr);
  MPI_Comm_rank(MPI_COMM_WORLD,&proc_id);
#endif /* !ENABLE_MPI */
  
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
    case 'D': /* Debugging level. Default is 0. */
      dbg_lvl=(unsigned short)strtol(optarg,(char **)NULL,10);
      break;
    case 'd': /* Copy argument for later processing */
      lmt_arg[lmt_nbr]=(char *)strdup(optarg);
      lmt_nbr++;
      break;
    case 'F': /* Toggle index convention. Default is 0-based arrays (C-style). */
      FORTRAN_IDX_CNV=!FORTRAN_IDX_CNV;
      break;
    case 'H': /* Toggle writing input file list attribute */
      FL_LST_IN_APPEND=!FL_LST_IN_APPEND;
      break;
    case 'h': /* Toggle appending to history global attribute */
      HISTORY_APPEND=!HISTORY_APPEND;
      break;
    case 'l': /* Local path prefix for files retrieved from remote file system */
      fl_pth_lcl=(char *)strdup(optarg);
      break;
    case 'n': /* NINTAP-style abbreviation of files to average */
      fl_lst_abb=lst_prs_2D(optarg,",",&abb_arg_nbr);
      if(abb_arg_nbr < 1 || abb_arg_nbr > 5){
	(void)fprintf(stdout,gettext("%s: ERROR Incorrect abbreviation for file list\n"),prg_nm);
	(void)nco_usg_prn();
	nco_exit(EXIT_FAILURE);
      } /* end if */
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
    case 'P': /* Packing policy */
      nco_pck_plc_sng=(char *)strdup(optarg);
      nco_pck_plc=nco_pck_plc_get(nco_pck_plc_sng);
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
      nbr_xtr=var_lst_in_nbr;
      break;
    case 'x': /* Exclude rather than extract variables specified with -v */
      EXCLUDE_INPUT_LIST=True;
      break;
    case 'Y': /* Pseudonym */
      /* Call prg_prs to reset pseudonym */
      optarg_lcl=(char *)strdup(optarg);
      prg_nm=prg_prs(optarg_lcl,&prg);
      break;
    case 'y': /* Operation type */
      nco_op_typ_sng=(char *)strdup(optarg);
      if(prg == ncra || prg == ncea ) nco_op_typ=nco_op_typ_get(nco_op_typ_sng);
      break;
    case 'Z': /* [flg] Create output file with 64-bit offsets */
      FMT_64BIT=True;
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
  if(lmt_nbr > 0) lmt=nco_lmt_prs(lmt_nbr,lmt_arg);
  
  /* Make netCDF errors fatal and print the diagnostic */ 
  
  /* Parse filename */
  fl_in=nco_fl_nm_prs(fl_in,0,&fl_nbr,fl_lst_in,abb_arg_nbr,fl_lst_abb,fl_pth);
  /* Make sure file is on local system and is readable or die trying */
  fl_in=nco_fl_mk_lcl(fl_in,fl_pth_lcl,&FILE_RETRIEVED_FROM_REMOTE_LOCATION);
  /* Open file for reading */
  rcd=nco_open(fl_in,NC_NOWRITE,&in_id);
  
  /* Get number of variables, dimensions, and record dimension ID of input file */
  (void)nco_inq(in_id,&nbr_dmn_fl,&nbr_var_fl,(int *)NULL,&rec_dmn_id);
  
  /* Form initial extraction list which may include extended regular expressions */
  xtr_lst=nco_var_lst_mk(in_id,nbr_var_fl,var_lst_in,EXTRACT_ALL_COORDINATES,&nbr_xtr);
  
  /* Change included variables to excluded variables */
  if(EXCLUDE_INPUT_LIST) xtr_lst=nco_var_lst_xcl(in_id,nbr_var_fl,xtr_lst,&nbr_xtr);
  
  /* Add all coordinate variables to extraction list */
  if(EXTRACT_ALL_COORDINATES) xtr_lst=nco_var_lst_add_crd(in_id,nbr_dmn_fl,xtr_lst,&nbr_xtr);
  
  /* Make sure coordinates associated extracted variables are also on extraction list */
  if(EXTRACT_ASSOCIATED_COORDINATES) xtr_lst=nco_var_lst_ass_crd_add(in_id,xtr_lst,&nbr_xtr);
  
  /* Sort extraction list by variable ID for fastest I/O */
  if(nbr_xtr > 1) xtr_lst=nco_lst_srt_nm_id(xtr_lst,nbr_xtr,False);
  
  /* We now have final list of variables to extract. Phew. */
  
  /* Find coordinate/dimension values associated with user-specified limits */
  for(idx=0;idx<lmt_nbr;idx++) (void)nco_lmt_evl(in_id,lmt[idx],0L,FORTRAN_IDX_CNV);
  
  /* Find dimensions associated with variables to be extracted */
  dmn_lst=nco_dmn_lst_ass_var(in_id,xtr_lst,nbr_xtr,&nbr_dmn_xtr);
  
  /* Fill in dimension structure for all extracted dimensions */
  dim=(dmn_sct **)nco_malloc(nbr_dmn_xtr*sizeof(dmn_sct *));
  for(idx=0;idx<nbr_dmn_xtr;idx++) dim[idx]=nco_dmn_fll(in_id,dmn_lst[idx].id,dmn_lst[idx].nm);
  /* Dimension list no longer needed */
  dmn_lst=nco_nm_id_lst_free(dmn_lst,nbr_dmn_xtr);
  
  /* Merge hyperslab limit information into dimension structures */
  if(lmt_nbr > 0) (void)nco_dmn_lmt_mrg(dim,nbr_dmn_xtr,lmt,lmt_nbr);
  
  /* Duplicate input dimension structures for output dimension structures */
  dmn_out=(dmn_sct **)nco_malloc(nbr_dmn_xtr*sizeof(dmn_sct *));
  for(idx=0;idx<nbr_dmn_xtr;idx++){
    dmn_out[idx]=nco_dmn_dpl(dim[idx]);
    (void)nco_dmn_xrf(dim[idx],dmn_out[idx]); 
  } /* end loop over idx */
  
  /* Create stand-alone limit structure just for record dimension */
  if(prg == ncra || prg == ncrcat){
    if(rec_dmn_id == NCO_REC_DMN_UNDEFINED){
      (void)fprintf(stdout,gettext("%s: ERROR input file %s lacks a record dimension\n"),prg_nm_get(),fl_in);
      if(fl_nbr == 1)(void)fprintf(stdout,gettext("%s: HINT Use ncks instead of %s\n"),prg_nm_get(),prg_nm_get());
      nco_exit(EXIT_FAILURE);
    } /* endif */
    lmt_rec=nco_lmt_sct_mk(in_id,rec_dmn_id,lmt,lmt_nbr,FORTRAN_IDX_CNV);
  } /* endif */
  
  /* Is this an CCM/CCSM/CF-format history tape? */
  CNV_CCM_CCSM_CF=nco_cnv_ccm_ccsm_cf_inq(in_id);
  
  /* Is this an ARM-format data file? */
  CNV_ARM=arm_inq(in_id);
  if(CNV_ARM) base_time_srt=arm_base_time_get(in_id);
  
  /* Fill in variable structure list for all extracted variables */
  var=(var_sct **)nco_malloc(nbr_xtr*sizeof(var_sct *));
  var_out=(var_sct **)nco_malloc(nbr_xtr*sizeof(var_sct *));
  for(idx=0;idx<nbr_xtr;idx++){
    var[idx]=nco_var_fll(in_id,xtr_lst[idx].id,xtr_lst[idx].nm,dim,nbr_dmn_xtr);
    var_out[idx]=nco_var_dpl(var[idx]);
    (void)nco_xrf_var(var[idx],var_out[idx]);
    (void)nco_xrf_dmn(var_out[idx]);
  } /* end loop over idx */
  /* Extraction list no longer needed */
  xtr_lst=nco_nm_id_lst_free(xtr_lst,nbr_xtr);
  
  /* Divide variable lists into lists of fixed variables and variables to be processed */
  (void)nco_var_lst_dvd(var,var_out,nbr_xtr,CNV_CCM_CCSM_CF,nco_pck_plc_nil,nco_pck_map_nil,NULL,0,&var_fix,&var_fix_out,&nbr_var_fix,&var_prc,&var_prc_out,&nbr_var_prc);
  
#ifdef ENABLE_MPI
  if(proc_id == 0){ /* MPI manager code */
#endif /* !ENABLE_MPI */
    /* Open output file */
    fl_out_tmp=nco_fl_out_open(fl_out,FORCE_APPEND,FORCE_OVERWRITE,FMT_64BIT,&out_id);
    
    /* Copy global attributes */
    (void)nco_att_cpy(in_id,out_id,NC_GLOBAL,NC_GLOBAL,True);
    
    /* Catenate time-stamped command line to "history" global attribute */
    if(HISTORY_APPEND) (void)nco_hst_att_cat(out_id,cmd_ln);
    
    /* Add input file list global attribute */
    if(FL_LST_IN_APPEND && HISTORY_APPEND && FL_LST_IN_FROM_STDIN) (void)nco_fl_lst_att_cat(out_id,fl_lst_in,fl_nbr);
    
#ifdef ENABLE_MPI
    /* Initialize MPI task information */
    if(proc_nbr > 0 && HISTORY_APPEND) (void)nco_mpi_att_cat(out_id,proc_nbr);
#endif /* !ENABLE_MPI */
    
    /* Initialize thread information */
    thr_nbr=nco_openmp_ini(thr_nbr);
    if(thr_nbr > 0 && HISTORY_APPEND) (void)nco_thr_att_cat(out_id,thr_nbr);
    
    /* Define dimensions in output file */
    (void)nco_dmn_dfn(fl_out,out_id,dmn_out,nbr_dmn_xtr);
    
    /* Define variables in output file, copy their attributes */
    (void)nco_var_dfn(in_id,fl_out,out_id,var_out,nbr_xtr,(dmn_sct **)NULL,(int)0,nco_pck_plc_nil,nco_pck_map_nil);
    
    /* Turn off default filling behavior to enhance efficiency */
    (void)nco_set_fill(out_id,NC_NOFILL,&fll_md_old);
    
    /* Take output file out of define mode */
    (void)nco_enddef(out_id);
    
#ifdef ENABLE_MPI
  } /* proc_id != 0 */
  
  /* Manager obtains output filename and broadcasts to workers */
  if(proc_id == 0) fl_nm_lng=(int)strlen(fl_out_tmp); 
  MPI_Bcast(&fl_nm_lng,1,MPI_INT,0,MPI_COMM_WORLD);
  if(proc_id != 0) fl_out_tmp=(char *)malloc((fl_nm_lng+1)*sizeof(char));
  MPI_Bcast(fl_out_tmp,fl_nm_lng+1,MPI_CHAR,0,MPI_COMM_WORLD);
  
  /* Original location (breaks SMP mpncea test 3) */
  /* Zero start vectors for all output variables */
  (void)nco_var_srt_zero(var_out,nbr_xtr);
  
  if(proc_id == 0){ /* MPI manager code */
    TOKEN_FREE=False;
#endif /* !ENABLE_MPI */
    /* Copy variable data for non-processed variables */
    (void)nco_var_val_cpy(in_id,out_id,var_fix,nbr_var_fix);
#ifdef ENABLE_MPI
    /* Close output file so workers can open it */
    nco_close(out_id);
    TOKEN_FREE=True;
  } /* proc_id != 0 */
#endif /* !ENABLE_MPI */
  
  /* New location (breaks MPI mpncea test 1) */
  /* Zero start vectors for all output variables */
  /*  (void)nco_var_srt_zero(var_out,nbr_xtr);*/
  
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
      (void)nco_zero_long(var_prc_out[idx]->sz,var_prc_out[idx]->tally);
      var_prc_out[idx]->val.vp=(void *)nco_malloc(var_prc_out[idx]->sz*nco_typ_lng(var_prc_out[idx]->type));
      (void)nco_var_zero(var_prc_out[idx]->type,var_prc_out[idx]->sz,var_prc_out[idx]->val);
    } /* end if */
  } /* end loop over idx */
  
#ifdef ENABLE_MPI
  fl_idx=0;
  rcd=nco_open(fl_in,NC_NOWRITE,&in_id);
  
  /* Variables may have different ID, missing_value, type, in each file */
  for(idx=0;idx<nbr_var_prc;idx++) (void)nco_var_mtd_refresh(in_id,var_prc[idx]); /* Routine contains OpenMP critical regions */
  
  /* Each file can have a different number of records to process */
  if(prg == ncra || prg == ncrcat) (void)nco_lmt_evl(in_id,lmt_rec,idx_rec_out,FORTRAN_IDX_CNV); /* Routine is thread-unsafe */
  
  /* Is this an ARM-format data file? */
  if(CNV_ARM) base_time_crr=arm_base_time_get(in_id); /* Routine is thread-unsafe */
  
  /* Perform various error-checks on input file */
  if(False) (void)nco_fl_cmp_err_chk();
  
  if(prg == ncra || prg == ncrcat){ /* ncea jumps to else branch */
    /* Loop over each record in current file */
    if(lmt_rec->srt > lmt_rec->end) (void)fprintf(stdout,gettext("%s: WARNING %s (input file %d) is superfluous\n"),prg_nm_get(),fl_in,fl_idx);
    idx_rec=lmt_rec->srt;
    if(fl_idx == fl_nbr-1 && idx_rec >= 1L+lmt_rec->end-lmt_rec->srd) LAST_RECORD=True;
    /* Process all variables in first record */
    if(dbg_lvl > 1) (void)fprintf(stderr,gettext("Record %ld of %s is input record %ld\n"),idx_rec,fl_in,idx_rec_out);
    
    if(proc_id == 0){ /* MPI manager code */
      /* Compensate for incrementing on each worker's first message */
      var_wrt_nbr=-proc_nbr+1;
      idx=0;
      /* While variables remain to be processed or written... */
      while(var_wrt_nbr < nbr_var_prc){
	/* Receive message from any worker */
	MPI_Recv(wrk_id_bfr,wrk_id_bfr_lng,MPI_INT,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&mpi_stt);
	/* Obtain MPI message type */
	msg_typ=mpi_stt.MPI_TAG;
	/* Get sender's proc_id */
	wrk_id=wrk_id_bfr[0];
	
	/* Allocate next variable, if any, to worker */
	if(msg_typ == WORK_REQUEST){
	  var_wrt_nbr++; /* [nbr] Number of variables written */
	  /* Worker closed output file before sending WORK_REQUEST */
	  if(prg == ncrcat) TOKEN_FREE=True; /* File written to at this point only for ncrcat */
	  
	  if(idx > nbr_var_prc-1){
	    /* Variable index = -1 indicates NO_MORE_WORK */
	    info_bfr[0]=NO_MORE_WORK; /* [idx] -1 */
	    info_bfr[1]=out_id; /* Output file ID */
	  }else{
	    /* Tell requesting worker to allocate space for next variable */
	    info_bfr[0]=idx; /* [idx] Variable to be processed */
	    info_bfr[1]=out_id; /* Output file ID */
	    info_bfr[2]=var_prc_out[idx]->id; /* [id] Variable ID in output file */
	    /* Point to next variable on list */
	    idx++;
	  } /* endif idx */
	  MPI_Send(info_bfr,info_bfr_lng,MPI_INT,wrk_id,WORK_ALLOC,MPI_COMM_WORLD);
	}else if(msg_typ == TOKEN_REQUEST && prg == ncrcat){ /* msg_typ != WORK_REQUEST */
	  /* Allocate token if free, else ask worker to try later */
	  if(TOKEN_FREE){
	    TOKEN_FREE=False;
	    info_bfr[0]=1; /* Allow */
	  }else{
	    info_bfr[0]=0; /* Wait */
	  } /* !TOKEN_FREE */
	  MPI_Send(info_bfr,info_bfr_lng,MPI_INT,wrk_id,TOKEN_RESULT,MPI_COMM_WORLD);
	} /* msg_typ != TOKEN_REQUEST */
      } /* end while var_wrt_nbr < nbr_var_prc */
    }else{ /* proc_id != 0, end Manager code begin Worker code */
      wrk_id_bfr[0]=proc_id;
      var_wrt_nbr=0;
      while(1){ /* While work remains... */
        /* Send WORK_REQUEST */
        wrk_id_bfr[0]=proc_id;
        MPI_Send(wrk_id_bfr,wrk_id_bfr_lng,MPI_INT,mgr_id,WORK_REQUEST,MPI_COMM_WORLD);
        /* Receive WORK_ALLOC */
        MPI_Recv(info_bfr,info_bfr_lng,MPI_INT,mgr_id,WORK_ALLOC,MPI_COMM_WORLD,&mpi_stt);
        idx=info_bfr[0];
        out_id=info_bfr[1];
        if(idx == NO_MORE_WORK){
	  break;
	}else{ /* idx != NO_MORE_WORK */
	  lcl_idx_lst[lcl_nbr_var]=idx; /* storing the indices for subsequent processing by the worker */
	  lcl_nbr_var++;
          var_prc_out[idx]->id=info_bfr[2];
	  if(dbg_lvl > 2) rcd+=nco_var_prc_crr_prn(idx,var_prc[idx]->nm);
	  if(dbg_lvl > 0) (void)fflush(fp_stderr);
	  /* Update hyperslab start indices to current record for each variable */
	  var_prc[idx]->srt[0]=idx_rec;
	  var_prc[idx]->end[0]=idx_rec;
	  var_prc[idx]->cnt[0]=1L;
	  /* Retrieve variable from disk into memory */
	  (void)nco_var_get(in_id,var_prc[idx]); /* Routine contains OpenMP critical regions */
	  if(prg == ncra){
	    /* Convert char, short, long, int types to doubles before arithmetic */
	    /* Output variable type is "sticky" so only convert on first record */
	    if(idx_rec_out == 0) var_prc_out[idx]=nco_typ_cnv_rth(var_prc_out[idx],nco_op_typ);
	    /* Convert var_prc to type of var_prc_out in case type of variable on disk has changed */
	    var_prc[idx]=nco_var_cnf_typ(var_prc_out[idx]->type,var_prc[idx]);
	    /* Perform arithmetic operations: avg, min, max, ttl, ... */
	    nco_opr_drv(idx_rec_out,nco_op_typ,var_prc[idx],var_prc_out[idx]);
	  } /* !ncra */
	  
	  /* Append current record to output file */
	  if(prg == ncrcat){
	    var_prc_out[idx]->srt[0]=var_prc_out[idx]->end[0]=idx_rec_out;
	    var_prc_out[idx]->cnt[0]=1L;
	    /* Replace this time_offset value with time_offset from initial file base_time */
	    if(CNV_ARM && !strcmp(var_prc[idx]->nm,"time_offset")) var_prc[idx]->val.dp[0]+=(base_time_crr-base_time_srt);
	    
	    /* Obtain token and prepare to write */
	    while(1){ /* Send TOKEN_REQUEST repeatedly until token obtained */
	      wrk_id_bfr[0]=proc_id;
	      MPI_Send(wrk_id_bfr,wrk_id_bfr_lng,MPI_INT,mgr_id,TOKEN_REQUEST,MPI_COMM_WORLD);
	      /* Receive TOKEN_RESULT (1,0)=(ALLOW,WAIT) */
	      MPI_Recv(info_bfr,info_bfr_lng,MPI_INT,mgr_id,TOKEN_RESULT,MPI_COMM_WORLD,&mpi_stt);
	      tkn_rsp=info_bfr[0];
	      /* Wait then re-send request */
	      if(tkn_rsp == TOKEN_WAIT) sleep(sleep_tm); else break;
	    } /* end while loop waiting for write token */
	    
	    /* Worker has token---prepare to write */
	    if(tkn_rsp == TOKEN_ALLOC){
	      rcd=nco_open(fl_out_tmp,NC_WRITE,&out_id);
	      if(var_prc_out[idx]->sz_rec > 1) (void)nco_put_vara(out_id,var_prc_out[idx]->id,var_prc_out[idx]->srt,var_prc_out[idx]->cnt,var_prc[idx]->val.vp,var_prc_out[idx]->type);
	      else (void)nco_put_var1(out_id,var_prc_out[idx]->id,var_prc_out[idx]->srt,var_prc[idx]->val.vp,var_prc_out[idx]->type);
	      /* Close output file and increment written counter */
	      nco_close(out_id);
	      var_wrt_nbr++;
	    } /* endif TOKEN_ALLOC */
	  } /* end if ncrcat */
	  
	  /* Make sure record coordinate, if any, is monotonic */
	  if(prg == ncrcat && var_prc[idx]->is_crd_var) (void)rec_crd_chk(var_prc[idx],fl_in,fl_out,idx_rec,idx_rec_out);
	  /* Convert missing_value, if any, back to unpacked type */
	  if(var_prc[idx]->has_mss_val && var_prc[idx]->type != var_prc[idx]->typ_upk && !LAST_RECORD)
	    var_prc[idx]=nco_cnv_mss_val_typ(var_prc[idx],var_prc[idx]->typ_upk);
	  /* Free current input buffer */
	  var_prc[idx]->val.vp=nco_free(var_prc[idx]->val.vp);
	  
	  if(dbg_lvl > 2) (void)fprintf(stderr,"\n");
	} /* !NO_MORE_WORK */
      } /* while(1) loop requesting work/token in Worker */
      idx_rec_out++; /* [idx] Index of current record in output file (0 is first, ...) */
    } /* endif Worker */
    printf("DEBUG: End of first pass of ncra/ncrcat at node %d\n",proc_id);
    /* MPI_Barrier(MPI_COMM_WORLD); */
    
    /* End of ncra, ncrcat section */
  }else{ /* ncea */
    
    if(proc_id == 0){ /* MPI manager code */
      /* Compensate for incrementing on each worker's first message */
      var_wrt_nbr=-proc_nbr+1;
      idx=0;
      
      /* While variables remain to be processed or written... */
      while(var_wrt_nbr < nbr_var_prc){
	/* Receive message from any worker */
	MPI_Recv(wrk_id_bfr,wrk_id_bfr_lng,MPI_INT,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&mpi_stt);
	/* Obtain MPI message type */
	msg_typ=mpi_stt.MPI_TAG;
	/* Get sender's proc_id */
	wrk_id=wrk_id_bfr[0];
	/* Allocate next variable, if any, to worker */
	if(msg_typ == WORK_REQUEST){
	  var_wrt_nbr++; /* [nbr] Number of variables written */
	  /* Worker closed output file before sending WORK_REQUEST */
	  /* TOKEN_FREE=True; ncea does not do file write here */
	  
	  if(idx > nbr_var_prc-1){
	    /* Variable index = -1 indicates NO_MORE_WORK */
	    info_bfr[0]=NO_MORE_WORK; /* [idx] -1 */
	    info_bfr[1]=out_id; /* Output file ID */
	  }else{
	    /* Tell requesting worker to allocate space for next variable */
	    info_bfr[0]=idx; /* [idx] Variable to be processed */
	    info_bfr[1]=out_id; /* Output file ID */
	    info_bfr[2]=var_prc_out[idx]->id; /* [id] Variable ID in output file */
	    /* Point to next variable on list */
	    idx++;
	  } /* endif idx */
	  MPI_Send(info_bfr,info_bfr_lng,MPI_INT,wrk_id,WORK_ALLOC,MPI_COMM_WORLD);
	} /* msg_typ != WORK_REQUEST */
      } /* end while var_wrt_nbr < nbr_var_prc */
    }else{ /* proc_id != 0, end Manager code begin Worker code */
      while(1){ /* While work remains... */
	/* Send WORK_REQUEST */
	wrk_id_bfr[0]=proc_id;
	MPI_Send(wrk_id_bfr,wrk_id_bfr_lng,MPI_INT,mgr_id,WORK_REQUEST,MPI_COMM_WORLD);
	/* Receive WORK_ALLOC */
	MPI_Recv(info_bfr,info_bfr_lng,MPI_INT,mgr_id,WORK_ALLOC,MPI_COMM_WORLD,&mpi_stt);
	idx=info_bfr[0];
	out_id=info_bfr[1];
	if(idx == NO_MORE_WORK) break;
	else{
	  lcl_idx_lst[lcl_nbr_var]=idx; /* storing the indices for subsequent processing by the worker */
	  lcl_nbr_var++;
	  var_prc_out[idx]->id=info_bfr[2];
	  if(dbg_lvl > 0) rcd+=nco_var_prc_crr_prn(idx,var_prc[idx]->nm);
	  if(dbg_lvl > 0) (void)fflush(fp_stderr);
	  /* Retrieve variable from disk into memory */
	  (void)nco_var_get(in_id,var_prc[idx]); /* Routine contains OpenMP critical regions */
	  
	  /* Convert char, short, long, int types to doubles before arithmetic */
	  /* var_prc[idx]=nco_typ_cnv_rth(var_prc[idx],nco_op_typ); */
	  /* Output variable type is "sticky" so only convert on first record */
	  if(fl_idx == 0) var_prc_out[idx]=nco_typ_cnv_rth(var_prc_out[idx],nco_op_typ);
	  /* Convert var_prc to type of var_prc_out in case type of variable on disk has changed */
	  var_prc[idx]=nco_var_cnf_typ(var_prc_out[idx]->type,var_prc[idx]);
	  /* Perform arithmetic operations: avg, min, max, ttl, ... */ /* Note: fl_idx not idx_rec_out! */
	  nco_opr_drv(fl_idx,nco_op_typ,var_prc[idx],var_prc_out[idx]);
	  
	  /* Free current input buffer */
	  var_prc[idx]->val.vp=nco_free(var_prc[idx]->val.vp);
	} /* !NO_MORE_WORK */
      } /* while(1) loop requesting work/token in Worker */
    } /* endif Worker */
  } /* end else ncea */
  
  if(dbg_lvl > 1) (void)fprintf(stderr,"\n");
  
  /* Close input netCDF file */
  nco_close(in_id);
  
  /* End of pass 1 - fl_idx = 0, rec_idx = srt; now workers will have their list of vars */
  /* GV - actual loop, indices are stored at workers' in the case of MPI */
  printf("DEBUG: proc_id %d is done with 1st pass\n",proc_id);
  
#endif /* !ENABLE_MPI */
  
  /* Loop over input files */
  for(fl_idx=0;fl_idx<fl_nbr;fl_idx++){
#ifdef ENABLE_MPI
    /* MPI_Barrier(MPI_COMM_WORLD); */
#endif /* !ENABLE_MPI */
    /* Parse filename */
    if(fl_idx != 0) fl_in=nco_fl_nm_prs(fl_in,fl_idx,(int *)NULL,fl_lst_in,abb_arg_nbr,fl_lst_abb,fl_pth);
    if(dbg_lvl > 0) (void)fprintf(stderr,gettext("\nInput file %d is %s; "),fl_idx,fl_in);
    /* Make sure file is on local system and is readable or die trying */
    if(fl_idx != 0) fl_in=nco_fl_mk_lcl(fl_in,fl_pth_lcl,&FILE_RETRIEVED_FROM_REMOTE_LOCATION);
    if(dbg_lvl > 0) (void)fprintf(stderr,gettext("local file %s:\n"),fl_in);
    rcd=nco_open(fl_in,NC_NOWRITE,&in_id);
#ifdef ENABLE_MPI
    printf("DEBUG: input file opened in proc_id %d inside the loop\n",proc_id);
#endif /* !ENABLE_MPI */
    
    /* Variables may have different IDs and missing_values in each file */
    for(idx=0;idx<nbr_var_prc;idx++) (void)nco_var_mtd_refresh(in_id,var_prc[idx]); /* Routine contains OpenMP critical regions */
    
    /* Each file can have a different number of records to process */
    if(prg == ncra || prg == ncrcat) (void)nco_lmt_evl(in_id,lmt_rec,idx_rec_out,FORTRAN_IDX_CNV); /* Routine is thread-unsafe */
    
    /* Is this an ARM-format data file? */
    if(CNV_ARM) base_time_crr=arm_base_time_get(in_id); /* Routine is thread-unsafe */
    
    /* Perform various error-checks on input file */
    if(False) (void)nco_fl_cmp_err_chk();
    
    if(prg == ncra || prg == ncrcat){ /* ncea jumps to else branch */
      /* Loop over each record in current file */
      if(lmt_rec->srt > lmt_rec->end) (void)fprintf(stdout,gettext("%s: WARNING %s (input file %d) is superfluous\n"),prg_nm_get(),fl_in,fl_idx);
      for(idx_rec=lmt_rec->srt;idx_rec<=lmt_rec->end;idx_rec+=lmt_rec->srd){
	if(fl_idx == fl_nbr-1 && idx_rec >= 1L+lmt_rec->end-lmt_rec->srd) LAST_RECORD=True;
	
#ifdef ENABLE_MPI
	/* MPI_Barrier(MPI_COMM_WORLD); */
	if(fl_idx == 0 && idx_rec == lmt_rec->srt){
	  continue;
	}else{ /* a loop of idx = stored indices */    
	  if(proc_id == 0){ /* For ncrcat, Manager gives write access for each record in each file */
	    if(prg == ncrcat){ /* Give Write access to write current record */
	      /* var_wrt_nbr=-proc_nbr+1; */
	      var_wrt_nbr=0;
	      while(var_wrt_nbr < nbr_var_prc){ /* Give write access to Workers who have some variables; wrong condn? */
		/* Receive message from any worker */
		MPI_Recv(wrk_id_bfr,wrk_id_bfr_lng,MPI_INT,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&mpi_stt);
		/* Obtain MPI message type */
		msg_typ=mpi_stt.MPI_TAG;
		/* Get sender's proc_id */
		wrk_id=wrk_id_bfr[0];
		if(msg_typ == WORK_DONE) TOKEN_FREE=True;
		if(msg_typ == TOKEN_REQUEST){
		  if(wrk_id == tkn_alloc_id){ /* Prev write completed */
		    TOKEN_FREE=True;
		  } /* wrk_id != tkn_alloc_id */
		  /* Allocate token if free, else ask worker to try later */
		  if(TOKEN_FREE){
		    TOKEN_FREE=False;
		    info_bfr[0]=1; /* Allow */
		    tkn_alloc_id=wrk_id; /* To track who has the token */
		    var_wrt_nbr++;
		  }else{
		    info_bfr[0]=0; /* Wait */
		  } /* !TOKEN_FREE */
		  MPI_Send(info_bfr,info_bfr_lng,MPI_INT,wrk_id,TOKEN_RESULT,MPI_COMM_WORLD);
		} /* msg_typ != TOKEN_REQUEST */
	      } /* End-while token request loop */
	    } /* !ncrcat */
	  }else{ /* proc_id != 0, end Manager code begin Worker code */
            wrk_id_bfr[0]=proc_id;
	    var_wrt_nbr=0;
	    /* if(fl_idx == 0 && idx_rec == lmt_rec->srt) continue;
	       else  a loop of idx = stored indices */    
	    for(jdx=0;jdx<lcl_nbr_var;jdx++){
	      idx=lcl_idx_lst[jdx];
#endif /* !ENABLE_MPI */
	      /* Process all variables in current record */
	      if(dbg_lvl > 1) (void)fprintf(stderr,gettext("Record %ld of %s is input record %ld\n"),idx_rec,fl_in,idx_rec_out);
#if 0
	      /* NB: Immediately preceding MPI for scope confounds Emacs indentation
		 Fake end scope restores correct indentation, simplifies code-checking */
	    } /* fake end for */
#endif /* !0 */
#ifndef ENABLE_MPI
#ifdef _OPENMP
#pragma omp parallel for default(none) private(idx) shared(CNV_ARM,base_time_crr,base_time_srt,dbg_lvl,fl_in,fl_out,fp_stderr,idx_rec,idx_rec_out,in_id,LAST_RECORD,nbr_var_prc,nco_op_typ,out_id,prg,rcd,var_prc,var_prc_out)
#endif /* !_OPENMP */
	    /* UP and SMP codes main loop over variables */
	    for(idx=0;idx<nbr_var_prc;idx++){
#endif /* ENABLE_MPI */
	      if(dbg_lvl > 2) rcd+=nco_var_prc_crr_prn(idx,var_prc[idx]->nm);
	      if(dbg_lvl > 0) (void)fflush(fp_stderr);
	      /* Update hyperslab start indices to current record for each variable */
	      var_prc[idx]->srt[0]=idx_rec;
	      var_prc[idx]->end[0]=idx_rec;
	      var_prc[idx]->cnt[0]=1L;
	      /* Retrieve variable from disk into memory */
	      (void)nco_var_get(in_id,var_prc[idx]); /* Routine contains OpenMP critical regions */
	      if(prg == ncra){
		/* Convert char, short, long, int types to doubles before arithmetic */
		var_prc[idx]=nco_typ_cnv_rth(var_prc[idx],nco_op_typ);
		/* Output variable type is "sticky" so only convert on first record */
		if(idx_rec_out == 0) var_prc_out[idx]=nco_typ_cnv_rth(var_prc_out[idx],nco_op_typ);
		/* Convert var_prc to type of var_prc_out in case type of variable on disk has changed */
		var_prc[idx]=nco_var_cnf_typ(var_prc_out[idx]->type,var_prc[idx]);
		/* Perform arithmetic operations: avg, min, max, ttl, ... */
		nco_opr_drv(idx_rec_out,nco_op_typ,var_prc[idx],var_prc_out[idx]);
	      } /* end if ncra */
	      
	      /* Append current record to output file */
	      if(prg == ncrcat){
		var_prc_out[idx]->srt[0]=var_prc_out[idx]->end[0]=idx_rec_out;
		var_prc_out[idx]->cnt[0]=1L;
		/* Replace this time_offset value with time_offset from initial file base_time */
		if(CNV_ARM && !strcmp(var_prc[idx]->nm,"time_offset")) var_prc[idx]->val.dp[0]+=(base_time_crr-base_time_srt);
		
#ifdef ENABLE_MPI
		/* Obtain token and prepare to write */
		while(1){ /* Send TOKEN_REQUEST repeatedly until token obtained */
		  wrk_id_bfr[0]=proc_id;
		  MPI_Send(wrk_id_bfr,wrk_id_bfr_lng,MPI_INT,mgr_id,TOKEN_REQUEST,MPI_COMM_WORLD);
		  /* Receive TOKEN_RESULT (1,0)=(ALLOW,WAIT) */
		  MPI_Recv(info_bfr,info_bfr_lng,MPI_INT,mgr_id,TOKEN_RESULT,MPI_COMM_WORLD,&mpi_stt);
		  tkn_rsp=info_bfr[0];
		  /* Wait then re-send request */
		  if(tkn_rsp == TOKEN_WAIT) sleep(sleep_tm); else break;
		} /* end while loop waiting for write token */
		
		/* Worker has token---prepare to write */
		if(tkn_rsp == TOKEN_ALLOC){
		  rcd=nco_open(fl_out_tmp,NC_WRITE,&out_id);
#else /* !ENABLE_MPI */
#ifdef _OPENMP
#pragma omp critical
#endif /* _OPENMP */
#endif /* !ENABLE_MPI */
		  if(var_prc_out[idx]->sz_rec > 1) (void)nco_put_vara(out_id,var_prc_out[idx]->id,var_prc_out[idx]->srt,var_prc_out[idx]->cnt,var_prc[idx]->val.vp,var_prc_out[idx]->type);
		  else (void)nco_put_var1(out_id,var_prc_out[idx]->id,var_prc_out[idx]->srt,var_prc[idx]->val.vp,var_prc_out[idx]->type);
#ifdef ENABLE_MPI
		  /* Close output file and increment written counter */
		  nco_close(out_id);
		  var_wrt_nbr++;
		} /* endif TOKEN_ALLOC */
#endif /* !ENABLE_MPI */
	      } /* end if ncrcat */
	      /* Make sure record coordinate, if any, is monotonic */
	      if(prg == ncrcat && var_prc[idx]->is_crd_var) (void)rec_crd_chk(var_prc[idx],fl_in,fl_out,idx_rec,idx_rec_out);
	      /* Convert missing_value, if any, back to disk type */
	      if(var_prc[idx]->has_mss_val && var_prc[idx]->type != var_prc[idx]->typ_upk && !LAST_RECORD)
		var_prc[idx]=nco_cnv_mss_val_typ(var_prc[idx],var_prc[idx]->typ_upk);
	      /* Free current input buffer */
	      var_prc[idx]->val.vp=nco_free(var_prc[idx]->val.vp);
	    } /* end (OpenMP Parallel for) loop over variables */
#ifdef ENABLE_MPI
	    if(prg == ncrcat){
	      /* After writing the last variable of the record, if the worker doesnt return the token, others just wait */
	      wrk_id_bfr[0]=proc_id;
	      MPI_Send(wrk_id_bfr,wrk_id_bfr_lng,MPI_INT,mgr_id,WORK_DONE,MPI_COMM_WORLD);
	    } /* !ncrcat */
#endif /* !ENABLE_MPI */
	    idx_rec_out++; /* [idx] Index of current record in output file (0 is first, ...) */
	    if(dbg_lvl > 2) (void)fprintf(stderr,"\n");
#ifdef ENABLE_MPI
	  } /* !Worker */
	} /* end else ! fl_idx=0,idx_rec=srt */
	/* MPI_Barrier(MPI_COMM_WORLD); */
#endif /* !ENABLE_MPI */
      } /* end loop over idx_rec */
#ifdef ENABLE_MPI
      if(proc_id != 0){ /* Only Worker */
#endif /* !ENABLE_MPI */
	/* Warn if fewer than number of requested records were read and final file has been processed */
	if(lmt_rec->lmt_typ == lmt_dmn_idx && lmt_rec->is_usr_spc_min && lmt_rec->is_usr_spc_max){
	  long rec_nbr_rqs; /* Number of records user requested */
	  rec_nbr_rqs=1L+(lmt_rec->max_idx-lmt_rec->min_idx)/lmt_rec->srd;
	  if(fl_idx == fl_nbr-1 && rec_nbr_rqs != idx_rec_out) (void)fprintf(stdout,gettext("%s: WARNING User requested %li records but only %li were found\n"),prg_nm_get(),rec_nbr_rqs,idx_rec_out);
	} /* end if */
	/* Error if no records were read and final file has been processed */
	if(idx_rec_out <= 0 && fl_idx == fl_nbr-1){
	  (void)fprintf(stdout,gettext("%s: ERROR No records lay within specified hyperslab\n"),prg_nm_get());
	  nco_exit(EXIT_FAILURE);
	} /* end if */
#ifdef ENABLE_MPI
      } /* !Worker */
	/* MPI_Barrier(MPI_COMM_WORLD); */
      printf("DEBUG: proc_id %d at the end of ncra/rcat\n",proc_id);
#endif /* !ENABLE_MPI */
      /* End of ncra, ncrcat section */
    }else{ /* ncea */
#ifdef ENABLE_MPI
      if(proc_id != 0){ /* Only Worker does the ncea processing */
	if(fl_idx == 0){
	  continue;
	}else{ /* a loop of idx = stored indices */
	  for(jdx=0;jdx<lcl_nbr_var;jdx++){
	    idx=lcl_idx_lst[jdx];
#else /* !ENABLE_MPI */
#ifdef _OPENMP
#pragma omp parallel for default(none) private(idx) shared(dbg_lvl,fl_idx,fp_stderr,in_id,nbr_var_prc,nco_op_typ,rcd,var_prc,var_prc_out)
#endif /* !_OPENMP */
	    for(idx=0;idx<nbr_var_prc;idx++){ /* Process all variables in current file */
#endif /* !ENABLE_MPI */	
	      if(dbg_lvl > 0) rcd+=nco_var_prc_crr_prn(idx,var_prc[idx]->nm);
	      if(dbg_lvl > 0) (void)fflush(fp_stderr);
	      /* Retrieve variable from disk into memory */
	      (void)nco_var_get(in_id,var_prc[idx]); /* Routine contains OpenMP critical regions */
	      
	      /* Convert char, short, long, int types to doubles before arithmetic */
	      /* var_prc[idx]=nco_typ_cnv_rth(var_prc[idx],nco_op_typ); */
	      /* Output variable type is "sticky" so only convert on first record */
	      if(fl_idx == 0) var_prc_out[idx]=nco_typ_cnv_rth(var_prc_out[idx],nco_op_typ);
	      /* Convert var_prc to type of var_prc_out in case type of variable on disk has changed */
	      var_prc[idx]=nco_var_cnf_typ(var_prc_out[idx]->type,var_prc[idx]);
	      /* Perform arithmetic operations: avg, min, max, ttl, ... */ /* Note: fl_idx not idx_rec_out! */
	      nco_opr_drv(fl_idx,nco_op_typ,var_prc[idx],var_prc_out[idx]);
	      
	      /* Free current input buffer */
	      var_prc[idx]->val.vp=nco_free(var_prc[idx]->val.vp);
	    } /* end (OpenMP parallel for) loop over idx */
#ifdef ENABLE_MPI
	  } /* end else !fl_idx=0 */
	} /* !Worker */
#endif /* !ENABLE_MPI */
      } /* end else ncea */
      
      if(dbg_lvl > 1) (void)fprintf(stderr,"\n");
      
      /* Close input netCDF file */
      nco_close(in_id);
      
      /* MPI_Barrier(MPI_COMM_WORLD); */
      /* Dispose local copy of file */
      if(FILE_RETRIEVED_FROM_REMOTE_LOCATION && REMOVE_REMOTE_FILES_AFTER_PROCESSING) (void)nco_fl_rm(fl_in);
    } /* end loop over fl_idx */
#ifdef ENABLE_MPI
    printf("DEBUG: proc_id %d is out of file idx loop\n",proc_id); 
    /* MPI_Barrier(MPI_COMM_WORLD); */
#endif /* !ENABLE_MPI */
    /* Normalize, multiply, etc where necessary */
    if(prg == ncra || prg == ncea){
#ifdef ENABLE_MPI
      if(proc_id != 0){ /* Only workers have indices of variables to process */
	for(jdx=0;jdx<lcl_nbr_var;jdx++){
	  idx=lcl_idx_lst[jdx];
#if 0
	  /* NB: Immediately preceding MPI if/for scopes confound Emacs indentation
	     Fake end scopes restore correct indentation, simplify code-checking */
	} /* fake end for */
      } /* fake end if */
#endif /* !0 */
#else /* !ENABLE_MPI */
#ifdef _OPENMP
#pragma omp parallel for default(none) private(idx) shared(nbr_var_prc,nco_op_typ,var_prc,var_prc_out)
#endif /* !_OPENMP */
      for(idx=0;idx<nbr_var_prc;idx++){
#endif /* !ENABLE_MPI */
	switch(nco_op_typ){
	case nco_op_avg: /* Normalize sum by tally to create mean */
	case nco_op_sqrt: /* Normalize sum by tally to create mean */
	case nco_op_sqravg: /* Normalize sum by tally to create mean */
	case nco_op_rms: /* Normalize sum of squares by tally to create mean square */
	case nco_op_avgsqr: /* Normalize sum of squares by tally to create mean square */
	  (void)nco_var_nrm(var_prc_out[idx]->type,var_prc_out[idx]->sz,var_prc[idx]->has_mss_val,var_prc[idx]->mss_val,var_prc[idx]->tally,var_prc_out[idx]->val);
	  break;
	case nco_op_rmssdn: /* Normalize sum of squares by tally-1 to create mean square for sdn */
	  (void)nco_var_nrm_sdn(var_prc_out[idx]->type,var_prc_out[idx]->sz,var_prc[idx]->has_mss_val,var_prc[idx]->mss_val,var_prc[idx]->tally,var_prc_out[idx]->val);
	  break;
	case nco_op_min: /* Minimum is already in buffer, do nothing */
	case nco_op_max: /* Maximum is already in buffer, do nothing */
	case nco_op_ttl: /* Total is already in buffer, do nothing */
	default:
	  break;
	} /* end switch */
	  /* Some operations require additional processing */
	switch(nco_op_typ){
	case nco_op_rms: /* Take root of mean of sum of squares to create root mean square */
	case nco_op_rmssdn: /* Take root of sdn mean of sum of squares to create root mean square for sdn */
	case nco_op_sqrt: /* Take root of mean to create root mean */
	  (void)nco_var_sqrt(var_prc_out[idx]->type,var_prc_out[idx]->sz,var_prc[idx]->has_mss_val,var_prc[idx]->mss_val,var_prc[idx]->tally,var_prc_out[idx]->val,var_prc_out[idx]->val);
	  break;
	case nco_op_sqravg: /* Square mean to create square of the mean (for sdn) */
	  (void)nco_var_mlt(var_prc_out[idx]->type,var_prc_out[idx]->sz,var_prc_out[idx]->has_mss_val,var_prc_out[idx]->mss_val,var_prc_out[idx]->val,var_prc_out[idx]->val);
	  break;
	default:
	  break;
	} /* end switch */
	var_prc_out[idx]->tally=var_prc[idx]->tally=(long *)nco_free(var_prc[idx]->tally);
      } /* end (OpenMP parallel for) loop over variables */
#ifdef ENABLE_MPI
      printf("DEBUG: End of Normzn at proc_id %d\n",proc_id);
    } /* proc_id == 0 */
    printf("DEBUG: Mgr shud prnt this too, proc_id %d\n",proc_id);
#endif /* !ENABLE_MPI */
  } /* !ncra/ncea */
#ifdef ENABLE_MPI
  printf("DEBUG: After all processing; Before barrier, proc_id %d\n",proc_id);
  /* MPI_Barrier(MPI_COMM_WORLD); */
  if(proc_id == 0){ /* Only Manager */
    rcd=nco_open(fl_out_tmp,NC_WRITE,&out_id);
    printf("proc_id %d opened out file\n",proc_id);
#endif /* !ENABLE_MPI */
    /* Manually fix YYMMDD date which was mangled by averaging */
    if(CNV_CCM_CCSM_CF && prg == ncra) (void)nco_cnv_ccm_ccsm_cf_date(out_id,var_out,nbr_xtr);
    
    /* Add time variable to output file */
    if(CNV_ARM && prg == ncrcat) (void)nco_arm_time_install(out_id,base_time_srt);
#ifdef ENABLE_MPI
    nco_close(out_id); 
    printf("DEBUG: Mgr proc_id %d closed out file %d after fixing date, time \n", proc_id, out_id);
    MPI_Send(info_bfr,info_bfr_lng,MPI_INT,proc_id+1,TOKEN_RESULT,MPI_COMM_WORLD);
    printf("DEBUG: Mgr sent token to worker 1 to write\n");
  }else{ /* Workers */
    printf("DEBUG: proc_id %d waiting for msg from Mgr for final write\n",proc_id);
    MPI_Recv(info_bfr,info_bfr_lng,MPI_INT,proc_id-1,TOKEN_RESULT,MPI_COMM_WORLD,&mpi_stt);
    printf("DEBUG: proc_id %d got token to write to %d\n",proc_id, out_id);
    rcd=nco_open(fl_out_tmp,NC_WRITE,&out_id);
    printf("DEBUG: proc_id %d opened output file write\n",proc_id);
    /* Copy averages to output file and free averaging buffers */
    if(prg == ncra || prg == ncea){
      for(jdx=0;jdx<lcl_nbr_var;jdx++){
	idx=lcl_idx_lst[jdx];
	/* Revert any arithmetic promotion but leave unpacking (for now) */
	/*	printf("DEBUG: Before nco_var_cnf_typ proc_id %d var val %f\n",proc_id,var_prc_out[idx]->val.lp[0]); */
	var_prc_out[idx]=nco_var_cnf_typ(var_prc_out[idx]->typ_upk,var_prc_out[idx]);
	/*        printf("DEBUG: After nco_var_cnf_typ proc_id %d var val %f\n",proc_id,var_prc_out[idx]->val.lp[0]); */
	/* Packing/Unpacking */
	if(nco_pck_plc == nco_pck_plc_all_new_att) var_prc_out[idx]=nco_put_var_pck(out_id,var_prc_out[idx],nco_pck_plc);
	printf("DEBUG: proc_id %d to write var %s with idx %d val %ld\n",proc_id,var_prc_out[idx]->nm,idx,var_prc_out[idx]->val.lp[0]);
	if(var_prc_out[idx]->nbr_dim == 0){
	  (void)nco_put_var1(out_id,var_prc_out[idx]->id,var_prc_out[idx]->srt,var_prc_out[idx]->val.vp,var_prc_out[idx]->type);
	}else{ /* end if variable is a scalar */
	  /* Size of record dimension is 1 in output file */
	  if(prg == ncra) var_prc_out[idx]->cnt[0]=1L;
	  (void)nco_put_vara(out_id,var_prc_out[idx]->id,var_prc_out[idx]->srt,var_prc_out[idx]->cnt,var_prc_out[idx]->val.vp,var_prc_out[idx]->type);
	} /* end if variable is an array */
	var_prc_out[idx]->val.vp=nco_free(var_prc_out[idx]->val.vp);
      } /* end loop over jdx */
    } /* end if */
    
      /* Close output file */
    nco_close(out_id); 
    printf("DEBUG: proc_id %d closed out file after writing\n",proc_id);
    /* Send Token to Manager */
    if(proc_id == proc_nbr-1) MPI_Send(info_bfr,info_bfr_lng,MPI_INT,mgr_id,TOKEN_RESULT,MPI_COMM_WORLD); else MPI_Send(info_bfr,info_bfr_lng,MPI_INT,proc_id+1,TOKEN_RESULT,MPI_COMM_WORLD);
  } /* !Workers */
  if(proc_id == 0){ /* Only Manager */
    MPI_Recv(info_bfr,info_bfr_lng,MPI_INT,proc_nbr-1,TOKEN_RESULT,MPI_COMM_WORLD,&mpi_stt);
    (void)nco_fl_mv(fl_out_tmp,fl_out);
  } /* !Manager */
  
    /* MPI_Barrier(MPI_COMM_WORLD); */
  MPI_Finalize();
  
#else /* !ENABLE_MPI */
  /* Copy averages to output file and free averaging buffers */
  if(prg == ncra || prg == ncea){
    for(idx=0;idx<nbr_var_prc;idx++){
      /* Revert any arithmetic promotion but leave unpacking (for now) */
      var_prc_out[idx]=nco_var_cnf_typ(var_prc_out[idx]->typ_upk,var_prc_out[idx]);
      /* Packing/Unpacking */
      if(nco_pck_plc == nco_pck_plc_all_new_att) var_prc_out[idx]=nco_put_var_pck(out_id,var_prc_out[idx],nco_pck_plc);
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
  (void)nco_fl_out_cls(fl_out,fl_out_tmp,out_id);
  
#endif /* !ENABLE_MPI */
  
  /* ncra-specific memory cleanup */
  if(prg == ncra || prg == ncrcat) lmt_rec=nco_lmt_free(lmt_rec);
  
  /* NCO-generic clean-up */
  /* Free individual strings */
  if(cmd_ln != NULL) cmd_ln=(char *)nco_free(cmd_ln);
  if(fl_in != NULL) fl_in=(char *)nco_free(fl_in);
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
  if(nbr_dmn_xtr > 0) dim=nco_dmn_lst_free(dim,nbr_dmn_xtr);
  if(nbr_dmn_xtr > 0) dmn_out=nco_dmn_lst_free(dmn_out,nbr_dmn_xtr);
  /* Free variable lists */
  if(nbr_xtr > 0) var=nco_var_lst_free(var,nbr_xtr);
  if(nbr_xtr > 0) var_out=nco_var_lst_free(var_out,nbr_xtr);
  var_prc=(var_sct **)nco_free(var_prc);
  var_prc_out=(var_sct **)nco_free(var_prc_out);
  var_fix=(var_sct **)nco_free(var_fix);
  var_fix_out=(var_sct **)nco_free(var_fix_out);
  
  nco_exit_gracefully();
  return EXIT_SUCCESS;
} /* end main() */



