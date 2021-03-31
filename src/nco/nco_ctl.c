/* $Header$ */

/* Purpose: Program flow control functions */

/* Copyright (C) 1995--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

#include "nco_ctl.h" /* Program flow control functions */

const char * /* O [sng] Compiler and version */
nco_cmp_get(void) /* [fnc] Return compiler and version */
{ 
  /* Purpose: Return string containing compiler and version */
  const char fnc_nm[]="nco_cmp_get()";
#if defined(_AIX) && !defined(__GNUC__) && !defined(__xlC__)
#define NCO_XLC_LIKELY
  static const char cmp_nm[]="xlc"; /* [sng] Compiler name */
  static const char cmp_sng[]="Token _AIX defined in nco_cmp_get(), probably compiled with AIX xlc_r or xlc"; /* [sng] Compiler string */
#endif /* !_AIX */
#if defined(__xlC__)
#define NCO_XLC_LIKELY
  static const char cmp_nm[]="xlC"; /* [sng] Compiler name */
  static const char cmp_sng[]="Token __xlC__ defined in nco_cmp_get(), probably compiled with AIX xlC_r or xlC"; /* [sng] Compiler string */
#endif /* !__xlC__ */
#if defined(__INTEL_COMPILER)
  // https://software.intel.com/content/www/us/en/develop/documentation/cpp-compiler-developer-guide-and-reference/top/compiler-reference/macros/additional-predefined-macros.html
  /* Some compilers, including icc, also define __GNUC__ by default */
  static const char cmp_nm[]="icc";
  static const char cmp_sng[]="Token __INTEL_COMPILER defined in nco_cmp_get(), probably compiled with Intel icc"; /* [sng] Compiler string */
  static const char itl_vrs[]=TKN2SNG(__INTEL_COMPILER); // [sng] Compiler version
  if(nco_dbg_lvl_get() >= nco_dbg_std){
    (void)fprintf(stderr,"%s: INFO icc version defined as __INTEL_COMPILER is %s\n",nco_prg_nm_get(),itl_vrs);
    (void)fprintf(stderr,"%s: INFO icc version defined as __INTEL_COMPILER_BUILD_DATE is %d\n",nco_prg_nm_get(),__INTEL_COMPILER_BUILD_DATE);
  } /* endif dbg */
#endif /* !__INTEL_COMPILER */
#if defined(__GNUC__) && !defined(__clang__) && !defined(__INTEL_COMPILER) && !defined(__NVCC__) && !defined(__PATHCC__) && !defined(PGI_CC)
  /* Testing for GCC macros early is dangerous because some compilers, 
     including Intel icc and clang, define GCC macros for compatibility */
#if defined(__GNUG__)
  static const char cmp_nm[]="g++"; /* [sng] Compiler name */
  static const char cmp_sng[]="Token __GNUG__ defined in nco_cmp_get(). Compiled with GNU g++ (or a compiler that emulates g++)."; /* [sng] Compiler string */
#else /* !__GNUG__ */
  static const char cmp_nm[]="gcc"; /* [sng] Compiler name */
  static const char cmp_sng[]="Token __GNUC__ defined in nco_cmp_get(). Compiled with GNU gcc (or a compiler that emulates gcc)."; /* [sng] Compiler string */
#endif /* !__GNUG__ */
  static const char cmp_vrs[]=TKN2SNG(__VERSION__); // [sng] Compiler version
  static const char cmp_vrs_mjr[]=TKN2SNG(__GNUC__); // [sng] Compiler major version
  static const char cmp_vrs_mnr[]=TKN2SNG(__GNUC_MINOR__); // [sng] Compiler minor version
  static const char cmp_vrs_pch[]=TKN2SNG(__GNUC_PATCHLEVEL__); // [sng] Compiler patch version

  /* 20200519: Construct numeric library version */
# define GCC_LIB_VERSION ( __GNUC__ * 100 + __GNUC_MINOR__ * 10 + __GNUC_PATCHLEVEL__ )

  if(nco_dbg_lvl_get() >= nco_dbg_fl){
    (void)fprintf(stderr,"%s: INFO GCC major version is %s\n",nco_prg_nm_get(),cmp_vrs_mjr);
    (void)fprintf(stderr,"%s: INFO GCC minor version is %s\n",nco_prg_nm_get(),cmp_vrs_mnr);
    (void)fprintf(stderr,"%s: INFO GCC patch version is %s\n",nco_prg_nm_get(),cmp_vrs_pch);
  } /* endif dbg */
  if(nco_dbg_lvl_get() >= nco_dbg_std){
    (void)fprintf(stderr,"%s: INFO GCC version defined as __VERSION__ is %s\n",nco_prg_nm_get(),cmp_vrs);
    (void)fprintf(stderr,"%s: INFO GCC version constructed as integer is %d\n",nco_prg_nm_get(),GCC_LIB_VERSION);
  } /* endif dbg */
#endif /* !__GNUC__ */
#if defined(__clang__) && !defined(__INTEL_COMPILER)
  /* Some compilers, including clang, also define __GNUC__ by default */
  static const char cmp_nm[]="clang";
  static const char cmp_sng[]="Token __clang__ defined in nco_cmp_get(), compiled with LLVM clang"; /* [sng] Compiler string */
  /* 20200513 Obtain clang info with this trick from LWN:
     cc -dM -E - < /dev/null | grep clang */
  static const char clg_vrs[]=TKN2SNG(__clang_version__); // [sng] Compiler version
  static const char clg_vrs_mjr[]=TKN2SNG(__clang_major__); // [sng] Compiler major version
  static const char clg_vrs_mnr[]=TKN2SNG(__clang_minor__); // [sng] Compiler minor version
  static const char clg_vrs_pch[]=TKN2SNG(__clang_patchlevel__); // [sng] Compiler patch version
  if(nco_dbg_lvl_get() >= nco_dbg_fl){
    (void)fprintf(stderr,"%s: INFO clang major version is %s\n",nco_prg_nm_get(),clg_vrs_mjr);
    (void)fprintf(stderr,"%s: INFO clang minor version is %s\n",nco_prg_nm_get(),clg_vrs_mnr);
    (void)fprintf(stderr,"%s: INFO clang patch version is %s\n",nco_prg_nm_get(),clg_vrs_pch);
  } /* endif dbg */
  /* 20200519: Construct numeric library version */
# define CLANG_LIB_VERSION ( __clang_major__ * 100 + __clang_minor__ * 10 + __clang_patchlevel__ )
  if(nco_dbg_lvl_get() >= nco_dbg_std){
    (void)fprintf(stderr,"%s: INFO clang version defined as __clang_version__ is %s\n",nco_prg_nm_get(),clg_vrs);
    (void)fprintf(stderr,"%s: INFO clang version constructed as integer is %d\n",nco_prg_nm_get(),CLANG_LIB_VERSION);
  } /* endif dbg */
#endif /* !__clang__ */
#ifdef __NVCC__
  /* https://docs.nvidia.com/cuda/cuda-compiler-driver-nvcc/index.html */
  static const char cmp_nm[]="nvc";
  static const char cmp_sng[]="Token __NVCC__ defined in nco_cmp_get(), probably compiled with Nvidia CUDA nvc"; /* [sng] Compiler string */
  static const char nvd_vrs[]=TKN2SNG(__CUDACC_VER_BUILD__); // [sng] Compiler version
  static const char nvd_vrs_mjr[]=TKN2SNG(__CUDACC_VER_MAJOR__); // [sng] Compiler major version
  static const char nvd_vrs_mnr[]=TKN2SNG(__CUDACC_VER_MINOR__); // [sng] Compiler minor version
  if(nco_dbg_lvl_get() >= nco_dbg_fl){
    (void)fprintf(stderr,"%s: INFO nvc major version is %s\n",nco_prg_nm_get(),nvd_vrs_mjr);
    (void)fprintf(stderr,"%s: INFO nvc minor version is %s\n",nco_prg_nm_get(),nvd_vrs_mnr);
  } /* endif dbg */
  /* 20201210: Construct numeric library version */
# define NVC_LIB_VERSION ( __CUDACC_VER_MAJOR__ * 100 + __CUDACC_VER_MINOR__ * 10 )
  if(nco_dbg_lvl_get() >= nco_dbg_std){
    (void)fprintf(stderr,"%s: INFO nvc version defined as __CUDACC_VER_BUILD__ is %s\n",nco_prg_nm_get(),nvd_vrs);
    (void)fprintf(stderr,"%s: INFO nvc version constructed as integer is %d\n",nco_prg_nm_get(),NVC_LIB_VERSION);
  } /* endif dbg */
#endif /* !__NVCC__ */
#ifdef __PATHCC__
  /* Some compilers, including pathcc, also define __GNUC__ by default */
  static const char cmp_nm[]="pathcc";
  static const char cmp_sng[]="Token __PATHCC__ defined in nco_cmp_get(), probably compiled with PathScale (Qlogic) pathcc"; /* [sng] Compiler string */
#endif /* !__PATHCC__ */
#ifdef PGI_CC
  static const char cmp_nm[]="pgcc";
  static const char cmp_sng[]="Token PGI_CC defined in nco_cmp_get(), probably compiled with PGI pgcc"; /* [sng] Compiler string */
#endif /* !PGI_CC */

  /* No tokens matched */
#if !defined(NCO_XLC_LIKELY) && !defined(__clang__) && !defined(__GNUC__) && !defined(__INTEL_COMPILER) && !defined(__NVCC__) && !defined(__PATHCC__) && !defined(PGI_CC)
  /* Unknown compiler */
  static const char cmp_nm[]="unknown"; /* [sng] Compiler name */
  static const char cmp_sng[]="Unknown compiler tokens in nco_cmp_get(), compiler is unknown"; /* [sng] Compiler string */
#endif /* !unknown */

  if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stderr,"%s: INFO %s reports compiler name is \"%s\"\n%s\n",nco_prg_nm_get(),fnc_nm,cmp_nm,cmp_sng);

  return cmp_nm;
} /* end nco_cmp_get() */

const char * /* O [sng] MPI implementation */
nco_mpi_get(void) /* [fnc] Return MPI implementation */
{ 
  /* Purpose: Return string containing MPI implementation information */
  const char fnc_nm[]="nco_mpi_get()";

#ifdef _H_MPI
  static const char mpi_nm[]="PPE"; /* [sng] MPI name */
  static const char mpi_sng[]="Token _H_MPI defined in nco_mpi_get(), MPI environment is probably AIX PPE MPI"; /* [sng] MPI string */
#endif /* !_H_MPI */
#ifdef LAM_MPI
  static const char mpi_nm[]="LAM"; /* [sng] MPI name */
  static const char mpi_sng[]="Token LAM_MPI defined in nco_mpi_get(), MPI environment is probably LAM-MPI"; /* [sng] MPI string */
#endif /* !LAM_MPI */
#if MPICH_NAME == '1'
  static const char mpi_nm[]="MPICH"; /* [sng] MPI name */
  static const char mpi_sng[]="Token MPICH_NAME defined to 1 in nco_mpi_get(), MPI environment is probably MPICH version 1"; /* [sng] MPI string */
#endif /* !MPICH_NAME */
#ifdef MPICH2
  static const char mpi_nm[]="MPICH2"; /* [sng] MPI name */
  static const char mpi_sng[]="Token MPICH2 defined in nco_mpi_get(), MPI environment is probably MPICH2, i.e., MPICH version 2"; /* [sng] MPI string */
#endif /* !MPICH2 */
#ifdef OMPI_MPI_H
  static const char mpi_nm[]="OPENMPI"; /* [sng] MPI name */
  static const char mpi_sng[]="Token OMPI_MPI_H defined in nco_mpi_get(), MPI environment is probably OpenMPI"; /* [sng] MPI string */
#endif /* !OMPI_MPI_H */

  /* In case no token matched */
#if !defined(_H_MPI) && !defined(LAM_MPI) && (MPICH_NAME != '1') && !defined(MPICH2) && !defined(OMPI_MPI_H)
#ifndef MPI_VERSION
  /* MPI is not installed */
  static const char mpi_nm[]="none"; /* [sng] MPI name */
  static const char mpi_sng[]="No MPI tokens found in nco_mpi_get(), MPI environment is not active"; /* [sng] MPI string */
#else /* MPI_VERSION */
  /* Unknown MPI implementation */
  static const char mpi_nm[]="unknown"; /* [sng] MPI name */
  static const char mpi_sng[]="Unknown MPI tokens found in nco_mpi_get(), MPI environment is present and of unknown pedigree"; /* [sng] MPI string */
#endif /* MPI_VERSION */
#endif /* !unknown */

  if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stderr,"%s: INFO %s reports MPI implementation name is \"%s\"\n%s\n",nco_prg_nm_get(),fnc_nm,mpi_nm,mpi_sng);
  return mpi_nm;
} /* end nco_mpi_get() */

int /* O [rcd] Return code */
nco_ddra /* [fnc] Count operations */
(const char * const var_nm, /* I [sng] Variable name */
 const char * const wgt_nm, /* I [sng] Weight name */
 const ddra_info_sct * const ddra_info) /* I [sct] DDRA information */
{
  /* Purpose: Estimate operation counts required */

  /* 20070509 CEWI RUVICFFU: ntg_nbr_rdc_dfl, ntg_nbr_nrm_dfl, flp_nbr_rdc_dfl, flp_nbr_nrm_dfl, rnk_avg */

  const char fnc_nm[]="nco_ddra()";
  
  /* Following speed parameter estimates are for clay.ess.uci.edu 
     clay is the dual opteron used in ppr_ZeM07 */
  const float ntg_nbr_brd_fdg_fct=1.8f; /* [frc] Empirical correction to broadcasting */
  const float spd_flp_ncwa=153e6f; /* [# s-1] Floating point operation speed */
  const float spd_ntg_ncwa=200e6f; /* [# s-1] Integer operation speed */
  const float spd_flp_ncbo=353.2e6f; /* [# s-1] Floating point operation speed */
  const float spd_ntg_ncbo=1386.54e6f; /* [# s-1] Integer operation speed */
  const float spd_rd=63.375e6f; /* [B s-1] Disk read bandwidth */
  const float spd_wrt=57.865e6f; /* [B s-1] Disk write bandwidth */

  /* 20060602: 
     hjm estimates faster speeds for ncbo:     
     spd_flp_ncbo = 353.2e6: Based on unoptimized operations in tacg function
     spd_ntg_ncbo = 1386.54e6: Based on counting unoptimized operations in tacg function
     hjm estimates slower speeds for ncwa:     
     spd_flp_ncwa = 153e6: Based on direct PAPI measures
     spd_ntg_ncwa = 200e6: Based on direct measure, using large data set, calculating backwards */

  float spd_flp=float_CEWI; /* [# s-1] Floating point operation speed */
  float spd_ntg=float_CEWI; /* [# s-1] Integer operation speed */
  int rcd=NC_NOERR; /* [rcd] Return code */

  /* Timer information */
  static clock_t tm_obs_old; /* [us] Microseconds initially */
  static float tm_obs_ttl=0.0; /* [s] Total seconds elapsed */
  clock_t tm_obs_crr; /* [us] Microseconds currently */
  float tm_obs_dff; /* [s] Seconds elapsed */
  
  /* Cumulative file costs */
  static long long lmn_nbr_ttl=0LL; /* I/O [nbr] Cumulative variable size */
  static long long ntg_nbr_ttl=0LL; /* I/O [nbr] Cumulative integer operations */
  static long long flp_nbr_ttl=0LL; /* I/O [nbr] Cumulative floating point operations */

  /* Cumulative times */
  static float tm_ntg_ttl=0.0f; /* I/O [s] Cumulative integer time */
  static float tm_flp_ttl=0.0f; /* I/O [s] Cumulative floating point time */
  static float tm_rd_ttl=0.0f; /* I/O [s] Cumulative read time */
  static float tm_wrt_ttl=0.0f; /* I/O [s] Cumulative write time */
  static float tm_io_ttl=0.0f; /* [s] I/O time */
  static float tm_ttl=0.0f; /* I/O [s] Cumulative time */
  
  /* Current variable costs */
  float tm_ntg; /* [s] Integer time */
  float tm_flp; /* [s] Floating point time */
  float tm_rd; /* [s] Read time */
  float tm_wrt; /* [s] Write time */
  float tm_io=float_CEWI; /* [s] I/O time */
  float tm_crr=float_CEWI; /* [s] Time for this variable */
  long long ntg_nbr=long_long_CEWI; /* [nbr] Integer operations */
  long long flp_nbr=long_long_CEWI; /* [nbr] Floating point operations */
  long long rd_nbr_byt=long_long_CEWI; /* [B] Bytes read */
  long long wrt_nbr_byt=long_long_CEWI; /* [B] Bytes written */
  
  /* Summary statistics */
  float tm_frc_flp_ttl=float_CEWI; /* [frc] Floating point time fraction */
  float tm_frc_io_ttl=float_CEWI; /* [frc] I/O time fraction */
  float tm_frc_ntg_ttl=float_CEWI; /* [frc] Integer time fraction */
  float tm_frc_rd_ttl=float_CEWI; /* [frc] Read time fraction */
  float tm_frc_wrt_ttl=float_CEWI; /* [frc] Write time fraction */

  /* Default algorithm costs if invoked */
  long long ntg_nbr_byt_swp_dfl; /* [nbr] Integer operations for byte-swap */
  long long ntg_nbr_brd_dfl; /* [nbr] Integer operations for broadcasting */
  long long ntg_nbr_clc_dfl; /* [nbr] Integer operations for collection */
  /* long long ntg_nbr_rdc_dfl; */ /* [nbr] Integer operations for reduction */
  /* long long ntg_nbr_nrm_dfl; */ /* [nbr] Integer operations for normalization */
  long long flp_nbr_bnr_dfl; /* [nbr] Floating point operations for binary arithmetic */
  /* long long flp_nbr_rdc_dfl; */ /* [nbr] Floating point operations for reduction */
  /* long long flp_nbr_nrm_dfl; */ /* [nbr] Floating point operations for normalization */

  /* Initialize all algorithm counts for this variable to zero then increment */
  long long ntg_nbr_byt_swp=0LL; /* [nbr] Integer operations for byte-swap */
  long long ntg_nbr_brd=0LL; /* [nbr] Integer operations for broadcasting */
  long long ntg_nbr_clc=0LL; /* [nbr] Integer operations for collection */
  long long ntg_nbr_rdc=0LL; /* [nbr] Integer operations for reduction */
  long long ntg_nbr_nrm=0LL; /* [nbr] Integer operations for normalization */
  long long flp_nbr_bnr=0LL; /* [nbr] Floating point operations for binary arithmetic */
  long long flp_nbr_rdc=0LL; /* [nbr] Floating point operations for reduction */
  long long flp_nbr_nrm=0LL; /* [nbr] Floating point operations for normalization */

  /* Copies of DDRA information */
  int nco_op_typ; /* [enm] Operation type */
  /* int rnk_avg; */ /* [nbr] Rank of averaging space */
  int rnk_var; /* [nbr] Variable rank (in input file) */
  int rnk_wgt; /* [nbr] Rank of weight */
  int var_idx=int_CEWI; /* [enm] Index */
  int wrd_sz; /* [B] Bytes per element */
  long long lmn_nbr=int_CEWI; /* [nbr] Variable size */
  long long lmn_nbr_avg; /* [nbr] Averaging block size */
  long long lmn_nbr_wgt; /* [nbr] Weight size */
  nco_bool MRV_flg; /* [flg] Avergaging dimensions are MRV dimensions */
  nco_bool wgt_brd_flg; /* [flg] Broadcast weight for this variable */

  /* Locals */
  long long lmn_nbr_out=long_long_CEWI; /* [nbr] Output elements */

  /* Where possible, work in terms of "default" counts per algorithm
     Algorithms (e.g., reduction, byte-swapping) are generally subroutines
     Record hand-counted counts in only one place
     This makes it easier and less error-prone to propagate optimizations
     Also, easier to think in terms of procedures than raw counts

     Algorithms for subtraction:
     "byte_swap+byte_swap+binary_arithmetic+byte_swap"
     Algorithms for non-weighted, non-MRV averaging:
     "byte_swap(input)+collection+reduction+normalization+byte_swap(output)"
     Algorithms for non-weighted, MRV averaging:
     "byte_swap(input)+reduction+normalization+byte_swap(output)"
     Algorithms for weighted, non-MRV averaging:
     "byte_swap(input)+byte_swap(weight)+broadcast(weight)+
     collection(numerator)+collection(denominator)+
     binary_arithmetic(weight*value)+
     reduction(numerator)+reduction(denominator)+
     normalization(numerator)+normalization(denominator)+byte_swap(output)"
     Units: 
     [nbr] = Operation counts for lmn_nbr elements
     [nbr nbr-1] = Operation counts per element */

  switch(ddra_info->tmr_flg){
  case nco_tmr_srt: /* [enm] Initialize timer (first timer call) */
    tm_obs_crr=clock();
    tm_obs_old=tm_obs_crr;
    return rcd; /* [rcd] Return code */
    break;
  case nco_tmr_mtd: /* [enm] Metadata timer (second timer call) */
  case nco_tmr_end: /* [enm] Close timer (last timer call) */
    goto update_timers;
    break;
  case nco_tmr_rgl: /* [enm] Regular timer call (main loop timer call) */
    break;
  default: nco_dfl_case_tmr_typ_err(); break;
  } /* end switch */

  /* Decode input */
  MRV_flg=ddra_info->MRV_flg; /* [flg] Avergaging dimensions are MRV dimensions */
  lmn_nbr=ddra_info->lmn_nbr; /* [nbr] Variable size */
  lmn_nbr_avg=ddra_info->lmn_nbr_avg; /* [nbr] Averaging block size */
  lmn_nbr_wgt=ddra_info->lmn_nbr_wgt; /* [nbr] Weight size */
  nco_op_typ=ddra_info->nco_op_typ; /* [enm] Operation type */
  /* rnk_avg=ddra_info->rnk_avg; */ /* [nbr] Rank of averaging space */
  rnk_var=ddra_info->rnk_var; /* [nbr] Variable rank (in input file) */
  rnk_wgt=ddra_info->rnk_wgt; /* [nbr] Rank of weight */
  var_idx=ddra_info->var_idx; /* [enm] Index */
  wgt_brd_flg=ddra_info->wgt_brd_flg; /* [flg] Broadcast weight for this variable */
  wrd_sz=ddra_info->wrd_sz; /* [B] Bytes per element */

  /* Derived variables */
  switch(nco_op_typ){
    /* Types used in ncbo(), ncflint() */
  case nco_op_add: /* [enm] Add file_1 to file_2 */
  case nco_op_dvd: /* [enm] Divide file_1 by file_2 */
  case nco_op_mlt: /* [enm] Multiply file_1 by file_2 */
  case nco_op_sbt: /* [enm] Subtract file_2 from file_1 */
    spd_flp=spd_flp_ncbo; /* [# s-1] Floating point operation speed */
    spd_ntg=spd_ntg_ncbo; /* [# s-1] Integer operation speed */
    lmn_nbr_out=lmn_nbr; /* [nbr] Output elements */
    break;
    /* Types used in ncra(), ncrcat(), ncwa(): */
  case nco_op_avg: /* [enm] Average */
  case nco_op_min: /* [enm] Minimum value */
  case nco_op_max: /* [enm] Maximum value */
  case nco_op_ttl: /* [enm] Linear sum */
  case nco_op_sqravg: /* [enm] Square of mean */
  case nco_op_avgsqr: /* [enm] Mean of sum of squares */
  case nco_op_sqrt: /* [enm] Square root of mean */
  case nco_op_rms: /* [enm] Root-mean-square (normalized by N) */
  case nco_op_rmssdn: /* [enm] Root-mean square normalized by N-1 */
    spd_flp=spd_flp_ncwa; /* [# s-1] Floating point operation speed */
    spd_ntg=spd_ntg_ncwa; /* [# s-1] Integer operation speed */
    lmn_nbr_out=lmn_nbr/lmn_nbr_avg; /* [nbr] Output elements */
    break;
  case nco_op_nil: /* [enm] Nil or undefined operation type  */
    break;
  default:
    (void)fprintf(stdout,"%s: ERROR Illegal nco_op_typ in %s\n",nco_prg_nm_get(),fnc_nm);
    nco_exit(EXIT_FAILURE);
    break;
  } /* end switch */

  flp_nbr_bnr_dfl=lmn_nbr; /* [nbr] Floating point operations for binary arithmetic */
  /* flp_nbr_nrm_dfl=lmn_nbr_out; */ /* [nbr] Floating point operations for normalization */
  /* flp_nbr_rdc_dfl=lmn_nbr; */ /* [nbr] Floating point operations for reduction */

  /* Integer operations for broadcasting weight */
  ntg_nbr_brd_dfl=(long long int)(ntg_nbr_brd_fdg_fct*lmn_nbr*(6*rnk_var+8*rnk_wgt+2)); /* [nbr] N(6R+8R_w+2) */

  /* Byte-swap integer operations per element */
  ntg_nbr_byt_swp_dfl=wrd_sz+2; /* [nbr nbr-1] W+2 */

  /* Integer operations for collection */
  ntg_nbr_clc_dfl=lmn_nbr*(14*rnk_var+4); /* [nbr] N(14R+4) */

  /* Integer operations for normalization */
  /* ntg_nbr_nrm_dfl=4*lmn_nbr_out; */ /* [nbr] 4N/N_A = 4N_O */

  /* Integer operations for reduction */
  /* ntg_nbr_rdc_dfl=lmn_nbr*6+lmn_nbr_out; */ /* [nbr] N(6+N/N_A) */

  switch(nco_op_typ){
    /* Types used in ncbo(), ncflint() */
  case nco_op_add: /* [enm] Add file_1 to file_2 */
  case nco_op_dvd: /* [enm] Divide file_1 by file_2 */
  case nco_op_mlt: /* [enm] Multiply file_1 by file_2 */
  case nco_op_sbt: /* [enm] Subtract file_2 from file_1 */
    /* Subtraction computation assumes variables are same size
       fxm: Account for broadcasting */
    /* One floating point (add/subtract/multiply/divide) per element */
    flp_nbr_bnr=flp_nbr_bnr_dfl;
    /* Byte-swap elements from two input files and one output file */
    ntg_nbr_byt_swp=3*lmn_nbr*ntg_nbr_byt_swp_dfl; /* 3N(W+2) */
    rd_nbr_byt=2*lmn_nbr*wrd_sz; /* [B] Bytes read */
    wrt_nbr_byt=lmn_nbr_out*wrd_sz; /* [B] Bytes written */
    break;
    /* Types used in ncra(), ncrcat(), ncwa(): */
  case nco_op_avg: /* [enm] Average */
  case nco_op_min: /* [enm] Minimum value */
  case nco_op_max: /* [enm] Maximum value */
  case nco_op_ttl: /* [enm] Linear sum */
  case nco_op_sqravg: /* [enm] Square of mean */
  case nco_op_avgsqr: /* [enm] Mean of sum of squares */
  case nco_op_sqrt: /* [enm] Square root of mean */
  case nco_op_rms: /* [enm] Root-mean-square (normalized by N) */
  case nco_op_rmssdn: /* [enm] Root-mean square normalized by N-1 */
    rd_nbr_byt=lmn_nbr*wrd_sz; /* [B] Bytes read */
    wrt_nbr_byt=lmn_nbr_out*wrd_sz; /* [B] Bytes written */
    /* One floating point add per input element to sum numerator */
    flp_nbr_rdc=lmn_nbr;
    /* One floating point divide per output element to normalize numerator by denominatro (tally) */
    flp_nbr_nrm=lmn_nbr_out;
    /* Byte-swap elements from one input file and one (rank-reduced) output file */
    ntg_nbr_byt_swp=(lmn_nbr+lmn_nbr_out)*ntg_nbr_byt_swp_dfl;
    if(!MRV_flg){
      /* Collection required for numerator */
      ntg_nbr_clc+=ntg_nbr_clc_dfl;
    } /* !MRV_flg */
    if(wgt_nm){
      if(var_idx == 0){
	/* Set cost = 0 after first variable since only read weight once */
	rd_nbr_byt+=lmn_nbr_wgt*wrd_sz; /* [B] Bytes read */
	/* Byte-swap cost for first weight input is usually negligible */
	ntg_nbr_byt_swp+=lmn_nbr_wgt*ntg_nbr_byt_swp_dfl;
      } /* var_idx != 0 */
      /* One floating point multiply per input element for weight*value in numerator,
	 and one floating point add per input element to sum weight in denominator */
      flp_nbr_rdc+=2*lmn_nbr;
      /* One floating point divide per output element to normalize denominator by tally */
      flp_nbr_nrm+=lmn_nbr_out;
      if(wgt_brd_flg){
	/* fxm: Charge for broadcasting weight at least once */
	/* Broadcasting cost for weight */
	ntg_nbr_brd=ntg_nbr_brd_dfl;
      } /* !wgt_brd_flg */
      if(!MRV_flg){
	/* Collection required for denominator */
	ntg_nbr_clc+=ntg_nbr_clc_dfl;
      } /* !MRV_flg */
    } /* !wgt_nm */
    break;
  case nco_op_nil: /* [enm] Nil or undefined operation type  */
    break;
  default:
    (void)fprintf(stdout,"%s: ERROR Illegal nco_op_typ in %s\n",nco_prg_nm_get(),fnc_nm);
    nco_exit(EXIT_FAILURE);
    break;
  } /* end switch */

  flp_nbr= /* [nbr] Floating point operations */
    flp_nbr_bnr+ /* [nbr] Floating point operations for binary arithmetic */
    flp_nbr_rdc+ /* [nbr] Floating point operations for reduction */
    flp_nbr_nrm; /* [nbr] Floating point operations for normalization */

  ntg_nbr= /* [nbr] Integer operations */
    ntg_nbr_byt_swp+ /* [nbr] Integer operations for byte-swap */
    ntg_nbr_brd+ /* [nbr] Integer operations for broadcasting */
    ntg_nbr_clc+ /* [nbr] Integer operations for collection */
    ntg_nbr_rdc+ /* [nbr] Integer operations for reduction */
    ntg_nbr_nrm; /* [nbr] Integer operations for normalization */

  tm_ntg=ntg_nbr/spd_ntg; /* [s] Integer time */
  tm_flp=flp_nbr/spd_flp; /* [s] Floating point time */
  tm_rd=rd_nbr_byt/spd_rd; /* [s] Read time */
  tm_wrt=wrt_nbr_byt/spd_wrt; /* [s] Write time */

  tm_io=tm_rd+tm_wrt; /* [s] I/O time */
  tm_crr=tm_ntg+tm_flp+tm_rd+tm_wrt; /* [s] Time for this variable */

  /* Increment running totals */
  lmn_nbr_ttl+=lmn_nbr; /* [nbr] Cumulative variable size */
  flp_nbr_ttl+=flp_nbr; /* [nbr] Cumulative floating point operations */
  ntg_nbr_ttl+=ntg_nbr; /* [nbr] Cumulative integer operations */
  
  tm_ntg_ttl+=tm_ntg; /* [s] Cumulative integer time */
  tm_flp_ttl+=tm_flp; /* [s] Cumulative floating point time */
  tm_rd_ttl+=tm_rd; /* [s] Cumulative read time */
  tm_wrt_ttl+=tm_wrt; /* [s] Cumulative write time */
  tm_io_ttl+=tm_io; /* [s] Cumulative I/O time */
  tm_ttl+=tm_crr; /* [s] Cumulative time */

  tm_frc_flp_ttl=tm_flp_ttl/tm_ttl; /* [frc] Floating point time fraction */
  tm_frc_io_ttl=tm_io_ttl/tm_ttl; /* [frc] I/O time fraction */
  tm_frc_ntg_ttl=tm_ntg_ttl/tm_ttl; /* [frc] Integer time fraction */
  tm_frc_rd_ttl=tm_rd_ttl/tm_ttl; /* [frc] Read time fraction */
  tm_frc_wrt_ttl=tm_wrt_ttl/tm_ttl; /* [frc] Write time fraction */

  if(var_idx == 0){
    /* Table headings */
    (void)fprintf(stderr,"%3s %8s %8s %8s %8s %5s %5s %8s %8s %8s %4s %4s %4s %4s %4s %7s %7s\n",
		  "idx"," var_nm ","   lmn  ","   flp  ","   ntg  ","tm_io","  tm "," lmn_ttl"," flp_ttl"," ntg_ttl"," flp"," ntg","  rd"," wrt"," io"," tm_ttl"," tm_obs");
    (void)fprintf(stderr,"%3s %8s %8s %8s %8s %5s %5s %8s %8s %8s %4s %4s %4s %4s %4s %7s %7s\n",
		  "   ","        ","    #   ","    #   ","    #   ","  s  ","  s  ","   #    ","   #    ","    #   ","  % ","  % ","  % ","  % ","  % ","   s   ","   s   ");
  } /* var_idx != 0 */

  /* Update timers */
 update_timers:
  /* NB: POSIX requires CLOCKS_PER_SEC equals 1000000 so resolution = 1 microsecond */
  tm_obs_crr=clock();
  tm_obs_dff=(float)(tm_obs_crr-tm_obs_old)/CLOCKS_PER_SEC;
  tm_obs_old=tm_obs_crr;
  tm_obs_ttl+=tm_obs_dff;

  switch(ddra_info->tmr_flg){
  case nco_tmr_mtd: /* [enm] Metadata timer (second timer call) */
    if(ddra_info->flg_ddra || nco_dbg_lvl_get() >= nco_dbg_fl) (void)fprintf(stderr,"%s: TIMER Metadata setup and file layout before main loop took %7.2f s\n",nco_prg_nm_get(),tm_obs_ttl);
    break;
  case nco_tmr_rgl: /* [enm] Regular timer call (main loop timer call) */
    (void)fprintf(stderr,
		  "%3d %8s %8.2e %8.2e %8.2e %5.2f %5.2f %8.2e %8.2e %8.2e %4.1f %4.1f %4.1f %4.1f %4.1f %7.2f %7.2f\n",
		  var_idx,var_nm,(float)lmn_nbr,(float)flp_nbr,(float)ntg_nbr,tm_io,tm_crr,(float)lmn_nbr_ttl,(float)flp_nbr_ttl,(float)ntg_nbr_ttl,100.0*tm_frc_flp_ttl,100.0*tm_frc_ntg_ttl,100.0*tm_frc_rd_ttl,100.0*tm_frc_wrt_ttl,100.0*tm_frc_io_ttl,tm_ttl,tm_obs_ttl);
    break;
  case nco_tmr_end: /* [enm] Close timer (last timer call) */
    if(ddra_info->flg_ddra || nco_dbg_lvl_get() >= nco_dbg_fl) (void)fprintf(stderr,"%s: TIMER Elapsed clock() time for command is %7.2f s\n",nco_prg_nm_get(),tm_obs_ttl);
    break;
  default: nco_dfl_case_tmr_typ_err(); break;
  } /* end switch */

  return rcd; /* [rcd] Return code */
} /* nco_ddra() */

void 
nco_dfl_case_tmr_typ_err(void) /* [fnc] Print error and exit for illegal switch(tmr_typ) case */
{
  /* Purpose: Convenience routine for printing error and exiting when
     switch(tmr_typ) statement receives an illegal default case

     Placing this in its own routine saves many lines 
     of code since this function is used in many many switch() statements. */
  const char fnc_nm[]="nco_dfl_case_tmr_typ_err()";
  (void)fprintf(stdout,"%s: ERROR switch(tmr_typ) statement fell through to default case, which is unsafe. This catch-all error handler ensures all switch(tmr_typ) statements are fully enumerated. Exiting...\n",fnc_nm);
  nco_err_exit(0,fnc_nm);
} /* end nco_dfl_case_tmr_typ_err() */

void
nco_exit /* [fnc] Wrapper for exit() */
(int rcd) /* I [enm] Return code */
{
  /* Purpose: Wrapper for exit()
     https://stackoverflow.com/questions/397075/what-is-the-difference-between-exit-and-abort
     "abort() exits your program without calling functions registered using atexit() first, and without calling objects' destructors first. exit() does both before exiting your program. It does not call destructors for automatic objects though."
     In other words, abort() allows debuggers to access memory at time of error while exit() does not */
  const char fnc_nm[]="nco_exit()";
#ifdef NCO_ABORT_ON_ERROR
  const char exit_nm[]="abort()";
#else /* !NCO_ABORT_ON_ERROR */
  const char exit_nm[]="exit(EXIT_FAILURE)";
#endif /* !NCO_ABORT_ON_ERROR */

  if(rcd == EXIT_SUCCESS){
    exit(rcd);
  }else{
    if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stdout,"%s: ERROR Exiting through %s which will now call %s\n",nco_prg_nm_get(),fnc_nm,exit_nm);
#ifdef NCO_ABORT_ON_ERROR
    /* abort() allows crashes to be backtraced, more useful to developers */
    abort();
#else /* !NCO_ABORT_ON_ERROR */
    /* exit() does not create core files that annoy users */
    exit(rcd);
#endif /* !NCO_ABORT_ON_ERROR */
  } /* endif rcd */
} /* nco_exit() */

void 
nco_exit_gracefully(void) /* [fnc] Clean up timers, file descriptors, memory, then exit */
{
  /* Purpose: Clean up timers, file descriptors, memory, then exit */
  (void)fclose(stderr);
  (void)fclose(stdin);
  (void)fclose(stdout);

  /* This should be penultimate command in program 
     After program name is free'd, calls to nco_prg_nm_get() will fail */
  (void)nco_free(nco_prg_nm_get());

  nco_exit(EXIT_SUCCESS);
} /* end nco_exit_gracefully() */

void
nco_exit_lbr_rcd(void) /* [fnc] Exit with netCDF library version as return code */
{
  /* Purpose: Exit with status equaling a numeric code determined by netCDF library version
     Exit status must be 0-255
     Modern netCDF versions range from 300-470
     Hence program returns netCDF version minus an offset of 300 so version 4.4.0 exit()s with $?=140
     Usage: ncks --lbr_rcd
     Verify exit status with "echo $?" */
  char lbr_sng[NC_MAX_NAME+1];

  int rcd=360;
#if defined(ENABLE_NETCDF4) && defined(HAVE_NETCDF4_H)
  strcpy(lbr_sng,nc_inq_libvers());
  rcd=400;
  /* Detect buggy netCDF version 4.1 so that workarounds may be implemented
     Other versions used to enable version-specific regression tests in NCO_rgr.pm */
  if(lbr_sng[0] == '4' && lbr_sng[1] == '.' && lbr_sng[2] == '1'){rcd=410;}
  else if(lbr_sng[0] == '4' && lbr_sng[1] == '.' && lbr_sng[2] == '3' && lbr_sng[3] == '.' && lbr_sng[4] == '0'){rcd=430;}
  else if(lbr_sng[0] == '4' && lbr_sng[1] == '.' && lbr_sng[2] == '3' && lbr_sng[3] == '.' && lbr_sng[4] == '1'){rcd=431;}
  else if(lbr_sng[0] == '4' && lbr_sng[1] == '.' && lbr_sng[2] == '3' && lbr_sng[3] == '.' && lbr_sng[4] == '2'){rcd=432;}
  /* NB: Same return values for 4.3.3 and 4.3.3.1. Few people installed 4.3.3, most installed 4.3.3.1. */
  else if(lbr_sng[0] == '4' && lbr_sng[1] == '.' && lbr_sng[2] == '3' && lbr_sng[3] == '.' && lbr_sng[4] == '3'){rcd=433;}
  else if(lbr_sng[0] == '4' && lbr_sng[1] == '.' && lbr_sng[2] == '4' && lbr_sng[3] == '.' && lbr_sng[4] == '0'){rcd=440;}
  /* NB: Same return values for 4.4.1 and 4.4.1.1, which simply fixes an ncgen bug in 4.4.1 */
  else if(lbr_sng[0] == '4' && lbr_sng[1] == '.' && lbr_sng[2] == '4' && lbr_sng[3] == '.' && lbr_sng[4] == '1'){rcd=441;}
  else if(lbr_sng[0] == '4' && lbr_sng[1] == '.' && lbr_sng[2] == '5' && lbr_sng[3] == '.' && lbr_sng[4] == '0'){rcd=450;}
  /* NB: 4.5.1 was never released, though some development branches employ this version number */
  else if(lbr_sng[0] == '4' && lbr_sng[1] == '.' && lbr_sng[2] == '5' && lbr_sng[3] == '.' && lbr_sng[4] == '1'){rcd=451;}
  /* NB: Same return values for 4.6.0 and 4.6.0.1, which appeared quickly after 4.6.0 */
  else if(lbr_sng[0] == '4' && lbr_sng[1] == '.' && lbr_sng[2] == '6' && lbr_sng[3] == '.' && lbr_sng[4] == '0'){rcd=460;}
  else if(lbr_sng[0] == '4' && lbr_sng[1] == '.' && lbr_sng[2] == '6' && lbr_sng[3] == '.' && lbr_sng[4] == '1'){rcd=461;}
  else if(lbr_sng[0] == '4' && lbr_sng[1] == '.' && lbr_sng[2] == '6' && lbr_sng[3] == '.' && lbr_sng[4] == '2'){rcd=462;}
  else if(lbr_sng[0] == '4' && lbr_sng[1] == '.' && lbr_sng[2] == '6' && lbr_sng[3] == '.' && lbr_sng[4] == '3'){rcd=463;}
  /* NB: 4.6.4 was never released, though some development branches employ this version number */
  else if(lbr_sng[0] == '4' && lbr_sng[1] == '.' && lbr_sng[2] == '6' && lbr_sng[3] == '.' && lbr_sng[4] == '4'){rcd=464;}
  else if(lbr_sng[0] == '4' && lbr_sng[1] == '.' && lbr_sng[2] == '7' && lbr_sng[3] == '.' && lbr_sng[4] == '0'){rcd=470;}
  else if(lbr_sng[0] == '4' && lbr_sng[1] == '.' && lbr_sng[2] == '7' && lbr_sng[3] == '.' && lbr_sng[4] == '1'){rcd=471;}
  else if(lbr_sng[0] == '4' && lbr_sng[1] == '.' && lbr_sng[2] == '7' && lbr_sng[3] == '.' && lbr_sng[4] == '2'){rcd=472;}
  else if(lbr_sng[0] == '4' && lbr_sng[1] == '.' && lbr_sng[2] == '7' && lbr_sng[3] == '.' && lbr_sng[4] == '3'){rcd=473;}
  else if(lbr_sng[0] == '4' && lbr_sng[1] == '.' && lbr_sng[2] == '7' && lbr_sng[3] == '.' && lbr_sng[4] == '4'){rcd=474;}
  else if(lbr_sng[0] == '4' && lbr_sng[1] == '.' && lbr_sng[2] == '8' && lbr_sng[3] == '.' && lbr_sng[4] == '0'){rcd=480;}
  else if(lbr_sng[0] == '4' && lbr_sng[1] == '.' && lbr_sng[2] == '8' && lbr_sng[3] == '.' && lbr_sng[4] == '1'){rcd=481;}
  else if(lbr_sng[0] == '4' && lbr_sng[1] == '.' && lbr_sng[2] == '8' && lbr_sng[3] == '.' && lbr_sng[4] == '2'){rcd=482;}
#endif /* HAVE_NETCDF4_H */
  /* exit() with custom rcd for use by Perl regression tester nco_bm.pl/NCO_rgr.pm */
  rcd-=300;
  exit(rcd);
} /* end nco_exit_lbr_rcd() */

nco_bool /* [flg] Program is multi-file operator */
nco_is_mfo /* [fnc] Query whether program is multi-file operator */
(const int nco_prg_id) /* [enm] Program ID */
{
  /* Purpose: Is this a multi-file operator (MFO)?
     MFOs take arbitrarily large numbers of input files
     ncbo and ncflint are not MFOs because they take a fixed number (two) of input files
     MFOs attempt to read input filenames from stdin when input filenames are not positional arguments */
  switch(nco_prg_id){
  case ncfe:
  case ncecat: 
  case ncra:
  case ncrcat: 
  case ncge: 
    return True;
    break;
  case ncap: 
  case ncatted: 
  case ncbo: 
  case ncflint: 
  case ncks: 
  case ncpdq: 
  case ncrename: 
  case ncwa: 
    return False;
    break;
  default: nco_dfl_case_prg_id_err(); break;
  } /* end switch */
  return False;
} /* end nco_is_mfo() */

nco_bool /* [flg] Program does arithmetic */
nco_is_rth_opr /* [fnc] Query whether program does arithmetic */
(const int nco_prg_id) /* [enm] Program ID */
{
  /* Purpose: Is operator arithmetic?
     For purposes of this function, an arithmetic operator is one which changes values
     Concatenators (ncrcat, ncecat) are not arithmetic because they just glue data
     Permutor (ncpdq) is not arithmetic because it just re-arranges values
     Packer (ncpdq) is not arithmetic because it re-represents values
     Attributors (ncrename, ncatted) are not arithmetic because they change metadata, not data
     nco_is_rth_opr() flag help control packing behavior
     Clearly, arithmetic operators must operate on unpacked values
     Hence, one use of nco_is_rth_opr() is to tell arithmetic operators to 
     automatically unpack variables when reading them
     Non-arithmetic operators do not need to unpack variables */
  switch(nco_prg_id){
  case ncap: 
  case ncbo:
  case ncfe:
  case ncflint:
  case ncra:
  case ncwa:
  case ncge:
    return True;
    break;
  case ncatted: 
  case ncecat: 
  case ncks: 
  case ncpdq: 
  case ncrcat: 
  case ncrename: 
    return False;
    break;
  default: nco_dfl_case_prg_id_err(); break;
  } /* end switch */
  return False;
} /* end nco_is_rth_opr() */

nco_bool /* [flg] Program does arithmetic and preserves rank */
nco_is_sz_rnk_prv_rth_opr /* [fnc] Is program size and rank-preserving arithmetic operator? */
(const int nco_prg_id, /* I [enm] Program ID */
 const int nco_pck_plc) /* I [enm] Packing policy */
{
  /* Purpose: Is program size and rank-preserving arithmetic operator?
     For purposes of this function, arithmetic operators change values
     Concatenators (ncrcat, ncecat) are not arithmetic because they just glue data
     Permutor (ncpdq) _is not_ arithmetic because it only re-arranges values
     Packer (ncpdq) _is_ arithmetic because it uses floating point arithmetic to re-represent values
     nco_pck_plc flag is required as input and used only to distinguish between ncpdq packing and permuting
     Attributors (ncrename, ncatted) are not arithmetic because they change metadata, not data
     Averager ncwa is clearly not size or rank-preserving
     Averager ncra preserves numeric rank though not record-dimension size and so is so is not size and rank-preserving
     One use of nco_is_sz_rnk_prv_rth_opr() is to tell which operators should not process multidimensional coordinate values
     For example, we want ncwa to act on coordinates that are reduced 
     However, we do not want ncfe, ncbo, or ncflint, for example, to load and process single or multi-dimensional coordinate variables
     Nor do we want ncpdq to pack variables like gaussian weights, or area since that causes a significant loss of arithmetic precision when those are used as weights in re-inflated files
     Such variables to these operators are best treated as "fixed" variables to be copied directly from the input to the output file */ 
  switch(nco_prg_id){
  case ncap: 
  case ncbo:
  case ncfe:
  case ncge:
  case ncflint:
    return True;
    break;
  case ncatted: 
  case ncecat: 
  case ncks: 
  case ncrcat: 
  case ncra: /* Process (not fix) time-varying fields like date, datesec */
  case ncrename: 
  case ncwa:
    return False;
    break;
  case ncpdq: 
    if(nco_pck_plc != nco_pck_plc_nil) return True; else return False; 
    break;
  default: nco_dfl_case_prg_id_err(); break;
  } /* end switch */
  return False;
} /* end nco_is_sz_rnk_prv_rth_opr() */

void
nco_cnf_prn(void) /* [fnc] Print NCO configuration and help text */
{
  /* Purpose: Print NCO configuration and help text */
  const char bld_ngn[]=TKN2SNG(NCO_BUILDENGINE); // [sng] Build-engine

  (void)fprintf(stdout,"Homepage: http://nco.sf.net\n");
  (void)fprintf(stdout,"Code: http://github.com/nco/nco\n");
  (void)fprintf(stdout,"Build-engine: %s\n",bld_ngn);
  (void)fprintf(stdout,"User Guide: http://nco.sf.net/nco.html\n");
  /* fxm: TKN2YESNO breaks when TKN is undefined
     Full macro language like M4 might be useful here, though probably too much trouble */
#define TKN2YESNO(x) ((x+0) ? ("No"):("Yes"))
  /* NB: Keep configuration option tokens consistent among configure.ac, bld/Makefile, and nco_ctl.c
     Alphabetize list by first word in English text description of token */
  (void)fprintf(stdout,"Configuration Option:\tActive?\tMeaning or Reference:\nCheck _FillValue\t%s\thttp://nco.sf.net/nco.html#mss_val\nCommunity Codec Repo\t%s\thttp://github.com/ccr/ccr\nDAP support\t\t%s\thttp://nco.sf.net/nco.html#dap\nDebugging: Custom\t%s\tPedantic, bounds checking (slowest execution)\nDebugging: Symbols\t%s\tProduce symbols for debuggers (e.g., dbx, gdb)\nGNU Scientific Library\t%s\thttp://nco.sf.net/nco.html#gsl\nHDF4 support\t\t%s\thttp://nco.sf.net/nco.html#hdf4\nInternationalization\t%s\thttp://nco.sf.net/nco.html#i18n (pre-alpha)\nLogging\t\t\t%s\thttp://nco.sf.net/nco.html#dbg\nnetCDF3 64-bit offset\t%s\thttp://nco.sf.net/nco.html#lfs\nnetCDF3 64-bit data\t%s\thttp://nco.sf.net/nco.html#cdf5\nnetCDF4/HDF5 support\t%s\thttp://nco.sf.net/nco.html#nco4\nOpenMP SMP threading\t%s\thttp://nco.sf.net/nco.html#omp\nRegular Expressions\t%s\thttp://nco.sf.net/nco.html#rx\nUDUnits2 conversions\t%s\thttp://nco.sf.net/nco.html#udunits\n%s",
		(!strcmp("_FillValue",nco_mss_val_sng_get())) ? "Yes" : "No",
#if defined(ENABLE_CCR) && (ENABLE_CCR)
		"Yes",
#else /* !ENABLE_CCR */
		"No",
#endif /* !ENABLE_CCR */
#if defined(ENABLE_DAP) && (ENABLE_DAP)
		"Yes",
#else /* !ENABLE_DAP */
		"No",
#endif /* !ENABLE_DAP */
#if defined(ENABLE_DEBUG_CUSTOM) && (ENABLE_DEBUG_CUSTOM)
		"Yes",
#else /* !ENABLE_DEBUG_CUSTOM */
		"No",
#endif /* !ENABLE_DEBUG_CUSTOM */
#if defined(ENABLE_DEBUG_SYMBOLS) && (ENABLE_DEBUG_SYMBOLS)
		"Yes",
#else /* !ENABLE_DEBUG_SYMBOLS */
		"No",
#endif /* !ENABLE_DEBUG_SYMBOLS */
#if defined(ENABLE_GSL) && (ENABLE_GSL)
		"Yes",
#else /* !ENABLE_GSL */
		"No",
#endif /* !ENABLE_GSL */
/* 20131018: This switch is not active yet. Requires netCDF library support */
#if NC_LIB_VERSION >= 433 
# if NC_HAS_HDF4
		"Yes",
# else /* !NC_HAS_HDF4 */
		"No",
# endif /* !NC_HAS_HDF4 */
#else /* !NC_LIB_VERSION */
		"Unknown",
#endif /* !NC_LIB_VERSION */
#if defined(I18N) && (I18N)
		"Yes",
#else /* !I18N */
		"No",
#endif /* !I18N */
#if defined(ENABLE_LOGGING) && (ENABLE_LOGGING)
		"Yes",
#else /* !ENABLE_LOGGING */
		"No",
#endif /* !ENABLE_LOGGING */
#if defined(NC_64BIT_OFFSET) && (NC_64BIT_OFFSET)
		"Yes",
#else /* !NC_64BIT_OFFSET */
		"No",
#endif /* !NC_64BIT_OFFSET */
#if defined(NC_LIB_VERSION) && (NC_LIB_VERSION >= 440)
# if defined(NC_HAS_CDF5) && (NC_HAS_CDF5 != 0)
		/* 20180916: Simple version test is insufficient since --enable-cdf5 required for netCDF 4.5.x-4.6.1 */
		"Yes",
# else /* !NC_HAS_CDF5 */
		"No",
# endif /* !NC_HAS_CDF5 */
#else /* !NC_64BIT_DATA */
		"No",
#endif /* !NC_64BIT_DATA */
#if defined(ENABLE_NETCDF4) && (ENABLE_NETCDF4)
# if defined(HAVE_NETCDF4_H) && (HAVE_NETCDF4_H)
		"Yes",
# else /* !HAVE_NETCDF4_H */
		"No",
# endif /* !HAVE_NETCDF4_H */
#else /* !ENABLE_NETCDF4 */
		"No",
#endif /* !ENABLE_NETCDF4 */
#if defined(_OPENMP) && (_OPENMP)
		"Yes",
#else /* !_OPENMP */
		"No",
#endif /* !_OPENMP */
#if defined(NCO_HAVE_REGEX_FUNCTIONALITY) && (NCO_HAVE_REGEX_FUNCTIONALITY)
		"Yes",
#else /* !NCO_HAVE_REGEX_FUNCTIONALITY */
		"No",
#endif /* !NCO_HAVE_REGEX_FUNCTIONALITY */
#if defined(ENABLE_UDUNITS) && (ENABLE_UDUNITS)
# if defined(HAVE_UDUNITS2_H) && (HAVE_UDUNITS2_H)
		"Yes",
# else /* !HAVE_UDUNITS2_H */
		"No",
# endif /* !HAVE_UDUNITS2_H */
#else /* !ENABLE_UDUNITS */
		"No",
#endif /* !ENABLE_UDUNITS */
		"\n"); /* End of print statement marker */
} /* end nco_cnf_prn() */

char * /* O [sng] nm_in stripped of any path (i.e., program name stub) */ 
nco_prg_prs /* [fnc] Strip program name to stub and set program ID */
(const char * const nm_in, /* I [sng] Name of program, i.e., argv[0] (may include path prefix) */
 int * const prg_lcl) /* O [enm] Enumerated number corresponding to nm_in */
{
  /* Purpose: Set program name and enum */
  char *nm_out;
  char *nm_out_tmp;
  char *nm_out_orig;

  /* Get program name (use strrchr() first in case nm_in contains a path) */
  nm_out_orig=nm_out_tmp=(char *)strdup(nm_in);
#ifdef _MSC_VER
  int len;
  if(strrchr(nm_out_tmp,'\\')) nm_out_tmp=strrchr(nm_out_tmp,'\\')+1;
  char *sfx=strstr(nm_out_tmp,".exe");
  if(sfx && !strcmp(sfx,".exe")){
    len=strlen(nm_out_tmp); /* cut any '.exe' from name */ 
    nm_out_tmp[len-4]='\0';   
  } /* endif */
#else /* !_MSC_VER */
  if(strrchr(nm_out_tmp,'/')) nm_out_tmp=strrchr(nm_out_tmp,'/')+1;
#endif /* !_MSC_VER */

  /* Skip possible libtool prefix */
  if(!strncmp(nm_out_tmp,"lt-",3)){nm_out_tmp+=3;}

  /* Classify calling program */
  /* ncap and acceptable synonyms (symbolic links): */
  if(!strcmp(nm_out_tmp,"ncap")){*prg_lcl=ncap;}
  else if(!strcmp(nm_out_tmp,"ncap2")){*prg_lcl=ncap;}
  /* ncatted and acceptable synonyms (symbolic links): */
  else if(!strcmp(nm_out_tmp,"ncatted")){*prg_lcl=ncatted;}
  /* ncbo and acceptable synonyms (symbolic links): */
  else if(!strcmp(nm_out_tmp,"mpncbo")){*prg_lcl=ncbo;}
  else if(!strcmp(nm_out_tmp,"mpncdiff")){*prg_lcl=ncbo;}
  else if(!strcmp(nm_out_tmp,"ncadd")){*prg_lcl=ncbo;}
  else if(!strcmp(nm_out_tmp,"ncbo")){*prg_lcl=ncbo;}
  else if(!strcmp(nm_out_tmp,"ncdiff")){*prg_lcl=ncbo;}
  else if(!strcmp(nm_out_tmp,"ncdivide")){*prg_lcl=ncbo;}
  else if(!strcmp(nm_out_tmp,"ncmult")){*prg_lcl=ncbo;}
  else if(!strcmp(nm_out_tmp,"ncmultiply")){*prg_lcl=ncbo;}
  else if(!strcmp(nm_out_tmp,"ncsub")){*prg_lcl=ncbo;}
  else if(!strcmp(nm_out_tmp,"ncsubtract")){*prg_lcl=ncbo;}
  /* ncecat and acceptable synonyms (symbolic links): */
  else if(!strcmp(nm_out_tmp,"ncecat")){*prg_lcl=ncecat;}
  else if(!strcmp(nm_out_tmp,"mpncecat")){*prg_lcl=ncecat;}
  /* ncfe and acceptable synonyms (symbolic links): */
  else if(!strcmp(nm_out_tmp,"ncea")){*prg_lcl=ncfe;}
  else if(!strcmp(nm_out_tmp,"mpncea")){*prg_lcl=ncfe;}
  else if(!strcmp(nm_out_tmp,"nces")){*prg_lcl=ncfe;}
  else if(!strcmp(nm_out_tmp,"mpnces")){*prg_lcl=ncfe;}
  else if(!strcmp(nm_out_tmp,"ncfe")){*prg_lcl=ncfe;}
  else if(!strcmp(nm_out_tmp,"mpncfe")){*prg_lcl=ncfe;}
  /* ncge and acceptable synonyms (symbolic links): */
  else if(!strcmp(nm_out_tmp,"ncge")){*prg_lcl=ncge;}
  else if(!strcmp(nm_out_tmp,"mpncge")){*prg_lcl=ncge;}
  /* ncflint and acceptable synonyms (symbolic links): */
  else if(!strcmp(nm_out_tmp,"ncflint")){*prg_lcl=ncflint;}
  else if(!strcmp(nm_out_tmp,"mpncflint")){*prg_lcl=ncflint;}
  /* ncks and acceptable synonyms (symbolic links): */
  else if(!strcmp(nm_out_tmp,"ncks")){*prg_lcl=ncks;}
  /* ncpdq and acceptable synonyms (symbolic links): */
  else if(!strcmp(nm_out_tmp,"ncpdq")){*prg_lcl=ncpdq;}
  else if(!strcmp(nm_out_tmp,"mpncpdq")){*prg_lcl=ncpdq;}
  else if(!strcmp(nm_out_tmp,"ncpack")){*prg_lcl=ncpdq;}
  else if(!strcmp(nm_out_tmp,"ncunpack")){*prg_lcl=ncpdq;}
  /* ncra and acceptable synonyms (symbolic links): */
  else if(!strcmp(nm_out_tmp,"ncrs")){*prg_lcl=ncra;}
  else if(!strcmp(nm_out_tmp,"mpncrs")){*prg_lcl=ncra;}
  else if(!strcmp(nm_out_tmp,"ncra")){*prg_lcl=ncra;}
  else if(!strcmp(nm_out_tmp,"mpncra")){*prg_lcl=ncra;}
  /* ncrcat and acceptable synonyms (symbolic links): */
  else if(!strcmp(nm_out_tmp,"ncrcat")){*prg_lcl=ncrcat;}
  else if(!strcmp(nm_out_tmp,"mpncrcat")){*prg_lcl=ncrcat;}
  /* ncrename and acceptable synonyms (symbolic links): */
  else if(!strcmp(nm_out_tmp,"ncrename")){*prg_lcl=ncrename;}
  /* ncwa and acceptable synonyms (symbolic links): */
  else if(!strcmp(nm_out_tmp,"ncws")){*prg_lcl=ncwa;}
  else if(!strcmp(nm_out_tmp,"mpncws")){*prg_lcl=ncwa;}
  else if(!strcmp(nm_out_tmp,"ncwa")){*prg_lcl=ncwa;}
  else if(!strcmp(nm_out_tmp,"mpncwa")){*prg_lcl=ncwa;}
  else if(!strcmp(nm_out_tmp,"vrl-tst")){*prg_lcl=ncks;}
  else{
    (void)fprintf(stdout,"%s: ERROR executable name %s not registered in nco_prg_prs()\n",nm_out_tmp,nm_out_tmp);
    nco_exit(EXIT_FAILURE);
  } /* end else */

  /* Duplicate stub for returning */
  nm_out=(char *)strdup(nm_out_tmp);
  /* Free copy of argv[0] */
  nm_out_orig=(char *)nco_free(nm_out_orig);
  return nm_out;

} /* end nco_prg_prs() */

void 
nco_usg_prn(void)
{
  /* Purpose: Print correct command-line usage (currently to stdout) */

  char *opt_sng=NULL_CEWI;

  int prg_lcl;

  prg_lcl=nco_prg_id_get();

  switch(prg_lcl){
  case ncap:
    opt_sng=(char *)strdup("[-3] [-4] [-5] [-6] [-7] [-A] [--bfr byt] [-C] [-c] [--cnk_byt byt] [--cnk_csh byt] [--cnk_dmn nm,lmn] [--cnk_map map] [--cnk_min byt] [--cnk_plc plc] [--cnk_scl lmn] [-D dbg_lvl] [-F] [-f] [--fl_fmt fmt] [--glb ...] [-h] [--hdf] [--hdr_pad nbr] [--hpss] [-L lvl] [-l path] [--no_tmp_fl] [-O] [-o out.nc] [-p path] [-R] [-r] [--ram_all] [-s algebra] [-S fl.nco] [-t thr_nbr] [--uio] [-v] [in.nc] [out.nc]\n");
    break;
  case ncatted:
    opt_sng=(char *)strdup("[-a ...] [--bfr byt] [-D dbg_lvl] [--glb ...] [-h] [--hdr_pad nbr] [--hpss] [-l path] [-O] [-o out.nc] [-p path] [-R] [-r] [-t] [--uio] in.nc [[out.nc]]\n");
    break;
  case ncbo:
    opt_sng=(char *)strdup("[-3] [-4] [-5] [-6] [-7] [-A] [--bfr byt] [-C] [-c] [--cnk_byt byt] [--cnk_csh byt] [--cnk_dmn nm,lmn] [--cnk_map map] [--cnk_min byt] [--cnk_plc plc] [--cnk_scl sz] [-D dbg_lvl] [-d ...] [-F] [--fl_fmt fmt] [-G grp:lvl] [-g ...] [--glb ...] [-h] [--hdf] [--hdr_pad nbr] [--hpss] [-L lvl] [-l path] [--msa] [-n ...] [--no_cll_msr] [--no_frm_trm] [--no_tmp_fl] [-O] [-o out.nc] [-p path] [-R] [-r] [--ram_all] [-t thr_nbr] [--uio] [--unn] [-v ...] [-X box] [-x] [-y op_typ] in_1.nc in_2.nc [out.nc]\n");
    break;
  case ncflint:
    opt_sng=(char *)strdup("[-3] [-4] [-5] [-6] [-7] [-A] [--bfr byt] [-C] [-c] [--cnk_byt byt] [--cnk_csh byt] [--cnk_dmn nm,lmn] [--cnk_map map] [--cnk_min byt] [--cnk_plc plc] [--cnk_scl sz] [-D dbg_lvl] [-d ...] [-F] [--fix_rec_crd] [--fl_fmt fmt] [--glb ...] [-h] [--hdf] [--hdr_pad nbr] [--hpss] [-i var,val] [-L lvl] [-l path] [--msa] [--no_cll_msr] [--no_frm_trm] [--no_tmp_fl] [-O] [-o out.nc] [-p path] [--ppc ...] [-R] [-r] [--ram_all] [-t thr_nbr] [--uio] [-v ...] [-X box] [-x] [-w wgt_1[,wgt_2]] in_1.nc in_2.nc [out.nc]\n");
    break;
  case ncks:
    opt_sng=(char *)strdup("[-3] [-4] [-5] [-6] [-7] [-A] [-a] [--area_wgt] [-b fl_bnr] [--bfr byt] [-C] [-c] [--cal] [--cdl] [--chk_map] [--chk_nan] [--cnk_byt byt] [--cnk_csh byt] [--cnk_dmn nm,lmn] [--cnk_map map] [--cnk_min byt] [--cnk_plc plc] [--cnk_scl sz] [-D dbg_lvl] [-d ...] [--dt_fmt fmt] [-F] [--fix_rec_dmn dim] [--fl_fmt fmt] [--fmt_val fmt] [-G grp:lvl] [-g ...] [--glb ...] [--grp_xtr_var_xcl] [-H] [-h] [--hdn] [--hdr_pad nbr] [--hpss] [--jsn] [--jsn_fmt lvl] [-L lvl] [-l path] [-M] [-m] [--map map.nc] [--md5_dgs] [--md5_wrt] [--mk_rec_dmn dim] [--msa] [--no_blank] [--no_cll_msr] [--no_frm_trm] [--no_tmp_fl] [-O] [-o out.nc] [-P] [-p path] [--ppc ...] [-Q] [-q] [-R] [-r] [--rad] [--rgr] [--ram_all] [--rnr=wgt] [-s format] [--sparse] [-t thr_nbr] [-u] [--uio] [--unn] [-V] [-v ...] [--vrt vrt.nc] [-X box] [-x] [--xml] [--xml_no_loc] [--xml_spr_chr sng] [--xml_spr_nmr sng] [--xtn_var ...] in.nc [[out.nc]]\n");
    break;
  case ncpdq:
    opt_sng=(char *)strdup("[-3] [-4] [-5] [-6] [-7] [-A] [-a ...] [--bfr byt] [-C] [-c] [--cnk_byt byt] [--cnk_csh byt] [--cnk_dmn nm,lmn] [--cnk_map map] [--cnk_min byt] [--cnk_plc plc] [--cnk_scl sz] [-D dbg_lvl] [-d ...] [-F] [--fl_fmt fmt] [-G grp:lvl] [-g ...] [--glb ...] [-h] [--hdf] [--hdr_pad nbr] [--hpss] [-L lvl] [-l path] [-M pck_map] [--mrd] [--msa] [--no_cll_msr] [--no_frm_trm] [--no_tmp_fl] [-O] [-o out.nc] [-P pck_plc] [-p path] [--ppc ...] [-R] [-r] [--ram_all] [-t thr_nbr] [--uio] [--unn] [-U] [-v ...] [-X box] [-x] in.nc [out.nc]\n");
    break;
  case ncra:
    opt_sng=(char *)strdup("[-3] [-4] [-5] [-6] [-7] [-A] [--bfr byt] [-C] [-c] [--cb ...] [--cnk_byt byt] [--cnk_csh byt] [--cnk_dmn nm,lmn] [--cnk_map map] [--cnk_min byt] [--cnk_plc plc] [--cnk_scl sz] [-D dbg_lvl] [-d ...]  [--dbl|flt] [-F] [--fl_fmt fmt] [-G grp:lvl] [-g ...] [--glb ...] [-H] [-h] [--hdf] [--hdr_pad nbr] [--hpss] [-L lvl] [-l path] [--mro] [--msa] [-N] [-n ...] [--no_cll_msr] [--no_cll_mth] [--no_frm_trm] [--no_tmp_fl] [-O] [-o out.nc] [-p path] [--prm_ints] [--prw] [--ppc ...] [-R] [-r] [--ram_all] [--rec_apn] [-t thr_nbr] [--uio] [--unn] [-w wgt] [-v ...] [-X box] [-x] [-y op_typ] in.nc [...] [out.nc]\n");
    break;
  case ncfe:
    opt_sng=(char *)strdup("[-3] [-4] [-5] [-6] [-7] [-A] [--bfr byt] [-C] [-c] [--cb ...] [--cnk_byt byt] [--cnk_csh byt] [--cnk_dmn nm,lmn] [--cnk_map map] [--cnk_min byt] [--cnk_plc plc] [--cnk_scl sz] [-D dbg_lvl] [-d ...]  [--dbl|flt] [-F] [--fl_fmt fmt] [-G grp:lvl] [-g ...] [--glb ...] [-H] [-h] [--hdf] [--hdr_pad nbr] [--hpss] [-L lvl] [-l path] [--msa] [-n ...] [--no_cll_msr] [--no_frm_trm] [--no_tmp_fl] [--nsm_fl] [--nsm_grp] [--nsm_sfx] [-O] [-o out.nc] [-p path] [--ppc ...] [-R] [-r] [--ram_all] [-t thr_nbr] [--uio] [--unn] [-v ...] [-w wgt] [-X box] [-x] [-y op_typ] in.nc [...] [out.nc]\n");
    break;
  case ncge:
    opt_sng=(char *)strdup("[-3] [-4] [-5] [-6] [-7] [-A] [--bfr byt] [-C] [-c] [--cb ...] [--cnk_byt byt] [--cnk_csh byt] [--cnk_dmn nm,lmn] [--cnk_map map] [--cnk_min byt] [--cnk_plc plc] [--cnk_scl sz] [-D dbg_lvl] [-d ...]  [--dbl|flt] [-F] [--fl_fmt fmt] [-G grp:lvl] [-g ...] [--glb ...] [-H] [-h] [--hdf] [--hdr_pad nbr] [--hpss] [-L lvl] [-l path] [--msa] [-n ...] [--no_cll_msr] [--no_frm_trm] [--no_tmp_fl] [--nsm_fl] [--nsm_grp] [--nsm_sfx] [-O] [-o out.nc] [-p path] [--ppc ...] [-R] [-r] [--ram_all] [-t thr_nbr] [--uio] [--unn] [-v ...] [-w wgt] [-X box] [-x] [-y op_typ] in.nc [...] [out.nc]\n");
    break;
  case ncrcat:
    opt_sng=(char *)strdup("[-3] [-4] [-5] [-6] [-7] [-A] [--bfr byt] [-C] [-c] [--cb ...] [--cnk_byt byt] [--cnk_csh byt] [--cnk_dmn nm,lmn] [--cnk_map map] [--cnk_min byt] [--cnk_plc plc] [--cnk_scl sz] [-D dbg_lvl] [-d ...] [-F] [--fl_fmt fmt] [-G grp:lvl] [-g ...] [--glb ...] [-H] [-h] [--hdr_pad nbr] [--hpss] [-L lvl] [-l path] [--md5_dgs] [--msa] [-n ...] [--no_cll_msr] [--no_frm_trm] [--no_tmp_fl] [-O] [-o out.nc] [-p path] [--ppc ...] [-R] [-r] [--ram_all] [--rec_apn] [-t thr_nbr] [--uio] [--unn] [-v ...] [-X box] [-x] in.nc [...] [out.nc]\n");
    break;
  case ncecat:
    opt_sng=(char *)strdup("[-3] [-4] [-5] [-6] [-7] [-A] [--bfr byt] [-C] [-c] [--cnk_byt byt] [--cnk_dmn nm,lmn] [--cnk_map map] [--cnk_min byt] [--cnk_plc plc] [--cnk_scl sz] [-D dbg_lvl] [-d ...] [-F] [--fl_fmt fmt] [-G grp:lvl] [-g ...] [--gag] [--glb ...] [-H] [-h] [--hdr_pad nbr] [--hpss] [-L lvl] [-l path] [-M] [--md5_dgs] [--mrd] [--msa] [-n ...] [--no_cll_msr] [--no_frm_trm] [--no_tmp_fl] [-O] [-o out.nc] [-p path] [--ppc ...] [-R] [-r] [--ram_all] [-t thr_nbr] [--uio] [-u ulm_nm] [--unn] [-v ...] [-X box] [-x] in.nc [...] [out.nc]\n");
    break;
  case ncrename:
    opt_sng=(char *)strdup("[-a ...] [--bfr byt] [-D dbg_lvl] [-d ...] [-g ...] [--glb ...] [-h] [--hdr_pad nbr] [--hpss] [-l path] [-O] [-o out.nc] [-p path] [-R] [-r] [--uio] [-v ...] in.nc [[out.nc]]\n");
    break;
  case ncwa:
    opt_sng=(char *)strdup("[-3] [-4] [-5] [-6] [-7] [-A] [-a ...] [-B mask_cond] [-b] [--bfr byt] [-C] [-c] [--cnk_byt byt] [--cnk_csh byt] [--cnk_dmn nm,lmn] [--cnk_map map] [--cnk_min byt] [--cnk_plc plc] [--cnk_scl sz] [-D dbg_lvl] [-d ...] [--dbl|flt] [-F] [--fl_fmt fmt] [-G grp:lvl] [-g ...] [--glb ...] [-h] [--hdf] [--hdr_pad nbr] [--hpss] [-I] [-L lvl] [-l path] [-m mask] [-M mask_val] [-N] [--no_cll_msr] [--no_cll_mth] [--no_frm_trm] [--no_tmp_fl] [-O] [-o out.nc] [-p path] [--ppc ...] [-R] [-r] [--ram_all] [-T mask_comp] [-t thr_nbr] [--uio] [--unn] [-v ...] [-w wgt] [-x] [-y op_typ] in.nc [out.nc]\n");
    break;
  default: nco_dfl_case_prg_id_err(); break;
  } /* end switch */
  
  /* We now have command-specific command line option string */
  (void)fprintf(stdout,"%s Command line options cheatsheet (full details at http://nco.sf.net/nco.html#%s):\n",nco_prg_nm_get(),nco_prg_nm_get());
  (void)fprintf(stdout,"%s %s\n",nco_prg_nm_get(),opt_sng);

  if(strstr(opt_sng,"[-3]")) (void)fprintf(stdout,"-3, --3, classic\tOutput file in netCDF3 CLASSIC (32-bit offset) storage format\n");
#ifdef ENABLE_NETCDF4
  if(strstr(opt_sng,"[-4]")) (void)fprintf(stdout,"-4, --4, netcdf4\tOutput file in netCDF4 (HDF5) storage format\n");
#endif /* !ENABLE_NETCDF4 */
  if(NC_LIB_VERSION >= 440){
    if(strstr(opt_sng,"[-5]")) (void)fprintf(stdout,"-5, --5, 64bit_data\tOutput file in netCDF3 64-bit data (i.e., CDF5, PnetCDF) storage format\n");
  } /* !NC_LIB_VERSION */
  if(strstr(opt_sng,"[-6]")) (void)fprintf(stdout,"-6, --6, 64, 64bit_offset\tOutput file in netCDF3 64-bit offset storage format\n");
#ifdef ENABLE_NETCDF4
  if(strstr(opt_sng,"[-7]")) (void)fprintf(stdout,"-7, --7, netcdf4_classic\tOutput file in netCDF4 CLASSIC format (3+4=7)\n");
#endif /* !ENABLE_NETCDF4 */
  if(strstr(opt_sng,"[-A]")) (void)fprintf(stdout,"-A, --apn, append\tAppend to existing output file, if any\n");
  if(strstr(opt_sng,"[-a")){
    if(prg_lcl == ncatted) (void)fprintf(stdout,"-a, --attribute att_nm,var_nm,mode,att_typ,att_val Attribute specification:\n\t\t\tmode = a,c,d,m,n,o (append, create, delete, modify, nappend, overwrite)\n\t\t\tatt_typ = f,d,l/i,s,c,b (float, double, long/int, short, char, byte)\n");
#ifdef ENABLE_NETCDF4
    if(prg_lcl == ncatted) (void)fprintf(stdout,"\t\t\tnetCDF4 types = ub,us,u,ll,ull,sng (unsigned byte, u. short, u. int, long long int, u. long long int, string)\n");
#endif /* !ENABLE_NETCDF4 */
    if(prg_lcl == ncks) (void)fprintf(stdout,"-a, --abc, alphabetize\tDisable alphabetization of extracted variables\n");
    if(prg_lcl == ncpdq) (void)fprintf(stdout,"-a, --arrange, permute, reorder, rdr [-]rdr_dim1[,[-]rdr_dim2[...]] Re-order dimensions\n");
    if(prg_lcl == ncrename) (void)fprintf(stdout,"-a, --attribute old_att,new_att Attribute's old and new names\n");
    if(prg_lcl == ncwa) (void)fprintf(stdout,"-a, --avg, average avg_dim1[,avg_dim2[...]] Averaging dimensions\n");
  } /* end if */
  if(strstr(opt_sng,"--area_wgt")) (void)fprintf(stdout,"    --area_wgt\t\tArea-weight map-file statistics\n");
  if(strstr(opt_sng,"[-B")){
#ifndef _MSC_VER
    if(prg_lcl == ncwa) (void)fprintf(stdout,"-B, --msk_cnd, mask_condition mask_cond\tMask condition (e.g., \"ORO < 1\")\n");
#endif /* _MSC_VER */
  } /* end if -B */
  if(strstr(opt_sng,"[-b ")) (void)fprintf(stdout,"-b, --fl_bnr, binary-file fl_bnr\tUnformatted binary file to write\n");
  if(strstr(opt_sng,"[-b]")) (void)fprintf(stdout,"-b, --rdd, retain-degenerate-dimensions\tRetain degenerate dimensions\n");
  if(strstr(opt_sng,"--bfr")) (void)fprintf(stdout,"    --bfr_sz, buffer_size sz\tBuffer size to open files with\n");
  if(strstr(opt_sng,"[-C]")) (void)fprintf(stdout,"-C, --no_crd, xcl_ass_var\tExclude coordinates, CF-associated variables (ancillary, bounds, ...)\n");
  if(strstr(opt_sng,"[-c]")) (void)fprintf(stdout,"-c, --crd, xtr_ass_var\tExtract coordinates, CF-associated variables (ancillary, bounds, ...)\n");
  if(strstr(opt_sng,"--cb")) (void)fprintf(stdout,"    --cb, clm_bnd\tCF Climatology and bounds information as yr_srt,yr_end,mth_srt,mth_end,tpd\n");
  if(strstr(opt_sng,"--cal")) (void)fprintf(stdout,"    --cal,--cln\tPrint UDUnits-compatible dates/times in human-legible calendar format\n");
  if(strstr(opt_sng,"--cdl")) (void)fprintf(stdout,"    --cdl\t\tPrint CDL (netCDF lingua franca used by ncdump/ncgen)\n");
  if(strstr(opt_sng,"--chk_map")) (void)fprintf(stdout,"    --chk_map\t\tCheck map-file quality\n");
  if(strstr(opt_sng,"--chk_nan")) (void)fprintf(stdout,"    --chk_nan\t\tDetect NaN and NaNf in floating-point variables\n");
  if(strstr(opt_sng,"--cnk_byt")) (void)fprintf(stdout,"    --cnk_byt, chunk_byte sz_byt\tChunksize in bytes\n");
  if(strstr(opt_sng,"--cnk_csh")) (void)fprintf(stdout,"    --cnk_csh, chunk_cache sz_byt\tChunk cache size in bytes\n");
  if(strstr(opt_sng,"--cnk_dmn")) (void)fprintf(stdout,"    --cnk_dmn, chunk_dimension nm,sz_lmn\tChunksize of dimension nm (in elements not bytes)\n");
  if(strstr(opt_sng,"--cnk_map")) (void)fprintf(stdout,"    --cnk_map, chunk_map map\tChunking map [dmn,lfp,nc4,nco,prd,rd1,rew,scl,xpl,xst]\n");
  if(strstr(opt_sng,"--cnk_min")) (void)fprintf(stdout,"    --cnk_min, chunk_min sz_byt\tMinimum size [B] of variable to chunk\n");
  if(strstr(opt_sng,"--cnk_plc")) (void)fprintf(stdout,"    --cnk_plc, chunk_policy plc\tChunking policy [all,g2d,g3d,xpl,xst,uck]\n");
  if(strstr(opt_sng,"--cnk_scl")) (void)fprintf(stdout,"    --cnk_scl, chunk_scalar sz_lmn\tChunksize scalar (in elements not bytes) (for all dimensions)\n");
  if(strstr(opt_sng,"[-D")) (void)fprintf(stdout,"-D, --dbg_lvl, debug-level lvl\tDebug-level is lvl\n");
  if(strstr(opt_sng,"[-d")){
    if(prg_lcl == ncrename) (void)fprintf(stdout,"-d, --dmn, dimension old_dim,new_dim Dimension's old and new names\n");
    else if(prg_lcl == ncra || prg_lcl == ncrcat) (void)fprintf(stdout,"-d, --dmn, dimension dim,[min][,[max][[[,stride[,subcycle[,interleave]]]]]] Dimension's limits, stride, subcycle, interleave in hyperslab\n");
    else (void)fprintf(stdout,"-d, --dmn, dimension dim,[min][,[max]][,[stride]] Dimension's limits and stride in hyperslab\n");
  } /* end if -d */
  if(strstr(opt_sng,"--dbl|flt")) (void)fprintf(stdout,"    --dbl, flt, rth_dbl|flt\tdbl: Promote single- to double-precision b4 arithmetic (default). flt: Single-precision arithmetic is allowed.\n");
  if(strstr(opt_sng,"--dt_fmt")) (void)fprintf(stdout,"    --dt_fmt, date_format\tPrint UDUnits-compatible dates/times in short, regular, or ISO8601 calendar format (fmt=1,2,3)\n");
  if(strstr(opt_sng,"[-F]")) (void)fprintf(stdout,"-F, --ftn, fortran\tFortran indexing conventions (1-based) for I/O\n");
  if(strstr(opt_sng,"[-f]")) (void)fprintf(stdout,"-f, --fnc_tbl, prn_fnc_tbl\tPrint function table\n");
  if(strstr(opt_sng,"--fix_rec_crd")) (void)fprintf(stdout,"    --fix_rec_crd\tDo not interpolate/multiply record coordinate variables\n");
  if(strstr(opt_sng,"--fix_rec_dmn dim")) (void)fprintf(stdout,"    --fix_rec_dmn dim\tChange dimension dim (or all) to fixed dimension in output file\n");
#ifdef ENABLE_NETCDF4
  if(strstr(opt_sng,"--fl_fmt")) (void)fprintf(stdout,"    --fl_fmt, file_format fmt\tFile format for output [classic,64bit_offset,64bit_data,netcdf4,netcdf4_classic]\n");
#else /* !ENABLE_NETCDF4 */
  if(strstr(opt_sng,"--fl_fmt")) (void)fprintf(stdout,"    --fl_fmt, file_format fmt\tFile format for output [classic,64bit_offset,64bit_data]\n");
#endif /* !ENABLE_NETCDF4 */
#ifdef ENABLE_NETCDF4
  if(strstr(opt_sng,"--gag")) (void)fprintf(stdout,"    --gag, aggregate_group\tGroup Aggregation (not Record Aggregation)\n");
  if(strstr(opt_sng,"[-G")) (void)fprintf(stdout,"-G, --gpe [grp_nm][:[lvl]]\tGroup Path Editing path, levels to replace\n");
  if(strstr(opt_sng,"[-g")){
    if(prg_lcl == ncrename) (void)fprintf(stdout,"-g, --grp, group\told_grp,new_grp Group's old and new names\n");
    if(prg_lcl != ncrename) (void)fprintf(stdout,"-g, --grp grp1[,grp2[...]] Group(s) to process (regular expressions supported)\n");
  } /* end if */
  if(strstr(opt_sng,"--glb")) (void)fprintf(stdout,"    --glb_att_add nm=val\tGlobal attribute to add\n");
  if(strstr(opt_sng,"--gxvx, --grp_xtr_var_xcl")) (void)fprintf(stdout,"    --gxvx, grp_xtr_var_xcl\tGroup Extraction Variable Exclusion\n");
#endif /* !ENABLE_NETCDF4 */
  if(strstr(opt_sng,"[-H]")){
    if(prg_lcl == ncks) (void)fprintf(stdout,"-H, --data, hieronymus\tToggle printing data\n");
    if(nco_is_mfo(prg_lcl)) (void)fprintf(stdout,"-H, --fl_lst_in, file_list\tDo not create \"input_file_list\" global attribute\n");
  } /* end if -H */
  if(strstr(opt_sng,"[-h]")) (void)fprintf(stdout,"-h, --hst, history\tDo not append to \"history\" global attribute\n");
  if(strstr(opt_sng,"--hdn")) (void)fprintf(stdout,"    --hdn, hidden\tPrint hidden (aka special) attributes\n");
  if(strstr(opt_sng,"--hdf")) (void)fprintf(stdout,"    --hdf_upk, hdf_unpack\tHDF unpack convention: unpacked=scale_factor*(packed-add_offset)\n");
  if(strstr(opt_sng,"--hdr_pad")) (void)fprintf(stdout,"    --hdr_pad, header_pad\tPad output header with nbr bytes\n");
  if(strstr(opt_sng,"--hpss")) (void)fprintf(stdout,"    --hpss, hpss_try\tSearch for unfound files on HPSS with 'hsi get ...'\n");
  if(strstr(opt_sng,"[-i var,val]")) (void)fprintf(stdout,"-i, --ntp, interpolate var,val\tInterpolant and value\n");
  if(strstr(opt_sng,"[-I]")) (void)fprintf(stdout,"-I, --wgt_msk_crd_var\tDo not weight or mask coordinate variables\n");
  if(strstr(opt_sng,"--jsn")) (void)fprintf(stdout,"    --jsn, json\tPrint JSON (JavaScript Object Notation)\n");
  if(strstr(opt_sng,"--jsn_fmt lvl")) (void)fprintf(stdout,"    --jsn_fmt lvl\tVerbosity of JSON format [0 = least verbose, 2 = most pedantic, add 4 to remove brackets]\n");
#ifdef ENABLE_NETCDF4
  if(strstr(opt_sng,"[-L")) (void)fprintf(stdout,"-L, --dfl_lvl, deflate lvl\tLempel-Ziv deflation/compression (lvl=0..9) for netCDF4 output\n");
#endif /* !ENABLE_NETCDF4 */
  if(strstr(opt_sng,"[-l")) (void)fprintf(stdout,"-l, --lcl, local path\tLocal storage path for remotely-retrieved files\n");
  if(strstr(opt_sng,"[-M")){
    if(prg_lcl == ncecat) (void)fprintf(stdout,"-M, --no_glb_mtd\tSuppress (do not copy) global metadata\n");
    if(prg_lcl == ncks) (void)fprintf(stdout,"-M, --Mtd, Metadata\tToggle printing global metadata\n");
    if(prg_lcl == ncpdq) (void)fprintf(stdout,"-M, --pck_map, pack_map, map pck_map\tPack map [flt_sht,flt_byt,hgh_sht,hgh_byt,nxt_lsr,dbl_flt,flt_dbl]\n");
    if(prg_lcl == ncwa) (void)fprintf(stdout,"-M, --msk_val, mask-value, mask_value mask_val\tMasking value (default is 1.0)\n");
  } /* end if */
  if(strstr(opt_sng,"[-m")){
    if(prg_lcl == ncwa) (void)fprintf(stdout,"-m, --msk_nm, msk_var, mask-variable, mask_variable mask_var\tMasking variable name\n");
    if(prg_lcl == ncks) (void)fprintf(stdout,"-m, --mtd, metadata\tToggle printing variable metadata\n");
  } /* end if */
  if(strstr(opt_sng,"--map")) (void)fprintf(stdout,"    --map, rgr_map map.nc\tFile containing (ESMF- or SCRIP-format) weights to regrid input to output grid\n");
  if(strstr(opt_sng,"--md5_dgs")) (void)fprintf(stdout,"    --md5_dgs, md5_digest\tPerform MD5 digests\n");
  if(strstr(opt_sng,"--md5_wrt")) (void)fprintf(stdout,"    --md5_wrt, md5_write\tWrite MD5 digests as attributes\n");
  if(strstr(opt_sng,"--mk_rec_dmn")) (void)fprintf(stdout,"    --mk_rec_dmn dim\tDefine dim as record dimension in output file\n");
  if(strstr(opt_sng,"--mro")) (void)fprintf(stdout,"    --mro\t\tMulti-Record Output\n");
  if(strstr(opt_sng,"--msa")) (void)fprintf(stdout,"    --msa, msa_usr_rdr\tMulti-Slab-Algorithm output in User-Order\n");
  if(strstr(opt_sng,"[-N]")){
    if(prg_lcl == ncflint) (void)fprintf(stdout,"-N, --nrm, normalize\tNormalize input weights so w1:=w1/(w1+w2), w2:=w2/(w1+w2)\n");
    if(prg_lcl == ncwa) (void)fprintf(stdout,"-N, --nmr, numerator\tNo normalization\n");
    if(prg_lcl == ncra || prg_lcl == ncfe || prg_lcl == ncge) (void)fprintf(stdout,"-N, --no_nrm_by_wgt\tNo normalization by weights\n");
  } /* !-N */
  if(strstr(opt_sng,"[-n ...]")){
    /*    if(prg_lcl == ncwa) (void)fprintf(stdout,"-n\t\tNormalize by tally but not weight\n");*/
    if(prg_lcl != ncwa) (void)fprintf(stdout,"-n, --nintap nbr_files,[nbr_numeric_chars[,increment]] NINTAP-style abbreviation of file list\n");
  } /* !-n */
  if(strstr(opt_sng,"--no_blank")) (void)fprintf(stdout,"    --no_blank\t\tPrint numeric missing values instead of blanks (underscores)\n");
  if(strstr(opt_sng,"--no_cll_msr")) (void)fprintf(stdout,"    --no_cll_msr\tDo not extract cell_measures variables\n");
  if(strstr(opt_sng,"--no_cll_mth")) (void)fprintf(stdout,"    --no_cll_mth\tDo not add/modify cell_methods attributes\n");
  if(strstr(opt_sng,"--no_frm_trm")) (void)fprintf(stdout,"    --no_frm_trm\tDo not extract formula_terms variables\n");
  if(strstr(opt_sng,"--no_tmp_fl")) (void)fprintf(stdout,"    --no_tmp_fl\t\tDo not write output to temporary file\n");
  if(strstr(opt_sng,"--nsm_fl")) (void)fprintf(stdout,"    --nsm_fl, ensemble_file\tEnsembles comprise equally weighted files\n");
  if(strstr(opt_sng,"--nsm_grp")) (void)fprintf(stdout,"    --nsm_grp, ensemble_group\tEnsembles comprise equally weighted groups\n");
  if(strstr(opt_sng,"--nsm_sfx")) (void)fprintf(stdout,"    --nsm_sfx, ensemble_suffix\tPlace ensemble output in group parent/parent+nsm_sfx\n");
  if(strstr(opt_sng,"[-O]")) (void)fprintf(stdout,"-O, --ovr, overwrite\tOverwrite existing output file, if any\n");
  if(strstr(opt_sng,"[-o")) (void)fprintf(stdout,"-o, --output, fl_out \tOutput file name (or use last positional argument)\n");
  if(strstr(opt_sng,"[-P")){
    if(prg_lcl == ncks) (void)fprintf(stdout,"-P, --prn, print\tPrint data, metadata, and units. Abbreviation for -C -H -M -m -u.\n");
    if(prg_lcl == ncpdq) (void)fprintf(stdout,"-P, --pck_plc, pack_policy pck_plc\tPacking policy [all_new,all_xst,xst_new,upk]\n");
  } /* end if -P */
  if(strstr(opt_sng,"[-p")) (void)fprintf(stdout,"-p, --pth, path path\tPath prefix for all input filenames\n");
  if(strstr(opt_sng,"--ppc")) (void)fprintf(stdout,"    --ppc v1[,v2[,...]]=prc\tPrecision-Preserving Compression: Number of total or decimal significant digits\n");
  if(strstr(opt_sng,"--prm_int")) (void)fprintf(stdout,"    --prm_int, prm_ntg\tPromote integers to floating-point in output\n");
  if(strstr(opt_sng,"--prw")) (void)fprintf(stdout,"    --prw, per_recor...\tPer-Record Weights\n");
  if(strstr(opt_sng,"[-Q]")) (void)fprintf(stdout,"-Q, --quiet\t\tQuiet printing of dimension indices and coordinate values\n");
  if(strstr(opt_sng,"[-q]")) (void)fprintf(stdout,"-q, --quench\t\tQuench (turn-off) all printing to screen\n");
  if(strstr(opt_sng,"[-R]")) (void)fprintf(stdout,"-R, --rtn, retain\tRetain remotely-retrieved files after use\n");
  if(strstr(opt_sng,"[-r]")) (void)fprintf(stdout,"-r, --revision, version\tCompile-time configuration and/or program version\n");
  if(strstr(opt_sng,"--rad")) (void)fprintf(stdout,"    --orphan, rph_dmn\tRetain or print all (including orphaned) dimensions\n");
  if(strstr(opt_sng,"--ram_all")) (void)fprintf(stdout,"    --ram_all, diskless_all\tOpen netCDF3 files and create output files in RAM\n");
  if(strstr(opt_sng,"--rec_apn")) (void)fprintf(stdout,"    --rec_apn, record_append\tAppend records directly to output file\n");
  if(strstr(opt_sng,"--rgr")) (void)fprintf(stdout,"    --rgr key=value\tSee http://nco.sf.net/nco.html#regrid for valid keys\n");
  if(strstr(opt_sng,"--rnr")) (void)fprintf(stdout,"    --rnr_thr=wgt \tCoverage threshold to renormalize valid destination value\n");
  // if(strstr(opt_sng,"--rgr_grd_dst")) (void)fprintf(stdout,"    --rgr_grd_dst file\tDestination grid\n");
  // if(strstr(opt_sng,"--rgr_grd_src")) (void)fprintf(stdout,"    --rgr_grd_src file\tSource grid\n");
  // if(strstr(opt_sng,"--rgr_in")) (void)fprintf(stdout,"    --rgr_in file\tFile containing fields to be regridded\n");
  if(strstr(opt_sng,"[-s")){
    if(prg_lcl != ncap) (void)fprintf(stdout,"-s, --sng_fmt, string format\tString format for text output\n");
    if(prg_lcl == ncap) (void)fprintf(stdout,"-s, --spt, script algebra\tAlgebraic command defining single output variable\n");
  } /* end if */
  if(strstr(opt_sng,"[-S")) (void)fprintf(stdout,"-S, --fl_spt, script-file fl.nco\tScript file containing multiple algebraic commands\n");
  if(strstr(opt_sng,"[-T")) (void)fprintf(stdout,"-T, --mask_comparator, msk_cmp_typ, op_rlt comparator\tComparator for mask condition: eq,ne,ge,le,gt,lt\n");
  if(strstr(opt_sng,"[-t")){
    if(prg_lcl == ncatted) (void)fprintf(stdout,"-t, --typ_mch, type_match \tType-match attribute edits\n");
    if(prg_lcl != ncatted) (void)fprintf(stdout,"-t, --thr_nbr, threads, omp_num_threads thr_nbr\tThread number for OpenMP\n");
  } /* end if */
  if(strstr(opt_sng,"[-U]")) (void)fprintf(stdout,"-U, --unpack\tUnpack input file\n");
  if(strstr(opt_sng,"[-u")){
    if(prg_lcl == ncks) (void)fprintf(stdout,"-u, --units\t\tToggle printing units of variables, if any\n");
    if(prg_lcl == ncecat) (void)fprintf(stdout,"-u, --ulm_nm, rcd_nm\tNew unlimited (record) dimension name\n");
  } /* end if */
  if(strstr(opt_sng,"--uio")) (void)fprintf(stdout,"    --uio, --share_all\tUnbuffered I/O to read/write netCDF3 file(s)\n");
  if(strstr(opt_sng,"--unn")) (void)fprintf(stdout,"    --unn, union\tSelect union of specified groups and variables\n");
  if(strstr(opt_sng,"[-V")) (void)fprintf(stdout,"-V, --var_val\t\tPrint variable values only\n");
  if(strstr(opt_sng,"[-v")){
    if(prg_lcl == ncrename) (void)fprintf(stdout,"-v, --variable old_var,new_var Variable's old and new names\n");
    if(prg_lcl == ncap) (void)fprintf(stdout,"-v, --variable \t\tOutput file includes ONLY user-defined variables\n");
    if(prg_lcl != ncrename && prg_lcl != ncap) (void)fprintf(stdout,"-v, --variable var1[,var2[...]] Variable(s) to process (regular expressions supported)\n");
  } /* end if */
  if(strstr(opt_sng,"--vrt")) (void)fprintf(stdout,"    --vrt_fl, vrt_crd vrt.nc\tFile containing vertical coordinate grid to interpolate to\n");
  /*  if(strstr(opt_sng,"[-W]")) (void)fprintf(stdout,"-W\t\tNormalize by weight but not tally\n");*/
  if(strstr(opt_sng,"[-w")){
    if(prg_lcl == ncra || prg_lcl == ncfe) (void)fprintf(stdout,"-w, --wgt_var, weight wgt\tPer-file weights or variable name\n");
    if(prg_lcl == ncwa) (void)fprintf(stdout,"-w, --wgt_var, weight wgt\tWeighting variable name\n");
    if(prg_lcl == ncflint) (void)fprintf(stdout,"-w, --wgt_var, weight wgt_1[,wgt_2] Weight(s) of file(s)\n");
  } /* end if */
  if(strstr(opt_sng,"[-X")) (void)fprintf(stdout,"-X, --auxiliary lon_min,lon_max,lat_min,lat_max\tAuxiliary coordinate bounding box\n");
  if(strstr(opt_sng,"[-x]")) (void)fprintf(stdout,"-x, --xcl, exclude\tExtract all variables EXCEPT those specified with -v\n");
  if(strstr(opt_sng,"--sparse")) (void)fprintf(stdout,"    --s1d, --sparse\tUnpack sparse-1D CLM/ELM variables\n");
  if(strstr(opt_sng,"--xml")) (void)fprintf(stdout,"    --xml\t\tPrint XML (NcML, netCDF Markup Language)\n");
  if(strstr(opt_sng,"--xml_no_loc")) (void)fprintf(stdout,"    --xml_no_location\tOmit NcML location element\n");
  if(strstr(opt_sng,"--xml_spr_chr")) (void)fprintf(stdout,"    --xml_spr_chr sng\tSeparator for NcML character types\n");
  if(strstr(opt_sng,"--xml_spr_nmr")) (void)fprintf(stdout,"    --xml_spr_nmr sng\tSeparator for NcML numeric types\n");
  if(strstr(opt_sng,"--xtn_var")) (void)fprintf(stdout,"    --xtn_var, extensive var\tExtensive variables for regridding (summed not averaged)\n");
  if(strstr(opt_sng,"[-y op_typ]")){
    if(prg_lcl == ncbo)(void)fprintf(stdout,"-y, --op_typ, operation op_typ\tBinary arithmetic operation: add,sbt,mlt,dvd (+,-,*,/)\n");
    if(prg_lcl == ncra || prg_lcl == ncfe || prg_lcl == ncge || prg_lcl == ncwa)(void)fprintf(stdout,"-y, --op_typ, operation op_typ\tArithmetic operation: avg,mabs,mebs,mibs,min,max,tabs,ttl,sqravg,avgsqr,sqrt,rms,rmssdn\n");
  }
  /* All operators have input files, no need to strstr(in.nc) */
  if(prg_lcl == ncbo || prg_lcl == ncflint){
    (void)fprintf(stdout,"in_1.nc in_2.nc\t\tInput file names\n");
  }else{
    if(nco_is_mfo(prg_lcl)) (void)fprintf(stdout,"in.nc [...]\t\tInput file names\n"); else (void)fprintf(stdout,"in.nc\t\t\tInput file name\n");
  } /* endif in.nc */
  if(strstr(opt_sng,"[out.nc]")) (void)fprintf(stdout,"\t\t\tOutput file name (or use -o switch)\n");
/*  if(strstr(opt_sng,"-")) (void)fprintf(stdout,"-\n");*/

  /* Free the space holding option string */
  opt_sng=(char *)nco_free(opt_sng);

  /* Public service announcements */
  (void)fprintf(stdout,"\nEight ways to find more help on %s and/or NCO:\n",nco_prg_nm_get());
  (void)fprintf(stdout,"1. Examples:     http://nco.sf.net/nco.html#xmp_%s\n",nco_prg_nm_get());
  (void)fprintf(stdout,"2. Ref. manual:  http://nco.sf.net/nco.html#%s\n",nco_prg_nm_get());
  (void)fprintf(stdout,"3. User Guide:   http://nco.sf.net#RTFM\n");
  (void)fprintf(stdout,"4. Manual pages: \'man %s\', \'man nco\', ...\n",nco_prg_nm_get());
  (void)fprintf(stdout,"5. Homepage:     http://nco.sf.net\n");
  (void)fprintf(stdout,"6. Code:         http://github.com/nco/nco\n");
  (void)fprintf(stdout,"7. Help Forum:   http://sf.net/p/nco/discussion/9830\n");
  (void)fprintf(stdout,"8. Publications: http://nco.sf.net#pub\n");
  (void)fprintf(stdout,"Post questions, suggestions, patches at http://sf.net/projects/nco\n");

} /* end nco_usg_prn() */
