/* $Header$ */
/* Purpose: netCDF arithmetic processor definitions and function prototypes for ncap.c, ncap_utl.c, ncap_lex.l, and ncap_yacc.y */

/* Copyright (C) 1995--2014 Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You can redistribute and/or modify NCO under the terms of the 
   GNU General Public License (GPL) Version 3 with exceptions described in the LICENSE file */

/* Usage:
   #include "ncap.h" *//* netCDF arithmetic processor-specific definitions (symbol table, ...) */

#ifndef NCAP2_UTL_HH /* Header file has not yet been defined in current source file */
#define NCAP2_UTL_HH

#ifdef HAVE_CONFIG_H
#include <config.h> /* Autotools tokens */
#endif /* !HAVE_CONFIG_H */

/* Standard header files */
#include <string>
#include <math.h> /* sin cos cos sin 3.14159 */
#include <stdio.h> /* stderr, FILE, NULL, etc. */
#include <stdlib.h> /* atof, atoi, malloc, getopt */
#include <string.h> /* strcmp. . . */
#include <time.h> /* machine time */
#ifndef _MSC_VER
#include <unistd.h> /* POSIX stuff */
#endif
#if !(defined __xlC__) && !(defined SGIMP64) // C++ compilers that do not allow stdint.h
#include <stdint.h> // Required by g++ for LLONG_MAX, ULLONG_MAX, by icpc for int64_t
#endif // C++ compilers that do not allow stdint.h

#include <assert.h>
#include <cmath> // sin cos cos sin 3.14159
#include <ctype.h>

// Standard C++ headers
#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>


/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions and C library */
#include "nco_netcdf.h" /* NCO wrappers for libnetcdf.a */
#include <antlr/AST.hpp> /* nneeded for ast_ind struct */
#include "ncoParserTokenTypes.hpp"


/* Personal headers */
#include "libnco.h" /* netCDF Operator (NCO) library */
#include "ncap2.hh"
#include "NcapVector.hh"
#include "NcapVarVector.hh"
#include "NcapVar.hh"
#include "sdo_utl.hh"
#include "VarOpNew.hh" 
#include "prs_cls.hh"
#include "ncap2_att.hh"

/* Don't know what Charlies done to the bools */
/* Temporary fix for now !!*/
#define nco_bool int

// Compress  code a little
#define SCS(x)  static_cast<std::string>(x)     

/* Used to do an pre-mpi sort */
 typedef struct exp_sct_tag { 
  ANTLR_USE_NAMESPACE(antlr)RefAST etr;
  std::vector<std::string> lvl_vtr; // lvalues
  std::vector<int>    dpd_vtr; // dependencies
  std::vector<exp_sct_tag**> srp_vtr; //self reverential pointer
} exp_sct ;	


/***************  misc functions ********************************************************/

std::vector<std::string> /* [O] [vector] list of files paths to be used to locate include files */
ncap_make_include_paths  
(const char *sin);       /* list of file path(s) delimited by ':' */


nco_bool
ncap_def_dim(
std::string dmn_nm,
long sz,
bool ltype,
prs_cls *prs_arg);

void
nco_get_var_mem(
var_sct *var_rhs,
NcapVector<dmn_sct*> &dmn_vtr);

void
nco_put_var_mem(
var_sct *var_in,
var_sct *var_nw,
NcapVector<lmt_sct*> &lmt_vtr);

nco_bool         /* Returns True if shape of vars match (using cnt vectors */
nco_shp_chk(
var_sct* var1, 
var_sct* var2); 

var_sct *      /* initialize var to defaults & undefined to true */
ncap_var_udf
(const char *var_nm);
 


nco_bool 
ncap_var_is_op_doable( 
var_sct *var1, 
var_sct *var2); 


nco_bool       /* returns true if order and size of dims match exactly */
ncap_top_shp_chk(
var_sct* var1, 
var_sct* var2); 

nco_bool       /* true if oder & size of dims match (after removing (degenerate) size 1 dims */
ncap_norm_shp_chk(
var_sct* var1, 
var_sct* var2); 

void
ncap_sngcpy(  /* take a list of nco_string's and refresh them with strdup */
char *cp,
int bsz);

/***************************************************************************************/


/*************** custom math functions *********************************************************/


var_sct *   /* O [sct] Remainder of modulo operation of input variables (var_1%var_2) */
ncap_var_var_mod /* [fnc] Remainder (modulo) operation of two variables */
(var_sct *var_1, /* I [sc,t] Variable structure containing field */
 var_sct *var_2); /* I [sct] Variable structure containing divisor */

var_sct *         /* O [sct] Empowerment of input variables (var_1^var_2) */
ncap_var_var_pwr  /* [fnc] Empowerment of two variables */ 
(var_sct *var_1,  /* I [sct] Variable structure containing base */
 var_sct *var_2); /* I [sct] Variable structure containing exponent */

var_sct *           /* O [sct] Calculate atan2(Y,X) for each element */
ncap_var_var_atan2  
(var_sct *var1,     /* I [sct] Variable structure containing Y  */
 var_sct *var2);    /* I [sct] Variable structure containing X  */

void ncap_lmt_evl( 
int nc_id,
lmt_sct* lmt_ptr,
long hint_sz,
prs_cls *prs_arg);

var_sct*
ncap_var_fnc(
var_sct* var_in,
double(*fnc_dbl)(double),
float(*fnc_flt)(float));

var_sct *         /* O [sct] Resultant variable (actually is var) */
ncap_var_abs(     /* Purpose: Find absolute value of each element of var */
var_sct *var);    /* I/O [sct] input variable */

bool            /* O [flg] true if all var elemenst are true */
ncap_var_lgcl   /* [fnc] calculate a aggregate bool value from a variable */
(var_sct* var);  /* I [sct] input variable */



/******************************************************************************************/


/******************  list functions **********************************************************/

nm_id_sct *            /* O [sct] new copy of xtr_lst */
nco_var_lst_copy(      /*   [fnc] Purpose: Copy xtr_lst and return new list */
nm_id_sct *xtr_lst,    /* I  [sct] input list */ 
int lst_nbr);           /* I  [nbr] number of elements in list */

nm_id_sct *             /* O [sct] New list */
nco_var_lst_sub(        /* [fnc] subract elements of lst_b from )lst */
nm_id_sct *xtr_lst,     /* I [sct] input list */   
int *nbr_xtr,           /* I/O [ptr] size of xtr_lst and new list */
nm_id_sct *xtr_lst_b,   /* I [sct] list to be subtracted */   
int nbr_lst_b);          /* I [nbr] size eof xtr_lst_b */ 

nm_id_sct *            /* O [sct] -- new list */
nco_var_lst_add(       /* [fnc]  add elemenst of lst_a to lst */
nm_id_sct *xtr_lst,    /* I [sct] input list */ 
int *nbr_xtr,          /* I/O [ptr] -- size of xtr_lst & new output list */ 
nm_id_sct *xtr_lst_a,  /* I [sct] list of elemenst to be added to new list */
int nbr_lst_a);         /* I [nbr] size of xtr_lst_a */

nm_id_sct *               /* O [sct] List of dimensions associated with input variable list */
nco_dmn_lst               /* [fnc] Create list of all dimensions in file  */
(const int nc_id,         /* I [id] netCDF input-file ID */
 int * const nbr_dmn);    /* O [nbr] Number of dimensions in  list */

nm_id_sct *                /* O [sct] output list */ 
nco_att_lst_mk      
(const int in_id,         /* I [id] of input file */
 const int out_id,        /* I [id] id of output file */
 NcapVarVector &var_vtr,  /* I [vec] vector of vars & att */
 int *nbr_lst);            /* O [ptr] size of output list */

/******************************************************************************************/


/************************ casting functions ***********************************************/

var_sct*                           /* O [sct] casting variable has its own private dims */ 
ncap_cst_mk(                       /* [fnc] create casting var from a list of dims */
std::vector<std::string> &str_vtr,  /* I [sng] list of dimension subscripts */
prs_cls *prs_arg);

var_sct*
ncap_cst_do(
var_sct* var,
var_sct* var_cst,
bool bntlscn);

NcapVector<dmn_sct*>                /* O [sct] list of new dims to limit over */ 
ncap_dmn_mtd(
var_sct *var,                       /*  [sct] create casting var from a list of dims */
std::vector<std::string> &str_vtr);  /* I [sng] list of dimension names */


/******************************************************************************************/

/************************************AST (tree) functions ************************************/

bool           /* Returns true if expression contains a utility fuction */ 
ncap_fnc_srh(
RefAST ntr
);

void ncap_mpi_get_id  /* Extract all VAR_ID & ATT_ID from an Expression */
(
RefAST ntr,
std::vector<std::string> &str_vtr);


int            /* Sort expressions for MPI Optimization */  
ncap_mpi_srt(
RefAST ntr,
int icnt,
std::vector< std::vector<RefAST> > &all_ast_vtr, // Return a Vector of Vectors
prs_cls *prs_arg);


/******************************************************************************************/

/********************** var/var conformance  *********************************************/


nco_bool /* O [flg] Variables now conform */
ncap_var_stretch /* [fnc] Stretch variables */
(var_sct **var_1, /* I/O [ptr] First variable */
 var_sct **var_2); /* I/O [ptr] Second variable */



int                /* [flg] true they conform */         
ncap_var_att_cnf   /* [fnc] Make vars/atts conform */
(var_sct *&var1,   /* I [sct] Input variable structure  */
 var_sct *&var2    /* I [sct] Input variable structure  */
);

var_sct *             /* O [sct] Result if ntl_scn true otherwize null */ 
ncap_var_att_arr_cnf( /* I [fnc] Make all of vars in array conform to each other */
bool ntl_scn,
var_sct ***var_arr,   /* I/O [sct] Array of variables */
int sz);               /* size of array */




var_sct *              /* O [sct] Result var_sct as if binary op had taken place */
ncap_var_att_cnf_ntl   /*   [fnc] determine resultant struct */
(var_sct *var1,        /* I [sct] Input variable structure containing first operand */
 var_sct *var2         /* I [sct] Input variable structure containing second operand */
);


var_sct *         /* O  Result of variable operation */
ncap_var_var_stc  
(var_sct *var1,  /* I [sct] Input variable structure containing first operand */
 var_sct *var2,  /* I [sct] Input variable structure containing second operand */
 int op);

var_sct *             /* O [sct] Sum of input variables (var1+var2) INITIAL SCAN ONLY */
ncap_var_var_op_ntl   /* [fnc] Add two variables */
(var_sct *var1,       /* I [sct] Input variable structure containing first operand */
 var_sct *var2,       /* I [sct] Input variable structure containing second operand */
 int op);             /* Operation +-% */

var_sct *         /* O [sct] Sum of input variables (var1+var2) */
ncap_var_var_op   /* [fnc] Add two variables */
(var_sct *var1,  /* I [sct] Input variable structure containing first operand */
 var_sct *var2,  /* I [sct] Input variable structure containing second operand */
 int op);        /* Operation +-% */

var_sct *           /* O [sct] Sum of input variables (var1+var2) */
ncap_var_var_inc    /* [fnc] Add two variables */
(var_sct *var1,     /* I [sct] Input variable structure containing first operand */
 var_sct *var2,     /* I [sct] Input variable structure containing second operand */
 int op,            /* Deal with incremental operators i.e +=,-=,*=,/= */
 bool bram,         /* I [flg] Make a RAM variable */
 bool bret,         /* I [flg] if true then return var else return NULL */
 prs_cls *prs_arg);


/******************************************************************************************/


#endif  /* NCAP2_UTL_HH */






