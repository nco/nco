/* $Header: /data/zender/nco_20150216/nco/src/nco/ncap.h,v 1.2 1998-08-19 04:27:26 zender Exp $ */

/* Header file for netCDF arithmetic processor */ 

/* (c) Copyright 1995--1998University Corporation for Atmospheric Research/
   National Center for Atmospheric Research/
   Climate and Global Dynamics Division

   The file LICENSE contains the full copyright notice, or 
   you may contact NSF/UCAR/NCAR/CGD/CMS for copyright assistance. */

#ifndef NCAP_H /* This include file has not yet been defined in the current source file */ 

#define SYM_NBR_MAX 20 /* maximum number of symbols (should make table a linked list) */ 

/* Define a symbol table */ 
/* NB: YACC seems to initialize all user-defined pointers (like those in the symbol table) to NULL. 
   A symbol's type may, therefore, safely be determined by testing for non-NULL members.
   This technique is frequently used in the pigeon book. */
typedef struct {
  char *nm;
  var_sct *var;
  double (*fnc)();
  double val;
} sym_sct;
sym_sct sym_tbl[SYM_NBR_MAX];

/* These funtions are kept either in ncap.y or ncap_utl.c */ 
extern sym_sct *sym_look(char *sym_nm);
extern void fnc_add(char *nm, double (*fnc)());
extern sym_sct *scalar_mk_sym(double val);
extern var_sct *ncap_var_add(var_sct *var_1,var_sct *var_2);
extern int ncap_write_var(int,var_sct *);
extern int yyerror(char *sng);
extern void nc_lib_vrs_prn();

#endif /* NCAP_H */ 







