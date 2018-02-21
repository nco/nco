/* $Header$ */

/* Purpose: Binary file utilities */

/* Copyright (C) 1995--2018 Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   GNU General Public License (GPL) Version 3 with exceptions described in the LICENSE file */

#include "nco_bnr.h" /* Binary file utilities */

FILE * /* O [fl] Unformatted binary file handle */
nco_bnr_open /* [fnc] Open unformatted binary data file */
(const char * const fl_bnr, /* [sng] Unformatted binary file */
 const char * const fl_mode) /* [sng] Open-mode ("r", "w", ...) */
{
  /* Purpose: Open unformatted binary data file for writing */
  FILE *fp_bnr; /* [fl] Unformatted binary output file handle */
  /* Open output file */
  if((fp_bnr=fopen(fl_bnr,fl_mode)) == NULL){
    (void)fprintf(stderr,"%s: ERROR unable to open binary output file %s\n",nco_prg_nm_get(),fl_bnr);
    nco_exit(EXIT_FAILURE);
  } /* end if */
  if(nco_dbg_lvl_get() >= nco_dbg_fl) (void)fprintf(stdout,"%s: Opened binary file %s\n",nco_prg_nm_get(),fl_bnr);
  if(nco_dbg_lvl_get() >= nco_dbg_scl) (void)fprintf(stdout,"Variable(s): Name (native type, # elements x bytes per element):\n");
  return fp_bnr; /* O [fl] Unformatted binary output file handle */
} /* end nco_bnr_open() */

int /* [rcd] Return code */
nco_bnr_close /* [fnc] Close unformatted binary data file for writing */
(FILE *fp_bnr, /* I [fl] Unformatted binary output file handle */
 const char * const fl_bnr) /* [sng] Unformatted binary output file */
{
  /* Purpose: Close unformatted binary data file for writing */
  int rcd; /* [rcd] Return code */
  /* Close output file */
  rcd=fclose(fp_bnr);
  if(rcd != 0){
    (void)fprintf(stderr,"%s: ERROR unable to close binary output file %s\n",nco_prg_nm_get(),fl_bnr);
    nco_exit(EXIT_FAILURE);
  } /* end if */
  if(nco_dbg_lvl_get() >= nco_dbg_scl) (void)fprintf(stdout,"\n");
  if(nco_dbg_lvl_get() >= nco_dbg_fl) (void)fprintf(stdout,"%s: Closed binary file %s\n",nco_prg_nm_get(),fl_bnr);
  return rcd; /* O [rcd] Return code */
} /* end nco_bnr_close() */

size_t /* O [nbr] Number of elements successfully written */
nco_bnr_wrt /* [fnc] Write unformatted binary data */
(FILE * const fp_bnr, /* I [fl] Unformatted binary output file handle */
 const char * const var_nm, /* I [sng] Variable name */
 const long var_sz, /* I [nbr] Variable size */
 const nc_type var_typ, /* I [enm] Variable type */
 const void * const void_ptr) /* I [ptr] Data to write */
{
  /* Purpose: Write unformatted binary data */
  /* Testing:
     ncks -O -D 3 -B -b foo.bnr ~/nco/data/in.nc ~/foo.nc */

  long wrt_nbr; /* [nbr] Number of elements successfully written */
  /* Write unformatted data to binary output file */
  wrt_nbr=fwrite(void_ptr,(size_t)nco_typ_lng(var_typ),(size_t)var_sz,fp_bnr);
  if(wrt_nbr != var_sz){
    (void)fprintf(stderr,"%s: ERROR only succeeded in writing %ld of %ld elements of variable %s\n",nco_prg_nm_get(),wrt_nbr,var_sz,var_nm);
    nco_exit(EXIT_FAILURE);
  } /* end if */
  if(nco_dbg_lvl_get() >= nco_dbg_scl) (void)fprintf(stdout,"%s (%s, %ld x %lu b), ",var_nm,c_typ_nm(var_typ),var_sz,(unsigned long)nco_typ_lng(var_typ));
  if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fflush(stderr);
  return wrt_nbr; /* O [nbr] Number of elements successfully written */
} /* end nco_bnr_wrt() */

size_t /* O [nbr] Number of elements successfully read */
nco_bnr_rd /* [fnc] Read unformatted binary data */
(FILE * const fp_bnr, /* I [fl] Unformatted binary input file handle */
 const char * const var_nm, /* I [sng] Variable name */
 const long var_sz, /* I [nbr] Variable size */
 const nc_type var_typ, /* I [enm] Variable type */
 void * const void_ptr) /* O [ptr] Data to read */
{
  /* Purpose: Read unformatted binary data */
  /* Testing:
     ncks -O -D 73 ~/nco/data/in.nc ~/foo.nc */

  long rd_nbr; /* [nbr] Number of elements successfully written */
  /* Read unformatted data from binary input file */
  rd_nbr=fread(void_ptr,(size_t)nco_typ_lng(var_typ),(size_t)var_sz,fp_bnr);
  if(rd_nbr != var_sz){
    (void)fprintf(stderr,"%s: ERROR only succeeded in reading %ld of %ld elements into variable %s\n",nco_prg_nm_get(),rd_nbr,var_sz,var_nm);
    nco_exit(EXIT_FAILURE);
  } /* end if */
  if(nco_dbg_lvl_get() >= nco_dbg_scl) (void)fprintf(stdout,"Binary read of %s (%s, %ld x %lu B)",var_nm,c_typ_nm(var_typ),var_sz,(unsigned long)nco_typ_lng(var_typ));
  if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fflush(stderr);
  return rd_nbr; /* O [nbr] Number of elements successfully written */
} /* end nco_bnr_rd() */
