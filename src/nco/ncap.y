%{ /* $Header: /data/zender/nco_20150216/nco/src/nco/ncap.y,v 1.34 2002-01-27 06:14:05 zender Exp $ -*-C-*- */

/* Begin C declarations section */
 
/* Purpose: Grammar parser for ncap */

/* Copyright (C) 1995--2002 Charlie Zender

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.
   
   As a special exception to the terms of the GPL, you are permitted 
   to link the NCO source code with the NetCDF and HDF libraries 
   and distribute the resulting executables under the terms of the GPL, 
   but in addition obeying the extra stipulations of the netCDF and 
   HDF library licenses.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

   The file LICENSE contains the GNU General Public License, version 2
   It may be viewed interactively by typing, e.g., ncks -L

   The author of this software, Charlie Zender, would like to receive
   your suggestions, improvements, bug-reports, and patches for NCO.
   Please contact the project at http://sourceforge.net/projects/nco or by writing

   Charlie Zender
   Department of Earth System Science
   University of California at Irvine
   Irvine, CA 92697-3100
*/

  /* Example yacc text:
     /data/zender/ora/lexyacc/ch3-05.y
     /home/thibaud/usr/local/src/gcc-2.7.2/c-parse.y
     /data/zender/gcc-2.7.2/c-parse.y
     parser_build_binary_op is in /data/zender/gcc-2.7.2/c-typeck.c
     unidata ncgen.y
  */

/* Standard header files */
#include <math.h> /* sin cos cos sin 3.14159 */
#include <stdlib.h> /* atof, atoi, malloc, getopt */
#include <string.h> /* strcmp. . . */
#include <stdio.h> /* stderr, FILE, NULL, etc. */

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
/* prs_sct must be consistent between ncap.y and ncap.c 
   fxm: Is there a way to define prs_sct in only one place? */

#define YYPARSE_PARAM prs_arg
#define YYLEX_PARAM prs_arg 
int rcd; /* [enm] Return value for function calls */

/* Global variables */
extern long ln_nbr_crr; /* [cnt] Line number incremented in ncap.l */
extern char *fl_spt_glb; /* [fl] Script file */

/* File scope variables */
/* fxm: turn arbitrary size into pre-processor token */
char err_sng[200]; /* Error string for short error messages */

/* End C declarations section */
%}
/* Begin parser declaration section */

/* Request pure, reentrant parser, so we can pass a structure to parser
   fxm: 20020122 Code breaks on Linux when pure_parser is not used---why?
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
  char *str;
  char *output_var;
  char *vara;
  aed_sct att;
  sym_sct *sym;
  parse_sct attribute;
  var_sct *var;
} /* end YYSTYPE union (type of yylval value) */

/* Tell parser which kind of values each token takes
   Token name (traditionally in all caps) becomes #define directive in parser
   so we can refer to token name rather than token's numeric code */
%token <str> STRING
%token <attribute> ATTRIBUTE
%token <vara> VAR
%token <output_var> OUT_VAR
%token <att> OUT_ATT
%token <sym> FUNCTION
%token POWER ABS ATOSTR IGNORE EPROVOKE

/* "type" declaration sets type for non-terminal symbols which otherwise need no declaration */
%type <attribute> att_exp
%type <str> string_exp
%type <var> var_exp
%type <output_var> out_var_exp
%type <att> out_att_exp

/* "left", "right", and "nonassoc" perform same function as "token" and,
   in addition, specify associativity and relative precedense of symbols */
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

statement_list: statement_list statement ';'
| statement_list error ';'
| statement ';'
| error ';' /* Catches most errors then reads up to next semicolon */
;

statement: out_att_exp '=' att_exp { 
  int aed_idx; 
  aed_sct *ptr_aed;
  
  aed_idx=ncap_aed_lookup($1.var_nm,$1.att_nm,((prs_sct *)prs_arg)->att_lst,((prs_sct *)prs_arg)->nbr_att,True);
  ptr_aed=((prs_sct *)prs_arg)->att_lst[aed_idx];                               
  ptr_aed->val=ncap_attribute_2_ptr_unn($3);
  ptr_aed->type=$3.type;
  ptr_aed->sz=1L;
  (void)cast_nctype_void(ptr_aed->type,&ptr_aed->val);    
  (void)sprintf(err_sng,"Saving attribute %s@%s to %s",$1.var_nm,$1.att_nm,((prs_sct *)prs_arg)->fl_out);
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
  (void)free($1.var_nm);
  (void)free($1.att_nm);
} /* end out_att_exp '=' att_exp */
| out_att_exp '=' string_exp 
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
  
  (void)sprintf(err_sng,"Saving attribute %s@%s=%s",$1.var_nm,$1.att_nm,$3);
  (void)yyerror(err_sng);
  (void)free($1.var_nm);
  (void)free($1.att_nm);
  (void)free($3);
} /* end out_att_exp '=' string_exp */
| out_att_exp '=' var_exp
{ 
  /* It is OK to store 0 dimensional variables in an attribute */ 
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
    (void)sprintf(err_sng,"Saving attribute %s@%s %d dimensional variable",$1.var_nm,$1.att_nm,$3->nbr_dim);
    (void)yyerror(err_sng); 
  }else{
    (void)sprintf(err_sng,"Warning: Cannot store in attribute %s@%s a variable with dimension %d",$1.var_nm,$1.att_nm,$3->nbr_dim);
    (void)yyerror(err_sng);
  } /* endif */
  (void)free($1.var_nm);
  (void)free($1.att_nm);
  (void)var_free($3); 
} /* end out_att_exp '=' var_exp */
| out_var_exp '=' var_exp 
{
  int rcd;
  int var_id;
  $3->nm=strdup($1);
  /* fxm: If LHS was cast by user then must cast RHS to same dimensionality */
  /* Is variable already in output file? */
  rcd=nco_inq_varid_flg(((prs_sct *)prs_arg)->out_id,$3->nm,&var_id);
  if(rcd == NC_NOERR){
    (void)sprintf(err_sng,"Warning: Variable %s has aleady been saved in %s",$3->nm,((prs_sct *)prs_arg)->fl_out);
    (void)yyerror(err_sng);                                   
  }else{  
    (void)ncap_var_write($3,(prs_sct *)prs_arg);
    (void)sprintf(err_sng,"Saving variable %s to %s",$3->nm,((prs_sct *)prs_arg)->fl_out);
    (void)yyerror(err_sng);
  } /* end else */
  (void)free($1);
  (void)var_free($3);
} /* end out_var_exp '=' var_exp */
| out_var_exp '=' att_exp
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
    var->sz=1;
    var->val=ncap_attribute_2_ptr_unn($3);
    var->type=$3.type;
    (void)ncap_var_write(var,(prs_sct *)prs_arg);
    (void)var_free(var);
    (void)sprintf(err_sng,"Saving variable %s to %s",$1,((prs_sct *)prs_arg)->fl_out);
    (void)yyerror(err_sng);
  }
  (void)free($1);
} /* end out_var_exp '=' att_exp */
| out_var_exp '=' string_exp
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
    (void)sprintf(err_sng,"Saving variable %s to %s",$1,((prs_sct *)prs_arg)->fl_out);
    (void)yyerror(err_sng);
  }
  (void)free($1);
  (void)free($3);
} /* end out_var_exp '=' string_exp */
;                    

att_exp: att_exp '+' att_exp {
  (void)ncap_retype(&$1,&$3);
  $$=ncap_attribute_calc($1,'+',$3);                                
}
|  att_exp '-' att_exp {
  (void)ncap_retype(&$1,&$3); 
  $$=ncap_attribute_calc($1,'-',$3);
}
| att_exp '*' att_exp {
  (void)ncap_retype(&$1,&$3);
  $$=ncap_attribute_calc($1,'*',$3);
}
| att_exp '/' att_exp {
  (void)ncap_retype(&$1,&$3); 
  $$=ncap_attribute_calc($1,'/',$3);  
}
| att_exp '%' att_exp {
  (void)ncap_retype(&$1,&$3);
  
  $$=ncap_attribute_calc($1,'%',$3);  
}
| '-' att_exp  %prec UMINUS {
  (void)ncap_attribute_minus(&$2);
  $$=$2;
}
| '+' att_exp  %prec UMINUS {
  $$=$2;
}
| att_exp '^' att_exp {
  if($1.type <= NC_FLOAT && $3.type <= NC_FLOAT) {
    (void)ncap_attribute_conform_type(NC_FLOAT,&$1);
    (void)ncap_attribute_conform_type(NC_FLOAT,&$3);
    $$.val.f=powf($1.val.f,$3.val.f);
    $$.type=NC_FLOAT;
  } else { 
  (void)ncap_attribute_conform_type(NC_DOUBLE,&$1);
  (void)ncap_attribute_conform_type(NC_DOUBLE,&$3);
  $$.val.d=pow($1.val.d,$3.val.d);
  $$.type=NC_DOUBLE; 
  }
}
| POWER '(' att_exp ',' att_exp ')' {
  if($3.type <= NC_FLOAT && $5.type <= NC_FLOAT) {
    (void)ncap_attribute_conform_type(NC_FLOAT,&$3);
    (void)ncap_attribute_conform_type(NC_FLOAT,&$5);
    $$.val.f=powf($3.val.f,$5.val.f);
    $$.type=NC_FLOAT;
  } else { 
  (void)ncap_attribute_conform_type(NC_DOUBLE,&$3);
  (void)ncap_attribute_conform_type(NC_DOUBLE,&$5);
  $$.val.d=pow($3.val.d,$5.val.d);
  $$.type=NC_DOUBLE; 
  }
}

| ABS '(' att_exp ')' {
  $$=ncap_attribute_abs($3);
}
| FUNCTION '(' att_exp ')' {
  
  if($3.type <= NC_FLOAT) {
    (void)ncap_attribute_conform_type(NC_FLOAT,&$3);
    $$.val.f=(*($1->fncf))($3.val.f);
    $$.type=NC_FLOAT;
  } else {
  $$.val.d=(*($1->fnc))($3.val.d);
  $$.type=NC_DOUBLE;
  }
}
| '(' att_exp ')' {$$=$2;}
| ATTRIBUTE {$$=$1;}
;

out_var_exp: OUT_VAR {$$=$1;}
;

out_att_exp: OUT_ATT {$$=$1;}
;
      
string_exp: string_exp '+' string_exp {
  size_t sng_lng;
  sng_lng=strlen($1)+strlen($3);
  $$=(char*)nco_malloc((sng_lng+1)*sizeof(char));
  strcpy($$,$1);
  strcat($$,$3);
  (void)free($1);
  (void)free($3);
} 
| ATOSTR '(' att_exp ')' {
  char bfr[50];
  switch ($3.type){
  case  NC_DOUBLE: sprintf(bfr,"%.10G",$3.val.d); break;
  case  NC_FLOAT:  sprintf(bfr,"%G",$3.val.f); break;
  case  NC_INT:    sprintf(bfr,"%ld",$3.val.l); break;
  case  NC_SHORT:  sprintf(bfr,"%d",$3.val.s); break;
  case  NC_BYTE:   sprintf(bfr,"%d",$3.val.b); break;
  default:  break;
  } /* end switch */
  $$=strdup(bfr);      
}
| ATOSTR '(' att_exp ',' string_exp ')' {
  char bfr[150];
  /* Format string according to string expression */
  /* User decides which format corresponds to which type */
  switch ($3.type){
  case  NC_DOUBLE: sprintf(bfr,$5,$3.val.d); break;
  case  NC_FLOAT:  sprintf(bfr,$5,$3.val.f); break;
  case  NC_INT:    sprintf(bfr,$5,$3.val.l); break;
  case  NC_SHORT:  sprintf(bfr,$5,$3.val.s); break;
  case  NC_BYTE:   sprintf(bfr,$5,$3.val.b); break;
  default:  break;
  } /* end switch */
  (void)free($5);
  $$=strdup(bfr);      
}
| STRING {$$=$1;}
;

var_exp: var_exp '+' var_exp { 
  $$=ncap_var_var_add($1,$3); 
  var_free($1); var_free($3);
}
| var_exp '+' att_exp {
  $$=ncap_var_attribute_add($1,$3);
  var_free($1);
}            
| att_exp '+' var_exp {
  $$=ncap_var_attribute_add($3,$1);
  var_free($3);
}            
| var_exp '-' var_exp { 
  $$=ncap_var_var_sub($1,$3);
  var_free($1); 
  var_free($3);
}
| att_exp '-' var_exp { 
  var_sct *var1;
  parse_sct minus;
  minus.val.b=-1;
  minus.type=NC_BYTE;
  (void)ncap_attribute_conform_type($3->type,&minus);
  var1=ncap_var_attribute_sub($3,$1);
  $$=ncap_var_attribute_multiply(var1,minus);
  var_free(var1);
  var_free($3);
}
| var_exp '-' att_exp {
  $$=ncap_var_attribute_sub($1,$3);
  var_free($1);
}
| var_exp '*' var_exp {
  $$=ncap_var_var_multiply($1,$3); 
  var_free($1); var_free($3); 
}
| var_exp '*' att_exp {
  $$=ncap_var_attribute_multiply($1,$3);
  var_free($1);
}
| var_exp '%' att_exp {
  $$=ncap_var_attribute_modulus($1,$3);
  var_free($1);
}
| att_exp '*' var_exp {
  $$=ncap_var_attribute_multiply($3,$1);
  var_free($3);
}
| var_exp '/' var_exp {
  $$=ncap_var_var_divide($1,$3); 
  var_free($1); var_free($3); 
}
| var_exp '/' att_exp {
  $$=ncap_var_attribute_divide($1,$3);
  var_free($1);
}
| var_exp '^' att_exp {
  $$=ncap_var_attribute_power($1,$3);
  var_free($1);
}
| POWER '(' var_exp ',' att_exp ')' {
  $$=ncap_var_attribute_power($3,$5);
  var_free($3);
}
| '-' var_exp %prec UMINUS { 
  parse_sct minus;
  minus.val.b=-1;
  minus.type=NC_BYTE;
  $$=ncap_var_attribute_multiply($2,minus);
  var_free($2);      
}
| '+' var_exp %prec UMINUS {
  $$=$2;
}
| ABS '(' var_exp ')' {
  $$=ncap_var_abs($3);
  var_free($3);
} 
| FUNCTION '(' var_exp ')' {
  $$=ncap_var_function($3,$1);
  var_free($3);
}  
| '(' var_exp ')' {
  $$=$2;
}
| VAR { 
  $$=ncap_var_init($1,((prs_sct *)prs_arg));
  if ( $$==(var_sct *)NULL ) YYERROR;
}
;
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
  (void)fprintf(stderr,"%s: %s line %ld %s\n",prg_nm_get(),fl_spt_glb,ln_nbr_crr,err_sng);
  
  if(err_sng[0] == '#') eprovoke_skip=True;
  eprovoke_skip=eprovoke_skip; /* Do nothing except avoid compiler warnings */
  return 0;
} /* end yyerror() */
