/* $Header: /data/zender/nco_20150216/nco/src/nco/ncap.h,v 1.51 2002-09-09 06:39:18 zender Exp $ */

/* Purpose: netCDF arithmetic processor */

/* Copyright (C) 1995--2002 Charlie Zender
   This software is distributed under the terms of the GNU General Public License
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

/* Usage:
   #include "ncap.h" *//* netCDF arithmetic processor */

#ifndef NCAP_H /* Header file has not yet been defined in current source file */
#define NCAP_H

#ifdef HAVE_CONFIG_H
#include <config.h> /* Autotools tokens */
#endif /* !HAVE_CONFIG_H */

/* Standard header files */
#include <math.h> /* sin cos cos sin 3.14159 */
#include <stdio.h> /* stderr, FILE, NULL, etc. */
#include <stdlib.h> /* atof, atoi, malloc, getopt */
#include <string.h> /* strcmp. . . */
#include <time.h> /* machine time */
#include <unistd.h> /* POSIX stuff */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions */
#include "nco_netcdf.h" /* netCDF3.0 wrapper functions */

/* Personal headers */
#include "libnco.h" /* netCDF operator library */

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

/* Name list structure (for subscript lists) */
typedef struct{ /* nm_lst_sct */
  nm_id_sct *lst; /* [sct] List element */
  int nbr; /* [nbr] Number of structures in list */
} nm_lst_sct;

/* Parse structure 
   prs_sct is easy to confuse with yylval since both are passed to lexer
   yylval is union of all possible data types for semantic values
   prs_sct is passed into lexer by calling routine (e.g., main())
   prs_sct elements are essentially global variables which can be modified in lexer
   Calling routine (e.g., main()) and parser then have access prs_sct 
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
  bool ntl_scn; /* [flg] Initial scan of script */
  var_sct *var_LHS; /* [var] LHS cast variable */
  int nco_op_typ; /* [enm] Operation type */
} prs_sct;

/* These funtions are either in ncap.y or ncap_utl.c */

nodeType * /* O [unn] Syntax tree node */
opr_ctl /* [fnc] Operation controller function Nie02 opr() */
(int opr_tkn, /* I [enm] Operator token */
 int arg_nbr, /* I [nbr] Number of arguments */
 ... /* I [llp] Ellipsis defined in stdarg.h */
 ); /* end opr_ctl() */

void
freeNode /* [fnc] Free syntax tree node Nie02 freeNode() */
(nodeType *nod); /* I/O [sct] Syntax tree node to free */

aed_sct *  /* O [idx] Pointer to attribute in list */
ncap_aed_lookup /* [fnc] Find location of existing attribute or add new attribute */
(const char * const var_nm, /* I [sng] Variable name */
 const char * const att_nm, /* I [sng] Attribute name */
 prs_sct * const prs_arg,  /* I/O [sct] contains attribute list  */
 const bool update); /* I [flg] Delete existing value or add new attribute to list */

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

bool ncap_var_stretch(var_sct **,var_sct **);
int ncap_scv_minus(scv_sct *);
int ncap_var_write(var_sct *,prs_sct*);
int yyerror(char *sng);
nm_id_sct *nco_var_lst_crd_make(int,nm_id_sct *,int *);
nm_id_sct *nco_var_lst_add(nm_id_sct *,int *,nm_id_sct *,int);
nm_id_sct *nco_var_lst_copy(nm_id_sct *,int);
nm_id_sct *nco_var_lst_free(nm_id_sct *,int);
nm_id_sct *nco_var_lst_sub(nm_id_sct *,int *,nm_id_sct *,int);
nm_id_sct *nco_dmn_lst(const int, int * const);
nm_id_sct *nco_att_lst_mk(int, int,aed_sct** ,int ,int *);
ptr_unn ncap_scv_2_ptr_unn(scv_sct); 
scv_sct ncap_scv_abs(scv_sct);
scv_sct ncap_scv_calc(scv_sct,char,scv_sct);
sym_sct *ncap_sym_init(char *,double (*fnc_dbl)(double),float (*fnc_flt)(float));
sym_sct *ncap_scl_mk_sym(double val);
sym_sct *ncap_sym_look(char *sym_nm);
var_sct *ncap_var_abs(var_sct *);
var_sct *ncap_var_fnc(var_sct *,sym_sct *);
var_sct *ncap_var_init(const char * const,prs_sct *);
var_sct *ncap_var_scv_add(var_sct *,scv_sct);
var_sct *ncap_var_scv_dvd(var_sct *,scv_sct);
var_sct *ncap_var_scv_mod(var_sct *,scv_sct);
var_sct *ncap_var_scv_mlt(var_sct *,scv_sct);
var_sct *ncap_var_scv_pwr(var_sct *,scv_sct);
var_sct *ncap_var_scv_sub(var_sct *,scv_sct);
var_sct *ncap_var_var_add(var_sct *var_1,var_sct *var_2);
var_sct *ncap_var_var_dvd(var_sct *var_1,var_sct *var_2);
var_sct *ncap_var_var_mlt(var_sct *var_1,var_sct *var_2);
var_sct *ncap_var_var_sub(var_sct *var_1,var_sct *var_2);
void ncap_fnc_add(char *nm, double (*fnc_dbl)());
void nco_lib_vrs_prn();
void nco_var_free_wrp(var_sct **);
dmn_sct **nco_dmn_out_grow(prs_sct *);
/* Source code for following functions is generated by parser but parser
   does not do good job of making these prototypes known so routines that
   call these functions must either include their prototypes locally,
   which is bad form if it needs to be done in more than one location,
   or prototype the parser routines here */

  /* fxm: Eventually make this MACOSX kludge go away */
#ifndef MACOSX_FLT_FNC
#define MACOSX_FLT_FNC
#ifdef MACOSX
#define NEED_ACOSF 1
#define NEED_ACOSHF 1
#define NEED_ASINF 1
#define NEED_ASINHF 1
#define NEED_ATANF 1
#define NEED_ATANHF 1
#define NEED_CEILF 1
#define NEED_COSF 1
#define NEED_COSHF 1
#define NEED_ERFCF 1
#define NEED_ERFF 1
#define NEED_EXPF 1
#define NEED_FLOORF 1
#define NEED_FMODF 1
#define NEED_GAMMAF 1
#define NEED_LOG10F 1
#define NEED_LOGF 1
#define NEED_POWF 1
#define NEED_SINF 1
#define NEED_SINHF 1
#define NEED_SQRTF 1
#define NEED_TANF 1
#define NEED_TANHF 1
#endif /* !MACOSX */
#ifdef NEED_FMODF
inline float fmodf(float x,float y){return (float)(fmod(x,y));}
#endif /* !NEED_FMODF */ 
#ifdef NEED_POWF
inline float powf(float x,float y){return (float)(pow(x,y));}
#endif /* !NEED_POWF */ 
#ifdef NEED_GAMMAF
#ifdef NEED_ACOSF
inline float acosf(float x){return (float)(acos(x));}
#endif /* !NEED_ACOSF */ 
#ifdef NEED_ACOSHF
inline float acoshf(float x){return (float)(acosh(x));}
#endif /* !NEED_ACOSHF */ 
#ifdef NEED_ASINF
inline float asinf(float x){return (float)(asin(x));}
#endif /* !NEED_ASINF */ 
#ifdef NEED_ASINHF
inline float asinhf(float x){return (float)(asinh(x));}
#endif /* !NEED_ASINHF */ 
#ifdef NEED_ATANF
inline float atanf(float x){return (float)(atan(x));}
#endif /* !NEED_ATANF */ 
#ifdef NEED_ATANHF
inline float atanhf(float x){return (float)(atanh(x));}
#endif /* !NEED_ATANHF */ 
#ifdef NEED_CEILF
inline float ceilf(float x){return (float)(ceil(x));}
#endif /* !NEED_CEILF */ 
#ifdef NEED_COSF
inline float cosf(float x){return (float)(cos(x));}
#endif /* !NEED_COSF */ 
#ifdef NEED_COSHF
inline float coshf(float x){return (float)(cosh(x));}
#endif /* !NEED_COSHF */ 
#ifdef NEED_ERFCF
inline float erfcf(float x){return (float)(erfc(x));}
#endif /* !NEED_ERFCF */ 
#ifdef NEED_ERFF
inline float erff(float x){return (float)(erf(x));}
#endif /* !NEED_ERFF */ 
#ifdef NEED_EXPF
inline float expf(float x){return (float)(exp(x));}
#endif /* !NEED_EXPF */ 
#ifdef NEED_FLOORF
inline float floorf(float x){return (float)(floor(x));}
#endif /* !NEED_FLOORF */ 
inline float gammaf(float x){return (float)(gamma(x));}
#endif /* !NEED_GAMMAF */ 
#ifdef NEED_LOG10F
inline float log10f(float x){return (float)(log10(x));}
#endif /* !NEED_LOG10F */ 
#ifdef NEED_LOGF
inline float logf(float x){return (float)(log(x));}
#endif /* !NEED_LOGF */ 
#ifdef NEED_SINF
inline float sinf(float x){return (float)(sin(x));}
#endif /* !NEED_SINF */ 
#ifdef NEED_SINHF
inline float sinhf(float x){return (float)(sinh(x));}
#endif /* !NEED_SINHF */ 
#ifdef NEED_SQRTF
inline float sqrtf(float x){return (float)(sqrt(x));}
#endif /* !NEED_SQRTF */ 
#ifdef NEED_TANF
inline float tanf(float x){return (float)(tan(x));}
#endif /* !NEED_TANF */ 
#ifdef NEED_TANHF
inline float tanhf(float x){return (float)(tanh(x));}
#endif /* !NEED_TANHF */ 
#endif /* MACOSX_FLT_FNC */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCAP_H */




