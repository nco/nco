/* $Header: /data/zender/nco_20150216/nco/src/nco/ncatted.c,v 1.36 2001-11-29 15:33:14 hmb Exp $ */

/* ncatted -- netCDF attribute editor */

/* Purpose: Add, create, delete, or overwrite attributes in a netCDF file */

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

   Use C language escape sequences:
   ncatted -D 3 -h -a history,global,o,c,"String\tformatting\tfor\nDennis" in.nc ; ncks -M in.nc
   ncatted -D 3 -h -a history,global,o,c,'\a\b\f\n\r\t\v\\\?\0' in.nc ; ncks -M in.nc
   ncatted -D 3 -h -a history,global,o,c,'Characters which require protection by backslash:\nDouble quote: \"\nTwo consecutive double quotes: \"\"\nSingle quote: Beyond my shell abilities!\nBackslash: \\\nTwo consecutive backslashes: \\\\\nQuestion mark: \?\n' in.nc ; ncks -M in.nc
   ncatted -D 3 -h -a history,global,o,c,'Characters which do not require protection by backslash:\nSingle backquote: `\nDollarsign: $\nLeft brace: {\nRight brace: }\nPipe: |\nAmpersand: &\nAt sign: @\nPercent: %\n\n' in.nc ; ncks -M in.nc

   Append to existing string:
   ncatted -D 5 -O -a char_att,att_var,a,c,"and appended Sentence three." in.nc foo.nc

   Append to existing string with internal delimiter characters (commas):
   ncatted -D 5 -O -a char_att,att_var,a,c,"appended a comma, and three more commas,,," in.nc foo.nc

   Append to existing float:
   ncatted -D 5 -O -a float_att,att_var,a,f,74 in.nc foo.nc
   ncatted -D 5 -O -a float_att,att_var,a,f,74,75,76 in.nc foo.nc

   Create new float:
   ncatted -D 5 -O -a new_float_att,att_var,c,f,74 in.nc foo.nc

   Delete attribute:
   ncatted -D 5 -O -a float_att,att_var,d,,, in.nc foo.nc

   Modify existing float:
   ncatted -D 5 -O -a float_att,att_var,m,f,74 in.nc foo.nc

   Modify existing missing value attribute:
   ncatted -D 5 -O -a missing_value,mss_val,m,f,74 in.nc foo.nc
   
   Multiple attribute edits:
   ncatted -D 5 -O -a char_att,att_var,a,c,"and appended Sentence three." -a short_att,att_var,c,s,37,38,39 -a float_att,att_var,d,,, -a long_att,att_var,o,l,37 -a new_att,att_var,o,d,73,74,75 in.nc foo.nc

   Create global attribute:
   ncatted -D 5 -O -a float_att,global,c,f,74 in.nc foo.nc

   Verify results:
   ncks -C -H -v att_var foo.nc

   Test algorithm for all variables:
   Append to existing string for all variables:
   ncatted -D 5 -O -a char_att,,a,c,"and appended Sentence three." in.nc foo.nc

   Append to existing float for all variables:
   ncatted -D 5 -O -a float_att,,a,f,74 in.nc foo.nc
   ncatted -D 5 -O -a float_att,,a,f,74,75,76 in.nc foo.nc

   Create new float for all variables:
   ncatted -D 5 -O -a float_att,,c,f,74 in.nc foo.nc

   Delete attribute for all variables:
   ncatted -D 5 -O -a float_att,,d,,, in.nc foo.nc

   Modify existing float for all variables:
   ncatted -D 5 -O -a float_att,,m,f,74 in.nc foo.nc

   Verify results:
   ncks -C -h foo.nc | m

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
#include "nco_netcdf.h" /* netCDF 3.0 wrapper functions */

/* #define MAIN_PROGRAM_FILE MUST precede #include nco.h */
#define MAIN_PROGRAM_FILE
#include "nco.h" /* NCO definitions */

int 
main(int argc,char **argv)
{
  aed_sct *prs_aed_lst(int,char **);
  //void aed_prc(int,int,aed_sct);

  bool OUTPUT_TO_NEW_NETCDF_FILE=False;
  bool FORCE_APPEND=False; /* Option A */
  bool FORCE_OVERWRITE=False; /* Option O */
  bool FILE_RETRIEVED_FROM_REMOTE_LOCATION;
  bool HISTORY_APPEND=True; /* Option h */
  bool REMOVE_REMOTE_FILES_AFTER_PROCESSING=True; /* Option R */
  
  char **fl_lst_abb=NULL; /* Option n */
  char **fl_lst_in;
  char *fl_in=NULL;
  char *aed_arg[NC_MAX_ATTRS];
  char *opt_sng;
  char *fl_out;
  char *fl_pth_lcl=NULL; /* Option l */
  char *fl_pth=NULL; /* Option p */
  char *time_bfr_srt;
  char *cmd_ln;
  char CVS_Id[]="$Id: ncatted.c,v 1.36 2001-11-29 15:33:14 hmb Exp $"; 
  char CVS_Revision[]="$Revision: 1.36 $";
  
  aed_sct *aed_lst=NULL_CEWI;

  extern char *optarg;
  extern int optind;
  
  int idx;
  int idx_var;
  int nc_id;  
  int nbr_abb_arg=0;
  int nbr_var_fl;
  int nbr_aed=0; /* Option a. NB: nbr_var_aed gets incremented */
  int nbr_fl=0;
  int opt;
  int rcd=NC_NOERR; /* [rcd] Return code */

  time_t clock;

  /* Start the clock and save the command line */ 
  cmd_ln=cmd_ln_sng(argc,argv);
  clock=time((time_t *)NULL);
  time_bfr_srt=ctime(&clock); time_bfr_srt=time_bfr_srt; /* Avoid compiler warning until variable is used for something */
  
  /* Get program name and set program enum (e.g., prg=ncra) */
  prg_nm=prg_prs(argv[0],&prg);

  /* Parse command line arguments */
  opt_sng="Aa:D:hl:Op:Rr";
  while((opt = getopt(argc,argv,opt_sng)) != EOF){
    switch(opt){
    case 'A': /* Toggle FORCE_APPEND */
      FORCE_APPEND=!FORCE_APPEND;
      break;
    case 'a': /* Copy argument for later processing */
      aed_arg[nbr_aed]=optarg;
      nbr_aed++;
      break;
    case 'D': /* Debugging level. Default is 0. */
      dbg_lvl=(unsigned short)strtol(optarg,(char **)NULL,10);
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
    case 'p': /* Common file path */
      fl_pth=optarg;
      break;
    case 'R': /* Toggle removal of remotely-retrieved-files. Default is True. */
      REMOVE_REMOTE_FILES_AFTER_PROCESSING=!REMOVE_REMOTE_FILES_AFTER_PROCESSING;
      break;
    case 'r': /* Print CVS program information and copyright notice */
      (void)copyright_prn(CVS_Id,CVS_Revision);
      (void)nco_lib_vrs_prn();
      exit(EXIT_SUCCESS);
      break;
    default: /* Print proper usage */
      (void)usg_prn();
      exit(EXIT_FAILURE);
    } /* end switch */
  } /* end while loop */
  
  /* Process positional arguments and fill in filenames */
  fl_lst_in=fl_lst_mk(argv,argc,optind,&nbr_fl,&fl_out);
  if(fl_out != NULL) OUTPUT_TO_NEW_NETCDF_FILE=True; else fl_out=fl_lst_in[0];

  if(nbr_aed == 0){
    (void)fprintf(stdout,"%s: ERROR must specify an attribute to edit\n",prg_nm);
    usg_prn();
    exit(EXIT_FAILURE);
  } /* end if */ 

  /* Make a uniform list of the user-specified rename structures */
  if(nbr_aed > 0) aed_lst=prs_aed_lst(nbr_aed,aed_arg);

  /* We now have the final list of attributes to edit. */
  
  /* Parse filename */
  fl_in=fl_nm_prs(fl_in,0,&nbr_fl,fl_lst_in,nbr_abb_arg,fl_lst_abb,fl_pth);
  /* Make sure file is on local system and is readable or die trying */
  fl_in=fl_mk_lcl(fl_in,fl_pth_lcl,&FILE_RETRIEVED_FROM_REMOTE_LOCATION);

  if(OUTPUT_TO_NEW_NETCDF_FILE){

    if(!FORCE_OVERWRITE){
      int rcd;

      struct stat stat_sct;
      
      rcd=stat(fl_out,&stat_sct);

      /* If file already exists, then query the user whether to overwrite */
      if(rcd != -1){
        char usr_reply;
        
        usr_reply='z';
        while(usr_reply != 'n' && usr_reply != 'y'){
          (void)fprintf(stdout,"ncrename: overwrite %s (y/n)? ",fl_out);
	  (void)fflush(stdout);
          usr_reply=(char)fgetc(stdin);
        } /* end while */
        
        if(usr_reply == 'n'){
          exit(EXIT_SUCCESS);
        } /* end if */
      } /* end if */
    } /* end if */
    
    /* Copy input file to output file, then search through output, editing attributes along the way.
       This avoids possible XDR translation performance penalty of copying each variable with netCDF. */
    (void)fl_cp(fl_in,fl_out);

  } /* end if */

  
  /* Open file. Writing must be enabled and file should be in define mode for renaming */
  rcd=nco_open(fl_out,NC_WRITE,&nc_id);
  (void)nco_redef(nc_id);

  /* Get number of variables in file */
  (void)nco_inq(nc_id,(int *)NULL,&nbr_var_fl,(int *)NULL,(int *)NULL);

  for(idx=0;idx<nbr_aed;idx++){

    if(aed_lst[idx].var_nm != NULL){

      /* Is this a global attribute? */
      if(!strcmp(aed_lst[idx].var_nm,"global")) aed_lst[idx].id=NC_GLOBAL; else (void)nco_inq_varid(nc_id,aed_lst[idx].var_nm,&aed_lst[idx].id);

      /* Edit attribute */
      (void)aed_prc(nc_id,aed_lst[idx].id,aed_lst[idx]);

    }else{ /* var_nm == NULL */
      /* Perform operation for every variable for which it makes sense */
      
      /* Edit attribute for every variable */
      for(idx_var=0;idx_var<nbr_var_fl;idx_var++) (void)aed_prc(nc_id,idx_var,aed_lst[idx]);

    } /* end else var_nm == NULL */

  } /* end loop over idx */
  
  /* Catenate the timestamped command line to the "history" global attribute */
  if(HISTORY_APPEND) (void)hst_att_cat(nc_id,cmd_ln);
  
  /* Take file out of define mode */
  (void)nco_enddef(nc_id);
    
  /* Close the open netCDF file */
  nco_close(nc_id);
  
  /* Remove local copy of file */
  if(FILE_RETRIEVED_FROM_REMOTE_LOCATION && REMOVE_REMOTE_FILES_AFTER_PROCESSING) (void)fl_rm(fl_in);

  if(rcd != NC_NOERR) nco_err_exit(rcd,"main");
  Exit_gracefully();
  return EXIT_SUCCESS;
} /* end main() */

aed_sct *
prs_aed_lst(int nbr_aed,char **aed_arg)
/* 
   int nbr_aed: input number of attributes
   char **aed_arg: input list of user-specified dimension attributes
   aed_sct *prs_aed_lst(): output structure holding user-specified strings for attributes
 */
{
  /* Routine to set name, type, size, and value elements of a comma separated list of attribute info. 
     This routine merely evaluates syntax of input expressions but does not validate attributes or variables against those present in input netCDF file. */

  /* Options are:
     -a att_nm,var_nm,mode,att_typ,att_val (modifies attribute att_nm for the single variable var_nm)

     -a att_nm,,mode,att_typ,att_val (modifies attribute att_nm for every variable in file)
     If option -a is given with var_nm = NULL, then var_nm is expanded into every variable name in file
     Thus attribute editing operation is performed on every variable in file.

     mode,att_nm,att_typ,att_val (modifies global attribute att_nm for file)
     This option may be combined with modes -a, -c, -d, or -o to specify 
     appending to, changing, deleting, or overwriting, any existing global attribute named att_nm

     One mode must be set for each edited attribute: append (a), create (c), delete (d), modify (m), or overwrite (o).
     -a: Attribute append mode
     Append value att_val to current var_nm attribute att_nm value att_val, if any. 
     If var_nm does not have an attribute att_nm, there is not effect.

     -c: Attribute create mode
     Create variable var_nm attribute att_nm with att_val if att_nm does not yet exist. 
     If var_nm already has an attribute att_nm, there is not effect.

     -d: Attribute delete mode
     Delete current var_nm attribute att_nm.
     If var_nm does not have an attribute att_nm, there is not effect.

     -m: Attribute modify mode
     Change value of current var_nm attribute att_nm to value att_val.
     If var_nm does not have an attribute att_nm, there is not effect.

     -o: Attribute overwrite mode
     Write attribute att_nm with value att_val to variable var_nm, overwriting existing attribute att_nm, if any.
     This is the default mode.
   */

  void usg_prn(void);

  char **arg_lst;

  char *dlm_sng=",";

  aed_sct *aed_lst;

  int idx;
  int arg_nbr;

  long idx_att_val_arg=4L; /* Number of required delimiters preceding attribute values in -a argument list */

  aed_lst=(aed_sct *)nco_malloc(nbr_aed*sizeof(aed_sct));

  for(idx=0;idx<nbr_aed;idx++){

    /* Attribute edit specifications are processed as a normal text list. */
    arg_lst=lst_prs(aed_arg[idx],dlm_sng,&arg_nbr);

    /* Check syntax */
    if(
       arg_nbr < 5 || /* Need more info */
       arg_lst[0] == NULL || /*  att_nm not specified */
       arg_lst[2] == NULL || /*  mode not specified */
       (*(arg_lst[2]) != 'd' && (arg_lst[3] == NULL || (arg_lst[idx_att_val_arg] == NULL && *(arg_lst[3]) != 'c'))) || /* att_typ and att_val must be specified when mode is not delete, except that att_val = "" is valid for character type */
       False){
      (void)fprintf(stdout,"%s: ERROR in attribute edit specification %s\n",prg_nm_get(),aed_arg[idx]);
      exit(EXIT_FAILURE);
    } /* end if */

    /* Initialize structure */
    /* aed strings which are not explicitly set by the user will remain as NULLs,
       i.e., specifying the default setting will appear as if nothing at all was set.
       Hopefully, in the routines that follow, the branch followed by an aed for which
       all the default settings were specified (e.g.,"-a foo,,,,") will yield the same result
       as the branch for which all defaults were set.
     */
    aed_lst[idx].att_nm=NULL;
    aed_lst[idx].var_nm=NULL;
    aed_lst[idx].val.vp=NULL;
    aed_lst[idx].type=NC_CHAR;
    aed_lst[idx].mode=aed_overwrite;
    aed_lst[idx].sz=-1L;
    aed_lst[idx].id=-1;

    /* Fill in structure */
    aed_lst[idx].att_nm=arg_lst[0];
    aed_lst[idx].var_nm=arg_lst[1];

    /* fxm: These switches should be changed to string comparisons someday */
    /* Set mode of current aed structure */
    /* Convert single letter code to mode enum */
    /*    if(!strstr("append",arg_lst[2])){aed_lst[idx].mode=aed_append;
    }else if(!strstr("create",arg_lst[2])){aed_lst[idx].mode=aed_create;
    }else if(!strstr("delete",arg_lst[2])){aed_lst[idx].mode=aed_delete;
    }else if(!strstr("modify",arg_lst[2])){aed_lst[idx].mode=aed_modify;
    }else if(!strstr("overwrite",arg_lst[2])){aed_lst[idx].mode=aed_overwrite;} */
    switch(*(arg_lst[2])){
    case 'a':	aed_lst[idx].mode=aed_append; break;
    case 'c':	aed_lst[idx].mode=aed_create; break;
    case 'd':	aed_lst[idx].mode=aed_delete; break;
    case 'm':	aed_lst[idx].mode=aed_modify; break;
    case 'o':	aed_lst[idx].mode=aed_overwrite; break;
    default: 
      (void)fprintf(stderr,"%s: ERROR `%s' is not a supported mode\n",prg_nm_get(),arg_lst[2]);
      (void)fprintf(stderr,"%s: HINT: Valid modes are `a' = append, `c' = create,`d' = delete, `m' = modify, and `o' = overwrite",prg_nm_get());
      exit(EXIT_FAILURE);
      break;
    } /* end switch */

    /* Attribute type and value do not matter if we are deleting it */
    if(aed_lst[idx].mode != aed_delete){

      /* Set type of current aed structure */
      /* Convert single letter code to type enum */
      switch(*(arg_lst[3])){
      case 'f':	aed_lst[idx].type=NC_FLOAT; break;
      case 'd':	aed_lst[idx].type=NC_DOUBLE; break;
      case 'l':	
      case 'i':	aed_lst[idx].type=NC_INT; break;
      case 's':	aed_lst[idx].type=NC_SHORT; break;
      case 'c':	aed_lst[idx].type=NC_CHAR; break;
      case 'b':	aed_lst[idx].type=NC_BYTE; break;
      default: 
	(void)fprintf(stderr,"%s: ERROR `%s' is not a supported netCDF data type\n",prg_nm_get(),arg_lst[3]);
	(void)fprintf(stderr,"%s: HINT: Valid data types are `c' = char, `f' = float, `d' = double,`s' = short, `l' = long, `b' = byte",prg_nm_get());
	exit(EXIT_FAILURE);
	break;
      } /* end switch */
      
      /* Reassemble string list values which inadvertently contain delimiters */
      if(aed_lst[idx].type == NC_CHAR && arg_nbr > idx_att_val_arg+1){
	/* Number of elements which must be concatenated into single string value */
	long lmn_nbr;
	lmn_nbr=arg_nbr-idx_att_val_arg; 
	if(dbg_lvl >= 2) (void)fprintf(stdout,"%s: WARNING NC_CHAR (string) attribute is embedded with %li literal element delimiters (\"%s\"), reassembling...\n",prg_nm_get(),lmn_nbr-1L,dlm_sng);
	/* Rewrite list, splicing in original delimiter string */
	arg_lst[idx_att_val_arg]=sng_lst_prs(arg_lst+idx_att_val_arg,lmn_nbr,dlm_sng);
	/* Keep bookkeeping straight, just in case */
	arg_nbr=idx_att_val_arg+1L;
	lmn_nbr=1L;
      } /* endif arg_nbr > idx_att_val_arg+1 */
      
      /* Replace any C language '\X' escape codes with ASCII bytes */
      if(aed_lst[idx].type == NC_CHAR) (void)sng_ascii_trn(arg_lst[idx_att_val_arg]);

      /* Set size of current aed structure */
      if(aed_lst[idx].type == NC_CHAR){
	/* Include NUL-terminator in string length */
	aed_lst[idx].sz=(arg_lst[idx_att_val_arg] == NULL) ? 0 : strlen(arg_lst[idx_att_val_arg])+1;
      }else{
	/* Number of elements of numeric types is determined by number of delimiters */
	aed_lst[idx].sz=arg_nbr-idx_att_val_arg;
      } /* end else */
      
      /* Set value of current aed structure */
      if(aed_lst[idx].type == NC_CHAR){
	aed_lst[idx].val.cp=(unsigned char *)arg_lst[idx_att_val_arg];
      }else{
	double *val_arg_dbl=NULL_CEWI;
	
	long lmn;
	
	val_arg_dbl=(double *)nco_malloc(aed_lst[idx].sz*sizeof(double));
	aed_lst[idx].val.vp=(void *)nco_malloc(aed_lst[idx].sz*nco_typ_lng(aed_lst[idx].type));
	
	for(lmn=0L;lmn<aed_lst[idx].sz;lmn++) val_arg_dbl[lmn]=strtod(arg_lst[idx_att_val_arg+lmn],(char **)NULL); 
	
	/* Copy and typecast entire array of values, using implicit coercion rules of C */
	/* 20011001: Use explicit coercion rules to quiet C++ compiler warnings */
	switch(aed_lst[idx].type){
	case NC_FLOAT: for(lmn=0L;lmn<aed_lst[idx].sz;lmn++) {aed_lst[idx].val.fp[lmn]=(float)val_arg_dbl[lmn];} break; 
	case NC_DOUBLE: for(lmn=0L;lmn<aed_lst[idx].sz;lmn++) {aed_lst[idx].val.dp[lmn]=(double)val_arg_dbl[lmn];} break; 
	case NC_INT: for(lmn=0L;lmn<aed_lst[idx].sz;lmn++) {aed_lst[idx].val.lp[lmn]=(long)val_arg_dbl[lmn];} break; 
	case NC_SHORT: for(lmn=0L;lmn<aed_lst[idx].sz;lmn++) {aed_lst[idx].val.sp[lmn]=(short)val_arg_dbl[lmn];} break; 
	case NC_CHAR: for(lmn=0L;lmn<aed_lst[idx].sz;lmn++) {aed_lst[idx].val.cp[lmn]=(unsigned char)val_arg_dbl[lmn];} break; 
	case NC_BYTE: for(lmn=0L;lmn<aed_lst[idx].sz;lmn++) {aed_lst[idx].val.bp[lmn]=(signed char)val_arg_dbl[lmn];} break; 
	default: nco_dfl_case_nctype_err(); break;
	} /* end switch */
	
	/* Free array used to hold double values */
	if(val_arg_dbl != NULL) (void)free(val_arg_dbl);
      } /* end else */
      /* Un-typecast pointer to values after access */
      (void)cast_nctype_void(aed_lst[idx].type,&aed_lst[idx].val);
      
    } /* end if mode is not delete */
  } /* end loop over aed */
  
  if(dbg_lvl == 5){
    for(idx=0;idx<nbr_aed;idx++){
      (void)fprintf(stderr,"aed_lst[%d].att_nm = %s\n",idx,aed_lst[idx].att_nm);
      (void)fprintf(stderr,"aed_lst[%d].var_nm = %s\n",idx,aed_lst[idx].var_nm == NULL ? "NULL" : aed_lst[idx].var_nm);
      (void)fprintf(stderr,"aed_lst[%d].id = %i\n",idx,aed_lst[idx].id);
      (void)fprintf(stderr,"aed_lst[%d].sz = %li\n",idx,aed_lst[idx].sz);
      (void)fprintf(stderr,"aed_lst[%d].type = %s\n",idx,nco_typ_sng(aed_lst[idx].type));
      /*      (void)fprintf(stderr,"aed_lst[%d].val = %s\n",idx,aed_lst[idx].val);*/
      (void)fprintf(stderr,"aed_lst[%d].mode = %i\n",idx,aed_lst[idx].mode);
    } /* end loop over idx */
  } /* end debug */
  
  return aed_lst;
  
} /* end prs_aed_lst() */

