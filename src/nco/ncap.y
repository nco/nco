%{
/* Begin C declarations section */ 

/* $Header: /data/zender/nco_20150216/nco/src/nco/ncap.y,v 1.3 1999-05-12 03:06:47 zender Exp $ */

/* Purpose: Grammar parser for ncap */ 

/* (c) Copyright 1995--1998University Corporation for Atmospheric Research/
   National Center for Atmospheric Research/
   Clmt.te and Global Dynamics Division

   The file LICENSE contains the full copyright notice, or 
   you may contact NSF/UCAR/NCAR/CGD/CMS for copyright assistance. */

  /* Example yacc text:
     /data2/zender/ora/lexyacc/ch3-05.y
     /home/thibaud/usr/local/src/gcc-2.7.2/c-parse.y
     /data2/zender/gcc-2.7.2/c-parse.y
     parser_build_binary_op is in /data2/zender/gcc-2.7.2/c-typeck.c
     unidata ncgen.y
     */ 

/* Standard header files */
#include <math.h>               /* sin cos cos sin 3.14159 */
#include <stdlib.h>             /* atof, atoi, malloc, getopt */ 
#include <string.h>             /* strcmp. . . */
#include <stdio.h>              /* stderr, FILE, NULL, etc. */

#include <netcdf.h>             /* netCDF def'ns */
#include "nc.h"                 /* global definitions */
#include "ncap.h"               /* symbol table definition */ 

/* Turn on parser debugging option (bison man p. 85) */ 
#define YYDEBUG 1
int yydebug=0;

/* Turns on more verbose errors than just plain "parse error" when yyerror() is called by parser */ 
#define YYERROR_VERBOSE 1

/* Add descriptive info to parser debugging output (bison man p. 86) */ 
#define YYPRINT(file,type,value) yyprint(file,type,value)

/* Bison manual p. 60 describes how to call yyparse() with arguments */ 
/* prs_sct must be consistent between ncap.y and ncap.c 
   DBG XXX: Is there a way to define prs_sct in only one place? */ 
typedef struct{
    char *fl_in;
    int in_id;  
    char *fl_out;
    int out_id;  
    char *sng;
    dim_sct **dim;
    int nbr_dim_xtr;
} prs_sct;
#define YYPARSE_PARAM prs_arg

 int rcd; /* return value for function calls */ 

/* End C declarations section */ 
%}
/* Begin parser declaration section */ 

/* Request a pure, reentrant parser, so we can pass the parser a structure */ 
%pure_parser

/* NB: A "terminal symbol" is just a fancy name for a token produced by the lexer. 
   Symbols defined on the LHS of rules are called "non-terminal symbols" or "non-terminals". 
   Examples of non-terminals are xpr, stt, stt_lst.
   Examples of terminal symbols, or tokens, are NAME, NUMBER
   The convention is to make token names all uppercase, and non-terminals lowercase. */ 

/* Define the YYSTYPE union (the type of the lex variable yylval) */
%union{
  double val_double; /* store input in double precision */ 
  sym_sct *sym; /* pointer to entry in the symbol table */ 
  var_sct var;
}

/* Tell the parser which kind of values each token takes */
%token <val_double> NUMBER
%token <sym> NAME

/* Set the precedence and associativity of the arithmetic expressions, the "literal tokens".
   Precedence levels are declared lowest to highest. */
%left '-' '+'
%left '*' '/'
%left '^'
%nonassoc UMINUS

/* The type declaration sets the type for non-terminal symbols which otherwise need no declaration */ 
/*%type <val_double> xpr*/
%type <sym> xpr
/*%type <var> xpr*/

/* End parser declaration section */ 
%%
/* Begin Rules section */
/* Format is rule: action */ 

/* NB: yacc automatically dereferences the correct member of each token's structure.
   Thus, if the third symbol is a NUMBER, a reference to $3 acts like $3.val_double */

/* NB: The $$ symbol refers to the value for the symbol to the left of the colon */

/* A statement list can be a single line or a collection of statements separated by newlines */
 stt_lst: stt '\n'
| stt_lst stt '\n'
;

 stt: NAME '=' xpr { 
   $1=$3; 
   if(dbg_lvl_get() > 2) (void)fprintf(stderr,"stt: $1 = %s, $3 = %s\n",$1->nm,$3->nm); 
   if(dbg_lvl_get() > 2) (void)fprintf(stderr,"stt: $1->var->val.fp[0]=%g\n",$1->var->val.fp[0]); 
 }
;

 xpr: xpr '+' xpr { 
   $$->var=ncap_var_add($1->var,$3->var);
   $$->var->nm=$$->nm;
   if(dbg_lvl_get() > 0) (void)fprintf(stderr,"xpr: %s+%s\n",$1->nm,$3->nm); 
   if(dbg_lvl_get() > 0) (void)fprintf(stderr,"xpr: $$->var->nm= %s, $$->var->val.fp[0]=%g\n",$$->var->nm,$$->var->val.fp[0]); 
   /* Now the variable has been (re)defined. Save it to disk. */ 
   /*rcd=ncap_write_var(((prs_sct *)prs_arg)->out_id,$$->var);*/
 } /* end '+' */ 

/*| xpr '-' xpr { $$=$1-$3; }*/
/*| xpr '*' xpr { $$=$1*$3; }*/
/*| xpr '/' xpr { if($3 == 0.) yyerror("divide by zero"); else $$=$1/$3; }*/
/*| xpr '^' xpr { $$=pow($1,$3); }*/
/* The %prec UMINUS tells YACC to use the precedence of UMINUS for this rule */ 
/*| '-' xpr %prec UMINUS { $$=-$2; }*/
| '(' xpr ')' { $$=$2; }
/* | NUMBER { *//* Make the xpr a netCDF variable based on the NUMBER see pigeon book p. 58 "...not strictly necessary..." */ 
/*$$=scalar_mk_sym($1); }*/
/*| NUMBER { $$=$1; }*/ /* pigeon book p. 58 "...not strictly necessary..." */
| NAME { /* Lookup undefined NAME in the input file */
  if($1->var != NULL){
    $$->var=$1->var;
    if(dbg_lvl_get() > 0) (void)fprintf(stderr,"NAME: variable %s is already in symbol table\n",$1->nm); 
  }else{
    var_sct *var;
    int var_id;

    if(dbg_lvl_get() > 1) (void)fprintf(stderr,"NAME: getting %s from netCDF %s\n",$1->nm,((prs_sct *)prs_arg)->fl_in);

    /* Get the variable ID */ 
    var_id=ncvarid(((prs_sct *)prs_arg)->in_id,$1->nm);
    if(var_id == -1){
      (void)fprintf(stderr,"can't find %s in %s\n",$1->nm,((prs_sct *)prs_arg)->fl_in);
    }else{
      var=var_fll(((prs_sct *)prs_arg)->in_id,var_id,$1->nm,((prs_sct *)prs_arg)->dim,((prs_sct *)prs_arg)->nbr_dim_xtr);
      /* Allocate and initialize accumulation space for the variable */ 
      var->tally=(long *)malloc(var->sz*nctypelen(NC_LONG));
      (void)zero_long(var->sz,var->tally);
      var->val.vp=(void *)malloc(var->sz*nctypelen(var->type));
      /* Retrieve the variable values from disk into memory */ 
      (void)var_get(((prs_sct *)prs_arg)->in_id,var);
      if(dbg_lvl_get() > 3) (void)fprintf(stderr,"var->nm=%s, var->id=%d, var->nc_id=%d\n",var->nm,var->id,var->nc_id);
      $$->var=var;
    } /* endif */ 
  } /* endif */ 
} 
/*| NAME '(' xpr ')' {*/
  /* Assume a name followed by a parenthesized expression is a function */ 
/*  if($1->fnc){*/
/*    $$=($1->fnc)($3); */
/*  }else{*/
/*    (void)fprintf(stderr,"%s not a function\n",$1->nm);*/
/*    $$=0.;*/
/*  }*/ /* end else */ 
/*}*/
;

/* End Rules section */
%%
/* Begin User Subroutines section */

sym_sct *
scalar_mk_sym(double val)
{
  /* Routine to turn a scalar into a netCDF variable */ 
  sym_sct *sym;

  sym=(sym_sct *)malloc(sizeof(sym_sct));

  return sym;
} /* end scalar_mk_sym */ 

int 
yyprint(FILE *file,int type,YYSTYPE value)
{
  /* Add descriptive info to parser debugging output (bison man p. 86) */ 
  if(type == NAME){
    fprintf(file," %s",value.sym->nm);
  }else if(type == NUMBER){
    fprintf(file," %d",value.val_double);
  } /* end else */ 
} /* end yyprint() */ 

sym_sct *
sym_look(char *sym_nm)
/* 
   char *sym_nm: input name of symbol to locate/define
   sym_sct *sym_look(): output pointer to symbol
 */ 
{
  /* Routine to look up a symbol table entry. 
     If the symbol is present, the routine returns a pointer to the entry. 
     If the symbol is not present then it is added to the end of the symbol table. 
  */

  sym_sct *sym;
  
  for(sym=sym_tbl;sym<&sym_tbl[SYM_NBR_MAX];sym++){
    /* Is the requested symbol already in the symbol table? */
    if(sym->nm && !strcmp(sym->nm,sym_nm)) return sym;
    
    /* Is the current entry in the symbol table empty? i.e., Are we at the end of the valid entries?
       If so, enter the new symbol in the current slot. */
    if(!sym->nm){
      sym->nm=(char *)strdup(sym_nm);
      return sym;
    } /* end if */ 
  } /* end for */ 
  yyerror("Too many symbols");
  exit(EXIT_FAILURE);
} /* end sym_look() */

void
fnc_add(char *nm, double (*fnc)())
/* 
   char *nm: input name of function to add to symbol table
   double (*fnc)(): entry point of function
 */ 
{
  /* Routine to add a function to the symbol table */ 

  sym_sct *sym;

  sym=sym_look(nm);
  sym->fnc=fnc;
} /* end fnc_add() */ 

/* End User Subroutines section */




