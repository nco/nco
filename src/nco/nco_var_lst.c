/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_var_lst.c,v 1.38 2004-09-07 04:31:19 zender Exp $ */

/* Purpose: Variable list utilities */

/* Copyright (C) 1995--2004 Charlie Zender
   This software may be modified and/or re-distributed under the terms of the GNU General Public License (GPL) Version 2
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

#include "nco_var_lst.h" /* Variable list utilities */

nm_id_sct * /* O [sct] Variable extraction list */
nco_var_lst_mk_old /* [fnc] Create variable extraction list */
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
} /* end nco_var_lst_mk_old() */

nm_id_sct * /* O [sct] Variable extraction list */
nco_var_lst_mk /* [fnc] Create variable extraction list using regular expressions */
(const int nc_id, /* I [enm] netCDF file ID */
 const int nbr_var, /* I [nbr] Number of variables in input file */
 char **var_lst_in, /* I [sng] User-specified list of variable names */
 const bool PROCESS_ALL_COORDINATES, /* I [flg] Process all coordinates */
 int * const nbr_xtr) /* I/O [nbr] Number of variables in current extraction list */
{
  /* Purpose: Create variable extraction list with or without regular expressions */

  int idx;
  int jdx;
  int nbr_tmp=0;
  int nbr_match;

  char *var_sng;

  nm_id_sct *xtr_lst=NULL; /* xtr_lst may be alloc()'d from NULL with -c option */
  nm_id_sct *in_lst=NULL; 
  bool *in_bool=NULL;

  /* Create list of all vars in input file */
  char var_nm[NC_MAX_NAME];
    
  in_lst=(nm_id_sct *)nco_malloc(nbr_var*sizeof(nm_id_sct));
  for(idx=0;idx<nbr_var;idx++){
    /* Get name of each variable. */
    (void)nco_inq_varname(nc_id,idx,var_nm);
    in_lst[idx].nm=(char *)strdup(var_nm);
    in_lst[idx].id=idx;
  } /* end loop over idx */
  
    /* Return all variables if .. */
  if(*nbr_xtr==0 && !PROCESS_ALL_COORDINATES){
    *nbr_xtr=nbr_var;
    return in_lst;
  } /* end if */     
      
  /* intialize and alloc  bool array to all False */
  in_bool=(bool *)nco_calloc((size_t)nbr_var,sizeof(bool));
  
  /* Loop through var_lst_in */
  for(idx=0;idx<*nbr_xtr;idx++){
     var_sng=var_lst_in[idx];

     /* Convert pounds (#s) back to commas */
     while(*var_sng){
       if (*var_sng=='#') *var_sng=',';
       var_sng++;
     } /* end while */
     var_sng=var_lst_in[idx];

     /* If var_sng is regular expression... */
     if(strpbrk(var_sng,".*^$\\[]()<>+?|{}")){
       /* ...and regular expression library is present */
#ifdef NCO_HAVE_REGEX_FUNCTIONALITY
       nbr_match=nco_var_meta_search(nbr_var,in_lst,var_sng,in_bool);
       if(nbr_match==0) (void)fprintf(stdout,"%s: WARNING: Regular expression \"%s\" does not match any variables\nHINT: http://nco.sf.net/nco.html#rx",prg_nm_get(),var_sng); 
       continue;
#else
      (void)fprintf(stdout,"%s: ERROR: Sorry, wildcarding (extended regular expression matches to variables) is not available, so unable to compile regular expression \"%s\".\nHINT: Make sure libregex.a is on path and re-build NCO.\n",prg_nm_get(),var_sng);
       nco_exit(EXIT_FAILURE);
#endif /* NCO_HAVE_REGEX_FUNCTIONALITY */
     } /* end if regular expression */
    
    /* Normal variable so search through var array */
     for(jdx=0;jdx<nbr_var;jdx++)
       if(!strcmp(var_sng,in_lst[jdx].nm)) break;
       /* No matches found so die gently */
     if(jdx==nbr_var){
       (void)fprintf(stdout,"%s: ERROR nco_var_lst_mk() reports user-specified variable \"%s\" is not in input file\n",prg_nm_get(),var_sng); 
       nco_exit(EXIT_FAILURE);
     }else{
       in_bool[jdx]=True;
     } /* end else */
  }
  /* Create final list of vars using var list and bool array */

  /* malloc() xtr_lst to maximium size(nbr_var) */
  xtr_lst=(nm_id_sct *)nco_malloc(nbr_var*sizeof(nm_id_sct));
  nbr_tmp=0;

  for(idx=0;idx<nbr_var;idx++){
    /* Copy var to output array */
    if(in_bool[idx]){
      xtr_lst[nbr_tmp].nm=(char *)strdup(in_lst[idx].nm);
      xtr_lst[nbr_tmp].id=in_lst[idx].id;
      nbr_tmp++;
    } /* end if */
    (void)nco_free(in_lst[idx].nm);
  } /* end loop over var */

  /* realloc() list to actual size */  
  xtr_lst=(nm_id_sct *)nco_realloc(xtr_lst,nbr_tmp*sizeof(nm_id_sct));

  (void)nco_free(in_lst);
  (void)nco_free(in_bool);

  *nbr_xtr=nbr_tmp;    
  return xtr_lst;
} /* end nco_var_lst_mk_meta() */

/* Compile only if regular expression library is present */
#ifdef NCO_HAVE_REGEX_FUNCTIONALITY

int /* O [nbr] Number of matches found */
nco_var_meta_search /* [fnc] Search for pattern matches in var string list */
(int nbr_var, /* I [nbr] number of vars in srch_sng and size of in_bool */
 nm_id_sct *in_lst, /* I [sct] List of all variables in input file (with IDs) */
 char *rexp, /* I [sng] Regular expression pattern */
 bool *in_bool) /* O [flg] Matched vars holder */
{
  /* If regular expression does not compile then program dies */
  int idx;
  int err_no;
  int cflags;
  int eflags;
  int nbr_mtch=0;
  size_t no_sub;
  
  regmatch_t *result;
  regex_t *r;

  r=(regex_t *)nco_malloc(sizeof(regex_t));

  /* Choose RE_SYNTAX_POSIX_EXTENDED regular expression type */
  cflags=(REG_EXTENDED | REG_NEWLINE);
  /* Set execution flags */
  eflags=0;

  /* Compile regular expression */
  if((err_no=regcomp(r,rexp,cflags)) != 0){ /* Compile regular expression */
    char const * rx_err_sng;  
    /* POSIX regcomp return error codes */
    switch(err_no){
    case REG_BADPAT: rx_err_sng="Invalid pattern."; break;  
    case REG_ECOLLATE: rx_err_sng="Not implemented."; break;
    case REG_ECTYPE: rx_err_sng="Invalid character class name."; break;
    case REG_EESCAPE: rx_err_sng="Trailing backslash."; break;
    case REG_ESUBREG: rx_err_sng="Invalid back reference."; break;
    case REG_EBRACK: rx_err_sng="Unmatched left bracket."; break;
    case REG_EPAREN: rx_err_sng="Parenthesis imbalance."; break;
    case REG_EBRACE: rx_err_sng="Unmatched {."; break;
    case REG_BADBR: rx_err_sng="Invalid contents of { }."; break;
    case REG_ERANGE: rx_err_sng="Invalid range end."; break;
    case REG_ESPACE: rx_err_sng="Ran out of memory."; break;
    case REG_BADRPT: rx_err_sng="No preceding re for repetition op"; break;
    default: rx_err_sng="Invalid pattern."; break;  
    } /* end switch */
    (void)fprintf(stdout,"%s: ERROR nco_var_meta_search() error in regular expression \"%s\" %s \n",prg_nm_get(),rexp,rx_err_sng); 
    nco_exit(EXIT_FAILURE);
  } /* end if err */

  no_sub=r->re_nsub+1; /* How many matches are there in a line? */

  /* Search string */
  result=(regmatch_t *)nco_malloc(sizeof(regmatch_t)*no_sub);

  /* Search each var string for matches */
  for(idx=0;idx<nbr_var;idx++){  
    if(!regexec(r, in_lst[idx].nm,no_sub,result,eflags)){
      in_bool[idx]=True;
      nbr_mtch++;
    } /* end if */
  } /* end loop over variables */

  regfree(r); /* Free regular expression data structure */
  (void *)nco_free(r);
  (void *)nco_free(result);

   return nbr_mtch;
} /* end nco_var_meta_search() */

#endif /* NCO_HAVE_REGEX_FUNCTIONALITY */

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
	/* Coordinate is not already on the list, put it there */
	if(*nbr_xtr == 0) xtr_lst=(nm_id_sct *)nco_malloc((*nbr_xtr+1)*sizeof(nm_id_sct)); else xtr_lst=(nm_id_sct *)nco_realloc((void *)xtr_lst,(*nbr_xtr+1)*sizeof(nm_id_sct));
	/* According to man page for realloc(), this should work even when xtr_lst == NULL */
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
 const bool NCAR_CCSM_FORMAT, /* I [flg] File adheres to NCAR CCSM conventions */
 const int nco_pck_map, /* I [enm] Packing map */
 const int nco_pck_plc, /* I [enm] Packing policy */
 CST_X_PTR_CST_PTR_CST_Y(dmn_sct,dmn_xcl), /* I [sct] Dimensions not allowed in fixed variables */
 const int nbr_dmn_xcl, /* I [nbr] Number of altered dimensions */
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
    fix, /* 0 [enm] Fix variable (operator alters neither data nor metadata) */
    prc /* 1 [enm] Process variable (operator may alter data or metadata) */
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
    
    /* Initialize operation type to processed. Change to fixed where warranted later. */
    var_op_typ[idx]=prc;
    var_nm=var[idx]->nm;
    var_type=var[idx]->type;

    /* Override operation type based depending on both variable type and program */
    switch(prg){
    case ncap:
      var_op_typ[idx]=fix;
      break;
    case ncatted:
      /* Do nothing */
      break;
    case ncbo:
      if((var[idx]->is_crd_var) || (var_type == NC_CHAR) || (var_type == NC_BYTE)) var_op_typ[idx]=fix;
      break;
    case ncea:
      if((var[idx]->is_crd_var) || (var_type == NC_CHAR) || (var_type == NC_BYTE)) var_op_typ[idx]=fix;
      break;
    case ncecat:
      if(var[idx]->is_crd_var) var_op_typ[idx]=fix;
      break;
    case ncflint:
      if((var_type == NC_CHAR) || (var_type == NC_BYTE) || (var[idx]->is_crd_var && !var[idx]->is_rec_var)) var_op_typ[idx]=fix;
      break;
    case ncks:
      /* Do nothing */
      break;
    case ncra:
      if(!var[idx]->is_rec_var) var_op_typ[idx]=fix;
      break;
    case ncrcat:
      if(!var[idx]->is_rec_var) var_op_typ[idx]=fix;
      break;
    case ncpdq:
    case ncwa:
      if(nco_pck_plc != nco_pck_plc_nil){
	/* Variables are processed for packing/unpacking operator unless ... */
	if(
	   /* ...unpacking requested for unpacked variable... */
	   (nco_pck_plc == nco_pck_plc_upk && !var[idx]->pck_ram) ||
	   /* ...or packing unpacked requested and variable is already packed... */
	   (nco_pck_plc == nco_pck_plc_all_xst_att && var_prc[idx]->pck_ram) ||
	   /* ...or re-packing packed requested and variable is unpacked... */
	   (nco_pck_plc == nco_pck_plc_xst_new_att && !var_prc[idx]->pck_ram)
	   )
	  var_op_typ[idx]=fix;
	/* fxm: ncpdq packing treats all variables as processed */
	var_op_typ[idx]=prc;
      }else{
	/* Process every variable containing an altered (averaged, re-ordered, reversed) dimension */
	for(idx_dmn=0;idx_dmn<var[idx]->nbr_dim;idx_dmn++){
	  for(idx_xcl=0;idx_xcl<nbr_dmn_xcl;idx_xcl++){
	    if(var[idx]->dim[idx_dmn]->id == dmn_xcl[idx_xcl]->id) break;
	  } /* end loop over idx_xcl */
	  if(idx_xcl != nbr_dmn_xcl){
	    var_op_typ[idx]=prc;
	    break;
	  } /* end if */
	} /* end loop over idx_dmn */
	/* Fix variables with no altered (averaged, re-ordered, reversed) dimensions */
	if(idx_dmn == var[idx]->nbr_dim) var_op_typ[idx]=fix;
      } /* endif averaging or re-ordering */
      break;
    default: nco_dfl_case_prg_id_err(); break;
    } /* end switch */
    
    if(NCAR_CCSM_FORMAT){
      if(!strcmp(var_nm,"ntrm") || !strcmp(var_nm,"ntrn") || !strcmp(var_nm,"ntrk") || !strcmp(var_nm,"ndbase") || !strcmp(var_nm,"nsbase") || !strcmp(var_nm,"nbdate") || !strcmp(var_nm,"nbsec") || !strcmp(var_nm,"mdt") || !strcmp(var_nm,"mhisf")) var_op_typ[idx]=fix;
      if(prg == ncbo && (!strcmp(var_nm,"hyam") || !strcmp(var_nm,"hybm") || !strcmp(var_nm,"hyai") || !strcmp(var_nm,"hybi") || !strcmp(var_nm,"gw") || !strcmp(var_nm,"ORO") || !strcmp(var_nm,"date") || !strcmp(var_nm,"datesec"))) var_op_typ[idx]=fix;
    } /* end if NCAR_CCSM_FORMAT */

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
      if(((var[idx]->type == NC_CHAR) || (var[idx]->type == NC_BYTE)) && ((prg != ncecat) && (prg != ncpdq) && (prg != ncrcat))){
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
  /* fxm: ncpdq processes all variables when pakcing requested */
  if(*nbr_var_prc == 0 && prg != ncap && prg != ncpdq){
    (void)fprintf(stdout,"%s: ERROR no variables fit criteria for processing\n",prg_nm_get());
    switch(prg_get()){
    case ncap:
      (void)fprintf(stdout,"%s: HINT Extraction list must contain at least one derived field\n",prg_nm_get());
    case ncatted:
      /* Do nothing */
      break;
    case ncbo:
      (void)fprintf(stdout,"%s: HINT Extraction list must contain a non-coordinate variable that is not NC_CHAR or NC_BYTE in order to perform a binary operation (e.g., subtraction)\n",prg_nm_get());
      break;
    case ncea:
      (void)fprintf(stdout,"%s: HINT Extraction list must contain a non-coordinate variable that is not NC_CHAR or NC_BYTE\n",prg_nm_get());
      break;
    case ncecat:
      (void)fprintf(stdout,"%s: HINT Extraction list must contain a non-coordinate variable\n",prg_nm_get());
      break;
    case ncflint:
      (void)fprintf(stdout,"%s: HINT Extraction list must contain a variable that is not NC_CHAR or NC_BYTE\n",prg_nm_get());
      break;
    case ncks:
      /* Do nothing */
      break;
    case ncpdq:
      (void)fprintf(stdout,"%s: HINT Extraction list must contain a variable that shares at least one dimension with the re-order list\n",prg_nm_get());
      break;
    case ncra:
      (void)fprintf(stdout,"%s: HINT Extraction list must contain a record variable that is not NC_CHAR or NC_BYTE\n",prg_nm_get());
      break;
    case ncrcat:
      (void)fprintf(stdout,"%s: HINT Extraction list must contain a record variable which to concatenate\n",prg_nm_get());
      break;
    case ncwa:
      (void)fprintf(stdout,"%s: HINT Extraction list must contain a variable that contains an averaging dimension\n",prg_nm_get());
      break;
    default: nco_dfl_case_prg_id_err(); break;
    } /* end switch */
    nco_exit(EXIT_FAILURE);
  } /* end if */

  /* Free unused space and save pointers in output variables */
  *var_fix_ptr=(var_sct **)nco_realloc(var_fix,*nbr_var_fix*sizeof(var_sct *));
  *var_fix_out_ptr=(var_sct **)nco_realloc(var_fix_out,*nbr_var_fix*sizeof(var_sct *));
  *var_prc_ptr=(var_sct **)nco_realloc(var_prc,*nbr_var_prc*sizeof(var_sct *));
  *var_prc_out_ptr=(var_sct **)nco_realloc(var_prc_out,*nbr_var_prc*sizeof(var_sct *));

} /* end nco_var_lst_dvd */

