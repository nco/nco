void *nco_malloc(size_t size)
{
  /* Purpose: Custom wrapper for malloc()
     Routine prints error when malloc() returns a NULL pointer 
     Routine does not call malloc() when size == 0 */
  
  void *ptr; /* [ptr] Pointer to new buffer */
  
  /* malloc(0) is ANSI-legal, albeit unnecessary
     NCO sometimes employs this degenerate case behavior of malloc() to simplify code
     Some debugging tools like Electric Fence consider any NULL returned by malloc() to be an error
     So circumvent malloc() calls when size == 0 */
  if(size == 0) return NULL;
  
  ptr=malloc(size); /* [ptr] Pointer to new buffer */
  if(ptr == NULL){
    (void)fprintf(stdout,"%s: ERROR nco_malloc() unable to allocate %li bytes\n",prg_nm_get(),(long)size);
    /* fxm: Should be exit(8) on ENOMEM errors? */
    exit(EXIT_FAILURE);
  } /* endif */
  return ptr; /* [ptr] Pointer to new buffer */
} /* nco_malloc() */

void *nco_free(void *vp)
{
  /* Purpose: Custom wrapper for free()
     Free memory and set pointer to NULL
     Routine does not call free() when vp == NULL
     Usage: vp=nco_free(vp) */
  if(vp != NULL) free(vp);
  return NULL; /* [ptr] Pointer to new buffer */
} /* nco_free() */

void *nco_calloc(size_t lmn_nbr,size_t lmn_sz)
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
    exit(EXIT_FAILURE);
  } /* endif */
  return ptr; /* [ptr] Pointer to new buffer */
} /* nco_calloc() */

void *nco_realloc(void *ptr,size_t size)
{
  /* Purpose: Custom wrapper for realloc()
     Routine prints error when realloc() returns a NULL pointer
     Routine does not call realloc() when size == 0 */
  
  void *new_ptr; /* [ptr] Pointer to new buffer */
  
  /* This degenerate case sometimes occurs
     Performing realloc() call here would be ANSI-legal but would trigger Electric Fence */
  if(ptr == NULL && size == 0) return ptr;
  if(ptr != NULL && size == 0){
    ptr=nco_free(ptr);
    ptr=NULL;
    return ptr;
  } /* endif */
  
  /* Passing NULL to realloc() is ANSI-legal, but may cause portability problems */
  if(ptr == NULL && size != 0){
    new_ptr=nco_malloc(size); /* [ptr] Pointer to new buffer */
  }else{
    new_ptr=realloc(ptr,size); /* [ptr] Pointer to new buffer */
  } /* endif */
  if(new_ptr == NULL && size != 0){
    (void)fprintf(stdout,"%s: ERROR nco_realloc() unable to realloc() %li bytes\n",prg_nm_get(),(long)size); 
    /* fxm: Should be exit(8) on ENOMEM errors? */
    exit(EXIT_FAILURE);
  } /* endif */
  return new_ptr; /* [ptr] Pointer to new buffer */
} /* nco_realloc() */

