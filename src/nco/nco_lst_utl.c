/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_lst_utl.c,v 1.60 2011-08-03 04:35:55 zender Exp $ */

/* Purpose: List utilities */

/* Copyright (C) 1995--2011 Charlie Zender
   License: GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */

#include "nco_lst_utl.h" /* List utilities */

/* NB: Free (speech) sorting routines by Joerg Shoen available from
   http://www.pci.uni-heidelberg.de/tc/usr/joerg
   are faster than ANSI system qsort() in all cases 
   See code in ${DATA}/tmp/testsort */

/* Compile following routines only if regular expression library is present */
#ifdef NCO_HAVE_REGEX_FUNCTIONALITY

int /* O [nbr] Number of matches found */
nco_lst_meta_search /* [fnc] Search for pattern matches in var string list */
(int var_nbr_all, /* I [nbr] Size of var_lst_all and var_xtr_rqs */
 nm_id_sct *var_lst_all, /* I [sct] All variables in input file (with IDs) */
 char *rx_sng, /* I [sng] Regular expression pattern */
 nco_bool *var_xtr_rqs) /* O [flg] Matched variable holder */
{
  /* Purpose: Match regular expressions to members of list */

  int idx;
  int err_id;
  int flg_cmp; /* Comparison flags */
  int flg_exe; /* Execution flages */
  int mch_nbr=0;
  size_t nbr_sub_xpr;
  
  regmatch_t *result;
  regex_t *rx;

  rx=(regex_t *)nco_malloc(sizeof(regex_t));

  /* Choose RE_SYNTAX_POSIX_EXTENDED regular expression type */
  flg_cmp=(REG_EXTENDED | REG_NEWLINE);
  /* Set execution flags */
  flg_exe=0;

  /* Compile regular expression */
  if((err_id=regcomp(rx,rx_sng,flg_cmp)) != 0){ /* Compile regular expression */
    char const * rx_err_sng;  
    /* POSIX regcomp return error codes */
    switch(err_id){
    case REG_BADPAT: rx_err_sng="Invalid pattern."; break;  
    case REG_ECOLLATE: rx_err_sng="Not implemented."; break;
    case REG_ECTYPE: rx_err_sng="Invalid character class name."; break;
    case REG_EESCAPE: rx_err_sng="Trailing backslash."; break;
    case REG_ESUBREG: rx_err_sng="Invalid back reference."; break;
    case REG_EBRACK: rx_err_sng="Unmatched left bracket."; break;
    case REG_EPAREN: rx_err_sng="Parenthesis imbalance."; break;
    case REG_EBRACE: rx_err_sng="Unmatched {."; break;
    case REG_BADBR: rx_err_sng="Invalid contents of { }."; break;
    case REG_ERANGE: rx_err_sng="Invalid range end."; break;
    case REG_ESPACE: rx_err_sng="Ran out of memory."; break;
    case REG_BADRPT: rx_err_sng="No preceding re for repetition op"; break;
    default: rx_err_sng="Invalid pattern."; break;  
    } /* end switch */
    (void)fprintf(stdout,"%s: ERROR nco_lst_meta_search() error in regular expression \"%s\" %s \n",prg_nm_get(),rx_sng,rx_err_sng); 
    nco_exit(EXIT_FAILURE);
  } /* end if err */

  nbr_sub_xpr=rx->re_nsub+1; /* How many matches are there in line? */

  /* Search string */
  result=(regmatch_t *)nco_malloc(sizeof(regmatch_t)*nbr_sub_xpr);

  /* Search each variable string for matches */
  for(idx=0;idx<var_nbr_all;idx++){  
    if(!regexec(rx,var_lst_all[idx].nm,nbr_sub_xpr,result,flg_exe)){
      var_xtr_rqs[idx]=True;
      mch_nbr++;
    } /* end if */
  } /* end loop over variables */

  regfree(rx); /* Free regular expression data structure */
  rx=(regex_t *)nco_free(rx);
  result=(regmatch_t *)nco_free(result);

  return mch_nbr;
} /* end nco_lst_meta_search() */

#endif /* !NCO_HAVE_REGEX_FUNCTIONALITY */

void 
nco_srt_ntg /* [fnc] Sort array of integers */
(const int lmn_nbr, /* I [nbr] Number of elements */
 const int * const arr_in, /* I [idx] Array to sort */
 int * const idx) /* O [idx] Indices to sorted array */
{
  /* Purpose: Stub for Numerical Recipes-compatible indexx() routine */
  long foo=sizeof(lmn_nbr)+sizeof(arr_in)+sizeof(idx); /* CEWI */
  foo++; /* CEWI */
  (void)fprintf(stdout,"%s: ERROR nco_srt_ntg() routine should not be called\n",prg_nm_get());
  nco_exit(EXIT_FAILURE);
} /* end nco_srt_ntg() */

void 
nco_srt_lph /* [fnc] Sort array of strings */
(const int lmn_nbr, /* I [nbr] Number of elements */
 char * const * const arr_in, /* I [sng] Strings to sort */
 int * const idx) /* O [idx] Indices to sorted array */
{
  /* Purpose: Stub for Numerical Recipes-compatible indexx_alpha() routine */
  long foo=sizeof(lmn_nbr)+sizeof(arr_in)+sizeof(idx); /* CEWI */
  foo++; /* CEWI */
  (void)fprintf(stdout,"%s: ERROR nco_srt_lph() routine should not be called\n",prg_nm_get());
  nco_exit(EXIT_FAILURE);
} /* end nco_srt_lph() */

nm_id_sct * /* O [sct] Sorted output list */
lst_heapsort /* [fnc] Heapsort input lists numerically or alphabetically */
(nm_id_sct *lst, /* I/O [sct] Current list (destroyed) */
 const int nbr_lst, /* I [nbr] Number of members in list */
 const nco_bool ALPHABETIZE_OUTPUT) /* I [flg] Alphabetize extraction list */
{
  /* Purpose: Sort extraction lists numerically or alphabetically
     Routine is deprecated in favor of nco_lst_srt_nm_id() which uses system qsort()
     lst_heapsort drives Numerical Recipes indexx-style routines

     Advantage of indexx* routines is they return list of sorted indices,
     allowing original list to be untouched and sorted indices to be used.
     Disadvantage of indexx* routines is they employ 1-based indexing,
     and are non-free.

     This driver routine IS free, and maintained for future reference
     Main purpose of routine is to handle bookkeeping of copying structure
     elements to be sorted and rearranging original list on basis of sorted indices */

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
    /* Replace with free (speech) index_alpha() replacement */
    /*(void)index_alpha(nbr_lst,xtr_nm-1,srt_idx-1);*/
    xtr_nm=(char **)nco_free(xtr_nm);
  }else{
    /* Heapsort list by variable ID 
       This theoretically allows fastest I/O when creating output file */
    int *xtr_id;
    xtr_id=(int *)nco_malloc(nbr_lst*sizeof(int));
    for(idx=0;idx<nbr_lst;idx++) xtr_id[idx]=lst[idx].id;
    /* Replace with free (speech) indexx() replacement */
    /* (void)indexx(nbr_lst,xtr_id-1,srt_idx-1);*/
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

char ** /* O [sng] Array of list elements */
nco_lst_prs_1D /* [fnc] Create 1D array of strings from given string and delimiter */
(char * const sng_in, /* I/O [sng] Delimited argument list (delimiters are changed to NULL on output */
 const char * const dlm_sng, /* I [sng] Delimiter string */
 int * const nbr_lst) /* O [nbr] Number of elements in list */
{
  /* Purpose: Create list of strings from given string and arbitrary delimiter
     Routine is often called with system memory, e.g., with strings from
     command line arguments whose memory was allocated by shell or by getopt().
     Conservative policy would be, therefore, to never modify input string
     However, we are safe if any modifications do not extend input string
     Thus this routine is allowed to replace delimiter strings by NULs

     NB: Function takes single string as input and returns "list of strings" 
     However, this list of strings was not obtained by malloc'ing each string 
     It was obtained by inserting delimiters in a single string 
     Hence do not try to separately free() each member of list of strings

     Contrasting nco_lst_prs_1D() to successor nco_lst_prs_2D():
     nco_lst_prs_2D() creates two-dimensional output string list, i.e., list of pointers to separately malloc()'d buffers
     nco_lst_prs_2D() does not modify input
     nco_lst_prs_2D() output list should be free'd by nco_sng_lst_free()
     nco_lst_prs_1D() creates one-dimensional string list by inserting NULs into single buffer
     nco_lst_prs_1D() modifies input (by inserting NULs)
     nco_lst_prs_1D() output list may be free'd by nco_free() */

  /* Number of list members is always one more than number of delimiters, e.g.,
     foo,,3, has 4 arguments: "foo", "", "3" and "".
     A delimiter without an argument is valid syntax to indicate default argument
     Therefore a storage convention is necessary to indicate default argument was selected
     Either NULL or '\0' can be used without requiring additional flag
     NULL is not printable, but is useful as a logical flag since its value is False
     On the other hand, '\0', the empty string, can be printed but is not as useful as a flag
     Currently, NCO implements former convention, where default selections are set to NULL */
    
  char **lst; /* O [sng] Array of list elements */
  char *sng_in_ptr;

  int dlm_lng;
  int idx;

  /* Delimiter must be NUL-terminated (a string) so we may find its length */
  dlm_lng=strlen(dlm_sng); 

  /* Increment dummy pointer instead of actual sng_in pointer while searching for delimiters */
  sng_in_ptr=sng_in; 

  /* First element does not require preceding delimiter */
  *nbr_lst=1;

  /* Count list members */
  while((sng_in_ptr=strstr(sng_in_ptr,dlm_sng))){
    sng_in_ptr+=dlm_lng;
    (*nbr_lst)++;
  } /* end while */

  lst=(char **)nco_malloc(*nbr_lst*sizeof(char *));

  sng_in_ptr=sng_in; 
  lst[0]=sng_in;
  idx=0;
  while((sng_in_ptr=strstr(sng_in_ptr,dlm_sng))){
    /* NUL-terminate previous arg */
    *sng_in_ptr='\0';
    sng_in_ptr+=dlm_lng;
    lst[++idx]=sng_in_ptr;
  } /* end while */

  /* Default list member is assumed when two delimiters are adjacent to eachother, 
     i.e., when length of string between delimiters is 0. 
     If list ends with delimiter, last element of list is also assumed to be default list member */
  /* This loop sets default list members to NULL */
  for(idx=0;idx<*nbr_lst;idx++)
    if(strlen(lst[idx]) == 0) lst[idx]=NULL;

  if(dbg_lvl_get() == 5){
    (void)fprintf(stderr,"nco_lst_prs_1d() reports %d elements in list delimited by \"%s\"\n",*nbr_lst,dlm_sng);
    for(idx=0;idx<*nbr_lst;idx++) 
      (void)fprintf(stderr,"lst[%d] = %s\n",idx,(lst[idx] == NULL) ? "NULL" : lst[idx]);
    (void)fprintf(stderr,"\n");
    (void)fflush(stderr);
  } /* end debug */

  return lst;
} /* end nco_lst_prs_1D() */

char ** /* O [sng] List of strings */
nco_lst_prs_2D /* [fnc] Create list of strings from given string and delimiter */
(const char * const sng_in, /* I [sng] Delimited argument list */
 const char * const dlm_sng, /* I [sng] Delimiter string */
 int * const nbr_lst) /* O [nbr] Number of elements in list */
{
  /* Purpose: Create list of strings from given string and arbitrary delimiter
     Algorithm recursively copies all text up to delimiter into new string and
     then appends new string to output list
     Output list has no delimiter strings
     Contrasting nco_lst_prs_2D() to predecessor nco_lst_prs_1D():
     nco_lst_prs_2D() creates two-dimensional output string list, i.e., list of pointers to separately malloc()'d buffers
     nco_lst_prs_2D() does not modify input
     nco_lst_prs_2D() output list should be free'd by nco_sng_lst_free()
     nco_lst_prs_1D() creates one-dimensional string list by inserting NULs into single buffer
     nco_lst_prs_1D() modifies input (by inserting NULs)
     nco_lst_prs_1D() output list may be free'd by nco_free() */

  /* Number of list members is always one more than number of delimiters, e.g.,
     foo,,3, has 4 arguments: "foo", "", "3" and "".
     A delimiter without an argument is valid syntax to indicate default argument
     Therefore a storage convention is necessary to indicate default argument was selected
     Either NULL or '\0' can be used without requiring additional flag
     NULL is not printable, but is useful as a logical flag since its value is False
     On the other hand, '\0', the empty string, can be printed but is not as useful as a flag
     Currently, NCO implements former convention, where default selections are set to NULL */
    
  char **sng_lst_out; /* O [sng] Array of list elements */
  char *sng_in_cpy;
  char *dlm_ptr_crr;
  char *sng_out_srt;

  int dlm_lng;
  int idx;

  /* Delimiter must be NUL-terminated (a string) so we may find its length */
  dlm_lng=strlen(dlm_sng);

  /* Create duplicate to search, modify, copy, and free */
  sng_in_cpy=(char *)strdup(sng_in); 

  /* Increment temporary dummy pointer dlm_ptr_crr in strstr() search loops */
  dlm_ptr_crr=sng_in_cpy;

  /* First element does not require preceding delimiter */
  *nbr_lst=1;

  /* Count list members */
  while((dlm_ptr_crr=strstr(dlm_ptr_crr,dlm_sng))){
    dlm_ptr_crr+=dlm_lng;
    (*nbr_lst)++;
  } /* end while */

  /* Calling routine has responsibility to free this memory */
  sng_lst_out=(char **)nco_malloc(*nbr_lst*sizeof(char *));

  dlm_ptr_crr=sng_in_cpy; 
  sng_out_srt=sng_in_cpy;
  idx=0;
  while((dlm_ptr_crr=strstr(sng_out_srt,dlm_sng))){
    /* This loop brackets and grabs "previous" arguments
       Grab final argument after loop */
    /* NUL-terminate previous arg */
    *dlm_ptr_crr='\0';
    /* Calling routine has responsibility to free this memory */
    sng_lst_out[idx++]=(char *)strdup(sng_out_srt);
    sng_out_srt=dlm_ptr_crr+dlm_lng;
  } /* end while */
  /* Grab final argument after last delimiter
     This also handles case of string with no delimiters */
  sng_lst_out[idx++]=(char *)strdup(sng_out_srt);

  /* Assume default list member when two delimiters are adjacent to eachother, 
     i.e., when length of string between delimiters is 0. 
     If list ends with delimiter, last element of list is also assumed to be default list member */
  /* This loop sets default list members to NULL */
  for(idx=0;idx<*nbr_lst;idx++)
    if(strlen(sng_lst_out[idx]) == 0) sng_lst_out[idx]=NULL;

  if(dbg_lvl_get() == 5){
    (void)fprintf(stderr,"nco_lst_prs_2D() reports %d elements in list delimited by \"%s\"\n",*nbr_lst,dlm_sng);
    for(idx=0;idx<*nbr_lst;idx++) 
      (void)fprintf(stderr,"sng_lst_out[%d] = %s\n",idx,(sng_lst_out[idx] == NULL) ? "NULL" : sng_lst_out[idx]);
    (void)fprintf(stderr,"\n");
    (void)fflush(stderr);
  } /* end debug */

  /* Free duplicate of sng_in */
  sng_in_cpy=(char *)nco_free(sng_in_cpy); 

  return sng_lst_out;
} /* end nco_lst_prs_2D() */


char ** /* O [sng] List of strings */
nco_lst_prs_sgl_2D /* [fnc] Create list of strings from given string and delimiter */
(const char * const sng_in, /* I [sng] Delimited argument list */
 const char * const dlm_sng, /* I [sng] Delimiter string */
 int * const nbr_lst) /* O [nbr] Number of elements in list */
{
  /* Similar to nco_lst_prs_2D() except for following            */
  /* Only first char of dlm_sng is used and                      */
  /* empty substrings are ignored and NOT returned as nulls      */
  /* eg with dlm_sng=' ' and sng_in=" one two  three    five   " */
  /* the following is returned "one"/"two"/"three"/"five"        */
              
                                    
  int sng_len;
  int nbr_sng;
  char ch_dlm;
  char *cp_sng;
  char *cp_end;
  char *cp_ptr;
  char **lst=NULL;;

  sng_len=strlen(sng_in);
  
  if(sng_len==0){
    *nbr_lst=0;
    return lst;
  }
    
  /* use copy of string */
  cp_sng=strdup(sng_in);

  /* use only first char of delimiter string */
  ch_dlm=dlm_sng[0];

  cp_end=cp_sng+sng_len;
  cp_ptr=cp_sng;


  /* replace delimiters with nulls */
  while( cp_ptr<cp_end){
    if(*cp_ptr==ch_dlm)
      *cp_ptr='\0';
    cp_ptr++; 
  }

  nbr_sng=0;
  cp_ptr=cp_sng;  
  
  /* loop thru string finding null delimited substrings */
  while(cp_ptr < cp_end){
    sng_len=strlen(cp_ptr);
    if(sng_len >0 ){ 
      lst=(char**)nco_realloc(lst, ++nbr_sng*sizeof(char*));
      lst[nbr_sng-1]=strdup(cp_ptr); 
      cp_ptr+=(sng_len+1);
    }else
      cp_ptr++;
  }
   
  (void)nco_free(cp_sng);

  *nbr_lst=nbr_sng;

  return lst;

} /* end nco_lst_prs_sgl_2D() */






int /* O [enm] Comparison result [<,=,>] 0 iff val_1 [<,==,>] val_2 */
nco_cmp_chr /* [fnc] Compare two characters */
(const void * val_1, /* I [chr] Character to compare */
 const void * val_2) /* I [chr] Character to compare */
{
  /* Purpose: Compare two characters
     Function is suitable for argument to ANSI C qsort() routine in stdlib.h
     Code based on responses to my comp.lang.c thread 20040101 */
  const char * const val_1_cp=(const char *)val_1;
  const char * const val_2_cp=(const char *)val_2;
  return *val_1_cp < *val_2_cp ? -1 : (*val_1_cp > *val_2_cp);
  /* Alternative one-liner:
     return (*val_1_cp > *val_2_cp) - (*val_1_cp < *val_2_cp); */
} /* end nco_cmp_chr() */

int /* O [enm] Comparison result [<,=,>] 0 iff val_1 [<,==,>] val_2 */
nco_cmp_int /* [fnc] Compare two integers */
(const void *val_1, /* I [nbr] Number to compare */
 const void *val_2) /* I [nbr] Number to compare */
{
  /* Purpose: Compare two integers
     Function is suitable for argument to ANSI C qsort() routine in stdlib.h
     Code based on responses to my comp.lang.c thread 20040101 */
  const int * const val_1_ip=(const int *)val_1;
  const int * const val_2_ip=(const int *)val_2;
  return *val_1_ip < *val_2_ip ? -1 : (*val_1_ip > *val_2_ip);
  /* Alternative one-liner:
     return (*val_1_ip > *val_2_ip) - (*val_1_ip < *val_2_ip); */
} /* end nco_cmp_int() */

int /* O [enm] Comparison result [<,=,>] 0 iff val_1 [<,==,>] val_2 */
nco_cmp_sng /* [fnc] Compare two strings */
(const void *val_1, /* I [sng] String to compare */
 const void *val_2) /* I [sng] String to compare */
{
  /* Purpose: Compare two strings
     Function is suitable for argument to ANSI C qsort() routine in stdlib.h
     http://www.eskimo.com/~scs/C-faq/q13.8.html describes sorting strings: 
     Arguments to qsort()'s comparison function are pointers to objects being sorted,
     i.e., pointers to pointers to chars 
     strcmp(), however, accepts simple pointers to char
     Therefore, strcmp() cannot be used directly as comparison function for qsort()
     This wrapper casts input values to simple char pointers, calls strcmp(), and feeds results back to qsort() */
  return strcmp(*(char * const *)val_1,*(char * const *)val_2);
} /* end nco_cmp_sng() */

int /* O [enm] Comparison result [<,=,>] 0 iff val_1 [<,==,>] val_2 */
nco_cmp_nm_id_nm /* [fnc] Compare two nm_id_sct's by name member */
(const void *val_1, /* I [sct] nm_id_sct to compare */
 const void *val_2) /* I [sct] nm_id_sct to compare */
{
  /* Purpose: Compare two nm_id_sct's by name structure member */
  return strcmp((*(nm_id_sct const *)val_1).nm,(*(nm_id_sct const *)val_2).nm);
} /* end nco_cmp_nm_id_nm() */

int /* O [enm] Comparison result [<,=,>] 0 iff val_1 [<,==,>] val_2 */
nco_cmp_nm_id_id /* [fnc] Compare two nm_id_sct's by ID member */
(const void *val_1, /* I [sct] nm_id_sct to compare */
 const void *val_2) /* I [sct] nm_id_sct to compare */
{
  /* Purpose: Compare two nm_id_sct's by ID structure member
     NB: This function uses a method which is, in general, unsafe
     By performing casts and then subracting, we are subject to overflow
     conditions should integer values be close to INT_MAX or INT_MIN.
     However, we know that nm_id_sct.id values are always small
     Thus we use this slightly unsafe method in order to show that
     comparison function may be written (albeit unsafely) in one line. */
  return (*(nm_id_sct const *)val_1).id-(*(nm_id_sct const *)val_2).id;
} /* end nco_cmp_nm_id_nm() */

int /* O [enm]  Comparison result [<,=,>] 0 iff op1 [<,==,>] op2 */
nco_cmp_ptr_unn /* Compare values of two pointer unions of same type */
(const nc_type type, /* I [enm] netCDF type of operands */
 const ptr_unn op1, /* I [sct] First operand to compare */
 const ptr_unn op2) /* I [sct] Second operand to compare */
{
  /* Purpose: Compare values of two scalar pointer unions of same type 
     Function is almost suitable for argument to ANSI C qsort() routine in stdlib.h
     Like strcmp(), this routine returns <,=,> zero iff op1 <,=,> op2
     Routine based on nco_cmp_int()
     Note that only first value of pointer unions is compared */
  switch(type){
  case NC_FLOAT: 
    {const float * const op1_fp=op1.fp;
    const float * const op2_fp=op2.fp;
    return *op1_fp < *op2_fp ? -1 : (*op1_fp > *op2_fp);}
    break;
  case NC_DOUBLE:
    {const double * const op1_dp=op1.dp;
    const double * const op2_dp=op2.dp;
    return *op1_dp < *op2_dp ? -1 : (*op1_dp > *op2_dp);}
    break;
  case NC_INT:
    {const nco_int * const op1_lp=op1.ip;
    const nco_int * const op2_lp=op2.ip;
    return *op1_lp < *op2_lp ? -1 : (*op1_lp > *op2_lp);}
    break;
  case NC_SHORT:
    {const short * const op1_sp=op1.sp;
    const short * const op2_sp=op2.sp;
    return *op1_sp < *op2_sp ? -1 : (*op1_sp > *op2_sp);}
    break;
  case NC_USHORT:
    {const nco_ushort * const op1_usp=op1.usp;
    const nco_ushort * const op2_usp=op2.usp;
    return *op1_usp < *op2_usp ? -1 : (*op1_usp > *op2_usp);}
    break;
  case NC_UINT:
    {const nco_uint * const op1_uip=op1.uip;
    const nco_uint * const op2_uip=op2.uip;
    return *op1_uip < *op2_uip ? -1 : (*op1_uip > *op2_uip);}
    break;
  case NC_INT64:
    {const nco_int64 * const op1_i64p=op1.i64p;
    const nco_int64 * const op2_i64p=op2.i64p;
    return *op1_i64p < *op2_i64p ? -1 : (*op1_i64p > *op2_i64p);}
    break;
  case NC_UINT64:
    {const nco_uint64 * const op1_ui64p=op1.ui64p;
    const nco_uint64 * const op2_ui64p=op2.ui64p;
    return *op1_ui64p < *op2_ui64p ? -1 : (*op1_ui64p > *op2_ui64p);}
    break;
  case NC_BYTE:
    {const nco_byte * const op1_bp=op1.bp;
    const nco_byte * const op2_bp=op2.bp;
    return *op1_bp < *op2_bp ? -1 : (*op1_bp > *op2_bp);}
    break;
  case NC_UBYTE:
    {const nco_ubyte * const op1_ubp=op1.ubp;
    const nco_ubyte * const op2_ubp=op2.ubp;
    return *op1_ubp < *op2_ubp ? -1 : (*op1_ubp > *op2_ubp);}
    break;
  case NC_CHAR:
    {const nco_char * const op1_cp=op1.cp;
    const nco_char * const op2_cp=op2.cp;
    return *op1_cp < *op2_cp ? -1 : (*op1_cp > *op2_cp);}
    break;
  case NC_STRING:
    {const nco_string * const op1_sngp=op1.sngp;
    const nco_string * const op2_sngp=op2.sngp;
    return *op1_sngp < *op2_sngp ? -1 : (*op1_sngp > *op2_sngp);}
    break;
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */

  /* Some compilers, e.g., SGI cc, need return statement to end non-void functions */
  return 0;

} /* end nco_cmp_ptr_unn() */

void 
nco_lst_comma2hash /* [fnc] Replace commas with hashes when within braces */
(char * const rx_sng) /* [sng] Regular expression */
{
  /* Purpose: Convert commas within braces to hashes within braces in regular expressions
     Required for handling corner cases in wildcarding regular expressions
     NB: Usually this code operates on system memory (e.g., optarg) so be very careful 
     not to overwrite ends of strings */
  char *cp;
  char *cp_cnv=NULL; /* [ptr] Location of comma following open brace */
  nco_bool openbrace=False; /* [flg] Open brace has been found */
  cp=rx_sng;
  /* Loop over each character in string until first NUL encountered */
  while(*cp){
    /* Find open brace */
    if(*cp=='{') openbrace=True;
    if(openbrace && *cp==',') cp_cnv=cp;
    /* Find close brace */
    if(*cp=='}'){ 
      /* Change comma following open brace, if any, to hash */
      if(cp_cnv) *cp_cnv='#';
      /* Reset comma location following open brace */
      openbrace=False;
      /* Reset indicator and location of comma following open brace */
      cp_cnv=NULL;
    } /* endif */
    /* Increment position in regular expression */
    cp++;
  } /* end while character is not NUL */
} /* end nco_lst_comma2hash() */

nm_id_sct * /* O [sct] Sorted output list */
nco_lst_srt_nm_id /* [fnc] Sort name/ID input list numerically or alphabetically */
(nm_id_sct * const lst, /* I/O [sct] Current list (destroyed) */
 const int nbr_lst, /* I [nbr] number of members in list */
 const nco_bool ALPHABETIZE_OUTPUT) /* I [flg] Alphabetize extraction list */
{
  /* Purpose: Sort extraction lists numerically or alphabetically */
  if(ALPHABETIZE_OUTPUT){
    /* Alphabetize list by variable name
       This produces easy-to-read screen output with ncks */
    qsort(lst,(size_t)nbr_lst,sizeof(lst[0]),nco_cmp_nm_id_nm);
  }else{
    /* Heapsort list by variable ID 
       This theoretically allows fastest I/O when creating output file */
    qsort(lst,(size_t)nbr_lst,sizeof(lst[0]),nco_cmp_nm_id_id);
  } /* end else */
  return lst;
} /* end nco_lst_srt_nm_id() */

nm_id_sct * /* O [sct] Pointer to free'd structure list */
nco_nm_id_lst_free /* [fnc] Free memory associated with name-ID structure list */
(nm_id_sct *nm_id_lst, /* I/O [sct] Name-ID structure list to free */
 const int nm_id_nbr) /* I [nbr] Number of name-ID structures in list */
{
  /* Threads: Routine is thread safe and calls no unsafe routines */
  /* Purpose: Free all memory associated with dynamically allocated name-ID structure list */
  int idx;

  for(idx=0;idx<nm_id_nbr;idx++){
    if(nm_id_lst[idx].nm) nm_id_lst[idx].nm=(char *)nco_free(nm_id_lst[idx].nm);
    /* fxm: not safe to free new nm_id_sct members until code consistently initializes them 
       this will cause a temporary memory leak until initialization is consistent */
    /*    if(nm_id_lst[idx].var_nm_fll) nm_id_lst[idx].var_nm_fll=(char *)nco_free(nm_id_lst[idx].var_nm_fll);*/
    /*    if(nm_id_lst[idx].grp_nm) nm_id_lst[idx].grp_nm=(char *)nco_free(nm_id_lst[idx].grp_nm);*/
    /*    if(nm_id_lst[idx].grp_nm_fll) nm_id_lst[idx].grp_nm_fll=(char *)nco_free(nm_id_lst[idx].grp_nm_fll);*/
  } /* end loop over idx */

  /* Free structure pointer last */
  nm_id_lst=(nm_id_sct *)nco_free(nm_id_lst);

  return nm_id_lst;
} /* end nco_nm_id_lst_free() */

char * /* O [sng] Concatenated string formed by joining all input strings */
sng_lst_cat /* [fnc] Join string list together into one string, delete originals */
(char ** const sng_lst, /* I/O [sng] List of pointers to strings to join together */
 const long lmn_nbr, /* I [nbr] Number of strings in list */
 const char * const dlm_sng) /* I [sng] Delimiter string to use as glue */
{
  /* Purpose: Join list of strings together into one string
     Delete original string list
     Elements of input list should all be NUL-terminated strings
     Elements with value NUL are interpreted as strings of zero length */

  char *sng; /* Output string */

  int dlm_lng;
  long lmn;
  long sng_sz=0L; /* NB: sng_sz get incremented */

  if(lmn_nbr == 1L){
    sng=(char *)strdup(sng_lst[0]);
    goto cln_and_xit;
  } /* lmn_nbr != 1L */

  /* Delimiter must be NUL-terminated (a string) so strlen() works */
  if(dlm_sng == NULL){
    (void)fprintf(stdout,"%s: ERROR sng_lst_cat() reports delimiter string is NULL\n",prg_nm_get());
    nco_exit(EXIT_FAILURE);
  } /* end if */
  dlm_lng=strlen(dlm_sng); 

  /* List elements must be NUL-terminated (strings) so strlen() works */
  for(lmn=0L;lmn<lmn_nbr;lmn++) sng_sz+=(sng_lst[lmn] == NULL) ? 0L : strlen(sng_lst[lmn])+dlm_lng;
  /* Add one for NUL byte */
  sng=(char *)nco_malloc(sizeof(char)*(sng_sz+1L));
  /* NUL-terminate string for safety */
  sng[0]='\0';
  for(lmn=0L;lmn<lmn_nbr;lmn++){
    /* List elements must be NUL-terminated (strings) so strcat() works */
    sng=(sng_lst[lmn] == NULL) ? sng : strcat(sng,sng_lst[lmn]);
    if(lmn != lmn_nbr-1L && dlm_lng != 0) sng=strcat(sng,dlm_sng);
  } /* end loop over lmn */

  /* Jump here if only one string */
 cln_and_xit:
  for(lmn=0L;lmn<lmn_nbr;lmn++){
    if(sng_lst[lmn]) sng_lst[lmn]=(char *)nco_free(sng_lst[lmn]);
  } /* end loop over lmn */

  return sng;
} /* end sng_lst_cat() */

char ** /* O [sng] Pointer to free'd string list */
nco_sng_lst_free /* [fnc] Free memory associated with string list */
(char **sng_lst, /* I/O [sng] String list to free */
 const int sng_nbr) /* I [nbr] Number of strings in list */
{
  /* Threads: Routine is thread safe and calls no unsafe routines */
  /* Purpose: Free all memory associated with dynamically allocated string list */
  int idx;

  for(idx=0;idx<sng_nbr;idx++){
    sng_lst[idx]=(char *)nco_free(sng_lst[idx]);
  } /* end loop over idx */

  /* Free structure pointer last */
  sng_lst=(char **)nco_free(sng_lst);

  return sng_lst;
} /* end nco_sng_lst_free() */

