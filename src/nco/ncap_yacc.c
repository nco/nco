/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.0.4"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1


/* Substitute the variable and function names.  */
#define yyparse         nco_yyparse
#define yylex           nco_yylex
#define yyerror         nco_yyerror
#define yydebug         nco_yydebug
#define yynerrs         nco_yynerrs


/* Copy the first part of user declarations.  */
#line 1 "ncap_yacc.y" /* yacc.c:339  */
 /* $Header$ -*-C-*- */
  
/* Begin C declarations section */
  
/* Purpose: Grammar parser for ncap */
  
/* Copyright (C) 1995--2018 Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   GNU General Public License (GPL) Version 3.
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
  /* 20161204 GNU since gcc 4.7.3 string.h provides strcasestr() as non-standard extension iff _GNU_SOURCE is defined
     20161205 stpcpy() prototype provided since glibc 2.10 with _POSIX_C_SOURCE >= 200809L and before glibc 2.10 with _GNU_SOURCE
     Abandoned _GNU_SOURCE because unable to get ncap to find stpcpy() prototype */
#if 0
#ifdef __GNUC__  
# define _GNU_SOURCE
#endif /* __GNUC__ */
#endif
#include <string.h> /* strcmp() */
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

/* Turn-on parser debugging option (Bison manual p. 85) */
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

#line 168 "ncap_yacc.c" /* yacc.c:339  */

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* In a future release of Bison, this section will be replaced
   by #include "y.tab.h".  */
#ifndef YY_NCO_YY_Y_TAB_H_INCLUDED
# define YY_NCO_YY_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int nco_yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    OUT_ATT = 258,
    COMPARISON = 259,
    LHS_SBS = 260,
    SCV = 261,
    SNG = 262,
    FUNCTION = 263,
    OUT_VAR = 264,
    VAR = 265,
    CNV_TYPE = 266,
    ABS = 267,
    ATOSTR = 268,
    EPROVOKE = 269,
    IGNORE = 270,
    NAMED_CONSTANT = 271,
    PACK = 272,
    POWER = 273,
    RDC = 274,
    UNPACK = 275,
    IF = 276,
    PRINT = 277,
    AND = 278,
    NOT = 279,
    OR = 280,
    UMINUS = 281,
    LOWER_THAN_ELSE = 282,
    ELSE = 283
  };
#endif
/* Tokens.  */
#define OUT_ATT 258
#define COMPARISON 259
#define LHS_SBS 260
#define SCV 261
#define SNG 262
#define FUNCTION 263
#define OUT_VAR 264
#define VAR 265
#define CNV_TYPE 266
#define ABS 267
#define ATOSTR 268
#define EPROVOKE 269
#define IGNORE 270
#define NAMED_CONSTANT 271
#define PACK 272
#define POWER 273
#define RDC 274
#define UNPACK 275
#define IF 276
#define PRINT 277
#define AND 278
#define NOT 279
#define OR 280
#define UMINUS 281
#define LOWER_THAN_ELSE 282
#define ELSE 283

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 114 "ncap_yacc.y" /* yacc.c:355  */

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

#line 277 "ncap_yacc.c" /* yacc.c:355  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif



int nco_yyparse (prs_sct *prs_arg);

#endif /* !YY_NCO_YY_Y_TAB_H_INCLUDED  */

/* Copy the second part of user declarations.  */

#line 293 "ncap_yacc.c" /* yacc.c:358  */

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

#if !defined _Noreturn \
     && (!defined __STDC_VERSION__ || __STDC_VERSION__ < 201112)
# if defined _MSC_VER && 1200 <= _MSC_VER
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn YY_ATTRIBUTE ((__noreturn__))
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif


#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  13
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   519

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  40
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  12
/* YYNRULES -- Number of rules.  */
#define YYNRULES  74
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  190

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   283

#define YYTRANSLATE(YYX)                                                \
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,    30,     2,     2,
      36,    37,    28,    26,    39,    27,     2,    29,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,    35,
       2,    38,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,    31,     2,     2,     2,     2,     2,
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
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    32,    33,    34
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   172,   172,   176,   177,   178,   183,   184,   185,   194,
     198,   202,   198,   208,   244,   263,   290,   302,   342,   368,
     372,   376,   380,   384,   389,   393,   396,   409,   422,   425,
     435,   439,   440,   443,   446,   450,   459,   481,   503,   507,
     509,   511,   513,   519,   522,   525,   529,   532,   535,   545,
     548,   551,   555,   561,   564,   568,   571,   574,   578,   581,
     584,   588,   592,   595,   599,   605,   608,   611,   619,   632,
     642,   645,   648,   651,   654
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "OUT_ATT", "COMPARISON", "LHS_SBS",
  "SCV", "SNG", "FUNCTION", "OUT_VAR", "VAR", "CNV_TYPE", "ABS", "ATOSTR",
  "EPROVOKE", "IGNORE", "NAMED_CONSTANT", "PACK", "POWER", "RDC", "UNPACK",
  "IF", "PRINT", "AND", "NOT", "OR", "'+'", "'-'", "'*'", "'/'", "'%'",
  "'^'", "UMINUS", "LOWER_THAN_ELSE", "ELSE", "';'", "'('", "')'", "'='",
  "','", "$accept", "program", "stmt_lst", "stmt", "$@1", "$@2", "scv_xpr",
  "out_var_xpr", "out_att_xpr", "sng_xpr", "bln_xpr", "var_xpr", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,    43,    45,    42,    47,
      37,    94,   281,   282,   283,    59,    40,    41,    61,    44
};
# endif

#define YYPACT_NINF -35

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-35)))

#define YYTABLE_NINF -3

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      64,   -34,   -35,   -35,   -27,   -35,    14,     2,   -10,   -16,
      -6,   -35,   140,   -35,    10,   -35,    11,   -35,    70,    70,
     -35,    18,   -35,    27,    30,   -35,    32,    33,    34,    36,
     140,   140,   140,     0,    37,    13,   -35,   -35,   -35,    39,
      74,    53,   488,    74,    53,   488,   140,   140,   140,   140,
     140,   140,   140,   -35,   -35,   -35,   -35,   223,   235,   140,
     140,   140,   140,   140,   140,   140,    12,   140,   140,   140,
     140,   140,   140,   140,   191,     3,   247,   259,   271,   284,
     296,   308,    74,   320,   184,   202,   332,   344,   -35,   -35,
      74,   488,    63,    79,    63,    79,    67,    89,    67,    89,
      67,    89,    67,    89,    50,    74,   488,    63,    79,    63,
      79,    67,    89,    67,    89,    67,    89,    67,    89,    59,
      85,    86,    90,   191,   191,   191,    88,   -35,   -35,   -35,
     -35,   -35,   -35,   -35,   -35,   140,   140,   -35,   -35,    12,
     191,   191,   191,   191,   -35,   -35,   356,   191,   191,   191,
     191,   191,   191,   -35,     3,   368,   380,   392,   404,   -35,
     416,   428,   440,   208,   149,   149,   101,   101,   101,   101,
     -13,   -35,   -35,   -35,   -35,   111,   191,   -35,    98,   452,
     191,   464,   110,   -35,   125,   113,   140,   476,   126,   -35
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,    34,    33,     0,     3,     0,     0,     0,     0,
       0,     8,     0,     1,     0,     4,     0,     7,     0,     0,
      32,     0,    74,     0,     0,    73,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     6,     5,    38,     0,
      17,    18,    16,    13,    14,    15,     0,     0,     0,     0,
       0,     0,     0,    25,    65,    24,    64,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    31,    71,
      40,    42,    19,    45,    20,    48,    21,    51,    22,    54,
      23,    57,    26,    60,     9,    41,    39,    44,    43,    47,
      46,    50,    49,    53,    52,    56,    55,    59,    58,     0,
       0,     0,     0,     0,     0,     0,     0,    35,    29,    70,
      30,    72,    28,    66,    68,     0,     0,    67,    69,     0,
       0,     0,     0,     0,    25,    24,     0,     0,     0,     0,
       0,     0,     0,    36,     0,     0,     0,     0,     0,    10,
       0,     0,     0,     0,    19,    20,    21,    22,    23,    26,
       0,    27,    63,    62,    61,     0,     0,    37,     0,     0,
       0,     0,     0,    11,     0,     0,     0,     0,     0,    12
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -35,   -35,   -35,     5,   -35,   -35,   -12,   -35,   -35,   -11,
     -35,   123
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     6,     7,     8,   175,   184,    82,     9,    10,    41,
      34,    35
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      33,    11,    -2,    14,    59,     2,    40,    43,    44,    12,
      38,     3,    16,    75,    13,     2,    39,    67,    53,    55,
      57,     3,    18,     4,   177,    17,    60,    61,    62,    63,
      64,    65,    19,     4,    76,    78,    80,    15,    84,    68,
      69,    70,    71,    72,    73,    36,    37,    90,    92,    94,
      96,    98,   100,   102,    46,   105,   107,   109,   111,   113,
     115,   117,   126,    47,   127,     1,    48,     2,    49,    50,
      51,   104,    52,     3,    66,    74,    20,    38,    21,    75,
      22,    23,    24,    39,   139,     4,    25,    26,    27,    28,
      29,    62,    63,    64,    65,   140,    30,    31,    65,     5,
      60,    61,    62,    63,    64,    65,    32,    70,    71,    72,
      73,   144,   145,   146,   147,   148,   149,   150,   151,   152,
      73,   141,   142,   155,   157,   153,   143,   154,   160,   161,
     162,   163,   152,   178,   180,   164,   165,   166,   167,   168,
     169,    42,    45,   170,   159,   183,    20,   185,    21,   186,
      22,    23,    24,    54,    56,    58,    25,    26,    27,    28,
      29,   189,     0,     0,   179,     0,    30,    31,   181,    77,
      79,    81,    83,    85,    86,    87,    32,   149,   150,   151,
     152,     0,    91,    93,    95,    97,    99,   101,   103,     0,
     106,   108,   110,   112,   114,   116,   118,    20,     0,   119,
       0,     0,   120,   121,     0,     0,     0,     0,     0,   122,
      60,    61,    62,    63,    64,    65,     0,   123,   124,     0,
       0,     0,     0,   135,     0,     0,     0,   125,    68,    69,
      70,    71,    72,    73,   147,   148,   149,   150,   151,   152,
       0,   136,     0,     0,     0,     0,     0,   176,     0,    60,
      61,    62,    63,    64,    65,     0,     0,     0,   156,   158,
      88,    68,    69,    70,    71,    72,    73,     0,     0,     0,
       0,     0,    89,    60,    61,    62,    63,    64,    65,     0,
       0,     0,     0,     0,   128,    68,    69,    70,    71,    72,
      73,     0,     0,     0,     0,     0,   129,    60,    61,    62,
      63,    64,    65,     0,     0,     0,     0,     0,   130,   187,
      68,    69,    70,    71,    72,    73,     0,     0,     0,     0,
       0,   131,    60,    61,    62,    63,    64,    65,     0,     0,
       0,     0,     0,   132,    68,    69,    70,    71,    72,    73,
       0,     0,     0,     0,     0,   133,    68,    69,    70,    71,
      72,    73,     0,     0,     0,     0,     0,   134,    68,    69,
      70,    71,    72,    73,     0,     0,     0,     0,     0,   137,
      68,    69,    70,    71,    72,    73,     0,     0,     0,     0,
       0,   138,   147,   148,   149,   150,   151,   152,     0,     0,
       0,     0,     0,    88,    60,    61,    62,    63,    64,    65,
       0,     0,     0,     0,     0,   171,    68,    69,    70,    71,
      72,    73,     0,     0,     0,     0,     0,   172,    60,    61,
      62,    63,    64,    65,     0,     0,     0,     0,     0,   173,
      68,    69,    70,    71,    72,    73,     0,     0,     0,     0,
       0,   174,   147,   148,   149,   150,   151,   152,     0,     0,
       0,     0,     0,   128,   147,   148,   149,   150,   151,   152,
       0,     0,     0,     0,     0,   130,   147,   148,   149,   150,
     151,   152,     0,     0,     0,     0,     0,   132,   147,   148,
     149,   150,   151,   152,     0,     0,     0,     0,     0,   171,
     147,   148,   149,   150,   151,   152,     0,     0,     0,     0,
       0,   182,    68,    69,    70,    71,    72,    73,     0,     0,
       0,     0,     0,   188,    68,    69,    70,    71,    72,    73
};

static const yytype_int16 yycheck[] =
{
      12,    35,     0,     1,     4,     3,    18,    19,    19,    36,
       7,     9,     7,    26,     0,     3,    13,     4,    30,    31,
      32,     9,    38,    21,    37,    35,    26,    27,    28,    29,
      30,    31,    38,    21,    46,    47,    48,    35,    50,    26,
      27,    28,    29,    30,    31,    35,    35,    59,    60,    61,
      62,    63,    64,    65,    36,    67,    68,    69,    70,    71,
      72,    73,    74,    36,    75,     1,    36,     3,    36,    36,
      36,    66,    36,     9,    37,    36,     6,     7,     8,    26,
      10,    11,    12,    13,    34,    21,    16,    17,    18,    19,
      20,    28,    29,    30,    31,    36,    26,    27,    31,    35,
      26,    27,    28,    29,    30,    31,    36,    28,    29,    30,
      31,   123,   124,   125,    26,    27,    28,    29,    30,    31,
      31,    36,    36,   135,   136,    37,    36,    39,   140,   141,
     142,   143,    31,    22,    36,   147,   148,   149,   150,   151,
     152,    18,    19,   154,   139,    35,     6,    22,     8,    36,
      10,    11,    12,    30,    31,    32,    16,    17,    18,    19,
      20,    35,    -1,    -1,   176,    -1,    26,    27,   180,    46,
      47,    48,    49,    50,    51,    52,    36,    28,    29,    30,
      31,    -1,    59,    60,    61,    62,    63,    64,    65,    -1,
      67,    68,    69,    70,    71,    72,    73,     6,    -1,     8,
      -1,    -1,    11,    12,    -1,    -1,    -1,    -1,    -1,    18,
      26,    27,    28,    29,    30,    31,    -1,    26,    27,    -1,
      -1,    -1,    -1,    39,    -1,    -1,    -1,    36,    26,    27,
      28,    29,    30,    31,    26,    27,    28,    29,    30,    31,
      -1,    39,    -1,    -1,    -1,    -1,    -1,    39,    -1,    26,
      27,    28,    29,    30,    31,    -1,    -1,    -1,   135,   136,
      37,    26,    27,    28,    29,    30,    31,    -1,    -1,    -1,
      -1,    -1,    37,    26,    27,    28,    29,    30,    31,    -1,
      -1,    -1,    -1,    -1,    37,    26,    27,    28,    29,    30,
      31,    -1,    -1,    -1,    -1,    -1,    37,    26,    27,    28,
      29,    30,    31,    -1,    -1,    -1,    -1,    -1,    37,   186,
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
      -1,    37,    26,    27,    28,    29,    30,    31,    -1,    -1,
      -1,    -1,    -1,    37,    26,    27,    28,    29,    30,    31
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     1,     3,     9,    21,    35,    41,    42,    43,    47,
      48,    35,    36,     0,     1,    35,    43,    35,    38,    38,
       6,     8,    10,    11,    12,    16,    17,    18,    19,    20,
      26,    27,    36,    46,    50,    51,    35,    35,     7,    13,
      46,    49,    51,    46,    49,    51,    36,    36,    36,    36,
      36,    36,    36,    46,    51,    46,    51,    46,    51,     4,
      26,    27,    28,    29,    30,    31,    37,     4,    26,    27,
      28,    29,    30,    31,    36,    26,    46,    51,    46,    51,
      46,    51,    46,    51,    46,    51,    51,    51,    37,    37,
      46,    51,    46,    51,    46,    51,    46,    51,    46,    51,
      46,    51,    46,    51,    43,    46,    51,    46,    51,    46,
      51,    46,    51,    46,    51,    46,    51,    46,    51,     8,
      11,    12,    18,    26,    27,    36,    46,    49,    37,    37,
      37,    37,    37,    37,    37,    39,    39,    37,    37,    34,
      36,    36,    36,    36,    46,    46,    46,    26,    27,    28,
      29,    30,    31,    37,    39,    46,    51,    46,    51,    43,
      46,    46,    46,    46,    46,    46,    46,    46,    46,    46,
      49,    37,    37,    37,    37,    44,    39,    37,    22,    46,
      36,    46,    37,    35,    45,    22,    36,    51,    37,    35
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    40,    41,    42,    42,    42,    42,    42,    42,    43,
      44,    45,    43,    43,    43,    43,    43,    43,    43,    46,
      46,    46,    46,    46,    46,    46,    46,    46,    46,    46,
      46,    46,    46,    47,    48,    49,    49,    49,    49,    50,
      50,    50,    50,    51,    51,    51,    51,    51,    51,    51,
      51,    51,    51,    51,    51,    51,    51,    51,    51,    51,
      51,    51,    51,    51,    51,    51,    51,    51,    51,    51,
      51,    51,    51,    51,    51
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     1,     2,     3,     3,     2,     2,     5,
       0,     0,    19,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     2,     2,     3,     6,     4,     4,
       4,     3,     1,     1,     1,     3,     4,     6,     1,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     6,     6,     6,     2,     2,     4,     4,     4,     4,
       4,     3,     4,     1,     1
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
do                                                              \
  if (yychar == YYEMPTY)                                        \
    {                                                           \
      yychar = (Token);                                         \
      yylval = (Value);                                         \
      YYPOPSTACK (yylen);                                       \
      yystate = *yyssp;                                         \
      goto yybackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      yyerror (prs_arg, YY_("syntax error: cannot back up")); \
      YYERROR;                                                  \
    }                                                           \
while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value, prs_arg); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*----------------------------------------.
| Print this symbol's value on YYOUTPUT.  |
`----------------------------------------*/

static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, prs_sct *prs_arg)
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  YYUSE (prs_arg);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, prs_sct *prs_arg)
{
  YYFPRINTF (yyoutput, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep, prs_arg);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, int yyrule, prs_sct *prs_arg)
{
  unsigned long int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &(yyvsp[(yyi + 1) - (yynrhs)])
                                              , prs_arg);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule, prs_arg); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
yystrlen (const char *yystr)
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            /* Fall through.  */
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, prs_sct *prs_arg)
{
  YYUSE (yyvaluep);
  YYUSE (prs_arg);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/*----------.
| yyparse.  |
`----------*/

int
yyparse (prs_sct *prs_arg)
{
/* The lookahead symbol.  */
int yychar;


/* The semantic value of the lookahead symbol.  */
/* Default value used for initialization, for pacifying older GCCs
   or non-GCC compilers.  */
YY_INITIAL_VALUE (static YYSTYPE yyval_default;)
YYSTYPE yylval YY_INITIAL_VALUE (= yyval_default);

    /* Number of syntax errors so far.  */
    int yynerrs;

    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        YYSTYPE *yyvs1 = yyvs;
        yytype_int16 *yyss1 = yyss;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * sizeof (*yyssp),
                    &yyvs1, yysize * sizeof (*yyvsp),
                    &yystacksize);

        yyss = yyss1;
        yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yytype_int16 *yyss1 = yyss;
        union yyalloc *yyptr =
          (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
                  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex (&yylval, prs_arg);
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 3:
#line 176 "ncap_yacc.y" /* yacc.c:1646  */
    { ;}
#line 1554 "ncap_yacc.c" /* yacc.c:1646  */
    break;

  case 4:
#line 177 "ncap_yacc.y" /* yacc.c:1646  */
    { ;}
#line 1560 "ncap_yacc.c" /* yacc.c:1646  */
    break;

  case 5:
#line 178 "ncap_yacc.y" /* yacc.c:1646  */
    {
  /* Purpose: Actions to be performed at end-of-statement go here */
  /* Clean up from and exit LHS_cst mode */
  (void)nco_var_free_wrp(&((prs_sct *)prs_arg)->var_LHS);
}
#line 1570 "ncap_yacc.c" /* yacc.c:1646  */
    break;

  case 6:
#line 183 "ncap_yacc.y" /* yacc.c:1646  */
    {(void)nco_var_free_wrp(&((prs_sct *)prs_arg)->var_LHS);}
#line 1576 "ncap_yacc.c" /* yacc.c:1646  */
    break;

  case 7:
#line 184 "ncap_yacc.y" /* yacc.c:1646  */
    {(void)nco_var_free_wrp(&((prs_sct *)prs_arg)->var_LHS);}
#line 1582 "ncap_yacc.c" /* yacc.c:1646  */
    break;

  case 8:
#line 185 "ncap_yacc.y" /* yacc.c:1646  */
    {(void)nco_var_free_wrp(&((prs_sct *)prs_arg)->var_LHS);}
#line 1588 "ncap_yacc.c" /* yacc.c:1646  */
    break;

  case 9:
#line 194 "ncap_yacc.y" /* yacc.c:1646  */
    {
  /* LMB92 p. 234 */
  ;
}
#line 1597 "ncap_yacc.c" /* yacc.c:1646  */
    break;

  case 10:
#line 198 "ncap_yacc.y" /* yacc.c:1646  */
    {
  /* LMB92 p. 234 */
  ;
}
#line 1606 "ncap_yacc.c" /* yacc.c:1646  */
    break;

  case 11:
#line 202 "ncap_yacc.y" /* yacc.c:1646  */
    {
  ;
}
#line 1614 "ncap_yacc.c" /* yacc.c:1646  */
    break;

  case 12:
#line 205 "ncap_yacc.y" /* yacc.c:1646  */
    {
  ;
}
#line 1622 "ncap_yacc.c" /* yacc.c:1646  */
    break;

  case 13:
#line 208 "ncap_yacc.y" /* yacc.c:1646  */
    { 
  aed_sct *ptr_aed;
  
  ptr_aed=ncap_aed_lookup((yyvsp[-2].aed).var_nm,(yyvsp[-2].aed).att_nm,((prs_sct *)prs_arg),True);
  
  ptr_aed->val=ncap_scv_2_ptr_unn((yyvsp[0].scv));
  ptr_aed->type=(yyvsp[0].scv).type;
  ptr_aed->sz=1L;
  (void)cast_nctype_void(ptr_aed->type,&ptr_aed->val);    
  if(nco_dbg_lvl_get() >= nco_dbg_std) (void)sprintf(ncap_err_sng,"Saving attribute %s@%s to %s",(yyvsp[-2].aed).var_nm,(yyvsp[-2].aed).att_nm,((prs_sct *)prs_arg)->fl_out);
  (void)nco_yyerror(prs_arg,ncap_err_sng);
  
  if(nco_dbg_lvl_get() >= nco_dbg_fl){
    (void)fprintf(stderr,"Saving in array attribute %s@%s=",(yyvsp[-2].aed).var_nm,(yyvsp[-2].aed).att_nm);
    switch((yyvsp[0].scv).type){
  /* NB: Format depends on opaque type of nco_int
     Until 200911, nco_int was C-type long, and now nco_int is C-type int
     case NC_INT: (void)fprintf(stderr,"%ld\n",$3.val.i); break; */
    case NC_FLOAT: (void)fprintf(stderr,"%G\n",(yyvsp[0].scv).val.f); break;
    case NC_DOUBLE: (void)fprintf(stderr,"%.5G\n",(yyvsp[0].scv).val.d);break;
    case NC_INT: (void)fprintf(stderr,"%d\n",(yyvsp[0].scv).val.i); break;
    case NC_SHORT: (void)fprintf(stderr,"%hi\n",(yyvsp[0].scv).val.s); break;
    case NC_BYTE: (void)fprintf(stderr,"%hhi\n",(yyvsp[0].scv).val.b); break;
    case NC_UBYTE: (void)fprintf(stderr,"%hhu\n",(yyvsp[0].scv).val.ub); break;
    case NC_USHORT: (void)fprintf(stderr,"%hu\n",(yyvsp[0].scv).val.us); break;
    case NC_UINT: (void)fprintf(stderr,"%u\n",(yyvsp[0].scv).val.ui); break;
    case NC_INT64: (void)fprintf(stderr,"%lld\n",(yyvsp[0].scv).val.i64); break;
    case NC_UINT64: (void)fprintf(stderr,"%llu\n",(yyvsp[0].scv).val.ui64); break;
    case NC_CHAR: (void)fprintf(stderr,"%c\n",(yyvsp[0].scv).val.c); break;
    case NC_STRING: (void)fprintf(stderr,"%s\n",(yyvsp[0].scv).val.sng); break;
    default: break;
    } /* end switch */
  } /* end if */
  (yyvsp[-2].aed).var_nm=(char *)nco_free((yyvsp[-2].aed).var_nm);
  (yyvsp[-2].aed).att_nm=(char *)nco_free((yyvsp[-2].aed).att_nm);
}
#line 1663 "ncap_yacc.c" /* yacc.c:1646  */
    break;

  case 14:
#line 245 "ncap_yacc.y" /* yacc.c:1646  */
    {
  aed_sct *ptr_aed;
  size_t sng_lng;
  
  sng_lng=strlen((yyvsp[0].sng));
  ptr_aed=ncap_aed_lookup((yyvsp[-2].aed).var_nm,(yyvsp[-2].aed).att_nm,((prs_sct *)prs_arg),True);
  ptr_aed->type=NC_CHAR;
  ptr_aed->sz=(long int)((sng_lng+1)*nco_typ_lng(NC_CHAR));
  ptr_aed->val.cp=(nco_char *)nco_malloc((sng_lng+1)*nco_typ_lng(NC_CHAR));
  strcpy((char *)(ptr_aed->val.cp),(yyvsp[0].sng));
  (void)cast_nctype_void((nc_type)NC_CHAR,&ptr_aed->val);    
  
  if(nco_dbg_lvl_get() >= nco_dbg_std) (void)sprintf(ncap_err_sng,"Saving attribute %s@%s=%s",(yyvsp[-2].aed).var_nm,(yyvsp[-2].aed).att_nm,(yyvsp[0].sng));
  (void)nco_yyerror(prs_arg,ncap_err_sng);
  (yyvsp[-2].aed).var_nm=(char *)nco_free((yyvsp[-2].aed).var_nm);
  (yyvsp[-2].aed).att_nm=(char *)nco_free((yyvsp[-2].aed).att_nm);
  (yyvsp[0].sng)=(char *)nco_free((yyvsp[0].sng));
}
#line 1686 "ncap_yacc.c" /* yacc.c:1646  */
    break;

  case 15:
#line 264 "ncap_yacc.y" /* yacc.c:1646  */
    { 
  /* Storing 0-dimensional variables in attribute is OK */ 
  aed_sct *ptr_aed;
  
  if((yyvsp[0].var)->nbr_dim < 2){
    ptr_aed=ncap_aed_lookup((yyvsp[-2].aed).var_nm,(yyvsp[-2].aed).att_nm,((prs_sct *)prs_arg),True);
    ptr_aed->sz=(yyvsp[0].var)->sz;
    ptr_aed->type=(yyvsp[0].var)->type;
    /* if inital scan then fill attribute with zeros */
    if( ((prs_sct*)prs_arg)->ntl_scn) {
    ptr_aed->val.vp=(void*)nco_calloc( ptr_aed->sz,nco_typ_lng(ptr_aed->type));
    } else {
    ptr_aed->val.vp=(void*)nco_malloc((ptr_aed->sz)*nco_typ_lng(ptr_aed->type));
    (void)nco_var_copy(ptr_aed->type,ptr_aed->sz,(yyvsp[0].var)->val,ptr_aed->val);
    }
    /* cast_nctype_void($3->type,&ptr_aed->val); */
    if(nco_dbg_lvl_get() >= nco_dbg_std) (void)sprintf(ncap_err_sng,"Saving attribute %s@%s %d dimensional variable",(yyvsp[-2].aed).var_nm,(yyvsp[-2].aed).att_nm,(yyvsp[0].var)->nbr_dim);
    (void)yyerror(prs_arg,ncap_err_sng); 
  }else{
    (void)sprintf(ncap_err_sng,"Warning: Cannot store in attribute %s@%s a variable with dimension %d",(yyvsp[-2].aed).var_nm,(yyvsp[-2].aed).att_nm,(yyvsp[0].var)->nbr_dim);
    (void)yyerror(prs_arg,ncap_err_sng);
  } /* endif */
  (yyvsp[-2].aed).var_nm=(char *)nco_free((yyvsp[-2].aed).var_nm);
  (yyvsp[-2].aed).att_nm=(char *)nco_free((yyvsp[-2].aed).att_nm);
  (void)nco_var_free((yyvsp[0].var)); 
}
#line 1717 "ncap_yacc.c" /* yacc.c:1646  */
    break;

  case 16:
#line 291 "ncap_yacc.y" /* yacc.c:1646  */
    {
  ((yyvsp[0].var)->nm)=(char*)nco_free((yyvsp[0].var)->nm);
  (yyvsp[0].var)->nm=strdup((yyvsp[-2].var_nm_LHS));
  (void)ncap_var_write((yyvsp[0].var),(prs_sct *)prs_arg);
  
  /* Print mess only for defined variables */
  if(nco_dbg_lvl_get() >= nco_dbg_std && !(yyvsp[0].var)->undefined){(void)sprintf(ncap_err_sng,"Saving variable %s to %s",(yyvsp[-2].var_nm_LHS),((prs_sct *)prs_arg)->fl_out);
  (void)yyerror(prs_arg,ncap_err_sng);
  } /* endif */
  (yyvsp[-2].var_nm_LHS)=(char *)nco_free((yyvsp[-2].var_nm_LHS));
}
#line 1733 "ncap_yacc.c" /* yacc.c:1646  */
    break;

  case 17:
#line 303 "ncap_yacc.y" /* yacc.c:1646  */
    {
  var_sct *var;
  var_sct *var_tmp;  
  if(nco_dbg_lvl_get() > 5) (void)fprintf(stderr,"%s: DEBUG out_var_xpr = scv_xpr rule for %s\n",nco_prg_nm_get(),(yyvsp[-2].var_nm_LHS));
  
  /* Turn attribute into temporary variable for writing */
  var=(var_sct *)nco_malloc(sizeof(var_sct));
  /* Set defaults */
  (void)var_dfl_set(var); /* [fnc] Set defaults for each member of variable structure */
  /* Overwrite with attribute expression information */
  var->nm=strdup((yyvsp[-2].var_nm_LHS));
  var->nbr_dim=0;
  var->sz=1;
  var->type=(yyvsp[0].scv).type;
  var->val=ncap_scv_2_ptr_unn((yyvsp[0].scv));
  
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

    if(nco_dbg_lvl_get() >= nco_dbg_scl) (void)fprintf(stderr,"%s: Stretching former scv_xpr defining %s with LHS template: Template var->nm %s, var->nbr_dim %d, var->sz %li\n",nco_prg_nm_get(),(yyvsp[-2].var_nm_LHS),((prs_sct *)prs_arg)->var_LHS->nm,((prs_sct *)prs_arg)->var_LHS->nbr_dim,((prs_sct *)prs_arg)->var_LHS->sz);
  } /* endif LHS_cst */

  var->undefined=False;
  (void)ncap_var_write(var,(prs_sct *)prs_arg);
  
  if(nco_dbg_lvl_get() >= nco_dbg_std ) (void)sprintf(ncap_err_sng,"Saving variable %s to %s",(yyvsp[-2].var_nm_LHS),((prs_sct *)prs_arg)->fl_out);
  (void)yyerror(prs_arg,ncap_err_sng);
  
  
  (yyvsp[-2].var_nm_LHS)=(char *)nco_free((yyvsp[-2].var_nm_LHS));
}
#line 1777 "ncap_yacc.c" /* yacc.c:1646  */
    break;

  case 18:
#line 343 "ncap_yacc.y" /* yacc.c:1646  */
    {
  
  var_sct *var;
  
  var=(var_sct *)nco_calloc((size_t)1,sizeof(var_sct));
  var->nm=strdup((yyvsp[-2].var_nm_LHS));
  var->nbr_dim=0;
  var->dmn_id=(int *)NULL;
  var->sz=strlen((yyvsp[0].sng))+1;
  var->val.cp=(nco_char *)strdup((yyvsp[0].sng));
  var->type=NC_CHAR;
  var->undefined=False;
  (void)cast_nctype_void((nc_type)NC_CHAR,&var->val);
  (void)ncap_var_write(var,(prs_sct *)prs_arg);
  
  if(nco_dbg_lvl_get() >= nco_dbg_std) (void)sprintf(ncap_err_sng,"Saving variable %s to %s",(yyvsp[-2].var_nm_LHS),((prs_sct *)prs_arg)->fl_out);
  (void)yyerror(prs_arg,ncap_err_sng);
  
  (yyvsp[-2].var_nm_LHS)=(char *)nco_free((yyvsp[-2].var_nm_LHS));
  (yyvsp[0].sng)=(char *)nco_free((yyvsp[0].sng));
}
#line 1803 "ncap_yacc.c" /* yacc.c:1646  */
    break;

  case 19:
#line 368 "ncap_yacc.y" /* yacc.c:1646  */
    {
  (void)ncap_scv_scv_cnf_typ_hgh_prc(&(yyvsp[-2].scv),&(yyvsp[0].scv));
  (yyval.scv)=ncap_scv_clc((yyvsp[-2].scv),'+',(yyvsp[0].scv));                            
}
#line 1812 "ncap_yacc.c" /* yacc.c:1646  */
    break;

  case 20:
#line 372 "ncap_yacc.y" /* yacc.c:1646  */
    {
  (void)ncap_scv_scv_cnf_typ_hgh_prc(&(yyvsp[-2].scv),&(yyvsp[0].scv)); 
  (yyval.scv)=ncap_scv_clc((yyvsp[-2].scv),'-',(yyvsp[0].scv));
}
#line 1821 "ncap_yacc.c" /* yacc.c:1646  */
    break;

  case 21:
#line 376 "ncap_yacc.y" /* yacc.c:1646  */
    {
  (void)ncap_scv_scv_cnf_typ_hgh_prc(&(yyvsp[-2].scv),&(yyvsp[0].scv));
  (yyval.scv)=ncap_scv_clc((yyvsp[-2].scv),'*',(yyvsp[0].scv));
}
#line 1830 "ncap_yacc.c" /* yacc.c:1646  */
    break;

  case 22:
#line 380 "ncap_yacc.y" /* yacc.c:1646  */
    {
  (void)ncap_scv_scv_cnf_typ_hgh_prc(&(yyvsp[-2].scv),&(yyvsp[0].scv)); 
  (yyval.scv)=ncap_scv_clc((yyvsp[-2].scv),'/',(yyvsp[0].scv));  
}
#line 1839 "ncap_yacc.c" /* yacc.c:1646  */
    break;

  case 23:
#line 384 "ncap_yacc.y" /* yacc.c:1646  */
    {
  (void)ncap_scv_scv_cnf_typ_hgh_prc(&(yyvsp[-2].scv),&(yyvsp[0].scv));
  
  (yyval.scv)=ncap_scv_clc((yyvsp[-2].scv),'%',(yyvsp[0].scv));  
}
#line 1849 "ncap_yacc.c" /* yacc.c:1646  */
    break;

  case 24:
#line 389 "ncap_yacc.y" /* yacc.c:1646  */
    {
  (void)ncap_scv_minus(&(yyvsp[0].scv));
  (yyval.scv)=(yyvsp[0].scv);
}
#line 1858 "ncap_yacc.c" /* yacc.c:1646  */
    break;

  case 25:
#line 393 "ncap_yacc.y" /* yacc.c:1646  */
    {
  (yyval.scv)=(yyvsp[0].scv);
}
#line 1866 "ncap_yacc.c" /* yacc.c:1646  */
    break;

  case 26:
#line 396 "ncap_yacc.y" /* yacc.c:1646  */
    {
  if(nco_rth_prc_rnk((yyvsp[-2].scv).type) <= nco_rth_prc_rnk_float && nco_rth_prc_rnk((yyvsp[0].scv).type) <= nco_rth_prc_rnk_float) {
    (void)nco_scv_cnf_typ((nc_type)NC_FLOAT,&(yyvsp[-2].scv));
    (void)nco_scv_cnf_typ((nc_type)NC_FLOAT,&(yyvsp[0].scv));
    (yyval.scv).val.f=powf((yyvsp[-2].scv).val.f,(yyvsp[0].scv).val.f);
    (yyval.scv).type=NC_FLOAT;
  }else{
    (void)nco_scv_cnf_typ((nc_type)NC_DOUBLE,&(yyvsp[-2].scv));
    (void)nco_scv_cnf_typ((nc_type)NC_DOUBLE,&(yyvsp[0].scv));
    (yyval.scv).val.d=pow((yyvsp[-2].scv).val.d,(yyvsp[0].scv).val.d);
    (yyval.scv).type=NC_DOUBLE; 
  } /* end else */
}
#line 1884 "ncap_yacc.c" /* yacc.c:1646  */
    break;

  case 27:
#line 409 "ncap_yacc.y" /* yacc.c:1646  */
    { /* fxm: ncap52 this is identical to previous clause except for argument numbering, should be functionalized to use common code */
  if(nco_rth_prc_rnk((yyvsp[-3].scv).type) <= nco_rth_prc_rnk_float && nco_rth_prc_rnk((yyvsp[-1].scv).type) <= nco_rth_prc_rnk_float) {
    (void)nco_scv_cnf_typ((nc_type)NC_FLOAT,&(yyvsp[-3].scv));
    (void)nco_scv_cnf_typ((nc_type)NC_FLOAT,&(yyvsp[-1].scv));
    (yyval.scv).val.f=powf((yyvsp[-3].scv).val.f,(yyvsp[-1].scv).val.f);
    (yyval.scv).type=NC_FLOAT;
  }else{ 
    (void)nco_scv_cnf_typ((nc_type)NC_DOUBLE,&(yyvsp[-3].scv));
    (void)nco_scv_cnf_typ((nc_type)NC_DOUBLE,&(yyvsp[-1].scv));
    (yyval.scv).val.d=pow((yyvsp[-3].scv).val.d,(yyvsp[-1].scv).val.d);
    (yyval.scv).type=NC_DOUBLE; 
  } /* end else */
}
#line 1902 "ncap_yacc.c" /* yacc.c:1646  */
    break;

  case 28:
#line 422 "ncap_yacc.y" /* yacc.c:1646  */
    {
  (yyval.scv)=ncap_scv_abs((yyvsp[-1].scv));
}
#line 1910 "ncap_yacc.c" /* yacc.c:1646  */
    break;

  case 29:
#line 425 "ncap_yacc.y" /* yacc.c:1646  */
    {
  if(nco_rth_prc_rnk((yyvsp[-1].scv).type) <= nco_rth_prc_rnk_float) {
    (void)nco_scv_cnf_typ((nc_type)NC_FLOAT,&(yyvsp[-1].scv));
    (yyval.scv).val.f=(*((yyvsp[-3].sym)->fnc_flt))((yyvsp[-1].scv).val.f);
    (yyval.scv).type=NC_FLOAT;
  }else{
    (yyval.scv).val.d=(*((yyvsp[-3].sym)->fnc_dbl))((yyvsp[-1].scv).val.d);
    (yyval.scv).type=NC_DOUBLE;
  } /* end else */
}
#line 1925 "ncap_yacc.c" /* yacc.c:1646  */
    break;

  case 30:
#line 435 "ncap_yacc.y" /* yacc.c:1646  */
    {
  (void)nco_scv_cnf_typ((yyvsp[-3].cnv_type),&(yyvsp[-1].scv));
  (yyval.scv)=(yyvsp[-1].scv);
}
#line 1934 "ncap_yacc.c" /* yacc.c:1646  */
    break;

  case 31:
#line 439 "ncap_yacc.y" /* yacc.c:1646  */
    {(yyval.scv)=(yyvsp[-1].scv);}
#line 1940 "ncap_yacc.c" /* yacc.c:1646  */
    break;

  case 32:
#line 440 "ncap_yacc.y" /* yacc.c:1646  */
    {(yyval.scv)=(yyvsp[0].scv);}
#line 1946 "ncap_yacc.c" /* yacc.c:1646  */
    break;

  case 33:
#line 443 "ncap_yacc.y" /* yacc.c:1646  */
    {(yyval.var_nm_LHS)=(yyvsp[0].var_nm_LHS);}
#line 1952 "ncap_yacc.c" /* yacc.c:1646  */
    break;

  case 34:
#line 446 "ncap_yacc.y" /* yacc.c:1646  */
    {(yyval.aed)=(yyvsp[0].aed);}
#line 1958 "ncap_yacc.c" /* yacc.c:1646  */
    break;

  case 35:
#line 450 "ncap_yacc.y" /* yacc.c:1646  */
    {
  size_t sng_lng;
  sng_lng=strlen((yyvsp[-2].sng))+strlen((yyvsp[0].sng));
  (yyval.sng)=(char*)nco_malloc((sng_lng+1)*sizeof(char));
  strcpy((yyval.sng),(yyvsp[-2].sng));
  strcat((yyval.sng),(yyvsp[0].sng));
  (yyvsp[-2].sng)=(char *)nco_free((yyvsp[-2].sng));
  (yyvsp[0].sng)=(char *)nco_free((yyvsp[0].sng));
}
#line 1972 "ncap_yacc.c" /* yacc.c:1646  */
    break;

  case 36:
#line 459 "ncap_yacc.y" /* yacc.c:1646  */
    {
  char bfr[50];
  switch ((yyvsp[-1].scv).type){
  /* NB: Format depends on opaque type of nco_int
     Until 200911, nco_int was C-type long, and now nco_int is C-type int
     case NC_INT: sprintf(bfr,"%ld",$3.val.i); break; */
  case NC_DOUBLE: sprintf(bfr,"%.10G",(yyvsp[-1].scv).val.d); break;
  case NC_FLOAT: sprintf(bfr,"%G",(yyvsp[-1].scv).val.f); break;
  case NC_INT: sprintf(bfr,"%d",(yyvsp[-1].scv).val.i); break;
  case NC_SHORT: sprintf(bfr,"%hi",(yyvsp[-1].scv).val.s); break;
  case NC_BYTE: sprintf(bfr,"%hhi",(yyvsp[-1].scv).val.b); break;
  case NC_UBYTE: sprintf(bfr,"%hhu",(yyvsp[-1].scv).val.ub); break;
  case NC_USHORT: sprintf(bfr,"%hu",(yyvsp[-1].scv).val.us); break;
  case NC_UINT: sprintf(bfr,"%u",(yyvsp[-1].scv).val.ui); break;
  case NC_INT64: sprintf(bfr,"%lld",(yyvsp[-1].scv).val.i64); break;
  case NC_UINT64: sprintf(bfr,"%llu",(yyvsp[-1].scv).val.ui64); break;
  case NC_CHAR: sprintf(bfr,"%c",(yyvsp[-1].scv).val.c); break;
  case NC_STRING: sprintf(bfr,"%s",(yyvsp[-1].scv).val.sng); break;
  default:  break;
  } /* end switch */
  (yyval.sng)=strdup(bfr);      
}
#line 1999 "ncap_yacc.c" /* yacc.c:1646  */
    break;

  case 37:
#line 481 "ncap_yacc.y" /* yacc.c:1646  */
    {
  char bfr[150];
  /* Format string according to string expression */
  /* User decides which format corresponds to which type */
  switch ((yyvsp[-3].scv).type){
  case NC_DOUBLE: sprintf(bfr,(yyvsp[-1].sng),(yyvsp[-3].scv).val.d); break;
  case NC_FLOAT: sprintf(bfr,(yyvsp[-1].sng),(yyvsp[-3].scv).val.f); break;
  case NC_INT: sprintf(bfr,(yyvsp[-1].sng),(yyvsp[-3].scv).val.i); break;
  case NC_SHORT: sprintf(bfr,(yyvsp[-1].sng),(yyvsp[-3].scv).val.s); break;
  case NC_BYTE: sprintf(bfr,(yyvsp[-1].sng),(yyvsp[-3].scv).val.b); break;
  case NC_UBYTE: sprintf(bfr,(yyvsp[-1].sng),(yyvsp[-3].scv).val.ub); break;
  case NC_USHORT: sprintf(bfr,(yyvsp[-1].sng),(yyvsp[-3].scv).val.us); break;
  case NC_UINT: sprintf(bfr,(yyvsp[-1].sng),(yyvsp[-3].scv).val.ui); break;
  case NC_INT64: sprintf(bfr,(yyvsp[-1].sng),(yyvsp[-3].scv).val.i64); break;
  case NC_UINT64: sprintf(bfr,(yyvsp[-1].sng),(yyvsp[-3].scv).val.ui64); break;
  case NC_CHAR: sprintf(bfr,(yyvsp[-1].sng),(yyvsp[-3].scv).val.c); break;
  case NC_STRING: sprintf(bfr,(yyvsp[-1].sng),(yyvsp[-3].scv).val.sng); break;
  default:  break;
  } /* end switch */
  (yyvsp[-1].sng)=(char *)nco_free((yyvsp[-1].sng));
  (yyval.sng)=strdup(bfr);      
}
#line 2026 "ncap_yacc.c" /* yacc.c:1646  */
    break;

  case 38:
#line 503 "ncap_yacc.y" /* yacc.c:1646  */
    {(yyval.sng)=(yyvsp[0].sng);}
#line 2032 "ncap_yacc.c" /* yacc.c:1646  */
    break;

  case 39:
#line 507 "ncap_yacc.y" /* yacc.c:1646  */
    {
}
#line 2039 "ncap_yacc.c" /* yacc.c:1646  */
    break;

  case 40:
#line 509 "ncap_yacc.y" /* yacc.c:1646  */
    {
}
#line 2046 "ncap_yacc.c" /* yacc.c:1646  */
    break;

  case 41:
#line 511 "ncap_yacc.y" /* yacc.c:1646  */
    {
}
#line 2053 "ncap_yacc.c" /* yacc.c:1646  */
    break;

  case 42:
#line 513 "ncap_yacc.y" /* yacc.c:1646  */
    {
}
#line 2060 "ncap_yacc.c" /* yacc.c:1646  */
    break;

  case 43:
#line 519 "ncap_yacc.y" /* yacc.c:1646  */
    { /* Begin Addition */
  (yyval.var)=ncap_var_var_add((yyvsp[-2].var),(yyvsp[0].var)); 
}
#line 2068 "ncap_yacc.c" /* yacc.c:1646  */
    break;

  case 44:
#line 522 "ncap_yacc.y" /* yacc.c:1646  */
    {
  (yyval.var)=ncap_var_scv_add((yyvsp[-2].var),(yyvsp[0].scv));
}
#line 2076 "ncap_yacc.c" /* yacc.c:1646  */
    break;

  case 45:
#line 525 "ncap_yacc.y" /* yacc.c:1646  */
    {
  /* Addition commutes so swap arguments and use S+V = V+S */
  (yyval.var)=ncap_var_scv_add((yyvsp[0].var),(yyvsp[-2].scv));
}
#line 2085 "ncap_yacc.c" /* yacc.c:1646  */
    break;

  case 46:
#line 529 "ncap_yacc.y" /* yacc.c:1646  */
    { /* Begin Subtraction */
  (yyval.var)=ncap_var_var_sub((yyvsp[-2].var),(yyvsp[0].var));
}
#line 2093 "ncap_yacc.c" /* yacc.c:1646  */
    break;

  case 47:
#line 532 "ncap_yacc.y" /* yacc.c:1646  */
    {
  (yyval.var)=ncap_var_scv_sub((yyvsp[-2].var),(yyvsp[0].scv));
}
#line 2101 "ncap_yacc.c" /* yacc.c:1646  */
    break;

  case 48:
#line 535 "ncap_yacc.y" /* yacc.c:1646  */
    { 
  /* Subtraction is non-commutative, do not swap arguments and/or re-use V-S subtraction function
     Use anti-symmetric property that V-S=-(S-V) */
  scv_sct minus;
  minus.val.b=-1;
  minus.type=NC_BYTE;
  (void)nco_scv_cnf_typ((yyvsp[0].var)->type,&minus);
  (void)ncap_var_scv_sub((yyvsp[0].var),(yyvsp[-2].scv));
  (yyval.var)=ncap_var_scv_mlt((yyvsp[0].var),minus);
}
#line 2116 "ncap_yacc.c" /* yacc.c:1646  */
    break;

  case 49:
#line 545 "ncap_yacc.y" /* yacc.c:1646  */
    { /* Begin Multiplication */
  (yyval.var)=ncap_var_var_mlt((yyvsp[-2].var),(yyvsp[0].var)); 
}
#line 2124 "ncap_yacc.c" /* yacc.c:1646  */
    break;

  case 50:
#line 548 "ncap_yacc.y" /* yacc.c:1646  */
    {
  (yyval.var)=ncap_var_scv_mlt((yyvsp[-2].var),(yyvsp[0].scv));
}
#line 2132 "ncap_yacc.c" /* yacc.c:1646  */
    break;

  case 51:
#line 551 "ncap_yacc.y" /* yacc.c:1646  */
    {
  /* Addition commutes so swap arguments and use S*V = V*S */
  (yyval.var)=ncap_var_scv_mlt((yyvsp[0].var),(yyvsp[-2].scv));
}
#line 2141 "ncap_yacc.c" /* yacc.c:1646  */
    break;

  case 52:
#line 555 "ncap_yacc.y" /* yacc.c:1646  */
    { /* Begin Division */
  /* NB: Order was important (keeping denominator as I/O variable)
     This is no longer true with ncbo
     Maybe rewrite to keep argument ordering consitent with multiplication, addition */
  (yyval.var)=ncap_var_var_dvd((yyvsp[0].var),(yyvsp[-2].var));
}
#line 2152 "ncap_yacc.c" /* yacc.c:1646  */
    break;

  case 53:
#line 561 "ncap_yacc.y" /* yacc.c:1646  */
    { /* Keep V as I/O */
  (yyval.var)=ncap_var_scv_dvd((yyvsp[-2].var),(yyvsp[0].scv));
}
#line 2160 "ncap_yacc.c" /* yacc.c:1646  */
    break;

  case 54:
#line 564 "ncap_yacc.y" /* yacc.c:1646  */
    {
 /* Division is non-commutative, use S/V not V/S division function */
  (yyval.var)=ncap_scv_var_dvd((yyvsp[-2].scv),(yyvsp[0].var));
}
#line 2169 "ncap_yacc.c" /* yacc.c:1646  */
    break;

  case 55:
#line 568 "ncap_yacc.y" /* yacc.c:1646  */
    { /* Begin Modulo */
  (yyval.var)=ncap_var_var_mod((yyvsp[-2].var),(yyvsp[0].var));
}
#line 2177 "ncap_yacc.c" /* yacc.c:1646  */
    break;

  case 56:
#line 571 "ncap_yacc.y" /* yacc.c:1646  */
    {
  (yyval.var)=ncap_var_scv_mod((yyvsp[-2].var),(yyvsp[0].scv));
}
#line 2185 "ncap_yacc.c" /* yacc.c:1646  */
    break;

  case 57:
#line 574 "ncap_yacc.y" /* yacc.c:1646  */
    {
  /* Modulo is non-commutative, use S%V not V%S modulo function */
  (yyval.var)=ncap_scv_var_mod((yyvsp[-2].scv),(yyvsp[0].var));
}
#line 2194 "ncap_yacc.c" /* yacc.c:1646  */
    break;

  case 58:
#line 578 "ncap_yacc.y" /* yacc.c:1646  */
    { /* Begin Empowerment of form x^y */
  (yyval.var)=ncap_var_var_pwr((yyvsp[-2].var),(yyvsp[0].var));
}
#line 2202 "ncap_yacc.c" /* yacc.c:1646  */
    break;

  case 59:
#line 581 "ncap_yacc.y" /* yacc.c:1646  */
    {
  (yyval.var)=ncap_var_scv_pwr((yyvsp[-2].var),(yyvsp[0].scv));
}
#line 2210 "ncap_yacc.c" /* yacc.c:1646  */
    break;

  case 60:
#line 584 "ncap_yacc.y" /* yacc.c:1646  */
    {
  /* Empowerment is non-commutative, use S^V not V^S empowerment function */
  (yyval.var)=ncap_scv_var_pwr((yyvsp[-2].scv),(yyvsp[0].var));
}
#line 2219 "ncap_yacc.c" /* yacc.c:1646  */
    break;

  case 61:
#line 588 "ncap_yacc.y" /* yacc.c:1646  */
    { /* Begin Empowerment of form pow(x,y) */
  /* fxm: TODO ncap52 Combine pow() with ^ parsing in parser ncap_yacc.y */
  (yyval.var)=ncap_var_var_pwr((yyvsp[-3].var),(yyvsp[-1].var));
}
#line 2228 "ncap_yacc.c" /* yacc.c:1646  */
    break;

  case 62:
#line 592 "ncap_yacc.y" /* yacc.c:1646  */
    {
  (yyval.var)=ncap_var_scv_pwr((yyvsp[-3].var),(yyvsp[-1].scv));
}
#line 2236 "ncap_yacc.c" /* yacc.c:1646  */
    break;

  case 63:
#line 595 "ncap_yacc.y" /* yacc.c:1646  */
    {
  /* Empowerment is non-commutative, use S^V not V^S empowerment function */
  (yyval.var)=ncap_scv_var_pwr((yyvsp[-3].scv),(yyvsp[-1].var));
}
#line 2245 "ncap_yacc.c" /* yacc.c:1646  */
    break;

  case 64:
#line 599 "ncap_yacc.y" /* yacc.c:1646  */
    { /* Begin Unary Subtraction */
  scv_sct minus;
  minus.val.b=-1;
  minus.type=NC_BYTE;
  (yyval.var)=ncap_var_scv_mlt((yyvsp[0].var),minus);
}
#line 2256 "ncap_yacc.c" /* yacc.c:1646  */
    break;

  case 65:
#line 605 "ncap_yacc.y" /* yacc.c:1646  */
    { /* Begin Unary Addition */
  (yyval.var)=(yyvsp[0].var);
}
#line 2264 "ncap_yacc.c" /* yacc.c:1646  */
    break;

  case 66:
#line 608 "ncap_yacc.y" /* yacc.c:1646  */
    {
  (yyval.var)=ncap_var_abs((yyvsp[-1].var));
}
#line 2272 "ncap_yacc.c" /* yacc.c:1646  */
    break;

  case 67:
#line 611 "ncap_yacc.y" /* yacc.c:1646  */
    {
  (yyval.var)=ncap_var_abs((yyvsp[-1].var));
  /* fxm Finish avg,min,max,ttl */
  /* $$=nco_var_avg($3,dim,dmn_nbr,nco_op_typ); */
  /* if(prs_arg->nco_op_typ == nco_op_avg) (void)nco_var_dvd(var_prc_out[idx]->type,var_prc_out[idx]->sz,var_prc_out[idx]->has_mss_val,var_prc_out[idx]->mss_val,wgt_avg->val,var_prc_out[idx]->val); */
  (void)fprintf(stderr,"%s: WARNING RDC tokens not implemented yet\n",nco_prg_nm_get());
  /* $3 is freed in nco_var_avg() */
}
#line 2285 "ncap_yacc.c" /* yacc.c:1646  */
    break;

  case 68:
#line 619 "ncap_yacc.y" /* yacc.c:1646  */
    {
  /* Packing variable does not create duplicate so DO NOT free $3 */
  const nc_type nc_typ_pck_dfl=NC_SHORT; /* [enm] Default type to pack to */
  nco_bool PCK_VAR_WITH_NEW_PCK_ATT;
  
  if(((prs_sct*)prs_arg)->ntl_scn){
    (yyvsp[-1].var)->undefined=True;
    (yyval.var)=(yyvsp[-1].var);
  }else{ 
    (yyval.var)=nco_var_pck((yyvsp[-1].var),nc_typ_pck_dfl,&PCK_VAR_WITH_NEW_PCK_ATT);
    PCK_VAR_WITH_NEW_PCK_ATT=PCK_VAR_WITH_NEW_PCK_ATT+0; /* CEWI */
  } /* end else */
}
#line 2303 "ncap_yacc.c" /* yacc.c:1646  */
    break;

  case 69:
#line 632 "ncap_yacc.y" /* yacc.c:1646  */
    {
  /* Unpacking variable does not create duplicate so DO NOT free $3 */
  /* Do not unpack on first pass */
  if(((prs_sct*)prs_arg)->ntl_scn){
    (yyvsp[-1].var)->undefined=True;
    (yyval.var)=(yyvsp[-1].var);
  }else{ 
    (yyval.var)=nco_var_upk((yyvsp[-1].var));
  } /* end else */
}
#line 2318 "ncap_yacc.c" /* yacc.c:1646  */
    break;

  case 70:
#line 642 "ncap_yacc.y" /* yacc.c:1646  */
    {
  (yyval.var)=ncap_var_fnc((yyvsp[-1].var),(yyvsp[-3].sym));
}
#line 2326 "ncap_yacc.c" /* yacc.c:1646  */
    break;

  case 71:
#line 645 "ncap_yacc.y" /* yacc.c:1646  */
    {
  (yyval.var)=(yyvsp[-1].var);
}
#line 2334 "ncap_yacc.c" /* yacc.c:1646  */
    break;

  case 72:
#line 648 "ncap_yacc.y" /* yacc.c:1646  */
    {
  (yyval.var)=nco_var_cnf_typ((yyvsp[-3].cnv_type),(yyvsp[-1].var));
}
#line 2342 "ncap_yacc.c" /* yacc.c:1646  */
    break;

  case 73:
#line 651 "ncap_yacc.y" /* yacc.c:1646  */
    { /* Terminal symbol action */
  /* fxm: Allow commands like a=M_PI*rds^2; to work */
}
#line 2350 "ncap_yacc.c" /* yacc.c:1646  */
    break;

  case 74:
#line 654 "ncap_yacc.y" /* yacc.c:1646  */
    { /* Terminal symbol action */
  
  var_sct *var;
  var_sct *var_tmp;
  prs_sct *prs_drf; /*Pointer for de-referencing */
  
  prs_drf=(prs_sct*)prs_arg;
  
  var=ncap_var_init((yyvsp[0].var_nm_RHS),prs_drf);
  var->undefined=False;
  
  if(prs_drf->ntl_scn == True && prs_drf->var_LHS != NULL){
    var_tmp=nco_var_dpl(prs_drf->var_LHS);
    var_tmp->id=var->id;
    var_tmp->nm=(char*)nco_free(var_tmp->nm);
    var_tmp->nm=strdup((yyvsp[0].var_nm_RHS));
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
  
    if(nco_dbg_lvl_get() >= nco_dbg_scl) (void)fprintf(stderr,"%s: Stretching variable %s with LHS template: Template var->nm %s, var->nbr_dim %d, var->sz %li\n",nco_prg_nm_get(),var->nm,prs_drf->var_LHS->nm,prs_drf->var_LHS->nbr_dim,prs_drf->var_LHS->sz);
    var->undefined=False;
  } /* endif LHS_cst */
  (yyvsp[0].var_nm_RHS)=(char*)nco_free((yyvsp[0].var_nm_RHS));
  (yyval.var)=var;
  /* Sanity check */
  if ((yyval.var)==(var_sct *)NULL) YYERROR;
}
#line 2398 "ncap_yacc.c" /* yacc.c:1646  */
    break;


#line 2402 "ncap_yacc.c" /* yacc.c:1646  */
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (prs_arg, YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (prs_arg, yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval, prs_arg);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp, prs_arg);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (prs_arg, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, prs_arg);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[*yyssp], yyvsp, prs_arg);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}
#line 700 "ncap_yacc.y" /* yacc.c:1906  */

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
  
  prs_arg=prs_arg+0; /* CEWI otherwise unused parameter error */

  /* if(eprovoke_skip){eprovoke_skip=False ; return 0;} */
  if(nco_dbg_lvl_get() >= nco_dbg_std){
    (void)fprintf(stderr,"%s: %s line %lu",nco_prg_nm_get(),ncap_fl_spt_glb[ncap_ncl_dpt_crr],(unsigned long)ncap_ln_nbr_crr[ncap_ncl_dpt_crr]);
    if(nco_dbg_lvl_get() >= nco_dbg_fl) (void)fprintf(stderr," %s",err_sng_lcl);
    (void)fprintf(stderr,"\n");
    (void)fflush(stderr);
  } /* endif dbg */
  
  if(err_sng_lcl[0] == '#') eprovoke_skip=True;
  eprovoke_skip=eprovoke_skip+0; /* CEWI Do nothing except avoid compiler warnings */
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
