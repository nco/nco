%{ /* $Header: /data/zender/nco_20150216/nco/src/nco/ncap.y,v 1.57 2002-05-17 07:36:34 zender Exp $ -*-C-*- */

/* Begin C declarations section */
 
/* Purpose: Grammar parser for ncap */

/* Copyright (C) 1995--2002 Charlie Zender
   This software is distributed under the terms of the GNU General Public License
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

/* Example yacc text:
   /data/zender/ora/lexyacc/ch3-05.y
   /data/zender/gcc-2.7.2/c-parse.y
   parser_build_binary_op is in /data/zender/gcc-2.7.2/c-typeck.c
   unidata ncgen.y */

/* Standard header files */
#include <math.h> /* sin cos cos sin 3.14159 */
#include <stdio.h> /* stderr, FILE, NULL, etc. */
#include <stdlib.h> /* atof, atoi, malloc, getopt */
#include <string.h> /* strcmp. . . */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions */

/* Personal headers */
#include "nco.h" /* NCO definitions */
#include "nco_netcdf.h" /* netCDF3 wrapper calls */
#include "ncap.h" /* symbol table definition */

/* Turn on parser debugging option (Bison manual p. 85) */
#define YYDEBUG 1
int yydebug=0; /* 0: Normal operation. 1: Print parser rules during execution */

/* Turns on more verbose errors than just plain "parse error" when yyerror() is called by parser */
#define YYERROR_VERBOSE 1

/* Bison manual p. 60 describes how to call yyparse() with arguments */
#define YYPARSE_PARAM prs_arg
#define YYLEX_PARAM prs_arg 
int rcd; /* [enm] Return value for function calls */

/* Global variables */
extern int ncl_dpt_crr; /* [nbr] Depth of current #include file (declared in ncap.c) */
extern long *ln_nbr_crr; /* [cnt] Line number (declared in ncap.c) */
extern char *fl_spt_glb; /* [fl] Script file (declared in ncap.c) */
extern char err_sng[200]; /* [sng] Buffer for error string (declared in ncap.l) */

/* End C declarations section */
%}
/* Begin parser declaration section */

/* Request pure, re-entrant parser, so we can pass a structure to parser
   fxm: 20020122 Code breaks on Linux when pure_parser is not used---why?
   e.g., ncap -O -D 5 -S ${HOME}/dst/dst.nco ${DATA}/${caseid}/${caseid}_${yr_sng}_xy.nc ${DATA}/${caseid}/${caseid}_${yr_sng}_xy.nc
   Possibly because hardcoded yy* function prototypes change? */
%pure_parser

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
} /* end YYSTYPE union (type of yylval value) */

/* Tell parser which kind of values each token takes
   Token name (traditionally in all caps) becomes #define directive in parser
   so we can refer to token name rather than token's numeric code */
%token <sng> SNG
%token <scv> SCV
%token <var_nm_RHS> VAR
%token <var_nm_LHS> OUT_VAR
%token <aed> OUT_ATT
%token <sym> FUNCTION
%token <sbs_lst> LHS_SBS
%token ABS ATOSTR EPROVOKE IGNORE RDC PACK POWER UNPACK

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
   in addition, specify associativity and relative precedence of symbols */
%left '+' '-'
%left '*' '/' '%'
%right '^'
%nonassoc UMINUS

/* End parser declaration section */
%%
/* Begin Rules section
   Format is rule: action
   Comments OK in space between C code but must be indented */

program: statement_list
;

statement_list: 
statement_list statement ';' {
  /* Purpose: Actions to be performed at end-of-statement go here */
  /* Clean up from and exit LHS_cst mode */
  (void)nco_var_free(&((prs_sct *)prs_arg)->var_LHS);
} /* end statement ';' */
| statement_list error ';' {(void)nco_var_free(&((prs_sct *)prs_arg)->var_LHS);}
| statement ';' {(void)nco_var_free(&((prs_sct *)prs_arg)->var_LHS);}
| error ';' {(void)nco_var_free(&((prs_sct *)prs_arg)->var_LHS);}
/* Catch most errors then read up to next semi-colon */
; /* end statement_list */

statement: /* statement is definition of out_att_xpr or out_var_xpr (LHS tokens)
	      in terms of scv_xpr, var_xpr, and sng_xpr (RHS tokens). 
	      All permutations are valid so this rule has 6 possible actions */
out_att_xpr '=' scv_xpr { 
  int aed_idx; 
  aed_sct *ptr_aed;

  aed_idx=ncap_aed_lookup($1.var_nm,$1.att_nm,((prs_sct *)prs_arg)->att_lst,((prs_sct *)prs_arg)->nbr_att,True);
  ptr_aed=((prs_sct *)prs_arg)->att_lst[aed_idx];                               
  ptr_aed->val=ncap_scv_2_ptr_unn($3);
  ptr_aed->type=$3.type;
  ptr_aed->sz=1L;
  (void)cast_nctype_void(ptr_aed->type,&ptr_aed->val);    
  if(dbg_lvl_get() > 0) (void)sprintf(err_sng,"Saving attribute %s@%s to %s",$1.var_nm,$1.att_nm,((prs_sct *)prs_arg)->fl_out);
  (void)yyerror(err_sng);

  if(dbg_lvl_get() > 1){
    (void)fprintf(stderr,"Saving in array attribute %s@%s=",$1.var_nm,$1.att_nm);
    switch($3.type){
    case NC_BYTE: (void)fprintf(stderr,"%d\n",$3.val.b); break;
    case NC_SHORT: (void)fprintf(stderr,"%d\n",$3.val.s); break;
    case NC_INT: (void)fprintf(stderr,"%ld\n",$3.val.l); break;
    case NC_FLOAT: (void)fprintf(stderr,"%G\n",$3.val.f); break;		  
    case NC_DOUBLE: (void)fprintf(stderr,"%.5G\n",$3.val.d);break;
    default: break;
    } /* end switch */
  } /* end if */
  $1.var_nm=(char *)nco_free($1.var_nm);
  $1.att_nm=(char *)nco_free($1.att_nm);
} /* end out_att_xpr '=' scv_xpr */
| out_att_xpr '=' sng_xpr 
{
  int aed_idx; 
  int sng_lng;
  aed_sct *ptr_aed;
  
  sng_lng=strlen($3);
  aed_idx=ncap_aed_lookup($1.var_nm,$1.att_nm,((prs_sct *)prs_arg)->att_lst,((prs_sct *)prs_arg)->nbr_att,True);
  ptr_aed=((prs_sct *)prs_arg)->att_lst[aed_idx];
  ptr_aed->type=NC_CHAR;
  ptr_aed->sz=(long)((sng_lng+1)*nco_typ_lng(NC_CHAR));
  ptr_aed->val.cp=(unsigned char *)nco_malloc((sng_lng+1)*nco_typ_lng(NC_CHAR));
  strcpy((char *)(ptr_aed->val.cp),$3);
  (void)cast_nctype_void(NC_CHAR,&ptr_aed->val);    
  
  if(dbg_lvl_get() > 0) (void)sprintf(err_sng,"Saving attribute %s@%s=%s",$1.var_nm,$1.att_nm,$3);
  (void)yyerror(err_sng);
  $1.var_nm=(char *)nco_free($1.var_nm);
  $1.att_nm=(char *)nco_free($1.att_nm);
  $3=(char *)nco_free($3);
} /* end out_att_xpr '=' sng_xpr */
| out_att_xpr '=' var_xpr
{ 
  /* Storing 0-dimensional variables in attribute is OK */ 
  int aed_idx;
  aed_sct *ptr_aed;
  
  if($3->nbr_dim < 2){
    aed_idx=ncap_aed_lookup($1.var_nm,$1.att_nm,((prs_sct *)prs_arg)->att_lst,((prs_sct *)prs_arg)->nbr_att,True);
    ptr_aed=((prs_sct *)prs_arg)->att_lst[aed_idx];
    ptr_aed->sz=$3->sz;
    ptr_aed->type=$3->type;
    ptr_aed->val.vp=(void*)nco_malloc((ptr_aed->sz)*nco_typ_lng(ptr_aed->type));
    (void)var_copy(ptr_aed->type,ptr_aed->sz,$3->val,ptr_aed->val);
    /* cast_nctype_void($3->type,&ptr_aed->val); */
    if(dbg_lvl_get() > 0) (void)sprintf(err_sng,"Saving attribute %s@%s %d dimensional variable",$1.var_nm,$1.att_nm,$3->nbr_dim);
    (void)yyerror(err_sng); 
  }else{
    (void)sprintf(err_sng,"Warning: Cannot store in attribute %s@%s a variable with dimension %d",$1.var_nm,$1.att_nm,$3->nbr_dim);
    (void)yyerror(err_sng);
  } /* endif */
  $1.var_nm=(char *)nco_free($1.var_nm);
  $1.att_nm=(char *)nco_free($1.att_nm);
  (void)var_free($3); 
} /* end out_att_xpr '=' var_xpr */
| out_var_xpr '=' var_xpr 
{
  int rcd;
  int var_id;
  $3->nm=strdup($1);
  /* Is variable already in output file? */
  rcd=nco_inq_varid_flg(((prs_sct *)prs_arg)->out_id,$3->nm,&var_id);
  if(rcd == NC_NOERR){
    (void)sprintf(err_sng,"Warning: Variable %s has aleady been saved in %s",$3->nm,((prs_sct *)prs_arg)->fl_out);
    (void)yyerror(err_sng);                                   
  }else{  
    (void)ncap_var_write($3,(prs_sct *)prs_arg);
    if(dbg_lvl_get() > 0) (void)sprintf(err_sng,"Saving variable %s to %s",$3->nm,((prs_sct *)prs_arg)->fl_out);
    (void)yyerror(err_sng);
  } /* end else */
  $1=(char *)nco_free($1);
  (void)var_free($3);
} /* end out_var_xpr '=' var_xpr */
| out_var_xpr '=' scv_xpr
{
  int rcd;
  int var_id;
  var_sct *var;
  rcd=nco_inq_varid_flg(((prs_sct *)prs_arg)->out_id,$1,&var_id);
  if(dbg_lvl_get() > 5) (void)fprintf(stderr,"%s: DEBUG out_var_xpr = scv_xpr rule for %s\n",prg_nm_get(),$1);
  if(rcd == NC_NOERR){
    (void)sprintf(err_sng,"Warning: Variable %s has aleady been saved in %s",$1,((prs_sct *)prs_arg)->fl_out);
    (void)yyerror(err_sng);
  }else{  
    /* Turn attribute into temporary variable for writing */
    var=(var_sct *)nco_malloc(sizeof(var_sct));
    /* Set defaults */
    (void)var_dfl_set(var); /* [fnc] Set defaults for each member of variable structure */
    /* Overwrite with attribute expression information */
    var->nm=strdup($1);
    var->nbr_dim=0;
    var->sz=1;
    var->val=ncap_scv_2_ptr_unn($3);
    var->type=$3.type;

    if(((prs_sct *)prs_arg)->var_LHS != NULL){
      /* User intends LHS to cast RHS to same dimensionality
	 Stretch newly initialized variable to size of LHS template */
      /*    (void)ncap_var_conform_dim(&$$,&(((prs_sct *)prs_arg)->var_LHS));*/
      (void)ncap_var_stretch(&var,&(((prs_sct *)prs_arg)->var_LHS));
      
      if(dbg_lvl_get() > 2) (void)fprintf(stderr,"%s: Stretching former scv_xpr defining %s with LHS template: Template var->nm %s, var->nbr_dim %d, var->sz %li\n",prg_nm_get(),$1,((prs_sct *)prs_arg)->var_LHS->nm,((prs_sct *)prs_arg)->var_LHS->nbr_dim,((prs_sct *)prs_arg)->var_LHS->sz);
      
    } /* endif LHS_cst */
    
    (void)ncap_var_write(var,(prs_sct *)prs_arg);
    (void)var_free(var);
    if(dbg_lvl_get() > 0) (void)sprintf(err_sng,"Saving variable %s to %s",$1,((prs_sct *)prs_arg)->fl_out);
    (void)yyerror(err_sng);
  } /* endif */
  $1=(char *)nco_free($1);
} /* end out_var_xpr '=' scv_xpr */
| out_var_xpr '=' sng_xpr
{
  int rcd;
  int var_id;
  var_sct *var;
  rcd=nco_inq_varid_flg(((prs_sct *)prs_arg)->out_id,$1,&var_id);
  if(rcd == NC_NOERR){
    (void)sprintf(err_sng,"Warning: Variable %s has aleady been saved in %s",$1,((prs_sct *)prs_arg)->fl_out);
    (void)yyerror(err_sng);  
  }else{  
    var=(var_sct *)nco_calloc((size_t)1,sizeof(var_sct));
    var->nm=strdup($1);
    var->nbr_dim=0;
    var->dmn_id=(int *)NULL;
    var->sz=strlen($3)+1;
    var->val.cp=(unsigned char *)strdup($3);
    var->type=NC_CHAR;
    (void)cast_nctype_void(NC_CHAR,&var->val);
    (void)ncap_var_write(var,(prs_sct *)prs_arg);
    (void)var_free(var);
    if(dbg_lvl_get() > 0) (void)sprintf(err_sng,"Saving variable %s to %s",$1,((prs_sct *)prs_arg)->fl_out);
    (void)yyerror(err_sng);
  } /* endelse */
  $1=(char *)nco_free($1);
  $3=(char *)nco_free($3);
} /* end out_var_xpr '=' sng_xpr */
; /* end statement */

scv_xpr: /* scv_xpr results from RHS action which involves only scv_xpr's
	    One action exists for each binary and unary attribute-valid operator */
scv_xpr '+' scv_xpr {
  (void)ncap_scv_scv_cnf_typ_hgh_prc(&$1,&$3);
  $$=ncap_scv_calc($1,'+',$3);                                
}
| scv_xpr '-' scv_xpr {
  (void)ncap_scv_scv_cnf_typ_hgh_prc(&$1,&$3); 
  $$=ncap_scv_calc($1,'-',$3);
}
| scv_xpr '*' scv_xpr {
  (void)ncap_scv_scv_cnf_typ_hgh_prc(&$1,&$3);
  $$=ncap_scv_calc($1,'*',$3);
}
| scv_xpr '/' scv_xpr {
  (void)ncap_scv_scv_cnf_typ_hgh_prc(&$1,&$3); 
  $$=ncap_scv_calc($1,'/',$3);  
}
| scv_xpr '%' scv_xpr {
  (void)ncap_scv_scv_cnf_typ_hgh_prc(&$1,&$3);
  
  $$=ncap_scv_calc($1,'%',$3);  
}
| '-' scv_xpr  %prec UMINUS {
  (void)ncap_scv_minus(&$2);
  $$=$2;
}
| '+' scv_xpr  %prec UMINUS {
  $$=$2;
}
| scv_xpr '^' scv_xpr {
  if($1.type <= NC_FLOAT && $3.type <= NC_FLOAT) {
    (void)scv_conform_type(NC_FLOAT,&$1);
    (void)scv_conform_type(NC_FLOAT,&$3);
    $$.val.f=powf($1.val.f,$3.val.f);
    $$.type=NC_FLOAT;
  }else{
    (void)scv_conform_type(NC_DOUBLE,&$1);
    (void)scv_conform_type(NC_DOUBLE,&$3);
    $$.val.d=pow($1.val.d,$3.val.d);
    $$.type=NC_DOUBLE; 
  } /* end else */
} /* end scv_xpr '^' scv_xpr */
| POWER '(' scv_xpr ',' scv_xpr ')' { /* fxm: this is identical to previous clause except for argument numbering, should be functionalized to use common code */
  if($3.type <= NC_FLOAT && $5.type <= NC_FLOAT) {
    (void)scv_conform_type(NC_FLOAT,&$3);
    (void)scv_conform_type(NC_FLOAT,&$5);
    $$.val.f=powf($3.val.f,$5.val.f);
    $$.type=NC_FLOAT;
  }else{ 
    (void)scv_conform_type(NC_DOUBLE,&$3);
    (void)scv_conform_type(NC_DOUBLE,&$5);
    $$.val.d=pow($3.val.d,$5.val.d);
    $$.type=NC_DOUBLE; 
  } /* end else */
} /* end POWER '(' scv_xpr ',' scv_xpr ')' */
| ABS '(' scv_xpr ')' {
  $$=ncap_scv_abs($3);
}
| FUNCTION '(' scv_xpr ')' {
  if($3.type <= NC_FLOAT) {
    (void)scv_conform_type(NC_FLOAT,&$3);
    $$.val.f=(*($1->flt_flt))($3.val.f);
    $$.type=NC_FLOAT;
  }else{
    $$.val.d=(*($1->fnc_dbl))($3.val.d);
    $$.type=NC_DOUBLE;
  } /* end else */
} /* end FUNCTION '(' scv_xpr ')' */
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
} 
| ATOSTR '(' scv_xpr ')' {
  char bfr[50];
  switch ($3.type){
  case NC_DOUBLE: sprintf(bfr,"%.10G",$3.val.d); break;
  case NC_FLOAT: sprintf(bfr,"%G",$3.val.f); break;
  case NC_INT: sprintf(bfr,"%ld",$3.val.l); break;
  case NC_SHORT: sprintf(bfr,"%d",$3.val.s); break;
  case NC_BYTE: sprintf(bfr,"%d",$3.val.b); break;
  default:  break;
  } /* end switch */
  $$=strdup(bfr);      
}
| ATOSTR '(' scv_xpr ',' sng_xpr ')' {
  char bfr[150];
  /* Format string according to string expression */
  /* User decides which format corresponds to which type */
  switch ($3.type){
  case NC_DOUBLE: sprintf(bfr,$5,$3.val.d); break;
  case NC_FLOAT: sprintf(bfr,$5,$3.val.f); break;
  case NC_INT: sprintf(bfr,$5,$3.val.l); break;
  case NC_SHORT: sprintf(bfr,$5,$3.val.s); break;
  case NC_BYTE: sprintf(bfr,$5,$3.val.b); break;
  default:  break;
  } /* end switch */
  $5=(char *)nco_free($5);
  $$=strdup(bfr);      
}
| SNG {$$=$1;}
; /* end sng_xpr */

var_xpr: /* var_xpr results from RHS action which involves a var_xpr, i.e.,
	    OP var, var OP var, var OP att, att OP var */
var_xpr '+' var_xpr { 
  $$=ncap_var_var_add($1,$3); 
  var_free($1); var_free($3);
}
| var_xpr '+' scv_xpr {
  $$=ncap_var_scv_add($1,$3);
  var_free($1);
}            
| scv_xpr '+' var_xpr {
  $$=ncap_var_scv_add($3,$1);
  var_free($3);
}            
| var_xpr '-' var_xpr { 
  $$=ncap_var_var_sub($1,$3);
  var_free($1); 
  var_free($3);
}
| scv_xpr '-' var_xpr { 
  var_sct *var1;
  scv_sct minus;
  minus.val.b=-1;
  minus.type=NC_BYTE;
  (void)scv_conform_type($3->type,&minus);
  var1=ncap_var_scv_sub($3,$1);
  $$=ncap_var_scv_multiply(var1,minus);
  var_free(var1);
  var_free($3);
}
| var_xpr '-' scv_xpr {
  $$=ncap_var_scv_sub($1,$3);
  var_free($1);
}
| var_xpr '*' var_xpr {
  $$=ncap_var_var_multiply($1,$3); 
  var_free($1); var_free($3); 
}
| var_xpr '*' scv_xpr {
  $$=ncap_var_scv_multiply($1,$3);
  var_free($1);
}
| var_xpr '%' scv_xpr {
  $$=ncap_var_scv_modulus($1,$3);
  var_free($1);
}
| scv_xpr '*' var_xpr {
  $$=ncap_var_scv_multiply($3,$1);
  var_free($3);
}
| var_xpr '/' var_xpr {
  $$=ncap_var_var_divide($3,$1); /* NB: Ordering is important */
  var_free($1); var_free($3); 
}
| var_xpr '/' scv_xpr {
  $$=ncap_var_scv_divide($1,$3);
  var_free($1);
}
| var_xpr '^' scv_xpr {
  $$=ncap_var_scv_power($1,$3);
  var_free($1);
}
| POWER '(' var_xpr ',' scv_xpr ')' {
  $$=ncap_var_scv_power($3,$5);
  var_free($3);
}
| '-' var_xpr %prec UMINUS { 
  scv_sct minus;
  minus.val.b=-1;
  minus.type=NC_BYTE;
  $$=ncap_var_scv_multiply($2,minus);
  var_free($2);      
}
| '+' var_xpr %prec UMINUS {
  $$=$2;
}
| ABS '(' var_xpr ')' {
  $$=ncap_var_abs($3);
  var_free($3);
} /* end ABS */
| RDC '(' var_xpr ')' {
  $$=ncap_var_abs($3);
  /* fxm Finish avg,min,max,ttl */
  /* int nco_op_typ=nco_op_avg; *//* [enm] Operation type */
  /* $$=var_avg($3,dim,dmn_nbr,nco_op_typ); */
  (void)fprintf(stderr,"%s: WARNING RDC tokens not implemented yet\n",prg_nm_get());
  /* $3 is freed in var_avg() */
} /* end ABS */
| PACK '(' var_xpr ')' {
  /* Packing variable does not create duplicate so DO NOT free $3 */
  $$=var_pck($3,NC_SHORT,False);
} /* end PACK */
| UNPACK '(' var_xpr ')' {
  /* Unpacking variable does not create duplicate so DO NOT free $3 */
  $$=var_upk($3);
} /* end UNPACK */
| FUNCTION '(' var_xpr ')' {
  $$=ncap_var_function($3,$1);
  var_free($3);
}  
| '(' var_xpr ')' {
  $$=$2;
}
| VAR { 
  $$=ncap_var_init($1,(prs_sct *)prs_arg);

  if((((prs_sct *)prs_arg)->var_LHS) != NULL){
    /* User intends LHS to cast RHS to same dimensionality
       Stretch newly initialized variable to size of LHS template */
    /*    (void)ncap_var_conform_dim(&$$,&(((prs_sct *)prs_arg)->var_LHS));*/
    (void)ncap_var_stretch(&$$,&(((prs_sct *)prs_arg)->var_LHS));

    if(dbg_lvl_get() > 2) (void)fprintf(stderr,"%s: Stretching variable %s with LHS template: Template var->nm %s, var->nbr_dim %d, var->sz %li\n",prg_nm_get(),$$->nm,((prs_sct *)prs_arg)->var_LHS->nm,((prs_sct *)prs_arg)->var_LHS->nbr_dim,((prs_sct *)prs_arg)->var_LHS->sz);

  } /* endif LHS_cst */

  /* Sanity check */
  if ($$==(var_sct *)NULL) YYERROR;
}
; /* end var_xpr */

/* End Rules section */
%%
/* Begin User Subroutines section */

int
ncap_aed_lookup(char *var_nm,char *att_nm,aed_sct **att_lst,int *nbr_att,bool update)
{
  int att_idx;
  for(att_idx=0;att_idx<*nbr_att;att_idx++)
    if (!strcmp(att_lst[att_idx]->att_nm,att_nm) && !strcmp(att_lst[att_idx]->var_nm,var_nm)){
      /* Free memory if we are doing an update */
      if(update) free(att_lst[att_idx]->val.vp);   
      return att_idx;
    } /* end if */

  if(!update) return -1;
  
  att_lst[*nbr_att]=(aed_sct *)nco_malloc(sizeof(aed_sct));
  att_lst[*nbr_att]->var_nm=strdup(var_nm);
  att_lst[*nbr_att]->att_nm=strdup(att_nm);
  
  return (*nbr_att)++;
} /* end ncap_aed_lookup */

int
yyerror(char *err_sng)
{
  /* Use eprokoke_skip to skip error message after sending error message from yylex()
     Stop provoked error message from yyparse being printed */

  static bool eprovoke_skip;
  
  /* if(eprovoke_skip){eprovoke_skip=False ; return 0;} */
  if(dbg_lvl_get() > 0) (void)fprintf(stderr,"%s: %s line %ld",prg_nm_get(),fl_spt_glb,ln_nbr_crr[ncl_dpt_crr]);
  if(dbg_lvl_get() > 1) (void)fprintf(stderr," %s",err_sng);
  if(dbg_lvl_get() > 0) (void)fprintf(stderr,"\n");
  
  if(err_sng[0] == '#') eprovoke_skip=True;
  eprovoke_skip=eprovoke_skip; /* Do nothing except avoid compiler warnings */
  return 0;
} /* end yyerror() */

void 
nco_var_free /* [fnc] Safely free variable */
(var_sct **var) /* [sct] Variable */
{
  /* Purpose: Safely free variable
     Routine is wrapper for var_free() that simplifies code in calling routine */
  if(*var != NULL) *var=var_free(*var);
} /* end nco_var_free() */
