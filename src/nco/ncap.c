/* $Header: /data/zender/nco_20150216/nco/src/nco/ncap.c,v 1.85 2002-08-21 11:47:41 zender Exp $ */

/* ncap -- netCDF arithmetic processor */

/* Purpose: Compute user-defined derived fields using forward algebraic notation applied to netCDF files */

/* Copyright (C) 1995--2002 Charlie Zender
   This software is distributed under the terms of the GNU General Public License
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

/* Usage:
   ncap -O -D 1 -S ${HOME}/nco/data/ncap.in ${HOME}/nco/data/in.nc ${HOME}/nco/data/foo.nc
   ncap -O -D 1 -s a=b+c -s "b=c-d/2." -S ncap.in in.nc foo.nc
   ncap -O -D 1 -s two=one+two in.nc foo.nc
   scp ~/nco/src/nco/ncap.c dataproc.ucar.edu:nco/src/nco/ncap.c */

/* Standard C headers */
#include <assert.h>  /* assert() debugging macro */
#include <math.h> /* sin cos cos sin 3.14159 */
#include <stdio.h> /* stderr, FILE, NULL, etc. */
#include <stdlib.h> /* atof, atoi, malloc, getopt */
#include <string.h> /* strcmp. . . */
#include <sys/stat.h> /* stat() */
#include <time.h> /* machine time */
#include <unistd.h> /* all sorts of POSIX stuff */

#if ( defined LINUX || defined LINUXALPHA )
#include <getopt.h> /* GNU getopt() is standard on Linux */
#else /* not LINUX || SUN */
#if ( !defined AIX ) && ( !defined CRAY ) && ( !defined NECSX ) && ( !defined SUNMP ) && ( !defined SUN4SOL2 ) /* getopt() is in <unistd.h> or <stdlib.h> */
#include "getopt.h" /* GNU getopt() */
#endif /* not AIX */
#endif /* not LINUX */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions */
#include "nco_netcdf.h"  /* NCO wrappers for libnetcdf.a */

/* Personal headers */
/* #define MAIN_PROGRAM_FILE MUST precede #include nco.h */
#define MAIN_PROGRAM_FILE
#include "ncap.h" /* ncap-specific definitions */
#include "libnco.h" /* netCDF operator library */

/* Global variables */
size_t ncl_dpt_crr=0UL; /* [nbr] Depth of current #include file (incremented in ncap.l) */
size_t *ln_nbr_crr; /* [cnt] Line number (incremented in ncap.l) */
char **fl_spt_glb; /* [fl] Script file */

int 
main(int argc,char **argv)
{
  extern int yyparse(void *); /* Prototype here as in bison.simple to avoid compiler warning */
/* Following declaration gets rid of implicit declaration compiler warning
   It is a condensation of the lexer declaration from lex.yy.c:
   YY_BUFFER_STATE yy_scan_string YY_PROTO(( yyconst char *yy_str )); */
  extern int yy_scan_string(const char *);
  
  extern FILE *yyin; /* [fl] Input script file */
  
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
  bool PROCESS_ALL_VARS=True;     /* option v */  
  char **var_lst_in=NULL_CEWI;
  char **fl_lst_abb=NULL; /* Option n */
  char **fl_lst_in;
  char *fl_spt_usr=NULL; /* Option s */
  char *fl_in=NULL;
  char *fl_pth_lcl=NULL; /* Option l */
  char *lmt_arg[NC_MAX_DIMS];
#define NCAP_SPT_NBR_MAX 100
  char *spt_arg[NCAP_SPT_NBR_MAX]; /* fxm: Arbitrary size, should be dynamic */
  char *spt_arg_cat=NULL; /* [sng] User-specified script */
  char *opt_sng;
  char *fl_out;
  char *fl_out_tmp;
  char *fl_pth=NULL; /* Option p */
  char *time_bfr_srt;
  char *cmd_ln;
  char CVS_Id[]="$Id: ncap.c,v 1.85 2002-08-21 11:47:41 zender Exp $"; 
  char CVS_Revision[]="$Revision: 1.85 $";
  
  dmn_sct **dmn=NULL_CEWI;
  dmn_sct **dmn_out;
  
  extern char *optarg;
  extern int optind;
  
  /* Math float prototypes required by AIX, Solaris, but not by Linux, IRIX */
  /* Basic math: acos, asin, atan, cos, exp, log, log10, sin, sqrt, tan */
  extern float acosf(float);
  extern float asinf(float);
  extern float atanf(float);
  extern float cosf(float);
  extern float erff(float);
  extern float erfcf(float);
  extern float expf(float);
  extern float gammaf(float);
  extern float logf(float);
  extern float log10f(float);
  extern float sinf(float);
  extern float sqrtf(float);
  extern float tanf(float);

  /* Hyperbolic trigonometric: acosh, asinh, atanh, cosh, sinh, tanh */
  extern float acoshf(float);
  extern float asinhf(float);
  extern float atanhf(float);
  extern float coshf(float);
  extern float sinhf(float);
  extern float tanhf(float);

  /* Basic Rounding: ceil, floor */
  extern float ceilf(float);
  extern float floorf(float);

  /* Advanced Rounding: nearbyint, rint, round, trunc */
  extern float nearbyintf(float);
  extern float rintf(float);
  extern float roundf(float);
  extern float truncf(float);

  int fll_md_old; /* [enm] Old fill mode */
  int idx;
  int in_id;  
  int out_id;  
  int nbr_abb_arg=0;
  int nbr_dmn_fl;
  int lmt_nbr=0; /* Option d. NB: lmt_nbr gets incremented */
  int nbr_spt=0; /* Option s. NB: nbr_spt gets incremented */
  int nbr_var_fl;
  int nbr_var_fix; /* nbr_var_fix gets incremented */
  int nbr_var_prc; /* nbr_var_prc gets incremented */
  int nbr_xtr=0; /* nbr_xtr will not otherwise be set for -c with no -v */
  int nbr_xtr_2=0;
  int nbr_dmn_xtr=int_CEWI;
  int nbr_fl=0;
  int nbr_lst_a=0;
  int nbr_lst_b=0;
  int nbr_lst_c=0;
  int nbr_lst_d=0;
  int opt;
  int rec_dmn_id=NCO_REC_DMN_UNDEFINED;
  int rcd=NC_NOERR; /* [rcd] Return code */
  int sng_lng;
  int var_id;
  int spt_arg_lng=int_CEWI;
  int nbr_att=0; /* [nbr] Number of attributes in script */
  
  sym_sct **sym_tbl; /* [fnc] Symbol table for functions */
  int sym_tbl_nbr; /* [nbr] Size of symbol table */
  int sym_idx=0; /* [idx] Counter for symbols */
  
  lmt_sct *lmt=NULL_CEWI;
  
  nm_id_sct *dmn_lst=NULL;
  nm_id_sct *xtr_lst=NULL;
  nm_id_sct *xtr_lst_2=NULL;
  
  nm_id_sct *xtr_lst_a=NULL;
  nm_id_sct *xtr_lst_b=NULL;
  nm_id_sct *xtr_lst_c=NULL;
  nm_id_sct *xtr_lst_d=NULL;
  time_t clock;
  
  var_sct **var;
  var_sct **var_fix;
  var_sct **var_fix_out;
  var_sct **var_out;
  var_sct **var_prc;
  var_sct **var_prc_out;
  
#define NCAP_ATT_LST_NBR_MAX 500 /* fxm: Arbitrary size, should be dynamic */
  aed_sct *att_lst[NCAP_ATT_LST_NBR_MAX]; /* Structure filled in by yyparse(), contains attributes to write to disk */
  prs_sct prs_arg; /* [sct] Global information required in parser routines */
  
  /* Start clock and save command line */ 
  cmd_ln=nco_cmd_ln_sng(argc,argv);
  clock=time((time_t *)NULL);
  time_bfr_srt=ctime(&clock); time_bfr_srt=time_bfr_srt; /* Avoid compiler warning until variable is used for something */
  
  /* Get program name and set program enum (e.g., prg=ncra) */
  prg_nm=prg_prs(argv[0],&prg);

  /* Parse command line arguments */
  opt_sng="ACcD:d:Fhl:n:Op:Rrs:S:vx";
  while((opt=getopt(argc,argv,opt_sng)) != EOF){
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
    case 'n': /* NINTAP-style abbreviation of files to process */
      /* Currently not used in ncap but should be to allow processing multiple input files by same script */
      (void)fprintf(stderr,"%s: ERROR %s does not currently implement -n option\n",prg_nm_get(),prg_nm_get());
      fl_lst_abb=lst_prs(optarg,",",&nbr_abb_arg);
      if(nbr_abb_arg < 1 || nbr_abb_arg > 3){
	(void)fprintf(stdout,"%s: ERROR Incorrect abbreviation for file list\n",prg_nm);
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
    case 'R': /* Toggle removal of remotely-retrieved-files. Default is True. */
      REMOVE_REMOTE_FILES_AFTER_PROCESSING=!REMOVE_REMOTE_FILES_AFTER_PROCESSING;
      break;
    case 'r': /* Print CVS program information and copyright notice */
      (void)copyright_prn(CVS_Id,CVS_Revision);
      (void)nco_lib_vrs_prn();
      nco_exit(EXIT_SUCCESS);
      break;
    case 's': /* Copy command script for later processing */
      spt_arg[nbr_spt++]=(char *)strdup(optarg);
      if(nbr_spt == NCAP_SPT_NBR_MAX-1) (void)fprintf(stderr,"%s: WARNING No more than %d script arguments allowed. TODO #24\n",prg_nm_get(),NCAP_SPT_NBR_MAX);
      break;
    case 'S': /* Read command script from file rather than from command line */
      fl_spt_usr=(char *)strdup(optarg);
      break;
    case 'v': /* Variables to extract/exclude */
      PROCESS_ALL_VARS=False;
      nbr_xtr=0;
      /* var_lst_in=lst_prs(optarg,",",&nbr_xtr); */
      break;
    case 'x': /* Exclude rather than extract variables specified with -v */
      EXCLUDE_INPUT_LIST=True;
      (void)fprintf(stderr,"%s: ERROR %s does not currently implement -x option\n",prg_nm_get(),prg_nm_get());
      nco_exit(EXIT_FAILURE);
      break;
    default: /* Print proper usage */
      (void)nco_usg_prn();
      nco_exit(EXIT_FAILURE);
      break;
    } /* end switch */
  } /* end while loop */
  
  /* Append ";\n" to command-script arguments, then concatenate them */
  for(idx=0;idx<nbr_spt;idx++){
    sng_lng=strlen(spt_arg[idx]);
    if(idx == 0){
      spt_arg_cat=(char *)nco_malloc(sng_lng+3);
      strcpy(spt_arg_cat,spt_arg[idx]);
      strcat(spt_arg_cat,";\n");
      spt_arg_lng=sng_lng+3;
    }else{
      spt_arg_lng+=sng_lng+2;
      spt_arg_cat=(char *)nco_realloc(spt_arg_cat,spt_arg_lng);
      strcat(spt_arg_cat,spt_arg[idx]);
      strcat(spt_arg_cat,";\n");
    } /* end else */
  } /* end if */    

  /* Create function table */
  sym_tbl_nbr= /* fxm: Make this dynamic */
   +10 /* Basic math: acos, asin, atan, cos, exp, log, log10, sin, sqrt, tan */
    +6 /* Hyperbolic trigonometric: acosh, asinh, atanh, cosh, sinh, tanh */
    +2 /* Basic Rounding: ceil, floor */
    +4 /* Advanced Rounding: nearbyint, rint, round, trunc */
    +3 /* Advanced math: erf, erfc, gamma */
    ;
  sym_tbl=(sym_sct **)nco_malloc(sizeof(sym_sct *)*sym_tbl_nbr);

  /* Basic math: acos, asin, atan, cos, exp, log, log10, sin, sqrt, tan */
  sym_tbl[sym_idx++]=ncap_sym_init("acos",acos,acosf);  
  sym_tbl[sym_idx++]=ncap_sym_init("asin",asin,asinf);
  sym_tbl[sym_idx++]=ncap_sym_init("atan",atan,atanf);
  sym_tbl[sym_idx++]=ncap_sym_init("cos",cos,cosf);  
  sym_tbl[sym_idx++]=ncap_sym_init("exp",exp,expf);
  sym_tbl[sym_idx++]=ncap_sym_init("log",log,logf);
  sym_tbl[sym_idx++]=ncap_sym_init("log10",log10,log10f);
  sym_tbl[sym_idx++]=ncap_sym_init("sin",sin,sinf);
  sym_tbl[sym_idx++]=ncap_sym_init("sqrt",sqrt,sqrtf);
  sym_tbl[sym_idx++]=ncap_sym_init("tan",tan,tanf);

  /* Basic Rounding: ceil, floor */
  sym_tbl[sym_idx++]=ncap_sym_init("ceil",ceil,ceilf); /* Round up to nearest integer */
  sym_tbl[sym_idx++]=ncap_sym_init("floor",floor,floorf); /* Round down to nearest integer */

#if (defined AIX) || (defined SGI6) || (defined SGI64) || (defined SGIMP64) || (defined WIN32)
  /* 20020122 and 20020422: AIX, SGI*, WIN32 do not define erff(), erfcf(), gammaf() */
  sym_tbl_nbr-=3; /* Advanced math: erf, erfc, gamma */
  /* sym_tbl_nbr-=2; *//* Basic Rounding: ceil, floor */
  /* 20020703: AIX, SGI*, WIN32 do not define rintf */
  sym_tbl_nbr-=4; /* Advanced Rounding: nearbyint, rintf, round, trunc */
  /* 20020703: AIX, SGI*, WIN32 do not define acoshf, asinhf, atanhf */
  sym_tbl_nbr-=6; /* Hyperbolic trigonometric: acosh, asinh, atanh, cosh, sinh, tanh */
#else /* not AIX || SGI* || WIN32 */
  /* LINUX* and SUN* provide these functions */
  /* Advanced math: erf, erfc, gamma */
  sym_tbl[sym_idx++]=ncap_sym_init("erf",erf,erff);
  sym_tbl[sym_idx++]=ncap_sym_init("erfc",erfc,erfcf);
  sym_tbl[sym_idx++]=ncap_sym_init("gamma",gamma,gammaf);

  /* Hyperbolic trigonometric: acosh, asinh, atanh, cosh, sinh, tanh */
  sym_tbl[sym_idx++]=ncap_sym_init("acosh",acosh,acoshf);
  sym_tbl[sym_idx++]=ncap_sym_init("asinh",asinh,asinhf);
  sym_tbl[sym_idx++]=ncap_sym_init("atanh",atanh,atanhf);
  sym_tbl[sym_idx++]=ncap_sym_init("cosh",cosh,coshf);
  sym_tbl[sym_idx++]=ncap_sym_init("sinh",sinh,sinhf);
  sym_tbl[sym_idx++]=ncap_sym_init("tanh",tanh,tanhf);

  /* Only LINUX* supplies all of these and I do not care about them enough
     to activate them on LINUX* but not on SUN* */
  sym_tbl_nbr-=4; /* Advanced Rounding: nearbyint, rint, round, trunc */
  /* Advanced Rounding: nearbyint, round, trunc */
  /* sym_tbl[sym_idx++]=ncap_sym_init("nearbyint",nearbyint,nearbyintf); *//* Round to integer value in floating point format using current rounding direction, do not raise inexact exceptions */
  /* sym_tbl[sym_idx++]=ncap_sym_init("round",round,roundf); *//* Round to nearest integer away from zero */
  /* sym_tbl[sym_idx++]=ncap_sym_init("trunc",trunc,truncf); *//* Round to nearest integer not larger in absolute value */
  /* sym_tbl[sym_idx++]=ncap_sym_init("rint",rint,rintf); *//* Round to integer value in floating point format using current rounding direction, raise inexact exceptions */
#endif /* not AIX || SGI* || WIN32 */
  assert(sym_idx == sym_tbl_nbr);
 
  /* Process positional arguments and fill in filenames */
  fl_lst_in=nco_fl_lst_mk(argv,argc,optind,&nbr_fl,&fl_out);
  
  /* Make uniform list of user-specified dimension limits */
  if(lmt_nbr > 0) lmt=nco_lmt_prs(lmt_nbr,lmt_arg);
  
  /* Parse filename */
  fl_in=nco_fl_nm_prs(fl_in,0,&nbr_fl,fl_lst_in,nbr_abb_arg,fl_lst_abb,fl_pth);
  /* Make sure file is on local system and is readable or die trying */
  fl_in=nco_fl_mk_lcl(fl_in,fl_pth_lcl,&FILE_RETRIEVED_FROM_REMOTE_LOCATION);
  /* Open file for reading */
  rcd=nco_open(fl_in,NC_NOWRITE,&in_id);
  
  /* Pass information parser may need in prs_arg rather than in global variables */
  prs_arg.fl_in=fl_in; /* [fl] Input data file */
  prs_arg.in_id=in_id; /* [id] Input data file ID */
  prs_arg.fl_out=NULL; /* [sng] Output data file */
  prs_arg.out_id=-1; /* [id] Output data file ID */
  prs_arg.att_lst=att_lst; /* [sct] Attributes in script */
  prs_arg.nbr_att=&nbr_att; /* [nbr] Number of attributes in script */
  prs_arg.dmn=dmn; /* [dmn] List of extracted dimensions */
  prs_arg.nbr_dmn_xtr=nbr_dmn_xtr; /* [nbr] Number of extracted dimensions */
  prs_arg.sym_tbl=sym_tbl; /* [fnc] Symbol table for functions */
  prs_arg.sym_tbl_nbr=sym_tbl_nbr; /* [nbr] Number of functions in table */
  prs_arg.ntl_scn=True; /* [flg] Initial scan of script */
  prs_arg.var_LHS=NULL; /* [var] LHS cast variable */
  prs_arg.nco_op_typ=nco_op_nil; /* [enm] Operation type */
  
  /* Initialize line counter */
  ln_nbr_crr=(size_t *)nco_realloc(ln_nbr_crr,ncl_dpt_crr+1UL); 
  ln_nbr_crr[ncl_dpt_crr]=1UL; /* [cnt] Line number incremented in ncap.l */
  /* Copy script file name to global variable */
  fl_spt_glb=(char **)nco_realloc(fl_spt_glb,ncl_dpt_crr+1UL); 
  fl_spt_glb[ncl_dpt_crr]=fl_spt_usr;
  /* Perform initial scan of input script to create three lists of variables:
     lst_a: RHS variables present in input file
     lst_b: LHS variables present in input file
     lst_c: Variables of attributes on LHS which are present in input file 
     lst_d: Dimensions defined in LHS subscripts */
  (void)ncap_ntl_scn(&prs_arg,spt_arg_cat,&xtr_lst_a,&nbr_lst_a,&xtr_lst_b,&nbr_lst_b,&xtr_lst_c,&nbr_lst_c,&xtr_lst_d,&nbr_lst_d);
  
  /* Get number of variables, dimensions, and record dimension ID of input file */
  rcd=nco_inq(in_id,&nbr_dmn_fl,&nbr_var_fl,(int *)NULL,&rec_dmn_id);

  if(PROCESS_ALL_VARS){
    /* Form initial extraction list from user input */
    xtr_lst=nco_var_lst_mk(in_id,nbr_var_fl,var_lst_in,PROCESS_ALL_COORDINATES,&nbr_xtr);
    if(nbr_lst_b > 0) xtr_lst=nco_var_lst_sub(xtr_lst,&nbr_xtr,xtr_lst_b,nbr_lst_b);
    /* Copy list for later */
    xtr_lst_2=nco_var_lst_copy(xtr_lst,nbr_xtr);
    nbr_xtr_2=nbr_xtr;
  } /* end if PROCESS_ALL_VARS */
  
  if(!PROCESS_ALL_VARS){
    /* Create two lists of variables
       xtr_lst: Used to find dimensions
       xtr_lst_2: Actual variable extraction list     
       
       xtr_lst=list_a + list_c + coordinate variables 
       xtr_lst_2=list_c + coordinate variables - list_b */
    
    /* Add list c to extraction list */
    if(nbr_lst_c > 0) xtr_lst=nco_var_lst_add(xtr_lst,&nbr_xtr,xtr_lst_c,nbr_lst_c);
    
    /* Add list a to extraction list */
    if(nbr_lst_a > 0) xtr_lst=nco_var_lst_add(xtr_lst,&nbr_xtr,xtr_lst_a,nbr_lst_a);
    
    /* Add all coordinate variables to extraction list */
    if(PROCESS_ALL_COORDINATES) xtr_lst=nco_var_lst_add_crd(in_id,nbr_var_fl,nbr_dmn_fl,xtr_lst,&nbr_xtr);
    
    /* Make sure coordinates associated extracted variables are also on extraction list */
    if(PROCESS_ASSOCIATED_COORDINATES) xtr_lst=nco_var_lst_ass_crd_add(in_id,xtr_lst,&nbr_xtr);
    
    /* Create list of coordinate variables */
    if(nbr_xtr > 0 && (PROCESS_ALL_COORDINATES || PROCESS_ASSOCIATED_COORDINATES)){
      nbr_xtr_2=nbr_xtr;
      xtr_lst_2=nco_var_lst_copy(xtr_lst,nbr_xtr);
      
      /* Add dimensions defined in LHS subscripts */
      if(nbr_lst_d > 0) xtr_lst_2=nco_var_lst_add(xtr_lst_2,&nbr_xtr_2,xtr_lst_d,nbr_lst_d); 
      
      /* Creat list of coordinates only */
      xtr_lst_2=nco_var_lst_crd_make(in_id,xtr_lst_2,&nbr_xtr_2);
    } /* endif */
    
    /* Add list_c to new list */
    if(nbr_lst_c > 0) xtr_lst_2=nco_var_lst_add(xtr_lst_2,&nbr_xtr_2,xtr_lst_c,nbr_lst_c);
    
    /* Subtract list_b from this list */   
    if(nbr_lst_b > 0) xtr_lst_2=nco_var_lst_sub(xtr_lst_2,&nbr_xtr_2,xtr_lst_b,nbr_lst_b);
  } /* end if PROCESS_ALL_VARS */
  
  /* Sort extraction list by variable ID for fastest I/O */
  if(nbr_xtr > 1) xtr_lst=nco_lst_srt(xtr_lst,nbr_xtr,False);
  if(nbr_xtr_2 > 1) xtr_lst_2=nco_lst_srt(xtr_lst_2,nbr_xtr_2,False);
  
  /* Find coordinate/dimension values associated with user-specified limits */
  for(idx=0;idx<lmt_nbr;idx++) (void)nco_lmt_evl(in_id,lmt+idx,0L,FORTRAN_STYLE);
  
  /* Find dimensions associated with variables to be extracted */
  dmn_lst=nco_dmn_lst_ass_var(in_id,xtr_lst,nbr_xtr,&nbr_dmn_xtr);
  
  /* Add list d */
  if(nbr_lst_d > 0) dmn_lst=nco_var_lst_add(dmn_lst,&nbr_dmn_xtr,xtr_lst_d,nbr_lst_d);  
  
  /* Fill in dimension structure for all extracted dimensions */
  dmn=(dmn_sct **)nco_malloc(nbr_dmn_xtr*sizeof(dmn_sct *));
  for(idx=0;idx<nbr_dmn_xtr;idx++) dmn[idx]=nco_dmn_fll(in_id,dmn_lst[idx].id,dmn_lst[idx].nm);
  
  /* Merge hyperslab limit information into dimension structures */
  if(lmt_nbr > 0) (void)nco_dmn_lmt_mrg(dmn,nbr_dmn_xtr,lmt,lmt_nbr);
  
  /* Duplicate input dimension structures for output dimension structures */
  dmn_out=(dmn_sct **)nco_malloc(nbr_dmn_xtr*sizeof(dmn_sct *));
  for(idx=0;idx<nbr_dmn_xtr;idx++){
    dmn_out[idx]=nco_dmn_dpl(dmn[idx]);
    (void)nco_dmn_xrf(dmn[idx],dmn_out[idx]); 
  } /* end loop over idx */
  
  /* Is this an NCAR CSM-format history tape? */
  NCAR_CSM_FORMAT=nco_ncar_csm_inq(in_id);
  
  /* Fill in variable structure list for all extracted variables */
  var=(var_sct **)nco_malloc(nbr_xtr_2*sizeof(var_sct *));
  var_out=(var_sct **)nco_malloc(nbr_xtr_2*sizeof(var_sct *));
  for(idx=0;idx<nbr_xtr_2;idx++){
    var[idx]=nco_var_fll(in_id,xtr_lst_2[idx].id,xtr_lst_2[idx].nm,dmn,nbr_dmn_xtr);
    var_out[idx]=nco_var_dpl(var[idx]);
    (void)nco_xrf_var(var[idx],var_out[idx]);
    (void)nco_xrf_dmn(var_out[idx]);
  } /* end loop over idx */
  
  /* NB: ncap is not suited for nco_var_lst_dvd() */
  /* Divide variable lists into lists of fixed variables and variables to be processed */
  (void)nco_var_lst_dvd(var,var_out,nbr_xtr_2,NCAR_CSM_FORMAT,(dmn_sct **)NULL,0,&var_fix,&var_fix_out,&nbr_var_fix,&var_prc,&var_prc_out,&nbr_var_prc);
  
  /* Open output file */
  fl_out_tmp=nco_fl_out_open(fl_out,FORCE_APPEND,FORCE_OVERWRITE,&out_id);
  
  /* Copy global attributes */
  (void)nco_att_cpy(in_id,out_id,NC_GLOBAL,NC_GLOBAL,True);
  
  /* Catenate time-stamped command line to "history" global attribute */
  if(HISTORY_APPEND) (void)nco_hst_att_cat(out_id,cmd_ln);
  
  /* Define dimensions in output file */
  (void)nco_dmn_dfn(fl_out,out_id,dmn_out,nbr_dmn_xtr);
  
  /* Take output file out of define mode */
  (void)nco_enddef(out_id); 

  /* Set arguments for second scan and script execution */
  prs_arg.fl_in=fl_in; /* [sng] Input data file */
  prs_arg.in_id=in_id; /* [id] Input data file ID */
  prs_arg.fl_out=fl_out; /* [sng] Output data file */
  prs_arg.out_id=out_id; /* [id] Output data file ID */
  prs_arg.att_lst=att_lst; /* [sct] Attributes in script */
  prs_arg.nbr_att=&nbr_att; /* [nbr] Number of attributes in script */
  prs_arg.dmn=dmn_out; /* [dmn] List of extracted dimensions */
  prs_arg.nbr_dmn_xtr=nbr_dmn_xtr; /* [nbr] Number of extracted dimensions */
  prs_arg.sym_tbl=sym_tbl; /* [fnc] Symbol table for functions */
  prs_arg.sym_tbl_nbr=sym_tbl_nbr; /* [nbr] Number of functions in table */
  prs_arg.ntl_scn=False; /* [flg] Initial scan of script */
  prs_arg.var_LHS=NULL; /* [var] LHS cast variable */
  prs_arg.nco_op_typ=nco_op_nil; /* [enm] Operation type */
  
  /* Initialize line counter */
  ln_nbr_crr=(size_t *)nco_realloc(ln_nbr_crr,ncl_dpt_crr+1UL); 
  ln_nbr_crr[ncl_dpt_crr]=1UL; /* [cnt] Line number incremented in ncap.l */
  if(fl_spt_usr == NULL){
    /* No script file specified, look for command-line scripts */
    if(nbr_spt == 0){
      (void)fprintf(stderr,"%s: ERROR no script file or command line scripts specified\n",prg_nm_get());
      (void)fprintf(stderr,"%s: HINT Use, e.g., -s \"foo=bar\"\n",prg_nm_get());
      nco_exit(EXIT_FAILURE);
    } /* end if */
    
    /* Print all command line scripts */
    if(dbg_lvl_get() > 0){
      for(idx=0;idx<nbr_spt;idx++) (void)fprintf(stderr,"spt_arg[%d] = %s\n",idx,spt_arg[idx]);
    } /* endif debug */

    /* Run parser on command line scripts */
    fl_spt_usr=(char *)strdup("Command-line scripts");
    yy_scan_string(spt_arg_cat);

  }else{ /* ...endif command-line scripts, begin script file... */

    /* Open script file for reading */
    if((yyin=fopen(fl_spt_usr,"r")) == NULL){
      (void)fprintf(stderr,"%s: ERROR Unable to open script file %s\n",prg_nm_get(),fl_spt_usr);
      nco_exit(EXIT_FAILURE);
    } /* end if */

    /* Copy script file name to global variable */
    fl_spt_glb=(char **)nco_realloc(fl_spt_glb,ncl_dpt_crr+1UL); 
    fl_spt_glb[ncl_dpt_crr]=fl_spt_usr;
  } /* end else */

  /* Invoke parser */
  rcd=yyparse((void *)&prs_arg);

  rcd=nco_redef(out_id);
  (void)nco_var_dfn(in_id,fl_out,out_id,var_out,nbr_xtr_2,(dmn_sct **)NULL,0);
  
  /* Turn off default filling behavior to enhance efficiency */
  rcd=nco_set_fill(out_id,NC_NOFILL,&fll_md_old);
  
  /* Take output file out of define mode */
  rcd=nco_enddef(out_id);
  
  /* Copy variable data for non-processed variables */
  (void)nco_var_val_cpy(in_id,out_id,var_fix,nbr_var_fix);
  
  (void)nco_redef(out_id);
  /* Copy new attributes overwriting old ones */
  for(idx=0;idx<nbr_att;idx++){
    rcd=nco_inq_varid_flg(out_id,att_lst[idx]->var_nm,&var_id);
    if(rcd == NC_NOERR){
      att_lst[idx]->mode=aed_overwrite;
      (void)nco_aed_prc(out_id,var_id,*att_lst[idx]);
    } /* end if */
  } /* end for */
  
  /* Take output file out of define mode */
  rcd=nco_enddef(out_id);

  /* Close input netCDF file */
  rcd=nco_close(in_id);
  
  /* Remove local copy of file */
  if(FILE_RETRIEVED_FROM_REMOTE_LOCATION && REMOVE_REMOTE_FILES_AFTER_PROCESSING) (void)nco_fl_rm(fl_in);
  
  /* Close output file and move it from temporary to permanent location */
  (void)nco_fl_out_cls(fl_out,fl_out_tmp,out_id);
  
  nco_exit_gracefully();
  return EXIT_SUCCESS;
} /* end main() */
