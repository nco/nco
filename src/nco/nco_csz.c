/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_csz.c,v 1.1 2002-05-02 06:10:30 zender Exp $ */

/* Purpose: Standalone utilities for C programs (no netCDF required) */

/* Copyright (C) 1995--2002 Charlie Zender
   This software is distributed under the terms of the GNU General Public License
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

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

