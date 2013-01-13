/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_bnr.c,v 1.23 2013-01-13 06:07:47 zender Exp $ */

/* Purpose: Binary write utilities */

/* Copyright (C) 1995--2013 Charlie Zender
   License: GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */

#include "nco_bnr.h" /* Binary write utilities */

FILE * /* O [fl] Unformatted binary output file handle */
nco_bnr_open /* [fnc] Open unformatted binary data file for writing */
(const char * const fl_bnr) /* [sng] Unformatted binary output file */
{
  /* Purpose: Open unformatted binary data file for writing */
  FILE *fp_bnr; /* [fl] Unformatted binary output file handle */
  /* Open output file */
  if((fp_bnr=fopen(fl_bnr,"w")) == NULL){
    (void)fprintf(stderr,"%s: ERROR unable to open binary output file %s\n",prg_nm_get(),fl_bnr);
    nco_exit(EXIT_FAILURE);
  } /* end if */
  if(dbg_lvl_get() > 1) (void)fprintf(stdout,"%s: Opened binary file %s\n",prg_nm_get(),fl_bnr);
  if(dbg_lvl_get() > 2) (void)fprintf(stdout,"Variable(s): Name (native type, # elements x bytes per element):\n");
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
    (void)fprintf(stderr,"%s: ERROR unable to close binary output file %s\n",prg_nm_get(),fl_bnr);
    nco_exit(EXIT_FAILURE);
  } /* end if */
  if(dbg_lvl_get() > 2) (void)fprintf(stdout,"\n");
  if(dbg_lvl_get() > 1) (void)fprintf(stdout,"%s: Closed binary file %s\n",prg_nm_get(),fl_bnr);
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
     ncks -O -D 3 -B -b foo.bnr ~/nco/data/in.nc ~/nco/data/foo.nc */

  long wrt_nbr; /* [nbr] Number of elements successfully written */
  /* Write unformatted data to binary output file */
  wrt_nbr=fwrite(void_ptr,(size_t)nco_typ_lng(var_typ),(size_t)var_sz,fp_bnr);
  if(wrt_nbr != var_sz){
    (void)fprintf(stderr,"%s: ERROR only succeeded in writing %ld of %ld elements of variable %s\n",prg_nm_get(),wrt_nbr,var_sz,var_nm);
    nco_exit(EXIT_FAILURE);
  } /* end if */
  if(dbg_lvl_get() > 2) (void)fprintf(stdout,"%s (%s, %ld x %lu b), ",var_nm,c_typ_nm(var_typ),var_sz,(unsigned long)nco_typ_lng(var_typ));
  if(dbg_lvl_get() > 0) (void)fflush(stderr);
  return wrt_nbr; /* O [nbr] Number of elements successfully written */
} /* end nco_bnr_wrt() */
