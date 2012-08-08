/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_grp_trav.c,v 1.1 2012-08-08 07:51:44 pvicente Exp $ */

/* Purpose: netCDF4 traversal storage */

/* Copyright (C) 2011--2012 Charlie Zender/Pedro Vicente
   License: GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */


/* Testing:
   ncks -D 1 ~/nco/data/in_grp.nc
 */

#include "nco_grp_trav.h" /* Group traversal storage */
#include <assert.h>


int                          /* O   [rcd] Return code */
trav_table_init
(trav_table_t **table)       /* I/O [sct] table */
{

  return 0;
}
