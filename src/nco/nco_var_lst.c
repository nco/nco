/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_var_lst.c,v 1.16 2003-08-14 14:07:00 hmb Exp $ */

/* Purpose: Variable list utilities */

/* Copyright (C) 1995--2003 Charlie Zender
   This software is distributed under the terms of the GNU General Public License
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

#include "nco_var_lst.h" /* Variable list utilities */

nm_id_sct * /* O [sct] Variable extraction list */
nco_var_lst_mk /* [fnc] Create variable extraction list */
(const int nc_id, /* I [enm] netCDF file ID */
 const int nbr_var, /* I [nbr] Number of variables in input file */
 CST_X_PTR_CST_PTR_CST_Y(char,var_lst_in), /* I [sng] User-specified list of variable names */
 const bool PROCESS_ALL_COORDINATES, /* I [flg] Process all coordinates */
 int * const nbr_xtr) /* I/O [nbr] Number of variables in current extraction list */
{
  /* Purpose: Create variable extraction list */
  bool err_flg=False;
  int rcd=NC_NOERR; /* [rcd] Return code */
  int idx;

  nm_id_sct *xtr_lst=NULL; /* xtr_lst may bealloc()'d from NULL with -c option */

  if(*nbr_xtr > 0){
    /* If user named variables with -v option then check validity of user's list and find IDs */
    xtr_lst=(nm_id_sct *)nco_malloc(*nbr_xtr*sizeof(nm_id_sct));
     
    for(idx=0;idx<*nbr_xtr;idx++){
      xtr_lst[idx].nm=(char *)strdup(var_lst_in[idx]);
      rcd=nco_inq_varid_flg(nc_id,xtr_lst[idx].nm,&xtr_lst[idx].id);
      if(rcd != NC_NOERR){
	(void)fprintf(stdout,"%s: ERROR nco_var_lst_mk() reports user-specified variable \"%s\" is not in input file\n",prg_nm_get(),xtr_lst[idx].nm);
	err_flg=True;
      } /* endif */
    } /* end loop over idx */
    
    if(err_flg) nco_exit(EXIT_FAILURE);
  }else if(!PROCESS_ALL_COORDINATES){
    /* If the user did not specify variables with the -v option,
       and the user did not request automatic processing of all coords,
       then extract all variables in file. In this case
       we can assume variable IDs range from 0..nbr_var-1. */
    char var_nm[NC_MAX_NAME];
    
    *nbr_xtr=nbr_var;
    xtr_lst=(nm_id_sct *)nco_malloc(*nbr_xtr*sizeof(nm_id_sct));
    for(idx=0;idx<nbr_var;idx++){
      /* Get name of each variable. */
      (void)nco_inq_varname(nc_id,idx,var_nm);
      xtr_lst[idx].nm=(char *)strdup(var_nm);
      xtr_lst[idx].id=idx;
    } /* end loop over idx */
  } /* end else */

  return xtr_lst;

} /* end nco_var_lst_mk() */



nm_id_sct * /* O [sct] Variable extraction list */
nco_var_lst_mk_meta /* [fnc] Create variable extraction list using regular expressions*/
(const int nc_id, /* I [enm] netCDF file ID */
 const int nbr_var, /* I [nbr] Number of variables in input file */
 CST_X_PTR_CST_PTR_CST_Y(char,var_lst_in), /* I [sng] User-specified list of variable names */
 const bool PROCESS_ALL_COORDINATES, /* I [flg] Process all coordinates */
 int * const nbr_xtr) /* I/O [nbr] Number of variables in current extraction list */
{
  /* Purpose: Create variable extraction list with or without regular expressions*/

  int idx;
  int jdx;
  int rcd=NC_NOERR; /* [rcd] Return code */
  int sng_lng;
  int srch_sng_lng=0; /* size of srch sng */
  int nbr_tmp=0;
  int nbr_match;

  char *srch_sng=NULL;  /* all var names concatenated WITH \n\0 as a separator */
  char *var_sng;

  nm_id_sct *xtr_lst=NULL; /* xtr_lst may bealloc()'d from NULL with -c option */
  nm_id_sct *in_lst=NULL; 
  bool *in_bool=NULL;
  


  /* create list of all vars in input file */

  char var_nm[NC_MAX_NAME];
    
  in_lst=(nm_id_sct *)nco_malloc(nbr_var*sizeof(nm_id_sct));
    for(idx=0;idx<nbr_var;idx++){
      /* Get name of each variable. */
      (void)nco_inq_varname(nc_id,idx,var_nm);
      in_lst[idx].nm=(char *)strdup(var_nm);
      in_lst[idx].id=idx;
    } /* end loop over idx */

    /* return all variables if .. */
    if( *nbr_xtr==0 && !PROCESS_ALL_COORDINATES ) {
      *nbr_xtr=nbr_var;
      return in_lst;
    }      
      
  /* intialize and alloc  bool array to all False */
  in_bool=(bool *)nco_calloc(nbr_var,sizeof(bool));
  
  
  
  /* create search string from list */
  for(idx=0;idx<nbr_var;idx++){
    sng_lng=strlen(in_lst[idx].nm);
    srch_sng=(char*)nco_realloc(srch_sng,(srch_sng_lng+sng_lng+2)*sizeof(char));
    strncpy(srch_sng+srch_sng_lng,in_lst[idx].nm,sng_lng);
     srch_sng_lng=srch_sng_lng+sng_lng+2;
    *(srch_sng+srch_sng_lng-2)='\n';
    *(srch_sng+srch_sng_lng-1)='\0';
    }    /* end loop over idx */
  

  /* loop through var_lst_in */

  for(idx=0 ; idx<*nbr_xtr ; idx++) {
  
     var_sng=var_lst_in[idx];
     /* if var_sng is  a regular expressiom */
     if ( strpbrk(var_sng,".*^$\[]()<>+?|")  ) {
       nbr_match=nco_var_meta_search(nbr_var,srch_sng,var_sng,in_bool);
       if( nbr_match==0)  
        	(void)fprintf(stdout,"%s: WARNING: regular expression \"%s\" doesn't match any variables\n",prg_nm_get(),var_sng); 
       continue;
     }   

    
    /* normal variable -- search through var array */
     for(jdx=0; jdx<nbr_var; jdx++)
       if( !strcmp(var_sng,in_lst[jdx].nm)) break;
    
       /* no match -- die gently */
     if(jdx==nbr_var) {
	(void)fprintf(stdout,"%s: ERROR nco_var_lst_mk() reports user-specified variable \"%s\" is not in input file\n",prg_nm_get(),var_sng); 
        nco_exit(EXIT_FAILURE);
      }else{
      in_bool[jdx]=True;
     }
            
  }

  
  /* create final list of vars using var list and bool array */

  /* malloc xtr_lst to maximium size(nbr_var )  */
  xtr_lst=(nm_id_sct *)nco_malloc(nbr_var*sizeof(nm_id_sct));
  nbr_tmp=0;

  for(idx=0 ; idx<nbr_var;idx++) {

    /* copy var to output array */
    if(in_bool[idx]) {
      xtr_lst[nbr_tmp].nm=strdup(in_lst[idx].nm);
      xtr_lst[nbr_tmp].id =in_lst[idx].id;
      nbr_tmp++; 
    } 
    (void)nco_free(in_lst[idx].nm);
         
  }
     /*realloc to actual size */  
  xtr_lst=(nm_id_sct *)nco_realloc(xtr_lst,nbr_tmp*sizeof(nm_id_sct));

  (void)nco_free(in_lst);
  (void)nco_free(in_bool);
  (void)nco_free(srch_sng);

  *nbr_xtr=nbr_tmp;    
  return xtr_lst;

} /* end nco_var_lst_mk_meta() */


int /* O number of matches found */
nco_var_meta_search  /* search for pattern matches in the var string list */
(int nbr_var,        /* I number of vars in srch_sng and size of in_bool */
char *srch_sng,     /* I concatentaed list of vars */
char *rexp,          /* I regular expression pattern */
bool *in_bool)       /* O matched vars holder */
{
  /* if regular expression doesn't compile then prog crashes out */
  int idx;
  int err_no;
  int cflags;
  int eflags;
  int nbr_mtch=0;
  char *cp;
  
  size_t no_sub;
  
  regmatch_t *result;
  regex_t *r;

  r = (regex_t *) malloc(sizeof(regex_t));

  /* choose the RE_SYNTAX_POSIX_EXTENDED regular expression type */
  cflags=(REG_EXTENDED | REG_NEWLINE);
  /* set execution flags */
  eflags=0;

  /* compile regular expression */
  
  if((err_no=regcomp(r,rexp ,cflags))!=0) /* Compile the regex */
    {
      char *err_sng;  
    /* POSIX regcomp return error codes. */

      switch(err_no) {
      case REG_BADPAT:      err_sng="Invalid pattern."; break;  
      case REG_ECOLLATE:    err_sng="Not implemented."; break;
      case REG_ECTYPE:	    err_sng="Invalid character class name." ;break;
      case REG_EESCAPE:	    err_sng="Trailing backslash."; break;
      case REG_ESUBREG:	    err_sng="Invalid back reference."; break ;
      case REG_EBRACK:	    err_sng="Unmatched left bracket."; break ;
      case REG_EPAREN:	    err_sng=" Parenthesis imbalance."; break ;
      case REG_EBRACE:	    err_sng=" Unmatched \{.  "; break ;
      case REG_BADBR:	    err_sng=" Invalid contents of { }."; break ;
      case REG_ERANGE:	    err_sng=" Invalid range end."; break ;
      case REG_ESPACE:	    err_sng=" Ran out of memory."; break ;
      case REG_BADRPT:	    err_sng=" No preceding re for repetition op"; break ;
      default:              err_sng="Invalid pattern."; break;  
      }


 (void)fprintf(stdout,"%s: ERROR nco_var_meta_search() error in regular expression \"%s\" %s \n",prg_nm_get(),rexp, err_sng); 
    nco_exit(EXIT_FAILURE);
    }

   no_sub = r->re_nsub+1; /* How many matches are there in a line? */


  /* search string */
   result = (regmatch_t *) malloc(sizeof(regmatch_t) * no_sub);

   cp=srch_sng;
   /* search each of the var strings for matches */
   for(idx=0; idx<nbr_var ;idx++){  
     if( !regexec(r, cp, no_sub, result, eflags))
       { in_bool[idx]=True;
         nbr_mtch++;
       }
      cp+=strlen(cp)+1; 
   }

  regfree(r); /* Free the regular expression data structure */
  (void *)nco_free(r);
  (void *)nco_free(result);

   return nbr_mtch;
   
}

nm_id_sct * /* O [sct] Extraction list */
nco_var_lst_xcl /* [fnc] Convert exclusion list to extraction list */
(const int nc_id, /* I netCDF file ID */
 const int nbr_var, /* I [nbr] Number of variables in input file */
 nm_id_sct *xtr_lst, /* I/O [sct] Current exclusion list (destroyed) */
 int * const nbr_xtr) /* I/O [nbr] Number of variables in exclusion/extraction list */
{
  /* Purpose: Convert exclusion list to extraction list
     User wants to extract all variables except those currently in list
     Since it is hard to edit existing list, copy existing extraction list into 
     exclusion list, then construct new extraction list from scratch. */

  char var_nm[NC_MAX_NAME];

  int idx;
  int lst_idx;
  int nbr_xcl;

  nm_id_sct *xcl_lst;
  
  /* Turn the extract list into the exclude list and reallocate the extract list  */
  nbr_xcl=*nbr_xtr;
  *nbr_xtr=0;
  xcl_lst=(nm_id_sct *)nco_malloc(nbr_xcl*sizeof(nm_id_sct));
  (void)memcpy((void *)xcl_lst,(void *)xtr_lst,nbr_xcl*sizeof(nm_id_sct));
  xtr_lst=(nm_id_sct *)nco_realloc((void *)xtr_lst,(nbr_var-nbr_xcl)*sizeof(nm_id_sct));
  
  for(idx=0;idx<nbr_var;idx++){
    /* Get name and ID of variable */
    (void)nco_inq_varname(nc_id,idx,var_nm);
    for(lst_idx=0;lst_idx<nbr_xcl;lst_idx++){
      if(idx == xcl_lst[lst_idx].id) break;
    } /* end loop over lst_idx */
    /* If variable was not found in the exclusion list then 
       add it to the new list. */
    if(lst_idx == nbr_xcl){
      xtr_lst[*nbr_xtr].nm=(char *)strdup(var_nm);
      xtr_lst[*nbr_xtr].id=idx;
      ++*nbr_xtr;
    } /* end if */
  } /* end loop over idx */
  
  /* Free memory for names in exclude list before losing pointers to names */
  /* NB: cannot free memory if list points to names in argv[] */
  /* for(idx=0;idx<nbr_xcl;idx++) xcl_lst[idx].nm=(char *)nco_free(xcl_lst[idx].nm);*/
  xcl_lst=(nm_id_sct *)nco_free(xcl_lst);
  
  return xtr_lst;
} /* end nco_var_lst_xcl() */

nm_id_sct * /* O [sct] Extraction list */
nco_var_lst_add_crd /* [fnc] Add all coordinates to extraction list */
(const int nc_id, /* I [id] netCDF file ID */
 const int nbr_var, /* I [nbr] Number of variables in input file */
 const int nbr_dim, /* I [nbr] Number of dimensions in input file */
 nm_id_sct *xtr_lst, /* I/O [sct] Current extraction list (destroyed) */
 int * const nbr_xtr) /* I/O [nbr] Number of variables in current extraction list */
{
  /* Purpose: Add all coordinates to extraction list
     Find all coordinates (dimensions which are also variables) and
     add them to the list if they are not already there. */
  
  char crd_nm[NC_MAX_NAME];

  int crd_id;
  int idx;
  int rcd=NC_NOERR; /* [rcd] Return code */

  for(idx=0;idx<nbr_dim;idx++){
    (void)nco_inq_dimname(nc_id,idx,crd_nm);
    
    /* Does variable of same name exist in input file? */
    rcd=nco_inq_varid_flg(nc_id,crd_nm,&crd_id);
    if(rcd == NC_NOERR){
      /* Dimension is coordinate. Is it already on list? */
      int lst_idx;
      
      for(lst_idx=0;lst_idx<*nbr_xtr;lst_idx++){
	if(crd_id == xtr_lst[lst_idx].id) break;
      } /* end loop over lst_idx */
      if(lst_idx == *nbr_xtr){
	/* Coordinate is not already on the list, put it there. */
	if(*nbr_xtr == 0) xtr_lst=(nm_id_sct *)nco_malloc((*nbr_xtr+1)*sizeof(nm_id_sct)); else xtr_lst=(nm_id_sct *)nco_realloc((void *)xtr_lst,(*nbr_xtr+1)*sizeof(nm_id_sct));
	/* According to the man page for realloc(), this should work even when xtr_lst == NULL */
/*	xtr_lst=(nm_id_sct *)nco_realloc((void *)xtr_lst,(*nbr_xtr+1)*sizeof(nm_id_sct));*/
	xtr_lst[*nbr_xtr].nm=(char *)strdup(crd_nm);
	xtr_lst[*nbr_xtr].id=crd_id;
	(*nbr_xtr)++;
      } /* end if */
    } /* end if */
  } /* end loop over idx */
  
  return xtr_lst;
  
} /* end nco_var_lst_add_crd() */

void
var_lst_convert /* [fnc] Make variable structure list from variable name ID list */
(const int nc_id, /* I [enm] netCDF file ID */
 nm_id_sct *xtr_lst, /* I [sct] Current extraction list (destroyed) */
 const int nbr_xtr, /* I [nbr] Number of variables in input file */
 dmn_sct * const * const dim, /* I [sct] Dimensions associated with input variable list */
 const int nbr_dmn_xtr, /* I [nbr] Number of dimensions in list  */
 var_sct *** const var_ptr, /* O [sct] Variable list (for input file) */
 var_sct *** const var_out_ptr) /* O [sct] Duplicate variable list (for output file) */
{
  /* Purpose: Make var_sct list from nm_id list 
     The var_sct lst is duplicated to be used for output list */

  int idx;

  var_sct **var;
  var_sct **var_out;

  var=(var_sct **)nco_malloc(nbr_xtr*sizeof(var_sct *));
  var_out=(var_sct **)nco_malloc(nbr_xtr*sizeof(var_sct *));

  /* Fill in variable structure list for all extracted variables */
  for(idx=0;idx<nbr_xtr;idx++){
    var[idx]=nco_var_fll(nc_id,xtr_lst[idx].id,xtr_lst[idx].nm,dim,nbr_dmn_xtr);
    var_out[idx]=nco_var_dpl(var[idx]);
    (void)nco_xrf_var(var[idx],var_out[idx]);
    (void)nco_xrf_dmn(var_out[idx]);
  } /* end loop over idx */
  
  *var_ptr=var;
  *var_out_ptr=var_out;

} /* end var_lst_convert() */

void
nco_var_lst_dvd /* [fnc] Divide input lists into output lists */
(var_sct * const * const var, /* I [sct] Variable list (input file) */
 var_sct * const * const var_out, /* I [sct] Variable list (output file) */
 const int nbr_var, /* I [nbr] Number of variables */
 const bool NCAR_CSM_FORMAT, /* I [flg] File adheres to NCAR CSM conventions */
 CST_X_PTR_CST_PTR_CST_Y(dmn_sct,dmn_xcl), /* I [sct] Dimensions not allowed in fixed variables */
 const int nbr_dmn_xcl, /* I [nbr] Number of excluded dimensions */
 var_sct *** const var_fix_ptr, /* O [sct] Fixed-variables (input file) */
 var_sct *** const var_fix_out_ptr, /* O [sct] Fixed-variables (output file) */
 int * const nbr_var_fix, /* O [nbr] Number of fixed variables */
 var_sct *** const var_prc_ptr, /* O [sct] Processed-variables (input file) */
 var_sct *** const var_prc_out_ptr, /* O [sct] Processed-variables (output file) */
 int * const nbr_var_prc) /* O [nbr] Number of processed variables */
{
  /* Purpose: Divide two input lists into output lists based on program type */

  char *var_nm=NULL_CEWI;

  int idx;
  int prg; /* Program key */

  enum op_typ{
    fix, /* 0 */
    prc /* 1 */
  };

  int idx_dmn;
  int idx_xcl;
  int var_op_typ[NC_MAX_VARS];

  nc_type var_type=NC_NAT; /* NC_NAT present in netcdf.h version netCDF 3.5+ */

  var_sct **var_fix;
  var_sct **var_fix_out;
  var_sct **var_prc;
  var_sct **var_prc_out;

  prg=prg_get(); /* Program key */

  /* Allocate space for too many structures, then realloc() at the end, to avoid duplication. */
  var_fix=(var_sct **)nco_malloc(NC_MAX_VARS*sizeof(var_sct *));
  var_fix_out=(var_sct **)nco_malloc(NC_MAX_VARS*sizeof(var_sct *));
  var_prc=(var_sct **)nco_malloc(NC_MAX_VARS*sizeof(var_sct *));
  var_prc_out=(var_sct **)nco_malloc(NC_MAX_VARS*sizeof(var_sct *));

  /* Find operation type for each variable: for now this is either fix or prc */
  for(idx=0;idx<nbr_var;idx++){
    
    /* Initialize operation type */
    var_op_typ[idx]=prc;
    var_nm=var[idx]->nm;
    var_type=var[idx]->type;

    /* Override operation type based depending on both variable type and program */
    switch(prg){
    case ncap:
      var_op_typ[idx]=fix;
      break;
    case ncra:
      if(!var[idx]->is_rec_var) var_op_typ[idx]=fix;
      break;
    case ncea:
      if((var[idx]->is_crd_var) || (var_type == NC_CHAR) || (var_type == NC_BYTE)) var_op_typ[idx]=fix;
      break;
    case ncbo:
      if((var[idx]->is_crd_var) || (var_type == NC_CHAR) || (var_type == NC_BYTE)) var_op_typ[idx]=fix;
      break;
    case ncflint:
      if((var_type == NC_CHAR) || (var_type == NC_BYTE) || (var[idx]->is_crd_var && !var[idx]->is_rec_var)) var_op_typ[idx]=fix;
      break;
    case ncwa:
      /* Process every variable containing an excluded (averaged) dimension */
      for(idx_dmn=0;idx_dmn<var[idx]->nbr_dim;idx_dmn++){
	for(idx_xcl=0;idx_xcl<nbr_dmn_xcl;idx_xcl++){
	  if(var[idx]->dim[idx_dmn]->id == dmn_xcl[idx_xcl]->id) break;
	} /* end loop over idx_xcl */
	if(idx_xcl != nbr_dmn_xcl){
	  var_op_typ[idx]=prc;
	  break;
	} /* end if */
      } /* end loop over idx_dmn */
      /* Variables which do not contain an excluded (averaged) dimension must be fixed */
      if(idx_dmn == var[idx]->nbr_dim) var_op_typ[idx]=fix;
      break;
    case ncrcat:
      if(!var[idx]->is_rec_var) var_op_typ[idx]=fix;
      break;
    case ncecat:
      if(var[idx]->is_crd_var) var_op_typ[idx]=fix;
      break;
    } /* end switch */
    
    if(NCAR_CSM_FORMAT){
      if(!strcmp(var_nm,"ntrm") || !strcmp(var_nm,"ntrn") || !strcmp(var_nm,"ntrk") || !strcmp(var_nm,"ndbase") || !strcmp(var_nm,"nsbase") || !strcmp(var_nm,"nbdate") || !strcmp(var_nm,"nbsec") || !strcmp(var_nm,"mdt") || !strcmp(var_nm,"mhisf")) var_op_typ[idx]=fix;
      if(prg == ncbo && (!strcmp(var_nm,"hyam") || !strcmp(var_nm,"hybm") || !strcmp(var_nm,"hyai") || !strcmp(var_nm,"hybi") || !strcmp(var_nm,"gw") || !strcmp(var_nm,"ORO") || !strcmp(var_nm,"date") || !strcmp(var_nm,"datesec"))) var_op_typ[idx]=fix;
    } /* end if NCAR_CSM_FORMAT */

  } /* end loop over var */

  /* Assign list pointers based on operation type for each variable */
  *nbr_var_prc=*nbr_var_fix=0;
  for(idx=0;idx<nbr_var;idx++){
    if(var_op_typ[idx] == fix){
      var_fix[*nbr_var_fix]=var[idx];
      var_fix_out[*nbr_var_fix]=var_out[idx];
      ++*nbr_var_fix;
    }else{
      var_prc[*nbr_var_prc]=var[idx];
      var_prc_out[*nbr_var_prc]=var_out[idx];
      ++*nbr_var_prc;
      if(((var[idx]->type == NC_CHAR) || (var[idx]->type == NC_BYTE)) && ((prg != ncrcat) && (prg != ncecat))){
	(void)fprintf(stderr,"%s: WARNING Variable %s is of type %s, for which processing (i.e., averaging, differencing) is ill-defined\n",prg_nm_get(),var[idx]->nm,nco_typ_sng(var[idx]->type));
      } /* end if */
    } /* end else */
  } /* end loop over var */
  
  /* Sanity check */
  if(*nbr_var_prc+*nbr_var_fix != nbr_var){
    (void)fprintf(stdout,"%s: ERROR nbr_var_prc+nbr_var_fix != nbr_var\n",prg_nm_get());
    nco_exit(EXIT_FAILURE);
  } /* end if */

  /* fxm: Remove ncap exception when finished with ncap list processing */
  if(*nbr_var_prc==0 && prg != ncap){
    (void)fprintf(stdout,"%s: ERROR no variables fit criteria for processing\n",prg_nm_get());
    switch(prg_get()){
    case ncap:
      (void)fprintf(stdout,"%s: HINT Extraction list must contain some derived fields\n",prg_nm_get());
    case ncra:
      (void)fprintf(stdout,"%s: HINT Extraction list must contain record variables that are not NC_CHAR or NC_BYTE in order to perform a running average\n",prg_nm_get());
      break;
    case ncea:
      (void)fprintf(stdout,"%s: HINT Extraction list must contain non-coordinate variables that are not NC_CHAR or NC_BYTE in order to perform an ensemble average\n",prg_nm_get());
      break;
    case ncbo:
      (void)fprintf(stdout,"%s: HINT Extraction list must contain non-coordinate variables that are not NC_CHAR or NC_BYTE in order to perform a binary operation (e.g., subtraction)\n",prg_nm_get());
      break;
    case ncflint:
      (void)fprintf(stdout,"%s: HINT Extraction list must contain variables that are not NC_CHAR or NC_BYTE in order to interpolate\n",prg_nm_get());
      break;
    case ncwa:
      (void)fprintf(stdout,"%s: HINT Extraction list must contain variables that contain an averaging dimension in order to perform an average\n",prg_nm_get());
      break;
    case ncrcat:
      (void)fprintf(stdout,"%s: HINT Extraction list must contain record variables in order to perform a record concatenation\n",prg_nm_get());
      break;
    case ncecat:
      (void)fprintf(stdout,"%s: HINT Extraction list must contain non-coordinate variables in order to perform an ensemble concatenation\n",prg_nm_get());
      break;
    } /* end switch */
    nco_exit(EXIT_FAILURE);
  } /* end if */

  /* Free unused space and save pointers in output variables */
  if(*nbr_var_fix > 0) *var_fix_ptr=(var_sct **)nco_realloc(var_fix,*nbr_var_fix*sizeof(var_sct *)); else *var_fix_ptr=NULL;
  if(*nbr_var_fix > 0) *var_fix_out_ptr=(var_sct **)nco_realloc(var_fix_out,*nbr_var_fix*sizeof(var_sct *)); else *var_fix_out_ptr=NULL;
  if(*nbr_var_prc > 0) *var_prc_ptr=(var_sct **)nco_realloc(var_prc,*nbr_var_prc*sizeof(var_sct *)); else *var_prc_ptr=NULL;
  if(*nbr_var_prc > 0) *var_prc_out_ptr=(var_sct **)nco_realloc(var_prc_out,*nbr_var_prc*sizeof(var_sct *)); else *var_prc_out_ptr=NULL;

} /* end nco_var_lst_dvd */

