/* $Header: /data/zender/nco_20150216/nco/src/nco/ncap.c,v 1.107 2003-01-09 00:27:37 rorik Exp $ */

/* ncap -- netCDF arithmetic processor */

/* Purpose: Compute user-defined derived fields using forward algebraic notation applied to netCDF files */

/* Copyright (C) 1995--2003 Charlie Zender
   This software is distributed under the terms of the GNU General Public License
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

/* Usage:
   ncap -O -D 1 -S ${HOME}/nco/data/ncap.in ${HOME}/nco/data/in.nc ${HOME}/nco/data/foo.nc
   ncap -O -D 1 -s a=b+c -s "b=c-d/2." -S ncap.in in.nc foo.nc
   ncap -O -D 1 -s two=one+two in.nc foo.nc
   scp ~/nco/src/nco/ncap.c dataproc.ucar.edu:nco/src/nco/ncap.c */

#ifdef HAVE_CONFIG_H
#include <config.h> /* Autotools tokens */
#endif /* !HAVE_CONFIG_H */

/* Standard C headers */
#include <assert.h>  /* assert() debugging macro */
#include <math.h> /* sin cos cos sin 3.14159 */
#include <stdio.h> /* stderr, FILE, NULL, etc. */
#include <stdlib.h> /* atof, atoi, malloc, getopt */
#include <string.h> /* strcmp. . . */
#include <sys/stat.h> /* stat() */
#include <time.h> /* machine time */
#include <unistd.h> /* all sorts of POSIX stuff */
#ifdef HAVE_GETOPT_H 
/* GNU getopt() is independent system header on FREEBSD, LINUX, LINUXALPHA, WIN32
   AT&T getopt() is in unistd.h or stdlib.h on AIX, CRAY, NECSX, SUNMP, SUN4SOL2
   fxm: I'm not sure what ALPHA and SGI do */
#include <getopt.h> /* getopt_long() */
#else
#include "nco_getopt.h"
#endif /* !HAVE_GETOPT_H */

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
  bool PRINT_FUNCTION_TABLE=False;

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
  char CVS_Id[]="$Id: ncap.c,v 1.107 2003-01-09 00:27:37 rorik Exp $"; 
  char CVS_Revision[]="$Revision: 1.107 $";
  
  dmn_sct **dmn_in=NULL_CEWI;  /* holds ALL DIMS in the input file */
  dmn_sct **dmn_out=NULL_CEWI; /* Holds DIMS that have been written to OUTPUT */
  dmn_sct **dmn_new=NULL_CEWI; /* Temporary dim to reduce referencing */
  int nbr_dmn_out=0;           /* Number of dims in list dmn_out */
    
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
  int jdx;
  int in_id;  
  int out_id;  
  int nbr_abb_arg=0;
  int nbr_dmn_fl;
  int lmt_nbr=0; /* Option d. NB: lmt_nbr gets incremented */
  int nbr_spt=0; /* Option s. NB: nbr_spt gets incremented */
  int nbr_var_fl;/* number of vars in a file */
  int nbr_var_fix; /* nbr_var_fix gets incremented */
  int nbr_var_prc; /* nbr_var_prc gets incremented */
  int nbr_xtr=0; /* nbr_xtr will not otherwise be set for -c with no -v */
  int nbr_dmn_in=int_CEWI; /* number of dims in dim_in */
  int nbr_dmn_ass=int_CEWI;/* number of dims in temporary list */
  int nbr_fl=0;
  int nbr_lst_a=0; /* size of xtr_lst_a */
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
  nm_id_sct *xtr_lst=NULL;   /* The list of non-processed variables which get copied to OUTPUT */
  nm_id_sct *xtr_lst_a=NULL; /* initialized to ALL the vars in the OUTPUT file */

  time_t clock;
  
  var_sct **var;
  var_sct **var_fix;
  var_sct **var_fix_out;
  var_sct **var_out;
  var_sct **var_prc;
  var_sct **var_prc_out;

  aed_sct **att_lst=NULL;
  
  prs_sct prs_arg; /* [sct] Global information required in parser routines */
  
  static struct option opt_lng[]=
    {
      {"append",no_argument,0,'A'},
      {"coords",no_argument,0,'c'},
      {"nocoords",no_argument,0,'C'},
      {"debug",required_argument,0,'D'},
      {"dbg_lvl",required_argument,0,'D'},
      {"dimension",required_argument,0,'d'},
      {"dmn",required_argument,0,'d'},
      {"ftn",no_argument,0,'F'},
      {"ftn",no_argument,0,'F'},
      {"history",no_argument,0,'h'},
      {"hst",no_argument,0,'h'},
      {"local",required_argument,0,'l'},
      {"nintap",required_argument,0,'n'},
      {"overwrite",no_argument,0,'O'},
      {"path",required_argument,0,'p'},
      {"retain",no_argument,0,'R'},
      {"revision",no_argument,0,'r'},
      {"script",required_argument,0,'s'},
      {"file",required_argument,0,'S'},
      {"units",no_argument,0,'u'},
      {"variable",no_argument,0,'v'},
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
  opt_sng="ACcD:d:Fhl:n:Op:Rrs:S:vx-:";
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
      PRINT_FUNCTION_TABLE=True;
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

  /* fxm: Change whole section to autotools format #if HAVE_ERF ... */
#if (defined AIX) || (defined CRAY) || (defined SGI6) || (defined SGI64) || (defined SGIMP64) || (defined WIN32)
  /* 20020122 and 20020422: AIX, CRAY, SGI*, WIN32 do not define erff(), erfcf(), gammaf() */
  sym_tbl_nbr-=3; /* Advanced math: erf, erfc, gamma */
  /* sym_tbl_nbr-=2; *//* Basic Rounding: ceil, floor */
  /* 20020703: AIX, MACOSX, SGI*, WIN32 do not define rintf */
  sym_tbl_nbr-=4; /* Advanced Rounding: nearbyint, rintf, round, trunc */
  /* 20020703: AIX, SGI*, WIN32 do not define acoshf, asinhf, atanhf */
  sym_tbl_nbr-=6; /* Hyperbolic trigonometric: acosh, asinh, atanh, cosh, sinh, tanh */
#else /* not AIX || CRAY || SGI* || WIN32 */
  /* LINUX*, MACOSX*, and SUN* provide these functions */
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
     to activate them on LINUX* but not on MACOSX* and SUN* */
  sym_tbl_nbr-=4; /* Advanced Rounding: nearbyint, rint, round, trunc */
  /* Advanced Rounding: nearbyint, round, trunc */
  /* sym_tbl[sym_idx++]=ncap_sym_init("nearbyint",nearbyint,nearbyintf); *//* Round to integer value in floating point format using current rounding direction, do not raise inexact exceptions */
  /* sym_tbl[sym_idx++]=ncap_sym_init("round",round,roundf); *//* Round to nearest integer away from zero */
  /* sym_tbl[sym_idx++]=ncap_sym_init("trunc",trunc,truncf); *//* Round to nearest integer not larger in absolute value */
  /* sym_tbl[sym_idx++]=ncap_sym_init("rint",rint,rintf); *//* Round to integer value in floating point format using current rounding direction, raise inexact exceptions */
#endif /* not AIX || CRAY || SGI* || WIN32 */
  assert(sym_idx == sym_tbl_nbr);


  if(PRINT_FUNCTION_TABLE){

    printf("\n  Available Maths functions:\n");
    printf("  ----------------------\n");
    printf("  NAME    FLOAT   DOUBLE\n");          
    for(idx = 0 ;idx < sym_tbl_nbr ; idx++)
      printf("  %-7s %-7c %-7c\n", sym_tbl[idx]->nm, (sym_tbl[idx]->fnc_flt ? 'y' : 'n'), (sym_tbl[idx]->fnc_dbl ? 'y' : 'n'));
     printf("  ----------------------\n");
  nco_exit(EXIT_SUCCESS);

  }


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
  

  /* Form list of ALL DIMENSIONS */  
  dmn_lst=nco_dmn_lst(in_id,&nbr_dmn_in);

  dmn_in=(dmn_sct **)nco_malloc(nbr_dmn_in*sizeof(dmn_sct *));
  for(idx=0;idx<nbr_dmn_in;idx++) dmn_in[idx]=nco_dmn_fll(in_id,dmn_lst[idx].id,dmn_lst[idx].nm);

  /* Merge hyperslab limit information into dimension structures */
    if(lmt_nbr > 0) (void)nco_dmn_lmt_mrg(dmn_in,nbr_dmn_in,lmt,lmt_nbr);


    /* Open output file */
    fl_out_tmp=nco_fl_out_open(fl_out,FORCE_APPEND,FORCE_OVERWRITE,&out_id);
  
    /* Copy global attributes */
    (void)nco_att_cpy(in_id,out_id,NC_GLOBAL,NC_GLOBAL,True);
  
    /* Catenate time-stamped command line to "history" global attribute */
    if(HISTORY_APPEND) (void)nco_hst_att_cat(out_id,cmd_ln);

    (void)nco_enddef(out_id);

  /* Set arguments for second scan and script execution */
  prs_arg.fl_in=fl_in; /* [sng] Input data file */
  prs_arg.in_id=in_id; /* [id] Input data file ID */
  prs_arg.fl_out=fl_out; /* [sng] Output data file */
  prs_arg.out_id=out_id; /* [id] Output data file ID */
  prs_arg.att_lst=&att_lst; /* [sct] Attributes in script */
  prs_arg.nbr_att=&nbr_att; /* [nbr] Number of attributes in script */
  prs_arg.dmn_in=dmn_in; /* [dmn_in] List of all dimensions in input */
  prs_arg.nbr_dmn_in=nbr_dmn_in; /* [nbr] Number of  dimensions in input */
  prs_arg.dmn_out=&dmn_out;     /* pointer to list of dims in output */
  prs_arg.nbr_dmn_out=&nbr_dmn_out; /* number of dims in above list */
  prs_arg.sym_tbl=sym_tbl; /* [fnc] Symbol table for functions */
  prs_arg.sym_tbl_nbr=sym_tbl_nbr; /* [nbr] Number of functions in table */
  prs_arg.ntl_scn=False; /* No longer do an initial scan */
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
    fl_spt_usr=(char *)strdup("Command-line script");
    yy_scan_string(spt_arg_cat);

  }else{ /* ...endif command-line scripts, begin script file... */

    /* Open script file for reading */
    if((yyin=fopen(fl_spt_usr,"r")) == NULL){
      (void)fprintf(stderr,"%s: ERROR Unable to open script file %s\n",prg_nm_get(),fl_spt_usr);
      nco_exit(EXIT_FAILURE);
    } /* end if */

  } /* end else */
  
  /* Copy script file name to global variable */
  fl_spt_glb=(char **)nco_realloc(fl_spt_glb,ncl_dpt_crr+1UL); 
  fl_spt_glb[ncl_dpt_crr]=fl_spt_usr;

  /* Invoke parser */
  rcd=yyparse((void *)&prs_arg);

  /* Get number of variables in output file */
  rcd=nco_inq(out_id,(int *)NULL,&nbr_var_fl,(int *)NULL,(int*)NULL);

  /* Make list of all NEW VARS in output_file */  
  xtr_lst_a=nco_var_lst_mk(out_id,nbr_var_fl,var_lst_in,False,&nbr_lst_a);

  if(PROCESS_ALL_VARS){
    /* Get number of variables in input file */
    rcd=nco_inq(in_id,(int *)NULL,&nbr_var_fl,(int *)NULL,(int *)NULL);

    /* Form initial list of ALL VARIABLES IN INPUT FILE */
    xtr_lst=nco_var_lst_mk(in_id,nbr_var_fl,var_lst_in,False,&nbr_xtr);
  } /* endif */
   
  if(!PROCESS_ALL_VARS){
    /* Make list of vars of new attributes whose parent var is ONLY in input file */
    xtr_lst=nco_att_lst_mk(in_id,out_id,att_lst,nbr_att,&nbr_xtr);
  } /* endif */
    /* Find dims associated with xtr_lst */
    /* Write to O only new dims
       Add apropriate co-ordinate vars to extraction list 
      options -c      -process all cordinates 
                       i.e add  co-ords to var list 
                       Also add their dims

      options --none   -process associated co-ords
                       loop though dim_out and append to var list
 
     options -C         no co-ordinates   Do nothing */

    /* Subtract list a again */
    /* Finally extract vars on list */

   /* Subtract list a */
   if(nbr_lst_a >0) xtr_lst=nco_var_lst_sub(xtr_lst, &nbr_xtr,xtr_lst_a,nbr_lst_a);

   (void)nco_redef(out_id);

   /* Make list of dims of vars in xtr_lst */
   if(nbr_xtr > 0) dmn_lst=nco_dmn_lst_ass_var(in_id,xtr_lst,nbr_xtr,&nbr_dmn_ass);
  
    /* Find and add any new dims to output */
    for(idx=0;idx < nbr_dmn_ass;idx++)
      for(jdx=0;jdx < nbr_dmn_in;jdx++){
        /* if dimension in list and it hasn't been defined yet */
	if(!strcmp(dmn_lst[idx].nm, dmn_in[jdx]->nm) && !dmn_in[jdx]->xrf){     
          /* add dim to output list dmn_prc */
          dmn_new=nco_dmn_out_grow((void *)&prs_arg);
          *dmn_new=nco_dmn_dpl(dmn_in[jdx]);
          (void)nco_dmn_xrf(*dmn_new,dmn_in[jdx]);
	  /* write dim to output */
	  (void)nco_dmn_dfn(fl_out,out_id,dmn_new,1);
	  break;
	}
      } 

    /* All dims for all vars are now in output    */
    /* Need to add co-ordinate vars to extraction list */
    /* if PROCESS_ALL_COORDINATES then the associated DIM needs to written to output */
    if(PROCESS_ASSOCIATED_COORDINATES){
      for(idx=0; idx <nbr_dmn_in ; idx++){
        if(!dmn_in[idx]->is_crd_dmn) continue;
	
        if(PROCESS_ALL_COORDINATES && !dmn_in[idx]->xrf) {
	  
          /* add dim to output list dmn_out */
          dmn_new=nco_dmn_out_grow((void *)&prs_arg);
          *dmn_new=nco_dmn_dpl(dmn_in[idx]);
          (void)nco_dmn_xrf(*dmn_new,dmn_in[idx]);
	  /* write dim to output */
	  (void)nco_dmn_dfn(fl_out,out_id,dmn_new,1);
        }
	/* Add co-odinate var to extraction list, dim has already been output */
	if(dmn_in[idx]->xrf) {
	  for(jdx=0;jdx < nbr_xtr;jdx++)
	    if(!strcmp(xtr_lst[jdx].nm,dmn_in[idx]->nm)) break;
	  if(jdx != nbr_xtr) continue;
	  /* If co-ord is not on list then add it to extraction list */
	  xtr_lst=(nm_id_sct *)nco_realloc(xtr_lst,(nbr_xtr+1)*sizeof(nm_id_sct));     
	  xtr_lst[nbr_xtr].nm=(char *)strdup(dmn_in[idx]->nm);
	  xtr_lst[nbr_xtr++].id=dmn_in[idx]->cid;
	}  
      } /* for idx */	      
    } /* if */ 
    
    /* now subtract list a again (it may contain re-defined co-ordinates !!!) */
    if(nbr_xtr >0) xtr_lst=nco_var_lst_sub(xtr_lst, &nbr_xtr,xtr_lst_a,nbr_lst_a);
    
    /* sort extraction list for faster I/O */
  if(nbr_xtr > 1) xtr_lst=nco_lst_srt(xtr_lst,nbr_xtr,False);

  /* Is this an NCAR CSM-format history tape? */
  NCAR_CSM_FORMAT=nco_ncar_csm_inq(in_id);

  /* now write the "fixed" variables */
  var=(var_sct **)nco_malloc(nbr_xtr*sizeof(var_sct *));
  var_out=(var_sct **)nco_malloc(nbr_xtr*sizeof(var_sct *));
  for(idx=0;idx<nbr_xtr;idx++){
    var[idx]=nco_var_fll(in_id,xtr_lst[idx].id,xtr_lst[idx].nm,dmn_in,nbr_dmn_in);
    var_out[idx]=nco_var_dpl(var[idx]);
    (void)nco_xrf_var(var[idx],var_out[idx]);
    (void)nco_xrf_dmn(var_out[idx]);
  } /* end loop over idx */
  
  /* NB: ncap is not suited for nco_var_lst_dvd() */
  /* Divide variable lists into lists of fixed variables and variables to be processed */
  (void)nco_var_lst_dvd(var,var_out,nbr_xtr,NCAR_CSM_FORMAT,(dmn_sct **)NULL,0,&var_fix,&var_fix_out,&nbr_var_fix,&var_prc,&var_prc_out,&nbr_var_prc);

  /* define non-processed vars  */
   (void)nco_var_dfn(in_id,fl_out,out_id,var_out,nbr_xtr,(dmn_sct**)NULL,0);

   (void)nco_enddef(out_id);


  rcd=nco_set_fill(out_id,NC_NOFILL,&fll_md_old);

  /* copy non-processed vars */
  (void)nco_var_val_cpy(in_id,out_id,var_fix,nbr_var_fix);


  nco_redef(out_id);

  /* Now write out new attributes possibly overwriting old ones */
  for(idx=0; idx < nbr_att ; idx++){
    rcd=nco_inq_varid_flg(out_id,att_lst[idx]->var_nm,&var_id);
    if(rcd != NC_NOERR) continue;
    att_lst[idx]->mode=aed_overwrite;
    (void)nco_aed_prc(out_id,var_id,*att_lst[idx]);
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
