/* $Header: /data/zender/nco_20150216/nco/src/nco/csz.c,v 1.32 1999-12-14 22:39:31 zender Exp $ */

/* (c) Copyright 1995--1999 University Corporation for Atmospheric Research 
   The file LICENSE contains the full copyright notice 
   Contact NSF/UCAR/NCAR/CGD/CMS for copyright assistance */

/* Purpose: Standalone utilities for C programs (no netCDF required) */ 

/* Standard header files */
#include <math.h>               /* sin cos cos sin 3.14159 */
#include <stdio.h>              /* stderr, FILE, NULL, etc. */
#include <stdlib.h>             /* atof, atoi, malloc, getopt */ 
#include <string.h>             /* strcmp. . . */
#include <sys/stat.h>           /* stat() */
#include <time.h>               /* machine time */
#include <unistd.h>             /* all sorts of POSIX stuff */ 
/* #include <errno.h> */             /* errno */
/* #include <malloc.h>    */         /* malloc() stuff */
/* #include <assert.h> */            /* assert() debugging macro */ 

#include <sys/types.h>          /* needed for _res */ 
#include <netinet/in.h>         /* needed for _res */ 
#include <pwd.h>                /* password structures for getpwuid() */
#ifndef WIN32
#include <arpa/nameser.h>       /* needed for _res */ 
#include <resolv.h>             /* Internet structures for _res */
#endif

/* I'm only keeping these netCDF include files around because I'm worried that 
   function prototypes in nc.h are needed here. Eventually prototypes for these
   routines should be broken into separate files, like csz.h... */ 
#include <netcdf.h>             /* netCDF def'ns */
#include "nc.h"                 /* netCDF operator universal def'ns */

#ifndef bool
#define bool int
#endif
#ifndef True
#define True 1
#endif
#ifndef False
#define False 0
#endif

void 
Exit_gracefully(void)
{
  char *time_bfr_end;
  time_t clock;
  
  /* end the clock */  
  
  clock=time((time_t *)NULL);
  time_bfr_end=ctime(&clock);
/*  (void)fprintf(stderr,"\tend = %s\n",time_bfr_end);*/

  (void)fclose(stderr);
  (void)fclose(stdin);
  (void)fclose(stdout);

  exit(EXIT_SUCCESS);
} /* end Exit_gracefully() */ 

char * 
nmn_get()
{ 
  /* Purpose: Return a mnemonic that describes the current NCO version */
  return "Son of Zot";
} /* end nmn_get() */

char *
cmd_ln_sng(int argc,char **argv)
/* 
   int argc: I argument count
   char **argv: I argument list
   char *cmd_ln_sng(): O command line
*/ 
{
  char *cmd_ln;
  
  int cmd_ln_sz=0;
  int idx;

  for(idx=0;idx<argc;idx++){
    cmd_ln_sz+=(int)strlen(argv[idx])+1;
  } /* end loop over args */ 
  cmd_ln=(char *)malloc(cmd_ln_sz*sizeof(char));
  if(argc <= 0){
    cmd_ln=(char *)malloc(sizeof(char));
    cmd_ln[0]='\0';
  }else{
    (void)strcpy(cmd_ln,argv[0]);
    for(idx=1;idx<argc;idx++){
      (void)strcat(cmd_ln," ");
      (void)strcat(cmd_ln,argv[idx]);
    } /* end loop over args */ 
  } /* end else */ 

  return cmd_ln;
} /* end cmd_ln_sng() */ 

lmt_sct *
lmt_prs(int nbr_lmt,char **lmt_arg)
/* 
   int nbr_lmt: I number of dimensions with limits
   char **lmt_arg: I list of user-specified dimension limits
   lmt_sct *lmt_prs(): O structure holding user-specified strings for min and max limits
 */ 
{
  /* Routine to set name, min_sng, max_sng elements of 
     a comma separated list of names and ranges. This routine
     merely evaluates the syntax of the input expressions and
     does not attempt to validate the dimensions or their ranges
     against those present in the input netCDF file. */

  /* Valid syntax adheres to nm,[min_sng][,[max_sng]][,srd_sng] */

  void usg_prn(void);

  char **arg_lst;

  char *dlm_sng=",";

  lmt_sct *lmt;

  int idx;
  int arg_nbr;

  lmt=(lmt_sct *)malloc(nbr_lmt*sizeof(lmt_sct));

  for(idx=0;idx<nbr_lmt;idx++){

    /* Hyperslab specifications are processed as a normal text list */ 
    arg_lst=lst_prs(lmt_arg[idx],dlm_sng,&arg_nbr);

    /* Check syntax */ 
    if(
       arg_nbr < 2 || /* Need more than just dimension name */ 
       arg_nbr > 4 || /* Too much information */ 
       arg_lst[0] == NULL || /* Dimension name not specified */ 
       (arg_nbr == 3 && arg_lst[1] == NULL && arg_lst[2] == NULL) || /* No min or max when stride not specified */ 
       (arg_nbr == 4 && arg_lst[3] == NULL) || /* Stride should be specified */ 
       False){
      (void)fprintf(stdout,"%s: ERROR in hyperslab specification for dimension %s\n",prg_nm_get(),lmt_arg[idx]);
      exit(EXIT_FAILURE);
    } /* end if */ 

    /* Initialize structure */ 
    /* lmt strings which are not explicitly set by the user remain NULL, i.e., 
       specifying the default setting will appear as if nothing at all was set.
       Hopefully, in the routines that follow, the branch followed by a dimension for which
       all the default settings were specified (e.g.,"-d foo,,,,") will yield the same answer
       as the branch for which no hyperslab along that dimension was set.
     */ 
    lmt[idx].nm=NULL;
    lmt[idx].is_usr_spc_lmt=True; /* True if any part of limit is user-specified, else False */
    lmt[idx].min_sng=NULL;
    lmt[idx].max_sng=NULL;
    lmt[idx].srd_sng=NULL;

    /* Fill in structure */ 
    lmt[idx].nm=arg_lst[0];
    lmt[idx].min_sng=lmt[idx].max_sng=arg_lst[1];
    if(arg_nbr > 2) lmt[idx].max_sng=arg_lst[2];
    if(arg_nbr > 3) lmt[idx].srd_sng=arg_lst[3];

    if(lmt[idx].max_sng == NULL) lmt[idx].is_usr_spc_max=False; else lmt[idx].is_usr_spc_max=True;
    if(lmt[idx].min_sng == NULL) lmt[idx].is_usr_spc_min=False; else lmt[idx].is_usr_spc_min=True;
  } /* End loop over lmt */

  return lmt;

} /* end lmt_prs() */ 

char *
sng_lst_prs(char **sng_lst,const long lmn_nbr, const char *dlm_sng)
/* 
   const char **sng_lst: I List of pointers to strings to join together
   const char *dlm_sng: I delimiter string to use as glue
   const long lmn_nbr: O number of strings in list
   char *sng_lst_prs: O Concatenated string formed by joining all input strings
 */ 
{
  /* Routine takes a list of strings and joins them together into one string
     Elements of input list should all be NUL-terminated strings
     Element with the value NULL, will be interpreted as strings of zero length
  */ 

  char *sng; /* Output string */ 

  int dlm_len;
  long lmn;
  long sng_sz=0L; /* NB: sng_sz get incremented */

  if(lmn_nbr == 1L) return sng_lst[0];

  /* Delimiter must be NUL-terminated (a string) so strlen() works */ 
  if(dlm_sng == NULL){
    (void)fprintf(stdout,"%s: ERROR sng_lst_prs() reports delimiter string is NULL\n",prg_nm_get());
    exit(EXIT_FAILURE);
  } /* end if */
  dlm_len=strlen(dlm_sng); 

  /* List elements must be NUL-terminated (strings) so strlen() works */ 
  for(lmn=0L;lmn<lmn_nbr;lmn++) sng_sz+=(sng_lst[lmn] == NULL) ? 0L : strlen(sng_lst[lmn])+dlm_len;
  /* Add one for NULL byte */
  sng=(char *)malloc(sizeof(char)*(sng_sz+1));
  /* NUL-terminate string for safety */
  sng[0]='\0';
  for(lmn=0L;lmn<lmn_nbr;lmn++){
    /* List elements must be NUL-terminated (strings) so strcat() works */ 
    sng=(sng_lst[lmn] == NULL) ? sng : strcat(sng,sng_lst[lmn]);
    if(lmn != lmn_nbr-1 && dlm_len != 0) sng=strcat(sng,dlm_sng);
  } /* end loop over lmn */

  return sng;
} /* end lst_prs() */ 

char **
lst_prs(char *sng_in,const char *dlm_sng,int *nbr_lst)
/* 
   char *sng_in: I/O delimited argument list (delimiters are changed to NULL on output)
   const char *dlm_sng: I delimiter string
   int *nbr_lst: O number of elements in list
   char **lst_prs: O array of list elements
 */ 
{
  /* Routine creates a list of strings from a given string and an arbitrary delimiter */ 

  /* Number of list members is always one more than number of delimiters, e.g.,
     foo,,3, has 4 arguments: "foo", "", "3" and "".
     A delimiter without an argument is valid syntax to indicate the default argument
     Therefore a storage convention is necessary to indicate the default argument was selected
     Either NULL or '\0' can be used without requiring the use of an additional flag
     NULL can not be printed, but is useful as a logical flag since it's value is False
     On the other hand, '\0', the empty string, can be printed but is not as useful as a flag
     Currently, NCO implements the former convention, where default selections are set to NULL
   */
    
  char **lst;
  char *sng_in_ptr;

  int dlm_len;
  int idx;

  /* Delimiter must be NUL-terminated (a string) so we may find its length */ 
  dlm_len=strlen(dlm_sng); 

  /* Do not increment actual sng_in pointer while searching for delimiters---increment a dummy pointer instead. */ 
  sng_in_ptr=sng_in; 

  /* First element does not require a delimiter in front of it */ 
  *nbr_lst=1;

  /* Count list members */ 
  while((sng_in_ptr=strstr(sng_in_ptr,dlm_sng))){
    sng_in_ptr+=dlm_len;
    (*nbr_lst)++;
  } /* end while */ 

  lst=(char **)malloc(*nbr_lst*sizeof(char *));

  sng_in_ptr=sng_in; 
  lst[0]=sng_in;
  idx=0;
  while((sng_in_ptr=strstr(sng_in_ptr,dlm_sng))){
    /* NULL terminate previous arg */ 
    *sng_in_ptr='\0';
    sng_in_ptr+=dlm_len;
    lst[++idx]=sng_in_ptr;
  } /* end while */ 

  /* A default list member is assumed whenever two delimiters are adjacent to eachother, such that
     the length of the string between them is 0. If the list ends with a delimiter, then the last
     element of the list is also assumed to be a default list member. */ 
  /* This loop sets default list members to NULL */
  for(idx=0;idx<*nbr_lst;idx++)
    if(strlen(lst[idx]) == 0) lst[idx]=NULL;

  if(dbg_lvl_get() == 5){
    (void)fprintf(stderr,"%d elements in delimited list\n",*nbr_lst);
    for(idx=0;idx<*nbr_lst;idx++) 
      (void)fprintf(stderr,"lst[%d] = %s\n",idx,(lst[idx] == NULL) ? "NULL" : lst[idx]);
    (void)fprintf(stderr,"\n");
    (void)fflush(stderr);
  } /* end debug */

  return lst;
} /* end lst_prs() */ 

char *
fl_nm_prs(char *fl_nm,int fl_nbr,int *nbr_fl,char **fl_lst_in,int nbr_abb_arg,char **fl_lst_abb,char *fl_pth)
/* 
   char *fl_nm: I current filename, if any
   int fl_nbr: I ordinal index of file in input file list
   int *nbr_fl: I/O number of files to be processed
   char **fl_lst_in: I user-specified filenames
   char **fl_lst_abb: I NINTAP-style arguments, if any
   char *fl_pth: I path prefix for files in fl_lst_in
   char *fl_nm_prs: O name of file to retrieve
 */ 
{
  /* Routine to construct a file name from various input arguments and switches.
     This routine implements the NINTAP-style specification by using static
     memory to avoid repetition in the construction of the filename */

  static short FIRST_INVOCATION=1;

  static char *fl_nm_1st_dgt;
  static char *fl_nm_nbr_sng;
  static char fl_nm_nbr_sng_fmt[10];

  static int fl_nm_nbr_crr;
  static int fl_nm_nbr_dgt;
  static int fl_nm_nbr_ncr;
  static int fl_nm_nbr_max;
  static int fl_nm_nbr_min;

  /* Free any old filename space */ 
  if(fl_nm != NULL) (void)free(fl_nm);

  /* Construct filename from NINTAP-style arguments and input name */
  if(fl_lst_abb != NULL){
    if(FIRST_INVOCATION){
      int fl_nm_sfx_len=0;
      
      /* Parse abbreviation list analogously to CCM Processor ICP "NINTAP" */
      if(nbr_fl != NULL) *nbr_fl=atoi(fl_lst_abb[0]);
      
      if(nbr_abb_arg > 1){
	fl_nm_nbr_dgt=atoi(fl_lst_abb[1]);
      }else{
	fl_nm_nbr_dgt=3;
      }/* end if */
      
      if(nbr_abb_arg > 2){
	fl_nm_nbr_ncr=atoi(fl_lst_abb[2]);
      }else{
	fl_nm_nbr_ncr=1;
      } /* end if */
      
      if(nbr_abb_arg > 3){
	fl_nm_nbr_max=atoi(fl_lst_abb[3]);
      }else{
	fl_nm_nbr_max=0;
      } /* end if */
      
      if(nbr_abb_arg > 4){
	fl_nm_nbr_min=atoi(fl_lst_abb[4]);
      }else{
	fl_nm_nbr_min=1;
      } /* end if */
      
      /* Is there a .nc, .cdf, .hdf, or .hd5 suffix? */ 
      if(strncmp(fl_lst_in[0]+strlen(fl_lst_in[0])-3,".nc",3) == 0) 
	fl_nm_sfx_len=3;
      else if(strncmp(fl_lst_in[0]+strlen(fl_lst_in[0])-4,".cdf",4) == 0)
	fl_nm_sfx_len=4;
      else if(strncmp(fl_lst_in[0]+strlen(fl_lst_in[0])-4,".hdf",4) == 0)
	fl_nm_sfx_len=4;
      else if(strncmp(fl_lst_in[0]+strlen(fl_lst_in[0])-4,".hd5",4) == 0)
	fl_nm_sfx_len=4;
      
      /* Initialize static information useful for future invocations */
      fl_nm_1st_dgt=fl_lst_in[0]+strlen(fl_lst_in[0])-fl_nm_nbr_dgt-fl_nm_sfx_len;
      fl_nm_nbr_sng=(char *)malloc((fl_nm_nbr_dgt+1)*sizeof(char));
      fl_nm_nbr_sng=strncpy(fl_nm_nbr_sng,fl_nm_1st_dgt,fl_nm_nbr_dgt);
      fl_nm_nbr_sng[fl_nm_nbr_dgt]='\0';
      fl_nm_nbr_crr=atoi(fl_nm_nbr_sng);
      (void)sprintf(fl_nm_nbr_sng_fmt,"%%0%dd",fl_nm_nbr_dgt);

      /* First filename is always specified on command line anyway... */ 
      fl_nm=(char *)strdup(fl_lst_in[0]);

      /* Set flag that this routine has already been invoked at least once */ 
      FIRST_INVOCATION=False;

    }else{ /* end if FIRST_INVOCATION */
      /* Create current filename from previous filename */ 
      fl_nm_nbr_crr+=fl_nm_nbr_ncr;
      if(fl_nm_nbr_max) 
	if(fl_nm_nbr_crr > fl_nm_nbr_max) 
	  fl_nm_nbr_crr=fl_nm_nbr_min; 
      (void)sprintf(fl_nm_nbr_sng,fl_nm_nbr_sng_fmt,fl_nm_nbr_crr);
      fl_nm=(char *)strdup(fl_lst_in[0]);
      (void)strncpy(fl_nm+(fl_nm_1st_dgt-fl_lst_in[0]),fl_nm_nbr_sng,fl_nm_nbr_dgt);
    } /* end if not FIRST_INVOCATION */
  }else{ /* end if abbreviation list */
    fl_nm=(char *)strdup(fl_lst_in[fl_nbr]);
  } /* end if no abbreviation list */
  
  /* Prepend path prefix */ 
  if(fl_pth != NULL){
    char *fl_nm_stub;

    fl_nm_stub=fl_nm;

    /* Allocate enough room for joining slash '/' and terminating NUL */ 
    fl_nm=(char *)malloc((strlen(fl_nm_stub)+strlen(fl_pth)+2)*sizeof(char));
    (void)strcpy(fl_nm,fl_pth);
    (void)strcat(fl_nm,"/");
    (void)strcat(fl_nm,fl_nm_stub);

    /* Free filestub space */ 
    (void)free(fl_nm_stub);
  } /* end if */

  /* Return new filename */ 
  return(fl_nm);
} /* end fl_nm_prs() */ 

char *
fl_mk_lcl(char *fl_nm,char *fl_pth_lcl,int *FILE_RETRIEVED_FROM_REMOTE_LOCATION)
/* 
   char *fl_nm: I/O current filename, if any (destroyed)
   char *fl_pth_lcl: I local storage area for files retrieved from remote locations, if any
   int *FILE_RETRIEVED_FROM_REMOTE_LOCATION: O flag set if file had to be retrieved from remote system
   char *fl_mk_lcl(): O filename locally available file
*/ 
{
  /* Routine to locate the input file, retrieve it from a remote storage system if necessary, 
     create the local storage directory if neccessary, check the file for read-access,
     return the filename of the file on the local system */ 

  FILE *fp_in;
  char *cln_ptr; /* [ptr] Colon pointer */
  char *fl_nm_lcl;
  char *fl_nm_stub;
  int rcd;
  struct stat stat_sct;
  
  /* Assume the local filename is the input filename */ 
  fl_nm_lcl=(char *)strdup(fl_nm);

  /* Remove any URL and machine-name components from the local filename */
  if(strstr(fl_nm_lcl,"ftp://") == fl_nm_lcl){
    char *fl_nm_lcl_tmp;
    char *fl_pth_lcl_tmp;

    /* Rearrange the fl_nm_lcl to get rid of the ftp://hostname part */ 
    fl_pth_lcl_tmp=strchr(fl_nm_lcl+6,'/');
    fl_nm_lcl_tmp=fl_nm_lcl;
    fl_nm_lcl=(char *)malloc(strlen(fl_pth_lcl_tmp)+1);
    (void)strcpy(fl_nm_lcl,fl_pth_lcl_tmp);
    (void)free(fl_nm_lcl_tmp);
  }else if((cln_ptr=strchr(fl_nm_lcl,':'))){
    /* 19990804
       A colon separates the machine name from the filename in rcp and scp requests
       However, a colon is also legal in any UNIX filename
       Thus whether a colon signifies an rcp or scp request is somewhat ambiguous 
       NCO treats names with more than one colon as regular filenames
       In order for a colon to be interpreted as a machine name delimiter,
       it must be preceded by a period within three or four spaces, e.g., uci.edu:
    */
    if(((cln_ptr-4 >= fl_nm_lcl) && *(cln_ptr-4) == '.') ||
       ((cln_ptr-3 >= fl_nm_lcl) && *(cln_ptr-3) == '.')){
      char *fl_nm_lcl_tmp;
      char *fl_pth_lcl_tmp;
      
      /* Rearrange the fl_nm_lcl to get rid of the hostname: part */ 
      fl_pth_lcl_tmp=strchr(fl_nm_lcl+6,'/');
      fl_nm_lcl_tmp=fl_nm_lcl;
      fl_nm_lcl=(char *)malloc(strlen(fl_pth_lcl_tmp)+1);
      (void)strcpy(fl_nm_lcl,fl_pth_lcl_tmp);
      (void)free(fl_nm_lcl_tmp);
    } /* endif period is three or four characters from colon */
  } /* end if */ 
  
  /* Does the file exist on the local system? */ 
  rcd=stat(fl_nm_lcl,&stat_sct);
  
  /* If not, then check if the file exists on the local system under 
     the same path interpreted relative to the current working directory */ 
  if(rcd == -1){
    if(fl_nm_lcl[0] == '/'){
      rcd=stat(fl_nm_lcl+1,&stat_sct);
    } /* end if */ 
    if(rcd == 0){
      char *fl_nm_lcl_tmp;
      
      /* NB: simply adding one to the filename pointer is like deleting
	 the initial slash on the filename. Without copying the new name
	 into its own memory space, free(fl_nm_lcl) would not be able to free 
	 the initial byte. */ 
      fl_nm_lcl_tmp=(char *)strdup(fl_nm_lcl+1);
      (void)free(fl_nm_lcl);
      fl_nm_lcl=fl_nm_lcl_tmp;
      (void)fprintf(stderr,"%s: WARNING not searching for %s on remote filesystem, using local file %s instead\n",prg_nm_get(),fl_nm,fl_nm_lcl+1);
    } /* end if */
  } /* end if */ 
  
  /* Finally, check to see if the file exists on the local system in the
     directory specified for the storage of remotely retrieved files.
     This would be the case if some files had already been retrieved in
     a previous invocation of the program */
  if(rcd == -1){
    /* Where does the filename stub begin? NB: We are assuming that the local filename
       has a slash in it from now on (because the remote file system always has a slash) */ 
    fl_nm_stub=strrchr(fl_nm_lcl,'/')+1;

    /* Construct the local filename from the user-supplied local file path 
       along with the existing file stub. */
    if(fl_pth_lcl != NULL){
      char *fl_nm_lcl_tmp;
      
      fl_nm_lcl_tmp=fl_nm_lcl;
      /* Allocate enough room for the joining slash '/' and the terminating NUL */ 
      fl_nm_lcl=(char *)malloc((strlen(fl_pth_lcl)+strlen(fl_nm_stub)+2)*sizeof(char));
      (void)strcpy(fl_nm_lcl,fl_pth_lcl);
      (void)strcat(fl_nm_lcl,"/");
      (void)strcat(fl_nm_lcl,fl_nm_stub);
      /* Free the old filename space */ 
      (void)free(fl_nm_lcl_tmp);
    } /* end if */ 
    
    /* At last, check for the file in the local storage directory */ 
    rcd=stat(fl_nm_lcl,&stat_sct);
    if (rcd != -1) (void)fprintf(stderr,"%s: WARNING not searching for %s on remote filesystem, using local file %s instead\n",prg_nm_get(),fl_nm,fl_nm_lcl);
  } /* end if */ 

  /* The file was not found locally, try to fetch it from the remote file system */
  if(rcd == -1){

    typedef struct{
      char *fmt;
      int nbr_fmt_char;
      int transfer_mode;
      int file_order;
    } rmt_fch_cmd_sct;

    char *cmd_sys;
    char *fl_nm_rmt;
    char *fl_pth_lcl_tmp=NULL;
    
#if ( ! defined SUN4 )
    char cmd_mkdir[]="mkdir -m 777 -p";
#else
    char cmd_mkdir[]="mkdir -p";
#endif

    enum {
      synchronous, /* 0 */ 
      asynchronous}; /* 1 */ 

    enum {
      lcl_rmt, /* 0 */ 
      rmt_lcl}; /* 1 */ 

    int fl_pth_lcl_len;
    
    rmt_fch_cmd_sct *rmt_cmd=NULL;
    rmt_fch_cmd_sct msread={"msread -R %s %s",4,synchronous,lcl_rmt};
    rmt_fch_cmd_sct msrcp={"msrcp mss:%s %s",4,synchronous,rmt_lcl};
    rmt_fch_cmd_sct nrnet={"nrnet msget %s r flnm=%s l mail=FAIL",4,asynchronous,lcl_rmt};
    rmt_fch_cmd_sct rcp={"rcp -p %s %s",4,synchronous,rmt_lcl};
    rmt_fch_cmd_sct scp={"scp -p %s %s",4,synchronous,rmt_lcl};
    rmt_fch_cmd_sct ftp={"",4,synchronous,rmt_lcl};

    /* Why did the stat() command fail? */ 
/*    (void)perror(prg_nm_get());*/
    
    /* The remote filename is the input filename by definition */ 
    fl_nm_rmt=fl_nm;
    
    /* A URL specifier in the filename unambiguously signals to use anonymous ftp */     if(rmt_cmd == NULL){
      if(strstr(fl_nm_rmt,"ftp://") == fl_nm_rmt){
#ifdef WIN32
      /* I have no idea how networking calls work in NT, so just exit */
      (void)fprintf(stdout,"%s: ERROR Networking required to obtain %s is not supported for Windows NT\n",prg_nm_get(),fl_nm_rmt);
      exit(EXIT_FAILURE);
#else /* not WIN32 */ 
	char *fmt;
	char *usr_nm;
	char *host_nm_lcl;
	char *host_nm_rmt;
	char *usr_email;
	char fmt_template[]="ftp -n << END\nopen %s\nuser anonymous %s\nbin\nget %s %s\nquit\nEND";

	struct passwd *usr_pwd;

	uid_t usr_uid;

	rmt_cmd=&ftp;

	usr_uid=getuid();
	usr_pwd=getpwuid(usr_uid);
	usr_nm=usr_pwd->pw_name;
	/* DEBUG: 256 should be replaced by MAXHOSTNAMELEN from <sys/param.h>, but
	   MAXHOSTNAMELEN isn't in there on Solaris */ 
	host_nm_lcl=(char *)malloc((256+1)*sizeof(char));
	(void)gethostname(host_nm_lcl,256+1);
	if(strchr(host_nm_lcl,'.') == NULL){
	  /* The returned hostname did not include the full Internet domain name */ 
	  (void)res_init();
	  (void)strcat(host_nm_lcl,".");
	  (void)strcat(host_nm_lcl,_res.defdname);
	} /* end if */ 

	/* Add one for the joining "@" and one for the NULL byte */ 
	usr_email=(char *)malloc((strlen(usr_nm)+1+strlen(host_nm_lcl)+1)*sizeof(char));
	(void)sprintf(usr_email,"%s@%s",usr_nm,host_nm_lcl);
	/* Free the hostname space */ 
	(void)free(host_nm_lcl);

	/* The remote hostname begins directly after "ftp://" */ 
	host_nm_rmt=fl_nm_rmt+6;
	/* The filename begins right after the slash */ 
	fl_nm_rmt=strstr(fl_nm_rmt+6,"/")+1;
	/* NUL-terminate the hostname */
	*(fl_nm_rmt-1)='\0';
	
	/* Subtract the four characters replaced by new strings, and add one for the NULL byte */ 
	fmt=(char *)malloc((strlen(fmt_template)+strlen(host_nm_rmt)+strlen(usr_email)-4+1)*sizeof(char));
	(void)sprintf(fmt,fmt_template,host_nm_rmt,usr_email,"%s","%s");
	rmt_cmd->fmt=fmt;
	/* Free the space holding the user's E-mail address */ 
	(void)free(usr_email);
#endif /* not WIN32 */
      } /* end if */
    } /* end if */

    /* Otherwise, a single colon preceded by a period in the filename unambiguously signals to use rcp or scp */
    /* Determining whether to try scp instead of rcp is difficult
       Ideally, NCO would test remote machine for rcp/scp priveleges with a system command like, e.g., "ssh echo ok"
    */
    if(rmt_cmd == NULL){
      if((cln_ptr=strchr(fl_nm_rmt,':')))
	if(((cln_ptr-4 >= fl_nm_rmt) && *(cln_ptr-4) == '.') ||
	   ((cln_ptr-3 >= fl_nm_rmt) && *(cln_ptr-3) == '.'))
	  rmt_cmd=&rcp;
    } /* end if */
    
    if(rmt_cmd == NULL){
      /* Does the msrcp command exist on the local system? */ 
      rcd=stat("/usr/local/bin/msrcp",&stat_sct); /* SCD Dataproc, Ouray */
      if(rcd != 0) rcd=stat("/usr/bin/msrcp",&stat_sct); /* ACD Linux */
      if(rcd != 0) rcd=stat("/opt/local/bin/msrcp",&stat_sct); /* CGD */
      if(rcd != 0) rcd=stat("/usr/local/dcs/bin/msrcp",&stat_sct); /* ACD */
      if(rcd == 0) rmt_cmd=&msrcp;
    } /* end if */
	
    if(rmt_cmd == NULL){
      /* Does the msread command exist on the local system? */ 
      rcd=stat("/usr/local/bin/msread",&stat_sct);
      if(rcd == 0) rmt_cmd=&msread;
    } /* end if */
	
    if(rmt_cmd == NULL){
      /* Does the nrnet command exist on the local system? */ 
      rcd=stat("/usr/local/bin/nrnet",&stat_sct);
      if(rcd == 0) rmt_cmd=&nrnet;
    } /* end if */

    /* Before we look on the remote system for the filename, make sure 
       the filename has the correct syntax to exist on the remote system */
    if(rmt_cmd == &msread || rmt_cmd == &nrnet || rmt_cmd == &msrcp){
      if (fl_nm_rmt[0] != '/' || fl_nm_rmt[1] < 'A' || fl_nm_rmt[1] > 'Z'){
	(void)fprintf(stderr,"%s: ERROR %s is not on local filesystem and is not a syntactically valid filename on remote file system\n",prg_nm_get(),fl_nm_rmt);
	exit(EXIT_FAILURE);
      } /* end if */
    } /* end if */
    
    if(rmt_cmd == NULL){
      (void)fprintf(stderr,"%s: ERROR unable to determine method for remote retrieval of %s\n",prg_nm_get(),fl_nm_rmt);
      exit(EXIT_FAILURE);
    } /* end if */

    /* Find the path for storing the local file */
    fl_nm_stub=strrchr(fl_nm_lcl,'/')+1;
    /* Construct the local storage filepath name */ 
    fl_pth_lcl_len=strlen(fl_nm_lcl)-strlen(fl_nm_stub)-1;
    /* Allocate enough room for the terminating NUL */ 
    fl_pth_lcl_tmp=(char *)malloc((fl_pth_lcl_len+1)*sizeof(char));
    (void)strncpy(fl_pth_lcl_tmp,fl_nm_lcl,fl_pth_lcl_len);
    fl_pth_lcl_tmp[fl_pth_lcl_len]='\0';
    
    /* Warn user when local filepath was machine-derived from remote name */
    if(fl_pth_lcl == NULL) (void)fprintf(stderr,"%s: WARNING deriving local filepath from remote filename, using %s\n",prg_nm_get(),fl_pth_lcl_tmp);

    /* Does local filepath already exist on local system? */
    rcd=stat(fl_pth_lcl_tmp,&stat_sct);
    /* If not, then create the local filepath */
    if(rcd != 0){
      /* Allocate enough room for joining space ' ' and terminating NUL */
      cmd_sys=(char *)malloc((strlen(cmd_mkdir)+fl_pth_lcl_len+2)*sizeof(char));
      (void)strcpy(cmd_sys,cmd_mkdir);
      (void)strcat(cmd_sys," ");
      (void)strcat(cmd_sys,fl_pth_lcl_tmp);
      if(dbg_lvl_get() > 0) (void)fprintf(stderr,"%s: Creating local directory %s with %s\n",prg_nm_get(),fl_pth_lcl_tmp,cmd_sys);
      rcd=system(cmd_sys); 
      if(rcd != 0){
	(void)fprintf(stderr,"%s: ERROR Unable to create local directory %s\n",prg_nm_get(),fl_pth_lcl_tmp);
	if(fl_pth_lcl == NULL) (void)fprintf(stderr,"%s: HINT Use -l option\n",prg_nm_get());
	exit(EXIT_FAILURE);
      } /* end if */
      /* Free local command space */ 
      (void)free(cmd_sys);
    } /* end if */ 

    /* Free local path space, if any */ 
    if(fl_pth_lcl_tmp != NULL) (void)free(fl_pth_lcl_tmp);

    /* Allocate enough room for joining space ' ' and terminating NUL */ 
    cmd_sys=(char *)malloc((strlen(rmt_cmd->fmt)-rmt_cmd->nbr_fmt_char+strlen(fl_nm_lcl)+strlen(fl_nm_rmt)+2)*sizeof(char));
    if(rmt_cmd->file_order == lcl_rmt){
      (void)sprintf(cmd_sys,rmt_cmd->fmt,fl_nm_lcl,fl_nm_rmt);
    }else{
      (void)sprintf(cmd_sys,rmt_cmd->fmt,fl_nm_rmt,fl_nm_lcl);
    } /* end else */
    if(dbg_lvl_get() > 0) (void)fprintf(stderr,"%s: Retrieving file from remote location:\n%s",prg_nm_get(),cmd_sys);
    (void)fflush(stderr);
    /* Fetch file from remote file system */ 
    rcd=system(cmd_sys);
    /* Free local command space */ 
    (void)free(cmd_sys);

    /* Free ftp script, which is the only dynamically allocated command */ 
    if(rmt_cmd == &ftp) (void)free(rmt_cmd->fmt);
   
    if(rmt_cmd->transfer_mode == synchronous){
      if(dbg_lvl_get() > 0) (void)fprintf(stderr,"\n");
      if(rcd != 0){
	(void)fprintf(stderr,"%s: ERROR Synchronous fetch command failed\n",prg_nm_get());
	exit(EXIT_FAILURE);
      } /* end if */
    }else{
      /* This is the appropriate place to insert a shell script invocation 
	 of a command to retrieve the file asynchronously and return the 
	 status to the NCO synchronously. */ 

      int fl_sz_crr=-2;
      int fl_sz_ntl=-1;
      int nbr_tm=100; /* Maximum number of sleep periods before error exit */ 
      int tm_idx;
      int tm_sleep=10; /* Seconds per stat() check for successful return */ 

      /* Asynchronous retrieval uses a sleep and poll technique */ 
      for(tm_idx=0;tm_idx<nbr_tm;tm_idx++){
	rcd=stat(fl_nm_lcl,&stat_sct);
	if(rcd == 0){
	  /* What is current size of file? */ 
	  fl_sz_ntl=fl_sz_crr;
	  fl_sz_crr=stat_sct.st_size;
	  /* If size of file has not changed for an entire sleep period, assume 
	     file is completely retrieved. */ 
	  if(fl_sz_ntl == fl_sz_crr){
	    break;
	  } /* end if */
	} /* end if */
	/* Sleep for specified time */ 
	(void)sleep((unsigned)tm_sleep);
	if(dbg_lvl_get() > 0) (void)fprintf(stderr,".");
	(void)fflush(stderr);
      } /* end for */ 
      if(tm_idx == nbr_tm){
	(void)fprintf(stderr,"%s: ERROR Maximum time (%d seconds = %.1f minutes) for asynchronous file retrieval exceeded.\n",prg_nm_get(),nbr_tm*tm_sleep,nbr_tm*tm_sleep/60.);
	exit(EXIT_FAILURE);
      } /* end if */
      if(dbg_lvl_get() > 0) (void)fprintf(stderr,"\n%s Retrieval successful after %d sleeps of %d seconds each = %.1f minutes\n",prg_nm_get(),tm_idx,tm_sleep,tm_idx*tm_sleep/60.);
    } /* end else transfer mode is asynchronous */
    *FILE_RETRIEVED_FROM_REMOTE_LOCATION=True;
  }else{ /* end if input file did not exist locally */
    *FILE_RETRIEVED_FROM_REMOTE_LOCATION=False;
  } /* end if file was already on the local system */ 

  /* Make sure we have read permission on local file */
  if((fp_in=fopen(fl_nm_lcl,"r")) == NULL){
    (void)fprintf(stderr,"%s: ERROR User does not have read permission for %s\n",prg_nm_get(),fl_nm_lcl);
    exit(EXIT_FAILURE);
  }else{
    (void)fclose(fp_in);
  } /* end else */ 
  
  /* Free input filename space */ 
  (void)free(fl_nm);

  /* Return the local filename */ 
  return(fl_nm_lcl);

} /* end fl_mk_lcl() */ 

/* indexx() is from Numerical Recipes. It computes an index table which 
   sorts the input array into ascending order. I have made the arrin argument 
   and the local variable q integers for netCDF purposes. */ 
/* NB: Many Numerical Recipes routines, including this one, employ "one-based" arrays */ 
void indexx(int n,int *arrin,int *indx)
/*     int n,indx[];*/
/*     float arrin[];*/
/*     int arrin[];*/
{
  int l,j,ir,indxt,i;
/*  float q;*/
  int q;
  
  for (j=1;j<=n;j++) indx[j]=j;
  l=(n >> 1) + 1;
  ir=n;
  for (;;) {
    if (l > 1)
      q=arrin[(indxt=indx[--l])];
    else {
      q=arrin[(indxt=indx[ir])];
      indx[ir]=indx[1];
      if (--ir == 1) {
	indx[1]=indxt;
	return;
      }
    }
    i=l;
    j=l << 1;
    while (j <= ir) {
      if (j < ir && arrin[indx[j]] < arrin[indx[j+1]]) j++;
      if (q < arrin[indx[j]]) {
	indx[i]=indx[j];
	j += (i=j);
      }
      else j=ir+1;
    }
    indx[i]=indxt;
  }
} /* end indexx() */ 

void index_alpha(int n,char **arrin,int *indx)
     /* This is indexx() from Numerical recipes hacked to alphabetize a list of strings */
{
  int l,j,ir,indxt,i;
/*  float q;*/
  char *q;
  
  for (j=1;j<=n;j++) indx[j]=j;
  l=(n >> 1) + 1;
  ir=n;
  for (;;) {
    if (l > 1)
      q=arrin[(indxt=indx[--l])];
    else {
      q=arrin[(indxt=indx[ir])];
      indx[ir]=indx[1];
      if (--ir == 1) {
	indx[1]=indxt;
	return;
      }
    }
    i=l;
    j=l << 1;
    while (j <= ir) {
      /*      if (j < ir && arrin[indx[j]] < arrin[indx[j+1]]) j++;*/
      if (j < ir && strcmp(arrin[indx[j]],arrin[indx[j+1]]) < 0) j++;
      /*      if (q < arrin[indx[j]]) {*/
      if (strcmp(q,arrin[indx[j]]) < 0) {
	indx[i]=indx[j];
	j += (i=j);
      }
      else j=ir+1;
    }
    indx[i]=indxt;
  }
} /* end index_alpha() */ 

char *
cvs_vrs_prs()
{
  /* Purpose: Return CVS version string */ 
  bool dly_snp;

  char *cvs_mjr_vrs_sng=NULL;
  char *cvs_mnr_vrs_sng=NULL;
  char *cvs_nm_ptr=NULL;
  char *cvs_nm_sng=NULL;
  char *cvs_pch_vrs_sng=NULL;
  char *cvs_vrs_sng=NULL;
  char *dlr_ptr=NULL;
  char *nco_sng_ptr=NULL;
  char *usc_1_ptr=NULL;
  char *usc_2_ptr=NULL;
  char cvs_Name[]="$Name: not supported by cvs2svn $";
  char nco_sng[]="nco"; 

  int cvs_nm_sng_len;
  int cvs_vrs_sng_len;
  int cvs_mjr_vrs_len;
  int cvs_mnr_vrs_len;
  int cvs_pch_vrs_len;
  int nco_sng_len;
  
  long cvs_mjr_vrs=-1L;
  long cvs_mnr_vrs=-1L;
  long cvs_pch_vrs=-1L;

  /* Is cvs_Name keyword expanded? */ 
  dlr_ptr=strstr(cvs_Name," $");
  if(dlr_ptr == NULL)(void)fprintf(stderr,"%s: WARNING cvs_vrs_prs() reports dlr_ptr == NULL\n%s: HINT Make sure CVS export uses -kkv\n",prg_nm_get(),prg_nm_get());
  cvs_nm_ptr=strstr(cvs_Name,"$Name: ");
  if(cvs_nm_ptr == NULL)(void)fprintf(stderr,"%s: WARNING cvs_vrs_prs() reports cvs_nm_ptr == NULL\n%s: HINT Make sure CVS export uses -kkv\n",prg_nm_get(),prg_nm_get());
  cvs_nm_sng_len=(int)(dlr_ptr-cvs_nm_ptr-7); /* 7 is strlen("$Name: ") */ 
  if(cvs_nm_sng_len > 0) dly_snp=False; else dly_snp=True;

  /* If not, this is a daily snapshot so use YYYYMMDD date for version string */ 
  if(dly_snp){
    int mth;
    int day;
    int yr;
    struct tm *gmt_tm;
    time_t clock;

    clock=time((time_t *)NULL);
    gmt_tm=gmtime(&clock); 
    /* localtime() gives YYYYMMDD in MDT, but this conflicts with CVS, which uses GMT */ 
    /*    gmt_tm=localtime(&clock); */

    mth=gmt_tm->tm_mon+1;
    day=gmt_tm->tm_mday;
    yr=gmt_tm->tm_year+1900;

    cvs_vrs_sng_len=4+2+2;
    cvs_vrs_sng=(char *)malloc(cvs_vrs_sng_len+1);
    (void)sprintf(cvs_vrs_sng,"%04i%02i%02i",yr,mth,day);
    return cvs_vrs_sng;
  } /* endif dly_snp */ 

  /* cvs_nm_sng is, e.g., "nco1_1" */ 
  cvs_nm_sng=(char *)malloc(cvs_nm_sng_len+1);
  strncpy(cvs_nm_sng,cvs_Name+7,cvs_nm_sng_len); /* 7 is strlen("$Name: ") */
  cvs_nm_sng[cvs_nm_sng_len]='\0';

  /* cvs_vrs_sng is, e.g., "1.1" */ 
  nco_sng_len=strlen(nco_sng);
  nco_sng_ptr=strstr(cvs_nm_sng,nco_sng);
  if(nco_sng_ptr == NULL)(void)fprintf(stderr,"%s: WARNING cvs_vrs_prs() reports nco_sng_ptr == NULL\n",prg_nm_get());
  usc_1_ptr=strstr(cvs_nm_sng,"_");
  if(usc_1_ptr == NULL)(void)fprintf(stderr,"%s: WARNING cvs_vrs_prs() reports usc_1_ptr == NULL\n",prg_nm_get());
  cvs_mjr_vrs_len=(int)(usc_1_ptr-cvs_nm_sng)-nco_sng_len; /* NB: cast pointer to int before subtracting */ 
  usc_2_ptr=strstr(usc_1_ptr+1,"_");
  cvs_mjr_vrs_sng=(char *)malloc(cvs_mjr_vrs_len+1);
  cvs_mjr_vrs_sng=strncpy(cvs_mjr_vrs_sng,cvs_nm_sng+nco_sng_len,cvs_mjr_vrs_len);
  cvs_mjr_vrs_sng[cvs_mjr_vrs_len]='\0';
  cvs_mjr_vrs=strtol(cvs_mjr_vrs_sng,(char **)NULL,10);
  if(usc_2_ptr == NULL){
    cvs_mnr_vrs_len=cvs_nm_sng_len-cvs_mjr_vrs_len-1;
    cvs_pch_vrs_len=0;
    cvs_vrs_sng_len=cvs_mjr_vrs_len+1+cvs_mnr_vrs_len;
  }else{
    cvs_mnr_vrs_len=usc_2_ptr-usc_1_ptr-1;
    cvs_pch_vrs_len=cvs_nm_sng_len-cvs_mjr_vrs_len-1-cvs_mnr_vrs_len-1;
    cvs_vrs_sng_len=cvs_mjr_vrs_len+1+cvs_mnr_vrs_len+1+cvs_pch_vrs_len;
  } /* end else */ 
  cvs_mnr_vrs_sng=(char *)malloc(cvs_mnr_vrs_len+1);
  cvs_mnr_vrs_sng=strncpy(cvs_mnr_vrs_sng,usc_1_ptr+1,cvs_mnr_vrs_len);
  cvs_mnr_vrs_sng[cvs_mnr_vrs_len]='\0';
  cvs_mnr_vrs=strtol(cvs_mnr_vrs_sng,(char **)NULL,10);

  cvs_pch_vrs_sng=(char *)malloc(cvs_pch_vrs_len+1);
  cvs_pch_vrs_sng[cvs_pch_vrs_len]='\0';
  cvs_vrs_sng=(char *)malloc(cvs_vrs_sng_len+1);
  if(usc_2_ptr != NULL){
    cvs_pch_vrs_sng=strncpy(cvs_pch_vrs_sng,usc_2_ptr+1,cvs_pch_vrs_len);
    cvs_pch_vrs=strtol(cvs_pch_vrs_sng,(char **)NULL,10);
    (void)sprintf(cvs_vrs_sng,"%li.%li.%li",cvs_mjr_vrs,cvs_mnr_vrs,cvs_pch_vrs);
  }else{
    (void)sprintf(cvs_vrs_sng,"%li.%li",cvs_mjr_vrs,cvs_mnr_vrs);
  }/* end else */ 

  if(dbg_lvl_get() == 4){
    (void)fprintf(stderr,"NCO version %s\n",cvs_vrs_sng);
    (void)fprintf(stderr,"cvs_nm_sng %s\n",cvs_nm_sng);
    (void)fprintf(stderr,"cvs_mjr_vrs_sng %s\n",cvs_mjr_vrs_sng);
    (void)fprintf(stderr,"cvs_mnr_vrs_sng %s\n",cvs_mnr_vrs_sng);
    (void)fprintf(stderr,"cvs_pch_vrs_sng %s\n",cvs_pch_vrs_sng);
    (void)fprintf(stderr,"cvs_mjr_vrs %li\n",cvs_mjr_vrs);
    (void)fprintf(stderr,"cvs_mnr_vrs %li\n",cvs_mnr_vrs);
    (void)fprintf(stderr,"cvs_pch_vrs %li\n",cvs_pch_vrs);
  } /* endif dbg */ 

  (void)free(cvs_mjr_vrs_sng);
  (void)free(cvs_mnr_vrs_sng);
  (void)free(cvs_pch_vrs_sng);
  (void)free(cvs_nm_sng);

  return cvs_vrs_sng;
} /* end cvs_vrs_prs() */ 

void
nc_lib_vrs_prn()
{
  /* Purpose: Print netCDF library version */ 

  char *lib_sng;
  char *nst_sng;
  char *vrs_sng;
  char *of_ptr;
  char *dlr_ptr;

  int vrs_sng_len;
  int nst_sng_len;

  /* Ability to compile without netCDF 3.x calls is still valuable because
     HDF 4.x only supports netCDF 2.x library. */      
#ifndef NETCDF2_ONLY
  /* As of netCDF 3.4, nc_inq_libvers() returned strings such as "3.4 of May 16 1998 14:06:16 $" */   
  lib_sng=(char *)strdup(nc_inq_libvers());
  of_ptr=strstr(lib_sng," of ");
  if(of_ptr == NULL)(void)fprintf(stderr,"%s: WARNING nc_lib_vrs_prn() reports of_ptr == NULL\n",prg_nm_get());
  vrs_sng_len=(int)(of_ptr-lib_sng);
  vrs_sng=(char *)malloc(vrs_sng_len+1);
  strncpy(vrs_sng,lib_sng,vrs_sng_len);
  vrs_sng[vrs_sng_len]='\0';

  dlr_ptr=strstr(lib_sng," $");
  if(dlr_ptr == NULL)(void)fprintf(stderr,"%s: WARNING nc_lib_vrs_prn() reports dlr_ptr == NULL\n",prg_nm_get());
  nst_sng_len=(int)(dlr_ptr-of_ptr-4); /* 4 is the length of " of " */ 
  nst_sng=(char *)malloc(nst_sng_len+1);
  strncpy(nst_sng,of_ptr+4,nst_sng_len); /* 4 is the length of " of " */ 
  nst_sng[nst_sng_len]='\0';

  (void)fprintf(stderr,"Linked to netCDF library version %s, compiled %s\n",vrs_sng,nst_sng);
  (void)free(vrs_sng);
  (void)free(lib_sng);
  (void)free(nst_sng);
#endif /* NETCDF2_ONLY */ 
  (void)fprintf(stdout,"NCO homepage URL is http://www.cgd.ucar.edu/cms/nco\n");

} /* end nc_lib_vrs_prn() */

void
copyright_prn(char *CVS_Id,char *CVS_Revision)
/* 
   char *CVS_Id: I [sng] CVS identification string
   char *CVS_Revision: I [sng] CVS revision string
 */ 
{
  char *date_sng;
  char *vrs_sng;
  char *cvs_vrs_sng;

  int date_sng_len;
  int vrs_sng_len;
  
  if(strlen(CVS_Id) > strlen("*Id*")){
    /* CVS_Id is defined */
    date_sng_len=10;
    date_sng=(char *)malloc((date_sng_len+1)*sizeof(char));
    (void)strncpy(date_sng,strchr(CVS_Id,'/')-4,date_sng_len);
    date_sng[date_sng_len]='\0';
  }else{
    /* CVS_Id is undefined */
    date_sng=(char *)strdup("Current");
  } /* endif */

  if(strlen(CVS_Revision) > strlen("*Revision*") || strlen(CVS_Revision) < strlen("*Revision*")){
    /* CVS_Revision is defined */
    vrs_sng_len=strrchr(CVS_Revision,'$')-strchr(CVS_Revision,':')-3;
    vrs_sng=(char *)malloc((vrs_sng_len+1)*sizeof(char));
    (void)strncpy(vrs_sng,strchr(CVS_Revision,':')+2,vrs_sng_len);
    vrs_sng[vrs_sng_len]='\0';
  }else{
    /* CVS_Revision is undefined */
    vrs_sng=(char *)strdup("Current");
  } /* endif */

  cvs_vrs_sng=cvs_vrs_prs();

  (void)fprintf(stderr,"NCO netCDF Operators version %s by Charlie Zender\n",cvs_vrs_sng);
  (void)fprintf(stderr,"%s version %s (%s) \"%s\"\nCopyright 1995--1999 University Corporation for Atmospheric Research\n",prg_nm_get(),vrs_sng,date_sng,nmn_get());
  (void)fprintf(stderr,"Portions copyright 1999 Regents of the University of California\n");
  (void)free(vrs_sng);
  (void)free(cvs_vrs_sng);
} /* end copyright_prn() */

void
fl_cp(char *fl_src,char *fl_dst)
/* 
   char *fl_src: I [sng] Name of the source file to copy
   char *fl_dst: I [sng] Name of the destination file
 */ 
{
  /* Routine to copy the first file to the second */ 

  char *cp_cmd;
  char cp_cmd_fmt[]="cp %s %s";

  int rcd;
  int nbr_fmt_char=4;
  
  /* Construct and execute the copy command */ 
  cp_cmd=(char *)malloc((strlen(cp_cmd_fmt)+strlen(fl_src)+strlen(fl_dst)-nbr_fmt_char+1)*sizeof(char));
  if(dbg_lvl_get() > 0) (void)fprintf(stderr,"Copying %s to %s...",fl_src,fl_dst);
  (void)sprintf(cp_cmd,cp_cmd_fmt,fl_src,fl_dst);
  rcd=system(cp_cmd);
  if(rcd == -1){
    (void)fprintf(stdout,"%s: ERROR fl_cp() is unable to execute cp command \"%s\"\n",prg_nm_get(),cp_cmd);
    exit(EXIT_FAILURE); 
  } /* end if */ 
  (void)free(cp_cmd);
  if(dbg_lvl_get() > 0) (void)fprintf(stderr,"done\n");
  
} /* end fl_cp() */ 

void
fl_mv(char *fl_src,char *fl_dst)
/* 
   char *fl_src: I name of the file to move
   char *fl_dst: I name of the destination file
 */ 
{
  /* Routine to move the first file to the second */ 

  char *mv_cmd;
  char mv_cmd_fmt[]="mv -f %s %s";

  int rcd;
  int nbr_fmt_char=4;
  
  /* Construct and execute the copy command */ 
  mv_cmd=(char *)malloc((strlen(mv_cmd_fmt)+strlen(fl_src)+strlen(fl_dst)-nbr_fmt_char+1)*sizeof(char));
  if(dbg_lvl_get() > 0) (void)fprintf(stderr,"Moving %s to %s...",fl_src,fl_dst);
  (void)sprintf(mv_cmd,mv_cmd_fmt,fl_src,fl_dst);
  rcd=system(mv_cmd);
  if(rcd == -1){
    (void)fprintf(stdout,"%s: ERROR fl_mv() is unable to execute mv command \"%s\"\n",prg_nm_get(),mv_cmd);
    exit(EXIT_FAILURE); 
  } /* end if */ 
  (void)free(mv_cmd);
  if(dbg_lvl_get() > 0) (void)fprintf(stderr,"done\n");
  
} /* end fl_mv() */ 

void 
fl_rm(char *fl_nm)
/* 
   char *fl_nm: I file to be removed
 */ 
{
  /* Purpose: Remove specified file from local system */ 

  int rcd;
  char rm_cmd_sys_dep[]="rm -f";
  char *rm_cmd;
  
  /* Remember to add one for the space and one for the terminating NUL character */ 
  rm_cmd=(char *)malloc((strlen(rm_cmd_sys_dep)+1+strlen(fl_nm)+1)*sizeof(char));
  (void)sprintf(rm_cmd,"%s %s",rm_cmd_sys_dep,fl_nm);

  if(dbg_lvl_get() > 0) (void)fprintf(stderr,"%s: Removing %s with %s\n",prg_nm_get(),fl_nm,rm_cmd);
  rcd=system(rm_cmd);
  if(rcd == -1) (void)fprintf(stderr,"%s: WARNING unable to remove %s, continuing anyway...\n",prg_nm_get(),fl_nm);

  (void)free(rm_cmd);

} /* end fl_rm() */ 

int
sng_ascii_trn(char *sng)
     /* 
	char *sng: I/O [sng] String to process
	int sng_ascii_trn: O [nbr] Number of escape sequences translated
     */
{
  /* Purpose: Replace any C language '\X' escape codes in a string into ASCII bytes 
     Return the number of escape sequences found and actually translated
     This should be the same as the number of bytes by which the string length has shrunk
     For example, the consecutive characters "\n" are translated into ASCII '\n' = 10 which diminishes the string length by 1
     Function works for an arbitrary number of escape codes in the input string
     The escape sequence for NUL itself, \0, causes a warning and is not translated
     Input string must be NUL-terminated or NULL
     Translation is done in place, so if the original string is required, it should be copied prior to calling sng_ascii_trn()
     This procedure can only diminish, not lengthen, the size of the input string
     Therefore it may safely be performed in place without the need to operate on a copy of the string
     The address pointed to by sng does not change, but the memory at that address is altered
     when characters are "moved to the left" if C language escape sequences are embedded.
     Thus the length of the string may shrink
   */ 

  bool trn_flg; /* Translate this escape sequence */

  char *backslash_ptr; /* [ptr] Pointer to backslash character */
  char backslash_chr='\\'; /* [chr] Backslash character */

  int esc_sqn_nbr=0; /* Number of escape sequences found */
  int trn_nbr=0; /* Number of escape sequences translated */
  
  /* ncatted allows character attributes of 0 length
     Such "strings" do not even have a NUL-terminator and so may not safely be tested by strchr() */
  if(sng == NULL) return trn_nbr;
  
  /* C language '\X' escape codes are always preceded by a backslash */
  /* Check if control codes are embedded once before entering loop */ 
  backslash_ptr=strchr(sng,backslash_chr);

  while(backslash_ptr != NULL){
    /* Default is to translate this escape sequence */
    trn_flg=True;
    /* Replace backslash character by corresponding control code */ 
    switch(*(backslash_ptr+1)){ /* man ascii:Oct   Dec   Hex   Char \X  */
    case 'a':	*backslash_ptr='\a'; break; /* 007   7     07    BEL '\a' Bell */
    case 'b':	*backslash_ptr='\b'; break; /* 010   8     08    BS  '\b' Backspace */
    case 'f':	*backslash_ptr='\f'; break; /* 014   12    0C    FF  '\f' Formfeed */
    case 'n':	*backslash_ptr='\n'; break; /* 012   10    0A    LF  '\n' Linefeed */
    case 'r':	*backslash_ptr='\r'; break; /* 015   13    0D    CR  '\r' Carriage return */
    case 't':	*backslash_ptr='\t'; break; /* 011   9     09    HT  '\t' Horizontal tab */
    case 'v':	*backslash_ptr='\v'; break; /* 013   11    0B    VT  '\v' Vertical tab */
    case '\\': *backslash_ptr='\\'; break; /* 134   92    5C    \   '\\' */
    case '\?': *backslash_ptr='\?'; break; /*  */
    case '\'': *backslash_ptr='\''; break; /*  */
    case '\"': *backslash_ptr='\"'; break; /*  */
      /* Do not translate \0 to NUL since this would erase the rest of the string */
    case '0':	
      /* Do not translate \0 to NUL since this would make the rest of the string invisible to all string functions */
      /* *backslash_ptr='\0'; */ /* 000   0     00    NUL '\0' */
      (void)fprintf(stderr,"%s: WARNING C language escape code %.2s found in string, not translating to NUL since this would make the rest of the string invisible to all string functions\n",prg_nm_get(),backslash_ptr);
      trn_flg=False;
      break;
    default: 
      (void)fprintf(stderr,"%s: WARNING No ASCII equivalent to possible C language escape code %.2s so no action taken\n",prg_nm_get(),backslash_ptr);
      trn_flg=False;
      break;
    } /* end switch */ 
    if(trn_flg){
      /* Get rid of character after backslash character */ 
      (void)memmove(backslash_ptr+1,backslash_ptr+2,(strlen(backslash_ptr+2)+1)*sizeof(char));
      /* Count translations performed */ 
      trn_nbr++;
    } /* end if */
    /* Look for next backslash starting at character following current escape sequence (but remember that not all escape sequences are translated) */
    if (trn_flg) backslash_ptr=strchr(backslash_ptr+1,backslash_chr); else backslash_ptr=strchr(backslash_ptr+2,backslash_chr);
    /* Count escape sequences */ 
    esc_sqn_nbr++;
  } /* end if */ 

  /* Usually there are no escape codes and sng still equals input value */ 
  if(dbg_lvl_get() > 2) (void)fprintf(stderr,"%s: DEBUG sng_ascii_trn() Found %d C-language escape sequences, translated %d of them\n",prg_nm_get(),esc_sqn_nbr,trn_nbr);

  return trn_nbr;

} /* end sng_ascii_trn() */ 

