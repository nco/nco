/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_omp.c,v 1.14 2004-07-02 23:01:13 zender Exp $ */

/* Purpose: OpenMP utilities */

/* Copyright (C) 1995--2004 Charlie Zender
   This software may be modified and/or re-distributed under the terms of the GNU General Public License (GPL) Version 2
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

#include "nco_omp.h" /* OpenMP utilities */

int /* O [nbr] Thread number */
nco_openmp_ini /* [fnc] Initialize OpenMP threading environment */
(const int thr_nbr) /* I [nbr] User-requested thread number */
{
  /* Purpose: Initialize OpenMP multi-threading environment
     Honor user-requested thread number, balance against known code efficiency,
     print diagnostics */
  FILE * const fp_stderr=stderr; /* [fl] stderr filehandle CEWI */
  int thr_nbr_act=0; /* O [nbr] Number of threads NCO uses */

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
  bool USR_SPC_THR_RQS=False;
  const int dyn_thr=1; /* [flg] Allow system to dynamically set number of threads */
  int thr_nbr_max_fsh=4; /* [nbr] Maximum number of threads program can use efficiently */
  int thr_nbr_max; /* [nbr] Maximum number of threads system/user allow program to use */
  int prc_nbr_max; /* [nbr] Maximum number of processors available */
  int thr_nbr_rqs=int_CEWI; /* [nbr] Number of threads to request */

  if(thr_nbr < 0){
    (void)fprintf(fp_stderr,"%s: ERROR User-requested thread number = %d is less than zero\n",prg_nm_get(),thr_nbr);
    nco_exit(EXIT_FAILURE);
  } /* endif err */

  if(thr_nbr > 0) USR_SPC_THR_RQS=True;

  thr_nbr_max=omp_get_max_threads(); /* [nbr] Maximum number of threads system allows */
  prc_nbr_max=omp_get_num_procs(); /* [nbr] Maximum number of processors available */
  if(dbg_lvl_get() > 2){
    (void)fprintf(fp_stderr,"%s: INFO Maximum number of threads system allows is %d\n",prg_nm_get(),thr_nbr_max);
    (void)fprintf(fp_stderr,"%s: INFO Number of processors available is %d\n",prg_nm_get(),prc_nbr_max);
  } /* endif dbg */

  if(USR_SPC_THR_RQS){
    /* Honor user-specified thread request... */
    thr_nbr_rqs=thr_nbr; /* [nbr] Number of threads to request */
    /* ...if possible... */
    if(dbg_lvl_get() > 2) (void)fprintf(fp_stderr,"%s: INFO User requested %d threads\n",prg_nm_get(),thr_nbr);
    if(thr_nbr > thr_nbr_max){
      (void)fprintf(fp_stderr,"%s: WARNING Reducing user-requested thread number = %d to maximum thread number allowed = %d\n",prg_nm_get(),thr_nbr,thr_nbr_max);
      thr_nbr_rqs=thr_nbr_max; /* [nbr] Number of threads to request */
    } /* endif */
  }else{
    /* Automatic thread allocation algorithm */

    /* Request maximum number of threads permitted */
    thr_nbr_rqs=thr_nbr_max; /* [nbr] Number of threads to request */

    /* Disable threading on per-program basis to play nicely with others */
    /* ncrcat is extremely I/O intensive 
       Maximum efficiency when one thread reads from input file while other writes to output file */
    if(strstr(prg_nm_get(),"ncrcat")) thr_nbr_max_fsh=1;
    /* fxm TODO nco345: ncwa threading has not been fully debugged */
    if(strstr(prg_nm_get(),"ncwa")) thr_nbr_max_fsh=1;
    
    /* Play nice with others */
    (void)omp_set_dynamic(dyn_thr); /* [flg] Allow system to dynamically set number of threads */
    if(dbg_lvl_get() > 0) (void)fprintf(fp_stderr,"%s: INFO Allowing OS to dynamically set threads\n",prg_nm_get());

    /* Apply program/system limitations */
    if(thr_nbr_max > thr_nbr_max_fsh){
      if(dbg_lvl_get() > 0) (void)fprintf(fp_stderr,"%s: INFO Reducing default thread number from %d to %d, a \"play-nice\" number set in nco_openmp_ini()\n",prg_nm_get(),thr_nbr_max,thr_nbr_max_fsh);
      thr_nbr_rqs=thr_nbr_max_fsh; /* [nbr] Number of threads to request */
    } /* endif */      
  } /* endif */      

  /* Set thread number */
  if(omp_in_parallel()){
    (void)fprintf(fp_stderr,"%s: ERROR Attempted to set thread number from within parallel region\n",prg_nm_get());
    nco_exit(EXIT_FAILURE);
  }else{
    (void)omp_set_num_threads(thr_nbr_rqs); 
    if(dbg_lvl_get() > 0) (void)fprintf(fp_stderr,"%s: INFO %s requested %d threads from system\n",prg_nm_get(),prg_nm_get(),thr_nbr_rqs);
  } /* end error */

  if(!USR_SPC_THR_RQS){
  } /* USR_SPC_THR_RQS */

#pragma omp parallel default(none) shared(fp_stderr,thr_nbr_act)
  { /* begin OpenMP parallel */
#pragma omp single nowait
    { /* begin OpenMP single */
      thr_nbr_act=omp_get_num_threads(); /* [nbr] Number of threads NCO uses */
      if(dbg_lvl_get() > 0) (void)fprintf(fp_stderr,"%s: INFO Parallel regions spawn teams of %d threads\n",prg_nm_get(),thr_nbr_act);
    } /* end OpenMP single */
  } /* end OpenMP parallel */
#else /* !_OPENMP */
  thr_nbr_act+=0*thr_nbr; /* CEWI */
  if(dbg_lvl_get() > 0) (void)fprintf(fp_stderr,"%s: INFO Not attempting OpenMP threading\n",prg_nm_get());
#endif /* !_OPENMP */
  
  return thr_nbr_act; /* O [nbr] Number of threads NCO uses */
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
