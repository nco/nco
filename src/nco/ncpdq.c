/* $Header: /data/zender/nco_20150216/nco/src/nco/ncpdq.c,v 1.14 2004-08-04 21:50:59 zender Exp $ */

/* ncpdq -- netCDF pack, re-dimension, query */

/* Purpose: Pack, re-dimension, query single netCDF file and output to a single file */

/* Copyright (C) 1995--2004 Charlie Zender

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
   ncpdq -O ~/nco/data/in.nc ~/foo.nc;ncks -H ~/foo.nc
   ncpdq -O -D 3 -a lat,lev,lon -v three_dmn_var ~/nco/data/in.nc ~/foo.nc;ncks -H ~/foo.nc
   ncpdq -O -D 3 -a lon,lev,lat -v three_dmn_var ~/nco/data/in.nc ~/foo.nc;ncks -H ~/foo.nc
   ncpdq -O -D 3 -a lat,lev,lon -v PS ~/nco/data/in.nc ~/foo.nc;ncks -H ~/foo.nc
   ncpdq -O -D 3 -a lon,lev,lat -v PS ~/nco/data/in.nc ~/foo.nc;ncks -H ~/foo.nc
*/

#ifdef HAVE_CONFIG_H
#include <config.h> /* Autotools tokens */
#endif /* !HAVE_CONFIG_H */

/* Standard C headers */
#include <math.h> /* sin cos cos sin 3.14159 */
#include <stdio.h> /* stderr, FILE, NULL, etc. */
#include <stdlib.h> /* atof, atoi, malloc, getopt */
#include <string.h> /* strcmp. . . */
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
  bool EXCLUDE_INPUT_LIST=False; /* Option c */
  bool FILE_RETRIEVED_FROM_REMOTE_LOCATION;
  bool FL_LST_IN_FROM_STDIN=False; /* [flg] fl_lst_in comes from stdin */
  bool FORCE_APPEND=False; /* Option A */
  bool FORCE_OVERWRITE=False; /* Option O */
  bool FORTRAN_IDX_CNV=False; /* Option F */
  bool HISTORY_APPEND=True; /* Option h */
  bool NCAR_CCSM_FORMAT;
  bool PROCESS_ALL_COORDINATES=False; /* Option c */
  bool PROCESS_ASSOCIATED_COORDINATES=True; /* Option C */
  bool REDEFINED_RECORD_DIMENSION=False; /* [flg] Re-defined record dimension */
  bool REMOVE_REMOTE_FILES_AFTER_PROCESSING=True; /* Option R */

  bool **dmn_rvr_in=NULL; /* [flg] Reverse dimension */
  bool *dmn_rvr_rdr=NULL; /* [flg] Reverse dimension */

  char **dmn_rdr_lst_in=NULL_CEWI; /* Option a */
  char **fl_lst_abb=NULL; /* Option n */
  char **fl_lst_in=NULL_CEWI;
  char **var_lst_in=NULL_CEWI;
  char *cmd_ln;
  char *fl_in=NULL;
  char *fl_out=NULL; /* Option o */
  char *fl_out_tmp=NULL_CEWI;
  char *fl_pth=NULL; /* Option p */
  char *fl_pth_lcl=NULL; /* Option l */
  char *lmt_arg[NC_MAX_DIMS];
  char *rec_dmn_nm_in=NULL; /* [sng] Record dimension name, original */
  char *rec_dmn_nm_out=NULL; /* [sng] Record dimension name, re-ordered */
  char *rec_dmn_nm_out_crr=NULL; /* [sng] Name of record dimension, if any, required by re-order */
  char *time_bfr_srt;
  
  const char * const CVS_Id="$Id: ncpdq.c,v 1.14 2004-08-04 21:50:59 zender Exp $"; 
  const char * const CVS_Revision="$Revision: 1.14 $";
  const char * const opt_sng="Aa:CcD:d:Fhl:Oo:p:Rrt:v:x-:";
  
  dmn_sct **dim=NULL_CEWI;
  dmn_sct **dmn_out;
  dmn_sct **dmn_rdr=NULL; /* [sct] Dimension structures to be re-ordered */
  
  extern char *optarg;
  extern int optind;
  
  /* Using naked stdin/stdout/stderr in parallel region generates warning
     Copy appropriate filehandle to variable scoped shared in parallel clause */
  FILE * const fp_stderr=stderr; /* [fl] stderr filehandle CEWI */
  FILE * const fp_stdout=stdout; /* [fl] stdout filehandle CEWI */

  int **dmn_idx_out_in; /* [idx] Dimension correspondence, output->input */

  int abb_arg_nbr=0;
  int dmn_rdr_nbr=0; /* [nbr] Number of dimension to re-order */
  int dmn_rdr_nbr_utl=0; /* [nbr] Number of dimension to re-order, utilized */
  int fl_idx=int_CEWI;
  int fl_nbr=0;
  int fll_md_old; /* [enm] Old fill mode */
  int idx=int_CEWI;
  int idx_rdr=int_CEWI;
  int in_id=int_CEWI;  
  int lmt_nbr=0; /* Option d. NB: lmt_nbr gets incremented */
  int nbr_dmn_fl;
  int nbr_dmn_out;
  int nbr_dmn_xtr;
  int nbr_var_fix; /* nbr_var_fix gets incremented */
  int nbr_var_fl;
  int nbr_var_prc; /* nbr_var_prc gets incremented */
  int nbr_xtr=0; /* nbr_xtr won't otherwise be set for -c with no -v */
  int opt;
  int out_id;  
  int rcd=NC_NOERR; /* [rcd] Return code */
  int rec_dmn_id_in=NCO_REC_DMN_UNDEFINED; /* [id] Record dimension ID in input file */
  int dmn_out_idx; /* [idx] Index over output dimension list */
  int dmn_out_idx_rec_in=NCO_REC_DMN_UNDEFINED; /* [idx] Record dimension index in output dimension list, original */
  int thr_nbr=0; /* [nbr] Thread number Option t */
  
  lmt_sct *lmt;
  
  nm_id_sct *dmn_lst;
  nm_id_sct *dmn_rdr_lst;
  nm_id_sct *xtr_lst=NULL; /* xtr_lst may bealloc()'d from NULL with -c option */
  
  time_t time_crr_time_t;
  
  var_sct **var;
  var_sct **var_fix;
  var_sct **var_fix_out;
  var_sct **var_out;
  var_sct **var_prc;
  var_sct **var_prc_out;
  
  static struct option opt_lng[]=
    { /* Structure ordered by short option key if possible */
      {"append",no_argument,0,'A'},
      {"arrange",no_argument,0,'a'},
      {"permute",no_argument,0,'a'},
      {"reorder",no_argument,0,'a'},
      {"rdr",no_argument,0,'a'},
      {"no-coords",no_argument,0,'C'},
      {"no-crd",no_argument,0,'C'},
      {"coords",no_argument,0,'c'},
      {"crd",no_argument,0,'c'},
      {"debug",required_argument,0,'D'},
      {"dbg_lvl",required_argument,0,'D'},
      {"dimension",required_argument,0,'d'},
      {"dmn",required_argument,0,'d'},
      {"fortran",no_argument,0,'F'},
      {"ftn",no_argument,0,'F'},
      {"history",no_argument,0,'h'},
      {"hst",no_argument,0,'h'},
      {"local",required_argument,0,'l'},
      {"lcl",required_argument,0,'l'},
      {"overwrite",no_argument,0,'O'},
      {"ovr",no_argument,0,'O'},
      {"output",required_argument,0,'o'},
      {"fl_out",required_argument,0,'o'},
      {"path",required_argument,0,'p'},
      {"retain",no_argument,0,'R'},
      {"rtn",no_argument,0,'R'},
      {"revision",no_argument,0,'r'},
      {"version",no_argument,0,'r'},
      {"vrs",no_argument,0,'r'},
      {"thr_nbr",required_argument,0,'t'},
      {"threads",required_argument,0,'t'},
      {"omp_num_threads",required_argument,0,'t'},
      {"variable",required_argument,0,'v'},
      {"exclude",no_argument,0,'x'},
      {"xcl",no_argument,0,'x'},
      {"help",no_argument,0,'?'},
      {0,0,0,0}
    }; /* end opt_lng */
  int opt_idx=0; /* Index of current long option into opt_lng array */

  /* Start clock and save command line */ 
  cmd_ln=nco_cmd_ln_sng(argc,argv);
  time_crr_time_t=time((time_t *)NULL);
  time_bfr_srt=ctime(&time_crr_time_t); time_bfr_srt=time_bfr_srt; /* Avoid compiler warning until variable is used for something */

  time_bfr_srt=time_bfr_srt; /* CEWI: Avert compiler warning that variable is set but never used */
  
  /* Get program name and set program enum (e.g., prg=ncra) */
  prg_nm=prg_prs(argv[0],&prg);

  /* Parse command line arguments */
  while((opt = getopt_long(argc,argv,opt_sng,opt_lng,&opt_idx)) != EOF){
    switch(opt){
    case 'A': /* Toggle FORCE_APPEND */
      FORCE_APPEND=!FORCE_APPEND;
      break;
    case 'a': /* Re-order dimensions */
      dmn_rdr_lst_in=lst_prs(optarg,",",&dmn_rdr_nbr);
      break;
    case 'C': /* Extract all coordinates associated with extracted variables? */
      PROCESS_ASSOCIATED_COORDINATES=False;
      break;
    case 'c':
      PROCESS_ALL_COORDINATES=True;
      break;
    case 'D': /* Debugging level. Default is 0. */
      dbg_lvl=(unsigned short)strtol(optarg,(char **)NULL,10);
      break;
    case 'd': /* Copy argument for later processing */
      lmt_arg[lmt_nbr]=(char *)strdup(optarg);
      lmt_nbr++;
      break;
    case 'F': /* Toggle index convention. Default is 0-based arrays (C-style). */
      FORTRAN_IDX_CNV=!FORTRAN_IDX_CNV;
      break;
    case 'h': /* Toggle appending to history global attribute */
      HISTORY_APPEND=!HISTORY_APPEND;
      break;
    case 'l': /* Local path prefix for files retrieved from remote file system */
      fl_pth_lcl=optarg;
      break;
    case 'O': /* Toggle FORCE_OVERWRITE */
      FORCE_OVERWRITE=!FORCE_OVERWRITE;
      break;
    case 'o': /* Name of output file */
      fl_out=(char *)strdup(optarg);
      break;
    case 'p': /* Common file path */
      fl_pth=optarg;
      break;
    case 'R': /* Toggle removal of remotely-retrieved-files. Default is True. */
      REMOVE_REMOTE_FILES_AFTER_PROCESSING=!REMOVE_REMOTE_FILES_AFTER_PROCESSING;
      break;
    case 'r': /* Print CVS program information and copyright notice */
      (void)copyright_prn(CVS_Id,CVS_Revision);
      (void)nco_lbr_vrs_prn();
       nco_exit(EXIT_SUCCESS);
      break;
    case 't': /* Thread number */
      thr_nbr=(int)strtol(optarg,(char **)NULL,10);
      break;
    case 'v': /* Variables to extract/exclude */
      /* Replace commas with hashes when within braces (convert back later) */
      (void)nco_lst_comma2hash(optarg);
      var_lst_in=lst_prs(optarg,",",&nbr_xtr);
      break;
    case 'x': /* Exclude rather than extract variables specified with -v */
      EXCLUDE_INPUT_LIST=True;
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
  
  /* Get number of variables, dimensions, and record dimension ID of input file */
  (void)nco_inq(in_id,&nbr_dmn_fl,&nbr_var_fl,(int *)NULL,&rec_dmn_id_in);
  
  /* Form initial extraction list which may include extended regular expressions */
  xtr_lst=nco_var_lst_mk(in_id,nbr_var_fl,var_lst_in,PROCESS_ALL_COORDINATES,&nbr_xtr);

  /* Change included variables to excluded variables */
  if(EXCLUDE_INPUT_LIST) xtr_lst=nco_var_lst_xcl(in_id,nbr_var_fl,xtr_lst,&nbr_xtr);

  /* Add all coordinate variables to extraction list */
  if(PROCESS_ALL_COORDINATES) xtr_lst=nco_var_lst_add_crd(in_id,nbr_dmn_fl,xtr_lst,&nbr_xtr);

  /* Make sure coordinates associated extracted variables are also on extraction list */
  if(PROCESS_ASSOCIATED_COORDINATES) xtr_lst=nco_var_lst_ass_crd_add(in_id,xtr_lst,&nbr_xtr);

  /* Sort extraction list by variable ID for fastest I/O */
  if(nbr_xtr > 1) xtr_lst=nco_lst_srt(xtr_lst,nbr_xtr,False);
    
  /* Find coordinate/dimension values associated with user-specified limits */
  for(idx=0;idx<lmt_nbr;idx++) (void)nco_lmt_evl(in_id,lmt+idx,0L,FORTRAN_IDX_CNV);
  
  /* Find dimensions associated with variables to be extracted */
  dmn_lst=nco_dmn_lst_ass_var(in_id,xtr_lst,nbr_xtr,&nbr_dmn_xtr);

  /* Fill in dimension structure for all extracted dimensions */
  dim=(dmn_sct **)nco_malloc(nbr_dmn_xtr*sizeof(dmn_sct *));
  for(idx=0;idx<nbr_dmn_xtr;idx++){
    dim[idx]=nco_dmn_fll(in_id,dmn_lst[idx].id,dmn_lst[idx].nm);
  } /* end loop over idx */
  
  /* Merge hyperslab limit information into dimension structures */
  if(lmt_nbr > 0) (void)nco_dmn_lmt_mrg(dim,nbr_dmn_xtr,lmt,lmt_nbr);

  /* Duplicate input dimension structures for output dimension structures */
  nbr_dmn_out=nbr_dmn_xtr;
  dmn_out=(dmn_sct **)nco_malloc(nbr_dmn_out*sizeof(dmn_sct *));
  for(idx=0;idx<nbr_dmn_out;idx++){
    dmn_out[idx]=nco_dmn_dpl(dim[idx]);
    (void)nco_dmn_xrf(dim[idx],dmn_out[idx]);
  } /* end loop over idx */

  /* Is this an NCAR CCSM-format history tape? */
  NCAR_CCSM_FORMAT=nco_ncar_csm_inq(in_id);

  /* Fill in variable structure list for all extracted variables */
  var=(var_sct **)nco_malloc(nbr_xtr*sizeof(var_sct *));
  var_out=(var_sct **)nco_malloc(nbr_xtr*sizeof(var_sct *));
  for(idx=0;idx<nbr_xtr;idx++){
    var[idx]=nco_var_fll(in_id,xtr_lst[idx].id,xtr_lst[idx].nm,dim,nbr_dmn_xtr);
    var_out[idx]=nco_var_dpl(var[idx]);
    (void)nco_xrf_var(var[idx],var_out[idx]);
    (void)nco_xrf_dmn(var_out[idx]);
  } /* end loop over idx */

  /* Divide variable lists into lists of fixed variables and variables to be processed */
  (void)nco_var_lst_dvd(var,var_out,nbr_xtr,NCAR_CCSM_FORMAT,NULL,0,&var_fix,&var_fix_out,&nbr_var_fix,&var_prc,&var_prc_out,&nbr_var_prc);

  /* We now have final list of variables to extract. Phew. */
  if(dbg_lvl > 0){
    for(idx=0;idx<nbr_xtr;idx++) (void)fprintf(stderr,"var[%d]->nm = %s, ->id=[%d]\n",idx,var[idx]->nm,var[idx]->id);
    for(idx=0;idx<nbr_var_fix;idx++) (void)fprintf(stderr,"var_fix[%d]->nm = %s, ->id=[%d]\n",idx,var_fix[idx]->nm,var_fix[idx]->id);
    for(idx=0;idx<nbr_var_prc;idx++) (void)fprintf(stderr,"var_prc[%d]->nm = %s, ->id=[%d]\n",idx,var_prc[idx]->nm,var_prc[idx]->id);
  } /* end if */
  
  /* Open output file */
  fl_out_tmp=nco_fl_out_open(fl_out,FORCE_APPEND,FORCE_OVERWRITE,&out_id);
  if(dbg_lvl > 4) (void)fprintf(stderr,"Input, output file IDs = %d, %d\n",in_id,out_id);

  /* Copy global attributes */
  (void)nco_att_cpy(in_id,out_id,NC_GLOBAL,NC_GLOBAL,True);
  
  /* Catenate time-stamped command line to "history" global attribute */
  if(HISTORY_APPEND) (void)nco_hst_att_cat(out_id,cmd_ln);

  /* Initialize thread information */
  thr_nbr=nco_openmp_ini(thr_nbr);
  if(thr_nbr > 0 && HISTORY_APPEND) (void)nco_thr_att_cat(out_id,thr_nbr);
  
  if(dmn_rdr_nbr > 0){
    /* NB: Same logic as in ncwa, perhaps combine into single function, nco_dmn_avg_rdr_prp()? */
    /* Make list of user-specified dimension re-orders */
    /* fxm: dmn_rdr is already known, is nco_prs_rdr_lst() obsolete? */
    if(False) dmn_rdr=nco_dmn_avg_rdr_prp(dmn_rdr,dmn_rdr_lst_in,dmn_rdr_nbr);

    /* Create reversed dimension list */
    dmn_rvr_rdr=(bool *)nco_malloc(dmn_rdr_nbr*sizeof(bool));
    for(idx_rdr=0;idx_rdr<dmn_rdr_nbr;idx_rdr++){
      if(dmn_rdr_lst_in[idx_rdr][0] == '-'){
	dmn_rvr_rdr[idx_rdr]=True;
	/* Move name pointer one past negative sign (lose one byte) */
	dmn_rdr_lst_in[idx_rdr]++;
      }else{
	dmn_rvr_rdr[idx_rdr]=False;
      } /* end else */
    } /* end loop over idx_rdr */

    /* Create structured list of re-ordering dimension names and IDs */
    dmn_rdr_lst=nco_dmn_lst_mk(in_id,dmn_rdr_lst_in,dmn_rdr_nbr);

    /* Form list of re-ordering dimensions from extracted input dimensions */
    dmn_rdr=(dmn_sct **)nco_malloc(dmn_rdr_nbr*sizeof(dmn_sct *));
    /* Loop over original number of re-order dimensions */
    for(idx_rdr=0;idx_rdr<dmn_rdr_nbr;idx_rdr++){
      for(idx=0;idx<nbr_dmn_xtr;idx++){
	if(!strcmp(dmn_rdr_lst[idx_rdr].nm,dim[idx]->nm)) break;
      } /* end loop over idx_rdr */
      if(idx != nbr_dmn_xtr) dmn_rdr[dmn_rdr_nbr_utl++]=dim[idx]; else (void)fprintf(stderr,"%s: WARNING re-ordering dimension \"%s\" is not contained in any variable in extraction list\n",prg_nm,dmn_rdr_lst[idx_rdr].nm);
    } /* end loop over idx_rdr */
    dmn_rdr_nbr=dmn_rdr_nbr_utl;
    /* Collapse extra dimension structure space to prevent accidentally using it */
    dmn_rdr=(dmn_sct **)nco_realloc(dmn_rdr,dmn_rdr_nbr*sizeof(dmn_sct *));
    /* Dimension list space now refers to requested rather than utilized dimensions */
    dmn_rdr_lst=(nm_id_sct *)nco_free(dmn_rdr_lst);

    /* Make sure no re-ordering dimension is specified more than once */
    for(idx=0;idx<dmn_rdr_nbr;idx++){
      for(idx_rdr=0;idx_rdr<dmn_rdr_nbr;idx_rdr++){
	if(idx_rdr != idx){
	  if(dmn_rdr[idx]->id == dmn_rdr[idx_rdr]->id){
	    (void)fprintf(fp_stdout,"%s: ERROR %s specified more than once in reducing list\n",prg_nm,dmn_rdr[idx]->nm);
	    nco_exit(EXIT_FAILURE);
	  } /* end if */
	} /* end if */
      } /* end loop over idx_rdr */
    } /* end loop over idx */

    if(dmn_rdr_nbr > nbr_dmn_xtr){
      (void)fprintf(fp_stdout,"%s: ERROR More re-ordering dimensions than extracted dimensions\n",prg_nm);
      nco_exit(EXIT_FAILURE);
    } /* end if */

  } /* dmn_rdr_nbr <= 0 */

  /* In files with record dimension... */
  if(rec_dmn_id_in != NCO_REC_DMN_UNDEFINED){
    /* ...which, if any, output dimension structure currently holds record dimension? */
    for(dmn_out_idx=0;dmn_out_idx<nbr_dmn_out;dmn_out_idx++)
      if(dmn_out[dmn_out_idx]->is_rec_dmn) break;
    if(dmn_out_idx != nbr_dmn_out){
      dmn_out_idx_rec_in=dmn_out_idx;
      /* Initialize output record dimension to input record dimension */
      rec_dmn_nm_in=rec_dmn_nm_out=dmn_out[dmn_out_idx_rec_in]->nm;
    }else{
      dmn_out_idx_rec_in=NCO_REC_DMN_UNDEFINED;
    } /* end else */
  } /* end if file contains record dimension */
  
  /* Determine and set new dimensionality in metadata of each re-ordered variable */
  dmn_idx_out_in=(int **)nco_malloc(nbr_var_prc*sizeof(int *));
  dmn_rvr_in=(bool **)nco_malloc(nbr_var_prc*sizeof(bool *));
  for(idx=0;idx<nbr_var_prc;idx++){
    dmn_idx_out_in[idx]=(int *)nco_malloc(var_prc[idx]->nbr_dim*sizeof(int));
    dmn_rvr_in[idx]=(bool *)nco_malloc(var_prc[idx]->nbr_dim*sizeof(bool));
    /* nco_var_dmn_rdr_mtd() does re-order heavy lifting */
    rec_dmn_nm_out_crr=nco_var_dmn_rdr_mtd(var_prc[idx],var_prc_out[idx],dmn_rdr,dmn_rdr_nbr,dmn_idx_out_in[idx],dmn_rvr_rdr,dmn_rvr_in[idx]);
    /* If record dimension required by re-order of current variable... */
    if(rec_dmn_nm_out_crr){
      /* ...differs from current output record dimension... */
      if(strstr(rec_dmn_nm_out_crr,rec_dmn_nm_out)){
	/* ...and current output record dimension already differs from input record dimension... */
	if(REDEFINED_RECORD_DIMENSION){
	  /* ...then requested re-order requires multiple record dimensions... */
	  (void)fprintf(fp_stdout,"%s: ERROR Requested re-order requires multiple record dimensions\n. Record dimensions involved (original,first change request,second change request)=(%s,%s,%s)\n",prg_nm,rec_dmn_nm_in,rec_dmn_nm_out,rec_dmn_nm_out_crr);
	  nco_exit(EXIT_FAILURE);
	} /* end if REDEFINED_RECORD_DIMENSION */
	/* ...otherwise, update output record dimension name... */
	rec_dmn_nm_out=rec_dmn_nm_out_crr;
	/* ...and set new and un-set old record dimensions... */
	var_prc_out[idx]->dim[0]->is_rec_dmn=True;
	dmn_out[dmn_out_idx_rec_in]->is_rec_dmn=False;
	/* ...and set flag that record dimension has been re-defined... */
	REDEFINED_RECORD_DIMENSION=True;
      } /* endif new and old record dimensions differ */
    } /* endif current variable is record variable */
  } /* end loop over var_prc */
    
  if(REDEFINED_RECORD_DIMENSION) (void)fprintf(fp_stdout,"%s: INFO Requested re-order requires changing record dimension from %s to %s\n",prg_nm,rec_dmn_nm_in,rec_dmn_nm_out);
    
  /* Once new record dimension, if any, is known, define dimensions in output file */
  (void)nco_dmn_dfn(fl_out,out_id,dmn_out,nbr_dmn_out);

  /* Define variables in output file, copy their attributes */
  (void)nco_var_dfn(in_id,fl_out,out_id,var_out,nbr_xtr,(dmn_sct **)NULL,(int)0);

  /* Turn off default filling behavior to enhance efficiency */
  rcd=nco_set_fill(out_id,NC_NOFILL,&fll_md_old);
  
  /* Take output file out of define mode */
  (void)nco_enddef(out_id);
  
  /* Zero start vectors for all output variables */
  (void)nco_var_srt_zero(var_out,nbr_xtr);

  /* Copy variable data for non-processed variables */
  (void)nco_var_val_cpy(in_id,out_id,var_fix,nbr_var_fix);

  /* Close first input netCDF file */
  nco_close(in_id);
  
  /* Loop over input files (not currently used, fl_nbr == 1) */
  for(fl_idx=0;fl_idx<fl_nbr;fl_idx++){
    /* Parse filename */
    if(fl_idx != 0) fl_in=nco_fl_nm_prs(fl_in,fl_idx,&fl_nbr,fl_lst_in,abb_arg_nbr,fl_lst_abb,fl_pth);
    if(dbg_lvl > 0) (void)fprintf(stderr,"\nInput file %d is %s; ",fl_idx,fl_in);
    /* Make sure file is on local system and is readable or die trying */
    if(fl_idx != 0) fl_in=nco_fl_mk_lcl(fl_in,fl_pth_lcl,&FILE_RETRIEVED_FROM_REMOTE_LOCATION);
    if(dbg_lvl > 0) (void)fprintf(stderr,"local file %s:\n",fl_in);
    rcd=nco_open(fl_in,NC_NOWRITE,&in_id);
    
#ifdef _OPENMP
  /* OpenMP notes:
     firstprivate(): 
     shared(): 
     private(): */
#pragma omp parallel for default(none) private(idx) shared(dbg_lvl,dmn_idx_out_in,dmn_rvr_in,fp_stderr,fp_stdout,in_id,nbr_var_prc,out_id,prg_nm,rcd,var_prc,var_prc_out)
#endif /* not _OPENMP */
    for(idx=0;idx<nbr_var_prc;idx++){ /* Process all variables in current file */
      if(dbg_lvl > 0) rcd+=nco_var_prc_crr_prn(idx,var_prc[idx]->nm);
      if(dbg_lvl > 0) (void)fflush(fp_stderr);
      
      /* Retrieve variable from disk into memory */
      (void)nco_var_get(in_id,var_prc[idx]); /* Routine contains OpenMP critical regions */
      
      if(True){
	if((var_prc_out[idx]->val.vp=(void *)nco_malloc_flg(var_prc_out[idx]->sz*nco_typ_lng(var_prc_out[idx]->type))) == NULL){
	  (void)fprintf(fp_stdout,"%s: ERROR Unable to malloc() %ld*%lu bytes for value buffer for variable %s in main()\n",prg_nm_get(),var_prc_out[idx]->sz,(unsigned long)nco_typ_lng(var_prc_out[idx]->type),var_prc_out[idx]->nm);
	  nco_exit(EXIT_FAILURE); 
	} /* endif err */
      }else{
	/* ncpdq may, instead, one day, want to copy input data. That would be done here. */
	/* Copy input variable buffer to processed variable buffer */
	var_prc_out[idx]->val=var_prc[idx]->val;
      } /* endif */

      /* Change dimension ordering of values */
      rcd=nco_var_dmn_rdr_val(var_prc[idx],var_prc_out[idx],dmn_idx_out_in[idx],dmn_rvr_in[idx]);

      /* Free current input buffer */
      var_prc[idx]->val.vp=nco_free(var_prc[idx]->val.vp);

#ifdef _OPENMP
#pragma omp critical
#endif /* _OPENMP */
      { /* begin OpenMP critical */
	/* Copy variable to output file then free value buffer */
	if(var_prc_out[idx]->nbr_dim == 0){
	  (void)nco_put_var1(out_id,var_prc_out[idx]->id,var_prc_out[idx]->srt,var_prc_out[idx]->val.vp,var_prc_out[idx]->type);
	}else{ /* end if variable is scalar */
	  (void)nco_put_vara(out_id,var_prc_out[idx]->id,var_prc_out[idx]->srt,var_prc_out[idx]->cnt,var_prc_out[idx]->val.vp,var_prc_out[idx]->type);
	} /* end if variable is array */
      } /* end OpenMP critical */
      /* Free current output buffer */
      var_prc_out[idx]->val.vp=nco_free(var_prc_out[idx]->val.vp);
      /* Free current dimension correspondence */
      dmn_idx_out_in[idx]=nco_free(dmn_idx_out_in[idx]);
      dmn_rvr_in[idx]=nco_free(dmn_rvr_in[idx]);
      
    } /* end (OpenMP parallel for) loop over idx */
    
    if(dbg_lvl > 0) (void)fprintf(fp_stderr,"\n");
    
    /* Close input netCDF file */
    nco_close(in_id);
    
    /* Remove local copy of file */
    if(FILE_RETRIEVED_FROM_REMOTE_LOCATION && REMOVE_REMOTE_FILES_AFTER_PROCESSING) (void)nco_fl_rm(fl_in);
    
  } /* end loop over fl_idx */
  
  /* Free dimension correspondence list */
  dmn_idx_out_in=nco_free(dmn_idx_out_in);

  /* Close output file and move it from temporary to permanent location */
  (void)nco_fl_out_cls(fl_out,fl_out_tmp,out_id);
  
  nco_exit_gracefully();
  return EXIT_SUCCESS;
} /* end main() */


