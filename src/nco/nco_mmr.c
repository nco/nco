/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_mmr.c,v 1.6 2002-12-30 02:56:15 zender Exp $ */

/* Purpose: Memory management */

/* Copyright (C) 1995--2003 Charlie Zender
   This software is distributed under the terms of the GNU General Public License
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

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
    (void)fprintf(stdout,"%s: ERROR nco_calloc() unable to allocate %li elements of %li bytes = %li bytes\n",prg_nm_get(),(long)lmn_nbr,(long)lmn_sz,(long)(lmn_nbr*lmn_sz));
    nco_exit(EXIT_FAILURE);
  } /* endif */
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
  return NULL; /* [ptr] Pointer to new buffer */
} /* nco_free() */

void * /* O [ptr] Pointer to allocated memory */
nco_malloc /* [fnc] Wrapper for malloc() */
(const size_t sz) /* I [nbr] Number of bytes to allocate */
{
  /* Purpose: Custom wrapper for malloc()
     Routine prints error when malloc() returns a NULL pointer 
     Routine does not call malloc() when sz == 0 */
  
  void *ptr; /* [ptr] Pointer to new buffer */
  
  /* malloc(0) is ANSI-legal, albeit unnecessary
     NCO sometimes employs this degenerate case behavior of malloc() to simplify code
     Some debugging tools like Electric Fence consider any NULL returned by malloc() to be an error
     So circumvent malloc() calls when sz == 0 */
  if(sz == 0) return NULL;
  
  ptr=malloc(sz); /* [ptr] Pointer to new buffer */
  if(ptr == NULL){
    (void)fprintf(stdout,"%s: ERROR nco_malloc() unable to allocate %li bytes\n",prg_nm_get(),(long)sz);
    /* fxm: Should be exit(8) on ENOMEM errors? */
    nco_exit(EXIT_FAILURE);
  } /* endif */
  return ptr; /* [ptr] Pointer to new buffer */
} /* nco_malloc() */

void * /* O [ptr] Pointer to re-allocated memory */
nco_realloc /* [fnc] Wrapper for realloc() */
(void *ptr, /* I/O [ptr] Buffer to reallocate */
 const size_t sz) /* I [nbr] Number of bytes required */
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
    (void)fprintf(stdout,"%s: ERROR nco_realloc() unable to realloc() %li bytes\n",prg_nm_get(),(long)sz); 
    /* fxm: Should be exit(8) on ENOMEM errors? */
    nco_exit(EXIT_FAILURE);
  } /* endif */
  return new_ptr; /* [ptr] Pointer to new buffer */
} /* nco_realloc() */

