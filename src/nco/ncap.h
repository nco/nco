/* $Header: /data/zender/nco_20150216/nco/src/nco/ncap.h,v 1.23 2002-01-27 06:14:05 zender Exp $ */

/* Header file for netCDF arithmetic processor */

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

#ifndef NCAP_H /* Header file has not yet been defined in current source file */
#define NCAP_H

/* Define symbol table */
/* YACC seems to initialize all user-defined pointers (like those in symbol table) to NULL
   Symbol type may, therefore, safely be determined by testing for non-NULL members
   This technique is frequently used in Pigeon book */

typedef struct { /* sym_sct */
  char *nm;
  double (*fnc)(double);
  float (*fncf)(float);
} sym_sct;

typedef struct { /* parse_sct */
  val_unn val;
  nc_type type;
} parse_sct;      

typedef struct{ /* prs_sct */
  char *fl_in; /* [sng] Input data file */
  int in_id; /* [id] Input data file ID */
  char *fl_out; /* [sng] Output data file */
  int out_id; /* [id] Output data file ID */
  char *fl_spt; /* Instruction file to be parsed */
  aed_sct **att_lst;
  int *nbr_att;
  dmn_sct **dmn; /* [dmn] List of extracted dimensions */
  int nbr_dmn_xtr; /* [nbr] Number of extracted dimensions */
  sym_sct **sym_tbl;
  int sym_tbl_nbr;
  bool initial_scan;
} prs_sct;

/* These funtions are either in ncap.y or ncap_utl.c */
extern bool ncap_var_conform_dim(var_sct **,var_sct **);
extern bool ncap_var_stretch(var_sct **,var_sct **);
extern int ncap_aed_lookup(char *,char *,aed_sct **,int *,bool);
extern int ncap_attribute_conform_type(nc_type,parse_sct *);
extern int ncap_attribute_minus(parse_sct *);
extern int ncap_retype(parse_sct *,parse_sct *);
extern int ncap_var_retype(var_sct *,var_sct *);
extern int ncap_var_write(var_sct *,prs_sct*);
extern int yyerror(char *sng);
extern nm_id_sct *var_lst_copy(nm_id_sct *,int);
extern nm_id_sct *var_lst_add(int,nm_id_sct *,int *,nm_id_sct *,int);
extern nm_id_sct *var_lst_sub(int,nm_id_sct *,int *,nm_id_sct *,int);
extern nm_id_sct *ncap_var_lst_crd_make(int,nm_id_sct *,int *);
extern parse_sct ncap_attribute_abs(parse_sct);
extern parse_sct ncap_attribute_calc(parse_sct,char,parse_sct);
extern parse_sct ncap_ptr_unn_2_attribute(nc_type,ptr_unn);
extern ptr_unn ncap_attribute_2_ptr_unn(parse_sct); 
extern sym_sct *ncap_sym_init(char * ,double (*fnc)(double),float (*fncf)(float));
extern sym_sct *scalar_mk_sym(double val);
extern sym_sct *sym_look(char *sym_nm);
extern var_sct *ncap_var_abs(var_sct *);
extern var_sct *ncap_var_attribute_add(var_sct *,parse_sct);
extern var_sct *ncap_var_attribute_divide(var_sct *,parse_sct);
extern var_sct *ncap_var_attribute_modulus(var_sct *,parse_sct);
extern var_sct *ncap_var_attribute_multiply(var_sct *,parse_sct);
extern var_sct *ncap_var_attribute_power(var_sct *,parse_sct);
extern var_sct *ncap_var_attribute_sub(var_sct *,parse_sct);
extern var_sct *ncap_var_function(var_sct *,sym_sct *);
extern var_sct *ncap_var_init(char*,prs_sct *);
extern var_sct *ncap_var_var_add(var_sct *var_1,var_sct *var_2);
extern var_sct *ncap_var_var_multiply(var_sct *var_1,var_sct *var_2);
extern var_sct *ncap_var_var_divide(var_sct *var_1,var_sct *var_2);
extern var_sct *ncap_var_var_sub(var_sct *var_1,var_sct *var_2);
extern void fnc_add(char *nm, double (*fnc)());
extern void ncap_initial_scan(prs_sct*,char *,nm_id_sct**,int *,nm_id_sct**,int *,nm_id_sct**,int *);
extern void nco_lib_vrs_prn();
extern void var_abs(nc_type,long,int,ptr_unn,ptr_unn);
extern void var_attribute_add(nc_type,long,int,ptr_unn,ptr_unn,parse_sct *);
extern void var_attribute_divide(nc_type,long,int,ptr_unn,ptr_unn,parse_sct *);
extern void var_attribute_modulus(nc_type,long,int,ptr_unn,ptr_unn,parse_sct *);
extern void var_attribute_multiply(nc_type,long,int,ptr_unn,ptr_unn,parse_sct *);

/* Source code for following functions is generated by parser but parser
   does not do good job of making these prototypes known so routines that
   call these functions must either include their prototypes locally,
   which is bad form if it needs to be done in more than one location,
   or prototype the parser routines here */

#endif /* NCAP_H */










