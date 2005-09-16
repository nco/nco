/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_ctl.c,v 1.104 2005-09-16 05:54:47 zender Exp $ */

/* Purpose: Program flow control functions */

/* Copyright (C) 1995--2005 Charlie Zender
   This software may be modified and/or re-distributed under the terms of the GNU General Public License (GPL) Version 2
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

#include "nco_ctl.h" /* Program flow control functions */

const char * /* O [sng] Compiler and version */
nco_cmp_get(void) /* [fnc] Return compiler and version */
{ 
  /* Purpose: Return string containing compiler and version */
  const char fnc_nm[]="nco_cmp_get()";
#ifdef _AIX
  static const char cmp_nm[]="xlc"; /* [sng] Compiler name */
  static const char cmp_sng[]="Token _AIX_ defined in nco_cmp_get(), probably compiled with xlc"; /* [sng] Compiler string */
#endif /* !_AIX */
#if defined(__GNUC__) && !defined(__INTEL_COMPILER)
  static const char cmp_nm[]="gcc"; /* [sng] Compiler name */
  static const char cmp_sng[]="Token __GNUC__ defined in nco_cmp_get(), probably compiled with gcc"; /* [sng] Compiler string */
#endif /* !__GNUC__ */
#ifdef __INTEL_COMPILER
  /* Some compilers, including icc, also define __GNUC__ by default */
  static const char cmp_nm[]="icc";
  static const char cmp_sng[]="Token __INTEL_COMPILER defined in nco_cmp_get(), probably compiled with icc"; /* [sng] Compiler string */
#endif /* !__INTEL_COMPILER */
  /* In case none of the above tokens matched */
#if !defined(_AIX) && !defined(__GNUC__) && !defined(__INTEL_COMPILER)
  /* Unknown compiler */
  static const char cmp_nm[]="unknown"; /* [sng] Compiler name */
  static const char cmp_sng[]="Unknown compiler tokens in nco_cmp_get(), compiler is unknown"; /* [sng] Compiler string */
#endif /* !unknown */
  if(dbg_lvl_get() > 4) (void)fprintf(stderr,"%s: INFO %s reports compiler name is %s, compiler string is %s\n",prg_nm_get(),fnc_nm,cmp_nm,cmp_sng);

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

  /* In case no token matched */
#if !defined(_H_MPI) && !defined(LAM_MPI) && (MPICH_NAME != '1') && !defined(MPICH2)
#ifndef MPI_VERSION
  /* MPI is not installed */
  static const char mpi_nm[]="none"; /* [sng] MPI name */
  static const char mpi_sng[]="No MPI tokens found in nco_cmp_get(), MPI environment is not active"; /* [sng] MPI string */
#else /* MPI_VERSION */
  /* Unknown MPI implementation */
  static const char mpi_nm[]="unknown"; /* [sng] MPI name */
  static const char mpi_sng[]="Unknown MPI tokens found in nco_cmp_get(), MPI environment is present but of unknown pedigree"; /* [sng] MPI string */
#endif /* MPI_VERSION */
#endif /* !unknown */

  if(dbg_lvl_get() > 4) (void)fprintf(stderr,"%s: INFO %s reports MPI implementation name is %s, MPI implementation string is %s\n",prg_nm_get(),fnc_nm,mpi_nm,mpi_sng);
  return mpi_nm;
} /* end nco_mpi_get() */

void
nco_exit /* [fnc] Wrapper for exit() */
(int rcd) /* I [enm] Return code */
{
  /* Purpose: Wrapper for exit() */
  const char fnc_nm[]="nco_exit()";
#ifdef NCO_ABORT_ON_ERROR
  const char exit_nm[]="abort()";
#else /* !NCO_ABORT_ON_ERROR */
  const char exit_nm[]="exit(EXIT_FAILURE)";
#endif /* !NCO_ABORT_ON_ERROR */

  if(rcd == EXIT_SUCCESS){
    exit(rcd);
  }else{
    if(dbg_lvl_get() > 1) (void)fprintf(stdout,"%s: ERROR exiting through %s which will now call %s\n",prg_nm_get(),fnc_nm,exit_nm);
#ifdef NCO_ABORT_ON_ERROR
    abort();
#else /* !NCO_ABORT_ON_ERROR */
    exit(rcd);
#endif /* !NCO_ABORT_ON_ERROR */
  } /* endif rcd */
} /* nco_exit() */

void 
nco_exit_gracefully(void) /* [fnc] Clean up timers, file descriptors, memory, then exit */
{
  /* Purpose: Clean up timers, file descriptors, memory, then exit */
  char *time_bfr_end;
  time_t time_crr_time_t;
  
  /* End clock */ 
  time_crr_time_t=time((time_t *)NULL);
  time_bfr_end=ctime(&time_crr_time_t); time_bfr_end=time_bfr_end; /* Avoid compiler warning until variable is used for something */
  /*  (void)fprintf(stderr,"\tend = %s\n",time_bfr_end);*/

  (void)fclose(stderr);
  (void)fclose(stdin);
  (void)fclose(stdout);

  /* This should be penultimate command in program 
     After program name is free'd, calls to prg_nm_get() will fail */
  (void)nco_free(prg_nm_get());

  nco_exit(EXIT_SUCCESS);
} /* end nco_exit_gracefully() */

bool /* [flg] Program is multi-file operator */
nco_is_mlt_fl_opr /* [fnc] Query whether program is multi-file operator */
(const int prg_id) /* [enm] Program ID */
{
  /* Purpose: Is this a multi-file operator?
     Such operators attempt to read input filenames from stdin when input filenames are not positional arguments */
  switch(prg_id){
  case ncea:
  case ncecat: 
  case ncra:
  case ncrcat: 
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
} /* end nco_is_mlt_fl_opr() */

bool /* [flg] Program does arithmetic */
nco_is_rth_opr /* [fnc] Query whether program does arithmetic */
(const int prg_id) /* [enm] Program ID */
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
  switch(prg_id){
  case ncap: 
  case ncbo:
  case ncea:
  case ncflint:
  case ncra:
  case ncwa:
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

void
nco_lbr_vrs_prn(void) /* [fnc] Print netCDF library version */
{
  /* Purpose: Print netCDF library version */

  char *cmp_dat_sng;
  char *dlr_ptr;
  char *lbr_sng;
  char *lbr_vrs_sng;
  char *of_ptr;

  size_t cmp_dat_sng_lng;
  size_t lbr_vrs_sng_lng;

  /* As of netCDF 3.4, nc_inq_libvers() returns strings like "3.4 of May 16 1998 14:06:16 $" */  
  lbr_sng=(char *)strdup(nc_inq_libvers());
  of_ptr=strstr(lbr_sng," of ");
  if(of_ptr == NULL)(void)fprintf(stderr,"%s: WARNING nco_lbr_vrs_prn() reports of_ptr == NULL\n",prg_nm_get());
  lbr_vrs_sng_lng=(size_t)(of_ptr-lbr_sng);
  lbr_vrs_sng=(char *)nco_malloc(lbr_vrs_sng_lng+1);
  strncpy(lbr_vrs_sng,lbr_sng,lbr_vrs_sng_lng);
  lbr_vrs_sng[lbr_vrs_sng_lng]='\0'; /* NUL-terminate */

  dlr_ptr=strstr(lbr_sng," $");
  if(dlr_ptr == NULL)(void)fprintf(stderr,"%s: WARNING nco_lbr_vrs_prn() reports dlr_ptr == NULL\n",prg_nm_get());
  cmp_dat_sng_lng=(size_t)(dlr_ptr-of_ptr-4); /* 4 is the length of " of " */
  cmp_dat_sng=(char *)nco_malloc(cmp_dat_sng_lng+1ul);
  strncpy(cmp_dat_sng,of_ptr+4,cmp_dat_sng_lng); /* 4 is the length of " of " */
  cmp_dat_sng[cmp_dat_sng_lng]='\0'; /* NUL-terminate */

  (void)fprintf(stderr,"Linked to netCDF library version %s, compiled %s\n",lbr_vrs_sng,cmp_dat_sng);
  (void)fprintf(stdout,"Homepage URL: http://nco.sf.net\n");
  (void)fprintf(stdout,"User's Guide: http://nco.sf.net/nco.html\n");
  /* fxm: TKN2YESNO breaks when TKN is undefined
     Full macro language like M4 might be useful here, though probably too much trouble */
#define TKN2YESNO(x) ((x+0) ? ("No"):("Yes"))
  /* Configuration option tokens must be consistent among configure.in, bld/Makefile, and nco_ctl.c
     Arrange tokens alphabetically by first word in English text description */
  (void)fprintf(stderr,"Configuration Option:\tActive?\tMeaning or Reference:\nDebugging: Custom\t%s\tPedantic, bounds checking (slowest execution)\nDebugging: Symbols\t%s\tProduce symbols for debuggers (e.g., dbx, gdb)\nInternationalization\t%s\thttp://nco.sf.net/nco.html#i18n (pre-alpha)\nMPI parallelization\t%s\thttp://nco.sf.net/nco.html#mpi (alpha)\nnetCDF 64-bit files\t%s\thttp://nco.sf.net/nco.html#lfs\nOPeNDAP/DODS clients\t%s\thttp://nco.sf.net/nco.html#dap\nOpenMP threading\t%s\thttp://nco.sf.net/nco.html#omp\nOptimization: run-time\t%s\tFastest execution possible (slowest compilation)\nShared libraries built\t%s\tSmall, dynamically linked executables\nStatic libraries built\t%s\tLarge executables with private namespaces\nUDUnits conversions\t%s\thttp://nco.sf.net/nco.html#udunits\nWildcarding (regex)\t%s\thttp://nco.sf.net/nco.html#rx\n%s",
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
#if defined(I18N) && (I18N)
		"Yes",
#else /* !I18N */
		"No",
#endif /* !I18N */
#if defined(NC_64BIT_OFFSET) && (NC_64BIT_OFFSET != 0)
		"Yes",
#else /* !NC_64BIT_OFFSET */
		"No",
#endif /* !NC_64BIT_OFFSET */
#if defined(ENABLE_MPI) && (ENABLE_MPI)
		"Yes",
#else /* !ENABLE_MPI */
		"No",
#endif /* !ENABLE_MPI */
#if defined(ENABLE_DAP) && (ENABLE_DAP)
		"Yes",
#else /* !ENABLE_DAP */
		"No",
#endif /* !ENABLE_DAP */
#if defined(_OPENMP) && (_OPENMP)
		"Yes",
#else /* !_OPENMP */
		"No",
#endif /* !_OPENMP */
#if defined(ENABLE_OPTIMIZE_CUSTOM) && (ENABLE_OPTIMIZE_CUSTOM)
		"Yes",
#else /* !ENABLE_OPTIMIZE_CUSTOM */
		"No",
#endif /* !ENABLE_OPTIMIZE_CUSTOM */
#if defined(ENABLE_SHARED) && (ENABLE_SHARED)
		"Yes",
#else /* !ENABLE_SHARED */
		"No",
#endif /* !ENABLE_SHARED */
#if defined(ENABLE_STATIC) && (ENABLE_STATIC)
		"Yes",
#else /* !ENABLE_STATIC */
		"No",
#endif /* !ENABLE_STATIC */
#if defined(ENABLE_UDUNITS) && (ENABLE_UDUNITS)
		"Yes",
#else /* !ENABLE_UDUNITS */
		"No",
#endif /* !ENABLE_UDUNITS */
#if defined(NCO_HAVE_REGEX_FUNCTIONALITY) && (NCO_HAVE_REGEX_FUNCTIONALITY)
		"Yes",
#else /* !NCO_HAVE_REGEX_FUNCTIONALITY */
		"No",
#endif /* !NCO_HAVE_REGEX_FUNCTIONALITY */
		""); /* End of print statement marker */
  (void)fprintf(stderr,"\n%s",nco_nmn_get());

  lbr_vrs_sng=(char *)nco_free(lbr_vrs_sng);
  lbr_sng=(char *)nco_free(lbr_sng);
  cmp_dat_sng=(char *)nco_free(cmp_dat_sng);
} /* end nco_lbr_vrs_prn() */

const char * /* O [sng] Mnemonic that describes current NCO version */
nco_nmn_get(void) /* [fnc] Return mnemonic that describes current NCO version */
{ 
  /* Purpose: Return mnemonic describing current NCO version */
  return "It's a Girl! 8 lbs. 9 oz.\n";
} /* end nco_nmn_get() */

char * /* O [sng] nm_in stripped of any path (i.e., program name stub) */ 
prg_prs /* [fnc] Strip program name to stub and return program ID */
(const char * const nm_in, /* I [sng] Name of program, i.e., argv[0] (may include path prefix) */
 int * const prg_lcl) /* O [enm] Enumerated number corresponding to nm_in */
{
  /* Purpose: Set program name and enum */
  char *nm_out;
  char *nm_out_tmp;
  char *nm_out_orig;

  /* Get program name (use strrchr() first in case nm_in contains a path) */
  nm_out_orig=nm_out_tmp=(char *)strdup(nm_in);
  if(strrchr(nm_out_tmp,'/') != NULL) nm_out_tmp=strrchr(nm_out_tmp,'/')+1;

  /* Skip possible libtool prefix */
  if(!strncmp(nm_out_tmp,"lt-",3)){nm_out_tmp+=3;}

  /* Classify calling program */
  /* ncap and acceptable synonyms (symbolic links): */
  if(!strcmp(nm_out_tmp,"ncap")){*prg_lcl=ncap;}
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
  /* ncea and acceptable synonyms (symbolic links): */
  else if(!strcmp(nm_out_tmp,"ncea")){*prg_lcl=ncea;}
  else if(!strcmp(nm_out_tmp,"mpncea")){*prg_lcl=ncea;}
  /* ncecat and acceptable synonyms (symbolic links): */
  else if(!strcmp(nm_out_tmp,"ncecat")){*prg_lcl=ncecat;}
  else if(!strcmp(nm_out_tmp,"mpncecat")){*prg_lcl=ncecat;}
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
  else if(!strcmp(nm_out_tmp,"ncra")){*prg_lcl=ncra;}
  else if(!strcmp(nm_out_tmp,"mpncra")){*prg_lcl=ncra;}
  /* ncrcat and acceptable synonyms (symbolic links): */
  else if(!strcmp(nm_out_tmp,"ncrcat")){*prg_lcl=ncrcat;}
  else if(!strcmp(nm_out_tmp,"mpncrcat")){*prg_lcl=ncrcat;}
  /* ncrename and acceptable synonyms (symbolic links): */
  else if(!strcmp(nm_out_tmp,"ncrename")){*prg_lcl=ncrename;}
  /* ncwa and acceptable synonyms (symbolic links): */
  else if(!strcmp(nm_out_tmp,"ncwa")){*prg_lcl=ncwa;}
  else if(!strcmp(nm_out_tmp,"mpncwa")){*prg_lcl=ncwa;}
  else{
    (void)fprintf(stdout,"%s: ERROR executable name %s not registered in prg_prs()\n",nm_out_tmp,nm_out_tmp);
    nco_exit(EXIT_FAILURE);
  } /* end else */

  /* Duplicate stub for returning */
  nm_out=(char *)strdup(nm_out_tmp);
  /* Free copy of argv[0] */
  nm_out_orig=(char *)nco_free(nm_out_orig);
  return nm_out;

} /* end prg_prs() */

void 
nco_usg_prn(void)
{
  /* Purpose: Print correct command-line usage (currently to stdout) */

  char *opt_sng=NULL_CEWI;

  int prg_lcl;

  prg_lcl=prg_get();

  switch(prg_lcl){
  case ncap:
    opt_sng=(char *)strdup("[-4] [-A] [-C] [-c] [-D dbg_lvl] [-d ...] [-F] [-f] [-h] [-l path] [-O] [-o out.nc] [-p path] [-R] [-r] [-s algebra] [-S fl.nco] [-v] [-Z] in.nc [out.nc]\n");
    break;
  case ncatted:
    opt_sng=(char *)strdup("[-4] [-a ...] [-D dbg_lvl] [-h] [-l path] [-O] [-o out.nc] [-p path] [-R] [-r] [-Z] in.nc [[out.nc]]\n");
    break;
  case ncbo:
    opt_sng=(char *)strdup("[-4] [-A] [-C] [-c] [-D dbg_lvl] [-d ...] [-F] [-h] [-l path] [-n ...] [-O] [-o out.nc] [-p path] [-R] [-r] [-t thr_nbr] [-v ...] [-x] [-y op_typ] in_1.nc in_2.nc [out.nc]\n");
    break;
  case ncflint:
    opt_sng=(char *)strdup("[-4] [-A] [-C] [-c] [-D dbg_lvl] [-d ...] [-F] [-h] [-i var,val] [-l path] [-O] [-o out.nc] [-p path] [-R] [-r] [-t thr_nbr] [-v ...] [-x] [-w wgt_1[,wgt_2]] [-Z] in_1.nc in_2.nc [out.nc]\n");
    break;
  case ncks:
    opt_sng=(char *)strdup("[-4] [-A] [-a] [-B] [-b fl_bnr] [-C] [-c] [-D dbg_lvl] [-d ...] [-F] [-H] [-h] [-l path] [-m] [-M] [-O] [-o out.nc] [-P] [-p path] [-Q] [-q] [-R] [-r] [-s format] [-u] [-v ...] [-x] [-Z] in.nc [[out.nc]]\n");
    break;
  case ncpdq:
    opt_sng=(char *)strdup("[-4] [-A] [-a ...] [-C] [-c] [-D dbg_lvl] [-d ...] [-F] [-h] [-l path] [-M pck_map][-O] [-o out.nc] [-P pck_plc] [-p path] [-R] [-r] [-t thr_nbr] [-v ...] [-U] [-x] [-Z] in.nc [out.nc]\n");
    break;
  case ncra:
  case ncea:
    opt_sng=(char *)strdup("[-4] [-A] [-C] [-c] [-D dbg_lvl] [-d ...] [-F] [-H] [-h] [-l path] [-n ...] [-O] [-o out.nc] [-p path] [-R] [-r] [-t thr_nbr] [-v ...] [-x] [-y op_typ] [-Z] in.nc [...] [out.nc]\n");
    break;
  case ncrcat:
  case ncecat:
    opt_sng=(char *)strdup("[-4] [-A] [-C] [-c] [-D dbg_lvl] [-d ...] [-F] [-H] [-h] [-l path] [-n ...] [-O] [-o out.nc] [-p path] [-R] [-r] [-t thr_nbr] [-v ...] [-x] [-Z] in.nc [...] [out.nc]\n");
    break;
  case ncrename:
    opt_sng=(char *)strdup("[-4] [-a ...] [-D dbg_lvl] [-d ...] [-h] [-l path] [-O] [-o out.nc] [-p path] [-R] [-r] [-v ...] [-Z] in.nc [[out.nc]]\n");
    break;
  case ncwa:
    opt_sng=(char *)strdup("[-4] [-A] [-a ...] [-C] [-c] [-D dbg_lvl] [-d ...] [-F] [-h] [-I] [-l path] [-m mask] [-M val] [-N] [-O] [-o out.nc] [-p path] [-R] [-r] [-T condition] [-t thr_nbr] [-v ...] [-w wgt] [-x] [-y op_typ] [-Z] in.nc [out.nc]\n");
    break;
  default: nco_dfl_case_prg_id_err(); break;
  } /* end switch */
  
  /* We now have command-specific command line option string */
  (void)fprintf(stdout,"%s command line options cheat-sheet:\n",prg_nm_get());
  (void)fprintf(stdout,"%s %s\n",prg_nm_get(),opt_sng);

  if(strstr(opt_sng,"-4")) (void)fprintf(stdout,"-4, --hdf, --netcdf4\tOutput netCDF4/HDF storage format\n");
  if(strstr(opt_sng,"-A")) (void)fprintf(stdout,"-A, --apn, --append\tAppend to existing output file, if any\n");
  if(strstr(opt_sng,"-a")){
    if(prg_lcl == ncatted) (void)fprintf(stdout,"-a, --attribute att_nm,var_nm,mode,att_typ,att_val Attribute specification:\n\t\t\tmode = a,c,d,m,o and att_typ = f,d,l,s,c,b\n");
    if(prg_lcl == ncks) (void)fprintf(stdout,"-a, --abc, --alphabetize\tDisable alphabetization of extracted variables\n");
    if(prg_lcl == ncpdq) (void)fprintf(stdout,"-a, --arrange, --permute, --reorder, --rdr [-]rdr_dim1[,[-]rdr_dim2[...]] Re-order dimensions\n");
    if(prg_lcl == ncrename) (void)fprintf(stdout,"-a, --attribute old_att,new_att Attribute's old and new names\n");
    if(prg_lcl == ncwa) (void)fprintf(stdout,"-a, --avg, --average avg_dim1[,avg_dim2[...]] Averaging dimensions\n");
  } /* end if */
  if(strstr(opt_sng,"-B")) (void)fprintf(stdout,"-B, --bnr, --binary\tWrite data to unformatted binary file\n");
  if(strstr(opt_sng,"-b")) (void)fprintf(stdout,"-b, --fl_bnr, --binary-file fl_bnr\tUnformatted binary file to write\n");
  if(strstr(opt_sng,"-c")) (void)fprintf(stdout,"-c, --crd, --coords\tCoordinate variables will all be processed\n");
  if(strstr(opt_sng,"-C")) (void)fprintf(stdout,"-C, --nocoords\t\tAssociated coordinate variables should not be processed\n");
  if(strstr(opt_sng,"-D")) (void)fprintf(stdout,"-D, --dbg_lvl, --debug-level dbg_lvl\tDebugging level\n");
  if(strstr(opt_sng,"-d")){
    if(prg_lcl == ncrename) (void)fprintf(stdout,"-d, --dmn, --dimension old_dim,new_dim Dimension's old and new names\n");
    else if(prg_lcl == ncks) (void)fprintf(stdout,"-d, --dmn, --dimension dim,[min][,[max]][,[stride]] Dimension's limits and stride in hyperslab\n");
    else if(prg_lcl == ncra || prg_lcl == ncrcat) (void)fprintf(stdout,"-d, --dmn, --dimension dim,[min][,[max]][,[stride]] Dimension's limits (any dimension) and stride (record dimension only) in hyperslab\n");
    else (void)fprintf(stdout,"-d, --dmn, --dimension dim,[min][,[max]] Dimension's limits in hyperslab\n");
  } /* end if -d */
  if(strstr(opt_sng,"-F")) (void)fprintf(stdout,"-F, --ftn, --fortran\tFortran indexing conventions (1-based) for I/O\n");
  if(strstr(opt_sng,"-f")) (void)fprintf(stdout,"-f, --fnc_tbl, --prn_fnc_tbl\tPrint function table\n");
  if(strstr(opt_sng,"-H")){
    if(prg_lcl == ncks) (void)fprintf(stdout,"-H, --huh, --hmm\tToggle printing data\n");
    if(nco_is_mlt_fl_opr(prg_lcl)) (void)fprintf(stdout,"-H, --fl_lst_in, --file_list\tDo not create \"input_file_list\" global attribute\n");
  } /* end if -H */
  if(strstr(opt_sng,"-h")) (void)fprintf(stdout,"-h, --hst, --history\tDo not append to \"history\" global attribute\n");
  if(strstr(opt_sng,"-i")) (void)fprintf(stdout,"-i, --ntp, --interpolate var,val\tInterpolant and value\n");
  if(strstr(opt_sng,"-I")) (void)fprintf(stdout,"-I, --wgt_msk_crd_var \tDo not weight or mask coordinate variables\n");
  if(strstr(opt_sng,"-l")) (void)fprintf(stdout,"-l, --lcl, --local path\tLocal storage path for remotely-retrieved files\n");
  if(strstr(opt_sng,"-M")){
    if(prg_lcl == ncwa) (void)fprintf(stdout,"-M, --msk_val, --mask-value, --mask_value mask_val\tMasking value (default is 1.0)\n");
    if(prg_lcl == ncks) (void)fprintf(stdout,"-M, --Mtd, --Metadata\tToggle printing global metadata\n");
    if(prg_lcl == ncpdq) (void)fprintf(stdout,"-M, --pck_map, --map pck_map\tPack map [flt_sht,flt_byt,hgh_sht,hgh_byt,nxt_lsr]\n");
  } /* end if */
  if(strstr(opt_sng,"-m")){
    if(prg_lcl == ncwa) (void)fprintf(stdout,"-m, --msk_nm, --msk_var, --mask-variable, --mask_variable mask_var\tMasking variable name\n");
    if(prg_lcl == ncks) (void)fprintf(stdout,"-m, --mtd, --metadata\tToggle printing variable metadata\n");
  } /* end if */
  if(strstr(opt_sng,"-N")) (void)fprintf(stdout,"-N, --nmr, --numerator\tNo normalization\n");
  if(strstr(opt_sng,"-n")){
    /*    if(prg_lcl == ncwa) (void)fprintf(stdout,"-n\t\tNormalize by tally but not weight\n");*/
    if(prg_lcl != ncwa) (void)fprintf(stdout,"-n, --nintap nbr_files,[nbr_numeric_chars[,increment]] NINTAP-style abbreviation of file list\n");
  } /* end if -n */
  if(strstr(opt_sng,"-o")) (void)fprintf(stdout,"-o, --output, --fl_out fl_out\tOutput file name (or use last positional argument)\n");
  if(strstr(opt_sng,"-O")) (void)fprintf(stdout,"-O, --ovr, --overwrite\tOverwrite existing output file, if any\n");
  if(strstr(opt_sng,"-P")){
    if(prg_lcl == ncks) (void)fprintf(stdout,"-P, --prn, --print\tPrint data, metadata, and units. Abbreviation for -C -H -M -m -u.\n");
    if(prg_lcl == ncpdq) (void)fprintf(stdout,"-P, --pck_plc, --pack_policy pck_plc\tPacking policy [all_new,all_xst,xst_new,upk]\n");
  } /* end if -P */
  if(strstr(opt_sng,"-p")) (void)fprintf(stdout,"-p, --pth, --path path\tPath prefix for all input filenames\n");
  if(strstr(opt_sng,"-Q")) (void)fprintf(stdout,"-Q, \t\t\tToggle printing of dimension indices and coordinate values\n");
  if(strstr(opt_sng,"-q")) (void)fprintf(stdout,"-q, --quiet\t\tTurn off all printing to screen\n");
  if(strstr(opt_sng,"-R")) (void)fprintf(stdout,"-R, --rtn, --retain\tRetain remotely-retrieved files after use\n");
  if(strstr(opt_sng,"-r")) (void)fprintf(stdout,"-r, --revision, --vrs, --version\tProgram version and copyright notice\n");
  if(strstr(opt_sng,"-s")){
    if(prg_lcl != ncap) (void)fprintf(stdout,"-s, --sng, --string, --fmt, --format format\tString format for text output\n");
    if(prg_lcl == ncap) (void)fprintf(stdout,"-s, --spt, --script algebra\tAlgebraic command defining single output variable\n");
  } /* end if */
  if(strstr(opt_sng,"-S")) (void)fprintf(stdout,"-S, --fl_spt, --script-file fl.nco\tScript file containing multiple algebraic commands\n");
  if(strstr(opt_sng,"-T")) (void)fprintf(stdout,"-T, --truth_condition, --msk_cmp_typ, --op_rlt condition\tTruth condition for masking: eq,ne,ge,le,gt,lt\n");
  if(strstr(opt_sng,"-t")) (void)fprintf(stdout,"-t, --thr_nbr, --threads, --omp_num_threads thr_nbr\tThread number for OpenMP\n");
  if(strstr(opt_sng,"-u")) (void)fprintf(stdout,"-u, --units\t\tToggle printing units of variables, if any\n");
  if(strstr(opt_sng,"-U")) (void)fprintf(stdout,"-U, --upk, --unpack\t\tUnpack input file\n");
  if(strstr(opt_sng,"-v")){
    if(prg_lcl == ncrename) (void)fprintf(stdout,"-v, --variable old_var,new_var Variable's old and new names\n");
	if(prg_lcl == ncap) (void)fprintf(stdout,"-v, --variable \t\tOutput file includes ONLY user-defined variables\n");
    if(prg_lcl != ncrename && prg_lcl != ncap) (void)fprintf(stdout,"-v, --variable var1[,var2[...]] Variable(s) to process (regular expressions supported)\n");
  } /* end if */
  /*  if(strstr(opt_sng,"-W")) (void)fprintf(stdout,"-W\t\tNormalize by weight but not tally\n");*/
  if(strstr(opt_sng,"-w")){
    if(prg_lcl == ncwa) (void)fprintf(stdout,"-w, --wgt_var, --weight wgt\tWeighting variable name\n");
    if(prg_lcl == ncflint) (void)fprintf(stdout,"-w, --wgt_var, --weight wgt_1[,wgt_2] Weight(s) of file(s)\n");
  } /* end if */
  if(strstr(opt_sng,"-x")) (void)fprintf(stdout,"-x, --xcl, --exclude\tExtract all variables EXCEPT those specified with -v\n");
  if(strstr(opt_sng,"-y")){
    if(prg_lcl == ncbo)(void)fprintf(stdout,"-y, --op_typ, --operation op_typ\tBinary arithmetic operation: add,sbt,mlt,dvd (+,-,*,/)\n");
    if(prg_lcl == ncra || prg_lcl == ncea || prg_lcl == ncwa)(void)fprintf(stdout,"-y, --op_typ, --operation op_typ\tArithmetic operation: avg,min,max,ttl,sqravg,avgsqr,sqrt,rms,rmssdn\n");
  }
  if(strstr(opt_sng,"-Z")) (void)fprintf(stdout,"-Z, --64-bit-offset\tWrite output file in 64-bit offset mode\n");
  if(strstr(opt_sng,"in.nc")) (void)fprintf(stdout,"in.nc\t\t\tInput file name(s)\n");
  if(strstr(opt_sng,"[out.nc]")) (void)fprintf(stdout,"[out.nc]\t\tOutput file name (or use -o switch)\n");
/*  if(strstr(opt_sng,"-")) (void)fprintf(stdout,"-\n");*/

  /* Free the space holding option string */
  opt_sng=(char *)nco_free(opt_sng);

  /* Public service announcements */
  (void)fprintf(stdout,"Where to find more help on %s and/or NCO:\n",prg_nm_get());
  (void)fprintf(stdout,"1. User's Guide/Reference Manual: http://nco.sf.net#RTFM\n");
  (void)fprintf(stdout,"2. On-line reference manual for %s: http://nco.sf.net/nco.html#%s\n",prg_nm_get(),prg_nm_get());
  (void)fprintf(stdout,"3. UNIX man page: \'man %s\'\n",prg_nm_get());
  (void)fprintf(stdout,"4. Known problems: http://nco.sf.net#Bugs\n");
  (void)fprintf(stdout,"5. Help Forum: http://sourceforge.net/forum/forum.php?forum_id=9830\n");
  (void)fprintf(stdout,"Post questions, suggestions, patches at http://sf.net/projects/nco\n");

} /* end nco_usg_prn() */

