%{ /* $Header: /data/zender/nco_20150216/nco/src/nco/ncap_yacc.y,v 1.57 2010-09-19 01:01:50 zender Exp $ -*-C-*- */
  
/* Begin C declarations section */
  
/* Purpose: Grammar parser for ncap */
  
/* Copyright (C) 1995--2010 Charlie Zender
     
   You may copy, distribute, and/or modify this software under the terms of the GNU General Public License (GPL) Version 2
   The full license text is at http://www.gnu.org/copyleft/gpl.html 
   and in the file nco/doc/LICENSE in the NCO source distribution.
   
   As a special exception to the terms of the GPL, you are permitted 
   to link the NCO source code with the HDF, netCDF, OPeNDAP, and UDUnits
   libraries and to distribute the resulting executables under the terms 
   of the GPL, but in addition obeying the extra stipulations of the 
   HDF, netCDF, OPeNDAP, and UDUnits licenses.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
   See the GNU General Public License for more details.
   
   The original author of this software, Charlie Zender, seeks to improve
   it with your suggestions, contributions, bug-reports, and patches.
   Please contact the NCO project at http://nco.sf.net or write to
   Charlie Zender
   Department of Earth System Science
   University of California, Irvine
   Irvine, CA 92697-3100 */

/* Usage:
   bison --output=${HOME}/nco/src/nco/ncap_yacc.c -d ~/nco/src/nco/ncap_yacc.y */

/* Example yacc text:
   Nie02 "A Compact Guide to Lex & Yacc" by Thomas Niemann, ePaper Press, URL:http://epaperpress.com/lexandyacc/index.html
   LMB92 ${DATA}/ora/lexyacc/ch3-05.y
   GCC c-parse.y
   GCC parser_build_binary_op() c-typeck.c
   Unidata ncgen.y */

/* Standard header files */
#include <math.h> /* sin cos cos sin 3.14159 */
#include <stdarg.h> /* va_start, va_arg, va_end */
#include <stdio.h> /* stderr, FILE, NULL, etc. */
#include <stdlib.h> /* atof, atoi, malloc, getopt */
#include <string.h> /* strcmp. . . */
#include <assert.h>
/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions and C library */
#include "nco_netcdf.h" /* NCO wrappers for netCDF C library */

/* Personal headers */
#include "nco.h" /* netCDF Operator (NCO) definitions */
#include "ncap.h" /* netCDF arithmetic processor-specific definitions (symbol table, ...) */

/* Bison adds routines which reference YY*LEX token to ncap_yacc.c 
   These routines generate warnings unless YY*LEX prototype appears above
   YYLEX prototype generates error unless YYSTYPE token is defined
   Thus must #include ncap_yacc.h solely to pre-define YY*STYPE for YY*LEX prototype
   There is no other reason for ncap_yacc.h to appear in ncap_yacc.y 
   Yes, this is rather circular */
/* Get YYSTYPE prior to prototyping scanner */
#include "ncap_yacc.h" /* ncap_yacc.h (ncap.tab.h) is produced from ncap_yacc.y by parser generator */

  #define YY_DECL int yylex(YYSTYPE *lval_ptr,prs_sct *prs_arg)
  YY_DECL;

/* Turn on parser debugging option (Bison manual p. 85) */
#define YYDEBUG 0
int yydebug=0; /* 0: Normal operation. 1: Print parser rules during execution */

/* Turns on more verbose errors than just plain "parse error" when yyerror() is called by parser */
#define YYERROR_VERBOSE 1
/* Bison manual p. 60 describes how to call yyparse() with arguments 
   Following two statements superceded 20051213 by parse-param and lex-param below */
/* #define YYPARSE_PARAM prs_arg */
/* #define YYLEX_PARAM prs_arg */

int rcd; /* [enm] Return value for function calls */

/* Global variables */
extern size_t ncap_ncl_dpt_crr; /* [nbr] Depth of current #include file (declared in ncap.c) */
extern size_t *ncap_ln_nbr_crr; /* [cnt] Line number (declared in ncap.c) */
extern char **ncap_fl_spt_glb; /* [fl] Script file (declared in ncap.c) */
extern char ncap_err_sng[200]; /* [sng] Buffer for error string (declared in ncap_lex.l) */

/* End C declarations section */
%}
/* Begin parser declaration section */

/* Request pure, re-entrant parser, so we can pass a structure to parser
   fxm: 20020122 ncap16: Code breaks on Linux when pure_parser is _not_ used---why?
   e.g., ncap -O -D 5 -S ${HOME}/dst/dst.nco ${DATA}/${caseid}/${caseid}_${yr_sng}_xy.nc ${DATA}/${caseid}/${caseid}_${yr_sng}_xy.nc
   Possibly because hardcoded yy* function prototypes change? */
%pure-parser
%parse-param {prs_sct *prs_arg}
%lex-param {prs_sct *prs_arg}

/* NB: "terminal symbol" is just a fancy name for token produced by lexer 
   Symbols defined on LHS of rules are called "non-terminal symbols" or "non-terminals" 
   Examples of non-terminals are xpr, stt, stt_lst
   Examples of terminal symbols, or tokens, are NAME, NUMBER
   Convention is to make token names all uppercase, and non-terminals lowercase */

/* Define YYSTYPE union (type of lex variable yylval value)
   This specifies all possible data types for semantic values */
%union{
  char *sng; /* [sng] String value */
  char *var_nm_LHS; /* [sng] Variables on LHS */
  char *var_nm_RHS; /* [sng] Variables on RHS */
  aed_sct aed; /* [sct] Attribute */
  sym_sct *sym; /* [sct] Intrinsic function name */
  scv_sct scv; /* [sct] Scalar value */
  var_sct *var; /* [sct] Variable */
  nm_lst_sct *sbs_lst; /* [sct] Subscript list */
  int nco_rlt_opr; /* [enm] Comparison operator type */
  nc_type cnv_type;  /* [enm] Used for type conversion functions */
} /* end YYSTYPE union (type of yylval value) */

/* Tell parser which kind of values each token takes
   Token name (traditionally in all caps) becomes #define directive in parser
   so we can refer to token name rather than token's numeric code */
%token <aed> OUT_ATT
%token <nco_rlt_opr> COMPARISON
%token <sbs_lst> LHS_SBS
%token <scv> SCV
%token <sng> SNG
%token <sym> FUNCTION
%token <var_nm_LHS> OUT_VAR
%token <var_nm_RHS> VAR
%token <cnv_type> CNV_TYPE
%token ABS ATOSTR EPROVOKE IGNORE NAMED_CONSTANT PACK POWER RDC UNPACK
%token IF PRINT

/* "type" declaration sets type for non-terminal symbols which otherwise need no declaration
   Format of "type" declaration is
   %type <YYSTYPE_member> RHS_xpr
   RHS expression type RHS_xpr is same type as YYSTYPE union member with name in <> */
%type <scv> scv_xpr
%type <sng> sng_xpr
%type <var> var_xpr
%type <var_nm_LHS> out_var_xpr
%type <aed> out_att_xpr

/* "left", "right", and "nonassoc" perform same function as "token" and,
   in addition, specify associativity and relative precedence of symbols.
   Each successive line has higher precedence than preceding lines */
/* fxm: 20020608 AND, NOT, OR not implemented in lexer yet */
%left AND NOT OR /* && ! || */
%left COMPARISON /* == != < > <= >= */
%left '+' '-'
%left '*' '/' '%'
%right '^'
%nonassoc UMINUS
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

/* End parser declaration section */
%%
/* Begin Rules section
   Format is rule: action
   Comments OK in space between C code but must be indented */

program: 
stmt_lst
; /* end program */

stmt_lst:
 ';' { ;}           /* collect extra ; */
| stmt_lst ';' { ;} /* collect extra ; */
| stmt_lst stmt ';' {
  /* Purpose: Actions to be performed at end-of-statement go here */
  /* Clean up from and exit LHS_cst mode */
  (void)nco_var_free_wrp(&((prs_sct *)prs_arg)->var_LHS);
} /* end stmt ';' */
| stmt_lst error ';' {(void)nco_var_free_wrp(&((prs_sct *)prs_arg)->var_LHS);}
| stmt ';' {(void)nco_var_free_wrp(&((prs_sct *)prs_arg)->var_LHS);}
| error ';' {(void)nco_var_free_wrp(&((prs_sct *)prs_arg)->var_LHS);}


/* Catch most errors then read up to next semi-colon */
; /* end stmt_lst */

stmt: /* Statement is definition of out_att_xpr or out_var_xpr (LHS tokens)
	 in terms of scv_xpr, var_xpr, and sng_xpr (RHS tokens). 
	 All permutations are valid so this rule has six possible actions */
IF '(' bln_xpr ')' stmt %prec LOWER_THAN_ELSE {
  /* LMB92 p. 234 */
  ;
} /* end IF bln_xpr stmt */
| IF '(' bln_xpr ')' stmt ELSE stmt {
  /* LMB92 p. 234 */
  ;
} /* end IF bln_xpr stmt ELSE stmt */
PRINT '(' scv_xpr ')' ';' {
  ;
} /* end PRINT '(' scv_xpr ')' */
PRINT '(' var_xpr ')' ';' {
  ;
} /* end PRINT '(' var_xpr ')' */
| out_att_xpr '=' scv_xpr { 
  aed_sct *ptr_aed;
  
  ptr_aed=ncap_aed_lookup($1.var_nm,$1.att_nm,((prs_sct *)prs_arg),True);
  
  ptr_aed->val=ncap_scv_2_ptr_unn($3);
  ptr_aed->type=$3.type;
  ptr_aed->sz=1L;
  (void)cast_nctype_void(ptr_aed->type,&ptr_aed->val);    
  if(dbg_lvl_get() > 0) (void)sprintf(ncap_err_sng,"Saving attribute %s@%s to %s",$1.var_nm,$1.att_nm,((prs_sct *)prs_arg)->fl_out);
  (void)nco_yyerror(prs_arg,ncap_err_sng);
  
  if(dbg_lvl_get() > 1){
    (void)fprintf(stderr,"Saving in array attribute %s@%s=",$1.var_nm,$1.att_nm);
    switch($3.type){
  /* NB: Format depends on opaque type of nco_int
     Until 200911, nco_int was C-type long, and now nco_int is C-type int
     case NC_INT: (void)fprintf(stderr,"%ld\n",$3.val.i); break; */
    case NC_FLOAT: (void)fprintf(stderr,"%G\n",$3.val.f); break;
    case NC_DOUBLE: (void)fprintf(stderr,"%.5G\n",$3.val.d);break;
    case NC_INT: (void)fprintf(stderr,"%d\n",$3.val.i); break;
    case NC_SHORT: (void)fprintf(stderr,"%hi\n",$3.val.s); break;
    case NC_BYTE: (void)fprintf(stderr,"%hhi\n",$3.val.b); break;
    case NC_UBYTE: (void)fprintf(stderr,"%hhu\n",$3.val.ub); break;
    case NC_USHORT: (void)fprintf(stderr,"%hu\n",$3.val.us); break;
    case NC_UINT: (void)fprintf(stderr,"%u\n",$3.val.ui); break;
    case NC_INT64: (void)fprintf(stderr,"%lld\n",$3.val.i64); break;
    case NC_UINT64: (void)fprintf(stderr,"%llu\n",$3.val.ui64); break;
    case NC_CHAR: (void)fprintf(stderr,"%c\n",$3.val.c); break;
    case NC_STRING: (void)fprintf(stderr,"%s\n",$3.val.sng); break;
    default: break;
    } /* end switch */
  } /* end if */
  $1.var_nm=(char *)nco_free($1.var_nm);
  $1.att_nm=(char *)nco_free($1.att_nm);
} /* end out_att_xpr '=' scv_xpr */
| out_att_xpr '=' sng_xpr 
{
  aed_sct *ptr_aed;
  size_t sng_lng;
  
  sng_lng=strlen($3);
  ptr_aed=ncap_aed_lookup($1.var_nm,$1.att_nm,((prs_sct *)prs_arg),True);
  ptr_aed->type=NC_CHAR;
  ptr_aed->sz=(long int)((sng_lng+1)*nco_typ_lng(NC_CHAR));
  ptr_aed->val.cp=(nco_char *)nco_malloc((sng_lng+1)*nco_typ_lng(NC_CHAR));
  strcpy((char *)(ptr_aed->val.cp),$3);
  (void)cast_nctype_void((nc_type)NC_CHAR,&ptr_aed->val);    
  
  if(dbg_lvl_get() > 0) (void)sprintf(ncap_err_sng,"Saving attribute %s@%s=%s",$1.var_nm,$1.att_nm,$3);
  (void)nco_yyerror(prs_arg, ncap_err_sng);
  $1.var_nm=(char *)nco_free($1.var_nm);
  $1.att_nm=(char *)nco_free($1.att_nm);
  $3=(char *)nco_free($3);
} /* end out_att_xpr '=' sng_xpr */
| out_att_xpr '=' var_xpr
{ 
  /* Storing 0-dimensional variables in attribute is OK */ 
  aed_sct *ptr_aed;
  
  if($3->nbr_dim < 2){
    ptr_aed=ncap_aed_lookup($1.var_nm,$1.att_nm,((prs_sct *)prs_arg),True);
    ptr_aed->sz=$3->sz;
    ptr_aed->type=$3->type;
    /* if inital scan then fill attribute with zeros */
    if( ((prs_sct*)prs_arg)->ntl_scn) {
    ptr_aed->val.vp=(void*)nco_calloc( ptr_aed->sz,nco_typ_lng(ptr_aed->type));
    } else {
    ptr_aed->val.vp=(void*)nco_malloc((ptr_aed->sz)*nco_typ_lng(ptr_aed->type));
    (void)var_copy(ptr_aed->type,ptr_aed->sz,$3->val,ptr_aed->val);
    }
    /* cast_nctype_void($3->type,&ptr_aed->val); */
    if(dbg_lvl_get() > 0) (void)sprintf(ncap_err_sng,"Saving attribute %s@%s %d dimensional variable",$1.var_nm,$1.att_nm,$3->nbr_dim);
    (void)yyerror(prs_arg,ncap_err_sng); 
  }else{
    (void)sprintf(ncap_err_sng,"Warning: Cannot store in attribute %s@%s a variable with dimension %d",$1.var_nm,$1.att_nm,$3->nbr_dim);
    (void)yyerror(prs_arg,ncap_err_sng);
  } /* endif */
  $1.var_nm=(char *)nco_free($1.var_nm);
  $1.att_nm=(char *)nco_free($1.att_nm);
  (void)nco_var_free($3); 
} /* end out_att_xpr '=' var_xpr */
| out_var_xpr '=' var_xpr 
{
  ($3->nm)=(char*)nco_free($3->nm);
  $3->nm=strdup($1);
  (void)ncap_var_write($3,(prs_sct *)prs_arg);
  
  /* Print mess only for defined variables */
  if(dbg_lvl_get() > 0 && !$3->undefined){(void)sprintf(ncap_err_sng,"Saving variable %s to %s",$1,((prs_sct *)prs_arg)->fl_out);
  (void)yyerror(prs_arg,ncap_err_sng);
  } /* endif */
  $1=(char *)nco_free($1);
} /* end out_var_xpr '=' var_xpr */
| out_var_xpr '=' scv_xpr
{
  var_sct *var;
  var_sct *var_tmp;  
  if(dbg_lvl_get() > 5) (void)fprintf(stderr,"%s: DEBUG out_var_xpr = scv_xpr rule for %s\n",prg_nm_get(),$1);
  
  /* Turn attribute into temporary variable for writing */
  var=(var_sct *)nco_malloc(sizeof(var_sct));
  /* Set defaults */
  (void)var_dfl_set(var); /* [fnc] Set defaults for each member of variable structure */
  /* Overwrite with attribute expression information */
  var->nm=strdup($1);
  var->nbr_dim=0;
  var->sz=1;
  var->type=$3.type;
  var->val=ncap_scv_2_ptr_unn($3);
  
  if(((prs_sct *)prs_arg)->var_LHS != NULL){
    /* User intends LHS to cast RHS to same dimensionality
       Stretch newly initialized variable to size of LHS template */
    /*    (void)ncap_var_cnf_dmn(&$$,&(((prs_sct *)prs_arg)->var_LHS));*/
    var_tmp=var;
    (void)ncap_var_stretch(&var_tmp,&(((prs_sct *)prs_arg)->var_LHS));
    if(var_tmp != var) {
      var=nco_var_free(var);
      var=var_tmp;
	}

    if(dbg_lvl_get() > 2) (void)fprintf(stderr,"%s: Stretching former scv_xpr defining %s with LHS template: Template var->nm %s, var->nbr_dim %d, var->sz %li\n",prg_nm_get(),$1,((prs_sct *)prs_arg)->var_LHS->nm,((prs_sct *)prs_arg)->var_LHS->nbr_dim,((prs_sct *)prs_arg)->var_LHS->sz);
  } /* endif LHS_cst */

  var->undefined=False;
  (void)ncap_var_write(var,(prs_sct *)prs_arg);
  
  if(dbg_lvl_get() > 0 ) (void)sprintf(ncap_err_sng,"Saving variable %s to %s",$1,((prs_sct *)prs_arg)->fl_out);
  (void)yyerror(prs_arg,ncap_err_sng);
  
  
  $1=(char *)nco_free($1);
} /* end out_var_xpr '=' scv_xpr */
| out_var_xpr '=' sng_xpr
{
  
  var_sct *var;
  
  var=(var_sct *)nco_calloc((size_t)1,sizeof(var_sct));
  var->nm=strdup($1);
  var->nbr_dim=0;
  var->dmn_id=(int *)NULL;
  var->sz=strlen($3)+1;
  var->val.cp=(nco_char *)strdup($3);
  var->type=NC_CHAR;
  var->undefined=False;
  (void)cast_nctype_void((nc_type)NC_CHAR,&var->val);
  (void)ncap_var_write(var,(prs_sct *)prs_arg);
  
  if(dbg_lvl_get() > 0) (void)sprintf(ncap_err_sng,"Saving variable %s to %s",$1,((prs_sct *)prs_arg)->fl_out);
  (void)yyerror(prs_arg,ncap_err_sng);
  
  $1=(char *)nco_free($1);
  $3=(char *)nco_free($3);
} /* end out_var_xpr '=' sng_xpr */
; /* end stmt */

scv_xpr: /* scv_xpr results from RHS action which involves only scv_xpr's
	    One action exists for each binary and unary attribute-valid operator */
scv_xpr '+' scv_xpr {
  (void)ncap_scv_scv_cnf_typ_hgh_prc(&$1,&$3);
  $$=ncap_scv_clc($1,'+',$3);                            
}
| scv_xpr '-' scv_xpr {
  (void)ncap_scv_scv_cnf_typ_hgh_prc(&$1,&$3); 
  $$=ncap_scv_clc($1,'-',$3);
}
| scv_xpr '*' scv_xpr {
  (void)ncap_scv_scv_cnf_typ_hgh_prc(&$1,&$3);
  $$=ncap_scv_clc($1,'*',$3);
}
| scv_xpr '/' scv_xpr {
  (void)ncap_scv_scv_cnf_typ_hgh_prc(&$1,&$3); 
  $$=ncap_scv_clc($1,'/',$3);  
}
| scv_xpr '%' scv_xpr {
  (void)ncap_scv_scv_cnf_typ_hgh_prc(&$1,&$3);
  
  $$=ncap_scv_clc($1,'%',$3);  
}
| '-' scv_xpr %prec UMINUS {
  (void)ncap_scv_minus(&$2);
  $$=$2;
}
| '+' scv_xpr %prec UMINUS {
  $$=$2;
}
| scv_xpr '^' scv_xpr {
  if(nco_rth_prc_rnk($1.type) <= nco_rth_prc_rnk_float && nco_rth_prc_rnk($3.type) <= nco_rth_prc_rnk_float) {
    (void)nco_scv_cnf_typ((nc_type)NC_FLOAT,&$1);
    (void)nco_scv_cnf_typ((nc_type)NC_FLOAT,&$3);
    $$.val.f=powf($1.val.f,$3.val.f);
    $$.type=NC_FLOAT;
  }else{
    (void)nco_scv_cnf_typ((nc_type)NC_DOUBLE,&$1);
    (void)nco_scv_cnf_typ((nc_type)NC_DOUBLE,&$3);
    $$.val.d=pow($1.val.d,$3.val.d);
    $$.type=NC_DOUBLE; 
  } /* end else */
} /* end scv_xpr '^' scv_xpr */
| POWER '(' scv_xpr ',' scv_xpr ')' { /* fxm: ncap52 this is identical to previous clause except for argument numbering, should be functionalized to use common code */
  if(nco_rth_prc_rnk($3.type) <= nco_rth_prc_rnk_float && nco_rth_prc_rnk($5.type) <= nco_rth_prc_rnk_float) {
    (void)nco_scv_cnf_typ((nc_type)NC_FLOAT,&$3);
    (void)nco_scv_cnf_typ((nc_type)NC_FLOAT,&$5);
    $$.val.f=powf($3.val.f,$5.val.f);
    $$.type=NC_FLOAT;
  }else{ 
    (void)nco_scv_cnf_typ((nc_type)NC_DOUBLE,&$3);
    (void)nco_scv_cnf_typ((nc_type)NC_DOUBLE,&$5);
    $$.val.d=pow($3.val.d,$5.val.d);
    $$.type=NC_DOUBLE; 
  } /* end else */
} /* end POWER '(' scv_xpr ',' scv_xpr ')' */
| ABS '(' scv_xpr ')' {
  $$=ncap_scv_abs($3);
}
| FUNCTION '(' scv_xpr ')' {
  if(nco_rth_prc_rnk($3.type) <= nco_rth_prc_rnk_float) {
    (void)nco_scv_cnf_typ((nc_type)NC_FLOAT,&$3);
    $$.val.f=(*($1->fnc_flt))($3.val.f);
    $$.type=NC_FLOAT;
  }else{
    $$.val.d=(*($1->fnc_dbl))($3.val.d);
    $$.type=NC_DOUBLE;
  } /* end else */
} /* end FUNCTION '(' scv_xpr ')' */
| CNV_TYPE '(' scv_xpr ')' {
  (void)nco_scv_cnf_typ($1,&$3);
  $$=$3;
}
| '(' scv_xpr ')' {$$=$2;}
| SCV {$$=$1;}
; /* end scv_xpr */

out_var_xpr: OUT_VAR {$$=$1;}
;

out_att_xpr: OUT_ATT {$$=$1;}
;

sng_xpr: /* sng_xpr is any combination of sng_xpr or attribute */
sng_xpr '+' sng_xpr {
  size_t sng_lng;
  sng_lng=strlen($1)+strlen($3);
  $$=(char*)nco_malloc((sng_lng+1)*sizeof(char));
  strcpy($$,$1);
  strcat($$,$3);
  $1=(char *)nco_free($1);
  $3=(char *)nco_free($3);
} /* end sng_xpr '+' sng_xpr */
| ATOSTR '(' scv_xpr ')' {
  char bfr[50];
  switch ($3.type){
  /* NB: Format depends on opaque type of nco_int
     Until 200911, nco_int was C-type long, and now nco_int is C-type int
     case NC_INT: sprintf(bfr,"%ld",$3.val.i); break; */
  case NC_DOUBLE: sprintf(bfr,"%.10G",$3.val.d); break;
  case NC_FLOAT: sprintf(bfr,"%G",$3.val.f); break;
  case NC_INT: sprintf(bfr,"%d",$3.val.i); break;
  case NC_SHORT: sprintf(bfr,"%hi",$3.val.s); break;
  case NC_BYTE: sprintf(bfr,"%hhi",$3.val.b); break;
  case NC_UBYTE: sprintf(bfr,"%hhu",$3.val.ub); break;
  case NC_USHORT: sprintf(bfr,"%hu",$3.val.us); break;
  case NC_UINT: sprintf(bfr,"%u",$3.val.ui); break;
  case NC_INT64: sprintf(bfr,"%lld",$3.val.i64); break;
  case NC_UINT64: sprintf(bfr,"%llu",$3.val.ui64); break;
  case NC_CHAR: sprintf(bfr,"%c",$3.val.c); break;
  case NC_STRING: sprintf(bfr,"%s",$3.val.sng); break;
  default:  break;
  } /* end switch */
  $$=strdup(bfr);      
} /* end ATOSTR '(' scv_xpr ')' */
| ATOSTR '(' scv_xpr ',' sng_xpr ')' {
  char bfr[150];
  /* Format string according to string expression */
  /* User decides which format corresponds to which type */
  switch ($3.type){
  case NC_DOUBLE: sprintf(bfr,$5,$3.val.d); break;
  case NC_FLOAT: sprintf(bfr,$5,$3.val.f); break;
  case NC_INT: sprintf(bfr,$5,$3.val.i); break;
  case NC_SHORT: sprintf(bfr,$5,$3.val.s); break;
  case NC_BYTE: sprintf(bfr,$5,$3.val.b); break;
  case NC_UBYTE: sprintf(bfr,$5,$3.val.ub); break;
  case NC_USHORT: sprintf(bfr,$5,$3.val.us); break;
  case NC_UINT: sprintf(bfr,$5,$3.val.ui); break;
  case NC_INT64: sprintf(bfr,$5,$3.val.i64); break;
  case NC_UINT64: sprintf(bfr,$5,$3.val.ui64); break;
  case NC_CHAR: sprintf(bfr,$5,$3.val.c); break;
  case NC_STRING: sprintf(bfr,$5,$3.val.sng); break;
  default:  break;
  } /* end switch */
  $5=(char *)nco_free($5);
  $$=strdup(bfr);      
} /* end ATOSTR '(' scv_xpr ',' sng_xpr ')' */
| SNG {$$=$1;} /* Terminal symbol action */
; /* end sng_xpr */

bln_xpr: /* bln_xpr results from comparison of var_xpr's and scv_xpr's */
var_xpr COMPARISON var_xpr {
}
| scv_xpr COMPARISON scv_xpr {
}
| var_xpr COMPARISON scv_xpr {
}
| scv_xpr COMPARISON var_xpr {
}
; /* end bln_xpr */

var_xpr: /* var_xpr results from RHS action which involves a var_xpr, i.e.,
	    OP var, var OP var, var OP att, att OP var */
var_xpr '+' var_xpr { /* Begin Addition */
  $$=ncap_var_var_add($1,$3); 
}
| var_xpr '+' scv_xpr {
  $$=ncap_var_scv_add($1,$3);
}            
| scv_xpr '+' var_xpr {
  /* Addition commutes so swap arguments and use S+V = V+S */
  $$=ncap_var_scv_add($3,$1);
}  /* End Addition */
| var_xpr '-' var_xpr { /* Begin Subtraction */
  $$=ncap_var_var_sub($1,$3);
}
| var_xpr '-' scv_xpr {
  $$=ncap_var_scv_sub($1,$3);
}
| scv_xpr '-' var_xpr { 
  /* Subtraction is non-commutative, do not swap arguments and/or re-use V-S subtraction function
     Use anti-symmetric property that V-S=-(S-V) */
  scv_sct minus;
  minus.val.b=-1;
  minus.type=NC_BYTE;
  (void)nco_scv_cnf_typ($3->type,&minus);
  (void)ncap_var_scv_sub($3,$1);
  $$=ncap_var_scv_mlt($3,minus);
} /* End Subtraction */
| var_xpr '*' var_xpr { /* Begin Multiplication */
  $$=ncap_var_var_mlt($1,$3); 
}
| var_xpr '*' scv_xpr {
  $$=ncap_var_scv_mlt($1,$3);
}
| scv_xpr '*' var_xpr {
  /* Addition commutes so swap arguments and use S*V = V*S */
  $$=ncap_var_scv_mlt($3,$1);
} /* End Multiplication */
| var_xpr '/' var_xpr { /* Begin Division */
  /* NB: Order was important (keeping denominator as I/O variable)
     This is no longer true with ncbo
     Maybe rewrite to keep argument ordering consitent with multiplication, addition */
  $$=ncap_var_var_dvd($3,$1);
}
| var_xpr '/' scv_xpr { /* Keep V as I/O */
  $$=ncap_var_scv_dvd($1,$3);
}
| scv_xpr '/' var_xpr {
 /* Division is non-commutative, use S/V not V/S division function */
  $$=ncap_scv_var_dvd($1,$3);
} /* End Division */
| var_xpr '%' var_xpr { /* Begin Modulo */
  $$=ncap_var_var_mod($1,$3);
}
| var_xpr '%' scv_xpr {
  $$=ncap_var_scv_mod($1,$3);
}
| scv_xpr '%' var_xpr {
  /* Modulo is non-commutative, use S%V not V%S modulo function */
  $$=ncap_scv_var_mod($1,$3);
} /* End Modulo */
| var_xpr '^' var_xpr { /* Begin Empowerment of form x^y */
  $$=ncap_var_var_pwr($1,$3);
}
| var_xpr '^' scv_xpr {
  $$=ncap_var_scv_pwr($1,$3);
}
| scv_xpr '^' var_xpr {
  /* Empowerment is non-commutative, use S^V not V^S empowerment function */
  $$=ncap_scv_var_pwr($1,$3);
}
| POWER '(' var_xpr ',' var_xpr ')' { /* Begin Empowerment of form pow(x,y) */
  /* fxm: TODO ncap52 Combine pow() with ^ parsing in parser ncap_yacc.y */
  $$=ncap_var_var_pwr($3,$5);
}
| POWER '(' var_xpr ',' scv_xpr ')' {
  $$=ncap_var_scv_pwr($3,$5);
}
| POWER '(' scv_xpr ',' var_xpr ')' {
  /* Empowerment is non-commutative, use S^V not V^S empowerment function */
  $$=ncap_scv_var_pwr($3,$5);
} /* End Empowerment */
| '-' var_xpr %prec UMINUS { /* Begin Unary Subtraction */
  scv_sct minus;
  minus.val.b=-1;
  minus.type=NC_BYTE;
  $$=ncap_var_scv_mlt($2,minus);
} /* End Unary Subtraction */
| '+' var_xpr %prec UMINUS { /* Begin Unary Addition */
  $$=$2;
} /* End Unary Addition */
| ABS '(' var_xpr ')' {
  $$=ncap_var_abs($3);
} /* end ABS */
| RDC '(' var_xpr ')' {
  $$=ncap_var_abs($3);
  /* fxm Finish avg,min,max,ttl */
  /* $$=nco_var_avg($3,dim,dmn_nbr,nco_op_typ); */
  /* if(prs_arg->nco_op_typ == nco_op_avg) (void)nco_var_dvd(var_prc_out[idx]->type,var_prc_out[idx]->sz,var_prc_out[idx]->has_mss_val,var_prc_out[idx]->mss_val,wgt_avg->val,var_prc_out[idx]->val); */
  (void)fprintf(stderr,"%s: WARNING RDC tokens not implemented yet\n",prg_nm_get());
  /* $3 is freed in nco_var_avg() */
} /* end ABS */
| PACK '(' var_xpr ')' {
  /* Packing variable does not create duplicate so DO NOT free $3 */
  const nc_type nc_typ_pck_dfl=NC_SHORT; /* [enm] Default type to pack to */
  nco_bool PCK_VAR_WITH_NEW_PCK_ATT;
  
  if(((prs_sct*)prs_arg)->ntl_scn){
    $3->undefined=True;
    $$=$3;
  }else{ 
    $$=nco_var_pck($3,nc_typ_pck_dfl,&PCK_VAR_WITH_NEW_PCK_ATT);
    PCK_VAR_WITH_NEW_PCK_ATT=PCK_VAR_WITH_NEW_PCK_ATT; /* CEWI */
  } /* end else */
} /* end PACK */
| UNPACK '(' var_xpr ')' {
  /* Unpacking variable does not create duplicate so DO NOT free $3 */
  /* Do not unpack on first pass */
  if(((prs_sct*)prs_arg)->ntl_scn){
    $3->undefined=True;
    $$=$3;
  }else{ 
    $$=nco_var_upk($3);
  } /* end else */
} /* end UNPACK */
| FUNCTION '(' var_xpr ')' {
  $$=ncap_var_fnc($3,$1);
}  
| '(' var_xpr ')' {
  $$=$2;
}
| CNV_TYPE '(' var_xpr ')' {
  $$=nco_var_cnf_typ($1,$3);
}
| NAMED_CONSTANT { /* Terminal symbol action */
  /* fxm: Allow commands like a=M_PI*rds^2; to work */
}
| VAR { /* Terminal symbol action */
  
  var_sct *var;
  var_sct *var_tmp;
  prs_sct *prs_drf; /*Pointer for de-referencing */
  
  prs_drf=(prs_sct*)prs_arg;
  
  var=ncap_var_init($1,prs_drf);
  var->undefined=False;
  
  if(prs_drf->ntl_scn == True && prs_drf->var_LHS != NULL){
    var_tmp=nco_var_dpl(prs_drf->var_LHS);
    var_tmp->id=var->id;
    var_tmp->nm=(char*)nco_free(var_tmp->nm);
    var_tmp->nm=strdup($1);
    var_tmp->type=var->type;
    var_tmp->typ_dsk=var->typ_dsk;
    var_tmp->undefined=False;
    var_tmp->val.vp=(void*)NULL;
    var=nco_var_free(var);
    var=var_tmp;
  } /* endif ntl_scn */
  
  if(prs_drf->ntl_scn == False && prs_drf->var_LHS != NULL){
    /* User intends LHS to cast RHS to same dimensionality
       Stretch newly initialized variable to size of LHS template */
    /*    (void)ncap_var_cnf_dmn(&$$,&(((prs_sct *)prs_arg)->var_LHS));*/
    var_tmp=var;
    (void)ncap_var_stretch(&var_tmp,&(prs_drf->var_LHS));
    if(var_tmp != var) { 
      var=nco_var_free(var); 
      var=var_tmp;
    }
  
    if(dbg_lvl_get() > 2) (void)fprintf(stderr,"%s: Stretching variable %s with LHS template: Template var->nm %s, var->nbr_dim %d, var->sz %li\n",prg_nm_get(),var->nm,prs_drf->var_LHS->nm,prs_drf->var_LHS->nbr_dim,prs_drf->var_LHS->sz);
    var->undefined=False;
  } /* endif LHS_cst */
  $1=(char*)nco_free($1);
  $$=var;
  /* Sanity check */
  if ($$==(var_sct *)NULL) YYERROR;
} /* end VAR terminal symbol */
; /* end var_xpr */

/* End Rules section */
%%
/* Begin User Functions section */

aed_sct *  /* O [idx] Location of attribute in list */
ncap_aed_lookup /* [fnc] Find location of existing attribute or add new attribute */
(const char * const var_nm, /* I [sng] Variable name */
 const char * const att_nm, /* I [sng] Attribute name */
 prs_sct *  prs_arg,   /* contains attribute list */       
 const nco_bool update) /* I [flg] Delete existing value or add new attribute to list */
{
  int idx;
  int size;
  aed_sct *ptr_aed;
  
  size=*(prs_arg->nbr_att);
  
  for(idx=0;idx<size;idx++){
    ptr_aed=(*(prs_arg->att_lst))[idx];
    
    if(strcmp(ptr_aed->att_nm,att_nm) || strcmp(ptr_aed->var_nm,var_nm)) 
      continue; 
    
    if(update) ptr_aed->val.vp=nco_free(ptr_aed->val.vp);   
    /* Return pointer to list element */
    return ptr_aed;
    
  } /* end for */
  
  if(!update) return (aed_sct *)NULL;
  
  *(prs_arg->att_lst)=(aed_sct **)nco_realloc(*(prs_arg->att_lst),(size+1)*sizeof(aed_sct*));
  ++*(prs_arg->nbr_att);
  (*(prs_arg->att_lst))[size]=(aed_sct *)nco_malloc(sizeof(aed_sct));
  (*(prs_arg->att_lst))[size]->var_nm=strdup(var_nm);
  (*(prs_arg->att_lst))[size]->att_nm=strdup(att_nm);
  
  return (*(prs_arg->att_lst))[size];
} /* end ncap_aed_lookup() */

var_sct * /*I [sct] varibale in list */
ncap_var_lookup
(var_sct *var, /* I  [sct] variable  */
 prs_sct *prs_arg, /* I/O [sct] contains var list */
 const nco_bool add) /* I if not in list then add to list */          
{
  int idx;
  int size;
  
  var_sct *ptr_var; 
  
  size = *(prs_arg->nbr_var);
  
  for(idx=0;idx<size;idx++){
    
    ptr_var=(*(prs_arg->var_lst))[idx];
    /*
      assert(var->nm);
      assert(ptr_var->nm);
      if(!strcmp(var->nm,ptr_var->nm)) return ptr_var;    
    */
    if(ptr_var==NULL || strcmp(var->nm,ptr_var->nm) ) continue;
    
    return ptr_var;
  } /* end loop over idx */
  
  if(!add) return (var_sct *)NULL;
  
  *(prs_arg->var_lst)=(var_sct **)nco_realloc(*(prs_arg->var_lst),(size+1)*sizeof(var_sct*));
  ++*(prs_arg->nbr_var);
  (*(prs_arg->var_lst))[size]=var;
  
  return (var_sct *)NULL;
} /* end ncap_var_lookup() */

int /* [rcd] Return code */
yyerror /* [fnc] Print error/warning/info messages generated by parser */
(prs_sct *prs_arg, const char * const err_sng_lcl) /* [sng] Message to print */
{
  /* Purpose: Print error/warning/info messages generated by parser
     Use eprokoke_skip to skip error message after sending error message from yylex()
     Stop provoked error message from yyparse being printed */
  
  static nco_bool eprovoke_skip;
  
  prs_arg=prs_arg; /* CEWI otherwise unused parameter error */

  /* if(eprovoke_skip){eprovoke_skip=False ; return 0;} */
  if(dbg_lvl_get() > 0){
    (void)fprintf(stderr,"%s: %s line %lu",prg_nm_get(),ncap_fl_spt_glb[ncap_ncl_dpt_crr],(unsigned long)ncap_ln_nbr_crr[ncap_ncl_dpt_crr]);
    if(dbg_lvl_get() > 1) (void)fprintf(stderr," %s",err_sng_lcl);
    (void)fprintf(stderr,"\n");
    (void)fflush(stderr);
  } /* endif dbg */
  
  if(err_sng_lcl[0] == '#') eprovoke_skip=True;
  eprovoke_skip=eprovoke_skip; /* Do nothing except avoid compiler warnings */
  return 0;
} /* end yyerror() */

void 
nco_var_free_wrp /* [fnc] Safely free variable */
(var_sct **var) /* I/O [sct] Variable */
{
  /* Purpose: Safely free variable
     Routine is wrapper for nco_var_free() that simplifies code in calling routine */
  if(*var != NULL) *var=nco_var_free(*var);
} /* end nco_var_free_wrp() */

nodeType * /* O [unn] Syntax tree node */
opr_ctl /* [fnc] Operation controller function Nie02 opr() */
(int opr_tkn, /* I [enm] Operator token */
 int arg_nbr, /* I [nbr] Number of optional arguments to malloc() wrapper */
 ...) /* I [llp] Ellipsis defined in stdarg.h */
{
  /* Purpose: Create and return syntax tree node */
  va_list arg_lst; /* [] Variable argument list */
  nodeType *nod; /* [sct] Syntax tree node */
  size_t nod_sz; /* [nbr] Node size */
  int arg_idx; /* [idx] Argument index */
  
  /* Operator node requires space for token and arguments */
  nod_sz=sizeof(opr_nod_sct)+(arg_nbr-1)*sizeof(nodeType *);
  nod=(nodeType *)nco_malloc(nod_sz);
  /* Copy information into new node */
  nod->nod_typ=typ_opr; /* [enm] Node type */
  nod->opr.opr_tkn=opr_tkn; /* [enm] Operator token */
  nod->opr.arg_nbr=arg_nbr; /* [nbr] Number of arguments */
  /* Begin variable argument list access */
  va_start(arg_lst,arg_nbr);
  for(arg_idx=0;arg_idx<nod->opr.arg_nbr;arg_idx++) nod->opr.arg[arg_idx]=va_arg(arg_lst,nodeType); /* NB: Nie02 p. 27 has typo in va_arg() */
  /* End variable argument list access */
  va_end(arg_lst);
  return nod;
} /* end opr_ctl() */

void
freeNode /* [fnc] Free syntax tree node Nie02 freeNode() */
(nodeType *nod) /* I/O [sct] Syntax tree node to free */
{
  /* Purpose: Free syntax tree node */
  int arg_idx; /* [idx] Argument index */
  
  if(!nod) return;
  
  /* Operator nodes have copies of arguments. Free these first. */
  if(nod->nod_typ == typ_opr){
    /* Recursive call to freeNode continue until statement is reduced */
    for(arg_idx=0;arg_idx<nod->opr.arg_nbr;arg_idx++) freeNode(nod->opr.arg+arg_idx); /* Nie02 p. 28 has typo and passes node not node pointer */
  } /* endif */
  /* Free node itself */
  nod=(nodeType *)nco_free(nod);
  
  return; /* 20050109: fxm added return to void function to squelch erroneous gcc-3.4.2 warning */ 
} /* end freeNode() */

/* End User Functions section */
