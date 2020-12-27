/* $Header$ */

/* ncks -- netCDF Kitchen Sink */

/* Purpose: Extract (subsets of) variables from a netCDF file 
   Print them to screen, copy them to another file, or regrid them
   Since 2015 ncks has also been the back-end to the ncremap regridder,
   meaning that ncks implements the regridder API and functionality, 
   though all of that is a segregable component of ncks.
   The regridder is really just an API and that ncks implements */

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

/* URL: https://github.com/nco/nco/tree/master/src/nco/ncks.c

   Usage:
   ncks ~/nco/data/in.nc 
   ncks -v one ~/nco/data/in.nc
   ncks ~/nco/data/in.nc ~/foo.nc
   ncks -O -4 ~/nco/data/in.nc ~/foo.nc
   ncks -v one ~/nco/data/in.nc ~/foo.nc
   ncks -p /ZENDER/tmp -l /data/zender/tmp h0001.nc ~/foo.nc
   ncks -s "%+16.10f\n" -H -C -v three_dmn_var ~/nco/data/in.nc
   ncks -H -v fl_nm,fl_nm_arr ~/nco/data/in.nc
   ncks -H -C -u -d wvl,'0.4 micron','0.7 micron' -v wvl ~/nco/data/in.nc
   ncks -H -d fl_dim,1 -d char_dim,6,12 -v fl_nm,fl_nm_arr ~/nco/data/in.nc
   ncks -H -m -v char_var_nul,char_var_space,char_var_multinul ~/nco/data/in.nc
   ncks -H -C -v three_dmn_rec_var -d time,,,2 ~/nco/data/in.nc
   ncks -H -C -v lon -d lon,3,1 ~/nco/data/in.nc 
   ncks -M -p http://thredds-test.ucar.edu/thredds/dodsC/testdods in.nc
   ncks -O -v one -p http://thredds-testa.ucar.edu/thredds/dodsC/testdods in.nc ~/foo.nc
   ncks -O -G foo ~/nco/data/in.nc ~/foo.nc
   ncks -O -G :-5 -v v7 ~/nco/data/in_grp.nc ~/foo.nc
   ncks -O -G level3name:-5 -v v7 ~/nco/data/in_grp.nc ~/foo.nc
   ncks -O -v time ~/in_grp.nc ~/foo.nc
   ncks -O --sysconf ~/in_grp.nc ~/foo.nc
   ncks -C --xml_spr_chr=', ' -v two_dmn_rec_var_sng ~/nco/data/in.nc
   ncks --cdl -v one_dmn_rec_var ~/nco/data/in.nc
   ncks --jsn -C -v one_dmn_rec_var ~/nco/data/in.nc
   ncks --jsn -C -m -v one_dmn_rec_var ~/nco/data/in_grp.nc
   ncks -O -4 --ppc ppc_dbl=1 --ppc ppc_flt,ppc_big=4 ~/nco/data/in.nc ~/foo.nc
   ncks -O --ppc ppc_dbl=1 --ppc '/g1/ppc.?',/g1/g1g1/ppc_dbl=4 ~/nco/data/in_grp.nc ~/foo.nc
   ncks -O -m -M -v Snow_Cover_Monthly_CMG ${DATA}/hdf/MOD10CM.A2007001.005.2007108111758.hdf */

#ifdef HAVE_CONFIG_H
# include <config.h> /* Autotools tokens */
#endif /* !HAVE_CONFIG_H */

/* Standard C headers */
#include <assert.h> /* assert() */
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

#ifdef ENABLE_GSL
  #include <gsl/gsl_errno.h>
#endif

/* Personal headers */
/* #define MAIN_PROGRAM_FILE MUST precede #include libnco.h */
#define MAIN_PROGRAM_FILE
#include "libnco.h" /* netCDF Operator (NCO) library */
#include "nco.h"

int 
main(int argc,char **argv)
{
  char **fl_lst_abb=NULL; /* Option a */
  char **fl_lst_in;
  char **gaa_arg=NULL; /* [sng] Global attribute arguments */
  char **grp_lst_in=NULL;
  char **rgr_arg=NULL; /* [sng] Regridding arguments */
  char **trr_arg=NULL; /* [sng] Terraref arguments */
  char **var_lst_in=NULL;
  char **xtn_lst_in=NULL; /* [sng] Extensive variables */
  char *aux_arg[NC_MAX_DIMS];
  char *cmd_ln;
  char *cnk_arg[NC_MAX_DIMS];
  char *cnk_map_sng=NULL_CEWI; /* [sng] Chunking map */
  char *cnk_plc_sng=NULL_CEWI; /* [sng] Chunking policy */
  char *dlm_sng=NULL;
  char *fl_bnr=NULL; /* [sng] Unformatted binary output file */
  char *fl_in=NULL;
  char *fl_out=NULL; /* Option o */
  char *fl_out_tmp=NULL_CEWI;
  char *fl_prn=NULL; /* [sng] Formatted text output file */
  char *fl_pth=NULL; /* Option p */
  char *fl_pth_lcl=NULL; /* Option l */
  char *flt_sng=NULL; /* [sng] Filter string */
  char *fmt_val=NULL; /* [sng] Format string for variable values */
  char *lmt_arg[NC_MAX_DIMS];
  char *opt_crr=NULL; /* [sng] String representation of current long-option name */
  char *optarg_lcl=NULL; /* [sng] Local copy of system optarg */
  char *ppc_arg[NC_MAX_VARS]; /* [sng] PPC arguments */
  char *rec_dmn_nm=NULL; /* [sng] Record dimension name */
  char *rec_dmn_nm_fix=NULL; /* [sng] Record dimension name (Original input name without _fix prefix) */
  char *rgr_in=NULL; /* [sng] File containing fields to be regridded */
  char *rgr_grd_src=NULL; /* [sng] File containing input grid */
  char *rgr_grd_dst=NULL; /* [sng] File containing destination grid */
  char *rgr_hrz=NULL; /* [sng] File containing horizontal coordinate grid */
  char *rgr_map=NULL; /* [sng] File containing mapping weights from source to destination grid */
  char *rgr_out=NULL; /* [sng] File containing regridded fields */
  char *rgr_var=NULL; /* [sng] Variable for special regridding treatment */
  char *rgr_vrt=NULL; /* [sng] File containing vertical coordinate grid */
  char *smr_fl_sz_sng=NULL; /* [sng] String describing estimated file size */
  char *smr_sng=NULL; /* [sng] File summary string */
  char *smr_xtn_sng=NULL; /* [sng] File extended summary string */
  char *sng_cnv_rcd=NULL_CEWI; /* [sng] strtol()/strtoul() return code */
  char *spr_chr=NULL; /* [sng] Separator for XML character types */
  char *spr_nmr=NULL; /* [sng] Separator for XML numeric types */
  char *trr_in=NULL; /* [sng] File containing raw Terraref imagery */
  char *trr_wxy=NULL; /* [sng] Terraref dimension sizes */

  char trv_pth[]="/"; /* [sng] Root path of traversal tree */

  const char * const CVS_Id="$Id$"; 
  const char * const CVS_Revision="$Revision$";
  const char * const opt_sht_lst="34567aABb:CcD:d:FG:g:HhL:l:MmOo:Pp:qQrRs:t:uVv:X:xz-:";

  cnk_sct cnk; /* [sct] Chunking structure */

#if defined(__cplusplus) || defined(PGI_CC)
  ddra_info_sct ddra_info;
  ddra_info.flg_ddra=False;
#else /* !__cplusplus */
  ddra_info_sct ddra_info={.flg_ddra=False};
#endif /* !__cplusplus */

  double wgt_vld_thr=NC_MIN_DOUBLE; /* [frc] Weight threshold for valid destination value */

  extern char *optarg;
  extern int optind;

  FILE *fp_bnr=NULL; /* [fl] Unformatted binary output file handle */
  FILE *fp_prn=NULL; /* [fl] Formatted text output file handle */

  gpe_sct *gpe=NULL; /* [sng] Group Path Editing (GPE) structure */

  int *in_id_arr; /* [id] netCDF file IDs used by OpenMP code */

  int JSN_ATT_FMT=0; /* [enm] JSON format for netCDF attributes: 0 (no object, only data), 1 (data only for string, char, int, and floating-point types, otherwise object), 2 (always object) */
  int JSN_VAR_FMT=2; /* [flg] JSON format for netCDF variables: 0 (no type except for user-defined types), 1 (type for non-default types), 2 (always type) */
  nco_bool JSN_DATA_BRK=True; /* [flg] JSON format for netCDF variables: 0 (no brackets), 1 (bracket inner dimensions of multi-dimensional data) */

  int abb_arg_nbr=0;
  int att_glb_nbr;
  int att_grp_nbr;
  int att_var_nbr;
  int aux_nbr=0; /* [nbr] Number of auxiliary coordinate hyperslabs specified */
  int cnk_map=nco_cnk_map_nil; /* [enm] Chunking map */
  int cnk_nbr=0; /* [nbr] Number of chunk sizes */
  int cnk_plc=nco_cnk_plc_nil; /* [enm] Chunking policy */
  int dfl_lvl=NCO_DFL_LVL_UNDEFINED; /* [enm] Deflate level */
  int dmn_nbr_fl;
  int dmn_rec_fl;
  int dt_fmt=fmt_dt_nil;
  int fl_in_fmt=NCO_FORMAT_UNDEFINED; /* [enm] Input file format */
  int fl_nbr=0;
  int fl_out_fmt=NCO_FORMAT_UNDEFINED; /* [enm] Output file format */
  int fll_md_old; /* [enm] Old fill mode */
  int gaa_nbr=0; /* [nbr] Number of global attributes to add */
  int grp_dpt_fl; /* [nbr] Maximum group depth (root = 0) */
  int grp_lst_in_nbr=0; /* [nbr] Number of groups explicitly specified by user */
  int grp_nbr_fl;
  int idx;
  int in_id;  
  int lmt_nbr=0; /* Option d. NB: lmt_nbr gets incremented */
  int log_lvl=0; /* [enm] netCDF library debugging verbosity [0..5] */
  int md_open; /* [enm] Mode flag for nc_open() call */
  int opt;
  int ppc_nbr=0; /* [nbr] Number of PPC arguments */
  int rgr_nbr=0; /* [nbr] Number of regridding arguments */
  int trr_nbr=0; /* [nbr] Number of TERRAREF arguments */
  int rcd=NC_NOERR; /* [rcd] Return code */
  int srt_mth=0; /* [enm] Sort method [0=ascending, 1=descending] */
  int thr_idx; /* [idx] Index of current thread */
  int thr_nbr=int_CEWI; /* [nbr] Thread number Option t */
  int var_lst_in_nbr=0;
  int var_nbr_fl;
  int var_udt_fl;
  int xtn_nbr=0; /* [nbr] Number of extensive variables */
  int xtr_nbr=0; /* [nbr] xtr_nbr will not otherwise be set for -c with no -v */

  md5_sct *md5=NULL; /* [sct] MD5 configuration */
 
  nco_bool ALPHABETIZE_OUTPUT=True; /* Option a */
  nco_bool CHK_MAP=False; /* [flg] Check map-file quality */
  nco_bool CHK_NAN=False; /* [flg] Check for NaNs */
  nco_bool CPY_GRP_METADATA; /* [flg] Copy group metadata (attributes) */
  nco_bool EXCLUDE_INPUT_LIST=False; /* Option x */
  nco_bool EXTRACT_ALL_COORDINATES=False; /* Option c */
  nco_bool EXTRACT_ASSOCIATED_COORDINATES=True; /* Option C */
  nco_bool EXTRACT_CLL_MSR=True; /* [flg] Extract cell_measures variables */
  nco_bool EXTRACT_FRM_TRM=True; /* [flg] Extract formula_terms variables */
  nco_bool FL_RTR_RMT_LCN;
  nco_bool FL_LST_IN_FROM_STDIN=False; /* [flg] fl_lst_in comes from stdin */
  nco_bool FORCE_APPEND=False; /* Option A */
  nco_bool FORCE_NOCLOBBER=False; /* Option no-clobber */
  nco_bool FORCE_OVERWRITE=False; /* Option O */
  nco_bool FORTRAN_IDX_CNV=False; /* Option F */
  nco_bool GRP_VAR_UNN=False; /* [flg] Select union of specified groups and variables */
  nco_bool GRP_XTR_VAR_XCL=False; /* [flg] Extract matching groups, exclude matching variables */
  nco_bool HAVE_LIMITS=False; /* [flg] Are there user limits? (-d) */
  nco_bool HISTORY_APPEND=True; /* Option h */
  nco_bool HPSS_TRY=False; /* [flg] Search HPSS for unfound files */
  nco_bool LST_RNK_GE2=False; /* [flg] Print extraction list of rank >= 2 variables */
  nco_bool LST_XTR=False; /* [flg] Print extraction list */
  nco_bool MSA_USR_RDR=False; /* [flg] Multi-Slab Algorithm returns hyperslabs in user-specified order */
  nco_bool PRN_CDL=False; /* [flg] Print CDL */
  nco_bool PRN_HDN=False; /* [flg] Print hidden attributes */
  nco_bool PRN_UDT=False; /* [flg] Print non-atomic variables */
  nco_bool PRN_SRM=False; /* [flg] Print ncStream */
  nco_bool PRN_JSN=False; /* [flg] Print JSON */
  nco_bool PRN_TRD=False; /* [flg] Print traditional */
  nco_bool PRN_XML=False; /* [flg] Print XML (NcML) */
  nco_bool PRN_XML_LOCATION=True; /* [flg] Print XML location tag */
  nco_bool PRN_DMN_IDX_CRD_VAL=True; /* [flg] Print leading dimension/coordinate indices/values Option Q */
  nco_bool PRN_DMN_UNITS=True; /* [flg] Print dimensional units Option u */
  nco_bool PRN_DMN_VAR_NM=True; /* [flg] Print dimension/variable names */
  nco_bool PRN_DMN_UNITS_TGL=False; /* [flg] Toggle print dimensional units Option u */
  nco_bool PRN_GLB_METADATA=False; /* [flg] Print global metadata */
  nco_bool PRN_GLB_METADATA_TGL=False; /* [flg] Toggle print global metadata Option M */
  nco_bool PRN_MSS_VAL_BLANK=True; /* [flg] Print missing values as blanks */
  nco_bool PRN_QUENCH=False; /* [flg] Quench (turn-off) all printing to screen */
  nco_bool PRN_VAR_DATA=False; /* [flg] Print variable data */
  nco_bool PRN_VAR_DATA_TGL=False; /* [flg] Toggle print variable data Option H */
  nco_bool PRN_VAR_METADATA=False; /* [flg] Print variable metadata */
  nco_bool PRN_VAR_METADATA_TGL=False; /* [flg] Toggle print variable metadata Option m */
  nco_bool PRN_CLN_LGB=False; /* [flg] Print UDUnits-formatted calendar dates/times human-legibly */
  nco_bool PRN_VRB=False; /* [flg] Print data and metadata by default */
  nco_bool RETAIN_ALL_DIMS=False; /* [flg] Retain all dimensions */
  nco_bool RAM_CREATE=False; /* [flg] Create file in RAM */
  nco_bool RAM_OPEN=False; /* [flg] Open (netCDF3-only) file(s) in RAM */
  nco_bool SHARE_CREATE=False; /* [flg] Create (netCDF3-only) file(s) with unbuffered I/O */
  nco_bool SHARE_OPEN=False; /* [flg] Open (netCDF3-only) file(s) with unbuffered I/O */
  nco_bool RM_RMT_FL_PST_PRC=True; /* Option R */
  nco_bool WRT_TMP_FL=True; /* [flg] Write output to temporary file */
  nco_bool flg_area_wgt=False; /* [flg] Area-weight map-file statistics */
  nco_bool flg_dmm_in=False; /* [flg] Make dummy input file */
  nco_bool flg_frac_b_nrm=False; /* [flg] Normalize map-file weights when frac_b >> 1 */
  nco_bool flg_mmr_cln=True; /* [flg] Clean memory prior to exit */
  nco_bool flg_rgr=False; /* [flg] Regrid */
  nco_bool flg_s1d=False; /* [flg] Unpack sparse-1D CLM/ELM variables */
  nco_bool flg_trr=False; /* [flg] Terraref */

  nco_dmn_dne_t *flg_dne=NULL; /* [lst] Flag to check if input dimension -d "does not exist" */

  size_t bfr_sz_hnt=NC_SIZEHINT_DEFAULT; /* [B] Buffer size hint */
  size_t cnk_csh_byt=NCO_CNK_CSH_BYT_DFL; /* [B] Chunk cache size */
  size_t cnk_min_byt=NCO_CNK_SZ_MIN_BYT_DFL; /* [B] Minimize size of variable to chunk */
  size_t cnk_sz_byt=0UL; /* [B] Chunk size in bytes */
  size_t cnk_sz_scl=0UL; /* [nbr] Chunk size scalar */
  size_t hdr_pad=0UL; /* [B] Pad at end of header section */

  trv_tbl_sct *trv_tbl=NULL; /* [lst] Traversal table */

#ifdef ENABLE_MPI
  /* Declare all MPI-specific variables here */
  MPI_Comm mpi_cmm=MPI_COMM_WORLD; /* [prc] Communicator */
  int prc_rnk; /* [idx] Process rank */
  int prc_nbr=0; /* [nbr] Number of MPI processes */
#endif /* !ENABLE_MPI */
  
  static struct option opt_lng[]={ /* Structure ordered by short option key if possible */
    /* Long options with no argument, no short option counterpart */
    {"area_wgt",no_argument,0,0}, /* [flg] Area-weight map-file statistics */
    {"bld",no_argument,0,0}, /* [sng] Build-engine */
    {"build_engine",no_argument,0,0}, /* [sng] Build-engine */
    {"calendar",no_argument,0,0}, /* [flg] Print UDUnits-formatted calendar dates/times human-legibly */
    {"cln_lgb",no_argument,0,0}, /* [flg] Print UDUnits-formatted calendar dates/times human-legibly */
    {"prn_cln_lgb",no_argument,0,0}, /* [flg] Print UDUnits-formatted calendar dates/times human-legibly */
    {"prn_lgb",no_argument,0,0}, /* [flg] Print UDUnits-formatted calendar dates/times human-legibly */
    {"timestamp",no_argument,0,0}, /* [flg] Print UDUnits-formatted calendar dates/times human-legibly */
    {"cdl",no_argument,0,0}, /* [flg] Print CDL */
    {"cll_msr",no_argument,0,0}, /* [flg] Extract cell_measures variables */
    {"cell_measures",no_argument,0,0}, /* [flg] Extract cell_measures variables */
    {"no_cll_msr",no_argument,0,0}, /* [flg] Do not extract cell_measures variables */
    {"no_cell_measures",no_argument,0,0}, /* [flg] Do not extract cell_measures variables */
    {"frm_trm",no_argument,0,0}, /* [flg] Extract formula_terms variables */
    {"formula_terms",no_argument,0,0}, /* [flg] Extract formula_terms variables */
    {"no_frm_trm",no_argument,0,0}, /* [flg] Do not extract formula_terms variables */
    {"no_formula_terms",no_argument,0,0}, /* [flg] Do not extract formula_terms variables */
    {"chk_map",no_argument,0,0}, /* [flg] Check map-file quality */
    {"check_map",no_argument,0,0}, /* [flg] Check map-file quality */
    {"chk_nan",no_argument,0,0}, /* [flg] Check file for NaNs */
    {"check_nan",no_argument,0,0}, /* [flg] Check file for NaNs */
    {"nan",no_argument,0,0}, /* [flg] Check file for NaNs */
    {"NaN",no_argument,0,0}, /* [flg] Check file for NaNs */
    {"clean",no_argument,0,0}, /* [flg] Clean memory prior to exit */
    {"mmr_cln",no_argument,0,0}, /* [flg] Clean memory prior to exit */
    {"drt",no_argument,0,0}, /* [flg] Allow dirty memory on exit */
    {"dirty",no_argument,0,0}, /* [flg] Allow dirty memory on exit */
    {"mmr_drt",no_argument,0,0}, /* [flg] Allow dirty memory on exit */
    {"cmp",no_argument,0,0},
    {"compiler",no_argument,0,0},
    {"copyright",no_argument,0,0},
    {"cpy",no_argument,0,0},
    {"dmm_in_mk",no_argument,0,0}, /* [flg] Make dummy input file */
    {"fl_dmm",no_argument,0,0}, /* [flg] Make dummy input file */
    {"license",no_argument,0,0},
    {"hdf4",no_argument,0,0}, /* [flg] Treat file as HDF4 */
    {"hdn",no_argument,0,0}, /* [flg] Print hidden attributes */
    {"hidden",no_argument,0,0}, /* [flg] Print hidden attributes */
    {"help",no_argument,0,0},
    {"hlp",no_argument,0,0},
    {"hpss_try",no_argument,0,0}, /* [flg] Search HPSS for unfound files */
    {"id",no_argument,0,0}, /* [flg] Print normally hidden information, like file, group, and variable IDs */
    {"lbr",no_argument,0,0},
    {"library",no_argument,0,0},
    {"lst_rnk_ge2",no_argument,0,0}, /* [flg] Print extraction list of rank >= 2 variables */
    {"lst_xtr",no_argument,0,0}, /* [flg] Print extraction list */
    {"frac_b_nrm",no_argument,0,0}, /* [flg] Normalize map-file weights when frac_b >> 1 */
    {"md5_dgs",no_argument,0,0}, /* [flg] Perform MD5 digests */
    {"md5_digest",no_argument,0,0}, /* [flg] Perform MD5 digests */
    {"md5_wrt_att",no_argument,0,0}, /* [flg] Write MD5 digests as attributes */
    {"md5_write_attribute",no_argument,0,0}, /* [flg] Write MD5 digests as attributes */
    {"mpi_implementation",no_argument,0,0},
    {"msa_usr_rdr",no_argument,0,0}, /* [flg] Multi-Slab Algorithm returns hyperslabs in user-specified order */
    {"msa_user_order",no_argument,0,0}, /* [flg] Multi-Slab Algorithm returns hyperslabs in user-specified order */
    {"no_abc",no_argument,0,0},
    {"no-abc",no_argument,0,0},
    {"no_alphabetize",no_argument,0,0},
    {"no-alphabetize",no_argument,0,0},
    {"no_blank",no_argument,0,0}, /* [flg] Print numeric missing values */
    {"no-blank",no_argument,0,0}, /* [flg] Print numeric missing values */
    {"noblank",no_argument,0,0}, /* [flg] Print numeric missing values */
    {"no_clb",no_argument,0,0},
    {"noclobber",no_argument,0,0},
    {"no-clobber",no_argument,0,0},
    {"no_clobber",no_argument,0,0},
    {"no_dmn_var_nm",no_argument,0,0}, /* [flg] Omit variable and dimension names and indices but print all values */
    {"no_nm_prn",no_argument,0,0}, /* [flg] Omit variable and dimension names and indices but print all values */
    {"ntm",no_argument,0,0}, /* [flg] Print non-atomic variables */
    {"nonatomic",no_argument,0,0}, /* [flg] Print non-atomic variables */
    {"udt",no_argument,0,0}, /* [flg] Print non-atomic variables */
    {"user_defined_types",no_argument,0,0}, /* [flg] Print non-atomic variables */
    {"rad",no_argument,0,0}, /* [flg] Retain all dimensions */
    {"retain_all_dimensions",no_argument,0,0}, /* [flg] Retain all dimensions */
    {"orphan_dimensions",no_argument,0,0}, /* [flg] Retain all dimensions */
    {"rph_dmn",no_argument,0,0}, /* [flg] Retain all dimensions */
    {"ram_all",no_argument,0,0}, /* [flg] Open and create (netCDF3) file(s) in RAM */
    {"create_ram",no_argument,0,0}, /* [flg] Create file in RAM */
    {"open_ram",no_argument,0,0}, /* [flg] Open (netCDF3) file(s) in RAM */
    {"diskless_all",no_argument,0,0}, /* [flg] Open and create (netCDF3) file(s) in RAM */
    {"share_all",no_argument,0,0}, /* [flg] Open and create (netCDF3) file(s) with unbuffered I/O */
    {"create_share",no_argument,0,0}, /* [flg] Create (netCDF3) file(s) with unbuffered I/O */
    {"open_share",no_argument,0,0}, /* [flg] Open (netCDF3) file(s) with unbuffered I/O */
    {"unbuffered_io",no_argument,0,0}, /* [flg] Open and create (netCDF3) file(s) with unbuffered I/O */
    {"uio",no_argument,0,0}, /* [flg] Open and create (netCDF3) file(s) with unbuffered I/O */
    {"secret",no_argument,0,0},
    {"shh",no_argument,0,0},
    {"srm",no_argument,0,0}, /* [flg] Print ncStream */
    {"s1d",no_argument,0,0}, /* [flg] Unpack sparse-1D CLM/ELM variables */
    {"sparse",no_argument,0,0}, /* [flg] Unpack sparse-1D CLM/ELM variables */
    {"unpack_sparse",no_argument,0,0}, /* [flg] Unpack sparse-1D CLM/ELM variables */
    {"sysconf",no_argument,0,0}, /* [flg] Perform sysconf() test */
    {"wrt_tmp_fl",no_argument,0,0}, /* [flg] Write output to temporary file */
    {"write_tmp_fl",no_argument,0,0}, /* [flg] Write output to temporary file */
    {"no_tmp_fl",no_argument,0,0}, /* [flg] Do not write output to temporary file */
    {"intersection",no_argument,0,0}, /* [flg] Select intersection of specified groups and variables */
    {"nsx",no_argument,0,0}, /* [flg] Select intersection of specified groups and variables */
    {"union",no_argument,0,0}, /* [flg] Select union of specified groups and variables */
    {"unn",no_argument,0,0}, /* [flg] Select union of specified groups and variables */
    {"grp_xtr_var_xcl",no_argument,0,0}, /* [flg] Extract matching groups, exclude matching variables */
    {"version",no_argument,0,0},
    {"vrs",no_argument,0,0},
    {"jsn",no_argument,0,0}, /* [flg] Print JSON */
    {"json",no_argument,0,0}, /* [flg] Print JSON */
    {"trd",no_argument,0,0}, /* [flg] Print traditional */
    {"traditional",no_argument,0,0}, /* [flg] Print traditional */
    {"w10",no_argument,0,0}, /* [flg] Print JSON */
    {"w10n",no_argument,0,0}, /* [flg] Print JSON */
    {"xml",no_argument,0,0}, /* [flg] Print XML (NcML) */
    {"ncml",no_argument,0,0}, /* [flg] Print XML (NcML) */
    {"xml_no_location",no_argument,0,0}, /* [flg] Omit XML location tag */
    {"ncml_no_location",no_argument,0,0}, /* [flg] Omit XML location tag */
    /* Long options with argument, no short option counterpart */
    {"baa",required_argument,0,0}, /* [enm] Bit-Adjustment Algorithm */
    {"bit_alg",required_argument,0,0}, /* [enm] Bit-Adjustment Algorithm */
    {"bfr_sz_hnt",required_argument,0,0}, /* [B] Buffer size hint */
    {"buffer_size_hint",required_argument,0,0}, /* [B] Buffer size hint */
    {"bsa",required_argument,0,0}, /* [enm] Binary byte-swap algorithm */
    {"byte_swap",required_argument,0,0}, /* [enm] Binary byte-swap algorithm */
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
    {"dt_fmt",required_argument,0,0}, /* [enm] Date format for CDL output with --cal */
    {"date_format",required_argument,0,0}, /* [enm] Date format for CDL output with --cal */
    {"fl_fmt",required_argument,0,0},
    {"file_format",required_argument,0,0},
    {"fix_rec_dmn",required_argument,0,0}, /* [sng] Fix record dimension */
    {"no_rec_dmn",required_argument,0,0}, /* [sng] Fix record dimension */
    {"fl_prn",required_argument,0,0}, /* [sng] Formatted text output file */
    {"file_print",required_argument,0,0}, /* [sng] Formatted text output file */
    {"prn_fl",required_argument,0,0}, /* [sng] Formatted text output file */
    {"print_file",required_argument,0,0}, /* [sng] Formatted text output file */
    {"ccr",required_argument,0,0}, /* [sng] CCR codec name */
    {"cdc",required_argument,0,0}, /* [sng] CCR codec name */
    {"codec",required_argument,0,0}, /* [sng] CCR codec name */
    {"flt",required_argument,0,0}, /* [sng] Filter string */
    {"filter",required_argument,0,0}, /* [sng] Filter string */
    {"fmt_val",required_argument,0,0}, /* [sng] Format string for variable values */
    {"val_fmt",required_argument,0,0}, /* [sng] Format string for variable values */
    {"value_format",required_argument,0,0}, /* [sng] Format string for variable values */
    {"gaa",required_argument,0,0}, /* [sng] Global attribute add */
    {"glb_att_add",required_argument,0,0}, /* [sng] Global attribute add */
    {"hdr_pad",required_argument,0,0},
    {"header_pad",required_argument,0,0},
    {"jsn_fmt",required_argument,0,0}, /* [enm] JSON format */
    {"jsn_format",required_argument,0,0}, /* [enm] JSON format */
    {"json_fmt",required_argument,0,0}, /* [enm] JSON format */
    {"json_format",required_argument,0,0}, /* [enm] JSON format */
    {"log_lvl",required_argument,0,0}, /* [enm] netCDF library debugging verbosity [0..5] */
    {"log_level",required_argument,0,0}, /* [enm] netCDF library debugging verbosity [0..5] */
    {"mk_rec_dmn",required_argument,0,0}, /* [sng] Name of record dimension in output */
    {"mk_rec_dim",required_argument,0,0}, /* [sng] Name of record dimension in output */
    {"mta_dlm",required_argument,0,0}, /* [sng] Multi-argument delimiter */
    {"dlm_mta",required_argument,0,0}, /* [sng] Multi-argument delimiter */
    {"ppc",required_argument,0,0}, /* [nbr] Precision-preserving compression, i.e., number of total or decimal significant digits */
    {"precision_preserving_compression",required_argument,0,0}, /* [nbr] Precision-preserving compression, i.e., number of total or decimal significant digits */
    {"quantize",required_argument,0,0}, /* [nbr] Precision-preserving compression, i.e., number of total or decimal significant digits */
    {"rgr",required_argument,0,0}, /* [sng] Regridding */
    {"regridding",required_argument,0,0}, /* [sng] Regridding */
    {"rgr_in",required_argument,0,0}, /* [sng] File containing fields to be regridded */
    {"rgr_grd_src",required_argument,0,0}, /* [sng] File containing input grid */
    {"grd_src",required_argument,0,0}, /* [sng] File containing input grid */
    {"src_grd",required_argument,0,0}, /* [sng] File containing input grid */
    {"rgr_grd_dst",required_argument,0,0}, /* [sng] File containing destination grid */
    {"dst_grd",required_argument,0,0}, /* [sng] File containing destination grid */
    {"grd_dst",required_argument,0,0}, /* [sng] File containing destination grid */
    {"rgr_map",required_argument,0,0}, /* [sng] File containing mapping weights from source to destination grid */
    {"map_file",required_argument,0,0}, /* [sng] File containing mapping weights from source to destination grid */
    {"map_fl",required_argument,0,0}, /* [sng] File containing mapping weights from source to destination grid */
    {"rgr_hrz",required_argument,0,0}, /* [sng] File containing horizontal coordinate grid */
    {"hrz_fl",required_argument,0,0}, /* [sng] File containing horizontal coordinate grid */
    {"hrz_crd",required_argument,0,0}, /* [sng] File containing horizontal coordinate grid */
    {"rgr_rnr",required_argument,0,0}, /* [flg] Renormalize destination values by valid area */
    {"rgr_var",required_argument,0,0}, /* I [sng] Variable for special regridding treatment */
    {"rgr_vrt",required_argument,0,0}, /* [sng] File containing vertical coordinate grid */
    {"vrt_fl",required_argument,0,0}, /* [sng] File containing vertical coordinate grid */
    {"vrt_crd",required_argument,0,0}, /* [sng] File containing vertical coordinate grid */
    {"rnr_thr",required_argument,0,0}, /* [flg] Renormalize destination values by valid area */
    {"renormalize",required_argument,0,0}, /* [flg] Renormalize destination values by valid area */
    {"renormalization_threshold",required_argument,0,0}, /* [flg] Renormalize destination values by valid area */
    {"trr",required_argument,0,0}, /* [sng] Terraref */
    {"terraref",required_argument,0,0}, /* [sng] Terraref */
    {"trr_in",required_argument,0,0}, /* [sng] File containing raw Terraref imagery */
    {"trr_wxy",required_argument,0,0}, /* [sng] Terraref dimension sizes */
    {"tst_udunits",required_argument,0,0},
    {"xml_spr_chr",required_argument,0,0}, /* [flg] Separator for XML character types */
    {"xml_spr_nmr",required_argument,0,0}, /* [flg] Separator for XML numeric types */
    {"xtn_var_lst",required_argument,0,0}, /* [sng] Extensive variables */
    {"extensive",required_argument,0,0}, /* [sng] Extensive variables */
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
    {"abc",no_argument,0,'a'},
    {"alphabetize",no_argument,0,'a'},
    {"append",no_argument,0,'A'},
    {"apn",no_argument,0,'A'},
    {"bnr",required_argument,0,'b'},
    {"binary",required_argument,0,'b'},
    {"binary-file",required_argument,0,'b'},
    {"fl_bnr",required_argument,0,'b'},
    {"coords",no_argument,0,'c'},
    {"crd",no_argument,0,'c'},
    {"xtr_ass_var",no_argument,0,'c'},
    {"xcl_ass_var",no_argument,0,'C'},
    {"no_coords",no_argument,0,'C'},
    {"no_crd",no_argument,0,'C'},
    {"data",required_argument,0,'H'},
    {"dbg_lvl",required_argument,0,'D'},
    {"debug",required_argument,0,'D'},
    {"nco_dbg_lvl",required_argument,0,'D'},
    {"dimension",required_argument,0,'d'},
    {"dmn",required_argument,0,'d'},
    {"fortran",no_argument,0,'F'},
    {"ftn",no_argument,0,'F'},
    {"gpe",required_argument,0,'G'}, /* [sng] Group Path Edit (GPE) */
    {"grp",required_argument,0,'g'},
    {"group",required_argument,0,'g'},
    {"history",no_argument,0,'h'},
    {"hst",no_argument,0,'h'},
    {"hieronymus",no_argument,0,'H'}, /* fxm: need better mnemonic for -H */
    {"dfl_lvl",required_argument,0,'L'}, /* [enm] Deflate level */
    {"deflate",required_argument,0,'L'}, /* [enm] Deflate level */
    {"lcl",required_argument,0,'l'},
    {"local",required_argument,0,'l'},
    {"metadata",no_argument,0,'m'},
    {"mtd",no_argument,0,'m'},
    {"Metadata",no_argument,0,'M'},
    {"Mtd",no_argument,0,'M'},
    {"overwrite",no_argument,0,'O'},
    {"ovr",no_argument,0,'O'},
    {"output",required_argument,0,'o'},
    {"fl_out",required_argument,0,'o'},
    {"print",required_argument,0,'P'},
    {"prn",required_argument,0,'P'},
    {"path",required_argument,0,'p'},
    {"quench",no_argument,0,'q'},
    {"quiet",no_argument,0,'Q'},
    {"retain",no_argument,0,'R'},
    {"rtn",no_argument,0,'R'},
    {"revision",no_argument,0,'r'},
    {"spinlock",no_argument,0,'S'}, /* [flg] Suspend with signal handler to facilitate debugging */
    {"sng_fmt",required_argument,0,'s'},
    {"string",required_argument,0,'s'},
    {"thr_nbr",required_argument,0,'t'},
    {"threads",required_argument,0,'t'},
    {"omp_num_threads",required_argument,0,'t'},
    {"units",no_argument,0,'u'},
    {"val_var",no_argument,0,'V'}, /* [flg] Print variable values only */
    {"variable",required_argument,0,'v'},
    {"auxiliary",required_argument,0,'X'},
    {"exclude",no_argument,0,'x'},
    {"xcl",no_argument,0,'x'},
    {"lbr_rcd",no_argument,0,0},
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
  
  /* MPI I/O: Either Parallel netCDF (PnetCDF) or HDF5-based
     export NETCDF_ROOT=/usr/local/parallel;export NETCDF_INC=/usr/local/parallel/include;export NETCDF_LIB=/usr/local/parallel/lib;export NETCDF4_ROOT=/usr/local/parallel;
     cd ~/nco/bld;make MPI=Y;cd -
     LD_LIBRARY_PATH=/usr/local/parallel/lib\:${LD_LIBRARY_PATH}
     ldd `which ncks`
     ncks -O -5 ~/nco/data/in.nc ~/foo.nc # PnetCDF 
     mpiexec -n 1 ncks -O -4 ~/nco/data/in.nc ~/foo.nc # HDF5
     od -An -c -N4 ~/foo.nc */
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
      } /* endif bfr_sz */
      if(!strcmp(opt_crr,"bsa") || !strcmp(opt_crr,"byte_swap")){
	nco_bnr_cnv=(unsigned short int)strtoul(optarg,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
	if(*sng_cnv_rcd) nco_sng_cnv_err(optarg,"strtoul",sng_cnv_rcd);
      } /* endif bnr */
      if(!strcmp(opt_crr,"bld") || !strcmp(opt_crr,"build_engine")){
	const char bld_ngn[]=TKN2SNG(NCO_BUILDENGINE); // [sng] Build-engine
        (void)fprintf(stdout,"%s\n",bld_ngn);
        nco_exit(EXIT_SUCCESS);
      } /* endif "bld" */
      if(!strcmp(opt_crr,"dt_fmt") || !strcmp(opt_crr,"date_format")){
        dt_fmt=strtoul(optarg,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
      } /* !dt_fmt */
      if(!strcmp(opt_crr,"calendar") || !strcmp(opt_crr,"cln_lgb") || !strcmp(opt_crr,"prn_cln_lgb") || !strcmp(opt_crr,"prn_lgb") || !strcmp(opt_crr,"timestamp")) PRN_CLN_LGB=True; /* [flg] Print UDUnits-formatted calendar dates/times human-legibly */
      if(!strcmp(opt_crr,"chk_map") || !strcmp(opt_crr,"check_map")) CHK_MAP=True; /* [flg] Check map-file quality */
      if(!strcmp(opt_crr,"chk_nan") || !strcmp(opt_crr,"check_nan") || !strcmp(opt_crr,"nan") || !strcmp(opt_crr,"NaN")) CHK_NAN=True; /* [flg] Check for NaNs */
      if(!strcmp(opt_crr,"cnk_byt") || !strcmp(opt_crr,"chunk_byte")){
        cnk_sz_byt=strtoul(optarg,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
        if(*sng_cnv_rcd) nco_sng_cnv_err(optarg,"strtoul",sng_cnv_rcd);
      } /* endif cnk_byt */
      if(!strcmp(opt_crr,"cnk_csh") || !strcmp(opt_crr,"chunk_cache")){
        cnk_csh_byt=strtoul(optarg,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
        if(*sng_cnv_rcd) nco_sng_cnv_err(optarg,"strtoul",sng_cnv_rcd);
      } /* endif cnk_csh */
      if(!strcmp(opt_crr,"cnk_min") || !strcmp(opt_crr,"chunk_min")){
        cnk_min_byt=strtoul(optarg,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
        if(*sng_cnv_rcd) nco_sng_cnv_err(optarg,"strtoul",sng_cnv_rcd);
      } /* endif cnk_min */
      if(!strcmp(opt_crr,"cnk_dmn") || !strcmp(opt_crr,"chunk_dimension")){
        /* Copy limit argument for later processing */
        cnk_arg[cnk_nbr]=(char *)strdup(optarg);
        cnk_nbr++;
      } /* endif cnk_dmn */
      if(!strcmp(opt_crr,"cnk_scl") || !strcmp(opt_crr,"chunk_scalar")){
        cnk_sz_scl=strtoul(optarg,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
        if(*sng_cnv_rcd) nco_sng_cnv_err(optarg,"strtoul",sng_cnv_rcd);
      } /* endif cnk_scl */
      if(!strcmp(opt_crr,"cnk_map") || !strcmp(opt_crr,"chunk_map")){
        /* Chunking map */
        cnk_map_sng=(char *)strdup(optarg);
        cnk_map=nco_cnk_map_get(cnk_map_sng);
      } /* endif cnk_map */
      if(!strcmp(opt_crr,"cnk_plc") || !strcmp(opt_crr,"chunk_policy")){
        /* Chunking policy */
        cnk_plc_sng=(char *)strdup(optarg);
        cnk_plc=nco_cnk_plc_get(cnk_plc_sng);
      } /* endif cnk_plc */
      if(!strcmp(opt_crr,"cll_msr") || !strcmp(opt_crr,"cell_measures")) EXTRACT_CLL_MSR=True; /* [flg] Extract cell_measures variables */
      if(!strcmp(opt_crr,"no_cll_msr") || !strcmp(opt_crr,"no_cell_measures")) EXTRACT_CLL_MSR=False; /* [flg] Do not extract cell_measures variables */
      if(!strcmp(opt_crr,"frm_trm") || !strcmp(opt_crr,"formula_terms")) EXTRACT_FRM_TRM=True; /* [flg] Extract formula_terms variables */
      if(!strcmp(opt_crr,"no_frm_trm") || !strcmp(opt_crr,"no_formula_terms")) EXTRACT_FRM_TRM=False; /* [flg] Do not extract formula_terms variables */
      if(!strcmp(opt_crr,"cmp") || !strcmp(opt_crr,"compiler")){
        (void)fprintf(stdout,"%s\n",nco_cmp_get());
        nco_exit(EXIT_SUCCESS);
      } /* endif "cmp" */
      if(!strcmp(opt_crr,"cpy") || !strcmp(opt_crr,"copyright") || !strcmp(opt_crr,"license")){
	(void)nco_cpy_prn();
        nco_exit(EXIT_SUCCESS);
      } /* endif "copyright" */
      if(!strcmp(opt_crr,"cdl")) PRN_CDL=True; /* [flg] Print CDL */
      if(!strcmp(opt_crr,"trd") || !strcmp(opt_crr,"traditional")) PRN_TRD=True; /* [flg] Print traditional */
      if(!strcmp(opt_crr,"mmr_cln") || !strcmp(opt_crr,"clean")) flg_mmr_cln=True; /* [flg] Clean memory prior to exit */
      if(!strcmp(opt_crr,"drt") || !strcmp(opt_crr,"mmr_drt") || !strcmp(opt_crr,"dirty")) flg_mmr_cln=False; /* [flg] Clean memory prior to exit */
      if(!strcmp(opt_crr,"dmm_in_mk") || !strcmp(opt_crr,"fl_dmm")) flg_dmm_in=True; /* [flg] Make dummy input file */
      if(!strcmp(opt_crr,"fix_rec_dmn") || !strcmp(opt_crr,"no_rec_dmn")){
        const char fix_pfx[]="fix_"; /* [sng] Prefix string to fix dimension */
        rec_dmn_nm=(char *)nco_malloc((strlen(fix_pfx)+strlen(optarg)+1L)*sizeof(char));
        rec_dmn_nm=strcpy(rec_dmn_nm,fix_pfx);
        rec_dmn_nm=strcat(rec_dmn_nm,optarg);
        rec_dmn_nm_fix=strdup(optarg);
      } /* !fix_rec_dmn */
      if(!strcmp(opt_crr,"fl_fmt") || !strcmp(opt_crr,"file_format")) rcd=nco_create_mode_prs(optarg,&fl_out_fmt);
      if(!strcmp(opt_crr,"fl_prn") || !strcmp(opt_crr,"file_print") || !strcmp(opt_crr,"prn_fl") || !strcmp(opt_crr,"print_file")) fl_prn=(char *)strdup(optarg);
      if(!strcmp(opt_crr,"flt") || !strcmp(opt_crr,"filter")){
	flt_sng=(char *)strdup(optarg);
	/* [fnc] Parse filter string and exit */
	if(flt_sng) nco_flt_prs(flt_sng);
      } /* !flt */
      if(!strcmp(opt_crr,"ccr") || !strcmp(opt_crr,"cdc") || !strcmp(opt_crr,"codec")){
	nco_flt_glb=nco_flt_sng2enm(optarg);
	(void)fprintf(stdout,"%s: INFO %s reports user-specified filter string translates to CCR string \"%s\"<.\n",nco_prg_nm,nco_prg_nm,nco_flt_enm2sng((nco_flt_typ_enm)nco_flt_glb_get()));
	nco_exit(EXIT_SUCCESS);
      } /* !ccr */
      if(!strcmp(opt_crr,"fmt_val") || !strcmp(opt_crr,"val_fmt") || !strcmp(opt_crr,"value_format")) fmt_val=(char *)strdup(optarg);
      if(!strcmp(opt_crr,"gaa") || !strcmp(opt_crr,"glb_att_add")){
        gaa_arg=(char **)nco_realloc(gaa_arg,(gaa_nbr+1)*sizeof(char *));
        gaa_arg[gaa_nbr++]=(char *)strdup(optarg);
      } /* !gaa */
      if(!strcmp(opt_crr,"hdf4")) nco_fmt_xtn=nco_fmt_xtn_hdf4; /* [enm] Treat file as HDF4 */
      if(!strcmp(opt_crr,"hdn") || !strcmp(opt_crr,"hidden")) PRN_HDN=True; /* [flg] Print hidden attributes */
      if(!strcmp(opt_crr,"hdr_pad") || !strcmp(opt_crr,"header_pad")){
        hdr_pad=strtoul(optarg,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
        if(*sng_cnv_rcd) nco_sng_cnv_err(optarg,"strtoul",sng_cnv_rcd);
      } /* endif "hdr_pad" */
      if(!strcmp(opt_crr,"help") || !strcmp(opt_crr,"hlp")){
	(void)nco_usg_prn();
	nco_exit(EXIT_SUCCESS);
      } /* endif "help" */
      if(!strcmp(opt_crr,"hpss_try")) HPSS_TRY=True; /* [flg] Search HPSS for unfound files */
      if(!strcmp(opt_crr,"lbr") || !strcmp(opt_crr,"library")){
        (void)nco_lbr_vrs_prn();
        nco_exit(EXIT_SUCCESS);
      } /* endif "lbr" */
      if(!strcmp(opt_crr,"lbr_rcd")) nco_exit_lbr_rcd();
      if(!strcmp(opt_crr,"log_lvl") || !strcmp(opt_crr,"log_level")){
	log_lvl=(int)strtol(optarg,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
	if(*sng_cnv_rcd) nco_sng_cnv_err(optarg,"strtol",sng_cnv_rcd);
	nc_set_log_level(log_lvl);
      } /* !log_lvl */
      if(!strcmp(opt_crr,"lst_rnk_ge2") || !strcmp(opt_crr,"rank_ge2"))	LST_RNK_GE2=True; /* [flg] Print extraction list of rank >= 2 variables */
      if(!strcmp(opt_crr,"lst_xtr") || !strcmp(opt_crr,"xtr_lst")) LST_XTR=True; /* [flg] Print extraction list */
      if(!strcmp(opt_crr,"mk_rec_dmn") || !strcmp(opt_crr,"mk_rec_dim")){
	if(strchr(optarg,',')){
	  (void)fprintf(stdout,"%s: ERROR record dimension name %s contains a comma and appears to be a list\n",nco_prg_nm_get(),optarg);
	  (void)fprintf(stdout,"%s: HINT --mk_rec_dmn currently accepts only one dimension name as an argument (relaxing this limit is TODO nco1129, let us know if this is important to you). To change multiple dimensions into record dimensions, run ncks multiple times and change one dimension each time. Be sure the output file format is netCDF4.\n",nco_prg_nm_get());
	  nco_exit(EXIT_FAILURE);
	} /* endif */
	rec_dmn_nm=strdup(optarg);
      } /* !mk_rec_dmn */
      if(!strcmp(opt_crr,"mpi_implementation")){
        (void)fprintf(stdout,"%s\n",nco_mpi_get());
        nco_exit(EXIT_SUCCESS);
      } /* endif "mpi" */
      if(!strcmp(opt_crr,"md5_dgs") || !strcmp(opt_crr,"md5_digest")){
        if(!md5) md5=nco_md5_ini();
	md5->dgs=True;
        if(nco_dbg_lvl >= nco_dbg_std) (void)fprintf(stderr,"%s: INFO Will perform MD5 digests of input and output hyperslabs\n",nco_prg_nm_get());
      } /* endif "md5_dgs" */
      if(!strcmp(opt_crr,"md5_wrt_att") || !strcmp(opt_crr,"md5_write_attribute")){
        if(!md5) md5=nco_md5_ini();
	md5->wrt=md5->dgs=True;
        if(nco_dbg_lvl >= nco_dbg_fl) (void)fprintf(stderr,"%s: INFO Will write MD5 digests as attributes\n",nco_prg_nm_get());
      } /* endif "md5_wrt_att" */
      if(!strcmp(opt_crr,"msa_usr_rdr") || !strcmp(opt_crr,"msa_user_order")) MSA_USR_RDR=True; /* [flg] Multi-Slab Algorithm returns hyperslabs in user-specified order */
      if(!strcmp(opt_crr,"mta_dlm") || !strcmp(opt_crr,"dlm_mta")) nco_mta_dlm_set(optarg);
      if(!strcmp(opt_crr,"no_abc") || !strcmp(opt_crr,"no-abc") || !strcmp(opt_crr,"no_alphabetize") || !strcmp(opt_crr,"no-alphabetize")) ALPHABETIZE_OUTPUT=False;
      if(!strcmp(opt_crr,"no_blank") || !strcmp(opt_crr,"no-blank") || !strcmp(opt_crr,"noblank")) PRN_MSS_VAL_BLANK=!PRN_MSS_VAL_BLANK;
      if(!strcmp(opt_crr,"no_clb") || !strcmp(opt_crr,"no-clobber") || !strcmp(opt_crr,"no_clobber") || !strcmp(opt_crr,"noclobber")) FORCE_NOCLOBBER=!FORCE_NOCLOBBER;
      if(!strcmp(opt_crr,"no_nm_prn") || !strcmp(opt_crr,"no_dmn_var_nm")) PRN_DMN_VAR_NM=False; /* endif "no_nm_prn" */
      if(!strcmp(opt_crr,"ntm") || !strcmp(opt_crr,"nonatomic") || !strcmp(opt_crr,"udt") || !strcmp(opt_crr,"user_defined_types")) PRN_UDT=True; /* [flg] Print non-atomic variables */
      if(!strcmp(opt_crr,"ppc") || !strcmp(opt_crr,"precision_preserving_compression") || !strcmp(opt_crr,"quantize")) ppc_arg[ppc_nbr++]=(char *)strdup(optarg);
      if(!strcmp(opt_crr,"rad") || !strcmp(opt_crr,"retain_all_dimensions") || !strcmp(opt_crr,"orphan_dimensions") || !strcmp(opt_crr,"rph_dmn")) RETAIN_ALL_DIMS=True;
      if(!strcmp(opt_crr,"ram_all") || !strcmp(opt_crr,"create_ram") || !strcmp(opt_crr,"diskless_all")) RAM_CREATE=True; /* [flg] Create (netCDF3) file(s) in RAM */
      if(!strcmp(opt_crr,"ram_all") || !strcmp(opt_crr,"open_ram") || !strcmp(opt_crr,"diskless_all")) RAM_OPEN=True; /* [flg] Open (netCDF3) file(s) in RAM */
      if(!strcmp(opt_crr,"share_all") || !strcmp(opt_crr,"unbuffered_io") || !strcmp(opt_crr,"uio") || !strcmp(opt_crr,"create_share")) SHARE_CREATE=True; /* [flg] Create (netCDF3) file(s) with unbuffered I/O */
      if(!strcmp(opt_crr,"share_all") || !strcmp(opt_crr,"unbuffered_io") || !strcmp(opt_crr,"uio") || !strcmp(opt_crr,"open_share")) SHARE_OPEN=True; /* [flg] Open (netCDF3) file(s) with unbuffered I/O */
      if(!strcmp(opt_crr,"area_wgt") || !strcmp(opt_crr,"area_weight")){
	flg_area_wgt=True;
	CHK_MAP=True;
      } /* !area_wgt */
      if(!strcmp(opt_crr,"frac_b_nrm")){
	flg_frac_b_nrm=True;
	CHK_MAP=True;
      } /* !frac_b_nrm */
      if(!strcmp(opt_crr,"rgr") || !strcmp(opt_crr,"regridding")){
        flg_rgr=True;
        rgr_arg=(char **)nco_realloc(rgr_arg,(rgr_nbr+1)*sizeof(char *));
        rgr_arg[rgr_nbr++]=(char *)strdup(optarg);
      } /* endif "rgr" */
      if(!strcmp(opt_crr,"rgr_in")) rgr_in=(char *)strdup(optarg);
      if(!strcmp(opt_crr,"rgr_grd_src") || !strcmp(opt_crr,"grd_src") || !strcmp(opt_crr,"src_grd")) rgr_grd_src=(char *)strdup(optarg);
      if(!strcmp(opt_crr,"rgr_grd_dst") || !strcmp(opt_crr,"grd_dst") || !strcmp(opt_crr,"dst_grd")) rgr_grd_dst=(char *)strdup(optarg);
      if(!strcmp(opt_crr,"rgr_map") || !strcmp(opt_crr,"map_file") || !strcmp(opt_crr,"map_fl")){
        flg_rgr=True;
	rgr_map=(char *)strdup(optarg);
      } /* endif rgr_map */
      if(!strcmp(opt_crr,"hrz_fl") || !strcmp(opt_crr,"hrz_crd") || !strcmp(opt_crr,"rgr_hrz")){
	flg_s1d=flg_rgr=True;
	rgr_hrz=(char *)strdup(optarg);
      } /* !vrt_fl */
      if(!strcmp(opt_crr,"vrt_fl") || !strcmp(opt_crr,"vrt_crd") || !strcmp(opt_crr,"rgr_vrt")){
        flg_rgr=True;
	rgr_vrt=(char *)strdup(optarg);
      } /* !vrt_fl */
      if(!strcmp(opt_crr,"rnr_thr") || !strcmp(opt_crr,"rgr_rnr") || !strcmp(opt_crr,"renormalize") || !strcmp(opt_crr,"renormalization_threshold")){
        wgt_vld_thr=strtod(optarg,&sng_cnv_rcd);
        if(*sng_cnv_rcd) nco_sng_cnv_err(optarg,"strtod",sng_cnv_rcd);
      } /* endif rgr_rnr */
      if(!strcmp(opt_crr,"rgr_var")){
        flg_rgr=True;
	rgr_var=(char *)strdup(optarg);
      } /* !rgr_var */
      if(!strcmp(opt_crr,"secret") || !strcmp(opt_crr,"scr") || !strcmp(opt_crr,"shh")){
        (void)fprintf(stdout,"Hidden/unsupported NCO options:\nBit-Adjustment Alg.\t--baa, --bit_alg\nBuild-engine\t\t--bld, --build_engine\nByte-swap algorithm\t--bsa, --byte_swap\nCompiler used\t\t--cmp, --compiler\nCopyright\t\t--cpy, --copyright, --license\nHidden functions\t--scr, --shh, --secret\nLibrary used\t\t--lbr, --library\nLog level\t\t--log_lvl, --log_level\nList rank >= 2 vars\t--lst_rnk_ge2\nList extracted vars\t--lst_xtr\nMemory clean\t\t--mmr_cln, --cln, --clean\nMemory dirty\t\t--mmr_drt, --drt, --dirty\nMPI implementation\t--mpi_implementation\nNo-clobber files\t--no_clb, --no-clobber\nPseudonym\t\t--pseudonym, -Y (ncra only)\nSpinlock\t\t--spinlock\nStreams\t\t\t--srm\nSysconf\t\t\t--sysconf\nTest UDUnits\t\t--tst_udunits,'units_in','units_out','cln_sng'? \nVersion\t\t\t--vrs, --version\n\n");
        nco_exit(EXIT_SUCCESS);
      } /* endif "shh" */
      if(!strcmp(opt_crr,"srm")) PRN_SRM=True; /* [flg] Print ncStream */
      if(!strcmp(opt_crr,"s1d") || !strcmp(opt_crr,"sparse") || !strcmp(opt_crr,"unpack_sparse")) flg_s1d=flg_rgr=True; /* [flg] Unpack sparse-1D CLM/ELM variables */
      if(!strcmp(opt_crr,"sysconf")){
	long maxrss; /* [B] Maximum resident set size */
	maxrss=nco_mmr_usg_prn((int)0);
	maxrss+=0; /* CEWI */
        nco_exit(EXIT_SUCCESS);
      } /* endif "sysconf" */
      if(!strcmp(opt_crr,"trr") || !strcmp(opt_crr,"terraref")){
        flg_trr=True;
        trr_arg=(char **)nco_realloc(trr_arg,(trr_nbr+1)*sizeof(char *));
        trr_arg[trr_nbr++]=(char *)strdup(optarg);
      } /* endif "trr" */
      if(!strcmp(opt_crr,"trr_in")){trr_in=(char *)strdup(optarg);flg_trr=True;}
      if(!strcmp(opt_crr,"trr_wxy")){trr_wxy=(char *)strdup(optarg);flg_trr=True;}
      if(!strcmp(opt_crr,"tst_udunits")){ 
	/* Use this feature with, e.g.,
	   ncks --tst_udunits='5 meters',centimeters ~/nco/data/in.nc
	   ncks --tst_udunits='0 days since 1918-11-11','days since 1939-09-09',standard ~/nco/data/in.nc
	   ncks --tst_udunits='0 days since 1918-11-11','days since 1939-09-09',360_day ~/nco/data/in.nc
	   ncks --tst_udunits='0 days since 1918-11-11','days since 1939-09-09',365_day ~/nco/data/in.nc
	   ncks --tst_udunits='0 days since 1918-11-11','days since 1939-09-09',366_day ~/nco/data/in.nc */
        char *cp;
        char **args;
        double crr_val;
#ifndef ENABLE_UDUNITS
        (void)fprintf(stdout,"%s: Build NCO with UDUnits support to use this option\n",nco_prg_nm_get());
        nco_exit(EXIT_FAILURE);
#endif /* !ENABLE_UDUNITS */
        cp=strdup(optarg); 
        args=nco_lst_prs_1D(cp,",",&lmt_nbr);         
        nco_cln_clc_dbl_org(args[0],args[1],(lmt_nbr > 2 ? nco_cln_get_cln_typ(args[2]) : cln_nil),&crr_val);        
        (void)fprintf(stdout,"Value+Units in=%s, units out=%s, time-difference (for dates) or value-conversion (for non-dates) = %f\n",args[0],args[1],crr_val);
        if(cp) cp=(char *)nco_free(cp);
        nco_exit(EXIT_SUCCESS);
      } /* endif "tst_udunits" */
      if(!strcmp(opt_crr,"unn") || !strcmp(opt_crr,"union")) GRP_VAR_UNN=True;
      if(!strcmp(opt_crr,"nsx") || !strcmp(opt_crr,"intersection")) GRP_VAR_UNN=False;
      if(!strcmp(opt_crr,"grp_xtr_var_xcl")){
	EXCLUDE_INPUT_LIST=True;
	GRP_XTR_VAR_XCL=True;
      } /* endif "grp_xtr_var_xcl" */
      if(!strcmp(opt_crr,"vrs") || !strcmp(opt_crr,"version")){
        (void)nco_vrs_prn(CVS_Id,CVS_Revision);
        nco_exit(EXIT_SUCCESS);
      } /* endif "vrs" */
      if(!strcmp(opt_crr,"jsn_fmt") || !strcmp(opt_crr,"json_format") || !strcmp(opt_crr,"json_fmt") || !strcmp(opt_crr,"jsn_format")){
	PRN_JSN=True;
	JSN_ATT_FMT=(int)strtoul(optarg,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
	if(*sng_cnv_rcd) nco_sng_cnv_err(optarg,"strtoul",sng_cnv_rcd);
	if(JSN_ATT_FMT >= 4) JSN_DATA_BRK=False; /* [flg] Print JSON with bracket data */
	JSN_ATT_FMT%=4; /* 20161221: Valid values are 0,1,2 */
	if(JSN_ATT_FMT == 3) JSN_ATT_FMT=2;
	JSN_VAR_FMT=JSN_ATT_FMT;
      } /* !jsn_att_fmt */
      if(!strcmp(opt_crr,"jsn") || !strcmp(opt_crr,"json") || !strcmp(opt_crr,"w10") || !strcmp(opt_crr,"w10n")) PRN_JSN=True; /* [flg] Print JSON */
      if(!strcmp(opt_crr,"wrt_tmp_fl") || !strcmp(opt_crr,"write_tmp_fl")) WRT_TMP_FL=True;
      if(!strcmp(opt_crr,"no_tmp_fl")) WRT_TMP_FL=False;
      if(!strcmp(opt_crr,"xml") || !strcmp(opt_crr,"ncml")) PRN_XML=True; /* [flg] Print XML (NcML) */
      if(!strcmp(opt_crr,"xml_no_location") || !strcmp(opt_crr,"ncml_no_location")){PRN_XML_LOCATION=False;PRN_XML=True;} /* [flg] Print XML location tag */
      if(!strcmp(opt_crr,"xml_spr_chr")){spr_chr=(char *)strdup(optarg);PRN_XML=True;} /* [flg] Separator for XML character types */
      if(!strcmp(opt_crr,"xml_spr_nmr")){spr_nmr=(char *)strdup(optarg);PRN_XML=True;} /* [flg] Separator for XML numeric types */
      if(!strcmp(opt_crr,"xtn_var_lst") || !strcmp(opt_crr,"extensive")){
	/* Extensive variables */
	optarg_lcl=(char *)strdup(optarg);
	(void)nco_rx_comma2hash(optarg_lcl);
	xtn_lst_in=nco_lst_prs_2D(optarg_lcl,",",&xtn_nbr);
	optarg_lcl=(char *)nco_free(optarg_lcl);
      } /* !xtn */
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
      FORCE_APPEND=!FORCE_APPEND;
      break;
    case 'a': /* Do not alphabetize output */
      ALPHABETIZE_OUTPUT=False;
      (void)fprintf(stderr,"%s: WARNING the ncks '-a', '--abc', and '--alphabetize' switches are misleadingly named because they turn-off the default alphabetization. These switches are deprecated as of NCO 4.7.1 and will soon be deleted. Instead, please use the new long options --no_abc, --no-abc, --no_alphabetize, or --no-alphabetize, all of which turn-off the default alphabetization.\n",nco_prg_nm_get());
      break;
    case 'b': /* Set file for binary output */
      fl_bnr=(char *)strdup(optarg);
      break;
    case 'C': /* Extract all coordinates associated with extracted variables? */
      EXTRACT_ASSOCIATED_COORDINATES=!EXTRACT_ASSOCIATED_COORDINATES;
      break;
    case 'c': /* Add all coordinates to extraction list? */
      EXTRACT_ALL_COORDINATES=True;
      break;
    case 'D': /* Debugging level. Default is 0. */
      nco_dbg_lvl=(unsigned short int)strtoul(optarg,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
      if(*sng_cnv_rcd) nco_sng_cnv_err(optarg,"strtoul",sng_cnv_rcd);
      break;
    case 'd': /* Copy limit argument for later processing */
      lmt_arg[lmt_nbr]=(char *)strdup(optarg);
      lmt_nbr++;
      HAVE_LIMITS=True;
      break;
    case 'F': /* Toggle index convention. Default is 0-based arrays (C-style). */
      FORTRAN_IDX_CNV=!FORTRAN_IDX_CNV;
      break;
    case 'G': /* Apply Group Path Editing (GPE) to output group */
      gpe=nco_gpe_prs_arg(optarg);
      /*      fl_out_fmt=NC_FORMAT_NETCDF4; */
      break;
    case 'g': /* Copy group argument for later processing */
      /* Replace commas with hashes when within braces (convert back later) */
      optarg_lcl=(char *)strdup(optarg);
      (void)nco_rx_comma2hash(optarg_lcl);
      grp_lst_in=nco_lst_prs_2D(optarg_lcl,",",&grp_lst_in_nbr);
      optarg_lcl=(char *)nco_free(optarg_lcl);
      break;
    case 'H': /* Toggle printing data to screen */
      PRN_VAR_DATA_TGL=True;
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
    case 'm': /* Toggle printing variable metadata to screen */
      PRN_VAR_METADATA_TGL=True;
      break;
    case 'M': /* Toggle printing global metadata to screen */
      PRN_GLB_METADATA_TGL=True;
      break;
    case 'O': /* Toggle FORCE_OVERWRITE */
      FORCE_OVERWRITE=!FORCE_OVERWRITE;
      break;
    case 'o': /* Name of output file */
      fl_out=(char *)strdup(optarg);
      break;
    case 'P': /* Print data to screen, maximal verbosity */
      PRN_VRB=True;
      EXTRACT_ASSOCIATED_COORDINATES=!EXTRACT_ASSOCIATED_COORDINATES;
      break;
    case 'p': /* Common file path */
      fl_pth=(char *)strdup(optarg);
      break;
    case 'q': /* [flg] Quench (turn-off) all printing to screen */
      PRN_QUENCH=True; /* [flg] Quench (turn-off) all printing to screen */
      break;
    case 'Q': /* Turn off printing of dimension indices and coordinate values */
      PRN_DMN_IDX_CRD_VAL=!PRN_DMN_IDX_CRD_VAL;
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
#ifdef ENABLE_MPI
    case 'S': /* Suspend with signal handler to facilitate debugging */
      if(signal(SIGUSR1,nco_cnt_run) == SIG_ERR) (void)fprintf(stdout,"%s: ERROR Could not install suspend handler.\n",nco_prg_nm);
      while(!nco_spn_lck_brk) usleep(nco_spn_lck_us); /* Spinlock. fxm: should probably insert a sched_yield */
      break;
#endif /* !ENABLE_MPI */
    case 's': /* User-specified delimiter string for traditional printed output */
      PRN_TRD=True; /* [flg] Print traditional */
      dlm_sng=(char *)strdup(optarg);
      break;
    case 't': /* Thread number */
      thr_nbr=(int)strtol(optarg,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
      if(*sng_cnv_rcd) nco_sng_cnv_err(optarg,"strtol",sng_cnv_rcd);
      break;
    case 'u': /* Toggle printing dimensional units */
      PRN_DMN_UNITS_TGL=True;
      break;
    case 'V': /* Print variable values only (same as -Q --no_nm_prn) */
      PRN_DMN_IDX_CRD_VAL=False;
      PRN_DMN_VAR_NM=False;
      break;
    case 'v': /* Variables to extract/exclude */
      /* Replace commas with hashes when within braces (convert back later) */
      optarg_lcl=(char *)strdup(optarg);
      (void)nco_rx_comma2hash(optarg_lcl);
      var_lst_in=nco_lst_prs_2D(optarg_lcl,",",&var_lst_in_nbr);
      optarg_lcl=(char *)nco_free(optarg_lcl);
      xtr_nbr=var_lst_in_nbr;
      break;
    case 'X': /* Copy auxiliary coordinate argument for later processing */
      aux_arg[aux_nbr]=(char *)strdup(optarg);
      aux_nbr++;
      MSA_USR_RDR=True; /* [flg] Multi-Slab Algorithm returns hyperslabs in user-specified order */
      HAVE_LIMITS=True;
      break;
    case 'x': /* Exclude rather than extract groups and variables specified with -v */
      EXCLUDE_INPUT_LIST=True;
      break;
    case 'z': /* Print absolute path of all input variables then exit */
      trv_tbl_prn(trv_tbl);
      goto close_and_free; 
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

  /* 20170107: Unlike all other operators, ncks may benefit from setting chunk cache when input file (not output file) is netCDF4 because there is anecdotal evidence that ncdump netCDF4 print speed may be improved by cache adjustments. We cannot verify whether input, output, or both file formats are netCDF4 because nco_set_chunk_cache() must be called before opening file(s). Setting this for netCDF3 library is harmless and calls a no-op stub function */
  /* Set/report global chunk cache */
  rcd+=nco_cnk_csh_ini(cnk_csh_byt);

#ifdef _LANGINFO_H
/* Internationalization i18n
   Linux Journal 200211 p. 57--59 http://www.linuxjournal.com/article/6176 
   Fedora: http://fedoraproject.org/wiki/How_to_do_I18N_through_gettext
   cd ~/nco/bld;make I18N=Y
   cd ~/nco/bld;xgettext --default-domain=nco --join-existing -o ../po/nco.pot ../src/nco/ncks.c ../src/nco/ncra.c
   for LL in fr es; do
     mkdir -p ~/share/locale/${LL}/LC_MESSAGES
     msgfmt ~/nco/po/${LL}/nco.po -o ~/nco/po/${LL}/nco.mo
     /bin/cp ~/nco/po/${LL}/nco.mo ~/share/locale/${LL}/LC_MESSAGES
#     sudo /bin/cp ~/nco/po/${LL}/nco.mo /usr/share/locale/${LL}/LC_MESSAGES  
   done
   export LOCALEDIR=${HOME}/share/locale
   LC_ALL=en ncks -D 1 -O ~/nco/data/in.nc ~/foo.nc
   LANG=en_GB.utf8 LANGUAGE=en_GB:en:fr_FR:fr LC_ALL=en_GB.utf8 ncks -D 1 -O ~/nco/data/in.nc ~/foo.nc
   LANG=es ncks -D 1 -O ~/nco/data/in.nc ~/foo.nc
   LANG=fr_FR.utf8 LANGUAGE=fr_FR:fr:en_GB:en LC_ALL=fr_FR.utf8 ncks -D 1 -O ~/nco/data/in.nc ~/foo.nc */
  if(nco_dbg_lvl >= nco_dbg_std) (void)fprintf(stdout,gettext("%s: I18N Current charset = %s\n"),nco_prg_nm,nl_langinfo(CODESET));
  if(nco_dbg_lvl >= nco_dbg_std) (void)fprintf(stdout,gettext("%s: I18N This text may appear in a foreign language\n"),nco_prg_nm);
#endif /* !_LANGINFO_H */

  #if defined(ENABLE_GSL)
    gsl_set_error_handler_off();
  #endif /* !ENABLE_GSL */

  /* Initialize traversal table */
  (void)trv_tbl_init(&trv_tbl);
 
  /* Process positional arguments and fill-in filenames */
  fl_lst_in=nco_fl_lst_mk(argv,argc,optind,&fl_nbr,&fl_out,&FL_LST_IN_FROM_STDIN,FORCE_OVERWRITE);
  
  /* Initialize thread information */
  if(flg_rgr) thr_nbr=nco_openmp_ini(thr_nbr); else thr_nbr=nco_openmp_ini((int)1);
  in_id_arr=(int *)nco_malloc(thr_nbr*sizeof(int));
  trv_tbl->thr_nbr=thr_nbr;
  trv_tbl->in_id_arr=in_id_arr;

  /* Parse filename */
  fl_in=nco_fl_nm_prs(fl_in,0,&fl_nbr,fl_lst_in,abb_arg_nbr,fl_lst_abb,fl_pth);
  /* Make dummy input file (this step must precede nco_fl_mk_lcl()) */
  if(flg_dmm_in) nco_fl_dmm_mk(fl_in);
  /* Make sure file is on local system and is readable or die trying */
  fl_in=nco_fl_mk_lcl(fl_in,fl_pth_lcl,HPSS_TRY,&FL_RTR_RMT_LCN);
  /* Open file using appropriate buffer size hints and verbosity */
  if(RAM_OPEN) md_open=NC_NOWRITE|NC_DISKLESS; else md_open=NC_NOWRITE;
  if(SHARE_OPEN) md_open=md_open|NC_SHARE;
  for(thr_idx=0;thr_idx<thr_nbr;thr_idx++) rcd+=nco_fl_open(fl_in,md_open,&bfr_sz_hnt,in_id_arr+thr_idx);
  in_id=in_id_arr[0];
  
  /* Construct GTT (Group Traversal Table), check -v and -g input names and create extraction list */
  (void)nco_bld_trv_tbl(in_id,trv_pth,lmt_nbr,lmt_arg,aux_nbr,aux_arg,MSA_USR_RDR,FORTRAN_IDX_CNV,grp_lst_in,grp_lst_in_nbr,var_lst_in,xtr_nbr,EXTRACT_ALL_COORDINATES,GRP_VAR_UNN,GRP_XTR_VAR_XCL,EXCLUDE_INPUT_LIST,EXTRACT_ASSOCIATED_COORDINATES,EXTRACT_CLL_MSR,EXTRACT_FRM_TRM,nco_pck_plc_nil,&flg_dne,trv_tbl);

  if(ALPHABETIZE_OUTPUT) trv_tbl_srt(srt_mth,trv_tbl);

  /* [fnc] Print extraction list and exit */
  if(LST_XTR) nco_xtr_lst(trv_tbl);

  /* [fnc] Print extraction list of N>=D variables and exit */
  if(LST_RNK_GE2) nco_xtr_ND_lst(trv_tbl);

  /* Were all user-specified dimensions found? */ 
  (void)nco_chk_dmn(lmt_nbr,flg_dne);    

  /* Get number of variables, dimensions, and global attributes in file */
  (void)trv_tbl_inq(&att_glb_nbr,&att_grp_nbr,&att_var_nbr,&dmn_nbr_fl,&dmn_rec_fl,&grp_dpt_fl,&grp_nbr_fl,&var_udt_fl,&var_nbr_fl,trv_tbl);

  /* Make output and input files consanguinous */
  (void)nco_inq_format(in_id,&fl_in_fmt);
  if(fl_out_fmt == NCO_FORMAT_UNDEFINED) fl_out_fmt=fl_in_fmt;
  if(fl_out_fmt == NC_FORMAT_NETCDF4 || fl_out_fmt == NC_FORMAT_NETCDF4_CLASSIC) (void)omp_set_num_threads((int)0);

#ifdef ENABLE_MPI
  if(prc_rnk == rnk_mgr) (void)fprintf(stdout,"%s: MPI process rank %d reports %d process%s\n",nco_prg_nm,prc_rnk,prc_nbr,(prc_nbr == 1) ? "" : "es");
  /* Roll call */
  (void)fprintf(stdout,"%s: MPI process rank %d reports %d process%s\n",nco_prg_nm,prc_rnk,prc_nbr,(prc_nbr == 1) ? "" : "es");
#endif /* !ENABLE_MPI */

  /* We now have final list of variables to extract. Phew. */

  if(fl_out){
    /* Copy everything (all data and metadata) to output file by default */
    if(PRN_VAR_DATA_TGL) PRN_VAR_DATA=False; else PRN_VAR_DATA=True;
    if(PRN_VAR_METADATA_TGL) PRN_VAR_METADATA=False; else PRN_VAR_METADATA=True;
    if(PRN_GLB_METADATA_TGL) PRN_GLB_METADATA=False; else PRN_GLB_METADATA=True;
    if(FORCE_APPEND){
      /* When appending, do not copy global metadata by default */
      if(var_lst_in) PRN_GLB_METADATA=False; else PRN_GLB_METADATA=True;
      if(PRN_GLB_METADATA_TGL) PRN_GLB_METADATA=!PRN_GLB_METADATA;
    } /* !FORCE_APPEND */
  }else{ /* !fl_out */
    /* Only input file is specified, so print it */
    if(PRN_VRB || (!PRN_VAR_DATA_TGL && !PRN_VAR_METADATA_TGL && !PRN_GLB_METADATA_TGL)){
      /* Verbose printing simply means assume user wants deluxe frills by default */
      if(PRN_DMN_UNITS_TGL) PRN_DMN_UNITS=False; else PRN_DMN_UNITS=True;
      if(PRN_VAR_DATA_TGL) PRN_VAR_DATA=False; else PRN_VAR_DATA=True;
      if(PRN_VAR_METADATA_TGL) PRN_VAR_METADATA=False; else PRN_VAR_METADATA=True;
      /* Assume user wants global metadata unless variable extraction is invoked */
      if(var_lst_in == NULL) PRN_GLB_METADATA=True;
      if(PRN_GLB_METADATA_TGL) PRN_GLB_METADATA=!PRN_GLB_METADATA;
    }else{ /* end if PRN_VRB */
      /* Default is to print data and metadata to screen if output file is not specified */
      if(PRN_DMN_UNITS_TGL) PRN_DMN_UNITS=True; else PRN_DMN_UNITS=False;
      if(PRN_VAR_DATA_TGL) PRN_VAR_DATA=True; else PRN_VAR_DATA=False;
      if(PRN_VAR_METADATA_TGL) PRN_VAR_METADATA=True; else PRN_VAR_METADATA=False;
      if(PRN_GLB_METADATA_TGL) PRN_GLB_METADATA=True; else PRN_GLB_METADATA=False;
    } /* !PRN_VRB */  
    /* PRN_QUENCH turns off all printing to screen */
    if(PRN_QUENCH) PRN_VAR_DATA=PRN_VAR_METADATA=PRN_GLB_METADATA=False;
  } /* !fl_out */  

  if(fl_bnr && !fl_out){
    /* Native binary files depend on writing netCDF file to enter generic I/O logic */
    (void)fprintf(stdout,"%s: ERROR Native binary files cannot be written unless netCDF output filename also specified.\nHINT: Repeat command with dummy netCDF file specified for output file (e.g., -o foo.nc)\n",nco_prg_nm_get());
    nco_exit(EXIT_FAILURE);
  } /* endif fl_bnr */
    
  if(flg_rgr && !fl_out){
    (void)fprintf(stdout,"%s: ERROR Regridding requested but no output file specified\nHINT: Specify output file with \"-o fl_out\" or as last argument\n",nco_prg_nm_get());
    nco_exit(EXIT_FAILURE);
  } /* !flg_rgr */
    
  if(gpe){
    if(nco_dbg_lvl >= nco_dbg_fl) (void)fprintf(stderr,"%s: INFO Group Path Edit (GPE) feature enabled\n",nco_prg_nm_get());
    if(fl_out && fl_out_fmt != NC_FORMAT_NETCDF4 && nco_dbg_lvl >= nco_dbg_std) (void)fprintf(stderr,"%s: WARNING Group Path Edit (GPE) requires netCDF4 output format in most cases (except flattening) but user explicitly requested output format = %s. This command will fail if the output file requires netCDF4 features like groups, non-atomic types, or multiple record dimensions. However, it _will_ autoconvert netCDF4 atomic types (e.g., NC_STRING, NC_UBYTE...) to netCDF3 atomic types (e.g., NC_CHAR, NC_SHORT...).\n",nco_prg_nm_get(),nco_fmt_sng(fl_out_fmt));
  } /* !gpe */

  /* Terraref */
  if(flg_trr){
    char *trr_out;
    trr_sct *trr_nfo;
    trr_out=(char *)strdup(fl_out);
    trr_nfo=nco_trr_ini(cmd_ln,dfl_lvl,trr_arg,trr_nbr,trr_in,trr_out,trr_wxy);
    (void)nco_trr_read(trr_nfo);
    /* Free Terraref structure */
    trr_nfo=nco_trr_free(trr_nfo);
    if(trr_wxy) trr_wxy=(char *)nco_free(trr_wxy);
    nco_exit_gracefully();
    return EXIT_SUCCESS;
  } /* !Terraref */
  
  if(fl_out){
    /* Output file was specified so PRN_ tokens refer to (meta)data copying */
    int out_id;
    
    /* Make output and input files consanguinous */
    if(fl_out_fmt == NCO_FORMAT_UNDEFINED) fl_out_fmt=fl_in_fmt;

    /* Regridding */
    if(flg_rgr){
      rgr_sct *rgr_nfo;
      /* Initialize regridding structure */
      rgr_in=(char *)strdup(fl_in);
      rgr_out=(char *)strdup(fl_out);
      rgr_nfo=nco_rgr_ini(cmd_ln,in_id,rgr_arg,rgr_nbr,rgr_in,rgr_out,rgr_grd_src,rgr_grd_dst,rgr_hrz,rgr_map,rgr_var,rgr_vrt,wgt_vld_thr,xtn_lst_in,xtn_nbr);
      rgr_nfo->fl_out_fmt=fl_out_fmt;
      rgr_nfo->dfl_lvl=dfl_lvl;
      rgr_nfo->hdr_pad=hdr_pad;
      rgr_nfo->flg_area_out=EXTRACT_CLL_MSR; /* [flg] Add area to output */
      rgr_nfo->flg_s1d=flg_s1d; /* [flg] Unpack sparse-1D CLM/ELM variables */
      rgr_nfo->flg_uio=SHARE_CREATE;
      rgr_nfo->fl_out_tmp=nco_fl_out_open(rgr_nfo->fl_out,&FORCE_APPEND,FORCE_OVERWRITE,fl_out_fmt,&bfr_sz_hnt,RAM_CREATE,RAM_OPEN,SHARE_CREATE,SHARE_OPEN,WRT_TMP_FL,&out_id);

      /* Copy Global Metadata */
      rgr_nfo->out_id=out_id;
      rgr_nfo->thr_nbr=thr_nbr;
      nco_bool PCK_ATT_CPY=True; /* [flg] Copy attributes "scale_factor", "add_offset" */
      (void)nco_att_cpy(in_id,out_id,NC_GLOBAL,NC_GLOBAL,PCK_ATT_CPY);
      /* Catenate time-stamped command line to "history" global attribute */
      if(HISTORY_APPEND) (void)nco_hst_att_cat(out_id,cmd_ln);
      if(gaa_nbr > 0) (void)nco_glb_att_add(out_id,gaa_arg,gaa_nbr);
      if(HISTORY_APPEND) (void)nco_vrs_att_cat(out_id);
      if(thr_nbr > 1 && HISTORY_APPEND) (void)nco_thr_att_cat(out_id,thr_nbr);

      /* Generate grids/maps or regrid horizontally/vertically */
      rcd=nco_rgr_ctl(rgr_nfo,trv_tbl);
      /* Change from NCO_NOERR to NC_NOERR */
      rcd=NC_NOERR;

      /* Close output file and move it from temporary to permanent location */
      (void)nco_fl_out_cls(rgr_nfo->fl_out,rgr_nfo->fl_out_tmp,out_id);

      /* Free regridding structure */
      rgr_nfo=nco_rgr_free(rgr_nfo);
    } /* endif !flg_rgr */

    if(!flg_rgr){
      
      /* Initialize, decode, and set PPC information */
      if(ppc_nbr > 0) nco_ppc_ini(in_id,&dfl_lvl,fl_out_fmt,ppc_arg,ppc_nbr,trv_tbl);
      
      /* Verify output file format supports requested actions */
      (void)nco_fl_fmt_vet(fl_out_fmt,cnk_nbr,dfl_lvl);
      
      /* Open output file */
      fl_out_tmp=nco_fl_out_open(fl_out,&FORCE_APPEND,FORCE_OVERWRITE,fl_out_fmt,&bfr_sz_hnt,RAM_CREATE,RAM_OPEN,SHARE_CREATE,SHARE_OPEN,WRT_TMP_FL,&out_id);
    
      /* Initialize chunking from user-specified inputs */
      if(fl_out_fmt == NC_FORMAT_NETCDF4 || fl_out_fmt == NC_FORMAT_NETCDF4_CLASSIC) rcd+=nco_cnk_ini(in_id,fl_out,cnk_arg,cnk_nbr,cnk_map,cnk_plc,cnk_csh_byt,cnk_min_byt,cnk_sz_byt,cnk_sz_scl,&cnk);

      /* Define extracted groups, variables, and attributes in output file */
      CPY_GRP_METADATA=PRN_GLB_METADATA;
      (void)nco_xtr_dfn(in_id,out_id,&cnk,dfl_lvl,gpe,md5,CPY_GRP_METADATA,PRN_VAR_METADATA,RETAIN_ALL_DIMS,nco_pck_plc_nil,rec_dmn_nm,trv_tbl);

      /* Catenate time-stamped command line to "history" global attribute */
      if(HISTORY_APPEND) (void)nco_hst_att_cat(out_id,cmd_ln);
      if(HISTORY_APPEND && FORCE_APPEND) (void)nco_prv_att_cat(fl_in,in_id,out_id);
      if(gaa_nbr > 0) (void)nco_glb_att_add(out_id,gaa_arg,gaa_nbr);
      if(HISTORY_APPEND) (void)nco_vrs_att_cat(out_id);
#ifdef ENABLE_MPI
      if(prc_rnk == rnk_mgr)
	if(prc_nbr > 0 && HISTORY_APPEND) (void)nco_mpi_att_cat(out_id,prc_nbr);
#endif /* !ENABLE_MPI */
      
      /* Turn-off default filling behavior to enhance efficiency */
      nco_set_fill(out_id,NC_NOFILL,&fll_md_old);
      
      /* Take output file out of define mode */
      if(hdr_pad == 0UL){
	(void)nco_enddef(out_id);
      }else{
	(void)nco__enddef(out_id,hdr_pad);
	if(nco_dbg_lvl >= nco_dbg_scl) (void)fprintf(stderr,"%s: INFO Padding header with %lu extra bytes\n",nco_prg_nm_get(),(unsigned long)hdr_pad);
      } /* hdr_pad */

      /* [fnc] Open unformatted binary data file for writing */
      if(fl_bnr) fp_bnr=nco_bnr_open(fl_bnr,"w");
      
      /* Timestamp end of metadata setup and disk layout */
      rcd+=nco_ddra((char *)NULL,(char *)NULL,&ddra_info);
      ddra_info.tmr_flg=nco_tmr_rgl;
      /* Write extracted data to output file */
      if(PRN_VAR_DATA) (void)nco_xtr_wrt(in_id,out_id,gpe,fp_bnr,md5,HAVE_LIMITS,trv_tbl);
      
      /* [fnc] Close unformatted binary data file */
      if(fp_bnr) (void)nco_bnr_close(fp_bnr,fl_bnr);
      
      if(nco_dbg_lvl_get() == 14){
	(void)nco_wrt_trv_tbl(in_id,trv_tbl,True);
	(void)nco_wrt_trv_tbl(out_id,trv_tbl,True);
      } /* endif dbg */

      /* Close output file and move it from temporary to permanent location */
      (void)nco_fl_out_cls(fl_out,fl_out_tmp,out_id);
    
    } /* flg_rgr */
    
  }else{ /* !fl_out */

    nco_bool ALPHA_BY_FULL_GROUP=False; /* [flg] Print alphabetically by full group */
    nco_bool ALPHA_BY_STUB_GROUP=True; /* [flg] Print alphabetically by stub group */
    char *fl_nm_stub;
    char *fl_in_dpl=NULL;
    char *sfx_ptr;

    /* Update all GTT dimensions with hyperslabbed size */
    (void)nco_dmn_trv_msa_tbl(in_id,rec_dmn_nm,trv_tbl);   

    /* No output file was specified so PRN_ tokens refer to screen printing */
    prn_fmt_sct prn_flg;
    /* Printing defaults to stdout */
    prn_flg.fp_out=stdout;
    PRN_CDL=PRN_CDL || !(PRN_TRD || PRN_XML || PRN_JSN); // 20170817
    prn_flg.cdl=PRN_CDL;
    prn_flg.trd=PRN_TRD;
    prn_flg.jsn=PRN_JSN;
    prn_flg.srm=PRN_SRM;
    prn_flg.xml=PRN_XML;
    if((prn_flg.cdl || prn_flg.xml) && nco_dbg_lvl >= nco_dbg_std) prn_flg.nfo_xtr=True; else prn_flg.nfo_xtr=False;
    prn_flg.new_fmt=(PRN_CDL || PRN_JSN || PRN_SRM || PRN_XML);
    prn_flg.hdn=PRN_HDN;
    prn_flg.udt=PRN_UDT;
    prn_flg.rad=RETAIN_ALL_DIMS;
    /* CDL must print filename stub */
    if(prn_flg.cdl || prn_flg.xml){
      fl_in_dpl=strdup(fl_in);
      fl_nm_stub=strrchr(fl_in_dpl,'/');
      if(fl_nm_stub) fl_nm_stub++; else fl_nm_stub=fl_in_dpl;
      sfx_ptr=strrchr(fl_nm_stub,'.');
      if(sfx_ptr) *sfx_ptr='\0';
      prn_flg.fl_stb=fl_nm_stub;
    } /* endif CDL */
    /* JSON and XML need filename (unless location will be omitted) */
    if(prn_flg.xml || prn_flg.jsn) prn_flg.fl_in=fl_in;
    prn_flg.spr_nmr=spr_nmr;
    prn_flg.spr_chr=spr_chr;
    prn_flg.xml_lcn=PRN_XML_LOCATION;
    prn_flg.gpe=gpe;
    prn_flg.md5=md5;
    prn_flg.ndn=0; /* Initialize for prn_flg->trd */
    prn_flg.spc_per_lvl=2;
    prn_flg.sxn_fst=2;
    prn_flg.var_fst=2;
    prn_flg.tab=4;
    prn_flg.nbr_zro=0;  
    if(nco_dbg_lvl >= nco_dbg_scl) prn_flg.fll_pth=True; else prn_flg.fll_pth=False;
    if(prn_flg.xml) prn_flg.nwl_pst_val=False; else prn_flg.nwl_pst_val=True;
    prn_flg.dlm_sng=dlm_sng;
    if(PRN_CLN_LGB && dt_fmt == fmt_dt_nil) dt_fmt=fmt_dt_sht;
    prn_flg.cdl_fmt_dt=dt_fmt;
    prn_flg.fl_out_fmt=fl_out_fmt;
    prn_flg.fmt_val=fmt_val;
    prn_flg.ALPHA_BY_FULL_GROUP=ALPHA_BY_FULL_GROUP;
    prn_flg.ALPHA_BY_STUB_GROUP=ALPHA_BY_STUB_GROUP;
    prn_flg.FORTRAN_IDX_CNV=FORTRAN_IDX_CNV;
    prn_flg.PRN_DMN_IDX_CRD_VAL=PRN_DMN_IDX_CRD_VAL;
    prn_flg.PRN_DMN_UNITS=PRN_DMN_UNITS;
    prn_flg.PRN_DMN_VAR_NM=PRN_DMN_VAR_NM;
    prn_flg.PRN_GLB_METADATA=PRN_GLB_METADATA;
    prn_flg.PRN_MSS_VAL_BLANK=PRN_MSS_VAL_BLANK;
    prn_flg.PRN_VAR_DATA=PRN_VAR_DATA;
    prn_flg.PRN_VAR_METADATA=PRN_VAR_METADATA;
    prn_flg.PRN_CLN_LGB=PRN_CLN_LGB;

    /* Derived formats */
    if(prn_flg.cdl){
      prn_flg.PRN_DMN_UNITS=True;
      prn_flg.PRN_DMN_VAR_NM=True;
      prn_flg.PRN_MSS_VAL_BLANK=True;
    } /* endif */

    if(prn_flg.jsn){
      /* In JSON numeric notation, a terminal decimal place, like 0. and 20., is invalid---correct is 0.0, 20.0 */
      prn_flg.nbr_zro=1;
      /* JSON numerical arrays have no notion of missing values */
      prn_flg.PRN_MSS_VAL_BLANK=False;
      prn_flg.jsn_data_brk=JSN_DATA_BRK;
      prn_flg.jsn_var_fmt=JSN_VAR_FMT;
    }else { /* endif JSON */
      prn_flg.jsn_att_fmt=0;
      prn_flg.jsn_data_brk=False;
      prn_flg.jsn_var_fmt=2;
    } /* !JSON */
      
    if(prn_flg.xml) prn_flg.PRN_MSS_VAL_BLANK=False;

    /* File summary */
    if(PRN_GLB_METADATA){
      prn_flg.smr_sng=smr_sng=(char *)nco_malloc((strlen(fl_in)+300L*sizeof(char))); /* [sng] File summary string */
      smr_xtn_sng=(char *)nco_malloc(300L*sizeof(char)); /* [sng] File extended summary string */
      if(nco_dbg_lvl > nco_dbg_std) (void)sprintf(smr_xtn_sng," (representation of extended/underlying filetype %s)",nco_fmt_xtn_sng(nco_fmt_xtn_get())); else smr_xtn_sng[0]='\0';
      (void)sprintf(smr_sng,"Summary of %s: filetype = %s%s, %i groups (max. depth = %i), %i dimensions (%i fixed, %i record), %i variables (%i atomic, %i user-defined types), %i attributes (%i global, %i group, %i variable)",fl_in,nco_fmt_sng(fl_in_fmt),smr_xtn_sng,grp_nbr_fl,grp_dpt_fl,trv_tbl->nbr_dmn,trv_tbl->nbr_dmn-dmn_rec_fl,dmn_rec_fl,var_nbr_fl,var_nbr_fl-var_udt_fl,var_udt_fl,att_glb_nbr+att_grp_nbr+att_var_nbr,att_glb_nbr,att_grp_nbr,att_var_nbr);

      if(nco_dbg_lvl > nco_dbg_std){
	prn_flg.smr_fl_sz_sng=smr_fl_sz_sng=(char *)nco_malloc(300L*sizeof(char)); /* [sng] String describing estimated file size */
 	(void)nco_fl_sz_est(smr_fl_sz_sng,trv_tbl);
      } /* !dbg */
    } /* endif summary */

    if(fl_prn){
      if((fp_prn=fopen(fl_prn,"w")) == NULL){
	(void)fprintf(stderr,"%s: ERROR unable to open formatted output file %s\n",nco_prg_nm_get(),fl_prn);
	nco_exit(EXIT_FAILURE);
      } /* !fp_prn */
      prn_flg.fp_out=fp_prn;
    } /* !fl_prn */

    if(!prn_flg.new_fmt){
      /* Traditional printing order/format always used prior to 201307 */
      if(PRN_GLB_METADATA){
        int dmn_ids_rec[NC_MAX_DIMS]; /* [ID] Record dimension IDs array */
        int nbr_rec_lcl; /* [nbr] Number of record dimensions visible in root */
        /* Get unlimited dimension information from input file/group */
        rcd=nco_inq_unlimdims(in_id,&nbr_rec_lcl,dmn_ids_rec);
        if(nbr_rec_lcl > 0){
          char dmn_nm[NC_MAX_NAME]; 
          long rec_dmn_sz;
          for(int rec_idx=0;rec_idx<nbr_rec_lcl;rec_idx++){
            (void)nco_inq_dim(in_id,dmn_ids_rec[rec_idx],dmn_nm,&rec_dmn_sz);
            (void)fprintf(prn_flg.fp_out,"Root record dimension %d: name = %s, size = %li\n",rec_idx,dmn_nm,rec_dmn_sz);
          } /* end loop over rec_idx */
          (void)fprintf(stdout,"\n");
        } /* NCO_REC_DMN_UNDEFINED */
        /* Print group attributes recursively */
        (void)nco_prn_att_trv(in_id,&prn_flg,trv_tbl);
      } /* !PRN_GLB_METADATA */

      if(PRN_VAR_METADATA) (void)nco_prn_xtr_mtd(in_id,&prn_flg,trv_tbl);
      if(PRN_VAR_DATA) (void)nco_prn_xtr_val(in_id,&prn_flg,trv_tbl);
      
    }else{ 

      if(CHK_MAP){
	/* Check map-file quality */
	nco_map_chk(fl_in,flg_area_wgt,flg_frac_b_nrm);
        goto close_and_free;
      } /* !CHK_MAP */

      if(CHK_NAN){
	/* Check floating-point fields for NaNs */
        nco_chk_nan(in_id,trv_tbl);
        goto close_and_free;
      } /* !CHK_NAN */

      /* New file dump format(s) developed 201307 for CDL, JSN, SRM, TRD, XML */
      if(PRN_SRM){
	/* Stream printing is pre-alpha. Great project for volunteers! */
        nco_srm_hdr();
        goto close_and_free;
      } /* !PRN_SRM */

      if(ALPHA_BY_FULL_GROUP || ALPHA_BY_STUB_GROUP){
	/* Print CDL, JSN, TRD, and XML formats */
        if(prn_flg.jsn) rcd+=nco_prn_jsn(in_id,trv_pth,&prn_flg,trv_tbl);
        else if(prn_flg.xml) rcd+=nco_prn_xml(in_id,trv_pth,&prn_flg,trv_tbl);
        else if(prn_flg.cdl || prn_flg.trd) rcd+=nco_prn_cdl_trd(in_id,trv_pth,&prn_flg,trv_tbl); 
      }else{
	/* Place-holder for other options for organization/alphabetization */
	if(PRN_VAR_METADATA) (void)nco_prn_xtr_mtd(in_id,&prn_flg,trv_tbl);
	if(PRN_VAR_DATA) (void)nco_prn_xtr_val(in_id,&prn_flg,trv_tbl);
      } /* end if */
    } /* endif new format */

    if(fl_prn){
      rcd=fclose(fp_prn);
      if(rcd != 0){
	(void)fprintf(stderr,"%s: ERROR unable to close formatted output file %s\n",nco_prg_nm_get(),fl_prn);
	nco_exit(EXIT_FAILURE);
      } /* !fp_prn */
    } /* !fl_prn */
    if(fl_in_dpl) fl_in_dpl=(char *)nco_free(fl_in_dpl);

  } /* !fl_out */

  /* goto close_and_free */
close_and_free: 

  /* Close input netCDF files */
  for(thr_idx=0;thr_idx<thr_nbr;thr_idx++) nco_close(in_id_arr[thr_idx]);

  /* Remove local copy of file */
  if(FL_RTR_RMT_LCN && RM_RMT_FL_PST_PRC) (void)nco_fl_rm(fl_in);
  
  /* Clean memory unless dirty memory allowed */
  if(flg_mmr_cln){
    /* ncks-specific memory */
    if(fl_bnr) fl_bnr=(char *)nco_free(fl_bnr);
    if(fl_prn) fl_prn=(char *)nco_free(fl_prn);
    if(flt_sng) flt_sng=(char *)nco_free(flt_sng);
    if(rec_dmn_nm) rec_dmn_nm=(char *)nco_free(rec_dmn_nm); 
    /* NCO-generic clean-up */
    /* Free individual strings/arrays */
    if(cmd_ln) cmd_ln=(char *)nco_free(cmd_ln);
    if(cnk_map_sng) cnk_map_sng=(char *)nco_free(cnk_map_sng);
    if(cnk_plc_sng) cnk_plc_sng=(char *)nco_free(cnk_plc_sng);
    if(fl_in) fl_in=(char *)nco_free(fl_in);
    if(fl_out) fl_out=(char *)nco_free(fl_out);
    if(fl_out_tmp) fl_out_tmp=(char *)nco_free(fl_out_tmp);
    if(fl_pth) fl_pth=(char *)nco_free(fl_pth);
    if(fl_pth_lcl) fl_pth_lcl=(char *)nco_free(fl_pth_lcl);
    if(in_id_arr) in_id_arr=(int *)nco_free(in_id_arr);
    if(spr_nmr) spr_nmr=(char *)nco_free(spr_nmr);
    if(spr_chr) spr_chr=(char *)nco_free(spr_chr);
    /* Free lists of strings */
    if(fl_lst_in && fl_lst_abb == NULL) fl_lst_in=nco_sng_lst_free(fl_lst_in,fl_nbr); 
    if(fl_lst_in && fl_lst_abb) fl_lst_in=nco_sng_lst_free(fl_lst_in,1);
    if(fl_lst_abb) fl_lst_abb=nco_sng_lst_free(fl_lst_abb,abb_arg_nbr);
    if(gaa_nbr > 0) gaa_arg=nco_sng_lst_free(gaa_arg,gaa_nbr);
    if(grp_lst_in_nbr > 0) grp_lst_in=nco_sng_lst_free(grp_lst_in,grp_lst_in_nbr);
    if(var_lst_in_nbr > 0) var_lst_in=nco_sng_lst_free(var_lst_in,var_lst_in_nbr);
    /* Free limits */
    for(idx=0;idx<aux_nbr;idx++) aux_arg[idx]=(char *)nco_free(aux_arg[idx]);
    for(idx=0;idx<lmt_nbr;idx++) lmt_arg[idx]=(char *)nco_free(lmt_arg[idx]);
    for(idx=0;idx<ppc_nbr;idx++) ppc_arg[idx]=(char *)nco_free(ppc_arg[idx]);
    /* Free chunking information */
    for(idx=0;idx<cnk_nbr;idx++) cnk_arg[idx]=(char *)nco_free(cnk_arg[idx]);
    if(cnk_nbr > 0 && (fl_out_fmt == NC_FORMAT_NETCDF4 || fl_out_fmt == NC_FORMAT_NETCDF4_CLASSIC)) cnk.cnk_dmn=(cnk_dmn_sct **)nco_cnk_lst_free(cnk.cnk_dmn,cnk_nbr);
    trv_tbl_free(trv_tbl);
    for(idx=0;idx<lmt_nbr;idx++) flg_dne[idx].dim_nm=(char *)nco_free(flg_dne[idx].dim_nm);
    if(flg_dne) flg_dne=(nco_dmn_dne_t *)nco_free(flg_dne);
    if(gpe) gpe=(gpe_sct *)nco_gpe_free(gpe);
    if(md5) md5=(md5_sct *)nco_md5_free(md5);
    if(rec_dmn_nm_fix) rec_dmn_nm_fix=(char *)nco_free(rec_dmn_nm_fix);
    if(smr_sng) smr_sng=(char *)nco_free(smr_sng);
    if(smr_fl_sz_sng) smr_fl_sz_sng=(char *)nco_free(smr_fl_sz_sng);
    if(smr_xtn_sng) smr_xtn_sng=(char *)nco_free(smr_xtn_sng);
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
