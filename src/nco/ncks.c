/* $Header: /data/zender/nco_20150216/nco/src/nco/ncks.c,v 1.52 2002-05-06 02:17:56 zender Exp $ */

/* ncks -- netCDF Kitchen Sink */

/* Purpose: Extract (subsets of) variables from a netCDF file 
   Print them to screen, or copy them to a new file, or both */

/* Copyright (C) 1995--2002 Charlie Zender

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.
   
   As a special exception to the terms of the GPL, you are permitted 
   to link the NCO source code with the NetCDF and HDF libraries 
   and distribute the resulting executables under the terms of the GPL, 
   but in addition obeying the extra stipulations of the netCDF and 
   HDF library licenses.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

   The file LICENSE contains the GNU General Public License, version 2
   It may be viewed interactively by typing, e.g., ncks -L

   The author of this software, Charlie Zender, would like to receive
   your suggestions, improvements, bug-reports, and patches for NCO.
   Please contact the project at http://sourceforge.net/projects/nco or by writing

   Charlie Zender
   Department of Earth System Science
   University of California at Irvine
   Irvine, CA 92697-3100
 */

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
   ncks -H -C -v lon -d lon,3,1 in.nc
*/

/* Standard C headers */
#include <stdio.h> /* stderr, FILE, NULL, etc. */
#include <stdlib.h> /* atof, atoi, malloc, getopt */
#include <string.h> /* strcmp. . . */
#include <sys/stat.h> /* stat() */
#include <time.h> /* machine time */
#include <unistd.h> /* all sorts of POSIX stuff */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions */

/* #define MAIN_PROGRAM_FILE MUST precede #include nco.h */
#define MAIN_PROGRAM_FILE
#include "nco.h" /* NCO definitions */
#include "nco_netcdf.h" /* Wrappers for netCDF 3.X C-library */
#include "libnco.h" /* netCDF operator library */

int 
main(int argc,char **argv)
{
  int cpy_var_dfn(int,int,int,char *);
  int cpy_var_dfn_lmt(int,int,int,char *,lmt_sct *,int);
  void cpy_var_val(int,int,char *);
  void cpy_var_val_lmt(int,int,char *,lmt_sct *,int);
  void prn_att(int,int);
  void prn_var_dfn(int,char *);
  void prn_var_val_lmt(int,char *,lmt_sct *,int,char *,bool,bool,bool);
   
  bool ALPHABETIZE_OUTPUT=True; /* Option a */
  bool EXCLUDE_INPUT_LIST=False; /* Option c */
  bool FILE_RETRIEVED_FROM_REMOTE_LOCATION;
  bool FORCE_APPEND=False; /* Option A */
  bool FORCE_OVERWRITE=False; /* Option O */
  bool FORTRAN_STYLE=False; /* Option F */
  bool HISTORY_APPEND=True; /* Option h */
  bool OUTPUT_DATA=False; /* Option H */
  bool OUTPUT_VARIABLE_METADATA=False; /* Option m */
  bool OUTPUT_GLOBAL_METADATA=False; /* Option M */
  bool PRINT_DIMENSIONAL_UNITS=False; /* Option u */
  bool PRN_DMN_IDX_CRD_VAL=True; /* Option q Print leading dimension/coordinate indices/values */
  bool PROCESS_ALL_COORDINATES=False; /* Option c */
  bool PROCESS_ASSOCIATED_COORDINATES=True; /* Option C */
  bool REMOVE_REMOTE_FILES_AFTER_PROCESSING=True; /* Option R */

  char **var_lst_in=NULL_CEWI;
  char **fl_lst_abb=NULL; /* Option a */
  char **fl_lst_in;
  char *dlm_sng=NULL;
  char *fl_in=NULL;
  char *fl_pth_lcl=NULL; /* Option l */
  char *lmt_arg[NC_MAX_DIMS];
  char *opt_sng;
  char *fl_out;
  char *fl_out_tmp;
  char *fl_pth=NULL; /* Option p */
  char *time_bfr_srt;
  char *cmd_ln;
  char CVS_Id[]="$Id: ncks.c,v 1.52 2002-05-06 02:17:56 zender Exp $"; 
  char CVS_Revision[]="$Revision: 1.52 $";
  
  extern char *optarg;
  
  extern int optind;
  
  int fll_md_old; /* [enm] Old fill mode */
  int idx;
  int in_id;  
  int nbr_abb_arg=0;
  int nbr_dmn_fl;
  int lmt_nbr=0; /* Option d. NB: lmt_nbr gets incremented */
  int nbr_glb_att;
  int nbr_var_fl;
  int nbr_xtr=0; /* nbr_xtr won't otherwise be set for -c with no -v */
  int nbr_fl=0;
  int opt;
  int rec_dmn_id=NCO_REC_DMN_UNDEFINED;
    
  lmt_sct *lmt;

  nm_id_sct *xtr_lst=NULL; /* xtr_lst can get realloc()'d from NULL with -c option */

  time_t clock;

  /* Start the clock and save the command line */ 
  cmd_ln=cmd_ln_sng(argc,argv);
  clock=time((time_t *)NULL);
  time_bfr_srt=ctime(&clock); time_bfr_srt=time_bfr_srt; /* Avoid compiler warning until variable is used for something */
  
  /* Get program name and set program enum (e.g., prg=ncra) */
  prg_nm=prg_prs(argv[0],&prg);

  /* Parse command line arguments */
  opt_sng="aACcD:d:FHhl:MmOp:qrRs:uv:x";
  while((opt = getopt(argc,argv,opt_sng)) != EOF){
    switch(opt){
    case 'a': /* Toggle ALPHABETIZE_OUTPUT */
      ALPHABETIZE_OUTPUT=!ALPHABETIZE_OUTPUT;
      break;
    case 'A': /* Toggle FORCE_APPEND */
      FORCE_APPEND=!FORCE_APPEND;
      break;
    case 'C': /* Extraction list should include all coordinates associated with extracted variables? */
      PROCESS_ASSOCIATED_COORDINATES=False;
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
    case 'H': /* Print data to screen */
      OUTPUT_DATA=True;
      break;
    case 'h': /* Toggle appending to history global attribute */
      HISTORY_APPEND=!HISTORY_APPEND;
      break;
    case 'l': /* Local path prefix for files retrieved from remote file system */
      fl_pth_lcl=optarg;
      break;
    case 'm': /* Print variable metadata to screen */
      OUTPUT_VARIABLE_METADATA=True;
      break;
    case 'M': /* Print global metadata to screen */
      OUTPUT_GLOBAL_METADATA=True;
      break;
    case 'O': /* Toggle FORCE_OVERWRITE */
      FORCE_OVERWRITE=!FORCE_OVERWRITE;
      break;
    case 'F': /* Toggle index convention. Default is 0-based arrays (C-style). */
      FORTRAN_STYLE=!FORTRAN_STYLE;
      break;
    case 'p': /* Common file path */
      fl_pth=optarg;
      break;
    case 'q': /* Turn off printing of dimension indices and coordinate values */
      PRN_DMN_IDX_CRD_VAL=!PRN_DMN_IDX_CRD_VAL;
      break;
    case 'R': /* Toggle removal of remotely-retrieved-files. Default is True. */
      REMOVE_REMOTE_FILES_AFTER_PROCESSING=!REMOVE_REMOTE_FILES_AFTER_PROCESSING;
      break;
    case 'r': /* Print CVS program information and copyright notice */
      (void)copyright_prn(CVS_Id,CVS_Revision);
      (void)nco_lib_vrs_prn();
      nco_exit(EXIT_SUCCESS);
      break;
    case 's': /* User specified delimiter string for printed output */
      dlm_sng=optarg;
      break;
    case 'u': /* Turn on the printing of dimensional units. */
      PRINT_DIMENSIONAL_UNITS=!PRINT_DIMENSIONAL_UNITS;
      break;
    case 'v': /* Variables to extract/exclude */
      var_lst_in=lst_prs(optarg,",",&nbr_xtr);
      break;
    case 'x': /* Exclude rather than extract variables specified with -v */
      EXCLUDE_INPUT_LIST=True;
      break;
    default: /* Print proper usage */
      (void)nco_usg_prn();
      nco_exit(EXIT_FAILURE);
      break;
    } /* end switch */
  } /* end while loop */
  
  /* Process positional arguments and fill in filenames */
  fl_lst_in=fl_lst_mk(argv,argc,optind,&nbr_fl,&fl_out);
  
  /* Make uniform list of user-specified dimension limits */
  lmt=lmt_prs(lmt_nbr,lmt_arg);
  
  /* Parse filename */
  fl_in=fl_nm_prs(fl_in,0,&nbr_fl,fl_lst_in,nbr_abb_arg,fl_lst_abb,fl_pth);
  /* Make sure file is on local system and is readable or die trying */
  fl_in=fl_mk_lcl(fl_in,fl_pth_lcl,&FILE_RETRIEVED_FROM_REMOTE_LOCATION);
  /* Open file for reading */
  nco_open(fl_in,NC_NOWRITE,&in_id);
  
  /* Get number of variables, dimensions, and global attributes in file */
  (void)nco_inq(in_id,&nbr_dmn_fl,&nbr_var_fl,&nbr_glb_att,&rec_dmn_id);
  
  /* Form initial extraction list from user input */
  xtr_lst=var_lst_mk(in_id,nbr_var_fl,var_lst_in,PROCESS_ALL_COORDINATES,&nbr_xtr);

  /* Change included variables to excluded variables */
  if(EXCLUDE_INPUT_LIST) xtr_lst=var_lst_xcl(in_id,nbr_var_fl,xtr_lst,&nbr_xtr);

  /* Add all coordinate variables to extraction list */
  if(PROCESS_ALL_COORDINATES) xtr_lst=var_lst_add_crd(in_id,nbr_var_fl,nbr_dmn_fl,xtr_lst,&nbr_xtr);

  /* Make sure coordinates associated extracted variables are also on extraction list */
  if(PROCESS_ASSOCIATED_COORDINATES) xtr_lst=var_lst_ass_crd_add(in_id,xtr_lst,&nbr_xtr);

  /* Heapsort extraction list alphabetically or by variable ID */
  if(nbr_xtr > 1) xtr_lst=lst_heapsort(xtr_lst,nbr_xtr,ALPHABETIZE_OUTPUT);
    
  /* We now have final list of variables to extract. Phew. */
  
  /* Find coordinate/dimension values associated with user-specified limits */
  for(idx=0;idx<lmt_nbr;idx++) (void)lmt_evl(in_id,lmt+idx,0L,FORTRAN_STYLE);
  
  if(fl_out != NULL){
    int out_id;  

    /* Open output file */
    fl_out_tmp=fl_out_open(fl_out,FORCE_APPEND,FORCE_OVERWRITE,&out_id);
    
    /* Copy global attributes */
    (void)att_cpy(in_id,out_id,NC_GLOBAL,NC_GLOBAL);
    
    /* Catenate the timestamped command line to the "history" global attribute */
    if(HISTORY_APPEND) (void)hst_att_cat(out_id,cmd_ln);

    for(idx=0;idx<nbr_xtr;idx++){
      int var_out_id;
      
      /* Define variable in output file */
      if(lmt_nbr > 0) var_out_id=cpy_var_dfn_lmt(in_id,out_id,rec_dmn_id,xtr_lst[idx].nm,lmt,lmt_nbr); else var_out_id=cpy_var_dfn(in_id,out_id,rec_dmn_id,xtr_lst[idx].nm);
      /* Copy variable's attributes */
      (void)att_cpy(in_id,out_id,xtr_lst[idx].id,var_out_id);
    } /* end loop over idx */

    /* Turn off default filling behavior to enhance efficiency */
    nco_set_fill(out_id,NC_NOFILL,&fll_md_old);
  
    /* Take output file out of define mode */
    (void)nco_enddef(out_id);
    
    /* Copy variable data */
    for(idx=0;idx<nbr_xtr;idx++){
      if(dbg_lvl > 2) (void)fprintf(stderr,"%s, ",xtr_lst[idx].nm);
      if(dbg_lvl > 0) (void)fflush(stderr);
      if(lmt_nbr > 0) (void)cpy_var_val_lmt(in_id,out_id,xtr_lst[idx].nm,lmt,lmt_nbr); else (void)cpy_var_val(in_id,out_id,xtr_lst[idx].nm);
    } /* end loop over idx */
    
    /* Close output file and move it from temporary to permanent location */
    (void)fl_out_cls(fl_out,fl_out_tmp,out_id);

  } /* end if fl_out != NULL */
  
  if(OUTPUT_GLOBAL_METADATA){
    (void)fprintf(stdout,"Opened file %s: dimensions = %i, variables = %i, global atts. = %i, id = %i\n",fl_in,nbr_dmn_fl,nbr_var_fl,nbr_glb_att,in_id);
    if(rec_dmn_id != NCO_REC_DMN_UNDEFINED){
      char rec_dmn_nm[NC_MAX_NAME];
      long rec_dmn_sz;
      
      (void)nco_inq_dim(in_id,rec_dmn_id,rec_dmn_nm,&rec_dmn_sz);
      (void)fprintf(stdout,"Record dimension: name = %s, size = %li\n\n",rec_dmn_nm,rec_dmn_sz);
    } /* end if */
    
    /* Print all global attributes */
    (void)prn_att(in_id,NC_GLOBAL);
  } /* endif OUTPUT_GLOBAL_METADATA */
  
  if(OUTPUT_VARIABLE_METADATA){
    for(idx=0;idx<nbr_xtr;idx++){
      /* Print variable's definition */
      (void)prn_var_dfn(in_id,xtr_lst[idx].nm);
      /* Print variable's attributes */
      (void)prn_att(in_id,xtr_lst[idx].id);
    } /* end loop over idx */
  } /* end if OUTPUT_VARIABLE_METADATA */
  
  if(OUTPUT_DATA){
    for(idx=0;idx<nbr_xtr;idx++) (void)prn_var_val_lmt(in_id,xtr_lst[idx].nm,lmt,lmt_nbr,dlm_sng,FORTRAN_STYLE,PRINT_DIMENSIONAL_UNITS,PRN_DMN_IDX_CRD_VAL);
  } /* end if OUTPUT_DATA */
  
  /* Close input netCDF file */
  nco_close(in_id);
  
  /* Remove local copy of file */
  if(FILE_RETRIEVED_FROM_REMOTE_LOCATION && REMOVE_REMOTE_FILES_AFTER_PROCESSING) (void)fl_rm(fl_in);

  Exit_gracefully();
  return EXIT_SUCCESS;
} /* end main() */

/* Other printout method must use recursive call to step through dimensions, 
   because number of dimensions, and thus loops, is not known in advance. */
void recursive_prn
(int *min_dmn_idx, /* input var: array of minimum dimension values */
 int *max_dmn_idx, /* input var: array of maximum dimension values */
 int idx, /* input var: current dimension to process */
 int nbr_dim, /* input var: total number of dimensions in variable */
 int lmn) /* output var: index into the 1D variable array */
{
  int tmp_idx;

  /* Check for recursion exiting condition first */
  if(idx > nbr_dim-1){
    ;
  }else{
    /* Recursively move to next dimension */
    for(tmp_idx=min_dmn_idx[idx];
	tmp_idx<=max_dmn_idx[idx];
	tmp_idx++){
      (void)recursive_prn(min_dmn_idx,max_dmn_idx,idx,nbr_dim,lmn);
    } /* end loop over tmp_idx */
  } /* end else */
} /* end recursive_prn() */

int 
cpy_var_dfn(int in_id,int out_id,int rec_dmn_id,char *var_nm)
/* 
   int in_id: input netCDF input-file ID
   int out_id: input netCDF output-file ID
   int rec_dmn_id: input input-file record dimension ID
   char *var_nm: input variable name
   int cpy_var_dfn(): output output-file variable ID
 */
{
  /* Routine to copy variable metadata from an input netCDF file
     to an output netCDF file. This routine does not take into 
     account any user-specified limits, it just copies what it finds. */

  int *dmn_in_id;
  int *dmn_out_id;
  int idx;
  int nbr_dim;
  int rcd=NC_NOERR; /* [rcd] Return code */
  int var_in_id;
  int var_out_id;
  
  nc_type var_type;
  
  /* See if requested variable is already in output file */
  rcd=nco_inq_varid_flg(out_id,var_nm,&var_out_id);
  if(rcd == NC_NOERR) return var_out_id;
  /* See if requested variable is in input file */
  rcd=nco_inq_varid_flg(in_id,var_nm,&var_in_id);
  if(rcd != NC_NOERR) (void)fprintf(stdout,"%s: ERROR unable to find variable \"%s\"\n",prg_nm_get(),var_nm);
  
  /* Get type of variable and number of dimensions */
  (void)nco_inq_var(in_id,var_in_id,(char *)NULL,&var_type,&nbr_dim,(int *)NULL,(int *)NULL);
  
  /* Recall:
     1. Dimensions must be defined before variables
     2. Variables must be defined before attributes */

  /* Allocate space to hold dimension IDs */
  dmn_in_id=(int *)nco_malloc(nbr_dim*sizeof(int));
  dmn_out_id=(int *)nco_malloc(nbr_dim*sizeof(int));
  
  /* Get dimension IDs */
  (void)nco_inq_vardimid(in_id,var_in_id,dmn_in_id);
  
  /* Get dimension sizes and names */
  for(idx=0;idx<nbr_dim;idx++){
    char dmn_nm[NC_MAX_NAME];
    long dmn_sz;
    int rcd; /* [rcd] Return code */
    
    (void)nco_inq_dim(in_id,dmn_in_id[idx],dmn_nm,&dmn_sz);
    
    /* Has dimension been defined in output file? */
    rcd=nco_inq_dimid_flg(out_id,dmn_nm,dmn_out_id+idx);
    
    /* If dimension has not been defined, copy it */
    if(rcd != NC_NOERR){
      if(dmn_in_id[idx] != rec_dmn_id){
	/* dmn_out_id[idx]=ncdimdef(out_id,dmn_nm,dmn_sz); */
	(void)nco_def_dim(out_id,dmn_nm,dmn_sz,dmn_out_id+idx);
      }else{
	/* dmn_out_id[idx]=ncdimdef(out_id,dmn_nm,NC_UNLIMITED); */
	(void)nco_def_dim(out_id,dmn_nm,NC_UNLIMITED,dmn_out_id+idx);
      } /* end else */
    } /* end if */
  } /* end loop over dim */
  
  /* Define variable in output file */
  (void)nco_def_var(out_id,var_nm,var_type,nbr_dim,dmn_out_id,&var_out_id);
  
  /* Free the space holding dimension IDs */
  dmn_in_id=nco_free(dmn_in_id);
  dmn_out_id=nco_free(dmn_out_id);
  
  return var_out_id;
} /* end cpy_var_dfn() */

int 
cpy_var_dfn_lmt(int in_id,int out_id,int rec_dmn_id,char *var_nm,lmt_sct *lmt,int lmt_nbr)
/* 
   int in_id: input netCDF input-file ID
   int out_id: input netCDF output-file ID
   int rec_dmn_id: input input-file record dimension ID
   char *var_nm: input variable name
   lmt_sct *lmt: input structure from lmt_evl() holding dimension limit info.
   int lmt_nbr: input number of dimensions with user-specified limits
   int cpy_var_dfn_lmt(): output output-file variable ID
 */
{
  /* Routine to copy variable metadata from an input netCDF file
     to an output netCDF file. This routine truncates dimensions
     in variable definition in output file according to any
     user-specified limits. */

  int *dmn_in_id;
  int *dmn_out_id;
  int idx;
  int nbr_dim;
  int rcd=NC_NOERR; /* [rcd] Return code */
  int var_in_id;
  int var_out_id;
  
  nc_type var_type;

  /* Is requested variable already in output file? */
  rcd=nco_inq_varid_flg(out_id,var_nm,&var_out_id);
  if(rcd == NC_NOERR) return var_out_id;
  /* Is requested variable already in input file? */
  rcd=nco_inq_varid_flg(in_id,var_nm,&var_in_id);
  if(rcd != NC_NOERR) (void)fprintf(stdout,"%s: ERROR unable to find variable \"%s\"\n",prg_nm_get(),var_nm);
  
  /* Get type of variable and number of dimensions */
  (void)nco_inq_var(in_id,var_in_id,(char *)NULL,&var_type,&nbr_dim,(int *)NULL,(int *)NULL);
  
  /* Recall:
     1. Dimensions must be defined before variable
     2. Variable must be defined before attributes */
     
  /* Allocate space to hold dimension IDs */
  dmn_in_id=(int *)nco_malloc(nbr_dim*sizeof(int));
  dmn_out_id=(int *)nco_malloc(nbr_dim*sizeof(int));
  
  /* Get dimension IDs */
  (void)nco_inq_vardimid(in_id,var_in_id,dmn_in_id);
  
  /* Get dimension sizes and names */
  for(idx=0;idx<nbr_dim;idx++){
    char dmn_nm[NC_MAX_NAME];
    long dmn_sz;
    int rcd; /* [rcd] Return code */
    
    (void)nco_inq_dim(in_id,dmn_in_id[idx],dmn_nm,&dmn_sz);
    
    /* Has dimension been defined in output file? */
    rcd=nco_inq_dimid_flg(out_id,dmn_nm,dmn_out_id+idx);
    
    /* If dimension has not been defined, copy it */
    if(rcd != NC_NOERR){
      if(dmn_in_id[idx] != rec_dmn_id){
	int lmt_idx;

	/* Decide whether this dimension has any user-specified limits */
	for(lmt_idx=0;lmt_idx<lmt_nbr;lmt_idx++){
	  if(lmt[lmt_idx].id == dmn_in_id[idx]){
	    dmn_sz=lmt[lmt_idx].cnt;
	    break;
	  } /* end if */
	} /* end loop over lmt_idx */
	(void)nco_def_dim(out_id,dmn_nm,dmn_sz,dmn_out_id+idx );
      }else{
	(void)nco_def_dim(out_id,dmn_nm,NC_UNLIMITED,dmn_out_id+idx );
      } /* end else */
    } /* end if */
  } /* end loop over dim */
  
  /* Define variable in output file */
  (void)nco_def_var(out_id,var_nm,var_type,nbr_dim,dmn_out_id,&var_out_id);
  
  /* Free space holding dimension IDs */
  dmn_in_id=nco_free(dmn_in_id);
  dmn_out_id=nco_free(dmn_out_id);
  
  return var_out_id;
} /* end cpy_var_dfn_lmt() */

void 
cpy_var_val(int in_id,int out_id,char *var_nm)
/* 
   int in_id: input netCDF input-file ID
   int out_id: input netCDF output-file ID
   char *var_nm: input variable name
 */
{
  /* Routine to copy variable data from an input netCDF file
     to an output netCDF file. This routine does not take into 
     account any user-specified limits, it just copies what it finds. */

  int *dmn_id;
  int idx;
  int nbr_dim;
  int nbr_dmn_in;
  int nbr_dmn_out;
  int var_in_id;
  int var_out_id;

  long *dmn_cnt;
  long *dmn_sz;
  long *dmn_srt;
  long var_sz=1L;

  nc_type var_type;

  void *void_ptr;

  /* Get var_id for requested variable from both files */
  (void)nco_inq_varid(in_id,var_nm,&var_in_id);
  (void)nco_inq_varid(out_id,var_nm,&var_out_id);
  
  /* Get type and number of dimensions for variable */
  (void)nco_inq_var(out_id,var_out_id,(char *)NULL,&var_type,&nbr_dmn_out,(int *)NULL,(int *)NULL);
  (void)nco_inq_var(in_id,var_in_id,(char *)NULL,&var_type,&nbr_dmn_in,(int *)NULL,(int *)NULL);
  if(nbr_dmn_out != nbr_dmn_in){
    (void)fprintf(stderr,"%s: ERROR attempt to write %d dimensional input variable %s to %d dimensional space in output file. \nHINT: When using -A (append) option, all appended variables must be the same rank in the input file as in the output file. ncwa operator is useful at ridding variables of extraneous (size = 1) dimensions. Read the manual to see how.\n",prg_nm_get(),nbr_dmn_in,var_nm,nbr_dmn_out);
    nco_exit(EXIT_FAILURE);
  } /* endif */
  nbr_dim=nbr_dmn_out;
  
  /* Allocate space to hold dimension IDs */
  dmn_cnt=(long *)nco_malloc(nbr_dim*sizeof(long));
  dmn_id=(int *)nco_malloc(nbr_dim*sizeof(int));
  dmn_sz=(long *)nco_malloc(nbr_dim*sizeof(long));
  dmn_srt=(long *)nco_malloc(nbr_dim*sizeof(long));
  
  /* Get dimension IDs from input file */
  (void)nco_inq_vardimid(in_id,var_in_id,dmn_id);
  
  /* Get dimension sizes from input file */
  for(idx=0;idx<nbr_dim;idx++){
  /* NB: For the record dimension, ncdiminq() returns the maximum 
     value used so far in writing data for that dimension.
     Thus if you read dimension sizes from output file, then
     the ncdiminq() returns dmn_sz=0 for the record dimension
     until a variable has been written with that dimension. This is
     the reason for always reading input file for dimension
     sizes. */
    (void)nco_inq_dimlen(in_id,dmn_id[idx],dmn_cnt+idx);

    /* Initialize the indicial offset and stride arrays */
    dmn_srt[idx]=0L;
    var_sz*=dmn_cnt[idx];
  } /* end loop over dim */
      
  /* Allocate enough space to hold variable */
  void_ptr=(void *)malloc(var_sz*nco_typ_lng(var_type));
  if(void_ptr == NULL){
    (void)fprintf(stderr,"%s: ERROR unable to malloc() %ld bytes for %s\n",prg_nm_get(),var_sz*nco_typ_lng(var_type),var_nm);
    nco_exit(EXIT_FAILURE);
  } /* end if */

  /* Get variable */
  if(nbr_dim==0){
    nco_get_var1(in_id,var_in_id,0L,void_ptr,var_type);
    nco_put_var1(out_id,var_out_id,0L,void_ptr,var_type);
  }else{ /* end if variable is a scalar */
    nco_get_vara(in_id,var_in_id,dmn_srt,dmn_cnt,void_ptr,var_type);
    nco_put_vara(out_id,var_out_id,dmn_srt,dmn_cnt,void_ptr,var_type);
  } /* end if variable is an array */

  /* Free the space that held dimension IDs */
  dmn_cnt=nco_free(dmn_cnt);
  dmn_id=nco_free(dmn_id);
  dmn_sz=nco_free(dmn_sz);
  dmn_srt=nco_free(dmn_srt);

  /* Free the space that held variable */
  void_ptr=nco_free(void_ptr);

} /* end cpy_var_val() */

void 
cpy_var_val_lmt(int in_id,int out_id,char *var_nm,lmt_sct *lmt,int lmt_nbr)
/* 
   int in_id: input netCDF input-file ID
   int out_id: input netCDF output-file ID
   char *var_nm: input variable name
   lmt_sct *lmt: input structure from lmt_evl() holding dimension limit info.
   int lmt_nbr: input number of dimensions with user-specified limits
 */
{
  /* Routine to copy variable data from input netCDF file to output netCDF file. 
     Routine truncates dimensions in variable definition in output file according to user-specified limits. */

  bool SRD=False;
  bool WRP=False;

  int *dmn_id;

  int dmn_idx;
  int lmt_idx;
  int nbr_dim;
  int nbr_dmn_in;
  int nbr_dmn_out;
  int var_in_id;
  int var_out_id;

  /* For regular data */
  long *dmn_cnt;
  long *dmn_in_srt;
  long *dmn_map;
  long *dmn_out_srt;
  long *dmn_srd;
  long *dmn_sz;

  long var_sz=1L;

  nc_type var_type;

  void *void_ptr;

  /* Get var_id for requested variable from both files */
  nco_inq_varid(in_id,var_nm,&var_in_id);
  nco_inq_varid(out_id,var_nm,&var_out_id);
  
  /* Get type and number of dimensions for variable */
  (void)nco_inq_var(out_id,var_out_id,(char *)NULL,&var_type,&nbr_dmn_out,(int *)NULL,(int *)NULL);
  (void)nco_inq_var(in_id,var_in_id,(char *)NULL,&var_type,&nbr_dmn_in,(int *)NULL,(int *)NULL);
  if(nbr_dmn_out != nbr_dmn_in){
    (void)fprintf(stderr,"%s: ERROR attempt to write %d dimensional input variable %s to %d dimensional space in output file\n",prg_nm_get(),nbr_dmn_in,var_nm,nbr_dmn_out);
    nco_exit(EXIT_FAILURE);
  } /* endif */
  nbr_dim=nbr_dmn_out;
  
  /* Allocate space to hold dimension IDs */
  dmn_cnt=(long *)nco_malloc(nbr_dim*sizeof(long));
  dmn_id=(int *)nco_malloc(nbr_dim*sizeof(int));
  dmn_in_srt=(long *)nco_malloc(nbr_dim*sizeof(long));
  dmn_map=(long *)nco_malloc(nbr_dim*sizeof(long));
  dmn_out_srt=(long *)nco_malloc(nbr_dim*sizeof(long));
  dmn_srd=(long *)nco_malloc(nbr_dim*sizeof(long));
  dmn_sz=(long *)nco_malloc(nbr_dim*sizeof(long));
  
  /* Get dimension IDs from input file */
  (void)nco_inq_vardimid(in_id,var_in_id,dmn_id);
  
  /* Get dimension sizes from input file */
  for(dmn_idx=0;dmn_idx<nbr_dim;dmn_idx++){
    
    /* NB: For record dimension, ncdiminq() returns maximum 
       value used so far in writing data for that dimension.
       Thus if you read dimension sizes from output file, then
       ncdiminq() returns dmn_sz=0 for the record dimension
       until a variable has been written with that dimension. This is
       the reason for always reading input file for dimension
       sizes. */

    /* dmn_cnt may be overwritten by user-specified limits */
    (void)nco_inq_dimlen(in_id,dmn_id[dmn_idx],dmn_sz+dmn_idx);

    /* Set default start vectors: dmn_in_srt may be overwritten by user-specified limits */
    dmn_cnt[dmn_idx]=dmn_sz[dmn_idx];
    dmn_in_srt[dmn_idx]=0L;
    dmn_out_srt[dmn_idx]=0L;
    dmn_srd[dmn_idx]=1L;
    dmn_map[dmn_idx]=1L;

    /* Decide whether this dimension has any user-specified limits */
    for(lmt_idx=0;lmt_idx<lmt_nbr;lmt_idx++){
      if(lmt[lmt_idx].id == dmn_id[dmn_idx]){
	dmn_cnt[dmn_idx]=lmt[lmt_idx].cnt;
	dmn_in_srt[dmn_idx]=lmt[lmt_idx].srt;
	dmn_srd[dmn_idx]=lmt[lmt_idx].srd;
	if(lmt[lmt_idx].srt > lmt[lmt_idx].end) WRP=True;
	if(lmt[lmt_idx].srd != 1L) SRD=True;
	break;
      } /* end if */
    } /* end loop over lmt_idx */

    var_sz*=dmn_cnt[dmn_idx];
  } /* end loop over dim */
      
  /* Allocate enough space to hold variable */
  void_ptr=(void *)malloc(var_sz*nco_typ_lng(var_type));
  if(void_ptr == NULL){
    (void)fprintf(stderr,"%s: ERROR unable to malloc() %ld bytes for %s\n",prg_nm_get(),var_sz*nco_typ_lng(var_type),var_nm);
    nco_exit(EXIT_FAILURE);
  } /* end if */

  /* Copy variable */
  if(nbr_dim==0){ /* copy scalar */
    nco_get_var1(in_id,var_in_id,0L,void_ptr,var_type);
    nco_put_var1(out_id,var_out_id,0L,void_ptr,var_type);
  }else if(!WRP){ /* copy contiguous array */
    if(!SRD) nco_get_vara(in_id,var_in_id,dmn_in_srt,dmn_cnt,void_ptr,var_type); else nco_get_varm(in_id,var_in_id,dmn_in_srt,dmn_cnt,dmn_srd,(long *)NULL,void_ptr,var_type);
    nco_put_vara(out_id,var_out_id,dmn_out_srt,dmn_cnt,void_ptr,var_type);
  }else if(WRP){ /* copy wrapped array */
    int dmn_idx;
    int lmt_idx;
    
    /* For wrapped data */
    long *dmn_in_srt_1=NULL;
    long *dmn_in_srt_2=NULL;
    long *dmn_out_srt_1=NULL;
    long *dmn_out_srt_2=NULL;
    long *dmn_cnt_1=NULL;
    long *dmn_cnt_2=NULL;
    
    dmn_in_srt_1=(long *)nco_malloc(nbr_dim*sizeof(long));
    dmn_in_srt_2=(long *)nco_malloc(nbr_dim*sizeof(long));
    dmn_out_srt_1=(long *)nco_malloc(nbr_dim*sizeof(long));
    dmn_out_srt_2=(long *)nco_malloc(nbr_dim*sizeof(long));
    dmn_cnt_1=(long *)nco_malloc(nbr_dim*sizeof(long));
    dmn_cnt_2=(long *)nco_malloc(nbr_dim*sizeof(long));
    
    /* Variable contains a wrapped dimension, requires two reads */
    /* For each dimension in the input variable */
    for(dmn_idx=0;dmn_idx<nbr_dim;dmn_idx++){
      
      /* dmn_cnt may be overwritten by user-specified limits */
      (void)nco_inq_dimlen(in_id,dmn_id[dmn_idx],dmn_sz+dmn_idx);
      
      /* Set default vectors */
      dmn_cnt[dmn_idx]=dmn_cnt_1[dmn_idx]=dmn_cnt_2[dmn_idx]=dmn_sz[dmn_idx];
      dmn_in_srt[dmn_idx]=dmn_in_srt_1[dmn_idx]=dmn_in_srt_2[dmn_idx]=0L;
      dmn_out_srt[dmn_idx]=dmn_out_srt_1[dmn_idx]=dmn_out_srt_2[dmn_idx]=0L;
      dmn_srd[dmn_idx]=1L;
      dmn_map[dmn_idx]=1L;
      
      /* Is there a limit specified for this dimension? */
      for(lmt_idx=0;lmt_idx<lmt_nbr;lmt_idx++){
	if(lmt[lmt_idx].id == dmn_id[dmn_idx]){ /* Yes, there is a limit on this dimension */
	  dmn_cnt[dmn_idx]=dmn_cnt_1[dmn_idx]=dmn_cnt_2[dmn_idx]=lmt[lmt_idx].cnt;
	  dmn_in_srt[dmn_idx]=dmn_in_srt_1[dmn_idx]=dmn_in_srt_2[dmn_idx]=lmt[lmt_idx].srt;
	  dmn_srd[dmn_idx]=lmt[lmt_idx].srd;
	  if(lmt[lmt_idx].srd != 1L) SRD=True;
	  if(lmt[lmt_idx].srt > lmt[lmt_idx].end){ /* WRP true for this dimension */
	    WRP=True;
	    if(lmt[lmt_idx].srd != 1L){ /* SRD true for this dimension */
	      long greatest_srd_multiplier_1st_hyp_slb; /* greatest integer m such that srt+m*srd < dmn_sz */
	      long last_good_idx_1st_hyp_slb; /* C index of last valid member of 1st hyperslab (= srt+m*srd) */
	      long left_over_idx_1st_hyp_slb; /* # elements from first hyperslab to count towards current stride */
	      /* long first_good_idx_2nd_hyp_slb; *//* C index of first valid member of 2nd hyperslab, if any */

	      /* NB: Perform these operations with integer arithmatic or else! */
	      dmn_cnt_1[dmn_idx]=1L+(dmn_sz[dmn_idx]-lmt[lmt_idx].srt-1L)/lmt[lmt_idx].srd; 
	      /* Wrapped dimensions with a stride may not start at idx 0 on second read */
	      greatest_srd_multiplier_1st_hyp_slb=(dmn_sz[dmn_idx]-lmt[lmt_idx].srt-1L)/lmt[lmt_idx].srd;
	      last_good_idx_1st_hyp_slb=lmt[lmt_idx].srt+lmt[lmt_idx].srd*greatest_srd_multiplier_1st_hyp_slb;
	      left_over_idx_1st_hyp_slb=dmn_sz[dmn_idx]-last_good_idx_1st_hyp_slb-1L;
	      /*	      first_good_idx_2nd_hyp_slb=(last_good_idx_1st_hyp_slb+lmt[lmt_idx].srd)%dmn_sz[dmn_idx];*/ /* Variable is unused but instructive anyway */
	      dmn_in_srt_2[dmn_idx]=lmt[lmt_idx].srd-left_over_idx_1st_hyp_slb-1L;
	    }else{ /* !SRD */
	      dmn_in_srt_2[dmn_idx]=0L;
	      dmn_cnt_1[dmn_idx]=dmn_sz[dmn_idx]-lmt[lmt_idx].srt;
	    } /* end else */
	    dmn_cnt_2[dmn_idx]=dmn_cnt[dmn_idx]-dmn_cnt_1[dmn_idx];
	    dmn_out_srt_2[dmn_idx]=dmn_cnt_1[dmn_idx];
	  } /* end if WRP */
	  break; /* Move on to next dimension in variable */
	} /* end if */
      } /* end loop over lmt */
    } /* end loop over dim */
    
    if(dbg_lvl >= 5){
      (void)fprintf(stderr,"\nvar = %s\n",var_nm);
      (void)fprintf(stderr,"dim\tcnt\tsrtin1\tcnt1\tsrtout1\tsrtin2\tcnt2\tsrtout2\n");
      for(dmn_idx=0;dmn_idx<nbr_dim;dmn_idx++) (void)fprintf(stderr,"%d\t%ld\t%ld\t%ld\t%ld\t%ld\t%ld\t%ld\t\n",dmn_idx,dmn_cnt[dmn_idx],dmn_in_srt_1[dmn_idx],dmn_cnt_1[dmn_idx],dmn_out_srt_1[dmn_idx],dmn_in_srt_2[dmn_idx],dmn_cnt_2[dmn_idx],dmn_out_srt_2[dmn_idx]);
      (void)fflush(stderr);
    } /* end if dbg */

    if(False){
      /* If this is a coordinate variable, perform a monotonicity check */
      bool CRD=False;
      bool MNT=False;

      double val_dbl;
      double wrp_spn;
      double wrp_max;
      double wrp_min;

      long idx;

      if(nbr_dim == 1){
	char dmn_nm[NC_MAX_NAME];
	
	(void)nco_inq_dimname(in_id,dmn_id[0],dmn_nm);
	if(!strcmp(dmn_nm,var_nm)) CRD=True; else CRD=False;
      } /* end if */      
      
      if(CRD && MNT){ /* If this is a wrapped coordinate then apply monotonicity filter if requested */
	(void)nco_get_vara(in_id,var_in_id,dmn_in_srt_1,dmn_cnt_1,void_ptr,var_type);
	/* Convert coordinate to double */
	for(idx=0;idx<var_sz;idx++){
	  switch(var_type){
	  case NC_FLOAT: /* val_dbl=void_ptr.fp[idx]; */break; 
	  case NC_DOUBLE:
	  case NC_INT:
	  case NC_SHORT:
	  case NC_CHAR:
	  case NC_BYTE:
	  default:
	    (void)fprintf(stdout,"%s: ERROR Unknown nc_type %d in cpy_var_val_lmt()\n",prg_nm_get(),var_type);
	    nco_exit(EXIT_FAILURE);
	    break;
	  } /* end switch */
	  
	  /* Ensure val_dbl is between specified bounds */
	  wrp_spn=wrp_max-wrp_min;
	  if(val_dbl < wrp_min) val_dbl+=wrp_spn;
	  if(val_dbl > wrp_max) val_dbl-=wrp_spn;
	} /* end loop over idx */
      } /* endif CRD && MNT */
    } /* endif False */
    
    if(!SRD){
      (void)nco_get_vara(in_id,var_in_id,dmn_in_srt_1,dmn_cnt_1,void_ptr,var_type);
      (void)nco_put_vara(out_id,var_out_id,dmn_out_srt_1,dmn_cnt_1,void_ptr,var_type);
      (void)nco_get_vara(in_id,var_in_id,dmn_in_srt_2,dmn_cnt_2,void_ptr,var_type);
      (void)nco_put_vara(out_id,var_out_id,dmn_out_srt_2,dmn_cnt_2,void_ptr,var_type);
    }else{
      (void)nco_get_varm(in_id,var_in_id,dmn_in_srt_1,dmn_cnt_1,dmn_srd,(long *)NULL,void_ptr,var_type);
      (void)nco_put_vara(out_id,var_out_id,dmn_out_srt_1,dmn_cnt_1,void_ptr,var_type);
      (void)nco_get_varm(in_id,var_in_id,dmn_in_srt_2,dmn_cnt_2,dmn_srd,(long *)NULL,void_ptr,var_type);
      (void)nco_put_vara(out_id,var_out_id,dmn_out_srt_2,dmn_cnt_2,void_ptr,var_type);
    } /* end else */
    
    dmn_in_srt_1=nco_free(dmn_in_srt_1);
    dmn_in_srt_2=nco_free(dmn_in_srt_2);
    dmn_out_srt_1=nco_free(dmn_out_srt_1);
    dmn_out_srt_2=nco_free(dmn_out_srt_2);
    dmn_cnt_1=nco_free(dmn_cnt_1);
    dmn_cnt_2=nco_free(dmn_cnt_2);

  } /* end if WRP */

  /* Free space that held dimension IDs */
  dmn_map=nco_free(dmn_map);
  dmn_srd=nco_free(dmn_srd);
  dmn_cnt=nco_free(dmn_cnt);
  dmn_id=nco_free(dmn_id);
  dmn_in_srt=nco_free(dmn_in_srt);
  dmn_out_srt=nco_free(dmn_out_srt);
  dmn_sz=nco_free(dmn_sz);

  /* Free space that held variable */
  void_ptr=nco_free(void_ptr);

} /* end cpy_var_val_lmt() */
