%{
/* Begin C declarations section */ 

/* $Header: /data/zender/nco_20150216/nco/src/nco/ncap.y,v 1.7 2000-07-31 00:29:18 zender Exp $ -*-C-*- */

/* Purpose: Grammar parser for ncap */ 

/* Copyright (C) 1995--2000 Charlie Zender

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.
   
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
   Please contact me via e-mail at zender@uci.edu or by writing

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
#include <math.h>               /* sin cos cos sin 3.14159 */
#include <stdlib.h>             /* atof, atoi, malloc, getopt */ 
#include <string.h>             /* strcmp. . . */
#include <stdio.h>              /* stderr, FILE, NULL, etc. */

#include <netcdf.h>             /* netCDF definitions */
#include "nc.h"                 /* NCO definitions */
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
   fxm: Is there a way to define prs_sct in only one place? */ 
typedef struct{
    char *fl_in;
    int in_id;  
    char *fl_out;
    int out_id;  
    char *sng;
    dmn_sct **dim;
    int nbr_dmn_xtr;
} prs_sct;
#define YYPARSE_PARAM prs_arg

 int rcd; /* [enm] Return value for function calls */ 

/* End C declarations section */ 
%}
/* Begin parser declaration section */ 

/* Request pure, reentrant parser, so we can pass a structure to parser */ 
%pure_parser

/* NB: "terminal symbol" is just a fancy name for token produced by lexer 
   Symbols defined on LHS of rules are called "non-terminal symbols" or "non-terminals" 
   Examples of non-terminals are xpr, stt, stt_lst
   Examples of terminal symbols, or tokens, are NAME, NUMBER
   Convention is to make token names all uppercase, and non-terminals lowercase */ 

/* Define YYSTYPE union (type of lex variable yylval) */
%union{
  double val_double; /* Store input in double precision */ 
  sym_sct *sym; /* Pointer to entry in symbol table */ 
  var_sct var;
}

/* Tell parser which kind of values each token takes */
%token <val_double> NUMBER
%token <sym> NAME

/* Set precedence and associativity of arithmetic expressions, "literal tokens".
   Precedence levels are declared lowest to highest. */
%left '-' '+'
%left '*' '/'
%left '^'
%nonassoc UMINUS

/* "type" declaration sets type for non-terminal symbols which otherwise need no declaration */
/*%type <val_double> xpr*/
%type <sym> xpr
/*%type <var> xpr*/

/* End parser declaration section */ 
%%
/* Begin Rules section */
/* Format is rule: action */ 

/* yacc automatically dereferences correct member of each token's structure
   Thus, if third symbol is a NUMBER, a reference to $3 acts like $3.val_double */

/* $$ symbol refers to value for symbol to left of colon */

/* Statement list can be single line or collection of statements separated by newlines */
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
   /* Now variable has been (re)defined. Save it to disk. */ 
   /*rcd=ncap_write_var(((prs_sct *)prs_arg)->out_id,$$->var);*/
 } /* end '+' */ 

/*| xpr '-' xpr { $$=$1-$3; }*/
/*| xpr '*' xpr { $$=$1*$3; }*/
/*| xpr '/' xpr { if($3 == 0.) yyerror("divide by zero"); else $$=$1/$3; }*/
/*| xpr '^' xpr { $$=pow($1,$3); }*/
/* The %prec UMINUS tells YACC to use the precedence of UMINUS for this rule */ 
/*| '-' xpr %prec UMINUS { $$=-$2; }*/
| '(' xpr ')' { $$=$2; }
/* | NUMBER { *//* Make xpr a netCDF variable based on NUMBER see pigeon book p. 58 "...not strictly necessary..." */ 
/*$$=scalar_mk_sym($1); }*/
/*| NUMBER { $$=$1; }*/ /* pigeon book p. 58 "...not strictly necessary..." */
| NAME { /* Lookup undefined NAME in input file */
  if($1->var != NULL){
    $$->var=$1->var;
    if(dbg_lvl_get() > 0) (void)fprintf(stderr,"NAME: variable %s is already in symbol table\n",$1->nm); 
  }else{
    var_sct *var;
    int var_id;

    if(dbg_lvl_get() > 1) (void)fprintf(stderr,"NAME: getting %s from netCDF %s\n",$1->nm,((prs_sct *)prs_arg)->fl_in);

    /* Get variable ID */ 
    var_id=ncvarid(((prs_sct *)prs_arg)->in_id,$1->nm);
    if(var_id == -1){
      (void)fprintf(stderr,"can't find %s in %s\n",$1->nm,((prs_sct *)prs_arg)->fl_in);
    }else{
      var=var_fll(((prs_sct *)prs_arg)->in_id,var_id,$1->nm,((prs_sct *)prs_arg)->dim,((prs_sct *)prs_arg)->nbr_dmn_xtr);
      /* Allocate and initialize accumulation space for variable */ 
      var->tally=(long *)malloc(var->sz*nctypelen(NC_LONG));
      (void)zero_long(var->sz,var->tally);
      var->val.vp=(void *)malloc(var->sz*nctypelen(var->type));
      /* Retrieve variable values from disk into memory */ 
      (void)var_get(((prs_sct *)prs_arg)->in_id,var);
      if(dbg_lvl_get() > 3) (void)fprintf(stderr,"var->nm=%s, var->id=%d, var->nc_id=%d\n",var->nm,var->id,var->nc_id);
      $$->var=var;
    } /* endif */ 
  } /* endif */ 
} 
/*| NAME '(' xpr ')' {*/
  /* Assume name followed by parenthesized expression is function */ 
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
  /* Purpose: Turn scalar into netCDF variable */ 
  sym_sct *sym;

  sym=(sym_sct *)malloc(sizeof(sym_sct));

  return sym;
} /* end scalar_mk_sym */ 

int 
yyprint(FILE *file,int type,YYSTYPE value)
{
  /* Purpose: Add descriptive info to parser debugging output (bison man p. 86) */ 
  if(type == NAME){
    fprintf(file," %s",value.sym->nm);
  }else if(type == NUMBER){
    fprintf(file," %f",value.val_double);
  } /* end else */ 
  return 1; /* Return an int or compiler will complain */
} /* end yyprint() */ 

sym_sct *
sym_look(char *sym_nm)
/* 
   char *sym_nm: input name of symbol to locate/define
   sym_sct *sym_look(): output pointer to symbol
 */ 
{
  /* Purpose: Look up symbol table entry 
     If symbol is present, routine returns pointer to entry 
     If symbol is not present then add it to end of symbol table */

  sym_sct *sym;
  
  for(sym=sym_tbl;sym<&sym_tbl[SYM_NBR_MAX];sym++){
    /* Is the requested symbol already in the symbol table? */
    if(sym->nm && !strcmp(sym->nm,sym_nm)) return sym;
    
    /* Is current entry in symbol table empty? i.e., are we at end of valid entries?
       If so, enter new symbol in current slot */
    if(!sym->nm){
      sym->nm=(char *)strdup(sym_nm);
      return sym;
    } /* end if */ 
  } /* end for */ 
  yyerror("Too many symbols");
  exit(EXIT_FAILURE);
} /* end sym_look() */

void
fnc_add  /* [fnd] Add function to symbol table */
(char *nm, /* I [sng] Name of function to add to symbol table */
 double (*fnc)()) /* I [fnc] Entry point of function */ 
{
  /* Purpose: Add function to symbol table */

  sym_sct *sym;

  sym=sym_look(nm);
  sym->fnc=fnc;
} /* end fnc_add() */ 

/* End User Subroutines section */
