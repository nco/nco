/* $Header$ */

/* This single source file compiles into one executable that behaves as three different commands depending on invocation name:
   ncra -- netCDF record averager
   nces -- netCDF ensemble statistics
   ncrcat -- netCDF record concatenator */

/* Purpose: Compute averages or extract series of specified hyperslabs of 
   specfied variables of multiple input netCDF files and output them 
   to a single file. */

/* Copyright (C) 1995--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License.
   You are permitted to link NCO with the HDF, netCDF, OPeNDAP, and UDUnits
   libraries and to distribute the resulting executables under the terms 
   of the BSD, but in addition obeying the extra stipulations of the 
   HDF, netCDF, OPeNDAP, and UDUnits licenses.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
   See the 3-Clause BSD License for more details.
   
   The original author of this software, Charlie Zender, seeks to improve
   it with your suggestions, contributions, bug-reports, and patches.
   Please contact the NCO project at http://nco.sf.net or write to
   Charlie Zender
   Department of Earth System Science
   University of California, Irvine
   Irvine, CA 92697-3100 */

/* URL: https://github.com/nco/nco/tree/master/src/nco/ncra.c
   
   Usage:
   ncra -O -n 3,4,1 -p ${HOME}/nco/data h0001.nc ~/foo.nc
   ncra -O -n 3,4,1 -p ${HOME}/nco/data -l ${HOME} h0001.nc ~/foo.nc
   ncra -O -n 3,4,1 -p /ZENDER/tmp -l ${HOME}/nco/data h0001.nc ~/foo.nc
   ncrcat -O -C -d time,0,5,4,2 -v time -p ~/nco/data in.nc ~/foo.nc
   ncra -O -C -d time,0,5,4,2 -v time -p ~/nco/data in.nc ~/foo.nc
   ncra -O -C --mro -d time,0,5,4,2 -v time -p ~/nco/data in.nc ~/foo.nc
   ncra -O -w 1,2,3 -n 3,4,1 -p ${HOME}/nco/data h0001.nc ~/foo.nc
   ncra -O -w one_dmn_rec_var -n 3,4,1 -p ${HOME}/nco/data h0001.nc ~/foo.nc
   
   scp ~/nco/src/nco/ncra.c esmf.ess.uci.edu:nco/src/nco
   
   nces in.nc in.nc ~/foo.nc
   nces -O -n 3,4,1 -p ${HOME}/nco/data h0001.nc ~/foo.nc
   nces -O -n 3,4,1 -p ${HOME}/nco/data -l ${HOME} h0001.nc ~/foo.nc
   nces -O -n 3,4,1 -p /ZENDER/tmp -l ${HOME} h0001.nc ~/foo.nc

   ncra -Y ncge -O -p ~/nco/data mdl_1.nc ~/foo.nc
   ncra -Y ncge -O --nsm_sfx=_avg -p ~/nco/data mdl_1.nc ~/foo.nc */

#ifdef HAVE_CONFIG_H
# include <config.h> /* Autotools tokens */
#endif /* !HAVE_CONFIG_H */

/* Standard C headers */
#include <math.h> /* sin cos cos sin 3.14159 */
#include <stdio.h> /* stderr, FILE, NULL, etc. */
#include <stdlib.h> /* atof, atoi, malloc, getopt */
#include <string.h> /* strcmp() */
#include <sys/stat.h> /* stat() */
#include <time.h> /* machine time */
#ifndef _MSC_VER
# include <unistd.h> /* POSIX stuff */
#endif
#ifndef HAVE_GETOPT_LONG
# include "nco_getopt.h"
#else /* HAVE_GETOPT_LONG */
# ifdef HAVE_GETOPT_H
#  include <getopt.h>
# endif /* !HAVE_GETOPT_H */ 
#endif /* HAVE_GETOPT_LONG */

#ifdef I18N
# include <langinfo.h> /* nl_langinfo() */
# include <libintl.h> /* Internationalization i18n */
# include <locale.h> /* Locale setlocale() */
# define _(sng) gettext (sng)
# define gettext_noop(sng) (sng)
# define N_(sng) gettext_noop(sng)
#endif /* I18N */
/* Supply stub gettext() function in case i18n failed */
#ifndef _LIBINTL_H
# define gettext(foo) foo
#endif /* _LIBINTL_H */

/* 3rd party vendors */	 
#include <netcdf.h> /* netCDF definitions and C library */	 
#ifdef ENABLE_MPI
# include <mpi.h> /* MPI definitions */
# include <netcdf_par.h> /* Parallel netCDF definitions */
# include "nco_mpi.h" /* MPI utilities */
#endif /* !ENABLE_MPI */

/* Personal headers */
/* #define MAIN_PROGRAM_FILE MUST precede #include libnco.h */
#define MAIN_PROGRAM_FILE
#include "nco.h" /* netCDF Operator (NCO) definitions */
#include "libnco.h" /* netCDF Operator (NCO) library */

/* Define inline'd functions in header so source is visible to calling files
   C99 only: Declare prototype in exactly one header
   http://www.drdobbs.com/the-new-c-inline-functions/184401540 */
extern int min_int(int a, int b);
extern int max_int(int a, int b);
inline int min_int(int a, int b){return (a < b) ? a : b;}
inline int max_int(int a, int b){return (a > b) ? a : b;}
extern long min_lng(long a, long b);
extern long max_lng(long a, long b);
inline long min_lng(long a, long b){return (a < b) ? a : b;}
inline long max_lng(long a, long b){return (a > b) ? a : b;}

int
main(int argc,char **argv)
{
  char **fl_lst_abb=NULL; /* Option n */
  char **fl_lst_in;
  char **gaa_arg=NULL; /* [sng] Global attribute arguments */
  char **grp_lst_in=NULL_CEWI;
  char **var_lst_in=NULL_CEWI;
  char **wgt_lst_in=NULL_CEWI;
  char *aux_arg[NC_MAX_DIMS];
  char *cmd_ln;
  char *clm_nfo_sng=NULL; /* [sng] Climatology information string */
  char *cnk_arg[NC_MAX_DIMS];
  char *cnk_map_sng=NULL_CEWI; /* [sng] Chunking map */
  char *cnk_plc_sng=NULL_CEWI; /* [sng] Chunking policy */
  char *fl_in=NULL;
  char *fl_out=NULL; /* Option o */
  char *fl_out_tmp=NULL_CEWI;
  char *fl_pth=NULL; /* Option p */
  char *fl_pth_lcl=NULL; /* Option l */
  char *grp_out_fll=NULL; /* [sng] Group name */
  char *lmt_arg[NC_MAX_DIMS];
  char *nco_op_typ_sng=NULL_CEWI; /* [sng] Operation type Option y */
  char *nco_pck_plc_sng=NULL_CEWI; /* [sng] Packing policy Option P */
  char *nsm_sfx=NULL; /* [sng] Ensemble suffix */
  char *opt_crr=NULL; /* [sng] String representation of current long-option name */
  char *optarg_lcl=NULL; /* [sng] Local copy of system optarg */
  char *ppc_arg[NC_MAX_VARS]; /* [sng] PPC arguments */
  char *sng_cnv_rcd=NULL_CEWI; /* [sng] strtol()/strtoul() return code */
  char *wgt_nm=NULL_CEWI; /* [sng] Weight variable */
  char trv_pth[]="/"; /* [sng] Root path of traversal tree */

  const char * const CVS_Id="$Id$"; 
  const char * const CVS_Revision="$Revision$";
  const char * const opt_sht_lst="34567ACcD:d:FG:g:HhL:l:Nn:Oo:p:P:rRt:v:w:X:xY:y:-:";

  clm_bnd_sct *cb=NULL;
  
  cnk_sct cnk; /* [sct] Chunking structure */

  cnv_sct *cnv; /* [sct] Convention structure */

#if defined(__cplusplus) || defined(PGI_CC)
  ddra_info_sct ddra_info;
  ddra_info.flg_ddra=False;
#else /* !__cplusplus */
  ddra_info_sct ddra_info={.flg_ddra=False};
#endif /* !__cplusplus */

  dmn_sct **dim=NULL; /* CEWI */
  dmn_sct **dmn_out=NULL; /* CEWI */

  double *wgt_arr=NULL; /* Option w */
  double wgt_avg_scl=0.0; /* [frc] Scalar version of wgt_avg */

  extern char *optarg;
  extern int optind;

  /* Using naked stdin/stdout/stderr in parallel region generates warning
  Copy appropriate filehandle to variable scoped shared in parallel clause */
  FILE * const fp_stderr=stderr; /* [fl] stderr filehandle CEWI */
  FILE * const fp_stdout=stdout; /* [fl] stdout filehandle CEWI */

  gpe_sct *gpe=NULL; /* [sng] Group Path Editing (GPE) structure */

  int *in_id_arr;

  const int rec_dmn_idx=0; /* [idx] Assumed index of current record dimension where zero assumes record is leading dimension */

  int abb_arg_nbr=0;
  int aux_nbr=0; /* [nbr] Number of auxiliary coordinate hyperslabs specified */
  int cnk_map=nco_cnk_map_nil; /* [enm] Chunking map */
  int cnk_nbr=0; /* [nbr] Number of chunk sizes */
  int cnk_plc=nco_cnk_plc_nil; /* [enm] Chunking policy */
  int dfl_lvl=NCO_DFL_LVL_UNDEFINED; /* [enm] Deflate level */
  int dmn_rec_fl;
  int fl_idx;
  int fl_in_fmt; /* [enm] Input file format */
  int fl_nbr=0;
  int fl_out_fmt=NCO_FORMAT_UNDEFINED; /* [enm] Output file format */
  int flg_input_complete_nbr=0; /* [nbr] Number of record dimensions completed */
  int fll_md_old; /* [enm] Old fill mode */
  int gaa_nbr=0; /* [nbr] Number of global attributes to add */
  int grp_id;        /* [ID] Group ID */
  int grp_lst_in_nbr=0; /* [nbr] Number of groups explicitly specified by user */
  int grp_out_id;    /* [ID] Group ID (output) */
  int idx=int_CEWI;
  int idx_rec=0; /* [idx] Index that iterates over number of record dimensions */
  int in_id;
  int lmt_nbr=0; /* Option d. NB: lmt_nbr gets incremented */
  int log_lvl=0; /* [enm] netCDF library debugging verbosity [0..5] */
  int md_open; /* [enm] Mode flag for nc_open() call */
  int nbr_dmn_fl;
  int nbr_dmn_xtr=0;
  int nbr_rec; /* [nbr] (ncra) Number of record dimensions  */
  int nbr_var_fix; /* nbr_var_fix gets incremented */
  int nbr_var_fl;
  int nbr_var_prc; /* nbr_var_prc gets incremented */
  int nco_op_typ=nco_op_avg; /* [enm] Default operation is averaging */
  int nco_pck_plc=nco_pck_plc_nil; /* [enm] Default packing is none */
  int opt;
  int out_id;  
  int ppc_nbr=0; /* [nbr] Number of PPC arguments */
  int rcd=NC_NOERR; /* [rcd] Return code */
  int thr_idx; /* [idx] Index of current thread */
  int thr_nbr=int_CEWI; /* [nbr] Thread number Option t */
  int var_lst_in_nbr=0;
  int var_out_id; /* [ID] Variable ID (output) */
  int wgt_nbr=0; 
  int xtr_nbr=0; /* xtr_nbr won't otherwise be set for -c with no -v */

  lmt_sct **lmt_rec=NULL; /* [lst] (ncra) Record dimensions */

  long idx_rec_crr_in; /* [idx] Index of current record in current input file */
  long *idx_rec_out=NULL; /* [idx] Index of current record in output file (0 is first, ...) */
  long ilv_srd; /* [idx] Interleave stride */
  long *rec_in_cml=NULL; /* [nbr] Number of records, read or not, in all processed files */
  long *rec_usd_cml=NULL; /* [nbr] Cumulative number of input records used (catenated by ncrcat or operated on by ncra) */
  long rec_dmn_sz=0L; /* [idx] Size of record dimension, if any, in current file (increments by srd) */
  long rec_rmn_prv_ssc=0L; /* [idx] Records remaining to be read in current subcycle group */
  long rec_rmn_prv_ilv=0L; /* [idx] Records remaining to be read in current interleaved index */

  md5_sct *md5=NULL; /* [sct] MD5 configuration */

  nco_bool *REC_LST_DSR=NULL; /* [flg] Record is last desired from all input files */
  nco_bool *flg_input_complete=NULL; /* [flg] All requested records in record dimension have been read */

  nco_bool CNV_ARM;
  nco_bool EXCLUDE_INPUT_LIST=False; /* Option c */
  nco_bool EXTRACT_ALL_COORDINATES=False; /* Option c */
  nco_bool EXTRACT_ASSOCIATED_COORDINATES=True; /* Option C */
  nco_bool EXTRACT_CLL_MSR=True; /* [flg] Extract cell_measures variables */
  nco_bool EXTRACT_FRM_TRM=True; /* [flg] Extract formula_terms variables */
  nco_bool FLG_BFR_NRM=False; /* [flg] Current output buffers need normalization */
  nco_bool FLG_ILV=False; /* [flg] Interleave Output */
  nco_bool FLG_MRO=False; /* [flg] Multi-Record Output */
  nco_bool FLG_MSO=False; /* [flg] Multi-Subcycle Output */
  nco_bool FL_LST_IN_APPEND=True; /* Option H */
  nco_bool FL_LST_IN_FROM_STDIN=False; /* [flg] fl_lst_in comes from stdin */
  nco_bool FL_RTR_RMT_LCN;
  nco_bool FORCE_APPEND=False; /* Option A */
  nco_bool FORCE_OVERWRITE=False; /* Option O */
  nco_bool FORTRAN_IDX_CNV=False; /* Option F */
  nco_bool GRP_VAR_UNN=False; /* [flg] Select union of specified groups and variables */
  nco_bool HISTORY_APPEND=True; /* Option h */
  nco_bool HPSS_TRY=False; /* [flg] Search HPSS for unfound files */
  nco_bool MSA_USR_RDR=False; /* [flg] Multi-Slab Algorithm returns hyperslabs in user-specified order */
  nco_bool NORMALIZE_BY_WEIGHT=True; /* [flg] Normalize by command-line weight */
  nco_bool NRM_BY_DNM=True; /* [flg] Normalize by denominator */
  nco_bool PROMOTE_INTS=False; /* [flg] Promote integers to floating point in output */
  nco_bool RAM_CREATE=False; /* [flg] Create file in RAM */
  nco_bool RAM_OPEN=False; /* [flg] Open (netCDF3-only) file(s) in RAM */
  nco_bool SHARE_CREATE=False; /* [flg] Create (netCDF3-only) file(s) with unbuffered I/O */
  nco_bool SHARE_OPEN=False; /* [flg] Open (netCDF3-only) file(s) with unbuffered I/O */
  nco_bool REC_APN=False; /* [flg] Append records directly to output file */
  nco_bool REC_FRS_GRP=False; /* [flg] Record is first in current group */
  nco_bool REC_LST_GRP=False; /* [flg] Record is last in current group */
  nco_bool REC_SRD_LST=False; /* [flg] Record belongs to last stride of current file */
  nco_bool RM_RMT_FL_PST_PRC=True; /* Option R */
  nco_bool WRT_TMP_FL=True; /* [flg] Write output to temporary file */
  nco_bool flg_cll_mth=True; /* [flg] Add/modify cell_methods attributes */
  nco_bool flg_cb=False; /* [flg] Climatology bounds */
  nco_bool flg_mmr_cln=True; /* [flg] Clean memory prior to exit */
  nco_bool flg_skp1; /* [flg] Current record is not dimension of this variable */
  nco_bool flg_skp2; /* [flg] Current record is not dimension of this variable */
  nco_bool flg_wgt_by_rec_not_by_fl=False; /* [flg] Weight each record (not file) by command-line numeric weights, if any */

  nco_dmn_dne_t *flg_dne=NULL; /* [lst] Flag to check if input dimension -d "does not exist" */

  nco_int base_time_srt=nco_int_CEWI;
  nco_int base_time_crr=nco_int_CEWI;

  nc_type var_prc_typ_pre_prm=NC_NAT; /* [enm] Type of variable before promotion */
  nc_type var_typ_out=NC_NAT; /* [enm] Type of variable in output file */

  scv_sct wgt_scv;
  scv_sct wgt_avg_scv;
  
  size_t bfr_sz_hnt=NC_SIZEHINT_DEFAULT; /* [B] Buffer size hint */
  size_t cnk_csh_byt=NCO_CNK_CSH_BYT_DFL; /* [B] Chunk cache size */
  size_t cnk_min_byt=NCO_CNK_SZ_MIN_BYT_DFL; /* [B] Minimize size of variable to chunk */
  size_t cnk_sz_byt=0UL; /* [B] Chunk size in bytes */
  size_t cnk_sz_scl=0UL; /* [nbr] Chunk size scalar */
  size_t hdr_pad=0UL; /* [B] Pad at end of header section */

  trv_sct *var_trv; /* [sct] Variable GTT object */

  trv_tbl_sct *trv_tbl; /* [lst] Traversal table */

  var_sct **var;
  var_sct **var_fix;
  var_sct **var_fix_out;
  var_sct **var_out=NULL_CEWI;
  var_sct **var_prc;
  var_sct **var_prc_out;
  var_sct *wgt=NULL; /* [sct] Raw weight on disk in input file */
  var_sct *wgt_out=NULL; /* [sct] Copy of wgt
			    Tally and val members malloc'd & initialized
			    IDs updated each new file by nco_var_mtd_refresh() in file loop
			    Current record value obtained by nco_msa_var_get_rec_trv() in record loop
			    One copy of wgt_out used for all variables */
  var_sct *wgt_avg=NULL; /* [sct] Copy of wgt_out created to mimic var_prc_out processing 
			    Holds running total and tally of weight
			    Acts as op2 for wgt_out averaging just before var_prc[nbr_var_prc-1] */
#ifdef ENABLE_MPI
  /* Declare all MPI-specific variables here */
  MPI_Comm mpi_cmm=MPI_COMM_WORLD; /* [prc] Communicator */
  int prc_rnk; /* [idx] Process rank */
  int prc_nbr=0; /* [nbr] Number of MPI processes */
#endif /* !ENABLE_MPI */
  
  static struct option opt_lng[]={ /* Structure ordered by short option key if possible */
    /* Long options with no argument, no short option counterpart */
    {"cll_msr",no_argument,0,0}, /* [flg] Extract cell_measures variables */
    {"cell_measures",no_argument,0,0}, /* [flg] Extract cell_measures variables */
    {"no_cll_msr",no_argument,0,0}, /* [flg] Do not extract cell_measures variables */
    {"no_cell_measures",no_argument,0,0}, /* [flg] Do not extract cell_measures variables */
    {"frm_trm",no_argument,0,0}, /* [flg] Extract formula_terms variables */
    {"formula_terms",no_argument,0,0}, /* [flg] Extract formula_terms variables */
    {"no_frm_trm",no_argument,0,0}, /* [flg] Do not extract formula_terms variables */
    {"no_formula_terms",no_argument,0,0}, /* [flg] Do not extract formula_terms variables */
    {"cll_mth",no_argument,0,0}, /* [flg] Add/modify cell_methods attributes */
    {"cell_methods",no_argument,0,0}, /* [flg] Add/modify cell_methods attributes */
    {"no_cll_mth",no_argument,0,0}, /* [flg] Do not add/modify cell_methods attributes */
    {"no_cell_methods",no_argument,0,0}, /* [flg] Do not add/modify cell_methods attributes */
    {"clean",no_argument,0,0}, /* [flg] Clean memory prior to exit */
    {"mmr_cln",no_argument,0,0}, /* [flg] Clean memory prior to exit */
    {"drt",no_argument,0,0}, /* [flg] Allow dirty memory on exit */
    {"dirty",no_argument,0,0}, /* [flg] Allow dirty memory on exit */
    {"mmr_drt",no_argument,0,0}, /* [flg] Allow dirty memory on exit */
    {"dbl",no_argument,0,0}, /* [flg] Arithmetic convention: promote float to double */
    {"flt",no_argument,0,0}, /* [flg] Arithmetic convention: keep single-precision */
    {"rth_dbl",no_argument,0,0}, /* [flg] Arithmetic convention: promote float to double */
    {"rth_flt",no_argument,0,0}, /* [flg] Arithmetic convention: keep single-precision */
    {"hdf4",no_argument,0,0}, /* [flg] Treat file as HDF4 */
    {"hdf_upk",no_argument,0,0}, /* [flg] HDF unpack convention: unpacked=scale_factor*(packed-add_offset) */
    {"hdf_unpack",no_argument,0,0}, /* [flg] HDF unpack convention: unpacked=scale_factor*(packed-add_offset) */
    {"help",no_argument,0,0},
    {"hlp",no_argument,0,0},
    {"hpss_try",no_argument,0,0}, /* [flg] Search HPSS for unfound files */
    {"md5_dgs",no_argument,0,0}, /* [flg] Perform MD5 digests */
    {"md5_digest",no_argument,0,0}, /* [flg] Perform MD5 digests */
    {"mro",no_argument,0,0}, /* [flg] Multi-Record Output */
    {"mso",no_argument,0,0}, /* [flg] Multi-Subcycle Output */
    {"multi_record_output",no_argument,0,0}, /* [flg] Multi-Record Output */
    {"multi_subcycle_output",no_argument,0,0}, /* [flg] Multi-Subcycle Output */
    {"msa_usr_rdr",no_argument,0,0}, /* [flg] Multi-Slab Algorithm returns hyperslabs in user-specified order */
    {"msa_user_order",no_argument,0,0}, /* [flg] Multi-Slab Algorithm returns hyperslabs in user-specified order */
    {"nsm_fl",no_argument,0,0},
    {"nsm_grp",no_argument,0,0},
    {"ram_all",no_argument,0,0}, /* [flg] Open and create (netCDF3) file(s) in RAM */
    {"create_ram",no_argument,0,0}, /* [flg] Create file in RAM */
    {"open_ram",no_argument,0,0}, /* [flg] Open (netCDF3) file(s) in RAM */
    {"diskless_all",no_argument,0,0}, /* [flg] Open and create (netCDF3) file(s) in RAM */
    {"per_record_weights",no_argument,0,0}, /* [flg] Weight each record (not file) by command-line numeric weights, if any */
    {"prm_ints",no_argument,0,0}, /* [flg] Promote integers to floating point in output */
    {"prm_ntg",no_argument,0,0}, /* [flg] Promote integers to floating point in output */
    {"promote_integers",no_argument,0,0}, /* [flg] Promote integers to floating point in output */
    {"prw",no_argument,0,0}, /* [flg] Weight each record (not file) by command-line numeric weights, if any */
    {"share_all",no_argument,0,0}, /* [flg] Open and create (netCDF3) file(s) with unbuffered I/O */
    {"create_share",no_argument,0,0}, /* [flg] Create (netCDF3) file(s) with unbuffered I/O */
    {"open_share",no_argument,0,0}, /* [flg] Open (netCDF3) file(s) with unbuffered I/O */
    {"unbuffered_io",no_argument,0,0}, /* [flg] Open and create (netCDF3) file(s) with unbuffered I/O */
    {"uio",no_argument,0,0}, /* [flg] Open and create (netCDF3) file(s) with unbuffered I/O */
    {"rec_apn",no_argument,0,0}, /* [flg] Append records directly to output file */
    {"record_append",no_argument,0,0}, /* [flg] Append records directly to output file */
    {"wrt_tmp_fl",no_argument,0,0}, /* [flg] Write output to temporary file */
    {"write_tmp_fl",no_argument,0,0}, /* [flg] Write output to temporary file */
    {"no_tmp_fl",no_argument,0,0}, /* [flg] Do not write output to temporary file */
    {"version",no_argument,0,0},
    {"vrs",no_argument,0,0},
    /* Long options with argument, no short option counterpart */
    {"bfr_sz_hnt",required_argument,0,0}, /* [B] Buffer size hint */
    {"buffer_size_hint",required_argument,0,0}, /* [B] Buffer size hint */
    {"cb",required_argument,0,0}, /* [sct] Climatology and bounds information */
    {"clm_bnd",required_argument,0,0}, /* [sct] Climatology and bounds information */
    {"clm_nfo",required_argument,0,0}, /* [sct] Climatology and bounds information */
    {"cnk_byt",required_argument,0,0}, /* [B] Chunk size in bytes */
    {"chunk_byte",required_argument,0,0}, /* [B] Chunk size in bytes */
    {"cnk_csh",required_argument,0,0}, /* [B] Chunk cache size in bytes */
    {"chunk_cache",required_argument,0,0}, /* [B] Chunk cache size in bytes */
    {"cnk_dmn",required_argument,0,0}, /* [nbr] Chunk size */
    {"chunk_dimension",required_argument,0,0}, /* [nbr] Chunk size */
    {"cnk_map",required_argument,0,0}, /* [nbr] Chunking map */
    {"chunk_map",required_argument,0,0}, /* [nbr] Chunking map */
    {"cnk_min",required_argument,0,0}, /* [B] Minimize size of variable to chunk */
    {"chunk_min",required_argument,0,0}, /* [B] Minimize size of variable to chunk */
    {"cnk_plc",required_argument,0,0}, /* [nbr] Chunking policy */
    {"chunk_policy",required_argument,0,0}, /* [nbr] Chunking policy */
    {"cnk_scl",required_argument,0,0}, /* [nbr] Chunk size scalar */
    {"chunk_scalar",required_argument,0,0}, /* [nbr] Chunk size scalar */
    {"fl_fmt",required_argument,0,0},
    {"file_format",required_argument,0,0},
    {"gaa",required_argument,0,0}, /* [sng] Global attribute add */
    {"glb_att_add",required_argument,0,0}, /* [sng] Global attribute add */
    {"hdr_pad",required_argument,0,0},
    {"header_pad",required_argument,0,0},
    {"ilv_srd",required_argument,0,0}, /* [flg] Interleave stride */
    {"interleave_srd",required_argument,0,0}, /* [flg] Interleave stride */
    {"log_lvl",required_argument,0,0}, /* [enm] netCDF library debugging verbosity [0..5] */
    {"log_level",required_argument,0,0}, /* [enm] netCDF library debugging verbosity [0..5] */
    {"ppc",required_argument,0,0}, /* [nbr] Precision-preserving compression, i.e., number of total or decimal significant digits */
    {"precision_preserving_compression",required_argument,0,0}, /* [nbr] Precision-preserving compression, i.e., number of total or decimal significant digits */
    {"quantize",required_argument,0,0}, /* [nbr] Precision-preserving compression, i.e., number of total or decimal significant digits */
    {"nsm_sfx",required_argument,0,0},
    {"ensemble_suffix",required_argument,0,0},
    /* Long options with short counterparts */
    {"3",no_argument,0,'3'},
    {"4",no_argument,0,'4'},
    {"netcdf4",no_argument,0,'4'},
    {"5",no_argument,0,'5'},
    {"64bit_data",no_argument,0,'5'},
    {"cdf5",no_argument,0,'5'},
    {"pnetcdf",no_argument,0,'5'},
    {"64bit_offset",no_argument,0,'6'},
    {"7",no_argument,0,'7'},
    {"append",no_argument,0,'A'},
    {"coords",no_argument,0,'c'},
    {"crd",no_argument,0,'c'},
    {"xtr_ass_var",no_argument,0,'c'},
    {"xcl_ass_var",no_argument,0,'C'},
    {"no_coords",no_argument,0,'C'},
    {"no_crd",no_argument,0,'C'},
    {"dbg_lvl",required_argument,0,'D'},
    {"debug",required_argument,0,'D'},
    {"nco_dbg_lvl",required_argument,0,'D'},
    {"dimension",required_argument,0,'d'},
    {"dmn",required_argument,0,'d'},
    {"fortran",no_argument,0,'F'},
    {"ftn",no_argument,0,'F'},
    {"fl_lst_in",no_argument,0,'H'},
    {"file_list",no_argument,0,'H'},
    {"history",no_argument,0,'h'},
    {"hst",no_argument,0,'h'},
    {"dfl_lvl",required_argument,0,'L'}, /* [enm] Deflate level */
    {"deflate",required_argument,0,'L'}, /* [enm] Deflate level */
    {"local",required_argument,0,'l'},
    {"lcl",required_argument,0,'l'},
    {"no-normalize-by-weight",no_argument,0,'N',},
    {"no_nrm_by_wgt",no_argument,0,'N',},
    {"nintap",required_argument,0,'n'},
    {"overwrite",no_argument,0,'O'},
    {"ovr",no_argument,0,'O'},
    {"output",required_argument,0,'o'},
    {"fl_out",required_argument,0,'o'},
    {"path",required_argument,0,'p'},
    {"pack",required_argument,0,'P'},
    {"retain",no_argument,0,'R'},
    {"rtn",no_argument,0,'R'},
    {"revision",no_argument,0,'r'},
    {"thr_nbr",required_argument,0,'t'},
    {"threads",required_argument,0,'t'},
    {"omp_num_threads",required_argument,0,'t'},
    {"variable",required_argument,0,'v'},
    {"wgt",required_argument,0,'w'},
    {"weight",required_argument,0,'w'},
    {"auxiliary",required_argument,0,'X'},
    {"exclude",no_argument,0,'x'},
    {"xcl",no_argument,0,'x'},
    {"pseudonym",required_argument,0,'Y'},
    {"program",required_argument,0,'Y'},
    {"prg_nm",required_argument,0,'Y'},
    {"math",required_argument,0,'y'},
    {"operation",required_argument,0,'y'},
    {"op_typ",required_argument,0,'y'},
    {0,0,0,0}
  }; /* end opt_lng */
  int opt_idx=0; /* Index of current long option into opt_lng array */
  
#ifdef _LIBINTL_H
  setlocale(LC_ALL,""); /* LC_ALL sets all localization tokens to same value */
  bindtextdomain("nco","/home/zender/share/locale"); /* ${LOCALEDIR} is e.g., /usr/share/locale */
  /* MO files should be in ${LOCALEDIR}/es/LC_MESSAGES */
  textdomain("nco"); /* PACKAGE is name of program or library */
#endif /* not _LIBINTL_H */

  /* Start timer and save command line */ 
  ddra_info.tmr_flg=nco_tmr_srt;
  rcd+=nco_ddra((char *)NULL,(char *)NULL,&ddra_info);
  ddra_info.tmr_flg=nco_tmr_mtd;
  cmd_ln=nco_cmd_ln_sng(argc,argv);

  /* Get program name and set program enum (e.g., nco_prg_id=ncra) */
  nco_prg_nm=nco_prg_prs(argv[0],&nco_prg_id);

#ifdef ENABLE_MPI
  /* MPI Initialization */
  if(False) (void)fprintf(stdout,gettext("%s: WARNING Compiled with MPI\n"),nco_prg_nm);
  MPI_Init(&argc,&argv);
  MPI_Comm_size(mpi_cmm,&prc_nbr);
  MPI_Comm_rank(mpi_cmm,&prc_rnk);
#endif /* !ENABLE_MPI */
  
  /* Parse command line arguments */
  while(1){
    /* getopt_long_only() allows one dash to prefix long options */
    opt=getopt_long(argc,argv,opt_sht_lst,opt_lng,&opt_idx);
    /* NB: access to opt_crr is only valid when long_opt is detected */
    if(opt == EOF) break; /* Parse positional arguments once getopt_long() returns EOF */
    opt_crr=(char *)strdup(opt_lng[opt_idx].name);

    /* Process long options without short option counterparts */
    if(opt == 0){
      if(!strcmp(opt_crr,"baa") || !strcmp(opt_crr,"bit_alg")){
	nco_baa_cnv=(unsigned short int)strtoul(optarg,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
	if(*sng_cnv_rcd) nco_sng_cnv_err(optarg,"strtoul",sng_cnv_rcd);
      } /* endif baa */
      if(!strcmp(opt_crr,"bfr_sz_hnt") || !strcmp(opt_crr,"buffer_size_hint")){
        bfr_sz_hnt=strtoul(optarg,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
        if(*sng_cnv_rcd) nco_sng_cnv_err(optarg,"strtoul",sng_cnv_rcd);
      } /* endif cnk */
      if(!strcmp(opt_crr,"cnk_byt") || !strcmp(opt_crr,"chunk_byte")){
        cnk_sz_byt=strtoul(optarg,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
        if(*sng_cnv_rcd) nco_sng_cnv_err(optarg,"strtoul",sng_cnv_rcd);
      } /* endif cnk_byt */
      if(!strcmp(opt_crr,"cnk_csh") || !strcmp(opt_crr,"chunk_cache")){
        cnk_csh_byt=strtoul(optarg,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
        if(*sng_cnv_rcd) nco_sng_cnv_err(optarg,"strtoul",sng_cnv_rcd);
      } /* endif cnk_csh_byt */
      if(!strcmp(opt_crr,"cnk_min") || !strcmp(opt_crr,"chunk_min")){
        cnk_min_byt=strtoul(optarg,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
        if(*sng_cnv_rcd) nco_sng_cnv_err(optarg,"strtoul",sng_cnv_rcd);
      } /* endif cnk_min */
      if(!strcmp(opt_crr,"cnk_dmn") || !strcmp(opt_crr,"chunk_dimension")){
        /* Copy limit argument for later processing */
        cnk_arg[cnk_nbr]=(char *)strdup(optarg);
        cnk_nbr++;
      } /* endif cnk */
      if(!strcmp(opt_crr,"cnk_scl") || !strcmp(opt_crr,"chunk_scalar")){
        cnk_sz_scl=strtoul(optarg,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
        if(*sng_cnv_rcd) nco_sng_cnv_err(optarg,"strtoul",sng_cnv_rcd);
      } /* endif cnk */
      if(!strcmp(opt_crr,"cnk_map") || !strcmp(opt_crr,"chunk_map")){
        /* Chunking map */
        cnk_map_sng=(char *)strdup(optarg);
        cnk_map=nco_cnk_map_get(cnk_map_sng);
      } /* endif cnk */
      if(!strcmp(opt_crr,"cnk_plc") || !strcmp(opt_crr,"chunk_policy")){
        /* Chunking policy */
        cnk_plc_sng=(char *)strdup(optarg);
        cnk_plc=nco_cnk_plc_get(cnk_plc_sng);
      } /* endif cnk */
      if(!strcmp(opt_crr,"cll_msr") || !strcmp(opt_crr,"cell_measures")) EXTRACT_CLL_MSR=True; /* [flg] Extract cell_measures variables */
      if(!strcmp(opt_crr,"no_cll_msr") || !strcmp(opt_crr,"no_cell_measures")) EXTRACT_CLL_MSR=False; /* [flg] Do not extract cell_measures variables */
      if(!strcmp(opt_crr,"cb") || !strcmp(opt_crr,"clm_bnd") || !strcmp(opt_crr,"clm_nfo") || !strcmp(opt_crr,"climatology_information")){
	clm_nfo_sng=(char *)strdup(optarg);
	flg_cb=True; /* [sct] Process climatology and bounds information */
      } /* !clm_nfo */
      if(!strcmp(opt_crr,"frm_trm") || !strcmp(opt_crr,"formula_terms")) EXTRACT_FRM_TRM=True; /* [flg] Extract formula_terms variables */
      if(!strcmp(opt_crr,"no_frm_trm") || !strcmp(opt_crr,"no_formula_terms")) EXTRACT_FRM_TRM=False; /* [flg] Do not extract formula_terms variables */
      if(!strcmp(opt_crr,"cll_mth") || !strcmp(opt_crr,"cell_methods")) flg_cll_mth=True; /* [flg] Add/modify cell_methods attributes */
      if(!strcmp(opt_crr,"no_cll_mth") || !strcmp(opt_crr,"no_cell_methods")) flg_cll_mth=False; /* [flg] Add/modify cell_methods attributes */
      if(!strcmp(opt_crr,"mmr_cln") || !strcmp(opt_crr,"clean")) flg_mmr_cln=True; /* [flg] Clean memory prior to exit */
      if(!strcmp(opt_crr,"drt") || !strcmp(opt_crr,"mmr_drt") || !strcmp(opt_crr,"dirty")) flg_mmr_cln=False; /* [flg] Clean memory prior to exit */
      if(!strcmp(opt_crr,"fl_fmt") || !strcmp(opt_crr,"file_format")) rcd=nco_create_mode_prs(optarg,&fl_out_fmt);
      if(!strcmp(opt_crr,"dbl") || !strcmp(opt_crr,"rth_dbl")) nco_rth_cnv=nco_rth_flt_dbl; /* [flg] Arithmetic convention: promote float to double */
      if(!strcmp(opt_crr,"flt") || !strcmp(opt_crr,"rth_flt")) nco_rth_cnv=nco_rth_flt_flt; /* [flg] Arithmetic convention: keep single-precision */
      if(!strcmp(opt_crr,"gaa") || !strcmp(opt_crr,"glb_att_add")){
        gaa_arg=(char **)nco_realloc(gaa_arg,(gaa_nbr+1)*sizeof(char *));
        gaa_arg[gaa_nbr++]=(char *)strdup(optarg);
      } /* endif gaa */
      if(!strcmp(opt_crr,"hdf4")) nco_fmt_xtn=nco_fmt_xtn_hdf4; /* [enm] Treat file as HDF4 */
      if(!strcmp(opt_crr,"hdf_upk") || !strcmp(opt_crr,"hdf_unpack")) nco_upk_cnv=nco_upk_HDF_MOD10; /* [flg] HDF unpack convention: unpacked=scale_factor*(packed-add_offset) */
      if(!strcmp(opt_crr,"hdr_pad") || !strcmp(opt_crr,"header_pad")){
        hdr_pad=strtoul(optarg,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
        if(*sng_cnv_rcd) nco_sng_cnv_err(optarg,"strtoul",sng_cnv_rcd);
      } /* endif "hdr_pad" */
      if(!strcmp(opt_crr,"help") || !strcmp(opt_crr,"hlp")){
	(void)nco_usg_prn();
	nco_exit(EXIT_SUCCESS);
      } /* endif "help" */
      if(!strcmp(opt_crr,"hpss_try")) HPSS_TRY=True; /* [flg] Search HPSS for unfound files */
      if(!strcmp(opt_crr,"ilv_srd") || !strcmp(opt_crr,"interleave_stride")){
        ilv_srd=strtol(optarg,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
        if(*sng_cnv_rcd) nco_sng_cnv_err(optarg,"strtol",sng_cnv_rcd);
	if(ilv_srd < 1L){
	  (void)fprintf(stdout,"%s: ERROR Interleave stride argument is %li but must be > 0\n",nco_prg_nm_get(),ilv_srd);
	  nco_exit(EXIT_FAILURE);
	} /* end if */
	FLG_ILV=FLG_MRO=True; /* [flg] Interleave stride */
      } /* !ilv_srd */
      if(!strcmp(opt_crr,"log_lvl") || !strcmp(opt_crr,"log_level")){
	log_lvl=(int)strtol(optarg,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
	if(*sng_cnv_rcd) nco_sng_cnv_err(optarg,"strtol",sng_cnv_rcd);
	nc_set_log_level(log_lvl);
      } /* !log_lvl */
      if(!strcmp(opt_crr,"md5_dgs") || !strcmp(opt_crr,"md5_digest")){
        if(!md5) md5=nco_md5_ini();
        md5->dgs=True;
        if(nco_dbg_lvl >= nco_dbg_std) (void)fprintf(stderr,"%s: INFO Will perform MD5 digests of input and output hyperslabs\n",nco_prg_nm_get());
      } /* endif "md5_dgs" */
      if(!strcmp(opt_crr,"mro") || !strcmp(opt_crr,"multi_record_output")) FLG_MRO=True; /* [flg] Multi-Record Output */
      if(!strcmp(opt_crr,"mso") || !strcmp(opt_crr,"multi_subcycle_output")) FLG_MSO=True; /* [flg] Multi-Subcycle Output */
      if(!strcmp(opt_crr,"msa_usr_rdr") || !strcmp(opt_crr,"msa_user_order")) MSA_USR_RDR=True; /* [flg] Multi-Slab Algorithm returns hyperslabs in user-specified order */
      if(!strcmp(opt_crr,"nsm_fl") || !strcmp(opt_crr,"nsm_file") || !strcmp(opt_crr,"ensemble_file")){
	if(nco_prg_nm) nco_prg_nm=(char *)nco_free(nco_prg_nm);
	nco_prg_nm=nco_prg_prs("ncfe",&nco_prg_id);
      } /* endif nsm_fl */
      if(!strcmp(opt_crr,"nsm_grp") || !strcmp(opt_crr,"nsm_group") || !strcmp(opt_crr,"ensemble_group")){
	if(nco_prg_nm) nco_prg_nm=(char *)nco_free(nco_prg_nm);
	nco_prg_nm=nco_prg_prs("ncge",&nco_prg_id);
      } /* endif nsm_grp */
      if(!strcmp(opt_crr,"nsm_sfx") || !strcmp(opt_crr,"ensemble_suffix")) nsm_sfx=(char *)strdup(optarg);
      if(!strcmp(opt_crr,"per_record_weights") || !strcmp(opt_crr,"prw")) flg_wgt_by_rec_not_by_fl=True; /* [flg] Weight each record (not file) by command-line numeric weights, if any */
      if(!strcmp(opt_crr,"ppc") || !strcmp(opt_crr,"precision_preserving_compression") || !strcmp(opt_crr,"quantize")){
        ppc_arg[ppc_nbr]=(char *)strdup(optarg);
        ppc_nbr++;
      } /* endif "ppc" */
      if(!strcmp(opt_crr,"prm_ints") || !strcmp(opt_crr,"prm_ntg") || !strcmp(opt_crr,"promote_integers")){
	PROMOTE_INTS=True; /* [flg] Promote integers to floating point in output */
	if(nco_prg_id_get() != ncra){
	  (void)fprintf(stdout,"%s: ERROR Option --promote_integers to archive arithmetically processed integer-valued variables as floating point values is only supported with ncra\n",nco_prg_nm_get());
	  nco_exit(EXIT_FAILURE);
	} /* end if */
      } /* !prm_int */
      if(!strcmp(opt_crr,"ram_all") || !strcmp(opt_crr,"create_ram") || !strcmp(opt_crr,"diskless_all")) RAM_CREATE=True; /* [flg] Create (netCDF3) file(s) in RAM */
      if(!strcmp(opt_crr,"ram_all") || !strcmp(opt_crr,"open_ram") || !strcmp(opt_crr,"diskless_all")) RAM_OPEN=True; /* [flg] Open (netCDF3) file(s) in RAM */
      if(!strcmp(opt_crr,"share_all") || !strcmp(opt_crr,"unbuffered_io") || !strcmp(opt_crr,"uio") || !strcmp(opt_crr,"create_share")) SHARE_CREATE=True; /* [flg] Create (netCDF3) file(s) with unbuffered I/O */
      if(!strcmp(opt_crr,"share_all") || !strcmp(opt_crr,"unbuffered_io") || !strcmp(opt_crr,"uio") || !strcmp(opt_crr,"open_share")) SHARE_OPEN=True; /* [flg] Open (netCDF3) file(s) with unbuffered I/O */
      if(!strcmp(opt_crr,"rec_apn") || !strcmp(opt_crr,"record_append")){
        REC_APN=True; /* [flg] Append records directly to output file */
        FORCE_APPEND=True;
      } /* endif "rec_apn" */
      if(!strcmp(opt_crr,"vrs") || !strcmp(opt_crr,"version")){
        (void)nco_vrs_prn(CVS_Id,CVS_Revision);
        nco_exit(EXIT_SUCCESS);
      } /* endif "vrs" */
      if(!strcmp(opt_crr,"wrt_tmp_fl") || !strcmp(opt_crr,"write_tmp_fl")) WRT_TMP_FL=True;
      if(!strcmp(opt_crr,"no_tmp_fl")) WRT_TMP_FL=False;
    } /* opt != 0 */
    /* Process short options */
    switch(opt){
    case 0: /* Long options have already been processed, return */
      break;
    case '3': /* Request netCDF3 output storage format */
      fl_out_fmt=NC_FORMAT_CLASSIC;
      break;
    case '4': /* Request netCDF4 output storage format */
      fl_out_fmt=NC_FORMAT_NETCDF4; 
      break;
    case '5': /* Request netCDF3 64-bit offset+data storage (i.e., pnetCDF) format */
      fl_out_fmt=NC_FORMAT_CDF5;
      break;
    case '6': /* Request netCDF3 64-bit offset output storage format */
      fl_out_fmt=NC_FORMAT_64BIT_OFFSET;
      break;
    case '7': /* Request netCDF4-classic output storage format */
      fl_out_fmt=NC_FORMAT_NETCDF4_CLASSIC;
      break;
    case 'A': /* Toggle FORCE_APPEND */
      FORCE_APPEND=True;
      break;
    case 'C': /* Extract all coordinates associated with extracted variables? */
      EXTRACT_ASSOCIATED_COORDINATES=False;
      break;
    case 'c':
      EXTRACT_ALL_COORDINATES=True;
      break;
    case 'D': /* Debugging level. Default is 0. */
      nco_dbg_lvl=(unsigned short int)strtoul(optarg,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
      if(*sng_cnv_rcd) nco_sng_cnv_err(optarg,"strtoul",sng_cnv_rcd);
      break;
    case 'd': /* Copy limit argument for later processing */
      lmt_arg[lmt_nbr]=(char *)strdup(optarg);
      lmt_nbr++;
      break;
    case 'F': /* Toggle index convention. Default is 0-based arrays (C-style). */
      FORTRAN_IDX_CNV=!FORTRAN_IDX_CNV;
      break;
    case 'G': /* Apply Group Path Editing (GPE) to output group */
      /* NB: GNU getopt() optional argument syntax is ugly (requires "=" sign) so avoid it
      http://stackoverflow.com/questions/1052746/getopt-does-not-parse-optional-arguments-to-parameters */
      gpe=nco_gpe_prs_arg(optarg);
      fl_out_fmt=NC_FORMAT_NETCDF4; 
      break;
    case 'g': /* Copy group argument for later processing */
      /* Replace commas with hashes when within braces (convert back later) */
      optarg_lcl=(char *)strdup(optarg);
      (void)nco_rx_comma2hash(optarg_lcl);
      grp_lst_in=nco_lst_prs_2D(optarg_lcl,",",&grp_lst_in_nbr);
      optarg_lcl=(char *)nco_free(optarg_lcl);
      break;
    case 'H': /* Toggle writing input file list attribute */
      FL_LST_IN_APPEND=!FL_LST_IN_APPEND;
      break;
    case 'h': /* Toggle appending to history global attribute */
      HISTORY_APPEND=!HISTORY_APPEND;
      break;
    case 'L': /* [enm] Deflate level. Default is 0. */
      dfl_lvl=(int)strtol(optarg,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
      if(*sng_cnv_rcd) nco_sng_cnv_err(optarg,"strtol",sng_cnv_rcd);
      break;
    case 'l': /* Local path prefix for files retrieved from remote file system */
      fl_pth_lcl=(char *)strdup(optarg);
      break;
    case 'N':
      NRM_BY_DNM=False;
      NORMALIZE_BY_WEIGHT=False;
      break;
    case 'n': /* NINTAP-style abbreviation of files to average */
      fl_lst_abb=nco_lst_prs_2D(optarg,",",&abb_arg_nbr);
      if(abb_arg_nbr < 1 || abb_arg_nbr > 6){
        (void)fprintf(stdout,gettext("%s: ERROR Incorrect abbreviation for file list\n"),nco_prg_nm_get());
        (void)nco_usg_prn();
        nco_exit(EXIT_FAILURE);
      } /* end if */
      break;
    case 'O': /* Toggle FORCE_OVERWRITE */
      FORCE_OVERWRITE=!FORCE_OVERWRITE;
      break;
    case 'o': /* Name of output file */
      fl_out=(char *)strdup(optarg);
      break;
    case 'p': /* Common file path */
      fl_pth=(char *)strdup(optarg);
      break;
    case 'P': /* Packing policy */
      nco_pck_plc_sng=(char *)strdup(optarg);
      nco_pck_plc=nco_pck_plc_get(nco_pck_plc_sng);
      break;
    case 'R': /* Toggle removal of remotely-retrieved-files. Default is True. */
      RM_RMT_FL_PST_PRC=!RM_RMT_FL_PST_PRC;
      break;
    case 'r': /* Print CVS program information and copyright notice */
      (void)nco_vrs_prn(CVS_Id,CVS_Revision);
      (void)nco_lbr_vrs_prn();
      (void)nco_cpy_prn();
      (void)nco_cnf_prn();
      nco_exit(EXIT_SUCCESS);
      break;
    case 't': /* Thread number */
      thr_nbr=(int)strtol(optarg,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
      if(*sng_cnv_rcd) nco_sng_cnv_err(optarg,"strtol",sng_cnv_rcd);
      break;
    case 'v': /* Variables to extract/exclude */
      /* Replace commas with hashes when within braces (convert back later) */
      optarg_lcl=(char *)strdup(optarg);
      (void)nco_rx_comma2hash(optarg_lcl);
      var_lst_in=nco_lst_prs_2D(optarg_lcl,",",&var_lst_in_nbr);
      optarg_lcl=(char *)nco_free(optarg_lcl);
      xtr_nbr=var_lst_in_nbr;
      break;
    case 'w': /* Per-file and per-record weights */
      if(isalpha(optarg[0]) || optarg[0] == '/'){
	wgt_nm=(char *)strdup(optarg);
      }else{ /* !wgt_nm */
	optarg_lcl=(char *)strdup(optarg);
	wgt_lst_in=nco_lst_prs_2D(optarg_lcl,",",&wgt_nbr);
	optarg_lcl=(char *)nco_free(optarg_lcl);
	wgt_arr=(double *)nco_malloc(wgt_nbr*sizeof(double));
	for(idx=0L;idx<wgt_nbr;idx++){
	  wgt_arr[idx]=strtod(wgt_lst_in[idx],&sng_cnv_rcd);
	  if(*sng_cnv_rcd) nco_sng_cnv_err(wgt_lst_in[idx],"strtod",sng_cnv_rcd);
	  wgt_avg_scl+=wgt_arr[idx];
	} /* end loop over elements */
	if(NORMALIZE_BY_WEIGHT) wgt_avg_scl/=wgt_nbr; else wgt_avg_scl=1.0/wgt_nbr;
	assert(wgt_avg_scl != 0.0);
	if(NORMALIZE_BY_WEIGHT)
	  for(idx=0L;idx<wgt_nbr;idx++)
	    wgt_arr[idx]/=wgt_avg_scl;
	if(nco_dbg_lvl >= nco_dbg_std){
	  (void)fprintf(stderr,"%s: INFO per-file or (with --prw) per-record weights: ",nco_prg_nm_get());
	  for(idx=0L;idx<wgt_nbr;idx++) (void)fprintf(stderr,"wgt_arr[%d]=%g%s",idx,wgt_arr[idx],idx < wgt_nbr-1 ? ", " : "\n");
	} /* !dbg */
      } /* !wgt_nm */
      break;
    case 'X': /* Copy auxiliary coordinate argument for later processing */
      aux_arg[aux_nbr]=(char *)strdup(optarg);
      aux_nbr++;
      MSA_USR_RDR=True; /* [flg] Multi-Slab Algorithm returns hyperslabs in user-specified order */      
      break;
    case 'x': /* Exclude rather than extract variables specified with -v */
      EXCLUDE_INPUT_LIST=True;
      break;
    case 'Y': /* Pseudonym */
      /* Call nco_prg_prs() to reset pseudonym */
      optarg_lcl=(char *)strdup(optarg);
      if(nco_prg_nm) nco_prg_nm=(char *)nco_free(nco_prg_nm);
      nco_prg_nm=nco_prg_prs(optarg_lcl,&nco_prg_id);
      optarg_lcl=(char *)nco_free(optarg_lcl);
      break;
    case 'y': /* Operation type */
      nco_op_typ_sng=(char *)strdup(optarg);
      if(nco_prg_id == ncra || nco_prg_id == ncfe) nco_op_typ=nco_op_typ_get(nco_op_typ_sng);
      break;
    case '?': /* Question mark means unrecognized option, print proper usage then EXIT_FAILURE */
      (void)fprintf(stdout,"%s: ERROR in command-line syntax/options. Missing or unrecognized option. Please reformulate command accordingly.\n",nco_prg_nm_get());
      (void)nco_usg_prn();
      nco_exit(EXIT_FAILURE);
      break;
    case '-': /* Long options are not allowed */
      (void)fprintf(stderr,"%s: ERROR Long options are not available in this build. Use single letter options instead.\n",nco_prg_nm_get());
      nco_exit(EXIT_FAILURE);
      break;
    default: /* Print proper usage */
      (void)fprintf(stdout,"%s ERROR in command-line syntax/options. Please reformulate command accordingly.\n",nco_prg_nm_get());
      (void)nco_usg_prn();
      nco_exit(EXIT_FAILURE);
      break;
    } /* end switch */
    if(opt_crr) opt_crr=(char *)nco_free(opt_crr);
  } /* end while loop */

  /* Set/report global chunk cache */
  rcd+=nco_cnk_csh_ini(cnk_csh_byt);

  /* Process positional arguments and fill-in filenames */
  fl_lst_in=nco_fl_lst_mk(argv,argc,optind,&fl_nbr,&fl_out,&FL_LST_IN_FROM_STDIN,FORCE_OVERWRITE);

  if(flg_wgt_by_rec_not_by_fl && nco_prg_id_get() != ncra){
    (void)fprintf(fp_stdout,"%s: ERROR Illegal invocation of flag --per_record_weights (or --prw)\nHINT: Per-record weighting by command-line numeric weights is only available with ncra\n",nco_prg_nm_get());
    nco_exit(EXIT_FAILURE);
  } /* flg_wgt_by_rec_not_by_fl */
  if(wgt_arr){
    if(wgt_nbr != fl_nbr && !flg_wgt_by_rec_not_by_fl){
      (void)fprintf(fp_stdout,"%s: ERROR User-specified per-file weight array has %d elements but there are %d input files.\nHINT: Specify one weight per input file, or toggle the default behavior by invoking with --per_record_weights (or synonym --prw) which causes command-line weights to be applied per-record not per-file.\n",nco_prg_nm_get(),wgt_nbr,fl_nbr);
      nco_exit(EXIT_FAILURE);
    } /* !wgt_nbr */
  } /* !wgt_arr */
    
  /* Initialize thread information */
  thr_nbr=nco_openmp_ini(thr_nbr);
  in_id_arr=(int *)nco_malloc(thr_nbr*sizeof(int));

  /* Parse filename */
  fl_in=nco_fl_nm_prs(fl_in,0,&fl_nbr,fl_lst_in,abb_arg_nbr,fl_lst_abb,fl_pth);
  /* Make sure file is on local system and is readable or die trying */
  fl_in=nco_fl_mk_lcl(fl_in,fl_pth_lcl,HPSS_TRY,&FL_RTR_RMT_LCN);
  /* Open file using appropriate buffer size hints and verbosity */
  if(RAM_OPEN) md_open=NC_NOWRITE|NC_DISKLESS; else md_open=NC_NOWRITE;
  if(SHARE_OPEN) md_open=md_open|NC_SHARE;
  rcd+=nco_fl_open(fl_in,md_open,&bfr_sz_hnt,&in_id);
  (void)nco_inq_format(in_id,&fl_in_fmt);

  /* Initialize traversal table */ 
  trv_tbl_init(&trv_tbl); 

  /* Construct GTT, Group Traversal Table (groups,variables,dimensions, limits) */
  (void)nco_bld_trv_tbl(in_id,trv_pth,lmt_nbr,lmt_arg,aux_nbr,aux_arg,MSA_USR_RDR,FORTRAN_IDX_CNV,grp_lst_in,grp_lst_in_nbr,var_lst_in,var_lst_in_nbr,EXTRACT_ALL_COORDINATES,GRP_VAR_UNN,False,EXCLUDE_INPUT_LIST,EXTRACT_ASSOCIATED_COORDINATES,EXTRACT_CLL_MSR,EXTRACT_FRM_TRM,nco_pck_plc_nil,&flg_dne,trv_tbl);
  
  /* Were all user-specified dimensions found? */ 
  (void)nco_chk_dmn(lmt_nbr,flg_dne);     

  /* Store ncge ensemble suffix in table */
  if(nco_prg_id == ncge && nsm_sfx) trv_tbl->nsm_sfx=nsm_sfx;

  /* Get number of variables, dimensions, and global attributes in file, file format */
  (void)trv_tbl_inq((int *)NULL,(int *)NULL,(int *)NULL,&nbr_dmn_fl,&dmn_rec_fl,(int *)NULL,(int *)NULL,(int *)NULL,&nbr_var_fl,trv_tbl);

  /* Record handling operators only */
  if(nco_prg_id == ncra || nco_prg_id == ncrcat){

    /* Build record dimensions array */
    (void)nco_bld_rec_dmn(in_id,FORTRAN_IDX_CNV,&lmt_rec,&nbr_rec,trv_tbl);  

    /* Allocate arrays for multi-records cases */
    flg_input_complete=(nco_bool *)nco_malloc(nbr_rec*sizeof(nco_bool));
    idx_rec_out=(long *)nco_malloc(nbr_rec*sizeof(long));
    rec_in_cml=(long *)nco_malloc(nbr_rec*sizeof(long));
    rec_usd_cml=(long *)nco_malloc(nbr_rec*sizeof(long));
    REC_LST_DSR=(nco_bool *)nco_malloc(nbr_rec*sizeof(nco_bool));

    /* Initialize arrays for multi-records cases */
    for(idx_rec=0;idx_rec<nbr_rec;idx_rec++){
      flg_input_complete[idx_rec]=False;
      idx_rec_out[idx_rec]=0L;
      rec_in_cml[idx_rec]=0L;
      rec_usd_cml[idx_rec]=0L;
      REC_LST_DSR[idx_rec]=False;
    } /* Initialize arrays */

  } /* Record handling operators only */

  /* Is this an ARM-format data file? */
  CNV_ARM=nco_cnv_arm_inq(in_id);
  /* NB: nco_cnv_arm_base_time_get() with same nc_id contains OpenMP critical region */
  if(CNV_ARM) base_time_srt=nco_cnv_arm_base_time_get(in_id);

  /* Fill-in variable structure list for all extracted variables */
  var=nco_fll_var_trv(in_id,&xtr_nbr,trv_tbl);

  /* Duplicate to output array */
  var_out=(var_sct **)nco_malloc(xtr_nbr*sizeof(var_sct *));
  for(idx=0;idx<xtr_nbr;idx++){
    var_out[idx]=nco_var_dpl(var[idx]);
    (void)nco_xrf_var(var[idx],var_out[idx]);
    (void)nco_xrf_dmn(var_out[idx]);
  } /* end loop over xtr */

  /* Refresh var_out with dim_out data */
  (void)nco_var_dmn_refresh(var_out,xtr_nbr);

  /* Determine conventions (ARM/CCM/CCSM/CF/MPAS) for treating file */
  cnv=nco_cnv_ini(in_id);

  /* Divide variable lists into lists of fixed variables and variables to be processed */
  (void)nco_var_lst_dvd(var,var_out,xtr_nbr,cnv,True,nco_pck_plc_nil,nco_pck_map_nil,(dmn_sct **)NULL,0,&var_fix,&var_fix_out,&nbr_var_fix,&var_prc,&var_prc_out,&nbr_var_prc,trv_tbl);

  /* Store processed and fixed variables info into GTT */
  (void)nco_var_prc_fix_trv(nbr_var_prc,var_prc,nbr_var_fix,var_fix,trv_tbl);

  /* Make output and input files consanguinous */
  if(fl_out_fmt == NCO_FORMAT_UNDEFINED) fl_out_fmt=fl_in_fmt;

  /* Initialize, decode, and set PPC information */
  if(ppc_nbr > 0) nco_ppc_ini(in_id,&dfl_lvl,fl_out_fmt,ppc_arg,ppc_nbr,trv_tbl);

  /* Verify output file format supports requested actions */
  (void)nco_fl_fmt_vet(fl_out_fmt,cnk_nbr,dfl_lvl);

  /* Open output file */
  fl_out_tmp=nco_fl_out_open(fl_out,&FORCE_APPEND,FORCE_OVERWRITE,fl_out_fmt,&bfr_sz_hnt,RAM_CREATE,RAM_OPEN,SHARE_CREATE,SHARE_OPEN,WRT_TMP_FL,&out_id);

  /* Initialize chunking from user-specified inputs */
  if(fl_out_fmt == NC_FORMAT_NETCDF4 || fl_out_fmt == NC_FORMAT_NETCDF4_CLASSIC) rcd+=nco_cnk_ini(in_id,fl_out,cnk_arg,cnk_nbr,cnk_map,cnk_plc,cnk_csh_byt,cnk_min_byt,cnk_sz_byt,cnk_sz_scl,&cnk);

  /* Keep integers promoted to double-precision on output */
  //  if(PROMOTE_INTS) (void)nco_set_prm_typ_out(xtr_nbr,var,trv_tbl);
  // (void)nco_set_prm_typ_out(xtr_nbr,var,trv_tbl);
  if(nco_prg_id_get() == ncra) (void)nco_set_prm_typ_out(PROMOTE_INTS,xtr_nbr,var,trv_tbl);
  
  /* Define dimensions, extracted groups, variables, and attributes in output file */
  (void)nco_xtr_dfn(in_id,out_id,&cnk,dfl_lvl,gpe,md5,!FORCE_APPEND,!REC_APN,False,nco_pck_plc_nil,(char *)NULL,trv_tbl);

  /* Define ensemble fixed variables (True parameter) */
  if(nco_prg_id_get() == ncge) (void)nco_nsm_dfn_wrt(in_id,out_id,&cnk,dfl_lvl,gpe,True,trv_tbl); 

  /* Catenate time-stamped command line to "history" global attribute */
  if(HISTORY_APPEND) (void)nco_hst_att_cat(out_id,cmd_ln);
  if(HISTORY_APPEND && FORCE_APPEND) (void)nco_prv_att_cat(fl_in,in_id,out_id);
  if(gaa_nbr > 0) (void)nco_glb_att_add(out_id,gaa_arg,gaa_nbr);
  if(HISTORY_APPEND) (void)nco_vrs_att_cat(out_id);
  if(thr_nbr > 1 && HISTORY_APPEND) (void)nco_thr_att_cat(out_id,thr_nbr);

  /* Add input file list global attribute */
  if(FL_LST_IN_APPEND && HISTORY_APPEND && FL_LST_IN_FROM_STDIN) (void)nco_fl_lst_att_cat(out_id,fl_lst_in,fl_nbr);

  /* Turn-off default filling behavior to enhance efficiency */
  (void)nco_set_fill(out_id,NC_NOFILL,&fll_md_old);

  /* Add climatology_bounds attribute to output file (before cell_methods) */
  if(flg_cb && (nco_prg_id == ncra || nco_prg_id == ncrcat || nco_prg_id == ncfe)){
    char bnd_sng[]="bounds"; /* CF-standard time-bounds attribute name */
    char clm_sng[]="climatology"; /* CF-standard climatology bounds attribute name */
    char cln_sng[]="calendar"; /* CF-standard calendar attribute name */
    char unt_sng[]="units"; /* NUG-standard units attribute name */
    long att_sz;
    nc_type att_typ;

    cb=(clm_bnd_sct *)nco_malloc(sizeof(clm_bnd_sct));
    cb->bnd2clm=False; /* [flg] Convert time-bounds to climatology bounds */
    cb->bnd_val=NULL; /* [frc] Time coordinate variable values */
    cb->clm2bnd=False; /* [flg] Convert climatology bounds to time-bounds */
    cb->clm2clm=False; /* [flg] Convert climatology bounds to climatology bounds */
    cb->clm_bnd_id_in=NC_MIN_INT; /* [id] Climatology bounds ID */
    cb->clm_bnd_id_out=NC_MIN_INT; /* [id] Climatology bounds ID */
    cb->clm_bnd_in=False; /* [flg] Climatology bounds appear in input */
    cb->clm_bnd_nm=NULL; /* [sng] Climatology bounds name */
    cb->cln_val=NULL; /* [sng] Bounds calendar value */
    cb->dmn_srt_end[0]=0L;cb->dmn_srt_end[1]=1L;
    cb->dmn_srt_srt[0]=0L;cb->dmn_srt_srt[1]=0L;
    cb->mth_end=NC_MIN_INT; /* [mth] Month at climo end [1..12] format */
    cb->mth_srt=NC_MIN_INT; /* [mth] Month at climo start [1..12] format */
    cb->tm_bnd_id_in=NC_MIN_INT; /* [id] Time-bounds ID */
    cb->tm_bnd_in=False; /* [flg] Time-bounds appear in input */
    cb->tm_bnd_nm=NULL; /* [sng] Time-bounds name */
    cb->tm_crd_id_in=NC_MIN_INT; /* [id] Time coordinate ID */
    cb->tm_crd_nm=NULL; /* [sng] Time coordinate name */
    cb->tm_val=NULL; /* [frc] Time (or climatology) bounds variable values */
    cb->tpd=NC_MIN_INT; /* [nbr] Timesteps per day [0=none, 1, 2, 3, 4, 6, 8,  12, 24, ...]*/
    cb->type=NC_NAT; /* [enm] Time coordinate type */
    cb->unt_val=NULL; /* [sng] Bounds units value */
    cb->yr_end=NC_MIN_INT; /* [yr] Year at climo start */
    cb->yr_srt=NC_MIN_INT; /* [yr] Year at climo start */

    if((rcd=nco_inq_varid_flg(in_id,"time",&cb->tm_crd_id_in)) == NC_NOERR) cb->tm_crd_nm=strdup("time");
    else if((rcd=nco_inq_varid_flg(in_id,"Time",&cb->tm_crd_id_in)) == NC_NOERR) cb->tm_crd_nm=strdup("Time");
    if(cb->tm_crd_id_in != NC_MIN_INT){
      rcd=nco_inq_vartype(in_id,cb->tm_crd_id_in,&cb->type);

      rcd=nco_inq_att_flg(in_id,cb->tm_crd_id_in,clm_sng,&att_typ,&att_sz);
      if(rcd == NC_NOERR && att_typ == NC_CHAR){
	cb->clm_bnd_nm=(char *)nco_malloc((att_sz+1L)*nco_typ_lng(att_typ));
	rcd+=nco_get_att(in_id,cb->tm_crd_id_in,clm_sng,cb->clm_bnd_nm,att_typ);
	/* NUL-terminate attribute before using strstr() */
	cb->clm_bnd_nm[att_sz]='\0';
	cb->clm_bnd_in=True;
      }else{
	cb->clm_bnd_nm=strdup("climatology_bounds");
	rcd=NC_NOERR;
      }	/* !rcd && att_typ */

      rcd=nco_inq_att_flg(in_id,cb->tm_crd_id_in,bnd_sng,&att_typ,&att_sz);
      if(rcd == NC_NOERR && att_typ == NC_CHAR){
	cb->tm_bnd_nm=(char *)nco_malloc((att_sz+1L)*nco_typ_lng(att_typ));
	rcd+=nco_get_att(in_id,cb->tm_crd_id_in,bnd_sng,cb->tm_bnd_nm,att_typ);
	/* NUL-terminate attribute before using strstr() */
	cb->tm_bnd_nm[att_sz]='\0';
	cb->tm_bnd_in=True;
      }else{
	cb->tm_bnd_nm=strdup("time_bnds");
	rcd=NC_NOERR;
      } /* !rcd && att_typ */

      /* Input file must have either (but not both) time bounds or climatology bounds */
      if(cb->tm_bnd_in && cb->clm_bnd_in){
	(void)fprintf(stderr,"%s: WARNING Climatology bounds invoked on time coordinate with both time bounds attribute \"%s\" (value = \"%s\") and climatology bounds attribute \"%s\" (value = \"%s\"). Results would be ambiguous. Turning-off climatology bounds mode.\n",nco_prg_nm_get(),bnd_sng,cb->tm_bnd_nm,clm_sng,cb->clm_bnd_nm);
	flg_cb=False;
	goto skp_cb;
      } /* !(cb->tm_bnd_in && cb->clm_bnd_in) */
      if(!cb->tm_bnd_in && !cb->clm_bnd_in){
	(void)fprintf(stderr,"%s: WARNING Climatology bounds invoked on time coordinate with neither time bounds attribute \"%s\" nor climatology bounds attribute \"%s\". No way to obtain bounding time values. Turning-off climatology bounds mode.\n",nco_prg_nm_get(),bnd_sng,clm_sng);
	flg_cb=False;
	goto skp_cb;
      } /* !cb->tm_bnd_in && !cb->clm_bnd_in */

    }else{ /* !tm_crd_id_in */
      if(nco_dbg_lvl >= nco_dbg_std) (void)fprintf(stderr,"%s: WARNING Climatology bounds invoked on dataset with unknown time coordinate. Turning-off climatology bounds mode.\n",nco_prg_nm_get());
      flg_cb=False;
      rcd=NC_NOERR;
      goto skp_cb; 
    } /* !tm_crd_in */

    if(cb->tm_bnd_in){
      rcd=nco_inq_varid_flg(in_id,cb->tm_bnd_nm,&cb->tm_bnd_id_in); 
      if(cb->tm_bnd_id_in == NC_MIN_INT){
	if(nco_dbg_lvl >= nco_dbg_std) (void)fprintf(stderr,"%s: WARNING Climatology bounds invoked on dataset with missing time bounds variable \"%s\". Turning-off climatology bounds mode.\n",nco_prg_nm_get(),cb->tm_bnd_nm);
	flg_cb=False;
	rcd=NC_NOERR;
	goto skp_cb; 
      } /* !tm_bnd_id_in */
    } /* !tm_bnd_in */

    if(cb->clm_bnd_in){
      rcd=nco_inq_varid_flg(in_id,cb->clm_bnd_nm,&cb->clm_bnd_id_in); 
      if(cb->clm_bnd_id_in == NC_MIN_INT){
	if(nco_dbg_lvl >= nco_dbg_std) (void)fprintf(stderr,"%s: WARNING Climatology bounds invoked on dataset with missing climatology bounds variable \"%s\". Turning-off climatology bounds mode.\n",nco_prg_nm_get(),cb->tm_bnd_nm);
	flg_cb=False;
	rcd=NC_NOERR;
	goto skp_cb; 
      } /* !tm_bnd_id_in */
    } /* !clm_bnd_in */

    rcd=nco_inq_varid_flg(out_id,cb->tm_crd_nm,&cb->tm_crd_id_out); 
    if(rcd != NC_NOERR){
      if(nco_dbg_lvl >= nco_dbg_std) (void)fprintf(stderr,"%s: ERROR Climatology bounds did not find time coordinate in output file\n",nco_prg_nm_get());
      nco_exit(EXIT_FAILURE);
    } /* !tm_crd_id_out */

    /* Populate cb structure with information from clm_nfo_sng */
    if(clm_nfo_sng) rcd=nco_clm_nfo_get(clm_nfo_sng,cb);

    if(cb->tpd == 0){
      /* Monthly mean input */
      if(cb->mth_srt == 1 && cb->mth_end == 12){
	/* Climatological monthly or seasonal means will be reduced to climatological annual means */
	/* DJF seasonal climos in SCD mode present as Y1,Y2,12,2 where
	   DJF seasonal climos in SDD mode present as Y1,Y2,1,12 which is the same as ANN
	   Thus determining clm2clm not clm2bnd for SDD DJF presents special difficulty
	   Hardcode this case as DJF/clm2clm unless fl_nbr = 4 or 12 in which case ANN/clm2bnd */
	if(fl_nbr == 3 && cb->clm_bnd_in) cb->clm2clm=True;
	else if((fl_nbr == 4 || fl_nbr == 12) && cb->clm_bnd_in) cb->clm2bnd=True;
	else{
	  (void)fprintf(stderr,"%s: INFO Combination of months and clm_nfo lead to ambiguous determination of clm2bnd or clm2clm. Turning-off climatology bounds mode.\n",nco_prg_nm_get());
	  flg_cb=False;
	  goto skp_cb;
	} 
      }else{
	/* Climatological monthly or seasonal means will be processed to non-annual means */
	if(cb->tm_bnd_in) cb->bnd2clm=True;
	if(cb->clm_bnd_in) cb->clm2clm=True;
      } /* !cb->mth */
      //    }else if(cb->tpd == 1){
      /* Daily mean input is currently not handled */
      //assert(cb->tpd != 1);
    }else if(cb->tpd >= 1){
      /* Diurnally resolved input */
      if(cb->tm_bnd_in) cb->bnd2clm=True;
      if(cb->clm_bnd_in) cb->clm2clm=True;
    } /* !cb->tpd */

    cb->tm_val=(double *)nco_malloc(max_int(1,cb->tpd)*sizeof(double)); /* [frc] Time coordinate variable values */
    cb->bnd_val=(double *)nco_malloc(max_int(1,cb->tpd)*2*sizeof(double)); /* [frc] Time (or climatology) bounds variable values */

    if(cb->bnd2clm){
      rcd=nco_inq_varid_flg(out_id,cb->tm_bnd_nm,&cb->tm_bnd_id_out); 
      if(rcd != NC_NOERR){
	if(nco_dbg_lvl >= nco_dbg_std) (void)fprintf(stderr,"%s: ERROR Time-bounds variable %s was not copied to output file\n",nco_prg_nm_get(),cb->tm_bnd_nm);
	nco_exit(EXIT_FAILURE);
      } /* !tm_bnd_id_out */
      /* Write climatology bounds to time-bounds then rename */
      cb->clm_bnd_id_out=cb->tm_bnd_id_out;
    } /* !bnd2clm */

    if(cb->clm2clm || cb->clm2bnd){
      rcd=nco_inq_varid_flg(out_id,cb->clm_bnd_nm,&cb->clm_bnd_id_out); 
      if(rcd != NC_NOERR){
	if(nco_dbg_lvl >= nco_dbg_std) (void)fprintf(stderr,"%s: ERROR Climatology bounds variable %s was not copied to output file\n",nco_prg_nm_get(),cb->clm_bnd_nm);
	nco_exit(EXIT_FAILURE);
      } /* !clm_bnd_id_out */
      /* clm2bnd writes time-bounds to climatology bounds then renames, and clm2clm uses tm_bnd_id_out */
      cb->tm_bnd_id_out=cb->clm_bnd_id_out;
    } /* !clm2clm */

    /* Begin attribute manipulation */
    aed_sct aed_mtd;
    char *att_nm;
    char *att_val;

    if(cb->bnd2clm || cb->clm2bnd){
      /* Add new bounds attribute */
      att_nm = (cb->bnd2clm) ? strdup(clm_sng) : strdup(bnd_sng);
      att_val= (cb->bnd2clm) ? strdup(cb->clm_bnd_nm) : strdup(cb->tm_bnd_nm);
      aed_mtd.att_nm=att_nm;
      aed_mtd.var_nm=cb->tm_crd_nm;
      aed_mtd.id=cb->tm_crd_id_out;
      aed_mtd.sz=strlen(att_val);
      aed_mtd.type=NC_CHAR;
      aed_mtd.val.cp=att_val;
      aed_mtd.mode=aed_create;
      (void)nco_aed_prc(out_id,cb->tm_crd_id_out,aed_mtd);
      if(att_nm) att_nm=(char *)nco_free(att_nm);
      if(att_val) att_val=(char *)nco_free(att_val);
      
      /* Delete old bounds attribute */
      att_nm= (cb->bnd2clm) ? strdup(bnd_sng) : strdup(clm_sng);
      aed_mtd.att_nm=att_nm;
      aed_mtd.var_nm=cb->tm_crd_nm;
      aed_mtd.id=cb->tm_crd_id_out;
      aed_mtd.mode=aed_delete;
      (void)nco_aed_prc(out_id,cb->tm_crd_id_out,aed_mtd);
      if(att_nm) att_nm=(char *)nco_free(att_nm);
    } /* !bnd2clm !clm2bnd */

    /* Obtain units string */
    rcd=nco_inq_att_flg(out_id,cb->tm_crd_id_out,unt_sng,&att_typ,&att_sz);
    if(rcd == NC_NOERR && att_typ == NC_CHAR){
      cb->unt_val=(char *)nco_malloc((att_sz+1L)*nco_typ_lng(att_typ));
      rcd+=nco_get_att(out_id,cb->tm_crd_id_out,unt_sng,cb->unt_val,att_typ);
      /* NUL-terminate attribute before using strstr() */
      cb->unt_val[att_sz]='\0';
    } /* !rcd */
      
    /* Copy units attribute from coordinate to new bounds if necessary */
    if(cb->tm_bnd_in) rcd=nco_inq_att_flg(out_id,cb->tm_bnd_id_out,unt_sng,&att_typ,&att_sz);
    if(cb->clm_bnd_in) rcd=nco_inq_att_flg(out_id,cb->clm_bnd_id_out,unt_sng,&att_typ,&att_sz);
    if(rcd != NC_NOERR){
      if(cb->bnd2clm || cb->clm2bnd){
	/* Add units attribute */
	att_nm=strdup(unt_sng);
	att_val=strdup(cb->unt_val);
	aed_mtd.att_nm=att_nm;
	aed_mtd.var_nm=(cb->bnd2clm) ? cb->tm_bnd_nm : cb->clm_bnd_nm;
	aed_mtd.id=(cb->bnd2clm) ? cb->tm_bnd_id_out : cb->clm_bnd_id_out;
	aed_mtd.sz=strlen(att_val);
	aed_mtd.type=NC_CHAR;
	aed_mtd.val.cp=att_val;
	aed_mtd.mode=aed_create;
	if(cb->bnd2clm) (void)nco_aed_prc(out_id,cb->tm_bnd_id_out,aed_mtd); else (void)nco_aed_prc(out_id,cb->clm_bnd_id_out,aed_mtd);
	if(att_nm) att_nm=(char *)nco_free(att_nm);
	if(att_val) att_val=(char *)nco_free(att_val);
      } /* !bnd2clm !clm2bnd */
      rcd=NC_NOERR;
    } /* !rcd */

    /* Obtain calendar string */
    rcd=nco_inq_att_flg(out_id,cb->tm_crd_id_out,cln_sng,&att_typ,&att_sz);
    if(rcd == NC_NOERR && att_typ == NC_CHAR){
      cb->cln_val=(char *)nco_malloc((att_sz+1L)*nco_typ_lng(att_typ));
      rcd+=nco_get_att(out_id,cb->tm_crd_id_out,cln_sng,cb->cln_val,att_typ);
      /* NUL-terminate attribute before using strstr() */
      cb->cln_val[att_sz]='\0';
    } /* !rcd */
      
    /* Copy calendar attribute from coordinate to new bounds if necessary */
    if(cb->tm_bnd_in) rcd=nco_inq_att_flg(out_id,cb->tm_bnd_id_out,cln_sng,&att_typ,&att_sz);
    if(cb->clm_bnd_in) rcd=nco_inq_att_flg(out_id,cb->clm_bnd_id_out,cln_sng,&att_typ,&att_sz);
    if(rcd != NC_NOERR){
      if(cb->bnd2clm || cb->clm2bnd){
	/* Add calendar attribute */
	att_nm=strdup(cln_sng);
	att_val=strdup(cb->cln_val);
	aed_mtd.att_nm=att_nm;
	aed_mtd.var_nm=(cb->bnd2clm) ? cb->tm_bnd_nm : cb->clm_bnd_nm;
	aed_mtd.id=(cb->bnd2clm) ? cb->tm_bnd_id_out : cb->clm_bnd_id_out;
	aed_mtd.sz=strlen(att_val);
	aed_mtd.type=NC_CHAR;
	aed_mtd.val.cp=att_val;
	aed_mtd.mode=aed_create;
	if(cb->bnd2clm) (void)nco_aed_prc(out_id,cb->tm_bnd_id_out,aed_mtd); else (void)nco_aed_prc(out_id,cb->clm_bnd_id_out,aed_mtd);
	if(att_nm) att_nm=(char *)nco_free(att_nm);
	if(att_val) att_val=(char *)nco_free(att_val);
      } /* !bnd2clm !clm2bnd */
      rcd=NC_NOERR;
    } /* !rcd */
    
    /* Combine calendar and units strings with clm_nfo_sng to create climatological time and bounds arrays */
    if(clm_nfo_sng) rcd=nco_clm_nfo_to_tm_bnds(cb->yr_srt,cb->yr_end,cb->mth_srt,cb->mth_end,cb->tpd,cb->unt_val,cb->cln_val,cb->bnd_val,cb->tm_val);
    //assert(rcd != NCO_NOERR);

  } /* !flg_cb */
  
  /* goto skp_cb */
 skp_cb: 
  /* free() any abandoned cb structure now or it will be inadvertently used in nco_cnv_cf_cll_mth_add() */
  if(!flg_cb)
    if(cb) cb=(clm_bnd_sct *)nco_free(cb);

  /* Add cell_methods attributes (before exiting define mode) */
  if(nco_prg_id == ncra || nco_prg_id == ncrcat){
    dmn_sct **dmn=NULL_CEWI;
    int nbr_dmn=nbr_rec;
    dmn=(dmn_sct **)nco_malloc(nbr_dmn*sizeof(dmn_sct *));
    /* Make dimension array from limit records array */
    (void)nco_dmn_lmt(lmt_rec,nbr_dmn,&dmn);
    /* Add cell_methods attributes (pass as dimension argument a records-only array) */
    if(flg_cll_mth) rcd+=nco_cnv_cf_cll_mth_add(out_id,var_prc_out,nbr_var_prc,dmn,nbr_dmn,nco_op_typ,gpe,cb,trv_tbl); 
    if(nbr_dmn > 0) dmn=nco_dmn_lst_free(dmn,nbr_dmn);
  } /* !ncra */

  /* Take output file out of define mode */
  if(hdr_pad == 0UL){
    (void)nco_enddef(out_id);
  }else{
    (void)nco__enddef(out_id,hdr_pad);
    if(nco_dbg_lvl >= nco_dbg_scl) (void)fprintf(stderr,"%s: INFO Padding header with %lu extra bytes\n",nco_prg_nm_get(),(unsigned long)hdr_pad);
  } /* hdr_pad */

  /* Zero start and stride vectors for all output variables */
  (void)nco_var_srd_srt_set(var_out,xtr_nbr);

  /* Copy variable data for non-processed variables */
  (void)nco_cpy_fix_var_trv(in_id,out_id,gpe,trv_tbl); 

  /* Write ensemble fixed variables (False parameter) */
  if(nco_prg_id_get() == ncge) (void)nco_nsm_dfn_wrt(in_id,out_id,&cnk,dfl_lvl,gpe,False,trv_tbl); 

  /* Allocate and, if necesssary, initialize accumulation space for processed variables */
  for(idx=0;idx<nbr_var_prc;idx++){
    /* Record operators only need space for one record, not entire variable */
    if(nco_prg_id == ncra || nco_prg_id == ncrcat) var_prc[idx]->sz=var_prc[idx]->sz_rec=var_prc_out[idx]->sz=var_prc_out[idx]->sz_rec;
    if(nco_prg_id == ncra || nco_prg_id == ncfe || nco_prg_id == ncge){
      /* 20200701: Iff has_mss_val then need wgt_sum to track running sum of time-varying (per-record or per-file) weights applied at each grid point in variables that may have spatio-temporally varying missing values */
      if((wgt_arr || wgt_nm) && var_prc[idx]->has_mss_val) var_prc_out[idx]->wgt_sum=var_prc[idx]->wgt_sum=(double *)nco_calloc(var_prc_out[idx]->sz,sizeof(double)); else var_prc_out[idx]->wgt_sum=NULL;
      var_prc_out[idx]->tally=var_prc[idx]->tally=(long *)nco_calloc(var_prc_out[idx]->sz,sizeof(long));
      var_prc_out[idx]->val.vp=(void *)nco_calloc(var_prc_out[idx]->sz,nco_typ_lng(var_prc_out[idx]->type));
    } /* end if */
  } /* end loop over idx */

  if(wgt_nm && (nco_op_typ == nco_op_avg || nco_op_typ == nco_op_mebs)){
    /* Find weight variable that matches current variable */
    wgt=nco_var_get_wgt_trv(in_id,lmt_nbr,lmt_arg,MSA_USR_RDR,FORTRAN_IDX_CNV,wgt_nm,var_prc[0],trv_tbl);
    /* ncra can handle scalar, 1-D, and degenerate 1-D weights, nces requires scalar weights */
    if(nco_prg_id == ncra) assert(wgt->nbr_dim < 2);
    if(nco_prg_id == ncfe || nco_prg_id == ncge){
      if(wgt->nbr_dim == 1) assert(wgt->sz_rec == 1L); else assert(wgt->nbr_dim == 0);
    } /* !ncfe */
    /* Change wgt from a normal full (scalar or 1-D) variable to a scalar variable 
       This permits us to weight with scalar arithmetic later, rather than broadcasting the weight
       This differs from ncwa wgt treatment (where wgt can be N-D and is always broadcast to match variable)
       20150708: Unsure why nco_var_dpl() calls below generate valgrind invalid read errors */
    /* 20200701: Verified that sz_rec == 1 when wgt is scalar */
    //    if(nco_dbg_lvl >= nco_dbg_std) (void)fprintf(stderr,"%s: DEBUG wgt_nm=%s, wgt->sz_rec=%li\n",nco_prg_nm_get(),wgt_nm,wgt->sz_rec);
    wgt->val.vp=(void *)nco_realloc(wgt->val.vp,wgt->sz_rec*nco_typ_lng(wgt->type));
    wgt->tally=(long *)nco_realloc(wgt->tally,wgt->sz_rec*sizeof(long));
    (void)nco_var_zero(wgt->type,wgt->sz_rec,wgt->val);
    (void)nco_zero_long(wgt->sz_rec,wgt->tally);
    wgt_out=nco_var_dpl(wgt);
    wgt_avg=nco_var_dpl(wgt_out);
  } /* !wgt_nm */

  /* Close first input netCDF file */
  nco_close(in_id);

  /* Timestamp end of metadata setup and disk layout */
  rcd+=nco_ddra((char *)NULL,(char *)NULL,&ddra_info);
  ddra_info.tmr_flg=nco_tmr_rgl;

  /* Loop over input files */
  for(fl_idx=0;fl_idx<fl_nbr;fl_idx++){

    /* Parse filename */
    if(fl_idx != 0) fl_in=nco_fl_nm_prs(fl_in,fl_idx,(int *)NULL,fl_lst_in,abb_arg_nbr,fl_lst_abb,fl_pth);
    if(nco_dbg_lvl >= nco_dbg_fl) (void)fprintf(stderr,gettext("%s: INFO Input file %d is %s"),nco_prg_nm_get(),fl_idx,fl_in);
    /* Make sure file is on local system and is readable or die trying */
    if(fl_idx != 0) fl_in=nco_fl_mk_lcl(fl_in,fl_pth_lcl,HPSS_TRY,&FL_RTR_RMT_LCN);
    if(nco_dbg_lvl >= nco_dbg_fl && FL_RTR_RMT_LCN) (void)fprintf(stderr,gettext(", local file is %s"),fl_in);
    if(nco_dbg_lvl >= nco_dbg_fl) (void)fprintf(stderr,"\n");

    /* Open file once per thread to improve caching */
    for(thr_idx=0;thr_idx<thr_nbr;thr_idx++) rcd+=nco_fl_open(fl_in,md_open,&bfr_sz_hnt,in_id_arr+thr_idx);
    in_id=in_id_arr[0];

    /* Do ncge ensemble refresh */
    if(nco_prg_id == ncge){
      /* Refresh ensembles */
      if(fl_idx > 0) (void)nco_nsm_ncr(in_id,trv_tbl);
      /* Check if ensembles are valid */
      (void)nco_chk_nsm(in_id,fl_idx,trv_tbl); 
    }else{ /* ! ncge */
      /* Variables may have different ID, missing_value, type, in each file */
      for(idx=0;idx<nbr_var_prc;idx++){
        /* Obtain variable GTT object using full variable name */
        trv_sct *trv=trv_tbl_var_nm_fll(var_prc[idx]->nm_fll,trv_tbl);
        /* Obtain group ID */
        (void)nco_inq_grp_full_ncid(in_id,trv->grp_nm_fll,&grp_id);
        (void)nco_var_mtd_refresh(grp_id,var_prc[idx]);
      } /* end loop over variables */
    } /* ! ncge */

    if(wgt_nm && (nco_op_typ == nco_op_avg || nco_op_typ == nco_op_mebs)){
      /* Get weight ID in this file */
      trv_sct *trv=trv_tbl_var_nm_fll(wgt_out->nm_fll,trv_tbl);
      (void)nco_inq_grp_full_ncid(in_id,trv->grp_nm_fll,&grp_id);
      (void)nco_var_mtd_refresh(grp_id,wgt_out);
    } /* !wgt_nm */

    if(FLG_ILV && (nco_prg_id == ncfe || nco_prg_id == ncge)){
      (void)fprintf(fp_stderr,"%s: ERROR Interleaving requested for operator %s\nHINT: Interleaving is only valid for ncra and ncrcat\n",nco_prg_nm_get(),nco_prg_nm_get());
      nco_exit(EXIT_FAILURE);
    } /* ! FLG_ILV */

    if(nco_prg_id == ncra || nco_prg_id == ncrcat){ /* ncfe and ncge jump to else branch */

      /* Loop over all record dimensions in file */
      for(idx_rec=0;idx_rec<nbr_rec;idx_rec++){
        char *fl_udu_sng=NULL_CEWI;
        char ***rgd_arr_bnds_lst=NULL_CEWI;
        char ***rgd_arr_climo_lst=NULL_CEWI;
        int rgd_arr_bnds_nbr=0;
        int rgd_arr_climo_nbr=0;
        int ilv_per_ssc; /* [nbr] Number of interleaves per sub-cycle */

        /* Obtain group ID */
        (void)nco_inq_grp_full_ncid(in_id,lmt_rec[idx_rec]->grp_nm_fll,&grp_id);

        /* Fill record array */
        //if(FLG_ILV){
	//lmt_rec[idx_rec]->flg_ilv=True;
	//lmt_rec[idx_rec]->ilv=ilv_srd;
	//} /* !FLG_ILV */
        (void)nco_lmt_evl(grp_id,lmt_rec[idx_rec],rec_usd_cml[idx_rec],FORTRAN_IDX_CNV);
	/* ILV and MRO may be set in nco_lmt_evl(), and MRO may also be set on command-line */
	FLG_ILV=lmt_rec[idx_rec]->flg_ilv;
        if(FLG_ILV) FLG_MRO=lmt_rec[idx_rec]->flg_mro;
        if(FLG_MRO) lmt_rec[idx_rec]->flg_mro=True;
        if(FLG_MSO) lmt_rec[idx_rec]->flg_mso=True;
        ilv_per_ssc=lmt_rec[idx_rec]->ssc/lmt_rec[idx_rec]->ilv; /* Sub-cycles never cross file boundaries in interleave-compliant files */

        if(lmt_rec[idx_rec]->is_rec_dmn){
          int crd_id;
          if(nco_inq_varid_flg(grp_id,lmt_rec[idx_rec]->nm,&crd_id) == NC_NOERR){
            fl_udu_sng=nco_lmt_get_udu_att(grp_id,crd_id,"units");
            rgd_arr_bnds_lst=nco_lst_cf_att(grp_id,"bounds",&rgd_arr_bnds_nbr);
            rgd_arr_climo_lst=nco_lst_cf_att(grp_id,"climatology",&rgd_arr_climo_nbr);
          } /* !crd_id */
        } /* !is_rec_dmn */

        if(REC_APN){
	  int rec_var_out_id;
          /* Append records directly to output file */
          int rec_dmn_out_id=NCO_REC_DMN_UNDEFINED;
          /* Get group ID using record group full name */
          (void)nco_inq_grp_full_ncid(out_id,lmt_rec[idx_rec]->grp_nm_fll,&grp_out_id);
          /* Get dimension ID (rec_dmn_out_id) of current record from its name */
          (void)nco_inq_dimid(grp_out_id,lmt_rec[idx_rec]->nm,&rec_dmn_out_id);
          /* Get current size of record dimension */
          (void)nco_inq_dimlen(grp_out_id,rec_dmn_out_id,&idx_rec_out[idx_rec]);
	  /* 20181212: Re-base relative to calendar units in output file, not first input file */
	  if(nco_inq_varid_flg(grp_out_id,lmt_rec[idx_rec]->nm,&rec_var_out_id) == NC_NOERR){
	    if(nco_dbg_lvl_get() >= nco_dbg_fl) (void)fprintf(fp_stderr,"%s: DEBUG REC_APN mode changing re-base units string of variable \"%s\" from input units \"%s\" ",nco_prg_nm_get(),lmt_rec[idx_rec]->nm,lmt_rec[idx_rec]->rbs_sng);
	    lmt_rec[idx_rec]->rbs_sng=nco_lmt_get_udu_att(grp_out_id,rec_var_out_id,"units"); 
	    if(nco_dbg_lvl_get() >= nco_dbg_fl) (void)fprintf(fp_stderr,"to output units \"%s\"\n",lmt_rec[idx_rec]->rbs_sng);
	  } /* endif record coordinate exists in output file */
	} /* !REC_APN */

        if(nco_dbg_lvl_get() >= nco_dbg_crr)  (void)fprintf(fp_stdout,"%s: DEBUG record %d id %d name %s rec_dmn_sz %ld units=\"%s\"\n",nco_prg_nm_get(),idx_rec,lmt_rec[idx_rec]->id,lmt_rec[idx_rec]->nm_fll,lmt_rec[idx_rec]->rec_dmn_sz,fl_udu_sng);

        /* NB: nco_cnv_arm_base_time_get() with same nc_id contains OpenMP critical region */
        if(CNV_ARM) base_time_crr=nco_cnv_arm_base_time_get(in_id);

        /* Perform various error-checks on input file */
        if(False) (void)nco_fl_cmp_err_chk();

        /* This file could be superfluous even though desired data may be found in upcoming files */
        if(nco_dbg_lvl >= nco_dbg_std)
          if((lmt_rec[idx_rec]->srt > lmt_rec[idx_rec]->end) && (lmt_rec[idx_rec]->rec_rmn_prv_ssc == 0L))
            (void)fprintf(fp_stdout,"%s: INFO %s (input file %d) is superfluous\n",nco_prg_nm_get(),fl_in,fl_idx);

        rec_dmn_sz=lmt_rec[idx_rec]->rec_dmn_sz;
        rec_rmn_prv_ssc=lmt_rec[idx_rec]->rec_rmn_prv_ssc; /* Local copy may be decremented later */
        idx_rec_crr_in= (rec_rmn_prv_ssc > 0L) ? 0L : lmt_rec[idx_rec]->srt;

	if(FLG_ILV && nco_dbg_lvl >= nco_dbg_fl) (void)fprintf(fp_stdout,"%s: DEBUG After lmt_evl() for fl_idx=%d ILV=%s MRO=%s, MSO=%s, srt=%ld, end=%ld, srd=%ld, ssc=%ld, ilv=%ld, rec_idx=%ld, rec_rmn_prv_ssc=%ld, rec_rmn_prv_ilv=%ld, idx_rec_out=%ld\n",nco_prg_nm_get(),fl_idx,FLG_ILV ? "YES" : "NO",FLG_MRO ? "YES" : "NO",FLG_MSO ? "YES" : "NO",lmt_rec[idx_rec]->srt,lmt_rec[idx_rec]->end,lmt_rec[idx_rec]->srd,lmt_rec[idx_rec]->ssc,lmt_rec[idx_rec]->ilv,idx_rec_crr_in,rec_rmn_prv_ssc,rec_rmn_prv_ilv,idx_rec_out[idx_rec]);

	/* Sub-cycles not allowed to cross file boundaries in interleave mode */
        if(FLG_ILV && lmt_rec[0]->ilv > 1 && rec_rmn_prv_ilv > 0L){
	  (void)fprintf(fp_stdout,"%s: ERROR interleaved sub-cycle crosses file boundary between %s (input file %d) and previous file. Diagnostic counters: rec_rmn_prv_ssc = %ld, rec_rmn_prv_ilv = %ld\n",nco_prg_nm_get(),fl_in,fl_idx,rec_rmn_prv_ssc,rec_rmn_prv_ilv);
	  nco_exit(EXIT_FAILURE);
	} /* !rec_rmn_prv_ilv */

	/* Master while loop over records in current file */
        while(idx_rec_crr_in >= 0L && idx_rec_crr_in < rec_dmn_sz){
          /* Following logic/assumptions built-in to this loop:
	     idx_rec_crr_in points to valid record before loop is entered
	     Loop is never entered if this file has no valid (i.e., desired) records
	     Much conditional logic needed to prescribe group position and next record

	     Index juggling:
	     idx_rec_crr_in: Index of current record in current input file (increments by 1 for ssc then srd-ssc ...)
	     idx_rec_out: Index of record in output file
	     lmt_rec->rec_rmn_prv_ssc: Structure member, at start of this while loop, contains records remaining-to-be-read to complete subcycle group from previous file. Structure member remains constant until next file is read.
	     rec_in_cml: Cumulative number of records, read or not, in all files opened so far. Similar to lmt_rec->rec_in_cml but augmented at end of record loop, rather than prior to record loop.
	     rec_rmn_prv_ssc: Local copy initialized from lmt_rec structure member begins with above, and then is set to and tracks number of records remaining remaining in current group. This means it is decremented from ssc_nbr->0 for each group contained in current file.
	     rec_rmn_prv_ilv: Tracks number of records remaining remaining in current interleaved index. This means it is decremented from ssc/ilv->0 a total of ssc_nbr/ilv_nbr times for each ssc in current file.
	     rec_usd_cml: Cumulative number of input records used (catenated by ncrcat or operated on by ncra)

	     Flag juggling:
	     Groups are the vernacular for a collection of records to output (ncrcat) or reduce (ncra)
	     When introduced in NCO 4.2.1 in 2012, "groups" and sub-cycles (née drn) were synonymous
	     NCO 4.9.4 in 2020 introduced interleaving, which alters the meaning of groups
	     A "group" is now a set of records that ncra reduces/normalizes/outputs as a single record
	     Thus groups and sub-cycles are still synonomous except in ncra in interleave mode
	     In interleave mode, ncra reduces/normalizes/outputs ilv records per ssc (i.e., one output per ssc/ilv records)
	     A non-interleaved group has ssc records, while an interleaved group has ssc/ilv records
	     
	     The relevant group flags REC_FRS_GRP and REC_LST_GRP are now context-sensitive:
	     ncra re-initializes memory at the beginning, and reduces/normalizes/outputs data 
	     at the end, respectively, of each group.
	     In normal (non-interleave) mode, groups are sub-cycles of ssc records
	     In interleave mode, the ilv groups per sub-cycle each contain ssc/ilv records
	     In both normal and interleaved mode, REC_FRS_GRP/REC_LST_GRP are true for 
	     first/last records in a group, respectively, and false otherwise

	     20200731 To disambiguate the meanings of REC_FRS_GRP and REC_LST_GRP we introduce:
	     REC_FRS_SSC and REC_LST_SSC for the first and last records in a sub-cycle
	     REC_FRS_CRR_GRP_OUT and REC_LST_CRR_GRP_OUT for the first and last records in the current (ncra output group (if any)

	     REC_LST_DSR is "sloppy"---it is only set in last input file. If last file(s) is/are superfluous, REC_LST_DSR is never set and final normalization is done outside file and record loops (along with nces normalization). FLG_BFR_NRM indicates these situations and allow us to be "sloppy" in setting REC_LST_DSR.
	     20200719: REC_LST_DSR is not used for FLG_ILV, since complete sub-cycles are assumed to be within a single file, and normalization always occurs at a group ending. */

	  if(FLG_ILV){
	    /* Even intra-ssc strides commence group beginnings */
	    if(rec_rmn_prv_ilv == 0L) REC_FRS_GRP=True; else REC_FRS_GRP=False;
	    //if(FLG_MSO && rec_usd_cml[idx_rec]) REC_FRS_GRP=False;
	  }else{
	    /* Even inter-ssc strides commence group beginnings */
	    if(rec_rmn_prv_ssc == 0L) REC_FRS_GRP=True; else REC_FRS_GRP=False;
	  } /* !FLG_ILV */

          /* Reset interleaved group counter to ssc/ilv records */
          if(FLG_ILV && rec_rmn_prv_ilv == 0L) rec_rmn_prv_ilv=ilv_per_ssc;

          /* Reset sub-cycle counter to ssc records */
          if(rec_rmn_prv_ssc == 0L) rec_rmn_prv_ssc=lmt_rec[idx_rec]->ssc;

          /* Final record triggers normalization regardless of its location within group */
          if(fl_idx == fl_nbr-1 && idx_rec_crr_in == min_int(lmt_rec[idx_rec]->end+lmt_rec[idx_rec]->ssc-1L,rec_dmn_sz-1L)) REC_LST_DSR[idx_rec]=True;

	  /* ncra reduction/normalization/writing code must know last record in current group (LRCG) for both MRO and non-MRO */
	  if(FLG_ILV){
	    if(rec_rmn_prv_ilv == 1L) REC_LST_GRP=True; else REC_LST_GRP=False;
	    //if(FLG_MSO && !REC_LST_DSR[idx_rec]) REC_LST_GRP=False;
	  }else{
	    if(rec_rmn_prv_ssc == 1L) REC_LST_GRP=True; else REC_LST_GRP=False;
	  } /* !FLG_ILV */

          /* Last stride in file has distinct index-augmenting behavior */
          if(idx_rec_crr_in >= lmt_rec[idx_rec]->end) REC_SRD_LST=True; else REC_SRD_LST=False;

          if(FLG_ILV && nco_dbg_lvl >= nco_dbg_fl) (void)fprintf(fp_stdout,"%s: DEBUG rec_idx=%ld, rec_rmn_prv_ssc=%ld, rec_rmn_prv_ilv=%ld, REC_FRS_GRP=%s, REC_LST_GRP=%s, REC_SRD_LST=%s, REC_LST_DSR=%s, idx_rec_out=%ld\n",nco_prg_nm_get(),idx_rec_crr_in,rec_rmn_prv_ssc,rec_rmn_prv_ilv,REC_FRS_GRP ? "YES" : "NO",REC_LST_GRP ? "YES" : "NO",REC_SRD_LST ? "YES" : "NO",REC_LST_DSR[idx_rec] ? "YES" : "NO",idx_rec_out[idx_rec]);

	  /* Retrieve this record of weight variable, if any */
	  if(wgt_nm && (nco_op_typ == nco_op_avg || nco_op_typ == nco_op_mebs))
	    (void)nco_msa_var_get_rec_trv(in_id,wgt_out,lmt_rec[idx_rec]->nm_fll,idx_rec_crr_in,trv_tbl);

          /* Process all variables in current record */
          if(nco_dbg_lvl >= nco_dbg_scl) (void)fprintf(fp_stdout,"%s: INFO Record %ld of %s contributes to output record %ld\n",nco_prg_nm_get(),idx_rec_crr_in,fl_in,idx_rec_out[idx_rec]);

#ifdef _OPENMP
#pragma omp parallel for private(idx,in_id) shared(CNV_ARM,FLG_BFR_NRM,FLG_ILV,FLG_MRO,FLG_MSO,NORMALIZE_BY_WEIGHT,NRM_BY_DNM,REC_FRS_GRP,REC_LST_DSR,base_time_crr,base_time_srt,fl_idx,fl_in,fl_nbr,fl_out,fl_udu_sng,flg_skp1,flg_skp2,gpe,grp_id,grp_out_fll,grp_out_id,idx_rec,idx_rec_crr_in,idx_rec_out,in_id_arr,lmt_rec,md5,nbr_dmn_fl,nbr_rec,nbr_var_prc,nco_dbg_lvl,nco_op_typ,nco_prg_id,out_id,rcd,rec_usd_cml,rgd_arr_bnds_lst,rgd_arr_bnds_nbr,rgd_arr_climo_lst,rgd_arr_climo_nbr,thr_nbr,trv_tbl,var_out_id,var_prc,var_prc_out,var_prc_typ_pre_prm,var_trv,wgt_arr,wgt_avg,wgt_nbr,wgt_nm,wgt_out,wgt_scv)
#endif /* !_OPENMP */
          for(idx=0;idx<nbr_var_prc;idx++){

            /* Skip variable if does not relate to current record */
            flg_skp1=nco_skp_var(var_prc[idx],lmt_rec[idx_rec]->nm_fll,trv_tbl);
            if(flg_skp1) continue;

	    if(thr_nbr > 1) in_id=in_id_arr[omp_get_thread_num()]; else in_id=in_id_arr[0];
            if(nco_dbg_lvl >= nco_dbg_var) rcd+=nco_var_prc_crr_prn(idx,var_prc[idx]->nm_fll);
            if(nco_dbg_lvl >= nco_dbg_var) (void)fflush(fp_stderr);

            /* Obtain variable GTT object using full variable name */
            var_trv=trv_tbl_var_nm_fll(var_prc[idx]->nm_fll,trv_tbl);
            /* Obtain group ID */
            (void)nco_inq_grp_full_ncid(in_id,var_trv->grp_nm_fll,&grp_id);
            /* Edit group name for output */
            grp_out_fll=NULL;
            if(gpe) grp_out_fll=nco_gpe_evl(gpe,var_trv->grp_nm_fll); else grp_out_fll=var_trv->grp_nm_fll;
            /* Obtain output group ID */
            (void)nco_inq_grp_full_ncid(out_id,grp_out_fll,&grp_out_id);
            /* Get variable ID */
            (void)nco_inq_varid(grp_out_id,var_trv->nm,&var_out_id);
            /* Memory management after current extracted group */
            if(gpe && grp_out_fll) grp_out_fll=(char *)nco_free(grp_out_fll);

            /* Store output variable ID */
            var_prc_out[idx]->id=var_out_id;

            /* Retrieve this record of this variable. NB: Updates hyperslab start indices to idx_rec_crr_in */
            (void)nco_msa_var_get_rec_trv(in_id,var_prc[idx],lmt_rec[idx_rec]->nm_fll,idx_rec_crr_in,trv_tbl);

            if(nco_prg_id == ncra) FLG_BFR_NRM=True; /* [flg] Current output buffers need normalization */

            /* Re-base record coordinate and bounds if necessary (e.g., time, time_bnds) */
            /* if(var_prc[idx]->is_crd_var|| nco_is_spc_in_cf_att(grp_id,"bounds",var_prc[idx]->id) || nco_is_spc_in_cf_att(grp_id,"climatology",var_prc[idx]->id)) */

            /* Re-base coordinate variable to units of coordinate in the first input file
	       If record hyperslab indice(s) are double or strings then coordinate variable and limits
	       are (re)-read earlier by nco_lmt_evl() and if units between files are incompatible 
	       then ncra will die in that call and not in nco_cln_clc_dbl_var_dff() below */
            if(var_prc[idx]->is_crd_var){
              nco_bool do_rebase=False;
              if(!strcmp(var_prc[idx]->nm,lmt_rec[idx_rec]->nm) ||
		 nco_rgd_arr_lst_chk(rgd_arr_bnds_lst,rgd_arr_bnds_nbr,lmt_rec[idx_rec]->nm,var_prc[idx]->nm) ||
		 nco_rgd_arr_lst_chk(rgd_arr_climo_lst,rgd_arr_climo_nbr,lmt_rec[idx_rec]->nm,var_prc[idx]->nm))
		do_rebase=True;
              if(do_rebase && fl_udu_sng && lmt_rec[idx_rec]->rbs_sng){
                if(nco_cln_clc_dbl_var_dff(fl_udu_sng,lmt_rec[idx_rec]->rbs_sng,lmt_rec[idx_rec]->cln_typ,(double*)NULL,var_prc[idx]) != NCO_NOERR){
                  (void)fprintf(fp_stderr,"%s: ERROR in nco_cln_clc_dbl_var_dff() when attempting to re-base variable \"%s\" from units \"%s\" to \"%s\"\n",nco_prg_nm_get(),var_prc[idx]->nm,fl_udu_sng,lmt_rec[idx_rec]->rbs_sng);
                  nco_exit(EXIT_FAILURE);
                } /* !nco_cln_clc_dbl_var_dff() */
                //nco_free(fl_udu_sng);
              } /* end !do_rebase */
            } /* !crd_var */
              
            if(nco_prg_id == ncra){
              nco_bool flg_rth_ntl;
              if(!rec_usd_cml[idx_rec] || (FLG_MRO && REC_FRS_GRP)) flg_rth_ntl=True; else flg_rth_ntl=False;
              /* Initialize tally and accumulation arrays when appropriate */
              if(flg_rth_ntl){
                (void)nco_zero_long(var_prc_out[idx]->sz,var_prc_out[idx]->tally);
                (void)nco_var_zero(var_prc_out[idx]->type,var_prc_out[idx]->sz,var_prc_out[idx]->val);
                if(var_prc_out[idx]->wgt_sum) (void)memset(var_prc_out[idx]->wgt_sum,0,var_prc_out[idx]->sz*sizeof(double));
              } /* end if flg_rth_ntl */

              if(var_prc[idx]->type == NC_CHAR || var_prc[idx]->type == NC_STRING){
		/* Do not promote un-averagable types (NC_CHAR, NC_STRING)
		   Stuff their first record into output buffer regardless of nco_op_typ, and ignore later records (rec_usd_cml > 1)
		   Temporarily fixes TODO nco941 */
                if(flg_rth_ntl) nco_opr_drv((long)0L,nco_op_min,var_prc[idx],var_prc_out[idx]);
              }else{
                /* Convert char, short, long, int, and float types to doubles before arithmetic
		   Output variable type is "sticky" so only convert on first record in group */
                if(flg_rth_ntl) var_prc_out[idx]=nco_typ_cnv_rth(var_prc_out[idx],nco_op_typ);
		var_prc_typ_pre_prm=var_prc[idx]->type; /* [enm] Type of variable before promotion */
                var_prc[idx]=nco_var_cnf_typ(var_prc_out[idx]->type,var_prc[idx]);

		/* Weight current record */
		if((wgt_arr || wgt_nm) && (nco_op_typ == nco_op_avg || nco_op_typ == nco_op_mebs) && !var_prc[idx]->is_crd_var){
		  if(wgt_arr){
		    wgt_scv.type=NC_DOUBLE;
		    if(flg_wgt_by_rec_not_by_fl) wgt_scv.val.d=wgt_arr[idx_rec_crr_in % wgt_nbr]; else wgt_scv.val.d=wgt_arr[fl_idx];
		  } /* !wgt_arr */
		  if(wgt_nm){
		    wgt_scv.type=wgt_out->type;
		    wgt_scv.val.d=wgt_out->val.dp[0]; /* Per-record weight */
		  } /* !wgt_nm */
		  if(var_prc[idx]->wgt_sum) var_prc[idx]->wgt_crr=wgt_scv.val.d;
		  nco_scv_cnf_typ(var_prc[idx]->type,&wgt_scv);
		  if(nco_dbg_lvl >= nco_dbg_grp && (wgt_nm || wgt_arr)) (void)fprintf(fp_stdout,"wgt_nm = %s, var_nm = %s, idx = %li, typ = %s, wgt_val = %g, wgt_crr = %g, var_val = %g, ttl = %g, tally = %ld\n",wgt_nm ? wgt_out->nm_fll : "NULL",var_prc[idx]->nm,idx_rec_crr_in,nco_typ_sng(wgt_scv.type),wgt_scv.val.d,var_prc[idx]->wgt_crr,var_prc[idx]->val.dp[0],var_prc_out[idx]->val.dp[0],var_prc_out[idx]->tally[0]);
		  (void)nco_var_scv_mlt(var_prc[idx]->type,var_prc[idx]->sz,var_prc[idx]->has_mss_val,var_prc[idx]->mss_val,var_prc[idx]->val,&wgt_scv);
		  if(wgt_nm && var_prc[idx]->has_mss_val){
		    (void)fprintf(fp_stdout,"%s: ERROR %s -w wgt_nm does not yet work on variables that contain missing values and variable %s contains a missing value attribute. This is TODO nco1124. %s will now quit rather than compute possibly erroneous values. HINT: Restrict the %s -w wgt_nm operation to variables with no missing value attributes.\n",nco_prg_nm_get(),nco_prg_nm_get(),nco_prg_nm_get(),var_prc[idx]->nm,nco_prg_nm_get());
		    nco_exit(EXIT_FAILURE);
		  } /* !wgt_nm */
		  /* Increment running total of wgt_out after its application to last processed variable for this record */
		  if(wgt_nm && (idx == nbr_var_prc-1)){
		    if(flg_rth_ntl) nco_opr_drv((long)0L,nco_op_typ,wgt_out,wgt_avg); else nco_opr_drv((long)1L,nco_op_typ,wgt_out,wgt_avg);
		  } /* !wgt_nm */
		} /* !wgt */
		/* Perform arithmetic operations: avg, min, max, ttl, ... */
		if(flg_rth_ntl) nco_opr_drv((long)0L,nco_op_typ,var_prc[idx],var_prc_out[idx]); else nco_opr_drv((long)1L,nco_op_typ,var_prc[idx],var_prc_out[idx]);
	      } /* end else */ 
            } /* end if ncra */

            /* All processed variables contain record dimension and both ncrcat and ncra write records singly */
            var_prc_out[idx]->srt[rec_dmn_idx]=var_prc_out[idx]->end[rec_dmn_idx]=idx_rec_out[idx_rec];
            var_prc_out[idx]->cnt[rec_dmn_idx]=1L;

            /* Append current record to output file */
            if(nco_prg_id == ncrcat){
              /* Replace this time_offset value with time_offset from initial file base_time */
              if(CNV_ARM && !strcmp(var_prc[idx]->nm,"time_offset")) var_prc[idx]->val.dp[0]+=(base_time_crr-base_time_srt);
	      if(var_trv->ppc != NC_MAX_INT){
		if(var_trv->flg_nsd) (void)nco_ppc_bitmask(var_trv->ppc,var_prc_out[idx]->type,var_prc_out[idx]->sz,var_prc_out[idx]->has_mss_val,var_prc_out[idx]->mss_val,var_prc[idx]->val); else (void)nco_ppc_around(var_trv->ppc,var_prc_out[idx]->type,var_prc_out[idx]->sz,var_prc_out[idx]->has_mss_val,var_prc_out[idx]->mss_val,var_prc[idx]->val);
	      } /* endif ppc */
	      if(nco_is_xcp(var_trv->nm)) nco_xcp_prc(var_trv->nm,var_prc_out[idx]->type,var_prc_out[idx]->sz,(char *)var_prc[idx]->val.vp);
#ifdef _OPENMP
#pragma omp critical
#endif /* _OPENMP */
              if(var_prc_out[idx]->sz_rec > 1L) (void)nco_put_vara(grp_out_id,var_prc_out[idx]->id,var_prc_out[idx]->srt,var_prc_out[idx]->cnt,var_prc[idx]->val.vp,var_prc_out[idx]->type); else (void)nco_put_var1(grp_out_id,var_prc_out[idx]->id,var_prc_out[idx]->srt,var_prc[idx]->val.vp,var_prc_out[idx]->type);
              /* Perform MD5 digest of input and output data if requested */
              if(md5) (void)nco_md5_chk(md5,var_prc_out[idx]->nm,var_prc_out[idx]->sz*nco_typ_lng(var_prc_out[idx]->type),grp_out_id,var_prc_out[idx]->srt,var_prc_out[idx]->cnt,var_prc[idx]->val.vp);
            } /* end if ncrcat */

            /* Warn if record coordinate, if any, is not monotonic (unless interleaved) */
            if(!FLG_ILV && nco_prg_id == ncrcat && var_prc[idx]->is_crd_var) (void)rec_crd_chk(var_prc[idx],fl_in,fl_out,idx_rec_crr_in,idx_rec_out[idx_rec]);
            /* Convert missing_value, if any, back to unpacked or unpromoted type
	       Otherwise missing_value will be double-promoted when next record read in nco_msa_var_get_trv()
	       Do not convert after last record otherwise normalization fails
	       due to wrong missing_value type (needs promoted type, not unpacked type)
	       20140930: This is (too?) confusing and hard-to-follow, a better solution is to add a field mss_val_typ 
	       to var_sct and then separately and explicitly track types of both val and mss_val members. */
            if(var_prc[idx]->has_mss_val && /* If there is a missing value and... */
	       !REC_LST_DSR[idx_rec] && /* ...More records will be read (more calls to nco_msa_var_get_trv()) and... */
	       !(var_prc[idx]->pck_dsk && var_prc_typ_pre_prm != var_prc_out[idx]->type) && /* Exclude conversion on situations like regression test ncra #32 */
	       var_prc[idx]->type != var_prc[idx]->typ_upk) /* ...variable was auto-promoted (e.g., --dbl) then */
	      var_prc[idx]=nco_cnv_mss_val_typ(var_prc[idx],var_prc[idx]->typ_upk); /* Demote missing value */

            /* Free current input buffer */
            var_prc[idx]->val.vp=nco_free(var_prc[idx]->val.vp);
          } /* end (OpenMP parallel for) loop over variables */

          if(nco_prg_id == ncra && ((FLG_MRO && REC_LST_GRP) || REC_LST_DSR[idx_rec])){
            /* Normalize, multiply, etc where necessary: ncra and nces normalization blocks are identical, 
	       except ncra normalizes after every ssc records, while nces normalizes once, after files loop. 
	       20131210: nco_cnv_mss_val_typ() can cause type of var_prc to be out-of-sync with var_prc_out
	       nco_cnv_mss_val_typ() above works correctly for case of packing/unpacking, not for rth_dbl
	       Options:
	       1. Avoid nco_cnv_mss_val_typ() above if rth_dbl is invoked. Keep it for packing.
	       2. In nco_opr_nrm() below, use mss_val from var_prc_out not var_prc
	       Problem is var_prc[idx]->mss_val is typ_upk while var_prc_out is type, so normalization
	       sets missing var_prc_out value to var_prc[idx]->mss_val read as type */

	    /* First, divide accumulated (not yet weighted) values by tally to obtain (non-weighted) time-means */
            if(NRM_BY_DNM) (void)nco_opr_nrm(nco_op_typ,nbr_var_prc,var_prc,var_prc_out,lmt_rec[idx_rec]->nm_fll,trv_tbl);
            FLG_BFR_NRM=False; /* [flg] Current output buffers need normalization */

	    /* Second, multiply unweighted time-mean values by time-mean weights */
	    for(idx=0;idx<nbr_var_prc;idx++){
	      if(var_prc[idx]->wgt_sum){
		// 20201002: fxm Condition this on if(NORMALIZE_BY_WEIGHT) as is done for ncea below?
		//if(NORMALIZE_BY_WEIGHT) (void)nco_var_nrm_wgt(var_prc_out[idx]->type,var_prc_out[idx]->sz,var_prc_out[idx]->has_mss_val,var_prc_out[idx]->mss_val,var_prc_out[idx]->tally,var_prc_out[idx]->wgt_sum,var_prc_out[idx]->val);
		(void)nco_var_nrm_wgt(var_prc_out[idx]->type,var_prc_out[idx]->sz,var_prc_out[idx]->has_mss_val,var_prc_out[idx]->mss_val,var_prc_out[idx]->tally,var_prc_out[idx]->wgt_sum,var_prc_out[idx]->val);
	      } /* !wgt_sum */
	    } /* !idx */
	      
	    if(wgt_nm && (nco_op_typ == nco_op_avg || nco_op_typ == nco_op_mebs)){
	      /* Third, and only if the weight comes from a record variable in the file ... 
		 Compute mean of per-record weight, by normalizing running sum of weight by tally
		 Then normalize all numerical record variables by mean of per-record weight
		 Still ill-defined when MRO is invoked with --wgt 
		 Same logic applies in two locations in this code:
		 1. During SSC normalization inside record loop when REC_LST_DSR is true
		 2. After file loop for nces, and for ncra with superfluous trailing files */
	      wgt_avg_scv.type=NC_DOUBLE;
	      wgt_avg->val.dp[0]/=wgt_out->tally[0]; /* NB: wgt_avg tally is kept in wgt_out */
	      wgt_avg_scv.val.d=wgt_avg->val.dp[0];
	      for(idx=0;idx<nbr_var_prc;idx++){
		if(var_prc_out[idx]->is_crd_var || var_prc[idx]->type == NC_CHAR || var_prc[idx]->type == NC_STRING) continue;
		nco_scv_cnf_typ(var_prc_out[idx]->type,&wgt_avg_scv);
		if(NORMALIZE_BY_WEIGHT) (void)nco_var_scv_dvd(var_prc_out[idx]->type,var_prc_out[idx]->sz,var_prc_out[idx]->has_mss_val,var_prc_out[idx]->mss_val,var_prc_out[idx]->val,&wgt_avg_scv);
	      } /* end loop over var */
	    } /* !wgt_nm */

            /* Copy averages to output file */
            for(idx=0;idx<nbr_var_prc;idx++){

              /* Skip variables that do not contain current record dimension */
              flg_skp2=nco_skp_var(var_prc[idx],lmt_rec[idx_rec]->nm_fll,trv_tbl);
              if(flg_skp2) continue;

              /* Obtain variable GTT object using full variable name */
              var_trv=trv_tbl_var_nm_fll(var_prc_out[idx]->nm_fll,trv_tbl);
              /* Edit group name for output */
              if(gpe) grp_out_fll=nco_gpe_evl(gpe,var_trv->grp_nm_fll); else grp_out_fll=(char *)strdup(var_trv->grp_nm_fll);
              /* Obtain output group ID */
              (void)nco_inq_grp_full_ncid(out_id,grp_out_fll,&grp_out_id);
              /* Memory management after current extracted group */
              if(grp_out_fll) grp_out_fll=(char *)nco_free(grp_out_fll);

	      // 20200831: var_typ_out may differ from typ_upk when PROMOTE_INTS is invoked
	      var_typ_out= PROMOTE_INTS ? var_trv->var_typ_out : var_prc_out[idx]->typ_upk;
	      var_prc_out[idx]=nco_var_cnf_typ(var_typ_out,var_prc_out[idx]);
              /* Packing/Unpacking */
              if(nco_pck_plc == nco_pck_plc_all_new_att) var_prc_out[idx]=nco_put_var_pck(grp_out_id,var_prc_out[idx],nco_pck_plc);
	      if(var_trv->ppc != NC_MAX_INT){
		if(var_trv->flg_nsd) (void)nco_ppc_bitmask(var_trv->ppc,var_prc_out[idx]->type,var_prc_out[idx]->sz,var_prc_out[idx]->has_mss_val,var_prc_out[idx]->mss_val,var_prc_out[idx]->val); else (void)nco_ppc_around(var_trv->ppc,var_prc_out[idx]->type,var_prc_out[idx]->sz,var_prc_out[idx]->has_mss_val,var_prc_out[idx]->mss_val,var_prc_out[idx]->val);
	      } /* endif ppc */
	      if(nco_is_xcp(var_trv->nm)) nco_xcp_prc(var_trv->nm,var_prc_out[idx]->type,var_prc_out[idx]->sz,(char *)var_prc_out[idx]->val.vp);
              if(var_prc_out[idx]->nbr_dim == 0) (void)nco_put_var1(grp_out_id,var_prc_out[idx]->id,var_prc_out[idx]->srt,var_prc_out[idx]->val.vp,var_typ_out); else (void)nco_put_vara(grp_out_id,var_prc_out[idx]->id,var_prc_out[idx]->srt,var_prc_out[idx]->cnt,var_prc_out[idx]->val.vp,var_typ_out);
            } /* end loop over idx */
            idx_rec_out[idx_rec]++; /* [idx] Index of current record in output file (0 is first, ...) */
          } /* end if normalize and write */

          /* Prepare indices and flags for next iteration */
          if(nco_prg_id == ncrcat) idx_rec_out[idx_rec]++; /* [idx] Index of current record in output file (0 is first, ...) */
          rec_usd_cml[idx_rec]++; /* [nbr] Cumulative number of input records used (catenated by ncrcat or operated on by ncra) */
          if(nco_dbg_lvl >= nco_dbg_var) (void)fprintf(fp_stderr,"\n");

          /* Finally, set index for next record or get outta' Dodge */
	  /* Decrement both counters for next record */
	  rec_rmn_prv_ssc--;
	  if(FLG_ILV) rec_rmn_prv_ilv--;
	  if(REC_SRD_LST){
	    /* Next stride or sub-cycle is not within current file */
	    if(FLG_ILV){
	      if(rec_rmn_prv_ssc > 0L){
		/* Next record is within current sub-cycle */
		if(rec_rmn_prv_ilv > 0L){
		  /* Next record is within current interleave so augment record index by interleave stride */
		  idx_rec_crr_in+=lmt_rec[idx_rec]->ilv;
		}else{
		  /* Otherwise set record index to start next interleave */
		  idx_rec_crr_in+=1L-(ilv_per_ssc-1L)*lmt_rec[idx_rec]->ilv;
		} /* !rec_rmn_prv_ilv */
	      }else{ /* !rec_rmn_prv_ssc */
		/* Finished current sub-cycle so break current while loop and jump to next file */
		break;
	      } /* !rec_rmn_prv_ssc */
	    }else{ /* !FLG_ILV */
	      /* Last index depends on whether user-specified end was exact, sloppy, or caused truncation */
	      long end_max_crr;
	      end_max_crr=min_lng(lmt_rec[idx_rec]->idx_end_max_abs-rec_in_cml[idx_rec],min_lng(lmt_rec[idx_rec]->end+lmt_rec[idx_rec]->ssc-1L,rec_dmn_sz-1L));
	      if(rec_rmn_prv_ssc > 0L && idx_rec_crr_in < end_max_crr) idx_rec_crr_in++; else break;
	    } /* !FLG_ILV */
          }else{ /* !REC_SRD_LST */
	    /* Next stride or sub-cycle is within current file */
	    if(FLG_ILV){
	      if(rec_rmn_prv_ssc > 0L){
		/* Next record is within current sub-cycle */
		if(rec_rmn_prv_ilv > 0L){
		  /* Next record is within current interleave so augment record index by interleave stride */
		  idx_rec_crr_in+=lmt_rec[idx_rec]->ilv;
		}else{
		  /* Otherwise set record index to start next interleave */
		  idx_rec_crr_in+=1L-(ilv_per_ssc-1L)*lmt_rec[idx_rec]->ilv;
		} /* !rec_rmn_prv_ilv */
	      }else{
		/* Finished current sub-cycle so hop to next sub-cycle within file */
		idx_rec_crr_in+=lmt_rec[idx_rec]->srd-lmt_rec[idx_rec]->ssc+1L;
	      } /* !rec_rmn_prv_ssc */
	    }else{ /* !FLG_ILV */
	      /* Augment index by one within sub-cycles or hop to next sub-cycle within file  */
	      if(rec_rmn_prv_ssc > 0L) idx_rec_crr_in++; else idx_rec_crr_in+=lmt_rec[idx_rec]->srd-lmt_rec[idx_rec]->ssc+1L;
	    } /* !FLG_ILV */
	  } /* !REC_SRD_LST */

        } /* end idx_rec_crr_in master while loop over records in current file */

        rec_in_cml[idx_rec]+=rec_dmn_sz; /* [nbr] Cumulative number of records in all files opened so far */
        lmt_rec[idx_rec]->rec_rmn_prv_ssc=rec_rmn_prv_ssc;

        if(fl_idx == fl_nbr-1){
          /* Warn if other than number of requested records were read */
          if(lmt_rec[idx_rec]->lmt_typ == lmt_dmn_idx && lmt_rec[idx_rec]->is_usr_spc_min && lmt_rec[idx_rec]->is_usr_spc_max){
            long ssc_grp_nbr_max; /* [nbr] Subcycle groups that start within range */
            long rec_nbr_rqs; /* Number of records user requested */
            long rec_nbr_rqs_max; /* [nbr] Records that would be used by ssc_grp_nbr_max groups */
            long rec_nbr_spn_act; /* [nbr] Records available within user-specified range */
            long rec_nbr_spn_max; /* [nbr] Minimum record number spanned by ssc_grp_nbr_max groups */
            long rec_nbr_trn; /* [nbr] Records truncated in last group */
            long srd_nbr_flr; /* [nbr] Whole strides that fit within specified range */
            /* Number of whole strides that fit within specified range */
            srd_nbr_flr=(lmt_rec[idx_rec]->max_idx-lmt_rec[idx_rec]->min_idx)/lmt_rec[idx_rec]->srd;
            ssc_grp_nbr_max=1L+srd_nbr_flr;
            /* Number of records that would be used by N groups */
            rec_nbr_rqs_max=ssc_grp_nbr_max*lmt_rec[idx_rec]->ssc;
            /* Minimum record number spanned by N groups of size D is N-1 strides, plus D-1 trailing members of last group */
            rec_nbr_spn_max=lmt_rec[idx_rec]->srd*(ssc_grp_nbr_max-1L)+lmt_rec[idx_rec]->ssc;
            /* Actual number of records available within range */
            rec_nbr_spn_act=1L+lmt_rec[idx_rec]->max_idx-lmt_rec[idx_rec]->min_idx;
            /* Number truncated in last group */
            rec_nbr_trn=max_int(rec_nbr_spn_max-rec_nbr_spn_act,0L);
            /* Records requested is maximum minus any truncated in last group */
            rec_nbr_rqs=rec_nbr_rqs_max-rec_nbr_trn;
            if(rec_nbr_rqs != rec_usd_cml[idx_rec]) (void)fprintf(fp_stdout,"%s: WARNING User requested %li records but %s%li were found and used\n",nco_prg_nm_get(),rec_nbr_rqs,(rec_usd_cml[idx_rec] < rec_nbr_rqs) ? "only " : "",rec_usd_cml[idx_rec]);
          } /* end if */
          /* ... and die if no records were read ... */
          if(rec_usd_cml[idx_rec] <= 0){
            (void)fprintf(fp_stdout,"%s: ERROR No records lay within specified hyperslab\n",nco_prg_nm_get());
            nco_exit(EXIT_FAILURE);
          } /* end if */
        } /* end if */

        if(fl_udu_sng) fl_udu_sng=(char*)nco_free(fl_udu_sng);

        nco_rgd_arr_lst_free(rgd_arr_bnds_lst,rgd_arr_bnds_nbr);
        nco_rgd_arr_lst_free(rgd_arr_climo_lst,rgd_arr_climo_nbr);
      } /* end idx_rec loop over different record variables to process */

      if(!clm_nfo_sng && flg_cb && (nco_prg_id == ncra || nco_prg_id == ncrcat)){
	/* Obtain climatology bounds from input file
	   20200822: Deprecate this original method to obtain bounds
	   20160824: Currently dmn_srt_srt and dmn_srt_end indices are 0 and 1, respectively
	   This means values are always/only taken for first record in input file
	   Thus climatology_bounds are only correct for input files with single timestep
	   To fix this requires updating dmn_srt_srt and dmn_srt_end with correct indices
	   Correct indices must account for multiple input records per file and hyperslabbing (e.g., -d time,3,5) */
	int var_id_in;
	double val_dbl;
	var_id_in= cb->tm_bnd_in ? cb->tm_bnd_id_in : cb->clm_bnd_id_in;
	rcd=nco_get_var1(in_id,var_id_in,cb->dmn_srt_srt,&val_dbl,(nc_type)NC_DOUBLE);
	if(fl_idx == 0) cb->tm_val[0]=val_dbl;
	if(val_dbl < cb->bnd_val[0]) cb->bnd_val[0]=val_dbl;
	rcd=nco_get_var1(in_id,var_id_in,cb->dmn_srt_end,&val_dbl,(nc_type)NC_DOUBLE);
	if(val_dbl > cb->bnd_val[1]) cb->bnd_val[1]=val_dbl;
      } /* !flg_cb */

      /* End ncra, ncrcat section */
    }else if(nco_prg_id == ncfe){ /* ncfe */

      if(wgt_nm && (nco_op_typ == nco_op_avg || nco_op_typ == nco_op_mebs))
	(void)nco_msa_var_get_trv(in_id,wgt_out,trv_tbl);

#ifdef _OPENMP
#pragma omp parallel for private(idx,in_id) shared(FLG_BFR_NRM,fl_idx,gpe,grp_id,grp_out_fll,grp_out_id,in_id_arr,nbr_dmn_fl,nbr_var_prc,nco_dbg_lvl,nco_op_typ,out_id,rcd,thr_nbr,trv_tbl,var_out_id,var_prc,var_prc_out,var_trv,wgt_arr,wgt_avg,wgt_nbr,wgt_nm,wgt_out,wgt_scv)
#endif /* !_OPENMP */
      for(idx=0;idx<nbr_var_prc;idx++){ /* Process all variables in current file */

	if(thr_nbr > 1) in_id=in_id_arr[omp_get_thread_num()]; else in_id=in_id_arr[0];
        if(nco_dbg_lvl >= nco_dbg_var) rcd+=nco_var_prc_crr_prn(idx,var_prc[idx]->nm);
        if(nco_dbg_lvl >= nco_dbg_var) (void)fflush(fp_stderr);

        /* Obtain variable GTT object using full variable name */
        var_trv=trv_tbl_var_nm_fll(var_prc[idx]->nm_fll,trv_tbl);
        /* Obtain group ID */
        (void)nco_inq_grp_full_ncid(in_id,var_trv->grp_nm_fll,&grp_id);
        /* Edit group name for output */
        if(gpe) grp_out_fll=nco_gpe_evl(gpe,var_trv->grp_nm_fll); else grp_out_fll=(char *)strdup(var_trv->grp_nm_fll);
        /* Obtain output group ID */
        (void)nco_inq_grp_full_ncid(out_id,grp_out_fll,&grp_out_id);
        /* Memory management after current extracted group */
        if(grp_out_fll) grp_out_fll=(char *)nco_free(grp_out_fll);
        /* Get variable ID */
        (void)nco_inq_varid(grp_out_id,var_trv->nm,&var_out_id);

        /* Store the output variable ID */
        var_prc_out[idx]->id=var_out_id;

        /* Retrieve variable from disk into memory */
        (void)nco_msa_var_get_trv(in_id,var_prc[idx],trv_tbl);

        /* Convert char, short, long, int, and float types to doubles before arithmetic
	   Output variable type is "sticky" so only convert on first record */
        if(fl_idx == 0) var_prc_out[idx]=nco_typ_cnv_rth(var_prc_out[idx],nco_op_typ);
        var_prc[idx]=nco_var_cnf_typ(var_prc_out[idx]->type,var_prc[idx]);

	/* Weight current variable (modified from per-record weighting code above) */
	nco_bool flg_rth_ntl;
	if(fl_idx == 0) flg_rth_ntl=True; else flg_rth_ntl=False;
	if((wgt_arr || wgt_nm) && (nco_op_typ == nco_op_avg || nco_op_typ == nco_op_mebs) && !var_prc[idx]->is_crd_var){
	  if(wgt_arr){
	    wgt_scv.type=NC_DOUBLE;
	    wgt_scv.val.d=wgt_arr[fl_idx]; /* Per-file weight */
	  } /* !wgt_arr */
	  if(wgt_nm){
	    wgt_scv.type=wgt_out->type;
	    wgt_scv.val.d=wgt_out->val.dp[0]; /* Per-file weight */
	  } /* !wgt_nm */
	  if(var_prc[idx]->wgt_sum) var_prc[idx]->wgt_crr=wgt_scv.val.d;
	  nco_scv_cnf_typ(var_prc[idx]->type,&wgt_scv);
	  if(nco_dbg_lvl >= nco_dbg_std && (wgt_nm || wgt_arr)) (void)fprintf(fp_stdout,"wgt_nm = %s, var_nm = %s, fl_idx = %i, typ = %s, wgt_val = %g, wgt_crr = %g, var_val = %g\n",wgt_nm ? wgt_out->nm_fll : "NULL",var_prc[idx]->nm,fl_idx,nco_typ_sng(wgt_scv.type),wgt_scv.val.d,var_prc[idx]->wgt_crr,var_prc[idx]->val.dp[0]);
	  (void)nco_var_scv_mlt(var_prc[idx]->type,var_prc[idx]->sz,var_prc[idx]->has_mss_val,var_prc[idx]->mss_val,var_prc[idx]->val,&wgt_scv);
	  if(wgt_nm && var_prc[idx]->has_mss_val){
	    (void)fprintf(fp_stdout,"%s: ERROR %s -w wgt_nm does not yet work on variables that contain missing values and variable %s contains a missing value attribute. This is TODO nco1124. %s will now quit rather than compute possibly erroneous values. HINT: Restrict the %s -w wgt_nm operation to variables with no missing value attributes.\n",nco_prg_nm_get(),nco_prg_nm_get(),nco_prg_nm_get(),var_prc[idx]->nm,nco_prg_nm_get());
	    nco_exit(EXIT_FAILURE);
	  } /* !wgt_nm */
	  /* Increment running total of wgt_out after its application to last processed variable for this record */
	  if(wgt_nm && (idx == nbr_var_prc-1)){
	    if(flg_rth_ntl) nco_opr_drv((long)0L,nco_op_typ,wgt_out,wgt_avg); else nco_opr_drv((long)1L,nco_op_typ,wgt_out,wgt_avg);
	  } /* !wgt_nm */
	} /* !wgt */
	/* Perform arithmetic operations: avg, min, max, ttl, ... */ /* Note: fl_idx not rec_usd_cml! */
        nco_opr_drv(fl_idx,nco_op_typ,var_prc[idx],var_prc_out[idx]);
        FLG_BFR_NRM=True; /* [flg] Current output buffers need normalization */

        /* Free current input buffer */
        var_prc[idx]->val.vp=nco_free(var_prc[idx]->val.vp);
      } /* end (OpenMP parallel for) loop over idx */
      /* End ncfe section */
    }else if(nco_prg_id == ncge){ /* ncge */

      trv_tbl_sct *trv_tbl1;    /* [lst] Traversal table (needed for multi-file cases)  */

      /* Initialize traversal table */ 
      trv_tbl_init(&trv_tbl1); 

      /* Construct GTT using current file ID */
      (void)nco_bld_trv_tbl(in_id,trv_pth,lmt_nbr,lmt_arg,aux_nbr,aux_arg,MSA_USR_RDR,FORTRAN_IDX_CNV,grp_lst_in,grp_lst_in_nbr,var_lst_in,var_lst_in_nbr,EXTRACT_ALL_COORDINATES,GRP_VAR_UNN,False,EXCLUDE_INPUT_LIST,EXTRACT_ASSOCIATED_COORDINATES,EXTRACT_CLL_MSR,EXTRACT_FRM_TRM,nco_pck_plc_nil,&flg_dne,trv_tbl1);

      /* Were all user-specified dimensions found? */ 
      (void)nco_chk_dmn(lmt_nbr,flg_dne);  

      /* Loop over ensembles in current file */
      for(int idx_nsm=0;idx_nsm<trv_tbl->nsm_nbr;idx_nsm++){ 

        if(nco_dbg_lvl > nco_dbg_std) (void)fprintf(stdout,"%s: ensemble %d: %s\n",nco_prg_nm_get(),idx_nsm,trv_tbl->nsm[idx_nsm].grp_nm_fll_prn);

        int mbr_srt=trv_tbl->nsm[idx_nsm].mbr_srt;
        int mbr_end=trv_tbl->nsm[idx_nsm].mbr_end;

        /* Loop over ensemble members in current file (use start and end members, multi-file cases) */
        for(int idx_mbr=mbr_srt;idx_mbr<mbr_end;idx_mbr++){

          /* Loop over all variables */
          for(int idx_prc=0;idx_prc<nbr_var_prc;idx_prc++){ 

            /* Obtain variable GTT object for member variable in ensemble */
            var_trv=trv_tbl_var_nm_fll(var_prc[idx_prc]->nm_fll,trv_tbl);
            assert(var_trv);

            /* Skip if from different ensembles */
            if(strcmp(var_trv->nsm_nm,trv_tbl->nsm[idx_nsm].grp_nm_fll_prn)) continue;

            /* Build new variable name */
            char *grp_nm_fll=trv_tbl->nsm[idx_nsm].mbr[idx_mbr].mbr_nm_fll; 
            char *var_nm_fll=nco_bld_nm_fll(grp_nm_fll,var_prc[idx_prc]->nm);;
            char *nm_fll=strdup(var_prc[idx_prc]->nm_fll);

            var_prc[idx_prc]->nm_fll=(char *)nco_free(var_prc[idx_prc]->nm_fll);
            var_prc[idx_prc]->nm_fll=nco_bld_nm_fll(grp_nm_fll,var_prc[idx_prc]->nm);
            if(nco_dbg_lvl > nco_dbg_std) (void)fprintf(fp_stdout,"%s:\t variable <%s>\n",nco_prg_nm_get(),var_prc[idx_prc]->nm_fll);

            /* Obtain group ID */
            (void)nco_inq_grp_full_ncid(in_id,grp_nm_fll,&grp_id);
            (void)nco_var_mtd_refresh(grp_id,var_prc[idx_prc]);

            /* Retrieve variable from disk into memory. NB: Using table in file loop */
            (void)nco_msa_var_get_trv(in_id,var_prc[idx_prc],trv_tbl1);

            /* Convert char, short, long, int, and float types to doubles before arithmetic
	       Output variable type is "sticky" so only convert on first member */
            if(fl_idx == 0 && idx_mbr == 0) var_prc_out[idx_prc]=nco_typ_cnv_rth(var_prc_out[idx_prc],nco_op_typ);
            var_prc[idx_prc]=nco_var_cnf_typ(var_prc_out[idx_prc]->type,var_prc[idx_prc]);
            /* Perform arithmetic operations: avg, min, max, ttl, ... */
            nco_opr_drv(fl_idx+idx_mbr,nco_op_typ,var_prc[idx_prc],var_prc_out[idx_prc]);
            FLG_BFR_NRM=True; /* [flg] Current output buffers need normalization */

            /* Put old name back */
            var_prc[idx_prc]->nm_fll=(char *)nco_free(var_prc[idx_prc]->nm_fll);
            var_prc[idx_prc]->nm_fll=strdup(nm_fll);

            /* Free current input buffer */
            var_prc[idx_prc]->val.vp=nco_free(var_prc[idx_prc]->val.vp);

            /* Free built variable name */
            var_nm_fll=(char *)nco_free(var_nm_fll);
            nm_fll=(char *)nco_free(nm_fll);

          } /* end loop over var_prc */
        } /* end loop over mbr */

      } /* !idx_mbr */

      (void)trv_tbl_free(trv_tbl1);

    } /* End ncge section */

    /* For ncge, save helpful metadata for later handling by ncbo */
    if(nco_prg_id == ncge && fl_idx == 0) (void)nco_nsm_wrt_att(in_id,out_id,gpe,trv_tbl);   

    if(nco_dbg_lvl >= nco_dbg_scl) (void)fprintf(fp_stderr,"\n");

    /* Close input netCDF file */
    for(thr_idx=0;thr_idx<thr_nbr;thr_idx++) nco_close(in_id_arr[thr_idx]);

    /* Dispose local copy of file */
    if(FL_RTR_RMT_LCN && RM_RMT_FL_PST_PRC) (void)nco_fl_rm(fl_in);

    /* Are all our data tanks already full? */
    if(nco_prg_id == ncra || nco_prg_id == ncrcat){
      for(idx_rec=0;idx_rec<nbr_rec;idx_rec++){
	if(!flg_input_complete[idx_rec]){
	  if((flg_input_complete[idx_rec]=lmt_rec[idx_rec]->flg_input_complete)){
	    /* NB: TODO nco1066 move input_complete break to precede record loop but remember to close open filehandles */
	    /* 20131209: Rewritten so file skipped only once all record dimensions have flg_input_complete
	       Warnings about superfluous files printed only once per dimension
	       fxm: use flg_input_complete[idx_rec] to skip completed entries in main record dimension loop above */
	    if(nco_dbg_lvl >= nco_dbg_std) (void)fprintf(fp_stderr,"%s: INFO All requested records for record dimension #%d (%s) were found within the first %d input file%s, next file was opened then skipped, and remaining %d input file%s need not be opened\n",nco_prg_nm_get(),idx_rec,lmt_rec[idx_rec]->nm_fll,fl_idx,(fl_idx == 1) ? "" : "s",fl_nbr-fl_idx-1,(fl_nbr-fl_idx-1 == 1) ? "" : "s");
	    flg_input_complete_nbr++;
	  } /* endif superfluous */
	} /* endif not already known to be complete */
      } /* end loop over record dimensions */
      /* Once all record dimensions are complete, break-out of file loop */
      if(flg_input_complete_nbr == nbr_rec) break;
    } /* endif ncra || ncrcat */

  } /* end loop over fl_idx */

  if(FLG_ILV && lmt_rec[0]->ilv > 1 && rec_rmn_prv_ilv != 0) (void)fprintf(stderr,"%s: WARNING input ended while last interleaved sub-cycle was incomplete. This means the interleaved dimension in the last sub-cycle will contain a non-uniform number of records contributing to different indices. Consider re-defining hyperslab or input data length to ensure output all based on complete sub-cycles. Diagnostics: full sub-cycle length = %ld, stride between first elements of consecutive sub-cycles = %ld, records needed for completion of last sub-cycle and of last interleaved index are, respectively, rec_rmn_prv_ssc = %ld, and rec_rmn_prv_ilv = %ld\n",nco_prg_nm_get(),lmt_rec[0]->ssc,lmt_rec[0]->srd,rec_rmn_prv_ssc,rec_rmn_prv_ilv);

  /* Subcycle argument warning */
  if(nco_prg_id == ncra || nco_prg_id == ncrcat){ /* fxm: Remove this or make DBG when crd_val SSC/MRO is predictable? */
    for(idx_rec=0;idx_rec<nbr_rec;idx_rec++){
      /* Check subcycle for each record */
      if(lmt_rec[idx_rec]->ssc > 1L && (lmt_rec[idx_rec]->lmt_typ == lmt_crd_val || lmt_rec[idx_rec]->lmt_typ == lmt_udu_sng)){
        if(nco_dbg_lvl >= nco_dbg_scl) (void)fprintf(stderr,"\n%s: INFO Subcycle argument SSC used in hyperslab specification for %s which will be determined based on coordinate values rather than dimension indices. The behavior of the subcycle hyperslab argument is ambiguous for coordinate-based hyperslabs---it could mean select the first SSC elements that are within the min and max coordinate values beginning with each strided point, or it could mean always select the first _consecutive_ SSC elements beginning with each strided point (regardless of their values relative to min and max). For such hyperslabs, NCO adopts the latter definition and always selects the group of SSC records beginning with each strided point. Strided points are themselves guaranteed to be within the min and max coordinates, though the subsequent members of each group are not. This is only the case when the record coordinate is not monotonic. The record coordinate is usually monotonic, so unpleasant surprises are only expected in corner cases unlikely to affect the majority of users.\n",nco_prg_nm_get(),lmt_rec[idx_rec]->nm);
      } /* Check subcycle for each record */
    } /* !idx_rec */
  } /* Subcycle argument warning */

  /* Normalize, multiply, etc where necessary: ncra and nces normalization blocks are identical, 
     except ncra normalizes after every SSC records, while nces normalizes once, after all files.
     Occassionally last input file(s) is/are superfluous so REC_LST_DSR never set
     In such cases FLG_BFR_NRM is still true, indicating ncra still needs normalization
     FLG_BFR_NRM is always true here for ncfe and ncge */
  if(FLG_BFR_NRM){
    /* First, divide accumulated (not yet weighted) values by tally to obtain (non-weighted) time-means */
    if(NRM_BY_DNM) (void)nco_opr_nrm(nco_op_typ,nbr_var_prc,var_prc,var_prc_out,(char *)NULL,(trv_tbl_sct *)NULL);
    
    /* Second, multiply unweighted time-mean values by time-mean weights */
    for(idx=0;idx<nbr_var_prc;idx++){
      if(var_prc[idx]->wgt_sum){
	//(void)nco_var_nrm_wgt(var_prc_out[idx]->type,var_prc_out[idx]->sz,var_prc_out[idx]->has_mss_val,var_prc_out[idx]->mss_val,var_prc_out[idx]->tally,var_prc_out[idx]->wgt_sum,var_prc_out[idx]->val);
	if(NORMALIZE_BY_WEIGHT) (void)nco_var_nrm_wgt(var_prc_out[idx]->type,var_prc_out[idx]->sz,var_prc_out[idx]->has_mss_val,var_prc_out[idx]->mss_val,var_prc_out[idx]->tally,var_prc_out[idx]->wgt_sum,var_prc_out[idx]->val); // original code
      } /* !wgt_sum */
    } /* !idx */

    if(wgt_nm && (nco_op_typ == nco_op_avg || nco_op_typ == nco_op_mebs)){
      /* Third, and only if the weight comes from a record variable in the file ... 
	 Compute mean of per-record weight, by normalizing running sum of weight by tally
	 Then normalize all numerical record variables by mean of per-record weight
	 Still ill-defined when MRO is invoked with --wgt 
	 Same logic applies in two locations in this code:
	 1. During SSC normalization inside record loop when REC_LST_DSR is true
	 2. After file loop for nces, and for ncra with superfluous trailing files */
      wgt_avg_scv.type=NC_DOUBLE;
      wgt_avg->val.dp[0]/=wgt_out->tally[0]; /* NB: wgt_avg tally is kept in wgt_out */
      wgt_avg_scv.val.d=wgt_avg->val.dp[0];
      for(idx=0;idx<nbr_var_prc;idx++){
	if(var_prc_out[idx]->is_crd_var || var_prc[idx]->type == NC_CHAR || var_prc[idx]->type == NC_STRING) continue;
	nco_scv_cnf_typ(var_prc_out[idx]->type,&wgt_avg_scv);
	if(NORMALIZE_BY_WEIGHT) (void)nco_var_scv_dvd(var_prc_out[idx]->type,var_prc_out[idx]->sz,var_prc_out[idx]->has_mss_val,var_prc_out[idx]->mss_val,var_prc_out[idx]->val,&wgt_avg_scv);
      } /* end loop over var */
    } /* !wgt_nm */
  } /* !FLG_BFR_NRM */

  /* Manually fix YYMMDD date which was mangled by averaging */
  if(cnv->CCM_CCSM_CF && nco_prg_id == ncra) (void)nco_cnv_ccm_ccsm_cf_date(grp_out_id,var_out,xtr_nbr);

  /* Add time variable to output file
     NB: nco_cnv_arm_time_install() contains OpenMP critical region */
  if(CNV_ARM && nco_prg_id == ncrcat) (void)nco_cnv_arm_time_install(grp_out_id,base_time_srt,dfl_lvl);

  /* Copy averages to output file for ncfe and ncge always and for ncra when trailing file(s) was/were superfluous */
  if(FLG_BFR_NRM){
    for(idx=0;idx<nbr_var_prc;idx++){

      /* Obtain variable GTT object using full variable name */
      var_trv=trv_tbl_var_nm_fll(var_prc_out[idx]->nm_fll,trv_tbl);

      /* For ncge, group to save is ensemble parent group */
      if(nco_prg_id == ncge){

        /* Check if suffix needed. Appends to default name */
        if(trv_tbl->nsm_sfx){
          /* Define (append) then use and forget new name */
          char *nm_fll_sfx=nco_bld_nsm_sfx(var_trv->grp_nm_fll_prn,trv_tbl);
          /* Use new name */
          if(gpe) grp_out_fll=nco_gpe_evl(gpe,nm_fll_sfx); else grp_out_fll=(char *)strdup(nm_fll_sfx);
          nm_fll_sfx=(char *)nco_free(nm_fll_sfx);
        }else{ /* Non suffix case */
          if(gpe) grp_out_fll=nco_gpe_evl(gpe,var_trv->nsm_nm); else grp_out_fll=(char *)strdup(var_trv->nsm_nm);
        } /* !trv_tbl->nsm_sfx */

      }else if(nco_prg_id == ncfe){
        /* Edit group name for output */
        if(gpe) grp_out_fll=nco_gpe_evl(gpe,var_trv->grp_nm_fll); else grp_out_fll=(char *)strdup(var_trv->grp_nm_fll);
      } /* end else */

      /* Obtain output group ID */
      (void)nco_inq_grp_full_ncid(out_id,grp_out_fll,&grp_out_id);
      /* Get output variable ID */
      (void)nco_inq_varid(grp_out_id,var_prc_out[idx]->nm,&var_out_id);   
      /* Store the output variable ID */
      var_prc_out[idx]->id=var_out_id;

      var_prc_out[idx]=nco_var_cnf_typ(var_prc_out[idx]->typ_upk,var_prc_out[idx]);
      /* Packing/Unpacking */
      if(nco_pck_plc == nco_pck_plc_all_new_att) var_prc_out[idx]=nco_put_var_pck(grp_out_id,var_prc_out[idx],nco_pck_plc);
      if(var_trv->ppc != NC_MAX_INT){
	if(var_trv->flg_nsd) (void)nco_ppc_bitmask(var_trv->ppc,var_prc_out[idx]->type,var_prc_out[idx]->sz,var_prc_out[idx]->has_mss_val,var_prc_out[idx]->mss_val,var_prc_out[idx]->val); else (void)nco_ppc_around(var_trv->ppc,var_prc_out[idx]->type,var_prc_out[idx]->sz,var_prc_out[idx]->has_mss_val,var_prc_out[idx]->mss_val,var_prc_out[idx]->val);
      } /* endif ppc */
      if(var_prc_out[idx]->nbr_dim == 0) (void)nco_put_var1(grp_out_id,var_prc_out[idx]->id,var_prc_out[idx]->srt,var_prc_out[idx]->val.vp,var_prc_out[idx]->type); else (void)nco_put_vara(grp_out_id,var_prc_out[idx]->id,var_prc_out[idx]->srt,var_prc_out[idx]->cnt,var_prc_out[idx]->val.vp,var_prc_out[idx]->type);

    } /* end loop over idx */
  } /* end if ncfe and ncge */

  /* Free averaging, tally, and weight buffers */
  if(nco_prg_id == ncra || nco_prg_id == ncfe || nco_prg_id == ncge){
    for(idx=0;idx<nbr_var_prc;idx++){
      if((wgt_arr || wgt_nm) && var_prc[idx]->has_mss_val) var_prc_out[idx]->wgt_sum=var_prc[idx]->wgt_sum=(double *)nco_free(var_prc[idx]->wgt_sum);
      var_prc_out[idx]->tally=var_prc[idx]->tally=(long *)nco_free(var_prc[idx]->tally);
      var_prc_out[idx]->val.vp=nco_free(var_prc_out[idx]->val.vp);
    } /* end loop over idx */
  } /* endif ncra || nces */

  if(flg_cb && (nco_prg_id == ncra || nco_prg_id == ncrcat || nco_prg_id == ncfe)){
    rcd=nco_put_var(out_id,cb->tm_crd_id_out,cb->tm_val,(nc_type)NC_DOUBLE);
    rcd=nco_put_var(out_id,cb->clm_bnd_id_out,cb->bnd_val,(nc_type)NC_DOUBLE);
  } /* !flg_cb */

  if(flg_cb && (cb->bnd2clm || cb->clm2bnd)){
    /* Rename time-bounds as climatology bounds, or visa-versa
       Otherwise wrong bounds will remain orphaned in output file
       Also, this ensures same dimensions are used
       Rename at end of procedure so that traversal table does not get out-of-sync
       Avoiding renaming would mean creating the new and deleting the old bounds variable 
       That would entail significant modifications to traversal table logic
       Renaming seems simpler and less error prone */
    rcd+=nco_redef(out_id);
    if(cb->bnd2clm) rcd+=nco_rename_var(out_id,cb->tm_bnd_id_out,cb->clm_bnd_nm);
    if(cb->clm2bnd) rcd+=nco_rename_var(out_id,cb->clm_bnd_id_out,cb->tm_bnd_nm);
    rcd+=nco_enddef(out_id);
  } /* !flg_cb */

  /* Close output file and move it from temporary to permanent location */
  (void)nco_fl_out_cls(fl_out,fl_out_tmp,out_id);

  /* Clean memory unless dirty memory allowed */
  if(flg_mmr_cln){
    /* NCO-generic clean-up */
    /* Free individual strings/arrays */
    //if(nco_dbg_lvl_get() >= nco_dbg_quiet) (void)fprintf(stdout,"%s: free quark3\n",nco_prg_nm_get());
    if(cmd_ln) cmd_ln=(char *)nco_free(cmd_ln);
    if(clm_nfo_sng) clm_nfo_sng=(char *)nco_free(clm_nfo_sng);
    if(cnk_map_sng) cnk_map_sng=(char *)nco_free(cnk_map_sng);
    if(cnk_plc_sng) cnk_plc_sng=(char *)nco_free(cnk_plc_sng);
    if(fl_in) fl_in=(char *)nco_free(fl_in);
    if(fl_out) fl_out=(char *)nco_free(fl_out);
    if(fl_out_tmp) fl_out_tmp=(char *)nco_free(fl_out_tmp);
    if(fl_pth) fl_pth=(char *)nco_free(fl_pth);
    if(fl_pth_lcl) fl_pth_lcl=(char *)nco_free(fl_pth_lcl);
    if(in_id_arr) in_id_arr=(int *)nco_free(in_id_arr);
    if(wgt_arr) wgt_arr=(double *)nco_free(wgt_arr);
    if(wgt_nm) wgt_nm=(char *)nco_free(wgt_nm);
    /* Free lists of strings */
    if(fl_lst_in && !fl_lst_abb) fl_lst_in=nco_sng_lst_free(fl_lst_in,fl_nbr); 
    if(fl_lst_in && fl_lst_abb) fl_lst_in=nco_sng_lst_free(fl_lst_in,1);
    if(fl_lst_abb) fl_lst_abb=nco_sng_lst_free(fl_lst_abb,abb_arg_nbr);
    if(gaa_nbr > 0) gaa_arg=nco_sng_lst_free(gaa_arg,gaa_nbr);
    if(var_lst_in_nbr > 0) var_lst_in=nco_sng_lst_free(var_lst_in,var_lst_in_nbr);
    if(wgt_nbr > 0) wgt_lst_in=nco_sng_lst_free(wgt_lst_in,wgt_nbr);
    /* Free limits */
    for(idx=0;idx<aux_nbr;idx++) aux_arg[idx]=(char *)nco_free(aux_arg[idx]);
    for(idx=0;idx<lmt_nbr;idx++) lmt_arg[idx]=(char *)nco_free(lmt_arg[idx]);
    for(idx=0;idx<ppc_nbr;idx++) ppc_arg[idx]=(char *)nco_free(ppc_arg[idx]);
    /* Free chunking information */
    for(idx=0;idx<cnk_nbr;idx++) cnk_arg[idx]=(char *)nco_free(cnk_arg[idx]);
    if(cnk_nbr > 0 && (fl_out_fmt == NC_FORMAT_NETCDF4 || fl_out_fmt == NC_FORMAT_NETCDF4_CLASSIC)) cnk.cnk_dmn=(cnk_dmn_sct **)nco_cnk_lst_free(cnk.cnk_dmn,cnk_nbr);
    /* Free dimension lists */
    if(nbr_dmn_xtr > 0) dim=nco_dmn_lst_free(dim,nbr_dmn_xtr);
    if(nbr_dmn_xtr > 0) dmn_out=nco_dmn_lst_free(dmn_out,nbr_dmn_xtr);
    /* Free variable lists */
    if(xtr_nbr > 0) var=nco_var_lst_free(var,xtr_nbr);
    if(xtr_nbr > 0) var_out=nco_var_lst_free(var_out,xtr_nbr);
    var_prc=(var_sct **)nco_free(var_prc);
    var_prc_out=(var_sct **)nco_free(var_prc_out);
    var_fix=(var_sct **)nco_free(var_fix);
    var_fix_out=(var_sct **)nco_free(var_fix_out);
    if(md5) md5=(md5_sct *)nco_md5_free(md5);
    if(wgt) wgt=(var_sct *)nco_var_free(wgt);
    if(wgt_out) wgt_out=(var_sct *)nco_var_free(wgt_out);
    if(wgt_avg) wgt_avg=(var_sct *)nco_var_free(wgt_avg);
    /* Free climatology bounds */
    if(cb){
      if(cb->bnd_val) cb->bnd_val=(double *)nco_free(cb->bnd_val);
      if(cb->clm_bnd_nm) cb->clm_bnd_nm=(char *)nco_free(cb->clm_bnd_nm);
      if(cb->cln_val) cb->cln_val=(char *)nco_free(cb->cln_val);
      if(cb->tm_bnd_nm) cb->tm_bnd_nm=(char *)nco_free(cb->tm_bnd_nm);
      if(cb->tm_crd_nm) cb->tm_crd_nm=(char *)nco_free(cb->tm_crd_nm);
      if(cb->tm_val) cb->tm_val=(double *)nco_free(cb->tm_val);
      if(cb->unt_val) cb->unt_val=(char *)nco_free(cb->unt_val);
      if(cb) cb=(clm_bnd_sct *)nco_free(cb);
    } /* !cb */

    (void)trv_tbl_free(trv_tbl);
    for(idx=0;idx<lmt_nbr;idx++) flg_dne[idx].dim_nm=(char *)nco_free(flg_dne[idx].dim_nm);
    if(flg_dne) flg_dne=(nco_dmn_dne_t *)nco_free(flg_dne);
    if(flg_input_complete) flg_input_complete=(nco_bool *)nco_free(flg_input_complete);
    if(idx_rec_out) idx_rec_out=(long *)nco_free(idx_rec_out);
    if(rec_in_cml) rec_in_cml=(long *)nco_free(rec_in_cml);
    if(rec_usd_cml) rec_usd_cml=(long *)nco_free(rec_usd_cml);
    if(REC_LST_DSR) REC_LST_DSR=(nco_bool *)nco_free(REC_LST_DSR);

  } /* !flg_mmr_cln */

#ifdef ENABLE_MPI
  MPI_Finalize();
#endif /* !ENABLE_MPI */
  
  /* End timer */ 
  ddra_info.tmr_flg=nco_tmr_end; /* [enm] Timer flag */
  rcd+=nco_ddra((char *)NULL,(char *)NULL,&ddra_info);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"main");

  nco_exit_gracefully();
  return EXIT_SUCCESS;
} /* end main() */
