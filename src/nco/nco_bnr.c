/* $Header$ */

/* Purpose: Binary file utilities */

/* Copyright (C) 1995--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

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
    (void)fprintf(stderr,"%s: ERROR unable to open for writing the binary output file %s\n",nco_prg_nm_get(),fl_bnr);
    (void)fprintf(stderr,"%s: HINT All components of the filepath must exist before file can be opened\n",nco_prg_nm_get());
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
 const void * const vp) /* I [ptr] Data to write */
{
  /* Purpose: Write unformatted binary data */

  /* Background:
     Best algorithmic resource is:
     https://stackoverflow.com/questions/19275955/convert-little-endian-to-big-endian     
     POSIX requires functions host-to-network and network-to-host functions for short (16-bit) and long (32-bit) types: htons(), ntohs(), htonl(), ntohl()
     These functions just implement __builtin_bswapXX() under-the-hood if necessary */

  /* Testing:
     ncks -O -D 3 -b ~/foo.bnr ~/nco/data/in.nc ~/foo.nc # Write binary in native order
     20181029: Implement global --nco_bnr_cnv
     ncks -O -D 3 --bsa=0 -b ~/foo.bnr ~/nco/data/in.nc ~/foo.nc # Write binary in native order
     ncks -O -D 3 --bsa=1 -b ~/foo.bnr ~/nco/data/in.nc ~/foo.nc # Write binary in byte-swapped (non-native) order
     ncks -O -D 3 -v uint32_var.? --bsa=0 -b ~/foo.bnr ~/nco/data/in_grp.nc ~/foo.nc # Write binary in native order
     ncks -O -D 3 -v uint32_var.? --bsa=1 -b ~/foo.bnr ~/nco/data/in_grp.nc ~/foo.nc # Write binary in byte-swapped (non-native) order
     Examine input and output:
     ncks --hdn -v uint32_var.? ~/nco/data/in_grp.nc
     od -An -X -N 8 ~/foo.bnr */

  const char fnc_nm[]="nco_bnr_wrt()"; /* [sng] Function name */

  long wrt_nbr; /* [nbr] Number of elements successfully written */

  nco_bool flg_byt_swp; /* [flg] Use byte-swap algorithm and write non-native order (write little- and big-endian on big- and little-endian machines, respectively) */
  
  size_t wrd_sz;
  size_t mmr_sz;
  size_t idx;

  unsigned short *u16_ptr=NULL;
  unsigned int *u32_ptr=NULL;
  unsigned long int *u64_ptr=NULL;

  void *vp_bs; /* [ptr] CEWI Byte-swapped copy of input data */

  wrd_sz=nco_typ_lng(var_typ);
  vp_bs=NULL; /* CEWI */

  flg_byt_swp=nco_bnr_cnv_get();

  /* Write unformatted data to binary output file */
  if(flg_byt_swp && wrd_sz > 1L){
    mmr_sz=var_sz*wrd_sz;
    vp_bs=(void *)nco_malloc(mmr_sz);
    vp_bs=memcpy(vp_bs,vp,mmr_sz);

    switch(wrd_sz){
#ifdef _MSC_VER
    case 8:
      u64_ptr=(unsigned long int *)vp_bs;
      for(idx=0;idx<var_sz;idx++) u64_ptr[idx]=_byteswap_uint64(u64_ptr[idx]);
      break;
    case 4:
      u32_ptr=(unsigned int *)vp_bs;
      for(idx=0;idx<var_sz;idx++) u32_ptr[idx]=_byteswap_ulong(u32_ptr[idx]);
      break;
    case 2:
      u16_ptr=(unsigned short *)vp_bs;
      for(idx=0;idx<var_sz;idx++) u16_ptr[idx]=_byteswap_ushort(u16_ptr[idx]);
      break;
#else /* !_MSC_VER */
    case 8:
      u64_ptr=(unsigned long int *)vp_bs;
# ifdef HAVE_BSWAP64
      for(idx=0;idx<var_sz;idx++) u64_ptr[idx]=__builtin_bswap64(u64_ptr[idx]);
# else /* !HAVE_BSWAP64 */
      /* fxm: Swap first half of 8-byte words? then offset by four and swap second half? */
      for(idx=0;idx<var_sz;idx++) u64_ptr[idx]=htonl(u64_ptr[idx]);
# endif /* !HAVE_BSWAP64 */
      break;
    case 4:
      u32_ptr=(unsigned int *)vp_bs;
# ifdef HAVE_BSWAP32
      for(idx=0;idx<var_sz;idx++) u32_ptr[idx]=__builtin_bswap32(u32_ptr[idx]);
# else /* !HAVE_BSWAP32 */
      for(idx=0;idx<var_sz;idx++) u32_ptr[idx]=htonl(u32_ptr[idx]);
# endif /* !HAVE_BSWAP32 */
      break;
    case 2:
      u16_ptr=(unsigned short *)vp_bs;
# ifdef HAVE_BSWAP16
      for(idx=0;idx<var_sz;idx++) u16_ptr[idx]=__builtin_bswap16(u16_ptr[idx]);
# else /* !HAVE_BSWAP16 */
      for(idx=0;idx<var_sz;idx++) u16_ptr[idx]=htons(u16_ptr[idx]);
# endif /* !HAVE_BSWAP16 */
      break;
#endif /* !_MSC_VER */
    case 1:
    default:
      (void)fprintf(stderr,"%s: ERROR %s reports variable %s of type %s has unexpected word-size = %lu\n",nco_prg_nm_get(),fnc_nm,var_nm,nco_typ_sng(var_typ),(unsigned long)wrd_sz);
      nco_exit(EXIT_FAILURE);
      break;
    } /* end switch */

    wrt_nbr=fwrite(vp_bs,wrd_sz,(size_t)var_sz,fp_bnr);

    if(vp_bs) vp_bs=nco_free(vp_bs);

  }else{
    /* Write data in native machine ordering. Easy Peasy! */
    wrt_nbr=fwrite(vp,wrd_sz,(size_t)var_sz,fp_bnr);
  } /* !flg_byt_swp */
  
  if(wrt_nbr != var_sz){
    (void)fprintf(stderr,"%s: ERROR only succeeded in writing %ld of %ld elements of variable %s\n",nco_prg_nm_get(),wrt_nbr,var_sz,var_nm);
    nco_exit(EXIT_FAILURE);
  } /* end if */
  if(nco_dbg_lvl_get() >= nco_dbg_scl) (void)fprintf(stdout,"%s (%s, %ld x %lu B), ",var_nm,c_typ_nm(var_typ),var_sz,(unsigned long)nco_typ_lng(var_typ));
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
