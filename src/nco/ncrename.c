/* $Header: /data/zender/nco_20150216/nco/src/nco/ncrename.c,v 1.202 2014-09-24 18:14:51 zender Exp $ */

/* ncrename -- netCDF renaming operator */

/* Purpose: Rename dimensions, variables, and attributes of a netCDF file */

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

/* Usage:
   ncrename -O -g g1,obama ~/nco/data/in_grp.nc ~/foo.nc
   ncrename -O -a Conventions,Geneva ~/nco/data/in.nc ~/foo.nc
   ncrename -O -a HDFEOSVersion,Geneva ${DATA}/hdf/MOD10CM.A2007001.005.2007108111758.hdf ~/foo.nc
   ncrename -O -d old_dim1,new_dim1 -v old_var1,new_var1 -v old_var2,new_var2 -a old_att1,new_att1 ~/nco/data/in.nc ~/foo.nc
   ncrename -O -d lon,new_lon -v scalar_var,new_scalar_var -a long_name,new_long_name ~/nco/data/in.nc ~/foo.nc */

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

/* #define MAIN_PROGRAM_FILE MUST precede #include libnco.h */
#define MAIN_PROGRAM_FILE
#include "libnco.h" /* netCDF Operator (NCO) library */

int 
main(int argc,char **argv)
{
#ifndef __GNUG__
  extern int errno; /* [enm] Error code in errno.h */
#endif /* __GNUG__ */

  nco_bool FL_RTR_RMT_LCN;
  nco_bool FL_LST_IN_FROM_STDIN=False; /* [flg] fl_lst_in comes from stdin */
  nco_bool FORCE_APPEND=False; /* Option A */
  nco_bool FORCE_OVERWRITE=False; /* Option O */
  nco_bool HISTORY_APPEND=True; /* Option h */
  nco_bool FL_OUT_NEW=False;
  nco_bool RAM_OPEN=False; /* [flg] Open (netCDF3-only) file(s) in RAM */
  nco_bool RM_RMT_FL_PST_PRC=True; /* Option R */
  nco_bool flg_cln=False; /* [flg] Clean memory prior to exit */

  char **fl_lst_abb=NULL; /* Option a */
  char **fl_lst_in;
  char *att_rnm_arg[NC_MAX_ATTRS];
  char *cmd_ln;
  char *dmn_rnm_arg[NC_MAX_DIMS];
  char *fl_in=NULL;
  char *fl_out=NULL; /* Option o */
  char *fl_pth=NULL; /* Option p */
  char *fl_pth_lcl=NULL; /* Option l */
  char *grp_rnm_arg[NC_MAX_DIMS];
  char *opt_crr=NULL; /* [sng] String representation of current long-option name */
  char *sng_cnv_rcd=NULL_CEWI; /* [sng] strtol()/strtoul() return code */
  char *var_rnm_arg[NC_MAX_VARS];
  char trv_pth[]="/"; /* [sng] Root path of traversal tree */

  char obj_nm[NC_MAX_NAME+1];
  char var_nm[NC_MAX_NAME+1];

  const char * const CVS_Id="$Id: ncrename.c,v 1.202 2014-09-24 18:14:51 zender Exp $"; 
  const char * const CVS_Revision="$Revision: 1.202 $";
  const char * const opt_sht_lst="a:D:d:g:hl:Oo:p:rv:-:";
  const char dlm_chr='@'; /* Character delimiting variable from attribute name  */
  const char opt_chr='.'; /* Character indicating presence of following variable/dimension/attribute/group in file is optional */
#if defined(__cplusplus) || defined(PGI_CC)
  ddra_info_sct ddra_info;
  ddra_info.flg_ddra=False;
#else /* !__cplusplus */
  ddra_info_sct ddra_info={.flg_ddra=False};
#endif /* !__cplusplus */

  extern char *optarg;
  extern int optind;

  int abb_arg_nbr=0;
  int fl_nbr=0;
  int mch_nbr_att=0; /* [nbr] Number of renamed attributes */
  int mch_nbr_var=0; /* [nbr] Number of renamed variables */
  int mch_nbr_grp=0; /* [nbr] Number of renamed groups */
  int mch_nbr_dmn=0; /* [nbr] Number of renamed dimensions */
  int md_open; /* [enm] Mode flag for nc_open() call */
  int nbr_att_rnm=0; /* Option a. NB: nbr_att_rnm gets incremented */
  int nbr_dmn_rnm=0; /* Option d. NB: nbr_dmn_rnm gets incremented */
  int nbr_grp_rnm=0; /* Option g. NB: nbr_grp_rnm gets incremented */
  int nbr_var_rnm=0; /* Option v. NB: nbr_var_rnm gets incremented */
  int nc_id;
  int grp_id;
  int opt;
  int rcd=NC_NOERR; /* [rcd] Return code */

  rnm_sct *var_rnm_lst=NULL_CEWI;
  rnm_sct *dmn_rnm_lst=NULL_CEWI;
  rnm_sct *grp_rnm_lst=NULL_CEWI;
  rnm_sct *att_rnm_lst=NULL_CEWI;

  size_t bfr_sz_hnt=NC_SIZEHINT_DEFAULT; /* [B] Buffer size hint */
  size_t hdr_pad=0UL; /* [B] Pad at end of header section */

  trv_tbl_sct *trv_tbl; /* [lst] Traversal table */

#ifdef ENABLE_MPI
  /* Declare all MPI-specific variables here */
  MPI_Comm mpi_cmm=MPI_COMM_WORLD; /* [prc] Communicator */
  int prc_rnk; /* [idx] Process rank */
  int prc_nbr=0; /* [nbr] Number of MPI processes */
#endif /* !ENABLE_MPI */
  
  static struct option opt_lng[] =
  { /* Structure ordered by short option key if possible */
    /* Long options with no argument, no short option counterpart */
    {"cln",no_argument,0,0}, /* [flg] Clean memory prior to exit */
    {"clean",no_argument,0,0}, /* [flg] Clean memory prior to exit */
    {"mmr_cln",no_argument,0,0}, /* [flg] Clean memory prior to exit */
    {"drt",no_argument,0,0}, /* [flg] Allow dirty memory on exit */
    {"dirty",no_argument,0,0}, /* [flg] Allow dirty memory on exit */
    {"mmr_drt",no_argument,0,0}, /* [flg] Allow dirty memory on exit */
    {"hdf4",no_argument,0,0}, /* [flg] Treat file as HDF4 */
    {"ram_all",no_argument,0,0}, /* [flg] Open (netCDF3) and create file(s) in RAM */
    {"create_ram",no_argument,0,0}, /* [flg] Create file in RAM */
    {"open_ram",no_argument,0,0}, /* [flg] Open (netCDF3) file(s) in RAM */
    {"diskless_all",no_argument,0,0}, /* [flg] Open (netCDF3) and create file(s) in RAM */
    {"version",no_argument,0,0},
    {"vrs",no_argument,0,0},
    /* Long options with argument, no short option counterpart */
    {"bfr_sz_hnt",required_argument,0,0}, /* [B] Buffer size hint */
    {"buffer_size_hint",required_argument,0,0}, /* [B] Buffer size hint */
    {"hdr_pad",required_argument,0,0},
    {"header_pad",required_argument,0,0},
    /* Long options with short counterparts */
    {"attribute",required_argument,0,'a'},
    {"debug",required_argument,0,'D'},
    {"nco_dbg_lvl",required_argument,0,'D'},
    {"dimension",required_argument,0,'d'},
    {"dmn",required_argument,0,'d'},
    {"group",required_argument,0,'g'},
    {"grp",required_argument,0,'g'},
    {"history",no_argument,0,'h'},
    {"hst",no_argument,0,'h'},
    {"local",required_argument,0,'l'},
    {"lcl",required_argument,0,'l'},
    {"overwrite",no_argument,0,'O'},
    {"ovr",no_argument,0,'O'},
    {"output",required_argument,0,'o'},
    {"fl_out",required_argument,0,'o'},
    {"path",required_argument,0,'p'},
    {"revision",no_argument,0,'r'},
    {"variable",required_argument,0,'v'},
    {"help",no_argument,0,'?'},
    {"hlp",no_argument,0,'?'},
    {0,0,0,0}
  };
  int opt_idx=0;  /* Index of current long option into opt_lng array */

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
      if(!strcmp(opt_crr,"bfr_sz_hnt") || !strcmp(opt_crr,"buffer_size_hint")){
        bfr_sz_hnt=strtoul(optarg,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
        if(*sng_cnv_rcd) nco_sng_cnv_err(optarg,"strtoul",sng_cnv_rcd);
      } /* endif cnk */
      if(!strcmp(opt_crr,"cln") || !strcmp(opt_crr,"mmr_cln") || !strcmp(opt_crr,"clean")) flg_cln=True; /* [flg] Clean memory prior to exit */
      if(!strcmp(opt_crr,"drt") || !strcmp(opt_crr,"mmr_drt") || !strcmp(opt_crr,"dirty")) flg_cln=False; /* [flg] Clean memory prior to exit */
      if(!strcmp(opt_crr,"hdf4")) nco_fmt_xtn=nco_fmt_xtn_hdf4; /* [enm] Treat file as HDF4 */
      if(!strcmp(opt_crr,"hdr_pad") || !strcmp(opt_crr,"header_pad")){
        hdr_pad=strtoul(optarg,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
        if(*sng_cnv_rcd) nco_sng_cnv_err(optarg,"strtoul",sng_cnv_rcd);
      } /* endif "hdr_pad" */
      if(!strcmp(opt_crr,"ram_all") || !strcmp(opt_crr,"open_ram") || !strcmp(opt_crr,"diskless_all")) RAM_OPEN=True; /* [flg] Create file in RAM */
      if(!strcmp(opt_crr,"vrs") || !strcmp(opt_crr,"version")){
        (void)nco_vrs_prn(CVS_Id,CVS_Revision);
        nco_exit(EXIT_SUCCESS);
      } /* endif "vrs" */
    } /* opt != 0 */
    /* Process short options */
    switch(opt){
    case 0: /* Long options have already been processed, return */
      break;
    case 'A': /* Toggle FORCE_APPEND */
      FORCE_APPEND=!FORCE_APPEND;
      break;
    case 'a': /* Copy argument for later processing */
      att_rnm_arg[nbr_att_rnm]=(char *)strdup(optarg);
      nbr_att_rnm++;
      break;
    case 'D': /* Debugging level. Default is 0. */
      nco_dbg_lvl=(unsigned short int)strtoul(optarg,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
      if(*sng_cnv_rcd) nco_sng_cnv_err(optarg,"strtoul",sng_cnv_rcd);
      nc_set_log_level(nco_dbg_lvl);
      break;
    case 'd': /* Copy argument for later processing */
      dmn_rnm_arg[nbr_dmn_rnm]=(char *)strdup(optarg);
      nbr_dmn_rnm++;
      break;
    case 'g': /* Copy argument for later processing */
      grp_rnm_arg[nbr_grp_rnm]=(char *)strdup(optarg);
      nbr_grp_rnm++;
      break;
    case 'h': /* Toggle appending to history global attribute */
      HISTORY_APPEND=!HISTORY_APPEND;
      break;
    case 'l': /* Local path prefix for files retrieved from remote file system */
      fl_pth_lcl=(char *)strdup(optarg);
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
    case 'v': /* Copy argument for later processing */
      var_rnm_arg[nbr_var_rnm]=(char *)strdup(optarg);
      nbr_var_rnm++;
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
    } /* end switch */
    if(opt_crr) opt_crr=(char *)nco_free(opt_crr);
  } /* end while loop */

  /* Process positional arguments and fill in filenames */
  fl_lst_in=nco_fl_lst_mk(argv,argc,optind,&fl_nbr,&fl_out,&FL_LST_IN_FROM_STDIN);
  if(fl_out) FL_OUT_NEW=True; else fl_out=(char *)strdup(fl_lst_in[0]);

  if(!nbr_var_rnm && !nbr_att_rnm && !nbr_grp_rnm && !nbr_dmn_rnm){
    (void)fprintf(stdout,"%s: ERROR must specify something to rename\n",nco_prg_nm);
    nco_usg_prn();
    nco_exit(EXIT_FAILURE);
  } /* end if */ 

  /* Make uniform list of user-specified rename structures */
  if(nbr_att_rnm > 0) att_rnm_lst=nco_prs_rnm_lst(nbr_att_rnm,att_rnm_arg);
  if(nbr_dmn_rnm > 0) dmn_rnm_lst=nco_prs_rnm_lst(nbr_dmn_rnm,dmn_rnm_arg);
  if(nbr_grp_rnm > 0) grp_rnm_lst=nco_prs_rnm_lst(nbr_grp_rnm,grp_rnm_arg);
  if(nbr_var_rnm > 0) var_rnm_lst=nco_prs_rnm_lst(nbr_var_rnm,var_rnm_arg);

  /* We have final list of variables, dimensions, and attributes to rename */

  /* Parse filename */
  fl_in=nco_fl_nm_prs(fl_in,0,&fl_nbr,fl_lst_in,abb_arg_nbr,fl_lst_abb,fl_pth);
  /* Make sure file is on local system and is readable or die trying */
  fl_in=nco_fl_mk_lcl(fl_in,fl_pth_lcl,&FL_RTR_RMT_LCN);

  if(FL_OUT_NEW){
    /* Obtain user consent, if needed, to overwrite output file (or die trying) */
    if(!FORCE_OVERWRITE) nco_fl_overwrite_prm(fl_out);

    /* Copy input file to output file and then search through output, 
    changing names on the fly. This avoids possible XDR translation
    performance penalty of copying each variable with netCDF. */
    (void)nco_fl_cp(fl_in,fl_out);

    /* Ensure output file is user/owner-writable */
    (void)nco_fl_chmod(fl_out);
  } /* end if FL_OUT_NEW */

  /* Open file enabled for writing. Place file in define mode for renaming. */
  if(RAM_OPEN) md_open=NC_WRITE|NC_DISKLESS; else md_open=NC_WRITE;
  rcd+=nco_fl_open(fl_out,md_open,&bfr_sz_hnt,&nc_id);
  (void)nco_redef(nc_id);

  /* Timestamp end of metadata setup and disk layout */
  rcd+=nco_ddra((char *)NULL,(char *)NULL,&ddra_info);
  ddra_info.tmr_flg=nco_tmr_rgl;

  /* Initialize traversal table */ 
  trv_tbl_init(&trv_tbl); 

  /* Construct GTT (Group Traversal Table), check -v and -g input names and create extraction list*/
  (void)nco_bld_trv_tbl(nc_id,trv_pth,(int)0,NULL,(int)0,NULL,False,False,NULL,(int)0,NULL,(int) 0,False,False,False,False,True,nco_pck_plc_nil,NULL,trv_tbl);

  /* Rename variables */
  for(int idx_var=0;idx_var<nbr_var_rnm;idx_var++){
    int mch_nbr=0; /* [nbr] Number of matching objects */
    nco_bool is_opt=False; /* [flg] Presence is optional (name has '.') */
    size_t mch_fst=0L; /* [nbr] Number of characters by which to offset string comparison */

    /* Initialize */
    if(var_rnm_lst[idx_var].old_nm[0] == opt_chr){
      is_opt=True;
      mch_fst=1L;
    } /* end if */

    /* Compare each variable in user list to each variable name in file */
    for(unsigned int tbl_idx=0;tbl_idx<trv_tbl->nbr;tbl_idx++)
      if(trv_tbl->lst[tbl_idx].nco_typ == nco_obj_typ_var)
	if(!strcmp(var_rnm_lst[idx_var].old_nm+mch_fst,trv_tbl->lst[tbl_idx].nm_fll) || !strcmp(var_rnm_lst[idx_var].old_nm+mch_fst,trv_tbl->lst[tbl_idx].nm)){
	    mch_nbr++;
	    (void)nco_inq_grp_full_ncid(nc_id,trv_tbl->lst[tbl_idx].grp_nm_fll,&grp_id);
	    rcd=nco_inq_varid(grp_id,trv_tbl->lst[tbl_idx].nm,&var_rnm_lst[idx_var].id);
	    (void)nco_rename_var(grp_id,var_rnm_lst[idx_var].id,var_rnm_lst[idx_var].new_nm);
	    if(nco_dbg_lvl >= nco_dbg_fl) (void)fprintf(stdout,"%s: Renamed variable \'%s\' to \'%s\'\n",nco_prg_nm,trv_tbl->lst[tbl_idx].nm_fll,var_rnm_lst[idx_var].new_nm);
	} /* endif */

    if(mch_nbr == 0 && !is_opt){
      (void)fprintf(stdout,"%s: ERROR Required variable \'%s\' is not present in input file. HINT: If presence is intended to be optional, then prefix old variable name with the period character \'%c\', i.e., \'%s -v %c%s,%s\'. With this syntax %s would succeed even if no such variable is in the file.\n",nco_prg_nm_get(),var_rnm_lst[idx_var].old_nm,opt_chr,nco_prg_nm_get(),opt_chr,var_rnm_lst[idx_var].old_nm,var_rnm_lst[idx_var].new_nm,nco_prg_nm_get());
      nco_exit(EXIT_FAILURE);
    }else if(mch_nbr == 0 && is_opt){
      (void)fprintf(stdout,"%s: INFO Optional variable \'%s\' not present in %s, skipping it.\n",nco_prg_nm,var_rnm_lst[idx_var].old_nm+1L,fl_in);
    }else{
      if(nco_dbg_lvl >= nco_dbg_std) (void)fprintf(stdout,"%s: Renamed %d variable%s from \'%s\' to \'%s\'\n",nco_prg_nm,mch_nbr,mch_nbr > 1 ? "s" : "",var_rnm_lst[idx_var].old_nm,var_rnm_lst[idx_var].new_nm);
    } /* end else */

    mch_nbr_var+=mch_nbr;

  } /* end variables */

  /* Rename groups */
  for(int idx_grp=0;idx_grp<nbr_grp_rnm;idx_grp++){
    int mch_nbr=0; /* [nbr] Number of matching objects */
    nco_bool is_opt=False; /* [flg] Presence is optional (name has '.') */
    size_t mch_fst=0L; /* [nbr] Number of characters by which to offset string comparison */

    /* Initialize */
    if(grp_rnm_lst[idx_grp].old_nm[0] == opt_chr){
      is_opt=True;
      mch_fst=1L;
    } /* end if */

    /* Compare each group in user list to each group name in file */
    for(unsigned int tbl_idx=0;tbl_idx<trv_tbl->nbr;tbl_idx++)
      if(trv_tbl->lst[tbl_idx].nco_typ == nco_obj_typ_grp)
	if(!strcmp(grp_rnm_lst[idx_grp].old_nm+mch_fst,trv_tbl->lst[tbl_idx].grp_nm_fll) || !strcmp(grp_rnm_lst[idx_grp].old_nm+mch_fst,trv_tbl->lst[tbl_idx].grp_nm)){
	    mch_nbr++;
	    (void)nco_inq_grp_full_ncid(nc_id,trv_tbl->lst[tbl_idx].grp_nm_fll,&grp_id);
	    (void)nco_rename_grp(grp_id,grp_rnm_lst[idx_grp].new_nm);
	    if(nco_dbg_lvl >= nco_dbg_fl) (void)fprintf(stdout,"%s: Renamed group \'%s\' to \'%s\'\n",nco_prg_nm,trv_tbl->lst[tbl_idx].grp_nm_fll,grp_rnm_lst[idx_grp].new_nm);
	} /* endif */

    if(mch_nbr == 0 && !is_opt){
      (void)fprintf(stdout,"%s: ERROR Required group \'%s\' is not present in input file. HINT: If presence is intended to be optional, then prefix old group name with the period character \'%c\', i.e., \'%s -g %c%s,%s\'. With this syntax %s would succeed even if no such group is in the file.\n",nco_prg_nm_get(),grp_rnm_lst[idx_grp].old_nm,opt_chr,nco_prg_nm_get(),opt_chr,grp_rnm_lst[idx_grp].old_nm,grp_rnm_lst[idx_grp].new_nm,nco_prg_nm_get());
      nco_exit(EXIT_FAILURE);
    }else if(mch_nbr == 0 && is_opt){
      (void)fprintf(stdout,"%s: INFO Optional group \'%s\' not present in %s, skipping it.\n",nco_prg_nm,grp_rnm_lst[idx_grp].old_nm+1L,fl_in);
    }else{
      if(nco_dbg_lvl >= nco_dbg_std) (void)fprintf(stdout,"%s: Renamed %d group%s from \'%s\' to \'%s\'\n",nco_prg_nm,mch_nbr,mch_nbr > 1 ? "s" : "",grp_rnm_lst[idx_grp].old_nm,grp_rnm_lst[idx_grp].new_nm);
    } /* end else */

    mch_nbr_grp+=mch_nbr;

  } /* end groups */

  /* Rename dimensions */
  for(int idx_dmn=0;idx_dmn<nbr_dmn_rnm;idx_dmn++){
    int mch_nbr=0; /* [nbr] Number of matching objects */
    nco_bool is_opt=False; /* [flg] Presence is optional (name has '.') */
    size_t mch_fst=0L; /* [nbr] Number of characters by which to offset string comparison */

    /* Initialize */
    if(dmn_rnm_lst[idx_dmn].old_nm[0] == opt_chr){
      is_opt=True;
      mch_fst=1L;
    } /* end if */

    /* Compare each dimension in user list to each dimension name in file */
    for(unsigned int tbl_idx=0;tbl_idx<trv_tbl->nbr_dmn;tbl_idx++)
      if(!strcmp(dmn_rnm_lst[idx_dmn].old_nm+mch_fst,trv_tbl->lst_dmn[tbl_idx].nm_fll) || !strcmp(dmn_rnm_lst[idx_dmn].old_nm+mch_fst,trv_tbl->lst_dmn[tbl_idx].nm)){
	mch_nbr++;
	(void)nco_inq_grp_full_ncid(nc_id,trv_tbl->lst_dmn[tbl_idx].grp_nm_fll,&grp_id);
	rcd=nco_inq_dimid(grp_id,trv_tbl->lst_dmn[tbl_idx].nm,&dmn_rnm_lst[idx_dmn].id);
	(void)nco_rename_dim(grp_id,dmn_rnm_lst[idx_dmn].id,dmn_rnm_lst[idx_dmn].new_nm);
	if(nco_dbg_lvl >= nco_dbg_fl) (void)fprintf(stdout,"%s: Renamed dimension \'%s\' to \'%s\'\n",nco_prg_nm,trv_tbl->lst_dmn[tbl_idx].nm_fll,dmn_rnm_lst[idx_dmn].new_nm);
      } /* endif */

    if(mch_nbr == 0 && !is_opt){
      (void)fprintf(stdout,"%s: ERROR Required dimension \'%s\' is not present in input file. HINT: If presence is intended to be optional, then prefix old dimension name with the period character \'%c\', i.e., \'%s -d %c%s,%s\'. With this syntax %s would succeed even if no such dimension is in the file.\n",nco_prg_nm_get(),dmn_rnm_lst[idx_dmn].old_nm,opt_chr,nco_prg_nm_get(),opt_chr,dmn_rnm_lst[idx_dmn].old_nm,dmn_rnm_lst[idx_dmn].new_nm,nco_prg_nm_get());
      nco_exit(EXIT_FAILURE);
    }else if(mch_nbr == 0 && is_opt){
      (void)fprintf(stdout,"%s: INFO Optional dimension \'%s\' not present in %s, skipping it.\n",nco_prg_nm,dmn_rnm_lst[idx_dmn].old_nm+1L,fl_in);
    }else{
      if(nco_dbg_lvl >= nco_dbg_std) (void)fprintf(stdout,"%s: Renamed %d dimension%s from \'%s\' to \'%s\'\n",nco_prg_nm,mch_nbr,mch_nbr > 1 ? "s" : "",dmn_rnm_lst[idx_dmn].old_nm,dmn_rnm_lst[idx_dmn].new_nm);
    } /* end else */

    mch_nbr_dmn+=mch_nbr;

  } /* end dimensions */

  /* Rename attributes */
  for(int idx_att=0;idx_att<nbr_att_rnm;idx_att++){
    int mch_nbr=0; /* [nbr] Number of matching objects */
    int var_id;
    nco_bool IS_GLB_GRP_ATT=False; /* [flg] Attribute is Global or Group attribute */
    nco_bool has_obj_nm=False; /* [flg] User supplied object name (old_name contains '@') */
    nco_bool obj_is_opt=False; /* [flg] Presence of object is optional (name has '.') */
    nco_bool obj_is_grp=False; /* [flg] Object with attribute is group */
    nco_bool obj_is_var=False; /* [flg] Object with attribute is variable */
    nco_bool att_is_opt=False; /* [flg] Presence of attribute is optional (name has '.') */
    nco_bool mch_obj=False; /* [flg] User-specified object matches current object */
    int rcd_att=0;
    size_t obj_mch_fst=0L; /* [nbr] Number of characters by which to offset object string comparison */
    size_t att_mch_fst=0L; /* [nbr] Number of characters by which to offset attribute string comparison */

    if(strchr(att_rnm_lst[idx_att].old_nm,dlm_chr)){
      has_obj_nm=True; /* [flg] User supplied variable name (old_name contains '@') */

      /* Extract variable name from old name */
      rcd_att=nco_prs_att((att_rnm_lst+idx_att),obj_nm,&IS_GLB_GRP_ATT);
      if(!rcd_att){
	(void)fprintf(stdout,"%s: ERROR Could not parse obj_nm@att_nm string \"%s\"\n",nco_prg_nm,att_rnm_lst[idx_att].old_nm);
	nco_exit(EXIT_FAILURE);
      } /* end if */ 

      /* Initialize */
      if(att_rnm_lst[idx_att].old_nm[0] == opt_chr){
	obj_is_opt=True;
	obj_mch_fst=1L;
      } /* end if */
    } /* !strchr() */

    obj_is_var=False;
    obj_is_grp=False;
    mch_obj=False;
    
    /* Compare each attribute in user list to each attribute name in file */
    for(unsigned int tbl_idx=0;tbl_idx<trv_tbl->nbr;tbl_idx++){
      if(has_obj_nm){
	/* Object name is provided so only consider matching objects */
	if(!strcmp(obj_nm+obj_mch_fst,trv_tbl->lst[tbl_idx].nm_fll) || !strcmp(obj_nm+obj_mch_fst,trv_tbl->lst[tbl_idx].nm)){
	  (void)nco_inq_grp_full_ncid(nc_id,trv_tbl->lst[tbl_idx].grp_nm_fll,&grp_id);
	  mch_obj=True;
	  if(trv_tbl->lst[tbl_idx].nco_typ == nco_obj_typ_var){
	    obj_is_var=True;
	    rcd=nco_inq_varid(grp_id,trv_tbl->lst[tbl_idx].nm,&var_id);
	  }else{
	    obj_is_grp=True;
	    IS_GLB_GRP_ATT=True;
	  } /* endif matches entry in traversal table */
	} /* strcmp() */
	  if(!strcmp(obj_nm+obj_mch_fst,"global") || !strcmp(obj_nm+obj_mch_fst,"group")){
	    obj_is_grp=True;
	    /* Forgive omission of period for global/group attributes. Users aren't perfect :) */
	    obj_is_opt=True; 
	    IS_GLB_GRP_ATT=True;
	    grp_id=nc_id;
	  } /* endif  */
	  if(IS_GLB_GRP_ATT){
	    (void)fprintf(stdout,"%s: INFO Assuming \"%s\" refers to a Global or Group attribute\n",nco_prg_nm,att_rnm_lst[idx_att].old_nm);
	    var_id=NC_GLOBAL;
	  } /* !IS_GLB_GRP_ATT */

	  if(!obj_is_grp && !obj_is_var){
	    /* Object is not in traversal table */
	    if(obj_is_opt){
	      (void)fprintf(stdout,"%s: INFO Optional object \'%s\' not present in %s, skipping it.\n",nco_prg_nm,obj_nm+obj_mch_fst,fl_in);
	      /* Optional variable not found, continue to next attribute in list */
	      continue;
	    }else{
	      (void)fprintf(stdout,"%s: ERROR Required object \'%s\' not present in %s.\n",nco_prg_nm,obj_nm,fl_in);
              nco_err_exit(rcd,"main");
	    } /* !obj_is_opt */
	  } /* endif specified object not found */
	} /* !has_obj_nm */

	if(att_is_opt) rcd=nco_inq_attid_flg(grp_id,var_id,att_rnm_lst[idx_att].old_nm+att_mch_fst,&att_rnm_lst[idx_att].id); else rcd=nco_inq_attid(grp_id,var_id,att_rnm_lst[idx_att].old_nm,&att_rnm_lst[idx_att].id);
	if(att_is_opt && rcd != NC_NOERR){
	  (void)fprintf(stdout,"%s: INFO Optional attribute \'%s\' not present in object \'%s\', skipping it.\n",nco_prg_nm,att_rnm_lst[idx_att].old_nm+att_mch_fst,obj_nm+obj_mch_fst);
	  continue;
	} /* endif optional attribute */
	if(!att_is_opt && rcd != NC_NOERR){
	  (void)fprintf(stdout,"%s: ERROR Required attribute \'%s\' not present in object \'%s\'\n",nco_prg_nm,att_rnm_lst
[idx_att].old_nm+att_mch_fst,obj_nm+obj_mch_fst);
	  (void)fprintf(stdout,"%s: ERROR Required attribute \'%s\' not present in %s \'%s\'. HINT: If attribute presence is intended to be optional, then prefix attribute name with the period character \'%c\', e.g., %c%s. With this syntax %s would succeed even if no variables or groups contained the attribute. If attribute is intended to be renamed only in a specific variable, then prepend the variable name plus an at-sign \'%c\' to the attribute name, e.g., var_nm%catt_nm. If attribute presence is required only for global or group attributes, then prefix attribute name with \"global\" and an at-sign, e.g., global%catt_nm.\n",nco_prg_nm_get(),att_rnm_lst[idx_att].old_nm,obj_is_var ? "variable " : "group",obj_nm+obj_mch_fst,opt_chr,opt_chr,att_rnm_lst[idx_att].old_nm,nco_prg_nm_get(),dlm_chr,dlm_chr,dlm_chr);
	    nco_err_exit(rcd,"main");
	} /* endif required attribute */

	(void)nco_rename_att(grp_id,var_id,att_rnm_lst[idx_att].old_nm+obj_mch_fst,att_rnm_lst[idx_att].new_nm);
	mch_nbr++;
	if(nco_dbg_lvl >= nco_dbg_std) (void)fprintf(stdout,"%s: Renamed attribute \'%s\' to \'%s\' for %s \'%s\'\n",nco_prg_nm,att_rnm_lst[idx_att].old_nm,att_rnm_lst[idx_att].new_nm,obj_is_var ? "variable" : "group",obj_nm+obj_mch_fst);
    } /* end loop over traversal table */

    if(nco_dbg_lvl >= nco_dbg_std) (void)fprintf(stdout,"%s: Renamed %d attribute%s from \'%s\' to \'%s\'\n",nco_prg_nm,mch_nbr,mch_nbr > 1 ? "s" : "",att_rnm_lst[idx_att].old_nm,att_rnm_lst[idx_att].new_nm);

    mch_nbr_att+=mch_nbr;

  } /* end attributes */

  if(nco_dbg_lvl >= nco_dbg_std){
    (void)fprintf(stdout,"%s: Renamed %d variable%s in total\n",nco_prg_nm,mch_nbr_var,mch_nbr_var > 1 ? "s" : "");
    (void)fprintf(stdout,"%s: Renamed %d group%s in total\n",nco_prg_nm,mch_nbr_grp,mch_nbr_grp > 1 ? "s" : "");
    (void)fprintf(stdout,"%s: Renamed %d dimension%s in total\n",nco_prg_nm,mch_nbr_dmn,mch_nbr_dmn > 1 ? "s" : "");
    (void)fprintf(stdout,"%s: Renamed %d attribute%s in total\n",nco_prg_nm,mch_nbr_att,mch_nbr_att > 1 ? "s" : "");
  } /* endif dbg */

  /* Catenate time-stamped command line to "history" global attribute */
  if(HISTORY_APPEND) (void)nco_hst_att_cat(nc_id,cmd_ln);

#ifdef _OPENMP
  /* fxm: hack to get libxlsmp library linked in */
  (void)omp_in_parallel();
#endif /* !_OPENMP */

  /* Take output file out of define mode */
  if(hdr_pad == 0UL){
    (void)nco_enddef(nc_id);
  }else{
    (void)nco__enddef(nc_id,hdr_pad);
    if(nco_dbg_lvl >= nco_dbg_scl) (void)fprintf(stderr,"%s: INFO Padding header with %lu extra bytes\n",nco_prg_nm_get(),(unsigned long)hdr_pad);
  } /* hdr_pad */

  /* Close the open netCDF file */
  nco_close(nc_id);

  /* Remove local copy of file */
  if(FL_RTR_RMT_LCN && RM_RMT_FL_PST_PRC) (void)nco_fl_rm(fl_in);

  /* Clean memory unless dirty memory allowed */
  if(flg_cln){
    /* ncrename-specific memory */
    for(int idx=0;idx<nbr_att_rnm;idx++) att_rnm_arg[idx]=(char *)nco_free(att_rnm_arg[idx]);
    for(int idx=0;idx<nbr_dmn_rnm;idx++) dmn_rnm_arg[idx]=(char *)nco_free(dmn_rnm_arg[idx]);
    for(int idx=0;idx<nbr_grp_rnm;idx++) grp_rnm_arg[idx]=(char *)nco_free(grp_rnm_arg[idx]);
    for(int idx=0;idx<nbr_var_rnm;idx++) var_rnm_arg[idx]=(char *)nco_free(var_rnm_arg[idx]);
    if(nbr_att_rnm > 0) att_rnm_lst=(rnm_sct *)nco_free(att_rnm_lst);
    if(nbr_dmn_rnm > 0) dmn_rnm_lst=(rnm_sct *)nco_free(dmn_rnm_lst);
    if(nbr_grp_rnm > 0) grp_rnm_lst=(rnm_sct *)nco_free(grp_rnm_lst);
    if(nbr_var_rnm > 0) var_rnm_lst=(rnm_sct *)nco_free(var_rnm_lst);

    /* NCO-generic clean-up */
    /* Free individual strings/arrays */
    if(cmd_ln) cmd_ln=(char *)nco_free(cmd_ln);
    if(fl_in) fl_in=(char *)nco_free(fl_in);
    if(fl_out) fl_out=(char *)nco_free(fl_out);
    if(fl_pth) fl_pth=(char *)nco_free(fl_pth);
    if(fl_pth_lcl) fl_pth_lcl=(char *)nco_free(fl_pth_lcl);
    /* Free lists of strings */
    if(fl_lst_in && fl_lst_abb == NULL) fl_lst_in=nco_sng_lst_free(fl_lst_in,fl_nbr); 
    if(fl_lst_in && fl_lst_abb) fl_lst_in=nco_sng_lst_free(fl_lst_in,1);
    if(fl_lst_abb) fl_lst_abb=nco_sng_lst_free(fl_lst_abb,abb_arg_nbr);
    (void)trv_tbl_free(trv_tbl);
  } /* !flg_cln */

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
