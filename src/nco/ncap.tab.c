
/*  A Bison parser, made from ../src/nco/ncap.y
    by GNU Bison version 1.28  */

#define YYBISON 1  /* Identify Bison output.  */

#define	STRING	257
#define	ATTRIBUTE	258
#define	VAR	259
#define	OUT_VAR	260
#define	OUT_ATT	261
#define	FUNCTION	262
#define	LHS_SBS	263
#define	POWER	264
#define	ABS	265
#define	ATOSTR	266
#define	IGNORE	267
#define	EPROVOKE	268
#define	UMINUS	269

#line 1 "../src/nco/ncap.y"
 /* $Header: /data/zender/nco_20150216/nco/src/nco/ncap.tab.c,v 1.2 2002-02-24 18:49:34 zender Exp $ -*-C-*- */

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
extern long ln_nbr_crr; /* [cnt] Line number (declared in ncap.c) */
extern char *fl_spt_glb; /* [fl] Script file (declared in ncap.c) */
extern char err_sng[200]; /* [sng] Buffer for error string (declared in ncap.l) */

/* End C declarations section */

#line 97 "../src/nco/ncap.y"
typedef union{
  char *str;
  char *output_var;
  char *vara;
  aed_sct att;
  sym_sct *sym;
  parse_sct attribute;
  var_sct *var;
  nm_lst_sct *sbs_lst;
} YYSTYPE;
#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		121
#define	YYFLAG		-32768
#define	YYNTBASE	27

#define YYTRANSLATE(x) ((unsigned)(x) <= 269 ? yytranslate[x] : 35)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,    19,     2,     2,    24,
    26,    17,    15,    25,    16,     2,    18,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,    22,     2,
    23,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,    20,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     1,     3,     4,     5,     6,
     7,     8,     9,    10,    11,    12,    13,    14,    21
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     2,     6,    10,    13,    16,    20,    24,    28,    32,
    36,    40,    44,    48,    52,    56,    60,    63,    66,    70,
    77,    82,    87,    91,    93,    95,    97,   101,   106,   113,
   115,   119,   123,   127,   131,   135,   139,   143,   147,   151,
   155,   159,   163,   167,   174,   177,   180,   185,   190,   194
};

static const short yyrhs[] = {    28,
     0,    28,    29,    22,     0,    28,     1,    22,     0,    29,
    22,     0,     1,    22,     0,    32,    23,    30,     0,    32,
    23,    33,     0,    32,    23,    34,     0,    31,    23,    34,
     0,    31,    23,    30,     0,    31,    23,    33,     0,    30,
    15,    30,     0,    30,    16,    30,     0,    30,    17,    30,
     0,    30,    18,    30,     0,    30,    19,    30,     0,    16,
    30,     0,    15,    30,     0,    30,    20,    30,     0,    10,
    24,    30,    25,    30,    26,     0,    11,    24,    30,    26,
     0,     8,    24,    30,    26,     0,    24,    30,    26,     0,
     4,     0,     6,     0,     7,     0,    33,    15,    33,     0,
    12,    24,    30,    26,     0,    12,    24,    30,    25,    33,
    26,     0,     3,     0,    34,    15,    34,     0,    34,    15,
    30,     0,    30,    15,    34,     0,    34,    16,    34,     0,
    30,    16,    34,     0,    34,    16,    30,     0,    34,    17,
    34,     0,    34,    17,    30,     0,    34,    19,    30,     0,
    30,    17,    34,     0,    34,    18,    34,     0,    34,    18,
    30,     0,    34,    20,    30,     0,    10,    24,    34,    25,
    30,    26,     0,    16,    34,     0,    15,    34,     0,    11,
    24,    34,    26,     0,     8,    24,    34,    26,     0,    24,
    34,    26,     0,     5,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   141,   144,   150,   151,   152,   156,   186,   207,   231,   249,
   288,   316,   322,   326,   330,   334,   339,   343,   346,   359,
   372,   375,   386,   387,   390,   393,   396,   406,   418,   433,
   436,   442,   446,   450,   455,   466,   470,   474,   478,   482,
   486,   490,   494,   498,   502,   509,   512,   516,   520,   523
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","STRING",
"ATTRIBUTE","VAR","OUT_VAR","OUT_ATT","FUNCTION","LHS_SBS","POWER","ABS","ATOSTR",
"IGNORE","EPROVOKE","'+'","'-'","'*'","'/'","'%'","'^'","UMINUS","';'","'='",
"'('","','","')'","program","statement_list","statement","att_exp","out_var_exp",
"out_att_exp","string_exp","var_exp", NULL
};
#endif

static const short yyr1[] = {     0,
    27,    28,    28,    28,    28,    29,    29,    29,    29,    29,
    29,    30,    30,    30,    30,    30,    30,    30,    30,    30,
    30,    30,    30,    30,    31,    32,    33,    33,    33,    33,
    34,    34,    34,    34,    34,    34,    34,    34,    34,    34,
    34,    34,    34,    34,    34,    34,    34,    34,    34,    34
};

static const short yyr2[] = {     0,
     1,     3,     3,     2,     2,     3,     3,     3,     3,     3,
     3,     3,     3,     3,     3,     3,     2,     2,     3,     6,
     4,     4,     3,     1,     1,     1,     3,     4,     6,     1,
     3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
     3,     3,     3,     6,     2,     2,     4,     4,     3,     1
};

static const short yydefact[] = {     0,
     0,    25,    26,     0,     0,     0,     0,     5,     0,     0,
     4,     0,     0,     3,     2,    30,    24,    50,     0,     0,
     0,     0,     0,     0,     0,    10,    11,     9,     6,     7,
     8,     0,     0,     0,     0,    18,    46,    17,    45,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,    23,    49,    12,
    33,    13,    35,    14,    40,    15,    16,    19,    27,    32,
    31,    36,    34,    38,    37,    42,    41,    39,    43,    22,
    48,     0,     0,    21,    47,     0,     0,     0,    18,    17,
     0,     0,     0,     0,     0,    28,     0,     0,     0,     0,
     0,    12,    13,    14,     0,    20,    44,    29,     0,     0,
     0
};

static const short yydefgoto[] = {   119,
     4,     5,    26,     6,     7,    27,    28
};

static const short yypact[] = {    53,
   -21,-32768,-32768,    27,   -20,   -16,   -15,-32768,    -4,     4,
-32768,     1,     1,-32768,-32768,-32768,-32768,-32768,    -5,    -1,
    13,    14,    98,    98,    98,   275,    21,   281,   275,    21,
   281,    98,    98,    98,   108,-32768,-32768,-32768,-32768,   110,
   122,    98,    98,    98,   108,   108,   108,    12,    98,    98,
    98,    98,   108,   108,   134,   146,   242,   253,   158,   170,
    28,    29,    34,   108,   108,   108,    52,-32768,-32768,    68,
   126,    68,   126,    59,    60,    59,    59,    59,-32768,    68,
   126,    68,   126,    59,    60,    59,    60,    59,    59,-32768,
-32768,   108,   108,-32768,-32768,   108,   108,   108,-32768,-32768,
   182,   108,   108,   108,    12,-32768,   194,   206,   218,   264,
   230,   138,   138,    59,   -12,-32768,-32768,-32768,    73,    91,
-32768
};

static const short yypgoto[] = {-32768,
-32768,    92,    -3,-32768,-32768,   -13,    32
};


#define	YYLAST		301


static const short yytable[] = {    30,
     8,    11,    48,    16,    17,    18,    12,    13,    19,    29,
    20,    21,    22,   118,    16,    23,    24,    14,    32,    36,
    38,    40,    33,    22,    25,    15,    -1,     9,    55,    57,
    59,    67,     2,     3,    79,    48,    34,    35,    70,    72,
    74,    76,    77,    78,    31,    80,    82,    84,    86,    88,
    89,    96,    97,     1,    37,    39,    41,    98,     2,     3,
    99,   100,   101,    56,    58,    60,   102,   103,   104,    45,
    46,    47,   120,    71,    73,    75,   105,   106,    47,    54,
    81,    83,    85,    87,    44,    45,    46,    47,   107,   108,
   121,   115,   109,   110,   111,    10,     0,     0,   112,   113,
   114,    17,    18,     0,     0,    19,     0,    20,    21,     0,
     0,    17,    23,    24,     0,    61,     0,    62,    63,     0,
     0,    25,    64,    65,    42,    43,    44,    45,    46,    47,
     0,    66,     0,     0,     0,    68,    49,    50,    51,    52,
    53,    54,    51,    52,    53,    54,     0,    69,    42,    43,
    44,    45,    46,    47,   104,    45,    46,    47,     0,    90,
    49,    50,    51,    52,    53,    54,     0,     0,     0,     0,
     0,    91,    42,    43,    44,    45,    46,    47,     0,     0,
     0,     0,     0,    94,    49,    50,    51,    52,    53,    54,
     0,     0,     0,     0,     0,    95,   102,   103,   104,    45,
    46,    47,     0,     0,     0,     0,     0,    68,   102,   103,
   104,    45,    46,    47,     0,     0,     0,     0,     0,   116,
   102,   103,   104,    45,    46,    47,     0,     0,     0,     0,
     0,   117,   102,   103,   104,    45,    46,    47,     0,     0,
     0,     0,     0,    90,   102,   103,   104,    45,    46,    47,
     0,     0,     0,     0,     0,    94,    42,    43,    44,    45,
    46,    47,     0,     0,     0,     0,    92,    49,    50,    51,
    52,    53,    54,     0,     0,     0,     0,    93,   102,   103,
   104,    45,    46,    47,     0,     0,     0,     0,    92,    42,
    43,    44,    45,    46,    47,    49,    50,    51,    52,    53,
    54
};

static const short yycheck[] = {    13,
    22,    22,    15,     3,     4,     5,    23,    23,     8,    13,
    10,    11,    12,    26,     3,    15,    16,    22,    24,    23,
    24,    25,    24,    12,    24,    22,     0,     1,    32,    33,
    34,    35,     6,     7,    48,    15,    24,    24,    42,    43,
    44,    45,    46,    47,    13,    49,    50,    51,    52,    53,
    54,    24,    24,     1,    23,    24,    25,    24,     6,     7,
    64,    65,    66,    32,    33,    34,    15,    16,    17,    18,
    19,    20,     0,    42,    43,    44,    25,    26,    20,    20,
    49,    50,    51,    52,    17,    18,    19,    20,    92,    93,
     0,   105,    96,    97,    98,     4,    -1,    -1,   102,   103,
   104,     4,     5,    -1,    -1,     8,    -1,    10,    11,    -1,
    -1,     4,    15,    16,    -1,     8,    -1,    10,    11,    -1,
    -1,    24,    15,    16,    15,    16,    17,    18,    19,    20,
    -1,    24,    -1,    -1,    -1,    26,    15,    16,    17,    18,
    19,    20,    17,    18,    19,    20,    -1,    26,    15,    16,
    17,    18,    19,    20,    17,    18,    19,    20,    -1,    26,
    15,    16,    17,    18,    19,    20,    -1,    -1,    -1,    -1,
    -1,    26,    15,    16,    17,    18,    19,    20,    -1,    -1,
    -1,    -1,    -1,    26,    15,    16,    17,    18,    19,    20,
    -1,    -1,    -1,    -1,    -1,    26,    15,    16,    17,    18,
    19,    20,    -1,    -1,    -1,    -1,    -1,    26,    15,    16,
    17,    18,    19,    20,    -1,    -1,    -1,    -1,    -1,    26,
    15,    16,    17,    18,    19,    20,    -1,    -1,    -1,    -1,
    -1,    26,    15,    16,    17,    18,    19,    20,    -1,    -1,
    -1,    -1,    -1,    26,    15,    16,    17,    18,    19,    20,
    -1,    -1,    -1,    -1,    -1,    26,    15,    16,    17,    18,
    19,    20,    -1,    -1,    -1,    -1,    25,    15,    16,    17,
    18,    19,    20,    -1,    -1,    -1,    -1,    25,    15,    16,
    17,    18,    19,    20,    -1,    -1,    -1,    -1,    25,    15,
    16,    17,    18,    19,    20,    15,    16,    17,    18,    19,
    20
};
#define YYPURE 1

/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/lib/bison.simple"
/* This file comes from bison-1.28.  */

/* Skeleton output parser for bison,
   Copyright (C) 1984, 1989, 1990 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

#ifndef YYSTACK_USE_ALLOCA
#ifdef alloca
#define YYSTACK_USE_ALLOCA
#else /* alloca not defined */
#ifdef __GNUC__
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi) || (defined (__sun) && defined (__i386))
#define YYSTACK_USE_ALLOCA
#include <alloca.h>
#else /* not sparc */
/* We think this test detects Watcom and Microsoft C.  */
/* This used to test MSDOS, but that is a bad idea
   since that symbol is in the user namespace.  */
#if (defined (_MSDOS) || defined (_MSDOS_)) && !defined (__TURBOC__)
#if 0 /* No need for malloc.h, which pollutes the namespace;
	 instead, just don't use alloca.  */
#include <malloc.h>
#endif
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
/* I don't know what this was needed for, but it pollutes the namespace.
   So I turned it off.   rms, 2 May 1997.  */
/* #include <malloc.h>  */
 #pragma alloca
#define YYSTACK_USE_ALLOCA
#else /* not MSDOS, or __TURBOC__, or _AIX */
#if 0
#ifdef __hpux /* haible@ilog.fr says this works for HPUX 9.05 and up,
		 and on HPUX 10.  Eventually we can turn this on.  */
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#endif /* __hpux */
#endif
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc */
#endif /* not GNU C */
#endif /* alloca not defined */
#endif /* YYSTACK_USE_ALLOCA not defined */

#ifdef YYSTACK_USE_ALLOCA
#define YYSTACK_ALLOC alloca
#else
#define YYSTACK_ALLOC malloc
#endif

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	goto yyacceptlab
#define YYABORT 	goto yyabortlab
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(token, value) \
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    { yychar = (token), yylval = (value);			\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { yyerror ("syntax error: cannot back up"); YYERROR; }	\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

#ifndef YYPURE
#define YYLEX		yylex()
#endif

#ifdef YYPURE
#ifdef YYLSP_NEEDED
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, &yylloc, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval, &yylloc)
#endif
#else /* not YYLSP_NEEDED */
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval)
#endif
#endif /* not YYLSP_NEEDED */
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYPURE

int	yychar;			/*  the lookahead symbol		*/
YYSTYPE	yylval;			/*  the semantic value of the		*/
				/*  lookahead symbol			*/

#ifdef YYLSP_NEEDED
YYLTYPE yylloc;			/*  location data for the lookahead	*/
				/*  symbol				*/
#endif

int yynerrs;			/*  number of parse errors so far       */
#endif  /* not YYPURE */

#if YYDEBUG != 0
int yydebug;			/*  nonzero means print parse trace	*/
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  YYINITDEPTH indicates the initial size of the parser's stacks	*/

#ifndef	YYINITDEPTH
#define YYINITDEPTH 200
#endif

/*  YYMAXDEPTH is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#if YYMAXDEPTH == 0
#undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

/* Define __yy_memcpy.  Note that the size argument
   should be passed with type unsigned int, because that is what the non-GCC
   definitions require.  With GCC, __builtin_memcpy takes an arg
   of type size_t, but it can handle unsigned int.  */

#if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#define __yy_memcpy(TO,FROM,COUNT)	__builtin_memcpy(TO,FROM,COUNT)
#else				/* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (to, from, count)
     char *to;
     char *from;
     unsigned int count;
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#else /* __cplusplus */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (char *to, char *from, unsigned int count)
{
  register char *t = to;
  register char *f = from;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif

#line 217 "/usr/lib/bison.simple"

/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
#ifdef __cplusplus
#define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#define YYPARSE_PARAM_DECL
#else /* not __cplusplus */
#define YYPARSE_PARAM_ARG YYPARSE_PARAM
#define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
#endif /* not __cplusplus */
#else /* not YYPARSE_PARAM */
#define YYPARSE_PARAM_ARG
#define YYPARSE_PARAM_DECL
#endif /* not YYPARSE_PARAM */

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
#ifdef YYPARSE_PARAM
int yyparse (void *);
#else
int yyparse (void);
#endif
#endif

int
yyparse(YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  register int yystate;
  register int yyn;
  register short *yyssp;
  register YYSTYPE *yyvsp;
  int yyerrstatus;	/*  number of tokens to shift before error messages enabled */
  int yychar1 = 0;		/*  lookahead token as an internal (translated) token number */

  short	yyssa[YYINITDEPTH];	/*  the state stack			*/
  YYSTYPE yyvsa[YYINITDEPTH];	/*  the semantic value stack		*/

  short *yyss = yyssa;		/*  refer to the stacks thru separate pointers */
  YYSTYPE *yyvs = yyvsa;	/*  to allow yyoverflow to reallocate them elsewhere */

#ifdef YYLSP_NEEDED
  YYLTYPE yylsa[YYINITDEPTH];	/*  the location stack			*/
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;

#define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
#define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  int yystacksize = YYINITDEPTH;
  int yyfree_stacks = 0;

#ifdef YYPURE
  int yychar;
  YYSTYPE yylval;
  int yynerrs;
#ifdef YYLSP_NEEDED
  YYLTYPE yylloc;
#endif
#endif

  YYSTYPE yyval;		/*  the variable used to return		*/
				/*  semantic values from the action	*/
				/*  routines				*/

  int yylen;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Starting parse\n");
#endif

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss - 1;
  yyvsp = yyvs;
#ifdef YYLSP_NEEDED
  yylsp = yyls;
#endif

/* Push a new state, which is found in  yystate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
yynewstate:

  *++yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YYSTYPE *yyvs1 = yyvs;
      short *yyss1 = yyss;
#ifdef YYLSP_NEEDED
      YYLTYPE *yyls1 = yyls;
#endif

      /* Get the current used size of the three stacks, in elements.  */
      int size = yyssp - yyss + 1;

#ifdef yyoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
#ifdef YYLSP_NEEDED
      /* This used to be a conditional around just the two extra args,
	 but that might be undefined if yyoverflow is a macro.  */
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yyls1, size * sizeof (*yylsp),
		 &yystacksize);
#else
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yystacksize);
#endif

      yyss = yyss1; yyvs = yyvs1;
#ifdef YYLSP_NEEDED
      yyls = yyls1;
#endif
#else /* no yyoverflow */
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	{
	  yyerror("parser stack overflow");
	  if (yyfree_stacks)
	    {
	      free (yyss);
	      free (yyvs);
#ifdef YYLSP_NEEDED
	      free (yyls);
#endif
	    }
	  return 2;
	}
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;
#ifndef YYSTACK_USE_ALLOCA
      yyfree_stacks = 1;
#endif
      yyss = (short *) YYSTACK_ALLOC (yystacksize * sizeof (*yyssp));
      __yy_memcpy ((char *)yyss, (char *)yyss1,
		   size * (unsigned int) sizeof (*yyssp));
      yyvs = (YYSTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yyvsp));
      __yy_memcpy ((char *)yyvs, (char *)yyvs1,
		   size * (unsigned int) sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yylsp));
      __yy_memcpy ((char *)yyls, (char *)yyls1,
		   size * (unsigned int) sizeof (*yylsp));
#endif
#endif /* no yyoverflow */

      yyssp = yyss + size - 1;
      yyvsp = yyvs + size - 1;
#ifdef YYLSP_NEEDED
      yylsp = yyls + size - 1;
#endif

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Stack size increased to %d\n", yystacksize);
#endif

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Entering state %d\n", yystate);
#endif

  goto yybackup;
 yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Reading a token: ");
#endif
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      yychar1 = YYTRANSLATE(yychar);

#if YYDEBUG != 0
      if (yydebug)
	{
	  fprintf (stderr, "Next token is %d (%s", yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise meaning
	     of a token, for further debugging info.  */
#ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
#endif
	  fprintf (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting token %d (%s), ", yychar, yytname[yychar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (yyerrstatus) yyerrstatus--;

  yystate = yyn;
  goto yynewstate;

/* Do the default action for the current state.  */
yydefault:

  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;

/* Do a reduction.  yyn is the number of a rule to reduce with.  */
yyreduce:
  yylen = yyr2[yyn];
  if (yylen > 0)
    yyval = yyvsp[1-yylen]; /* implement default value of the action */

#if YYDEBUG != 0
  if (yydebug)
    {
      int i;

      fprintf (stderr, "Reducing via rule %d (line %d), ",
	       yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (i = yyprhs[yyn]; yyrhs[i] > 0; i++)
	fprintf (stderr, "%s ", yytname[yyrhs[i]]);
      fprintf (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif


  switch (yyn) {

case 2:
#line 145 "../src/nco/ncap.y"
{
  /* Purpose: Actions to be performed at end-of-statement go here */
  /* Clean up from and exit LHS_cst mode */
  (void)quick_free(&((prs_sct *)prs_arg)->var_LHS);
;
    break;}
case 3:
#line 150 "../src/nco/ncap.y"
{(void)quick_free(&((prs_sct *)prs_arg)->var_LHS);;
    break;}
case 4:
#line 151 "../src/nco/ncap.y"
{(void)quick_free(&((prs_sct *)prs_arg)->var_LHS);;
    break;}
case 5:
#line 152 "../src/nco/ncap.y"
{(void)quick_free(&((prs_sct *)prs_arg)->var_LHS);;
    break;}
case 6:
#line 159 "../src/nco/ncap.y"
{ 
  int aed_idx; 
  aed_sct *ptr_aed;

  aed_idx=ncap_aed_lookup(yyvsp[-2].att.var_nm,yyvsp[-2].att.att_nm,((prs_sct *)prs_arg)->att_lst,((prs_sct *)prs_arg)->nbr_att,True);
  ptr_aed=((prs_sct *)prs_arg)->att_lst[aed_idx];                               
  ptr_aed->val=ncap_attribute_2_ptr_unn(yyvsp[0].attribute);
  ptr_aed->type=yyvsp[0].attribute.type;
  ptr_aed->sz=1L;
  (void)cast_nctype_void(ptr_aed->type,&ptr_aed->val);    
  if(dbg_lvl_get() > 0) (void)sprintf(err_sng,"Saving attribute %s@%s to %s",yyvsp[-2].att.var_nm,yyvsp[-2].att.att_nm,((prs_sct *)prs_arg)->fl_out);
  (void)yyerror(err_sng);

  if(dbg_lvl_get() > 1){
    (void)fprintf(stderr,"Saving in array attribute %s@%s=",yyvsp[-2].att.var_nm,yyvsp[-2].att.att_nm);
    switch(yyvsp[0].attribute.type){
    case NC_BYTE: (void)fprintf(stderr,"%d\n",yyvsp[0].attribute.val.b); break;
    case NC_SHORT: (void)fprintf(stderr,"%d\n",yyvsp[0].attribute.val.s); break;
    case NC_INT: (void)fprintf(stderr,"%ld\n",yyvsp[0].attribute.val.l); break;
    case NC_FLOAT: (void)fprintf(stderr,"%G\n",yyvsp[0].attribute.val.f); break;		  
    case NC_DOUBLE: (void)fprintf(stderr,"%.5G\n",yyvsp[0].attribute.val.d);break;
    default: break;
    } /* end switch */
  } /* end if */
  yyvsp[-2].att.var_nm=nco_free(yyvsp[-2].att.var_nm);
  yyvsp[-2].att.att_nm=nco_free(yyvsp[-2].att.att_nm);
;
    break;}
case 7:
#line 187 "../src/nco/ncap.y"
{
  int aed_idx; 
  int sng_lng;
  aed_sct *ptr_aed;
  
  sng_lng=strlen(yyvsp[0].str);
  aed_idx=ncap_aed_lookup(yyvsp[-2].att.var_nm,yyvsp[-2].att.att_nm,((prs_sct *)prs_arg)->att_lst,((prs_sct *)prs_arg)->nbr_att,True);
  ptr_aed=((prs_sct *)prs_arg)->att_lst[aed_idx];
  ptr_aed->type=NC_CHAR;
  ptr_aed->sz=(long)((sng_lng+1)*nco_typ_lng(NC_CHAR));
  ptr_aed->val.cp=(unsigned char *)nco_malloc((sng_lng+1)*nco_typ_lng(NC_CHAR));
  strcpy((char *)(ptr_aed->val.cp),yyvsp[0].str);
  (void)cast_nctype_void(NC_CHAR,&ptr_aed->val);    
  
  if(dbg_lvl_get() > 0) (void)sprintf(err_sng,"Saving attribute %s@%s=%s",yyvsp[-2].att.var_nm,yyvsp[-2].att.att_nm,yyvsp[0].str);
  (void)yyerror(err_sng);
  yyvsp[-2].att.var_nm=nco_free(yyvsp[-2].att.var_nm);
  yyvsp[-2].att.att_nm=nco_free(yyvsp[-2].att.att_nm);
  yyvsp[0].str=nco_free(yyvsp[0].str);
;
    break;}
case 8:
#line 208 "../src/nco/ncap.y"
{ 
  /* Storing 0-dimensional variables in attribute is OK */ 
  int aed_idx;
  aed_sct *ptr_aed;
  
  if(yyvsp[0].var->nbr_dim < 2){
    aed_idx=ncap_aed_lookup(yyvsp[-2].att.var_nm,yyvsp[-2].att.att_nm,((prs_sct *)prs_arg)->att_lst,((prs_sct *)prs_arg)->nbr_att,True);
    ptr_aed=((prs_sct *)prs_arg)->att_lst[aed_idx];
    ptr_aed->sz=yyvsp[0].var->sz;
    ptr_aed->type=yyvsp[0].var->type;
    ptr_aed->val.vp=(void*)nco_malloc((ptr_aed->sz)*nco_typ_lng(ptr_aed->type));
    (void)var_copy(ptr_aed->type,ptr_aed->sz,yyvsp[0].var->val,ptr_aed->val);
    /* cast_nctype_void($3->type,&ptr_aed->val); */
    if(dbg_lvl_get() > 0) (void)sprintf(err_sng,"Saving attribute %s@%s %d dimensional variable",yyvsp[-2].att.var_nm,yyvsp[-2].att.att_nm,yyvsp[0].var->nbr_dim);
    (void)yyerror(err_sng); 
  }else{
    (void)sprintf(err_sng,"Warning: Cannot store in attribute %s@%s a variable with dimension %d",yyvsp[-2].att.var_nm,yyvsp[-2].att.att_nm,yyvsp[0].var->nbr_dim);
    (void)yyerror(err_sng);
  } /* endif */
  yyvsp[-2].att.var_nm=nco_free(yyvsp[-2].att.var_nm);
  yyvsp[-2].att.att_nm=nco_free(yyvsp[-2].att.att_nm);
  (void)var_free(yyvsp[0].var); 
;
    break;}
case 9:
#line 232 "../src/nco/ncap.y"
{
  int rcd;
  int var_id;
  yyvsp[0].var->nm=strdup(yyvsp[-2].output_var);
  /* Is variable already in output file? */
  rcd=nco_inq_varid_flg(((prs_sct *)prs_arg)->out_id,yyvsp[0].var->nm,&var_id);
  if(rcd == NC_NOERR){
    (void)sprintf(err_sng,"Warning: Variable %s has aleady been saved in %s",yyvsp[0].var->nm,((prs_sct *)prs_arg)->fl_out);
    (void)yyerror(err_sng);                                   
  }else{  
    (void)ncap_var_write(yyvsp[0].var,(prs_sct *)prs_arg);
    if(dbg_lvl_get() > 0) (void)sprintf(err_sng,"Saving variable %s to %s",yyvsp[0].var->nm,((prs_sct *)prs_arg)->fl_out);
    (void)yyerror(err_sng);
  } /* end else */
  yyvsp[-2].output_var=nco_free(yyvsp[-2].output_var);
  (void)var_free(yyvsp[0].var);
;
    break;}
case 10:
#line 250 "../src/nco/ncap.y"
{
  int rcd;
  int var_id;
  var_sct *var;
  rcd=nco_inq_varid_flg(((prs_sct *)prs_arg)->out_id,yyvsp[-2].output_var,&var_id);
  if(dbg_lvl_get() > 5) (void)fprintf(stderr,"%s: DEBUG out_var_exp = att_exp rule for %s\n",prg_nm_get(),yyvsp[-2].output_var);
  if(rcd == NC_NOERR){
    (void)sprintf(err_sng,"Warning: Variable %s has aleady been saved in %s",yyvsp[-2].output_var,((prs_sct *)prs_arg)->fl_out);
    (void)yyerror(err_sng);
  }else{  
    /* Turn attribute into temporary variable for writing */
    var=(var_sct *)nco_malloc(sizeof(var_sct));
    /* Set defaults */
    (void)var_dfl_set(var); /* [fnc] Set defaults for each member of variable structure */
    /* Overwrite with attribute expression information */
    var->nm=strdup(yyvsp[-2].output_var);
    var->nbr_dim=0;
    var->sz=1;
    var->val=ncap_attribute_2_ptr_unn(yyvsp[0].attribute);
    var->type=yyvsp[0].attribute.type;

    if(((prs_sct *)prs_arg)->var_LHS != NULL){
      /* User intends LHS to cast RHS to same dimensionality
	 Stretch newly initialized variable to size of LHS template */
      /*    (void)ncap_var_conform_dim(&$$,&(((prs_sct *)prs_arg)->var_LHS));*/
      (void)ncap_var_stretch(&var,&(((prs_sct *)prs_arg)->var_LHS));
      
      if(dbg_lvl_get() > 2) (void)fprintf(stderr,"%s: Stretching former att_exp defining %s with LHS template: Template var->nm %s, var->nbr_dim %d, var->sz %li\n",prg_nm_get(),yyvsp[-2].output_var,((prs_sct *)prs_arg)->var_LHS->nm,((prs_sct *)prs_arg)->var_LHS->nbr_dim,((prs_sct *)prs_arg)->var_LHS->sz);
      
    } /* endif LHS_cst */
    
    (void)ncap_var_write(var,(prs_sct *)prs_arg);
    (void)var_free(var);
    if(dbg_lvl_get() > 0) (void)sprintf(err_sng,"Saving variable %s to %s",yyvsp[-2].output_var,((prs_sct *)prs_arg)->fl_out);
    (void)yyerror(err_sng);
  } /* endif */
  yyvsp[-2].output_var=nco_free(yyvsp[-2].output_var);
;
    break;}
case 11:
#line 289 "../src/nco/ncap.y"
{
  int rcd;
  int var_id;
  var_sct *var;
  rcd=nco_inq_varid_flg(((prs_sct *)prs_arg)->out_id,yyvsp[-2].output_var,&var_id);
  if(rcd == NC_NOERR){
    (void)sprintf(err_sng,"Warning: Variable %s has aleady been saved in %s",yyvsp[-2].output_var,((prs_sct *)prs_arg)->fl_out);
    (void)yyerror(err_sng);  
  }else{  
    var=(var_sct *)nco_calloc((size_t)1,sizeof(var_sct));
    var->nm=strdup(yyvsp[-2].output_var);
    var->nbr_dim=0;
    var->dmn_id=(int *)NULL;
    var->sz=strlen(yyvsp[0].str)+1;
    var->val.cp=(unsigned char *)strdup(yyvsp[0].str);
    var->type=NC_CHAR;
    (void)cast_nctype_void(NC_CHAR,&var->val);
    (void)ncap_var_write(var,(prs_sct *)prs_arg);
    (void)var_free(var);
    if(dbg_lvl_get() > 0) (void)sprintf(err_sng,"Saving variable %s to %s",yyvsp[-2].output_var,((prs_sct *)prs_arg)->fl_out);
    (void)yyerror(err_sng);
  } /* endelse */
  yyvsp[-2].output_var=nco_free(yyvsp[-2].output_var);
  yyvsp[0].str=nco_free(yyvsp[0].str);
;
    break;}
case 12:
#line 318 "../src/nco/ncap.y"
{
  (void)ncap_retype(&yyvsp[-2].attribute,&yyvsp[0].attribute);
  yyval.attribute=ncap_attribute_calc(yyvsp[-2].attribute,'+',yyvsp[0].attribute);                                
;
    break;}
case 13:
#line 322 "../src/nco/ncap.y"
{
  (void)ncap_retype(&yyvsp[-2].attribute,&yyvsp[0].attribute); 
  yyval.attribute=ncap_attribute_calc(yyvsp[-2].attribute,'-',yyvsp[0].attribute);
;
    break;}
case 14:
#line 326 "../src/nco/ncap.y"
{
  (void)ncap_retype(&yyvsp[-2].attribute,&yyvsp[0].attribute);
  yyval.attribute=ncap_attribute_calc(yyvsp[-2].attribute,'*',yyvsp[0].attribute);
;
    break;}
case 15:
#line 330 "../src/nco/ncap.y"
{
  (void)ncap_retype(&yyvsp[-2].attribute,&yyvsp[0].attribute); 
  yyval.attribute=ncap_attribute_calc(yyvsp[-2].attribute,'/',yyvsp[0].attribute);  
;
    break;}
case 16:
#line 334 "../src/nco/ncap.y"
{
  (void)ncap_retype(&yyvsp[-2].attribute,&yyvsp[0].attribute);
  
  yyval.attribute=ncap_attribute_calc(yyvsp[-2].attribute,'%',yyvsp[0].attribute);  
;
    break;}
case 17:
#line 339 "../src/nco/ncap.y"
{
  (void)ncap_attribute_minus(&yyvsp[0].attribute);
  yyval.attribute=yyvsp[0].attribute;
;
    break;}
case 18:
#line 343 "../src/nco/ncap.y"
{
  yyval.attribute=yyvsp[0].attribute;
;
    break;}
case 19:
#line 346 "../src/nco/ncap.y"
{
  if(yyvsp[-2].attribute.type <= NC_FLOAT && yyvsp[0].attribute.type <= NC_FLOAT) {
    (void)ncap_attribute_conform_type(NC_FLOAT,&yyvsp[-2].attribute);
    (void)ncap_attribute_conform_type(NC_FLOAT,&yyvsp[0].attribute);
    yyval.attribute.val.f=powf(yyvsp[-2].attribute.val.f,yyvsp[0].attribute.val.f);
    yyval.attribute.type=NC_FLOAT;
  } else { 
  (void)ncap_attribute_conform_type(NC_DOUBLE,&yyvsp[-2].attribute);
  (void)ncap_attribute_conform_type(NC_DOUBLE,&yyvsp[0].attribute);
  yyval.attribute.val.d=pow(yyvsp[-2].attribute.val.d,yyvsp[0].attribute.val.d);
  yyval.attribute.type=NC_DOUBLE; 
  }
;
    break;}
case 20:
#line 359 "../src/nco/ncap.y"
{
  if(yyvsp[-3].attribute.type <= NC_FLOAT && yyvsp[-1].attribute.type <= NC_FLOAT) {
    (void)ncap_attribute_conform_type(NC_FLOAT,&yyvsp[-3].attribute);
    (void)ncap_attribute_conform_type(NC_FLOAT,&yyvsp[-1].attribute);
    yyval.attribute.val.f=powf(yyvsp[-3].attribute.val.f,yyvsp[-1].attribute.val.f);
    yyval.attribute.type=NC_FLOAT;
  } else { 
  (void)ncap_attribute_conform_type(NC_DOUBLE,&yyvsp[-3].attribute);
  (void)ncap_attribute_conform_type(NC_DOUBLE,&yyvsp[-1].attribute);
  yyval.attribute.val.d=pow(yyvsp[-3].attribute.val.d,yyvsp[-1].attribute.val.d);
  yyval.attribute.type=NC_DOUBLE; 
  }
;
    break;}
case 21:
#line 372 "../src/nco/ncap.y"
{
  yyval.attribute=ncap_attribute_abs(yyvsp[-1].attribute);
;
    break;}
case 22:
#line 375 "../src/nco/ncap.y"
{
  
  if(yyvsp[-1].attribute.type <= NC_FLOAT) {
    (void)ncap_attribute_conform_type(NC_FLOAT,&yyvsp[-1].attribute);
    yyval.attribute.val.f=(*(yyvsp[-3].sym->fncf))(yyvsp[-1].attribute.val.f);
    yyval.attribute.type=NC_FLOAT;
  } else {
  yyval.attribute.val.d=(*(yyvsp[-3].sym->fnc))(yyvsp[-1].attribute.val.d);
  yyval.attribute.type=NC_DOUBLE;
  }
;
    break;}
case 23:
#line 386 "../src/nco/ncap.y"
{yyval.attribute=yyvsp[-1].attribute;;
    break;}
case 24:
#line 387 "../src/nco/ncap.y"
{yyval.attribute=yyvsp[0].attribute;;
    break;}
case 25:
#line 390 "../src/nco/ncap.y"
{yyval.output_var=yyvsp[0].output_var;;
    break;}
case 26:
#line 393 "../src/nco/ncap.y"
{yyval.att=yyvsp[0].att;;
    break;}
case 27:
#line 397 "../src/nco/ncap.y"
{
  size_t sng_lng;
  sng_lng=strlen(yyvsp[-2].str)+strlen(yyvsp[0].str);
  yyval.str=(char*)nco_malloc((sng_lng+1)*sizeof(char));
  strcpy(yyval.str,yyvsp[-2].str);
  strcat(yyval.str,yyvsp[0].str);
  yyvsp[-2].str=nco_free(yyvsp[-2].str);
  yyvsp[0].str=nco_free(yyvsp[0].str);
;
    break;}
case 28:
#line 406 "../src/nco/ncap.y"
{
  char bfr[50];
  switch (yyvsp[-1].attribute.type){
  case NC_DOUBLE: sprintf(bfr,"%.10G",yyvsp[-1].attribute.val.d); break;
  case NC_FLOAT: sprintf(bfr,"%G",yyvsp[-1].attribute.val.f); break;
  case NC_INT: sprintf(bfr,"%ld",yyvsp[-1].attribute.val.l); break;
  case NC_SHORT: sprintf(bfr,"%d",yyvsp[-1].attribute.val.s); break;
  case NC_BYTE: sprintf(bfr,"%d",yyvsp[-1].attribute.val.b); break;
  default:  break;
  } /* end switch */
  yyval.str=strdup(bfr);      
;
    break;}
case 29:
#line 418 "../src/nco/ncap.y"
{
  char bfr[150];
  /* Format string according to string expression */
  /* User decides which format corresponds to which type */
  switch (yyvsp[-3].attribute.type){
  case NC_DOUBLE: sprintf(bfr,yyvsp[-1].str,yyvsp[-3].attribute.val.d); break;
  case NC_FLOAT: sprintf(bfr,yyvsp[-1].str,yyvsp[-3].attribute.val.f); break;
  case NC_INT: sprintf(bfr,yyvsp[-1].str,yyvsp[-3].attribute.val.l); break;
  case NC_SHORT: sprintf(bfr,yyvsp[-1].str,yyvsp[-3].attribute.val.s); break;
  case NC_BYTE: sprintf(bfr,yyvsp[-1].str,yyvsp[-3].attribute.val.b); break;
  default:  break;
  } /* end switch */
  yyvsp[-1].str=nco_free(yyvsp[-1].str);
  yyval.str=strdup(bfr);      
;
    break;}
case 30:
#line 433 "../src/nco/ncap.y"
{yyval.str=yyvsp[0].str;;
    break;}
case 31:
#line 438 "../src/nco/ncap.y"
{ 
  yyval.var=ncap_var_var_add(yyvsp[-2].var,yyvsp[0].var); 
  var_free(yyvsp[-2].var); var_free(yyvsp[0].var);
;
    break;}
case 32:
#line 442 "../src/nco/ncap.y"
{
  yyval.var=ncap_var_attribute_add(yyvsp[-2].var,yyvsp[0].attribute);
  var_free(yyvsp[-2].var);
;
    break;}
case 33:
#line 446 "../src/nco/ncap.y"
{
  yyval.var=ncap_var_attribute_add(yyvsp[0].var,yyvsp[-2].attribute);
  var_free(yyvsp[0].var);
;
    break;}
case 34:
#line 450 "../src/nco/ncap.y"
{ 
  yyval.var=ncap_var_var_sub(yyvsp[-2].var,yyvsp[0].var);
  var_free(yyvsp[-2].var); 
  var_free(yyvsp[0].var);
;
    break;}
case 35:
#line 455 "../src/nco/ncap.y"
{ 
  var_sct *var1;
  parse_sct minus;
  minus.val.b=-1;
  minus.type=NC_BYTE;
  (void)ncap_attribute_conform_type(yyvsp[0].var->type,&minus);
  var1=ncap_var_attribute_sub(yyvsp[0].var,yyvsp[-2].attribute);
  yyval.var=ncap_var_attribute_multiply(var1,minus);
  var_free(var1);
  var_free(yyvsp[0].var);
;
    break;}
case 36:
#line 466 "../src/nco/ncap.y"
{
  yyval.var=ncap_var_attribute_sub(yyvsp[-2].var,yyvsp[0].attribute);
  var_free(yyvsp[-2].var);
;
    break;}
case 37:
#line 470 "../src/nco/ncap.y"
{
  yyval.var=ncap_var_var_multiply(yyvsp[-2].var,yyvsp[0].var); 
  var_free(yyvsp[-2].var); var_free(yyvsp[0].var); 
;
    break;}
case 38:
#line 474 "../src/nco/ncap.y"
{
  yyval.var=ncap_var_attribute_multiply(yyvsp[-2].var,yyvsp[0].attribute);
  var_free(yyvsp[-2].var);
;
    break;}
case 39:
#line 478 "../src/nco/ncap.y"
{
  yyval.var=ncap_var_attribute_modulus(yyvsp[-2].var,yyvsp[0].attribute);
  var_free(yyvsp[-2].var);
;
    break;}
case 40:
#line 482 "../src/nco/ncap.y"
{
  yyval.var=ncap_var_attribute_multiply(yyvsp[0].var,yyvsp[-2].attribute);
  var_free(yyvsp[0].var);
;
    break;}
case 41:
#line 486 "../src/nco/ncap.y"
{
  yyval.var=ncap_var_var_divide(yyvsp[-2].var,yyvsp[0].var); 
  var_free(yyvsp[-2].var); var_free(yyvsp[0].var); 
;
    break;}
case 42:
#line 490 "../src/nco/ncap.y"
{
  yyval.var=ncap_var_attribute_divide(yyvsp[-2].var,yyvsp[0].attribute);
  var_free(yyvsp[-2].var);
;
    break;}
case 43:
#line 494 "../src/nco/ncap.y"
{
  yyval.var=ncap_var_attribute_power(yyvsp[-2].var,yyvsp[0].attribute);
  var_free(yyvsp[-2].var);
;
    break;}
case 44:
#line 498 "../src/nco/ncap.y"
{
  yyval.var=ncap_var_attribute_power(yyvsp[-3].var,yyvsp[-1].attribute);
  var_free(yyvsp[-3].var);
;
    break;}
case 45:
#line 502 "../src/nco/ncap.y"
{ 
  parse_sct minus;
  minus.val.b=-1;
  minus.type=NC_BYTE;
  yyval.var=ncap_var_attribute_multiply(yyvsp[0].var,minus);
  var_free(yyvsp[0].var);      
;
    break;}
case 46:
#line 509 "../src/nco/ncap.y"
{
  yyval.var=yyvsp[0].var;
;
    break;}
case 47:
#line 512 "../src/nco/ncap.y"
{
  yyval.var=ncap_var_abs(yyvsp[-1].var);
  var_free(yyvsp[-1].var);
;
    break;}
case 48:
#line 516 "../src/nco/ncap.y"
{
  yyval.var=ncap_var_function(yyvsp[-1].var,yyvsp[-3].sym);
  var_free(yyvsp[-1].var);
;
    break;}
case 49:
#line 520 "../src/nco/ncap.y"
{
  yyval.var=yyvsp[-1].var;
;
    break;}
case 50:
#line 523 "../src/nco/ncap.y"
{ 
  yyval.var=ncap_var_init(yyvsp[0].vara,(prs_sct *)prs_arg);
  if (yyval.var==(var_sct *)NULL) YYERROR;

  if((((prs_sct *)prs_arg)->var_LHS) != NULL){
    /* User intends LHS to cast RHS to same dimensionality
       Stretch newly initialized variable to size of LHS template */
    /*    (void)ncap_var_conform_dim(&$$,&(((prs_sct *)prs_arg)->var_LHS));*/
    (void)ncap_var_stretch(&yyval.var,&(((prs_sct *)prs_arg)->var_LHS));

    if(dbg_lvl_get() > 2) (void)fprintf(stderr,"%s: Stretching variable %s with LHS template: Template var->nm %s, var->nbr_dim %d, var->sz %li\n",prg_nm_get(),yyval.var->nm,((prs_sct *)prs_arg)->var_LHS->nm,((prs_sct *)prs_arg)->var_LHS->nbr_dim,((prs_sct *)prs_arg)->var_LHS->sz);

  } /* endif LHS_cst */

  /* Sanity check */
  if (yyval.var==(var_sct *)NULL) YYERROR;
;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 543 "/usr/lib/bison.simple"

  yyvsp -= yylen;
  yyssp -= yylen;
#ifdef YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;

#ifdef YYLSP_NEEDED
  yylsp++;
  if (yylen == 0)
    {
      yylsp->first_line = yylloc.first_line;
      yylsp->first_column = yylloc.first_column;
      yylsp->last_line = (yylsp-1)->last_line;
      yylsp->last_column = (yylsp-1)->last_column;
      yylsp->text = 0;
    }
  else
    {
      yylsp->last_line = (yylsp+yylen-1)->last_line;
      yylsp->last_column = (yylsp+yylen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;

yyerrlab:   /* here on detecting error */

  if (! yyerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  int size = 0;
	  char *msg;
	  int x, count;

	  count = 0;
	  /* Start X at -yyn if nec to avoid negative indexes in yycheck.  */
	  for (x = (yyn < 0 ? -yyn : 0);
	       x < (sizeof(yytname) / sizeof(char *)); x++)
	    if (yycheck[x + yyn] == x)
	      size += strlen(yytname[x]) + 15, count++;
	  msg = (char *) malloc(size + 15);
	  if (msg != 0)
	    {
	      strcpy(msg, "parse error");

	      if (count < 5)
		{
		  count = 0;
		  for (x = (yyn < 0 ? -yyn : 0);
		       x < (sizeof(yytname) / sizeof(char *)); x++)
		    if (yycheck[x + yyn] == x)
		      {
			strcat(msg, count == 0 ? ", expecting `" : " or `");
			strcat(msg, yytname[x]);
			strcat(msg, "'");
			count++;
		      }
		}
	      yyerror(msg);
	      free(msg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exceeded");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror("parse error");
    }

  goto yyerrlab1;
yyerrlab1:   /* here on error raised explicitly by an action */

  if (yyerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Discarding token %d (%s).\n", yychar, yytname[yychar1]);
#endif

      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;

yyerrdefault:  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  yyn = yydefact[yystate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (yyn) goto yydefault;
#endif

yyerrpop:   /* pop the current state because it cannot handle the error token */

  if (yyssp == yyss) YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#ifdef YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

yyerrhandle:

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;

 yyacceptlab:
  /* YYACCEPT comes here.  */
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
  return 0;

 yyabortlab:
  /* YYABORT comes here.  */
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
  return 1;
}
#line 543 "../src/nco/ncap.y"

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
  if(dbg_lvl_get() > 0) (void)fprintf(stderr,"%s: %s line %ld",prg_nm_get(),fl_spt_glb,ln_nbr_crr);
  if(dbg_lvl_get() > 1) (void)fprintf(stderr," %s",err_sng);
  if(dbg_lvl_get() > 0) (void)fprintf(stderr,"\n");
  
  if(err_sng[0] == '#') eprovoke_skip=True;
  eprovoke_skip=eprovoke_skip; /* Do nothing except avoid compiler warnings */
  return 0;
} /* end yyerror() */

void quick_free(var_sct **var)
{
  if(*var != NULL) *var=var_free(*var);
}
