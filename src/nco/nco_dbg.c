/* $Header$ */

/* Purpose: Debugging */

/* Copyright (C) 1995--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

#include "nco_dbg.h" /* Debugging */

void
err_prn /* [fnc] Print formatted error messages */
(char *err_msg) /* I [sng] Formatted error message to print */
{
  /* Purpose: Print error message (currently only to stdout) approximately in GNU style, i.e., "program_name: ERROR error message....\n". 
     Routine is intended to make error messages convenient to use in routines without routine itself needing access to copy of program. 
     Therefore a copy of this routine should be placed in each main.c file, just like the program's nco_usg_prn() routine. */

  /* Routine is more difficult to implement practically than it first seemed, because 
     it assumes all calling routines have their error messages in a single string, when
     in reality, they often utilize var_args capabilities of fprintf() themselves and
     have multiple formatted inputs. Hold off for now. */

  (void)fprintf(stdout,"%s: %s",nco_prg_nm_get(),err_msg);

} /* end err_prn() */
