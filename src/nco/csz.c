/* $Header: /data/zender/nco_20150216/nco/src/nco/csz.c,v 1.81 2002-01-28 10:06:53 zender Exp $ */

/* Purpose: Standalone utilities for C programs (no netCDF required) */

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

#include <sys/types.h> /* needed for _res */
#include <netinet/in.h> /* needed for _res */
#include <pwd.h> /* password structures for getpwuid() */
#ifndef WIN32
#include <arpa/nameser.h> /* needed for _res */
#include <resolv.h> /* Internet structures for _res */
#endif

/* 3rd party vendors */
#ifdef _OPENMP
#include <omp.h> /* OpenMP pragmas */
#endif /* not _OPENMP */

/* I'm only keeping these netCDF include files around because I'm worried that 
   function prototypes in nco.h are needed here. Eventually prototypes for these
   routines should be broken into separate files, like csz.h... */
#include <netcdf.h> /* netCDF definitions */
#include "nco_netcdf.h" /* netCDF3.0 wrapper functions */
#include "nco.h" /* netCDF operator definitions */

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
  time_bfr_end=ctime(&clock);  time_bfr_end=time_bfr_end; /* Avoid compiler warning until variable is used for something */
/*  (void)fprintf(stderr,"\tend = %s\n",time_bfr_end);*/

  (void)fclose(stderr);
  (void)fclose(stdin);
  (void)fclose(stdout);

  exit(EXIT_SUCCESS);
} /* end Exit_gracefully() */

char * /* O [sng] Mnemonic that describes current NCO version */
nmn_get()
{ 
  /* Purpose: Return mnemonic that describes current NCO version */
  return "RIP George Harrison";
} /* end nmn_get() */

char * /* O [sng] Parsed command line */
cmd_ln_sng /* [fnc] Re-construct command line from arguments */
(int argc, /* I [nbr] Argument count */
 char **argv) /* I [sng] Command line argument values */
{
  /* Purpose: Re-construct command line from argument list and number */
  char *cmd_ln; /* [sng] Parsed command line */
  
  int cmd_ln_sz=0;
  int idx;

  for(idx=0;idx<argc;idx++){
    cmd_ln_sz+=(int)strlen(argv[idx])+1;
  } /* end loop over args */
  cmd_ln=(char *)nco_malloc(cmd_ln_sz*sizeof(char));
  if(argc <= 0){
    cmd_ln=(char *)nco_malloc(sizeof(char));
    cmd_ln[0]='\0';
  }else{
    (void)strcpy(cmd_ln,argv[0]);
    for(idx=1;idx<argc;idx++){
      (void)strcat(cmd_ln," ");
      (void)strcat(cmd_ln,argv[idx]);
    } /* end loop over args */
  } /* end else */

  return cmd_ln; /* [sng] Parsed command line */
} /* end cmd_ln_sng() */

lmt_sct *
lmt_prs(int lmt_nbr,char **lmt_arg)
/* 
   int lmt_nbr: I number of dimensions with limits
   char **lmt_arg: I list of user-specified dimension limits
   lmt_sct *lmt_prs(): O structure holding user-specified strings for min and max limits
 */
{
  /* Purpose: Set name, min_sng, max_sng elements of 
     a comma separated list of names and ranges. This routine
     merely evaluates syntax of input expressions and
     does not attempt to validate dimensions or their ranges
     against those present in input netCDF file. */

  /* Valid syntax adheres to nm,[min_sng][,[max_sng]][,srd_sng] */

  void usg_prn(void);

  char **arg_lst;

  char *dlm_sng=",";

  lmt_sct *lmt=NULL_CEWI;

  int idx;
  int arg_nbr;

  if(lmt_nbr > 0) lmt=(lmt_sct *)nco_malloc(lmt_nbr*sizeof(lmt_sct));

  for(idx=0;idx<lmt_nbr;idx++){

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
    /* lmt strings which are not explicitly set by user remain NULL, i.e., 
       specifying default setting will appear as if nothing at all was set.
       Hopefully, in routines that follow, branch followed by a dimension for which
       all default settings were specified (e.g.,"-d foo,,,,") will yield same answer
       as branch for which no hyperslab along that dimension was set.
     */
    lmt[idx].nm=NULL;
    lmt[idx].is_usr_spc_lmt=True; /* True if any part of limit is user-specified, else False */
    lmt[idx].min_sng=NULL;
    lmt[idx].max_sng=NULL;
    lmt[idx].srd_sng=NULL;
    /* rec_skp_nsh_spf is used for record dimension in multi-file operators */
    lmt[idx].rec_skp_nsh_spf=0L; /* Number of records skipped in initial superfluous files */

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
  /* Routine takes list of strings and joins them together into one string
     Elements of input list should all be NUL-terminated strings
     Elements with the value NUL will be interpreted as strings of zero length
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
  /* Add one for NUL byte */
  sng=(char *)nco_malloc(sizeof(char)*(sng_sz+1));
  /* NUL-terminate string for safety */
  sng[0]='\0';
  for(lmn=0L;lmn<lmn_nbr;lmn++){
    /* List elements must be NUL-terminated (strings) so strcat() works */
    sng=(sng_lst[lmn] == NULL) ? sng : strcat(sng,sng_lst[lmn]);
    if(lmn != lmn_nbr-1 && dlm_len != 0) sng=strcat(sng,dlm_sng);
  } /* end loop over lmn */

  return sng;
} /* end sng_lst_prs() */

char ** /* O [sng] Array of list elements */
lst_prs /* [fnc] Create list of strings from given string and arbitrary delimiter */
(char *sng_in, /* I/O [sng] Delimited argument list (delimiters are changed to NULL on output */
 const char *dlm_sng, /* I [sng] delimiter string */
 int *nbr_lst) /* O [nbr] number of elements in list */
{
  /* Purpose: Create list of strings from given string and arbitrary delimiter
     Routine is often called with system memory, e.g., with strings from
     command line arguments whose memory was allocated by shell or by getopt().
     A conservative policy would be, therefore, to never modify input string
     However, we are safe if any modifications do not extend input string
     Thus this routine is allowed to replace delimiter strings by NULs */

  /* Number of list members is always one more than number of delimiters, e.g.,
     foo,,3, has 4 arguments: "foo", "", "3" and "".
     A delimiter without an argument is valid syntax to indicate default argument
     Therefore a storage convention is necessary to indicate default argument was selected
     Either NULL or '\0' can be used without requiring additional flag
     NULL is not printable, but is useful as a logical flag since its value is False
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

  /* First element does not require delimiter in front of it */
  *nbr_lst=1;

  /* Count list members */
  while((sng_in_ptr=strstr(sng_in_ptr,dlm_sng))){
    sng_in_ptr+=dlm_len;
    (*nbr_lst)++;
  } /* end while */

  lst=(char **)nco_malloc(*nbr_lst*sizeof(char *));

  sng_in_ptr=sng_in; 
  lst[0]=sng_in;
  idx=0;
  while((sng_in_ptr=strstr(sng_in_ptr,dlm_sng))){
    /* NUL-terminate previous arg */
    *sng_in_ptr='\0';
    sng_in_ptr+=dlm_len;
    lst[++idx]=sng_in_ptr;
  } /* end while */

  /* Default list member is assumed when two delimiters are adjacent to eachother, 
     i.e., when length of string between delimiters is 0. 
     If list ends with delimiter, then last element of list is also assumed to be default list member. */
  /* This loop sets default list members to NULL */
  for(idx=0;idx<*nbr_lst;idx++)
    if(strlen(lst[idx]) == 0) lst[idx]=NULL;

  if(dbg_lvl_get() == 5){
    (void)fprintf(stderr,"%d elements in list delimited by \"%s\"\n",*nbr_lst,dlm_sng);
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
   const char *fl_nm: I/O [sng] Current filename, if any
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
  fl_nm=nco_free(fl_nm);

  /* Construct filename from NINTAP-style arguments and input name */
  if(fl_lst_abb != NULL){
    if(FIRST_INVOCATION){
      int fl_nm_sfx_len=0;
      
      /* Parse abbreviation list analogously to CCM Processor ICP "NINTAP" */
      if(nbr_fl != NULL) *nbr_fl=(int)strtol(fl_lst_abb[0],(char **)NULL,10);
      
      if(nbr_abb_arg > 1){
	fl_nm_nbr_dgt=(int)strtol(fl_lst_abb[1],(char **)NULL,10);
      }else{
	fl_nm_nbr_dgt=3;
      }/* end if */
      
      if(nbr_abb_arg > 2){
	fl_nm_nbr_ncr=(int)strtol(fl_lst_abb[2],(char **)NULL,10);
      }else{
	fl_nm_nbr_ncr=1;
      } /* end if */
      
      if(nbr_abb_arg > 3){
	fl_nm_nbr_max=(int)strtol(fl_lst_abb[3],(char **)NULL,10);
      }else{
	fl_nm_nbr_max=0;
      } /* end if */
      
      if(nbr_abb_arg > 4){
	fl_nm_nbr_min=(int)strtol(fl_lst_abb[4],(char **)NULL,10);
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
      fl_nm_nbr_sng=(char *)nco_malloc((fl_nm_nbr_dgt+1)*sizeof(char));
      fl_nm_nbr_sng=strncpy(fl_nm_nbr_sng,fl_nm_1st_dgt,fl_nm_nbr_dgt);
      fl_nm_nbr_sng[fl_nm_nbr_dgt]='\0';
      fl_nm_nbr_crr=(int)strtol(fl_nm_nbr_sng,(char **)NULL,10);
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
    fl_nm=(char *)nco_malloc((strlen(fl_nm_stub)+strlen(fl_pth)+2)*sizeof(char));
    (void)strcpy(fl_nm,fl_pth);
    (void)strcat(fl_nm,"/");
    (void)strcat(fl_nm,fl_nm_stub);

    /* Free filestub space */
    fl_nm_stub=nco_free(fl_nm_stub);
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
  /* Routine to locate input file, retrieve it from a remote storage system if necessary, 
     create local storage directory if neccessary, check file for read-access,
     return name of file on local system */

  FILE *fp_in;
  char *cln_ptr; /* [ptr] Colon pointer */
  char *fl_nm_lcl;
  char *fl_nm_stub;
  int rcd;
  struct stat stat_sct;
  
  /* Assume local filename is input filename */
  fl_nm_lcl=(char *)strdup(fl_nm);

  /* Remove any URL and machine-name components from local filename */
  if(strstr(fl_nm_lcl,"ftp://") == fl_nm_lcl){
    char *fl_nm_lcl_tmp;
    char *fl_pth_lcl_tmp;

    /* Rearrange fl_nm_lcl to get rid of ftp://hostname part */
    fl_pth_lcl_tmp=strchr(fl_nm_lcl+6,'/');
    fl_nm_lcl_tmp=fl_nm_lcl;
    fl_nm_lcl=(char *)nco_malloc(strlen(fl_pth_lcl_tmp)+1);
    (void)strcpy(fl_nm_lcl,fl_pth_lcl_tmp);
    fl_nm_lcl_tmp=nco_free(fl_nm_lcl_tmp);
  }else if(strstr(fl_nm_lcl,"http://") == fl_nm_lcl){

    /* If file is http protocol then pass file name on unaltered and let DODS deal with it */

  }else if((cln_ptr=strchr(fl_nm_lcl,':'))){
    /* 19990804
       A colon separates machine name from filename in rcp and scp requests
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
      fl_nm_lcl=(char *)nco_malloc(strlen(fl_pth_lcl_tmp)+1);
      (void)strcpy(fl_nm_lcl,fl_pth_lcl_tmp);
      fl_nm_lcl_tmp=nco_free(fl_nm_lcl_tmp);
    } /* endif period is three or four characters from colon */
  } /* end if */
  
  /* Does file exist on local system? */
  rcd=stat(fl_nm_lcl,&stat_sct);

  /* One exception: let DODS try to access remote HTTP protocol files as local files */
  if(strstr(fl_nm_lcl,"http://") == fl_nm_lcl) rcd=0;
  
  /* If not, check if file exists on local system under same path interpreted relative to current working directory */
  if(rcd == -1){
    if(fl_nm_lcl[0] == '/'){
      rcd=stat(fl_nm_lcl+1,&stat_sct);
    } /* end if */
    if(rcd == 0){
      char *fl_nm_lcl_tmp;
      
      /* NB: simply adding one to filename pointer is like deleting
	 the initial slash on the filename. Without copying the new name
	 into its own memory space, free(fl_nm_lcl) would not be able to free 
	 the initial byte. */
      fl_nm_lcl_tmp=(char *)strdup(fl_nm_lcl+1);
      fl_nm_lcl=nco_free(fl_nm_lcl);
      fl_nm_lcl=fl_nm_lcl_tmp;
      (void)fprintf(stderr,"%s: WARNING not searching for %s on remote filesystem, using local file %s instead\n",prg_nm_get(),fl_nm,fl_nm_lcl+1);
    } /* end if */
  } /* end if */
  
  /* Finally, check to see if file exists on local system in directory specified for storage of remotely retrieved files
     This would be the case if some files had already been retrieved in a previous invocation of the program */
  if(rcd == -1){
    /* Where does filename stub begin? NB: Assume local filename has a slash (because remote file system always has a slash) */
    fl_nm_stub=strrchr(fl_nm_lcl,'/')+1;

    /* Construct local filename from user-supplied local file path along with existing file stub */
    if(fl_pth_lcl != NULL){
      char *fl_nm_lcl_tmp;
      
      fl_nm_lcl_tmp=fl_nm_lcl;
      /* Allocate enough room for the joining slash '/' and the terminating NUL */
      fl_nm_lcl=(char *)nco_malloc((strlen(fl_pth_lcl)+strlen(fl_nm_stub)+2)*sizeof(char));
      (void)strcpy(fl_nm_lcl,fl_pth_lcl);
      (void)strcat(fl_nm_lcl,"/");
      (void)strcat(fl_nm_lcl,fl_nm_stub);
      /* Free the old filename space */
      fl_nm_lcl_tmp=nco_free(fl_nm_lcl_tmp);
    } /* end if */
    
    /* At last, check for file in the local storage directory */
    rcd=stat(fl_nm_lcl,&stat_sct);
    if (rcd != -1) (void)fprintf(stderr,"%s: WARNING not searching for %s on remote filesystem, using local file %s instead\n",prg_nm_get(),fl_nm,fl_nm_lcl);
  } /* end if */

  /* File was not found locally, try to fetch it from remote file system */
  if(rcd == -1){

    typedef struct{ /* [enm] Remote fetch command structure */
      char *fmt; /* [] Format */
      int nbr_fmt_char; /* [nbr] Number of formatting characters */
      int transfer_mode; /* [enm] Transfer mode */
      int file_order; /* [enm] File order */
    } rmt_fch_cmd_sct;

    char *cmd_sys;
    char *fl_nm_rmt;
    char *fl_pth_lcl_tmp=NULL;
    
#if ( ! defined SUN4 )
    char cmd_mkdir[]="mkdir -m 777 -p";
#else
    char cmd_mkdir[]="mkdir -p";
#endif /* SUN4 */

    enum{ /* [enm] Transfer mode */
      synchronous, /* Syncronous transfer */
      asynchronous}; /* Asynchronous transfer */

    enum{ /* [enm] File order */
      lcl_rmt, /* Local file argument before remote file argument */
      rmt_lcl}; /* Remote file argument before local file argument */

    int fl_pth_lcl_len;
    
    rmt_fch_cmd_sct *rmt_cmd=NULL;
    rmt_fch_cmd_sct msread={"msread -R %s %s",4,synchronous,lcl_rmt};
    rmt_fch_cmd_sct msrcp={"msrcp mss:%s %s",4,synchronous,rmt_lcl};
    rmt_fch_cmd_sct nrnet={"nrnet msget %s r flnm=%s l mail=FAIL",4,asynchronous,lcl_rmt};
    /*    rmt_fch_cmd_sct rcp={"rcp -p %s %s",4,synchronous,rmt_lcl};*/
    rmt_fch_cmd_sct scp={"scp -p %s %s",4,synchronous,rmt_lcl};
    rmt_fch_cmd_sct ftp={"",4,synchronous,rmt_lcl};

    /* Why did the stat() command fail? */
/*    (void)perror(prg_nm_get());*/
    
    /* The remote filename is input filename by definition */
    fl_nm_rmt=fl_nm;
    
    /* A URL specifier in filename unambiguously signals to use anonymous ftp */    if(rmt_cmd == NULL){
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
	host_nm_lcl=(char *)nco_malloc((256+1)*sizeof(char));
	(void)gethostname(host_nm_lcl,256+1);
	if(strchr(host_nm_lcl,'.') == NULL){
	  /* The returned hostname did not include the full Internet domain name */
	  (void)res_init();
	  (void)strcat(host_nm_lcl,".");
	  (void)strcat(host_nm_lcl,_res.defdname);
	} /* end if */

	/* Add one for the joining "@" and one for the NULL byte */
	usr_email=(char *)nco_malloc((strlen(usr_nm)+1+strlen(host_nm_lcl)+1)*sizeof(char));
	(void)sprintf(usr_email,"%s@%s",usr_nm,host_nm_lcl);
	/* Free the hostname space */
	host_nm_lcl=nco_free(host_nm_lcl);

	/* The remote hostname begins directly after "ftp://" */
	host_nm_rmt=fl_nm_rmt+6;
	/* filename begins right after the slash */
	fl_nm_rmt=strstr(fl_nm_rmt+6,"/")+1;
	/* NUL-terminate the hostname */
	*(fl_nm_rmt-1)='\0';
	
	/* Subtract the four characters replaced by new strings, and add one for the NULL byte */
	fmt=(char *)nco_malloc((strlen(fmt_template)+strlen(host_nm_rmt)+strlen(usr_email)-4+1)*sizeof(char));
	(void)sprintf(fmt,fmt_template,host_nm_rmt,usr_email,"%s","%s");
	rmt_cmd->fmt=fmt;
	/* Free the space holding the user's E-mail address */
	usr_email=nco_free(usr_email);
#endif /* not WIN32 */
      } /* end if */
    } /* end if */

    /* Otherwise, a single colon preceded by a period in the filename unambiguously signals to use rcp or scp */
    /* Determining whether to try scp instead of rcp is difficult
       Ideally, NCO would test remote machine for rcp/scp priveleges with a system command like, e.g., "ssh echo ok"
       To start with, we use scp which has its own fall through to rcp
    */
    if(rmt_cmd == NULL){
      if((cln_ptr=strchr(fl_nm_rmt,':')))
	if(((cln_ptr-4 >= fl_nm_rmt) && *(cln_ptr-4) == '.') ||
	   ((cln_ptr-3 >= fl_nm_rmt) && *(cln_ptr-3) == '.'))
	  rmt_cmd=&scp;
    } /* end if */
    
    if(rmt_cmd == NULL){
      /* Does msrcp command exist on local system? */
      rcd=stat("/usr/local/bin/msrcp",&stat_sct); /* SCD Dataproc, Ouray */
      if(rcd != 0) rcd=stat("/usr/bin/msrcp",&stat_sct); /* ACD Linux */
      if(rcd != 0) rcd=stat("/opt/local/bin/msrcp",&stat_sct); /* CGD */
      if(rcd != 0) rcd=stat("/usr/local/dcs/bin/msrcp",&stat_sct); /* ACD */
      if(rcd == 0) rmt_cmd=&msrcp;
    } /* end if */
	
    if(rmt_cmd == NULL){
      /* Does msread command exist on local system? */
      rcd=stat("/usr/local/bin/msread",&stat_sct);
      if(rcd == 0) rmt_cmd=&msread;
    } /* end if */
	
    if(rmt_cmd == NULL){
      /* Does nrnet command exist on local system? */
      rcd=stat("/usr/local/bin/nrnet",&stat_sct);
      if(rcd == 0) rmt_cmd=&nrnet;
    } /* end if */

    /* Before we look for file on remote system, make sure 
       filename has correct syntax to exist on remote system */
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
    fl_pth_lcl_tmp=(char *)nco_malloc((fl_pth_lcl_len+1)*sizeof(char));
    (void)strncpy(fl_pth_lcl_tmp,fl_nm_lcl,fl_pth_lcl_len);
    fl_pth_lcl_tmp[fl_pth_lcl_len]='\0';
    
    /* Warn user when local filepath was machine-derived from remote name */
    if(fl_pth_lcl == NULL) (void)fprintf(stderr,"%s: WARNING deriving local filepath from remote filename, using %s\n",prg_nm_get(),fl_pth_lcl_tmp);

    /* Does local filepath already exist on local system? */
    rcd=stat(fl_pth_lcl_tmp,&stat_sct);
    /* If not, then create the local filepath */
    if(rcd != 0){
      /* Allocate enough room for joining space ' ' and terminating NUL */
      cmd_sys=(char *)nco_malloc((strlen(cmd_mkdir)+fl_pth_lcl_len+2)*sizeof(char));
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
      cmd_sys=nco_free(cmd_sys);
    } /* end if */

    /* Free local path space, if any */
    fl_pth_lcl_tmp=nco_free(fl_pth_lcl_tmp);

    /* Allocate enough room for joining space ' ' and terminating NUL */
    cmd_sys=(char *)nco_malloc((strlen(rmt_cmd->fmt)-rmt_cmd->nbr_fmt_char+strlen(fl_nm_lcl)+strlen(fl_nm_rmt)+2)*sizeof(char));
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
    cmd_sys=nco_free(cmd_sys);

    /* Free ftp script, which is the only dynamically allocated command */
    if(rmt_cmd == &ftp) rmt_cmd->fmt=nco_free(rmt_cmd->fmt);
   
    if(rmt_cmd->transfer_mode == synchronous){
      if(dbg_lvl_get() > 0) (void)fprintf(stderr,"\n");
      if(rcd != 0){
	(void)fprintf(stderr,"%s: ERROR Synchronous fetch command failed\n",prg_nm_get());
	exit(EXIT_FAILURE);
      } /* end if */
    }else{
      /* This is the appropriate place to insert a shell script invocation 
	 of a command to retrieve file asynchronously and return the 
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
  if(strstr(fl_nm_lcl,"http://") == fl_nm_lcl){
    /* Attempt ncopen() on HTTP protocol files. Success means DODS can find file. */
    int in_id; /* [id] Temporary input file ID */
    
    (void)nco_open(fl_nm_lcl,NC_NOWRITE,&in_id);
    
    if(rcd < 0){ /* fxm: Should compare to NC_NOERR but that would mean needing to #include netcdf.h which I want to avoid */
      (void)fprintf(stderr,"%s: ERROR Attempted HTTP access protocol failed: DODS server is not responding, %s does not exist, or user does not have read permission for it\n",prg_nm_get(),fl_nm_lcl);
      exit(EXIT_FAILURE);
    } /* end if err */

   }else{
     if((fp_in=fopen(fl_nm_lcl,"r")) == NULL){
       (void)fprintf(stderr,"%s: ERROR User does not have read permission for %s\n",prg_nm_get(),fl_nm_lcl);
       exit(EXIT_FAILURE);
     }else{
       (void)fclose(fp_in);
     } /* end else */
   } /* end if really a local file */
  
  /* Free input filename space */
  fl_nm=nco_free(fl_nm);

  /* Return local filename */
  return(fl_nm_lcl);

} /* end fl_mk_lcl() */

void indexx(int n,int *arrin,int *indx)
/*     int n,indx[];*/
/*     float arrin[];*/
/*     int arrin[];*/
{
  /* Purpose: Sort an array of integers
     Based on indexx() from Numerical Recipes
     Routine computes an index table which sorts input array into ascending order
     I made arrin argument and local variable q integers for netCDF purposes
     Routine assumes "one-based" arrays */
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
{
/* Purpose: Sort input array alphanumerically
   This is indexx() from Numerical recipes hacked to alphabetize a list of strings */
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
  char *dsh_ptr=NULL;
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
  if(dlr_ptr == NULL && dbg_lvl_get() > 3)(void)fprintf(stderr,"%s: WARNING cvs_vrs_prs() reports dlr_ptr == NULL\n%s: HINT Make sure CVS export uses -kkv\n",prg_nm_get(),prg_nm_get());
  cvs_nm_ptr=strstr(cvs_Name,"$Name: ");
  if(cvs_nm_ptr == NULL && dbg_lvl_get() > 3)(void)fprintf(stderr,"%s: WARNING cvs_vrs_prs() reports cvs_nm_ptr == NULL\n%s: HINT Make sure CVS export uses -kkv\n",prg_nm_get(),prg_nm_get());
  cvs_nm_sng_len=(int)(dlr_ptr-cvs_nm_ptr-7); /* 7 is strlen("$Name: ") */
  if(cvs_nm_sng_len > 0) dly_snp=False; else dly_snp=True;

  /* If not, this is daily snapshot so use YYYYMMDD date for version string */
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
    cvs_vrs_sng=(char *)nco_malloc(cvs_vrs_sng_len+1);
    (void)sprintf(cvs_vrs_sng,"%04i%02i%02i",yr,mth,day);
    return cvs_vrs_sng;
  } /* endif dly_snp */

  /* cvs_nm_sng is, e.g., "nco1_1" */
  cvs_nm_sng=(char *)nco_malloc(cvs_nm_sng_len+1);
  strncpy(cvs_nm_sng,cvs_Name+7,cvs_nm_sng_len); /* 7 is strlen("$Name: ") */
  cvs_nm_sng[cvs_nm_sng_len]='\0';

  /* cvs_vrs_sng is, e.g., "1.1" */
  nco_sng_len=strlen(nco_sng);
  nco_sng_ptr=strstr(cvs_nm_sng,nco_sng);
  if(nco_sng_ptr == NULL)(void)fprintf(stderr,"%s: WARNING cvs_vrs_prs() reports nco_sng_ptr == NULL\n",prg_nm_get());
  dsh_ptr=strstr(cvs_nm_sng,"-");
  if(dsh_ptr == NULL)(void)fprintf(stderr,"%s: WARNING cvs_vrs_prs() reports dsh_ptr == NULL\n",prg_nm_get());
  usc_1_ptr=strstr(cvs_nm_sng,"_");
  if(usc_1_ptr == NULL)(void)fprintf(stderr,"%s: WARNING cvs_vrs_prs() reports usc_1_ptr == NULL\n",prg_nm_get());
  cvs_mjr_vrs_len=(int)(usc_1_ptr-dsh_ptr)-1; /* NB: cast pointer to int before subtracting */
  usc_2_ptr=strstr(usc_1_ptr+1,"_");
  cvs_mjr_vrs_sng=(char *)nco_malloc(cvs_mjr_vrs_len+1);
  cvs_mjr_vrs_sng=strncpy(cvs_mjr_vrs_sng,cvs_nm_sng+nco_sng_len+1,cvs_mjr_vrs_len);
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
  cvs_mnr_vrs_sng=(char *)nco_malloc(cvs_mnr_vrs_len+1);
  cvs_mnr_vrs_sng=strncpy(cvs_mnr_vrs_sng,usc_1_ptr+1,cvs_mnr_vrs_len);
  cvs_mnr_vrs_sng[cvs_mnr_vrs_len]='\0';
  cvs_mnr_vrs=strtol(cvs_mnr_vrs_sng,(char **)NULL,10);

  cvs_pch_vrs_sng=(char *)nco_malloc(cvs_pch_vrs_len+1);
  cvs_pch_vrs_sng[cvs_pch_vrs_len]='\0';
  cvs_vrs_sng=(char *)nco_malloc(cvs_vrs_sng_len+1);
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

  cvs_mjr_vrs_sng=nco_free(cvs_mjr_vrs_sng);
  cvs_mnr_vrs_sng=nco_free(cvs_mnr_vrs_sng);
  cvs_pch_vrs_sng=nco_free(cvs_pch_vrs_sng);
  cvs_nm_sng=nco_free(cvs_nm_sng);

  return cvs_vrs_sng;
} /* end cvs_vrs_prs() */

void
copyright_prn(char *CVS_Id,char *CVS_Revision)
/* 
   char *CVS_Id: I [sng] CVS identification string
   char *CVS_Revision: I [sng] CVS revision string
 */
{
  char *date_cvs; /* Date this file was last modified */
  char *vrs_rcs; /* Version of this file, e.g., 1.213 */
  char *vrs_cvs; /* Version according to CVS release tag */

  int date_cvs_lng;
  int vrs_cvs_lng;
  
  /* C pre-processor macros for instantiating variable values with string tokens */
#define XTKN2SNG(x) #x
#define TKN2SNG(x) XTKN2SNG(x)
  const char date_cpp[]=__DATE__; /* [sng] Date from C pre-processor */
  /*  const char time_cpp[]=__TIME__; *//* [sng] Time from C pre-processor */
  const char vrs_cpp[]=TKN2SNG(VERSION); /* [sng] Version from C pre-processor */
  const char hst_cpp[]=TKN2SNG(HOSTNAME); /* [sng] Hostname from C pre-processor */
  const char usr_cpp[]=TKN2SNG(USER); /* [sng] Hostname from C pre-processor */

  if(strlen(CVS_Id) > strlen("*Id*")){
    /* CVS_Id is defined */
    date_cvs_lng=10;
    date_cvs=(char *)nco_malloc((date_cvs_lng+1)*sizeof(char));
    (void)strncpy(date_cvs,strchr(CVS_Id,'/')-4,date_cvs_lng);
    date_cvs[date_cvs_lng]='\0';
  }else{
    /* CVS_Id is undefined */
    date_cvs=(char *)strdup("Current");
  } /* endif */

  if(strlen(CVS_Revision) > strlen("*Revision*") || strlen(CVS_Revision) < strlen("*Revision*")){
    /* CVS_Revision is defined */
    vrs_cvs_lng=strrchr(CVS_Revision,'$')-strchr(CVS_Revision,':')-3;
    vrs_rcs=(char *)nco_malloc((vrs_cvs_lng+1)*sizeof(char));
    (void)strncpy(vrs_rcs,strchr(CVS_Revision,':')+2,vrs_cvs_lng);
    vrs_rcs[vrs_cvs_lng]='\0';
  }else{
    /* CVS_Revision is undefined */
    vrs_rcs=(char *)strdup("Current");
  } /* endif */

  vrs_cvs=cvs_vrs_prs();

  if(strlen(CVS_Id) > strlen("*Id*")){
    (void)fprintf(stderr,"%s version %s last modified %s built %s on %s by %s\n",prg_nm_get(),vrs_cvs,date_cvs,date_cpp,hst_cpp,usr_cpp);
  }else{
    (void)fprintf(stderr,"%s version %s built %s on %s by %s\n",prg_nm_get(),vrs_cpp,date_cpp,hst_cpp,usr_cpp);
  } /* endif */

  /*  (void)fprintf(stderr,"NCO netCDF Operators version %s by Charlie Zender\n",vrs_cvs);
  (void)fprintf(stderr,"%s version %s (%s) \"%s\"\n",prg_nm_get(),vrs_rcs,date_cvs,nmn_get());
  (void)fprintf(stderr,"Copyright 1995--1999 University Corporation for Atmospheric Research\n");
  (void)fprintf(stderr,"Portions copyright 1999--2000 Regents of the University of California\n"); */

  (void)fprintf(stderr,"NCO netCDF Operators version %s\n",vrs_cvs);
  (void)fprintf(stderr,"Copyright (C) 1995--2002 Charlie Zender\n");
  (void)fprintf(stderr,"%s version %s (%s) \"%s\"\n",prg_nm_get(),vrs_rcs,date_cvs,nmn_get());
  (void)fprintf(stdout,"NCO is free software and comes with ABSOLUTELY NO WARRANTY\nNCO is distributed under the terms of the GNU General Public License\n");

  vrs_rcs=nco_free(vrs_rcs);
  vrs_cvs=nco_free(vrs_cvs);
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
  cp_cmd=(char *)nco_malloc((strlen(cp_cmd_fmt)+strlen(fl_src)+strlen(fl_dst)-nbr_fmt_char+1)*sizeof(char));
  if(dbg_lvl_get() > 0) (void)fprintf(stderr,"Copying %s to %s...",fl_src,fl_dst);
  (void)sprintf(cp_cmd,cp_cmd_fmt,fl_src,fl_dst);
  rcd=system(cp_cmd);
  if(rcd == -1){
    (void)fprintf(stdout,"%s: ERROR fl_cp() is unable to execute cp command \"%s\"\n",prg_nm_get(),cp_cmd);
    exit(EXIT_FAILURE); 
  } /* end if */
  cp_cmd=nco_free(cp_cmd);
  if(dbg_lvl_get() > 0) (void)fprintf(stderr,"done\n");
  
} /* end fl_cp() */

void
fl_mv(char *fl_src,char *fl_dst)
/* 
   char *fl_src: I name of file to move
   char *fl_dst: I name of the destination file
 */
{
  /* Routine to move the first file to the second */

  char *mv_cmd;
  char mv_cmd_fmt[]="mv -f %s %s";

  int rcd;
  int nbr_fmt_char=4;
  
  /* Construct and execute the copy command */
  mv_cmd=(char *)nco_malloc((strlen(mv_cmd_fmt)+strlen(fl_src)+strlen(fl_dst)-nbr_fmt_char+1)*sizeof(char));
  if(dbg_lvl_get() > 0) (void)fprintf(stderr,"Moving %s to %s...",fl_src,fl_dst);
  (void)sprintf(mv_cmd,mv_cmd_fmt,fl_src,fl_dst);
  rcd=system(mv_cmd);
  if(rcd == -1){
    (void)fprintf(stdout,"%s: ERROR fl_mv() is unable to execute mv command \"%s\"\n",prg_nm_get(),mv_cmd);
    exit(EXIT_FAILURE); 
  } /* end if */
  mv_cmd=nco_free(mv_cmd);
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
  rm_cmd=(char *)nco_malloc((strlen(rm_cmd_sys_dep)+1+strlen(fl_nm)+1)*sizeof(char));
  (void)sprintf(rm_cmd,"%s %s",rm_cmd_sys_dep,fl_nm);

  if(dbg_lvl_get() > 0) (void)fprintf(stderr,"%s: Removing %s with %s\n",prg_nm_get(),fl_nm,rm_cmd);
  rcd=system(rm_cmd);
  if(rcd == -1) (void)fprintf(stderr,"%s: WARNING unable to remove %s, continuing anyway...\n",prg_nm_get(),fl_nm);

  rm_cmd=nco_free(rm_cmd);

} /* end fl_rm() */

int
sng_ascii_trn(char *sng)
     /* 
	char *sng: I/O [sng] String to process
	int sng_ascii_trn: O [nbr] Number of escape sequences translated
     */
{
  /* Purpose: Replace any C language '\X' escape codes in a string into ASCII bytes 
     Return number of escape sequences found and actually translated
     This should be the same as number of bytes by which the string length has shrunk
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
    case 'a': *backslash_ptr='\a'; break; /* 007   7     07    BEL '\a' Bell */
    case 'b': *backslash_ptr='\b'; break; /* 010   8     08    BS  '\b' Backspace */
    case 'f': *backslash_ptr='\f'; break; /* 014   12    0C    FF  '\f' Formfeed */
    case 'n': *backslash_ptr='\n'; break; /* 012   10    0A    LF  '\n' Linefeed */
    case 'r': *backslash_ptr='\r'; break; /* 015   13    0D    CR  '\r' Carriage return */
    case 't': *backslash_ptr='\t'; break; /* 011   9     09    HT  '\t' Horizontal tab */
    case 'v': *backslash_ptr='\v'; break; /* 013   11    0B    VT  '\v' Vertical tab */
    case '\\': *backslash_ptr='\\'; break; /* 134   92    5C    \   '\\' */
    case '\?': *backslash_ptr='\?'; break; /* I'm not sure why or if this one works! */
    case '\'': *backslash_ptr='\''; break; /* I'm not sure why or if this one works! */
    case '\"': *backslash_ptr='\"'; break; /* I'm not sure why or if this one works! */
      /* Do not translate \0 to NUL since this would "erase" the rest of the string */
    case '0':	
      /* Do not translate \0 to NUL since this would make the rest of the string invisible to all string functions */
      /* *backslash_ptr='\0'; *//* 000   0     00    NUL '\0' */
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

void *nco_malloc(size_t size)
{
  /* Purpose: Custom wrapper for malloc()
     Routine prints error when malloc() returns a NULL pointer 
     Routine does not call malloc() when size == 0 */
  
  void *ptr; /* [ptr] Pointer to new buffer */
  
  /* malloc(0) is ANSI-legal, albeit unnecessary
     NCO sometimes employs this degenerate case behavior of malloc() to simplify code
     Some debugging tools like Electric Fence consider any NULL returned by malloc() to be an error
     So circumvent malloc() calls when size == 0 */
  if(size == 0) return NULL;
  
  ptr=malloc(size); /* [ptr] Pointer to new buffer */
  if(ptr == NULL){
    (void)fprintf(stdout,"%s: ERROR nco_malloc() unable to allocate %li bytes\n",prg_nm_get(),(long)size);
    /* fxm: Should be exit(8) on ENOMEM errors? */
    exit(EXIT_FAILURE);
  } /* endif */
  return ptr; /* [ptr] Pointer to new buffer */
} /* nco_malloc() */

void *nco_free(void *vp)
{
  /* Purpose: Custom wrapper for free()
     Free memory and set pointer to NULL
     Routine does not call free() when vp == NULL
     Usage: vp=nco_free(vp) */
  if(vp != NULL) free(vp);
  return NULL; /* [ptr] Pointer to new buffer */
} /* nco_free() */

void *nco_calloc(size_t lmn_nbr,size_t lmn_sz)
{
  /* Purpose: Custom wrapper for calloc(), modified from nco_malloc()
     Routine prints error when calloc() returns a NULL pointer 
     Routine does not call calloc() when lmn_sz == 0 or lmn_nbr == 0 */
  
  void *ptr; /* [ptr] Pointer to new buffer */
  
  /* Circumvent calloc() calls when lmn_sz == 0 */
  if(lmn_sz == 0 || lmn_nbr == 0) return NULL;
  
  ptr=calloc(lmn_nbr,lmn_sz); /* [ptr] Pointer to new buffer */
  if(ptr == NULL){
    (void)fprintf(stdout,"%s: ERROR nco_calloc() unable to allocate %li elements of %li bytes = %li bytes\n",prg_nm_get(),(long)lmn_nbr,(long)lmn_sz,(long)(lmn_nbr*lmn_sz));
    exit(EXIT_FAILURE);
  } /* endif */
  return ptr; /* [ptr] Pointer to new buffer */
} /* nco_calloc() */

void *nco_realloc(void *ptr,size_t size)
{
  /* Purpose: Custom wrapper for realloc()
     Routine prints error when realloc() returns a NULL pointer
     Routine does not call realloc() when size == 0 */
  
  void *new_ptr; /* [ptr] Pointer to new buffer */
  
  /* This degenerate case sometimes occurs
     Performing realloc() call here would be ANSI-legal but would trigger Electric Fence */
  if(ptr == NULL && size == 0) return ptr;
  if(ptr != NULL && size == 0){
    ptr=nco_free(ptr);
    ptr=NULL;
    return ptr;
  } /* endif */
  
  /* Passing NULL to realloc() is ANSI-legal, but may cause portability problems */
  if(ptr == NULL && size != 0){
    new_ptr=nco_malloc(size); /* [ptr] Pointer to new buffer */
  }else{
    new_ptr=realloc(ptr,size); /* [ptr] Pointer to new buffer */
  } /* endif */
  if(new_ptr == NULL && size != 0){
    (void)fprintf(stdout,"%s: ERROR nco_realloc() unable to realloc() %li bytes\n",prg_nm_get(),(long)size); 
    /* fxm: Should be exit(8) on ENOMEM errors? */
    exit(EXIT_FAILURE);
  } /* endif */
  return new_ptr; /* [ptr] Pointer to new buffer */
} /* nco_realloc() */

int /* O [enm] Return code */
nco_openmp_ini() /* [fnc] Set up OpenMP multi-threading environment */
{
  /* Purpose: Set up OpenMP multi-threading environment */
  int rcd=0; /* [rcd] Return code */

#ifdef _OPENMP
  /* System allocates OMP_NUM_THREADS if possible
     ncwa does not scale well beyond thr_nbr_max_fsh=4 threads because it is bottlenecked by I/O
     If OMP_NUM_THREADS > 4 then NCO will not be using threads efficiently
     Strategy: Determine maximum number of threads system will allocate (thr_nbr_max)
     Reduce maximum number of threads available to system to thr_nbr_max_fsh
     Play nice: Set dynamic threading so that system can make efficiency decisions
     When dynamic threads are set, then system will never allocate more than thr_nbr_max_fsh
  */
  const int dyn_thr=1; /* [flg] Allow system to dynamically set number of threads */
  int thr_nbr_max_fsh=4; /* [nbr] Maximum number of threads program can use efficiently */
  int thr_nbr_max; /* [nbr] Maximum number of threads system/user allow program to use */

  /* Disable threading on a per-program basis */
  /* ncrcat is extremely I/O intensive 
     Maximum efficiency when one thread reads from input file while other writes to output file */
  if(strstr(prg_nm_get(),"ncrcat")) thr_nbr_max_fsh=1;

  thr_nbr_max=omp_get_max_threads(); /* [nbr] Maximum number of threads system/user allow program to use */
  if(thr_nbr_max > thr_nbr_max_fsh){
    (void)fprintf(stderr,"%s: INFO Reducing number of threads from %d to %d since %s hits I/O bottleneck above %d threads\n",prg_nm_get(),thr_nbr_max,thr_nbr_max_fsh,prg_nm_get(),thr_nbr_max_fsh);
    (void)omp_set_num_threads(thr_nbr_max_fsh); /* [nbr] Maximum number of threads system is allowed */
  } /* endif */      
  (void)omp_set_dynamic(dyn_thr); /* [flg] Allow system to dynamically set number of threads */
  if(dbg_lvl_get() > 0) (void)fprintf(stderr,"%s: INFO Allowing OS to utilize dynamic threading\n",prg_nm_get());
#endif /* not _OPENMP */

  if(dbg_lvl_get() > 0){
#ifdef _OPENMP /* OpenMP-compliant compilers define _OPENMP=YYYYMM = year and month of OpenMP specification */
#pragma omp parallel
    { /* begin OpenMP parallel */
#pragma omp single nowait
      { /* begin OpenMP single */
	(void)fprintf(stderr,"%s: INFO OpenMP multi-threading using %d threads\n",prg_nm_get(),omp_get_num_threads());
      } /* end OpenMP single */
    } /* end OpenMP parallel */
#else /* not _OPENMP */
    (void)fprintf(stderr,"%s: INFO Not attempting OpenMP multi-threading\n",prg_nm_get());
#endif /* not _OPENMP */
  } /* endif dbg */

  return rcd;
} /* end nco_openmp_ini() */

int /* O [enm] Return code */
nco_var_prc_crr_prn /* [fnc] Print name of current variable */
(int idx, /* I [idx] Index of current variable */
 char *var_nm) /* I [sng] Variable name */
{
  /* Purpose: Print name of current variable */
  int rcd=0; /* [rcd] Return code */

#ifdef _OPENMP
  (void)fprintf(stderr,"%s: INFO Thread #%d processing var_prc[%d] = \"%s\"\n",prg_nm_get(),omp_get_thread_num(),idx,var_nm);
#else /* not _OPENMP */
  (void)fprintf(stderr,"%s, ",var_nm);
#endif /* not _OPENMP */

  return rcd;
} /* end nco_var_prc_crr_prn() */
