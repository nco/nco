/* $Header$ */

/* Purpose: String utilities */

/* Copyright (C) 1995--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

#include "nco_sng_utl.h" /* String utilities */

#ifdef NEED_STRSEP
/* 20161106 UNIX always provides strsep(), MSVC never does */
char * /* O [sng] String to separate */
strsep /* [fnc] Separate strings */
(char ** const sng_trg, /* I [sng] String to separate */
 const char * const sng_dlm) /* I [sng] Delimiter */
{
  /* Purpose: NCO replacement for strsep() for Windows MSVC
     http://stackoverflow.com/questions/8512958/is-there-a-windows-variant-of-strsep */
  char* sng_srt = *sng_trg;
  char* sng_ptr;

  sng_ptr= (sng_srt != NULL) ? strpbrk(sng_srt,sng_dlm) : NULL;

  if(sng_ptr == NULL){
    *sng_trg = NULL;
  }else{
    *sng_ptr='\0';
    *sng_trg=sng_ptr+1;
  } /* endif */
  return sng_srt;
} /* !strsep() */
#endif /* !NEED_STRSEP */

#ifdef NEED_STRCASECMP
int /* O [enm] [-1,0,1] sng_1 [<,=,>] sng_2 */
strcasecmp /* [fnc] Lexicographical case-insensitive string comparison */
(const char * const sng_1, /* I [sng] First string */
 const char * const sng_2) /* I [sng] Second string */
{
  /* NB: Written by CSZ for clarity and type-safety, not speed nor POSIX conformance
     POSIX may require conversion to unsigned char before comparison
     StackOverflow:
     strcasecmp(0 is not a C or C++ standard. It is defined by POSIX.1-2001 and 4.4BSD.
     Assuming your system is POSIX or BSD compliant, you must #include <strings.h> */

  /* Copy of (const) input strings */
  char *sng_1_c;
  char *sng_2_c;
  char chr_1;
  char chr_2;

  sng_1_c=(char *)sng_1;
  sng_2_c=(char *)sng_2;

  while(1){
    chr_1=tolower(*sng_1_c++);
    chr_2=tolower(*sng_2_c++);
    if(chr_1 < chr_2) return -1;
    if(chr_1 > chr_2) return 1;
    if(chr_1 == 0)    return 0;
  } /* end while */
} /* end strcasecmp() */
#endif /* !NEED_STRCASECMP */

#ifdef NEED_STRNCASECMP
int /* O [enm] [-1,0,1] sng_1 [<,=,>] sng_2 */
strncasecmp /* [fnc] Lexicographical case-insensitive string comparison */
(const char * const sng_1, /* I [sng] First string */
 const char * const sng_2, /* I [sng] Second string */
 const size_t chr_nbr) /* I [nbr] Compare at most chr_nbr characters */
{
  /* NB: Written by CSZ for clarity and type safety, not speed nor POSIX conformance
     POSIX may require conversion to unsigned char before comparison
     Use at own risk */
  /* Copy of (const) input strings */
  char *sng_1_c;
  char *sng_2_c;
  char chr_1;
  char chr_2;
  size_t chr_nbr_cpy;

  if(chr_nbr == 0L) return 0;

  sng_1_c=(char *)sng_1;
  sng_2_c=(char *)sng_2;
  chr_nbr_cpy=chr_nbr;

  while(chr_nbr_cpy-- > 0L){
    chr_1=tolower(*sng_1_c++);
    chr_2=tolower(*sng_2_c++);
    if(chr_1 < chr_2) return -1;
    if(chr_1 > chr_2) return 1;
    if(chr_1 == 0)    return 0;
  } /* end while */
  return 0;
} /* end strncasecmp() */
#endif /* !NEED_STRNCASECMP */

#ifdef NEED_ISBLANK
int /* O [flg] Character is a space or horizontal tab */
isblank /* [fnc] Is character a space or horizontal tab? */
(const int chr) /* I [enm] Character to check */
{
  /* 20150203 Initial version */
  return chr == ' ' || chr == '\t';
} /* end isblank() */
#endif /* !NEED_ISBLANK */

/* 20130827 GNU g++ always provides strcasestr(), MSVC never does */
#ifndef __GNUG__
# ifdef NEED_STRCASESTR
char * /* O [sng] Pointer to sng_2 in sng_1 */
strcasestr /* [fnc] Lexicographical case-insensitive string search */
(const char * const sng_1, /* I [sng] First string */
 const char * const sng_2) /* I [sng] Second string */
{
  /* 20120706 Initial version discards const, triggers compiler warnings
     20120803 Kludge with strdup() to try to keep const intact. Fail.
     20130827 Add (char *) cast (compile error in MSVC) */
  char *hys_ptr; /* Haystack pointer */
  char *startn=0;
  char *np=0;
  /* Loop exits on NUL */
  for(hys_ptr=(char *)sng_1;*hys_ptr;hys_ptr++){
    if(np){
      if(toupper(*hys_ptr) == toupper(*np)){
	if(!*++np) return startn;
      }else{
	np=0;
      } /* endif uppercases match */
    }else if(toupper(*hys_ptr) == toupper(*sng_2)){
      np=(char *)sng_2+1;
      startn=hys_ptr;
    } /* else if */
  } /* end loop over haystack */
  return 0;
} /* end strcasestr() */
# endif /* !NEED_STRCASESTR */
#endif /* __GNUG__ */

#ifdef NEED_STRDUP
char * /* [sng] Copy of input string */
strdup /* [fnc] Duplicate string */
(const char *sng_in) /* [sng] String to duplicate */
{
  /* Purpose: Provide strdup() for broken systems 
     Input string must be NUL-terminated */
  size_t sng_lng=strlen(sng_in)+1UL;
  /* Use nco_malloc() even though strdup() is system function 
     This ensures all NCO code goes through nco_malloc()  */
  char *sng_out=(char *)nco_malloc(sng_lng*sizeof(char));
  if(sng_out) strcpy(sng_out,sng_in);
  return sng_out;
} /* end strdup() */
#endif /* !NEED_STRDUP */

#ifdef NEED_STRTOLL
long long int /* O [nbr] String as long long integer */
strtoll /* [fnc] Convert string to a long long integer */
(const char * const nptr,
 char ** const endptr,
 const int base)
{
  /* Purpose: Compatibility function for strtoll()
     Needed by some C++ compilers, e.g., AIX xlC
     20120703: rewrite to cast result of strtol() to long long and return */
  long long nbr_out;
  nbr_out=(long long)strtol(nptr,endptr,base);
  return nbr_out;
} /* end strtoll() */
#endif /* !NEED_STRTOLL */

char * /* O [sng] Parsed command line */
nco_cmd_ln_sng /* [fnc] Re-construct command line from arguments */
(const int argc, /* I [nbr] Argument count */
 CST_X_PTR_CST_PTR_CST_Y(char,argv)) /* I [sng] Command line argument values */
{
  /* Purpose: Re-construct command line from argument list and number */
  char *cmd_ln; /* [sng] Parsed command line */
  
  int cmd_ln_sz=0;
  int idx;

  for(idx=0;idx<argc;idx++){
    /* Add one to size of each argument for a space character */
    cmd_ln_sz+=(int)strlen(argv[idx])+1;
  } /* end loop over args */
  if(argc <= 0){
    cmd_ln=(char *)nco_malloc(sizeof(char));
    cmd_ln[0]='\0';
  }else{
    cmd_ln=(char *)nco_malloc(cmd_ln_sz*sizeof(char));
    (void)strcpy(cmd_ln,argv[0]);
    for(idx=1;idx<argc;idx++){
      (void)strcat(cmd_ln," ");
      (void)strcat(cmd_ln,argv[idx]);
    } /* end loop over args */
  } /* end else */

  return cmd_ln; /* [sng] Parsed command line */
} /* end nco_cmd_ln_sng() */

void 
sng_idx_dlm_c2f /* [fnc] Replace brackets with parentheses in a string */
(char *sng) /* [sng] String to change from C to Fortran notation */
{
  /* Purpose: Replace brackets with parentheses in a string */
  while(*sng){
    if(*sng == '[') *sng='(';
    if(*sng == ']') *sng=')';
    sng++;
  } /* end while */
} /* end sng_idx_dlm_c2f() */

char * /* O [sng] CDL-compatible name */
nm2sng_cdl /* [fnc] Turn variable/dimension/attribute name into legal CDL */
(const char * const nm_sng) /* I [sng] Name to CDL-ize */
{
  /* Purpose: Turn variable/dimension/attribute name into legal CDL 
     Currently this means protecting special characters with backslash so ncgen can read them
     NB: Calling function must free() memory containing CDL-ized string
     Weird file menagerie shows that:
     NASA HDF4 TRMM files (3B43*.HDF) have filenames starting with numerals (and no metadata)
     NASA HDF4 AIRS files (AIRS*.hdf) have colons in dimension names
     NASA HDF4 CERES files (CER*) have forward slashes and spaces in attribute, dimension, and variable names
     NASA HDF4 AMSR_E files (AMSR_E*.hdf) have spaces in variable names, colons and spaces in dimension names
     NASA HDF4 MODIS files (MOD*.hdf) have colons in names
     NASA HDF4 MOPPITT files (MOP*.hdf) have spaces in variable names, colons and spaces in dimension names */

  /* https://www.unidata.ucar.edu/software/netcdf/docs/netcdf/CDL-Syntax.html:
     "In CDL, most special characters are escaped with a backslash '\' character, but that character is not actually part of the netCDF name. The special characters that do not need to be escaped in CDL names are underscore '_', period '.', plus '+', hyphen '-', or at sign '@'. */

  char *chr_in_ptr; /* [sng] Pointer to current character in input name */
  char *chr_out_ptr; /* [sng] Pointer to current character in output name */
  char *nm_cdl; /* [sng] CDL-compatible name */
  char *nm_cpy; /* [sng] Copy of input */

  int nm_lng; /* [nbr] Length of original name */
  
  if(nm_sng == NULL) return NULL;

  /* Otherwise name may contain special character(s)... */
  nm_lng=strlen(nm_sng);
  /* Maximum conceivable length of CDL-ized name */
  chr_out_ptr=nm_cdl=(char *)nco_malloc(4*nm_lng+1L);
  /* Copy to preserve const-ness */
  chr_in_ptr=nm_cpy=(char *)strdup(nm_sng);
  /* NUL-terminate in case input string is empty so will be output string */
  chr_out_ptr[0]='\0';

  /* Search and replace special characters */

  /* This block stolen from: ncdump/utils.c/escaped_name() */ 

  if((*chr_in_ptr >= 0x01 && *chr_in_ptr <= 0x20) || (*chr_in_ptr == 0x7f)){
    (void)fprintf(stderr,"%s: ERROR name begins with space or control-character: %c\n",nco_prg_nm_get(),*chr_in_ptr);
    nco_exit(EXIT_FAILURE);
  } /* endif error */

  /* Special case: leading number allowed, but we must escape it for CDL */
  if((*chr_in_ptr >= '0' && *chr_in_ptr <= '9')) *chr_out_ptr++ = '\\';

  while(*chr_in_ptr){
    if(isascii(*chr_in_ptr)){
      if(iscntrl(*chr_in_ptr)){	/* Render control chars as two hex digits, \%xx */
	/* 20170810: GCC7+ produces -Wformat-truncation warning here, that 4 B destination is too small
	   False-positive warning? How to eliminate? */
	snprintf(chr_out_ptr,4,"\\%%%.2x",*chr_in_ptr);
	chr_out_ptr+=4;
      }else{
	switch(*chr_in_ptr){
	case ' ':
	case '!':
	case '"':
	case '#':
	case '$':
	case '&':
	case '\'':
	case '(':
	case ')':
	case '*':
	case ',':
	case ':':
	case ';':
	case '<':
	case '=':
	case '>':
	case '?':
	case '[':
	case ']':
	case '\\':
	case '^':
	case '`':
	case '{':
	case '|':
	case '}':
	case '~':
	  *chr_out_ptr++='\\';
	  *chr_out_ptr++=*chr_in_ptr;
	  break;
	default: /* NB: includes '/' */
	  *chr_out_ptr++=*chr_in_ptr;
	  break;
	} /* end switch */
      } /* not a control character */
    }else{ /* not ascii, assume just UTF-8 byte */
      *chr_out_ptr++=*chr_in_ptr;
    } /* end else not ascii */
    /* Advance character */
    chr_in_ptr++;
  } /* end while loop */
  /* NUL-terminate */
  *chr_out_ptr='\0';

  /* Free memory */
  nm_cpy=(char *)nco_free(nm_cpy);

  return nm_cdl;
} /* end nm2sng_cdl() */

char * /* O [sng] JSON-compatible name */
nm2sng_jsn /* [fnc] Turn variable/dimension/attribute name into legal JSON */
(const char * const nm_sng) /* I [sng] Name to CDL-ize */
{
  /* Valid JSON strings can be any selected set of control chars, any non-conrol chars from ASCII and Unicode 
     Leave Unicode as is for now see Reference: http://www.json.org */   
 
  char *chr_in_ptr; /* [sng] Pointer to current character in input name */
  char *chr_out_ptr; /* [sng] Pointer to current character in output name */
  char *nm_jsn; /* [sng] CDL-compatible name */
  char *nm_cpy; /* [sng] Copy of input */

  int nm_lng; /* [nbr] Length of original name */
  
  if(nm_sng == NULL) return NULL;

  /* Otherwise name may contain special character(s)... */
  nm_lng=strlen(nm_sng);
  /* Maximum conceivable length of CDL-ized name */
  chr_out_ptr=nm_jsn=(char *)nco_malloc(4*nm_lng+1L);
  /* Copy to preserve const-ness */
  chr_in_ptr=nm_cpy=(char *)strdup(nm_sng);
  /* NUL-terminate in case input string is empty so will be output string */
  chr_out_ptr[0]='\0';

  /* Search and replace special characters */
  while(*chr_in_ptr){
    if(iscntrl(*chr_in_ptr)){ 
      switch(*chr_in_ptr){    
         case '\b':
         case '\f':
         case '\n':
         case '\r':
         case '\t': 
         case '\\': 
         case '\"': 
	 /* According to json spec control char  '/' should be escaped but this is mangles filepaths so leave it out for now */
         /* case '\/': */
         case '\0':
           *chr_out_ptr++='\\';
	   *chr_out_ptr++=*chr_in_ptr;
	   break;
         default: 
            /* ignore other control chars */
      	  break;
      } /* !switch */
    }else{ /* !iscntrl() */
      *chr_out_ptr++=*chr_in_ptr;
    } /* !iscntrl() */
    /* Advance character */
    chr_in_ptr++;
  } /* !while() */
  /* NUL-terminate */
  *chr_out_ptr='\0';

  /* Free memory */
  nm_cpy=(char *)nco_free(nm_cpy);

  return nm_jsn;
} /* end nm2sng_jsn() */

char * /* O [sng] CDL-compatible name */
nm2sng_fl /* [fnc] Turn file name into legal string for shell commands */
(const char * const nm_sng) /* I [sng] Name to sanitize */
{
  /* Purpose: Turn file name into legal string for shell commands
     Currently this means protecting special characters with backslash so shell is not confused
     Based on nm2sng_cdl()
     NB: Calling function must free() memory containing sanitized string */

  char *chr_in_ptr; /* [sng] Pointer to current character in input name */
  char *chr_out_ptr; /* [sng] Pointer to current character in output name */
  char *nm_fl; /* [sng] CDL-compatible name */
  char *nm_cpy; /* [sng] Copy of input */

  int nm_lng; /* [nbr] Length of original name */
  
  if(nm_sng == NULL) return NULL;

  /* Otherwise name may contain special character(s)... */
  nm_lng=strlen(nm_sng);
  /* Maximum conceivable length of sanitized name */
  chr_out_ptr=nm_fl=(char *)nco_malloc(4*nm_lng+1L);
  /* Copy to preserve const-ness */
  chr_in_ptr=nm_cpy=(char *)strdup(nm_sng);
  /* NUL-terminate in case input string is empty so will be output string */
  chr_out_ptr[0]='\0';

  /* Search and replace special characters */

  /* This block stolen from: ncdump/utils.c/escaped_name() */ 

  if((*chr_in_ptr >= 0x01 && *chr_in_ptr <= 0x20) || (*chr_in_ptr == 0x7f)){
    (void)fprintf(stderr,"%s: ERROR name begins with space or control-character: %c\n",nco_prg_nm_get(),*chr_in_ptr);
    nco_exit(EXIT_FAILURE);
  } /* endif error */

  while(*chr_in_ptr){
    if(isascii(*chr_in_ptr)){
      if(iscntrl(*chr_in_ptr)){	/* Render control chars as two hex digits, \%xx */
	/* 20170810: GCC7+ produces -Wformat-truncation warning here, that 4 B destination is too small 
	   False-positive warning? How to eliminate? */
	snprintf(chr_out_ptr,4,"\\%%%.2x",*chr_in_ptr);
	chr_out_ptr+=4;
      }else{
	switch(*chr_in_ptr){
	case ' ':
	case '!':
	case '"':
	case '#':
	case '$':
	case '&':
	case '\'':
	case '(':
	case ')':
	case '*':
	case ',':
	  // case ':': /* 20140822: Protecting colon with backslash causes Windows error parsing volume names, e.g., "C:\foo" */
	case ';':
	case '<':
	case '=':
	case '>':
	case '?':
	case '[':
	case ']':
	case '\\':
	case '^':
	case '`':
	case '{':
	case '|':
	case '}':
	case '~':
	  *chr_out_ptr++='\\';
	  *chr_out_ptr++=*chr_in_ptr;
	  break;
	default: /* NB: includes '/' */
	  *chr_out_ptr++=*chr_in_ptr;
	  break;
	} /* end switch */
      } /* not a control character */
    }else{ /* not ascii, assume just UTF-8 byte */
      *chr_out_ptr++=*chr_in_ptr;
    } /* end else not ascii */
    /* Advance character */
    chr_in_ptr++;
  } /* end while loop */
  /* NUL-terminate */
  *chr_out_ptr='\0';

  /* Free memory */
  nm_cpy=(char *)nco_free(nm_cpy);

  return nm_fl;
} /* end nm2sng_fl() */

char * /* O [sng] String containing printable result */
sng2sng_sf /* [fnc] Translate C language string to printable, visible ASCII bytes */
(const char * const sng_val,
 int flg_sf) /* I [sng] String to process */
{
  /* Purpose: Translate string to C-printable, visible ASCII bytes for CDL
     Reference: netcdf-c/ncdump/ncdump.c:pr_att_string() */
  char *sng_val_sng=NULL_CEWI; /* [sng] String of NC_CHAR */

  char chr_val; /* [sng] Current character */
  char val_sng[NCO_ATM_SNG_LNG];

  long chr_idx;
  long sng_lng=long_CEWI; /* [nbr] Length of NC_CHAR string */

  char * (*chr2sng_sf)(const char chr_val, char * const val_sng); 
  
  switch(flg_sf){

    case 1:   
       chr2sng_sf=chr2sng_cdl;
       break;
       
    case 2:
       chr2sng_sf=chr2sng_xml;
       break;
       
    case 3:  
       chr2sng_sf=chr2sng_jsn;
       break;
       
    default:
       chr2sng_sf=chr2sng_cdl;
       break;
  }

      
  sng_lng=strlen(sng_val);

  /* Worst case is printable strings are six or four times longer than unformatted, i.e., '\"' == "&quot;" or '\\' == "\\\\" */
  sng_val_sng=(char *)nco_malloc(6*sng_lng+1UL);
  sng_val_sng[0]='\0';
  for(chr_idx=0;chr_idx<sng_lng;chr_idx++){
    val_sng[0]='\0';
    chr_val=sng_val[chr_idx];
    (void)strcat(sng_val_sng,chr2sng_sf(chr_val,val_sng));
  } /* end loop over character */
  
  return sng_val_sng;
} /* end sng2sng_sf() */

  char * /* O [sng] String containing printable result */
chr2sng_cdl /* [fnc] Translate C language character to printable, visible ASCII bytes */
(const char chr_val, /* I [chr] Character to process */
 char * const val_sng) /* I/O [sng] String to stuff printable result into */
{
  /* Purpose: Translate character to C-printable, visible ASCII bytes for CDL
     Reference: netcdf-c/ncdump/ncdump.c:pr_att_string() */
  
  switch(chr_val){              /* man ascii:Oct   Dec   Hex   Char \X  */
  case '\a': strcpy(val_sng,"\\a"); break; /* 007   7     07    BEL '\a' Bell */
  case '\b': strcpy(val_sng,"\\b"); break; /* 010   8     08    BS  '\b' Backspace */
  case '\f': strcpy(val_sng,"\\f"); break; /* 014   12    0C    FF  '\f' Formfeed */
  case '\n': strcpy(val_sng,"\\n"); break; /* 012   10    0A    LF  '\n' Linefeed */
  case '\r': strcpy(val_sng,"\\r"); break; /* 015   13    0D    CR  '\r' Carriage return */
  case '\t': strcpy(val_sng,"\\t"); break; /* 011   9     09    HT  '\t' Horizontal tab */
  case '\v': strcpy(val_sng,"\\v"); break; /* 013   11    0B    VT  '\v' Vertical tab */
  case '\\': strcpy(val_sng,"\\\\"); break; /* 134   92    5C    \   '\\' */
  case '\'': strcpy(val_sng,"\\\'"); break; /* Unsure why or if this works! */
  case '\"': strcpy(val_sng,"\\\""); break; /* Unsure why or if this works! */
  case '\0':	
    break;
  default: 
    sprintf(val_sng,"%c",chr_val); break;
    break;
  } /* end switch */

  return val_sng;
} /* end chr2sng_cdl() */

char * /* O [sng] String containing printable result */
chr2sng_xml /* [fnc] Translate C language character to printable, visible ASCII bytes */
(const char chr_val, /* I [chr] Character to process */
 char * const val_sng) /* I/O [sng] String to stuff printable result into */
{
  /* Purpose: Translate character to C-printable, visible ASCII bytes for XML
     Reference: netcdf-c/ncdump/ncdump.c:pr_attx_string() 
     NB: Unclear whether and'ing with octal 0377 helps anything */
  //  unsigned char uchar;
  
  //  switch(uchar=chr_val & 0377){              /* man ascii:Oct   Dec   Hex   Char \X  */
  switch(chr_val){              /* man ascii:Oct   Dec   Hex   Char \X  */
  case '\n': strcpy(val_sng,"&#xA;"); break; /* 012   10    0A    LF  '\n' Linefeed */
  case '\r': strcpy(val_sng,"&#xD;"); break; /* 015   13    0D    CR  '\r' Carriage return */
  case '\t': strcpy(val_sng,"&#x9;"); break; /* 011   9     09    HT  '\t' Horizontal tab */
  case '<': strcpy(val_sng,"&lt;"); break;
  case '>': strcpy(val_sng,"&gt;"); break;
  case '&': strcpy(val_sng,"&amp;"); break;
  case '\"': strcpy(val_sng,"&quot;"); break;
  case '\0': /* NB: Unidata handles NUL differently */
    break;
  default: 
    //    if(iscntrl(uchar)) sprintf(val_sng,"&#%d;",uchar); else sprintf(val_sng,"%c",uchar);
    if(iscntrl(chr_val)) sprintf(val_sng,"&#%d;",chr_val); else sprintf(val_sng,"%c",chr_val);
    break;
  } /* end switch */

  return val_sng;
} /* end chr2sng_xml() */

char * /* O [sng] String containing printable result */
chr2sng_jsn /* [fnc] Translate C language character to printable, visible ASCII bytes */
(const char chr_val, /* I [chr] Character to process */
 char * const val_sng) /* I/O [sng] String to stuff printable result into */
{
  /* Purpose: Translate character to C-printable, visible ASCII bytes for JSON
     Reference: http://www.json.org */
  switch(chr_val){              /* man ascii:Oct   Dec   Hex   Char \X  */
  case '\b': strcpy(val_sng,"\\b"); break; /* 010   8     08    BS  '\b' Backspace */
  case '\f': strcpy(val_sng,"\\f"); break; /* 014   12    0C    FF  '\f' Formfeed */
  case '\n': strcpy(val_sng,"\\n"); break; /* 012   10    0A    LF  '\n' Linefeed */
  case '\r': strcpy(val_sng,"\\r"); break; /* 015   13    0D    CR  '\r' Carriage return */
  case '\t': strcpy(val_sng,"\\t"); break; /* 011   9     09    HT  '\t' Horizontal tab */
  case '\\': strcpy(val_sng,"\\\\"); break; /* 134  92    5C    \   '\\' */
  case '\"': strcpy(val_sng,"\\\""); break;/* Unsure why or if this works! */
  /* fxm: According to JSON spec '/' should be escaped but this mangles UNIX filepaths so leave it out for now */
  /* case '\/': strcpy(val_sng,"\\/"); break; */ /* 057   47    2F    /   '\\' */
  case '\0':	
    break;
  default: 
    /* JSON is quite strict about control-chars - only the above are allowed */
    // if(iscntrl(chr_val)) *val_sng=0; else sprintf(val_sng,"%c",chr_val);
    if(iscntrl(chr_val)) *val_sng='\0'; else sprintf(val_sng,"%c",chr_val);
    break;
  } /* end switch */

  return val_sng;
} /* end chr2sng_jsn() */

int /* O [nbr] Number of escape sequences translated */
sng_ascii_trn /* [fnc] Replace C language '\X' escape codes in string with ASCII bytes */
(char * const sng) /* I/O [sng] String to process */
{
  /* Purpose: Replace C language '\X' escape codes in string with ASCII bytes 
     Return number of escape sequences found and actually translated
     This should be same as number of bytes by which string length has shrunk
     For example, consecutive characters "\n" are translated into ASCII '\n' = 10
     Each such translation diminishes string length by one
     Function works for arbitrary number of escape codes in input string
     The escape sequence for NUL itself, \0, causes a warning and is not translated
     Input string must be NUL-terminated or NULL
     This procedure can only diminish, not lengthen, input string size
     Therefore it may safely be performed in-place, i.e., no string copy is necessary
     Translation is done in-place so copy original prior to calling sng_ascii_trn()!!!

     Address pointed to by sng does not change, but memory at that address is altered
     when characters are "moved to the left" if C language escape sequences are embedded.
     Thus string length may shrink */

  const char fnc_nm[]="sng_ascii_trn()"; /* [sng] Function name */

  nco_bool trn_flg; /* Translate this escape sequence */

  char *backslash_ptr; /* [ptr] Pointer to backslash character */
  char backslash_chr='\\'; /* [chr] Backslash character */

  int esc_sqn_nbr=0; /* Number of escape sequences found */
  int trn_nbr=0; /* Number of escape sequences translated */
  
  /* ncatted allows character attributes of 0 length
     Such "strings" do not have NUL-terminator and so may not safely be tested by strchr() */
  if(sng == NULL) return trn_nbr;
  
  /* C language '\X' escape codes are always preceded by a backslash */
  /* Check if control codes are embedded once before entering loop */
  backslash_ptr=strchr(sng,backslash_chr);

  while(backslash_ptr){
    /* Default is to translate this escape sequence */
    trn_flg=True;
    /* Replace backslash character by corresponding control code */
    switch(*(backslash_ptr+1)){ /* man ascii:Oct   Dec   Hex   Char \X  */
    case 'a': *backslash_ptr='\a'; break; /* 007   7     07    BEL '\a' Bell */
    case 'b': *backslash_ptr='\b'; break; /* 010   8     08    BS  '\b' Backspace */
    case 'f': *backslash_ptr='\f'; break; /* 014   12    0C    FF  '\f' Formfeed */
    case 'n': *backslash_ptr='\n'; break; /* 012   10    0A    LF  '\n' Linefeed */
    case 'r': *backslash_ptr='\r'; break; /* 015   13    0D    CR  '\r' Carriage return */
    case 't': *backslash_ptr='\t'; break; /* 011   9     09    HT  '\t' Horizontal tab */
    case 'v': *backslash_ptr='\v'; break; /* 013   11    0B    VT  '\v' Vertical tab */
    case '\\': *backslash_ptr='\\'; break; /* 134   92    5C    \   '\\' */
    case '\?': *backslash_ptr='\?'; break; /* Unsure why or if this works! */
    case '\'': *backslash_ptr='\''; break; /* Unsure why or if this works! */
    case '\"': *backslash_ptr='\"'; break; /* Unsure why or if this works! */
    case '0':	
      /* Translating \0 to NUL makes subsequent portion of input string invisible to all string functions */
      (void)fprintf(stderr,"%s: WARNING C language escape code %.2s found in string, not translating to NUL since this would make the subsequent portion of the string invisible to all C Standard Library string functions\n",nco_prg_nm_get(),backslash_ptr); 
      trn_flg=False;
      /* 20101013: Tried changing above behavior to following, and it opened a Hornet's nest of problems... */
      /* 20170221: Use-case where translating "\0" to NUL would be helpful is when user needs to pad netCDF3 character
	 array value to size of underlying dimension, and have user-specified value NUL-terminated short of full 
	 dimension size */
      /* *backslash_ptr='\0'; *//* 000   0     00    NUL '\0' */
      /*      (void)fprintf(stderr,"%s: WARNING translating C language escape code \"\\0\" found in user-supplied string to NUL. This will make the subsequent portion of the string, if any, invisible to C Standard Library string functions. And that may cause unintended consequences.\n",nco_prg_nm_get());*/
      break;
    default: 
      (void)fprintf(stderr,"%s: WARNING No ASCII equivalent to possible C language escape code %.2s so no action taken\n",nco_prg_nm_get(),backslash_ptr);
      trn_flg=False;
      break;
    } /* end switch */
    if(trn_flg){
      /* Remove character after backslash character */
      (void)memmove(backslash_ptr+1,backslash_ptr+2,(strlen(backslash_ptr+2)+1L)*sizeof(char));
      /* Count translations performed */
      trn_nbr++;
    } /* end if */
    /* Look for next backslash starting at character following current escape sequence (but remember that not all escape sequences are translated) */
    if(trn_flg) backslash_ptr=strchr(backslash_ptr+1,backslash_chr); else backslash_ptr=strchr(backslash_ptr+2,backslash_chr);
    /* Count escape sequences */
    esc_sqn_nbr++;
  } /* end if */

  /* Usually there are no escape codes and sng still equals input value */
  if(nco_dbg_lvl_get() > 3) (void)fprintf(stderr,"%s: DEBUG %s Found %d C-language escape sequences, translated %d of them\n",nco_prg_nm_get(),fnc_nm,esc_sqn_nbr,trn_nbr);

  return trn_nbr;
} /* end sng_ascii_trn() */

void /* O [nbr]  */
sng_trm_trl_zro /* [fnc] Trim zeros trailing decimal point and preceding exponent from floating point string */
(char * const sng, /* I/O [sng] String to process */
 const int trl_zro_max) /* [nbr] Maximum number of trailing zeros allowed */
{
  /* Purpose: Trim zeros trailing decimal point and preceding exponent from floating point string
     Allow trl_zro_max trailing zeros to remain */

  char *trl_zro_ptr; /* [sng] Trailing zero pointer */
  char *dcm_ptr; /* [sng] Decimal point pointer */
  char *xpn_ptr; /* [sng] Exponent pointer */
  char *vld_ptr=NULL; /* [sng] Valid pointer */
  char chr_val; /* [chr] Character value */
  char xpn_chr; /* [chr] Exponent indicator */

  int cnt_zro_rmn; /* [nbr] Number of trailing zeros remaining until maximum reached */
  
  /* Find decimal point, if any */
  dcm_ptr=strchr(sng,'.'); 
  if(dcm_ptr){
    /* Cleave string at exponent portion, if any, then restore exponent later */
    xpn_ptr=strchr(sng,'d'); 
    if(!xpn_ptr) xpn_ptr=strchr(sng,'D'); 
    if(!xpn_ptr) xpn_ptr=strchr(sng,'e'); 
    if(!xpn_ptr) xpn_ptr=strchr(sng,'E'); 
    if(xpn_ptr){
      xpn_chr=*xpn_ptr;
      /* Amputate exponent so strrchr() finds zeros in decimal not in exponent */
      *xpn_ptr='\0';
    } /* !xpn_ptr */

    /* Find last zero, if any, after decimal point and before exponent */
    trl_zro_ptr=strrchr(dcm_ptr,'0'); 
    /* Restore exponent indicator, if any */
    if(xpn_ptr) *xpn_ptr=xpn_chr;

    /* Zero exists to right of decimal and left of exponent */
    if(trl_zro_ptr){
      chr_val=*(trl_zro_ptr+1);
      /* If next character is a (non-zero) digit, then this is not a trailing zero */
      if(isdigit(chr_val)) return;
      /* Next character is a NUL or exponent (d,D,e,E) or floating type suffix (d,D,f,F) */
      /* This is a trailing zero. Allow given number of trailing zeros. */
      cnt_zro_rmn=trl_zro_max;
      while(cnt_zro_rmn > 0){
	cnt_zro_rmn--;
	/* Shift pointer back one character. If that is zero, continue else return. */
	if(*trl_zro_ptr-- != '0') return;
      } /* end while */

      /* All characters to right are valid */
      vld_ptr=trl_zro_ptr+1;

      /* Trim all remaining consecutive zeros leftward */
      while(*trl_zro_ptr == '0') *trl_zro_ptr--='\0';

      /* Copy allowed zeros and/or exponent, if any, to current location
	 20141124: This algorithm has worked fine for ~1 year
	 Learned today, though, that valgrind does not like, and Mac OS X disallows, 
	 source and destination to overlap in strcpy() and friends
	 http://www.network-theory.co.uk/docs/valgrind/valgrind_49.html
	 Use explicit loop (instead of strncpy()) to copy src to dst
	 This avoids triggering warnings in valgrind and crashes in Mac OS X */
      /*strncpy(trl_zro_ptr+1UL,vld_ptr,strlen(vld_ptr)+1UL);*/
      char *end=vld_ptr+strlen(vld_ptr)+1UL;
      char *dst=trl_zro_ptr+1UL;
      for(char *src=vld_ptr;src<=end;src++) *dst++=*src;
    } /* end if trl_zro_ptr */
  } /* end if dcm_ptr */

  return;
} /* end sng_trm_trl_zro() */

nc_type /* O [enm] netCDF type */
nco_sng2typ /* [fnc] Convert user-supplied string to netCDF type enum */
(const char * const typ_sng) /* I [sng] String indicating type */
{
  /* Purpose: Convert user-supplied string to netCDF type */
  const char fnc_nm[]="nco_sng2typ()";

  /* Convert single letter code to type enum */
  switch(*typ_sng){
  case 'F':	
  case 'f': return (nc_type)NC_FLOAT; break;
  case 'D':	
  case 'd': return (nc_type)NC_DOUBLE; break;
  case 'C':	
  case 'c': return (nc_type)NC_CHAR; break;
  case 'B':	
  case 'b': return (nc_type)NC_BYTE; break;
  default: 
    /* Ambiguous single letters and extended matches must use full string comparisons */
    if(!strcasecmp(typ_sng,"float") || !strcasecmp(typ_sng,"float32") || !strcasecmp(typ_sng,"NC_FLOAT")) return (nc_type)NC_FLOAT; 
    else if(!strcasecmp(typ_sng,"l") || !strcasecmp(typ_sng,"i") || !strcasecmp(typ_sng,"int") || !strcasecmp(typ_sng,"int32") || !strcasecmp(typ_sng,"NC_INT")) return (nc_type)NC_INT; 
    else if(!strcasecmp(typ_sng,"s") || !strcasecmp(typ_sng,"short") || !strcasecmp(typ_sng,"int16") || !strcasecmp(typ_sng,"NC_SHORT")) return (nc_type)NC_SHORT; 
#ifdef ENABLE_NETCDF4
    else if(!strcasecmp(typ_sng,"ub") || !strcasecmp(typ_sng,"ubyte") || !strcasecmp(typ_sng,"uint8") || !strcasecmp(typ_sng,"NC_UBYTE")) return (nc_type)NC_UBYTE; 
    else if(!strcasecmp(typ_sng,"us") || !strcasecmp(typ_sng,"ushort") || !strcasecmp(typ_sng,"uint16") || !strcasecmp(typ_sng,"NC_USHORT")) return (nc_type)NC_USHORT; 
    else if(!strcasecmp(typ_sng,"u") || !strcasecmp(typ_sng,"ui") || !strcasecmp(typ_sng,"uint") || !strcasecmp(typ_sng,"uint32") || !strcasecmp(typ_sng,"ul") || !strcasecmp(typ_sng,"NC_UINT")) return (nc_type)NC_UINT; 
    else if(!strcasecmp(typ_sng,"ll") || !strcasecmp(typ_sng,"int64") || !strcasecmp(typ_sng,"NC_INT64")) return (nc_type)NC_INT64; 
    else if(!strcasecmp(typ_sng,"ull") || !strcasecmp(typ_sng,"uint64") || !strcasecmp(typ_sng,"NC_UINT64")) return (nc_type)NC_UINT64; 
    else if(!strcasecmp(typ_sng,"sng") || !strcasecmp(typ_sng,"string") || !strcasecmp(typ_sng,"NC_STRING")) return (nc_type)NC_STRING; 
    else{
      (void)fprintf(stderr,"NCO: ERROR `%s' is not a supported netCDF data type\n",typ_sng);
      (void)fprintf(stderr,"NCO: HINT: Valid data types are `c' = char, `f' = float, `d' = double,`s' = short, `i' = `l' = integer, `b' = byte");
      (void)fprintf(stderr,", `ub' = unsigned byte, `us' = unsigned short, `u' or `ui' or `ul' = unsigned int,`ll' or `int64' = 64-bit signed integer, `ull' or `uint64` = unsigned 64-bit integer, `sng' or `string' = string");
      (void)fprintf(stderr,"\n");
      nco_err_exit(0,fnc_nm);
    }
#endif /* ENABLE_NETCDF4 */
    break;
  } /* end switch */
  
  return (nc_type)NC_NAT;

} /* end nco_sng2typ() */

char * /* O [sng] Sanitized string */
nco_sng_sntz /* [fnc] Ensure input string contains only white-listed innocuous characters */
(char * const sng_drt) /* I [sng] String to sanitize */
{
  /* Purpose: Ensure input string contains only white-listed innocuous characters 

     Testing: ncra -D 73 -O ~/nco/data/8?.nc ~/foo.nc 2>&1 | grep sanitized

     Whitelist algorithm based on:
     https://wiki.sei.cmu.edu/confluence/display/c/STR02-C.+Sanitize+data+passed+to+complex+subsystems 

     Whitelist alternatives suggested at:
     https://github.com/nco/nco/issues/104

     Other resources:
     https://en.wikipedia.org/wiki/Filename#Reserved_characters_and_words

     NCO modifications to CMU default whitelist:
     20180214: White-list colon (WRF filenames sometimes have timestamps with colons, Windows drive labels have colons) 
     20180214: White-list space (Methinks some Windows people do have data files with spaces)
     20180214: White-list forward slash on UNIX, backslash on Windows (path separators) 
     20180222: White-list percent sign (NCO regression test uses, e.g., %tmp_fl_00)
     20180227: White-list forward slash on Windows so URLs are acceptable (http://...)
     20180323: White-list question mark and asterisk so shell-globbing works (in*.nc, in?.nc)
     20180401: Black-list question mark and asterisk again since shell-globbing now works
     20180420: White-list DAP-constraint characters that follow protocol indicators (http://,https://,dap4://)
     Crucial characters that are currently implicitly blacklisted (and could be transformed into underscores) are:
     "!#$^;|<>[](){},&*?<>" */
  static char wht_lst_dap[]="#=:[];|{}/<>";
  static char wht_lst[]="abcdefghijklmnopqrstuvwxyz"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "1234567890_-.@"
    " :%/"
#ifdef _MSC_VER
    "\\"
#endif /* !_MSC_VER */
    ;
  /* ": re-balance syntax highlighting */

  const char fnc_nm[]="nco_sng_sntz()"; /* [sng] Function name */

  const char dap4_url_sng[]="dap4://";
  const char http_url_sng[]="http://";
  const char https_url_sng[]="https://";
    
  char *usr_dta=sng_drt;
  char *cp=usr_dta; /* Cursor into string */
  nco_bool flg_blk_lst=False;
  
  const char *sng_end=usr_dta+strlen(usr_dta);

  if(nco_dbg_lvl_get() == 73) (void)fprintf(stderr,"%s: INFO %s reports input unsanitized user-input string \"%s\".\n",nco_prg_nm_get(),fnc_nm,usr_dta);

  for(cp+=strspn(cp,wht_lst);cp!=sng_end;cp+=strspn(cp,wht_lst)){
    /* Uncomment next two lines to sanitize unsafe character with an innocuous underscore
     *cp='_';
     if(nco_dbg_lvl_get() >= nco_dbg_io) (void)fprintf(stderr,"%s: DEBUG %s reports sanitized usr_dta = %s\n",nco_prg_nm_get(),fnc_nm,usr_dta); */
    
#ifdef ENABLE_DAP
    if((strstr(sng_drt,http_url_sng) == sng_drt) || (strstr(sng_drt,https_url_sng) == sng_drt) || (strstr(sng_drt,dap4_url_sng) == sng_drt)){
      /* Filename starts with "http://" or "https://" or "dap4://" so allow DAP whitelist */
      if(strchr(wht_lst_dap,*cp)){
	(void)fprintf(stderr,"%s: INFO %s reports allowing black-listed character \'%c\' from unsanitized user-input string \"%s\" because DAP is enabled and \'%c\' is a valid DAP parameter or constraint character\n",nco_prg_nm_get(),fnc_nm,*cp,usr_dta,*cp);
	break;
      } /* !strchr() */
    } /* !strstr */
#endif /* !ENABLE_DAP */

    flg_blk_lst=True;
    break;
  } /* !cp */

  if(flg_blk_lst){
    if(nco_dbg_lvl_get() == 73){
      /* Provide escape route so newly broken workflows will still work with -D 73
	 Eliminate this "back-door" after a few new versions of NCO, e.g., by version 4.7.5 */
      (void)fprintf(stderr,"%s: WARNING %s reports manual override of exit() due to black-listed character \'%c\' from unsanitized user-input string \"%s\".\n",nco_prg_nm_get(),fnc_nm,*cp,usr_dta);
    }else{
      (void)fprintf(stderr,"%s: ERROR %s reports character \'%c\' from unsanitized user-input string \"%s\" is not on whitelist of acceptable characters. For security purposes NCO restricts the set of characters appearing in user input, including filenames, to: \"%s\". NB: This restriction was first imposed in NCO 4.7.3 (February, 2018), and may cause breakage of older workflows. Please contact NCO if you have a real-world use-case that shows why the character \'%c\' should be white-listed. HINT: Re-try command after replacing transgressing characters with innocuous characters.\n",nco_prg_nm_get(),fnc_nm,*cp,usr_dta,wht_lst,*cp);
      nco_exit(EXIT_FAILURE);
    } /* !dbg */
  } /* !flg_blk_lst */

  return usr_dta;
} /* !nco_sng_sntz() */
