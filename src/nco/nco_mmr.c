/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_mmr.c,v 1.17 2004-07-06 18:35:34 zender Exp $ */

/* Purpose: Memory management */

/* Copyright (C) 1995--2004 Charlie Zender
   This software may be modified and/or re-distributed under the terms of the GNU General Public License (GPL) Version 2
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

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
  if(vp != NULL) free(vp);
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

  extern int errno; /* [enm] Error code in errno.h */

  void *ptr; /* [ptr] Pointer to new buffer */
  
  /* malloc(0) is ANSI-legal, albeit unnecessary
     NCO sometimes employs this degenerate case behavior of malloc() to simplify code
     Some debugging tools like Electric Fence consider any NULL returned by malloc() to be an error
     So circumvent malloc() calls when sz == 0 */
  if(sz == 0) return NULL;
  
  ptr=malloc(sz); /* [ptr] Pointer to new buffer */
  if(ptr == NULL){
    (void)fprintf(stdout,"%s: WARNING nco_malloc_flg() unable to allocate %lu bytes\n",prg_nm_get(),(unsigned long)sz);
    (void)fprintf(stdout,"%s: malloc() error is \"%s\"\n",prg_nm_get(),strerror(errno));
    if(errno == ENOMEM) return NULL; /* Unlike nco_malloc(), allow simple OOM errors */
    else (void)fprintf(stdout,"%s: ERROR is not ENOMEM, exiting...\n",prg_nm_get());
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

  extern int errno; /* [enm] Error code in errno.h */

  void *ptr; /* [ptr] Pointer to new buffer */
  
  /* malloc(0) is ANSI-legal, albeit unnecessary
     NCO sometimes employs this degenerate case behavior of malloc() to simplify code
     Some debugging tools like Electric Fence consider any NULL returned by malloc() to be an error
     So circumvent malloc() calls when sz == 0 */
  if(sz == 0) return NULL;
  
  ptr=malloc(sz); /* [ptr] Pointer to new buffer */
  if(ptr == NULL){
    (void)fprintf(stdout,"%s: ERROR malloc() returns error on %s request for %lu bytes\n",prg_nm_get(),fnc_nm,(unsigned long)sz);
    (void)fprintf(stdout,"%s: malloc() error is \"%s\"\n",prg_nm_get(),strerror(errno));
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
const char * /* O [sng] String describing type */
nco_mmr_typ_sng /* [fnc] Convert netCDF type enum to string */
(nc_type type) /* I [enm] netCDF type */
{
  /* Purpose: Return name of memory function invoked */
  switch(type){
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

  switch(type){
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
  if(ptr != NULL && sz == 0){
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
(const int nco_mmr_typ, /* I [enm] Memory allocation type */
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
