
/*  A Bison parser, made from ../src/nco/ncap_yacc.y
    by GNU Bison version 1.28  */

#define YYBISON 1  /* Identify Bison output.  */

#define	OUT_ATT	257
#define	COMPARISON	258
#define	LHS_SBS	259
#define	SCV	260
#define	SNG	261
#define	FUNCTION	262
#define	OUT_VAR	263
#define	VAR	264
#define	CNV_TYPE	265
#define	ABS	266
#define	ATOSTR	267
#define	EPROVOKE	268
#define	IGNORE	269
#define	NAMED_CONSTANT	270
#define	PACK	271
#define	POWER	272
#define	RDC	273
#define	UNPACK	274
#define	IF	275
#define	PRINT	276
#define	AND	277
#define	NOT	278
#define	OR	279
#define	UMINUS	280
#define	LOWER_THAN_ELSE	281
#define	ELSE	282

#line 1 "../src/nco/ncap_yacc.y"
 /* $Header: /data/zender/nco_20150216/nco/src/nco/ncap_yacc.c,v 1.1 2003-01-09 00:50:37 zender Exp $ -*-C-*- */

/* Begin C declarations section */
 
/* Purpose: Grammar parser for ncap */

/* Copyright (C) 1995--2003 Charlie Zender
   This software is distributed under the terms of the GNU General Public License
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

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

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions */

/* Personal headers */
#include "nco.h" /* NCO definitions */
#include "nco_netcdf.h" /* netCDF3 wrapper calls */
#include "ncap.h" /* Symbol table definition */

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
extern size_t ncl_dpt_crr; /* [nbr] Depth of current #include file (declared in ncap.c) */
extern size_t *ln_nbr_crr; /* [cnt] Line number (declared in ncap.c) */
extern char **fl_spt_glb; /* [fl] Script file (declared in ncap.c) */
extern char err_sng[200]; /* [sng] Buffer for error string (declared in ncap.l) */

/* End C declarations section */

#line 69 "../src/nco/ncap_yacc.y"
typedef union{
  char *sng; /* [sng] String value */
  char *var_nm_LHS; /* [sng] Variables on LHS */
  char *var_nm_RHS; /* [sng] Variables on RHS */
  aed_sct aed; /* [sct] Attribute */
  sym_sct *sym; /* [sct] Intrinsic function name */
  scv_sct scv; /* [sct] Scalar value */
  var_sct *var; /* [sct] Variable */
  nm_lst_sct *sbs_lst; /* [sct] Subscript list */
  int nco_rlt_opr; /* [enm] Comparison operator type */
  nc_type cnv_type;  /* [enm] used for type conversion functions */
} YYSTYPE;
#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		171
#define	YYFLAG		-32768
#define	YYNTBASE	40

#define YYTRANSLATE(x) ((unsigned)(x) <= 282 ? yytranslate[x] : 51)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,    30,     2,     2,    36,
    37,    28,    26,    39,    27,     2,    29,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,    35,     2,
    38,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,    31,     2,     2,     2,     2,     2,     2,
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
     7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
    17,    18,    19,    20,    21,    22,    23,    24,    25,    32,
    33,    34
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     2,     6,    10,    13,    16,    22,    23,    24,    44,
    48,    52,    56,    60,    64,    68,    72,    76,    80,    84,
    88,    91,    94,    98,   105,   110,   115,   120,   124,   126,
   128,   130,   134,   139,   146,   148,   152,   156,   160,   164,
   168,   172,   176,   180,   184,   188,   192,   196,   200,   204,
   208,   212,   216,   223,   226,   229,   234,   239,   244,   249,
   254,   258,   263,   265
};

static const short yyrhs[] = {    41,
     0,    41,    42,    35,     0,    41,     1,    35,     0,    42,
    35,     0,     1,    35,     0,    21,    36,    49,    37,    42,
     0,     0,     0,    21,    36,    49,    37,    42,    34,    42,
    43,    22,    36,    45,    37,    35,    44,    22,    36,    50,
    37,    35,     0,    47,    38,    45,     0,    47,    38,    48,
     0,    47,    38,    50,     0,    46,    38,    50,     0,    46,
    38,    45,     0,    46,    38,    48,     0,    45,    26,    45,
     0,    45,    27,    45,     0,    45,    28,    45,     0,    45,
    29,    45,     0,    45,    30,    45,     0,    27,    45,     0,
    26,    45,     0,    45,    31,    45,     0,    18,    36,    45,
    39,    45,    37,     0,    12,    36,    45,    37,     0,     8,
    36,    45,    37,     0,    11,    36,    45,    37,     0,    36,
    45,    37,     0,     6,     0,     9,     0,     3,     0,    48,
    26,    48,     0,    13,    36,    45,    37,     0,    13,    36,
    45,    39,    48,    37,     0,     7,     0,    50,     4,    50,
     0,    45,     4,    45,     0,    50,     4,    45,     0,    45,
     4,    50,     0,    50,    26,    50,     0,    50,    26,    45,
     0,    45,    26,    50,     0,    50,    27,    50,     0,    45,
    27,    50,     0,    50,    27,    45,     0,    50,    28,    50,
     0,    50,    28,    45,     0,    50,    30,    45,     0,    45,
    28,    50,     0,    50,    29,    50,     0,    50,    29,    45,
     0,    50,    31,    45,     0,    18,    36,    50,    39,    45,
    37,     0,    27,    50,     0,    26,    50,     0,    12,    36,
    50,    37,     0,    19,    36,    50,    37,     0,    17,    36,
    50,    37,     0,    20,    36,    50,    37,     0,     8,    36,
    50,    37,     0,    36,    50,    37,     0,    11,    36,    50,
    37,     0,    16,     0,    10,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   126,   130,   136,   137,   138,   142,   149,   153,   156,   159,
   185,   205,   228,   246,   285,   313,   319,   323,   327,   331,
   336,   340,   343,   356,   369,   372,   382,   386,   387,   390,
   393,   396,   406,   418,   433,   436,   439,   441,   443,   447,
   454,   458,   462,   467,   478,   482,   486,   490,   494,   498,
   502,   506,   510,   514,   521,   524,   528,   536,   540,   544,
   548,   551,   556,   559
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","OUT_ATT",
"COMPARISON","LHS_SBS","SCV","SNG","FUNCTION","OUT_VAR","VAR","CNV_TYPE","ABS",
"ATOSTR","EPROVOKE","IGNORE","NAMED_CONSTANT","PACK","POWER","RDC","UNPACK",
"IF","PRINT","AND","NOT","OR","'+'","'-'","'*'","'/'","'%'","'^'","UMINUS","LOWER_THAN_ELSE",
"ELSE","';'","'('","')'","'='","','","program","stmt_lst","stmt","@1","@2","scv_xpr",
"out_var_xpr","out_att_xpr","sng_xpr","bln_xpr","var_xpr", NULL
};
#endif

static const short yyr1[] = {     0,
    40,    41,    41,    41,    41,    42,    43,    44,    42,    42,
    42,    42,    42,    42,    42,    45,    45,    45,    45,    45,
    45,    45,    45,    45,    45,    45,    45,    45,    45,    46,
    47,    48,    48,    48,    48,    49,    49,    49,    49,    50,
    50,    50,    50,    50,    50,    50,    50,    50,    50,    50,
    50,    50,    50,    50,    50,    50,    50,    50,    50,    50,
    50,    50,    50,    50
};

static const short yyr2[] = {     0,
     1,     3,     3,     2,     2,     5,     0,     0,    19,     3,
     3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
     2,     2,     3,     6,     4,     4,     4,     3,     1,     1,
     1,     3,     4,     6,     1,     3,     3,     3,     3,     3,
     3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
     3,     3,     6,     2,     2,     4,     4,     4,     4,     4,
     3,     4,     1,     1
};

static const short yydefact[] = {     0,
     0,    31,    30,     0,     0,     0,     0,     0,     5,     0,
     0,     0,     4,     0,     0,    29,     0,    64,     0,     0,
    63,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     3,     2,    35,     0,    14,    15,    13,    10,    11,
    12,     0,     0,     0,     0,     0,     0,     0,    22,    55,
    21,    54,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,    28,    61,    37,    39,    16,    42,    17,
    44,    18,    49,     0,     0,     0,     0,     0,     0,     0,
    19,    20,    23,     6,    38,    36,    41,    40,    45,    43,
    47,    46,    51,    50,    48,    52,     0,    32,    26,    60,
    27,    62,    25,    56,    58,     0,     0,    57,    59,     0,
     0,     0,     0,    22,    21,     0,     0,     0,     0,     0,
    33,     0,     0,     0,     0,     0,     0,     0,    16,    17,
    18,     7,     0,    24,    53,     0,    34,     0,     0,     0,
     0,     8,     0,     0,     0,     0,     0,     9,     0,     0,
     0
};

static const short yydefgoto[] = {   169,
     5,     6,   156,   163,    78,     7,     8,    37,    30,    31
};

static const short yypact[] = {     1,
   -29,-32768,-32768,   -28,   184,   -26,   -27,   -22,-32768,    57,
   -12,   -11,-32768,   109,   109,-32768,     2,-32768,     3,    11,
-32768,    12,    19,    20,    21,    57,    57,    57,   126,    25,
   133,-32768,-32768,-32768,    28,   437,     0,   443,   437,     0,
   443,    57,    57,    57,    57,    57,    57,    57,-32768,-32768,
-32768,-32768,   209,   221,    57,    57,    57,    57,     9,     9,
     9,    16,    57,    57,    57,    57,    57,     9,     9,     9,
    33,   233,   245,   257,   269,   281,   293,   437,   305,   183,
   189,   317,   329,-32768,-32768,   437,   443,    50,    66,    50,
    66,    35,    51,    49,    53,    55,    56,     9,     9,     9,
    35,    35,    35,    64,   437,   443,    50,    66,    50,    66,
    35,    51,    35,    51,    35,    35,   169,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,     9,     9,-32768,-32768,     9,
     9,     9,     9,-32768,-32768,   341,     9,     9,     9,    16,
-32768,    33,   353,   365,   377,   389,   401,   203,   118,   118,
    35,-32768,   -19,-32768,-32768,    84,-32768,    82,     9,   413,
    88,-32768,   102,    95,    57,   425,    98,-32768,   134,   139,
-32768
};

static const short yypgoto[] = {-32768,
-32768,    -2,-32768,-32768,    44,-32768,-32768,   -10,-32768,   -14
};


#define	YYLAST		474


static const short yytable[] = {    38,
    41,     1,    12,     2,    40,     9,    71,    10,    13,     3,
    14,    50,    52,    54,    16,    15,    94,   157,     2,    95,
    96,     4,    32,    33,     3,    71,    97,    73,    75,    77,
    79,    81,    82,    83,    98,    99,     4,    42,    43,    34,
    87,    89,    91,    93,   100,    35,    44,    45,   106,   108,
   110,   112,   114,    29,    46,    47,    48,    36,    39,   104,
   118,    62,    16,    70,    17,    61,    18,    19,    20,    49,
    51,    53,    21,    22,    23,    24,    25,    58,    59,    60,
    61,    69,    26,    27,   130,    72,    74,    76,   131,    80,
   132,   133,    28,    66,    67,    68,    69,   140,    86,    88,
    90,    92,   101,   102,   103,   158,   105,   107,   109,   111,
   113,   115,   116,   117,    16,    34,    17,   159,    18,    19,
    20,    35,   162,   164,    21,    22,    23,    24,    25,    55,
   165,   153,   168,   170,    26,    27,    63,   152,   171,     0,
     0,   134,   135,   136,    28,   139,    59,    60,    61,     0,
   166,    56,    57,    58,    59,    60,    61,     0,    64,    65,
    66,    67,    68,    69,     0,     0,     0,     0,     0,   143,
   144,     0,     0,   145,   146,   147,   148,     0,     0,     0,
   149,   150,   151,    -1,    11,     0,     2,     0,     0,     0,
     0,     0,     3,     0,   137,   138,   139,    59,    60,    61,
     0,     0,   160,     0,     4,   141,     0,   142,    56,    57,
    58,    59,    60,    61,    64,    65,    66,    67,    68,    69,
     0,   126,     0,     0,     0,     0,     0,   127,   137,   138,
   139,    59,    60,    61,    56,    57,    58,    59,    60,    61,
     0,   126,     0,     0,     0,    84,    64,    65,    66,    67,
    68,    69,     0,     0,     0,     0,     0,    85,    56,    57,
    58,    59,    60,    61,     0,     0,     0,     0,     0,   119,
    64,    65,    66,    67,    68,    69,     0,     0,     0,     0,
     0,   120,    56,    57,    58,    59,    60,    61,     0,     0,
     0,     0,     0,   121,    64,    65,    66,    67,    68,    69,
     0,     0,     0,     0,     0,   122,    56,    57,    58,    59,
    60,    61,     0,     0,     0,     0,     0,   123,    64,    65,
    66,    67,    68,    69,     0,     0,     0,     0,     0,   124,
    64,    65,    66,    67,    68,    69,     0,     0,     0,     0,
     0,   125,    64,    65,    66,    67,    68,    69,     0,     0,
     0,     0,     0,   128,    64,    65,    66,    67,    68,    69,
     0,     0,     0,     0,     0,   129,   137,   138,   139,    59,
    60,    61,     0,     0,     0,     0,     0,    84,   137,   138,
   139,    59,    60,    61,     0,     0,     0,     0,     0,   154,
   137,   138,   139,    59,    60,    61,     0,     0,     0,     0,
     0,   155,   137,   138,   139,    59,    60,    61,     0,     0,
     0,     0,     0,   119,   137,   138,   139,    59,    60,    61,
     0,     0,     0,     0,     0,   121,   137,   138,   139,    59,
    60,    61,     0,     0,     0,     0,     0,   123,   137,   138,
   139,    59,    60,    61,     0,     0,     0,     0,     0,   161,
    64,    65,    66,    67,    68,    69,     0,     0,     0,     0,
     0,   167,    56,    57,    58,    59,    60,    61,    64,    65,
    66,    67,    68,    69
};

static const short yycheck[] = {    14,
    15,     1,     5,     3,    15,    35,    26,    36,    35,     9,
    38,    26,    27,    28,     6,    38,     8,    37,     3,    11,
    12,    21,    35,    35,     9,    26,    18,    42,    43,    44,
    45,    46,    47,    48,    26,    27,    21,    36,    36,     7,
    55,    56,    57,    58,    36,    13,    36,    36,    63,    64,
    65,    66,    67,    10,    36,    36,    36,    14,    15,    62,
    71,    37,     6,    36,     8,    31,    10,    11,    12,    26,
    27,    28,    16,    17,    18,    19,    20,    28,    29,    30,
    31,    31,    26,    27,    36,    42,    43,    44,    36,    46,
    36,    36,    36,    28,    29,    30,    31,    34,    55,    56,
    57,    58,    59,    60,    61,    22,    63,    64,    65,    66,
    67,    68,    69,    70,     6,     7,     8,    36,    10,    11,
    12,    13,    35,    22,    16,    17,    18,    19,    20,     4,
    36,   142,    35,     0,    26,    27,     4,   140,     0,    -1,
    -1,    98,    99,   100,    36,    28,    29,    30,    31,    -1,
   165,    26,    27,    28,    29,    30,    31,    -1,    26,    27,
    28,    29,    30,    31,    -1,    -1,    -1,    -1,    -1,   126,
   127,    -1,    -1,   130,   131,   132,   133,    -1,    -1,    -1,
   137,   138,   139,     0,     1,    -1,     3,    -1,    -1,    -1,
    -1,    -1,     9,    -1,    26,    27,    28,    29,    30,    31,
    -1,    -1,   159,    -1,    21,    37,    -1,    39,    26,    27,
    28,    29,    30,    31,    26,    27,    28,    29,    30,    31,
    -1,    39,    -1,    -1,    -1,    -1,    -1,    39,    26,    27,
    28,    29,    30,    31,    26,    27,    28,    29,    30,    31,
    -1,    39,    -1,    -1,    -1,    37,    26,    27,    28,    29,
    30,    31,    -1,    -1,    -1,    -1,    -1,    37,    26,    27,
    28,    29,    30,    31,    -1,    -1,    -1,    -1,    -1,    37,
    26,    27,    28,    29,    30,    31,    -1,    -1,    -1,    -1,
    -1,    37,    26,    27,    28,    29,    30,    31,    -1,    -1,
    -1,    -1,    -1,    37,    26,    27,    28,    29,    30,    31,
    -1,    -1,    -1,    -1,    -1,    37,    26,    27,    28,    29,
    30,    31,    -1,    -1,    -1,    -1,    -1,    37,    26,    27,
    28,    29,    30,    31,    -1,    -1,    -1,    -1,    -1,    37,
    26,    27,    28,    29,    30,    31,    -1,    -1,    -1,    -1,
    -1,    37,    26,    27,    28,    29,    30,    31,    -1,    -1,
    -1,    -1,    -1,    37,    26,    27,    28,    29,    30,    31,
    -1,    -1,    -1,    -1,    -1,    37,    26,    27,    28,    29,
    30,    31,    -1,    -1,    -1,    -1,    -1,    37,    26,    27,
    28,    29,    30,    31,    -1,    -1,    -1,    -1,    -1,    37,
    26,    27,    28,    29,    30,    31,    -1,    -1,    -1,    -1,
    -1,    37,    26,    27,    28,    29,    30,    31,    -1,    -1,
    -1,    -1,    -1,    37,    26,    27,    28,    29,    30,    31,
    -1,    -1,    -1,    -1,    -1,    37,    26,    27,    28,    29,
    30,    31,    -1,    -1,    -1,    -1,    -1,    37,    26,    27,
    28,    29,    30,    31,    -1,    -1,    -1,    -1,    -1,    37,
    26,    27,    28,    29,    30,    31,    -1,    -1,    -1,    -1,
    -1,    37,    26,    27,    28,    29,    30,    31,    26,    27,
    28,    29,    30,    31
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
#line 131 "../src/nco/ncap_yacc.y"
{
  /* Purpose: Actions to be performed at end-of-statement go here */
  /* Clean up from and exit LHS_cst mode */
  (void)nco_var_free_wrp(&((prs_sct *)prs_arg)->var_LHS);
;
    break;}
case 3:
#line 136 "../src/nco/ncap_yacc.y"
{(void)nco_var_free_wrp(&((prs_sct *)prs_arg)->var_LHS);;
    break;}
case 4:
#line 137 "../src/nco/ncap_yacc.y"
{(void)nco_var_free_wrp(&((prs_sct *)prs_arg)->var_LHS);;
    break;}
case 5:
#line 138 "../src/nco/ncap_yacc.y"
{(void)nco_var_free_wrp(&((prs_sct *)prs_arg)->var_LHS);;
    break;}
case 6:
#line 145 "../src/nco/ncap_yacc.y"
{
  /* LMB92 p. 234 */
  ;
;
    break;}
case 7:
#line 149 "../src/nco/ncap_yacc.y"
{
  /* LMB92 p. 234 */
  ;
;
    break;}
case 8:
#line 153 "../src/nco/ncap_yacc.y"
{
  ;
;
    break;}
case 9:
#line 156 "../src/nco/ncap_yacc.y"
{
  ;
;
    break;}
case 10:
#line 159 "../src/nco/ncap_yacc.y"
{ 
  aed_sct *ptr_aed;

  ptr_aed=ncap_aed_lookup(yyvsp[-2].aed.var_nm,yyvsp[-2].aed.att_nm,((prs_sct *)prs_arg),True);

  ptr_aed->val=ncap_scv_2_ptr_unn(yyvsp[0].scv);
  ptr_aed->type=yyvsp[0].scv.type;
  ptr_aed->sz=1L;
  (void)cast_nctype_void(ptr_aed->type,&ptr_aed->val);    
  if(dbg_lvl_get() > 0) (void)sprintf(err_sng,"Saving attribute %s@%s to %s",yyvsp[-2].aed.var_nm,yyvsp[-2].aed.att_nm,((prs_sct *)prs_arg)->fl_out);
  (void)yyerror(err_sng);

  if(dbg_lvl_get() > 1){
    (void)fprintf(stderr,"Saving in array attribute %s@%s=",yyvsp[-2].aed.var_nm,yyvsp[-2].aed.att_nm);
    switch(yyvsp[0].scv.type){
    case NC_BYTE: (void)fprintf(stderr,"%d\n",yyvsp[0].scv.val.b); break;
    case NC_SHORT: (void)fprintf(stderr,"%d\n",yyvsp[0].scv.val.s); break;
    case NC_INT: (void)fprintf(stderr,"%ld\n",yyvsp[0].scv.val.l); break;
    case NC_FLOAT: (void)fprintf(stderr,"%G\n",yyvsp[0].scv.val.f); break;		  
    case NC_DOUBLE: (void)fprintf(stderr,"%.5G\n",yyvsp[0].scv.val.d);break;
    default: break;
    } /* end switch */
  } /* end if */
  yyvsp[-2].aed.var_nm=(char *)nco_free(yyvsp[-2].aed.var_nm);
  yyvsp[-2].aed.att_nm=(char *)nco_free(yyvsp[-2].aed.att_nm);
;
    break;}
case 11:
#line 186 "../src/nco/ncap_yacc.y"
{
  aed_sct *ptr_aed;
  int aed_idx; 
  size_t sng_lng;
  
  sng_lng=strlen(yyvsp[0].sng);
  ptr_aed=ncap_aed_lookup(yyvsp[-2].aed.var_nm,yyvsp[-2].aed.att_nm,((prs_sct *)prs_arg),True);
  ptr_aed->type=NC_CHAR;
  ptr_aed->sz=(long)((sng_lng+1)*nco_typ_lng(NC_CHAR));
  ptr_aed->val.cp=(unsigned char *)nco_malloc((sng_lng+1)*nco_typ_lng(NC_CHAR));
  strcpy((char *)(ptr_aed->val.cp),yyvsp[0].sng);
  (void)cast_nctype_void((nc_type)NC_CHAR,&ptr_aed->val);    
  
  if(dbg_lvl_get() > 0) (void)sprintf(err_sng,"Saving attribute %s@%s=%s",yyvsp[-2].aed.var_nm,yyvsp[-2].aed.att_nm,yyvsp[0].sng);
  (void)yyerror(err_sng);
  yyvsp[-2].aed.var_nm=(char *)nco_free(yyvsp[-2].aed.var_nm);
  yyvsp[-2].aed.att_nm=(char *)nco_free(yyvsp[-2].aed.att_nm);
  yyvsp[0].sng=(char *)nco_free(yyvsp[0].sng);
;
    break;}
case 12:
#line 206 "../src/nco/ncap_yacc.y"
{ 
  /* Storing 0-dimensional variables in attribute is OK */ 
  int aed_idx;
  aed_sct *ptr_aed;
  
  if(yyvsp[0].var->nbr_dim < 2){
     ptr_aed=ncap_aed_lookup(yyvsp[-2].aed.var_nm,yyvsp[-2].aed.att_nm,((prs_sct *)prs_arg),True);
    ptr_aed->sz=yyvsp[0].var->sz;
    ptr_aed->type=yyvsp[0].var->type;
    ptr_aed->val.vp=(void*)nco_malloc((ptr_aed->sz)*nco_typ_lng(ptr_aed->type));
    (void)var_copy(ptr_aed->type,ptr_aed->sz,yyvsp[0].var->val,ptr_aed->val);
    /* cast_nctype_void($3->type,&ptr_aed->val); */
    if(dbg_lvl_get() > 0) (void)sprintf(err_sng,"Saving attribute %s@%s %d dimensional variable",yyvsp[-2].aed.var_nm,yyvsp[-2].aed.att_nm,yyvsp[0].var->nbr_dim);
    (void)yyerror(err_sng); 
  }else{
    (void)sprintf(err_sng,"Warning: Cannot store in attribute %s@%s a variable with dimension %d",yyvsp[-2].aed.var_nm,yyvsp[-2].aed.att_nm,yyvsp[0].var->nbr_dim);
    (void)yyerror(err_sng);
  } /* endif */
  yyvsp[-2].aed.var_nm=(char *)nco_free(yyvsp[-2].aed.var_nm);
  yyvsp[-2].aed.att_nm=(char *)nco_free(yyvsp[-2].aed.att_nm);
  (void)nco_var_free(yyvsp[0].var); 
;
    break;}
case 13:
#line 229 "../src/nco/ncap_yacc.y"
{
  int rcd;
  int var_id;
  yyvsp[0].var->nm=strdup(yyvsp[-2].var_nm_LHS);
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
  yyvsp[-2].var_nm_LHS=(char *)nco_free(yyvsp[-2].var_nm_LHS);
  (void)nco_var_free(yyvsp[0].var);
;
    break;}
case 14:
#line 247 "../src/nco/ncap_yacc.y"
{
  int rcd;
  int var_id;
  var_sct *var;
  rcd=nco_inq_varid_flg(((prs_sct *)prs_arg)->out_id,yyvsp[-2].var_nm_LHS,&var_id);
  if(dbg_lvl_get() > 5) (void)fprintf(stderr,"%s: DEBUG out_var_xpr = scv_xpr rule for %s\n",prg_nm_get(),yyvsp[-2].var_nm_LHS);
  if(rcd == NC_NOERR){
    (void)sprintf(err_sng,"Warning: Variable %s has aleady been saved in %s",yyvsp[-2].var_nm_LHS,((prs_sct *)prs_arg)->fl_out);
    (void)yyerror(err_sng);
  }else{  
    /* Turn attribute into temporary variable for writing */
    var=(var_sct *)nco_malloc(sizeof(var_sct));
    /* Set defaults */
    (void)var_dfl_set(var); /* [fnc] Set defaults for each member of variable structure */
    /* Overwrite with attribute expression information */
    var->nm=strdup(yyvsp[-2].var_nm_LHS);
    var->nbr_dim=0;
    var->sz=1;
    var->val=ncap_scv_2_ptr_unn(yyvsp[0].scv);
    var->type=yyvsp[0].scv.type;

    if(((prs_sct *)prs_arg)->var_LHS != NULL){
      /* User intends LHS to cast RHS to same dimensionality
	 Stretch newly initialized variable to size of LHS template */
      /*    (void)ncap_var_cnf_dmn(&$$,&(((prs_sct *)prs_arg)->var_LHS));*/
      (void)ncap_var_stretch(&var,&(((prs_sct *)prs_arg)->var_LHS));
      
      if(dbg_lvl_get() > 2) (void)fprintf(stderr,"%s: Stretching former scv_xpr defining %s with LHS template: Template var->nm %s, var->nbr_dim %d, var->sz %li\n",prg_nm_get(),yyvsp[-2].var_nm_LHS,((prs_sct *)prs_arg)->var_LHS->nm,((prs_sct *)prs_arg)->var_LHS->nbr_dim,((prs_sct *)prs_arg)->var_LHS->sz);
      
    } /* endif LHS_cst */
    
    (void)ncap_var_write(var,(prs_sct *)prs_arg);
    (void)nco_var_free(var);
    if(dbg_lvl_get() > 0) (void)sprintf(err_sng,"Saving variable %s to %s",yyvsp[-2].var_nm_LHS,((prs_sct *)prs_arg)->fl_out);
    (void)yyerror(err_sng);
  } /* endif */
  yyvsp[-2].var_nm_LHS=(char *)nco_free(yyvsp[-2].var_nm_LHS);
;
    break;}
case 15:
#line 286 "../src/nco/ncap_yacc.y"
{
  int rcd;
  int var_id;
  var_sct *var;
  rcd=nco_inq_varid_flg(((prs_sct *)prs_arg)->out_id,yyvsp[-2].var_nm_LHS,&var_id);
  if(rcd == NC_NOERR){
    (void)sprintf(err_sng,"Warning: Variable %s has aleady been saved in %s",yyvsp[-2].var_nm_LHS,((prs_sct *)prs_arg)->fl_out);
    (void)yyerror(err_sng);  
  }else{  
    var=(var_sct *)nco_calloc((size_t)1,sizeof(var_sct));
    var->nm=strdup(yyvsp[-2].var_nm_LHS);
    var->nbr_dim=0;
    var->dmn_id=(int *)NULL;
    var->sz=strlen(yyvsp[0].sng)+1;
    var->val.cp=(unsigned char *)strdup(yyvsp[0].sng);
    var->type=NC_CHAR;
    (void)cast_nctype_void((nc_type)NC_CHAR,&var->val);
    (void)ncap_var_write(var,(prs_sct *)prs_arg);
    (void)nco_var_free(var);
    if(dbg_lvl_get() > 0) (void)sprintf(err_sng,"Saving variable %s to %s",yyvsp[-2].var_nm_LHS,((prs_sct *)prs_arg)->fl_out);
    (void)yyerror(err_sng);
  } /* endelse */
  yyvsp[-2].var_nm_LHS=(char *)nco_free(yyvsp[-2].var_nm_LHS);
  yyvsp[0].sng=(char *)nco_free(yyvsp[0].sng);
;
    break;}
case 16:
#line 315 "../src/nco/ncap_yacc.y"
{
  (void)ncap_scv_scv_cnf_typ_hgh_prc(&yyvsp[-2].scv,&yyvsp[0].scv);
  yyval.scv=ncap_scv_clc(yyvsp[-2].scv,'+',yyvsp[0].scv);                                
;
    break;}
case 17:
#line 319 "../src/nco/ncap_yacc.y"
{
  (void)ncap_scv_scv_cnf_typ_hgh_prc(&yyvsp[-2].scv,&yyvsp[0].scv); 
  yyval.scv=ncap_scv_clc(yyvsp[-2].scv,'-',yyvsp[0].scv);
;
    break;}
case 18:
#line 323 "../src/nco/ncap_yacc.y"
{
  (void)ncap_scv_scv_cnf_typ_hgh_prc(&yyvsp[-2].scv,&yyvsp[0].scv);
  yyval.scv=ncap_scv_clc(yyvsp[-2].scv,'*',yyvsp[0].scv);
;
    break;}
case 19:
#line 327 "../src/nco/ncap_yacc.y"
{
  (void)ncap_scv_scv_cnf_typ_hgh_prc(&yyvsp[-2].scv,&yyvsp[0].scv); 
  yyval.scv=ncap_scv_clc(yyvsp[-2].scv,'/',yyvsp[0].scv);  
;
    break;}
case 20:
#line 331 "../src/nco/ncap_yacc.y"
{
  (void)ncap_scv_scv_cnf_typ_hgh_prc(&yyvsp[-2].scv,&yyvsp[0].scv);
  
  yyval.scv=ncap_scv_clc(yyvsp[-2].scv,'%',yyvsp[0].scv);  
;
    break;}
case 21:
#line 336 "../src/nco/ncap_yacc.y"
{
  (void)ncap_scv_minus(&yyvsp[0].scv);
  yyval.scv=yyvsp[0].scv;
;
    break;}
case 22:
#line 340 "../src/nco/ncap_yacc.y"
{
  yyval.scv=yyvsp[0].scv;
;
    break;}
case 23:
#line 343 "../src/nco/ncap_yacc.y"
{
  if(yyvsp[-2].scv.type <= NC_FLOAT && yyvsp[0].scv.type <= NC_FLOAT) {
    (void)scv_conform_type((nc_type)NC_FLOAT,&yyvsp[-2].scv);
    (void)scv_conform_type((nc_type)NC_FLOAT,&yyvsp[0].scv);
    yyval.scv.val.f=powf(yyvsp[-2].scv.val.f,yyvsp[0].scv.val.f);
    yyval.scv.type=NC_FLOAT;
  }else{
    (void)scv_conform_type((nc_type)NC_DOUBLE,&yyvsp[-2].scv);
    (void)scv_conform_type((nc_type)NC_DOUBLE,&yyvsp[0].scv);
    yyval.scv.val.d=pow(yyvsp[-2].scv.val.d,yyvsp[0].scv.val.d);
    yyval.scv.type=NC_DOUBLE; 
  } /* end else */
;
    break;}
case 24:
#line 356 "../src/nco/ncap_yacc.y"
{ /* fxm: this is identical to previous clause except for argument numbering, should be functionalized to use common code */
  if(yyvsp[-3].scv.type <= NC_FLOAT && yyvsp[-1].scv.type <= NC_FLOAT) {
    (void)scv_conform_type((nc_type)NC_FLOAT,&yyvsp[-3].scv);
    (void)scv_conform_type((nc_type)NC_FLOAT,&yyvsp[-1].scv);
    yyval.scv.val.f=powf(yyvsp[-3].scv.val.f,yyvsp[-1].scv.val.f);
    yyval.scv.type=NC_FLOAT;
  }else{ 
    (void)scv_conform_type((nc_type)NC_DOUBLE,&yyvsp[-3].scv);
    (void)scv_conform_type((nc_type)NC_DOUBLE,&yyvsp[-1].scv);
    yyval.scv.val.d=pow(yyvsp[-3].scv.val.d,yyvsp[-1].scv.val.d);
    yyval.scv.type=NC_DOUBLE; 
  } /* end else */
;
    break;}
case 25:
#line 369 "../src/nco/ncap_yacc.y"
{
  yyval.scv=ncap_scv_abs(yyvsp[-1].scv);
;
    break;}
case 26:
#line 372 "../src/nco/ncap_yacc.y"
{
  if(yyvsp[-1].scv.type <= NC_FLOAT) {
    (void)scv_conform_type((nc_type)NC_FLOAT,&yyvsp[-1].scv);
    yyval.scv.val.f=(*(yyvsp[-3].sym->fnc_flt))(yyvsp[-1].scv.val.f);
    yyval.scv.type=NC_FLOAT;
  }else{
    yyval.scv.val.d=(*(yyvsp[-3].sym->fnc_dbl))(yyvsp[-1].scv.val.d);
    yyval.scv.type=NC_DOUBLE;
  } /* end else */
;
    break;}
case 27:
#line 382 "../src/nco/ncap_yacc.y"
{
  (void)scv_conform_type( yyvsp[-3].cnv_type,&yyvsp[-1].scv);
  yyval.scv=yyvsp[-1].scv;
;
    break;}
case 28:
#line 386 "../src/nco/ncap_yacc.y"
{yyval.scv=yyvsp[-1].scv;;
    break;}
case 29:
#line 387 "../src/nco/ncap_yacc.y"
{yyval.scv=yyvsp[0].scv;;
    break;}
case 30:
#line 390 "../src/nco/ncap_yacc.y"
{yyval.var_nm_LHS=yyvsp[0].var_nm_LHS;;
    break;}
case 31:
#line 393 "../src/nco/ncap_yacc.y"
{yyval.aed=yyvsp[0].aed;;
    break;}
case 32:
#line 397 "../src/nco/ncap_yacc.y"
{
  size_t sng_lng;
  sng_lng=strlen(yyvsp[-2].sng)+strlen(yyvsp[0].sng);
  yyval.sng=(char*)nco_malloc((sng_lng+1)*sizeof(char));
  strcpy(yyval.sng,yyvsp[-2].sng);
  strcat(yyval.sng,yyvsp[0].sng);
  yyvsp[-2].sng=(char *)nco_free(yyvsp[-2].sng);
  yyvsp[0].sng=(char *)nco_free(yyvsp[0].sng);
;
    break;}
case 33:
#line 406 "../src/nco/ncap_yacc.y"
{
  char bfr[50];
  switch (yyvsp[-1].scv.type){
  case NC_DOUBLE: sprintf(bfr,"%.10G",yyvsp[-1].scv.val.d); break;
  case NC_FLOAT: sprintf(bfr,"%G",yyvsp[-1].scv.val.f); break;
  case NC_INT: sprintf(bfr,"%ld",yyvsp[-1].scv.val.l); break;
  case NC_SHORT: sprintf(bfr,"%d",yyvsp[-1].scv.val.s); break;
  case NC_BYTE: sprintf(bfr,"%d",yyvsp[-1].scv.val.b); break;
  default:  break;
  } /* end switch */
  yyval.sng=strdup(bfr);      
;
    break;}
case 34:
#line 418 "../src/nco/ncap_yacc.y"
{
  char bfr[150];
  /* Format string according to string expression */
  /* User decides which format corresponds to which type */
  switch (yyvsp[-3].scv.type){
  case NC_DOUBLE: sprintf(bfr,yyvsp[-1].sng,yyvsp[-3].scv.val.d); break;
  case NC_FLOAT: sprintf(bfr,yyvsp[-1].sng,yyvsp[-3].scv.val.f); break;
  case NC_INT: sprintf(bfr,yyvsp[-1].sng,yyvsp[-3].scv.val.l); break;
  case NC_SHORT: sprintf(bfr,yyvsp[-1].sng,yyvsp[-3].scv.val.s); break;
  case NC_BYTE: sprintf(bfr,yyvsp[-1].sng,yyvsp[-3].scv.val.b); break;
  default:  break;
  } /* end switch */
  yyvsp[-1].sng=(char *)nco_free(yyvsp[-1].sng);
  yyval.sng=strdup(bfr);      
;
    break;}
case 35:
#line 433 "../src/nco/ncap_yacc.y"
{yyval.sng=yyvsp[0].sng;;
    break;}
case 36:
#line 437 "../src/nco/ncap_yacc.y"
{
;
    break;}
case 37:
#line 439 "../src/nco/ncap_yacc.y"
{
;
    break;}
case 38:
#line 441 "../src/nco/ncap_yacc.y"
{
;
    break;}
case 39:
#line 443 "../src/nco/ncap_yacc.y"
{
;
    break;}
case 40:
#line 449 "../src/nco/ncap_yacc.y"
{ 
  yyval.var=ncap_var_var_add(yyvsp[-2].var,yyvsp[0].var); 
  nco_var_free(yyvsp[-2].var);
  nco_var_free(yyvsp[0].var);
;
    break;}
case 41:
#line 454 "../src/nco/ncap_yacc.y"
{
  yyval.var=ncap_var_scv_add(yyvsp[-2].var,yyvsp[0].scv);
  nco_var_free(yyvsp[-2].var);
;
    break;}
case 42:
#line 458 "../src/nco/ncap_yacc.y"
{
  yyval.var=ncap_var_scv_add(yyvsp[0].var,yyvsp[-2].scv);
  nco_var_free(yyvsp[0].var);
;
    break;}
case 43:
#line 462 "../src/nco/ncap_yacc.y"
{ 
  yyval.var=ncap_var_var_sub(yyvsp[-2].var,yyvsp[0].var);
  nco_var_free(yyvsp[-2].var); 
  nco_var_free(yyvsp[0].var);
;
    break;}
case 44:
#line 467 "../src/nco/ncap_yacc.y"
{ 
  var_sct *var1;
  scv_sct minus;
  minus.val.b=-1;
  minus.type=NC_BYTE;
  (void)scv_conform_type(yyvsp[0].var->type,&minus);
  var1=ncap_var_scv_sub(yyvsp[0].var,yyvsp[-2].scv);
  yyval.var=ncap_var_scv_mlt(var1,minus);
  nco_var_free(var1);
  nco_var_free(yyvsp[0].var);
;
    break;}
case 45:
#line 478 "../src/nco/ncap_yacc.y"
{
  yyval.var=ncap_var_scv_sub(yyvsp[-2].var,yyvsp[0].scv);
  nco_var_free(yyvsp[-2].var);
;
    break;}
case 46:
#line 482 "../src/nco/ncap_yacc.y"
{
  yyval.var=ncap_var_var_mlt(yyvsp[-2].var,yyvsp[0].var); 
  nco_var_free(yyvsp[-2].var); nco_var_free(yyvsp[0].var); 
;
    break;}
case 47:
#line 486 "../src/nco/ncap_yacc.y"
{
  yyval.var=ncap_var_scv_mlt(yyvsp[-2].var,yyvsp[0].scv);
  nco_var_free(yyvsp[-2].var);
;
    break;}
case 48:
#line 490 "../src/nco/ncap_yacc.y"
{
  yyval.var=ncap_var_scv_mod(yyvsp[-2].var,yyvsp[0].scv);
  nco_var_free(yyvsp[-2].var);
;
    break;}
case 49:
#line 494 "../src/nco/ncap_yacc.y"
{
  yyval.var=ncap_var_scv_mlt(yyvsp[0].var,yyvsp[-2].scv);
  nco_var_free(yyvsp[0].var);
;
    break;}
case 50:
#line 498 "../src/nco/ncap_yacc.y"
{
  yyval.var=ncap_var_var_dvd(yyvsp[0].var,yyvsp[-2].var); /* NB: Ordering is important */
  nco_var_free(yyvsp[-2].var); nco_var_free(yyvsp[0].var); 
;
    break;}
case 51:
#line 502 "../src/nco/ncap_yacc.y"
{
  yyval.var=ncap_var_scv_dvd(yyvsp[-2].var,yyvsp[0].scv);
  nco_var_free(yyvsp[-2].var);
;
    break;}
case 52:
#line 506 "../src/nco/ncap_yacc.y"
{
  yyval.var=ncap_var_scv_pwr(yyvsp[-2].var,yyvsp[0].scv);
  nco_var_free(yyvsp[-2].var);
;
    break;}
case 53:
#line 510 "../src/nco/ncap_yacc.y"
{
  yyval.var=ncap_var_scv_pwr(yyvsp[-3].var,yyvsp[-1].scv);
  nco_var_free(yyvsp[-3].var);
;
    break;}
case 54:
#line 514 "../src/nco/ncap_yacc.y"
{ 
  scv_sct minus;
  minus.val.b=-1;
  minus.type=NC_BYTE;
  yyval.var=ncap_var_scv_mlt(yyvsp[0].var,minus);
  nco_var_free(yyvsp[0].var);      
;
    break;}
case 55:
#line 521 "../src/nco/ncap_yacc.y"
{
  yyval.var=yyvsp[0].var;
;
    break;}
case 56:
#line 524 "../src/nco/ncap_yacc.y"
{
  yyval.var=ncap_var_abs(yyvsp[-1].var);
  nco_var_free(yyvsp[-1].var);
;
    break;}
case 57:
#line 528 "../src/nco/ncap_yacc.y"
{
  yyval.var=ncap_var_abs(yyvsp[-1].var);
  /* fxm Finish avg,min,max,ttl */
  /* $$=nco_var_avg($3,dim,dmn_nbr,nco_op_typ); */
  /* if(prs_arg->nco_op_typ == nco_op_avg) (void)nco_var_dvd(var_prc_out[idx]->type,var_prc_out[idx]->sz,var_prc_out[idx]->has_mss_val,var_prc_out[idx]->mss_val,wgt_avg->val,var_prc_out[idx]->val); */
  (void)fprintf(stderr,"%s: WARNING RDC tokens not implemented yet\n",prg_nm_get());
  /* $3 is freed in nco_var_avg() */
;
    break;}
case 58:
#line 536 "../src/nco/ncap_yacc.y"
{
  /* Packing variable does not create duplicate so DO NOT free $3 */
  yyval.var=nco_var_pck(yyvsp[-1].var,NC_SHORT,False);
;
    break;}
case 59:
#line 540 "../src/nco/ncap_yacc.y"
{
  /* Unpacking variable does not create duplicate so DO NOT free $3 */
  yyval.var=nco_var_upk(yyvsp[-1].var);
;
    break;}
case 60:
#line 544 "../src/nco/ncap_yacc.y"
{
  yyval.var=ncap_var_fnc(yyvsp[-1].var,yyvsp[-3].sym);
  nco_var_free(yyvsp[-1].var);
;
    break;}
case 61:
#line 548 "../src/nco/ncap_yacc.y"
{
  yyval.var=yyvsp[-1].var;
;
    break;}
case 62:
#line 551 "../src/nco/ncap_yacc.y"
{
  yyval.var=nco_var_cnf_typ(yyvsp[-3].cnv_type,yyvsp[-1].var);
;
    break;}
case 63:
#line 556 "../src/nco/ncap_yacc.y"
{ /* Terminal symbol action */
  /* fxm: Allow commands like a=M_PI*rds^2; to work */
;
    break;}
case 64:
#line 559 "../src/nco/ncap_yacc.y"
{ /* Terminal symbol action */
  yyval.var=ncap_var_init(yyvsp[0].var_nm_RHS,(prs_sct *)prs_arg);

  if((((prs_sct *)prs_arg)->var_LHS) != NULL){
    /* User intends LHS to cast RHS to same dimensionality
       Stretch newly initialized variable to size of LHS template */
    /*    (void)ncap_var_cnf_dmn(&$$,&(((prs_sct *)prs_arg)->var_LHS));*/
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
#line 578 "../src/nco/ncap_yacc.y"

/* Begin User Subroutines section */

aed_sct *  /* O [idx] Location of attribute in list */
ncap_aed_lookup /* [fnc] Find location of existing attribute or add new attribute */
(const char * const var_nm, /* I [sng] Variable name */
 const char * const att_nm, /* I [sng] Attribute name */
 prs_sct *  prs_arg,   /* contains attribute list */       
 const bool update) /* I [flg] Delete existing value or add new attribute to list */
{
  int idx;
  int size;
  aed_sct *ptr_aed;

  size = *(prs_arg->nbr_att);

  for(idx = 0 ; idx < size ; idx++){
    ptr_aed = (*(prs_arg->att_lst))[idx];
    
    if(strcmp(ptr_aed->att_nm,att_nm) || strcmp(ptr_aed->var_nm,var_nm)) 
       continue; 
    
      if(update) ptr_aed->val.vp=nco_free(ptr_aed->val.vp);   
      /* Return pointer to list element */
      return ptr_aed;

  } /* end for */
   
  if(!update) return (aed_sct *)NULL;

  *(prs_arg->att_lst) = (aed_sct **) nco_realloc(*(prs_arg->att_lst), (size+1)*sizeof(aed_sct*));
  ++*(prs_arg->nbr_att);
  (*(prs_arg->att_lst))[size] = (aed_sct *)nco_malloc(sizeof(aed_sct));
  (*(prs_arg->att_lst))[size]->var_nm =strdup(var_nm);
  (*(prs_arg->att_lst))[size]->att_nm =strdup(att_nm);
  

  return (*(prs_arg->att_lst))[size];

}

int /* [rcd] Return code */
yyerror /* [fnc] Print error/warning/info messages generated by parser */
(char *err_sng) /* [sng] Message to print */
{
  /* Purpose: Print error/warning/info messages generated by parser
     Use eprokoke_skip to skip error message after sending error message from yylex()
     Stop provoked error message from yyparse being printed */

  static bool eprovoke_skip;
  
  /* if(eprovoke_skip){eprovoke_skip=False ; return 0;} */
  if(dbg_lvl_get() > 0){
    (void)fprintf(stderr,"%s: %s line %zu",prg_nm_get(),fl_spt_glb[ncl_dpt_crr],ln_nbr_crr[ncl_dpt_crr]);
    if(dbg_lvl_get() > 1) (void)fprintf(stderr," %s",err_sng);
    (void)fprintf(stderr,"\n");
    (void)fflush(stderr);
  } /* endif dbg */
 
  if(err_sng[0] == '#') eprovoke_skip=True;
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
 int arg_nbr, /* I [nbr] Number of arguments */
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
} /* end freeNode() */
