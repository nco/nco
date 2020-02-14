/* $Header$ */

/* Purpose: NCO utilities for packing and unpacking variables */

/* Copyright (C) 1995--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

#include "nco_pck.h" /* Packing and unpacking variables */

/* Notes on packing/unpacking:
   Routines in this file must be used in correct order:
   nco_pck_dsk_inq(): called first, e.g., in nco_var_fll(), before var_prc copied to var_prc_out
   nco_var_upk(): called in data retrieval routine, e.g., in nco_var_get()
   nco_var_pck(): called just before writing output file, e.g., in main()
   Bookkeeping hassle is keeping flags in var_prc synchronized with flags in var_prc_out

   From netCDF Users Guide:
   scale_factor: If present for a variable, the data are to be multiplied by this factor after the data are read by the application that accesses the data
   add_offset: If present for a variable, this number is added to the data after the data are read by the application. If both scale_factor and add_offset attributes are present, the data are first scaled before the offset is added. 
   When scale_factor and add_offset are used for packing, the associated variable (containing the packed data) is typically of type byte or short, whereas the unpacked values are intended to be of type float or double. Attribute's scale_factor and add_offset should both be of type intended for the unpacked data, e.g., float or double.

   20101007 Dave Allured points out that Unidata covers this in NetCDF Best Practices: 
   http://www.unidata.ucar.edu/software/netcdf/docs/BestPractices.html#Missing%20Data%20Values 
   "The _FillValue attribute should have the same data type as the variable it describes. If the variable is packed using scale_factor and add_offset attributes, the _FillValue attribute should have the data type of the packed data." 

   CF conventions say about the same thing. If there is a mismatch between the attribute type and the data type, or if an incorrect value was stored in the _FillValue attribute, then manual intervention is needed. However note that storing -999 in an attribute of type short vs. integer is not much of a problem, because the two are are numerically identical. The important question in this case is whether ncra has an automatic method to exclude missing values from being transformed by averaging, in the face of a packed variable. If so, how is it done, or where in the documentation is it described. */

const char * /* O [sng] Packing map string */
nco_pck_map_sng_get /* [fnc] Convert packing map enum to string */
(const int nco_pck_map) /* I [enm] Packing map */
{
  /* Purpose: Convert packing map enum to string */
  switch(nco_pck_map){
  case nco_pck_map_nil:
    return "nil";
  case nco_pck_map_hgh_sht:
    return "hgh_sht";
  case nco_pck_map_hgh_chr:
    return "hgh_chr";
  case nco_pck_map_hgh_byt:
    return "hgh_byt";
  case nco_pck_map_nxt_lsr:
    return "nxt_lsr";
  case nco_pck_map_flt_sht:
    return "flt_sht";
  case nco_pck_map_flt_chr:
    return "flt_chr";
  case nco_pck_map_flt_byt:
    return "flt_byt";
  case nco_pck_map_dbl_flt:
    return "dbl_flt";
  case nco_pck_map_flt_dbl:
    return "flt_dbl";
  default: nco_dfl_case_pck_map_err(); break;
  } /* end switch */
  /* Some compilers, e.g., SGI cc, need return statement to end non-void functions */
  return (char *)NULL;
} /* end nco_pck_map_sng_get() */

const char * /* O [sng] Packing policy string */
nco_pck_plc_sng_get /* [fnc] Convert packing policy enum to string */
(const int nco_pck_plc) /* I [enm] Packing policy */
{
  /* Purpose: Convert packing policy enum to string */
  switch(nco_pck_plc){
  case nco_pck_plc_nil:
    return "nil";
  case nco_pck_plc_all_xst_att: 
    return "all_xst";
  case nco_pck_plc_all_new_att: 
    return "all_new";
  case nco_pck_plc_xst_new_att: 
    return "xst_new";
  case nco_pck_plc_upk:
    return "upk";
  default: nco_dfl_case_pck_plc_err(); break;
  } /* end switch */
  /* Some compilers, e.g., SGI cc, need return statement to end non-void functions */
  return (char *)NULL;
} /* end nco_pck_plc_sng_get() */

void 
nco_dfl_case_pck_map_err(void) /* [fnc] Print error and exit for illegal switch(pck_map) case */
{
  /* Purpose: Convenience routine for printing error and exiting when
     switch(pck_map) statement receives an illegal default case

     Placing this in its own routine saves many lines 
     of code since this function is used in many many switch() statements. */
  const char fnc_nm[]="nco_dfl_case_pck_map_err()";
  (void)fprintf(stdout,"%s: ERROR switch(pck_map) statement fell through to default case, which is unsafe. This catch-all error handler ensures all switch(pck_map) statements are fully enumerated. Exiting...\n",fnc_nm);
  nco_err_exit(0,fnc_nm);
} /* end nco_dfl_case_pck_map_err() */

void 
nco_dfl_case_pck_plc_err(void) /* [fnc] Print error and exit for illegal switch(pck_plc) case */
{
  /* Purpose: Convenience routine for printing error and exiting when
     switch(pck_plc) statement receives an illegal default case

     Placing this in its own routine saves many lines 
     of code since this function is used in many many switch() statements. */
  const char fnc_nm[]="nco_dfl_case_pck_plc_err()";
  (void)fprintf(stdout,"%s: ERROR switch(pck_plc) statement fell through to default case, which is unsafe. This catch-all error handler ensures all switch(pck_plc) statements are fully enumerated. Exiting...\n",fnc_nm);
  nco_err_exit(0,fnc_nm);
} /* end nco_dfl_case_pck_plc_err() */

nco_bool /* O [flg] NCO will attempt to pack variable */
nco_is_packable /* [fnc] Will NCO attempt to pack variable? */
(const nc_type nc_typ_in) /* I [enm] Type of input variable */
{
  /* Purpose: Determine whether NCO should attempt to pack a given type
     Packing certain variable types is not recommended, e.g., packing NC_CHAR
     and NC_BYTE makes no sense, because precision would needlessly be lost.
     Routine should be consistent with nco_pck_plc_typ_get()
     NB: Routine is deprecated in favor of more flexible nco_pck_plc_typ_get() */
  const char fnc_nm[]="nco_is_packable()"; /* [sng] Function name */

  (void)fprintf(stdout,"%s: ERROR deprecated routine %s should not be called\n",nco_prg_nm_get(),fnc_nm);
  nco_exit(EXIT_FAILURE);

  switch(nc_typ_in){ 
  case NC_FLOAT: 
  case NC_DOUBLE: 
  case NC_INT64: 
  case NC_UINT64: 
  case NC_INT: 
  case NC_UINT: 
    return True;
    break;
  case NC_SHORT: 
  case NC_USHORT: 
  case NC_CHAR: 
  case NC_BYTE: 
  case NC_UBYTE: 
  case NC_STRING:
    return False;
    break;
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */ 

  /* Some compilers, e.g., SGI cc, need return statement to end non-void functions */
  return False;
} /* end nco_is_packable() */

int /* O [enm] Packing map */
nco_pck_map_get /* [fnc] Convert user-specified packing map to key */
(const char *nco_pck_map_sng) /* [sng] User-specified packing map */
{
  /* Purpose: Process ncpdq '-P' command line argument
     Convert user-specified string to packing map
     Return nco_pck_map_nil by default */
  const char fnc_nm[]="nco_pck_map_get()"; /* [sng] Function name */
  char *nco_prg_nm; /* [sng] Program name */
  nco_prg_nm=nco_prg_nm_get(); /* [sng] Program name */

  if(nco_pck_map_sng == NULL){ 
    (void)fprintf(stderr,"%s: ERROR %s reports empty user-specified packing map string %s\n",nco_prg_nm,fnc_nm,nco_pck_map_sng);
    nco_exit(EXIT_FAILURE);
  } /* endif */

  if(!strcmp(nco_pck_map_sng,"hgh_sht")) return nco_pck_map_hgh_sht;
  if(!strcmp(nco_pck_map_sng,"pck_map_hgh_sht")) return nco_pck_map_hgh_sht;
  if(!strcmp(nco_pck_map_sng,"hgh_chr")) return nco_pck_map_hgh_chr;
  if(!strcmp(nco_pck_map_sng,"pck_map_hgh_chr")) return nco_pck_map_hgh_chr;
  if(!strcmp(nco_pck_map_sng,"hgh_byt")) return nco_pck_map_hgh_byt;
  if(!strcmp(nco_pck_map_sng,"pck_map_hgh_byt")) return nco_pck_map_hgh_byt;
  if(!strcmp(nco_pck_map_sng,"nxt_lsr")) return nco_pck_map_nxt_lsr;
  if(!strcmp(nco_pck_map_sng,"pck_map_nxt_lsr")) return nco_pck_map_nxt_lsr;
  if(!strcmp(nco_pck_map_sng,"flt_sht")) return nco_pck_map_flt_sht;
  if(!strcmp(nco_pck_map_sng,"pck_map_flt_sht")) return nco_pck_map_flt_sht;
  if(!strcmp(nco_pck_map_sng,"flt_chr")) return nco_pck_map_flt_chr;
  if(!strcmp(nco_pck_map_sng,"pck_map_flt_chr")) return nco_pck_map_flt_chr;
  if(!strcmp(nco_pck_map_sng,"flt_byt")) return nco_pck_map_flt_byt;
  if(!strcmp(nco_pck_map_sng,"pck_map_flt_byt")) return nco_pck_map_flt_byt;
  if(!strcmp(nco_pck_map_sng,"dbl_flt")) return nco_pck_map_dbl_flt;
  if(!strcmp(nco_pck_map_sng,"pck_map_dbl_flt")) return nco_pck_map_dbl_flt;
  if(!strcmp(nco_pck_map_sng,"dbl_sgl")) return nco_pck_map_dbl_flt;
  if(!strcmp(nco_pck_map_sng,"pck_map_dbl_sgl")) return nco_pck_map_dbl_flt;
  if(!strcmp(nco_pck_map_sng,"flt_dbl")) return nco_pck_map_flt_dbl;
  if(!strcmp(nco_pck_map_sng,"pck_map_flt_dbl")) return nco_pck_map_flt_dbl;
  if(!strcmp(nco_pck_map_sng,"sgl_dbl")) return nco_pck_map_flt_dbl;
  if(!strcmp(nco_pck_map_sng,"pck_map_sgl_dbl")) return nco_pck_map_flt_dbl;

  (void)fprintf(stderr,"%s: ERROR %s reports unknown user-specified packing map %s\n",nco_prg_nm_get(),fnc_nm,nco_pck_map_sng);
  nco_exit(EXIT_FAILURE);
  return nco_pck_map_nil; /* Statement should not be reached */
} /* end nco_pck_map_get() */

int /* O [enm] Packing policy */
nco_pck_plc_get /* [fnc] Convert user-specified packing policy to key */
(const char *nco_pck_plc_sng) /* [sng] User-specified packing policy */
{
  /* Purpose: Process ncpdq '-P' command line argument
     Convert user-specified string to packing operation type 
     Return nco_pck_plc_nil by default */
  const char fnc_nm[]="nco_pck_plc_get()"; /* [sng] Function name */
  char *nco_prg_nm; /* [sng] Program name */
  nco_prg_nm=nco_prg_nm_get(); /* [sng] Program name */

  if(nco_pck_plc_sng == NULL){
    if(strstr(nco_prg_nm,"ncpdq")){
      if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stdout,"%s: INFO %s reports %s invoked without explicit packing or dimension permutation options. Defaulting to packing policy \"all_new\".\n",nco_prg_nm,fnc_nm,nco_prg_nm);
      return nco_pck_plc_all_new_att;
    } /* endif */
    if(strstr(nco_prg_nm,"ncpack")) return nco_pck_plc_all_new_att;
    if(strstr(nco_prg_nm,"ncunpack")) return nco_pck_plc_upk;
    (void)fprintf(stderr,"%s: ERROR %s reports empty user-specified packing string in conjunction with unknown or ambiguous executable name %s\n",nco_prg_nm,fnc_nm,nco_prg_nm);
    nco_exit(EXIT_FAILURE);
  } /* endif */

  if(!strcmp(nco_pck_plc_sng,"all_xst")) return nco_pck_plc_all_xst_att;
  if(!strcmp(nco_pck_plc_sng,"pck_all_xst_att")) return nco_pck_plc_all_xst_att;
  if(!strcmp(nco_pck_plc_sng,"all_new")) return nco_pck_plc_all_new_att;
  if(!strcmp(nco_pck_plc_sng,"pck_all_new_att")) return nco_pck_plc_all_new_att;
  if(!strcmp(nco_pck_plc_sng,"xst_new")) return nco_pck_plc_xst_new_att;
  if(!strcmp(nco_pck_plc_sng,"pck_xst_new_att")) return nco_pck_plc_xst_new_att;
  if(!strcmp(nco_pck_plc_sng,"upk")) return nco_pck_plc_upk;
  if(!strcmp(nco_pck_plc_sng,"unpack")) return nco_pck_plc_upk;
  if(!strcmp(nco_pck_plc_sng,"pck_upk")) return nco_pck_plc_upk;

  (void)fprintf(stderr,"%s: ERROR %s reports unknown user-specified packing policy %s\n",nco_prg_nm_get(),fnc_nm,nco_pck_plc_sng);
  nco_exit(EXIT_FAILURE);
  return nco_pck_plc_nil; /* Statement should not be reached */
} /* end nco_pck_plc_get() */

nco_bool /* O [flg] Packing policy allows packing nc_typ_in */
nco_pck_plc_typ_get /* [fnc] Determine type, if any, to pack input type to */
(const int nco_pck_map,  /* I [enm] Packing map */
 const nc_type nc_typ_in, /* I [enm] Type of input variable */
 nc_type *nc_typ_pck_out) /* O [enm] Type to pack variable to */
{
  /* Purpose: Determine type, if any, to pack input type to
     Routine enforces policy specified by nco_pck_map
     Replacement for simple deprecated routine nco_is_packable()
     There are two cases:
     1. nco_pck_map allows packing nc_typ_in:
        Routine returns true and sets nc_typ_pck_out accordingly
     2. nco_pck_map does not allow packing nc_typ_in:
        Routine returns false and sets nc_typ_pck_out=nc_typ_in
     In both cases, nc_typ_pck_out is only set if it is non-NULL */

  const char fnc_nm[]="nco_pck_plc_typ_get()"; /* [sng] Function name */
  nco_bool nco_pck_plc_alw; /* O [flg] Packing policy allows packing nc_typ_in */
  nc_type nc_typ_pck_out_tmp; /* O [enm] Type to pack variable to */

  /* Initialize output type to NAT and pack allow to False to help catch errors */
  nc_typ_pck_out_tmp=NC_NAT;
  nco_pck_plc_alw=False;
  switch(nco_pck_map){ 
  case nco_pck_map_nil:
    nc_typ_pck_out_tmp=nc_typ_in; nco_pck_plc_alw=False; break;
  case nco_pck_map_hgh_sht:
    switch(nc_typ_in){ 
    case NC_DOUBLE: 
    case NC_FLOAT: 
    case NC_INT64: 
    case NC_UINT64: 
    case NC_INT: 
    case NC_UINT: 
      nc_typ_pck_out_tmp=NC_SHORT; nco_pck_plc_alw=True; break;
    case NC_SHORT: 
    case NC_USHORT: 
    case NC_BYTE: 
    case NC_UBYTE: 
    case NC_CHAR: 
    case NC_STRING: 
      nc_typ_pck_out_tmp=nc_typ_in; nco_pck_plc_alw=False; break;
    default: nco_dfl_case_nc_type_err(); break;
    } /* end nc_type switch */ 
    break;
  case nco_pck_map_hgh_chr:
    switch(nc_typ_in){ 
    case NC_DOUBLE: 
    case NC_FLOAT: 
    case NC_INT64: 
    case NC_UINT64: 
    case NC_INT: 
    case NC_UINT: 
    case NC_SHORT: 
    case NC_USHORT: 
      nc_typ_pck_out_tmp=NC_CHAR; nco_pck_plc_alw=True; break;
    case NC_BYTE: 
    case NC_UBYTE: 
    case NC_CHAR: 
    case NC_STRING: 
      nc_typ_pck_out_tmp=nc_typ_in; nco_pck_plc_alw=False; break;
    default: nco_dfl_case_nc_type_err(); break;
    } /* end nc_type switch */ 
    break;
  case nco_pck_map_hgh_byt:
    switch(nc_typ_in){ 
    case NC_DOUBLE: 
    case NC_FLOAT: 
    case NC_INT64: 
    case NC_UINT64: 
    case NC_INT: 
    case NC_UINT: 
    case NC_SHORT: 
    case NC_USHORT: 
      nc_typ_pck_out_tmp=NC_BYTE; nco_pck_plc_alw=True; break;
    case NC_BYTE: 
    case NC_UBYTE: 
    case NC_CHAR: 
    case NC_STRING: 
      nc_typ_pck_out_tmp=nc_typ_in; nco_pck_plc_alw=False; break;
    default: nco_dfl_case_nc_type_err(); break;
    } /* end nc_type switch */ 
    break;
  case nco_pck_map_nxt_lsr:
    switch(nc_typ_in){ 
    case NC_DOUBLE: 
    case NC_INT64: 
    case NC_UINT64: 
      nc_typ_pck_out_tmp=NC_INT; nco_pck_plc_alw=True; break; 
    case NC_FLOAT: 
    case NC_INT: 
    case NC_UINT: 
      nc_typ_pck_out_tmp=NC_SHORT; nco_pck_plc_alw=True; break;
    case NC_SHORT: 
    case NC_USHORT: 
      nc_typ_pck_out_tmp=NC_BYTE; nco_pck_plc_alw=True; break;
    case NC_BYTE: 
    case NC_UBYTE: 
    case NC_CHAR: 
    case NC_STRING: 
      nc_typ_pck_out_tmp=nc_typ_in; nco_pck_plc_alw=False; break;
    default: nco_dfl_case_nc_type_err(); break;
    } /* end nc_type switch */ 
    break;
  case nco_pck_map_flt_sht:
    switch(nc_typ_in){ 
    case NC_DOUBLE: 
    case NC_FLOAT: 
      nc_typ_pck_out_tmp=NC_SHORT; nco_pck_plc_alw=True; break;
    case NC_INT64: 
    case NC_UINT64: 
    case NC_INT: 
    case NC_UINT: 
    case NC_SHORT: 
    case NC_USHORT: 
    case NC_BYTE: 
    case NC_UBYTE: 
    case NC_CHAR: 
    case NC_STRING: 
      nc_typ_pck_out_tmp=nc_typ_in; nco_pck_plc_alw=False; break;
    default: nco_dfl_case_nc_type_err(); break;
    } /* end nc_type switch */ 
    break;
  case nco_pck_map_flt_chr:
    switch(nc_typ_in){ 
    case NC_DOUBLE: 
    case NC_FLOAT: 
      nc_typ_pck_out_tmp=NC_CHAR; nco_pck_plc_alw=True; break;
    case NC_INT64: 
    case NC_UINT64: 
    case NC_INT: 
    case NC_UINT: 
    case NC_SHORT: 
    case NC_USHORT: 
    case NC_BYTE: 
    case NC_UBYTE: 
    case NC_CHAR: 
    case NC_STRING: 
      nc_typ_pck_out_tmp=nc_typ_in; nco_pck_plc_alw=False; break;
    default: nco_dfl_case_nc_type_err(); break;
    } /* end nc_type switch */ 
    break;
  case nco_pck_map_flt_byt:
    switch(nc_typ_in){ 
    case NC_DOUBLE: 
    case NC_FLOAT: 
      nc_typ_pck_out_tmp=NC_BYTE; nco_pck_plc_alw=True; break;
    case NC_INT64: 
    case NC_UINT64: 
    case NC_INT: 
    case NC_UINT: 
    case NC_SHORT: 
    case NC_USHORT: 
    case NC_BYTE: 
    case NC_UBYTE: 
    case NC_CHAR: 
    case NC_STRING: 
      nc_typ_pck_out_tmp=nc_typ_in; nco_pck_plc_alw=False; break;
    default: nco_dfl_case_nc_type_err(); break;
    } /* end nc_type switch */ 
    break;
  case nco_pck_map_dbl_flt:
    switch(nc_typ_in){ 
    case NC_DOUBLE: 
      nc_typ_pck_out_tmp=NC_FLOAT; nco_pck_plc_alw=True; break;
    case NC_FLOAT: 
    case NC_INT64: 
    case NC_UINT64: 
    case NC_INT: 
    case NC_UINT: 
    case NC_SHORT: 
    case NC_USHORT: 
    case NC_BYTE: 
    case NC_UBYTE: 
    case NC_CHAR: 
    case NC_STRING: 
      nc_typ_pck_out_tmp=nc_typ_in; nco_pck_plc_alw=False; break;
    default: nco_dfl_case_nc_type_err(); break;
    } /* end nc_type switch */ 
    break;
  case nco_pck_map_flt_dbl:
    switch(nc_typ_in){ 
    case NC_FLOAT: 
      nc_typ_pck_out_tmp=NC_DOUBLE; nco_pck_plc_alw=True; break;
    case NC_DOUBLE: 
    case NC_INT64: 
    case NC_UINT64: 
    case NC_INT: 
    case NC_UINT: 
    case NC_SHORT: 
    case NC_USHORT: 
    case NC_BYTE: 
    case NC_UBYTE: 
    case NC_CHAR: 
    case NC_STRING: 
      nc_typ_pck_out_tmp=nc_typ_in; nco_pck_plc_alw=False; break;
    default: nco_dfl_case_nc_type_err(); break;
    } /* end nc_type switch */ 
    break;
  default: 
    (void)fprintf(stdout,"%s: ERROR %s reports switch(nco_pck_map) statement fell through to default case\n",nco_prg_nm_get(),fnc_nm);
    nco_err_exit(0,fnc_nm);
    break;
  } /* end nco_pck_map switch */ 
  
  /* Only fill-in nc_typ_pck_out if it is non-NULL */
  if(nc_typ_pck_out) *nc_typ_pck_out=nc_typ_pck_out_tmp;
  
  return nco_pck_plc_alw; /* O [flg] Packing policy allows packing nc_typ_in */
} /* end nco_pck_plc_typ_get() */

nco_bool /* O [flg] Variable is packed on disk */
nco_pck_dsk_inq /* [fnc] Check whether variable is packed on disk */
(const int nc_id, /* I [idx] netCDF file ID */
 var_sct * const var) /* I/O [sct] Variable */
{
  /* Purpose: Check whether variable is packed on disk and set variable members 
     pck_dsk, has_scl_fct, has_add_fst, and typ_upk accordingly
     nco_pck_dsk_inq() should be called early in application, e.g., in nco_var_fll() 
     Call nco_pck_dsk_inq() before copying input list to output list 
     Multi-file operators which handle packing must call this routine prior
     to each read of a variable, in case that variable has been unpacked.
     NB: See also nco_inq_var_packing(), a light-overhead alternative to nco_pck_dsk_inq() */
  /* nces -O -D 3 -v pck ~/nco/data/in.nc ~/nco/data/foo.nc */
  
  const char add_fst_sng[]="add_offset"; /* [sng] Unidata standard string for add offset */
  const char scl_fct_sng[]="scale_factor"; /* [sng] Unidata standard string for scale factor */
  
  int rcd; /* [rcd] Return success code */
  
  long add_fst_lng; /* [idx] Number of elements in add_offset attribute */
  long scl_fct_lng; /* [idx] Number of elements in scale_factor attribute */

  nc_type add_fst_typ; /* [idx] Type of add_offset attribute */
  nc_type scl_fct_typ; /* [idx] Type of scale_factor attribute */

  /* Set some defaults in variable structure for safety in case of early return
     Flags for variables without valid scaling information should appear 
     same as flags for variables with _no_ scaling information
     Set has_scl_fct, has_add_fst in var_dfl_set()
     typ_upk:
     1. is required by ncra nco_cnv_mss_val_typ() 
     2. depends on var->type and so should not be set in var_dfl_set()
     3. is therefore set to default here */
  var->typ_upk=var->type; /* [enm] Type of variable when unpacked (expanded) (in memory) */

  /* Vet scale_factor */
  rcd=nco_inq_att_flg(nc_id,var->id,scl_fct_sng,&scl_fct_typ,&scl_fct_lng);
  if(rcd != NC_ENOTATT){
    if(scl_fct_typ == NC_BYTE || scl_fct_typ == NC_CHAR){
      if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stdout,"%s: WARNING nco_pck_dsk_inq() reports scale_factor for %s is NC_BYTE or NC_CHAR. Will not attempt to unpack using scale_factor.\n",nco_prg_nm_get(),var->nm); 
      return False;
    } /* endif */
    if(scl_fct_lng != 1){
      if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stdout,"%s: WARNING nco_pck_dsk_inq() reports %s has scale_factor of length %li. Will not attempt to unpack using scale_factor\n",nco_prg_nm_get(),var->nm,scl_fct_lng); 
      return False;
    } /* endif */
    var->has_scl_fct=True; /* [flg] Valid scale_factor attribute exists */
    var->typ_upk=scl_fct_typ; /* [enm] Type of variable when unpacked (expanded) (in memory) */
  } /* endif */

  /* Vet add_offset */
  rcd=nco_inq_att_flg(nc_id,var->id,add_fst_sng,&add_fst_typ,&add_fst_lng);
  if(rcd != NC_ENOTATT){
    if(add_fst_typ == NC_BYTE || add_fst_typ == NC_CHAR){
      if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stdout,"%s: WARNING nco_pck_dsk_inq() reports add_offset for %s is NC_BYTE or NC_CHAR. Will not attempt to unpack using add_offset.\n",nco_prg_nm_get(),var->nm); 
      return False;
    } /* endif */
    if(add_fst_lng != 1){
      if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stdout,"%s: WARNING nco_pck_dsk_inq() reports %s has add_offset of length %li. Will not attempt to unpack.\n",nco_prg_nm_get(),var->nm,add_fst_lng); 
      return False;
    } /* endif */
    var->has_add_fst=True; /* [flg] Valid add_offset attribute exists */
    var->typ_upk=add_fst_typ; /* [enm] Type of variable when unpacked (expanded) (in memory) */
  } /* endif */

  if(var->has_scl_fct && var->has_add_fst){
    if(scl_fct_typ != add_fst_typ){
      if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stdout,"%s: WARNING nco_pck_dsk_inq() reports type of scale_factor does not equal type of add_offset. Will not attempt to unpack.\n",nco_prg_nm_get());
      return False;
    } /* endif */
  } /* endif */

  if(var->has_scl_fct || var->has_add_fst){
    /* Variable is considered packed iff either or both valid scale_factor or add_offset exist */
    var->pck_dsk=True; /* [flg] Variable is packed on disk */
    /* If variable is packed on disk and is in memory then variable is packed in memory */
    var->pck_ram=True; /* [flg] Variable is packed in memory */
    var->typ_upk=(var->has_scl_fct) ? scl_fct_typ : add_fst_typ; /* [enm] Type of variable when unpacked (expanded) (in memory) */
    if(nco_is_rth_opr(nco_prg_id_get()) && nco_dbg_lvl_get() >= nco_dbg_var){
      (void)fprintf(stdout,"%s: PACKING Variable %s is type %s packed into type %s\n",nco_prg_nm_get(),var->nm,nco_typ_sng(var->typ_upk),nco_typ_sng(var->typ_dsk));
      (void)fprintf(stdout,"%s: DEBUG Packed variables processed by all arithmetic operators are unpacked automatically, and then stored unpacked in the output file. If you wish to repack them in the output file, use, e.g., ncap2 -O -s \"foo=pack(foo);\" out.nc out.nc. If you wish to pack all variables in a file, use, e.g., ncpdq -P all_new in.nc out.nc.\n",nco_prg_nm_get());
    } /* endif print packing information */
  }else{
    /* Variable is not packed since neither scale factor nor add_offset exist
       Insert hooks which depend on variable not being packed here
       Currently this is no-op */
    ;
  } /* end else */

  return var->pck_dsk; /* [flg] Variable is packed on disk (valid scale_factor, add_offset, or both attributes exist) */
  
} /* end nco_pck_dsk_inq() */

void
nco_pck_mtd /* [fnc] Alter metadata according to packing specification */
(const var_sct * const var_in, /* I [ptr] Variable in original disk state */
 var_sct * const var_out, /* I/O [ptr] Variable whose metadata will be altered */
 const int nco_pck_map,  /* I [enm] Packing map */
 const int nco_pck_plc)  /* I [enm] Packing policy */
{
  /* Purpose: Alter metadata according to packing specification */
  const char fnc_nm[]="nco_pck_mtd()"; /* [sng] Function name */
  nc_type nc_typ_pck_out; /* [enm] Type to pack to */
  
  switch(nco_pck_plc){
  case nco_pck_plc_all_xst_att:
    /* If variable is already packed do nothing otherwise pack to default type */
    if(var_in->pck_ram){
      if(nco_dbg_lvl_get() >= nco_dbg_var) (void)fprintf(stdout,"%s: DEBUG %s keeping existing packing parameters and type (%s) for %s\n",nco_prg_nm_get(),fnc_nm,nco_typ_sng(var_in->type),var_in->nm);
    }else{
      goto var_upk_try_to_pck;
    } /* endif */
    break;
  case nco_pck_plc_xst_new_att:
    /* If variable is already packed then re-pack otherwise do nothing */
    if(var_in->pck_ram){
      goto var_pck_try_to_rpk;
    }else{
      /* Variable is not packed so do nothing */
      if(nco_dbg_lvl_get() >= nco_dbg_var) (void)fprintf(stdout,"%s: INFO %s leaving variable %s of type %s as unpacked\n",nco_prg_nm_get(),fnc_nm,var_in->nm,nco_typ_sng(var_out->typ_upk));
    } /* endelse */
    break;
  case nco_pck_plc_all_new_att:
    /* 20180909: Never re-pack when simply converting double->float or float->double */
    if(var_in->pck_ram){
      if(nco_pck_map == nco_pck_map_dbl_flt || nco_pck_map == nco_pck_map_flt_dbl){
	if(nco_dbg_lvl_get() >= nco_dbg_var) (void)fprintf(stdout,"%s: INFO %s leaving variable %s of type %s as packed\n",nco_prg_nm_get(),fnc_nm,var_in->nm,nco_typ_sng(var_out->typ_pck));
      }else{
	goto var_pck_try_to_rpk;
      } /* !dbl_flt && !flt_dbl */
    }else{
      goto var_upk_try_to_pck;
    } /* endif */
    break;
  case nco_pck_plc_upk:
    var_out->type=var_in->typ_upk;
    if(nco_dbg_lvl_get() >= nco_dbg_sbr){
      if(var_in->pck_ram) (void)fprintf(stdout,"%s: DEBUG %s will unpack variable %s from %s to %s\n",nco_prg_nm_get(),fnc_nm,var_in->nm,nco_typ_sng(var_in->type),nco_typ_sng(var_out->type)); else (void)fprintf(stdout,"%s: DEBUG %s variable %s is already unpacked and of type %s\n",nco_prg_nm_get(),fnc_nm,var_in->nm,nco_typ_sng(var_in->type)); 
    } /* endif dbg */
    break;
  case nco_pck_plc_nil:
  default: nco_dfl_case_pck_plc_err(); break;
  } /* end case */

  /* Return after finishing switch() statement and before falling through
     to code-saving goto branches */
  return;

 var_upk_try_to_pck: /* end goto */
  /* Variable is not yet packed---try to pack it */
  if(nco_pck_plc_typ_get(nco_pck_map,var_in->type,&nc_typ_pck_out)){
    var_out->type=nc_typ_pck_out;
    if(nco_dbg_lvl_get() >= nco_dbg_sbr) (void)fprintf(stdout,"%s: DEBUG %s will pack variable %s from %s to %s\n",nco_prg_nm_get(),fnc_nm,var_in->nm,nco_typ_sng(var_in->type),nco_typ_sng(var_out->type));
  }else{
    if(nco_dbg_lvl_get() >= nco_dbg_var) (void)fprintf(stdout,"%s: INFO %s packing policy %s with packing map %s does not allow packing variable %s of type %s, skipping...\n",nco_prg_nm_get(),fnc_nm,nco_pck_plc_sng_get(nco_pck_plc),nco_pck_map_sng_get(nco_pck_map),var_in->nm,nco_typ_sng(var_in->type));
  } /* !nco_pck_plc_alw */
  return;
  
 var_pck_try_to_rpk: /* end goto */
  /* Variable is already packed---try to re-pack it 
     Final packed variable type may differ from original */
  if(nco_pck_plc_typ_get(nco_pck_map,var_in->typ_upk,&nc_typ_pck_out)){
    var_out->type=nc_typ_pck_out;
    if(nco_dbg_lvl_get() >= nco_dbg_sbr) (void)fprintf(stdout,"%s: DEBUG %s will re-pack variable %s of expanded type %s from current packing (type %s) into new packing of type %s\n",nco_prg_nm_get(),fnc_nm,var_in->nm,nco_typ_sng(var_in->typ_upk),nco_typ_sng(var_in->type),nco_typ_sng(var_out->type));
  }else{
    if(nco_dbg_lvl_get() >= nco_dbg_var) (void)fprintf(stdout,"%s: WARNING %s variable %s of expanded type %s is already packed into type %s and re-packing is requested but packing policy %s and packing map %s does not allow re-packing variables of type %s\n",nco_prg_nm_get(),fnc_nm,var_in->nm,nco_typ_sng(var_in->typ_upk),nco_typ_sng(var_in->type),nco_pck_plc_sng_get(nco_pck_plc),nco_pck_map_sng_get(nco_pck_map),nco_typ_sng(var_in->typ_upk));
  } /* !nco_pck_plc_alw */
  return;

} /* end nco_pck_mtd() */

void
nco_pck_val /* [fnc] Pack variable according to packing specification */
(var_sct * const var_in, /* I [ptr] Variable in original disk state */
 var_sct * var_out, /* I/O [ptr] Variable after packing/unpacking operation */
 const int nco_pck_map,  /* I [enm] Packing map */
 const int nco_pck_plc,  /* I [enm] Packing policy */
 aed_sct * const aed_lst_add_fst, /* O [enm] Attribute edit structure, add_offset */
 aed_sct * const aed_lst_scl_fct) /* O [enm] Attribute edit structure, scale_factor */
{
  /* Purpose: Alter metadata according to packing specification */
  const char fnc_nm[]="nco_pck_val()"; /* [sng] Function name */
  nco_bool PCK_VAR_WITH_NEW_PCK_ATT=False; /* [flg] Insert new scale_factor and add_offset into lists */
  nc_type typ_out; /* [enm] Type in output file */
  
  /* typ_out contains type of variable defined in output file
     as defined by var_out->type which was set in var_pck_mtd() 
     We will temporarily set var_out->type to RAM type of variable
     Packing routine will re-set var_out->type to typ_out if necessary */
  typ_out=var_out->type; /* [enm] Type in output file */
  
  switch(nco_pck_plc){
  case nco_pck_plc_all_xst_att:
    /* nco_var_pck() expects to alter var_out->type itself, if necessary */
    var_out->type=var_in->typ_dsk;
    if(var_in->pck_ram){
      if(nco_dbg_lvl_get() >= nco_dbg_sbr) (void)fprintf(stdout,"%s: INFO %s keeping existing packing attributes for variable %s\n",nco_prg_nm_get(),fnc_nm,var_in->nm);
      /* Warn if packing attribute values are in memory for pre-packed variables */
      if(var_out->scl_fct.vp || var_out->add_fst.vp) (void)fprintf(stdout,"%s: WARNING %s reports variable %s has packing attribute values in memory. This is not supposed to happen through known code paths, but is not necessarily dangerous.\n",nco_prg_nm_get(),fnc_nm,var_in->nm);
      /* Remove dangling pointer, see explanation below */
      var_in->val.vp=NULL; 
    }else{
      goto var_upk_try_to_pck; /* end goto */
    } /* endif input variable was not packed */
    break;
  case nco_pck_plc_xst_new_att:
    if(var_in->pck_ram){
      nco_var_upk_swp(var_in,var_out);
      goto var_upk_try_to_pck;
    }else{
      /* Remove dangling pointer, see explanation below */
      var_in->val.vp=NULL; 
    } /* endif */
    break;
  case nco_pck_plc_all_new_att:
    if(var_in->pck_ram){
      /* Variable is already packed---unpack it before re-packing it */
      nco_var_upk_swp(var_in,var_out);
    }else{
      /* nco_var_pck() expects to alter var_out->type itself, if necessary */
      var_out->type=var_in->typ_dsk;
    } /* endif */
    goto var_upk_try_to_pck; 
    break;
  case nco_pck_plc_upk:
    /* Unpack if possible, otherwise remove dangling pointer (explanation below) */
    if(var_in->pck_ram) nco_var_upk_swp(var_in,var_out); else var_in->val.vp=NULL; 
    break;
  case nco_pck_plc_nil:
  default: nco_dfl_case_pck_plc_err(); break;
  } /* end case */

  /* Ensure code goes to final block before falling through to next goto */
  goto put_new_pck_att_in_lst;

 var_upk_try_to_pck: /* end goto */
  /* Variable is not yet packed---try to pack it */
  if(nco_pck_plc_typ_get(nco_pck_map,var_out->type,(nc_type *)NULL)){
    if(nco_dbg_lvl_get() >= nco_dbg_sbr) (void)fprintf(stdout,"%s: INFO %s packing variable %s values from %s to %s\n",nco_prg_nm_get(),fnc_nm,var_in->nm,nco_typ_sng(var_out->typ_upk),nco_typ_sng(typ_out));
    /* 20180909: Implement dbl_flt separately */
    if(nco_pck_map == nco_pck_map_dbl_flt){
      var_out=nco_var_cnf_typ((nc_type)NC_FLOAT,var_out);
    }else if(nco_pck_map == nco_pck_map_flt_dbl){
      var_out=nco_var_cnf_typ((nc_type)NC_DOUBLE,var_out);
    }else{
      var_out=nco_var_pck(var_out,typ_out,&PCK_VAR_WITH_NEW_PCK_ATT);
    } /* !dbl_flt, !flt_dbl */
  }else{
    if(nco_dbg_lvl_get() >= nco_dbg_var) (void)fprintf(stdout,"%s: INFO %s packing policy %s with packing map %s does not allow packing variable %s of type %s, skipping...\n",nco_prg_nm_get(),fnc_nm,nco_pck_plc_sng_get(nco_pck_plc),nco_pck_map_sng_get(nco_pck_map),var_in->nm,nco_typ_sng(var_out->typ_upk));
  } /* !nco_pck_plc_alw */ 
  /* Packing function nco_var_pck() usually free()'s var_out->val.vp 
     Hence var_in->val.vp is left with a dangling pointer
     In ncpdq, var_in->val.vp and var_out->val.vp point to same buffer 
     This reduces peak memory consumption by ~50%, but is dangerous */
  var_in->val.vp=NULL; 
  /* Ensure code goes to final block before falling through to next goto */
  goto put_new_pck_att_in_lst;
  
 put_new_pck_att_in_lst: /* end goto */
  /* Fill attribute edit structures
     Use values directly from variable structures rather than copying
     Attribute structure dynamic memory will be free()'d in nco_var_free() call */
  if(PCK_VAR_WITH_NEW_PCK_ATT){
    aed_lst_add_fst->var_nm=aed_lst_scl_fct->var_nm=var_out->nm;
    aed_lst_add_fst->id=aed_lst_scl_fct->id=var_out->id;
    aed_lst_add_fst->sz=aed_lst_scl_fct->sz=1L;
    aed_lst_add_fst->type=aed_lst_scl_fct->type=var_out->typ_upk;
    /* Packing generates at least one of scale_factor or add_offset, though not necessarily both.
       Delete pre-defined attributes for those which were not created */
    if(var_out->has_add_fst) aed_lst_add_fst->mode=aed_overwrite; else aed_lst_add_fst->mode=aed_delete;
    if(var_out->has_scl_fct) aed_lst_scl_fct->mode=aed_overwrite; else aed_lst_scl_fct->mode=aed_delete;
    /* Insert values into attribute structures */
    aed_lst_add_fst->val=var_out->add_fst;
    aed_lst_scl_fct->val=var_out->scl_fct;
  } /* endif */

} /* end nco_pck_val() */

var_sct * /* O [sct] Packed variable */
nco_var_pck /* [fnc] Pack variable in memory */
(var_sct *var, /* I/O [sct] Variable to be packed */
 const nc_type nc_typ_pck, /* I [enm] Type of variable when packed (on disk). This should be same as typ_dsk except in cases where variable is packed in input file and unpacked in output file. */
 nco_bool *PCK_VAR_WITH_NEW_PCK_ATT) /* I/O [flg] Routine generated new scale_factor/add_offset - if true in [I] then pre-existing scale_factor/add_offset is used */
{
  /* Purpose: Pack variable 
     Routine is inverse of nco_var_upk(): nco_var_pck[nco_var_upk(var)]=var 
     Currently routine outputs same variable structure as given on input
     In other words, output structure may be neglected as all changes are made 
     to input structure.
     NB: Value buffer var->val.vp is usually free()'d here
     Variables in calling routine which point to var->val.vp will be left dangling */

  const char fnc_nm[]="nco_var_pck()"; /* [sng] Function name */

  double scl_fct_dbl=double_CEWI; /* [sct] Double precision value of scale_factor */
  double add_fst_dbl=double_CEWI; /* [sct] Double precision value of add_offset */

  nco_bool PURE_MSS_VAL_FLD=False; /* [flg] Field is pure missing_value, i.e., no valid values */

  static nco_bool FIRST_WARNING=True;

  /* Set flag true once new scale_factor/add_offset generated  
  *PCK_VAR_WITH_NEW_PCK_ATT=False; */

  /* Return if variable in memory is currently packed and should not be re-packed */
  if(var->pck_ram) return var;

  /* Routine should be called with variable already in memory */
  if(var->val.vp == NULL) (void)fprintf(stdout,"%s: ERROR %s called with empty var->val.vp\n",nco_prg_nm_get(),fnc_nm);
  
  /* Packed type must be NC_BYTE, NC_CHAR, NC_SHORT, or NC_INT */
  if(nc_typ_pck == NC_FLOAT || nc_typ_pck == NC_DOUBLE || 
     nc_typ_pck == NC_UINT  || nc_typ_pck == NC_USHORT || 
     nc_typ_pck == NC_UBYTE || nc_typ_pck == NC_STRING){
    (void)fprintf(stdout,"%s: ERROR %s called to pack variable %s with invalid packed type nc_typ_pck = %s\n",nco_prg_nm_get(),fnc_nm,var->nm,nco_typ_sng(nc_typ_pck));
    nco_exit(EXIT_FAILURE);
  } /* endif */

  /* Variable must be packable (usually NC_INT, NC_FLOAT, or NC_DOUBLE)
     Definition of "packable" determined by nco_pck_plc_typ_get()
     Prefer not to make nco_var_pck() rely directly on nco_pck_plc_typ_get()
     However, certain types are never packable */
  if(var->type == NC_BYTE || var->type == NC_UBYTE || var->type == NC_CHAR || var->type == NC_STRING){
    (void)fprintf(stdout,"%s: ERROR %s is asked to pack variable %s of type %s\n",nco_prg_nm_get(),fnc_nm,var->nm,nco_typ_sng(var->type));
    nco_exit(EXIT_FAILURE);
  } /* endif */

  if(*PCK_VAR_WITH_NEW_PCK_ATT == False){ /* Keep in own scope for eventual functionalization of core packing algorithm */
    /* Compute packing parameters to apply to var

       Linear packing in a nutshell:
       scale_factor = (max-min)/ndrv <--> (max-min)/scale_factor = ndrv <--> scale_factor*ndrv = max-min
       add_offset = 0.5*(min+max)
       pck = (upk-add_offset)/scale_factor = (upk-0.5*(min+max))*ndrv/(max-min)
       upk = scale_factor*pck + add_offset = (max-min)*pck/ndrv + 0.5*(min+max) 

       where 

       ndrv = number of discrete representable values for given type of packed variable and
       ndrv = 256 iff var->typ_pck == NC_CHAR
       ndrv = 256*256 iff var->typ_pck == NC_SHORT
       ndrv = 256*256*256*256 = 2^32 iff var->typ_pck == NC_INT */

    const double max_mns_min_dbl_wrn=1.0e10; /* [frc] Threshold value for warning */
    double mss_val_dbl; /* [ptr] Missing value of variable expressed in double precision */
    double ndrv_dbl=double_CEWI; /* [frc] Double precision value of number of discrete representable values */
    double max_mns_min_dbl; /* [frc] Maximum value minus minimum value */

    ptr_unn ptr_unn_min; /* [ptr] Pointer union to minimum value of variable */
    ptr_unn ptr_unn_max; /* [ptr] Pointer union to maximum value of variable */
    ptr_unn ptr_unn_mss_val_dbl; /* [ptr] Pointer union to missing value of variable */
    
    var_sct *min_var; /* [sct] Minimum value of variable */
    var_sct *max_var; /* [sct] Maximum value of variable */
    var_sct *max_var_dpl; /* [sct] Copy of Maximum value of variable */
    var_sct *hlf_var; /* [sct] NCO variable for value 0.5 */
    var_sct *zero_var; /* [sct] NCO variable for value 0.0 */
    var_sct *ndrv_var; /* [sct] NCO variable for number of discrete representable values */
    
    val_unn hlf_unn; /* [frc] Generic container for value 0.5 */
    val_unn zero_unn; /* [frc] Generic container for value 0.0 */
    val_unn ndrv_unn; /* [nbr] Generic container for number of discrete representable values */

    /* Initialize data */
    hlf_unn.d=0.5; /* Generic container for value 0.5 */
    zero_unn.d=0.0; /* Generic container for value 0.0 */

    /* Derive scalar values for scale_factor and add_offset */
    var->scl_fct.vp=nco_free(var->scl_fct.vp);
    var->add_fst.vp=nco_free(var->add_fst.vp);
    var->scl_fct.vp=(void *)nco_malloc(nco_typ_lng(var->type));
    var->add_fst.vp=(void *)nco_malloc(nco_typ_lng(var->type));
    ptr_unn_min.vp=(void *)nco_malloc(nco_typ_lng(var->type));
    ptr_unn_max.vp=(void *)nco_malloc(nco_typ_lng(var->type));

    /* Create double precision missing_value for use in min/max arithmetic */
    ptr_unn_mss_val_dbl.vp=(void *)NULL; /* CEWI */
    if(var->has_mss_val){
      ptr_unn_mss_val_dbl.vp=(void *)nco_malloc(nco_typ_lng((nc_type)NC_DOUBLE));
      (void)nco_val_cnf_typ(var->type,var->mss_val,(nc_type)NC_DOUBLE,ptr_unn_mss_val_dbl);
    } /* endif has_mss_val */

    /* Find minimum and maximum values in data */
    (void)nco_var_avg_rdc_max(var->type,var->sz,1L,var->has_mss_val,var->mss_val,var->val,ptr_unn_min);
    (void)nco_var_avg_rdc_min(var->type,var->sz,1L,var->has_mss_val,var->mss_val,var->val,ptr_unn_max);

    /* Convert to NC_DOUBLE before 0.5*(min+max) operation */
    min_var=scl_ptr_mk_var(ptr_unn_min,var->type);
    min_var=nco_var_cnf_typ((nc_type)NC_DOUBLE,min_var);
    max_var=scl_ptr_mk_var(ptr_unn_max,var->type);
    max_var=nco_var_cnf_typ((nc_type)NC_DOUBLE,max_var);
    /* Copy max_var for use in scale_factor computation */
    max_var_dpl=nco_var_dpl(max_var);
    hlf_var=scl_mk_var(hlf_unn,NC_DOUBLE); /* [sct] NCO variable for value one half */

    /* Field is pure missing_value iff either extrema is missing_value */
    if(var->has_mss_val)
      if(min_var->val.dp[0] == ptr_unn_mss_val_dbl.dp[0]) 
	PURE_MSS_VAL_FLD=True;

    /* fxm TODO nco1003 20101129 */
    /* Change value of missing value iff necessary to fit inside packed type */
    if(var->has_mss_val && !PURE_MSS_VAL_FLD){
      double mss_val_dfl_dbl=0.0; /* CEWI */
      double pck_rng_min_dbl=0.0; /* CEWI */
      double pck_rng_max_dbl=0.0; /* CEWI */
      switch(nc_typ_pck){ 
      case NC_FLOAT:
	mss_val_dfl_dbl=NC_FILL_FLOAT; 
	pck_rng_min_dbl=NC_MIN_FLOAT;
	pck_rng_max_dbl=NC_MAX_FLOAT;
	break; 
      case NC_DOUBLE:
	mss_val_dfl_dbl=NC_FILL_DOUBLE; 
	pck_rng_min_dbl=NC_MIN_DOUBLE;
	pck_rng_max_dbl=NC_MAX_DOUBLE;
	break; 
      case NC_INT:
	mss_val_dfl_dbl=NC_FILL_INT; 
	pck_rng_min_dbl=NC_MIN_INT;
	pck_rng_max_dbl=NC_MAX_INT;
	break;
      case NC_SHORT:
	mss_val_dfl_dbl=NC_FILL_SHORT; 
	pck_rng_min_dbl=NC_MIN_SHORT;
	pck_rng_max_dbl=NC_MAX_SHORT;
	break;
      case NC_USHORT:
	mss_val_dfl_dbl=NC_FILL_USHORT;
	pck_rng_min_dbl=0.0;
	pck_rng_max_dbl=NC_MAX_USHORT;
	break;
      case NC_UINT:
	mss_val_dfl_dbl=NC_FILL_UINT;
	pck_rng_min_dbl=0.0;
	pck_rng_max_dbl=NC_MAX_UINT;
	break;
      case NC_INT64:
	mss_val_dfl_dbl=(double)NC_FILL_INT64; /* CEWI for MSVC */
	pck_rng_min_dbl=(double)NC_MIN_INT64; /* CEWI for MSVC */
	pck_rng_max_dbl=(double)NC_MAX_INT64; /* CEWI for MSVC */
	break;
      case NC_UINT64:
	mss_val_dfl_dbl=(double)NC_FILL_UINT64; /* CEWI for MSVC */
	pck_rng_min_dbl=0.0;
	pck_rng_max_dbl=(double)NC_MAX_UINT64; /* CEWI for MSVC */
	break;
      case NC_BYTE:
	mss_val_dfl_dbl=NC_FILL_BYTE; 
	pck_rng_min_dbl=NC_MIN_BYTE;
	pck_rng_max_dbl=NC_MAX_BYTE;
	break;
      case NC_UBYTE:
	mss_val_dfl_dbl=NC_FILL_UBYTE;
	pck_rng_min_dbl=0.0;
	pck_rng_max_dbl=NC_MAX_UBYTE;
	break;
      case NC_CHAR:
	mss_val_dfl_dbl=NC_FILL_CHAR;
	pck_rng_min_dbl=0.0;
	pck_rng_max_dbl=NC_MAX_CHAR;
	break;
      case NC_STRING: break; /* Do nothing */
      default: nco_dfl_case_nc_type_err(); break;
      } /* end switch */ 
      if(nco_dbg_lvl_get() >= nco_dbg_io) (void)fprintf(stdout,"%s: %s mss_val_dfl_dbl = %g, pck_rng_min_dbl = %g, pck_rng_max_dbl = %g, \n",nco_prg_nm_get(),fnc_nm,mss_val_dfl_dbl,pck_rng_min_dbl,pck_rng_max_dbl);

      mss_val_dbl=ptr_unn_mss_val_dbl.dp[0];
      if(nc_typ_pck != NC_STRING && (mss_val_dbl < pck_rng_min_dbl || mss_val_dbl > pck_rng_max_dbl)){ 
	if(FIRST_WARNING) (void)fprintf(stdout,"%s: WARNING %s reports mss_val_dbl (= %g) is outside range (%g <= x <= %g) represented by packed data type (= %s). Conversion of missing values is unpredictable and could lead to erroneous results. Workaround is to set _FillValue to be within packed range with, e.g.,\nncatted -O -a _FillValue,,o,f,%g inout.nc\nTo avoid excessive noise, NCO prints this WARNING at most once per dataset. For more information on this workaround, see\nhttp://nco.sf.net/nco.html#mss_val\n",nco_prg_nm_get(),fnc_nm,mss_val_dbl,pck_rng_min_dbl,pck_rng_max_dbl,nco_typ_sng(nc_typ_pck),mss_val_dfl_dbl);
	FIRST_WARNING=False;
      } /* NC_STRING */
    } /* end if(var->has_mss_val && !PURE_MSS_VAL_FLD) */

    if(nco_dbg_lvl_get() >= nco_dbg_io) (void)fprintf(stdout,"%s: %s: min_var = %g, max_var = %g\n",nco_prg_nm_get(),var->nm,min_var->val.dp[0],max_var->val.dp[0]);

    /* add_offset is 0.5*(min+max) */
    /* max_var->val is overridden with add_offset answers, no longer valid as max_var */
    (void)nco_var_add((nc_type)NC_DOUBLE,1L,var->has_mss_val,ptr_unn_mss_val_dbl,min_var->val,max_var->val);
    (void)nco_var_mlt((nc_type)NC_DOUBLE,1L,var->has_mss_val,ptr_unn_mss_val_dbl,hlf_var->val,max_var->val);
    /* Contents of max_var are actually add_offset */
    (void)nco_val_cnf_typ((nc_type)NC_DOUBLE,max_var->val,var->type,var->add_fst);

    /* ndrv is 2^{bits per packed value} where bppv = 8 for NC_CHAR and bppv = 16 for NC_SHORT
       Subtract one to leave slop for rounding errors
       20101130 Subtract two to leave room for missing_value */
    if(nc_typ_pck == NC_BYTE || nc_typ_pck == NC_CHAR){
      ndrv_dbl=256.0-2.0; /* [sct] Double precision value of number of discrete representable values */
    }else if(nc_typ_pck == NC_SHORT){
      /* 20110203 Griffith Young reports: 
	 "I have noticed some small inconsistencies with the _FillValue values.
	 short int is [32767, -32768].  The default _FillValue is -32767.  
	 Therefore a reasonable range would be [32766, -32766] and a reasonable
	 scale_factor becomes (max - min) -> 32766 - (-32766) -> 65532." */
      ndrv_dbl=65536.0-4.0; /* [sct] Double precision value of number of discrete representable values */
    }else if(nc_typ_pck == NC_INT){
      ndrv_dbl=4294967295.0-2.0; /* [sct] Double precision value of number of discrete representable values */
    } /* end else */
    ndrv_unn.d=ndrv_dbl; /* Generic container for number of discrete representable values */
    ndrv_var=scl_mk_var(ndrv_unn,NC_DOUBLE); /* [sct] Variable structure for number of discrete representable values */

    /* scale_factor is (max-min)/ndrv
       If max-min == 0 then variable is constant value so scale_factor=0.0 and add_offset=var
       If max-min > ndrv then precision is worse  than 1.0
       If max-min < ndrv then precision is better than 1.0 */
    (void)nco_var_sbt((nc_type)NC_DOUBLE,1L,var->has_mss_val,ptr_unn_mss_val_dbl,min_var->val,max_var_dpl->val);
    /* max-min is currently stored in max_var_dpl */
    max_mns_min_dbl=ptr_unn_2_scl_dbl(max_var_dpl->val,max_var_dpl->type); 

    /* Manually set max-min=0.0 for pure missing_value fields to set add_offset correctly */
    if(PURE_MSS_VAL_FLD) max_mns_min_dbl=0.0;

    if(max_mns_min_dbl != 0.0){
      (void)nco_var_dvd((nc_type)NC_DOUBLE,1L,var->has_mss_val,ptr_unn_mss_val_dbl,ndrv_var->val,max_var_dpl->val);
      /* Contents of max_var_dpl are actually scale_factor */
      (void)nco_val_cnf_typ((nc_type)NC_DOUBLE,max_var_dpl->val,var->type,var->scl_fct);
    }else{
      /* Variable is constant, i.e., equal values everywhere */
      zero_var=scl_mk_var(zero_unn,var->type); /* [sct] NCO variable for value 0.0 */
      /* Set scale_factor to 0.0 */
      (void)memcpy(var->scl_fct.vp,zero_var->val.vp,nco_typ_lng(var->type));
      if(zero_var) zero_var=nco_var_free(zero_var);
      /* Set add_offset to first variable value 
	 Variable is constant everywhere so particular value copied is unimportant */
      (void)memcpy(var->add_fst.vp,var->val.vp,nco_typ_lng(var->type));
    } /* end else */

    if(max_mns_min_dbl > max_mns_min_dbl_wrn){
      if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stdout,"%s: WARNING %s reports data range of variable %s is = %g. The linear data packing technique defined by netCDF's packing convention and implemented by NCO result in significant precision loss over such a great range.\n",nco_prg_nm_get(),fnc_nm,var->nm,max_mns_min_dbl);
      if(nco_dbg_lvl_get() >= nco_dbg_std) if(var->has_mss_val) (void)fprintf(stdout,"%s: HINT variable %s has %s = %g. Consider specifying new %s to reduce range of data needing packing. See http://nco.sf.net/nco.html#ncatted for examples of how to change the %s attribute.\n",nco_prg_nm_get(),var->nm,nco_mss_val_sng_get(),ptr_unn_mss_val_dbl.dp[0],nco_mss_val_sng_get(),nco_mss_val_sng_get());
    } /* endif large data range */

    /* Free minimum and maximum values */
    ptr_unn_min.vp=nco_free(ptr_unn_min.vp);
    ptr_unn_max.vp=nco_free(ptr_unn_max.vp);

    /* Free temporary double missing_value */
    if(var->has_mss_val) ptr_unn_mss_val_dbl.vp=nco_free(ptr_unn_mss_val_dbl.vp);

    /* Free variables */
    if(min_var) min_var=nco_var_free(min_var);
    if(max_var) max_var=nco_var_free(max_var);
    if(max_var_dpl) max_var_dpl=nco_var_free(max_var_dpl);
    if(hlf_var) hlf_var=nco_var_free(hlf_var);
    if(ndrv_var) ndrv_var=nco_var_free(ndrv_var);

    /* Do not bother creating superfluous scale_factor (0.0 or 1.0) or add_offset (0.0) */
    scl_fct_dbl=ptr_unn_2_scl_dbl(var->scl_fct,var->type); 
    add_fst_dbl=ptr_unn_2_scl_dbl(var->add_fst,var->type);

    if(scl_fct_dbl != 0.0 && scl_fct_dbl != 1.0) var->has_scl_fct=True; /* [flg] Valid scale_factor attribute exists */
    if(add_fst_dbl != 0.0) var->has_add_fst=True; /* [flg] Valid add_offset attribute exists */

    /* However, must create either scale_factor or add_offset
       Otherwise, routine fails to pack field uniformly equal to zero (0.0)
       In zero corner case, create add_offset (avoids division by zero problems) */
    if(scl_fct_dbl == 0.0 && add_fst_dbl == 0.0) var->has_add_fst=True; 

  } /* endif True */

  /* Create double precision value of scale_factor for diagnostics */
  if(var->has_scl_fct){ /* [flg] Valid scale_factor attribute exists */
    scl_fct_dbl=ptr_unn_2_scl_dbl(var->scl_fct,var->type); 
    if(scl_fct_dbl == 0.0 && nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stdout,"%s: WARNING %s reports scl_fct_dbl = 0.0\n",nco_prg_nm_get(),fnc_nm);
  } /* endif */
  
  /* Create double precision value of add_offset for diagnostics */
  if(var->has_add_fst){ /* [flg] Valid add_offset attribute exists */
    add_fst_dbl=ptr_unn_2_scl_dbl(var->add_fst,var->type);
  } /* endif */
  
  if(nco_dbg_lvl_get() >= nco_dbg_io) (void)fprintf(stdout,"%s: %s reports variable %s has scl_fct_dbl = %g, add_fst_dbl = %g\n",nco_prg_nm_get(),fnc_nm,var->nm,scl_fct_dbl,add_fst_dbl);
  
  /* Packing attributes now exist and are same type as variable in memory */

  /* Apply scale_factor and add_offset to reduce variable size
     add_offset and scale_factor are always scalars so use nco_var_scv_* functions
     nco_var_scv_[sub,multiply] functions avoid cost of broadcasting attributes and doing element-by-element operations 
     Using nco_var_scv_[sub,multiply] instead of ncap_var_scv_[sub,multiply] avoids cost of deep copies
     Moreover, this keeps variable structure from changing (because ncap_var_scv_* functions all do deep copies before operations) and thus complicating memory management */
  if(var->has_add_fst){ /* [flg] Valid add_offset attribute exists */
    nco_bool has_mss_val_tmp; /* [flg] Temporary missing_value flag */

    /* Subtract add_offset from var */
    scv_sct add_fst_scv;
    add_fst_scv.type=NC_DOUBLE;
    add_fst_scv.val.d=add_fst_dbl;
    (void)nco_scv_cnf_typ(var->type,&add_fst_scv);
    /* Pass temporary missing_value flag to accomodate pure missing_value fields */
    has_mss_val_tmp=var->has_mss_val;
    /* Dupe nco_var_scv_sub() into subtracting missing values when all values are missing */
    if(PURE_MSS_VAL_FLD){
      has_mss_val_tmp=False;
      if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stdout,"%s: INFO %s reports variable %s is filled completely with %s = %g. Why do you store variables with no valid values?\n",nco_prg_nm_get(),fnc_nm,var->nm,nco_mss_val_sng_get(),add_fst_dbl);
    } /* !PURE_MSS_VAL_FLD */
    (void)nco_var_scv_sub(var->type,var->sz,has_mss_val_tmp,var->mss_val,var->val,&add_fst_scv);
  } /* endif */

  if(var->has_scl_fct){ /* [flg] Valid scale_factor attribute exists */
    /* Divide var by scale_factor */
    scv_sct scl_fct_scv;
    scl_fct_scv.type=NC_DOUBLE;
    scl_fct_scv.val.d=scl_fct_dbl;
    (void)nco_scv_cnf_typ(var->type,&scl_fct_scv);
    if(scl_fct_dbl != 0.0) (void)nco_var_scv_dvd(var->type,var->sz,var->has_mss_val,var->mss_val,var->val,&scl_fct_scv);
  } /* endif */

  if(!var->has_scl_fct && !var->has_add_fst){
    (void)fprintf(stderr,"%s: ERROR Reached end of %s without packing variable\n",nco_prg_nm_get(),fnc_nm);
    nco_exit(EXIT_FAILURE);
  }else{
    *PCK_VAR_WITH_NEW_PCK_ATT=True; /* O [flg] Routine generated new scale_factor/add_offset */
  } /* endif */

  /* Tell the world we packed the variable
     This is true if input variable satisfied nco_pck_plc_typ_get() criteria
     Variables that fail nco_pck_plc_typ_get() (e.g., type == NC_CHAR) are not packed 
     and should not have their packing attributes set */
  var->pck_ram=True; /* [flg] Variable is packed in memory */
  var->typ_pck=nc_typ_pck; /* [enm] Type of variable when packed (on disk). This should be same as typ_dsk except in cases where variable is packed in input file and unpacked in output file. */
  var->typ_upk=var->type; /* [enm] Type of variable when unpacked (expanded) (in memory) */

  /* Convert variable to user-specified packed type
     This is where var->type is changed from original to packed type */
  var=nco_var_cnf_typ(nc_typ_pck,var);

  if(nco_dbg_lvl_get() >= nco_dbg_sbr) (void)fprintf(stdout,"%s: PACKING %s packed %s into %s\n",nco_prg_nm_get(),fnc_nm,var->nm,nco_typ_sng(var->type));

  return var;
} /* end nco_var_pck() */

var_sct * /* O [sct] Unpacked variable */
nco_var_upk /* [fnc] Unpack variable in memory */
(var_sct *var) /* I/O [sct] Variable to be unpacked */
{
  /* Threads: Routine is thread-safe */
  /* Purpose: Unpack variable
     Routine is inverse of nco_var_pck(): nco_var_upk[nco_var_pck(var)]=var
     Routine handles missing_value's implicitly:
     nco_var_cnf_typ() automatically converts missing_value, if any, to unpacked type
     This may need to change when nco427 is addressed */

  const char fnc_nm[]="nco_var_upk()";
  const char scl_fct_sng[]="scale_factor"; /* [sng] Unidata standard string for scale factor */
  const char add_fst_sng[]="add_offset"; /* [sng] Unidata standard string for add offset */
  int nco_upk_cnv_typ;

  /* Return if variable in memory is not currently packed */
  if(!var->pck_ram) return var;

  /* Routine should be called with variable already in memory */
  if(var->val.vp == NULL){
    (void)fprintf(stdout,"%s: ERROR %s called with empty var->val.vp\n",nco_prg_nm_get(),fnc_nm);
    nco_exit(EXIT_FAILURE);
  } /* endif */

  /* Packed variables are not guaranteed to have both scale_factor and add_offset
     scale_factor is guaranteed to be of type NC_FLOAT or NC_DOUBLE and of size 1 (a scalar)
     Hence algorithm create scalar value structures from values of scale_factor, add_offset */

  /* 20130729:
     Rumors are true: 
     NASA HDF data, including NASA MODIS swaths, expect offsets to be _subtracted_ then scaled!
     http://modis-atmos.gsfc.nasa.gov/MOD08_D3/faq.html
     The netCDF convention for packing is opposite:
     http://www.unidata.ucar.edu/software/netcdf/docs/netcdf/Attribute-Conventions.html
     Hence unpacking NASA SDS data requires re-ordering and re-defining the netCDF-standard unpacking algorithm */

  /* Test unpacking conventions:
     ncpdq -O -C -U       -v ^pck_.? ~/nco/data/in.nc ~/foo.nc # Unpack netCDF
     ncpdq -O -C -U --hdf -v ^pck_.? ~/nco/data/in.nc ~/foo.nc # Unpack HDF
     ncpdq -O -C -P --hdf -v ^pck_.? ~/nco/data/in.nc ~/foo.nc # Unpack HDF and re-pack netCDF */

  nco_upk_cnv_typ=nco_upk_cnv_get();
  if(nco_upk_cnv_typ == nco_upk_netCDF){
    /* netCDF unpack definition: unpacked=(scale_factor*packed)+add_offset */
    
    if(var->has_scl_fct){ /* [flg] Valid scale_factor attribute exists */
      scv_sct scl_fct_scv;
      var->scl_fct.vp=(void *)nco_malloc(nco_typ_lng(var->typ_upk));
      (void)nco_get_att(var->nc_id,var->id,scl_fct_sng,var->scl_fct.vp,var->typ_upk);
      scl_fct_scv=ptr_unn_2_scv(var->typ_upk,var->scl_fct);
      /* Convert var to type of scale_factor for expansion */
      var=nco_var_cnf_typ(scl_fct_scv.type,var);
      /* Multiply var by scale_factor */
      (void)nco_var_scv_mlt(var->type,var->sz,var->has_mss_val,var->mss_val,var->val,&scl_fct_scv);
    } /* endif has_scl_fct */
    
    if(var->has_add_fst){ /* [flg] Valid add_offset attribute exists */
      scv_sct add_fst_scv;
      var->add_fst.vp=(void *)nco_malloc(nco_typ_lng(var->typ_upk));
      (void)nco_get_att(var->nc_id,var->id,add_fst_sng,var->add_fst.vp,var->typ_upk);
      add_fst_scv=ptr_unn_2_scv(var->typ_upk,var->add_fst);
      /* Convert var to type of scale_factor for expansion */
      var=nco_var_cnf_typ(add_fst_scv.type,var);
      /* Add add_offset to var */
      (void)nco_var_scv_add(var->type,var->sz,var->has_mss_val,var->mss_val,var->val,&add_fst_scv);
    } /* endif has_add_fst */

  }else if(nco_upk_cnv_typ == nco_upk_HDF_MOD10){ /* !netCDF_unpack_convention */
    /* 20130729: NASA HDF MODIS MOD10 unpack definition: unpacked=scale_factor*(packed-add_offset) */

    if(var->has_add_fst){ /* [flg] Valid add_offset attribute exists */
      scv_sct add_fst_scv;
      var->add_fst.vp=(void *)nco_malloc(nco_typ_lng(var->typ_upk));
      (void)nco_get_att(var->nc_id,var->id,add_fst_sng,var->add_fst.vp,var->typ_upk);
      add_fst_scv=ptr_unn_2_scv(var->typ_upk,var->add_fst);
      /* Convert var to type of scale_factor for expansion */
      var=nco_var_cnf_typ(add_fst_scv.type,var);
      /* Subtract add_offset from var */
      (void)nco_var_scv_sub(var->type,var->sz,var->has_mss_val,var->mss_val,var->val,&add_fst_scv);
    } /* endif has_add_fst */

    if(var->has_scl_fct){ /* [flg] Valid scale_factor attribute exists */
      scv_sct scl_fct_scv;
      var->scl_fct.vp=(void *)nco_malloc(nco_typ_lng(var->typ_upk));
      (void)nco_get_att(var->nc_id,var->id,scl_fct_sng,var->scl_fct.vp,var->typ_upk);
      scl_fct_scv=ptr_unn_2_scv(var->typ_upk,var->scl_fct);
      /* Convert var to type of scale_factor for expansion */
      var=nco_var_cnf_typ(scl_fct_scv.type,var);
      /* Multiply var by scale_factor */
      (void)nco_var_scv_mlt(var->type,var->sz,var->has_mss_val,var->mss_val,var->val,&scl_fct_scv);
    } /* endif has_scl_fct */

  }else if(nco_upk_cnv_typ == nco_upk_HDF_MOD13){ /* !netCDF_unpack_convention */
    /* 20160501: NASA HDF MODIS MOD13 unpack definition: unpacked=(packed-add_offset)/scale_factor */

    if(var->has_add_fst){ /* [flg] Valid add_offset attribute exists */
      scv_sct add_fst_scv;
      var->add_fst.vp=(void *)nco_malloc(nco_typ_lng(var->typ_upk));
      (void)nco_get_att(var->nc_id,var->id,add_fst_sng,var->add_fst.vp,var->typ_upk);
      add_fst_scv=ptr_unn_2_scv(var->typ_upk,var->add_fst);
      /* Convert var to type of scale_factor for expansion */
      var=nco_var_cnf_typ(add_fst_scv.type,var);
      /* Subtract add_offset from var */
      (void)nco_var_scv_sub(var->type,var->sz,var->has_mss_val,var->mss_val,var->val,&add_fst_scv);
    } /* endif has_add_fst */

    if(var->has_scl_fct){ /* [flg] Valid scale_factor attribute exists */
      scv_sct scl_fct_scv;
      var->scl_fct.vp=(void *)nco_malloc(nco_typ_lng(var->typ_upk));
      (void)nco_get_att(var->nc_id,var->id,scl_fct_sng,var->scl_fct.vp,var->typ_upk);
      scl_fct_scv=ptr_unn_2_scv(var->typ_upk,var->scl_fct);
      /* Convert var to type of scale_factor for expansion */
      var=nco_var_cnf_typ(scl_fct_scv.type,var);
      /* Divide var by scale_factor */
      (void)nco_var_scv_dvd(var->type,var->sz,var->has_mss_val,var->mss_val,var->val,&scl_fct_scv);
    } /* endif has_scl_fct */

  }else{ /* !netCDF_unpack_convention */

    (void)fprintf(stdout,"%s: ERROR %s reports unknown nco_upk_cnv\n",nco_prg_nm_get(),fnc_nm);
    nco_exit(EXIT_FAILURE);

  } /* !netCDF_unpack_convention */

  if(var->has_mss_val) var=nco_cnv_mss_val_typ(var,var->type);

  /* Tell the world */  
  var->pck_ram=False;

  /* Clean up tell-tale signs that variable was ever packed */
  var->has_scl_fct=False; /* [flg] Valid scale_factor attribute exists */
  var->has_add_fst=False; /* [flg] Valid add_offset attribute exists */
  var->scl_fct.vp=nco_free(var->scl_fct.vp);
  var->add_fst.vp=nco_free(var->add_fst.vp);

  if(nco_dbg_lvl_get() >= nco_dbg_var) (void)fprintf(stdout,"%s: PACKING %s unpacked %s into %s\n",nco_prg_nm_get(),fnc_nm,var->nm,nco_typ_sng(var->type));

  return var;
} /* end nco_var_upk() */

void
nco_var_upk_swp /* [fnc] Unpack var_in into var_out */
(var_sct * const var_in, /* I/O [sct] Variable to unpack */
 var_sct * const var_out) /* I/O [sct] Variable to unpack into */
{
  /* Purpose: Unpack var_in into var_out
     Information flow in ncpdq prevents ncpdq from calling nco_var_upk()
     directly with either var_in or var_out.
     Need combination of var_in (for correct file and variable IDs) 
     and var_out (so unpacked variable ends up in correct place)
     Accomplish this by unpacking into temporary variable and copying 
     needed information from temporary (swap) variable to var_out.
     Routine hides gory details of swapped unpacking
     var_in is untouched except var_in->val buffer is free()'d

     nco_pck_val() uses this routine for two purposes:
     1. Unpack already packed variable prior to re-packing them 
     2. Unpack already packed variables permanently */
  const char fnc_nm[]="nco_var_upk_swp()";
  var_sct *var_tmp; /* [sct] Temporary variable to be unpacked */
  
  if(var_in->pck_ram){
    if(nco_dbg_lvl_get() >= nco_dbg_io) (void)fprintf(stdout,"%s: DEBUG %s unpacking variable %s values from %s to %s\n",nco_prg_nm_get(),fnc_nm,var_in->nm,nco_typ_sng(var_out->typ_pck),nco_typ_sng(var_out->typ_upk));
  }else{
    (void)fprintf(stderr,"%s: ERROR %s variable %s is already unpacked\n",nco_prg_nm_get(),fnc_nm,var_in->nm);
    nco_exit(EXIT_FAILURE);
  } /* endif not already packed */

  /* Output file does not contain packing attributes yet 
     Hence unpacking var_out directly is impossible 
     Instead, make var_tmp a copy of var_in and unpack var_tmp 
     Then copy needed elements of var_tmp to var_out 
     Then delete the rest of var_tmp 
     Fields modified in nco_var_upk() must be explicitly updated in var_out */
  var_tmp=nco_var_dpl(var_in);
  /* Free current input buffer */
  var_in->val.vp=nco_free(var_in->val.vp);
  /* Unpack temporary variable */
  var_tmp=nco_var_upk(var_tmp); 
  /* Save relevent parts of temporary variable into output variable */
  var_out->type=var_tmp->type;
  var_out->val=var_tmp->val;
  var_out->pck_ram=var_tmp->pck_ram;
  /* nco_var_cnf_typ() in nco_var_upk() automatically converts missing_value, if any, to unpacked type */
  if(var_out->has_mss_val){
    /* Free current missing value before obtaining new one */
    var_out->mss_val.vp=(void *)nco_free(var_out->mss_val.vp);
    var_out->mss_val=var_tmp->mss_val;
    /* var_out now owns mss_val, make sure nco_var_free(var_tmp) ignores it */
    var_tmp->mss_val.vp=NULL;
  } /* endif */
  var_out->has_scl_fct=var_tmp->has_scl_fct;
  var_out->has_add_fst=var_tmp->has_add_fst;
  var_out->scl_fct.vp=nco_free(var_out->scl_fct.vp);
  var_out->add_fst.vp=nco_free(var_out->add_fst.vp);
  /* var_tmp->val buffer now doubles as var_out->val buffer
     Prevent nco_var_free() from free()'ing var_tmp->val
     Setting var_tmp->val.vp=NULL accomplishes this
     Use var_out->val.vp to free() this buffer later with nco_var_free() */
  var_tmp->val.vp=NULL;
  if(var_tmp) var_tmp=nco_var_free(var_tmp);
} /* end nco_var_upk_swp() */

var_sct * /* O [sct] Packed variable */
nco_put_var_pck /* [fnc] Pack variable in memory and write packing attributes to disk */
(const int out_id, /* I [id] netCDF output file ID */
 var_sct *var, /* I/O [sct] Variable to be packed */
 const int nco_pck_plc) /* [enm] Packing operation type */
{
  /* Purpose: Pack variable in memory and write packing attributes to disk
     NB: Routine is not complete, debugged, or currently used 
     ncpdq breaks up writing packed variables into multiple tasks, i.e.,
     ncpdq separates variable value writes from packing attribute value writes.
     This routine is intended to write a packed variable in one routine */
  nco_bool PCK_VAR_WITH_NEW_PCK_ATT=False; /* [flg] Insert new scale_factor and add_offset into lists */
  
  switch(nco_pck_plc){
  case nco_pck_plc_all_xst_att:
    break;
  case nco_pck_plc_xst_new_att:
    break;
  case nco_pck_plc_all_new_att:
    break;
  case nco_pck_plc_upk:
    break;
  case nco_pck_plc_nil:
  default: nco_dfl_case_pck_plc_err(); break;
  } /* end switch */

  /* Pack variable */
  if(var->xrf->pck_dsk && !var->xrf->pck_ram) var=nco_var_pck(var,var->typ_pck,&PCK_VAR_WITH_NEW_PCK_ATT);

  /* Write/overwrite scale_factor and add_offset attributes */
  if(var->pck_ram){ /* [flg] Variable is packed in memory */
    if(var->has_scl_fct){ /* [flg] Valid scale_factor attribute exists */
      (void)nco_put_att(out_id,var->id,"scale_factor",var->typ_upk,1,var->scl_fct.vp);
    } /* endif has_scl_fct */
    if(var->has_add_fst){ /* [flg] Valid add_offset attribute exists */
      (void)nco_put_att(out_id,var->id,"add_offset",var->typ_upk,1,var->add_fst.vp);
    } /* endif has_add_fst */
  } /* endif pck_ram */
  
  return var;
  
} /* end nco_put_var_pck() */
