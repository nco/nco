/* $Header: /data/zender/nco_20150216/nco/src/nco/ncap.h,v 1.9 2001-10-01 23:09:51 zender Exp $ */

/* Header file for netCDF arithmetic processor */

/* Copyright (C) 1995--2001 Charlie Zender

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

#define SYM_NBR_MAX 20 /* maximum number of symbols (should make table a linked list) */

/* Define symbol table */
/* YACC seems to initialize all user-defined pointers (like those in symbol table) to NULL
   Symbol type may, therefore, safely be determined by testing for non-NULL members
   This technique is frequently used in Pigeon book */
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
extern void nco_lib_vrs_prn();

#endif /* NCAP_H */






