/* $Header: /data/zender/nco_20150216/nco/src/nco/ncks.c,v 1.130 2005-05-26 16:10:17 hmb Exp $ */

/* ncks -- netCDF Kitchen Sink */

/* Purpose: Extract (subsets of) variables from a netCDF file 
   Print them to screen, or copy them to a new file, or both */

/* Copyright (C) 1995--2005 Charlie Zender

   This software may be modified and/or re-distributed under the terms of the GNU General Public License (GPL) Version 2
   The full license text is at http://www.gnu.ai.mit.edu/copyleft/gpl.html 
   and in the file nco/doc/LICENSE in the NCO source distribution.
   
   As a special exception to the terms of the GPL, you are permitted 
   to link the NCO source code with the DODS, HDF, netCDF, and UDUnits
   libraries and to distribute the resulting executables under the terms 
   of the GPL, but in addition obeying the extra stipulations of the 
   DODS, HDF, netCDF, and UDUnits licenses.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
   See the GNU General Public License for more details.
   
   The original author of this software, Charlie Zender, wants to improve it
   with the help of your suggestions, improvements, bug-reports, and patches.
   Please contact the NCO project at http://nco.sf.net or by writing
   Charlie Zender
   Department of Earth System Science
   University of California at Irvine
   Irvine, CA 92697-3100 */

/* Usage:
   ncks in.nc 
   ncks -v one in.nc 
   ncks in.nc foo.nc
   ncks -v one in.nc foo.nc
   ncks -p /ZENDER/tmp -l /data/zender/tmp h0001.nc foo.nc
   ncks -s "%+16.10f\n" -H -C -v three_dmn_var in.nc
   ncks -H -v fl_nm,fl_nm_arr ~/nco/data/in.nc
   ncks -H -d fl_dim,1 -d char_dim,6,12 -v fl_nm,fl_nm_arr ~/nco/data/in.nc
   ncks -H -m -v char_var_nul,char_var_space,char_var_multinul ~/nco/data/in.nc
   ncks -H -C -v three_dmn_rec_var -d time,,,2 in.nc
   ncks -H -C -v lon -d lon,3,1 in.nc */

#ifdef HAVE_CONFIG_H
#include <config.h> /* Autotools tokens */
#endif /* !HAVE_CONFIG_H */

/* Standard C headers */
#include <stdio.h> /* stderr, FILE, NULL, etc. */
#include <stdlib.h> /* atof, atoi, malloc, getopt */
#include <string.h> /* strcmp. . . */
#include <sys/stat.h> /* stat() */
#include <time.h> /* machine time */
#include <unistd.h> /* all sorts of POSIX stuff */
#ifndef HAVE_GETOPT_LONG
#include "nco_getopt.h"
#else /* !NEED_GETOPT_LONG */ 
#ifdef HAVE_GETOPT_H
#include <getopt.h>
#endif /* !HAVE_GETOPT_H */ 
#endif /* HAVE_GETOPT_LONG */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions and C library */
/* #define MAIN_PROGRAM_FILE MUST precede #include libnco.h */
#define MAIN_PROGRAM_FILE
#include "libnco.h" /* netCDF Operator (NCO) library */

int 
main(int argc,char **argv)
{
  bool ALPHABETIZE_OUTPUT=True; /* Option a */
  bool EXCLUDE_INPUT_LIST=False; /* Option c */
  bool FILE_RETRIEVED_FROM_REMOTE_LOCATION;
  bool FL_LST_IN_FROM_STDIN=False; /* [flg] fl_lst_in comes from stdin */
  bool FORCE_64BIT_OFFSET=False; /* Option Z */
  bool FORCE_APPEND=False; /* Option A */
  bool FORCE_OVERWRITE=False; /* Option O */
  bool FORTRAN_IDX_CNV=False; /* Option F */
  bool HISTORY_APPEND=True; /* Option h */
  bool NCO_BNR_WRT=False; /* [flg] Write binary file */
  bool PRN_DMN_IDX_CRD_VAL=True; /* [flg] Print leading dimension/coordinate indices/values Option Q */
  bool PRN_DMN_UNITS=False; /* [flg] Print dimensional units Option u */
  bool PRN_DMN_UNITS_TGL=False; /* [flg] Toggle print dimensional units Option u */
  bool PRN_GLB_METADATA=False; /* [flg] Print global metadata */
  bool PRN_GLB_METADATA_TGL=False; /* [flg] Toggle print global metadata Option M */
  bool PRN_QUIET=False; /* [flg] Turn off all printing to screen */
  bool PRN_VAR_DATA=False; /* [flg] Print variable data */
  bool PRN_VAR_DATA_TGL=False; /* [flg] Toggle print variable data Option H */
  bool PRN_VAR_METADATA=False; /* [flg] Print variable metadata */
  bool PRN_VAR_METADATA_TGL=False; /* [flg] Toggle print variable metadata Option m */
  bool PRN_VRB=False; /* [flg] Print data and metadata by default */
  bool PROCESS_ALL_COORDINATES=False; /* Option c */
  bool PROCESS_ASSOCIATED_COORDINATES=True; /* Option C */
  bool REMOVE_REMOTE_FILES_AFTER_PROCESSING=True; /* Option R */

  char **fl_lst_abb=NULL; /* Option a */
  char **fl_lst_in;
  char **var_lst_in=NULL;
  char *cmd_ln;
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
  char *time_bfr_srt;
  char dmn_nm[NC_MAX_NAME];

  const char * const CVS_Id="$Id: ncks.c,v 1.130 2005-05-26 16:10:17 hmb Exp $"; 
  const char * const CVS_Revision="$Revision: 1.130 $";
  const char * const opt_sht_lst="aABb:CcD:d:FHhl:MmOo:Pp:qQrRs:uv:xZ-:";

  extern char *optarg;
  extern int optind;
  
  FILE *fp_bnr=NULL_CEWI; /* [fl] Unformatted binary output file handle */

  int abb_arg_nbr=0;
  int fl_nbr=0;
  int fll_md_old; /* [enm] Old fill mode */
  int glb_att_nbr;
  int idx;
  int in_id;  
  int jdx;
  int lmt_nbr=0; /* Option d. NB: lmt_nbr gets incremented */
  int nbr_dmn_fl;
  int nbr_var_fl;
  int nbr_xtr=0; /* nbr_xtr will not otherwise be set for -c with no -v */
  int opt;
  int rcd=NC_NOERR; /* [rcd] Return code */
  int rec_dmn_id=NCO_REC_DMN_UNDEFINED;
  int var_lst_in_nbr=0;
    
  lmt_sct **lmt;

  lmt_all_sct **lmt_all_lst; /* Container for *lmt structure */
  lmt_all_sct *lmt_all_crr; /* Current lmt_all structure */

  long dmn_sz;

  nm_id_sct *xtr_lst=NULL; /* xtr_lst may be alloc()'d from NULL with -c option */

  time_t time_crr_time_t;

  static struct option opt_lng[]=
    { /* Structure ordered by short option key if possible */
      /* Long options with argument */
      {"cmp",no_argument,0,0},
      {"compiler",no_argument,0,0},
      /* Long options with short counterparts */
      {"abc",no_argument,0,'a'},
      {"alphabetize",no_argument,0,'a'},
      {"append",no_argument,0,'A'},
      {"apn",no_argument,0,'A'},
      {"bnr",no_argument,0,'B'},
      {"binary",no_argument,0,'B'},
      {"binary-file",required_argument,0,'b'},
      {"fl_bnr",required_argument,0,'b'},
      {"coords",no_argument,0,'c'},
      {"crd",no_argument,0,'c'},
      {"no-coords",no_argument,0,'C'},
      {"no-crd",no_argument,0,'C'},
      {"debug",required_argument,0,'D'},
      {"dbg_lvl",required_argument,0,'D'},
      {"dimension",required_argument,0,'d'},
      {"dmn",required_argument,0,'d'},
      {"fortran",no_argument,0,'F'},
      {"ftn",no_argument,0,'F'},
      {"history",no_argument,0,'h'},
      {"hst",no_argument,0,'h'},
      {"hieronymus",no_argument,0,'H'}, /* fxm: need better mnemonic for -H */
      {"local",required_argument,0,'l'},
      {"lcl",required_argument,0,'l'},
      {"lcl",required_argument,0,'l'},
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
      {"version",no_argument,0,'r'},
      {"vrs",no_argument,0,'r'},
      {"sng",required_argument,0,'s'},
      {"string",required_argument,0,'s'},
      {"format",required_argument,0,'s'},
      {"fmt",required_argument,0,'s'},
      {"units",no_argument,0,'u'},
      {"variable",required_argument,0,'v'},
      {"exclude",no_argument,0,'x'},
      {"xcl",no_argument,0,'x'},
      {"64-bit-offset",no_argument,0,'Z'},
      {"help",no_argument,0,'?'},
      {0,0,0,0}
    }; /* end opt_lng */
  int opt_idx=0; /* Index of current long option into opt_lng array */

  /* Start clock and save command line */ 
  cmd_ln=nco_cmd_ln_sng(argc,argv);
  time_crr_time_t=time((time_t *)NULL);
  time_bfr_srt=ctime(&time_crr_time_t); time_bfr_srt=time_bfr_srt; /* Avoid compiler warning until variable is used for something */
  fl_bnr=(char *)strdup("ncks.bnr");
  
  /* Get program name and set program enum (e.g., prg=ncra) */
  prg_nm=prg_prs(argv[0],&prg);

  /* Parse command line arguments */
  while(1){
    /* getopt_long_only() allows a single dash '-' to prefix long options as well */
    opt=getopt_long(argc,argv,opt_sht_lst,opt_lng,&opt_idx);
    /* NB: access to opt_crr is only valid when long_opt was detected */
    if(opt == EOF) break; /* Parse positional arguments once getopt_long() returns EOF */
    opt_crr=(char *)strdup(opt_lng[opt_idx].name);

    /* Process long options without short option counterparts */
    if(opt == 0){
      /* NB: This is the sole long-option-without-short-counterpart in NCO 
	 Let this serve as a template for more such options */
      if(!strcmp(opt_crr,"cmp") || !strcmp(opt_crr,"compiler")){
	(void)fprintf(stdout,"%s\n",nco_cmp_get());
	if(opt_crr != NULL) opt_crr=(char *)nco_free(opt_crr);
	nco_exit(EXIT_SUCCESS);
      } /* endif "cmp" */
    } /* opt != 0 */
    switch(opt){
    case 0: /* Long options have already been processed, return */
      break;
    case 'a': /* Toggle ALPHABETIZE_OUTPUT */
      ALPHABETIZE_OUTPUT=!ALPHABETIZE_OUTPUT;
      break;
    case 'A': /* Toggle FORCE_APPEND */
      FORCE_APPEND=!FORCE_APPEND;
      break;
    case 'B': /* Set NCO_BNR_WRT */
      NCO_BNR_WRT=True;
      break;
    case 'b': /* Set file for binary output */
      NCO_BNR_WRT=True;
      fl_bnr=(char *)strdup(optarg);
      break;
    case 'C': /* Extract all coordinates associated with extracted variables? */
      PROCESS_ASSOCIATED_COORDINATES=!PROCESS_ASSOCIATED_COORDINATES;
      break;
    case 'c': /* Add all coordinates to extraction list? */
      PROCESS_ALL_COORDINATES=True;
      break;
    case 'D': /* Debugging level. Default is 0. */
      dbg_lvl=(unsigned short)strtol(optarg,(char **)NULL,10);
      break;
    case 'd': /* Copy argument for later processing */
      lmt_arg[lmt_nbr]=(char *)strdup(optarg);
      lmt_nbr++;
      break;
    case 'H': /* Toggle printing data to screen */
      PRN_VAR_DATA_TGL=True;
      break;
    case 'h': /* Toggle appending to history global attribute */
      HISTORY_APPEND=!HISTORY_APPEND;
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
    case 'F': /* Toggle index convention. Default is 0-based arrays (C-style). */
      FORTRAN_IDX_CNV=!FORTRAN_IDX_CNV;
      break;
    case 'P': /* Print data to screen, maximal verbosity */
      PRN_VRB=True;
      PROCESS_ASSOCIATED_COORDINATES=!PROCESS_ASSOCIATED_COORDINATES;
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
      REMOVE_REMOTE_FILES_AFTER_PROCESSING=!REMOVE_REMOTE_FILES_AFTER_PROCESSING;
      PRN_QUIET=True; /* [flg] Turn off all printing to screen */
      break;
    case 'r': /* Print CVS program information and copyright notice */
      (void)copyright_prn(CVS_Id,CVS_Revision);
      (void)nco_lbr_vrs_prn();
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
      (void)nco_lst_comma2hash(optarg_lcl);
      var_lst_in=lst_prs_2D(optarg_lcl,",",&var_lst_in_nbr);
      optarg_lcl=(char *)nco_free(optarg_lcl);
      nbr_xtr=var_lst_in_nbr;
       break;
    case 'x': /* Exclude rather than extract variables specified with -v */
      EXCLUDE_INPUT_LIST=True;
      break;
    case 'Z': /* [flg] Create output file with 64-bit offsets */
      FORCE_64BIT_OFFSET=True;
      break;
    case '?': /* Print proper usage */
      (void)nco_usg_prn();
      nco_exit(EXIT_FAILURE);
      break;
    case '-': /* Long options are not allowed */
      (void)fprintf(stderr,"%s: ERROR Long options are not available in this build. Use single letter options instead.\n",prg_nm_get());
      nco_exit(EXIT_FAILURE);
      break;
    default: /* Print proper usage */
      (void)nco_usg_prn();
      nco_exit(EXIT_FAILURE);
      break;
    } /* end switch */
    if(opt_crr != NULL) opt_crr=(char *)nco_free(opt_crr);
  } /* end while loop */

  /* Process positional arguments and fill in filenames */
  fl_lst_in=nco_fl_lst_mk(argv,argc,optind,&fl_nbr,&fl_out,&FL_LST_IN_FROM_STDIN);
  
  /* Make uniform list of user-specified dimension limits */
  lmt=nco_lmt_prs(lmt_nbr,lmt_arg);
  
  /* Parse filename */
  fl_in=nco_fl_nm_prs(fl_in,0,&fl_nbr,fl_lst_in,abb_arg_nbr,fl_lst_abb,fl_pth);
  /* Make sure file is on local system and is readable or die trying */
  fl_in=nco_fl_mk_lcl(fl_in,fl_pth_lcl,&FILE_RETRIEVED_FROM_REMOTE_LOCATION);
  /* Open file for reading */
  rcd=nco_open(fl_in,NC_NOWRITE,&in_id);
  
  /* Get number of variables, dimensions, and global attributes in file */
  (void)nco_inq(in_id,&nbr_dmn_fl,&nbr_var_fl,&glb_att_nbr,&rec_dmn_id);
  
  /* Form initial extraction list which may include extended regular expressions */
  xtr_lst=nco_var_lst_mk(in_id,nbr_var_fl,var_lst_in,PROCESS_ALL_COORDINATES,&nbr_xtr);

  /* Change included variables to excluded variables */
  if(EXCLUDE_INPUT_LIST) xtr_lst=nco_var_lst_xcl(in_id,nbr_var_fl,xtr_lst,&nbr_xtr);

  /* Add all coordinate variables to extraction list */
  if(PROCESS_ALL_COORDINATES) xtr_lst=nco_var_lst_add_crd(in_id,nbr_dmn_fl,xtr_lst,&nbr_xtr);

  /* Make sure coordinates associated extracted variables are also on extraction list */
  if(PROCESS_ASSOCIATED_COORDINATES) xtr_lst=nco_var_lst_ass_crd_add(in_id,xtr_lst,&nbr_xtr);

  /* Sort extraction list alphabetically or by variable ID */
  if(nbr_xtr > 1) xtr_lst=nco_lst_srt_nm_id(xtr_lst,nbr_xtr,ALPHABETIZE_OUTPUT);
    
  /* We now have final list of variables to extract. Phew. */
  
  /* Place all dimensions in lmt_all_lst. A 2D variable */
  lmt_all_lst=(lmt_all_sct **)nco_malloc(nbr_dmn_fl*sizeof(lmt_all_sct*));

  for(idx=0;idx<nbr_dmn_fl;idx++){
    (void)nco_inq_dim(in_id,idx,dmn_nm,&dmn_sz);
    lmt_all_lst[idx]=(lmt_all_sct *)nco_malloc(sizeof(lmt_all_sct));
    lmt_all_crr=lmt_all_lst[idx];
    lmt_all_crr->lmt_dmn=(lmt_sct **)nco_malloc(sizeof(lmt_sct *));
    lmt_all_crr->lmt_dmn[0]=(lmt_sct *)nco_malloc(sizeof(lmt_sct));
    lmt_all_crr->dmn_nm=strdup(dmn_nm);
    lmt_all_crr->lmt_dmn_nbr=1;
    lmt_all_crr->dmn_sz_org=dmn_sz;
    lmt_all_crr->WRP=False;
    lmt_all_crr->BASIC_DMN=True;
    /* Initialize lmt struct */
    lmt_all_crr->lmt_dmn[0]->nm=lmt_all_crr->dmn_nm;
    lmt_all_crr->lmt_dmn[0]->id=idx;
    lmt_all_crr->lmt_dmn[0]->is_rec_dmn=(idx == rec_dmn_id ? True : False);
    lmt_all_crr->lmt_dmn[0]->srt=0L;
    lmt_all_crr->lmt_dmn[0]->end=dmn_sz-1L;
    lmt_all_crr->lmt_dmn[0]->cnt=dmn_sz;
    lmt_all_crr->lmt_dmn[0]->srd=1L;
    /* Flag to show that struct has been inialized. fxm: A HACK */
    lmt_all_crr->lmt_dmn[0]->lmt_typ=-1;
  } /* end loop over dimensions */

  /* Add user specified limits lmt_all_lst */
  for(idx=0;idx<lmt_nbr;idx++){
    /* Find coordinate/dimension values associated with user-specified limits */
    (void)nco_lmt_evl(in_id,lmt[idx],0L,FORTRAN_IDX_CNV);
    for(jdx=0;jdx<nbr_dmn_fl;jdx++) {
      if(!strcmp(lmt[idx]->nm,lmt_all_lst[jdx]->dmn_nm)){   
	lmt_all_crr=lmt_all_lst[jdx];
	lmt_all_crr->BASIC_DMN=False;
	if(lmt_all_crr->lmt_dmn[0]->lmt_typ == -1) { 
	  lmt_all_crr->lmt_dmn[0]=lmt[idx]; 
	}else{ 
	  lmt_all_crr->lmt_dmn=(lmt_sct **)nco_realloc(lmt_all_crr->lmt_dmn,((lmt_all_crr->lmt_dmn_nbr)+1)*sizeof(lmt_sct *));
	  lmt_all_crr->lmt_dmn[(lmt_all_crr->lmt_dmn_nbr)++]=lmt[idx];
	} /* endif */
	break;
      } /* end if */
    } /* end loop over dimensions */
    /* Dimension in limit not found */
    if(jdx == nbr_dmn_fl){
      (void)fprintf(stderr,"Unable to find limit dimension %s in list\n ",lmt[idx]->nm);
      nco_exit(EXIT_FAILURE);
    } /* end if err */
  } /* end loop over idx */       
  
  /* Split up wrapped limits */
  for(idx=0;idx<nbr_dmn_fl;idx++)
    if(lmt_all_lst[idx]->BASIC_DMN == False)
      (void)nco_msa_wrp_splt(lmt_all_lst[idx]);
  
  /* Find and store final size of each dimension */
  for(idx=0;idx<nbr_dmn_fl;idx++){
    (void)nco_msa_clc_cnt(lmt_all_lst[idx]);
  } /* end loop over dimensions */
  
  if(PRN_VRB || (fl_out == NULL && !PRN_VAR_DATA_TGL && !PRN_VAR_METADATA_TGL && !PRN_GLB_METADATA_TGL)){
    /* Verbose printing simply means assume user wants the deluxe frills by default */
    if(PRN_DMN_UNITS_TGL) PRN_DMN_UNITS=False; else PRN_DMN_UNITS=True;
    if(PRN_VAR_DATA_TGL) PRN_VAR_DATA=False; else PRN_VAR_DATA=True;
    if(PRN_VAR_METADATA_TGL) PRN_VAR_METADATA=False; else PRN_VAR_METADATA=True;
    /* Assume user wants to see global metadata unless variable extraction is invoked */
    if(var_lst_in == NULL) PRN_GLB_METADATA=True;
    if(PRN_GLB_METADATA_TGL) PRN_GLB_METADATA=!PRN_GLB_METADATA;
  }else{ /* end if PRN_VRB */
    /* Default is to print data and metadata to screen if output file is not specified */
    if(PRN_DMN_UNITS_TGL) PRN_DMN_UNITS=True; else PRN_DMN_UNITS=False;
    if(PRN_VAR_DATA_TGL) PRN_VAR_DATA=True; else PRN_VAR_DATA=False;
    if(PRN_VAR_METADATA_TGL) PRN_VAR_METADATA=True; else PRN_VAR_METADATA=False;
    if(PRN_GLB_METADATA_TGL) PRN_GLB_METADATA=True; else PRN_GLB_METADATA=False;
  } /* endelse */

  /* PRN_QUIET is catch-all which turns off all printing to screen */
  if(PRN_QUIET) PRN_VAR_DATA=PRN_VAR_METADATA=PRN_GLB_METADATA=False;

  if(NCO_BNR_WRT && fl_out == NULL){
    /* Native binary files depend on writing netCDF file to enter generic I/O logic */
    (void)fprintf(stdout,"%s: ERROR Native binary files cannot be written unless netCDF output filename also specified. HINT: Repeat command with throw-away netCDF file specified for output file (e.g., -o foo.nc)\n",prg_nm_get());
  } /* endif NCO_BNR_WRT */
    
  if(fl_out != NULL){
    int out_id;  
    
    /* Open output file */
    fl_out_tmp=nco_fl_out_open(fl_out,FORCE_APPEND,FORCE_OVERWRITE,FORCE_64BIT_OFFSET,&out_id);
    
    /* Copy global attributes */
    (void)nco_att_cpy(in_id,out_id,NC_GLOBAL,NC_GLOBAL,True);
    
    /* Catenate timestamped command line to "history" global attribute */
    if(HISTORY_APPEND) (void)nco_hst_att_cat(out_id,cmd_ln);
  cmd_ln=(char *)nco_free(cmd_ln);
    
    for(idx=0;idx<nbr_xtr;idx++){
      int var_out_id;

      /* Define variable in output file */
      if(lmt_nbr > 0) var_out_id=nco_cpy_var_dfn_lmt(in_id,out_id,rec_dmn_id,xtr_lst[idx].nm,lmt_all_lst,nbr_dmn_fl); else var_out_id=nco_cpy_var_dfn(in_id,out_id,rec_dmn_id,xtr_lst[idx].nm);
      /* Copy variable's attributes */
      (void)nco_att_cpy(in_id,out_id,xtr_lst[idx].id,var_out_id,True);
    } /* end loop over idx */
    
    /* Turn off default filling behavior to enhance efficiency */
    rcd=nco_set_fill(out_id,NC_NOFILL,&fll_md_old);
  
    /* Take output file out of define mode */
    (void)nco_enddef(out_id);
    
    /* [fnc] Open unformatted binary data file for writing */
    if(NCO_BNR_WRT) fp_bnr=nco_bnr_open(fl_bnr);

    /* Copy variable data */
    for(idx=0;idx<nbr_xtr;idx++){
      if(dbg_lvl > 2 && !NCO_BNR_WRT) (void)fprintf(stderr,"%s, ",xtr_lst[idx].nm);
      if(dbg_lvl > 0) (void)fflush(stderr);
      /* Old hyperslab routines */
      /* if(lmt_nbr > 0) (void)nco_cpy_var_val_lmt(in_id,out_id,fp_bnr,NCO_BNR_WRT,xtr_lst[idx].nm,lmt,lmt_nbr); else (void)nco_cpy_var_val(in_id,out_id,fp_bnr,NCO_BNR_WRT,xtr_lst[idx].nm); */
      /* Multi-slab routines */
      if(lmt_nbr > 0) (void)nco_cpy_var_val_mlt_lmt(in_id,out_id,fp_bnr,NCO_BNR_WRT,xtr_lst[idx].nm,lmt_all_lst,nbr_dmn_fl); else (void)nco_cpy_var_val(in_id,out_id,fp_bnr,NCO_BNR_WRT,xtr_lst[idx].nm);
    } /* end loop over idx */

    /* [fnc] Close unformatted binary data file */
    if(NCO_BNR_WRT) (void)nco_bnr_close(fp_bnr,fl_bnr);

    /* Close output file and move it from temporary to permanent location */
    (void)nco_fl_out_cls(fl_out,fl_out_tmp,out_id);

  } /* end if fl_out != NULL */
  
  if(PRN_GLB_METADATA){
    (void)fprintf(stdout,"Opened file %s: dimensions = %i, variables = %i, global atts. = %i, id = %i\n",fl_in,nbr_dmn_fl,nbr_var_fl,glb_att_nbr,in_id);
    if(rec_dmn_id != NCO_REC_DMN_UNDEFINED){
      char rec_dmn_nm[NC_MAX_NAME];
      long rec_dmn_sz;
      
      (void)nco_inq_dim(in_id,rec_dmn_id,rec_dmn_nm,&rec_dmn_sz);
      (void)fprintf(stdout,"Record dimension: name = %s, size = %li\n\n",rec_dmn_nm,rec_dmn_sz);
    } /* end if */
    
    /* Print all global attributes */
    (void)nco_prn_att(in_id,NC_GLOBAL);
  } /* endif PRN_GLB_METADATA */
  
  if(PRN_VAR_METADATA){
    for(idx=0;idx<nbr_xtr;idx++){
      /* Print variable's definition */
      (void)nco_prn_var_dfn(in_id,xtr_lst[idx].nm);
      /* Print variable's attributes */
      (void)nco_prn_att(in_id,xtr_lst[idx].id);
    } /* end loop over idx */
  } /* end if PRN_VAR_METADATA */

  /* if(PRN_VAR_DATA){
    for(idx=0;idx<nbr_xtr;idx++) (void)nco_prn_var_val_lmt(in_id,xtr_lst[idx].nm,lmt,lmt_nbr,dlm_sng,FORTRAN_IDX_CNV,PRN_DMN_UNITS,PRN_DMN_IDX_CRD_VAL);
    } */

  if(PRN_VAR_DATA){
    for(idx=0;idx<nbr_xtr;idx++) (void)nco_msa_prn_var_val(in_id,xtr_lst[idx].nm,lmt_all_lst,nbr_dmn_fl,dlm_sng,FORTRAN_IDX_CNV,PRN_DMN_UNITS,PRN_DMN_IDX_CRD_VAL);
  } /* end if PRN_VAR_DATA */
  /* Extraction list no longer needed */
  xtr_lst=nco_nm_id_lst_free(xtr_lst,nbr_xtr);
  
  /* Close input netCDF file */
  nco_close(in_id);
  
  /* Remove local copy of file */
  if(FILE_RETRIEVED_FROM_REMOTE_LOCATION && REMOVE_REMOTE_FILES_AFTER_PROCESSING) (void)nco_fl_rm(fl_in);

  /* ncks-unique memory */
  if(fl_bnr != NULL) fl_bnr=(char *)nco_free(fl_bnr);
  /*if(lmt_all_lst != NULL) lmt_all_lst=(lmt_all_sct *)nco_free(lmt_all_lst);*/
  /*  fxm: convert lmt_lst to lmt_all_sct ** so can use this routine to free() */
  //if(nbr_dmn_fl > 0) lmt_all_lst=nco_lmt_all_lst_free(lmt_all_lst,nbr_dmn_fl);

  /* NCO-generic clean-up */
  /* Free individual strings */
  if(fl_in != NULL) fl_in=(char *)nco_free(fl_in);
  if(fl_out != NULL) fl_out=(char *)nco_free(fl_out);
  if(fl_out_tmp != NULL) fl_out_tmp=(char *)nco_free(fl_out_tmp);
  if(fl_pth != NULL) fl_pth=(char *)nco_free(fl_pth);
  if(fl_pth_lcl != NULL) fl_pth_lcl=(char *)nco_free(fl_pth_lcl);
  /* Free lists of strings */
  if(fl_lst_in != NULL && fl_lst_abb == NULL) fl_lst_in=nco_sng_lst_free(fl_lst_in,fl_nbr); 
  if(fl_lst_in != NULL && fl_lst_abb != NULL) fl_lst_in=nco_sng_lst_free(fl_lst_in,1);
  if(fl_lst_abb != NULL) fl_lst_abb=nco_sng_lst_free(fl_lst_abb,abb_arg_nbr);
  if(var_lst_in_nbr > 0) var_lst_in=nco_sng_lst_free(var_lst_in,var_lst_in_nbr);
  /* Free limits */
  for(idx=0;idx<lmt_nbr;idx++) lmt_arg[idx]=(char *)nco_free(lmt_arg[idx]);
  if(lmt_nbr > 0) lmt=nco_lmt_lst_free(lmt,lmt_nbr);

  if(rcd != NC_NOERR) nco_err_exit(rcd,"main");
  nco_exit_gracefully();
  return EXIT_SUCCESS;
} /* end main() */
