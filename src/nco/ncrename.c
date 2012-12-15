/* $Header: /data/zender/nco_20150216/nco/src/nco/ncrename.c,v 1.149 2012-12-15 04:58:35 zender Exp $ */

/* ncrename -- netCDF renaming operator */

/* Purpose: Rename dimensions, variables, and attributes of a netCDF file */

/* Copyright (C) 1995--2012 Charlie Zender

   License: GNU General Public License (GPL) Version 3
   The full license text is at http://www.gnu.org/copyleft/gpl.html 
   and in the file nco/doc/LICENSE in the NCO source distribution.
   
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
   ncrename -O -g level_1_group_6,obama ~/nco/data/in_grp.nc ~/foo.nc
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

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions and C library */
#include "nco_netcdf.h" /* NCO wrappers for netCDF C library */

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
  nco_bool IS_GLB_GRP_ATT=False; /* [flg] Attribute is Global or Group attribute */
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

  const char * const CVS_Id="$Id: ncrename.c,v 1.149 2012-12-15 04:58:35 zender Exp $"; 
  const char * const CVS_Revision="$Revision: 1.149 $";
  const char * const opt_sht_lst="a:D:d:g:hl:Oo:p:rv:-:";
  const char dlm_chr='@'; /* Character delimiting variable from attribute name  */
  const char opt_chr='.'; /* Character indicating presence of following variable/dimension/attribute in file is optional */
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
  int idx;
  int md_open; /* [enm] Mode flag for nc_open() call */
  int nbr_att_rnm=0; /* Option a. NB: nbr_att_rnm gets incremented */
  int nbr_dmn_rnm=0; /* Option d. NB: nbr_dmn_rnm gets incremented */
  int nbr_grp_rnm=0; /* Option g. NB: nbr_grp_rnm gets incremented */
  int nbr_var_rnm=0; /* Option v. NB: nbr_var_rnm gets incremented */
  int nc_id;  
  int opt;
  int rcd=NC_NOERR; /* [rcd] Return code */
  
  rnm_sct *var_rnm_lst=NULL_CEWI;
  rnm_sct *dmn_rnm_lst=NULL_CEWI;
  rnm_sct *grp_rnm_lst=NULL_CEWI;
  rnm_sct *att_rnm_lst=NULL_CEWI;

  size_t bfr_sz_hnt=NC_SIZEHINT_DEFAULT; /* [B] Buffer size hint */
  size_t hdr_pad=0UL; /* [B] Pad at end of header section */

  static struct option opt_lng[] =
    { /* Structure ordered by short option key if possible */
      /* Long options with no argument, no short option counterpart */
      {"cln",no_argument,0,0}, /* [flg] Clean memory prior to exit */
      {"clean",no_argument,0,0}, /* [flg] Clean memory prior to exit */
      {"mmr_cln",no_argument,0,0}, /* [flg] Clean memory prior to exit */
      {"drt",no_argument,0,0}, /* [flg] Allow dirty memory on exit */
      {"dirty",no_argument,0,0}, /* [flg] Allow dirty memory on exit */
      {"mmr_drt",no_argument,0,0}, /* [flg] Allow dirty memory on exit */
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
      {"dbg_lvl",required_argument,0,'D'},
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
      {0,0,0,0}
    };
  int opt_idx=0;  /* Index of current long option into opt_lng array */

  /* Start timer and save command line */ 
  ddra_info.tmr_flg=nco_tmr_srt;
  rcd+=nco_ddra((char *)NULL,(char *)NULL,&ddra_info);
  ddra_info.tmr_flg=nco_tmr_mtd;
  cmd_ln=nco_cmd_ln_sng(argc,argv);
  
  /* Get program name and set program enum (e.g., prg=ncra) */
  prg_nm=prg_prs(argv[0],&prg);

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
      dbg_lvl=(unsigned short int)strtoul(optarg,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
      if(*sng_cnv_rcd) nco_sng_cnv_err(optarg,"strtoul",sng_cnv_rcd);
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
      (void)fprintf(stderr,"%s: ERROR Long options are not available in this build. Use single letter options instead.\n",prg_nm_get());
      nco_exit(EXIT_FAILURE);
      break;
    default: /* Print proper usage */
      (void)nco_usg_prn();
      nco_exit(EXIT_FAILURE);
    } /* end switch */
    if(opt_crr) opt_crr=(char *)nco_free(opt_crr);
  } /* end while loop */
  
  /* Process positional arguments and fill in filenames */
  fl_lst_in=nco_fl_lst_mk(argv,argc,optind,&fl_nbr,&fl_out,&FL_LST_IN_FROM_STDIN);
  if(fl_out) FL_OUT_NEW=True; else fl_out=(char *)strdup(fl_lst_in[0]);

  if(!nbr_var_rnm && !nbr_att_rnm && !nbr_grp_rnm && !nbr_dmn_rnm){
    (void)fprintf(stdout,"%s: ERROR must specify something to rename\n",prg_nm);
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

  /* Without further ado, change names */
  for(idx=0;idx<nbr_var_rnm;idx++){
    if(var_rnm_lst[idx].old_nm[0] == opt_chr){
      rcd=nco_inq_varid_flg(nc_id,var_rnm_lst[idx].old_nm+1L,&var_rnm_lst[idx].id);
      if(rcd == NC_NOERR){
	(void)nco_rename_var(nc_id,var_rnm_lst[idx].id,var_rnm_lst[idx].new_nm);
	if(dbg_lvl >= nco_dbg_std) (void)fprintf(stderr,"%s: Renamed variable \'%s\' to \'%s\'\n",prg_nm,var_rnm_lst[idx].old_nm+1L,var_rnm_lst[idx].new_nm);
      }else{
	(void)fprintf(stderr,"%s: WARNING Variable \"%s\" not present in %s, skipping it.\n",prg_nm,var_rnm_lst[idx].old_nm+1L,fl_in);
	/* Reset error code */
	rcd=NC_NOERR; 
      } /* end if */
    }else{ /* Variable name does not contain opt_chr so variable presence is required */
      rcd=nco_inq_varid(nc_id,var_rnm_lst[idx].old_nm,&var_rnm_lst[idx].id);
      (void)nco_rename_var(nc_id,var_rnm_lst[idx].id,var_rnm_lst[idx].new_nm);
      if(dbg_lvl >= nco_dbg_std) (void)fprintf(stderr,"%s: Renamed variable \'%s\' to \'%s\'\n",prg_nm,var_rnm_lst[idx].old_nm,var_rnm_lst[idx].new_nm);
    } /* end else */
  } /* end loop over idx */

  for(idx=0;idx<nbr_grp_rnm;idx++){
    if(grp_rnm_lst[idx].old_nm[0] == opt_chr){
      rcd=nco_inq_grp_ncid_flg(nc_id,grp_rnm_lst[idx].old_nm+1L,&grp_rnm_lst[idx].id);
      if(rcd == NC_NOERR){
	(void)nco_rename_grp(nc_id,grp_rnm_lst[idx].id,grp_rnm_lst[idx].new_nm);
	if(dbg_lvl >= nco_dbg_std) (void)fprintf(stderr,"%s: Renamed group \'%s\' to \'%s\'\n",prg_nm,grp_rnm_lst[idx].old_nm+1L,grp_rnm_lst[idx].new_nm);
      }else{
	(void)fprintf(stderr,"%s: WARNING Group \"%s\" not present in %s, skipping it.\n",prg_nm,grp_rnm_lst[idx].old_nm+1L,fl_in);
	/* Reset error code */
	rcd=NC_NOERR; 
      } /* end if */
    }else{ /* Group name does not contain opt_chr so group presence is required */
      rcd=nco_inq_grp_ncid(nc_id,grp_rnm_lst[idx].old_nm,&grp_rnm_lst[idx].id);
      (void)nco_rename_grp(nc_id,grp_rnm_lst[idx].id,grp_rnm_lst[idx].new_nm);
      if(dbg_lvl >= nco_dbg_std) (void)fprintf(stderr,"%s: Renamed group \'%s\' to \'%s\'\n",prg_nm,grp_rnm_lst[idx].old_nm,grp_rnm_lst[idx].new_nm);
    } /* end else */
  } /* end loop over idx */

  for(idx=0;idx<nbr_dmn_rnm;idx++){
    if(dmn_rnm_lst[idx].old_nm[0] == opt_chr){
      
      rcd=nco_inq_dimid_flg(nc_id,dmn_rnm_lst[idx].old_nm+1L,&dmn_rnm_lst[idx].id);
      if(rcd == NC_NOERR){
	(void)nco_rename_dim(nc_id,dmn_rnm_lst[idx].id,dmn_rnm_lst[idx].new_nm);
	if(dbg_lvl >= nco_dbg_std) (void)fprintf(stderr,"%s: Renamed dimension \'%s\' to \'%s\'\n",prg_nm,dmn_rnm_lst[idx].old_nm+1L,dmn_rnm_lst[idx].new_nm);
      }else{
	(void)fprintf(stderr,"%s: WARNING Dimension \'%s\' not present in %s, skipping it.\n",prg_nm,dmn_rnm_lst[idx].old_nm+1L,fl_in);
	/* Reset error code */
	rcd=NC_NOERR; 
      } /* end if */
      
    }else{
      rcd=nco_inq_dimid(nc_id,dmn_rnm_lst[idx].old_nm,&dmn_rnm_lst[idx].id);
      (void)nco_rename_dim(nc_id,dmn_rnm_lst[idx].id,dmn_rnm_lst[idx].new_nm);
      if(dbg_lvl >= nco_dbg_std) (void)fprintf(stderr,"%s: Renamed dimension \'%s\' to \'%s\'\n",prg_nm,dmn_rnm_lst[idx].old_nm,dmn_rnm_lst[idx].new_nm);
    } /* end else */
  } /* end loop over idx */

  if(nbr_att_rnm > 0){
    int nbr_var_fl;
    
    /* Get number of variables in file */
    (void)nco_inq(nc_id,(int *)NULL,&nbr_var_fl,(int *)NULL,(int *)NULL);

    for(idx=0;idx<nbr_att_rnm;idx++){
      int var_id;
      int nbr_rnm=0;
      int rcd_att=0;
      
      /* Rename attribute of single variable... */
      if(strchr(att_rnm_lst[idx].old_nm,dlm_chr)){
	/* Extract variable name from old name */
	char var_nm[NC_MAX_NAME+1];
	rcd_att=nco_prs_att((att_rnm_lst+idx),var_nm,&IS_GLB_GRP_ATT);
	if(!rcd_att){
	  (void)fprintf(stderr,"%s: ERROR Could not parse var_nm@att_nm string \"%s\"\n",prg_nm,att_rnm_lst[idx].old_nm);
	  nco_exit(EXIT_FAILURE);
	} /* end if */ 
	/* Get var_id of variable */
	if(IS_GLB_GRP_ATT){
	  (void)fprintf(stderr,"%s: INFO Assuming \"%s\" refers to a Global or Group attribute\n",prg_nm,att_rnm_lst[idx].old_nm);
	  var_id=NC_GLOBAL;
	}else{ /* !IS_GLB_GRP_ATT */
	  if(var_nm[0] == opt_chr){
	    rcd=nco_inq_varid_flg(nc_id,var_nm+1L,&var_id);
	    if(rcd != NC_NOERR){
	      (void)fprintf(stderr,"%s: WARNING Variable \'%s\' not present in %s, skipping it.\n",prg_nm,var_nm+1L,fl_in);
	      /* Reset error code */
	      rcd=NC_NOERR; 
	      /* Optional variable not found, continue to next attribute in list */
	      continue;
	    } /* end if */
	  }else{ /* Variable name does not contain opt_chr so variable presence is required */
	    rcd=nco_inq_varid(nc_id,var_nm,&var_id);
	  } /* end if */
	} /* !IS_GLB_GRP_ATT */
	if(rcd == NC_NOERR){
	  if(att_rnm_lst[idx].old_nm[0] == opt_chr){
	    /* Preceding opt_chr means attribute need not be present */
	    rcd=nco_inq_attid_flg(nc_id,var_id,att_rnm_lst[idx].old_nm+1L,&att_rnm_lst[idx].id);	  
	    if(rcd == NC_NOERR){
	      (void)nco_rename_att(nc_id,var_id,att_rnm_lst[idx].old_nm+1L,att_rnm_lst[idx].new_nm);
	      nbr_rnm++;
	      if(dbg_lvl >= nco_dbg_std) (void)fprintf(stderr,"%s: Renamed attribute \'%s\' to \'%s\' for variable \'%s\'\n",prg_nm,att_rnm_lst[idx].old_nm+1L,att_rnm_lst[idx].new_nm,(var_nm[0] == opt_chr ? var_nm+1L : var_nm));
	    }else{
	      (void)fprintf(stderr,"%s: WARNING Attribute \'%s\' not present in variable \'%s\'\n",prg_nm,att_rnm_lst[idx].old_nm+1L,(var_nm[0] == opt_chr ? var_nm+1L : var_nm));
	    } /* endelse */
	  }else{ 
	    /* Attribute must be present */
	    rcd=nco_inq_attid(nc_id,var_id,att_rnm_lst[idx].old_nm,&att_rnm_lst[idx].id);
	    if(rcd == NC_NOERR){
	      (void)nco_rename_att(nc_id,var_id,att_rnm_lst[idx].old_nm,att_rnm_lst[idx].new_nm);
	      nbr_rnm++;
	      if(dbg_lvl >= nco_dbg_std) (void)fprintf(stderr,"%s: Renamed attribute \'%s\' to \'%s\' for variable \'%s\'\n",prg_nm,att_rnm_lst[idx].old_nm,att_rnm_lst[idx].new_nm,(var_nm[0] == opt_chr ? var_nm+1L : var_nm));
	    } /* endif attribute is present */
	  } /* endelse attribute must be present */  
	}else{ /* variable not present */
	  (void)fprintf(stderr,"%s: WARNING variable \'%s\' not present in %s\n",prg_nm,var_nm,fl_in);
	}  /* variable not present */
	/* end if renaming single variable */
      }else{ /* ...or rename attribute for all variables... */
	for(var_id=NC_GLOBAL;var_id<nbr_var_fl;var_id++){ /* Start loop at NC_GLOBAL (-1) for global attributes */
	  if(att_rnm_lst[idx].old_nm[0] == opt_chr){
	    /* Rename attribute if variable contains attribute else do nothing */
	    rcd=nco_inq_attid_flg(nc_id,var_id,att_rnm_lst[idx].old_nm+1L,&att_rnm_lst[idx].id);
	    if(rcd == NC_NOERR){
	      (void)nco_rename_att(nc_id,var_id,att_rnm_lst[idx].old_nm+1L,att_rnm_lst[idx].new_nm);
	      nbr_rnm++;
	      /* Inform user which variable had attribute renamed */
	      if(var_id > NC_GLOBAL){
		char var_nm[NC_MAX_NAME+1];
		
		(void)nco_inq_varname(nc_id,var_id,var_nm);
		if(dbg_lvl >= nco_dbg_std) (void)fprintf(stderr,"%s: Renamed attribute \'%s\' to \'%s\' for variable \'%s\'\n",prg_nm,att_rnm_lst[idx].old_nm+1L,att_rnm_lst[idx].new_nm,var_nm);
	      }else{
		if(dbg_lvl >= nco_dbg_std) (void)fprintf(stderr,"%s: Renamed global attribute \'%s\' to \'%s\'\n",prg_nm,att_rnm_lst[idx].old_nm+1L,att_rnm_lst[idx].new_nm);
	      } /* end else */
	    }else{ /* end if attribute was found */
	      /* Reset error code */
	      rcd=NC_NOERR; 
	    } /* end else */
	  }else{
	    /* Rename attribute or die trying */
	    rcd=nco_inq_attid_flg(nc_id,var_id,att_rnm_lst[idx].old_nm,&att_rnm_lst[idx].id);
	    if(rcd == NC_NOERR){
	      (void)nco_rename_att(nc_id,var_id,att_rnm_lst[idx].old_nm,att_rnm_lst[idx].new_nm);
	      nbr_rnm++;
	      /* Inform user which variable had attribute renamed */
	      if(var_id > NC_GLOBAL){
		char var_nm[NC_MAX_NAME+1];
		
		(void)nco_inq_varname(nc_id,var_id,var_nm);
		if(dbg_lvl >= nco_dbg_std) (void)fprintf(stderr,"%s: Renamed attribute \'%s\' to \'%s\' for variable \'%s\'\n",prg_nm,att_rnm_lst[idx].old_nm,att_rnm_lst[idx].new_nm,var_nm);
	      }else{
		if(dbg_lvl >= nco_dbg_std) (void)fprintf(stderr,"%s: Renamed global attribute \'%s\' to \'%s\'\n",prg_nm,att_rnm_lst[idx].old_nm,att_rnm_lst[idx].new_nm);
	      } /* end else */
	    } /* end if */
	  } /* end else */
	} /* end loop over var_id */
      } /* end if renaming attribute for all variables */
      /* See to it that any mandatory renaming was performed, else abort */
      if(nbr_rnm == 0){
	if(att_rnm_lst[idx].old_nm[0] == opt_chr){
	  (void)fprintf(stderr,"%s: WARNING Attribute \'%s\' not renamed because not found in searched variable(s)\n",prg_nm,att_rnm_lst[idx].old_nm+1L);
	}else{
	  (void)fprintf(stdout,"%s: ERROR Attribute \'%s\' not present in %s, aborting.\n",prg_nm,att_rnm_lst[idx].old_nm,fl_in);
	  nco_exit(EXIT_FAILURE);
	} /* end else */
      } /* end if */
      
    } /* end loop over attributes to rename */
     
  } /* end if renaming attributes */
  
  /* Catenate timestamped command line to "history" global attribute */
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
    if(dbg_lvl >= nco_dbg_scl) (void)fprintf(stderr,"%s: INFO Padding header with %lu extra bytes\n",prg_nm_get(),(unsigned long)hdr_pad);
  } /* hdr_pad */

  /* Close the open netCDF file */
  nco_close(nc_id);
  
  /* Remove local copy of file */
  if(FL_RTR_RMT_LCN && RM_RMT_FL_PST_PRC) (void)nco_fl_rm(fl_in);

  /* Clean memory unless dirty memory allowed */
  if(flg_cln){
    /* ncrename-specific memory */
    for(idx=0;idx<nbr_att_rnm;idx++) att_rnm_arg[idx]=(char *)nco_free(att_rnm_arg[idx]);
    for(idx=0;idx<nbr_dmn_rnm;idx++) dmn_rnm_arg[idx]=(char *)nco_free(dmn_rnm_arg[idx]);
    for(idx=0;idx<nbr_grp_rnm;idx++) grp_rnm_arg[idx]=(char *)nco_free(grp_rnm_arg[idx]);
    for(idx=0;idx<nbr_var_rnm;idx++) var_rnm_arg[idx]=(char *)nco_free(var_rnm_arg[idx]);
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
  } /* !flg_cln */
  
  /* End timer */ 
  ddra_info.tmr_flg=nco_tmr_end; /* [enm] Timer flag */
  rcd+=nco_ddra((char *)NULL,(char *)NULL,&ddra_info);

  if(rcd != NC_NOERR) nco_err_exit(rcd,"main");
  nco_exit_gracefully();
  return EXIT_SUCCESS;
} /* end main() */
