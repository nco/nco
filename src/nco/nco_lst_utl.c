/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_lst_utl.c,v 1.9 2002-05-14 00:06:01 zender Exp $ */

/* Purpose: List utilities */

/* Copyright (C) 1995--2002 Charlie Zender
   This software is distributed under the terms of the GNU General Public License
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

#include "nco_lst_utl.h" /* List utilities */

/* NB: Free (speech) sorting by Joerg Shoen available from
   http://www.pci.uni-heidelberg.de/tc/usr/joerg
   are faster than ANSI system qsort() in all cases 
   See code in $DATA/tmp/testsort */

int /* O [enm] Comparison result [<,=,>]0 iff val_1 [<,==,>] val_2 */
nco_cmp_int /* [fnc] Compare two integers */
(const void *val_1, /* I [nbr] Number to compare */
 const void *val_2) /* I [nbr] Number to compare */
{
  /* Purpose: Compare two integers
     Function is suitable for argument to ANSI C qsort() routine in stdlib.h
     Code stolen from http://www.cplusplus.com/ref/cstdlib/qsort.html */
  return (*(int *)val_1-*(int *)val_2);
} /* end nco_cmp_int() */

int /* O [enm] Comparison result [<,=,>]0 iff val_1 [<,==,>] val_2 */
nco_cmp_chr /* [fnc] Compare two characters */
(const void *val_1, /* I [chr] Character to compare */
 const void *val_2) /* I [chr] Character to compare */
{
  /* Purpose: Compare two characters
     Function is suitable for argument to ANSI C qsort() routine in stdlib.h
     Code stolen from http://www.cplusplus.com/ref/cstdlib/qsort.html */
  return (*(char *)val_1-*(char *)val_2);
} /* end nco_cmp_chr() */

int /* O [enm] Comparison result [<,=,>]0 iff val_1 [<,==,>] val_2 */
nco_cmp_sng /* [fnc] Compare two strings */
(const void *val_1, /* I [sng] String to compare */
 const void *val_2) /* I [sng] String to compare */
{
  /* Purpose: Compare two string
     Function is suitable for argument to ANSI C qsort() routine in stdlib.h
     http://www.eskimo.com/~scs/C-faq/q13.8.html describes sorting strings: 
     Arguments to qsort()'s comparison function are pointers to objects being sorted,
     i.e., pointers to pointers to char. 
     strcmp(), however, accepts simple pointers to char
     Therefore, strcmp() cannot be used directly as comparison function for qsort()
     This wrapper casts input values to simple char pointers, calls strcmp(), and feeds results back to qsort() */
  return strcmp(*(char * const *)val_1,*(char * const *)val_2);
} /* end nco_cmp_sng() */

char * /* O [sng] Concatenated string formed by joining all input strings */
sng_lst_prs /* [fnc] Join list of strings together into one string */
(char * const * const sng_lst, /* I [sng] List of pointers to strings to join together */
 const long lmn_nbr, /* O [nbr] Number of strings in list */
 const char * const dlm_sng) /* I [sng] delimiter string to use as glue */
{
  /* Purpose: Join list of strings together into one string
     Elements of input list should all be NUL-terminated strings
     Elements with value NUL are interpreted as strings of zero length */

  char *sng; /* Output string */

  int dlm_len;
  long lmn;
  long sng_sz=0L; /* NB: sng_sz get incremented */

  if(lmn_nbr == 1L) return sng_lst[0];

  /* Delimiter must be NUL-terminated (a string) so strlen() works */
  if(dlm_sng == NULL){
    (void)fprintf(stdout,"%s: ERROR sng_lst_prs() reports delimiter string is NULL\n",prg_nm_get());
    nco_exit(EXIT_FAILURE);
  } /* end if */
  dlm_len=strlen(dlm_sng); 

  /* List elements must be NUL-terminated (strings) so strlen() works */
  for(lmn=0L;lmn<lmn_nbr;lmn++) sng_sz+=(sng_lst[lmn] == NULL) ? 0L : strlen(sng_lst[lmn])+dlm_len;
  /* Add one for NUL byte */
  sng=(char *)nco_malloc(sizeof(char)*(sng_sz+1));
  /* NUL-terminate string for safety */
  sng[0]='\0';
  for(lmn=0L;lmn<lmn_nbr;lmn++){
    /* List elements must be NUL-terminated (strings) so strcat() works */
    sng=(sng_lst[lmn] == NULL) ? sng : strcat(sng,sng_lst[lmn]);
    if(lmn != lmn_nbr-1 && dlm_len != 0) sng=strcat(sng,dlm_sng);
  } /* end loop over lmn */

  return sng;
} /* end sng_lst_prs() */

char ** /* O [sng] Array of list elements */
lst_prs /* [fnc] Create list of strings from given string and arbitrary delimiter */
(char * const sng_in, /* I/O [sng] Delimited argument list (delimiters are changed to NULL on output */
 const char * const dlm_sng, /* I [sng] delimiter string */
 int * const nbr_lst) /* O [nbr] number of elements in list */
{
  /* Purpose: Create list of strings from given string and arbitrary delimiter
     Routine is often called with system memory, e.g., with strings from
     command line arguments whose memory was allocated by shell or by getopt().
     A conservative policy would be, therefore, to never modify input string
     However, we are safe if any modifications do not extend input string
     Thus this routine is allowed to replace delimiter strings by NULs */

  /* Number of list members is always one more than number of delimiters, e.g.,
     foo,,3, has 4 arguments: "foo", "", "3" and "".
     A delimiter without an argument is valid syntax to indicate default argument
     Therefore a storage convention is necessary to indicate default argument was selected
     Either NULL or '\0' can be used without requiring additional flag
     NULL is not printable, but is useful as a logical flag since its value is False
     On the other hand, '\0', the empty string, can be printed but is not as useful as a flag
     Currently, NCO implements the former convention, where default selections are set to NULL
   */
    
  char **lst;
  char *sng_in_ptr;

  int dlm_len;
  int idx;

  /* Delimiter must be NUL-terminated (a string) so we may find its length */
  dlm_len=strlen(dlm_sng); 

  /* Do not increment actual sng_in pointer while searching for delimiters---increment a dummy pointer instead. */
  sng_in_ptr=sng_in; 

  /* First element does not require delimiter in front of it */
  *nbr_lst=1;

  /* Count list members */
  while((sng_in_ptr=strstr(sng_in_ptr,dlm_sng))){
    sng_in_ptr+=dlm_len;
    (*nbr_lst)++;
  } /* end while */

  lst=(char **)nco_malloc(*nbr_lst*sizeof(char *));

  sng_in_ptr=sng_in; 
  lst[0]=sng_in;
  idx=0;
  while((sng_in_ptr=strstr(sng_in_ptr,dlm_sng))){
    /* NUL-terminate previous arg */
    *sng_in_ptr='\0';
    sng_in_ptr+=dlm_len;
    lst[++idx]=sng_in_ptr;
  } /* end while */

  /* Default list member is assumed when two delimiters are adjacent to eachother, 
     i.e., when length of string between delimiters is 0. 
     If list ends with delimiter, then last element of list is also assumed to be default list member. */
  /* This loop sets default list members to NULL */
  for(idx=0;idx<*nbr_lst;idx++)
    if(strlen(lst[idx]) == 0) lst[idx]=NULL;

  if(dbg_lvl_get() == 5){
    (void)fprintf(stderr,"%d elements in list delimited by \"%s\"\n",*nbr_lst,dlm_sng);
    for(idx=0;idx<*nbr_lst;idx++) 
      (void)fprintf(stderr,"lst[%d] = %s\n",idx,(lst[idx] == NULL) ? "NULL" : lst[idx]);
    (void)fprintf(stderr,"\n");
    (void)fflush(stderr);
  } /* end debug */

  return lst;
} /* end lst_prs() */

void 
indexx /* [fnc] Sort array of integers */
(const int n, /* I [nbr] Number of elements */
 const int * const arrin, /* I [idx] Array to sort */
 int * const indx) /* O [idx] Indices to sorted array */
{
  /* Purpose: Sort array of integers
     Based on indexx() from Numerical Recipes
     Routine computes an index table which sorts input array into ascending order
     I made arrin argument and local variable q integers for netCDF purposes
     Routine assumes "one-based" arrays */
  int l,j,ir,indxt,i;
/*  float q;*/
  int q;
  
  for (j=1;j<=n;j++) indx[j]=j;
  l=(n >> 1) + 1;
  ir=n;
  for (;;) {
    if (l > 1)
      q=arrin[(indxt=indx[--l])];
    else {
      q=arrin[(indxt=indx[ir])];
      indx[ir]=indx[1];
      if (--ir == 1) {
	indx[1]=indxt;
	return;
      }
    }
    i=l;
    j=l << 1;
    while (j <= ir) {
      if (j < ir && arrin[indx[j]] < arrin[indx[j+1]]) j++;
      if (q < arrin[indx[j]]) {
	indx[i]=indx[j];
	j += (i=j);
      }
      else j=ir+1;
    }
    indx[i]=indxt;
  }
} /* end indexx() */

void 
index_alpha /* [fnc] Sort array of strings */
(const int n, /* I [nbr] Number of elements */
 char * const * const arrin, /* I [sng] Strings to sort */
 int * const indx) /* O [idx] Indices to sorted array */
{
/* Purpose: Sort input array alphanumerically
   This is indexx() from Numerical recipes hacked to alphabetize a list of strings */
  int l,j,ir,indxt,i;
/*  float q;*/
  char *q;
  
  for (j=1;j<=n;j++) indx[j]=j;
  l=(n >> 1) + 1;
  ir=n;
  for (;;) {
    if (l > 1)
      q=arrin[(indxt=indx[--l])];
    else {
      q=arrin[(indxt=indx[ir])];
      indx[ir]=indx[1];
      if (--ir == 1) {
	indx[1]=indxt;
	return;
      }
    }
    i=l;
    j=l << 1;
    while (j <= ir) {
      /*      if (j < ir && arrin[indx[j]] < arrin[indx[j+1]]) j++;*/
      if (j < ir && strcmp(arrin[indx[j]],arrin[indx[j+1]]) < 0) j++;
      /*      if (q < arrin[indx[j]]) {*/
      if (strcmp(q,arrin[indx[j]]) < 0) {
	indx[i]=indx[j];
	j += (i=j);
      }
      else j=ir+1;
    }
    indx[i]=indxt;
  }
} /* end index_alpha() */

nm_id_sct * /* O [sct] Sorted output list */
lst_heapsort /* [fnc] Heapsort input lists numerically or alphabetically */
(nm_id_sct *lst, /* I/O [sct] Current list (destroyed) */
 const int nbr_lst, /* I [nbr] number of members in list */
 const bool ALPHABETIZE_OUTPUT) /* I [flg] Alphabetize extraction list */
{
  /* Purpose: Sort extraction lists numerically or alphabetically */
  int *srt_idx; /* List to store sorted key map */
  int idx; /* Counting index */
  nm_id_sct *lst_tmp; /* Temporary copy of original extraction list */
  
  srt_idx=(int *)nco_malloc(nbr_lst*sizeof(int));
  lst_tmp=(nm_id_sct *)nco_malloc(nbr_lst*sizeof(nm_id_sct));
  (void)memcpy((void *)lst_tmp,(void *)lst,nbr_lst*sizeof(nm_id_sct));
  
  /* indexx() and relatives assume "one-based" arrays 
     Use pointer arithmetic to spoof zero-based arrays, i.e.,
     xtr_nm[0] in calling routine becomes xtr_nm[1] in sorting routine  */
  if(ALPHABETIZE_OUTPUT){
    /* Alphabetize list by variable name
       This produces easy-to-read screen output with ncks */
    char **xtr_nm;
    xtr_nm=(char **)nco_malloc(nbr_lst*sizeof(char *));
    for(idx=0;idx<nbr_lst;idx++) xtr_nm[idx]=lst[idx].nm;
    /* fxm: Replace with system qsort() */
    (void)index_alpha(nbr_lst,xtr_nm-1,srt_idx-1);
    xtr_nm=(char **)nco_free(xtr_nm);
  }else{
    /* Heapsort list by variable ID 
       This theoretically allows fastest I/O when creating output file */
    int *xtr_id;
    xtr_id=(int *)nco_malloc(nbr_lst*sizeof(int));
    for(idx=0;idx<nbr_lst;idx++) xtr_id[idx]=lst[idx].id;
    /* fxm: Replace with system qsort() */
    (void)indexx(nbr_lst,xtr_id-1,srt_idx-1);
    xtr_id=(int *)nco_free(xtr_id);
  } /* end else */

  /* indexx and relatives employ "one-based" arrays 
     Thus min(srt_idx) == 1 and max(srt_idx) == nbr_lst */
  for(idx=0;idx<nbr_lst;idx++){
    lst[idx].id=lst_tmp[srt_idx[idx]-1].id;
    lst[idx].nm=lst_tmp[srt_idx[idx]-1].nm;
  } /* end loop over idx */
  lst_tmp=(nm_id_sct *)nco_free(lst_tmp);
  srt_idx=(int *)nco_free(srt_idx);
  
  return lst;
  
} /* end lst_heapsort() */

