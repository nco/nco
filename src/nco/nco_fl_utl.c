/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_fl_utl.c,v 1.13 2002-08-22 22:04:15 zender Exp $ */

/* Purpose: File manipulation */

/* Copyright (C) 1995--2002 Charlie Zender
   This software is distributed under the terms of the GNU General Public License
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

#include "nco_fl_utl.h" /* File manipulation */

void
nco_fl_cp /* [fnc] Copy first file to second */
(const char * const fl_src, /* I [sng] Name of source file to copy */
 const char * const fl_dst) /* I [sng] Name of destination file */
{
  /* Purpose: Copy first file to second */
  char *cp_cmd;
  char cp_cmd_fmt[]="cp %s %s";

  int rcd;
  int nbr_fmt_char=4;
  
  /* Construct and execute copy command */
  cp_cmd=(char *)nco_malloc((strlen(cp_cmd_fmt)+strlen(fl_src)+strlen(fl_dst)-nbr_fmt_char+1)*sizeof(char));
  if(dbg_lvl_get() > 0) (void)fprintf(stderr,"Copying %s to %s...",fl_src,fl_dst);
  (void)sprintf(cp_cmd,cp_cmd_fmt,fl_src,fl_dst);
  rcd=system(cp_cmd);
  if(rcd == -1){
    (void)fprintf(stdout,"%s: ERROR nco_fl_cp() is unable to execute cp command \"%s\"\n",prg_nm_get(),cp_cmd);
    nco_exit(EXIT_FAILURE); 
  } /* end if */
  cp_cmd=(char *)nco_free(cp_cmd);
  if(dbg_lvl_get() > 0) (void)fprintf(stderr,"done\n");
} /* end nco_fl_cp() */

void
fl_mv /* [fnc] Move first file to second */
(const char * const fl_src, /* I [sng] Name of source file to move */
 const char * const fl_dst) /* I [sng] Name of destination file */
{
  /* Purpose: Move first file to second */
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
    nco_exit(EXIT_FAILURE); 
  } /* end if */
  mv_cmd=(char *)nco_free(mv_cmd);
  if(dbg_lvl_get() > 0) (void)fprintf(stderr,"done\n");
} /* end fl_mv() */

void 
nco_fl_rm /* [fnc] Remove file */
(char *fl_nm) /* I [sng] File to be removed */
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

  rm_cmd=(char *)nco_free(rm_cmd);
} /* end nco_fl_rm() */

char ** /* O [sng] List of user-specified filenames */
nco_fl_lst_mk /* [fnc] Create file list from command line positional arguments */
(const char * const * const argv, /* I [sng] Argument list */
 const int argc, /* I [nbr] Argument count */
 int arg_crr, /* I [idx] Index of current argument */
 int * const nbr_fl, /* O [nbr] Number of files in input file list */
 char ** const fl_out) /* O [sng] Name of output file */
{
  /* Purpose: Parse positional arguments on command line
     Name of calling program plays a role in this */

  /* Command-line switches are expected to have been digested already (e.g., by getopt()),
     and argv[arg_crr] points to the first argument on the command line following all the
     switches. */

  char **fl_lst_in;

  int idx;
  int fl_nm_sz_warning=80;

  /* Are there any remaining arguments that could be filenames? */
  if(arg_crr >= argc){
    (void)fprintf(stdout,"%s: ERROR must specify filename(s)\n",prg_nm_get());
    (void)nco_usg_prn();
    nco_exit(EXIT_FAILURE);
  } /* end if */

  /* See if there appear to be problems with any of the specified files */
  for(idx=arg_crr;idx<argc;idx++){
    if((int)strlen(argv[idx]) >= fl_nm_sz_warning) (void)fprintf(stderr,"%s: WARNING filename %s is very long (%ld characters)\n",prg_nm_get(),argv[idx],(long)strlen(argv[idx]));
  } /* end loop over idx */

  switch(prg_get()){
  case ncks:
  case ncatted:
  case ncrename:
    if(argc-arg_crr > 2){
      (void)fprintf(stdout,"%s: ERROR received %d filenames; need no more than 2\n",prg_nm_get(),argc-arg_crr);
      (void)nco_usg_prn();
      nco_exit(EXIT_FAILURE);
    } /* end if */
    fl_lst_in=(char **)nco_malloc(sizeof(char *));
    fl_lst_in[(*nbr_fl)++]=(char *)strdup(argv[arg_crr++]);
    if(arg_crr == argc-1) *fl_out=(char *)strdup(argv[arg_crr]); else *fl_out=NULL;
    return fl_lst_in;
    /*    break;*//* NB: putting break after return in case statement causes warning on SGI cc */
  case ncdiff:
  case ncflint:
    if(argc-arg_crr != 3){
      (void)fprintf(stdout,"%s: ERROR received %d filenames; need exactly 3\n",prg_nm_get(),argc-arg_crr);
      (void)nco_usg_prn();
      nco_exit(EXIT_FAILURE);
    } /* end if */
    break;
  case ncap:
  case ncwa:
    if(argc-arg_crr != 2){
      (void)fprintf(stdout,"%s: ERROR received %d filenames; need exactly 2\n",prg_nm_get(),argc-arg_crr);
      (void)nco_usg_prn();
      nco_exit(EXIT_FAILURE);
    } /* end if */
    break;
  case ncra:
  case ncea:
  case ncrcat:
  case ncecat:
    if(argc-arg_crr < 2){
      (void)fprintf(stdout,"%s: ERROR received %d filenames; need at least 2\n",prg_nm_get(),argc-arg_crr);
      (void)nco_usg_prn();
      nco_exit(EXIT_FAILURE);
    } /* end if */
    break;
  default:
    break;
  } /* end switch */

  /* Fill in file list and output file */
  fl_lst_in=(char **)nco_malloc((argc-arg_crr-1)*sizeof(char *));
  while(arg_crr < argc-1) fl_lst_in[(*nbr_fl)++]=(char *)strdup(argv[arg_crr++]);
  if(*nbr_fl == 0){
    (void)fprintf(stdout,"%s: ERROR Must specify input filename.\n",prg_nm_get());
    (void)nco_usg_prn();
    nco_exit(EXIT_FAILURE);
  } /* end if */
  *fl_out=(char *)strdup(argv[argc-1]);

  return fl_lst_in;

} /* end nco_fl_lst_mk() */

char * /* O [sng] Filename of locally available file */
nco_fl_mk_lcl /* [fnc] Retrieve input file and return local filename */
(char *fl_nm, /* I/O [sng] Current filename, if any (destroyed) */
 const char * const fl_pth_lcl, /* I [sng] Local storage area for files retrieved from remote locations */
 int * const FILE_RETRIEVED_FROM_REMOTE_LOCATION) /* O [flg] File was retrieved from remote location */
{
  /* Purpose: Locate input file, retrieve it from remote storage system if necessary, 
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
    fl_nm_lcl_tmp=(char *)nco_free(fl_nm_lcl_tmp);
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
      fl_nm_lcl_tmp=(char *)nco_free(fl_nm_lcl_tmp);
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
      fl_nm_lcl=(char *)nco_free(fl_nm_lcl);
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
      fl_nm_lcl_tmp=(char *)nco_free(fl_nm_lcl_tmp);
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

    int fl_pth_lcl_lng;
    
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
    
    /* A URL specifier in filename unambiguously signals to use anonymous ftp */    
    if(rmt_cmd == NULL){
      if(strstr(fl_nm_rmt,"ftp://") == fl_nm_rmt){
#ifdef WIN32
      /* I have no idea how networking calls work in NT, so just exit */
      (void)fprintf(stdout,"%s: ERROR Networking required to obtain %s is not supported for Microsoft Windows operating systems\n",prg_nm_get(),fl_nm_rmt);
      nco_exit(EXIT_FAILURE);
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
	host_nm_lcl=(char *)nco_free(host_nm_lcl);

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
	usr_email=(char *)nco_free(usr_email);
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
	nco_exit(EXIT_FAILURE);
      } /* end if */
    } /* end if */
    
    if(rmt_cmd == NULL){
      (void)fprintf(stderr,"%s: ERROR unable to determine method for remote retrieval of %s\n",prg_nm_get(),fl_nm_rmt);
      nco_exit(EXIT_FAILURE);
    } /* end if */

    /* Find the path for storing the local file */
    fl_nm_stub=strrchr(fl_nm_lcl,'/')+1;
    /* Construct the local storage filepath name */
    fl_pth_lcl_lng=strlen(fl_nm_lcl)-strlen(fl_nm_stub)-1;
    /* Allocate enough room for the terminating NUL */
    fl_pth_lcl_tmp=(char *)nco_malloc((fl_pth_lcl_lng+1)*sizeof(char));
    (void)strncpy(fl_pth_lcl_tmp,fl_nm_lcl,fl_pth_lcl_lng);
    fl_pth_lcl_tmp[fl_pth_lcl_lng]='\0';
    
    /* Warn user when local filepath was machine-derived from remote name */
    if(fl_pth_lcl == NULL) (void)fprintf(stderr,"%s: WARNING deriving local filepath from remote filename, using %s\n",prg_nm_get(),fl_pth_lcl_tmp);

    /* Does local filepath already exist on local system? */
    rcd=stat(fl_pth_lcl_tmp,&stat_sct);
    /* If not, then create the local filepath */
    if(rcd != 0){
      /* Allocate enough room for joining space ' ' and terminating NUL */
      cmd_sys=(char *)nco_malloc((strlen(cmd_mkdir)+fl_pth_lcl_lng+2)*sizeof(char));
      (void)strcpy(cmd_sys,cmd_mkdir);
      (void)strcat(cmd_sys," ");
      (void)strcat(cmd_sys,fl_pth_lcl_tmp);
      if(dbg_lvl_get() > 0) (void)fprintf(stderr,"%s: Creating local directory %s with %s\n",prg_nm_get(),fl_pth_lcl_tmp,cmd_sys);
      rcd=system(cmd_sys); 
      if(rcd != 0){
	(void)fprintf(stderr,"%s: ERROR Unable to create local directory %s\n",prg_nm_get(),fl_pth_lcl_tmp);
	if(fl_pth_lcl == NULL) (void)fprintf(stderr,"%s: HINT Use -l option\n",prg_nm_get());
	nco_exit(EXIT_FAILURE);
      } /* end if */
      /* Free local command space */
      cmd_sys=(char *)nco_free(cmd_sys);
    } /* end if */

    /* Free local path space, if any */
    fl_pth_lcl_tmp=(char *)nco_free(fl_pth_lcl_tmp);

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
    cmd_sys=(char *)nco_free(cmd_sys);

    /* Free ftp script, which is the only dynamically allocated command */
    if(rmt_cmd == &ftp) rmt_cmd->fmt=(char *)nco_free(rmt_cmd->fmt);
   
    if(rmt_cmd->transfer_mode == synchronous){
      if(dbg_lvl_get() > 0) (void)fprintf(stderr,"\n");
      if(rcd != 0){
	(void)fprintf(stderr,"%s: ERROR Synchronous fetch command failed\n",prg_nm_get());
	nco_exit(EXIT_FAILURE);
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
	nco_exit(EXIT_FAILURE);
      } /* end if */
      if(dbg_lvl_get() > 0) (void)fprintf(stderr,"\n%s Retrieval successful after %d sleeps of %d seconds each = %.1f minutes\n",prg_nm_get(),tm_idx,tm_sleep,tm_idx*tm_sleep/60.);
    } /* end else transfer mode is asynchronous */
    *FILE_RETRIEVED_FROM_REMOTE_LOCATION=True;
  }else{ /* end if input file did not exist locally */
    *FILE_RETRIEVED_FROM_REMOTE_LOCATION=False;
  } /* end if file was already on the local system */

  /* Make sure we have read permission on local file */
  if(strstr(fl_nm_lcl,"http://") == fl_nm_lcl){
    /* Attempt nc_open() on HTTP protocol files. Success means DODS found file. */
    int in_id; /* [id] Temporary input file ID */
    
    rcd=nco_open(fl_nm_lcl,NC_NOWRITE,&in_id);
    
    if(rcd != NC_NOERR){
      (void)fprintf(stderr,"%s: ERROR Attempted HTTP access protocol failed: DODS server is not responding, %s does not exist, or user does not have read permission for it\n",prg_nm_get(),fl_nm_lcl);
      nco_exit(EXIT_FAILURE);
    } /* end if err */

   }else{
     if((fp_in=fopen(fl_nm_lcl,"r")) == NULL){
       (void)fprintf(stderr,"%s: ERROR User does not have read permission for %s, or file does not exist\n",prg_nm_get(),fl_nm_lcl);
       nco_exit(EXIT_FAILURE);
     }else{
       (void)fclose(fp_in);
     } /* end else */
   } /* end if really a local file */
  
  /* Free input filename space */
  fl_nm=(char *)nco_free(fl_nm);

  /* Return local filename */
  return(fl_nm_lcl);

} /* end nco_fl_mk_lcl() */

char * /* O [sng] Name of file to retrieve */
nco_fl_nm_prs /* [fnc] Construct file name from input arguments */
(char *fl_nm, /* I/O [sng] Current filename, if any */
 const int fl_nbr, /* I [nbr] Ordinal index of file in input file list */
 int * const nbr_fl, /* I/O [nbr] number of files to be processed */
 char * const * const fl_lst_in, /* I [sng] User-specified filenames */
 const int nbr_abb_arg, /* I [nbr] Number of abbreviation arguments */
 const char * const * const fl_lst_abb, /* I [sng] NINTAP-style arguments, if any */
 const char * const fl_pth) /* I [sng] Path prefix for files in fl_lst_in */
{
  /* Purpose: Construct file name from various input arguments and switches.
     Routine implements NINTAP-style specification by using static
     memory to avoid repetition in construction of filename */

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
  fl_nm=(char *)nco_free(fl_nm);

  /* Construct filename from NINTAP-style arguments and input name */
  if(fl_lst_abb != NULL){
    if(FIRST_INVOCATION){
      int fl_nm_sfx_lng=0;
      
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
	fl_nm_sfx_lng=3;
      else if(strncmp(fl_lst_in[0]+strlen(fl_lst_in[0])-4,".cdf",4) == 0)
	fl_nm_sfx_lng=4;
      else if(strncmp(fl_lst_in[0]+strlen(fl_lst_in[0])-4,".hdf",4) == 0)
	fl_nm_sfx_lng=4;
      else if(strncmp(fl_lst_in[0]+strlen(fl_lst_in[0])-4,".hd5",4) == 0)
	fl_nm_sfx_lng=4;
      
      /* Initialize static information useful for future invocations */
      fl_nm_1st_dgt=fl_lst_in[0]+strlen(fl_lst_in[0])-fl_nm_nbr_dgt-fl_nm_sfx_lng;
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
    fl_nm_stub=(char *)nco_free(fl_nm_stub);
  } /* end if */

  /* Return new filename */
  return(fl_nm);
} /* end nco_fl_nm_prs() */

char * /* O [sng] Name of temporary file actually opened */
nco_fl_out_open /* [fnc] Open output file subject to availability and user input */
(const char * const fl_out, /* I [sng] Name of file to open */
 const bool FORCE_APPEND, /* I [flg] Append to existing file, if any */
 const bool FORCE_OVERWRITE, /* I [flg] Overwrite existing file, if any */
 int * const out_id) /* O [id] File ID */
{
  /* Purpose: Open output file subject to availability and user input 
     In accord with netCDF philosophy a temporary file (based on fl_out and process ID)
     is actually opened, so that errors can not infect intended output file */

  char *fl_out_tmp;
  char tmp_sng_1[]="pid"; /* Extra string appended to temporary filenames */
  char tmp_sng_2[]="tmp"; /* Extra string appended to temporary filenames */
  char *pid_sng; /* String containing decimal representation of PID */

  int rcd; /* [rcd] Return code */

  long fl_out_tmp_lng; /* [nbr] Length of temporary file name */
  long pid_sng_lng; /* [nbr] Theoretical length of decimal representation of this PID */
  long pid_sng_lng_max; /* [nbr] Maximum length of decimal representation of any PID */

  pid_t pid; /* Process ID */

  struct stat stat_sct;
  
  if(FORCE_OVERWRITE && FORCE_APPEND){
    (void)fprintf(stdout,"%s: ERROR FORCE_OVERWRITE and FORCE_APPEND are both set\n",prg_nm_get());
    nco_exit(EXIT_FAILURE);
  } /* end if */

  /* Generate unique temporary file name
     System routines tempnam(), tmpname(), mktemp() perform a similar function, but are OS dependent
     Maximum length of PID depends on pid_t
     Until about 1995 most OSs set pid_t = short = 16 or 32 bits
     Now some OSs have /usr/include/sys/types.h set pid_t = long = 32 or 64 bits
     20000126: Use sizeof(pid_t) rather than hardcoded size to fix longstanding bug on SGIs
  */
  /* Maximum length of decimal representation of PID is number of bits in PID times log10(2) */
  pid_sng_lng_max=(long)ceil(8*sizeof(pid_t)*log10(2.0));
  pid_sng=(char *)nco_malloc((pid_sng_lng_max+1)*sizeof(char));
  pid=getpid();
  (void)sprintf(pid_sng,"%ld",(long)pid);
  /* Theoretical length of decimal representation of PID is 1+ceil(log10(PID)) where the 1 is required iff PID is an exact power of 10 */
  pid_sng_lng=1L+(long)ceil(log10((double)pid));
  /* NCO temporary file name is user-specified file name + "." + tmp_sng_1 + PID + "." + prg_nm + "." + tmp_sng_2 + NUL */
  fl_out_tmp_lng=strlen(fl_out)+1L+strlen(tmp_sng_1)+strlen(pid_sng)+1L+strlen(prg_nm_get())+1L+strlen(tmp_sng_2)+1L;
  fl_out_tmp=(char *)nco_malloc(fl_out_tmp_lng*sizeof(char));
  (void)sprintf(fl_out_tmp,"%s.%s%s.%s.%s",fl_out,tmp_sng_1,pid_sng,prg_nm_get(),tmp_sng_2);
  if(dbg_lvl_get() > 5) (void)fprintf(stdout,"%s: nco_fl_out_open() reports sizeof(pid_t) = %d bytes, pid = %ld, pid_sng_lng = %ld bytes, strlen(pid_sng) = %ld bytes, fl_out_tmp_lng = %ld bytes, strlen(fl_out_tmp) = %ld, fl_out_tmp = %s\n",prg_nm_get(),(int)sizeof(pid_t),(long)pid,pid_sng_lng,(long)strlen(pid_sng),fl_out_tmp_lng,(long)strlen(fl_out_tmp),fl_out_tmp);
  rcd=stat(fl_out_tmp,&stat_sct);

  /* Free temporary memory */
  pid_sng=(char *)nco_free(pid_sng);

  if(dbg_lvl_get() == 8){
  /* Use builtin system routines to generate temporary filename
     This allows file to be built in fast directory like /tmp rather than local
     directory which could be a slow, NFS-mounted directories like /fs/cgd

     There are many options:
     tmpnam() uses P_tmpdir, does not allow specfication of drc
     tempnam(const char *drc, const char *pfx) uses writable $TMPDIR, else drc, else P_tmpdir, else /tmp and prefixes returned name with up to five characters from pfx, if supplied
     mkstemp(char *tpl) generates a filename and creates file in mode 0600

     Many sysadmins do not make /tmp large enough for huge temporary data files 
     tempnam(), however, allows $TMPDIR or drc to be set to override /tmp
     We tried tempnam() but as of 20001010 GCC 2.96 this causes a warning: "the use of `tempnam' is dangerous, better use `mkstemp'"
   */
    int fl_out_hnd; /* Temporary file */
    char *fl_out_tmp_sys; /* System-generated unique temporary filename */
    fl_out_tmp_sys=(char *)nco_malloc((strlen(fl_out)+7)*sizeof(char));
    fl_out_tmp_sys[0]='\0'; /* NUL-terminate */
    fl_out_tmp_sys=strcat(fl_out_tmp_sys,fl_out);
    fl_out_tmp_sys=strcat(fl_out_tmp_sys,"XXXXXX");
    /* 20020812: Cray does not support mkstemp */
#ifndef CRAY
    fl_out_hnd=mkstemp(fl_out_tmp_sys);
#else
    fl_out_hnd=creat(mktemp(fl_out_tmp_sys),0600);
#endif
    fl_out_hnd=fl_out_hnd; /* Removes compiler warning on SGI */
    if(dbg_lvl_get() > 2) (void)fprintf(stdout,"%s: nco_fl_out_open() reports strlen(fl_out_tmp_sys) = %ld, fl_out_tmp_sys = %s, \n",prg_nm_get(),(long)strlen(fl_out_tmp_sys),fl_out_tmp_sys);
  } /* endif dbg */

  /* If temporary file already exists, prompt user to remove temporary files and exit */
  if(rcd != -1){
    (void)fprintf(stdout,"%s: ERROR temporary file %s already exists, remove and try again\n",prg_nm_get(),fl_out_tmp);
    nco_exit(EXIT_FAILURE);
  } /* end if */

  if(FORCE_OVERWRITE){
    rcd=nco_create(fl_out_tmp,NC_CLOBBER,out_id);
    /*    rcd=nc_create(fl_out_tmp,NC_CLOBBER|NC_SHARE,out_id);*/
    return fl_out_tmp;
  } /* end if */

  if(False){
    if(prg_get() == ncrename){
      /* ncrename is different because a single filename is allowed without question */
      /* Incur expense of copying current file to temporary file */
      (void)nco_fl_cp(fl_out,fl_out_tmp);
      rcd=nco_open(fl_out_tmp,NC_WRITE,out_id); 
      (void)nco_redef(*out_id);
      return fl_out_tmp;
    } /* end if */
  } /* end if false */

  rcd=stat(fl_out,&stat_sct);
  
  /* If permanent file already exists, query user whether to overwrite, append, or exit */
  if(rcd != -1){
    char usr_reply='z';
    short nbr_itr=0;
    
    if(FORCE_APPEND){
      /* Incur expense of copying current file to temporary file */
      (void)nco_fl_cp(fl_out,fl_out_tmp);
      rcd=nco_open(fl_out_tmp,NC_WRITE,out_id); 
      (void)nco_redef(*out_id);
      return fl_out_tmp;
    } /* end if */

    while(usr_reply != 'o' && usr_reply != 'a' && usr_reply != 'e'){
      nbr_itr++;
      if(nbr_itr > 10){
	(void)fprintf(stdout,"\n%s: ERROR %hd failed attempts to obtain valid interactive input. Assuming non-interactive shell and exiting.\n",prg_nm_get(),nbr_itr-1);
	nco_exit(EXIT_FAILURE);
      } /* end if */
      if(nbr_itr > 1) (void)fprintf(stdout,"%s: ERROR Invalid response.\n",prg_nm_get());
      (void)fprintf(stdout,"%s: %s exists---`o'verwrite, `a'ppend/replace, or `e'xit (o/a/e)? ",prg_nm_get(),fl_out);
      (void)fflush(stdout);
      usr_reply=(char)fgetc(stdin);
      /* Allow one carriage return per response free of charge */
      if(usr_reply == '\n') usr_reply=(char)fgetc(stdin);
      (void)fflush(stdin);
    } /* end while */
    
    switch(usr_reply){
    case 'e':
      nco_exit(EXIT_SUCCESS);
      break;
    case 'o':
      rcd=nco_create(fl_out_tmp,NC_CLOBBER,out_id);
      /*    rcd=nc_create(fl_out_tmp,NC_CLOBBER|NC_SHARE,out_id);*/
      break;
    case 'a':
      /* Incur expense of copying current file to temporary file */
      (void)nco_fl_cp(fl_out,fl_out_tmp);
      rcd=nco_open(fl_out_tmp,NC_WRITE,out_id); 
      (void)nco_redef(*out_id);
      break;
    } /* end switch */
    
  }else{ /* output file does not yet already exist */
    rcd=nco_create(fl_out_tmp,NC_NOCLOBBER,out_id);
    /*    rcd=nc_create(fl_out_tmp,NC_NOCLOBBER|NC_SHARE,out_id);*/
  } /* end if output file does not already exist */
  
  return fl_out_tmp;
  
} /* end nco_fl_out_open() */

void
nco_fl_out_cls /* [fnc] Close temporary output file, move it to permanent output file */
(const char * const fl_out, /* I [sng] Name of permanent output file */
 const char * const fl_out_tmp, /* I [sng] Name of temporary output file to close and move to permanent output file */
 const int nc_id) /* I [id] file ID of fl_out_tmp */
{
  /* Purpose: Close temporary output file, move it to permanent output file */
  int rcd; /* [rcd] Return code */

  rcd=nco_close(nc_id);
  if(rcd != NC_NOERR){
    (void)fprintf(stdout,"%s: ERROR nco_fl_out_cls() is unable to ncclose() file %s\n",prg_nm_get(),fl_out_tmp);
    nco_exit(EXIT_FAILURE); 
  } /* end if */
  
  (void)fl_mv(fl_out_tmp,fl_out);

} /* end nco_fl_out_cls() */

void
nco_fl_cmp_err_chk(void) /* [fnc] Perform error checking on file */
{
  /* Purpose: Perform error checking on file */
} /* end nco_fl_cmp_err_chk() */

