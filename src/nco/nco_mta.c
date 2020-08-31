/* $Header$ */

/* Purpose: Multi-argument utilities */

/* Copyright (C) 2016--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file
   
   Original Author: Jerome Mao, UCI */

#include "nco_mta.h" /* Multi-argument parsing */

const char *nco_mta_sub_dlm=","; /* [sng] Multi-argument sub-delimiter */

kvm_sct /* O [kvm_sct] key-value pair*/
nco_sng2kvm /* [fnc] Convert string to key-value pair */
(const char *args) /* I [sng] Input string argument with equal sign connecting key & value */
{
  /* Implementation: parsing args so they can be sent to kvm (fake kvm here) as key-value pair
   *
   * Example 1: ... --gaa a=1 ... should be exported as kvm.key = a; kvm.value = 1
   * Example 2: ... --gaa "a;b;c"=1 should be exported as kvm[0].key="a", kvm[1].key="b", kvm[2].key="c"
   *          and kvm[@] = 1 (the ";" will be parsed by caller). 
   *
   * IMPORTANT: free() fake_kvm after using string_to_kvm. */
  const char fnc_nm[]="nco_sng2kvm()"; /* [sng] Function name */
  char *args_copy=strdup(args);
  char *ptr_for_free=args_copy;
  kvm_sct kvm;

  if(!strstr(args_copy,"=")){
    kvm.key=strdup(args_copy);
    kvm.val=NULL;
    nco_free(ptr_for_free);
    return kvm;
  } /* !args_copy */
  
  kvm.key=strdup(strsep(&args_copy,"="));
  kvm.val=strdup(args_copy);
  
  nco_free(ptr_for_free);
  
  /* If nco_malloc() cannot allocate sufficient memory then key or value are NULL */
  if(!kvm.key || !kvm.val){
    (void)fprintf(stderr,"%s: ERROR %s reports system has insufficient memory\n",nco_prg_nm_get(),fnc_nm);
    nco_exit(EXIT_FAILURE);
  } /* kvm.key */
  return kvm;
  
} /* nco_sng2kvm() */

char * /* O [sng] Stripped-string */
nco_sng_strip /* [fnc] Strip leading and trailing white space */
(char *sng) /* I/O [sng] String to strip */
{
  /* fxm: does not not work for \n??? */
  char *srt=sng;
  while(isspace(*srt)) srt++;
  size_t end=strlen(srt);
  if(srt != sng){
    memmove(sng,srt,end);
    sng[end]='\0';
  } /* !srt */
  while(isblank(*(sng+end-1L))) end--;
  sng[end]='\0';
  return sng;
} /* end nco_sng_strip() */

kvm_sct * /* O [sct] Pointer to free'd kvm list */
nco_kvm_lst_free /* [fnc] Relinquish dynamic memory from list of kvm structures */
(kvm_sct *kvm, /* I/O [sct] List of kvm structures */
 const int kvm_nbr) /* I [nbr] Number of kvm structures */
{
  /* Purpose: Relinquish dynamic memory from list of kvm structures
     End of list is indicated by NULL in key slot */
  for(int kvm_idx=0;kvm_idx<kvm_nbr;kvm_idx++){
    /* Check pointers' nullity */
    if(kvm[kvm_idx].key){kvm[kvm_idx].key=(char *)nco_free(kvm[kvm_idx].key);}
    if(kvm[kvm_idx].val){kvm[kvm_idx].val=(char *)nco_free(kvm[kvm_idx].val);}
  } /* end for */
  if(kvm) kvm=(kvm_sct *)nco_free(kvm);
  return kvm;
} /* end nco_kvm_lst_free() */

void
nco_kvm_prn(kvm_sct kvm)
{
  if(kvm.key) (void)fprintf(stdout,"%s = %s\n",kvm.key,kvm.val); else return;
} /* end nco_kvm_prn() */

char * /* O/I [sng] string that has backslash(es) */
nco_remove_backslash
(char *args) /* O/I [sng] String that had already been got rid of backslash(es) */
{
  /* Purpose: recursively remove backslash from string */
  char *backslash_pos=strstr(args,"\\"); 
  if(backslash_pos){
    int absolute_pos=backslash_pos-args;/* Get memory address offset */
    memmove(&args[absolute_pos],&args[absolute_pos+1L],strlen(args)-absolute_pos);
    return nco_remove_backslash(args);
  }else return args;
} /* !nco_remove_backslash() */

char * /* O [sng] Flag without hyphens */
nco_remove_hyphens /* [fnc] Remove hyphens that come before the flag */
(char* args) /* I [sng] Flag with hyphens in it */
{
  char *hyphen_pos=strstr(args,"-"); 
  if(hyphen_pos){
    int absolute_pos=hyphen_pos-args; /* Get memory address offset */
    memmove(&args[absolute_pos],&args[absolute_pos+1L],strlen(args)-absolute_pos);
    return nco_remove_hyphens(args);
  }else return args;
} /* !nco_remove_hyphens */

char ** /* O [sng] Group of split strings */
nco_sng_split /* [fnc] Split string by delimiter */
(const char *source, /* I [sng] Source string */
 const char *delimiter) /* I [char] Delimiter */
{
  /* Split string into double character pointer, each secondary pointer represents a string after splitting.
     Example: a, b=1 will be split into *<a> = "a" *<b> = "b=1" with delimiter of SUBDELIMITER 
     Remember to free() after calling this function */
  char **sng_fnl=NULL;
  char *sng_tmp=strdup(source);
  int *idx_lst=NULL;
  size_t counter=nco_count_blocks(source,delimiter);
  size_t dlm_idx=0L;    
  
  /* Special case for single argument */
  if(!strstr(sng_tmp,delimiter)){
    sng_fnl=(char **)nco_malloc(sizeof(char *));
    sng_fnl[0]=sng_tmp;
    return sng_fnl;
  } /* !sng_tmp */
  
  /* Count positions where delimiter appears */
  sng_fnl=(char **)nco_malloc(sizeof(char *)*counter);
  idx_lst=(int *)nco_malloc(sizeof(int)*(counter+2));
  if(sng_fnl){
    char *sng_tmp_ptr=sng_tmp;
    while(sng_tmp_ptr){
      if(sng_tmp_ptr == sng_tmp || (sng_tmp_ptr-1)[0] != '\\') idx_lst[dlm_idx++]=sng_tmp_ptr-sng_tmp;
      sng_tmp_ptr=strstr(sng_tmp_ptr+1L,delimiter);
    } /* !sng_tmp_ptr */
    idx_lst[dlm_idx]=strlen(sng_tmp);

    /* Copy first token since it is not preceded by delimiter */
    sng_fnl[0]=(char *)nco_malloc(idx_lst[1]*sizeof(char)+1L);
    memcpy(sng_fnl[0],sng_tmp,idx_lst[1]);
    sng_fnl[0][idx_lst[1]]='\0';

    /* Copy rest of tokens based on positions of delimiter */
    for(dlm_idx=1;dlm_idx<counter;dlm_idx++){
      int sng_sz=idx_lst[dlm_idx+1]-idx_lst[dlm_idx]-strlen(delimiter);
      sng_fnl[dlm_idx]=(char *)nco_malloc(sng_sz*sizeof(char)+1L);
      memcpy(sng_fnl[dlm_idx],sng_tmp+idx_lst[dlm_idx]+strlen(delimiter),sng_sz);
      sng_fnl[dlm_idx][sng_sz]='\0';  
    } /* !dlm_idx */
    if(idx_lst) idx_lst=(int *)nco_free(idx_lst);
    if(sng_tmp) sng_tmp=(char *)nco_free(sng_tmp);
  }else{
    if(idx_lst) idx_lst=(int *)nco_free(idx_lst);
    if(sng_tmp) sng_tmp=(char *)nco_free(sng_tmp);
    return NULL;
  } /* !sng_fnl */
  
  return sng_fnl;
} /* end nco_sng_split() */

int /* O [flg] Option is flag */
nco_opt_is_flg /* [fnc] Check whether option is registered as NCO flag */
(const char *flag) /* I [sng] Input string */
{
  const char fnc_nm[]="nco_opt_is_flg()"; /* [sng] Function name */
  const char *rgr_flags[]={
    "no_area",
    "no_area_out",
    "cell_measures",
    "cll_msr",
    "no_cell_measures",
    "no_cll_msr",
    "curvilinear",
    "crv",
    "dgn_area",
    "diagnose_area",
    "dgn_bnd",
    "diagnose_bounds",
    "infer",
    "nfr",
    "no_stagger",
    "no_stg",
    "cell_area_nco",
    "cell_area_quad"
    };
  const char *gaa_flags[]={""};
  const char *trr_flags[]={""};
  const char *ppc_flags[]={""};
  
  for(unsigned int index=0;index<sizeof(rgr_flags)/sizeof(char *);index++){
    if(!strcmp(flag,rgr_flags[index])){
      return NCO_NOERR;
    }
  }
  for(unsigned int index=0;index<sizeof(gaa_flags)/sizeof(char *);index++){
    if(!strcmp(flag, gaa_flags[index])) return NCO_NOERR;
  }
  for(unsigned int index=0;index<sizeof(trr_flags)/sizeof(char *);index++){
    if(!strcmp(flag, trr_flags[index])) return NCO_NOERR;
  }
  for(unsigned int index=0;index<sizeof(ppc_flags)/sizeof(char *);index++){
    if(!strcmp(flag, ppc_flags[index])) return NCO_NOERR;
  }
  
  (void)fprintf(stderr, "%s: ERROR %s Multi-Argument (MTA) parser reports unrecognized option \"%s\"\n%s: HINT Lack of equals sign indicates this may be a mis-typed flag rather than an erroneous key-value pair specification. Valid MTA flags are listed below. Synonyms for each flag are listed on the same line. A leading \"--\" is optional. MTA documentation is at http://nco.sf.net/nco.html#mta\n",nco_prg_nm_get(),fnc_nm,flag,nco_prg_nm_get());

  (void)fprintf(stderr, "Regridder flags (\"rgr\" indicator):\n");
  for(unsigned int index=0;index<sizeof(rgr_flags)/sizeof(char *);index++){
    (void)fprintf(stderr,"  %2d. %s\n",index+1,rgr_flags[index]);
  }
  /*
    (void)fprintf(stderr, "ncks gaa (Global Attribute Adding) flags:\n");
    for(unsigned int index=0;index<sizeof(gaa_flags)/sizeof(char *);index++)
    {
      (void)fprintf(stderr, "%s\n",gaa_flags[index]);
    }
  
  (void)fprintf(stderr, "ncks trr (Terraref) flags:\n");
  for(unsigned int index=0;index<sizeof(trr_flags)/sizeof(char *);index++)
    {
      (void)fprintf(stderr, "%s\n",trr_flags[index]);
    }
  (void)fprintf(stderr, "ncks ppc (Precision-Preserving Compression) flags:\n");
  for(unsigned int index=0;index<sizeof(ppc_flags)/sizeof(char *);index++)
    {
      (void)fprintf(stderr, "%s\n",ppc_flags[index]);
    }
  */
  
  return NCO_ERR;
}

int /* O [flg] Input has valid syntax */
nco_input_check /* [fnc] Check whether input has valid syntax */
(const char *args) /* O [sng] Input arguments */
{
  /* Check argument syntax
   * If return value is false the parser will terminate the program */
  const char fnc_nm[]="nco_input_check()"; /* [sng] Function name */
  
  if(!strstr(args,"=")){ // If no equal sign in arguments
    char *arg_copy=strdup(args);
    if(!nco_opt_is_flg(nco_remove_hyphens(arg_copy))){
      (void)fprintf(stderr,"%s: ERROR %s did not detect equal sign between key and value for argument \"%s\".\n%s: HINT This can occur when the designated or default key-value delimiter string \"%s\" is mixed into the literal text of the value. Try changing delimiter to a string guaranteed not to appear in the value string with, e.g., --dlm=\"##\".\n",nco_prg_nm_get(),fnc_nm,args,nco_prg_nm_get(),nco_mta_dlm_get());
      nco_free(arg_copy);
      return NCO_ERR;
    }
    nco_free(arg_copy);
  }
  if(strstr(args,"=") == args){ // Equal sign is at argument start (no key)
    (void)fprintf(stderr,"%s: ERROR %s reports no key in key-value pair for argument \"%s\".\n%s: HINT It appears that an equal sign is the first character of the argument, meaning that a value was specified with a corresponding key.\n",nco_prg_nm_get(),fnc_nm,args,nco_prg_nm_get()); 
    return NCO_ERR;
  }
  if(strstr(args,"=") == args+strlen(args)-1L){ // Equal sign is at argument end
    (void)fprintf(stderr,"%s: ERROR %s reports no value in key-value pair for argument \"%s\".\n%s: HINT This usually occurs when the value of a key is unintentionally omitted, e.g., --gaa foo= , --ppc foo= , --rgr foo= , or --trr foo= . Each equal sign must immediatte precede a value for the specified key(s).\n",nco_prg_nm_get(),fnc_nm,args,nco_prg_nm_get());
    return NCO_ERR;
  }
  return NCO_NOERR;
} /* !nco_input_check() */

int // O [int] Number of string blocks that will be split with delimiter
nco_count_blocks // [fnc] Check number of string blocks that will be split with delimiter
(const char *args, // I [sng] String to be split
 const char *delimiter) // I [sng] Delimiter
{
  int sng_nbr=1;
  const char *crnt_chr=strstr(args,delimiter);
  
  while(crnt_chr){
    if((crnt_chr-1L)[0] != '\\') sng_nbr++;
    crnt_chr=strstr(crnt_chr+1L,delimiter);
  }
  return sng_nbr;
} /* !nco_count_blocks() */

kvm_sct * /* O [kvm_sct] the pointer to the first kvm structure */
nco_arg_mlt_prs /* [fnc] main parser, split the string and assign to kvm structure */
(const char *args) /* I [sng] input string */
{
  /* Main parser: Split whole argument into key value pair and send to sng2kvm */
  if(!args) return NULL;
  
  char *nco_mta_dlm=nco_mta_dlm_get(); /* [sng] Multi-argument delimiter */
  char **separate_args=nco_sng_split(args,(const char *)nco_mta_dlm);
  size_t counter=nco_count_blocks(args,nco_mta_dlm)*nco_count_blocks(args,nco_mta_sub_dlm); /* [nbr] Maximum number of kvm structures in this argument */

   for(int index=0;index<nco_count_blocks(args,nco_mta_dlm);index++){
    if(!nco_input_check(separate_args[index])) nco_exit(EXIT_FAILURE);
  } /* !index */
  
  /* kvm array to return */
  kvm_sct *kvm_set=(kvm_sct *)nco_malloc(sizeof(kvm_sct)*(counter+5L));
  size_t kvm_idx=0;
  
  for(int sng_idx=0;sng_idx<nco_count_blocks(args,nco_mta_dlm);sng_idx++){
    char *value=NULL, *set_of_keys=NULL;
    if(strstr(separate_args[sng_idx],"=")){ /*key-value pair case*/
      value=strdup(strstr(separate_args[sng_idx],"="));
      set_of_keys=strdup(strtok(separate_args[sng_idx],"="));
    }else{ /* Pure key case (flags) */
      set_of_keys=strdup(nco_remove_hyphens(separate_args[sng_idx]));
      value=NULL;
    }
    char **individual_args=nco_sng_split(set_of_keys,nco_mta_sub_dlm);
    
    for(int sub_idx=0;sub_idx<nco_count_blocks(set_of_keys,nco_mta_sub_dlm);sub_idx++){
      char *temp_value=strdup(individual_args[sub_idx]);
      temp_value=(char *)nco_realloc(temp_value,strlen(temp_value)+(value ? strlen(value) : 0)+1L);
      if(value) temp_value= strcat(temp_value,value);
      kvm_set[kvm_idx++]=nco_sng2kvm(nco_remove_backslash(temp_value));
      nco_free(temp_value);
    } /* !sub_idx */
    individual_args=nco_sng_lst_free(individual_args,nco_count_blocks(set_of_keys,nco_mta_sub_dlm));
    nco_free(set_of_keys);
    nco_free(value);
  } /* end outer loop */
  separate_args=nco_sng_lst_free(separate_args,nco_count_blocks(args,nco_mta_dlm));
  /* Ending flag for kvm array */
  kvm_set[kvm_idx].key=NULL;

  return kvm_set;
} /* !nco_arg_mlt_prs() */

char * /* O [sng] Joined strings */
nco_join_sng /* [fnc] Join strings with delimiter */
(X_CST_PTR_CST_PTR_Y(char,sng_lst), /* I [sng] List of strings to join */
 const int sng_nbr) /* I [int] Number of strings */
{
  /* Purpose: Concatenate strings with delimiters */

  char *nco_mta_dlm=nco_mta_dlm_get(); /* [sng] Multi-argument delimiter */

  if(sng_nbr == 1) return strdup(sng_lst[0]);
  
  size_t sng_lng=0L;
  size_t cpy_ctr=0L;
  for(int sng_idx=0;sng_idx<sng_nbr;sng_idx++)
    sng_lng+=strlen(sng_lst[sng_idx])+1L;

  char *sng_fnl=(char *)nco_malloc(sng_lng*sizeof(char)+1L);
  for(int sng_idx=0;sng_idx<sng_nbr;sng_idx++){
    size_t tmp_lng=strlen(sng_lst[sng_idx]);
    strcpy(sng_fnl+cpy_ctr,sng_lst[sng_idx]);
    if(sng_idx<sng_nbr-1) strcpy(sng_fnl+cpy_ctr+tmp_lng,nco_mta_dlm);
    cpy_ctr+=(tmp_lng+1L);
  } /* !sng_idx */
  return sng_fnl;
} /* !nco_join_sng() */
