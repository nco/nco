/* $Header$ */

/* Purpose: netCDF arithmetic processor definitions and function prototypes for ncap.c, ncap_utl.c, ncap_lex.l, and ncap_yacc.y */

/* Copyright (C) 1995--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

/* Usage:
   #include "ncap.h" *//* netCDF arithmetic processor-specific definitions (symbol table, ...) */

#ifndef NCAP_UTL_H /* Header file has not yet been defined in current source file */
#define NCAP_UTL_H

#ifdef HAVE_CONFIG_H
# include <config.h> /* Autotools tokens */
#endif /* !HAVE_CONFIG_H */

/* Standard header files */
#include <math.h> /* sin cos cos sin 3.14159 */
#include <stdio.h> /* stderr, FILE, NULL, etc. */
#include <stdlib.h> /* atof, atoi, malloc, getopt */
#include <string.h> /* strcmp() */
#include <time.h> /* machine time */
#ifndef _MSC_VER
# include <unistd.h> /* POSIX stuff */
#endif

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions and C library */
#include "nco_netcdf.h" /* NCO wrappers for libnetcdf.a */

/* Personal headers */
#include "libnco.h" /* netCDF Operator (NCO) library */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Define symbol table */
/* YACC seems to initialize all user-defined pointers (like those in symbol table) to NULL
   Symbol type may, therefore, be determined safely by testing for non-NULL members
   This technique is frequently used in Pigeon book */

/* Symbol structure */
typedef struct{ /* sym_sct */
  char *nm; /* [sng] Symbol name */
  double (*fnc_dbl)(double); /* [fnc] Double-valued function */
  float (*fnc_flt)(float); /* [fnc] Float-valued function */
  nod_typ_enm nod_typ; /* [enm] Node type */
} sym_sct;

/* Operator */
typedef struct{ /* Nie02 oprNodeType */
  nod_typ_enm nod_typ; /* [enm] Node type */
  int opr_tkn; /* [enm] Operator token */
  int arg_nbr; /* [nbr] Number of arguments */
  union nod_typ_tag *arg; /* [unn] Argument list (has arg_nbr elements) */
} opr_nod_sct;

typedef union nod_typ_tag{ /* Nie02 nodeTypeTag */
  nod_typ_enm nod_typ; /* [enm] Node type */
  scv_sct scv; /* [sct] Scalar value */
  sym_sct sym; /* [sct] Intrinsic function name */
  opr_nod_sct opr; /* [sct] Operator structure */
} nodeType;

/* Parse structure 
   prs_sct is easy to confuse with yylval since both are passed to lexer
   yylval is union of all possible data types for semantic values
   prs_sct is passed into lexer by calling routine (e.g., main())
   prs_sct elements are essentially global variables which can be modified in lexer
   Calling routine (e.g., main()) and parser then have access to prs_sct 
   Two elements of prs_sct, var_LHS & nco_op_typ, are more local in that
   they may change line-to-line in input script. */
typedef struct{ /* prs_sct */
  char *fl_in; /* [sng] Input data file */
  int in_id; /* [id] Input data file ID */
  char *fl_out; /* [sng] Output data file */
  int out_id; /* [id] Output data file ID */
  aed_sct ***att_lst; /* [sct] list of Attributes defined in script */
  int *nbr_att; /* [nbr] Number of attributes in script */
  dmn_sct **dmn_in; /* [dmn] List of all  dimensions in input file */
  int nbr_dmn_in; /* [nbr] Number of  dimensions in above list */
  dmn_sct ***dmn_out; /* pointer to a list of dimensions currently in output file */
  int *nbr_dmn_out;   /* number of dimensions in above list */
  sym_sct **sym_tbl; /* [fnc] Symbol table for functions */
  int sym_tbl_nbr; /* [nbr] Number of functions in table */
  nco_bool ntl_scn; /* [flg] Initial scan of script */
  var_sct *var_LHS; /* [var] LHS cast variable */
  var_sct ***var_lst; /* list of variables to be defined in O */
  int *nbr_var;        /* [nbr] of variables in above list */ 
  int nco_op_typ; /* [enm] Operation type */
  int dfl_lvl; /* [enm] Deflate level */
  size_t *cnk_sz; /* [nbr] Chunk sizes */
} prs_sct;

  
/*  funtion in ncap_lex.l */
int 
ncap_ncwa_scn
(prs_sct * const prs_arg,
 const char * const msk_sng,
 char **msk_nm,
 double *msk_val,
 int *op_typ_rlt);            
/* End funtions in ncap_lex.l */

/* Begin funtions in ncap_yacc.y */
void
freeNode /* [fnc] Free syntax tree node Nie02 freeNode() */
(nodeType *nod); /* I/O [sct] Syntax tree node to free */

aed_sct *  /* O [idx] Pointer to attribute in list */
ncap_aed_lookup /* [fnc] Find location of existing attribute or add new attribute */
(const char * const var_nm, /* I [sng] Variable name */
 const char * const att_nm, /* I [sng] Attribute name */
 prs_sct * const prs_arg,  /* I/O [sct] contains attribute list  */
 const nco_bool update); /* I [flg] Delete existing value or add new attribute to list */

var_sct *                      /*I [sct] varibale in list */
ncap_var_lookup
(var_sct *var,   /* I  [sct] variable  */
 prs_sct *prs_arg,             /* I/O [sct] contains var list */
 const nco_bool add);              /* I if not in list then add to list */          

void nco_var_free_wrp(var_sct **);

nodeType * /* O [unn] Syntax tree node */
opr_ctl /* [fnc] Operation controller function Nie02 opr() */
(int opr_tkn, /* I [enm] Operator token */
 int arg_nbr, /* I [nbr] Number of arguments */
 ... /* I [llp] Ellipsis defined in stdarg.h */
 ); /* end opr_ctl() */

int nco_yyerror(prs_sct* prs_arg, const char * const sng);
/* End funtions in ncap_yacc.y */

  /* fxm: Following functions need editing, const'ifying, etc. */
nco_bool ncap_var_stretch(var_sct **,var_sct **);
dmn_sct **nco_dmn_out_grow(prs_sct *);
int ncap_scv_minus(scv_sct *);
int ncap_var_write(var_sct *,prs_sct*);
ptr_unn ncap_scv_2_ptr_unn(scv_sct); 
scv_sct ncap_scv_abs(scv_sct);
scv_sct ncap_scv_clc(scv_sct,const char,scv_sct);
sym_sct *ncap_sym_init(const char * const sym_nm,double (*fnc_dbl)(double),float (*fnc_flt)(float));
sym_sct *ncap_scl_mk_sym(double val);
sym_sct *ncap_sym_look(char *sym_nm);
var_sct *ncap_var_abs(var_sct *);
var_sct *ncap_var_fnc(var_sct *,sym_sct *);
/* var_sct *ncap_var_init(const char * const,prs_sct *);*/
var_sct *ncap_var_init(char * ,prs_sct *);
void ncap_fnc_add(char *nm, double (*fnc_dbl)());
void nco_lbr_vrs_prn(void);

/* End funtions in ncap_utl.c */

/*  the following file is ncap_yacc.h generated by BISON */

/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

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

#ifndef YY_NCO_YY_NCAP_YACC_H_INCLUDED
# define YY_NCO_YY_NCAP_YACC_H_INCLUDED
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
#line 114 "ncap_yacc.y" /* yacc.c:1909  */

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

#line 123 "ncap_yacc.h" /* yacc.c:1909  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif



int nco_yyparse (prs_sct *prs_arg);

#endif /* !YY_NCO_YY_NCAP_YACC_H_INCLUDED  */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCAP_UTL_H */

