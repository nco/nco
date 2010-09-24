/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_omp.c,v 1.51 2010-09-24 17:05:33 zender Exp $ */

/* Purpose: OpenMP utilities */

/* Copyright (C) 1995--2010 Charlie Zender
   License: GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */

#include "nco_omp.h" /* OpenMP utilities */

#ifndef _OPENMP
/* OpenMP is not available with this compiler
   Declare harmless stub routines for Uni-Processor (UP) code
   These stubs reduce pre-processor proliferation */
int omp_get_dynamic(void){return 0;}
int omp_get_max_threads(void){return 1;}
int omp_get_nested(void){return 0;}
int omp_get_num_procs(void){return 1;}
int omp_get_num_threads(void){return 1;}
int omp_get_thread_num(void){return 0;}
int omp_in_parallel(void){return 0;}
/* Fake work code in these functions to avoid C++ warnings, i.e., CEWI */
void omp_set_dynamic(int dynamic_threads){int foo=0;dynamic_threads+=foo;}
void omp_set_nested(int nested){int foo=0;nested+=foo;}
void omp_set_num_threads(int num_threads){int foo=0;num_threads+=foo;}
#endif /* _OPENMP */

int /* O [nbr] Thread number */
nco_openmp_ini /* [fnc] Initialize OpenMP threading environment */
(const int thr_nbr) /* I [nbr] User-requested thread number */
{
  /* Purpose: Initialize OpenMP multi-threading environment
     Honor user-requested thread number, balance against known code efficiency,
     print diagnostics
     Returns thr_nbr=1 in three situations:
     1. UP codes (not threaded)
     2. SMP codes compiled with compilers which lack OpenMP support
     3. SMP codes where single thread requested/advised
     Otherwise returns system-dependent thr_nbr */

  /* Using naked stdin/stdout/stderr in parallel region generates warning
     Copy appropriate filehandle to variable scoped shared in parallel clause */

  char *nvr_OMP_NUM_THREADS; /* [sng] Environment variable OMP_NUM_THREADS */
  char *sng_cnv_rcd=char_CEWI; /* [sng] strtol()/strtoul() return code */
  FILE * const fp_stderr=stderr; /* [fl] stderr filehandle CEWI */

  nco_bool USR_SPC_THR_RQS=False;

  int dyn_thr=1; /* [flg] Allow system to dynamically set number of threads */

  int ntg_OMP_NUM_THREADS=int_CEWI; // [nbr] OMP_NUM_THREADS environment variable
  int prc_nbr_max; /* [nbr] Maximum number of processors available */
  int thr_nbr_act; /* O [nbr] Number of threads NCO uses */
  int thr_nbr_max_fsh=4; /* [nbr] Maximum number of threads program can use efficiently */
  int thr_nbr_max=int_CEWI; /* [nbr] Maximum number of threads system allows */
  int thr_nbr_rqs=int_CEWI; /* [nbr] Number of threads to request */

#ifndef _OPENMP
  if(dbg_lvl_get() > 0) (void)fprintf(fp_stderr,"%s: INFO Build compiler lacked (or user turned off) OpenMP support. Code will execute with single thread in Uni-Processor (UP) mode.\n",prg_nm_get());
  return (int)1;
#endif /* !_OPENMP */

  /* Strategy: 
     0. Determine maximum number of threads system will allocate (thr_nbr_max)
     1. Command-line thread request, if any, overrides automatic algorithm
     2. If no command-line request then system allocates OMP_NUM_THREADS if possible
     3. Reduce maximum number of threads available to system to thr_nbr_max_fsh
     Many operators cannot use more than thr_nbr_max_fsh ~ 2--4 threads efficiently
     Play nice: Set dynamic threading so that system can make efficiency decisions
     When dynamic threads are set, system never allocates more than thr_nbr_max_fsh */
  if(thr_nbr < 0){
    (void)fprintf(fp_stderr,"%s: ERROR User-requested thread number = %d is less than zero\n",prg_nm_get(),thr_nbr);
    nco_exit(EXIT_FAILURE);
  } /* endif err */

  if(thr_nbr == 0)
    if(dbg_lvl_get() > 2)
      (void)fprintf(fp_stderr,"%s: INFO User did not specify thread request > 0 on command line. NCO will automatically assign threads based on OMP_NUM_THREADS environment and machine capabilities.\nHINT: Not specifiying any --thr_nbr (or specifying --thr_nbr=0) causes NCO to try to pick the optimal thread number. Specifying --thr_nbr=1 tells NCO to execute in Uni-Processor (UP) (i.e., single-threaded) mode.\n",prg_nm_get());

  if(thr_nbr > 0) USR_SPC_THR_RQS=True;

  prc_nbr_max=omp_get_num_procs(); /* [nbr] Maximum number of processors available */
  if(omp_in_parallel()){
    (void)fprintf(fp_stderr,"%s: ERROR Attempted to get maximum thread number from within parallel region\n",prg_nm_get());
    nco_exit(EXIT_FAILURE);
  }else{
    thr_nbr_max=omp_get_max_threads(); /* [nbr] Maximum number of threads system allows */
  } /* end error */

  if(dbg_lvl_get() > 2){
    if((nvr_OMP_NUM_THREADS=getenv("OMP_NUM_THREADS"))) ntg_OMP_NUM_THREADS=(int)strtol(nvr_OMP_NUM_THREADS,&sng_cnv_rcd,NCO_SNG_CNV_BASE10); /* [sng] Environment variable OMP_NUM_THREADS */
    if(*sng_cnv_rcd) nco_sng_cnv_err(nvr_OMP_NUM_THREADS,"strtol",sng_cnv_rcd);
    (void)fprintf(fp_stderr,"%s: INFO Environment variable OMP_NUM_THREADS ",prg_nm_get());
    if(ntg_OMP_NUM_THREADS > 0) (void)fprintf(fp_stderr,"= %d\n",ntg_OMP_NUM_THREADS); else (void)fprintf(fp_stderr,"does not exist\n");
    (void)fprintf(fp_stderr,"%s: INFO Number of processors available is %d\n",prg_nm_get(),prc_nbr_max);
    (void)fprintf(fp_stderr,"%s: INFO Maximum number of threads system allows is %d\n",prg_nm_get(),thr_nbr_max);
  } /* endif dbg */

  if(USR_SPC_THR_RQS){
    /* Always try to honor user-specified thread request... */
    thr_nbr_rqs=thr_nbr; /* [nbr] Number of threads to request */
    /* ...if possible... */
    if(dbg_lvl_get() > 2) (void)fprintf(fp_stderr,"%s: INFO User command-line-requested %d thread%s\n",prg_nm_get(),thr_nbr,(thr_nbr > 1) ? "s" : "");
    if(thr_nbr > thr_nbr_max){
      (void)fprintf(fp_stderr,"%s: WARNING Reducing user-requested thread number = %d to maximum thread number allowed = %d\n",prg_nm_get(),thr_nbr,thr_nbr_max);
      thr_nbr_rqs=thr_nbr_max; /* [nbr] Number of threads to request */
    } /* endif */
  }else{ /* !USR_SPC_THR_RQS */
    /* Otherwise use automatic thread allocation algorithm */

    /* Request maximum number of threads permitted */
    thr_nbr_rqs=thr_nbr_max; /* [nbr] Number of threads to request */

    /* Restrict threading on per-program basis to play nicely with others */
    switch(prg_get()){
      /* Operators with pre-set thread limit
	 NB: All operators currently have default restrictions
	 Only ncwa and ncap2 have a chance to scale on non-parallel filesystems
	 ncap2 may, one day, see a big performance boost from threading
	 However, as of 20090327, ncap2 threading may be buggy due to ANTLR
	 Moreover, we want to prevent hogging processes on 32-way nodes
	 until/unless clear benefits of threading are demonstrated. */
    case ncap: 
      /* 20090327: Restrict ncap2 to one thread until ANTLR threading resolved */
      thr_nbr_max_fsh=1;
      break;
    case ncecat: 
    case ncrcat: 
      /* ncecat and ncrcat are extremely I/O intensive 
	 Maximum efficiency when one thread reads from input file while other writes to output file */
      thr_nbr_max_fsh=2;
      break;
      /* Operators with higher maximum pre-set thread limit (NB: not all of these are threaded!) */
    case ncbo: 
    case ncatted: 
    case ncea:
    case ncflint: 
    case ncks: 
    case ncpdq: 
    case ncra:
    case ncrename: 
    case ncwa: 
      thr_nbr_max_fsh=4;
      break;
    default: nco_dfl_case_prg_id_err(); break;
    } /* end case */
    
    /* Automatic algorithm tries to play nice with others */
    (void)omp_set_dynamic(dyn_thr); /* [flg] Allow system to dynamically set number of threads */
    if(dbg_lvl_get() > 0) (void)fprintf(fp_stderr,"%s: INFO %s OS to dynamically set threads\n",prg_nm_get(),(dyn_thr ? "Allowing" : "Not allowing"));
    dyn_thr=omp_get_dynamic(); /* [flg] Allow system to dynamically set number of threads */
    if(dbg_lvl_get() > 0) (void)fprintf(fp_stderr,"%s: INFO System will%s utilize dynamic threading\n",prg_nm_get(),(dyn_thr ? "" : " not"));

    /* Apply program/system limitations */
    if(thr_nbr_max > thr_nbr_max_fsh){
      if(dbg_lvl_get() > 0) (void)fprintf(fp_stderr,"%s: INFO Reducing default thread number from %d to %d, an operator-dependent \"play-nice\" number set in nco_openmp_ini()\n",prg_nm_get(),thr_nbr_max,thr_nbr_max_fsh);
      thr_nbr_rqs=thr_nbr_max_fsh; /* [nbr] Number of threads to request */
    } /* endif */      
  } /* !USR_SPC_THR_RQS */

#ifdef ENABLE_NETCDF4
  if(thr_nbr_rqs > 1){
    if(dbg_lvl_get() > 0) (void)fprintf(stdout,"%s: WARNING This is TODO nco939. Requested threading with netCDF4 (HDF5) support. The NCO thread request algorithm considers user-input, environment variables, and software and hardware limitations in determining the number of threads to request, thr_nbr_rqs. At this point NCO would request result %d threads from a netCDF3-based library. However, this NCO was built with netCDF4, which relies on HDF5. netCDF4 is not thread-safe unless HDF5 is configured with the (non-default) --enable-threadsafe option. NCO currently has no way to know whether HDF5 was built thread-safe. Hence, all netCDF4-based operators are currently restricted to a single thread. The program will now automatically set thr_nbr_rqs = 1.\nThis unfortunate limitation is necessary to keep the NCO developers sane. If you want/need threading in netCDF4-based NCO, please politely yet firmly request of the Unidata netCDF developers that better thread support be built into netCDF4, and request of the HDF5 developers that they make the --enable-threadsafe option compatible with all HDF5 libraries and APIs, including Fortran (which, as of HDF5 1.8.0 in 2008, is incompatible with --enable-threadsafe).\n",prg_nm_get(),thr_nbr_rqs);
    thr_nbr_rqs=1;
  } /* endif */
#endif /* !ENABLE_NETCDF4 */

  /* Set thread number */
  if(omp_in_parallel()){
    (void)fprintf(fp_stderr,"%s: ERROR Attempted to set thread number from within parallel region\n",prg_nm_get());
    nco_exit(EXIT_FAILURE);
  }else{
    (void)omp_set_num_threads(thr_nbr_rqs); 
    if(dbg_lvl_get() > 0) (void)fprintf(fp_stderr,"%s: INFO omp_set_num_threads() used to set execution environment to spawn teams of %d threads\n",prg_nm_get(),thr_nbr_rqs);
  } /* end error */

  thr_nbr_act=omp_get_max_threads();
  if(dbg_lvl_get() > 2) (void)fprintf(fp_stderr,"%s: INFO After using omp_set_num_threads() to adjust for any user requests/NCO optimizations, omp_get_max_threads() reports that a parallel construct here/now would spawn %d threads\n",prg_nm_get(),thr_nbr_act);
#ifdef _OPENMP
  if(dbg_lvl_get() > 2){
#pragma omp parallel default(none) shared(thr_nbr_act)
    { /* begin OpenMP parallel */
#pragma omp single nowait
      { /* begin OpenMP single */
	thr_nbr_act=omp_get_num_threads(); /* [nbr] Number of threads NCO uses */
	if(dbg_lvl_get() > 0) (void)fprintf(fp_stderr,"%s: INFO Small parallel test region spawned team of %d threads\n",prg_nm_get(),thr_nbr_act);
      } /* end OpenMP single */
    } /* end OpenMP parallel */
  } /* end dbg */
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
  (void)fprintf(stderr,"%s: INFO main loop thread #%d processing var_prc[%d] = \"%s\"\n",prg_nm_get(),omp_get_thread_num(),idx,var_nm);
#else /* !_OPENMP */
  rcd+=idx*0; /* CEWI */
  (void)fprintf(stderr,"%s: INFO main loop processing var_prc[%d] = \"%s\"\n",prg_nm_get(),idx,var_nm);
#endif /* !_OPENMP */

  return rcd;
} /* end nco_var_prc_crr_prn() */
