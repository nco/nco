/* $Header$ */

/* Purpose: NCO utilities for compression filters */

/* Copyright (C) 2020--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

/* Usage:
   ncks -O -4 -D 1 --flt='*,32004,1' ~/nco/data/in.nc ~/foo.nc
   ncks -C -m --hdn --cdl ~/foo.nc

   Unidata _Filter docs:
   https://www.unidata.ucar.edu/software/netcdf/docs/md__Users_wfisher_Desktop_docs_netcdf-c_docs_filters.html
   Use NC_parsefilterspec(), NC_filterfix8() demonstrated in nc_test4/tst_filterparser.c, to parse options */

#include "nco_flt.h" /* Compression filters */

const char * /* O [sng] Parsed Filter string */
nco_flt_prs /* [fnc] Parse user-provided filter string */
(const char * flt_sng) /* I [sng] Filter string */
{
  /* Purpose: Parse user-provided filter string */
  const char fnc_nm[]="nco_flt_prs()";

  size_t prm_idx; /* [idx] Parameter index in list */
  size_t prm_nbr=0L; /* [nbr] Number of filter parameters in list */

  unsigned int flt_id=0; /* [id] Filter ID */
  unsigned int *prm_lst=NULL; /* [sct] List of filter parameters */
  
  if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stderr,"%s: INFO Requested filter string = %s\n",nco_prg_nm_get(),flt_sng);

  /* Use netCDF-provided helper function to parse user-provided filter request 
     EXTERNL int NC_parsefilterspec(const char* txt, int format, NC_Filterspec** specp);
     int NC_parsefilterspec(const char* spec, unsigned int* idp, size_t* nparamsp, unsigned int** paramsp);
     idp will contain the filter id value from the spec.
     nparamsp will contain the number of 4-byte parameters
     paramsp will contain a pointer to the parsed parameters – the caller must free. This function can parse filter spec strings as defined in the section on Filter Specification Syntax. This function parses the first argument and returns several values. */
  // Apparently four-argument NC_parsefilterspec() documented but not supported in 4.7.4
  //NC_parsefilterspec(flt_sng,&flt_id,&prm_nbr,&prm_lst);
  // Old NC_parsefilterspec() supported in 4.7.4 passes structures so, just forget it
  //NC_parsefilterspec(flt_sng,int format, NC_Filterspec** specp);

  if(nco_dbg_lvl_get() >= nco_dbg_std){
    (void)fprintf(stderr,"%s: INFO %s reports parsed filter string: ID = %u, prm_nbr = %lu, ",nco_prg_nm_get(),fnc_nm,flt_id,prm_nbr);
    for(prm_idx=0;prm_idx<prm_nbr;prm_idx++){
      (void)fprintf(stdout,"prm_lst[%lu] = %u%s",prm_idx,prm_lst[prm_idx],prm_idx == prm_nbr ? ", " : "");
    } /* !prm_idx */
    (void)fprintf(stdout,"\n");
  } /* !dbg */
    
  nco_exit(EXIT_SUCCESS);

  return (char *)NULL;
} /* !nco_flt_prs() */
