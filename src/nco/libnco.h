/* $Header: /data/zender/nco_20150216/nco/src/nco/libnco.h,v 1.56 2013-07-16 18:39:43 zender Exp $ */

/* Purpose: netCDF Operator (NCO) library */

/* Copyright (C) 1995--2013 Charlie Zender
   License: GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */

/* libnco.h headers depend on netcdf.h and nco_netcdf.h headers */

/* Usage:
   #include "libnco.h" *//* netCDF Operator (NCO) library */

#ifndef LIBNCO_H /* Contents have not yet been inserted in current source file */
#define LIBNCO_H

/* Core headers */
#include "nco_netcdf.h" /* NCO wrappers for netCDF C library */
#include "nco.h" /* netCDF Operator (NCO) definitions */

/* Library headers */
#include "nco_att_utl.h" /* Attribute utilities */
#include "nco_aux.h" /* Auxiliary coordinates */
#include "nco_bnr.h" /* Binary write utilities */
#include "nco_cln_utl.h" /* Calendar utilities */
#include "nco_cnf_dmn.h" /* Conform dimensions */
#include "nco_cnf_typ.h" /* Conform variable types */
#include "nco_cnk.h" /* Chunking */
#include "nco_cnv_arm.h" /* ARM conventions */
#include "nco_cnv_csm.h" /* CCM/CCSM/CF conventions */
#include "nco_ctl.h" /* Program flow control functions */
#include "nco_dbg.h" /* Debugging */
#include "nco_dmn_utl.h" /* Dimension utilities */
#include "nco_fl_utl.h" /* File manipulation */
#include "nco_grp_trv.h" /* Group traversal */
#include "nco_grp_utl.h" /* Group utilities */
#include "nco_lmt.h" /* Hyperslab limits */
#include "nco_lst_utl.h" /* List utilities */
#include "nco_md5.h" /* MD5 digests */
#include "nco_mmr.h" /* Memory management */
#include "nco_msa.h" /* Multi-slabbing algorithm */
#include "nco_mss_val.h" /* Missing value utilities */
#include "nco_omp.h" /* OpenMP utilities */
#include "nco_pck.h" /* Packing and unpacking variables */
#include "nco_prn.h" /* Print variables, attributes, metadata */
#include "nco_rec_var.h" /* Record variable utilities */
#include "nco_rth_flt.h" /* Float-precision arithmetic, MSVC macros */
#include "nco_rth_utl.h" /* Arithmetic controls and utilities */
#include "nco_scl_utl.h" /* Scalar utilities */
#include "nco_scm.h" /* Software configuration management */
#include "nco_sng_utl.h" /* String utilities */
#include "nco_var_avg.h" /* Average variables */
#include "nco_var_lst.h" /* Variable list utilities */
#include "nco_var_rth.h" /* Variable arithmetic */
#include "nco_var_scv.h" /* Arithmetic between variables and scalar values */
#include "nco_var_utl.h" /* Variable utilities */

#endif /* LIBNCO_H */
