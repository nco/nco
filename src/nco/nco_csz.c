/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_csz.c,v 1.2 2002-05-02 06:44:19 zender Exp $ */

/* Purpose: Standalone utilities for C programs (no netCDF required) */

/* Copyright (C) 1995--2002 Charlie Zender
   This software is distributed under the terms of the GNU General Public License
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

/* Standard C headers */
#include <stdio.h> /* stderr, FILE, NULL, etc. */
#include <string.h> /* strcmp. . . */
#include <time.h> /* machine time */

/* 3rd party vendors */
#ifdef _OPENMP
#include <omp.h> /* OpenMP pragmas */
#endif /* not _OPENMP */

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

