/* $Header$ */

/* Purpose: String utilities */

/* Copyright (C) 1995--2016 Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   GNU General Public License (GPL) Version 3 with exceptions described in the LICENSE file */

#include "nco_sng_utl.h" /* String utilities */

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
#endif /* !NEED_STRCASECMP */

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
} /* end chr2sng_cdl(0 */

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
} /* end chr2sng_xml(0 */

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
      (void)memmove(backslash_ptr+1,backslash_ptr+2,(strlen(backslash_ptr+2)+1)*sizeof(char));
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

kvm_sct /* O [kvm_sct] key-value pair*/
nco_sng2kvm /* [fnc] convert a string to key-value pair */
(const char *args) /* I [sng] input string argument with an equal sign connecting the key & value */
{
/*Implementation: parsing the args so they can be sent to a kvm (fake kvm here)
 * as a key-value pair.
 *
 * Example 1: ... --gaa a=1 ... should be exported as kvm.key = a; kvm.value = 1
 * Example 2: ... --gaa "a;b;c"=1 should be exported as kvm[0].key="a", kvm[1].key="b", kvm[2].key="c"
 *          and kvm[@] = 1 (the ";" will be parsed by caller). 
 *
 * IMPORTANT: Remember to free fake_kvm after use string_to_kvm.*/
  int arg_index = 0;
  kvm_sct kvm;

  kvm.val = NULL;

  for(char* char_token = strtok((char*)args, "="); char_token; char_token = strtok(NULL, "=")){
    //Use memcpy because strdup is not a standard C lib func and memcpy is faster than strcpy (little bit).
    char_token = nco_sng_strip(char_token);

    if(arg_index == 0){

      kvm.key = (char*)malloc(strlen(char_token) + 1);
      if(kvm.key){memcpy(kvm.key, char_token, strlen(char_token) + 1);}

    }else if(arg_index == 1){

      kvm.val = (char*)malloc(strlen(char_token) + 1);
      if(kvm.val){memcpy(kvm.val, char_token, strlen(char_token) + 1);}

    }else{break;} //end if
    //To get the next token.
    arg_index ++;
  } //end of loop

  //If malloc cannot alloc sufficient memory, either key or value would be NULL; print error message and not quit.
  if(!kvm.key || !kvm.val){

    perror("Error: system does not have sufficient memory.\n");
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

#ifndef NCO_STRING_SPLIT_
#define NCO_STRING_SPLIT_

char** /* O [pointer to sngs] group of splitted sngs*/
nco_string_split /* [fnc] split the string by delimiter */
(const char *restrict source, /* I [sng] the source string */
const char* delimiter) /* I [char] the delimiter*/
{
    /* Use to split the string into a double character pointer, which each sencondary pointer represents
     * the string after splitting.
     * Example: a, b=1 will be split into *<a> = "a" *<b> = "b=1" with a delimiter of "," 
     * Remember to free after calling this function. */
    char** final = NULL, *temp = strdup(source);
    size_t counter = nco_count_blocks(source, (char*)delimiter), index = 0;    

    if(!strstr(temp, delimiter)){ //special case for one single argument

      final    = (char**)malloc(sizeof(char*));

      final[0] = temp;

      return final;
    }

    final = (char**)malloc(sizeof(char*) * counter);

    if(final){

        for(char *token = strtok(temp, delimiter); token; token = strtok(NULL, delimiter)){

            final[index ++] = strdup(token);
        } //end for

        free(temp);

    }else{return NULL;} //end if

    return final;
}

#endif

#ifndef NCO_INPUT_CHECK_
#define NCO_INPUT_CHECK_

int /* O [int] the boolean for the checking result */
nco_input_check /* [fnc] check whether the input is legal and give feedback accordingly. */
(const char *restrict args) /* O [sng] input arguments */
{
    /* Use to check the syntax for the arguments.
     * If the return value is false (which means the input value is illegal) the parser will terminate the program. */
    if(!strstr(args, "=")){ //If no equal sign in arguments
        
        printf("\033[0;31mIn %s\n", args);

        perror("Formatting Error: No equal sign detected \033[0m\n");

        return 0;
    } //endif
    if(strstr(args, "=") == args){ //If equal sign is in the very beginning of the arguments (no key)
        
        printf("\033[0;31mIn %s\n", args);

        perror("Formatting Error: No key in key-value pair.\033[0m\n"); 

        return 0;
    } //endif
    if(strstr(args, "=") == args + strlen(args) - 1){ //If equal sign is in the very end of the arguments
        
        printf("\033[0;31mIn %s\n", args);

        perror("Formatting Error: No value in key-value pair.\033[0m\n"); 

        return 0;
    } //endif
    return 1;

}
#endif

int // O [int] the number of string blocks if will be split with delimiter
nco_count_blocks // [fnc] Check number of string blocks if will be split with delimiter
(const char* args, // I [sng] the string which is going to be split
char* delimiter) // I [sng] the delimiter
{
  int i = 0;

  const char *pch = strchr(args, *(delimiter));

  while (pch != NULL) {

    i++;

    pch = strchr(pch + 1, *(delimiter));
  }
  return i + 1;
}

void 
nco_sng_lst_free_void /* [fnc] free() string list */
(char **restrict sng_lst, /* I/O [sng] String list to free() */
 const int sng_nbr) /* I [int] Number of strings in list */
{
    /* Use to free the double character pointer, and set the pointer to NULL */
    for(int i=0; i < sng_nbr; i++){free(sng_lst[i]);}

    free(sng_lst);

    sng_lst = NULL;
}

#ifndef NCO_ARG_MLT_PRS_
#define NCO_ARG_MLT_PRS_

kvm_sct* /* O [kvm_sct] the pointer to the first kvm structure */
nco_arg_mlt_prs /* [fnc] main parser, split the string and assign to kvm structure */
(const char *restrict args) /* I [sng] input string */
{
    /* Main parser for the argument. This will split the whole argument into key value pair and send to sng2kvm*/
    if(!args){

        nco_exit(EXIT_FAILURE);
    }

    char **separate_args = nco_string_split(args, "#");
    size_t counter = nco_count_blocks(args, "#") * nco_count_blocks(args, ","); //Max number of kvm structure in this argument

    for(int i=0; i < nco_count_blocks(args, "#"); i++){

        if(!nco_input_check(separate_args[i])){

            nco_exit(EXIT_FAILURE);
        }//end if

    }//end loop

    kvm_sct* kvm_set = (kvm_sct*)malloc(sizeof(kvm_sct) * (counter + 1)); /* kvm array intended to be returned */
    counter          = 0;

    for(int i=0; i < nco_count_blocks(args, "#"); i++){
        
        char *value = strdup(strstr(separate_args[i], "="));

        char **individual_args = nco_string_split(separate_args[i], ",");

        for(int j=0; j < nco_count_blocks(separate_args[i], ","); j++){

            char* temp_value = strdup(individual_args[j]);
            if(!strstr(temp_value, "=")){

                temp_value = strcat(temp_value, value);
            }//end if

            kvm_sct kvm_object = nco_sng2kvm(temp_value);

            kvm_set[counter++] = kvm_object;

            free(temp_value);
        }//end inner loop

        nco_sng_lst_free_void(individual_args, nco_count_blocks(separate_args[i], ","));
        free(value);

    }//end outer loop
    nco_sng_lst_free_void(separate_args, nco_count_blocks(args, "#"));

    kvm_set[counter].key = NULL; //Add an ending flag for kvm array.

    return kvm_set;
}

#endif

#ifndef NCO_JOIN_SNG_
#define NCO_JOIN_SNG_

char * /* O [sng] Joined strings */
nco_join_sng /* [fnc] Join strings with delimiter */
(const char **restrict sng_lst, /* I [sng] List of strings being connected */
 const char *dlm_sng, /* I [sng] Delimiter string */
 const int sng_nbr) /* I [int] Number of strings */
{
    if(sng_nbr == 1) {return strdup(sng_lst[0]);}

    size_t word_length = 0;
    size_t copy_counter = 0;

    for(size_t i=0; i < sng_nbr; i++){

        word_length += strlen(sng_lst[i]) + 1;
    }

    char *final_string = (char*)malloc(word_length + 1);

    for(int i=0; i < sng_nbr; i++){

        size_t temp_length = strlen(sng_lst[i]);

        memcpy(final_string + copy_counter, sng_lst[i], temp_length);

        if(i < sng_nbr - 1){ // If it is not the last block of string
            memcpy(final_string + copy_counter + temp_length, dlm_sng, 1);
        }

        copy_counter += (temp_length + 1);
    }
    return final_string;

}

#endif
