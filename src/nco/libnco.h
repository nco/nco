/* $Header: /data/zender/nco_20150216/nco/src/nco/libnco.h,v 1.2 2002-05-02 06:56:41 zender Exp $ */

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
#include "nco_fl_utl.h" *//* File manipulation */
#include "nco_pck.h" /* Packing and unpacking variables */

#endif /* LIBNCO_H */
