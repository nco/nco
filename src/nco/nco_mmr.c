/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_mmr.c,v 1.50 2013-06-25 19:31:46 zender Exp $ */

/* Purpose: Memory management */

/* Copyright (C) 1995--2013 Charlie Zender
   License: GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */

/* Usage of NCO memory allocation routines nco_malloc(), nco_malloc_flg(), and nco_malloc_dbg():
   nco_malloc(): Use this for small memory requests
   nco_malloc() dies and exits with generic malloc() error for all error conditions
   nco_malloc() plug-in replacements are malloc() and nco_malloc_flg() 
   
   nco_malloc_flg(): Use this for large memory requests when it is useful for calling routine 
   to handle ENOMEM errors (e.g., calling routine has important debug information).
   nco_malloc_flg() dies and exits with generic malloc() error unless error is ENOMEM
   nco_malloc_flg() prints warning for ENOMEM errors, then returns control to calling routine
   nco_malloc_flg() plug-in replacements are malloc() and nco_malloc() 
   
   nco_malloc_dbg(): Use this for large memory requests when calling routine supplies
   its name and a useful supplemental error message
   nco_malloc_dbg() prints name of calling function, supplemental error message, and then 
   dies and exits for all error conditions.
   nco_malloc_dbg() has no plug-in replacements (since it requires two extra arguments)
   
   None of these routines call malloc() when sz == 0 */

#include "nco_mmr.h" /* Memory management */

void * /* O [ptr] Pointer to calloc'd memory */
nco_calloc /* [fnc] Wrapper for calloc() */
(const size_t lmn_nbr, /* I [nbr] Number of elements to allocate */
 const size_t lmn_sz) /* I [nbr] Size of each element */
{
  /* Purpose: Custom wrapper for calloc(), modified from nco_malloc()
     Routine prints error when calloc() returns a NULL pointer 
     Routine does not call calloc() when lmn_sz == 0 or lmn_nbr == 0 */
  
  void *ptr; /* [ptr] Pointer to new buffer */
  
  /* Circumvent calloc() calls when lmn_sz == 0 */
  if(lmn_sz == 0 || lmn_nbr == 0) return NULL;
  
  ptr=calloc(lmn_nbr,lmn_sz); /* [ptr] Pointer to new buffer */
  if(ptr == NULL){
    (void)fprintf(stdout,"%s: ERROR nco_calloc() unable to allocate %lu elements of %lu bytes each totaling %lu B = %lu kB = %lu MB = %lu GB\n",prg_nm_get(),(unsigned long)lmn_nbr,(unsigned long)lmn_sz,(unsigned long)(lmn_nbr*lmn_sz),(unsigned long)(lmn_nbr*lmn_sz)/NCO_BYT_PER_KB,(unsigned long)(lmn_nbr*lmn_sz)/NCO_BYT_PER_MB,(unsigned long)(lmn_nbr*lmn_sz)/NCO_BYT_PER_GB);
    nco_exit(EXIT_FAILURE);
  } /* endif */
#ifdef NCO_MMR_STT
  (void)nco_mmr_stt(nco_mmr_calloc,lmn_nbr*lmn_sz); /* fxm dbg */
#endif /* !NCO_MMR_STT */
  return ptr; /* [ptr] Pointer to new buffer */
} /* nco_calloc() */

void * /* O [ptr] Buffer after free'ing */
nco_free /* [fnc] Wrapper for free() */
(void *vp) /* I/O [ptr] Buffer to free() */
{
  /* Purpose: Custom wrapper for free()
     Free memory and set pointer to NULL
     Routine does not call free() when vp == NULL
     Usage: vp=nco_free(vp) */
  if(vp) free(vp);
#ifdef NCO_MMR_STT
  (void)nco_mmr_stt(nco_mmr_free,(size_t)0L); /* fxm dbg */
#endif /* !NCO_MMR_STT */
  return NULL; /* [ptr] Pointer to new buffer */
} /* nco_free() */

void * /* O [ptr] Pointer to allocated memory */
nco_malloc /* [fnc] Wrapper for malloc() */
(const size_t sz) /* I [B] Bytes to allocate */
{
  /* Purpose: Custom plugin wrapper for malloc()
     Top of nco_mmr.c explains usage of nco_malloc(), nco_malloc_flg(), and nco_malloc_dbg() */

  const char fnc_nm[]="nco_malloc()"; /* [sng] Function name */
  char *nvr_NCO_MMR_DBG; /* [sng] Environment variable NCO_MMR_DBG */
  char *sng_cnv_rcd=NULL_CEWI; /* [sng] strtol()/strtoul() return code */

  int ntg_NCO_MMR_DBG=int_CEWI; // [nbr] NCO_MMR_DBG environment variable

  const size_t sz_thr; /* I [B] Bytes to allocate threshold size for reporting */

  void *ptr; /* [ptr] Pointer to new buffer */
  
  /* malloc(0) is ANSI-legal, albeit unnecessary
     NCO sometimes employs this degenerate case behavior of malloc() to simplify code
     Some debugging tools like Electric Fence consider any NULL returned by malloc() to be an error
     So circumvent malloc() calls when sz == 0 */
  if(sz == 0) return NULL;
  
  /* Only poll memory if debug level set otherwise getenv() would be called on every nco_malloc() reading  */
  if(dbg_lvl_get() >= nco_dbg_scl){
    if((nvr_NCO_MMR_DBG=getenv("NCO_MMR_DBG"))) ntg_NCO_MMR_DBG=(int)strtol(nvr_NCO_MMR_DBG,&sng_cnv_rcd,NCO_SNG_CNV_BASE10); /* [sng] Environment variable NCO_MMR_DBG */
  } /* endif dbg */

  if(ntg_NCO_MMR_DBG && sz > sz_thr){
    (void)fprintf(stdout,"%s: INFO %s received request to allocate %zu B = %zu kB = %zu MB = %zu GB\n",prg_nm_get(),fnc_nm,sz,sz/NCO_BYT_PER_KB,sz/NCO_BYT_PER_MB,sz/NCO_BYT_PER_GB);
  } /* endif dbg */

  ptr=malloc(sz); /* [ptr] Pointer to new buffer */
  if(ptr == NULL){
    (void)fprintf(stdout,"%s: ERROR %s unable to allocate %zu B = %zu kB = %zu MB = %zu GB\n",prg_nm_get(),fnc_nm,sz,sz/NCO_BYT_PER_KB,sz/NCO_BYT_PER_MB,sz/NCO_BYT_PER_GB);
    (void)nco_malloc_err_hnt_prn();
    /* fxm: Should be exit(8) on ENOMEM errors? */
    nco_exit(EXIT_FAILURE);
  } /* endif */
#ifdef NCO_MMR_STT
  (void)nco_mmr_stt(nco_mmr_malloc,sz); /* fxm dbg */
#endif /* !NCO_MMR_STT */
  return ptr; /* [ptr] Pointer to new buffer */
} /* nco_malloc() */

void * /* O [ptr] Pointer to allocated memory */
nco_malloc_flg /* [fnc] Wrapper for malloc(), forgives ENOMEM errors */
(const size_t sz) /* I [B] Bytes to allocate */
{
  /* Purpose: Custom plugin wrapper for malloc() that allows ENOMEM errors
     Top of nco_mmr.c explains usage of nco_malloc(), nco_malloc_flg(), and nco_malloc_dbg() */

#ifndef __GNUG__
  extern int errno; /* [enm] Error code in errno.h */
#endif /* __GNUG__ */

  void *ptr; /* [ptr] Pointer to new buffer */
  
  /* malloc(0) is ANSI-legal, albeit unnecessary
     NCO sometimes employs this degenerate case behavior of malloc() to simplify code
     Some debugging tools like Electric Fence consider any NULL returned by malloc() to be an error
     So circumvent malloc() calls when sz == 0 */
  if(sz == 0) return NULL;
  
  ptr=malloc(sz); /* [ptr] Pointer to new buffer */
  if(ptr == NULL){
    (void)fprintf(stdout,"%s: ERROR nco_malloc_flg() unable to allocate %lu B = %lu kB = %lu MB = %lu GB\n",prg_nm_get(),(unsigned long)sz,(unsigned long)sz/NCO_BYT_PER_KB,(unsigned long)sz/NCO_BYT_PER_MB,(unsigned long)sz/NCO_BYT_PER_GB);
#ifndef __GNUG__
    /* 20051205: Triggers G++ error: undefined reference to `__errno_location()' */
    (void)fprintf(stdout,"%s: malloc() error is \"%s\"\n",prg_nm_get(),strerror(errno));
    if(errno == ENOMEM) return NULL; /* Unlike nco_malloc(), allow simple OOM errors */
#else
    return NULL; /* Unlike nco_malloc(), allow simple OOM errors */
#endif /* __GNUG__ */
    (void)fprintf(stdout,"%s: ERROR is not ENOMEM, exiting...\n",prg_nm_get());
    (void)nco_malloc_err_hnt_prn();
    nco_exit(EXIT_FAILURE);
  } /* endif */
#ifdef NCO_MMR_STT
  (void)nco_mmr_stt(nco_mmr_malloc,sz); /* fxm dbg */
#endif /* !NCO_MMR_STT */
  return ptr; /* [ptr] Pointer to new buffer */
} /* nco_malloc_flg() */

void * /* O [ptr] Pointer to allocated memory */
nco_malloc_dbg /* [fnc] Wrapper for malloc(), receives and prints more diagnostics */
(const size_t sz, /* I [B] Bytes to allocate */
 const char *fnc_nm, /* I [sng] Function name */
 const char *msg) /* I [sng] Supplemental error message */
{
  /* Purpose: Custom wrapper for malloc(), non-plugin, receives and prints more diagnostics
     Top of nco_mmr.c explains usage of nco_malloc(), nco_malloc_flg(), and nco_malloc_dbg() */

#ifndef __GNUG__
  extern int errno; /* [enm] Error code in errno.h */
#endif /* __GNUG__ */

  void *ptr; /* [ptr] Pointer to new buffer */
  
  /* malloc(0) is ANSI-legal, albeit unnecessary
     NCO sometimes employs this degenerate case behavior of malloc() to simplify code
     Some debugging tools like Electric Fence consider any NULL returned by malloc() to be an error
     So circumvent malloc() calls when sz == 0 */
  if(sz == 0) return NULL;
  
  ptr=malloc(sz); /* [ptr] Pointer to new buffer */
  if(ptr == NULL){
    (void)fprintf(stdout,"%s: ERROR malloc() returns error on %s request for %lu B = %lu kB = %lu MB = %lu GB\n",prg_nm_get(),fnc_nm,(unsigned long)sz,(unsigned long)sz/NCO_BYT_PER_KB,(unsigned long)sz/NCO_BYT_PER_MB,(unsigned long)sz/NCO_BYT_PER_GB);
#ifndef __GNUG__
    /* 20051205: Triggers G++ error: undefined reference to `__errno_location()' */
    (void)fprintf(stdout,"%s: malloc() error is \"%s\"\n",prg_nm_get(),strerror(errno));
#endif /* __GNUG__ */
    (void)fprintf(stdout,"%s: User-supplied supplemental error message is \"%s\"\n",prg_nm_get(),msg);
    (void)nco_malloc_err_hnt_prn();
    nco_exit(EXIT_FAILURE);
  } /* endif */
#ifdef NCO_MMR_STT
  (void)nco_mmr_stt(nco_mmr_malloc,sz); /* fxm dbg */
#endif /* !NCO_MMR_STT */
  return ptr; /* [ptr] Pointer to new buffer */
} /* nco_malloc_dbg() */

void
nco_malloc_err_hnt_prn /* [fnc] Explain meaning and workarounds for malloc() failures */
(void)
{
  /* Purpose: Explain meaning and workarounds for malloc() failures */
  (void)fprintf(stdout,"%s: INFO NCO has reported a malloc() failure. malloc() failures usually indicate that your machine does not have enough free memory (RAM+swap) to perform the requested operation. As such, malloc() failures result from the physical limitations imposed by your hardware. Read http://nco.sf.net/nco.html#mmr for a description of NCO memory usage. There are two workarounds in this scenario. One is to process your data in smaller chunks. The other is to use a machine with more free memory.\n\nLarge tasks may uncover memory leaks in NCO. This is likeliest to occur with ncap. ncap scripts are completely dynamic and may be of arbitrary length and complexity. A script that contains many thousands of operations may uncover a slow memory leak even though each single operation consumes little additional memory. Memory leaks are usually identifiable by their memory usage signature. Leaks cause peak memory usage to increase monotonically with time regardless of script complexity. Slow leaks are very difficult to find. Sometimes a malloc() failure is the only noticeable clue to their existance. If you have good reasons to believe that your malloc() failure is ultimately due to an NCO memory leak (rather than inadequate RAM on your system), then we would be very interested in receiving a detailed bug report.",prg_nm_get());
} /* nco_malloc_err_hnt_prn() */

/* fxm: when are const qualifiers on return values legal? is this a GNUism? */
const char * /* O [sng] String describing memory type */
nco_mmr_typ_sng /* [fnc] Convert NCO memory management type enum to string */
(const nco_mmr_typ_enm nco_mmr_typ) /* I [enm] NCO memory management type */
{
  /* Purpose: Return name of memory function invoked */
  switch(nco_mmr_typ){
  case nco_mmr_calloc:
    return "nco_mmr_calloc";
  case nco_mmr_free:
    return "nco_mmr_free";
  case nco_mmr_malloc:
    return "nco_mmr_malloc";
  case nco_mmr_realloc:
    return "nco_mmr_realloc";
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */

/* fxm: any advantage to this form? defining with file scope in header?
  static const char * const nco_mmr_calloc_sng="nco_mmr_calloc_sng";
  static const char * const nco_mmr_free_sng="nco_mmr_free_sng";
  static const char * const nco_mmr_malloc_sng="nco_mmr_malloc_sng";
  static const char * const nco_mmr_realloc_sng="nco_mmr_realloc_sng";

  switch(nco_mmr_typ){
  case nco_mmr_calloc:
    return nco_mmr_calloc_sng;
  case nco_mmr_free:
    return nco_mmr_free_sng;
  case nco_mmr_malloc:
    return nco_mmr_malloc_sng;
  case nco_mmr_realloc:
  return nco_mmr_realloc_sng;
  default: nco_dfl_case_nc_type_err(); break;
  } *//* end switch */
  
  /* Some compilers, e.g., SGI cc, need return statement to end non-void functions */
  return (char *)NULL;
} /* end nco_mmr_typ_sng() */

void * /* O [ptr] Pointer to re-allocated memory */
nco_realloc /* [fnc] Wrapper for realloc() */
(void *ptr, /* I/O [ptr] Buffer to reallocate */
 const size_t sz) /* I [B] Bytes required */
{
  /* Purpose: Custom wrapper for realloc()
     Routine prints error when realloc() returns a NULL pointer
     Routine does not call realloc() when sz == 0 */
  
  void *new_ptr; /* [ptr] Pointer to new buffer */
  
  /* This degenerate case sometimes occurs
     Performing realloc() call here would be ANSI-legal but would trigger Electric Fence */
  if(ptr == NULL && sz == 0) return ptr;
  if(ptr && sz == 0){
    ptr=nco_free(ptr);
    ptr=NULL;
    return ptr;
  } /* endif */
  
  /* Passing NULL to realloc() is ANSI-legal, but may cause portability problems */
  if(ptr == NULL && sz != 0){
    new_ptr=nco_malloc(sz); /* [ptr] Pointer to new buffer */
  }else{
    new_ptr=realloc(ptr,sz); /* [ptr] Pointer to new buffer */
  } /* endif */
  if(new_ptr == NULL && sz != 0){
    (void)fprintf(stdout,"%s: ERROR nco_realloc() unable to realloc() %lu bytes\n",prg_nm_get(),(unsigned long)sz); 
    /* fxm: Should be exit(8) on ENOMEM errors? */
    nco_exit(EXIT_FAILURE);
  } /* endif */
#ifdef NCO_MMR_STT
  (void)nco_mmr_stt(nco_mmr_realloc,sz); /* fxm dbg */
#endif /* !NCO_MMR_STT */
  return new_ptr; /* [ptr] Pointer to new buffer */
} /* nco_realloc() */

long /* O [nbr] Net memory currently allocated */
nco_mmr_stt /* [fnc] Track memory statistics */
(const nco_mmr_typ_enm nco_mmr_typ, /* I [enm] Memory management type */
 const size_t sz) /* I [B] Bytes allocated, deallocated, or reallocated */
{
  /* Purpose: Track memory statistics
     20130618: Maing problem with this is the nco_free() and nco_realloc() do 
     not provide sufficent information to adjust the total.
     Workarounds are to poll /proc/self/stat here, or to modify nco_free() and
     nco_realloc() to optionally propagate net memory used or freed. */

  static long mll_nbr=0L; /* [nbr] Number of malloc() invocations */
  static long fre_nbr=0L; /* [nbr] Number of free() invocations */
  static long mmr_mll_ttl=0L; /* [B] Total memory malloc()'d */
  static long mmr_fre_ttl=0L; /* [B] Total memory free()'d */
  static long mmr_net_crr=0L; /* [B] Net memory currently allocated */
  long sz_lng; /* [B] Bytes allocated, deallocated, or reallocated, long */

  sz_lng=(long)sz; /* [B] Bytes allocated, deallocated, or reallocated */
  switch(nco_mmr_typ){
  case nco_mmr_calloc:
    mll_nbr++; /* [nbr] Number of malloc() invocations */
    mmr_mll_ttl+=sz_lng; /* [B] Total memory malloc()'d */
    mmr_net_crr+=sz_lng; /* [B] Net memory currently allocated */
    break;
  case nco_mmr_free:
    fre_nbr++; /* [nbr] Number of free() invocations */
    mmr_fre_ttl-=sz_lng; /* [B] Total memory free()'d */
    mmr_net_crr-=sz_lng; /* [B] Net memory currently allocated */
    break;
  case nco_mmr_malloc:
    mll_nbr++; /* [nbr] Number of malloc() invocations */
    mmr_mll_ttl+=sz_lng; /* [B] Total memory malloc()'d */
    mmr_net_crr+=sz_lng; /* [B] Net memory currently allocated */
    break;
  case nco_mmr_realloc:
    mll_nbr++; /* [nbr] Number of malloc() invocations */
    mmr_mll_ttl+=sz_lng; /* [B] Total memory malloc()'d */
    mmr_net_crr+=sz_lng; /* [B] Net memory currently allocated */
    break;
  default: 
    nco_exit(EXIT_FAILURE);
    break;
  } /* end case */

  if(True){
    (void)fprintf(stdout,"%s: INFO nco_mmr_stt() called by %s(): fre_nbr=%li, mll_nbr=%li, mmr_mll_ttl=%li, mmr_fre_ttl=%li, mmr_net_crr=%li bytes\n",prg_nm_get(),nco_mmr_typ_sng(nco_mmr_typ),fre_nbr,mll_nbr,mmr_mll_ttl,mmr_fre_ttl,mmr_net_crr);
  } /* endif */

  return mmr_net_crr; /* [B] Net memory currently allocated */
} /* nco_mmr_stt() */

int /* Return code */
nco_prc_stt_get /* [fnc] Read /proc/PID/stat */
(const int pid, /* [enm] Process ID to read */
 prc_stt_sct *prc_stt) /* [sct] Structure to hold results */
{
  /* Purpose: Decode output of /proc/PID/stat, place in structure, and optionally print out */

  /* 201306: Unfortunately, format of /proc/self/stat is system-dependent
     Contents of /proc/self/stat defined in man 5 proc
     Have found _slightly_ different scanf() formats for fields looking at different Linux documentation 
     Definitive answer seems to examining contents of /usr/src/linux/fs/proc/array.c:do_task_stat()
     Fully documented sources at http://www.cs.tufts.edu/comp/111/assignments/a3/proc.c */

  const char fnc_nm[]="nco_prc_stt_get()"; /* [sng] Function name */
  char fl_slf[]="/proc/self/stat"; /* [sng] Process status pseudo-file name */
  char fl_pid[256]; /* [sng] Process status pseudo-file name */
  char *fl_prc; /* [sng] Process status pseudo-file name */

  const char *prc_stt_fmt="%d %s %c %d %d %d %d %d %lu %lu %lu %lu %lu %lu %lu %ld %ld %ld %ld %ld %ld %lu %lu %ld %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %d %d %lu %lu %llu";
  const char *prc_stt_fmt_out="pid = %d, comm = %s, state = %c, ppid = %d, pgrp = %d, session = %d, tty_nr = %d, tpgid = %d, flags = %lu, minflt = %lu, cminflt = %lu, majflt = %lu, cmajflt = %lu, utime = %lu, stime = %lu, cutime = %ld, cstime = %ld, priority = %ld, nice = %ld, num_threads = %ld, itrealvalue = %ld, starttime = %lu, vsize = %lu, rss = %ld, rlim = %lu, startcode = %lu, endcode = %lu, startstack = %lu, kstkesp = %lu, kstkeip = %lu, signal = %lu, blocked = %lu, sigignore = %lu, sigcatch = %lu, wchan = %lu, nswap = %lu, cnswap = %lu, exit_signal = %d, processor = %d, rt_priority = %lu, policy = %lu, delayacct_blkio_ticks = %llu\n";

  FILE *fp_prc=NULL; /* [fl] Process status file handle */

  int rcd_sys;

  if(pid) (void)sprintf(fl_pid,"/proc/%d/stat",pid);
  if(pid) fl_prc=fl_pid; else fl_prc=fl_slf;
  fp_prc=fopen(fl_prc,"r");
  /* Unable to open fp_prc */
  if(!fp_prc) return 0; 

  rcd_sys=fscanf
    (fp_prc,
     prc_stt_fmt,
     &prc_stt->pid,
     prc_stt->comm,
     &prc_stt->state,
     &prc_stt->ppid,
     &prc_stt->pgrp,
     &prc_stt->session,
     &prc_stt->tty_nr,
     &prc_stt->tpgid,
     &prc_stt->flags,
     &prc_stt->minflt,
     &prc_stt->cminflt,
     &prc_stt->majflt,
     &prc_stt->cmajflt,
     &prc_stt->utime,
     &prc_stt->stime,
     &prc_stt->cutime,
     &prc_stt->cstime,
     &prc_stt->priority,
     &prc_stt->nice,
     &prc_stt->num_threads,
     &prc_stt->itrealvalue,
     &prc_stt->starttime,
     &prc_stt->vsize,
     &prc_stt->rss,
     &prc_stt->rlim,
     &prc_stt->startcode,
     &prc_stt->endcode,
     &prc_stt->startstack,
     &prc_stt->kstkesp,
     &prc_stt->kstkeip,
     &prc_stt->signal,
     &prc_stt->blocked,
     &prc_stt->sigignore,
     &prc_stt->sigcatch,
     &prc_stt->wchan,
     &prc_stt->nswap,
     &prc_stt->cnswap,
     &prc_stt->exit_signal,
     &prc_stt->processor,
     &prc_stt->rt_priority,
     &prc_stt->policy,
     &prc_stt->delayacct_blkio_ticks);

  rcd_sys=fclose(fp_prc);

  if(dbg_lvl_get() > nco_dbg_std){
    char *prc_stt_sng;
    prc_stt_sng=(char *)nco_malloc(2048UL*sizeof(char));
    sprintf(prc_stt_sng,prc_stt_fmt_out,prc_stt->pid,prc_stt->comm,prc_stt->state,prc_stt->ppid,prc_stt->pgrp,prc_stt->session,prc_stt->tty_nr,prc_stt->tpgid,prc_stt->flags,prc_stt->minflt,prc_stt->cminflt,prc_stt->majflt,prc_stt->cmajflt,prc_stt->utime,prc_stt->stime,prc_stt->cutime,prc_stt->cstime,prc_stt->priority,prc_stt->nice,prc_stt->num_threads,prc_stt->itrealvalue,prc_stt->starttime,prc_stt->vsize,prc_stt->rss,prc_stt->rlim,prc_stt->startcode,prc_stt->endcode,prc_stt->startstack,prc_stt->kstkesp,prc_stt->kstkeip,prc_stt->signal,prc_stt->blocked,prc_stt->sigignore,prc_stt->sigcatch,prc_stt->wchan,prc_stt->nswap,prc_stt->cnswap,prc_stt->exit_signal,prc_stt->processor,prc_stt->rt_priority,prc_stt->policy,prc_stt->delayacct_blkio_ticks);
    (void)fprintf(stdout,"%s: INFO %s polled %s and found: %s\n",prg_nm_get(),fnc_nm,fl_prc,prc_stt_sng);
    if(prc_stt_sng) prc_stt_sng=(char *)nco_free(prc_stt_sng);
  } /* endif dbg */

  /* Were expected number of fields read? */
  if(rcd_sys == PRC_STT_SCT_NBR) return NCO_NOERR; else return NCO_ERR;

} /* nco_prc_stt_get() */

int /* Return code */
nco_prc_stm_get /* [fnc] Read /proc/PID/statm */
(const int pid, /* [enm] Process ID to read */
 prc_stm_sct *prc_stm) /* [sct] Structure to hold results */
{
  /* Purpose: Decode output of /proc/PID/stat, place in structure, and optionally print out */

  /* 201306: Unfortunately, format of /proc/self/statm is system-dependent
     Contents of /proc/self/statm defined in man 5 proc
     Fully documented sources at http://www.cs.tufts.edu/comp/111/assignments/a3/proc.c */

  const char fnc_nm[]="nco_prc_stm_get()"; /* [sng] Function name */
  char fl_slf[]="/proc/self/statm"; /* [sng] Process status pseudo-file name */
  char fl_pid[256]; /* [sng] Process status pseudo-file name */
  char *fl_prc; /* [sng] Process status pseudo-file name */

  const char *prc_stm_fmt="%lu %lu %lu %lu %lu %lu %lu";
  const char *prc_stm_fmt_out="size = %lu, resident = %lu, share = %lu, text = %lu, lib = %lu, data = %lu, dt = %lu\n";

  FILE *fp_prc=NULL; /* [fl] Process status file handle */

  int rcd_sys;

  if(pid) (void)sprintf(fl_pid,"/proc/%d/stat",pid);
  if(pid) fl_prc=fl_pid; else fl_prc=fl_slf;
  fp_prc=fopen(fl_prc,"r");
  /* Unable to open fp_prc */
  if(!fp_prc) return 0; 

  rcd_sys=fscanf
    (fp_prc,
     prc_stm_fmt,
     &prc_stm->size,
     &prc_stm->resident,
     &prc_stm->share,
     &prc_stm->text,
     &prc_stm->lib,
     &prc_stm->data,
     &prc_stm->dt);

  rcd_sys=fclose(fp_prc);

  if(dbg_lvl_get() > nco_dbg_std){
    char *prc_stm_sng;
    prc_stm_sng=(char *)nco_malloc(2048UL*sizeof(char));
    sprintf(prc_stm_sng,prc_stm_fmt_out,prc_stm->size,prc_stm->resident,prc_stm->share,prc_stm->text,prc_stm->lib,prc_stm->data,prc_stm->dt);
    (void)fprintf(stdout,"%s: INFO %s polled %s and found: %s\n",prg_nm_get(),fnc_nm,fl_prc,prc_stm_sng);
    if(prc_stm_sng) prc_stm_sng=(char *)nco_free(prc_stm_sng);
  } /* endif dbg */

  /* Were expected number of fields read? */
  if(rcd_sys == PRC_STM_SCT_NBR) return NCO_NOERR; else return NCO_ERR;

} /* nco_prc_stm_get() */

long /* O [B] Maximum resident set size */
nco_mmr_usg_prn /* [fnc] Print rusage memory usage statistics */
(const int rusage_who) /* [enm] RUSAGE_SELF, RUSAGE_CHILDREN, RUSAGE_LWP */
{
  /* Purpose: Track memory statistics */

  /* Routine is intended to be purely diagnostic
     Currently only accessed with ncks --sysconf
     rss [B] = RSS = Resident Set Size = Portion of process's memory in RAM. Rest is in swap or not loaded.
     vsize [B] = Virtual Memory Size
     rsslim [B] = Soft limit on process RSS, as per getrlimit() */

  /* 20130617: Remik Ziemlinski's ncx has _SC_PAGE_SIZE example in ezNcUtil.hpp */ 

  const char fnc_nm[]="nco_mmr_usg_prn()"; /* [sng] Function name */

  int rcd_stm; /* [enm] Return code for /proc/PID/statm call */
  int rcd_stt; /* [enm] Return code for /proc/PID/stat call */
  int rcd_sys; /* [enm] Return code for system call */

#ifndef __GNUG__
  extern int errno; /* [enm] Error code in errno.h */
#endif /* __GNUG__ */

  /* Get page size. NECSX does not have getpagesize(). */
  int sz_pg; /* [B] Page size in Bytes */
  sz_pg=rusage_who; /* CEWI */
#ifdef HAVE_GETPAGESIZE
  /* 20130617: Deprecate getpagesize() in favor of sysconf() because Linux man pages say that 
     "Portable applications should employ sysconf(_SC_PAGESIZE) instead of getpagesize()"
     sysconf() tells us to first use, if it exists, the PAGESIZE environment variable
     Otherwise use the sysconf() macro on _SC_PAGESIZE */
  //     sz_pg=getpagesize();
# ifdef PAGESIZE
  sz_pg=PAGESIZE
# else /* !PAGESIZE */
  sz_pg=sysconf(_SC_PAGESIZE);
#  ifndef __GNUG__
  if(sz_pg < 0) (void)fprintf(stdout,"%s: sysconf() error is \"%s\"\n",prg_nm_get(),strerror(errno));
#  endif /* __GNUG__ */
  if(sz_pg < 0) nco_exit(EXIT_FAILURE);
# endif /* !PAGESIZE */
#endif /* !HAVE_GETPAGESIZE */

  /* Interrorgate /proc/self/stat for current memory usage */
  prc_stt_sct prc_stt;
  rcd_stt=nco_prc_stt_get((int)0,&prc_stt);
  if(dbg_lvl_get() > nco_dbg_std) (void)fprintf(stdout,"%s: INFO %s thinks pid = %d, comm = %s, ppid = %d, rlim = %lu B = %lu kB = %lu MB, rss = %ld B = %ld kB = %ld MB, vsize = %lu B = %lu kB = %lu MB = %lu GB\n",prg_nm_get(),fnc_nm,prc_stt.pid,prc_stt.comm,prc_stt.ppid,prc_stt.rlim,prc_stt.rlim/NCO_BYT_PER_KB,prc_stt.rlim/NCO_BYT_PER_MB,prc_stt.rss,prc_stt.rss/NCO_BYT_PER_KB,prc_stt.rss/NCO_BYT_PER_MB,prc_stt.vsize,prc_stt.vsize/NCO_BYT_PER_KB,prc_stt.vsize/NCO_BYT_PER_MB,prc_stt.vsize/NCO_BYT_PER_GB);

  prc_stm_sct prc_stm;
  rcd_stm=nco_prc_stm_get((int)0,&prc_stm);
  if(dbg_lvl_get() > nco_dbg_std) (void)fprintf(stdout,"%s: INFO %s thinks size = %lu B = %lu kB = %lu MB = %lu GB, resident = %lu B = %lu kB = %lu MB = %lu GB\n",prg_nm_get(),fnc_nm,prc_stm.size,prc_stm.size/NCO_BYT_PER_KB,prc_stm.size/NCO_BYT_PER_MB,prc_stm.size/NCO_BYT_PER_GB,prc_stm.resident,prc_stm.resident/NCO_BYT_PER_KB,prc_stm.resident/NCO_BYT_PER_MB,prc_stm.resident/NCO_BYT_PER_GB);

#ifdef HAVE_GETRUSAGE

  /* getrusage() reports size and time in OS-dependent units:
     AIX getrusage() uses kilobytes [kB] for size [sz] and seconds [s] for time [tm]:
     ru_maxrss [kB], ru_ixrss [kB s], ru_idrss [kB], ru_idrss [kB]
     http://publib.boulder.ibm.com/infocenter/pseries/index.jsp?topic=/com.ibm.aix.doc/libs/basetrf1/getrusage_64.htm

     IRIX getrusage() uses bytes [B] for size [sz] and 
     ru_maxrss [kB], ru_ixrss [pg tck], ru_idrss [pg], ru_idrss [pg]

     Linux getrusage() uses kilobytes [kB] for size [sz]
     In 201306 (kernel 3.9) Linux (_still_) does not implement ru_ixrss, ru_idrss, ru_idrss
     As of 200912 (kernel 2.6.32) Linux implements ru_maxrss [kB]
     I first tried Linux getrusage() in 200503, on kernel 2.6.9
     In 200410 (kernel 2.6.9), Linux did not implement ru_maxrss, ru_ixrss, ru_idrss, or ru_idrs
     In 200410 (kernel 2.6.9), Linux only maintained rusage fields ru_utime, ru_stime, ru_minflt, ru_majflt, and ru_nswap

     Solaris getrusage() uses pages [pg] for size and ticks [tck] for time: 
     ru_maxrss [pg], ru_ixrss [pg tck], ru_idrss [pg], ru_idrss [pg]
     http://docs.sun.com/app/docs/doc/816-5168/6mbb3hr9o?a=view */

  struct rusage usg;

#ifdef AIX
  (void)fprintf(stdout,"%s: INFO %s() reports system type is AIX so getrusage() uses kilobytes [kB] for size and seconds [s] for time. Page size is %d kB.\n",prg_nm_get(),fnc_nm,sz_pg);
#endif /* !AIX */
#ifdef CRAY
  (void)fprintf(stdout,"%s: INFO %s() reports system type is CRAY so getrusage() units for page size and time are unknown.\n",prg_nm_get());
#endif /* !CRAY */
#if (defined LINUX) || (defined LINUXAMD64)
  (void)fprintf(stdout,"%s: INFO %s() reports system type is LINUX so getrusage() does implement ru_maxrss [kB] and DOES NOT implement ru_ixrss, ru_idrss, and ru_idrss. Page size is %d B.\n",prg_nm_get(),fnc_nm,sz_pg);
#endif /* !LINUX */
#ifdef NECSX
  (void)fprintf(stdout,"%s: INFO %s() reports system type is NECSX so getrusage() units for page size and time are unknown.\n",prg_nm_get());
#endif /* !NECSX */
#ifdef SGIMP64
  (void)fprintf(stdout,"%s: INFO %s() reports system type is SGIMP64 so getrusage() uses bytes [B] for size, while time units are unknown. Page size is %d B.\n",prg_nm_get(),fnc_nm,sz_pg);
#endif /* !SGIMP64 */
#ifdef SUNMP
  (void)fprintf(stdout,"%s: INFO %s() reports system type is SUNMP so getrusage() uses pages [pg] for size and ticks [tck] for time. Page size is %d B.\n",prg_nm_get(),fnc_nm,sz_pg);
#endif /* !SUNMP */

  /* fxm: CEWI, not necessary */
  rcd_sys=rusage_who;
  /* fxm: use input argument rusage_who instead of RUSAGE_SELF */
  rcd_sys=0*rcd_sys+getrusage(RUSAGE_SELF,&usg);
  if(dbg_lvl_get() > nco_dbg_io) (void)fprintf(stdout,"%s: INFO %s() reports: rusage.ru_utime.tv_sec = user time used = %li s, rusage.ru_utime.tv_usec = user time used = %li us, rusage.ru_stime.tv_sec = system time used = %li s, rusage.ru_stime.tv_usec = system time used = %li us, rusage.ru_maxrss = maximum resident set size = %li [sz], rusage.ru_ixrss = integral shared memory size =  %li [sz tm], rusage.ru_idrss = integral unshared data size = %li [sz], rusage.ru_isrss = integral unshared stack size = %li [sz], rusage.ru_minflt = page reclaims = %li, rusage.ru_majflt = page faults = %li, rusage.ru_nswap = swaps = %li\n",prg_nm_get(),fnc_nm,usg.ru_utime.tv_sec,usg.ru_utime.tv_usec,usg.ru_stime.tv_sec,usg.ru_stime.tv_usec,usg.ru_maxrss,usg.ru_ixrss,usg.ru_idrss,usg.ru_isrss,usg.ru_minflt,usg.ru_majflt,usg.ru_nswap);

  return (long)usg.ru_maxrss; /* [B] Maximum resident set size */
#else /* !HAVE_GETRUSAGE */
  return (long)0; /* [B] Fake return value */
#endif /* !HAVE_GETRUSAGE */

} /* nco_mmr_usg_prn() */
