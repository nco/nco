/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_omp.c,v 1.7 2004-06-30 19:57:26 zender Exp $ */

/* Purpose: OpenMP utilities */

/* Copyright (C) 1995--2004 Charlie Zender
   This software may be modified and/or re-distributed under the terms of the GNU General Public License (GPL) Version 2
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

#include "nco_omp.h" /* OpenMP utilities */

int /* O [enm] Return code */
nco_openmp_ini(void) /* [fnc] Set up OpenMP multi-threading environment */
{
  /* Purpose: Set up OpenMP multi-threading environment */
  FILE * const fp_stderr=stderr; // [fl] stderr filehandle CEWI
  int rcd=0; /* [rcd] Return code */

  /* Using naked stdin/stdout/stderr in parallel region generates warning
     Copy appropriate filehandle to variable scoped shared in parallel clause */

#ifdef _OPENMP
  /* System allocates OMP_NUM_THREADS if possible
     ncwa is I/O bottlenecked beyond about thr_nbr_max_fsh=4 threads
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
    (void)fprintf(fp_stderr,"%s: INFO Reducing number of threads from %d to %d since %s hits I/O bottleneck above %d threads\n",prg_nm_get(),thr_nbr_max,thr_nbr_max_fsh,prg_nm_get(),thr_nbr_max_fsh);
    (void)omp_set_num_threads(thr_nbr_max_fsh); /* [nbr] Maximum number of threads system is allowed */
  } /* endif */      
  (void)omp_set_dynamic(dyn_thr); /* [flg] Allow system to dynamically set number of threads */
  if(dbg_lvl_get() > 0) (void)fprintf(fp_stderr,"%s: INFO Allowing OS to utilize dynamic threading\n",prg_nm_get());
#endif /* not _OPENMP */

  if(dbg_lvl_get() > 0){
#ifdef _OPENMP /* OpenMP-compliant compilers define _OPENMP=YYYYMM = year and month of OpenMP specification */
#pragma omp parallel default(none) shared(fp_stderr)
    { /* begin OpenMP parallel */
#pragma omp single nowait
      { /* begin OpenMP single */
	(void)fprintf(fp_stderr,"%s: INFO OpenMP multi-threading using %d threads\n",prg_nm_get(),omp_get_num_threads());
      } /* end OpenMP single */
    } /* end OpenMP parallel */
#else /* not _OPENMP */
    (void)fprintf(fp_stderr,"%s: INFO Not attempting OpenMP multi-threading\n",prg_nm_get());
#endif /* not _OPENMP */
  } /* endif dbg */

  return rcd;
} /* end nco_openmp_ini() */

int /* O [enm] Return code */
nco_var_prc_crr_prn /* [fnc] Print name of current variable */
(const int idx, /* I [idx] Index of current variable */
 const char * const var_nm) /* I [sng] Variable name */
{
  /* Purpose: Print name of current variable */
  int rcd=0; /* [rcd] Return code */

#ifdef _OPENMP
  (void)fprintf(stderr,"%s: INFO Thread #%d processing var_prc[%d] = \"%s\"\n",prg_nm_get(),omp_get_thread_num(),idx,var_nm);
#else /* not _OPENMP */
  rcd+=idx*0; /* CEWI */
  (void)fprintf(stderr,"%s, ",var_nm);
#endif /* not _OPENMP */

  return rcd;
} /* end nco_var_prc_crr_prn() */
