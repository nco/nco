/* $Header: /data/zender/nco_20150216/nco/src/nco/ncks.c,v 1.42 2001-10-01 23:09:51 zender Exp $ */

/* ncks -- netCDF Kitchen Sink */

/* Purpose: Extract (subsets of) variables from a netCDF file 
   Print them to screen, or copy them to a new file, or both */

/* Copyright (C) 1995--2001 Charlie Zender

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
#include <math.h> /* sin cos cos sin 3.14159 */
#include <stdio.h> /* stderr, FILE, NULL, etc. */
#include <stdlib.h> /* atof, atoi, malloc, getopt */
#include <string.h> /* strcmp. . . */
#include <sys/stat.h> /* stat() */
#include <time.h> /* machine time */
#include <unistd.h> /* all sorts of POSIX stuff */
/* #include <assert.h> */ /* assert() debugging macro */
/* #include <errno.h> */ /* errno */
/* #include <malloc.h> */ /* malloc() stuff */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions */

/* #define MAIN_PROGRAM_FILE MUST precede #include nco.h */
#define MAIN_PROGRAM_FILE
#include "nco.h"                 /* NCO definitions */
#include "nco_netcdf.h"			/* netcdf3.x wrappers */

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
  char CVS_Id[]="$Id: ncks.c,v 1.42 2001-10-01 23:09:51 zender Exp $"; 
  char CVS_Revision[]="$Revision: 1.42 $";
  
  extern char *optarg;
  
  extern int optind;
  
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
  int rec_dmn_id;
    
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
    case 'c': /* Add all coordinates to the extraction list? */
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
      exit(EXIT_SUCCESS);
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
      (void)usg_prn();
      exit(EXIT_FAILURE);
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
  /* Open the file for reading */
  in_id=nco_open(fl_in,NC_NOWRITE);
  
  /* Get the number of variables, dimensions, and global attributes in the file */
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
      
      /* Define the variable in the output file */
      if(lmt_nbr > 0) var_out_id=cpy_var_dfn_lmt(in_id,out_id,rec_dmn_id,xtr_lst[idx].nm,lmt,lmt_nbr); else var_out_id=cpy_var_dfn(in_id,out_id,rec_dmn_id,xtr_lst[idx].nm);
      /* Copy the variable's attributes */
      (void)att_cpy(in_id,out_id,xtr_lst[idx].id,var_out_id);
    } /* end loop over idx */

  /* Turn off default filling behavior to enhance efficiency */
#if ( ! defined SUN4 ) && ( ! defined SUN4SOL2 ) && ( ! defined SUNMP )
    (void)nc_set_fill(out_id,NC_NOFILL,(int *)NULL);
#endif
  
    /* Take output file out of define mode */
    (void)nco_enddef(out_id);
    
    /* Copy the variable data */
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
    if(rec_dmn_id != -1){
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
      /* Print the variable's definition */
      (void)prn_var_dfn(in_id,xtr_lst[idx].nm);
      /* Print the variable's attributes */
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

char *
type_fmt_sng(nc_type type)
/*  
   nc_type type: input netCDF type
   char *nco_typ_sng(): output sprintf() format string for type
*/
{
  switch (type) {
  case NC_FLOAT:
    return "%g";
  case NC_DOUBLE:
    return "%g";
  case NC_INT:
    return "%li"; /* NCO currently stores NC_INT in native type long, but this will be deprecated when netCDF supports a 64 bit integer type */
    /*  case NC_LONG:
	return "%li"; */
  case NC_SHORT:
    return "%hi";
  case NC_CHAR:
    return "%c";
  case NC_BYTE:
    /* return "%c"; */ /* Default */
    /* Formats useful in printing byte data as decimal notation */
    /*    return "%u";*/
    return "%hhi"; /* Takes unsigned char as arg and prints 0..255 */
  default: nco_dfl_case_nctype_err(); break;
  } /* end switch */

  /* Some C compilers, e.g., SGI cc, need a return statement at the end of non-void functions */
  return (char *)NULL;
} /* end type_fmt_sng() */

/* The other printout method must use a recursive call to step
   through the dimensions, because the number of dimensions,
   and thus of loops, is not known in advance. */
void recursive_prn
(int *min_dmn_idx, /* input var: array of minimum dimension values */
 int *max_dmn_idx, /* input var: array of maximum dimension values */
 int idx, /* input var: current dimension to process */
 int nbr_dim, /* input var: total number of dimensions in variable */
 int lmn) /* output var: index into the 1D variable array */
{
  int tmp_idx;

  /* Check for the recursion exiting condition first */
  if(idx > nbr_dim-1){
    ;
  }else{
    /* Recursively move to the next dimension */
    for(tmp_idx=min_dmn_idx[idx];
	tmp_idx<=max_dmn_idx[idx];
	tmp_idx++){
      (void)recursive_prn(min_dmn_idx,max_dmn_idx,idx,nbr_dim,lmn);
    } /* end loop over tmp_idx */
  } /* end else */
} /* end recursive_prn() */

void 
prn_att(int in_id,int var_id)
/* 
   int in_id: input netCDF input-file ID
   int var_id: input netCDF input-variable ID
 */
{
  /* Routine to print all the global attributes in a netCDF file,
     or all the attributes for a particular netCDF variable. 
     If var_id == NC_GLOBAL ( = -1) the global attributes are printed,
     otherwise the variable's attributes are printed. */

  att_sct *att=NULL_CEWI;

  char dlm_sng[3];
  char src_sng[NC_MAX_NAME];

  long att_lmn;
  long att_sz;
  
  int idx;
  int nbr_att;

  if(var_id == NC_GLOBAL){
    /* Get the number of global attributes for the file */
    (void)nco_inq(in_id,(int *)NULL,(int *)NULL,&nbr_att,(int *)NULL);
    (void)strcpy(src_sng,"Global");
  }else{
    /* Get the name and number of attributes for the variable */
    (void)nco_inq_var(in_id,var_id,src_sng,(nc_type *)NULL,(int *)NULL,(int *)NULL,&nbr_att);
  } /* end else */

  /* Allocate space for the attribute names and types */
  if(nbr_att > 0) att=(att_sct *)nco_malloc(nbr_att*sizeof(att_sct));
    
  /* Get the attributes' names, types, lengths, and values */
  for(idx=0;idx<nbr_att;idx++){

    att[idx].nm=(char *)nco_malloc(NC_MAX_NAME*sizeof(char));
    (void)nco_inq_attname(in_id,var_id,idx,att[idx].nm);
    (void)nco_inq_att(in_id,var_id,att[idx].nm,&att[idx].type,&att[idx].sz);

    /* Copy value to avoid indirection in loop over att_sz */
    att_sz=att[idx].sz;

    /* Allocate enough space to hold attribute */
    att[idx].val.vp=(void *)nco_malloc(att_sz*nco_typ_lng(att[idx].type));
    (void)nco_get_att(in_id,var_id,att[idx].nm,att[idx].val.vp,att[idx].type);
    (void)fprintf(stdout,"%s attribute %i: %s, size = %li %s, value = ",src_sng,idx,att[idx].nm,att_sz,nco_typ_sng(att[idx].type));
    
    /* Typecast pointer to values before access */
    (void)cast_void_nctype(att[idx].type,&att[idx].val);

    (void)strcpy(dlm_sng,", ");
    switch(att[idx].type){
    case NC_FLOAT:
      for(att_lmn=0;att_lmn<att_sz;att_lmn++) (void)fprintf(stdout,"%g%s",att[idx].val.fp[att_lmn],(att_lmn != att_sz-1) ? dlm_sng : "");
      break;
    case NC_DOUBLE:
      for(att_lmn=0;att_lmn<att_sz;att_lmn++) (void)fprintf(stdout,"%g%s",att[idx].val.dp[att_lmn],(att_lmn != att_sz-1) ? dlm_sng : "");
      break;
    case NC_SHORT:
      for(att_lmn=0;att_lmn<att_sz;att_lmn++) (void)fprintf(stdout,"%hi%s",att[idx].val.sp[att_lmn],(att_lmn != att_sz-1) ? dlm_sng : "");
      break;
    case NC_INT:
      for(att_lmn=0;att_lmn<att_sz;att_lmn++) (void)fprintf(stdout,"%li%s",(long)att[idx].val.lp[att_lmn],(att_lmn != att_sz-1) ? dlm_sng : "");
      break;
    case NC_CHAR:
      for(att_lmn=0;att_lmn<att_sz;att_lmn++){
        char char_foo;

	/* Assume \0 is a string terminator and do not print it */
	if((char_foo=att[idx].val.cp[att_lmn]) != '\0') (void)fprintf(stdout,"%c",char_foo);
      } /* end loop over element */
      break;
    case NC_BYTE:
      for(att_lmn=0;att_lmn<att_sz;att_lmn++) (void)fprintf(stdout,"%c",att[idx].val.bp[att_lmn]);
      break;
    default: nco_dfl_case_nctype_err(); break;
    } /* end switch */
    (void)fprintf(stdout,"\n");
    
  } /* end loop over attributes */
  (void)fprintf(stdout,"\n");
  (void)fflush(stdout);
  
  /* Free the space holding the attribute values */
  for(idx=0;idx<nbr_att;idx++){
    (void)free(att[idx].val.vp);
    (void)free(att[idx].nm);
  } /* end loop over attributes */

  /* Free the rest of the space allocated for attribute info. */
  if(nbr_att > 0) (void)free(att);

} /* end prn_att() */

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
  /* Routine to copy the variable metadata from an input netCDF file
     to an output netCDF file. This routine does not take into 
     account any user-specified limits, it just copies what it finds. */

  int *dmn_in_id;
  int *dmn_out_id;
  int idx;
  int nbr_dim;
  int var_in_id;
  int var_out_id;

  
  
  nc_type var_type;

  
  /* See if the requested variable is already in the output file. */
  var_out_id=nco_inq_varid_flg(out_id,var_nm);
  if(var_out_id != -1) return var_out_id;
  /* See if the requested variable is in the input file. */
  var_in_id=nco_inq_varid_flg(in_id,var_nm);
  if(var_in_id == -1) (void)fprintf(stdout,"%s: ERROR unable to find variable \"%s\"\n",prg_nm_get(),var_nm);
  
  
  /* Get the type of the variable and the number of dimensions. */
  (void)nco_inq_var(in_id,var_in_id,(char *)NULL,&var_type,&nbr_dim,(int *)NULL,(int *)NULL);
  
  /* Recall:
     1. The dimensions must be defined before the variable.
     2. The variable must be defined before the attributes. */

  /* Allocate space to hold the dimension IDs */
  dmn_in_id=(int *)nco_malloc(nbr_dim*sizeof(int));
  dmn_out_id=(int *)nco_malloc(nbr_dim*sizeof(int));
  
  /* Get the dimension IDs */
  (void)nco_inq_var(in_id,var_in_id,(char *)NULL,(nc_type *)NULL,(int *)NULL,dmn_in_id,(int *)NULL);
  
  /* Get the dimension sizes and names */
  for(idx=0;idx<nbr_dim;idx++){
    char dmn_nm[NC_MAX_NAME];
    long dmn_sz;
    
    (void)nco_inq_dim(in_id,dmn_in_id[idx],dmn_nm,&dmn_sz);
    
    /* See if the dimension has already been defined */
    /* ncopts=0; */
    dmn_out_id[idx]=nco_inq_dimid_flg(out_id,dmn_nm);
    /* ncopts=NC_VERBOSE | NC_FATAL; */
    
    /* If the dimension hasn't been defined, copy it */
    if(dmn_out_id[idx] == -1){
      if(dmn_in_id[idx] != rec_dmn_id){
	/* dmn_out_id[idx]=ncdimdef(out_id,dmn_nm,dmn_sz); */
	(void)nco_def_dim(out_id,dmn_nm,dmn_sz,&dmn_out_id[idx]);
      }else{
	/* dmn_out_id[idx]=ncdimdef(out_id,dmn_nm,NC_UNLIMITED); */
	(void)nco_def_dim(out_id,dmn_nm,NC_UNLIMITED,&dmn_out_id[idx]);
      } /* end else */
    } /* end if */
  } /* end loop over dim */
  
  /* Define the variable in the output file */
  (void)nco_def_var(out_id,var_nm,var_type,nbr_dim,dmn_out_id,&var_out_id);
  
  /* Free the space holding the dimension IDs */
  (void)free(dmn_in_id);
  (void)free(dmn_out_id);
  
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
  /* Routine to copy the variable metadata from an input netCDF file
     to an output netCDF file. This routine truncates the dimensions
     in the variable definition in the output file according to any
     user-specified limits. */

  int *dmn_in_id;
  int *dmn_out_id;
  int idx;
  int nbr_dim;
  int var_in_id;
  int var_out_id;

  
  
  nc_type var_type;

  /* See if requested variable is already in output file. */
  
  var_out_id=nco_inq_varid_flg(out_id,var_nm);
  if(var_out_id != -1) return var_out_id;
  /* See if requested variable is in input file. */
  var_in_id=nco_inq_varid_flg(in_id,var_nm);
  if(var_in_id == -1) (void)fprintf(stdout,"%s: ERROR unable to find variable \"%s\"\n",prg_nm_get(),var_nm);
  
  
  /* Get type of variable and number of dimensions. */
  (void)nco_inq_var(in_id,var_in_id,(char *)NULL,&var_type,&nbr_dim,(int *)NULL,(int *)NULL);
  
  /* Recall:
     1. Dimensions must be defined before variable.
     2. Variable must be defined before attributes. */
     
  /* Allocate space to hold dimension IDs */
  dmn_in_id=(int *)nco_malloc(nbr_dim*sizeof(int));
  dmn_out_id=(int *)nco_malloc(nbr_dim*sizeof(int));
  
  /* Get dimension IDs */
  (void)nco_inq_var(in_id,var_in_id,(char *)NULL,(nc_type *)NULL,(int *)NULL,dmn_in_id,(int *)NULL);
  
  /* Get dimension sizes and names */
  for(idx=0;idx<nbr_dim;idx++){
    char dmn_nm[NC_MAX_NAME];
    long dmn_sz;
    
    (void)nco_inq_dim(in_id,dmn_in_id[idx],dmn_nm,&dmn_sz);
    
    /* See if dimension has already been defined */
    
    dmn_out_id[idx]=nco_inq_dimid_flg(out_id,dmn_nm);
    
    
    /* If dimension hasn't been defined, copy it */
    if(dmn_out_id[idx] == -1){
      if(dmn_in_id[idx] != rec_dmn_id){
	int lmt_idx;

	/* Decide whether this dimension has any user-specified limits */
	for(lmt_idx=0;lmt_idx<lmt_nbr;lmt_idx++){
	  if(lmt[lmt_idx].id == dmn_in_id[idx]){
	    dmn_sz=lmt[lmt_idx].cnt;
	    break;
	  } /* end if */
	} /* end loop over lmt_idx */
	
	/* dmn_out_id[idx]=ncdimdef(out_id,dmn_nm,dmn_sz); */
	(void)nco_def_dim(out_id,dmn_nm,dmn_sz,dmn_out_id+idx );
      }else{
	/* dmn_out_id[idx]=ncdimdef(out_id,dmn_nm,NC_UNLIMITED); */
	(void)nco_def_dim(out_id,dmn_nm,NC_UNLIMITED,dmn_out_id+idx );
      } /* end else */
    } /* end if */
  } /* end loop over dim */
  
  /* Define variable in output file */
  (void)nco_def_var(out_id,var_nm,var_type,nbr_dim,dmn_out_id,&var_out_id);
  
  /* Free space holding dimension IDs */
  (void)free(dmn_in_id);
  (void)free(dmn_out_id);
  
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
  /* Routine to copy the variable data from an input netCDF file
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
  var_in_id=nco_inq_varid(in_id,var_nm);
  var_out_id=nco_inq_varid(out_id,var_nm);
  
  /* Get number of dimensions for variable. */
  (void)nco_inq_var(out_id,var_out_id,(char *)NULL,&var_type,&nbr_dmn_out,(int *)NULL,(int *)NULL);
  (void)nco_inq_var(in_id,var_in_id,(char *)NULL,&var_type,&nbr_dmn_in,(int *)NULL,(int *)NULL);
  if(nbr_dmn_out != nbr_dmn_in){
    (void)fprintf(stderr,"%s: ERROR attempt to write %d dimensional input variable %s to %d dimensional space in output file. \nHINT: When using -A (append) option, all appended variables must be the same rank in the input file as in the output file. ncwa operator is useful at ridding variables of extraneous (size = 1) dimensions. Read the manual to see how.\n",prg_nm_get(),nbr_dmn_in,var_nm,nbr_dmn_out);
    exit(EXIT_FAILURE);
  } /* endif */
  nbr_dim=nbr_dmn_out;
  
  /* Allocate space to hold the dimension IDs */
  dmn_cnt=(long *)nco_malloc(nbr_dim*sizeof(long));
  dmn_id=(int *)nco_malloc(nbr_dim*sizeof(int));
  dmn_sz=(long *)nco_malloc(nbr_dim*sizeof(long));
  dmn_srt=(long *)nco_malloc(nbr_dim*sizeof(long));
  
  /* Get the dimension IDs from the input file */
  (void)nco_inq_var(in_id,var_in_id,(char *)NULL,(nc_type *)NULL,(int *)NULL,dmn_id,(int *)NULL);
  
  /* Get the dimension sizes from the input file */
  for(idx=0;idx<nbr_dim;idx++){
  /* NB: For the record dimension, ncdiminq() returns the maximum 
     value used so far in writing data for that dimension.
     Thus if you read the dimension sizes from the output file, then
     the ncdiminq() returns dmn_sz=0 for the record dimension
     until a variable has been written with that dimension. This is
     the reason for always reading the input file for the dimension
     sizes. */
    (void)nco_inq_dim(in_id,dmn_id[idx],(char *)NULL,dmn_cnt+idx);

    /* Initialize the indicial offset and stride arrays */
    dmn_srt[idx]=0L;
    var_sz*=dmn_cnt[idx];
  } /* end loop over dim */
      
  /* Allocate enough space to hold the variable */
  void_ptr=(void *)malloc(var_sz*nco_typ_lng(var_type));
  if(void_ptr == NULL){
    (void)fprintf(stderr,"%s: ERROR unable to malloc() %ld bytes for %s\n",prg_nm_get(),var_sz*nco_typ_lng(var_type),var_nm);
    exit(EXIT_FAILURE);
  } /* end if */

  /* Get the variable */
  if(nbr_dim==0){
    nco_get_var1(in_id,var_in_id,0L,void_ptr,var_type);
    nco_put_var1(out_id,var_out_id,0L,void_ptr,var_type);
  }else{ /* end if variable is a scalar */
    nco_get_vara(in_id,var_in_id,dmn_srt,dmn_cnt,void_ptr,var_type);
    nco_put_vara(out_id,var_out_id,dmn_srt,dmn_cnt,void_ptr,var_type);
  } /* end if variable is an array */

  /* Free the space that held the dimension IDs */
  (void)free(dmn_cnt);
  (void)free(dmn_id);
  (void)free(dmn_sz);
  (void)free(dmn_srt);

  /* Free the space that held the variable */
  (void)free(void_ptr);

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

  /* Get var_id for requested variable from both files. */
  var_in_id=nco_inq_varid(in_id,var_nm);
  var_out_id=nco_inq_varid(out_id,var_nm);
  
  /* Get number of dimensions for variable. */
  (void)nco_inq_var(out_id,var_out_id,(char *)NULL,&var_type,&nbr_dmn_out,(int *)NULL,(int *)NULL);
  (void)nco_inq_var(in_id,var_in_id,(char *)NULL,&var_type,&nbr_dmn_in,(int *)NULL,(int *)NULL);
  if(nbr_dmn_out != nbr_dmn_in){
    (void)fprintf(stderr,"%s: ERROR attempt to write %d dimensional input variable %s to %d dimensional space in output file\n",prg_nm_get(),nbr_dmn_in,var_nm,nbr_dmn_out);
    exit(EXIT_FAILURE);
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
  (void)nco_inq_var(in_id,var_in_id,(char *)NULL,(nc_type *)NULL,(int *)NULL,dmn_id,(int *)NULL);
  
  /* Get dimension sizes from input file */
  for(dmn_idx=0;dmn_idx<nbr_dim;dmn_idx++){
    
    /* NB: For record dimension, ncdiminq() returns maximum 
       value used so far in writing data for that dimension.
       Thus if you read dimension sizes from output file, then
       ncdiminq() returns dmn_sz=0 for the record dimension
       until a variable has been written with that dimension. This is
       the reason for always reading the input file for dimension
       sizes. */

    /* dmn_cnt may be overwritten by user-specified limits */
    (void)nco_inq_dim(in_id,dmn_id[dmn_idx],(char *)NULL,dmn_sz+dmn_idx);

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
    exit(EXIT_FAILURE);
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
      (void)nco_inq_dim(in_id,dmn_id[dmn_idx],(char *)NULL,dmn_sz+dmn_idx);
      
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
	
	(void)nco_inq_dim(in_id,dmn_id[0],dmn_nm,(long *)NULL);
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
	    exit(EXIT_FAILURE);
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
    
    (void)free(dmn_in_srt_1);
    (void)free(dmn_in_srt_2);
    (void)free(dmn_out_srt_1);
    (void)free(dmn_out_srt_2);
    (void)free(dmn_cnt_1);
    (void)free(dmn_cnt_2);

  } /* end if WRP */

  /* Free space that held dimension IDs */
  (void)free(dmn_map);
  (void)free(dmn_srd);
  (void)free(dmn_cnt);
  (void)free(dmn_id);
  (void)free(dmn_in_srt);
  (void)free(dmn_out_srt);
  (void)free(dmn_sz);

  /* Free space that held variable */
  (void)free(void_ptr);

} /* end cpy_var_val_lmt() */

void
prn_var_dfn(int in_id,char *var_nm)
/* 
   int in_id: input netCDF input-file ID
   char *var_nm: input variable name
 */
{
/* Routine to print the variable metadata. This routine does not take into 
   account any user-specified limits, it just prints what it finds. */

  
  int *dmn_id=NULL_CEWI;
  int idx;
  int nbr_dim;
  int nbr_att;
  int rec_dmn_id;
  int var_id;
  
  nc_type var_type;
  
  dmn_sct *dim=NULL_CEWI;

  /* See if the requested variable is in the input file. */
  var_id=nco_inq_varid(in_id,var_nm);

  /* Get the number of dimensions, type, and number of attributes for the variable. */
  (void)nco_inq_var(in_id,var_id,(char *)NULL,&var_type,&nbr_dim,(int *)NULL,&nbr_att);

  /* Get the ID of the record dimension, if any */
  (void)nco_inq(in_id,(int *)NULL,(int *)NULL,(int *)NULL,&rec_dmn_id);

  /* Print the header for the variable */
  (void)fprintf(stdout,"%s: # dim. = %i, %s, # att. = %i, ID = %i\n",var_nm,nbr_dim,nco_typ_sng(var_type),nbr_att,var_id);

  if(nbr_dim > 0){
    /* Allocate space for dimension info */
    dim=(dmn_sct *)nco_malloc(nbr_dim*sizeof(dmn_sct));
    dmn_id=(int *)nco_malloc(nbr_dim*sizeof(int));
  } /* end if nbr_dim > 0 */
  
  /* Get dimension IDs */
  (void)nco_inq_var(in_id,var_id,(char *)NULL,(nc_type *)NULL,(int *)NULL,dmn_id,(int *)NULL);
  
  /* Get dimension sizes and names */
  for(idx=0;idx<nbr_dim;idx++){
    
    dim[idx].nm=(char *)nco_malloc(NC_MAX_NAME*sizeof(char));
    dim[idx].id=dmn_id[idx];
    (void)nco_inq_dim(in_id,dim[idx].id,dim[idx].nm,&dim[idx].sz);
    
    /* Is dimension a coordinate, i.e., stored as a variable? */
    
    dim[idx].cid=nco_inq_varid_flg(in_id,dim[idx].nm);
    
    
    if(dim[idx].cid != -1){
      /* Find out what type of variable the coordinate is */
      (void)nco_inq_var(in_id,dim[idx].cid,(char *)NULL,&dim[idx].type,(int *)NULL,(int *)NULL,(int *)NULL);
      (void)fprintf(stdout,"%s dimension %i: %s, size = %li %s, dim. ID = %d (CRD)",var_nm,idx,dim[idx].nm,dim[idx].sz,nco_typ_sng(dim[idx].type),dim[idx].id);
    }else{
      (void)fprintf(stdout,"%s dimension %i: %s, size = %li, dim. ID = %d",var_nm,idx,dim[idx].nm,dim[idx].sz,dim[idx].id);
    } /* end else */
    if(dim[idx].id == rec_dmn_id) (void)fprintf(stdout,"(REC)"); 
    (void)fprintf(stdout,"\n"); 
    
  } /* end loop over dim */
  
  /* Find the total size of the variable array */
  if(nbr_dim>0){
    long var_sz=1L;
    char sz_sng[100];
    char sng_foo[200];

    for(idx=0;idx<nbr_dim;idx++) var_sz*=dim[idx].sz;
    sz_sng[0]='\0';
    for(idx=0;idx<nbr_dim-1;idx++){
      (void)sprintf(sng_foo,"%li*",dim[idx].sz);
      (void)strcat(sz_sng,sng_foo);
    } /* end loop over dim */
    (void)sprintf(sng_foo,"%li*nco_typ_lng(%s)",dim[idx].sz,nco_typ_sng(var_type));
    (void)strcat(sz_sng,sng_foo);
    (void)fprintf(stdout,"%s memory size is %s = %li*%i = %li bytes\n",var_nm,sz_sng,var_sz,nco_typ_lng(var_type),var_sz*nco_typ_lng(var_type));
  }else{
    long var_sz=1L;

    (void)fprintf(stdout,"%s memory size is %li*nco_typ_lng(%s) = %li*%i = %li bytes\n",var_nm,var_sz,nco_typ_sng(var_type),var_sz,nco_typ_lng(var_type),var_sz*nco_typ_lng(var_type));
  } /* end if variable is a scalar */
  (void)fflush(stdout);
  
  /* Free the space allocated for dimension info. */
  for(idx=0;idx<nbr_dim;idx++) (void)free(dim[idx].nm);
  if(nbr_dim > 0){
    (void)free(dim);
    (void)free(dmn_id);
  } /* end if nbr_dim > 0*/

} /* end prn_var_dfn() */

void 
prn_var_val_lmt(int in_id,char *var_nm,lmt_sct *lmt,int lmt_nbr,char *dlm_sng,bool FORTRAN_STYLE,bool PRINT_DIMENSIONAL_UNITS,bool PRN_DMN_IDX_CRD_VAL)
/* 
   int in_id: input netCDF input-file ID
   char *var_nm: input variable name
   lmt_sct *lmt: input structure from lmt_evl() holding dimension limit info.
   int lmt_nbr: input number of dimensions with user-specified limits
   char *dlm_sng: input user-specified delimiter string, if any
   bool FORTRAN_STYLE: input switch to determine syntactical interpretation of dimensional indices
   bool PRINT_DIMENSIONAL_UNITS: input switch for printing units attribute, if any.
 */
{
  /* Purpose: Print variable data 
     Routine truncates dimensions of printed output variable in accord with user-specified limits
     fxm: routine does not correctly print hyperslabs which are wrapped, or which use a non-unity stride
  */

  bool SRD=False; /* Stride is non-unity */
  bool WRP=False; /* Coordinate is wrapped */

  char *type_fmt_sng(nc_type);
  char *unit_sng="";
  /* fxm: strings statically allocated with MAX_LEN_FMT_SNG chars are not safe */
  /* Length could be computed at run time but is a pain */
#define MAX_LEN_FMT_SNG 100
  char var_sng[MAX_LEN_FMT_SNG];

  
  int *dmn_id=NULL_CEWI;
  int idx;
  
  long *dmn_cnt=NULL_CEWI;
  long *dmn_map=NULL_CEWI;
  long *dmn_mod=NULL_CEWI;
  long *dmn_sbs_ram=NULL_CEWI;
  long *dmn_sbs_dsk=NULL_CEWI;
  long *dmn_srd=NULL_CEWI;
  long *dmn_srt=NULL_CEWI;
  long *hyp_mod=NULL_CEWI;
  long lmn;  
  
  dmn_sct *dim=NULL_CEWI;
  var_sct var;

  /* Copy name into var structure for aesthetics. Unfortunately,
     Solaris machines can overwrite var.nm with next nco_malloc(), so
     continue to use var_nm for output just to be safe. */
  var.nm=(char *)strdup(var_nm);

  /* See if requested variable is in input file. */
  var.id=nco_inq_varid(in_id,var_nm);

  /* Get number of dimensions and type for variable. */
  (void)nco_inq_var(in_id,var.id,(char *)NULL,&var.type,&var.nbr_dim,(int *)NULL,(int *)NULL);

  if(var.nbr_dim > 0){
    /* Allocate space for dimension info */
    dim=(dmn_sct *)nco_malloc(var.nbr_dim*sizeof(dmn_sct));
    dmn_cnt=(long *)nco_malloc(var.nbr_dim*sizeof(long));
    dmn_id=(int *)nco_malloc(var.nbr_dim*sizeof(int));
    dmn_map=(long *)nco_malloc(var.nbr_dim*sizeof(long));
    dmn_srd=(long *)nco_malloc(var.nbr_dim*sizeof(long));
    dmn_srt=(long *)nco_malloc(var.nbr_dim*sizeof(long));

    /* Allocate space for related arrays */
    dmn_mod=(long *)nco_malloc(var.nbr_dim*sizeof(long));
    dmn_sbs_ram=(long *)nco_malloc(var.nbr_dim*sizeof(long));
    dmn_sbs_dsk=(long *)nco_malloc(var.nbr_dim*sizeof(long));
    hyp_mod=(long *)nco_malloc(var.nbr_dim*sizeof(long));
  } /* end if var.nbr_dim > 0 */
  
  /* Get dimension IDs */
  (void)nco_inq_var(in_id,var.id,(char *)NULL,(nc_type *)NULL,(int *)NULL,dmn_id,(int *)NULL);
  
  /* Get dimension sizes and names */
  for(idx=0;idx<var.nbr_dim;idx++){
    int lmt_idx;

    dim[idx].nm=(char *)nco_malloc(NC_MAX_NAME*sizeof(char));
    dim[idx].id=dmn_id[idx];
    (void)nco_inq_dim(in_id,dim[idx].id,dim[idx].nm,&dim[idx].sz);
    
    /* Initialize indicial offset and stride arrays */
    dmn_cnt[idx]=dim[idx].sz;
    dmn_map[idx]=1L;
    dmn_srd[idx]=1L;
    dmn_srt[idx]=0L;

    /* Decide whether this dimension has any user-specified limits */
    for(lmt_idx=0;lmt_idx<lmt_nbr;lmt_idx++){
      if(lmt[lmt_idx].id == dim[idx].id){
	dmn_srt[idx]=lmt[lmt_idx].srt;
	dmn_cnt[idx]=lmt[lmt_idx].cnt;
	dmn_srd[idx]=lmt[lmt_idx].srd;
	if(lmt[lmt_idx].srd != 1L) SRD=True;
	if(lmt[lmt_idx].min_idx > lmt[lmt_idx].max_idx) WRP=True;
	break;
      } /* end if */
    } /* end loop over lmt_idx */

    if(WRP){
      (void)fprintf(stdout,"%s: ERROR %s does not print variable hyperslabs where one or more of the coordinates is wrapped using the -d option (i.e., where the minimum index exceeds the maximum index such as longitude hyperslabs which cross the date-line. The workaround is to hyperslab into a new file (without -H) and then to print the values from that file (with -H).\n",prg_nm_get(),prg_nm_get());
      exit(EXIT_FAILURE);
    } /* endif error */
  
    /* Is dimension a coordinate, i.e., stored as a variable? */
    dim[idx].val.vp=NULL;
     
    dim[idx].cid=nco_inq_varid_flg(in_id,dim[idx].nm);
    
    
    /* Read in coordinate dimensions */
    if(dim[idx].cid != -1){
      /* Find out what type of variable coordinate is */
      (void)nco_inq_var(in_id,dim[idx].cid,(char *)NULL,&dim[idx].type,(int *)NULL,(int *)NULL,(int *)NULL);
      
      /* Allocate enough space to hold coordinate */
      dim[idx].val.vp=(void *)nco_malloc(dmn_cnt[idx]*nco_typ_lng(dim[idx].type));
      
      /* Retrieve this coordinate */
      if(dmn_srd[idx] == 1L) (void)nco_get_vara(in_id,dim[idx].cid,dmn_srt+idx,dmn_cnt+idx,dim[idx].val.vp,dim[idx].type); else nco_get_varm(in_id,dim[idx].cid,dmn_srt+idx,dmn_cnt+idx,dmn_srd+idx,(long *)NULL,dim[idx].val.vp,dim[idx].type);

      /* Typecast pointer to values before access */
      (void)cast_void_nctype(dim[idx].type,&dim[idx].val);

    } /* end if dimension is coordinate */     
    
  } /* end loop over dim */
  
  /* Find total size of variable array */
  var.sz=1L;
  for(idx=0;idx<var.nbr_dim;idx++) var.sz*=dmn_cnt[idx];

  /* Allocate enough space to hold variable */
  var.val.vp=(void *)malloc(var.sz*nco_typ_lng(var.type));
  if(var.val.vp == NULL){
    (void)fprintf(stderr,"%s: ERROR unable to malloc() %ld bytes for %s\n",prg_nm_get(),var.sz*nco_typ_lng(var.type),var.nm);
    exit(EXIT_FAILURE);
  } /* end if */

  /* Get variable */
  if(var.nbr_dim==0){
    nco_get_var1(in_id,var.id,0L,var.val.vp,var.type); 
  }else if(!SRD){
    nco_get_vara(in_id,var.id,dmn_srt,dmn_cnt,var.val.vp,var.type);
  }else if(SRD){
    nco_get_varm(in_id,var.id,dmn_srt,dmn_cnt,dmn_srd,(long *)NULL,var.val.vp,var.type);
  } /* end else */

  /* Typecast pointer to values before access */
  (void)cast_void_nctype(var.type,&var.val);

  if(PRINT_DIMENSIONAL_UNITS){
    int status;
    long att_sz;
    nc_type att_typ;

    /* Find if this variable has an attribute named "units" */
    /* ncopts=0;
	  status=ncattinq(in_id,var.id,"units",&att_typ,&att_sz);
	  ncopts=NC_VERBOSE | NC_FATAL; */
	  (void)nco_inq_attid(in_id,var.id,"units",&status);
	  
    if(status != -1){
	  (void)nco_inq_att(in_id,var.id,"units",&att_typ,&att_sz);
	   if(att_typ == NC_CHAR){
	unit_sng=(char *)nco_malloc((att_sz+1)*nco_typ_lng(att_typ));
	(void)nco_get_att(in_id,var.id,"units",unit_sng,att_typ);
	unit_sng[(att_sz+1)*nco_typ_lng(att_typ)-1]='\0';
      } /* end if */
    } /* end if */
  } /* end if */

  if(dlm_sng != NULL){
    /* Print each element with user-supplied formatting code */

    /* Replace any C language '\X' escape codes with ASCII bytes */
    (void)sng_ascii_trn(dlm_sng);

    /* Assume the -s argument (dlm_sng) formats the entire string
       Otherwise, one could assume that field will be printed with format type_fmt_sng(var.type),
       and that user is only allowed to affect text in between fields. 
       This would be accomplished with:
       (void)sprintf(var_sng,"%s%s",type_fmt_sng(var.type),dlm_sng);*/

    for(lmn=0;lmn<var.sz;lmn++){
      switch(var.type){
      case NC_FLOAT: (void)fprintf(stdout,dlm_sng,var.val.fp[lmn]); break;
      case NC_DOUBLE: (void)fprintf(stdout,dlm_sng,var.val.dp[lmn]); break;
      case NC_SHORT: (void)fprintf(stdout,dlm_sng,var.val.sp[lmn]); break;
      case NC_INT: (void)fprintf(stdout,dlm_sng,var.val.lp[lmn]); break;
      case NC_CHAR: (void)fprintf(stdout,dlm_sng,var.val.cp[lmn]); break;
      case NC_BYTE: (void)fprintf(stdout,dlm_sng,var.val.bp[lmn]); break;
      default: nco_dfl_case_nctype_err(); break;
      } /* end switch */
    } /* end loop over element */

  } /* end if */

  if(var.nbr_dim == 0 && dlm_sng == NULL){ 
    /* Variable is a scalar, byte, or character */
    lmn=0;
    (void)sprintf(var_sng,"%%s = %s %%s\n",type_fmt_sng(var.type));
    switch(var.type){
    case NC_FLOAT: (void)fprintf(stdout,var_sng,var_nm,var.val.fp[lmn],unit_sng); break;
    case NC_DOUBLE: (void)fprintf(stdout,var_sng,var_nm,var.val.dp[lmn],unit_sng); break;
    case NC_SHORT: (void)fprintf(stdout,var_sng,var_nm,var.val.sp[lmn],unit_sng); break;
    case NC_INT: (void)fprintf(stdout,var_sng,var_nm,var.val.lp[lmn],unit_sng); break;
    case NC_CHAR:
      (void)sprintf(var_sng,"%%s='%s' %%s\n",type_fmt_sng(var.type));
      (void)fprintf(stdout,var_sng,var_nm,var.val.cp[lmn],unit_sng);
      break;
    case NC_BYTE: (void)fprintf(stdout,var_sng,var_nm,(unsigned char)var.val.bp[lmn],unit_sng); break;
    default: nco_dfl_case_nctype_err(); break;
    } /* end switch */
  } /* end if variable is a scalar, byte, or character */

  if(var.nbr_dim > 0 && dlm_sng == NULL){ 
    /* Generate nicely formatted output for multidimensional arrays */

    char arr_lft_dlm=char_CEWI;
    char arr_rgt_dlm=char_CEWI;
    char dmn_sng[MAX_LEN_FMT_SNG];

    int crd_idx_crr=int_CEWI; /* Current coordinate index */
    int dmn_idx=int_CEWI;
    int dmn_idx_prn_srt; /* Index of first dimension to explicitly print */
    int dmn_nbr_prn=int_CEWI; /* Number of dimensions deconvolved */
    int mod_idx;

    long dmn_sbs_prn=long_CEWI; /* Subscript adjusted for C-Fortran indexing convention */
    long hyp_srt=0L;
    long idx_crr; /* Current index into equivalent 1-D array */

    short ftn_idx_off=short_CEWI;

    /* Variable is an array */
   
    /* Determine modulo masks for each index */
    for(idx=0;idx<var.nbr_dim;idx++) dmn_mod[idx]=1L;
    for(idx=0;idx<var.nbr_dim-1;idx++)
      for(mod_idx=idx+1;mod_idx<var.nbr_dim;mod_idx++) 
	dmn_mod[idx]*=dmn_cnt[mod_idx];

    /* Compute offset of hyperslab buffer from origin */
    if(lmt_nbr > 0){
      for(idx=0;idx<var.nbr_dim;idx++) hyp_mod[idx]=1L;
      for(idx=0;idx<var.nbr_dim-1;idx++)
	for(mod_idx=idx+1;mod_idx<var.nbr_dim;mod_idx++) 
	  hyp_mod[idx]*=dim[mod_idx].sz;

      for(idx=0;idx<var.nbr_dim;idx++) hyp_srt+=dmn_srt[idx]*hyp_mod[idx];
    } /* end if */

    if(FORTRAN_STYLE){
      ftn_idx_off=1;
      arr_lft_dlm='(';
      arr_rgt_dlm=')';
    }else{
      ftn_idx_off=0;
      arr_lft_dlm='[';
      arr_rgt_dlm=']';
    } /* end else */
    
    /* Until 19991226, ncks printed one-dimensional character arrays here using
       if(var.type == NC_CHAR && var.nbr_dim == 1) (void)fprintf(stdout,"%s = %s\n",var_nm,var.val.cp); */

    /* Loop over each element of variable */
    for(lmn=0;lmn<var.sz;lmn++){
      
      /* Default is to print all dimension indices and any coordinates */
      dmn_idx_prn_srt=0;
      dmn_nbr_prn=var.nbr_dim;
      /* Treat character arrays as strings if possible */
      if(var.type == NC_CHAR){
	/* Do not print final dimension (C-convention) of character arrays */
	if(FORTRAN_STYLE) dmn_idx_prn_srt=1; else dmn_nbr_prn=var.nbr_dim-1;
      } /* endif */
      
      dmn_sbs_ram[var.nbr_dim-1]=lmn%dmn_cnt[var.nbr_dim-1];
      for(idx=0;idx<var.nbr_dim-1;idx++){ /* NB: loop through nbr_dim-2 only */
	dmn_sbs_ram[idx]=(long)(lmn/dmn_mod[idx]);
	dmn_sbs_ram[idx]%=dmn_cnt[idx];
      } /* end loop over dimensions */
      
      /* Convert hyperslab (RAM) subscripts to absolute (file/disk) subscripts */
      for(idx=0;idx<var.nbr_dim;idx++) dmn_sbs_dsk[idx]=dmn_sbs_ram[idx]*dmn_srd[idx]+dmn_srt[idx];
      
      /* Skip rest of loop unless element is first in string */
      if(var.type == NC_CHAR && dmn_sbs_ram[var.nbr_dim-1] != 0L) continue;
      
      /* Does user wish to print leading dimension/coordinate indices/values? */
      if(PRN_DMN_IDX_CRD_VAL){
      
	/* Loop over dimensions whose coordinates are to be printed */
	for(idx=dmn_idx_prn_srt;idx<dmn_nbr_prn;idx++){
	  
	  /* Reverse dimension ordering for Fortran convention */
	  if(FORTRAN_STYLE) dmn_idx=var.nbr_dim-1-idx; else dmn_idx=idx;
	  
	  /* Printed dimension subscript includes indexing convention (C or Fortran) */
	  dmn_sbs_prn=dmn_sbs_dsk[dmn_idx]+ftn_idx_off;
	  
	  /* Format and print dimension part of output string for non-coordinate variables */
	  if(dim[dmn_idx].cid != var.id){ /* If variable is not a coordinate... */
	    if(dim[dmn_idx].cid != -1){ /* If dimension is a coordinate... */
	      (void)sprintf(dmn_sng,"%%s%c%%li%c=%s ",arr_lft_dlm,arr_rgt_dlm,type_fmt_sng(dim[dmn_idx].type));
	      /* Account for hyperslab offset in coordinate values*/
	      crd_idx_crr=dmn_sbs_ram[dmn_idx];
	      switch(dim[dmn_idx].type){
	      case NC_FLOAT: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,dim[dmn_idx].val.fp[crd_idx_crr]); break;
	      case NC_DOUBLE: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,dim[dmn_idx].val.dp[crd_idx_crr]); break;
	      case NC_SHORT: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,dim[dmn_idx].val.sp[crd_idx_crr]); break;
	      case NC_INT: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,dim[dmn_idx].val.lp[crd_idx_crr]); break;
	      case NC_CHAR: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,dim[dmn_idx].val.cp[crd_idx_crr]); break;
	      case NC_BYTE: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,(unsigned char)dim[dmn_idx].val.bp[crd_idx_crr]); break;
	      default: nco_dfl_case_nctype_err(); break;
	      } /* end switch */
	    }else{ /* if dimension is not a coordinate... */
	      (void)sprintf(dmn_sng,"%%s%c%%li%c ",arr_lft_dlm,arr_rgt_dlm);
	      (void)fprintf(stdout,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn);
	    } /* end else */
	  } /* end if */
	} /* end loop over dimensions */
      } /* end if PRN_DMN_IDX_CRD_VAL */
      
      /* Finally, print value of current element of variable */	
      idx_crr=lmn+hyp_srt+ftn_idx_off; /* Current index into equivalent 1-D array */
      (void)sprintf(var_sng,"%%s%c%%li%c=%s %%s\n",arr_lft_dlm,arr_rgt_dlm,type_fmt_sng(var.type));
      
      if(var.type == NC_CHAR && dmn_sbs_ram[var.nbr_dim-1] == 0L){
	/* Print all characters in last dimension each time penultimate dimension subscript changes to its start value
	   Ironic that printing characters is much more tedious than numbers */
	/* Search for NUL-termination within size of last dimension */
	if(memchr((void *)(var.val.cp+lmn),'\0',dmn_cnt[var.nbr_dim-1])){
	  /* Memory region is NUL-terminated, i.e., a valid string */
	  /* Print strings inside double quotes */
	  (void)sprintf(var_sng,"%%s%c%%li--%%li%c=\"%%s\" %%s",arr_lft_dlm,arr_rgt_dlm);
	  /* var.val.cp is unsigned char * but strlen() requires const char * */
	  (void)fprintf(stdout,var_sng,var_nm,idx_crr,idx_crr+strlen((char *)var.val.cp+lmn),(char *)var.val.cp+lmn,unit_sng);
	}else{
	  /* Memory region is not NUL-terminated, print block of chars instead */
	  /* Print block of chars inside single quotes */
	  /* Re-use dmn_sng for temporary format string */
	  (void)sprintf(dmn_sng,"%%.%lis",dmn_cnt[var.nbr_dim-1]);
	  (void)sprintf(var_sng,"%%s%c%%li--%%li%c='%s' %%s",arr_lft_dlm,arr_rgt_dlm,dmn_sng);
	  (void)fprintf(stdout,var_sng,var_nm,idx_crr,idx_crr+dmn_cnt[var.nbr_dim-1]-1L,var.val.cp+lmn,unit_sng);
	} /* endif */
	if(dbg_lvl >= 6)(void)fprintf(stdout,"DEBUG: format string used for chars is dmn_sng = %s, var_sng = %s\n",dmn_sng,var_sng); 
	/* Newline separates consecutive values within given variable */
	(void)fprintf(stdout,"\n");
	(void)fflush(stdout);
	/* Skip rest of loop for this element, move to next element */
	continue;
      } /* endif */
      
      switch(var.type){
      case NC_FLOAT: (void)fprintf(stdout,var_sng,var_nm,idx_crr,var.val.fp[lmn],unit_sng); break;
      case NC_DOUBLE: (void)fprintf(stdout,var_sng,var_nm,idx_crr,var.val.dp[lmn],unit_sng); break;
      case NC_SHORT: (void)fprintf(stdout,var_sng,var_nm,idx_crr,var.val.sp[lmn],unit_sng); break;
      case NC_INT: (void)fprintf(stdout,var_sng,var_nm,idx_crr,var.val.lp[lmn],unit_sng); break;
      case NC_CHAR: (void)fprintf(stdout,var_sng,var_nm,idx_crr,var.val.cp[lmn],unit_sng); break;
      case NC_BYTE: (void)fprintf(stdout,var_sng,var_nm,idx_crr,(unsigned char)var.val.bp[lmn],unit_sng); break;
      default: nco_dfl_case_nctype_err(); break;
      } /* end switch */
    } /* end loop over element */
  } /* end if variable is an array, not a scalar */
  
  /* Additional newline between consecutive variables or final variable and prompt */
  (void)fprintf(stdout,"\n");
  (void)fflush(stdout);
  
  /* Free space allocated for dimension */
  for(idx=0;idx<var.nbr_dim;idx++){
    (void)free(dim[idx].nm);
    if(dim[idx].val.vp != NULL) (void)free(dim[idx].val.vp);
  } /* end loop over dimensions */
  if(var.nbr_dim > 0){
    (void)free(dim);
    (void)free(dmn_cnt);
    (void)free(dmn_id);
    (void)free(dmn_map);
    (void)free(dmn_mod);
    (void)free(dmn_sbs_ram);
    (void)free(dmn_sbs_dsk);
    (void)free(dmn_srd);
    (void)free(dmn_srt);
    (void)free(hyp_mod);
  } /* end if nbr_dim > 0*/

  /* Free space allocated for variable */
  (void)free(var.val.vp);
  (void)free(var.nm);
  if(strlen(unit_sng) > 0) (void)free(unit_sng);
 
} /* end prn_var_val_lmt() */

