/* $Header: /data/zender/nco_20150216/nco/src/nco/ncks.c,v 1.726 2014-06-17 23:50:01 zender Exp $ */

/* ncks -- netCDF Kitchen Sink */

/* Purpose: Extract (subsets of) variables from a netCDF file 
   Print them to screen, or copy them to a new file, or both */

/* Copyright (C) 1995--2014 Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You can redistribute and/or modify NCO under the terms of the 
   GNU General Public License (GPL) Version 3.
   As a special exception to the terms of the GPL, you are permitted 
   to link the NCO source code with the HDF, netCDF, OPeNDAP, and UDUnits
   libraries and to distribute the resulting executables under the terms 
   of the GPL, but in addition obeying the extra stipulations of the 
   HDF, netCDF, OPeNDAP, and UDUnits licenses.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
   See the GNU General Public License for more details.
   
   The original author of this software, Charlie Zender, seeks to improve
   it with your suggestions, contributions, bug-reports, and patches.
   Please contact the NCO project at http://nco.sf.net or write to
   Charlie Zender
   Department of Earth System Science
   University of California, Irvine
   Irvine, CA 92697-3100 */

/* URL: http://nco.cvs.sf.net/nco/nco/src/nco/ncks.c

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
   ncks -O -v one -p http://thredds-test.ucar.edu/thredds/dodsC/testdods in.nc ~/foo.nc
   ncks -O -G foo ~/nco/data/in.nc ~/foo.nc
   ncks -O -G :-5 -v v7 ~/nco/data/in_grp.nc ~/foo.nc
   ncks -O -G level3name:-5 -v v7 ~/nco/data/in_grp.nc ~/foo.nc
   ncks -O -v time ~/in_grp.nc ~/foo.nc
   ncks -O --sysconf ~/in_grp.nc ~/foo.nc
   ncks -C --xml_spr_chr=', ' -v two_dmn_rec_var_sng ~/nco/data/in.nc
   ncks --cdl -v one_dmn_rec_var ~/nco/data/in.nc
   ncks --jsn -C -v one_dmn_rec_var ~/nco/data/in.nc
   ncks --jsn -C -m -v one_dmn_rec_var ~/nco/data/in_grp.nc
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

/* Personal headers */
/* #define MAIN_PROGRAM_FILE MUST precede #include libnco.h */
#define MAIN_PROGRAM_FILE
#include "libnco.h" /* netCDF Operator (NCO) library */

int 
main(int argc,char **argv)
{
  nco_bool ALPHABETIZE_OUTPUT=True; /* Option a */
  nco_bool EXCLUDE_INPUT_LIST=False; /* Option x */
  nco_bool EXTRACT_ALL_COORDINATES=False; /* Option c */
  nco_bool EXTRACT_ASSOCIATED_COORDINATES=True; /* Option C */
  nco_bool FL_RTR_RMT_LCN;
  nco_bool FL_LST_IN_FROM_STDIN=False; /* [flg] fl_lst_in comes from stdin */
  nco_bool FORCE_APPEND=False; /* Option A */
  nco_bool FORCE_NOCLOBBER=False; /* Option no-clobber */
  nco_bool FORCE_OVERWRITE=False; /* Option O */
  nco_bool FORTRAN_IDX_CNV=False; /* Option F */
  nco_bool GET_GRP_INFO=False; /* [flg] Iterate file, get group extended information */
  nco_bool GET_FILE_INFO=False; /* [flg] Get file information (#groups, #dimensions, #attributes, #variables) */
  nco_bool GET_PRG_INFO=False; /* [flg] Get compiled program information (e.g., libraries) */
  nco_bool GET_LIST=False; /* [flg] Iterate file, print variables and exit */
  nco_bool GRP_VAR_UNN=False; /* [flg] Select union of specified groups and variables */
  nco_bool GRP_XTR_VAR_XCL=False; /* [flg] Extract matching groups, exclude matching variables */
  nco_bool HISTORY_APPEND=True; /* Option h */
  nco_bool HAVE_LIMITS=False; /* [flg] Are there user limits? (-d) */
  nco_bool MSA_USR_RDR=False; /* [flg] Multi-Slab Algorithm returns hyperslabs in user-specified order */
  nco_bool PRN_CDL=False; /* [flg] Print CDL */
  nco_bool PRN_HDN=False; /* [flg] Print hidden attributes */
  nco_bool PRN_SRM=False; /* [flg] Print ncStream */
  nco_bool PRN_JSN=False; /* [flg] Print JSON */
  nco_bool PRN_XML=False; /* [flg] Print XML (NcML) */
  nco_bool PRN_XML_LOCATION=True; /* [flg] Print XML location tag */
  nco_bool PRN_DMN_IDX_CRD_VAL=True; /* [flg] Print leading dimension/coordinate indices/values Option Q */
  nco_bool PRN_DMN_UNITS=True; /* [flg] Print dimensional units Option u */
  nco_bool PRN_DMN_VAR_NM=True; /* [flg] Print dimension/variable names */
  nco_bool PRN_DMN_UNITS_TGL=False; /* [flg] Toggle print dimensional units Option u */
  nco_bool PRN_GLB_METADATA=False; /* [flg] Print global metadata */
  nco_bool PRN_GLB_METADATA_TGL=False; /* [flg] Toggle print global metadata Option M */
  nco_bool PRN_MSS_VAL_BLANK=True; /* [flg] Print missing values as blanks */
  nco_bool PRN_QUIET=False; /* [flg] Turn off all printing to screen */
  nco_bool PRN_VAR_DATA=False; /* [flg] Print variable data */
  nco_bool PRN_VAR_DATA_TGL=False; /* [flg] Toggle print variable data Option H */
  nco_bool PRN_VAR_METADATA=False; /* [flg] Print variable metadata */
  nco_bool PRN_VAR_METADATA_TGL=False; /* [flg] Toggle print variable metadata Option m */
  nco_bool PRN_VRB=False; /* [flg] Print data and metadata by default */
  nco_bool PRN_NEW_FMT=False; /* [flg] Print using new print format */
  nco_bool RETAIN_ALL_DIMS=False; /* [flg] Retain all dimensions */
  nco_bool RAM_CREATE=False; /* [flg] Create file in RAM */
  nco_bool RAM_OPEN=False; /* [flg] Open (netCDF3-only) file(s) in RAM */
  nco_bool RM_RMT_FL_PST_PRC=True; /* Option R */
  nco_bool WRT_TMP_FL=True; /* [flg] Write output to temporary file */
  nco_bool flg_cln=True; /* [flg] Clean memory prior to exit */

  char **fl_lst_abb=NULL; /* Option a */
  char **fl_lst_in;
  char **grp_lst_in=NULL;
  char **var_lst_in=NULL;
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
  char *fl_pth=NULL; /* Option p */
  char *fl_pth_lcl=NULL; /* Option l */
  char *lmt_arg[NC_MAX_DIMS];
  char *opt_crr=NULL; /* [sng] String representation of current long-option name */
  char *optarg_lcl=NULL; /* [sng] Local copy of system optarg */
  char *rec_dmn_nm=NULL; /* [sng] Record dimension name */
  char *rec_dmn_nm_fix=NULL; /* [sng] Record dimension name (Original input name without _fix prefix) */
  char *smr_sng=NULL; /* [sng] File summary string */
  char *smr_xtn_sng=NULL; /* [sng] File extended summary string */
  char *sng_cnv_rcd=NULL_CEWI; /* [sng] strtol()/strtoul() return code */
  char *spr_chr=NULL; /* [sng] Separator for XML character types */
  char *spr_nmr=NULL; /* [sng] Separator for XML numeric types */

  char trv_pth[]="/"; /* [sng] Root path of traversal tree */

  const char * const CVS_Id="$Id: ncks.c,v 1.726 2014-06-17 23:50:01 zender Exp $"; 
  const char * const CVS_Revision="$Revision: 1.726 $";
  const char * const opt_sht_lst="34567aABb:CcD:d:FG:g:HhL:l:MmOo:Pp:qQrRs:uv:X:xz-:";

  cnk_sct cnk; /* [sct] Chunking structure */

#if defined(__cplusplus) || defined(PGI_CC)
  ddra_info_sct ddra_info;
  ddra_info.flg_ddra=False;
#else /* !__cplusplus */
  ddra_info_sct ddra_info={.flg_ddra=False};
#endif /* !__cplusplus */

  extern char *optarg;
  extern int optind;

  FILE *fp_bnr=NULL; /* [fl] Unformatted binary output file handle */

  gpe_sct *gpe=NULL; /* [sng] Group Path Editing (GPE) structure */

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
  int fl_in_fmt=NCO_FORMAT_UNDEFINED; /* [enm] Input file format */
  int fl_nbr=0;
  int fl_out_fmt=NCO_FORMAT_UNDEFINED; /* [enm] Output file format */
  int fll_md_old; /* [enm] Old fill mode */
  int grp_dpt_fl; /* [nbr] Maximum group depth (root = 0) */
  int grp_lst_in_nbr=0; /* [nbr] Number of groups explicitly specified by user */
  int grp_nbr_fl;
  int idx;
  int in_id;  
  int lmt_nbr=0; /* Option d. NB: lmt_nbr gets incremented */
  int md_open; /* [enm] Mode flag for nc_open() call */
  int opt;
  int rcd=NC_NOERR; /* [rcd] Return code */
  int var_lst_in_nbr=0;
  int var_nbr_fl;
  int var_ntm_fl;
  int xtr_nbr=0; /* xtr_nbr will not otherwise be set for -c with no -v */

  md5_sct *md5=NULL; /* [sct] MD5 configuration */

  size_t bfr_sz_hnt=NC_SIZEHINT_DEFAULT; /* [B] Buffer size hint */
  size_t cnk_sz_byt=0UL; /* [B] Chunk size in bytes */
  size_t cnk_sz_scl=0UL; /* [nbr] Chunk size scalar */
  size_t hdr_pad=0UL; /* [B] Pad at end of header section */

  trv_tbl_sct *trv_tbl=NULL; /* [lst] Traversal table */

  nco_dmn_dne_t *flg_dne=NULL; /* [lst] Flag to check if input dimension -d "does not exist" */

  static struct option opt_lng[]=
    { /* Structure ordered by short option key if possible */
      /* Long options with no argument, no short option counterpart */
      {"cdl",no_argument,0,0}, /* [flg] Print CDL */
      {"cln",no_argument,0,0}, /* [flg] Clean memory prior to exit */
      {"clean",no_argument,0,0}, /* [flg] Clean memory prior to exit */
      {"mmr_cln",no_argument,0,0}, /* [flg] Clean memory prior to exit */
      {"drt",no_argument,0,0}, /* [flg] Allow dirty memory on exit */
      {"dirty",no_argument,0,0}, /* [flg] Allow dirty memory on exit */
      {"mmr_drt",no_argument,0,0}, /* [flg] Allow dirty memory on exit */
      {"cmp",no_argument,0,0},
      {"compiler",no_argument,0,0},
      {"copyright",no_argument,0,0},
      {"cpy",no_argument,0,0},
      {"license",no_argument,0,0},
      {"hdf4",no_argument,0,0}, /* [flg] Treat file as HDF4 */
      {"hdn",no_argument,0,0}, /* [flg] Print hidden attributes */
      {"hidden",no_argument,0,0}, /* [flg] Print hidden attributes */
      {"id",no_argument,0,0}, /* [flg] Print normally hidden information, like file, group, and variable IDs */
      {"lbr",no_argument,0,0},
      {"library",no_argument,0,0},
      {"md5_dgs",no_argument,0,0}, /* [flg] Perform MD5 digests */
      {"md5_digest",no_argument,0,0}, /* [flg] Perform MD5 digests */
      {"md5_wrt_att",no_argument,0,0}, /* [flg] Write MD5 digests as attributes */
      {"md5_write_attribute",no_argument,0,0}, /* [flg] Write MD5 digests as attributes */
      {"mpi_implementation",no_argument,0,0},
      {"msa_usr_rdr",no_argument,0,0}, /* [flg] Multi-Slab Algorithm returns hyperslabs in user-specified order */
      {"msa_user_order",no_argument,0,0}, /* [flg] Multi-Slab Algorithm returns hyperslabs in user-specified order */
      {"no_blank",no_argument,0,0}, /* [flg] Print numeric missing values */
      {"no-blank",no_argument,0,0}, /* [flg] Print numeric missing values */
      {"noblank",no_argument,0,0}, /* [flg] Print numeric missing values */
      {"no_clb",no_argument,0,0},
      {"noclobber",no_argument,0,0},
      {"no-clobber",no_argument,0,0},
      {"no_clobber",no_argument,0,0},
      {"no_dmn_var_nm",no_argument,0,0}, /* [flg] Print dimension/variable names */
      {"no_nm_prn",no_argument,0,0}, /* [flg] Print dimension/variable names */
      {"rad",no_argument,0,0}, /* [flg] Retain all dimensions */
      {"retain_all_dimensions",no_argument,0,0}, /* [flg] Retain all dimensions */
      {"orphan_dimensions",no_argument,0,0}, /* [flg] Retain all dimensions */
      {"rph_dmn",no_argument,0,0}, /* [flg] Retain all dimensions */
      {"ram_all",no_argument,0,0}, /* [flg] Open (netCDF3) and create file(s) in RAM */
      {"create_ram",no_argument,0,0}, /* [flg] Create file in RAM */
      {"open_ram",no_argument,0,0}, /* [flg] Open (netCDF3) file(s) in RAM */
      {"diskless_all",no_argument,0,0}, /* [flg] Open (netCDF3) and create file(s) in RAM */
      {"secret",no_argument,0,0},
      {"shh",no_argument,0,0},
      {"srm",no_argument,0,0}, /* [flg] Print ncStream */
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
      {"w10",no_argument,0,0}, /* [flg] Print JSON */
      {"w10n",no_argument,0,0}, /* [flg] Print JSON */
      {"xml",no_argument,0,0}, /* [flg] Print XML (NcML) */
      {"ncml",no_argument,0,0}, /* [flg] Print XML (NcML) */
      {"xml_no_location",no_argument,0,0}, /* [flg] Omit XML location tag */
      {"ncml_no_location",no_argument,0,0}, /* [flg] Omit XML location tag */
      /* Long options with argument, no short option counterpart */
      {"bfr_sz_hnt",required_argument,0,0}, /* [B] Buffer size hint */
      {"buffer_size_hint",required_argument,0,0}, /* [B] Buffer size hint */
      {"cnk_map",required_argument,0,0}, /* [nbr] Chunking map */
      {"chunk_map",required_argument,0,0}, /* [nbr] Chunking map */
      {"cnk_plc",required_argument,0,0}, /* [nbr] Chunking policy */
      {"chunk_policy",required_argument,0,0}, /* [nbr] Chunking policy */
      {"cnk_byt",required_argument,0,0}, /* [B] Chunk size in bytes */
      {"chunk_byte",required_argument,0,0}, /* [B] Chunk size in bytes */
      {"cnk_scl",required_argument,0,0}, /* [nbr] Chunk size scalar */
      {"chunk_scalar",required_argument,0,0}, /* [nbr] Chunk size scalar */
      {"cnk_dmn",required_argument,0,0}, /* [nbr] Chunk size */
      {"chunk_dimension",required_argument,0,0}, /* [nbr] Chunk size */
      {"fl_fmt",required_argument,0,0},
      {"file_format",required_argument,0,0},
      {"fix_rec_dmn",required_argument,0,0}, /* [sng] Fix record dimension */
      {"no_rec_dmn",required_argument,0,0}, /* [sng] Fix record dimension */
      {"hdr_pad",required_argument,0,0},
      {"header_pad",required_argument,0,0},
      {"mk_rec_dmn",required_argument,0,0}, /* [sng] Name of record dimension in output */
      {"mk_rec_dim",required_argument,0,0}, /* [sng] Name of record dimension in output */
      {"tst_udunits",required_argument,0,0},
      {"xml_spr_chr",required_argument,0,0}, /* [flg] Separator for XML character types */
      {"xml_spr_nmr",required_argument,0,0}, /* [flg] Separator for XML numeric types */
      /* Long options with short counterparts */
      {"3",no_argument,0,'3'},
      {"4",no_argument,0,'4'},
      {"64bit",no_argument,0,'4'},
      {"netcdf4",no_argument,0,'4'},
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
      {"no-coords",no_argument,0,'C'},
      {"no-crd",no_argument,0,'C'},
      {"data",required_argument,0,'H'},
      {"debug",required_argument,0,'D'},
      {"dbg_lvl",required_argument,0,'D'},
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
      {"quiet",no_argument,0,'q'},
      {"retain",no_argument,0,'R'},
      {"rtn",no_argument,0,'R'},
      {"revision",no_argument,0,'r'},
      {"sng_fmt",required_argument,0,'s'},
      {"string",required_argument,0,'s'},
      {"units",no_argument,0,'u'},
      {"variable",required_argument,0,'v'},
      {"auxiliary",required_argument,0,'X'},
      {"exclude",no_argument,0,'x'},
      {"xcl",no_argument,0,'x'},
      {"help",no_argument,0,'?'},
      {"hlp",no_argument,0,'?'},
      {"get_grp_info",no_argument,0,0},
      {"get_prg_info",no_argument,0,0},
      {"get_file_info",no_argument,0,0},
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

  /* Parse command line arguments */
  while(1){
    /* getopt_long_only() allows one dash to prefix long options */
    opt=getopt_long(argc,argv,opt_sht_lst,opt_lng,&opt_idx);
    /* NB: access to opt_crr is only valid when long_opt is detected */
    if(opt == EOF) break; /* Parse positional arguments once getopt_long() returns EOF */
    opt_crr=(char *)strdup(opt_lng[opt_idx].name);

    /* Process long options without short option counterparts */
    if(opt == 0){
      if(!strcmp(opt_crr,"bfr_sz_hnt") || !strcmp(opt_crr,"buffer_size_hint")){
        bfr_sz_hnt=strtoul(optarg,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
        if(*sng_cnv_rcd) nco_sng_cnv_err(optarg,"strtoul",sng_cnv_rcd);
      } /* endif bfr_sz */
      if(!strcmp(opt_crr,"cnk_byt") || !strcmp(opt_crr,"chunk_byte")){
        cnk_sz_byt=strtoul(optarg,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
        if(*sng_cnv_rcd) nco_sng_cnv_err(optarg,"strtoul",sng_cnv_rcd);
      } /* endif cnk_byt */
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
      if(!strcmp(opt_crr,"cmp") || !strcmp(opt_crr,"compiler")){
        (void)fprintf(stdout,"%s\n",nco_cmp_get());
        nco_exit(EXIT_SUCCESS);
      } /* endif "cmp" */
      if(!strcmp(opt_crr,"cpy") || !strcmp(opt_crr,"copyright") || !strcmp(opt_crr,"license")){
	(void)nco_cpy_prn();
        nco_exit(EXIT_SUCCESS);
      } /* endif "copyright" */
      if(!strcmp(opt_crr,"cdl")) PRN_CDL=True; /* [flg] Print CDL */
      if(!strcmp(opt_crr,"cln") || !strcmp(opt_crr,"mmr_cln") || !strcmp(opt_crr,"clean")) flg_cln=True; /* [flg] Clean memory prior to exit */
      if(!strcmp(opt_crr,"drt") || !strcmp(opt_crr,"mmr_drt") || !strcmp(opt_crr,"dirty")) flg_cln=False; /* [flg] Clean memory prior to exit */
      if(!strcmp(opt_crr,"fix_rec_dmn") || !strcmp(opt_crr,"no_rec_dmn")){
        const char fix_pfx[]="fix_"; /* [sng] Prefix string to fix dimension */
        rec_dmn_nm=(char *)nco_malloc((strlen(fix_pfx)+strlen(optarg)+1L)*sizeof(char));
        rec_dmn_nm=strcpy(rec_dmn_nm,fix_pfx);
        rec_dmn_nm=strcat(rec_dmn_nm,optarg);
        rec_dmn_nm_fix=strdup(optarg);
      } /* endif fix_rec_dmn */
      if(!strcmp(opt_crr,"fl_fmt") || !strcmp(opt_crr,"file_format")) rcd=nco_create_mode_prs(optarg,&fl_out_fmt);
      if(!strcmp(opt_crr,"get_grp_info") || !strcmp(opt_crr,"grp_info_get")) GET_GRP_INFO=True;
      if(!strcmp(opt_crr,"get_file_info")) GET_FILE_INFO=True;
      if(!strcmp(opt_crr,"get_prg_info")) GET_PRG_INFO=True;
      if(!strcmp(opt_crr,"hdf4")) nco_fmt_xtn=nco_fmt_xtn_hdf4; /* [enm] Treat file as HDF4 */
      if(!strcmp(opt_crr,"hdn") || !strcmp(opt_crr,"hidden")) PRN_HDN=True; /* [flg] Print hidden attributes */
      if(!strcmp(opt_crr,"hdr_pad") || !strcmp(opt_crr,"header_pad")){
        hdr_pad=strtoul(optarg,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
        if(*sng_cnv_rcd) nco_sng_cnv_err(optarg,"strtoul",sng_cnv_rcd);
      } /* endif "hdr_pad" */
      if(!strcmp(opt_crr,"lbr") || !strcmp(opt_crr,"library")){
        (void)nco_lbr_vrs_prn();
        nco_exit(EXIT_SUCCESS);
      } /* endif "lbr" */
      if(!strcmp(opt_crr,"mk_rec_dmn") || !strcmp(opt_crr,"mk_rec_dim")) rec_dmn_nm=strdup(optarg);
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
        if(nco_dbg_lvl >= nco_dbg_std) (void)fprintf(stderr,"%s: INFO Will write MD5 digests as attributes\n",nco_prg_nm_get());
      } /* endif "md5_wrt_att" */
      if(!strcmp(opt_crr,"msa_usr_rdr") || !strcmp(opt_crr,"msa_user_order")) MSA_USR_RDR=True; /* [flg] Multi-Slab Algorithm returns hyperslabs in user-specified order */
      if(!strcmp(opt_crr,"no_blank") || !strcmp(opt_crr,"no-blank") || !strcmp(opt_crr,"noblank")) PRN_MSS_VAL_BLANK=!PRN_MSS_VAL_BLANK;
      if(!strcmp(opt_crr,"no_clb") || !strcmp(opt_crr,"no-clobber") || !strcmp(opt_crr,"no_clobber") || !strcmp(opt_crr,"noclobber")) FORCE_NOCLOBBER=!FORCE_NOCLOBBER;
      if(!strcmp(opt_crr,"no_dmn_var_nm") || !strcmp(opt_crr,"no_nm_prn")) PRN_DMN_VAR_NM=False;
      if(!strcmp(opt_crr,"rad") || !strcmp(opt_crr,"retain_all_dimensions") || !strcmp(opt_crr,"orphan_dimensions") || !strcmp(opt_crr,"rph_dmn")) RETAIN_ALL_DIMS=True;
      if(!strcmp(opt_crr,"ram_all") || !strcmp(opt_crr,"create_ram") || !strcmp(opt_crr,"diskless_all")) RAM_CREATE=True; /* [flg] Open (netCDF3) file(s) in RAM */
      if(!strcmp(opt_crr,"ram_all") || !strcmp(opt_crr,"open_ram") || !strcmp(opt_crr,"diskless_all")) RAM_OPEN=True; /* [flg] Create file in RAM */
      if(!strcmp(opt_crr,"secret") || !strcmp(opt_crr,"scr") || !strcmp(opt_crr,"shh")){
        (void)fprintf(stdout,"Hidden/unsupported NCO options:\nCompiler used\t\t--cmp, --compiler\nCopyright\t\t--cpy, --copyright, --license\nHidden functions\t--scr, --ssh, --secret\nLibrary used\t\t--lbr, --library\nMemory clean\t\t--mmr_cln, --cln, --clean\nMemory dirty\t\t--mmr_drt, --drt, --dirty\nMPI implementation\t--mpi_implementation\nNameless printing\t--no_nm_prn, --no_dmn_var_nm\nNo-clobber files\t--no_clb, --no-clobber\nPseudonym\t\t--pseudonym, -Y (ncra only)\nStreams\t\t\t--srm\nSysconf\t\t\t--sysconf\nTest UDUnits\t\t--tst_udunits,'units_in','units_out','cln_sng'? \nVersion\t\t\t--vrs, --version\n\n");
        nco_exit(EXIT_SUCCESS);
      } /* endif "shh" */
      if(!strcmp(opt_crr,"srm")) PRN_SRM=True; /* [flg] Print ncStream */
      if(!strcmp(opt_crr,"sysconf")){
	long maxrss; /* [B] Maximum resident set size */
	maxrss=nco_mmr_usg_prn((int)0);
	maxrss+=0; /* CEWI */
        nco_exit(EXIT_SUCCESS);
      } /* endif "sysconf" */
      if(!strcmp(opt_crr,"tst_udunits")){ 
	/* Use this feature with, e.g.,
	   ncks --tst_udunits='5 meters',centimeters ~/nco/data/in.nc
	   ncks --tst_udunits='days since 1918-11-11','days since 1939-09-09',standard ~/nco/data/in.nc
	   ncks --tst_udunits='days since 1918-11-11','days since 1939-09-09',360_day ~/nco/data/in.nc
	   ncks --tst_udunits='days since 1918-11-11','days since 1939-09-09',365_day ~/nco/data/in.nc
	   ncks --tst_udunits='days since 1918-11-11','days since 1939-09-09',366_day ~/nco/data/in.nc */
        char *cp;
        char **args;
        double crr_val;
#ifndef ENABLE_UDUNITS
        (void)fprintf(stdout,"%s: Build NCO with UDUnits support to use this option\n",nco_prg_nm_get());
        nco_exit(EXIT_FAILURE);
#endif /* !ENABLE_UDUNITS */
        cp=strdup(optarg); 
        args=nco_lst_prs_1D(cp,",",&lmt_nbr);         
        nco_cln_clc_org(args[0],args[1],(lmt_nbr > 2 ? nco_cln_get_cln_typ(args[2]) : cln_nil),&crr_val);        
        (void)fprintf(stdout,"Units in=%s, units out=%s, difference (date) or conversion (non-date) = %f\n",args[0],args[1],crr_val);
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
      if(!strcmp(opt_crr,"wrt_tmp_fl") || !strcmp(opt_crr,"write_tmp_fl")) WRT_TMP_FL=True;
      if(!strcmp(opt_crr,"no_tmp_fl")) WRT_TMP_FL=False;
      if(!strcmp(opt_crr,"jsn") || !strcmp(opt_crr,"json") || !strcmp(opt_crr,"w10") || !strcmp(opt_crr,"w10n")) PRN_JSN=True; /* [flg] Print JSON */
      if(!strcmp(opt_crr,"xml") || !strcmp(opt_crr,"ncml")) PRN_XML=True; /* [flg] Print XML (NcML) */
      if(!strcmp(opt_crr,"xml_no_location") || !strcmp(opt_crr,"ncml_no_location")){PRN_XML_LOCATION=False;PRN_XML=True;} /* [flg] Print XML location tag */
      if(!strcmp(opt_crr,"xml_spr_chr")){spr_chr=(char *)strdup(optarg);PRN_XML=True;} /* [flg] Separator for XML character types */
      if(!strcmp(opt_crr,"xml_spr_nmr")){spr_nmr=(char *)strdup(optarg);PRN_XML=True;} /* [flg] Separator for XML numeric types */
    } /* opt != 0 */
    /* Process short options */
    switch(opt){
    case 0: /* Long options have already been processed, return */
      break;
    case '3': /* Request netCDF3 output storage format */
      fl_out_fmt=NC_FORMAT_CLASSIC;
      break;
    case '4': /* Catch-all to prescribe output storage format */
      if(!strcmp(opt_crr,"64bit")) fl_out_fmt=NC_FORMAT_64BIT; else fl_out_fmt=NC_FORMAT_NETCDF4; 
      break;
    case '5': /* Print using new print format */
      PRN_NEW_FMT=!PRN_NEW_FMT;
      break;
    case '6': /* Request netCDF3 64-bit offset output storage format */
      fl_out_fmt=NC_FORMAT_64BIT;
      break;
    case '7': /* Request netCDF4-classic output storage format */
      fl_out_fmt=NC_FORMAT_NETCDF4_CLASSIC;
      break;
    case 'A': /* Toggle FORCE_APPEND */
      FORCE_APPEND=!FORCE_APPEND;
      break;
    case 'a': /* Toggle ALPHABETIZE_OUTPUT */
      ALPHABETIZE_OUTPUT=!ALPHABETIZE_OUTPUT;
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
      nc_set_log_level(nco_dbg_lvl);
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
    case 'q': /* [flg] Turn off all printing to screen */
      PRN_QUIET=True; /* [flg] Turn off all printing to screen */
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
    case 's': /* User specified delimiter string for printed output */
      dlm_sng=(char *)strdup(optarg);
      break;
    case 'u': /* Toggle printing dimensional units */
      PRN_DMN_UNITS_TGL=True;
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
      GET_LIST=True;
      break;
    case '?': /* Print proper usage */
      (void)nco_usg_prn();
      nco_exit(EXIT_SUCCESS);
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

#ifdef _LANGINFO_H
/* Internationalization i18n
   Linux Journal 200211 p. 57--59 http://www.linuxjournal.com/article/6176 
   Fedora: http://fedoraproject.org/wiki/How_to_do_I18N_through_gettext
   cd ~/nco/bld;make I18N=Y
   cd ~/nco/bld;xgettext --default-domain=nco --join-existing -o ../po/nco.pot ../src/nco/ncks.c ../src/nco/ncra.c
   for LL in fr es; do
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

  /* Initialize traversal table */
  (void)trv_tbl_init(&trv_tbl);
 
  /* Get program info for regressions tests */
  if(GET_PRG_INFO) nco_get_prg_info();

  /* Process positional arguments and fill in filenames */
  fl_lst_in=nco_fl_lst_mk(argv,argc,optind,&fl_nbr,&fl_out,&FL_LST_IN_FROM_STDIN);
  
  /* Parse filename */
  fl_in=nco_fl_nm_prs(fl_in,0,&fl_nbr,fl_lst_in,abb_arg_nbr,fl_lst_abb,fl_pth);
  /* Make sure file is on local system and is readable or die trying */
  fl_in=nco_fl_mk_lcl(fl_in,fl_pth_lcl,&FL_RTR_RMT_LCN);
  /* Open file using appropriate buffer size hints and verbosity */
  if(RAM_OPEN) md_open=NC_NOWRITE|NC_DISKLESS; else md_open=NC_NOWRITE;
  rcd+=nco_fl_open(fl_in,md_open,&bfr_sz_hnt,&in_id);

  /* Construct GTT (Group Traversal Table), check -v and -g input names and create extraction list*/
  (void)nco_bld_trv_tbl(in_id,trv_pth,lmt_nbr,lmt_arg,aux_nbr,aux_arg,MSA_USR_RDR,FORTRAN_IDX_CNV,grp_lst_in,grp_lst_in_nbr,var_lst_in,xtr_nbr,EXTRACT_ALL_COORDINATES,GRP_VAR_UNN,GRP_XTR_VAR_XCL,EXCLUDE_INPUT_LIST,EXTRACT_ASSOCIATED_COORDINATES,nco_pck_plc_nil,&flg_dne,trv_tbl);

  /* Check if all input -d dimensions were found */ 
  (void)nco_chk_dmn(lmt_nbr,flg_dne);    

  /* Get number of variables, dimensions, and global attributes in file */
  (void)trv_tbl_inq(&att_glb_nbr,&att_grp_nbr,&att_var_nbr,&dmn_nbr_fl,&dmn_rec_fl,&grp_dpt_fl,&grp_nbr_fl,&var_ntm_fl,&var_nbr_fl,trv_tbl);

  /* Make output and input files consanguinous */
  (void)nco_inq_format(in_id,&fl_in_fmt);
  if(fl_out && fl_out_fmt == NCO_FORMAT_UNDEFINED) fl_out_fmt=fl_in_fmt;

  /* Process -z option if requested */ 
  if(GET_LIST){ 
    if(ALPHABETIZE_OUTPUT) trv_tbl_srt(trv_tbl);
    trv_tbl_prn(trv_tbl);
    goto close_and_free; 
  } /* end GET_LIST */ 

  /* Process --get_grp_info option if requested */ 
  if(GET_GRP_INFO){ 
    nco_prn_trv_tbl(in_id,trv_tbl);
    goto close_and_free; 
  } /* end GET_GRP_INFO */

  /* Process --get_file_info option if requested */ 
  if(GET_FILE_INFO){ 
    (void)fprintf(stderr,"%s: INFO reports file information\n",nco_prg_nm_get());
    (void)fprintf(stdout,"%d subgroups, %d fixed dimensions, %d record dimensions, %d group + global attributes, %d atomic-type variables, %d non-atomic variables\n",grp_nbr_fl,trv_tbl->nbr_dmn-dmn_rec_fl,dmn_rec_fl,att_glb_nbr+att_glb_nbr,var_nbr_fl,var_ntm_fl);
    goto close_and_free; 
  } /* end GET_FILE_INFO */

  if(ALPHABETIZE_OUTPUT) trv_tbl_srt(trv_tbl);

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
    /* Only input file is specified, so some printing should occur */
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
    /* PRN_QUIET turns off all printing to screen */
    if(PRN_QUIET) PRN_VAR_DATA=PRN_VAR_METADATA=PRN_GLB_METADATA=False;
  } /* !fl_out */  

  if(fl_bnr && !fl_out){
    /* Native binary files depend on writing netCDF file to enter generic I/O logic */
    (void)fprintf(stdout,"%s: ERROR Native binary files cannot be written unless netCDF output filename also specified. HINT: Repeat command with dummy netCDF file specified for output file (e.g., -o foo.nc)\n",nco_prg_nm_get());
    nco_exit(EXIT_FAILURE);
  } /* endif fl_bnr */
    
  if(gpe){
    if(nco_dbg_lvl >= nco_dbg_fl) (void)fprintf(stderr,"%s: INFO Group Path Edit (GPE) feature enabled\n",nco_prg_nm_get());
    if(fl_out && fl_out_fmt != NC_FORMAT_NETCDF4 && nco_dbg_lvl >= nco_dbg_std) (void)fprintf(stderr,"%s: WARNING Group Path Edit (GPE) requires netCDF4 output format in most cases (except flattening) but user explicitly requested output format = %s. This command will fail if the output file requires netCDF4 features like groups, non-atomic types, or multiple record dimensions. However, it _will_ autoconvert netCDF4 atomic types (e.g., NC_STRING, NC_UBYTE...) to netCDF3 atomic types (e.g., NC_CHAR, NC_SHORT...).\n",nco_prg_nm_get(),nco_fmt_sng(fl_out_fmt));
  } /* !gpe */

  if(fl_out){
    /* Output file was specified so PRN_ tokens refer to (meta)data copying */
    int out_id;
    
    /* Make output and input files consanguinous */
    if(fl_out_fmt == NCO_FORMAT_UNDEFINED) fl_out_fmt=fl_in_fmt;

    /* Verify output file format supports requested actions */
    (void)nco_fl_fmt_vet(fl_out_fmt,cnk_nbr,dfl_lvl);

    /* Open output file */
    fl_out_tmp=nco_fl_out_open(fl_out,FORCE_APPEND,FORCE_OVERWRITE,fl_out_fmt,&bfr_sz_hnt,RAM_CREATE,RAM_OPEN,WRT_TMP_FL,&out_id);
    
    /* Create structure with all chunking information */
    if(fl_out_fmt == NC_FORMAT_NETCDF4 || fl_out_fmt == NC_FORMAT_NETCDF4_CLASSIC) rcd+=nco_cnk_ini(fl_out,cnk_arg,cnk_nbr,cnk_map,cnk_plc,cnk_sz_byt,cnk_sz_scl,&cnk);

    if(nco_dbg_lvl == nco_dbg_old) (void)nco_prn_var(in_id,trv_tbl);       

    /* Define extracted groups, variables, and attributes in output file */
    (void)nco_xtr_dfn(in_id,out_id,&cnk,dfl_lvl,gpe,md5,PRN_GLB_METADATA,PRN_VAR_METADATA,RETAIN_ALL_DIMS,nco_pck_plc_nil,rec_dmn_nm,trv_tbl);

    /* Catenate timestamped command line to "history" global attribute */
    if(HISTORY_APPEND) (void)nco_hst_att_cat(out_id,cmd_ln);
    if(HISTORY_APPEND) (void)nco_vrs_att_cat(out_id);

    /* Turn off default filling behavior to enhance efficiency */
    nco_set_fill(out_id,NC_NOFILL,&fll_md_old);

    /* Take output file out of define mode */
    if(hdr_pad == 0UL){
      (void)nco_enddef(out_id);
    }else{
      (void)nco__enddef(out_id,hdr_pad);
      if(nco_dbg_lvl >= nco_dbg_scl) (void)fprintf(stderr,"%s: INFO Padding header with %lu extra bytes\n",nco_prg_nm_get(),(unsigned long)hdr_pad);
    } /* hdr_pad */

    /* [fnc] Open unformatted binary data file for writing */
    if(fl_bnr) fp_bnr=nco_bnr_open(fl_bnr);

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
    }

    /* Close output file and move it from temporary to permanent location */
    (void)nco_fl_out_cls(fl_out,fl_out_tmp,out_id);
    
  }else{ /* !fl_out */

    nco_bool ALPHA_BY_FULL_GROUP=False; /* [flg] Print alphabetically by full group */
    nco_bool ALPHA_BY_STUB_GROUP=True; /* [flg] Print alphabetically by stub group */
    //      nco_bool ALPHA_BY_FULL_OBJECT=False; /* [flg] Print alphabetically by full object */
    //      nco_bool ALPHA_BY_STUB_OBJECT=False; /* [flg] Print alphabetically by stub object */
    char *fl_nm_stub;
    char *fl_in_dpl=NULL;
    char *sfx_ptr;

    /* Update all GTT dimensions with hyperslabbed size */
    (void)nco_dmn_trv_msa_tbl(in_id,rec_dmn_nm,trv_tbl);   

    /* No output file was specified so PRN_ tokens refer to screen printing */
    prn_fmt_sct prn_flg;
    prn_flg.cdl=PRN_CDL;
    prn_flg.jsn=PRN_JSN;
    prn_flg.srm=PRN_SRM;
    prn_flg.xml=PRN_XML;
    prn_flg.trd=!(PRN_CDL || PRN_XML || PRN_JSN);
    if((prn_flg.cdl || prn_flg.xml) && nco_dbg_lvl >= nco_dbg_std) prn_flg.nfo_xtr=True; else prn_flg.nfo_xtr=False;
    prn_flg.new_fmt=(PRN_CDL || PRN_JSN || PRN_SRM || PRN_XML || PRN_NEW_FMT);
    prn_flg.hdn=PRN_HDN;
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
    prn_flg.nbr_zro=0;
    prn_flg.ndn=0; /* Initialize for prn_flg->trd */
    prn_flg.spc_per_lvl=2;
    prn_flg.sxn_fst=2;
    prn_flg.var_fst=2;
    prn_flg.tab=4;
    if(nco_dbg_lvl >= nco_dbg_scl) prn_flg.fll_pth=True; else prn_flg.fll_pth=False;
    if(prn_flg.xml) prn_flg.nwl_pst_val=False; else prn_flg.nwl_pst_val=True;
    prn_flg.dlm_sng=dlm_sng;
    prn_flg.ALPHA_BY_FULL_GROUP=ALPHA_BY_FULL_GROUP;
    // prn_flg.ALPHA_BY_FULL_OBJECT=ALPHA_BY_FULL_OBJECT;
    prn_flg.ALPHA_BY_STUB_GROUP=ALPHA_BY_STUB_GROUP;
    // prn_flg.ALPHA_BY_STUB_OBJECT=ALPHA_BY_STUB_OBJECT;
    prn_flg.FORTRAN_IDX_CNV=FORTRAN_IDX_CNV;
    prn_flg.PRN_DMN_IDX_CRD_VAL=PRN_DMN_IDX_CRD_VAL;
    prn_flg.PRN_DMN_UNITS=PRN_DMN_UNITS;
    prn_flg.PRN_DMN_VAR_NM=PRN_DMN_VAR_NM;
    prn_flg.PRN_GLB_METADATA=PRN_GLB_METADATA;
    prn_flg.PRN_MSS_VAL_BLANK=PRN_MSS_VAL_BLANK;
    prn_flg.PRN_VAR_DATA=PRN_VAR_DATA;
    prn_flg.PRN_VAR_METADATA=PRN_VAR_METADATA;
    /* Derived formats */
    if(prn_flg.cdl){
      prn_flg.PRN_DMN_UNITS=False;
      prn_flg.PRN_DMN_VAR_NM=True;
      prn_flg.PRN_MSS_VAL_BLANK=True;
    } /* endif */
    if(prn_flg.jsn){
      /* JSON either prints metadata or data, not both */
      if(prn_flg.PRN_VAR_DATA){
	prn_flg.PRN_VAR_METADATA=False;
	prn_flg.PRN_GLB_METADATA=False;
      } /* !PRN_VAR_DATA */
      if(prn_flg.PRN_GLB_METADATA) prn_flg.PRN_VAR_METADATA=False;
    } /* endif JSON */
    if(prn_flg.xml) prn_flg.PRN_MSS_VAL_BLANK=False;

    /* File summary */
    if(PRN_GLB_METADATA){
      prn_flg.smr_sng=smr_sng=(char *)nco_malloc((strlen(fl_in)+300L*sizeof(char))); /* [sng] File summary string */
      smr_xtn_sng=(char *)nco_malloc(300L*sizeof(char)); /* [sng] File extended summary string */
      if(nco_dbg_lvl > nco_dbg_std) (void)sprintf(smr_xtn_sng," (representation of extended/underlying filetype %s)",nco_fmt_xtn_sng(nco_fmt_xtn_get())); else smr_xtn_sng[0]='\0';
      (void)sprintf(smr_sng,"Summary of %s: filetype = %s%s, %i groups (max. depth = %i), %i dimensions (%i fixed, %i record), %i variables (%i atomic-type, %i non-atomic), %i attributes (%i global, %i group, %i variable)",fl_in,nco_fmt_sng(fl_in_fmt),smr_xtn_sng,grp_nbr_fl,grp_dpt_fl,trv_tbl->nbr_dmn,trv_tbl->nbr_dmn-dmn_rec_fl,dmn_rec_fl,var_nbr_fl+var_ntm_fl,var_nbr_fl,var_ntm_fl,att_glb_nbr+att_grp_nbr+att_var_nbr,att_glb_nbr,att_grp_nbr,att_var_nbr);
      if(!prn_flg.cdl && !prn_flg.xml && !prn_flg.srm) (void)fprintf(stdout,"%s\n\n",smr_sng);
    } /* endif summary */

    if(!prn_flg.new_fmt){

      /* Traditional printing order/format */
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
            (void)fprintf(stdout,"Root record dimension %d: name = %s, size = %li\n",rec_idx,dmn_nm,rec_dmn_sz);
          } /* end loop over rec_idx */
          (void)fprintf(stdout,"\n");
        } /* NCO_REC_DMN_UNDEFINED */
        /* Print group attributes recursively */
        (void)nco_prn_att_trv(in_id,&prn_flg,trv_tbl);
      } /* !PRN_GLB_METADATA */

      if(PRN_VAR_METADATA) (void)nco_prn_xtr_mtd(in_id,&prn_flg,trv_tbl);
      if(PRN_VAR_DATA) (void)nco_prn_xtr_val(in_id,&prn_flg,trv_tbl);
      
    }else{ 

      /* New file dump format developed 201307 for CDL, TRD, XML, SRM */

      if(PRN_SRM){
        nco_srm_hdr();
        goto close_and_free;
      } /* !PRN_SRM */

      if(ALPHA_BY_FULL_GROUP || ALPHA_BY_STUB_GROUP){
        rcd+=nco_grp_prn(in_id,trv_pth,&prn_flg,trv_tbl);
      }else{
        trv_sct trv_obj; /* [sct] Traversal table object */
        for(unsigned int obj_idx=0;obj_idx<trv_tbl->nbr;obj_idx++){
          /* Shallow copy to avoid indirection */
          trv_obj=trv_tbl->lst[obj_idx];
          /* Print this group */
          if(trv_obj.nco_typ == nco_obj_typ_grp){
            /* Print dimensions defined in this group */
            // (void)nco_prn_dmn_xtr(in_id,trv_tbl);
            /* Print group attributes */
            //if(PRN_GLB_METADATA) (void)nco_prn_grp_att(in_id,trv_tbl);
            ;
          } /* endif group */
          if(trv_obj.nco_typ == nco_obj_typ_var){
            if(PRN_VAR_METADATA) (void)nco_prn_xtr_mtd(in_id,&prn_flg,trv_tbl);
            if(PRN_VAR_DATA) (void)nco_prn_xtr_val(in_id,&prn_flg,trv_tbl);
          } /* endif variable */
        } /* end loop over obj_idx */
      } /* end if */
    } /* endif new format */

    if(fl_in_dpl) fl_in_dpl=(char *)nco_free(fl_in_dpl);
  } /* !fl_out */

  /* goto close_and_free */
close_and_free: 

  /* Close input netCDF file */
  nco_close(in_id);

  /* Remove local copy of file */
  if(FL_RTR_RMT_LCN && RM_RMT_FL_PST_PRC) (void)nco_fl_rm(fl_in);
  
  /* Clean memory unless dirty memory allowed */
  if(flg_cln){
    /* ncks-specific memory */
    if(fl_bnr) fl_bnr=(char *)nco_free(fl_bnr);
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
    if(spr_nmr) spr_nmr=(char *)nco_free(spr_nmr);
    if(spr_chr) spr_chr=(char *)nco_free(spr_chr);
    /* Free lists of strings */
    if(fl_lst_in && fl_lst_abb == NULL) fl_lst_in=nco_sng_lst_free(fl_lst_in,fl_nbr); 
    if(fl_lst_in && fl_lst_abb) fl_lst_in=nco_sng_lst_free(fl_lst_in,1);
    if(fl_lst_abb) fl_lst_abb=nco_sng_lst_free(fl_lst_abb,abb_arg_nbr);
    if(grp_lst_in_nbr > 0) grp_lst_in=nco_sng_lst_free(grp_lst_in,grp_lst_in_nbr);
    if(var_lst_in_nbr > 0) var_lst_in=nco_sng_lst_free(var_lst_in,var_lst_in_nbr);
    /* Free limits */
    for(idx=0;idx<lmt_nbr;idx++) lmt_arg[idx]=(char *)nco_free(lmt_arg[idx]);
    for(idx=0;idx<aux_nbr;idx++) aux_arg[idx]=(char *)nco_free(aux_arg[idx]);
    /* Free chunking information */
    for(idx=0;idx<cnk_nbr;idx++) cnk_arg[idx]=(char *)nco_free(cnk_arg[idx]);
    if(cnk_nbr > 0) cnk.cnk_dmn=(cnk_dmn_sct **)nco_cnk_lst_free(cnk.cnk_dmn,cnk_nbr);
    trv_tbl_free(trv_tbl);
    for(idx=0;idx<lmt_nbr;idx++) flg_dne[idx].dim_nm=(char *)nco_free(flg_dne[idx].dim_nm);
    if (flg_dne) flg_dne=(nco_dmn_dne_t *)nco_free(flg_dne);
    if(gpe) gpe=(gpe_sct *)nco_gpe_free(gpe);
    if(md5) md5=(md5_sct *)nco_md5_free(md5);
    if(smr_sng) smr_sng=(char *)nco_free(smr_sng);
    if(smr_xtn_sng) smr_xtn_sng=(char *)nco_free(smr_xtn_sng);
    rec_dmn_nm_fix=(char *)nco_free(rec_dmn_nm_fix);
  } /* !flg_cln */
  
  /* End timer */ 
  ddra_info.tmr_flg=nco_tmr_end; /* [enm] Timer flag */
  rcd+=nco_ddra((char *)NULL,(char *)NULL,&ddra_info);

  if(rcd != NC_NOERR) nco_err_exit(rcd,"main");
  nco_exit_gracefully();
  return EXIT_SUCCESS;
  
} /* end main() */
