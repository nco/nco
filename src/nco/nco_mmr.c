/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_mmr.c,v 1.43 2013-01-13 06:07:47 zender Exp $ */

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
    (void)fprintf(stdout,"%s: ERROR nco_calloc() unable to allocate %lu elements of %lu bytes = %lu bytes\n",prg_nm_get(),(unsigned long)lmn_nbr,(unsigned long)lmn_sz,(unsigned long)(lmn_nbr*lmn_sz));
    nco_exit(EXIT_FAILURE);
  } /* endif */
#ifdef NCO_MMR_DBG
  (void)nco_mmr_stt(nco_mmr_calloc,lmn_nbr*lmn_sz); /* fxm dbg */
#endif /* !NCO_MMR_DBG */
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
#ifdef NCO_MMR_DBG
  (void)nco_mmr_stt(nco_mmr_free,(size_t)0L); /* fxm dbg */
#endif /* !NCO_MMR_DBG */
  return NULL; /* [ptr] Pointer to new buffer */
} /* nco_free() */

void * /* O [ptr] Pointer to allocated memory */
nco_malloc /* [fnc] Wrapper for malloc() */
(const size_t sz) /* I [B] Bytes to allocate */
{
  /* Purpose: Custom plugin wrapper for malloc()
     Top of nco_mmr.c explains usage of nco_malloc(), nco_malloc_flg(), and nco_malloc_dbg() */

  void *ptr; /* [ptr] Pointer to new buffer */
  
  /* malloc(0) is ANSI-legal, albeit unnecessary
     NCO sometimes employs this degenerate case behavior of malloc() to simplify code
     Some debugging tools like Electric Fence consider any NULL returned by malloc() to be an error
     So circumvent malloc() calls when sz == 0 */
  if(sz == 0) return NULL;
  
  ptr=malloc(sz); /* [ptr] Pointer to new buffer */
  if(ptr == NULL){
    (void)fprintf(stdout,"%s: ERROR nco_malloc() unable to allocate %lu bytes\n",prg_nm_get(),(unsigned long)sz);
    (void)nco_malloc_err_hnt_prn();
    /* fxm: Should be exit(8) on ENOMEM errors? */
    nco_exit(EXIT_FAILURE);
  } /* endif */
#ifdef NCO_MMR_DBG
  (void)nco_mmr_stt(nco_mmr_malloc,sz); /* fxm dbg */
#endif /* !NCO_MMR_DBG */
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
    (void)fprintf(stdout,"%s: WARNING nco_malloc_flg() unable to allocate %lu bytes\n",prg_nm_get(),(unsigned long)sz);
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
#ifdef NCO_MMR_DBG
  (void)nco_mmr_stt(nco_mmr_malloc,sz); /* fxm dbg */
#endif /* !NCO_MMR_DBG */
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
    (void)fprintf(stdout,"%s: ERROR malloc() returns error on %s request for %lu bytes\n",prg_nm_get(),fnc_nm,(unsigned long)sz);
#ifndef __GNUG__
    /* 20051205: Triggers G++ error: undefined reference to `__errno_location()' */
    (void)fprintf(stdout,"%s: malloc() error is \"%s\"\n",prg_nm_get(),strerror(errno));
#endif /* __GNUG__ */
    (void)fprintf(stdout,"%s: User-supplied supplemental error message is \"%s\"\n",prg_nm_get(),msg);
    (void)nco_malloc_err_hnt_prn();
    nco_exit(EXIT_FAILURE);
  } /* endif */
#ifdef NCO_MMR_DBG
  (void)nco_mmr_stt(nco_mmr_malloc,sz); /* fxm dbg */
#endif /* !NCO_MMR_DBG */
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
#ifdef NCO_MMR_DBG
  (void)nco_mmr_stt(nco_mmr_realloc,sz); /* fxm dbg */
#endif /* !NCO_MMR_DBG */
  return new_ptr; /* [ptr] Pointer to new buffer */
} /* nco_realloc() */

long /* O [nbr] Net memory currently allocated */
nco_mmr_stt /* [fnc] Track memory statistics */
(const nco_mmr_typ_enm nco_mmr_typ, /* I [enm] Memory management type */
 const size_t sz) /* I [B] Bytes allocated, deallocated, or reallocated */
{
  /* Purpose: Track memory statistics */
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

long /* O [B] Maximum resident set size */
nco_mmr_rusage_prn /* [fnc] Print rusage memory usage statistics */
(const int rusage_who) /* [enm] RUSAGE_SELF, RUSAGE_CHILDREN, RUSAGE_LWP */
{
  /* Purpose: Track memory statistics */

  /* Routine is intended to be purely diagnostic.
     Currently only accessed by ncap when compiled with NCO_RUSAGE_DBG */

  /* NB: As of kernel 2.6.9, Linux only maintains rusage fields ru_utime, ru_stime, ru_minflt, ru_majflt, and ru_nswap */
  int rcd;
  int sz_pg; /* [B] Page size in Bytes */
#ifdef HAVE_GETRUSAGE
  struct rusage usg;
#endif /* !HAVE_GETRUSAGE */

  /* Get page size. NECSX does not have getpagesize(). */
#ifdef HAVE_GETPAGESIZE
  sz_pg=getpagesize();
#else /* !HAVE_GETPAGESIZE */
  /* CEWI */
  sz_pg=rusage_who;
#endif /* !HAVE_GETPAGESIZE */

#ifdef HAVE_GETRUSAGE

  /* rusage reports size and time in OS-dependent units:
     AIX uses kilobytes [kB] for size [sz] and seconds [s] for time [tm]:
     ru_maxrss [kB], ru_ixrss [kB s], ru_idrss [kB], ru_idrss [kB]
     http://publib.boulder.ibm.com/infocenter/pseries/index.jsp?topic=/com.ibm.aix.doc/libs/basetrf1/getrusage_64.htm

     IRIX uses bytes [B] for size [sz] and 
     ru_maxrss [kB], ru_ixrss [pg tck], ru_idrss [pg], ru_idrss [pg]

     Linux does not implement these fields yet
     ru_maxrss, ru_ixrss, ru_idrss, ru_idrss

     Solaris uses pages [pg] for size and ticks [tck] for time: 
     ru_maxrss [pg], ru_ixrss [pg tck], ru_idrss [pg], ru_idrss [pg]
     http://docs.sun.com/app/docs/doc/816-5168/6mbb3hr9o?a=view */

#ifdef AIX
  (void)fprintf(stdout,"%s: INFO nco_mmr_rusage_prn() reports system type is AIX so rusage uses kilobytes [kB] for size and seconds [s] for time. Page size is %d kB.\n",prg_nm_get(),sz_pg);
#endif /* !AIX */
#ifdef CRAY
  (void)fprintf(stdout,"%s: INFO nco_mmr_rusage_prn() reports system type is CRAY so rusage units for page size and time are unknown.\n",prg_nm_get());
#endif /* !CRAY */
#if (defined LINUX) || (defined LINUXAMD64)
  (void)fprintf(stdout,"%s: INFO nco_mmr_rusage_prn() reports system type is LINUX so rusage does not implement ru_maxrss, ru_ixrss, ru_idrss, and ru_idrss. Page size is %d B.\n",prg_nm_get(),sz_pg);
#endif /* !LINUX */
#ifdef NECSX
  (void)fprintf(stdout,"%s: INFO nco_mmr_rusage_prn() reports system type is NECSX so rusage units for page size and time are unknown.\n",prg_nm_get());
#endif /* !NECSX */
#ifdef SGIMP64
  (void)fprintf(stdout,"%s: INFO nco_mmr_rusage_prn() reports system type is SGIMP64 so rusage uses bytes [B] for size, while time units are unknown. Page size is %d B.\n",prg_nm_get(),sz_pg);
#endif /* !SGIMP64 */
#ifdef SUNMP
  (void)fprintf(stdout,"%s: INFO nco_mmr_rusage_prn() reports system type is SUNMP so rusage uses pages [pg] for size and ticks [tck] for time. Page size is %d B.\n",prg_nm_get(),sz_pg);
#endif /* !SUNMP */

  /* fxm: CEWI, not necessary */
  rcd=rusage_who;
  /* fxm: use input argument rusage_who instead or RUSAGE_SELF */
  rcd=0*rcd+getrusage(RUSAGE_SELF,&usg);
  (void)fprintf(stdout,"%s: INFO nco_mmr_rusage_prn() reports: rusage.ru_utime.tv_sec = user time used = %li s, rusage.ru_utime.tv_usec = user time used = %li us, rusage.ru_stime.tv_sec = system time used = %li s, rusage.ru_stime.tv_usec = system time used = %li us, rusage.ru_maxrss = maximum resident set size = %li [sz], rusage.ru_ixrss = integral shared memory size =  %li [sz tm], rusage.ru_idrss = integral unshared data size = %li [sz], rusage.ru_isrss = integral unshared stack size = %li [sz], rusage.ru_minflt = page reclaims = %li, rusage.ru_majflt = page faults = %li, rusage.ru_nswap = swaps = %li\n",prg_nm_get(),usg.ru_utime.tv_sec,usg.ru_utime.tv_usec,usg.ru_stime.tv_sec,usg.ru_stime.tv_usec,usg.ru_maxrss,usg.ru_ixrss,usg.ru_idrss,usg.ru_isrss,usg.ru_minflt,usg.ru_majflt,usg.ru_nswap);

  return (long)usg.ru_maxrss; /* [B] Maximum resident set size */
#else /* !HAVE_GETRUSAGE */
  return (long)0; /* [B] Fake return value */
#endif /* !HAVE_GETRUSAGE */

} /* nco_mmr_rusage_prn() */
