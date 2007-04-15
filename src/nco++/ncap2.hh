/* $Header: /data/zender/nco_20150216/nco/src/nco++/ncap2.hh,v 1.35 2007-04-15 11:58:56 hmb Exp $ */

/* Purpose: netCDF arithmetic processor definitions and function prototypes for ncap.c, ncap_utl.c, ncap_lex.l, and ncap_yacc.y */

/* Copyright (C) 1995--2005 Charlie Zender
   You may copy, distribute, and/or modify this software under the terms of the GNU General Public License (GPL) Version 2
   See http://www.gnu.org/copyleft/gpl.html for full license text */

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
//#include "libnco++.hh" /* netCDF Operator (NCO) C++ library */

// defines custom "template" lists
#include "Ncap2.hh"
#include <vector>
#include "NcapVector.hh"
#include "NcapVarVector.hh"
#include "NcapVar.hh"

#include <antlr/AST.hpp> /* nneeded for ast_ind struct */

/* Define symbol table */

/* Don't what Charlies done to the bools */
/* Temporary fix for now !!*/
#define nco_bool int

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


/* Structure to hold AST pointers to indices in hyperslabs -only temporary */
typedef struct{
  ANTLR_USE_NAMESPACE(antlr)RefAST ind[3];
} ast_lmt_sct;   


/* Used to do an pre-mpi sort */
 struct exp_sct_tmp { 
  ANTLR_USE_NAMESPACE(antlr)RefAST etr;
  std::vector<std::string> lvl_vtr; // lvalues
  std::vector<int>    dpd_vtr; // dependencies
  std::vector<exp_sct_tmp**> srp_vtr; //self reverential pointer
} ;	

typedef struct exp_sct_tmp exp_sct;



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
  NcapVarVector *ptr_int_vtr;            // stores vars/atts in FIRST PARSE
  bool ntl_scn;                          // [flg] Initial scan of script 
  bool FORTRAN_IDX_CNV;                  //Use fortran convention with hyperslab indices
  bool ATT_PROPAGATE;                    //Var on LHS gets attributtes from the leftermost var on the RHS
  bool ATT_INHERIT;                      //Var on LHS inherits attributtes from var of the same name
                                         // in the input file 
  bool NCAP_MPI_SORT;                    // sort exressions after second parse for MPI optimization
                                              
} prs_sct;


/* Begin funtions in ncap_utl.c */

var_sct *                  /* O [sct] initialized variable */
ncap_var_init(
const std::string &snm, /* I [sng] variable name constant */
prs_sct *prs_arg,          /* I/O  vectors of atts,vars,dims, filenames */
bool bfll);                /* if true fill var with data */ 

int                /* O  [bool] bool - ture if sucessful */
ncap_var_write     /*   [fnc] Write var to output file prs_arg->fl_out */ 
(var_sct *var,     /* I  [sct] variable to be written - freed at end */  
 bool bram,        /* I  [bool] true if a ram only variable */
prs_sct *prs_arg); /* I/O vectors of atts & vars & file names  */


var_sct *                /* O [sct] variable containing attribute */
ncap_att_init(           /*   [fnc] Grab an attribute from input file */
std::string va_nm,       /* I [sng] att name of form var_nm&att_nm */ 
prs_sct *prs_arg);       /* I/O vectors of atts & vars & file names  */

nco_bool                /* O [flg] true if var has been stretched */
ncap_att_stretch    /* stretch a single valued attribute from 1 to sz */
(var_sct* var,      /* I/O [sct] variable */       
 long nw_sz);       /* I [nbr] new var size */

var_sct *      /* initialize var to defaults & undefined to true */
ncap_var_udf
(const char *var_nm);
 
int             
ncap_att_cpy
(std::string s_dst,
 std::string s_src,
 prs_sct  *prs_arg);

sym_sct *                    /* O [sct] return sym_sct */
ncap_sym_init                /*  [fnc] populate & return a symbol table structure */
(const char * const sym_nm,  /* I [sng] symbol name */
 double (*fnc_dbl)(double),  /* I [fnc_dbl] Pointer to double function */
 float (*fnc_flt)(float));    /* I [fnc_flt] Pointer to float  function */

var_sct *   /* O [sct] Remainder of modulo operation of input variables (var_1%var_2) */
ncap_var_var_mod /* [fnc] Remainder (modulo) operation of two variables */
(var_sct *var_1, /* I [sc,t] Variable structure containing field */
 var_sct *var_2); /* I [sct] Variable structure containing divisor */

var_sct *         /* O [sct] Empowerment of input variables (var_1^var_2) */
ncap_var_var_pwr  /* [fnc] Empowerment of two variables */ 
(var_sct *var_1,  /* I [sct] Variable structure containing base */
 var_sct *var_2); /* I [sct] Variable structure containing exponent */

var_sct *           /* O [sct] Resultant variable (actually is var_in) */
ncap_var_fnc(       /* Apply function to var */   
var_sct *var_in,    /* I/O [sng] input variable */ 
sym_sct *app);       /* I [fnc_ptr] to apply to variable */

var_sct *         /* O [sct] Resultant variable (actually is var) */
ncap_var_abs(     /* Purpose: Find absolute value of each element of var */
var_sct *var);    /* I/O [sct] input variable */

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

nco_bool
ncap_def_dim(
const char *dmn_nm,
long sz,
prs_sct *prs_arg);

nco_bool /* O [flg] Variables now conform */
ncap_var_stretch /* [fnc] Stretch variables */
(var_sct **var_1, /* I/O [ptr] First variable */
 var_sct **var_2); /* I/O [ptr] Second variable */

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
 prs_sct *prs_arg);

var_sct *             /* O [sct] Sum of input variables (var1+var2) INITIAL SCAN ONLY */
ncap_var_var_op_ntl   /* [fnc] Add two variables */
(var_sct *var1,       /* I [sct] Input variable structure containing first operand */
 var_sct *var2,       /* I [sct] Input variable structure containing second operand */
 int op);             /* Operation +-% */

bool            /* O [flg] true if all var elemenst are true */
ncap_var_lgcl   /* [fnc] calculate a aggregate bool value from a variable */
(var_sct* var);  /* I [sct] input variable */


var_sct*                           /* O [sct] casting variable has its own private dims */ 
ncap_cst_mk(                       /* [fnc] create casting var from a list of dims */
std::vector<std::string> &str_vtr,  /* I [sng] list of dimension subscripts */
prs_sct *prs_arg);

var_sct*
ncap_cst_do(
var_sct* var,
var_sct* var_cst,
bool bntlscn);

/* End funtions in ncap_utl.c */

/* Let function live here for now */

nco_bool         /* Returns True if shape of vars match (using cnt vectors */
nco_shp_chk(
var_sct* var1, 
var_sct* var2); 

nco_bool        /* Reurns True if var has attribute style name */
ncap_var_is_att( 
var_sct *var);


void            /* define variables captured on first parse */
ncap_def_ntl_scn(
prs_sct *prs_arg);

int            /* Sort expressions for MPI Optimization */  
ncap_mpi_srt(
RefAST ntr,
int icnt,
std::vector< std::vector<RefAST> > &all_ast_vtr); // Return a Vector of Vectors

NcapVector<dmn_sct*>                /* O [sct] list of new dims to limit over */ 
ncap_dmn_mtd(
var_sct *var,                       /*  [sct] create casting var from a list of dims */
std::vector<std::string> &str_vtr);  /* I [sng] list of dimension names */

/* Create a scalar variable of type, if bfill then malloc ptr_unn */
var_sct*
ncap_sclr_var_mk(
char* var_nm,
nc_type type,
bool bfll=false);

var_sct *
ncap_sclr_var_mk(
char *var_nm,
unsigned char cdt);


var_sct *
ncap_sclr_var_mk(
char *var_nm,
signed char bdt);

var_sct *
ncap_sclr_var_mk(
char *var_nm,
short sdt);

var_sct *
ncap_sclr_var_mk(
char *var_nm,
int idt);

var_sct *
ncap_sclr_var_mk(
char *var_nm,
long ldt);

var_sct *
ncap_sclr_var_mk(
char *var_nm,
float fdt);

var_sct *
ncap_sclr_var_mk(
char *var_nm,
double ddt);

 
var_sct*
nco_get_var_mem(
var_sct *var_rhs,
NcapVector<dmn_sct*> &dmn_vtr);

 
void
nco_put_var_mem(
var_sct *var_in,
var_sct *var_nw,
NcapVector<lmt_sct*> &lmt_vtr);




#endif /* NCAP_H */






