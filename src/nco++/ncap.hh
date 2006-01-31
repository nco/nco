/* $Header: /data/zender/nco_20150216/nco/src/nco++/ncap.hh,v 1.1 2006-01-31 06:07:58 zender Exp $ */

/* Purpose: netCDF arithmetic processor definitions and function prototypes for ncap.c, ncap_utl.c, ncap_lex.l, and ncap_yacc.y */

/* Copyright (C) 1995--2005 Charlie Zender
   This software may be modified and/or re-distributed under the terms of the GNU General Public License (GPL) Version 2
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

/* Usage:
   #include "ncap.h" *//* netCDF arithmetic processor-specific definitions (symbol table, ...) */

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
#include <string>
/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions and C library */
#include "nco_netcdf.h" /* NCO wrappers for libnetcdf.a */

/* Personal headers */
#include "libnco.h" /* netCDF Operator (NCO) library */

// defines custom "template" lists
#include "Ncap.hh"
#include <vector>
#include "NcapVector.hh"
#include "NcapVar.hh"
#include "NcapVarVector.hh"


/* Define symbol table */

typedef struct{ /* sym_sct */
  char *nm; /* [sng] Symbol name */
  double (*fnc_dbl)(double); /* [fnc] Double-valued function */
  float (*fnc_flt)(float); /* [fnc] Float-valued function */
} sym_sct;

/* Name list structure ncap.c
(for subscript lists) */
typedef struct{ /* nm_lst_sct */
  nm_id_sct *lst; /* [sct] List element */
  int nbr; /* [nbr] Number of structures in list */
} nm_lst_sct;


typedef struct{ /* prs_sct */

public:
  char *fl_in; /* [sng] Input data file */
  int in_id; /* [id] Input data file ID */
  char *fl_out; /* [sng] Output data file */
  int out_id; /* [id] Output data file ID */

  NcapVector<dmn_sct*> *ptr_dmn_in_vtr;  //Vector of dimensions in input file nb doesn't change
  NcapVector<dmn_sct*> *ptr_dmn_out_vtr; //Vector of dimensions in output file file
  NcapVector<sym_sct*> *ptr_sym_vtr;     //Vector of functions nb doesn't change
  NcapVarVector *ptr_var_vtr;            // list of attributes & variables
  bool ntl_scn; /* [flg] Initial scan of script */
} prs_sct;


/* Begin funtions in ncap_utl.c */

var_sct * /* O [sct] Remainder of modulo operation of input variables (var_1%var_2) */
ncap_var_var_mod /* [fnc] Remainder operation of two variables */ 
(var_sct *var_1, /* I [sct] Variable structure containing field */
 var_sct *var_2); /* I [sct] Variable structure containing divisor */

var_sct * /* O [sct] Empowerment of input variables (var_1^var_2) */
ncap_var_var_pwr /* [fnc] Empowerment of two variables */ 
(var_sct *var_1, /* I [sct] Variable structure containing base */
 var_sct *var_2); /* I [sct] Variable structure containing exponent */


  /* fxm: Following functions need editing, const'ifying, etc. */
var_sct *ncap_var_abs(var_sct *);
var_sct *ncap_var_fnc(var_sct *,sym_sct *);

var_sct *ncap_var_init(char * ,prs_sct *);
var_sct *ncap_att_init(const char *,prs_sct *);

int ncap_var_write(var_sct *,prs_sct*);
nm_id_sct *nco_var_lst_add(nm_id_sct *,int *,nm_id_sct *,int);
nm_id_sct *nco_var_lst_copy(nm_id_sct *,int);
nm_id_sct *nco_var_lst_sub(nm_id_sct *,int *,nm_id_sct *,int);
nm_id_sct *nco_dmn_lst(const int, int * const);
nm_id_sct *nco_att_lst_mk(int,int,NcapVarVector & ,int *);
sym_sct *ncap_sym_init(const char * const sym_nm,double (*fnc_dbl)(double),float (*fnc_flt)(float));

// stretch a single valued attribute from 1 to sz
bool ncap_att_stretch (var_sct* , long );


bool ncap_var_stretch(var_sct **,var_sct **);
  // Call to super dooper C++ template 
var_sct *ncap_var_var_op(var_sct *,var_sct*,int);
  // See if var is true or false
bool ncap_var_lgcl(var_sct* var);

  // Generate casting variable
var_sct* ncap_cast_LHS(char **sbs_lst, int lst_nbr, prs_sct *prs_arg);

/* End funtions in ncap_utl.c */


#endif /* NCAP_H */




