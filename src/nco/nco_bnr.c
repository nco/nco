/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_bnr.c,v 1.1 2002-05-08 08:00:15 zender Exp $ */

/* Purpose: Binary write utilities */

/* Copyright (C) 1995--2002 Charlie Zender
   This software is distributed under the terms of the GNU General Public License
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

#include "nco_bnr.h" /* Binary write utilities */

FILE *fp_bnr /* O [fl] Unformatted binary output file handle */
nco_bnr_open /* [fnc] Open unformatted binary data file for writing */
(const char * const fl_bnr) /* [sng] Unformatted binary output file */
{
  /* Purpose: Open unformatted binary data file for writing */
  /* Open output file */
  if((fp_bnr=fopen(fl_bnr,"w")) == NULL){
    (void)fprintf(stderr,"%s: ERROR unable to open binary output file %s\n",prg_nm_get(),fl_bnr);
    nco_exit(EXIT_FAILURE);
  } /* end if */
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
  if(dbg_lvl_get() > 0) (void)fprintf(stderr,"%s: Closed binary file %s\n",prg_nm_get(),fl_bnr);
} /* end nco_bnr_close() */

size_t /* O [nbr] Number of elements successfully written */
nco_bnr_wrt /* [fnc] Write unformatted binary data */
(FILE * const fp_bnr, /* I [fl] Unformatted binary output file handle */
 const char * const fl_bnr, /* I [sng] Unformatted binary output file */
 const char * const var_nm, /* I [sng] Variable name */
 const long var_sz, /* I [nbr] Variable size */
 const nc_type var_typ, /* I [enm] Variable type */
 const void * const void_ptr) /* I [ptr] Data to write */
{
  /* Purpose: Write unformatted binary data */
  size_t wrt_nbr; /* [nbr] Number of elements successfully written */
  /* Write unformatted data to binary output file */
  wrt_nbr=fwrite(void_ptr,(size_t)nco_typ_lng(var_typ),(size_t)var_sz,fp_bnr);
  if(wrt_nbr != var_sz){
    (void)fprintf(stderr,"%s: ERROR only succeeded in writing %ld of %ld elements of variable %s\n",prg_nm_get(),wrt_nbr,var_sz,var_nm);
    nco_exit(EXIT_FAILURE);
  } /* end if */
  return wrt_nbr; /* O [nbr] Number of elements successfully written */
} /* end nco_bnr_wrt() */
