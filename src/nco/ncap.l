%{
  /* $Header: /data/zender/nco_20150216/nco/src/nco/ncap.l,v 1.3 1999-05-12 03:06:47 zender Exp $ */

  /* Purpose: Token generator for ncap parser */ 

  /* (c) Copyright 1995--1998University Corporation for Atmospheric Research/
     National Center for Atmospheric Research/
     Clmt.te and Global Dynamics Division

     The file LICENSE contains the full copyright notice, or 
     you may contact NSF/UCAR/NCAR/CGD/CMS for copyright assistance. */

  /* Example lex text:
     /data2/zender/ora/lexyacc/ch3-05.l
     Unfortunately, the GCC lexer is hard-coded in C for speed, but it's easy to read:
     /home/thibaud/usr/local/src/gcc-2.7.2/c-lex.c
     unidata ncgen.l 
   */ 

  /* NB: flex with -Cf or -Cf not -I if this scanner will not be interactive */ 

  /* Standard header files */
#include <math.h>               /* sin cos cos sin 3.14159 */
#include <stdlib.h>             /* atof, atoi, malloc, getopt */ 

#include <netcdf.h>             /* netCDF def'ns */
#include "nc.h"                 /* global definitions */
#include "ncap.h"               /* symbol table definition */ 
#include "ncap.tab.h"           /* symbol definitions from parser */ 

/* Reprototype yylex() to accept reentrant arguments (flex man p. 12, bison man p. 60) */ 
#define YY_DECL int yylex(lvalp) YYSTYPE *lvalp;

%}

%%
([0-9]+|([0-9]*\.[0-9]+)([eE][dD][-+]?[0-9]+)?) { /* Process floats */ 
  /* Recall that yylval is of type YYSTYPE, itself defined by the union of token types in the *.y file,
     and included as an extern by the *.tab.h file */
  lvalp->val_double=atof(yytext);
  if(dbg_lvl_get() > 0) (void)fprintf(stderr,"lexing: yytext = %s, yylval.val_double = %g\n",yytext,lvalp->val_double);
  return NUMBER;
}

[ \t] { ; /* Ignore white space */ }

[A-Za-z_][A-Za-z0-9_]* { /* Return symbol pointer */
  lvalp->sym=sym_look(yytext);
  return NAME;
}

"$" { return 0; }

<<EOF>> { return; /* EOF encountered and yywrap() returned 1 (flex man p. 20) */ }

\n | 
. { return yytext[0]; 
    /* NB: lex wants the | separator at the end of the line rather than the beginning of the next line */
    /* Catch-all default for unmatched input characters. The literal tokens ('+','-','/'...) are returned 
       by this rule. */ }

%%

/* Begin user subroutines section */
bool
yywrap()
/* 
   bool yywrap(): output flag whether lexer input is finished
 */ 
{
  /* Routine to replace library-defined yywrap(). 
     yywrap() is called when YY_INPUT returns an EOF.
     Default is to return true (one) when called---this terminates the scanner. 
     Returning false (zero) means there is more to scan, and the input has
     been redirected to a new source, e.g., yyin points to a new file. */ 
 return 1;
} /* end yywrap() */ 

/* End user subroutines section */


