/* $Header$ */

/* Purpose: Streams */

/* Copyright (C) 1995--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

#include "nco_srm.h" /* Streams */

void 
nco_srm_hdr /* [fnc] Write header to stream */
(void)
{
  /* Purpose: Write header to stream
     ncstream grammar explained at
     https://www.unidata.ucar.edu/software/thredds/current/netcdf-java/stream/NcstreamGrammer.html

     Test with:
     ncks --srm ~/nco/data/in.nc > ~/in.ncs
     ncks --srm ~/nco/data/in.nc */

  nco_srm_t nco_srm; /* [sct] Stream */

  /* From draft:
     MAGIC_START = 0x43, 0x44, 0x46, 0x53
     MAGIC_HEADER= 0xad, 0xec, 0xce, 0xda 
     MAGIC_DATA =  0xab, 0xec, 0xce, 0xba 
     MAGIC_VDATA = 0xab, 0xef, 0xfe, 0xba 
     MAGIC_VEND  = 0xed, 0xef, 0xfe, 0xda 
     MAGIC_ERR   = 0xab, 0xad, 0xba, 0xda 
     MAGIC_END =   0xed, 0xed, 0xde, 0xde */

  nco_srm_mgc_t nco_srm_mgc_srt[]={0x43,0x44,0x46,0x53}; /* [sng] "CDFS" */
  // nco_srm_mgc_t nco_srm_mgc_hdr[]={0xad,0xec,0xce,0xda};
  // nco_srm_mgc_t nco_srm_mgc_dta[]={0xab,0xec,0xce,0xba};
  // nco_srm_mgc_t nco_srm_mgc_vdt[]={0xab,0xef,0xfe,0xba};
  // nco_srm_mgc_t nco_srm_mgc_vnd[]={0xed,0xef,0xfe,0xda};
  // nco_srm_mgc_t nco_srm_mgc_err[]={0xab,0xad,0xba,0xda};
  // nco_srm_mgc_t nco_srm_mgc_end[]={0xed,0xed,0xde,0xde};

  /* fxm: there must be a better way... */
  memcpy(nco_srm.mgc_srt,nco_srm_mgc_srt,sizeof(nco_srm_mgc_srt)/sizeof(nco_srm_mgc_t));

  void *vp;
  vp=(void *)nco_srm.mgc_srt;

  (void)fwrite(vp,sizeof(nco_srm_mgc_t),(size_t)4,stdout);
} /* end nco_srm_hdr() */
