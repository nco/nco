%{
  /* $Header: /data/zender/nco_20150216/nco/src/nco/ncap.l,v 1.4 2000-01-17 01:53:56 zender Exp $ */

  /* Purpose: Token generator for ncap parser */ 

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


