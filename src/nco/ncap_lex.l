%{ /* $Header$ -*-C-*- */
  /* Everything from here to closing brace is placed at top of lexer */

  /* Purpose: Token generator for ncap parser */

  /* Copyright (C) 1995--2013 Charlie Zender
     
   License: 3-Clause BSD License
   The full license text is at http://www.gnu.org/copyleft/gpl.html 
   and in the file nco/doc/LICENSE in the NCO source distribution.
   
   You are permitted to link NCO with the HDF, netCDF, OPeNDAP, and UDUnits
   libraries and to distribute the resulting executables under the terms 
   of the BSD, but in addition obeying the extra stipulations of the 
   HDF, netCDF, OPeNDAP, and UDUnits licenses.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
   See the 3-Clause BSD License for more details.
   
   The original author of this software, Charlie Zender, seeks to improve
   it with your suggestions, contributions, bug-reports, and patches.
   Please contact the NCO project at http://nco.sf.net or write to
   Charlie Zender
   Department of Earth System Science
   University of California, Irvine
   Irvine, CA 92697-3100 */

  /* Example lexers:
     /data/zender/ora/lexyacc/ch3-05.l
     GCC lexer is hard-coded in C for speed, but is easy to read:
     ../src/gcc-3.x.x/c-lex.c
     Unidata ${DATA}/tmp/netcdf-3.5.1-beta10/src/ncgen/ncgen.l */

  /* Usage:
     scp ~/nco/src/nco/ncap_lex.l goldhill.cgd.ucar.edu:nco/src/nco
     scp ~/nco/src/nco/ncap_lex.l krein.math.uci.edu:nco/src/nco

     flex --stdout ~/nco/src/nco/ncap_lex.l > ~/nco/src/nco/ncap_lex.c
     flex --header-file=${HOME}/nco/src/nco/ncap_lex.h --stdout ~/nco/src/nco/ncap_lex.l > ~/nco/src/nco/ncap_lex.c
     NB: flex with -Cf or -CF not -I if this scanner will not be interactive */

  /* Standard header files */
#include <assert.h> /* assert() debugging macro */
#include <limits.h> /* Integer representation, INT_MIN, INT_MAX... */
#include <math.h> /* sin cos cos sin 3.14159 */
#include <stdio.h> /* stderr, FILE, NULL, etc. */
#include <stdlib.h> /* atof, atoi, malloc, getopt */
#include <string.h> /* strdup */

  /* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions and C library */
#include "nco_netcdf.h" /* NCO wrappers for netCDF C library */

  /* Headers specific to this program */
#include "ncap_utl.h" /* netCDF arithmetic processor-specific definitions (symbol table, ...) */
  //#include "ncap_yacc.h" /* Symbol definitions from parser */

  /* We want yylex() errors to skip current line rather than stop execution
     We do this by provoking error in parser by returning unrecognized token
     This causes parser to read up to next ';'
     To avoid having two error messages we prefix error message to yyerror with '#'
     This causes yyerror to print current error and skip next error
     fxm: Hackish, but I see no other way */ 

/* NCO prototypes yylex() in three separate places---Ensure that all prototypes agree!
   The alternative, prototyping yylex() once in ncap.h, requires YY*STYPE (ncap_yacc.h) too
   That is unappealing since we want ncap.h segregable from parser and scanner dependencies
   (Asterisks inserted into token names in comments so that cpp does not expand token)
   1. Top of ncap_lex.l via YY*DECL token
      Define YY*DECL token so yylex() accepts re-entrant arguments (Flex p. 12, Bison p. 60) 
      Use YY*DECL on line following its definition to make prototype visible in ncap_lex.c
      This quiets a compiler warning
   2. Top of ncap_yacc.y via YY*DECL token
      Required to quiet compiler warning (see additional comments in ncap_yacc.y)
   3. In ncap_lex.l ncap_ntl_scn() declaration
      Required for ncap_ntl_scn() call to yylex() (additional comments in ncap_ntl_scn()) */

#define YY_DECL int yylex(YYSTYPE *lval_ptr,prs_sct *prs_arg)
  YY_DECL;

#if (defined(__GNUC__) || defined(__INTEL_COMPILER)) && !defined(__PATHCC__) && !defined(PGI_CC) && (defined(ENABLE_DEBUG_CUSTOM) || defined(ENABLE_OPTIMIZE_CUSTOM))
  /* CEWI fxm 20040110 Prototype functions that cause warnings with pedantic GCC compiling
     Portability of this entire section is questionable, so enclose in ifdef GCC
     We already assume lexer is Flex since AT&T lex does not understand <EOF> rules
     Thus prototyping following Flex-specific functions adds no additional assumptions
     See also 20040112 thread in comp.compilers */
  /* Implicit declaration warnings: -D_POSIX_SOURCE also avoids fileno warning */
  int fileno(FILE *); /* fixes: warning: implicit declaration of function `fileno', -D_POSIX_SOURCE also avoids this warning */
  /* int yy_flex_realloc(void *,size_t); *//* fixes: warning: implicit declaration of function `yy_flex_realloc'. Is this obsolete? Other versions of flex on Linux seem to want void * yy_flex_realloc(void *,size_t), which actually makes more sense given that it is a realloc() function */
  /* 20050318: Fedora Core 2 flex 2.5.4 gcc 3.4.2 This prototype causes 
     error: conflicting types for 'yy_flex_realloc' */
  /* Missing prototypes warnings: Triggered by -Wmissing-prototypes */
  FILE *yyget_in(void); /* fixes: warning: no previous prototype for `yyget_in' */
  FILE *yyget_out(void); /* fixes: warning: no previous prototype for `yyget_out' */
  char *yyget_text(void); /* fixes: warning: no previous prototype for `yyget_text' */
  int yyget_debug(void); /* fixes: warning: no previous prototype for `yyget_debug' */
  /* 20150115: MacOSX with clang and --enable-debug-custom fails on yyget_leng() prototype
     Change return type of yyget_leng() from int to yy_size_t */
  /* int yyget_leng(void); *//* fixes: warning: no previous prototype for `yyget_leng' */
  /* 20161116: Debian (e.g., glace) NCO build fails in ncap_lex.l due to conflicting types for `yyget_leng'. Debian wants yy_size_t yyget_leng(void); */
  /* 20170804: Fedora (e.g., skyglow) NCO build fails in ncap_lex.l due to conflicting types for `yyget_leng'. Fedora wants int yyget_leng(void); */
  /* 20180907: Daniel Neumann reports (https://sourceforge.net/p/nco/discussion/9829/thread/d3ecc062) that NCO build fails in ncap_lex.l due to conflicting types for `yyget_leng'. After thorough investigation he determined that
        "All variables and functions, which are decalred as yy_size_t in the flex-2.6.0-generated code, are of type int in the flex-2.6.4-generated code. No variable or function is declared as the type yy_size_t in the code generated by flex 2.6.4. Therefore, the definitions of variables and functions which were generated by flex (2.6.4) and which should be of type yy_size_t conflict with the hard-coded definitions copied from ncap_lex.l.
        The error is only thrown in combination with --enable-optimize-custom or --enable-debug-custom because only then the conflicting hard-coded definitions are considered by the compiler/preprocessor (#if ... && (defined(ENABLE_DEBUG_CUSTOM) || defined(ENABLE_OPTIMIZE_CUSTOM)) ...)." 
     Once all platforms upgrade to Flex >= 2.6.4 we can deprecate yy_size_t code
     However, MacOSX may never upgrade to newer Flex
     CZ's decision is to keep current behavior until breakage cannot be ignored and then sacrifice buildability on older Flex */
  yy_size_t yyget_leng(void); /* fixes: warning: no previous prototype for `yyget_leng' */
  int yyget_lineno(void); /* fixes: warning: no previous prototype for `yyget_lineno' */
  int yylex_destroy(void); /* fixes: warning: no previous prototype for `yylex_destroy' */
  void yyset_debug(int bdebug); /* fixes: warning: no previous prototype for `yyset_debug' */
  void yyset_in(FILE *in_str); /* fixes: warning: no previous prototype for `yyset_in' */
  void yyset_lineno(int line_number); /* fixes: warning: no previous prototype for `yyset_lineno' */
  void yyset_out(FILE *out_str); /* fixes: warning: no previous prototype for `yyset_out' */
#endif /* !__GNUC__ */

  /* Global variables */
  extern size_t ncap_ncl_dpt_crr; /* [nbr] Depth of current #include file (declared in ncap.c) */
  extern size_t *ncap_ln_nbr_crr; /* [cnt] Line number (declared in ncap.c) */
  extern char **ncap_fl_spt_glb; /* [fl] Script file (declared in ncap.c) */

  /* File scope variables */
  /* fxm: turn arbitrary size into pre-processor token */
  char ncap_err_sng[200]; /* [sng] Buffer for error string */

  /* Is lexer on left or right hand side of equals sign?
     LHS should contain only a variable or attribute
     In this case return variable/attribute name to parser
     If on RHS then return attribute value or variable structure
     Initialize to false, change to true if token is '=', false is token is ';' */
  static nco_bool RHS=False; /* [flg] Is lexer on RHS of '=' sign? */

  /* Headers necessary for INCLUDE state */
#define NCO_MAX_NCL_DPT 10
  YY_BUFFER_STATE ncl_stk[NCO_MAX_NCL_DPT]; /* Include stack */
%}

/* %x defines exclusive start states <stt>
   Once state <stt> is active, other rules do not match until initial state is restored
   NB: State names may not contain hyphens
   CCOMMENT: Handle C comments LMB92 p. 172, p. 43
   INCLUDE: Read in #include file (from example in flex manpage)
   DMN_LST: Dimension list
   LHS_SUBSCRIPT: Left Hand Side subscripts (currently LHS casting)
   RHS_SUBSCRIPT: Right Hand Side subscripts (currently not used) */
%x CCOMMENT DMN_LST INCLUDE LHS_SUBSCRIPT RHS_SUBSCRIPT

/* Definitions:  LMB92 p. 153
   Definitions are named regular expressions, e.g., DGT [0-9]
   Definitions enclosed in braces in rules section, e.g. {DGT}, are interpreted literally
   DGT [0-9] Digit
   LPH [A-Za-z_] Alphabetic character
   LPHDGT [A-Za-z0-9_] Alphanumeric character
   XPN [eE][+-]?[0-9]+ Real number Exponent */
DOT '.'
DGT [0-9]
LPH [A-Za-z_]
LPHDGT [A-Za-z0-9_]
XPN [eE][+-]?[0-9]+

/* Following sections after %% are lexer rules
   Each rule is regular expression followed by C-code for action
   If token matches regular expression then C-code is executed */
%%

\#include {
  /* INCLUDE is exclusive start state for obtaining #include filename */
  BEGIN INCLUDE;
} /* end include */
<INCLUDE>[ \t]* /* Eat whitespace */
<INCLUDE>[^ \t\n]+ {
  /* Get include file name */
  if(ncap_ncl_dpt_crr >= NCO_MAX_NCL_DPT){
    (void)fprintf(stderr,"%s: ERROR File %s nesting depth of %lu is too deep",nco_prg_nm_get(),yytext,(unsigned long)ncap_ncl_dpt_crr);
    nco_exit(EXIT_FAILURE);
  } /* endif */
  if(nco_dbg_lvl_get() > 1) (void)fprintf(stderr,"%s: INFO Will descend from file %s at nesting depth %lu to #include'd file %s at depth %lu\n",nco_prg_nm_get(),ncap_fl_spt_glb[ncap_ncl_dpt_crr],(unsigned long)ncap_ncl_dpt_crr,yytext,(unsigned long)ncap_ncl_dpt_crr+1UL);
  /* Put current input buffer on stack */
  ncl_stk[ncap_ncl_dpt_crr]=YY_CURRENT_BUFFER; 
  /* Increment nesting level */
  ncap_ncl_dpt_crr++;
  /* Allocate another entry in line counter array */
  ncap_ln_nbr_crr=(size_t *)nco_realloc(ncap_ln_nbr_crr,ncap_ncl_dpt_crr+1UL); 
  /* Initialize line number counter for new file */
  ncap_ln_nbr_crr[ncap_ncl_dpt_crr]=1UL;
  /* Store file name for diagnostics */
  ncap_fl_spt_glb=(char **)nco_realloc(ncap_fl_spt_glb,ncap_ncl_dpt_crr+1UL); 
  ncap_fl_spt_glb[ncap_ncl_dpt_crr]=(char *)strdup(yytext);
  if((yyin=fopen(ncap_fl_spt_glb[ncap_ncl_dpt_crr],"r")) == NULL){
    (void)fprintf(stderr,"%s: ERROR Unable to open script file %s\n",nco_prg_nm_get(),ncap_fl_spt_glb[ncap_ncl_dpt_crr]);
    nco_exit(EXIT_FAILURE);
  } /* endif error */

  /* Create new scanner input buffer for include file and switch to it
     Current start conditions remains active in new input buffer */
  nco_yy_switch_to_buffer(yy_create_buffer(yyin,YY_BUF_SIZE));
  /* Return to default state, known as INITIAL state or 0 state LMB92 p. 43 */    
  BEGIN INITIAL;
} /* end enter #include file */

<<EOF>> {
  /* NB: AT&T lex does not appear to have <EOF> rules */
  nco_bool NCO_SCRIPT_WITH_EOF_IS_TOPLEVEL=True;
  /* Emerge from #include file if nested, else exit lexer */
  if(ncap_ncl_dpt_crr > 0UL){
    /* If we are exiting a #include'd script, delete its buffers */
    NCO_SCRIPT_WITH_EOF_IS_TOPLEVEL=False; /* [flg] */
    if(nco_dbg_lvl_get() > 1) (void)fprintf(stderr,"%s: DEBUG Found EOF, emerging from file %s at nesting depth %lu to file %s at depth %lu\n",nco_prg_nm_get(),ncap_fl_spt_glb[ncap_ncl_dpt_crr],(unsigned long)ncap_ncl_dpt_crr,ncap_fl_spt_glb[ncap_ncl_dpt_crr-1],(unsigned long)ncap_ncl_dpt_crr-1UL);
    ncap_fl_spt_glb[ncap_ncl_dpt_crr]=(char *)nco_free(ncap_fl_spt_glb[ncap_ncl_dpt_crr]);
    /* Decrement depth of nesting */
    ncap_ncl_dpt_crr--; 
    /* De-allocate entry from line counter array */
    ncap_ln_nbr_crr=(size_t *)nco_realloc(ncap_ln_nbr_crr,ncap_ncl_dpt_crr+1UL); 
    /* De-allocate entry from script name array */
    ncap_fl_spt_glb=(char **)nco_realloc(ncap_fl_spt_glb,ncap_ncl_dpt_crr+1UL); 
  } /* endif #include'd script */
  if(NCO_SCRIPT_WITH_EOF_IS_TOPLEVEL){
    /* Emerged from zeroth level, exit lexer */
    yyterminate();
  }else{
    /* Reclaim storage used by buffer just exited */
    yy_delete_buffer(YY_CURRENT_BUFFER);
    /* Switch back to parent buffer */
    nco_yy_switch_to_buffer(ncl_stk[ncap_ncl_dpt_crr]);
  } /* endelse */
} /* end <EOF> */

"/*" {
   /* CCOMMENT is exclusive start state for C comments LMB92 p. 172
      When CCOMMENT state is active, only CCOMMENT rules will match 
      (hence an "exclusive start state" rather than "regular start state")
      Thus must (re)-define line number incrementing */
  BEGIN CCOMMENT;
}
<CCOMMENT>\n {
  /* Throw away comment text but increment line counter */
  ncap_ln_nbr_crr[ncap_ncl_dpt_crr]++; 
}
<CCOMMENT>. {
  /* Throw away everything except end comment text */
}
<CCOMMENT>"*/" {
  /* Return to default state, known as INITIAL state or 0 state LMB92 p. 43 */
  BEGIN INITIAL;
} /* end CCOMMENT start state rules */

"(/" {
  /* Dimension list */
  if(nco_dbg_lvl_get() > 0) (void)fprintf(stderr,"Entering DMN_LST with %s\n",yytext);

  BEGIN DMN_LST;

  return IGNORE; 
} /* End dimension list */
<DMN_LST>[ \t\n]* /* Eat whitespace */
<DMN_LST>"/)" {
  if(nco_dbg_lvl_get() > 0) (void)fprintf(stderr,"Exiting DMN_LST with %s\n",yytext);

  /* Return to default state, known as INITIAL state or 0 state LMB92 p. 43 */    
  BEGIN INITIAL;
} /* End dimension list */
<DMN_LST>[^\]\n]*[\]\n]/"/)" {
/*<DMN_LST>{LPH}{LPHDGT}*[ \t\n]*[,]?[ \t\n]* {*/
  /* Recognize dimension lists */
  if(nco_dbg_lvl_get() > 0) (void)fprintf(stderr,"Lexing dimension list %s\n",yytext);

  /* Valid subscripts must contain alphanumeric string */
  if(yyleng < 3) goto end_dmn_lst;
     
 end_dmn_lst: /* Errors encountered during LHS processing jump to here */
  /* Return to default state, known as INITIAL state or 0 state LMB92 p. 43 */    
  BEGIN INITIAL;
} /* end dimension list */

\n { 
  /* NB: Remaining rules are default state, also known as INITIAL state, or 0 state */
  /* Ignore new lines */
  ncap_ln_nbr_crr[ncap_ncl_dpt_crr]++;
}

[ \t]+ { 
  /* Eat spaces and tabs */
}

\/\/[^\n]* {
  /* Enable C++ comments */
}

\"[^"\n]*["\n] {
  /* Process quoted strings */
  char *bfr;
                
  bfr=(char*)nco_malloc(yyleng*sizeof(char));
  strcpy(bfr,&yytext[1]);
  bfr[yyleng-2]='\0';
  
  /* Replace C-language escape codes with actual byte values */
  (void)sng_ascii_trn(bfr);
  lval_ptr->sng=strdup(bfr);
  bfr=(char*)nco_free(bfr);
  if(nco_dbg_lvl_get() > 3){
    (void)fprintf(stderr,"Lexing string: %s\n",yytext);
    (void)fprintf(stderr,"Made   string: %s\n",lval_ptr->sng);
  } /* endif */
  return SNG;
} /* end quoted strings */

{DGT}*\.{DGT}*({XPN})?[LlDd]?|{DGT}*({XPN})[LlDd]? {
  /* double or long double */
  /* {DGT}*\.{DGT}*({XPN})?[LlDd]?|{DGT}*({XPN})[LlDd]? # Original rule and comments used until 20140527
     NB: Tempted to prepend lexer expressions for floats and doubles with [+-]? 
     so that unary plus/minus is handled in lexer rather than parser.
     However, this has unintended side-effects so let parser handle it for now */
  /* 20140527: Many years after writing the above line I regret not recording what those unintended side-effects were...
     [+-]?{DGT}*\.{DGT}*({XPN})?[LlDd]?|{DGT}*({XPN})[LlDd]? # Potential rule tested 20140527
     Not including sign signifiers [+-]? causes bug parsing --mask_condition expressions with negative msk_val
     Without the optional signage, ncap_ncwa_scn() ignores unary negative sign so msk_val is returned as abs(msk_val)
     Prepending [+-]? to number rules for doubles, floats etc fixes this particular problem but causes others
     Specifically, when lexer ingests signage tokens then parser treatment of subtraction fails, e.g.,
     ncap -O -D 4 -v -s 'zero=1-1.0' ~/nco/data/in.nc ~/foo.nc # Result is zero=1 
     Hence the "unintended side-effects" mentioned above includes, roughly, 
     that using [+-]? in lexer interferes with parser handling of unary signs */
  char *sng_cnv_rcd=NULL_CEWI; /* [sng] strtol()/strtoul() return code */
  if(nco_dbg_lvl_get() > 3) (void)fprintf(stderr,"Lexing a double %s\n",yytext); 
  lval_ptr->scv.val.d=strtod(yytext,&sng_cnv_rcd);
  if(*sng_cnv_rcd) nco_sng_cnv_err(yytext,"strtod",sng_cnv_rcd);
  lval_ptr->scv.type=NC_DOUBLE;
  return SCV;
} /* end double or long double */

{DGT}*\.{DGT}*({XPN})?[Ff]|{DGT}*({XPN})[Ff] {
  /* float */
  float float_tkn;
  if(nco_dbg_lvl_get() > 3) (void)fprintf(stderr,"Lexing a float %s\n",yytext); 
  if(sscanf((char *)yytext,"%e",&float_tkn) != 1){
    sprintf(ncap_err_sng,"#Bad float: %s",yytext);
    nco_yyerror(prs_arg,ncap_err_sng);
    return EPROVOKE;
  } /* endif */
  lval_ptr->scv.val.f=float_tkn;
  lval_ptr->scv.type=NC_FLOAT;
  return SCV;
} /* end float */

{DGT}+("LL"|"ll") {
  /* int64 */
  nco_int64 int64_tkn;
  if(nco_dbg_lvl_get() > 3) (void)fprintf(stderr,"Lexing an int64 %s\n",yytext); 
  if(sscanf((char *)yytext,"%lli",&int64_tkn) != 1){
    sprintf(ncap_err_sng,"Bad int64 constant: %s",(char *)yytext);
    nco_yyerror(prs_arg,ncap_err_sng);
  } /* endif err */
  lval_ptr->scv.val.i=int64_tkn;
  lval_ptr->scv.type=NC_INT64;
  return SCV;
} /* end int64 */

{DGT}+("ULL"|"ull") {
  /* uint64 */
  nco_uint64 uint64_tkn;
  if(nco_dbg_lvl_get() > 3) (void)fprintf(stderr,"Lexing a uint64 %s\n",yytext); 
  if(sscanf((char *)yytext,"%llu",&uint64_tkn) != 1){
    sprintf(ncap_err_sng,"Bad uint64 constant: %s",(char *)yytext);
    nco_yyerror(prs_arg,ncap_err_sng);
  } /* endif err */
  lval_ptr->scv.val.i=uint64_tkn;
  lval_ptr->scv.type=NC_UINT64;
  return SCV;
} /* end uint64 */

{DGT}+[lL]? {
  /* int */
  nco_int int_tkn;
  if(nco_dbg_lvl_get() > 3) (void)fprintf(stderr,"Lexing an int %s\n",yytext); 
  /* NB: Format depends on opaque type of nco_int
     Until 200911, nco_int was C-type long, and now nco_int is C-type int
     if(sscanf((char *)yytext,"%li",&int_tkn) != 1){ */
  if(sscanf((char *)yytext,"%i",&int_tkn) != 1){
    sprintf(ncap_err_sng,"Bad int constant: %s",(char *)yytext);
    nco_yyerror(prs_arg,ncap_err_sng);
  } /* endif err */
  lval_ptr->scv.val.i=int_tkn;
  lval_ptr->scv.type=NC_INT;
  return SCV;
} /* end int */

{DGT}+([uU]|"ul"|"UL") {
  /* uint */
  nco_uint uint_tkn;
  if(nco_dbg_lvl_get() > 3) (void)fprintf(stderr,"Lexing a uint %s\n",yytext); 
  if(sscanf((char *)yytext,"%u",&uint_tkn) != 1){
    sprintf(ncap_err_sng,"Bad uint constant: %s",(char *)yytext);
    nco_yyerror(prs_arg,ncap_err_sng);
  } /* endif err */
  lval_ptr->scv.val.ui=uint_tkn;
  lval_ptr->scv.type=NC_UINT;
  return SCV;
} /* end uint */

{DGT}+[sS] {
  /* short */
  nco_short short_tkn;
  if(nco_dbg_lvl_get() > 3) (void)fprintf(stderr,"Lexing a short %s\n",yytext); 	
  if(sscanf((char *)yytext,"%hi",&short_tkn) != 1){
    sprintf(ncap_err_sng,"Bad short constant: %s",(char *)yytext);
    nco_yyerror(prs_arg,ncap_err_sng);
  } /* endif err */
  lval_ptr->scv.val.s=short_tkn;
  lval_ptr->scv.type=NC_SHORT;
  return SCV;
} /* end short */

{DGT}+("us"|"US") {
  /* ushort */
  nco_ushort short_tkn;
  if(nco_dbg_lvl_get() > 3) (void)fprintf(stderr,"Lexing a ushort %s\n",yytext); 	
  if(sscanf((char *)yytext,"%hu",&short_tkn) != 1){
    sprintf(ncap_err_sng,"Bad short constant: %s",(char *)yytext);
    nco_yyerror(prs_arg,ncap_err_sng);
  } /* endif err */
  lval_ptr->scv.val.s=short_tkn;
  lval_ptr->scv.type=NC_USHORT;
  return SCV;
} /* end short */

{DGT}+[Bb] {
  /* byte */
  nco_byte byte_tkn;
  if(nco_dbg_lvl_get() > 3) (void)fprintf(stderr,"Lexing a byte %s\n",yytext); 
  if(sscanf((char *)yytext,"%hhi",&byte_tkn) != 1){
    sprintf(ncap_err_sng,"#Bad byte: %s",yytext);
    nco_yyerror(prs_arg,ncap_err_sng);
    return EPROVOKE;
  } /* endif err */
  lval_ptr->scv.val.b=byte_tkn;
  lval_ptr->scv.type=NC_BYTE;
  return SCV;                    
} /* end byte */

{DGT}+("UB"|"ub") {
  /* ubyte */
  nco_ubyte ubyte_tkn;
  if(nco_dbg_lvl_get() > 3) (void)fprintf(stderr,"Lexing a ubyte %s\n",yytext); 
  if(sscanf((char *)yytext,"%hhu",&ubyte_tkn) != 1){
    sprintf(ncap_err_sng,"#Bad byte: %s",yytext);
    nco_yyerror(prs_arg,ncap_err_sng);
    return EPROVOKE;
  } /* endif err */
  lval_ptr->scv.val.ub=ubyte_tkn;
  lval_ptr->scv.type=NC_UBYTE;
  return SCV;                    
} /* end ubyte */

foo1_zyx {return ABS;} /* Strategy for lexing control statements: 
1. Look for language control statements (if, while, do, for)
2. Look for NCO-defined intrinsic functions (abs, pack, unpack)
3. Look for mathematical intrinsic functions (sin, cos)
4. Look for NCO-defined named constants (M_PI, M_E) */
"if" {return IF;}
"print" {return PRINT;}

foo2_zyx {return ABS;} /* TOKEN whitespace (: Token followed any amount of whitespace followed by opening parenthesis signifies function name, not variable name.
This disambiguates variable and functional namespaces.

Dimension reducing functions (min,max,avg,ttl) need thought
Each function should allow specification of optional dimension list, e.g., 
foo=avg(var,(/lat,lon/)); should return mean T over lat,lon dimensions

Certain functions, however, have binary versions, e.g., 
foo=min(var_1,var_2); should return point-wise minimum values

Dimension lists require syntax like (/.../) to disambiguate meanings like
foo=min(lat,lat)

End result:
foo=min(lat) Returns minimum value of lat
foo=min(lat,(/lat/)) Returns minimum value of lat
foo=min(lat,lat) Returns copy of lat (i.e., pointwise minimum of lat and lat)
foo=min(var,(/dmn_1,dmn_2/)) Returns minimum of variable over dmn_1, dmn_2 dimensions */
abs/[ ]*\( {return ABS;}
atostr/[ ]*\( {return ATOSTR;}
avg/[ ]*\( {prs_arg->nco_op_typ=nco_op_avg;return RDC;}
min/[ ]*\( {prs_arg->nco_op_typ=nco_op_min;return RDC;}
max/[ ]*\( {prs_arg->nco_op_typ=nco_op_max;return RDC;}
ttl/[ ]*\( {prs_arg->nco_op_typ=nco_op_ttl;return RDC;}
pck/[ ]*\( {return PACK;}
pack/[ ]*\( {return PACK;}
pow/[ ]*\( {return POWER;}
upk/[ ]*\( {return UNPACK;}
unpack/[ ]*\( {return UNPACK;}
float/[ ]*\( { lval_ptr->cnv_type = NC_FLOAT;return CNV_TYPE;} 
double/[ ]*\( { lval_ptr->cnv_type = NC_DOUBLE;return CNV_TYPE;} 
long/[ ]*\( { lval_ptr->cnv_type = NC_INT;return CNV_TYPE;} 
int/[ ]*\( { lval_ptr->cnv_type = NC_INT;return CNV_TYPE;} 
short/[ ]*\( { lval_ptr->cnv_type = NC_SHORT;return CNV_TYPE;} 
ushort/[ ]*\( { lval_ptr->cnv_type = NC_USHORT;return CNV_TYPE;} 
uint/[ ]*\( { lval_ptr->cnv_type = NC_UINT;return CNV_TYPE;} 
int64/[ ]*\( { lval_ptr->cnv_type = NC_INT64;return CNV_TYPE;} 
uint64/[ ]*\( { lval_ptr->cnv_type = NC_UINT64;return CNV_TYPE;} 
ubyte/[ ]*\( { lval_ptr->cnv_type = NC_UBYTE;return CNV_TYPE;} 
byte/[ ]*\( { lval_ptr->cnv_type = NC_BYTE;return CNV_TYPE;} 
char/[ ]*\( { lval_ptr->cnv_type = NC_CHAR;return CNV_TYPE;} 

M_PI {return NAMED_CONSTANT;}

{LPH}{LPHDGT}*/[ ]*\( {
/* Compare input with mathematical function names in table and return
   pointer to structure containing name, double function, float function */
   int idx;
   for(idx=0;idx<prs_arg->sym_tbl_nbr;idx++)
     if(!strcmp(yytext,(prs_arg->sym_tbl[idx]->nm))){ 
        lval_ptr->sym=prs_arg->sym_tbl[idx];
        return FUNCTION;
     } /* endif */
   (void)sprintf(ncap_err_sng,"Warning: Unable to locate function %s. Perhaps %s is a variable?",yytext,yytext);
   (void)nco_yyerror(prs_arg,ncap_err_sng);       
   return EPROVOKE;             
} /* end functions */

{LPH}{LPHDGT}*@{LPH}{LPHDGT}* { 
  /* Recognize variable attributes, e.g., var_nm@att_nm */
  aed_sct *ptr_aed;
  char *amp_ptr;
  char att_nm[NC_MAX_NAME];
  char var_nm[NC_MAX_NAME];
  int rcd=NC_NOERR;
  int rcd_out;
  int var_id;
  long att_sz;
  nc_type type;
  ptr_unn val;
  size_t sng_lng;
  nco_bool blkp;
  var_sct var_lkp; /* for us when calling lookup vars on first pass only */
  
  if(nco_dbg_lvl_get() > 3) (void)fprintf(stderr,"Lexing attribute %s\n",yytext);
  amp_ptr=strchr(yytext,'@');
  sng_lng=amp_ptr-yytext;
  
  strncpy(var_nm,yytext,sng_lng);
  var_nm[sng_lng]='\0';
  
  strcpy(att_nm,++amp_ptr);
  
  var_lkp.nm=var_nm; 

  /* Assume global attribute when variable name is "global" */
  if(!strcmp(var_nm,"global")) var_id=NC_GLOBAL; else rcd=nco_inq_varid_flg(prs_arg->in_id,var_nm,&var_id);
  
  /* We are on RHS so return value of attribute (if available) to parser
     During initial scan, just return attributes on RHS 
  if(prs_arg->ntl_scn){
    if(!RHS){
      lval_ptr->aed.att_nm=(char *)strdup(att_nm);
      lval_ptr->aed.var_nm=(char *)strdup(var_nm);
      return OUT_ATT;
    }else{
      return IGNORE;
    } 
  } 
  */  

  if(RHS){
    /* First check if attribute has already been saved in symbol table
       If so then obtain and return value, else check input file
       If attribute is present obtain and return value from disk else return NC_BYTE of value 0
       If attribute is NC_CHAR then return SNG type
       Other types (NC_FLOAT, NC_DOUBLE, NC_BYTE, NC_SHORT) return SCV */
    ptr_aed=ncap_aed_lookup(var_nm,att_nm,prs_arg,False);
    if(ptr_aed){
      if(ptr_aed->type == NC_CHAR){
	(void)cast_void_nctype(ptr_aed->type,&ptr_aed->val);
	lval_ptr->sng=(char *)nco_malloc((ptr_aed->sz+1)*nco_typ_lng(NC_CHAR));
	strncpy(lval_ptr->sng,(char *)(ptr_aed->val.cp),(size_t)(ptr_aed->sz));
	lval_ptr->sng[ptr_aed->sz]='\0';
	return SNG;
      }else{
	lval_ptr->scv=ptr_unn_2_scv(ptr_aed->type,ptr_aed->val);
	return SCV;  
      } /* end else */        
    } /* end if */
    if(rcd == NC_NOERR){  
      rcd=nco_inq_att_flg(prs_arg->in_id,var_id,att_nm,&type,&att_sz);
      if (rcd == NC_ENOTATT){
	(void)sprintf(ncap_err_sng,"Warning: Unable to locate RHS attribute %s of variable %s with var ID = %d in file %s. Returning 0-byte.",att_nm,var_nm,var_id,prs_arg->fl_in);
	(void)nco_yyerror(prs_arg,ncap_err_sng);
	lval_ptr->scv.val.b=0;
	lval_ptr->scv.type=NC_BYTE;
	return SCV;
      } /* end if */
      if(rcd == NC_NOERR){ 
	val.vp=(void *)nco_malloc(att_sz*nco_typ_lng(type));
	rcd=nco_get_att(prs_arg->in_id,var_id,att_nm,val.vp,type);
	if(type == NC_CHAR){
	  (void)cast_void_nctype(type,&val);
	  lval_ptr->sng=(char *)nco_malloc((att_sz+1)*sizeof(char));
	  strncpy(lval_ptr->sng,(char *)val.cp,(size_t)att_sz);
	  lval_ptr->sng[att_sz]='\0';;
          val.vp=(void*)nco_free(val.vp); 
	  return SNG;
	}else{
	  lval_ptr->scv=ptr_unn_2_scv(type,val);
	  val.vp=(void*)nco_free(val.vp); 
	  return SCV;   
	} /* end else */ 
      } /* end if */
    }else{ /* ...else rcd reported an error */
      /* Attribute is not in table or on disk so return 0-byte  */
      (void)sprintf(ncap_err_sng,"Warning: Unable to locate RHS attribute %s of variable %s with var ID = %d in file %s. Returning 0-byte.",att_nm,var_nm,var_id,prs_arg->fl_in);
      (void)nco_yyerror(prs_arg,ncap_err_sng);
      lval_ptr->scv.val.b=0;
      lval_ptr->scv.type=NC_BYTE;
      return SCV;
    } /* end if */
  }/* end if RHS */
  
  /* We are on LHS so simply save information for later processing by parser
     Atrribute is valid if its associated variable is in input or output file */
  if(!RHS){

    if(prs_arg->ntl_scn) 
      blkp= ( ncap_var_lookup(&var_lkp,prs_arg,False) !=NULL) ;
    else blkp=False;

    rcd_out=nco_inq_varid_flg(prs_arg->out_id,var_nm,&var_id);

    if(rcd == NC_NOERR || rcd_out == NC_NOERR || blkp ){
      lval_ptr->aed.att_nm=(char *)strdup(att_nm);
      lval_ptr->aed.var_nm=(char *)strdup(var_nm);
      return OUT_ATT;
    }else{
      (void)sprintf(ncap_err_sng,"#Warning: unable to locate variable %s. So cannot create attribute %s",var_nm,yytext);
      (void)nco_yyerror(prs_arg,ncap_err_sng);
      return EPROVOKE; 
    } /* end else */
  } /* end if */
  
} /* End scalar values. Phew! */

{LPH}{LPHDGT}*/\[ {
  /* Recognize variables with subscripts */
  if(nco_dbg_lvl_get() > 3) (void)fprintf(stderr,"Lexing variable with subscripts %s\n",yytext);
  if(RHS){
    lval_ptr->var_nm_RHS=(char *)strdup(yytext);
    BEGIN RHS_SUBSCRIPT;
    return VAR;
  }else{ 
    lval_ptr->var_nm_LHS=(char *)strdup(yytext);
    BEGIN LHS_SUBSCRIPT;
    return OUT_VAR;
  } /* end else */
} /* end variables with subscripts */

{LPH}{LPHDGT}* {
  /* Recognize plain variables */
  if(nco_dbg_lvl_get() > 3) (void)fprintf(stderr,"Lexing variable %s\n",yytext);
  if(RHS){
    /* fxm mmr: memory leak since var_nm_RHS is never free()'d */
    lval_ptr->var_nm_RHS=(char *)strdup(yytext); 
    return VAR;
  }else{ 
    lval_ptr->var_nm_LHS=(char *)strdup(yytext);
    return OUT_VAR;
  } /* end else */
} /* end variables without subscripts */

<LHS_SUBSCRIPT>\[[^\]\n]*[\]\n] {
  /* fxm: allow whitespace in dimension list */
  /* Recognize subscripted (hyperslabbed) variables
     Hyperslabbed variables for now indicated with square brackets
     Using square brackets so development does not break functions
     Left context is described in LMB92 p. 152
     
     Subscript syntax is taken from Fortran9X specification:
     x[dmn1,dmn2,...,dmnN]
     x[a:b,c:,:d,e] */
  const char * const sbs_dlm=","; /* [sng] Subscript delimiter */
  static const char * const tpl_nm="Internally generated template";
  
  char **sbs_lst; /* [sng] Array of dimension subscripts */
  char *sbs_sng; /* [sng] Subscript */
  
  int dmn_nbr; /* [nbr] Number of dimensions */
  int idx; /* [idx] Counter */
  int jdx; /* [idx] Counter */
  
  double val=1.0; /* [frc] Value of template */
  
  var_sct *var; /* [sct] Variable */
  
  dmn_sct **dmn; /* [dmn] Dimension structure list */
  dmn_sct *dmn_item;
  dmn_sct **dmn_new;

  /* Valid subscripts must contain alphanumeric string */
  if(nco_dbg_lvl_get() > 2) (void)fprintf(stderr,"Interpreting %s as LHS_SUBSCRIPT\n",yytext);
  if(yyleng < 3) goto end_LHS_sbs;

  /* if(prs_arg->ntl_scn) goto end_LHS_sbs; */  
  /* Copy token into user memory and turn into list of dimension names */
  sbs_sng=(char *)nco_malloc(yyleng*sizeof(char));
  strcpy(sbs_sng,&yytext[1]);
  sbs_sng[yyleng-2]='\0';  
  sbs_lst=nco_lst_prs_2D(sbs_sng,sbs_dlm,&dmn_nbr);

  dmn=(dmn_sct **)nco_malloc(dmn_nbr*sizeof(dmn_sct *));
  (void)nco_redef(prs_arg->out_id);
  for(idx=0;idx<dmn_nbr;idx++){
    /* first seach for dimension in psr_arg->dmn_out */
    for(jdx=0;jdx<*(prs_arg->nbr_dmn_out);jdx++){
      dmn_item=(*(prs_arg->dmn_out))[jdx];
      if(!strcmp(sbs_lst[idx],dmn_item->nm)){
      dmn[idx]=dmn_item;
      break;
      } /* endif */
    } /* end loop over jdx */
   if(jdx != *(prs_arg->nbr_dmn_out)) continue;   
    /* then try seach for dimension in prs_arg->dmn_in */
   for (jdx=0;jdx<prs_arg->nbr_dmn_in;jdx++){
     dmn_item = prs_arg->dmn_in[jdx];
     /* If found add dimension to dmn_out and dmn */
     if(!strcmp(sbs_lst[idx],dmn_item->nm)){
       /* BEWARE -- nco_dmn_out_grow() increments *(prs_arg->nbr_dmn_out) */
       dmn_new = nco_dmn_out_grow(prs_arg); 
       *dmn_new=nco_dmn_dpl(dmn_item);
       (void)nco_dmn_xrf(*dmn_new,dmn_item);
       /* write found  dimension to output  */
       (void)nco_dmn_dfn(prs_arg->fl_out,prs_arg->out_id,dmn_new,1);
       dmn[idx]=*dmn_new;
       break;
     } /* endif */
   } /* end loop over jdx */

   if(jdx != prs_arg->nbr_dmn_in) continue;
   /* Exit action if dimension not found */
   (void)sprintf(ncap_err_sng,"Warning: Unrecognized dimension \"%s\" in LHS subscripts",sbs_lst[idx]);
   (void)nco_yyerror(prs_arg,ncap_err_sng);
   (void)nco_enddef(prs_arg->out_id);
   goto end_LHS_sbs;                     
  } /* endif */
  (void)nco_enddef(prs_arg->out_id);

  /* Check that un-limited dimension is first dimension */
  for(idx=1;idx<dmn_nbr;idx++)
    if(dmn[idx]->is_rec_dmn){
      (void)sprintf(ncap_err_sng,"Warning:\"%s\" is the record dimension. It can only be the first dimension in a list",dmn[idx]->nm);
      (void)nco_yyerror(prs_arg,ncap_err_sng);
      goto end_LHS_sbs;                     
    } /* endif */

  /* Create template variable to cast all RHS expressions */
  var=(var_sct *)nco_malloc(sizeof(var_sct));
  
  /* Set defaults */
  (void)var_dfl_set(var); /* [fnc] Set defaults for each member of variable structure */
  /* Overwrite with LHS information */
  /* fxm mmr: memory leak with var->nm */
  var->nm=(char *)strdup(tpl_nm);
  var->type=NC_DOUBLE;
  var->nbr_dim=dmn_nbr;
  var->dim=dmn;
  /* Allocate space for dimension structures */
  if(var->nbr_dim > 0) var->dmn_id=(int *)nco_malloc(var->nbr_dim*sizeof(int)); else var->dmn_id=(int *)NULL;
  if(var->nbr_dim > 0) var->cnt=(long *)nco_malloc(var->nbr_dim*sizeof(long)); else var->cnt=(long *)NULL;
  if(var->nbr_dim > 0) var->srt=(long *)nco_malloc(var->nbr_dim*sizeof(long)); else var->srt=(long *)NULL;
  if(var->nbr_dim > 0) var->end=(long *)nco_malloc(var->nbr_dim*sizeof(long)); else var->end=(long *)NULL;
  if(var->nbr_dim > 0) var->srd=(long *)nco_malloc(var->nbr_dim*sizeof(long)); else var->srd=(long *)NULL;
  
  /* Defensive programming */
  var->sz=1L; 
  /* Attach LHS dimensions to variable */
  for(idx=0;idx<dmn_nbr;idx++){
    var->dim[idx]=dmn[idx];
    var->dmn_id[idx]=dmn[idx]->id;
    var->cnt[idx]=dmn[idx]->cnt;
    var->srt[idx]=dmn[idx]->srt;
    var->end[idx]=dmn[idx]->end;
    var->srd[idx]=dmn[idx]->srd;
    var->sz*=var->cnt[idx];
  } /* end loop over dim */
  
  /* Do not initialize val in initial scan */
  if(prs_arg->ntl_scn) {
    var->val.vp=(void*)NULL;
    goto end_var;
  }
  
  /* Allocate space for variable values 
     fxm: more efficient and safer to use nco_calloc() and not fill with values? */
  if((var->val.vp=(void *)nco_malloc_flg(var->sz*nco_typ_lng(var->type))) == NULL){
    (void)fprintf(stderr,"%s: ERROR Unable to malloc() %ld*%lu bytes for var_LHS() in lexer\n",nco_prg_nm_get(),var->sz,(unsigned long)nco_typ_lng(var->type));
    nco_exit(EXIT_FAILURE); 
  } /* end if */
  
  /* Copy arbitrary value into variable 
     Placing a uniform value in variable should be unnecessary since variable
     is intended for use solely as dimensional template for nco_var_cnf_dmn() 
     Nevertheless, copy 1.0 into value for safety */
  { /* Change scope to define convenience variables which reduce de-referencing */
    long var_sz; /* [nbr] Number of elements in variable */
    size_t var_typ_sz; /* [B] Size of single element of variable */
    char *var_val_cp; /* [ptr] Pointer to values */
    
    var_sz=var->sz; /* [nbr] Number of elements in variable */
    var_typ_sz=nco_typ_lng(var->type);
    var_val_cp=(char *)var->val.vp;
    for(idx=0;idx<var_sz;idx++) (void)memcpy(var_val_cp+idx,(void *)(&val),var_typ_sz);
  } /* end scope */
 end_var:

  /* Link variable into yylex() argument for use in parser */
  prs_arg->var_LHS=var;
  
  if(nco_dbg_lvl_get() > 2) (void)fprintf(stderr,"%s: Lexer creating LHS cast template: Template var->nm %s, var->nbr_dim %d, var->sz %li\n",nco_prg_nm_get(),prs_arg->var_LHS->nm,prs_arg->var_LHS->nbr_dim,prs_arg->var_LHS->sz);
  
  if(0){ /* Remove some compiler warning messages CEWI */
    /* Following two statements remove "defined but not used" messages in ncap_lex.c (lex.yy.c) */
    (void)yyunput((int)0,(char *)NULL);
    /*    (void)yy_flex_realloc((void *)NULL,(size_t)NULL);*/
    /* Remove warnings which only occur on SGI IRIX cc */
    yy_full_match=yy_full_match+0;
    yy_full_lp=yy_full_lp+0;
    yy_full_state=yy_full_state+0;
  } /* endif False */
  
  /* Free dimension list memory */
  (void)nco_free(sbs_sng); 
  /* char list */
  if(dmn_nbr > 0) sbs_lst=nco_sng_lst_free(sbs_lst,dmn_nbr);
   
 end_LHS_sbs: /* Errors encountered during LHS processing jump to here */
  /* Return to default state, known as INITIAL state or 0 state LMB92 p. 43 */    
  BEGIN INITIAL;
} /* end LHS subscripted variables */

<RHS_SUBSCRIPT>\[[^\]\n]*[\]\n] {
  /* RHS_SUBSCRIPT */
  (void)sprintf(ncap_err_sng,"Warning RHS subscripts are not yet implemented");
  (void)nco_yyerror(prs_arg,ncap_err_sng);
  /* Return to default state, known as INITIAL state or 0 state LMB92 p. 43 */    
  BEGIN INITIAL;
} /* end RHS_SUBSCRIPT */

"&&" { /* And */
  return AND;
} /* end && */
"||" { /* Or */
  return OR;
} /* end || */
"==" { /* Comparison equals */
  lval_ptr->nco_rlt_opr=nco_op_eq;
  return COMPARISON;
} /* end == */
"!=" { /* Comparison not equals */
  lval_ptr->nco_rlt_opr=nco_op_ne;
  return COMPARISON;
} /* end != */
"<=" { /* Comparison less than or equal to */
  lval_ptr->nco_rlt_opr=nco_op_le;
  return COMPARISON;
} /* end <= */
">=" { /* Comparison greater than or equal to */
  lval_ptr->nco_rlt_opr=nco_op_ge;
  return COMPARISON;
} /* end >= */
"<" { /* Comparison less than */
  lval_ptr->nco_rlt_opr=nco_op_lt;
  return COMPARISON;
} /* end < */
">" { /* Comparison greater than */
  lval_ptr->nco_rlt_opr=nco_op_gt;
  return COMPARISON;
} /* end > */

= {
  /* Equals assignment */
  RHS=True; 
  return yytext[0];
} /* end equals */

; { 
  /* semi-colon indicates end-of-statement */
  RHS=False;
  return yytext[0]; 
} /* end semi-colon */

. {
  /* Everything not parsed by above falls through to here */
  if(nco_dbg_lvl_get() > 5) (void)fprintf(stderr,"%s: Following token reached last lexer rule: %s\n",nco_prg_nm_get(),yytext);
  return yytext[0];
} /* end trickle down */

%%

/* Begin user functions section */
int /* fxm: 20060217 Change to int from bool to work with g++ */
yywrap(void)
{
  /* Purpose: Routine to replace library-defined yywrap() 
     yywrap() is called when YY_INPUT returns EOF, e.g., at end of script
     Returning false (zero) means there is more to scan, and input has
     been redirected to new source, e.g., yyin points to new file. 
     Returning true (non-zero) means scanning is complete, so scanner terminates and returns 0 to calling function
     Default is to return true (one) when called---this terminates scanner */
  if(nco_dbg_lvl_get() >= 3) (void)fprintf(stderr,"%s: DEBUG yywrap() called with ncap_ncl_dpt_crr = %lu\n",nco_prg_nm_get(),(unsigned long)ncap_ncl_dpt_crr);
  /* comp.compilers thread 20020523 yywrap() should always return 1, even when
     scanning a #include'd file
     return (ncap_ncl_dpt_crr <= 1) ? 1 : 0; */
  return 1;
} /* end yywrap() */


int /* O [enm] Return code */ 
ncap_ncwa_scn /* [fnc] Scan command script, construct I/O lists */
(prs_sct * const prs_arg, /* I/O [sct] Global information required in parser routines */
 const char * const msk_sng, /* I [sng] User-specified script */
 char **msk_nm, /* O [sng] Masking variable name */
 double *msk_val, /* O [frc] Masking value */ 
 int *op_typ_rlt) /* O [enm] Relational operator type */ 
{   
  /* Get YYSTYPE prior to calling scanner */
  /*
    #include "ncap_yacc.h" 
    extern int yylex(YYSTYPE *lval_ptr,prs_sct *prs_arg); 
    extern int yy_scan_string(const char *); */
  
  nco_bool scv_dfn=False;
  nco_bool minus_dfn=False;
  int op_typ=-1;
  int rcd=NCO_NOERR;
  char *fnl_sng;
  char *msk_nm_LHS=NULL;
  scv_sct scv_out;
  
  int tkn_crr=-1; /* [tkn] Current token, must initialize to non-zero value */
  
  YYSTYPE lval; /* [tkn] Token */
  
  fnl_sng=(char *)nco_malloc((strlen(msk_sng)+3UL)*sizeof(char));
  
  (void)strcpy(fnl_sng,msk_sng);
  (void)strcat(fnl_sng,";\n");
  
  yy_scan_string(fnl_sng);
  
  /* While there are more tokens... */
  while(tkn_crr != 0){
    /* ...obtain next token from lexer... */
    tkn_crr=yylex(&lval,prs_arg);
    
    /* ...determine which variables and attributes exist in input file... */
    switch(tkn_crr){
    case OUT_VAR:
      msk_nm_LHS=strdup(lval.var_nm_LHS);
      break;
    case COMPARISON:
      op_typ=lval.nco_rlt_opr;
      break;
    case '-':
      minus_dfn=True;
      break; 
    case SCV:	
      scv_out=lval.scv;
      if (minus_dfn) (void)ncap_scv_minus(&scv_out); 
      scv_dfn=True;
      break;
    default: 
      /* Tokens that fall through are perfectly safe and normal
	 Tokens values 1--256 are ASCII characters, values > 256 defined in yy.tab.h */
      if(nco_dbg_lvl_get() >= 5) (void)fprintf(stderr,"%s: DEBUG Token type %d not handled in ncap_ntl_scn()\n",nco_prg_nm_get(),tkn_crr);
      
      /* Assume single '=' means '==' if op_typ is not defined */
      if(tkn_crr == '=' && op_typ == -1) op_typ=nco_op_eq;
      break;
    } /* end switch */
  } /* end while */
  
  /* Check that all three required items for an ncwa mask_condition have been scanned */
  if(msk_nm_LHS == (char *)NULL){
    (void)fprintf(stderr,"%s: Mask string (%s) does not contain valid variable name\n",nco_prg_nm_get(),msk_sng);
    rcd=NCO_ERR;
  }else{
    *msk_nm=msk_nm_LHS;
  } /* endif */
  if(op_typ == -1){
    (void)fprintf(stderr,"%s: Mask string (%s) does not contain valid comparison operator\n",nco_prg_nm_get(),msk_sng);
    rcd=NCO_ERR;
  }else{
    *op_typ_rlt=op_typ;
  } /* endif */
  if(scv_dfn == False){
    (void)fprintf(stderr,"%s: Mask string (%s) does not contain valid number\n",nco_prg_nm_get(),msk_sng);
    rcd=NCO_ERR;
  }else{ 
    (void)nco_scv_cnf_typ((nc_type)NC_DOUBLE,&scv_out);
    *msk_val=scv_out.val.d;
  } /* endif */
  
  return rcd;
  
} /* end ncap_ncwa_scn() */






