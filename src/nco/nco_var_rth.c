/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_var_rth.c,v 1.95 2015-02-08 18:19:16 zender Exp $ */

/* Purpose: Variable arithmetic */

/* Copyright (C) 1995--2015 Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   GNU General Public License (GPL) Version 3 with exceptions described in the LICENSE file */

#include "nco_var_rth.h" /* Variable arithmetic */

void
nco_var_abs /* [fnc] Replace op1 values by their absolute values */
(const nc_type type, /* I [enm] netCDF type of operand */
 const long sz, /* I [nbr] Size (in elements) of operand */
 const int has_mss_val, /* I [flg] Flag for missing values */
 ptr_unn mss_val, /* I [val] Value of missing value */
 ptr_unn op1) /* I/O [val] Values of first operand */
{
  /* Threads: Routine is thread safe and calls no unsafe routines */
  /* Purpose: Replace op1 values by their absolute values */
  
  /* Absolute value is currently defined as op1:=abs(op1) */  
  
  /* NB: Many compilers need to #include "nco_rth_flt.h" for fabsf() prototype */
  
  long idx;
  
  /* Typecast pointer to values before access */
  (void)cast_void_nctype(type,&op1);
  if(has_mss_val) (void)cast_void_nctype(type,&mss_val);
  
  switch(type){
  case NC_FLOAT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.fp[idx]=fabsf(op1.fp[idx]);
    }else{
      const float mss_val_flt=*mss_val.fp;
      for(idx=0;idx<sz;idx++){
	if(op1.fp[idx] != mss_val_flt) op1.fp[idx]=fabsf(op1.fp[idx]); 
      } /* end for */
    } /* end else */
    break;
  case NC_DOUBLE:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.dp[idx]=fabs(op1.dp[idx]);
    }else{
      const double mss_val_dbl=*mss_val.dp;
      for(idx=0;idx<sz;idx++){
	if(op1.dp[idx] != mss_val_dbl) op1.dp[idx]=fabs(op1.dp[idx]);
      } /* end for */
    } /* end else */
    break;
  case NC_INT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.ip[idx]=labs(op1.ip[idx]); /* int abs(int), long labs(long) */
    }else{
      const nco_int mss_val_ntg=*mss_val.ip;
      for(idx=0;idx<sz;idx++){
	if(op1.ip[idx] != mss_val_ntg) op1.ip[idx]=labs(op1.ip[idx]); /* int abs(int), long labs(long) */
      } /* end for */
    } /* end else */
    break;
  case NC_SHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) if(op1.sp[idx] < 0) op1.sp[idx]=-op1.sp[idx] ;
    }else{
      const nco_short mss_val_short=*mss_val.sp;
      for(idx=0;idx<sz;idx++){
	if(op1.sp[idx] != mss_val_short && op1.sp[idx] < 0) op1.sp[idx]=-op1.sp[idx];
      } /* end for */
    } /* end else */
    break;
  case NC_BYTE:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) if(op1.bp[idx] < 0) op1.bp[idx]=-op1.bp[idx] ;
    }else{
      const nco_byte mss_val_byte=*mss_val.bp;
      for(idx=0;idx<sz;idx++){
	if(op1.bp[idx] != mss_val_byte && op1.bp[idx] < 0) op1.bp[idx]=-op1.bp[idx];
      } /* end for */
    } /* end else */
  case NC_UBYTE: break; /* Do nothing */
  case NC_USHORT: break; /* Do nothing */
  case NC_UINT: break; /* Do nothing */
  case NC_INT64: 
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.i64p[idx]=llabs(op1.i64p[idx]);
    }else{
      const nco_int64 mss_val_int64=*mss_val.i64p;
      for(idx=0;idx<sz;idx++){
	if(op1.i64p[idx] != mss_val_int64) op1.i64p[idx]=llabs(op1.i64p[idx]);
      } /* end for */
    } /* end else */
    break;
  case NC_UINT64: break; /* Do nothing */
  case NC_CHAR: break; /* Do nothing */
  case NC_STRING: break; /* Do nothing */
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
  
  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */
  
} /* end nco_var_abs() */

void
nco_var_around /* [fnc] Replace op1 values by their values rounded to decimal precision prc */
(const int ppc, /* I [nbr] Precision-preserving compression, i.e., number of total or decimal significant digits */
 const nc_type type, /* I [enm] netCDF type of operand */
 const long sz, /* I [nbr] Size (in elements) of operand */
 const int has_mss_val, /* I [flg] Flag for missing values */
 ptr_unn mss_val, /* I [val] Value of missing value */
 ptr_unn op1) /* I/O [val] Values of first operand */
{
  /* Threads: Routine is thread safe and calls no unsafe routines */

  /* Purpose: Replace op1 values by their values rounded to decimal precision ppc
     Similar to numpy.around() function, hence the name around()
     Based on implementation by Jeff Whitaker for netcdf4-python described here:
     http://netcdf4-python.googlecode.com/svn/trunk/docs/netCDF4-module.html
     which invokes the numpy.around() function documented here:
     http://docs.scipy.org/doc/numpy/reference/generated/numpy.around.html#numpy.around
     A practical discussion of rounding is at
     http://stackoverflow.com/questions/20388071/what-are-the-under-the-hood-differences-between-round-and-numpy-round
     This mentions the () NumPy source code:
     https://github.com/numpy/numpy/blob/7b2f20b406d27364c812f7a81a9c901afbd3600c/numpy/core/src/multiarray/calculation.c#L588

     Manually determine scale:
     ncap2 -O -v -s 'ppc=2;ppc_abs=abs(ppc);bit_nbr_xct=ppc_abs*ln(10.)/ln(2.);bit_nbr_int=ceil(bit_nbr_xct);scale=pow(2.0,bit_nbr_int);' ~/nco/data/in.nc ~/foo.nc 
     ncks -H ~/foo.nc

     Test full algorithm:
     ncks -4 -O -C -v ppc_dbl,ppc_big --ppc ppc_dbl=3 --ppc ppc_big=-2 ~/nco/data/in.nc ~/foo.nc

     Compare to Jeff Whitaker's nc3tonc4 results:
     nc3tonc4 -o --quantize=ppc_dbl=3,ppc_big=-2 ~/nco/data/in.nc ~/foo.nc
     ncks -H -C -v ppc_dbl,ppc_big ~/foo.nc */
  
  /* Rounding is currently defined as op1:=around(op1,ppc) */  
  
  /* Use constants defined in math.h */
  const double bit_per_dcm_dgt_prc=M_LN10/M_LN2; /* 3.32 [frc] Bits per decimal digit of precision */

  double scale; /* [frc] Number by which to scale data to achieve rounding */
  float scalef; /* [frc] Number by which to scale data to achieve rounding */

  int bit_nbr; /* [nbr] Number of bits required to exceed pow(10,-ppc) */
  int ppc_abs; /* [nbr] Absolute value of precision */

  long idx;
  
  ppc_abs=abs(ppc);
  assert(ppc_abs <= 16);
  switch(ppc_abs){
  case 0:
    bit_nbr=0;
    scale=1.0;
    break;
  case 1:
    bit_nbr=4;
    scale=16.0;
    break;
  case 2:
    bit_nbr=7;
    scale=128.0;
    break;
  case 3:
    bit_nbr=10;
    scale=1024.0;
    break;
  case 4:
    bit_nbr=14;
    scale=16384.0;
    break;
  case 5:
    bit_nbr=17;
    scale=131072.0;
    break;
  case 6:
    bit_nbr=20;
    scale=1048576.0;
    break;
  default:
    bit_nbr=(int)ceil(ppc_abs*bit_per_dcm_dgt_prc);
    scale=pow(2.0,bit_nbr);
    break;
  } /* end switch */   
  if(ppc < 0) scale=1.0/scale;

  if(nco_dbg_lvl_get() == nco_dbg_sbr) (void)fprintf(stdout,"%s: INFO nco_var_around() reports ppc = %d, bit_nbr= %d, scale = %g\n",nco_prg_nm_get(),ppc,bit_nbr,scale);

  /* Typecast pointer to values before access */
  (void)cast_void_nctype(type,&op1);
  if(has_mss_val) (void)cast_void_nctype(type,&mss_val);
  
  scalef=(float)scale;
  switch(type){
  case NC_FLOAT: 
    /* By default do float arithmetic in double precision before converting back to float
       Allow --flt to override
       NB: Use rint() not lrint()
       If ignoring this advice, be sure to bound calls to lrint(), e.g., 
       rint_arg=scale*op1.fp[idx];
       if(rint_arg > LONG_MIN && rint_arg < LONG_MAX) op1.fp[idx]=(float)lrint(scale*op1.fp[idx])/scale; */
    if(!has_mss_val){
      if(nco_rth_cnv_get() == nco_rth_flt_flt)
	for(idx=0L;idx<sz;idx++) op1.fp[idx]=rintf(scalef*op1.fp[idx])/scalef;
      else
	for(idx=0L;idx<sz;idx++) op1.fp[idx]=(float)(rint(scale*op1.fp[idx])/scale);
    }else{
      const float mss_val_flt=*mss_val.fp;
      if(nco_rth_cnv_get() == nco_rth_flt_flt){
	for(idx=0;idx<sz;idx++)
	  if(op1.fp[idx] != mss_val_flt)
	    op1.fp[idx]=rintf(scalef*op1.fp[idx])/scalef;
      }else{
	for(idx=0;idx<sz;idx++)
	  if(op1.fp[idx] != mss_val_flt)
	    op1.fp[idx]=(float)(rint(scale*op1.fp[idx])/scale); /* Coerce to avoid implicit conversions warning */
      } /* end else */
    } /* end else */
    break;
  case NC_DOUBLE: 
    if(!has_mss_val){
      for(idx=0L;idx<sz;idx++) op1.dp[idx]=rint(scale*op1.dp[idx])/scale;
    }else{
      const double mss_val_dbl=*mss_val.dp;
      for(idx=0;idx<sz;idx++)
	if(op1.dp[idx] != mss_val_dbl) op1.dp[idx]=rint(scale*op1.dp[idx])/scale;
    } /* end else */
    break;
  case NC_INT: /* Do nothing for non-floating point types ...*/
  case NC_SHORT:
  case NC_CHAR:
  case NC_BYTE:
  case NC_UBYTE:
  case NC_USHORT:
  case NC_UINT:
  case NC_INT64:
  case NC_UINT64:
  case NC_STRING: break;
  default: 
    nco_dfl_case_nc_type_err();
    break;
  } /* end switch */

  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */
  
} /* end nco_var_around() */

void
nco_var_bitmask /* [fnc] Mask-out insignificant bits of significand */
(const int nsd, /* I [nbr] Number of significant digits, i.e., arithmetic precision */
 const nc_type type, /* I [enm] netCDF type of operand */
 const long sz, /* I [nbr] Size (in elements) of operand */
 const int has_mss_val, /* I [flg] Flag for missing values */
 ptr_unn mss_val, /* I [val] Value of missing value */
 ptr_unn op1) /* I/O [val] Values of first operand */
{
  /* Threads: Routine is thread safe and calls no unsafe routines */

  /* Purpose: Mask-out insignificant bits of op1 values */
  
  /* Rounding is currently defined as op1:=bitmask(op1,ppc) */  
  
  /* Number of Significant Digits (NSD) algorithm
     NSD based on absolute precision, i.e., number of digits in significand and in decimal scientific notation
     DSD based on precision relative to decimal point, i.e., number of digits before/after decimal point
     DSD is more often used colloquially, e.g., "thermometers measure temperature accurate to 1 degree C" 
     NSD is more often used scientifically, e.g., "thermometers measure temperature to three significant digits"
     These statements are both equivalent and describe the same instrument and data
     If data are stored in C or K then optimal specifications for each algorithm would be DSD=0 and NSD=3
     However, if data are stored in mK (milli-Kelvin) then optimal specifications would be DSD=-3 and NSD=3
     In other words, the number of significant digits (NSD) does not depend on the units of storage, but DSD does
     Hence NSD is more instrinsic and portable than DSD
     NSD requires only bit-shifting and bit-masking, no floating point math
     DSD is implemented with rounding techniques that rely on floating point math
     This makes DSD subject to accompanying overflow and underflow problems when exponent near MAX_EXP/2
     Thus NSD is faster, more accurate, and less ambiguous than DSD
     Nevertheless many users think in terms of DSD not NSD
     
     Terminology: 
     Decimal Precision is number of significant digits following decimal point (DSD)
     Arithmetic Precision is number of significant digits (NSD)
     "Arithmetic precision can also be defined with reference to a fixed number of decimal places (the number of digits following the decimal point). This second definition is useful in applications where the number of digits in the fractional part has particular importance, but it does not follow the rules of significance arithmetic." -- Wikipedia
     "A common convention in science and engineering is to express accuracy and/or precision implicitly by means of significant figures. Here, when not explicitly stated, the margin of error is understood to be one-half the value of the last significant place. For instance, a recording of 843.6 m, or 843.0 m, or 800.0 m would imply a margin of 0.05 m (the last significant place is the tenths place), while a recording of 8,436 m would imply a margin of error of 0.5 m (the last significant digits are the units)." -- Wikipedia
     
     Test NSD:
     nc3tonc4 -o --quantize=ppc_big=3,ppc_bgr=3,ppc_flt=3 --quiet=1 ~/nco/data/in.nc ~/foo_n34.nc
     ncks -D 1 -4 -O -C -v ppc_big,ppc_bgr,ppc_flt --ppc .?=3 ~/nco/data/in.nc ~/foo.nc
     ncks -C -v ppc_big,ppc_bgr ~/foo.nc
     ncks -s '%16.12e\n' -C -H -v ppc_big,ppc_bgr ~/foo_n34.nc */
  
  /* IEEE single- and double-precision significands have 24 and 53 bits of precision (prc_bnr)
     Decimal digits of precision (prc_dcm) obtained via prc_dcm=prc_bnr*ln(2)/ln(10) = 7.22 and 15.95, respectively
     Binary digits of precision (prc_bnr) obtained via prc_bnr=prc_dcm*ln(10)/ln(2) */
  
  /* Use constants defined in math.h */
  const double bit_per_dcm_dgt_prc=M_LN10/M_LN2; /* 3.32 [frc] Bits per decimal digit of precision */
  
  /* Preserve at least two explicit bits, plus one implicit bit means three total bits */
  /* [nbr] Minimum number of explicit significand bits to preserve */
#define BIT_XPL_NBR_MIN 2
  const int bit_xpl_nbr_sgn_flt=23; /* [nbr] Bits 0-22 of SP significands are explicit. Bit 23 is implicit. */
  const int bit_xpl_nbr_sgn_dbl=53; /* [nbr] Bits 0-52 of DP significands are explicit. Bit 53 is implicit. */
  
  double prc_bnr_xct; /* [nbr] Binary digits of precision, exact */
  
  int bit_xpl_nbr_sgn=int_CEWI; /* [nbr] Number of explicit bits in significand */
  int bit_xpl_nbr_zro; /* [nbr] Number of explicit bits to zero */

  long idx;

  unsigned int *u32_ptr;
  unsigned int msk_f32_u32_zro;
  unsigned int msk_f32_u32_one;
  unsigned long int *u64_ptr;
  unsigned long int msk_f64_u64_zro;
  unsigned long int msk_f64_u64_one;
  unsigned short prc_bnr_ceil; /* [nbr] Exact binary digits of precision rounded-up */
  unsigned short prc_bnr_xpl_rqr; /* [nbr] Explicitly represented binary digits required to retain */
  
  assert(nsd > 0);
  assert(nsd <= 16);

  /* How many bits to preserve? */
  prc_bnr_xct=nsd*bit_per_dcm_dgt_prc;
  /* Be conservative, round upwards */
  prc_bnr_ceil=(unsigned short)ceil(prc_bnr_xct);
  /* First bit is implicit not explicit but corner cases prevent our taking advantage of this */
  //prc_bnr_xpl_rqr=prc_bnr_ceil-1;
  //prc_bnr_xpl_rqr=prc_bnr_ceil;
  prc_bnr_xpl_rqr=prc_bnr_ceil+1;
  if(type == NC_DOUBLE) prc_bnr_xpl_rqr++; /* Seems necessary for double-precision ppc=array(1.234567,1.e-6,$dmn) */
  /* 20150128: Hand-tuning shows we can sacrifice one or two more bits for almost all cases
     20150205: However, small integers are an exception. In fact they require two more bits, at least for NSD=1.
     Thus minimum threshold to preserve half of least significant digit (LSD) is prc_bnr_xpl_rqr=prc_bnr_ceil.
     Decrementing prc_bnr_xpl_rqr by one or two more bits produces maximum errors that exceed half the LSD
     ncra -4 -O -C --ppc default=1 --ppc one=1 -p ~/nco/data in.nc in.nc ~/foo.nc 
     ncks -H -v Q.. --cdl ~/foo.nc | m */

  /* 20150126: fxm casting pointers is tricky with this routine. Avoid for now. */
  /* Typecast pointer to values before access */
  //(void)cast_void_nctype(type,&op1);
  //if(has_mss_val) (void)cast_void_nctype(type,&mss_val);

  switch(type){
  case NC_FLOAT:
    bit_xpl_nbr_sgn=bit_xpl_nbr_sgn_flt;
    bit_xpl_nbr_zro=bit_xpl_nbr_sgn-prc_bnr_xpl_rqr;
    assert(bit_xpl_nbr_zro <= bit_xpl_nbr_sgn-BIT_XPL_NBR_MIN);
    u32_ptr=op1.uip;
    /* Create mask */
    msk_f32_u32_zro=0u; /* Zero all bits */
    msk_f32_u32_zro=~msk_f32_u32_zro; /* Turn all bits to ones */
    /* Left shift zeros into bits to be rounded */
    msk_f32_u32_zro <<= bit_xpl_nbr_zro;
    msk_f32_u32_one=~msk_f32_u32_zro;
    if(!has_mss_val){
      for(idx=0L;idx<sz;idx+=2L) u32_ptr[idx]&=msk_f32_u32_zro;
      for(idx=1L;idx<sz;idx+=2L) u32_ptr[idx]|=msk_f32_u32_one;
    }else{
      const float mss_val_flt=*mss_val.fp;
      for(idx=0L;idx<sz;idx+=2L)
	if(op1.fp[idx] != mss_val_flt) u32_ptr[idx]&=msk_f32_u32_zro;
      for(idx=1L;idx<sz;idx+=2L)
	if(op1.fp[idx] != mss_val_flt) u32_ptr[idx]|=msk_f32_u32_one;
    } /* end else */
    break;
  case NC_DOUBLE:
    bit_xpl_nbr_sgn=bit_xpl_nbr_sgn_dbl;
    bit_xpl_nbr_zro=bit_xpl_nbr_sgn-prc_bnr_xpl_rqr;
    assert(bit_xpl_nbr_zro <= bit_xpl_nbr_sgn-BIT_XPL_NBR_MIN);
    u64_ptr=(unsigned long int *)op1.ui64p;
    /* Create mask */
    msk_f64_u64_zro=0ul; /* Zero all bits */
    msk_f64_u64_zro=~msk_f64_u64_zro; /* Turn all bits to ones */
    /* Left shift zeros into bits to be rounded */
    msk_f64_u64_zro <<= bit_xpl_nbr_zro;
    msk_f64_u64_one=~msk_f64_u64_zro;
    if(!has_mss_val){
      for(idx=0L;idx<sz;idx+=2L) u64_ptr[idx]&=msk_f64_u64_zro;
      for(idx=1L;idx<sz;idx+=2L) u64_ptr[idx]|=msk_f64_u64_one;
    }else{
      const double mss_val_dbl=*mss_val.dp;
      for(idx=0L;idx<sz;idx+=2L)
	if(op1.dp[idx] != mss_val_dbl) u64_ptr[idx]&=msk_f64_u64_zro;
      for(idx=1L;idx<sz;idx+=2L)
	if(op1.dp[idx] != mss_val_dbl) u64_ptr[idx]|=msk_f64_u64_one;
    } /* end else */
    break;
  case NC_INT: /* Do nothing for non-floating point types ...*/
  case NC_SHORT:
  case NC_CHAR:
  case NC_BYTE:
  case NC_UBYTE:
  case NC_USHORT:
  case NC_UINT:
  case NC_INT64:
  case NC_UINT64:
  case NC_STRING: break;
  default: 
    nco_dfl_case_nc_type_err();
    break;
  } /* end switch */
  
  /* 20150126: fxm casting pointers is tricky with this routine. Avoid for now. */
  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */

} /* end nco_var_bitmask() */

void
nco_var_add /* [fnc] Add first operand to second operand */
(const nc_type type, /* I [enm] netCDF type of operands */
 const long sz, /* I [nbr] Size (in elements) of operands */
 const int has_mss_val, /* I [flg] Flag for missing values */
 ptr_unn mss_val, /* I [flg] Value of missing value */
 ptr_unn op1, /* I [val] Values of first operand */
 ptr_unn op2) /* I/O [val] Values of second operand on input, values of sum on output */
{
  /* Purpose: Add value of first operand to value of second operand 
     and store result in second operand. 
     Assume operands conform, are same type, and are in memory
     nco_var_add() does _not_ increment tally counter
     nco_var_add_tll_ncra() does increment tally counter */
  
  /* Addition is currently defined as op2:=op1+op2 where op1 != mss_val and op2 != mss_val */
  
  long idx;
  
  /* Typecast pointer to values before access */
  (void)cast_void_nctype(type,&op1);
  (void)cast_void_nctype(type,&op2);
  if(has_mss_val) (void)cast_void_nctype(type,&mss_val);
  
  switch(type){
  case NC_FLOAT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.fp[idx]+=op1.fp[idx];
    }else{
      const float mss_val_flt=*mss_val.fp;
      for(idx=0;idx<sz;idx++){
	if((op2.fp[idx] != mss_val_flt) && (op1.fp[idx] != mss_val_flt)) op2.fp[idx]+=op1.fp[idx]; else op2.fp[idx]=mss_val_flt;
      } /* end for */
    } /* end else */
    break;
  case NC_DOUBLE:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.dp[idx]+=op1.dp[idx];
    }else{
      const double mss_val_dbl=*mss_val.dp;
      for(idx=0;idx<sz;idx++){
	if((op2.dp[idx] != mss_val_dbl) && (op1.dp[idx] != mss_val_dbl)) op2.dp[idx]+=op1.dp[idx]; else op2.dp[idx]=mss_val_dbl;
      } /* end for */
    } /* end else */
    break;
  case NC_INT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.ip[idx]+=op1.ip[idx];
    }else{
      const nco_int mss_val_ntg=*mss_val.ip;
      for(idx=0;idx<sz;idx++){
	if((op2.ip[idx] != mss_val_ntg) && (op1.ip[idx] != mss_val_ntg)) op2.ip[idx]+=op1.ip[idx]; else op2.ip[idx]=mss_val_ntg;
      } /* end for */
    } /* end else */
    break;
  case NC_SHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.sp[idx]+=op1.sp[idx];
    }else{
      const nco_short mss_val_short=*mss_val.sp;
      for(idx=0;idx<sz;idx++){
	if((op2.sp[idx] != mss_val_short) && (op1.sp[idx] != mss_val_short)) op2.sp[idx]+=op1.sp[idx]; else op2.sp[idx]=mss_val_short;
      } /* end for */
    } /* end else */
    break;
  case NC_USHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.usp[idx]+=op1.usp[idx];
    }else{
      const nco_ushort mss_val_ushort=*mss_val.usp;
      for(idx=0;idx<sz;idx++){
	if((op2.usp[idx] != mss_val_ushort) && (op1.usp[idx] != mss_val_ushort)) op2.usp[idx]+=op1.usp[idx]; else op2.usp[idx]=mss_val_ushort;
      } /* end for */
    } /* end else */
    break;
  case NC_UINT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.uip[idx]+=op1.uip[idx];
    }else{
      const nco_uint mss_val_uint=*mss_val.uip;
      for(idx=0;idx<sz;idx++){
	if((op2.uip[idx] != mss_val_uint) && (op1.uip[idx] != mss_val_uint)) op2.uip[idx]+=op1.uip[idx]; else op2.uip[idx]=mss_val_uint;
      } /* end for */
    } /* end else */
    break;
  case NC_INT64:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.i64p[idx]+=op1.i64p[idx];
    }else{
      const nco_int64 mss_val_int64=*mss_val.i64p;
      for(idx=0;idx<sz;idx++){
	if((op2.i64p[idx] != mss_val_int64) && (op1.i64p[idx] != mss_val_int64)) op2.i64p[idx]+=op1.i64p[idx]; else op2.i64p[idx]=mss_val_int64;
      } /* end for */
    } /* end else */
    break;
  case NC_UINT64:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.ui64p[idx]+=op1.ui64p[idx];
    }else{
      const nco_uint64 mss_val_uint64=*mss_val.ui64p;
      for(idx=0;idx<sz;idx++){
	if((op2.ui64p[idx] != mss_val_uint64) && (op1.ui64p[idx] != mss_val_uint64)) op2.ui64p[idx]+=op1.ui64p[idx]; else op2.ui64p[idx]=mss_val_uint64;
      } /* end for */
    } /* end else */
    break;
  case NC_BYTE:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.bp[idx]+=op1.bp[idx];
    }else{
      const nco_byte mss_val_byte=*mss_val.bp;
      for(idx=0;idx<sz;idx++){
	if((op2.bp[idx] != mss_val_byte) && (op1.bp[idx] != mss_val_byte)) op2.bp[idx]+=op1.bp[idx]; else op2.bp[idx]=mss_val_byte;
      } /* end for */
    } /* end else */
    break;
  case NC_UBYTE:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.ubp[idx]+=op1.ubp[idx];
    }else{
      const nco_ubyte mss_val_ubyte=*mss_val.ubp;
      for(idx=0;idx<sz;idx++){
	if((op2.ubp[idx] != mss_val_ubyte) && (op1.ubp[idx] != mss_val_ubyte)) op2.ubp[idx]+=op1.ubp[idx]; else op2.ubp[idx]=mss_val_ubyte;
      } /* end for */
    } /* end else */
    break;
  case NC_CHAR: break; /* Do nothing */
  case NC_STRING: break; /* Do nothing */
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
  
  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */
  
} /* end nco_var_add() */

void
nco_var_add_tll_ncflint /* [fnc] Add first operand to second operand, increment tally */
(const nc_type type, /* I [enm] netCDF type of operands */
 const long sz, /* I [nbr] Size (in elements) of operands */
 const int has_mss_val, /* I [flg] Flag for missing values */
 ptr_unn mss_val, /* I [flg] Value of missing value */
 long * restrict const tally, /* I/O [nbr] Counter space */
 ptr_unn op1, /* I [val] Values of first operand */
 ptr_unn op2) /* I/O [val] Values of second operand on input, values of sum on output */
{
  /* Purpose: Add value of first operand to value of second operand 
     and store result in second operand. 
     Assume operands conform, are same type, and are in memory
     nco_var_add() does _not_ increment tally counter
     nco_var_add_tll_ncflint() does increment tally counter */
  
  /* Addition is currently defined as op2:=op1+op2 where op1 != mss_val and op2 != mss_val */
  
  long idx;
  
  /* Typecast pointer to values before access */
  (void)cast_void_nctype(type,&op1);
  (void)cast_void_nctype(type,&op2);
  if(has_mss_val) (void)cast_void_nctype(type,&mss_val);
  
  /* Return missing_value where either or both input values are missing 
     Algorithm used since 20040603 
     NB: Tally is incremented but not used */
  switch(type){
  case NC_FLOAT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.fp[idx]+=op1.fp[idx];
	tally[idx]++;
      } /* end for */
    }else{
      const float mss_val_flt=*mss_val.fp;
      for(idx=0;idx<sz;idx++){
	if((op2.fp[idx] != mss_val_flt) && (op1.fp[idx] != mss_val_flt)){
	  op2.fp[idx]+=op1.fp[idx];
	  tally[idx]++;
	}else{
	  op2.fp[idx]=mss_val_flt;
	}/* end else */
      } /* end for */
    } /* end else */
    break;
  case NC_DOUBLE:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.dp[idx]+=op1.dp[idx];
	tally[idx]++;
      } /* end for */
    }else{
      const double mss_val_dbl=*mss_val.dp;
      for(idx=0;idx<sz;idx++){
	if((op2.dp[idx] != mss_val_dbl) && (op1.dp[idx] != mss_val_dbl)){
	  op2.dp[idx]+=op1.dp[idx];
	  tally[idx]++;
	}else{
	  op2.dp[idx]=mss_val_dbl;
	} /* end else */
      } /* end for */
    } /* end else */
    break;
  case NC_INT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.ip[idx]+=op1.ip[idx];
	tally[idx]++;
      } /* end for */
    }else{
      const nco_int mss_val_ntg=*mss_val.ip;
      for(idx=0;idx<sz;idx++){
	if((op2.ip[idx] != mss_val_ntg) && (op1.ip[idx] != mss_val_ntg)){
	  op2.ip[idx]+=op1.ip[idx];
	  tally[idx]++;
	}else{
	  op2.ip[idx]=mss_val_ntg;
	} /* end else */
      } /* end for */
    } /* end else */
    break;
  case NC_SHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.sp[idx]+=op1.sp[idx];
	tally[idx]++;
      } /* end for */
    }else{
      const nco_short mss_val_short=*mss_val.sp;
      for(idx=0;idx<sz;idx++){
	if((op2.sp[idx] != mss_val_short) && (op1.sp[idx] != mss_val_short)){
	  op2.sp[idx]+=op1.sp[idx];
	  tally[idx]++;
	}else{
	  op2.sp[idx]=mss_val_short;
	} /* end else */
      } /* end for */
    } /* end else */
    break;
  case NC_USHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.usp[idx]+=op1.usp[idx];
	tally[idx]++;
      } /* end for */
    }else{
      const nco_ushort mss_val_ushort=*mss_val.usp;
      for(idx=0;idx<sz;idx++){
	if((op2.usp[idx] != mss_val_ushort) && (op1.usp[idx] != mss_val_ushort)){
	  op2.usp[idx]+=op1.usp[idx];
	  tally[idx]++;
	}else{
	  op2.usp[idx]=mss_val_ushort;
	} /* end else */
      } /* end for */
    } /* end else */
    break;
  case NC_UINT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.uip[idx]+=op1.uip[idx];
	tally[idx]++;
      } /* end for */
    }else{
      const nco_uint mss_val_uint=*mss_val.uip;
      for(idx=0;idx<sz;idx++){
	if((op2.uip[idx] != mss_val_uint) && (op1.uip[idx] != mss_val_uint)){
	  op2.uip[idx]+=op1.uip[idx];
	  tally[idx]++;
	}else{
	  op2.uip[idx]=mss_val_uint;
	} /* end else */
      } /* end for */
    } /* end else */
    break;
  case NC_INT64:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.i64p[idx]+=op1.i64p[idx];
	tally[idx]++;
      } /* end for */
    }else{
      const nco_int64 mss_val_int64=*mss_val.i64p;
      for(idx=0;idx<sz;idx++){
	if((op2.i64p[idx] != mss_val_int64) && (op1.i64p[idx] != mss_val_int64)){
	  op2.i64p[idx]+=op1.i64p[idx];
	  tally[idx]++;
	}else{
	  op2.i64p[idx]=mss_val_int64;
	} /* end else */
      } /* end for */
    } /* end else */
    break;
  case NC_UINT64:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.ui64p[idx]+=op1.ui64p[idx];
	tally[idx]++;
      } /* end for */
    }else{
      const nco_uint64 mss_val_uint64=*mss_val.ui64p;
      for(idx=0;idx<sz;idx++){
	if((op2.ui64p[idx] != mss_val_uint64) && (op1.ui64p[idx] != mss_val_uint64)){
	  op2.ui64p[idx]+=op1.ui64p[idx];
	  tally[idx]++;
	}else{
	  op2.ui64p[idx]=mss_val_uint64;
	} /* end else */
      } /* end for */
    } /* end else */
    break;
  case NC_BYTE:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.bp[idx]+=op1.bp[idx];
	tally[idx]++;
      } /* end for */
    }else{
      const nco_byte mss_val_byte=*mss_val.bp;
      for(idx=0;idx<sz;idx++){
	if((op2.bp[idx] != mss_val_byte) && (op1.bp[idx] != mss_val_byte)){
	  op2.bp[idx]+=op1.bp[idx];
	  tally[idx]++;
	}else{
	  op2.bp[idx]=mss_val_byte;
	} /* end else */
      } /* end for */
    } /* end else */
    break;
  case NC_UBYTE:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.ubp[idx]+=op1.ubp[idx];
	tally[idx]++;
      } /* end for */
    }else{
      const nco_ubyte mss_val_ubyte=*mss_val.ubp;
      for(idx=0;idx<sz;idx++){
	if((op2.ubp[idx] != mss_val_ubyte) && (op1.ubp[idx] != mss_val_ubyte)){
	  op2.ubp[idx]+=op1.ubp[idx];
	  tally[idx]++;
	}else{
	  op2.ubp[idx]=mss_val_ubyte;
	} /* end else */
      } /* end for */
    } /* end else */
    break;
  case NC_CHAR: break; /* Do nothing */
  case NC_STRING: break; /* Do nothing */
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
  
  /* Used this block of code until 20040603. 
     It keeps track of tally but does not do anything with it later */
#if 0
  switch(type){
  case NC_FLOAT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.fp[idx]+=op1.fp[idx];
	tally[idx]++;
      } /* end for */
    }else{
      const float mss_val_flt=*mss_val.fp;
      for(idx=0;idx<sz;idx++){
	if((op2.fp[idx] != mss_val_flt) && (op1.fp[idx] != mss_val_flt)){
	  op2.fp[idx]+=op1.fp[idx];
	  tally[idx]++;
	} /* end if */
      } /* end for */
    } /* end else */
    break;
  case NC_DOUBLE:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.dp[idx]+=op1.dp[idx];
	tally[idx]++;
      } /* end for */
    }else{
      const double mss_val_dbl=*mss_val.dp;
      for(idx=0;idx<sz;idx++){
	if((op2.dp[idx] != mss_val_dbl) && (op1.dp[idx] != mss_val_dbl)){
	  op2.dp[idx]+=op1.dp[idx];
	  tally[idx]++;
	} /* end if */
      } /* end for */
    } /* end else */
    break;
  case NC_INT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.ip[idx]+=op1.ip[idx];
	tally[idx]++;
      } /* end for */
    }else{
      const nco_int mss_val_ntg=*mss_val.ip;
      for(idx=0;idx<sz;idx++){
	if((op2.ip[idx] != mss_val_ntg) && (op1.ip[idx] != mss_val_ntg)){
	  op2.ip[idx]+=op1.ip[idx];
	  tally[idx]++;
	} /* end if */
      } /* end for */
    } /* end else */
    break;
  case NC_SHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.sp[idx]+=op1.sp[idx];
	tally[idx]++;
      } /* end for */
    }else{
      const nco_short mss_val_short=*mss_val.sp;
      for(idx=0;idx<sz;idx++){
	if((op2.sp[idx] != mss_val_short) && (op1.sp[idx] != mss_val_short)){
	  op2.sp[idx]+=op1.sp[idx];
	  tally[idx]++;
	} /* end if */
      } /* end for */
    } /* end else */
    break;
  case NC_USHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.usp[idx]+=op1.usp[idx];
	tally[idx]++;
      } /* end for */
    }else{
      const nco_ushort mss_val_ushort=*mss_val.usp;
      for(idx=0;idx<sz;idx++){
	if((op2.usp[idx] != mss_val_ushort) && (op1.usp[idx] != mss_val_ushort)){
	  op2.usp[idx]+=op1.usp[idx];
	  tally[idx]++;
	} /* end if */
      } /* end for */
    } /* end else */
    break;
  case NC_UINT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.uip[idx]+=op1.uip[idx];
	tally[idx]++;
      } /* end for */
    }else{
      const nco_uint mss_val_uint=*mss_val.uip;
      for(idx=0;idx<sz;idx++){
	if((op2.uip[idx] != mss_val_uint) && (op1.uip[idx] != mss_val_uint)){
	  op2.uip[idx]+=op1.uip[idx];
	  tally[idx]++;
	} /* end if */
      } /* end for */
    } /* end else */
    break;
  case NC_INT64:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.i64p[idx]+=op1.i64p[idx];
	tally[idx]++;
      } /* end for */
    }else{
      const nco_int64 mss_val_int64=*mss_val.i64p;
      for(idx=0;idx<sz;idx++){
	if((op2.i64p[idx] != mss_val_int64) && (op1.i64p[idx] != mss_val_int64)){
	  op2.i64p[idx]+=op1.i64p[idx];
	  tally[idx]++;
	} /* end if */
      } /* end for */
    } /* end else */
    break;
  case NC_UINT64:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.ui64p[idx]+=op1.ui64p[idx];
	tally[idx]++;
      } /* end for */
    }else{
      const nco_uint64 mss_val_uint64=*mss_val.ui64p;
      for(idx=0;idx<sz;idx++){
	if((op2.ui64p[idx] != mss_val_uint64) && (op1.ui64p[idx] != mss_val_uint64)){
	  op2.ui64p[idx]+=op1.ui64p[idx];
	  tally[idx]++;
	} /* end if */
      } /* end for */
    } /* end else */
    break;
  case NC_BYTE:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.bp[idx]+=op1.bp[idx];
	tally[idx]++;
      } /* end for */
    }else{
      const nco_byte mss_val_byte=*mss_val.bp;
      for(idx=0;idx<sz;idx++){
	if((op2.bp[idx] != mss_val_byte) && (op1.bp[idx] != mss_val_byte)){
	  op2.bp[idx]+=op1.bp[idx];
	  tally[idx]++;
	} /* end if */
      } /* end for */
    } /* end else */
    break;
  case NC_UBYTE:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.ubp[idx]+=op1.ubp[idx];
	tally[idx]++;
      } /* end for */
    }else{
      const nco_ubyte mss_val_ubyte=*mss_val.ubp;
      for(idx=0;idx<sz;idx++){
	if((op2.ubp[idx] != mss_val_ubyte) && (op1.ubp[idx] != mss_val_ubyte)){
	  op2.ubp[idx]+=op1.ubp[idx];
	  tally[idx]++;
	} /* end if */
      } /* end for */
    } /* end else */
    break;
  case NC_CHAR: break; /* Do nothing */
  case NC_STRING: break; /* Do nothing */
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
#endif /* endif 0 */
  
  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */
  
} /* end nco_var_add_tll_ncflint() */

void
nco_var_add_tll_ncra /* [fnc] Add first operand to second operand, increment tally */
(const nc_type type, /* I [enm] netCDF type of operands */
 const long sz, /* I [nbr] Size (in elements) of operands */
 const int has_mss_val, /* I [flg] Flag for missing values */
 ptr_unn mss_val, /* I [flg] Value of missing value */
 long * restrict const tally, /* I/O [nbr] Counter space */
 ptr_unn op1, /* I [val] Values of first operand */
 ptr_unn op2) /* I/O [val] Values of second operand (running sum) on input, values of new sum on output */
{
  /* Purpose: Add value of first operand to value of second operand 
     and store result in second operand. 
     Assume operands conform, are same type, and are in memory
     nco_var_add() does _not_ increment tally counter.
     nco_var_add_tll_ncflint() adds if neither operand equals missing value
     nco_var_add_tll_ncflint() does increment tally counter (unlike nco_var_add())
     nco_var_add_tll_ncra() adds if op1 does not equal missing value
     nco_var_add_tll_ncra() does increment tally counter (like nco_var_add_tll_ncflint())
     nco_var_add_tll_ncra() is designed to:
     1. Work for "running average" algorithms only
     2. Assume running sum is valid and is stored in op2
     3. Assume new record is stored in op1
     4. Check only if new record (not running sum) equals missing_value
     Note that missing_value is associated with op1, i.e., new record, not running sum
     5. Accumulate running sum only if new record is valid
     6. Increment tally
     Difference between nco_var_add_tll_ncra() and nco_var_add_tll_ncflint() is that
     nco_var_add_tll_ncflint() checks both operands against the missing_value, whereas 
     nco_var_add_tll_ncra() only checks first operand (new record) against missing_value
     nco_var_add_tll_ncflint() algorithm fails as running average algorithm when
     missing value is zero because running sum is bootstrapped to zero and this
     causes comparison to missing_value to always be true.
     nco_var_add_tll_ncflint() also fails as running average algorithm whenever
     running sum happens to equal missing_value (regardless if missing value is zero).
     NCO uses nco_var_add_tll_ncflint() only for ncflint
     NCO uses nco_var_add_tll_ncra() only for ncra/nces */
  
  /* Addition is currently defined as op2:=op1+op2 where op1 != mss_val */
  
  long idx;
  
  /* Typecast pointer to values before access */
  (void)cast_void_nctype(type,&op1);
  (void)cast_void_nctype(type,&op2);
  if(has_mss_val) (void)cast_void_nctype(type,&mss_val);
  
  switch(type){
  case NC_FLOAT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.fp[idx]+=op1.fp[idx];
	tally[idx]++;
      } /* end for */
    }else{
      const float mss_val_flt=*mss_val.fp;
      for(idx=0;idx<sz;idx++){
	if(op1.fp[idx] != mss_val_flt){
	  op2.fp[idx]+=op1.fp[idx];
	  tally[idx]++;
	} /* end if */
      } /* end for */
    } /* end else */
    break;
  case NC_DOUBLE:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.dp[idx]+=op1.dp[idx];
	tally[idx]++;
      } /* end for */
    }else{
      const double mss_val_dbl=*mss_val.dp;
      for(idx=0;idx<sz;idx++){
	if(op1.dp[idx] != mss_val_dbl){
	  op2.dp[idx]+=op1.dp[idx];
	  tally[idx]++;
	} /* end if */
      } /* end for */
    } /* end else */
    break;
  case NC_INT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.ip[idx]+=op1.ip[idx];
	tally[idx]++;
      } /* end for */
    }else{
      const nco_int mss_val_ntg=*mss_val.ip;
      for(idx=0;idx<sz;idx++){
	if(op1.ip[idx] != mss_val_ntg){
	  op2.ip[idx]+=op1.ip[idx];
	  tally[idx]++;
	} /* end if */
      } /* end for */
    } /* end else */
    break;
  case NC_SHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.sp[idx]+=op1.sp[idx];
	tally[idx]++;
      } /* end for */
    }else{
      const nco_short mss_val_short=*mss_val.sp;
      for(idx=0;idx<sz;idx++){
	if(op1.sp[idx] != mss_val_short){
	  op2.sp[idx]+=op1.sp[idx];
	  tally[idx]++;
	} /* end if */
      } /* end for */
    } /* end else */
    break;
  case NC_USHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.usp[idx]+=op1.usp[idx];
	tally[idx]++;
      } /* end for */
    }else{
      const nco_ushort mss_val_ushort=*mss_val.usp;
      for(idx=0;idx<sz;idx++){
	if(op1.usp[idx] != mss_val_ushort){
	  op2.usp[idx]+=op1.usp[idx];
	  tally[idx]++;
	} /* end if */
      } /* end for */
    } /* end else */
    break;
  case NC_UINT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.uip[idx]+=op1.uip[idx];
	tally[idx]++;
      } /* end for */
    }else{
      const nco_uint mss_val_uint=*mss_val.uip;
      for(idx=0;idx<sz;idx++){
	if(op1.uip[idx] != mss_val_uint){
	  op2.uip[idx]+=op1.uip[idx];
	  tally[idx]++;
	} /* end if */
      } /* end for */
    } /* end else */
    break;
  case NC_INT64:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.i64p[idx]+=op1.i64p[idx];
	tally[idx]++;
      } /* end for */
    }else{
      const nco_int64 mss_val_int64=*mss_val.i64p;
      for(idx=0;idx<sz;idx++){
	if(op1.i64p[idx] != mss_val_int64){
	  op2.i64p[idx]+=op1.i64p[idx];
	  tally[idx]++;
	} /* end if */
      } /* end for */
    } /* end else */
    break;
  case NC_UINT64:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.ui64p[idx]+=op1.ui64p[idx];
	tally[idx]++;
      } /* end for */
    }else{
      const nco_uint64 mss_val_uint64=*mss_val.ui64p;
      for(idx=0;idx<sz;idx++){
	if(op1.ui64p[idx] != mss_val_uint64){
	  op2.ui64p[idx]+=op1.ui64p[idx];
	  tally[idx]++;
	} /* end if */
      } /* end for */
    } /* end else */
    break;
  case NC_BYTE:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.bp[idx]+=op1.bp[idx];
	tally[idx]++;
      } /* end for */
    }else{
      const nco_byte mss_val_byte=*mss_val.bp;
      for(idx=0;idx<sz;idx++){
	if(op1.bp[idx] != mss_val_byte){
	  op2.bp[idx]+=op1.bp[idx];
	  tally[idx]++;
	} /* end if */
      } /* end for */
    } /* end else */
    break;
  case NC_UBYTE:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.ubp[idx]+=op1.ubp[idx];
	tally[idx]++;
      } /* end for */
    }else{
      const nco_ubyte mss_val_ubyte=*mss_val.ubp;
      for(idx=0;idx<sz;idx++){
	if(op1.ubp[idx] != mss_val_ubyte){
	  op2.ubp[idx]+=op1.ubp[idx];
	  tally[idx]++;
	} /* end if */
      } /* end for */
    } /* end else */
    break;
  case NC_CHAR: break; /* Do nothing */
  case NC_STRING: break; /* Do nothing */
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
  
  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */
  
} /* end nco_var_add_tll_ncra() */

void 
nco_var_copy_tll /* [fnc] Copy hyperslab variables of type var_typ from op1 to op2, accounting for missing values in tally */
(const nc_type type, /* I [enm] netCDF type */
 const long sz, /* I [nbr] Number of elements to copy */
 const int has_mss_val, /* I [flg] Flag for missing values */
 ptr_unn mss_val, /* I [val] Value of missing value */
 long * restrict const tally, /* O [nbr] Counter space */
 const ptr_unn op1, /* I [sct] Values to copy */
 ptr_unn op2) /* O [sct] Destination to copy values to */
{
  /* Purpose: Copy hyperslab variables of type var_typ from op1 to op2
     Assumes memory area in op2 has already been malloc()'d
     Where the value copied is not equal to the missing value, set the tally to one
     nco_var_copy(): Does nothing with missing values and tallies
     nco_var_copy_tll(): Accounts for missing values in tally */

  /* Algorithm is currently defined as: op2:=op1 */

  long idx;

  /* Use fast nco_var_copy() method to copy variable */
  (void)memcpy((void *)(op2.vp),(void *)(op1.vp),sz*nco_typ_lng(type));

  if(has_mss_val){
    /* Tally remains zero until verified (below) that datum is not missing value */
    (void)nco_set_long(sz,0L,tally);
  }else{ /* !has_mss_val */
    /* Tally is one if no missing value is defined */
    (void)nco_set_long(sz,1L,tally);
    return;
  } /* !has_mss_val */

  /* Typecast pointer to values before access */
  (void)cast_void_nctype(type,&op2);
  if(has_mss_val) (void)cast_void_nctype(type,&mss_val);

  /* Overwrite one's with zero's where value equals missing value */
  switch(type){
  case NC_FLOAT:
    {
      const float mss_val_flt=*mss_val.fp;
      for(idx=0;idx<sz;idx++) if(op2.fp[idx] == mss_val_flt) op2.fp[idx]=0.0f; else tally[idx]=1L;
    }
    break;
  case NC_DOUBLE:
    {
      const double mss_val_dbl=*mss_val.dp;
      for(idx=0;idx<sz;idx++) if(op2.dp[idx] == mss_val_dbl) op2.dp[idx]=0.0; else tally[idx]=1L;
    }
    break;
  case NC_INT:
    {
      const nco_int mss_val_ntg=*mss_val.ip;
      for(idx=0;idx<sz;idx++) if(op2.ip[idx] == mss_val_ntg) op2.ip[idx]=0; else tally[idx]=1L;
    }
    break;
  case NC_SHORT:
    {
      const nco_short mss_val_short=*mss_val.sp;
      for(idx=0;idx<sz;idx++) if(op2.sp[idx] == mss_val_short) op2.sp[idx]=0; else tally[idx]=1L;
    }
    break;
  case NC_USHORT:
    {
      const nco_ushort mss_val_ushort=*mss_val.usp;
      for(idx=0;idx<sz;idx++) if(op2.usp[idx] == mss_val_ushort) op2.usp[idx]=0; else tally[idx]=1L;
    }
    break;
  case NC_UINT:
    {
      const nco_uint mss_val_uint=*mss_val.uip;
      for(idx=0;idx<sz;idx++) if(op2.uip[idx] == mss_val_uint) op2.uip[idx]=0; else tally[idx]=1L;
    }
    break;
  case NC_INT64:
    {
      const nco_int64 mss_val_int64=*mss_val.i64p;
      for(idx=0;idx<sz;idx++) if(op2.i64p[idx] == mss_val_int64) op2.i64p[idx]=0; else tally[idx]=1L;
    }
    break;
  case NC_UINT64:
    {
      const nco_uint64 mss_val_uint64=*mss_val.ui64p;
      for(idx=0;idx<sz;idx++) if(op2.ui64p[idx] == mss_val_uint64) op2.ui64p[idx]=0; else tally[idx]=1L;
    }
    break;
  case NC_BYTE:
    {
      const nco_byte mss_val_byte=*mss_val.bp;
      for(idx=0;idx<sz;idx++) if(op2.bp[idx] == mss_val_byte) op2.bp[idx]=0; else tally[idx]=1L;
    }
    break;
  case NC_UBYTE:
    {
      const nco_ubyte mss_val_ubyte=*mss_val.ubp;
      for(idx=0;idx<sz;idx++) if(op2.ubp[idx] == mss_val_ubyte) op2.ubp[idx]=0; else tally[idx]=1L;
    }
    break;
  case NC_CHAR: break; /* Do nothing */
  case NC_STRING: break; /* Do nothing */
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
  
  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */

} /* end nco_var_copy_tll() */

void
nco_var_dvd /* [fnc] Divide second operand by first operand */
(const nc_type type, /* I [type] netCDF type of operands */
 const long sz, /* I [nbr] Size (in elements) of operands */
 const int has_mss_val, /* I [flg] Flag for missing values */
 ptr_unn mss_val, /* I [flg] Value of missing value */
 ptr_unn op1, /* I [val] Values of denominator */
 ptr_unn op2) /* I/O [val] Values of numerator on input, values of quotient on output */
{
  /* Threads: Routine is thread safe and calls no unsafe routines */
  /* Purpose: Divide value of first operand by value of second operand 
     and store result in second operand. 
     Assume operands conform, are same type, and are in memory */
  
  /* Variable-variable division is currently defined as op2:=op2/op1 */  
  
  long idx;
  
  /* Typecast pointer to values before access */
  (void)cast_void_nctype(type,&op1);
  (void)cast_void_nctype(type,&op2);
  if(has_mss_val) (void)cast_void_nctype(type,&mss_val);
  
  switch(type){
  case NC_FLOAT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.fp[idx]/=op1.fp[idx];
    }else{
      const float mss_val_flt=*mss_val.fp;
      for(idx=0;idx<sz;idx++){
	if((op2.fp[idx] != mss_val_flt) && (op1.fp[idx] != mss_val_flt)) op2.fp[idx]/=op1.fp[idx]; else op2.fp[idx]=mss_val_flt;
      } /* end for */
    } /* end else */
    break; /* end NC_FLOAT */
  case NC_DOUBLE:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.dp[idx]/=op1.dp[idx];
    }else{
      const double mss_val_dbl=*mss_val.dp;
      for(idx=0;idx<sz;idx++){
	if((op2.dp[idx] != mss_val_dbl) && (op1.dp[idx] != mss_val_dbl)) op2.dp[idx]/=op1.dp[idx]; else op2.dp[idx]=mss_val_dbl;
      } /* end for */
    } /* end else */
    break; /* end NC_DOUBLE */
  case NC_INT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.ip[idx]/=op1.ip[idx];
    }else{
      const nco_int mss_val_ntg=*mss_val.ip;
      for(idx=0;idx<sz;idx++){
	if((op2.ip[idx] != mss_val_ntg) && (op1.ip[idx] != mss_val_ntg)) op2.ip[idx]/=op1.ip[idx]; else op2.ip[idx]=mss_val_ntg;
      } /* end for */
    } /* end else */
    break; /* end NC_INT */
  case NC_SHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.sp[idx]/=op1.sp[idx];
    }else{
      const nco_short mss_val_short=*mss_val.sp;
      for(idx=0;idx<sz;idx++){
	if((op2.sp[idx] != mss_val_short) && (op1.sp[idx] != mss_val_short)) op2.sp[idx]/=op1.sp[idx]; else op2.sp[idx]=mss_val_short;
      } /* end for */
    } /* end else */
    break; /* end NC_SHORT */
  case NC_USHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.usp[idx]/=op1.usp[idx];
    }else{
      const nco_ushort mss_val_ushort=*mss_val.usp;
      for(idx=0;idx<sz;idx++){
	if((op2.usp[idx] != mss_val_ushort) && (op1.usp[idx] != mss_val_ushort)) op2.usp[idx]/=op1.usp[idx]; else op2.usp[idx]=mss_val_ushort;
      } /* end for */
    } /* end else */
    break; /* end NC_USHORT */
  case NC_UINT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.uip[idx]/=op1.uip[idx];
    }else{
      const nco_uint mss_val_uint=*mss_val.uip;
      for(idx=0;idx<sz;idx++){
	if((op2.uip[idx] != mss_val_uint) && (op1.uip[idx] != mss_val_uint)) op2.uip[idx]/=op1.uip[idx]; else op2.uip[idx]=mss_val_uint;
      } /* end for */
    } /* end else */
    break; /* end NC_UINT */
  case NC_INT64:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.i64p[idx]/=op1.i64p[idx];
    }else{
      const nco_int64 mss_val_int64=*mss_val.i64p;
      for(idx=0;idx<sz;idx++){
	if((op2.i64p[idx] != mss_val_int64) && (op1.i64p[idx] != mss_val_int64)) op2.i64p[idx]/=op1.i64p[idx]; else op2.i64p[idx]=mss_val_int64;
      } /* end for */
    } /* end else */
    break; /* end NC_INT64 */
  case NC_UINT64:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.ui64p[idx]/=op1.ui64p[idx];
    }else{
      const nco_uint64 mss_val_uint64=*mss_val.ui64p;
      for(idx=0;idx<sz;idx++){
	if((op2.ui64p[idx] != mss_val_uint64) && (op1.ui64p[idx] != mss_val_uint64)) op2.ui64p[idx]/=op1.ui64p[idx]; else op2.ui64p[idx]=mss_val_uint64;
      } /* end for */
    } /* end else */
    break; /* end NC_UINT64 */
  case NC_BYTE:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.bp[idx]/=op1.bp[idx];
    }else{
      const nco_byte mss_val_byte=*mss_val.bp;
      for(idx=0;idx<sz;idx++){
	if((op2.bp[idx] != mss_val_byte) && (op1.bp[idx] != mss_val_byte)) op2.bp[idx]/=op1.bp[idx]; else op2.bp[idx]=mss_val_byte;
      } /* end for */
    } /* end else */
    break; /* end NC_BYTE */
  case NC_UBYTE:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.ubp[idx]/=op1.ubp[idx];
    }else{
      const nco_ubyte mss_val_ubyte=*mss_val.ubp;
      for(idx=0;idx<sz;idx++){
	if((op2.ubp[idx] != mss_val_ubyte) && (op1.ubp[idx] != mss_val_ubyte)) op2.ubp[idx]/=op1.ubp[idx]; else op2.ubp[idx]=mss_val_ubyte;
      } /* end for */
    } /* end else */
    break; /* end NC_UBYTE */
  case NC_CHAR: break; /* Do nothing */
  case NC_STRING: break; /* Do nothing */
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
  
  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */
  
} /* end nco_var_dvd() */

void
nco_var_max_bnr /* [fnc] Maximize two operands */
(const nc_type type, /* I [type] netCDF type of operands */
 const long sz, /* I [nbr] Size (in elements) of operands */
 const int has_mss_val, /* I [flg] Flag for missing values */
 ptr_unn mss_val, /* I [flg] Value of missing value */
 ptr_unn op1, /* I [val] Values of first operand */
 ptr_unn op2) /* I/O [val] Values of second operand on input, values of maximum on output */
{
  /* Purpose: Find maximum value(s) of two operands and store result in second operand 
     Operands are assumed to conform, be of same specified type, and have values in memory */
  long idx;
  
  /* Typecast pointer to values before access */
  /* It is not necessary to untype-cast pointer types after using them as we have 
     operated on local copies of them */
  (void)cast_void_nctype(type,&op1);
  (void)cast_void_nctype(type,&op2);
  if(has_mss_val) (void)cast_void_nctype(type,&mss_val);
  
  switch(type){
  case NC_FLOAT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) 
	if(op2.fp[idx] < op1.fp[idx]) op2.fp[idx]=op1.fp[idx];
    }else{
      const float mss_val_flt=*mss_val.fp;
      for(idx=0;idx<sz;idx++){
	if(op2.fp[idx] == mss_val_flt)
	  op2.fp[idx]=op1.fp[idx];
	else if((op1.fp[idx] != mss_val_flt) && (op2.fp[idx] < op1.fp[idx]))
	  op2.fp[idx]=op1.fp[idx]; 
      } /* end for */
    } /* end else */
    break;
  case NC_DOUBLE:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) 
	if(op2.dp[idx] < op1.dp[idx]) op2.dp[idx]=op1.dp[idx];
    }else{
      const double mss_val_dbl=*mss_val.dp;
      for(idx=0;idx<sz;idx++){
	if(op2.dp[idx] == mss_val_dbl) 
	  op2.dp[idx]=op1.dp[idx];
	else if((op1.dp[idx] != mss_val_dbl) && (op2.dp[idx] < op1.dp[idx]))
	  op2.dp[idx]=op1.dp[idx]; 
      } /* end for */
    } /* end else */
    break;
  case NC_INT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) 
	if(op2.ip[idx] < op1.ip[idx]) op2.ip[idx]=op1.ip[idx];
    }else{
      const nco_int mss_val_ntg=*mss_val.ip;
      for(idx=0;idx<sz;idx++){
	if(op2.ip[idx] == mss_val_ntg) 
	  op2.ip[idx]=op1.ip[idx];
	else if((op1.ip[idx] != mss_val_ntg) && (op2.ip[idx] < op1.ip[idx]))
	  op2.ip[idx]=op1.ip[idx]; 
      } /* end for */
    } /* end else */
    break;
  case NC_SHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) 
	if(op2.sp[idx] < op1.sp[idx]) op2.sp[idx]=op1.sp[idx];
    }else{
      const nco_short mss_val_short=*mss_val.sp;
      for(idx=0;idx<sz;idx++){
	if(op2.sp[idx] == mss_val_short) 
	  op2.sp[idx]=op1.sp[idx];
	else if((op1.sp[idx] != mss_val_short) && (op2.sp[idx] < op1.sp[idx]))
	  op2.sp[idx]=op1.sp[idx]; 
      } /* end for */
    } /* end else */
    break;
  case NC_USHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) 
	if(op2.usp[idx] < op1.usp[idx]) op2.usp[idx]=op1.usp[idx];
    }else{
      const nco_ushort mss_val_ushort=*mss_val.usp;
      for(idx=0;idx<sz;idx++){
	if(op2.usp[idx] == mss_val_ushort) 
	  op2.usp[idx]=op1.usp[idx];
	else if((op1.usp[idx] != mss_val_ushort) && (op2.usp[idx] < op1.usp[idx]))
	  op2.usp[idx]=op1.usp[idx]; 
      } /* end for */
    } /* end else */
    break;
  case NC_UINT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) 
	if(op2.uip[idx] < op1.uip[idx]) op2.uip[idx]=op1.uip[idx];
    }else{
      const nco_uint mss_val_uint=*mss_val.uip;
      for(idx=0;idx<sz;idx++){
	if(op2.uip[idx] == mss_val_uint) 
	  op2.uip[idx]=op1.uip[idx];
	else if((op1.uip[idx] != mss_val_uint) && (op2.uip[idx] < op1.uip[idx]))
	  op2.uip[idx]=op1.uip[idx]; 
      } /* end for */
    } /* end else */
    break;
  case NC_INT64:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) 
	if(op2.i64p[idx] < op1.i64p[idx]) op2.i64p[idx]=op1.i64p[idx];
    }else{
      const nco_int64 mss_val_int64=*mss_val.i64p;
      for(idx=0;idx<sz;idx++){
	if(op2.i64p[idx] == mss_val_int64) 
	  op2.i64p[idx]=op1.i64p[idx];
	else if((op1.i64p[idx] != mss_val_int64) && (op2.i64p[idx] < op1.i64p[idx]))
	  op2.i64p[idx]=op1.i64p[idx]; 
      } /* end for */
    } /* end else */
    break;
  case NC_UINT64:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) 
	if(op2.ui64p[idx] < op1.ui64p[idx]) op2.ui64p[idx]=op1.ui64p[idx];
    }else{
      const nco_uint64 mss_val_uint64=*mss_val.ui64p;
      for(idx=0;idx<sz;idx++){
	if(op2.ui64p[idx] == mss_val_uint64) 
	  op2.ui64p[idx]=op1.ui64p[idx];
	else if((op1.ui64p[idx] != mss_val_uint64) && (op2.ui64p[idx] < op1.ui64p[idx]))
	  op2.ui64p[idx]=op1.ui64p[idx]; 
      } /* end for */
    } /* end else */
    break;
  case NC_BYTE:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) 
	if(op2.bp[idx] < op1.bp[idx]) op2.bp[idx]=op1.bp[idx];
    }else{
      const nco_byte mss_val_byte=*mss_val.bp;
      for(idx=0;idx<sz;idx++){
	if(op2.bp[idx] == mss_val_byte) 
	  op2.bp[idx]=op1.bp[idx];
	else if((op1.bp[idx] != mss_val_byte) && (op2.bp[idx] < op1.bp[idx]))
	  op2.bp[idx]=op1.bp[idx]; 
      } /* end for */
    } /* end else */
    break;
  case NC_UBYTE:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) 
	if(op2.ubp[idx] < op1.ubp[idx]) op2.ubp[idx]=op1.ubp[idx];
    }else{
      const nco_ubyte mss_val_ubyte=*mss_val.ubp;
      for(idx=0;idx<sz;idx++){
	if(op2.ubp[idx] == mss_val_ubyte) 
	  op2.ubp[idx]=op1.ubp[idx];
	else if((op1.ubp[idx] != mss_val_ubyte) && (op2.ubp[idx] < op1.ubp[idx]))
	  op2.ubp[idx]=op1.ubp[idx]; 
      } /* end for */
    } /* end else */
    break;
  case NC_CHAR: break; /* Do nothing */
  case NC_STRING: break; /* Do nothing */
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
} /* end nco_var_max_bnr() */

void
nco_var_min_bnr /* [fnc] Minimize two operands */
(const nc_type type, /* I [type] netCDF type of operands */
 const long sz, /* I [nbr] Size (in elements) of operands */
 const int has_mss_val, /* I [flg] Flag for missing values */
 ptr_unn mss_val, /* I [flg] Value of missing value */
 ptr_unn op1, /* I [val] Values of first operand */
 ptr_unn op2) /* I/O [val] Values of second operand on input, values of minimum on output */
{
  /* Purpose: Find minimum value(s) of two operands and store result in second operand 
     Operands are assumed to conform, be of same specified type, and have values in memory */
  long idx;
  
  /* Typecast pointer to values before access */
  /* It is not necessary to uncast pointer types after using them as we have 
     operated on local copies of them */
  
  (void)cast_void_nctype(type,&op1);
  (void)cast_void_nctype(type,&op2);
  if(has_mss_val) (void)cast_void_nctype(type,&mss_val);
  
  switch(type){
  case NC_FLOAT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) 
	if(op2.fp[idx] > op1.fp[idx]) op2.fp[idx]=op1.fp[idx];
    }else{
      const float mss_val_flt=*mss_val.fp;
      for(idx=0;idx<sz;idx++){
	if(op2.fp[idx] == mss_val_flt) 
	  op2.fp[idx]=op1.fp[idx];
	else if((op1.fp[idx] != mss_val_flt) && (op2.fp[idx] > op1.fp[idx]))
	  op2.fp[idx]=op1.fp[idx]; 
      } /* end for */
    } /* end else */
    break;
  case NC_DOUBLE:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) 
	if(op2.dp[idx] > op1.dp[idx]) op2.dp[idx]=op1.dp[idx];
    }else{
      const double mss_val_dbl=*mss_val.dp;
      for(idx=0;idx<sz;idx++){
	if(op2.dp[idx] == mss_val_dbl) 
	  op2.dp[idx]=op1.dp[idx];
	else if((op1.dp[idx] != mss_val_dbl) && (op2.dp[idx] > op1.dp[idx]))
	  op2.dp[idx]=op1.dp[idx]; 
      } /* end for */
    } /* end else */
    break;
  case NC_INT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) 
	if(op2.ip[idx] > op1.ip[idx]) op2.ip[idx]=op1.ip[idx];
    }else{
      const nco_int mss_val_ntg=*mss_val.ip;
      for(idx=0;idx<sz;idx++){
	if(op2.ip[idx] == mss_val_ntg) 
	  op2.ip[idx]=op1.ip[idx];
	else if((op1.ip[idx] != mss_val_ntg) && (op2.ip[idx] > op1.ip[idx]))
	  op2.ip[idx]=op1.ip[idx]; 
      } /* end for */
    } /* end else */
    break;
  case NC_SHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) 
	if(op2.sp[idx] > op1.sp[idx]) op2.sp[idx]=op1.sp[idx];
    }else{
      const nco_short mss_val_short=*mss_val.sp;
      for(idx=0;idx<sz;idx++){
	if(op2.sp[idx] == mss_val_short) 
	  op2.sp[idx]=op1.sp[idx];
	else if((op1.sp[idx] != mss_val_short) && (op2.sp[idx] > op1.sp[idx]))
	  op2.sp[idx]=op1.sp[idx]; 
      } /* end for */
    } /* end else */
    break;
  case NC_USHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) 
	if(op2.usp[idx] > op1.usp[idx]) op2.usp[idx]=op1.usp[idx];
    }else{
      const nco_ushort mss_val_ushort=*mss_val.usp;
      for(idx=0;idx<sz;idx++){
	if(op2.usp[idx] == mss_val_ushort) 
	  op2.usp[idx]=op1.usp[idx];
	else if((op1.usp[idx] != mss_val_ushort) && (op2.usp[idx] > op1.usp[idx]))
	  op2.usp[idx]=op1.usp[idx]; 
      } /* end for */
    } /* end else */
    break;
  case NC_UINT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) 
	if(op2.uip[idx] > op1.uip[idx]) op2.uip[idx]=op1.uip[idx];
    }else{
      const nco_uint mss_val_uint=*mss_val.uip;
      for(idx=0;idx<sz;idx++){
	if(op2.uip[idx] == mss_val_uint) 
	  op2.uip[idx]=op1.uip[idx];
	else if((op1.uip[idx] != mss_val_uint) && (op2.uip[idx] > op1.uip[idx]))
	  op2.uip[idx]=op1.uip[idx]; 
      } /* end for */
    } /* end else */
    break;
  case NC_INT64:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) 
	if(op2.i64p[idx] > op1.i64p[idx]) op2.i64p[idx]=op1.i64p[idx];
    }else{
      const nco_int64 mss_val_int64=*mss_val.i64p;
      for(idx=0;idx<sz;idx++){
	if(op2.i64p[idx] == mss_val_int64) 
	  op2.i64p[idx]=op1.i64p[idx];
	else if((op1.i64p[idx] != mss_val_int64) && (op2.i64p[idx] > op1.i64p[idx]))
	  op2.i64p[idx]=op1.i64p[idx]; 
      } /* end for */
    } /* end else */
    break;
  case NC_UINT64:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) 
	if(op2.ui64p[idx] > op1.ui64p[idx]) op2.ui64p[idx]=op1.ui64p[idx];
    }else{
      const nco_uint64 mss_val_uint64=*mss_val.ui64p;
      for(idx=0;idx<sz;idx++){
	if(op2.ui64p[idx] == mss_val_uint64) 
	  op2.ui64p[idx]=op1.ui64p[idx];
	else if((op1.ui64p[idx] != mss_val_uint64) && (op2.ui64p[idx] > op1.ui64p[idx]))
	  op2.ui64p[idx]=op1.ui64p[idx]; 
      } /* end for */
    } /* end else */
    break;
  case NC_BYTE:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) 
	if(op2.bp[idx] > op1.bp[idx]) op2.bp[idx]=op1.bp[idx];
    }else{
      const nco_byte mss_val_byte=*mss_val.bp;
      for(idx=0;idx<sz;idx++){
	if(op2.bp[idx] == mss_val_byte) 
	  op2.bp[idx]=op1.bp[idx];
	else if((op1.bp[idx] != mss_val_byte) && (op2.bp[idx] > op1.bp[idx]))
	  op2.bp[idx]=op1.bp[idx]; 
      } /* end for */
    } /* end else */
    break;
  case NC_UBYTE:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) 
	if(op2.ubp[idx] > op1.ubp[idx]) op2.ubp[idx]=op1.ubp[idx];
    }else{
      const nco_ubyte mss_val_ubyte=*mss_val.ubp;
      for(idx=0;idx<sz;idx++){
	if(op2.ubp[idx] == mss_val_ubyte) 
	  op2.ubp[idx]=op1.ubp[idx];
	else if((op1.ubp[idx] != mss_val_ubyte) && (op2.ubp[idx] > op1.ubp[idx]))
	  op2.ubp[idx]=op1.ubp[idx]; 
      } /* end for */
    } /* end else */
    break;
  case NC_CHAR: break; /* Do nothing */
  case NC_STRING: break; /* Do nothing */
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
} /* end nco_var_min_bnr() */

void
nco_var_mlt /* [fnc] Multiply first operand by second operand */
(const nc_type type, /* I [type] netCDF type of operands */
 const long sz, /* I [nbr] Size (in elements) of operands */
 const int has_mss_val, /* I [flg] Flag for missing values */
 ptr_unn mss_val, /* I [flg] Value of missing value */
 ptr_unn op1, /* I [val] Values of first operand */
 ptr_unn op2) /* I/O [val] Values of second operand on input, values of product on output */
{
  /* Threads: Routine is thread safe and calls no unsafe routines */
  /* Purpose: multiply value of first operand by value of second operand 
     and store result in second operand. 
     Assume operands conform, are same type, and are in memory */
  
  /* Multiplication is currently defined as op2:=op1*op2 */  
  
  long idx;
  
  /* Typecast pointer to values before access */
  (void)cast_void_nctype(type,&op1);
  (void)cast_void_nctype(type,&op2);
  if(has_mss_val) (void)cast_void_nctype(type,&mss_val);
  
  switch(type){
  case NC_FLOAT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.fp[idx]*=op1.fp[idx];
    }else{
      const float mss_val_flt=*mss_val.fp;
      for(idx=0;idx<sz;idx++){
	if((op2.fp[idx] != mss_val_flt) && (op1.fp[idx] != mss_val_flt)) op2.fp[idx]*=op1.fp[idx]; else op2.fp[idx]=mss_val_flt;
      } /* end for */
    } /* end else */
    break;
  case NC_DOUBLE:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.dp[idx]*=op1.dp[idx];
    }else{
      const double mss_val_dbl=*mss_val.dp;
      for(idx=0;idx<sz;idx++){
	if((op2.dp[idx] != mss_val_dbl) && (op1.dp[idx] != mss_val_dbl)) op2.dp[idx]*=op1.dp[idx]; else op2.dp[idx]=mss_val_dbl;
      } /* end for */
    } /* end else */
    break;
  case NC_INT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.ip[idx]*=op1.ip[idx];
    }else{
      const nco_int mss_val_ntg=*mss_val.ip;
      for(idx=0;idx<sz;idx++){
	if((op2.ip[idx] != mss_val_ntg) && (op1.ip[idx] != mss_val_ntg)) op2.ip[idx]*=op1.ip[idx]; else op2.ip[idx]=mss_val_ntg;
      } /* end for */
    } /* end else */
    break;
  case NC_SHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.sp[idx]*=op1.sp[idx];
    }else{
      const nco_short mss_val_short=*mss_val.sp;
      for(idx=0;idx<sz;idx++){
	if((op2.sp[idx] != mss_val_short) && (op1.sp[idx] != mss_val_short)) op2.sp[idx]*=op1.sp[idx]; else op2.sp[idx]=mss_val_short;
      } /* end for */
    } /* end else */
    break;
  case NC_USHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.usp[idx]*=op1.usp[idx];
    }else{
      const nco_ushort mss_val_ushort=*mss_val.usp;
      for(idx=0;idx<sz;idx++){
	if((op2.usp[idx] != mss_val_ushort) && (op1.usp[idx] != mss_val_ushort)) op2.usp[idx]*=op1.usp[idx]; else op2.usp[idx]=mss_val_ushort;
      } /* end for */
    } /* end else */
    break;
  case NC_UINT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.uip[idx]*=op1.uip[idx];
    }else{
      const nco_uint mss_val_uint=*mss_val.uip;
      for(idx=0;idx<sz;idx++){
	if((op2.uip[idx] != mss_val_uint) && (op1.uip[idx] != mss_val_uint)) op2.uip[idx]*=op1.uip[idx]; else op2.uip[idx]=mss_val_uint;
      } /* end for */
    } /* end else */
    break;
  case NC_INT64:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.i64p[idx]*=op1.i64p[idx];
    }else{
      const nco_int64 mss_val_int64=*mss_val.i64p;
      for(idx=0;idx<sz;idx++){
	if((op2.i64p[idx] != mss_val_int64) && (op1.i64p[idx] != mss_val_int64)) op2.i64p[idx]*=op1.i64p[idx]; else op2.i64p[idx]=mss_val_int64;
      } /* end for */
    } /* end else */
    break;
  case NC_UINT64:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.ui64p[idx]*=op1.ui64p[idx];
    }else{
      const nco_uint64 mss_val_uint64=*mss_val.ui64p;
      for(idx=0;idx<sz;idx++){
	if((op2.ui64p[idx] != mss_val_uint64) && (op1.ui64p[idx] != mss_val_uint64)) op2.ui64p[idx]*=op1.ui64p[idx]; else op2.ui64p[idx]=mss_val_uint64;
      } /* end for */
    } /* end else */
    break;
  case NC_BYTE:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.bp[idx]*=op1.bp[idx];
    }else{
      const nco_byte mss_val_byte=*mss_val.bp;
      for(idx=0;idx<sz;idx++){
	if((op2.bp[idx] != mss_val_byte) && (op1.bp[idx] != mss_val_byte)) op2.bp[idx]*=op1.bp[idx]; else op2.bp[idx]=mss_val_byte;
      } /* end for */
    } /* end else */
    break;
  case NC_UBYTE:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.ubp[idx]*=op1.ubp[idx];
    }else{
      const nco_ubyte mss_val_ubyte=*mss_val.ubp;
      for(idx=0;idx<sz;idx++){
	if((op2.ubp[idx] != mss_val_ubyte) && (op1.ubp[idx] != mss_val_ubyte)) op2.ubp[idx]*=op1.ubp[idx]; else op2.ubp[idx]=mss_val_ubyte;
      } /* end for */
    } /* end else */
    break;
  case NC_CHAR: break; /* Do nothing */
  case NC_STRING: break; /* Do nothing */
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
  
  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */
  
} /* end nco_var_mlt() */

void
nco_var_mod /* [fnc] Remainder (modulo) operation of two variables */
(const nc_type type, /* I [type] netCDF type of operands */
 const long sz, /* I [nbr] Size (in elements) of operands */
 const int has_mss_val, /* I [flg] Flag for missing values */
 ptr_unn mss_val, /* I [flg] Value of missing value */
 ptr_unn op1, /* I [val] Values of field */
 ptr_unn op2) /* I/O [val] Values of divisor on input, values of remainder on output */
{
  /* Threads: Routine is thread safe and calls no unsafe routines */
  /* Purpose: Divide value of first operand by value of second operand 
     and store remainder in second operand. 
     Assume operands conform, are same type, and are in memory */
  
  /* Remainder (modulo) operation is currently defined as op2:=op1%op2 */  
  
  long idx;
  
  /* Typecast pointer to values before access */
  (void)cast_void_nctype(type,&op1);
  (void)cast_void_nctype(type,&op2);
  if(has_mss_val) (void)cast_void_nctype(type,&mss_val);
  
  switch(type){
  case NC_FLOAT: /* Hand-code modulo operator for floating point arguments (intrinsic % requires integer arguments) */
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.fp[idx]=op1.fp[idx]-op2.fp[idx]*(int)(op1.fp[idx]/op2.fp[idx]);
    }else{
      const float mss_val_flt=*mss_val.fp;
      for(idx=0;idx<sz;idx++){
	if((op2.fp[idx] != mss_val_flt) && (op1.fp[idx] != mss_val_flt)) op2.fp[idx]=op1.fp[idx]-op2.fp[idx]*(int)(op1.fp[idx]/op2.fp[idx]); else op2.fp[idx]=mss_val_flt;
      } /* end for */
    } /* end else */
    break; /* end NC_FLOAT */
  case NC_DOUBLE: /* Hand-code modulo operator for floating point arguments (intrinsic % requires integer arguments) */
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.dp[idx]=op1.dp[idx]-op2.dp[idx]*(int)(op1.dp[idx]/op2.dp[idx]);
    }else{
      const double mss_val_dbl=*mss_val.dp;
      for(idx=0;idx<sz;idx++){
	if((op2.dp[idx] != mss_val_dbl) && (op1.dp[idx] != mss_val_dbl)) op2.dp[idx]=op1.dp[idx]-op2.dp[idx]*(int)(op1.dp[idx]/op2.dp[idx]); else op2.dp[idx]=mss_val_dbl;
      } /* end for */
    } /* end else */
    break; /* end NC_DOUBLE */
  case NC_INT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.ip[idx]=op1.ip[idx]%op2.ip[idx];
    }else{
      const nco_int mss_val_ntg=*mss_val.ip;
      for(idx=0;idx<sz;idx++){
	if((op2.ip[idx] != mss_val_ntg) && (op1.ip[idx] != mss_val_ntg)) op2.ip[idx]=op1.ip[idx]%op2.ip[idx]; else op2.ip[idx]=mss_val_ntg;
      } /* end for */
    } /* end else */
    break; /* end NC_INT */
  case NC_SHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.sp[idx]=op1.sp[idx]%op2.sp[idx];
    }else{
      const nco_short mss_val_short=*mss_val.sp;
      for(idx=0;idx<sz;idx++){
	if((op2.sp[idx] != mss_val_short) && (op1.sp[idx] != mss_val_short)) op2.sp[idx]=op1.sp[idx]%op2.sp[idx]; else op2.sp[idx]=mss_val_short;
      } /* end for */
    } /* end else */
    break; /* end NC_SHORT */
  case NC_USHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.usp[idx]=op1.usp[idx]%op2.usp[idx];
    }else{
      const nco_ushort mss_val_ushort=*mss_val.usp;
      for(idx=0;idx<sz;idx++){
	if((op2.usp[idx] != mss_val_ushort) && (op1.usp[idx] != mss_val_ushort)) op2.usp[idx]=op1.usp[idx]%op2.usp[idx]; else op2.usp[idx]=mss_val_ushort;
      } /* end for */
    } /* end else */
    break; /* end NC_USHORT */
  case NC_UINT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.uip[idx]=op1.uip[idx]%op2.uip[idx];
    }else{
      const nco_uint mss_val_uint=*mss_val.uip;
      for(idx=0;idx<sz;idx++){
	if((op2.uip[idx] != mss_val_uint) && (op1.uip[idx] != mss_val_uint)) op2.uip[idx]=op1.uip[idx]%op2.uip[idx]; else op2.uip[idx]=mss_val_uint;
      } /* end for */
    } /* end else */
    break; /* end NC_UINT */
  case NC_INT64:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.i64p[idx]=op1.i64p[idx]%op2.i64p[idx];
    }else{
      const nco_int64 mss_val_int64=*mss_val.i64p;
      for(idx=0;idx<sz;idx++){
	if((op2.i64p[idx] != mss_val_int64) && (op1.i64p[idx] != mss_val_int64)) op2.i64p[idx]=op1.i64p[idx]%op2.i64p[idx]; else op2.i64p[idx]=mss_val_int64;
      } /* end for */
    } /* end else */
    break; /* end NC_INT64 */
  case NC_UINT64:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.ui64p[idx]=op1.ui64p[idx]%op2.ui64p[idx];
    }else{
      const nco_uint64 mss_val_uint64=*mss_val.ui64p;
      for(idx=0;idx<sz;idx++){
	if((op2.ui64p[idx] != mss_val_uint64) && (op1.ui64p[idx] != mss_val_uint64)) op2.ui64p[idx]=op1.ui64p[idx]%op2.ui64p[idx]; else op2.ui64p[idx]=mss_val_uint64;
      } /* end for */
    } /* end else */
    break; /* end NC_UINT64 */
  case NC_BYTE:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.bp[idx]=op1.bp[idx]%op2.bp[idx];
    }else{
      const nco_byte mss_val_byte=*mss_val.bp;
      for(idx=0;idx<sz;idx++){
	if((op2.bp[idx] != mss_val_byte) && (op1.bp[idx] != mss_val_byte)) op2.bp[idx]=op1.bp[idx]%op2.bp[idx]; else op2.bp[idx]=mss_val_byte;
      } /* end for */
    } /* end else */
    break; /* end NC_BYTE */
  case NC_UBYTE:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.ubp[idx]=op1.ubp[idx]%op2.ubp[idx];
    }else{
      const nco_ubyte mss_val_ubyte=*mss_val.ubp;
      for(idx=0;idx<sz;idx++){
	if((op2.ubp[idx] != mss_val_ubyte) && (op1.ubp[idx] != mss_val_ubyte)) op2.ubp[idx]=op1.ubp[idx]%op2.ubp[idx]; else op2.ubp[idx]=mss_val_ubyte;
      } /* end for */
    } /* end else */
    break; /* end NC_UBYTE */
  case NC_CHAR: break; /* Do nothing */
  case NC_STRING: break; /* Do nothing */
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
  
  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */
  
} /* end nco_var_mod() */

void
nco_var_msk /* [fnc] Mask third operand where first and second operands fail comparison */
(const nc_type type, /* I [enm] netCDF type of operands */
 const long sz, /* I [nbr] Size (in elements) of operand op3 */
 const int has_mss_val, /* I [flg] Flag for missing values (basically assumed to be true) */
 ptr_unn mss_val, /* I [val] Value of missing value */
 const double op1, /* I [val] Target value to compare against mask field (i.e., argument of -M) */
 const int op_typ_rlt, /* I [enm] Comparison type test for op2 and op1 */
 ptr_unn op2, /* I [val] Value of mask field */
 ptr_unn op3) /* I/O [val] Values of second operand on input, masked values on output */
{
  /* Threads: Routine is thread safe and calls no unsafe routines */
  /* Purpose: Mask third operand where first and second operands fail comparison
     Set third operand to missing value wherever second operand fails comparison with first operand */
  
  /* Masking is currently defined as: if(op2 !op_typ_rlt op1) then op3:=mss_val */  
  
  long idx;
  double mss_val_dbl=double_CEWI;
  float mss_val_flt=float_CEWI;
  nco_int mss_val_ntg=nco_int_CEWI;
  nco_short mss_val_short=nco_short_CEWI;
  nco_ushort mss_val_ushort=nco_ushort_CEWI;
  nco_uint mss_val_uint=nco_uint_CEWI;
  nco_int64 mss_val_int64=nco_int64_CEWI;
  nco_uint64 mss_val_uint64=nco_uint64_CEWI;
  nco_byte mss_val_byte=nco_byte_CEWI;
  nco_ubyte mss_val_ubyte=nco_ubyte_CEWI;
  nco_char mss_val_char=nco_char_CEWI;
  /*  nco_string mss_val_string=nco_string_CEWI;*/ /* 20120206: mss_val_string is not yet used so do not define */
  
  /* Typecast pointer to values before access */
  (void)cast_void_nctype(type,&op2);
  (void)cast_void_nctype(type,&op3);
  if(has_mss_val){
    (void)cast_void_nctype(type,&mss_val);
  }else{
    (void)fprintf(stdout,"%s: ERROR has_mss_val is inconsistent with purpose of var_ask(), i.e., has_mss_val is not True\n",nco_prg_nm_get());
    nco_exit(EXIT_FAILURE);
  } /* end else */
  
  if(has_mss_val){
    switch(type){
    case NC_FLOAT: mss_val_flt=*mss_val.fp; break;
    case NC_DOUBLE: mss_val_dbl=*mss_val.dp; break;
    case NC_INT: mss_val_ntg=*mss_val.ip; break;
    case NC_SHORT: mss_val_short=*mss_val.sp; break;
    case NC_USHORT: mss_val_ushort=*mss_val.usp; break;
    case NC_UINT: mss_val_uint=*mss_val.uip; break;
    case NC_INT64: mss_val_int64=*mss_val.i64p; break;
    case NC_UINT64: mss_val_uint64=*mss_val.ui64p; break;
    case NC_BYTE: mss_val_byte=*mss_val.bp; break;
    case NC_UBYTE: mss_val_ubyte=*mss_val.ubp; break;
    case NC_CHAR: mss_val_char=*mss_val.cp; break;
      /*    case NC_STRING: mss_val_string=*mss_val.sngp; break;*/ /* 20120206: mss_val_string is not yet used so do not define */
    default: nco_dfl_case_nc_type_err(); break;
    } /* end switch */
  } /* endif */
  
  /* NB: Explicit coercion when comparing op2 to op1 is necessary */
  switch(type){
  case NC_FLOAT:
    switch(op_typ_rlt){
    case nco_op_eq: for(idx=0;idx<sz;idx++) if(op2.fp[idx] != (float)op1) op3.fp[idx]=mss_val_flt; break;
    case nco_op_ne: for(idx=0;idx<sz;idx++) if(op2.fp[idx] == (float)op1) op3.fp[idx]=mss_val_flt; break;
    case nco_op_lt: for(idx=0;idx<sz;idx++) if(op2.fp[idx] >= (float)op1) op3.fp[idx]=mss_val_flt; break;
    case nco_op_gt: for(idx=0;idx<sz;idx++) if(op2.fp[idx] <= (float)op1) op3.fp[idx]=mss_val_flt; break;
    case nco_op_le: for(idx=0;idx<sz;idx++) if(op2.fp[idx] >  (float)op1) op3.fp[idx]=mss_val_flt; break;
    case nco_op_ge: for(idx=0;idx<sz;idx++) if(op2.fp[idx] <  (float)op1) op3.fp[idx]=mss_val_flt; break;
    } /* end switch */
    break;
  case NC_DOUBLE:
    switch(op_typ_rlt){
    case nco_op_eq: for(idx=0;idx<sz;idx++) if(op2.dp[idx] != (double)op1) op3.dp[idx]=mss_val_dbl; break;
    case nco_op_ne: for(idx=0;idx<sz;idx++) if(op2.dp[idx] == (double)op1) op3.dp[idx]=mss_val_dbl; break;
    case nco_op_lt: for(idx=0;idx<sz;idx++) if(op2.dp[idx] >= (double)op1) op3.dp[idx]=mss_val_dbl; break;
    case nco_op_gt: for(idx=0;idx<sz;idx++) if(op2.dp[idx] <= (double)op1) op3.dp[idx]=mss_val_dbl; break;
    case nco_op_le: for(idx=0;idx<sz;idx++) if(op2.dp[idx] >  (double)op1) op3.dp[idx]=mss_val_dbl; break;
    case nco_op_ge: for(idx=0;idx<sz;idx++) if(op2.dp[idx] <  (double)op1) op3.dp[idx]=mss_val_dbl; break;
    } /* end switch */
    break;
  case NC_INT:
    switch(op_typ_rlt){
    case nco_op_eq: for(idx=0;idx<sz;idx++) if(op2.ip[idx] != (nco_int)op1) op3.ip[idx]=mss_val_ntg; break;
    case nco_op_ne: for(idx=0;idx<sz;idx++) if(op2.ip[idx] == (nco_int)op1) op3.ip[idx]=mss_val_ntg; break;
    case nco_op_lt: for(idx=0;idx<sz;idx++) if(op2.ip[idx] >= (nco_int)op1) op3.ip[idx]=mss_val_ntg; break;
    case nco_op_gt: for(idx=0;idx<sz;idx++) if(op2.ip[idx] <= (nco_int)op1) op3.ip[idx]=mss_val_ntg; break;
    case nco_op_le: for(idx=0;idx<sz;idx++) if(op2.ip[idx] >  (nco_int)op1) op3.ip[idx]=mss_val_ntg; break;
    case nco_op_ge: for(idx=0;idx<sz;idx++) if(op2.ip[idx] <  (nco_int)op1) op3.ip[idx]=mss_val_ntg; break;
    } /* end switch */
    break;
  case NC_SHORT:
    switch(op_typ_rlt){
    case nco_op_eq: for(idx=0;idx<sz;idx++) if(op2.sp[idx] != (nco_short)op1) op3.sp[idx]=mss_val_short; break;
    case nco_op_ne: for(idx=0;idx<sz;idx++) if(op2.sp[idx] == (nco_short)op1) op3.sp[idx]=mss_val_short; break;
    case nco_op_lt: for(idx=0;idx<sz;idx++) if(op2.sp[idx] >= (nco_short)op1) op3.sp[idx]=mss_val_short; break;
    case nco_op_gt: for(idx=0;idx<sz;idx++) if(op2.sp[idx] <= (nco_short)op1) op3.sp[idx]=mss_val_short; break;
    case nco_op_le: for(idx=0;idx<sz;idx++) if(op2.sp[idx] >  (nco_short)op1) op3.sp[idx]=mss_val_short; break;
    case nco_op_ge: for(idx=0;idx<sz;idx++) if(op2.sp[idx] <  (nco_short)op1) op3.sp[idx]=mss_val_short; break;
    } /* end switch */
    break;
  case NC_USHORT:
    switch(op_typ_rlt){
    case nco_op_eq: for(idx=0;idx<sz;idx++) if(op2.usp[idx] != (nco_ushort)op1) op3.usp[idx]=mss_val_ushort; break;
    case nco_op_ne: for(idx=0;idx<sz;idx++) if(op2.usp[idx] == (nco_ushort)op1) op3.usp[idx]=mss_val_ushort; break;
    case nco_op_lt: for(idx=0;idx<sz;idx++) if(op2.usp[idx] >= (nco_ushort)op1) op3.usp[idx]=mss_val_ushort; break;
    case nco_op_gt: for(idx=0;idx<sz;idx++) if(op2.usp[idx] <= (nco_ushort)op1) op3.usp[idx]=mss_val_ushort; break;
    case nco_op_le: for(idx=0;idx<sz;idx++) if(op2.usp[idx] >  (nco_ushort)op1) op3.usp[idx]=mss_val_ushort; break;
    case nco_op_ge: for(idx=0;idx<sz;idx++) if(op2.usp[idx] <  (nco_ushort)op1) op3.usp[idx]=mss_val_ushort; break;
    } /* end switch */
    break;
  case NC_UINT:
    switch(op_typ_rlt){
    case nco_op_eq: for(idx=0;idx<sz;idx++) if(op2.uip[idx] != (nco_uint)op1) op3.uip[idx]=mss_val_uint; break;
    case nco_op_ne: for(idx=0;idx<sz;idx++) if(op2.uip[idx] == (nco_uint)op1) op3.uip[idx]=mss_val_uint; break;
    case nco_op_lt: for(idx=0;idx<sz;idx++) if(op2.uip[idx] >= (nco_uint)op1) op3.uip[idx]=mss_val_uint; break;
    case nco_op_gt: for(idx=0;idx<sz;idx++) if(op2.uip[idx] <= (nco_uint)op1) op3.uip[idx]=mss_val_uint; break;
    case nco_op_le: for(idx=0;idx<sz;idx++) if(op2.uip[idx] >  (nco_uint)op1) op3.uip[idx]=mss_val_uint; break;
    case nco_op_ge: for(idx=0;idx<sz;idx++) if(op2.uip[idx] <  (nco_uint)op1) op3.uip[idx]=mss_val_uint; break;
    } /* end switch */
    break;
  case NC_INT64:
    switch(op_typ_rlt){
    case nco_op_eq: for(idx=0;idx<sz;idx++) if(op2.i64p[idx] != (nco_int64)op1) op3.i64p[idx]=mss_val_int64; break;
    case nco_op_ne: for(idx=0;idx<sz;idx++) if(op2.i64p[idx] == (nco_int64)op1) op3.i64p[idx]=mss_val_int64; break;
    case nco_op_lt: for(idx=0;idx<sz;idx++) if(op2.i64p[idx] >= (nco_int64)op1) op3.i64p[idx]=mss_val_int64; break;
    case nco_op_gt: for(idx=0;idx<sz;idx++) if(op2.i64p[idx] <= (nco_int64)op1) op3.i64p[idx]=mss_val_int64; break;
    case nco_op_le: for(idx=0;idx<sz;idx++) if(op2.i64p[idx] >  (nco_int64)op1) op3.i64p[idx]=mss_val_int64; break;
    case nco_op_ge: for(idx=0;idx<sz;idx++) if(op2.i64p[idx] <  (nco_int64)op1) op3.i64p[idx]=mss_val_int64; break;
    } /* end switch */
    break;
  case NC_UINT64:
    switch(op_typ_rlt){
    case nco_op_eq: for(idx=0;idx<sz;idx++) if(op2.ui64p[idx] != (nco_uint64)op1) op3.ui64p[idx]=mss_val_uint64; break;
    case nco_op_ne: for(idx=0;idx<sz;idx++) if(op2.ui64p[idx] == (nco_uint64)op1) op3.ui64p[idx]=mss_val_uint64; break;
    case nco_op_lt: for(idx=0;idx<sz;idx++) if(op2.ui64p[idx] >= (nco_uint64)op1) op3.ui64p[idx]=mss_val_uint64; break;
    case nco_op_gt: for(idx=0;idx<sz;idx++) if(op2.ui64p[idx] <= (nco_uint64)op1) op3.ui64p[idx]=mss_val_uint64; break;
    case nco_op_le: for(idx=0;idx<sz;idx++) if(op2.ui64p[idx] >  (nco_uint64)op1) op3.ui64p[idx]=mss_val_uint64; break;
    case nco_op_ge: for(idx=0;idx<sz;idx++) if(op2.ui64p[idx] <  (nco_uint64)op1) op3.ui64p[idx]=mss_val_uint64; break;
    } /* end switch */
    break;
  case NC_BYTE:
    switch(op_typ_rlt){
    case nco_op_eq: for(idx=0;idx<sz;idx++) if(op2.bp[idx] != (nco_byte)op1) op3.bp[idx]=mss_val_byte; break;
    case nco_op_ne: for(idx=0;idx<sz;idx++) if(op2.bp[idx] == (nco_byte)op1) op3.bp[idx]=mss_val_byte; break;
    case nco_op_lt: for(idx=0;idx<sz;idx++) if(op2.bp[idx] >= (nco_byte)op1) op3.bp[idx]=mss_val_byte; break;
    case nco_op_gt: for(idx=0;idx<sz;idx++) if(op2.bp[idx] <= (nco_byte)op1) op3.bp[idx]=mss_val_byte; break;
    case nco_op_le: for(idx=0;idx<sz;idx++) if(op2.bp[idx] >  (nco_byte)op1) op3.bp[idx]=mss_val_byte; break;
    case nco_op_ge: for(idx=0;idx<sz;idx++) if(op2.bp[idx] <  (nco_byte)op1) op3.bp[idx]=mss_val_byte; break;
    } /* end switch */
    break;
  case NC_UBYTE:
    switch(op_typ_rlt){
    case nco_op_eq: for(idx=0;idx<sz;idx++) if(op2.ubp[idx] != (nco_ubyte)op1) op3.ubp[idx]=mss_val_ubyte; break;
    case nco_op_ne: for(idx=0;idx<sz;idx++) if(op2.ubp[idx] == (nco_ubyte)op1) op3.ubp[idx]=mss_val_ubyte; break;
    case nco_op_lt: for(idx=0;idx<sz;idx++) if(op2.ubp[idx] >= (nco_ubyte)op1) op3.ubp[idx]=mss_val_ubyte; break;
    case nco_op_gt: for(idx=0;idx<sz;idx++) if(op2.ubp[idx] <= (nco_ubyte)op1) op3.ubp[idx]=mss_val_ubyte; break;
    case nco_op_le: for(idx=0;idx<sz;idx++) if(op2.ubp[idx] >  (nco_ubyte)op1) op3.ubp[idx]=mss_val_ubyte; break;
    case nco_op_ge: for(idx=0;idx<sz;idx++) if(op2.ubp[idx] <  (nco_ubyte)op1) op3.ubp[idx]=mss_val_ubyte; break;
    } /* end switch */
    break;
  case NC_CHAR:
    switch(op_typ_rlt){
    case nco_op_eq: for(idx=0;idx<sz;idx++) if(op2.cp[idx] != (nco_char)op1) op3.cp[idx]=mss_val_char; break;
    case nco_op_ne: for(idx=0;idx<sz;idx++) if(op2.cp[idx] == (nco_char)op1) op3.cp[idx]=mss_val_char; break;
    case nco_op_lt: for(idx=0;idx<sz;idx++) if(op2.cp[idx] >= (nco_char)op1) op3.cp[idx]=mss_val_char; break;
    case nco_op_gt: for(idx=0;idx<sz;idx++) if(op2.cp[idx] <= (nco_char)op1) op3.cp[idx]=mss_val_char; break;
    case nco_op_le: for(idx=0;idx<sz;idx++) if(op2.cp[idx] >  (nco_char)op1) op3.cp[idx]=mss_val_char; break;
    case nco_op_ge: for(idx=0;idx<sz;idx++) if(op2.cp[idx] <  (nco_char)op1) op3.cp[idx]=mss_val_char; break;
    } /* end switch */
    break;
  case NC_STRING: break; /* Do nothing */
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
  
  /* It is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */
  
} /* end nco_var_msk() */

void
nco_var_tll_zro_mss_val /* [fnc] Write missing value into elements with zero tally */
(const nc_type type, /* I [enm] netCDF type of operand */
 const long sz, /* I [nbr] Size (in elements) of operand */
 const int has_mss_val, /* I [flg] Flag for missing values */
 ptr_unn mss_val, /* I [val] Value of missing value */
 const long * const tally, /* I [nbr] Counter to normalize by */
 ptr_unn op1) /* I/O [val] Values of first operand on input, possibly missing values on output */
{
  /* Threads: Routine is thread safe and calls no unsafe routines */
  /* Purpose: Write missing value into elements with zero tally
     Routine is necessary because initialization of accumulating sums (specified, e.g., with -y ttl or with -N)
     sets initial sum to zero (so augmenting works) regardless if first slice is missing.
     Such sums are usually normalized and set to missing if tally is zero.
     However, totals are integrals and thus are never normalized.
     Initialization value of zero will be output even if tally is zero,
     _unless field is processed with this routine after summing and prior to writing_ */
  
  /* Filter currently works as op1:=mss_val where tally == 0 */
  
  long idx;
  
  /* Routine changes nothing unless a missing value is defined */
  if(!has_mss_val) return;

  /* Typecast pointer to values before access */
  (void)cast_void_nctype(type,&op1);
  if(has_mss_val) (void)cast_void_nctype(type,&mss_val);
  
  switch(type){
  case NC_FLOAT:
    {
      const float mss_val_flt=*mss_val.fp;
      for(idx=0;idx<sz;idx++) if(tally[idx] == 0L) op1.fp[idx]=mss_val_flt;
    }
    break;
  case NC_DOUBLE:
    {
      const double mss_val_dbl=*mss_val.dp;
      for(idx=0;idx<sz;idx++) if(tally[idx] == 0L) op1.dp[idx]=mss_val_dbl;
    }
    break;
  case NC_INT:
    {
      const nco_int mss_val_ntg=*mss_val.ip;
      for(idx=0;idx<sz;idx++) if(tally[idx] == 0L) op1.ip[idx]=mss_val_ntg;
    }
    break;
  case NC_SHORT:
    {
      const nco_short mss_val_short=*mss_val.sp;
      for(idx=0;idx<sz;idx++) if(tally[idx] == 0L) op1.sp[idx]=mss_val_short;
    }
    break;
  case NC_USHORT:
    {
      const nco_ushort mss_val_ushort=*mss_val.usp;
      for(idx=0;idx<sz;idx++) if(tally[idx] == 0L) op1.usp[idx]=mss_val_ushort;
    }
    break;
  case NC_UINT:
    {
      const nco_uint mss_val_uint=*mss_val.uip;
      for(idx=0;idx<sz;idx++) if(tally[idx] == 0L) op1.uip[idx]=mss_val_uint;
    }
    break;
  case NC_INT64:
    {
      const nco_int64 mss_val_int64=*mss_val.i64p;
      for(idx=0;idx<sz;idx++) if(tally[idx] == 0L) op1.i64p[idx]=mss_val_int64;
    }
    break;
  case NC_UINT64:
    {
      const nco_uint64 mss_val_uint64=*mss_val.ui64p;
      for(idx=0;idx<sz;idx++) if(tally[idx] == 0L) op1.ui64p[idx]=mss_val_uint64;
    }
    break;
  case NC_BYTE:
    {
      const nco_byte mss_val_byte=*mss_val.bp;
      for(idx=0;idx<sz;idx++) if(tally[idx] == 0L) op1.bp[idx]=mss_val_byte;
    }
    break;
  case NC_UBYTE:
    {
      const nco_ubyte mss_val_ubyte=*mss_val.ubp;
      for(idx=0;idx<sz;idx++) if(tally[idx] == 0L) op1.ubp[idx]=mss_val_ubyte;
    }
    break;
  case NC_CHAR: break; /* Do nothing */
  case NC_STRING: break; /* Do nothing */
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
  
  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */
  
} /* end nco_var_tll_zro_mss_val() */

void
nco_var_nrm /* [fnc] Normalize value of first operand by count in tally array */
(const nc_type type, /* I [enm] netCDF type of operand */
 const long sz, /* I [nbr] Size (in elements) of operand */
 const int has_mss_val, /* I [flg] Flag for missing values */
 ptr_unn mss_val, /* I [val] Value of missing value */
 const long * const tally, /* I [nbr] Counter to normalize by */
 ptr_unn op1) /* I/O [val] Values of first operand on input, normalized result on output */
{
  /* Threads: Routine is thread safe and calls no unsafe routines */
  /* Purpose: Normalize value of first operand by count in tally array */
  
  /* Normalization is currently defined as op1:=op1/tally */  
  
  long idx;
  
  /* Typecast pointer to values before access */
  (void)cast_void_nctype(type,&op1);
  if(has_mss_val) (void)cast_void_nctype(type,&mss_val);
  
  switch(type){
  case NC_FLOAT:
    if(!has_mss_val){
      /* Operations: 1 fp divide, 2 pointer offset, 2 user memory fetch
	 Repetitions: \dmnszavg^(\dmnnbr-\avgnbr)
	 Total Counts: \flpnbr=\dmnszavg^(\dmnnbr-\avgnbr), \rthnbr=2\dmnszavg^(\dmnnbr-\avgnbr), \mmrusrnbr=2\dmnszavg^(\dmnnbr-\avgnbr)
	 NB: Counted LHS+RHS+tally offsets and fetches */
      for(idx=0;idx<sz;idx++) op1.fp[idx]/=tally[idx];
    }else{
      const float mss_val_flt=*mss_val.fp;
      for(idx=0;idx<sz;idx++) if(tally[idx] != 0L) op1.fp[idx]/=tally[idx]; else op1.fp[idx]=mss_val_flt;
    } /* end else */
    break;
  case NC_DOUBLE:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.dp[idx]/=tally[idx];
    }else{
      const double mss_val_dbl=*mss_val.dp;
      for(idx=0;idx<sz;idx++) if(tally[idx] != 0L) op1.dp[idx]/=tally[idx]; else op1.dp[idx]=mss_val_dbl;
    } /* end else */
    break;
  case NC_INT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.ip[idx]/=tally[idx];
    }else{
      const nco_int mss_val_ntg=*mss_val.ip;
      for(idx=0;idx<sz;idx++) if(tally[idx] != 0L) op1.ip[idx]/=tally[idx]; else op1.ip[idx]=mss_val_ntg;
    } /* end else */
    break;
  case NC_SHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.sp[idx]/=tally[idx];
    }else{
      const nco_short mss_val_short=*mss_val.sp;
      for(idx=0;idx<sz;idx++) if(tally[idx] != 0L) op1.sp[idx]/=tally[idx]; else op1.sp[idx]=mss_val_short;
    } /* end else */
    break;
  case NC_USHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.usp[idx]/=tally[idx];
    }else{
      const nco_ushort mss_val_ushort=*mss_val.usp;
      for(idx=0;idx<sz;idx++) if(tally[idx] != 0L) op1.usp[idx]/=tally[idx]; else op1.usp[idx]=mss_val_ushort;
    } /* end else */
    break;
  case NC_UINT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.uip[idx]/=tally[idx];
    }else{
      const nco_uint mss_val_uint=*mss_val.uip;
      for(idx=0;idx<sz;idx++) if(tally[idx] != 0L) op1.uip[idx]/=tally[idx]; else op1.uip[idx]=mss_val_uint;
    } /* end else */
    break;
  case NC_INT64:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.i64p[idx]/=tally[idx];
    }else{
      const nco_int64 mss_val_int64=*mss_val.i64p;
      for(idx=0;idx<sz;idx++) if(tally[idx] != 0L) op1.i64p[idx]/=tally[idx]; else op1.i64p[idx]=mss_val_int64;
    } /* end else */
    break;
  case NC_UINT64:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.ui64p[idx]/=tally[idx];
    }else{
      const nco_uint64 mss_val_uint64=*mss_val.ui64p;
      for(idx=0;idx<sz;idx++) if(tally[idx] != 0L) op1.ui64p[idx]/=tally[idx]; else op1.ui64p[idx]=mss_val_uint64;
    } /* end else */
    break;
  case NC_BYTE:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.bp[idx]/=tally[idx];
    }else{
      const nco_byte mss_val_byte=*mss_val.bp;
      for(idx=0;idx<sz;idx++) if(tally[idx] != 0L) op1.bp[idx]/=tally[idx]; else op1.bp[idx]=mss_val_byte;
    } /* end else */
    break;
  case NC_UBYTE:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.ubp[idx]/=tally[idx];
    }else{
      const nco_ubyte mss_val_ubyte=*mss_val.ubp;
      for(idx=0;idx<sz;idx++) if(tally[idx] != 0L) op1.ubp[idx]/=tally[idx]; else op1.ubp[idx]=mss_val_ubyte;
    } /* end else */
    break;
  case NC_CHAR: break; /* Do nothing */
  case NC_STRING: break; /* Do nothing */
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
  
  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */
  
} /* end nco_var_nrm() */

void
nco_var_nrm_sdn /* [fnc] Normalize value of first operand by count-1 in tally array */
(const nc_type type, /* I [enm] netCDF type of operand */
 const long sz, /* I [nbr] Size (in elements) of operand */
 const int has_mss_val, /* I [flg] Flag for missing values */
 ptr_unn mss_val, /* I [val] Value of missing value */
 const long * const tally, /* I [nbr] Counter to normalize by */
 ptr_unn op1) /* I/O [val] Values of first operand on input, normalized result on output */
{
  /* Purpose: Normalize value of first operand by count-1 in tally array */
  
  /* Normalization is currently defined as op1:=op1/(--tally) */  
  
  /* nco_var_nrm_sdn() is based on nco_var_nrm() and algorithms should be kept consistent with eachother */
  
  long idx;
  
  /* Typecast pointer to values before access */
  (void)cast_void_nctype(type,&op1);
  if(has_mss_val) (void)cast_void_nctype(type,&mss_val);
  
  switch(type){
  case NC_FLOAT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.fp[idx]/=tally[idx]-1L;
    }else{
      const float mss_val_flt=*mss_val.fp;
      for(idx=0;idx<sz;idx++) if(tally[idx] > 1L) op1.fp[idx]/=tally[idx]-1L; else op1.fp[idx]=mss_val_flt;
    } /* end else */
    break;
  case NC_DOUBLE:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.dp[idx]/=tally[idx]-1L;
    }else{
      const double mss_val_dbl=*mss_val.dp;
      for(idx=0;idx<sz;idx++) if(tally[idx] > 1L) op1.dp[idx]/=tally[idx]-1L; else op1.dp[idx]=mss_val_dbl;
    } /* end else */
    break;
  case NC_INT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.ip[idx]/=tally[idx]-1L;
    }else{
      const nco_int mss_val_ntg=*mss_val.ip;
      for(idx=0;idx<sz;idx++) if(tally[idx] > 1L) op1.ip[idx]/=tally[idx]-1L; else op1.ip[idx]=mss_val_ntg;
    } /* end else */
    break;
  case NC_SHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.sp[idx]/=tally[idx]-1L;
    }else{
      const nco_short mss_val_short=*mss_val.sp;
      for(idx=0;idx<sz;idx++) if(tally[idx] > 1L) op1.sp[idx]/=tally[idx]-1L; else op1.sp[idx]=mss_val_short;
    } /* end else */
    break;
  case NC_USHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.usp[idx]/=tally[idx]-1L;
    }else{
      const nco_ushort mss_val_ushort=*mss_val.usp;
      for(idx=0;idx<sz;idx++) if(tally[idx] > 1L) op1.usp[idx]/=tally[idx]-1L; else op1.usp[idx]=mss_val_ushort;
    } /* end else */
    break;
  case NC_UINT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.uip[idx]/=tally[idx]-1L;
    }else{
      const nco_uint mss_val_uint=*mss_val.uip;
      for(idx=0;idx<sz;idx++) if(tally[idx] > 1L) op1.uip[idx]/=tally[idx]-1L; else op1.uip[idx]=mss_val_uint;
    } /* end else */
    break;
  case NC_INT64:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.i64p[idx]/=tally[idx]-1L;
    }else{
      const nco_int64 mss_val_int64=*mss_val.i64p;
      for(idx=0;idx<sz;idx++) if(tally[idx] > 1L) op1.i64p[idx]/=tally[idx]-1L; else op1.i64p[idx]=mss_val_int64;
    } /* end else */
    break;
  case NC_UINT64:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.ui64p[idx]/=tally[idx]-1L;
    }else{
      const nco_uint64 mss_val_uint64=*mss_val.ui64p;
      for(idx=0;idx<sz;idx++) if(tally[idx] > 1L) op1.ui64p[idx]/=tally[idx]-1L; else op1.ui64p[idx]=mss_val_uint64;
    } /* end else */
    break;
  case NC_BYTE:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.bp[idx]/=tally[idx]-1L;
    }else{
      const nco_byte mss_val_byte=*mss_val.bp;
      for(idx=0;idx<sz;idx++) if(tally[idx] > 1L) op1.bp[idx]/=tally[idx]-1L; else op1.bp[idx]=mss_val_byte;
    } /* end else */
    break;
  case NC_UBYTE:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.ubp[idx]/=tally[idx]-1L;
    }else{
      const nco_ubyte mss_val_ubyte=*mss_val.ubp;
      for(idx=0;idx<sz;idx++) if(tally[idx] > 1L) op1.ubp[idx]/=tally[idx]-1L; else op1.ubp[idx]=mss_val_ubyte;
    } /* end else */
    break;
  case NC_CHAR: break; /* Do nothing */
  case NC_STRING: break; /* Do nothing */
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
  
  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */
  
} /* end of nco_var_nrm_sdn */

void
nco_var_pwr /* [fnc] Raise first operand to power of second operand */
(const nc_type type, /* I [type] netCDF type of operands */
 const long sz, /* I [nbr] Size (in elements) of operands */
 const int has_mss_val, /* I [flg] Flag for missing values */
 ptr_unn mss_val, /* I [flg] Value of missing value */
 ptr_unn op1, /* I [val] Values of base */
 ptr_unn op2) /* I/O [val] Values of exponent on input, values of power on output */
{
  /* Threads: Routine is thread safe and calls no unsafe routines */
  /* Purpose: Raise value of first operand to power of second operand 
     and store result in second operand. 
     Assume operands conform, are same type, and are in memory */
  
  /* Em-powering is currently defined as op2:=op1^op2 */  
  
  long idx;
  
  /* Typecast pointer to values before access */
  (void)cast_void_nctype(type,&op1);
  (void)cast_void_nctype(type,&op2);
  if(has_mss_val) (void)cast_void_nctype(type,&mss_val);
  
  switch(type){ 
  case NC_FLOAT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.fp[idx]=powf(op1.fp[idx],op2.fp[idx]);
    }else{
      float mss_val_flt=*mss_val.fp; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
        if((op1.fp[idx] != mss_val_flt) && (op2.fp[idx] != mss_val_flt)) op2.fp[idx]=powf(op1.fp[idx],op2.fp[idx]); else op2.fp[idx]=mss_val_flt;
      } /* end for */
    } /* end else */
    break; /* end NC_FLOAT */
  case NC_DOUBLE:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.dp[idx]=pow(op1.dp[idx],op2.dp[idx]);
    }else{
      double mss_val_dbl=*mss_val.dp; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
        if((op1.dp[idx] != mss_val_dbl) && (op2.dp[idx] != mss_val_dbl)) op2.dp[idx]=pow(op1.dp[idx],op2.dp[idx]); else op2.dp[idx]=mss_val_dbl;
      } /* end for */
    } /* end else */
    break; /* end NC_DOUBLE */
  case NC_INT:
    (void)fprintf(stdout,"%s: ERROR Attempt to em-power integer type in nco_var_pwr(). See TODO #311.\n",nco_prg_nm_get());
    break;
  case NC_SHORT:
    (void)fprintf(stdout,"%s: ERROR Attempt to em-power integer type in nco_var_pwr(). See TODO #311.\n",nco_prg_nm_get());
    break;
  case NC_USHORT:
    (void)fprintf(stdout,"%s: ERROR Attempt to em-power integer type in nco_var_pwr(). See TODO #311.\n",nco_prg_nm_get());
    break;
  case NC_UINT:
    (void)fprintf(stdout,"%s: ERROR Attempt to em-power integer type in nco_var_pwr(). See TODO #311.\n",nco_prg_nm_get());
    break;
  case NC_INT64:
    (void)fprintf(stdout,"%s: ERROR Attempt to em-power integer type in nco_var_pwr(). See TODO #311.\n",nco_prg_nm_get());
    break;
  case NC_UINT64:
    (void)fprintf(stdout,"%s: ERROR Attempt to em-power integer type in nco_var_pwr(). See TODO #311.\n",nco_prg_nm_get());
    break;
  case NC_BYTE:
    (void)fprintf(stdout,"%s: ERROR Attempt to em-power integer type in nco_var_pwr(). See TODO #311.\n",nco_prg_nm_get());
    break;
  case NC_UBYTE:
    (void)fprintf(stdout,"%s: ERROR Attempt to em-power integer type in nco_var_pwr(). See TODO #311.\n",nco_prg_nm_get());
    break;
  case NC_CHAR: break; /* Do nothing */
  case NC_STRING: break; /* Do nothing */
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
  
  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */
  
} /* end nco_var_pwr */

void
nco_var_sbt /* [fnc] Subtract first operand from second operand */
(const nc_type type, /* I [type] netCDF type of operands */
 const long sz, /* I [nbr] Size (in elements) of operands */
 const int has_mss_val, /* I [flg] Flag for missing values */
 ptr_unn mss_val, /* I [flg] Value of missing value */
 ptr_unn op1, /* I [val] Values of first operand */
 ptr_unn op2) /* I/O [val] Values of second operand on input, values of difference on output */
{
  /* Purpose: Subtract value of first operand from value of second operand 
     and store result in second operand. 
     Assume operands conform, are same type, and are in memory */
  
  /* Subtraction is currently defined as op2:=op2-op1 */
  
  long idx;
  
  /* Typecast pointer to values before access */
  (void)cast_void_nctype(type,&op1);
  (void)cast_void_nctype(type,&op2);
  if(has_mss_val) (void)cast_void_nctype(type,&mss_val);
  
  switch(type){
  case NC_FLOAT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.fp[idx]-=op1.fp[idx];
    }else{
      const float mss_val_flt=*mss_val.fp;
      for(idx=0;idx<sz;idx++){
	if((op2.fp[idx] != mss_val_flt) && (op1.fp[idx] != mss_val_flt)) op2.fp[idx]-=op1.fp[idx]; else op2.fp[idx]=mss_val_flt;
      } /* end for */
    } /* end else */
    break;
  case NC_DOUBLE:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.dp[idx]-=op1.dp[idx];
    }else{
      const double mss_val_dbl=*mss_val.dp;
      for(idx=0;idx<sz;idx++){
	if((op2.dp[idx] != mss_val_dbl) && (op1.dp[idx] != mss_val_dbl)) op2.dp[idx]-=op1.dp[idx]; else op2.dp[idx]=mss_val_dbl;
      } /* end for */
    } /* end else */
    break;
  case NC_INT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.ip[idx]-=op1.ip[idx];
    }else{
      const nco_int mss_val_ntg=*mss_val.ip;
      for(idx=0;idx<sz;idx++){
	if((op2.ip[idx] != mss_val_ntg) && (op1.ip[idx] != mss_val_ntg)) op2.ip[idx]-=op1.ip[idx]; else op2.ip[idx]=mss_val_ntg;
      } /* end for */
    } /* end else */
    break;
  case NC_SHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.sp[idx]-=op1.sp[idx];
    }else{
      const nco_short mss_val_short=*mss_val.sp;
      for(idx=0;idx<sz;idx++){
	if((op2.sp[idx] != mss_val_short) && (op1.sp[idx] != mss_val_short)) op2.sp[idx]-=op1.sp[idx]; else op2.sp[idx]=mss_val_short;
      } /* end for */
    } /* end else */
    break;
  case NC_USHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.usp[idx]-=op1.usp[idx];
    }else{
      const nco_ushort mss_val_ushort=*mss_val.usp;
      for(idx=0;idx<sz;idx++){
	if((op2.usp[idx] != mss_val_ushort) && (op1.usp[idx] != mss_val_ushort)) op2.usp[idx]-=op1.usp[idx]; else op2.usp[idx]=mss_val_ushort;
      } /* end for */
    } /* end else */
    break;
  case NC_UINT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.uip[idx]-=op1.uip[idx];
    }else{
      const nco_uint mss_val_uint=*mss_val.uip;
      for(idx=0;idx<sz;idx++){
	if((op2.uip[idx] != mss_val_uint) && (op1.uip[idx] != mss_val_uint)) op2.uip[idx]-=op1.uip[idx]; else op2.uip[idx]=mss_val_uint;
      } /* end for */
    } /* end else */
    break;
  case NC_INT64:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.i64p[idx]-=op1.i64p[idx];
    }else{
      const nco_int64 mss_val_int64=*mss_val.i64p;
      for(idx=0;idx<sz;idx++){
	if((op2.i64p[idx] != mss_val_int64) && (op1.i64p[idx] != mss_val_int64)) op2.i64p[idx]-=op1.i64p[idx]; else op2.i64p[idx]=mss_val_int64;
      } /* end for */
    } /* end else */
    break;
  case NC_UINT64:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.ui64p[idx]-=op1.ui64p[idx];
    }else{
      const nco_uint64 mss_val_uint64=*mss_val.ui64p;
      for(idx=0;idx<sz;idx++){
	if((op2.ui64p[idx] != mss_val_uint64) && (op1.ui64p[idx] != mss_val_uint64)) op2.ui64p[idx]-=op1.ui64p[idx]; else op2.ui64p[idx]=mss_val_uint64;
      } /* end for */
    } /* end else */
    break;
  case NC_BYTE:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.bp[idx]-=op1.bp[idx];
    }else{
      const nco_byte mss_val_byte=*mss_val.bp;
      for(idx=0;idx<sz;idx++){
	if((op2.bp[idx] != mss_val_byte) && (op1.bp[idx] != mss_val_byte)) op2.bp[idx]-=op1.bp[idx]; else op2.bp[idx]=mss_val_byte;
      } /* end for */
    } /* end else */
    break;
  case NC_UBYTE:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.ubp[idx]-=op1.ubp[idx];
    }else{
      const nco_ubyte mss_val_ubyte=*mss_val.ubp;
      for(idx=0;idx<sz;idx++){
	if((op2.ubp[idx] != mss_val_ubyte) && (op1.ubp[idx] != mss_val_ubyte)) op2.ubp[idx]-=op1.ubp[idx]; else op2.ubp[idx]=mss_val_ubyte;
      } /* end for */
    } /* end else */
    break;
  case NC_CHAR: break; /* Do nothing */
  case NC_STRING: break; /* Do nothing */
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
  
  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */
  
} /* end nco_var_sbt() */

void
nco_var_sqrt /* [fnc] Place squareroot of first operand in value of second operand */
(const nc_type type, /* I [enm] netCDF type of operand */
 const long sz, /* I [nbr] Size (in elements) of operand */
 const int has_mss_val, /* I [flg] Flag for missing values */
 ptr_unn mss_val, /* I [val] Value of missing value */
 long * restrict const tally, /* I/O [nbr] Counter space */
 ptr_unn op1, /* I [val] Values of first operand */
 ptr_unn op2) /* O [val] Squareroot of first operand */
{
  /* Purpose: Place squareroot of first operand in value of second operand 
     Assume operands conform, are same type, and are in memory */
  
  /* Square root is currently defined as op2:=sqrt(op1) */
  
  /* NB: Many compilers need to #include "nco_rth_flt.h" for sqrtf() prototype */
  
  long idx;
  
  /* Typecast pointer to values before access */
  (void)cast_void_nctype(type,&op1);
  (void)cast_void_nctype(type,&op2);
  if(has_mss_val) (void)cast_void_nctype(type,&mss_val);
  
  switch(type){
  case NC_FLOAT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.fp[idx]=sqrtf(op1.fp[idx]);
	tally[idx]++;
      } /* end for */
    }else{
      const float mss_val_flt=*mss_val.fp;
      for(idx=0;idx<sz;idx++){
	if(op1.fp[idx] != mss_val_flt){
	  op2.fp[idx]=sqrtf(op1.fp[idx]);
	  tally[idx]++;
	} /* end if */
      } /* end for */
    } /* end else */
    break;
  case NC_DOUBLE:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.dp[idx]=sqrt(op1.dp[idx]);
	tally[idx]++;
      } /* end for */
    }else{
      const double mss_val_dbl=*mss_val.dp;
      for(idx=0;idx<sz;idx++){
	if(op1.dp[idx] != mss_val_dbl){
	  op2.dp[idx]=sqrt(op1.dp[idx]);
	  tally[idx]++;
	} /* end if */
      } /* end for */
    } /* end else */
    break;
  case NC_INT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.ip[idx]=(nco_int)sqrt((double)(op1.ip[idx]));
	tally[idx]++;
      } /* end for */
    }else{
      const nco_int mss_val_ntg=*mss_val.ip;
      for(idx=0;idx<sz;idx++){
	if(op1.ip[idx] != mss_val_ntg){
	  op2.ip[idx]=(nco_int)sqrt((double)(op1.ip[idx]));
	  tally[idx]++;
	} /* end if */
      } /* end for */
    } /* end else */
    break;
  case NC_SHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.sp[idx]=(nco_short)sqrt((double)(op1.sp[idx]));
	tally[idx]++;
      } /* end for */
    }else{
      const nco_short mss_val_short=*mss_val.sp;
      for(idx=0;idx<sz;idx++){
	if(op1.sp[idx] != mss_val_short){
	  op2.sp[idx]=(nco_short)sqrt((double)(op1.sp[idx]));
	  tally[idx]++;
	} /* end if */
      } /* end for */
    } /* end else */
    break;
  case NC_USHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.usp[idx]=(nco_ushort)sqrt((double)(op1.usp[idx]));
	tally[idx]++;
      } /* end for */
    }else{
      const nco_ushort mss_val_ushort=*mss_val.usp;
      for(idx=0;idx<sz;idx++){
	if(op1.usp[idx] != mss_val_ushort){
	  op2.usp[idx]=(nco_ushort)sqrt((double)(op1.usp[idx]));
	  tally[idx]++;
	} /* end if */
      } /* end for */
    } /* end else */
    break;
  case NC_UINT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.uip[idx]=(nco_uint)sqrt((double)(op1.uip[idx]));
	tally[idx]++;
      } /* end for */
    }else{
      const nco_uint mss_val_uint=*mss_val.uip;
      for(idx=0;idx<sz;idx++){
	if(op1.uip[idx] != mss_val_uint){
	  op2.uip[idx]=(nco_uint)sqrt((double)(op1.uip[idx]));
	  tally[idx]++;
	} /* end if */
      } /* end for */
    } /* end else */
    break;
  case NC_INT64:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.i64p[idx]=(nco_int64)sqrt((double)(op1.i64p[idx]));
	tally[idx]++;
      } /* end for */
    }else{
      const nco_int64 mss_val_int64=*mss_val.i64p;
      for(idx=0;idx<sz;idx++){
	if(op1.i64p[idx] != mss_val_int64){
	  op2.i64p[idx]=(nco_int64)sqrt((double)(op1.i64p[idx]));
	  tally[idx]++;
	} /* end if */
      } /* end for */
    } /* end else */
    break;
  case NC_UINT64:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.ui64p[idx]=(nco_uint64)sqrt((double)(op1.ui64p[idx]));
	tally[idx]++;
      } /* end for */
    }else{
      const nco_uint64 mss_val_uint64=*mss_val.ui64p;
      for(idx=0;idx<sz;idx++){
	if(op1.ui64p[idx] != mss_val_uint64){
	  op2.ui64p[idx]=(nco_uint64)sqrt((double)(op1.ui64p[idx]));
	  tally[idx]++;
	} /* end if */
      } /* end for */
    } /* end else */
    break;
  case NC_BYTE:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.bp[idx]=(nco_byte)sqrt((double)(op1.bp[idx]));
	tally[idx]++;
      } /* end for */
    }else{
      const nco_byte mss_val_byte=*mss_val.bp;
      for(idx=0;idx<sz;idx++){
	if(op1.bp[idx] != mss_val_byte){
	  op2.bp[idx]=(nco_byte)sqrt((double)(op1.bp[idx]));
	  tally[idx]++;
	} /* end if */
      } /* end for */
    } /* end else */
    break;
  case NC_UBYTE:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.ubp[idx]=(nco_ubyte)sqrt((double)(op1.ubp[idx]));
	tally[idx]++;
      } /* end for */
    }else{
      const nco_ubyte mss_val_ubyte=*mss_val.ubp;
      for(idx=0;idx<sz;idx++){
	if(op1.ubp[idx] != mss_val_ubyte){
	  op2.ubp[idx]=(nco_ubyte)sqrt((double)(op1.ubp[idx]));
	  tally[idx]++;
	} /* end if */
      } /* end for */
    } /* end else */
    break;
  case NC_CHAR: break; /* Do nothing */
  case NC_STRING: break; /* Do nothing */
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
  
  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */
  
} /* end nco_var_sqrt() */

void
nco_var_zero /* [fnc] Zero value of first operand */
(const nc_type type, /* I [enm] netCDF type of operand */
 const long sz, /* I [nbr] Size (in elements) of operand */
 ptr_unn op1) /* O [val] Values of first operand zeroed on output */
{
  /* Purpose: Zero value of first operand */
  
  /* fxm: According to hjm, floats and ints all use same bit pattern for zero
     ccc --tst=bnr --int_foo=0 
     and
     ccc --dbg=0 --tst=gsl --gsl_a=0.0 
     confirm this.
     Hence, it may be faster to use memset() system call to zero memory 
     Same approach is used in nco_zero_long() */
  
  size_t sz_byt; /* [B] Number of bytes in variable buffer */
  sz_byt=(size_t)sz*nco_typ_lng(type);
  switch(type){
  case NC_FLOAT:
  case NC_DOUBLE:
  case NC_INT:
  case NC_SHORT:
  case NC_USHORT:
  case NC_UINT:
  case NC_INT64:
  case NC_UINT64:
  case NC_BYTE:
  case NC_UBYTE:
    (void)memset(op1.vp,0,sz_byt);
    break;
  case NC_CHAR: break; /* Do nothing */
  case NC_STRING: break; /* Do nothing */
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
  
#if 0
  /* Presumably this old method (used until 20050321) is slower because of pointer de-referencing */
  long idx;
  
  /* Typecast pointer to values before access */
  (void)cast_void_nctype(type,&op1);
  
  switch(type){
  case NC_FLOAT:
    for(idx=0;idx<sz;idx++) op1.fp[idx]=0.0;
    break;
  case NC_DOUBLE:
    for(idx=0;idx<sz;idx++) op1.dp[idx]=0.0;
    break;
  case NC_INT:
    for(idx=0;idx<sz;idx++) op1.ip[idx]=0L;
    break;
  case NC_SHORT:
    for(idx=0;idx<sz;idx++) op1.sp[idx]=0;
    break;
  case NC_USHORT:
    for(idx=0;idx<sz;idx++) op1.usp[idx]=0;
    break;
  case NC_UINT:
    for(idx=0;idx<sz;idx++) op1.uip[idx]=0;
    break;
  case NC_INT64:
    for(idx=0;idx<sz;idx++) op1.i64p[idx]=0;
    break;
  case NC_UINT64:
    for(idx=0;idx<sz;idx++) op1.ui64p[idx]=0;
    break;
  case NC_BYTE:
    for(idx=0;idx<sz;idx++) op1.bp[idx]=0;
    break;
  case NC_UBYTE:
    for(idx=0;idx<sz;idx++) op1.ubp[idx]=0;
    break;
  case NC_CHAR: break; /* Do nothing */
  case NC_STRING: break; /* Do nothing */
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
#endif /* !0 */
  
  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */
  
} /* end nco_var_zero() */
