%{
  /* $Header: /data/zender/nco_20150216/nco/src/nco/ncap.l,v 1.9 2001-05-08 01:36:03 zender Exp $ -*-C-*- */

  /* Purpose: Token generator for ncap parser */

/* Copyright (C) 1995--2001 Charlie Zender

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

  /* Example Lex text:
     /data/zender/ora/lexyacc/ch3-05.l
     GCC lexer is hard-coded in C for speed, but is easy to read:
     /home/thibaud/usr/local/src/gcc-2.7.2/c-lex.c
     unidata ncgen.l 
   */

  /* NB: flex with -Cf or -Cf not -I if this scanner will not be interactive */

  /* Standard header files */
#include <math.h>               /* sin cos cos sin 3.14159 */
#include <stdlib.h>             /* atof, atoi, malloc, getopt */

  /* 3rd party vendors */
#include <netcdf.h>             /* netCDF definitions */

  /* Headers specific to this program */
#include "nc.h"                 /* NCO definitions */
#include "ncap.h"               /* symbol table definition */
#include "ncap.tab.h"           /* symbol definitions from parser */

/* Reprototype yylex() to accept reentrant arguments (flex man p. 12, bison man p. 60) */
#define YY_DECL int yylex(lvalp) YYSTYPE *lvalp;

%}

/* Following sections after %% are lexer rules
   Each rule is regular expression followed by C-code for action
   If token matches regular expression then C-code is executed */
%%

([0-9]+|([0-9]*\.[0-9]+)([eE][dD][-+]?[0-9]+)?) { /* Process floats */
  /* yylval is of type YYSTYPE, itself defined by union of token types in *.y file,
     and included as an extern by *.tab.h file */
  lvalp->val_double=strtod(yytext,(char **)NULL);
  if(dbg_lvl_get() > 0) (void)fprintf(stderr,"lexing: yytext = %s, yylval.val_double = %g\n",yytext,lvalp->val_double);
  return NUMBER;
}

[ \t] { ; /* Ignore white space */}

[A-Za-z_][A-Za-z0-9_]* { /* Return symbol pointer */
  lvalp->sym=sym_look(yytext);
  return NAME;
}

"$" { return 0; } /* Ignore everything on a line following '$' */

<<EOF>> { return 1; /* EOF encountered and yywrap() returned 1 (flex man p. 20) */}

\n | 
. { return yytext[0]; 
    /* Lex wants | separator at end of line rather than beginning of next line */
    /* Catch-all default for unmatched input characters
       Literal tokens ('+','-','/'...) are returned by this rule. */}

%%

/* Begin user subroutines section */

bool
yywrap()
/* 
   bool yywrap(): output flag whether lexer input is finished
 */
{
  /* Purpose: Routine to replace library-defined yywrap() 
     yywrap() is called when YY_INPUT returns EOF
     Default is to return true (one) when called---this terminates scanner 
     Returning false (zero) means there is more to scan, and the input has
     been redirected to a new source, e.g., yyin points to a new file. */
 return 1;
} /* end yywrap() */

/* End user subroutines section */


