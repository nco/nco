/* $Header$ */

/* Purpose: Multi-argument utilities */

/* Copyright (C) 2016--2016 Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   GNU General Public License (GPL) Version 3 with exceptions described in the LICENSE file
   
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
  char *args_copy=strdup(args);
  char *ptr_for_free=args_copy;
  kvm_sct kvm;
  
  kvm.key=strdup(strsep(&args_copy,"="));
  kvm.val=strdup(args_copy);
  
  nco_free(ptr_for_free);
  
  /* If nco_malloc() cannot allocate sufficient memory, either key or value would be NULL; print error message and not quit */
  if(!kvm.key || !kvm.val){
    (void)fprintf(stderr,"%s: ERROR system has insufficient memory\n",nco_prg_nm_get());
    nco_exit(EXIT_FAILURE);
  }
  return kvm;
}

char * /* O [sng] Stripped-string */
nco_sng_strip /* [fnc] Strip leading and trailing white space */
(char *sng) /* I/O [sng] String to strip */
{
  /* fxm: seems not working for \n??? */
  char *srt=sng;
  while(isspace(*srt)) srt++;
  size_t end=strlen(srt);
  if(srt != sng){
    memmove(sng,srt,end);
    sng[end]='\0';
  } /* endif */
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

char *nco_remove_backslash(char *args)
{ /* Purpose: recursively remove backslash from string */
  char *backslash_pos=strstr(args,"\\"); 
  if(backslash_pos){
    int absolute_pos=backslash_pos-args;/* Get memory address offset */
    memmove(&args[absolute_pos],&args[absolute_pos+1L],strlen(args)-absolute_pos);
    return nco_remove_backslash(args);
  }else return args;
}

char ** /* O [sng] Group of split strings */
nco_sng_split /* [fnc] Split string by delimiter */
(const char *source, /* I [sng] Source string */
 const char *delimiter) /* I [char] Delimiter */
{
  /* Split string into double character pointer, each sencondary pointer represents a string after splitting.
     Example: a, b=1 will be split into *<a> = "a" *<b> = "b=1" with a delimiter of SUBDELIMITER 
     Remember to free() after calling this function */
  char **sng_fnl=NULL;
  char *temp=strdup(source);
  size_t counter=nco_count_blocks(source,delimiter);
  size_t dlm_idx=0L;    
  
  /* Special case for single argument */
  if(!strstr(temp,delimiter)){
    sng_fnl=(char **)nco_malloc(sizeof(char *));
    sng_fnl[0]=temp;
    return sng_fnl;
  }
  
  /* Count positions where delimiter appears */
  sng_fnl=(char **)nco_malloc(sizeof(char *)*counter);
  int idx_lst[counter+2];
  if(sng_fnl){
    char *temp_pt=temp;
    while(temp_pt){
      if(temp_pt == temp || (temp_pt-1)[0] != '\\') idx_lst[dlm_idx++]=temp_pt-temp;
      temp_pt=strstr(temp_pt+1L,delimiter);
    }
    idx_lst[dlm_idx]=strlen(temp);

    /* Copy first token since it is not preceded by delimiter */
    sng_fnl[0]=(char*)malloc(idx_lst[1]+1);
    memcpy(sng_fnl[0], temp, idx_lst[1]);
    sng_fnl[0][idx_lst[1]]='\0';

    /* Copy rest of tokens based on positions of delimiter */
    for(dlm_idx=1;dlm_idx<counter;dlm_idx++){
      int sng_size=idx_lst[dlm_idx+1]-idx_lst[dlm_idx]-strlen(delimiter);
      sng_fnl[dlm_idx]=(char *)malloc(sng_size+1);
      memcpy(sng_fnl[dlm_idx],temp+idx_lst[dlm_idx]+strlen(delimiter),sng_size);
      sng_fnl[dlm_idx][sng_size]='\0';  
    }
    nco_free(temp);
  }else{
    nco_free(temp);
    return NULL;
  } //end if
  return sng_fnl;
} /* end nco_sng_split() */

int /* O [flg] Input has valid syntax */
nco_input_check /* [fnc] Check whether input has valid syntax */
(const char *args) /* O [sng] Input arguments */
{
  /* Check argument syntax
   * If return value is false the parser will terminate the program */
  const char fnc_nm[]="nco_input_check()"; /* [sng] Function name */

  if(!strstr(args,"=")){ //If no equal sign in arguments
    (void)fprintf(stderr,"%s: ERROR %s reports no equal sign detected. HINT: This can occur when the designated or default key-value delimiter \"%s\" is mixed into the literal text of the value. Try changing the delimiter to a string guaranteed not to appear in the value string with, e.g., --dlm=\"##\".\n",nco_prg_nm_get(),fnc_nm,nco_mta_dlm_get());
    return 0;
  } //endif
  if(strstr(args,"=")==args){ // Equal sign is at argument start (no key)
    (void)fprintf(stderr,"%s: ERROR %s reports no key in key-value pair.\033[0m\n",nco_prg_nm_get(),fnc_nm); 
    return 0;
  } //endif
  if(strstr(args,"=")==args+strlen(args)-1){ // Equal sign is at argument end
    (void)fprintf(stderr,"%s: ERROR %s reports no value in key-value pair.\033[0m\n",nco_prg_nm_get(),fnc_nm); 
    return 0;
  } //endif
  return 1;
}

int // O [int] Number of string blocks that will be split with delimiter
nco_count_blocks // [fnc] Check number of string blocks that will be split with delimiter
(const char *args, // I [sng] String to be split
 const char *delimiter) // I [sng] Delimiter
{
  int sng_nbr=1;
  const char *crnt_chr=strstr(args,delimiter);
  
  while (crnt_chr) {
    if((crnt_chr-1)[0]!='\\')
      sng_nbr++;
    crnt_chr=strstr(crnt_chr+1,delimiter);
  }
  return sng_nbr;
}

void 
nco_sng_lst_free_void /* [fnc] free() string list */
(char **sng_lst, /* I/O [sng] String list to free() */
 const int sng_nbr) /* I [int] Number of strings in list */
{
  /* Use to free the double character pointer, and set the pointer to NULL */
  for(int index=0;index<sng_nbr;index++){nco_free(sng_lst[index]);}
  nco_free(sng_lst);
  sng_lst=NULL;
}

kvm_sct * /* O [kvm_sct] the pointer to the first kvm structure */
nco_arg_mlt_prs /* [fnc] main parser, split the string and assign to kvm structure */
(const char *args) /* I [sng] input string */
{
  /* Main parser: Split whole argument into key value pair and send to sng2kvm */
  if(!args) return NULL;
  
  char *nco_mta_dlm=nco_mta_dlm_get(); /* [sng] Multi-argument delimiter */
  char **separate_args=nco_sng_split(args,(const char *)nco_mta_dlm);
  size_t counter=nco_count_blocks(args,nco_mta_dlm)*nco_count_blocks(args,nco_mta_sub_dlm); // Maximum number of kvm structures in this argument

  for(int index=0;index<nco_count_blocks(args,nco_mta_dlm);index++){
    if(!nco_input_check(separate_args[index])) nco_exit(EXIT_FAILURE);
  } //end loop
  
  kvm_sct* kvm_set=(kvm_sct*)nco_malloc(sizeof(kvm_sct)*(counter+5)); //kvm array intended to be returned
  size_t kvm_idx=0;
  
  for(int sng_idx=0;sng_idx<nco_count_blocks(args,nco_mta_dlm);sng_idx++){
    char *value=strdup(strstr(separate_args[sng_idx],"="));
    char *set_of_keys=strdup(strtok(separate_args[sng_idx],"=")); 
    char **individual_args=nco_sng_split(set_of_keys,nco_mta_sub_dlm);
    
    for(int sub_idx=0; sub_idx<nco_count_blocks(set_of_keys,nco_mta_sub_dlm);sub_idx++){
      char *temp_value=strdup(individual_args[sub_idx]);
	    temp_value=(char *)realloc(temp_value,strlen(temp_value)+strlen(value)+1);
      temp_value=strcat(temp_value,value);
      kvm_set[kvm_idx++]=nco_sng2kvm(nco_remove_backslash(temp_value));
      nco_free(temp_value);
    }//end inner loop
    nco_sng_lst_free_void(individual_args,nco_count_blocks(set_of_keys,nco_mta_sub_dlm));
    nco_free(set_of_keys);
    nco_free(value);//clean up
  }//end outer loop
  nco_sng_lst_free_void(separate_args,nco_count_blocks(args,nco_mta_dlm));
  kvm_set[kvm_idx].key=NULL; //Add an ending flag for kvm array.
  return kvm_set;
}

char * /* O [sng] Joined strings */
nco_join_sng /* [fnc] Join strings with delimiter */
(const char **sng_lst, /* I [sng] List of strings being connected */
 const int sng_nbr) /* I [int] Number of strings */
{
  /* Purpose: join the strings with delimiters. It will be used when the number of
   * arguments is larger than 1; usually it is in the old NCO argument style*/

  char *nco_mta_dlm=nco_mta_dlm_get(); /* [sng] Multi-argument delimiter */

  if(sng_nbr == 1) return strdup(sng_lst[0]);
  
  size_t word_length=0L;
  size_t copy_counter=0L;
  for(int index=0;index<sng_nbr;index++){
    word_length+=strlen(sng_lst[index])+1L;
  }
  char *final_string=(char *)nco_malloc(word_length+1L);
  for(int sng_idx=0;sng_idx<sng_nbr;sng_idx++){
    size_t temp_length=strlen(sng_lst[sng_idx]);
    strcpy(final_string+copy_counter,sng_lst[sng_idx]);
    
    if(sng_idx<sng_nbr-1) strcpy(final_string+copy_counter+temp_length,nco_mta_dlm);
    copy_counter+=(temp_length+1);
  }
  return final_string;
}
