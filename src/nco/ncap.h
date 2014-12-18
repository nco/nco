/* $Header: /data/zender/nco_20150216/nco/src/nco/ncap.h,v 1.101 2014-12-18 18:58:38 zender Exp $ */

/* Purpose: netCDF arithmetic processor definitions and function prototypes for ncap.c, ncap_utl.c, ncap_lex.l, and ncap_yacc.y */

/* Copyright (C) 1995--2014 Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You can redistribute and/or modify NCO under the terms of the 
   GNU General Public License (GPL) Version 3 with exceptions described in the LICENSE file */

/* Usage:
   #include "ncap.h" *//* netCDF arithmetic processor-specific definitions (symbol table, ...) */

#ifndef NCAP_H /* Header file has not yet been defined in current source file */
#define NCAP_H

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

/* Begin funtions in ncap_lex.l */
void
ncap_ntl_scn /* [fnc] Scan command script, construct I/O lists */
(prs_sct * const prs_arg, /* I/O [sct] Global information required in parser routines */
 const char * const spt_arg_cat, /* I [sng] User-specified script */
 nm_id_sct** const xtr_lst_a, /* O [sct] RHS variables present in input file */
 int * const nbr_lst_a, /* O [nbr] Number of distinct RHS variables in input file */
 nm_id_sct** const xtr_lst_b, /* O [sct] LHS variables present in input file */
 int * const nbr_lst_b, /* O [nbr] Number of distinct LHS variables in input file */
 nm_id_sct** const xtr_lst_c, /* O [sct] Parent variables of LHS attributes in input file */
 int * const nbr_lst_c, /* O [nbr] Number of attribute parent variables in input file */
 nm_id_sct** const xtr_lst_d, /* O [sct] LHS dimensions in input file */
 int * const nbr_lst_d); /* O [nbr] Number of LHS dimensions in input file */
 /* end ncap_ntl_scn() prototype */

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

/* Begin funtions in ncap_utl.c */
var_sct * /* O [sct] Sum of input variables (var_1+var_2) */
ncap_var_var_add /* [fnc] Add two variables (var_1+var_2) */
(var_sct *var_1, /* I [sct] Input variable structure containing first operand */
 var_sct *var_2); /* I [sct] Input variable structure containing second operand */

var_sct * /* O [sct] Quotient of input variables (var_2/var_1) */
ncap_var_var_dvd /* [fnc] Divide two variables (var_2/var_1) */ 
(var_sct *var_1, /* I [sct] Variable structure containing denominator */
 var_sct *var_2); /* I [sct] Variable structure containing numerator */

var_sct * /* O [sct] Product of input variables (var_1*var_2) */
ncap_var_var_mlt /* [fnc] Multiply two variables (var_1*var_2) */ 
(var_sct *var_1, /* I [sct] Variable structure containing first operand */
 var_sct *var_2); /* I [sct] Variable structure containing second operand */

var_sct * /* O [sct] Remainder of modulo operation of input variables (var_1%var_2) */
ncap_var_var_mod /* [fnc] Remainder operation of two variables */ 
(var_sct *var_1, /* I [sct] Variable structure containing field */
 var_sct *var_2); /* I [sct] Variable structure containing divisor */

var_sct * /* O [sct] Empowerment of input variables (var_1^var_2) */
ncap_var_var_pwr /* [fnc] Empowerment of two variables */ 
(var_sct *var_1, /* I [sct] Variable structure containing base */
 var_sct *var_2); /* I [sct] Variable structure containing exponent */

var_sct * /* O [frc] Remainder of input variables (var_2-var_1) */
ncap_var_var_sub /* [fnc] Subtract two variables (var_2-var_1) */ 
(var_sct *var_2, /* I [sct] Variable structure containing second operand */ /* fxm TODO: 19 non-standard argument order */
 var_sct *var_1); /* I [sct] Variable structure containing first operand */

  /* fxm: Following functions need editing, const'ifying, etc. */
nco_bool ncap_var_stretch(var_sct **,var_sct **);
dmn_sct **nco_dmn_out_grow(prs_sct *);
int ncap_scv_minus(scv_sct *);
int ncap_var_write(var_sct *,prs_sct*);
nm_id_sct *nco_nm_id_lst_crd_make(int,nm_id_sct *,int *);
nm_id_sct *nco_var_lst_add(nm_id_sct *,int *,nm_id_sct *,int);
nm_id_sct *nco_var_lst_copy(nm_id_sct *,int);
nm_id_sct *nco_var_lst_sub(nm_id_sct *,int *,nm_id_sct *,int);
nm_id_sct *nco_dmn_lst(const int, int * const);
nm_id_sct *nco_att_lst_mk(int,int,aed_sct **,int,int *);
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
var_sct *ncap_var_scv_add(var_sct *,scv_sct);
var_sct *ncap_var_scv_mlt(var_sct *,scv_sct);
var_sct *ncap_var_scv_sub(var_sct *,scv_sct);
var_sct *ncap_var_scv_dvd(var_sct *,scv_sct);
var_sct *ncap_var_scv_mod(var_sct *,scv_sct);
var_sct *ncap_var_scv_pwr(var_sct *,scv_sct);
var_sct *ncap_scv_var_dvd(scv_sct scv,var_sct *var);
var_sct *ncap_scv_var_mod(scv_sct scv,var_sct *var);
var_sct *ncap_scv_var_pwr(scv_sct scv,var_sct *var);
void ncap_fnc_add(char *nm, double (*fnc_dbl)());
void nco_lbr_vrs_prn(void);

/* End funtions in ncap_utl.c */

/* Source code for following functions is generated by parser but parser
   does not do good job of making these prototypes known so routines that
   call these functions must either include their prototypes locally,
   which is bad form if it needs to be done in more than one location,
   or prototype the parser routines here */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCAP_H */
