/* $Header: /data/zender/nco_20150216/nco/src/nco/libnco.h,v 1.16 2002-05-05 21:21:14 zender Exp $ */

/* Purpose: Prototypes, typedefs, and global variables for libcsm_c++ */

/* Copyright (C) 1995--2002 Charlie Zender
   This software is distributed under the terms of the GNU General Public License
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

/* libnco.h headers depend on netcdf.h and nco_netcdf.h headers */

/* Usage:
   #include "libnco.h" *//* netCDF operator library */

#ifndef LIBNCO_H /* Contents have not yet been inserted in current source file */
#define LIBNCO_H

/* Core headers */
#include "nco.h" /* netCDF operator definitions */
#include "nco_netcdf.h" /* Wrappers for netCDF 3.X C-library */

/* Library headers */
#include "nco_att_utl.h" /* Attribute utilities */
#include "nco_cln_utl.h" /* Calendar utilities */
#include "nco_cnf_dmn.h" /* Conform dimensions */
#include "nco_cnf_typ.h" /* Conform variable types */
#include "nco_cnv_arm.h" /* ARM conventions */
#include "nco_cnv_csm.h" /* CSM conventions */
#include "nco_ctl.h" /* Program flow control functions */
#include "nco_dmn_utl.h" /* Dimension utilities */
#include "nco_fl_utl.h" /* File manipulation */
#include "nco_lmt.h" /* Hyperslab limits */
#include "nco_lst_utl.h" /* List utilities */
#include "nco_mmr.h" /* Memory management */
#include "nco_mss_val.h" /* Missing value utilities */
#include "nco_omp.h" /* OpenMP utilities */
#include "nco_pck.h" /* Packing and unpacking variables */
#include "nco_rec_var.h" /* Record variable utilities */
#include "nco_rth_utl.h" /* Arithmetic controls and utilities */
#include "nco_scl_utl.h" /* Scalar utilities */
#include "nco_scm.h" /* Software configuration management */
#include "nco_sng_utl.h" /* String utilities */
#include "nco_var_utl.h" /* Variable utilities */
#include "nco_var_avg.h" /* Average variables */
#include "nco_var_lst.h" /* Variable list utilities */

#endif /* LIBNCO_H */
