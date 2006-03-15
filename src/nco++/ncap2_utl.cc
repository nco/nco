/* $Header: /data/zender/nco_20150216/nco/src/nco++/ncap2_utl.cc,v 1.6 2006-03-15 14:03:21 hmb Exp $ */

/* Purpose: netCDF arithmetic processor */

/* Copyright (C) 1995--2005 Charlie Zender
   This software may be modified and/or re-distributed under the terms of the GNU General Public License (GPL) Version 2
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

#include <assert.h>
#include "ncap2.hh" /* netCDF arithmetic processor */

/* have removed extern -- (not linking to ncap_lex.l */
/*extern*/ char ncap_err_sng[200]; /* [sng] Buffer for error string (declared in ncap_lex.l) */

var_sct *                  /* O [sct] initialized variable */
ncap_var_init(
const char * const var_nm, /* I [sng] variable name constant */
prs_sct *prs_arg)          /* I/O  vectors of atts,vars,dims, filenames */
{
  /* Purpose: Initialize variable structure, retrieve variable values from disk
     Parser calls ncap_var_init() when it encounters a new RHS variable */
  /* const char fnc_nm[]="ncap_var_init()"; *//* [sng] Function name */

  int idx;
  int nbr_dmn_var;
  int *dim_id=NULL;
  int var_id;
  int rcd;
  int fl_id;

  char dmn_nm[NC_MAX_NAME];

  dmn_sct *dmn_fd; 
  dmn_sct *dmn_nw;
  
  dmn_sct **dmn_out;  // dereferencing
  int nbr_dmn_out  ;  // dereferencing
  
  var_sct *var;


  /* Check output file for var */  
  rcd=nco_inq_varid_flg(prs_arg->out_id,var_nm,&var_id);
  if(rcd == NC_NOERR ){
    fl_id=prs_arg->out_id;
  }else{
    /* Check input file for ID */
    rcd=nco_inq_varid_flg(prs_arg->in_id,var_nm,&var_id);
    if(rcd != NC_NOERR){
      /* Return NULL if variable not in input or output file */
      (void)fprintf(stderr,"WARNING unable to find %s in %s or %s\n",var_nm,prs_arg->fl_in,prs_arg->fl_out);    
      return (var_sct *)NULL;
    } /* end if */
    
    /* Find dimensions used in var
       Learn which are not already in output list prs_arg->dmn_out and output file
       Add these to output list and output file */
    (void)nco_redef(prs_arg->out_id);
    fl_id=prs_arg->in_id;
    
    (void)nco_inq_varndims(fl_id,var_id,&nbr_dmn_var);
    if(nbr_dmn_var>0){
      dim_id=(int *)nco_malloc(nbr_dmn_var*sizeof(int));
      
      (void)nco_inq_vardimid(fl_id,var_id,dim_id);
      for(idx=0;idx<nbr_dmn_var;idx++){ 
	// get dim name
	(void)nco_inq_dimname(fl_id,dim_id[idx],dmn_nm);
        // check if dim is already in output
        if(prs_arg->ptr_dmn_out_vtr->find(dmn_nm) != NULL) continue; 
	// Get dim from input list
        dmn_fd= prs_arg->ptr_dmn_in_vtr->find(dmn_nm);
	// not in list -- crash out
	if(dmn_fd == (dmn_sct*)NULL){
	  (void)fprintf(stderr,"%s: DEBUG New dimension %s not found in %s or %s\n",prg_nm_get(),dmn_nm ,prs_arg->fl_in, prs_arg->fl_out);
	  exit(1);
	}
        dmn_nw=nco_dmn_dpl(dmn_fd);
	(void)nco_dmn_xrf(dmn_nw,dmn_fd);
	// write dim to output
	(void)nco_dmn_dfn(prs_arg->fl_out,prs_arg->out_id,&dmn_nw,1);          
	// Add new dim to output list
	(void)prs_arg->ptr_dmn_out_vtr->push(dmn_nw);
	  if(dbg_lvl_get() > 2) (void)fprintf(stderr,"%s: DEBUG Found new dimension %s in input variable %s in file %s. Defining dimension %s in output file %s\n",prg_nm_get(),dmn_nm,var_nm,prs_arg->fl_in,dmn_nm,prs_arg->fl_out);

      }
      (void)nco_free(dim_id);
    }
    (void)nco_enddef(prs_arg->out_id); 
  } // end else  
  
  if(dbg_lvl_get() > 2) (void)fprintf(stderr,"%s: parser VAR action called ncap_var_init() to retrieve %s from disk\n",prg_nm_get(),var_nm);

  nbr_dmn_out=prs_arg->ptr_dmn_out_vtr->size();
  dmn_out=prs_arg->ptr_dmn_out_vtr->ptr(0);

  var=nco_var_fll(fl_id,var_id,var_nm,dmn_out,nbr_dmn_out);
  /*  var->nm=(char *)nco_malloc((strlen(var_nm)+1UL)*sizeof(char));
  (void)strcpy(var->nm,var_nm); */

  /* Tally is not required yet since ncap does not perform cross-file operations (yet) */
  /* var->tally=(long *)nco_malloc_dbg(var->sz*sizeof(long),"Unable to malloc() tally buffer in variable initialization",fnc_nm);
      (void)nco_zero_long(var->sz,var->tally); */
  var->tally=(long *)NULL;

  /* Retrieve variable values from disk into memory */
  (void)nco_var_get(fl_id,var); 

  return var;
} /* end ncap_var_init() */


int                /* O  [bool] bool - ture if sucessful */
ncap_var_write     /*   [fnc] Write var to output file prs_arg->fl_out */ 
(var_sct *var,     /* I  [sct] variable to be written - freed at end */  
 prs_sct *prs_arg) /* I/O vectors of atts & vars & file names  */
{

  /* Purpose: Define variable in output file and write variable */
  const char mss_val_sng[]="missing_value"; /* [sng] Unidata standard string for missing value */
  const char add_fst_sng[]="add_offset"; /* [sng] Unidata standard string for add offset */
  const char scl_fct_sng[]="scale_factor"; /* [sng] Unidata standard string for scale factor */
  int rcd; /* [rcd] Return code */
  int var_out_id;

#ifdef NCO_RUSAGE_DBG
  long maxrss; /* [B] Maximum resident set size */
#endif /* !NCO_RUSAGE_DBG */


  rcd=nco_inq_varid_flg(prs_arg->out_id,var->nm,&var_out_id);

  if(rcd == NC_NOERR){
    (void)fprintf(stdout,"Warning: Variable %s has aleady been saved in %s",var->nm,prs_arg->fl_out);
    var = nco_var_free(var);
    return False;
  }
  
  /* Put file in define mode to allow metadata writing */
  (void)nco_redef(prs_arg->out_id);
  
  /* Define variable */   
  (void)nco_def_var(prs_arg->out_id,var->nm,var->type,var->nbr_dim,var->dmn_id,&var_out_id);
  /* Put missing value */  
  if(var->has_mss_val) (void)nco_put_att(prs_arg->out_id,var_out_id,mss_val_sng,var->type,1,var->mss_val.vp);
  
      /* Write/overwrite scale_factor and add_offset attributes */
  if(var->pck_ram){ /* Variable is packed in memory */
  if(var->has_scl_fct) (void)nco_put_att(prs_arg->out_id,var_out_id,scl_fct_sng,var->typ_upk,1,var->scl_fct.vp);
  if(var->has_add_fst) (void)nco_put_att(prs_arg->out_id,var_out_id,add_fst_sng,var->typ_upk,1,var->add_fst.vp);
      } /* endif pck_ram */

  /* Take output file out of define mode */
  (void)nco_enddef(prs_arg->out_id);
  
  /* Write variable */ 
  if(var->nbr_dim == 0){
    (void)nco_put_var1(prs_arg->out_id,var_out_id,0L,var->val.vp,var->type);
  }else{
    (void)nco_put_vara(prs_arg->out_id,var_out_id,var->srt,var->cnt,var->val.vp,var->type);
  } /* end else */
  
#ifdef NCO_RUSAGE_DBG
  /* Compile: cd ~/nco/bld;make 'USR_TKN=-DNCO_RUSAGE_DBG';cd - */
  /* Print rusage memory usage statistics */
  if(dbg_lvl_get() >= 0) (void)fprintf(stdout,"%s: INFO ncap_var_write() writing variable %s\n",prg_nm_get(),var->nm);
  maxrss=nco_mmr_rusage_prn((int)0);
#endif /* !NCO_RUSAGE_DBG */

  /* Free varible */
  var=nco_var_free(var);

  return True;
} /* end ncap_var_write() */


var_sct *                /* O [sct] variable containing attribute */
ncap_att_init(           /*   [fnc] Grab an attribute from input file */
const char *const va_nm, /* I [sng] att name of form var_nm&att_nm */ 
prs_sct *prs_arg)        /* I/O vectors of atts & vars & file names  */
{
int rcd;
int var_id;
long sz;
nc_type type;

char *var_nm;
char *att_nm;


var_sct *var_ret;

  var_nm=strdup(va_nm);
  att_nm=strchr(var_nm,'@');
  if (att_nm==NULL) return (var_sct*)NULL;
  *att_nm++='\0';

  if( !strcmp(var_nm,"global")){ 
    var_id=NC_GLOBAL;
  }else{
    rcd=nco_inq_varid_flg(prs_arg->in_id,var_nm,&var_id);
    if (rcd !=NC_NOERR) return (var_sct*)NULL;
  }
  rcd=nco_inq_att_flg(prs_arg->in_id,var_id,att_nm,&type,&sz);
  if (rcd == NC_ENOTATT) return (var_sct*)NULL;


  var_ret=(var_sct*)nco_malloc(sizeof(var_sct));
  (void)var_dfl_set(var_ret);

  var_ret->nm=strdup(va_nm);
  var_ret->id=var_id;
  var_ret->nc_id=prs_arg->in_id;
  var_ret->type=type;
  var_ret->sz=sz;
  // maybe needed ?
  var_ret->nbr_dim=0;

  var_ret->val.vp=(void *)nco_malloc(sz*nco_typ_lng(type));


  rcd=nco_get_att(prs_arg->in_id,var_id,att_nm,var_ret->val.vp,type);
  var_nm=(char*)nco_free(var_nm);
  if (rcd !=NC_NOERR) {
    var_ret=nco_var_free(var_ret);
    return (var_sct*)NULL;
  }

  return var_ret;
}
  
sym_sct *                    /* O [sct] return sym_sct */
ncap_sym_init                /*  [fnc] populate & return a symbol table structure */
(const char * const sym_nm,  /* I [sng] symbol name */
 double (*fnc_dbl)(double),  /* I [fnc_dbl] Pointer to double function */
 float (*fnc_flt)(float))    /* I [fnc_flt] Pointer to float  function */
{ 
  /* Purpose: Allocate space for sym_sct then initialize */
  sym_sct *symbol;
  symbol=(sym_sct *)nco_malloc(sizeof(sym_sct));
  symbol->nm=(char *)strdup(sym_nm);
  symbol->fnc_dbl=fnc_dbl;
  symbol->fnc_flt=fnc_flt;
  return symbol;
} /* end ncap_sym_init() */


var_sct * /* O [sct] Remainder of modulo operation of input variables (var_1%var_2) */
ncap_var_var_mod /* [fnc] Remainder (modulo) operation of two variables */
(var_sct *var_1, /* I [sc,t] Variable structure containing field */
 var_sct *var_2) /* I [sct] Variable structure containing divisor */
{
  /* Purpose: Remainder (modulo) operation of two variables (var_1%var_2) */
  /* Store result in var_2 */
  if(var_1->undefined) var_2->undefined=True;
    if(var_2->undefined) {
    var_1=nco_var_free(var_1);
    return var_2;
  }
  (void)ncap_var_retype(var_1,var_2);
   
  /* Handle initial scan */
  if(var_1->val.vp==(void*)NULL ) {
    if(var_1->nbr_dim > var_2->nbr_dim) {
      var_2=nco_var_free(var_2);
      return var_1;
    }else{
      var_1=nco_var_free(var_1);
      return var_2;
    }
  } 


 
  (void)ncap_var_cnf_dmn(&var_1,&var_2);
  if(var_1->has_mss_val){
    (void)nco_var_mod(var_1->type,var_1->sz,var_1->has_mss_val,var_1->mss_val,var_1->val,var_2->val);
  }else{
    (void)nco_var_mod(var_1->type,var_1->sz,var_2->has_mss_val,var_2->mss_val,var_1->val,var_2->val);
  } /* end else */
   
   var_1=nco_var_free(var_1);
   return var_2;
} /* end ncap_var_var_mod() */

var_sct * /* O [sct] Empowerment of input variables (var_1^var_2) */
ncap_var_var_pwr /* [fnc] Empowerment of two variables */ 
(var_sct *var_1, /* I [sct] Variable structure containing base */
 var_sct *var_2) /* I [sct] Variable structure containing exponent */
{
  /* Purpose: Empower two variables (var_1^var_2) */

  if(var_1->undefined) var_2->undefined=True;
    if(var_2->undefined) {
    var_1=nco_var_free(var_1);
    return var_2;
  }
  /* make sure vars are at least float */

  (void)ncap_var_retype(var_1,var_2);   

  /* Handle initial scan */
  if(var_1->val.vp==(void*)NULL ) {
    if(var_1->nbr_dim > var_2->nbr_dim) {
      var_2=nco_var_free(var_2);
      return var_1;
    }else{
      var_1=nco_var_free(var_1);
      return var_2;
    }
  } 

  (void)ncap_var_cnf_dmn(&var_1,&var_2);
  if(var_1->has_mss_val){
    (void)nco_var_pwr(var_1->type,var_1->sz,var_1->has_mss_val,var_1->mss_val,var_1->val,var_2->val);
  }else{
    (void)nco_var_pwr(var_1->type,var_1->sz,var_2->has_mss_val,var_2->mss_val,var_1->val,var_2->val);
  } /* end else */
   return var_2;
} /* end ncap_var_var_pwr() */

var_sct *           /* O [sct] Resultant variable (actually is var_in) */
ncap_var_fnc(   
var_sct *var_in,    /* I/O [sng] input variable */ 
sym_sct *app)       /* I [fnc_ptr] to apply to variable */
{
  /* Purpose: Evaluate fnc_dbl(var) or fnc_flt(var) for each value in variable
     Float and double functions are in app */
  long idx;
  long sz;
  ptr_unn op1;

  if(var_in->undefined) return var_in;
  
  
  /* Promote variable to NC_FLOAT */
  if(var_in->type < NC_FLOAT) var_in=nco_var_cnf_typ((nc_type)NC_FLOAT,var_in);

  /* deal with inital scan */
  if(var_in->val.vp==NULL) return var_in; 
  

  
  op1=var_in->val;
  sz=var_in->sz;
  (void)cast_void_nctype(var_in->type,&op1);
  if(var_in->has_mss_val) (void)cast_void_nctype(var_in->type,&(var_in->mss_val));
  
  switch(var_in->type){ 
  case NC_DOUBLE: {
    if(!var_in->has_mss_val){
      for(idx=0;idx<sz;idx++) op1.dp[idx]=(*(app->fnc_dbl))(op1.dp[idx]);
    }else{
      double mss_val_dbl=*(var_in->mss_val.dp); /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
        if(op1.dp[idx] != mss_val_dbl) op1.dp[idx]=(*(app->fnc_dbl))(op1.dp[idx]);
      } /* end for */
    } /* end else */
    break;
  }
  case NC_FLOAT: {
    if(!var_in->has_mss_val){
      for(idx=0;idx<sz;idx++) op1.fp[idx]=(*(app->fnc_flt))(op1.fp[idx]);
    }else{
      float mss_val_flt=*(var_in->mss_val.fp); /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
        if(op1.fp[idx] != mss_val_flt) op1.fp[idx]=(*(app->fnc_flt))(op1.fp[idx]);
      } /* end for */
    } /* end else */
    break;
  }
  default: nco_dfl_case_nc_type_err(); break;
  }/* end switch */
  
  if(var_in->has_mss_val) (void)cast_nctype_void(var_in->type,&(var_in->mss_val));
  return var_in;
} /* end ncap_var_fnc() */


var_sct *        /* O [sct] Resultant variable (actually is var) */
ncap_var_abs(    /* Purpose: Find absolute value of each element of var */
var_sct *var)    /* I/O [sct] input variable */
{

  if(var->undefined) return var;

  /* deal with inital scan */
  if(var->val.vp==NULL) return var; 

  (void)nco_var_abs(var->type,var->sz,var->has_mss_val,var->mss_val,var->val);
  return var;
} /* end ncap_var_abs */


nm_id_sct *            /* O [sct] new copy of xtr_lst */
nco_var_lst_copy(      /*   [fnc] Purpose: Copy xtr_lst and return new list */
nm_id_sct *xtr_lst,    /* I  [sct] input list */ 
int lst_nbr)           /* I  [nbr] number of elements in list */
{
  int idx;
  nm_id_sct *xtr_new_lst;
  
  if(lst_nbr == 0) return NULL;
  xtr_new_lst=(nm_id_sct *)nco_malloc(lst_nbr*sizeof(nm_id_sct));
  for(idx=0;idx<lst_nbr;idx++){
    xtr_new_lst[idx].nm=(char *)strdup(xtr_lst[idx].nm);
    xtr_new_lst[idx].id=xtr_lst[idx].id;
  } /* end loop over variable */
  return xtr_new_lst;           
} /* end nco_var_lst_copy() */


nm_id_sct *             /* O [sct] New list */
nco_var_lst_sub(
nm_id_sct *xtr_lst,     /* I [sct] input list */   
int *nbr_xtr,           /* I/O [ptr] size of xtr_lst and new list */
nm_id_sct *xtr_lst_b,   /* I [sct] list to be subtracted */   
int nbr_lst_b)          /* I [nbr] size eof xtr_lst_b */ 
{
  /* Purpose: Subtract from xtr_lst any elements from xtr_lst_b which are present and return new list */
  int idx;
  int xtr_idx;
  int xtr_nbr_new=0;
  
  nco_bool match;
  
  nm_id_sct *xtr_new_lst=NULL;
  
  if(*nbr_xtr == 0) return xtr_lst;
  
  xtr_new_lst=(nm_id_sct*)nco_malloc((size_t)(*nbr_xtr)*sizeof(nm_id_sct)); 
  for(idx=0;idx<*nbr_xtr;idx++){
    match=False;
    for(xtr_idx=0;xtr_idx<nbr_lst_b;xtr_idx++)
      if(!strcmp(xtr_lst[idx].nm,xtr_lst_b[xtr_idx].nm)){match=True;break;}
    if(match) continue;
    xtr_new_lst[xtr_nbr_new].nm=(char *)strdup(xtr_lst[idx].nm);
    xtr_new_lst[xtr_nbr_new++].id=xtr_lst[idx].id;
  } /* end loop over idx */
  /* realloc to actual size */
  xtr_new_lst=(nm_id_sct*)nco_realloc(xtr_new_lst,xtr_nbr_new*sizeof(nm_id_sct)); 
  /* free old list */
  xtr_lst=nco_nm_id_lst_free(xtr_lst,*nbr_xtr);

  *nbr_xtr=xtr_nbr_new;
  return xtr_new_lst;     
}/* end nco_var_lst_sub */

nm_id_sct *            /* O [sct] -- new list */
nco_var_lst_add(
nm_id_sct *xtr_lst,    /* I [sct] input list */ 
int *nbr_xtr,          /* I/O [ptr] -- size of xtr_lst & new output list */ 
nm_id_sct *xtr_lst_a,  /* I [sct] list of elemenst to be added to new list */
int nbr_lst_a)         /* I [nbr] size of xtr_lst_a */
{
  /* Purpose: Add to xtr_lst any elements from xtr_lst_a not already present and return new list */
  int idx;
  int xtr_idx;
  int nbr_xtr_crr;
  
  nm_id_sct *xtr_new_lst;
  
  nco_bool match;
  
  nbr_xtr_crr=*nbr_xtr;
  if(nbr_xtr_crr > 0){
    xtr_new_lst=(nm_id_sct*)nco_malloc((size_t)(*nbr_xtr)*sizeof(nm_id_sct));
    for(idx=0;idx<nbr_xtr_crr;idx++){
      xtr_new_lst[idx].nm=(char *)strdup(xtr_lst[idx].nm);
      xtr_new_lst[idx].id=xtr_lst[idx].id;
    } /* end loop over variables */
  }else{
    *nbr_xtr=nbr_lst_a;
    return nco_var_lst_copy(xtr_lst_a,nbr_lst_a);
  }/* end if */
  
  for(idx=0;idx<nbr_lst_a;idx++){
    match=False;
    for(xtr_idx=0;xtr_idx<*nbr_xtr;xtr_idx++)
      if(!strcmp(xtr_lst[xtr_idx].nm,xtr_lst_a[idx].nm)){match=True;break;}
    if(match) continue;
    xtr_new_lst=(nm_id_sct *)nco_realloc(xtr_new_lst,(size_t)(nbr_xtr_crr+1)*sizeof(nm_id_sct));
    xtr_new_lst[nbr_xtr_crr].nm=(char *)strdup(xtr_lst_a[idx].nm);
    xtr_new_lst[nbr_xtr_crr++].id=xtr_lst_a[idx].id;
  } /* end for */
  *nbr_xtr=nbr_xtr_crr;
  return xtr_new_lst;           
} /* end nco_var_lst_add */


nm_id_sct * /* O [sct] List of dimensions associated with input variable list */
nco_dmn_lst /* [fnc] Create list of all dimensions in file  */
(const int nc_id, /* I [id] netCDF input-file ID */
 int * const nbr_dmn) /* O [nbr] Number of dimensions in  list */
{
  int idx;
  int nbr_dmn_in;
  char dmn_nm[NC_MAX_NAME];
  nm_id_sct *dmn;
  /* Get number of dimensions */
  (void)nco_inq(nc_id,&nbr_dmn_in,(int *)NULL,(int *)NULL,(int *)NULL);
  
  dmn=(nm_id_sct *)nco_malloc(nbr_dmn_in*sizeof(nm_id_sct));
  
  for(idx=0;idx<nbr_dmn_in;idx++){
    (void)nco_inq_dimname(nc_id,idx,dmn_nm);
    dmn[idx].id=idx;
    dmn[idx].nm=(char *)strdup(dmn_nm);
  } /* end loop over dmn */
  
  *nbr_dmn=nbr_dmn_in;
  return dmn;
} /* end nco_dmn_lst() */

nm_id_sct *                /* O [sct] output list */ 
nco_att_lst_mk      
(const int in_id,         /* I [id] of input file */
 const int out_id,        /* I [id] id of output file */
 NcapVarVector &var_vtr,  /* I [vec] vector of vars & att */
 int *nbr_lst)            /* O [ptr] size of output list */
{
  int idx;
  int jdx;
  int rcd;
  int var_id;
  int size=0;
  char var_nm[NC_MAX_NAME];
  nm_id_sct *xtr_lst=NULL;  
  for(idx=0;idx<var_vtr.size();idx++){
    // Check for attribute
    if( var_vtr[idx]->type !=ncap_att) continue;
    (void)strcpy(var_nm, var_vtr[idx]->getVar().c_str());

    rcd=nco_inq_varid_flg(out_id,var_nm,&var_id);
    if(rcd== NC_NOERR) continue;   
    rcd=nco_inq_varid_flg(in_id,var_nm,&var_id);   
    if(rcd == NC_NOERR){
      /* eliminate any duplicates from list */
      for(jdx=0;jdx<size;jdx++)
	if(!strcmp(xtr_lst[jdx].nm,var_nm)) break;
      if(jdx!=size) continue;
      /* fxm mmr TODO 491: memory leak xtr_lst */
      xtr_lst=(nm_id_sct *)nco_realloc(xtr_lst,(size+1)*sizeof(nm_id_sct));
      xtr_lst[size].id=var_id;
      xtr_lst[size++].nm=(char *)strdup(var_nm);
    } /* end if */
  } /* end loop over att */
  
  *nbr_lst=size;
  
  return xtr_lst;
} /* end nco_att_lst_mk() */

nco_bool /* O [flg] Variables now conform */
ncap_var_stretch /* [fnc] Stretch variables */
(var_sct **var_1, /* I/O [ptr] First variable */
 var_sct **var_2) /* I/O [ptr] Second variable */
{
  /* Purpose: Make input variables conform or die
     var_1 and var_2 are considered completely symmetrically
     No assumption is made about var_1 relative to var_2
     Main difference betwee ncap_var_stretch() and nco_var_cnf_dmn() is
     If variables conform, then ncap_var_stretch() will broadcast
     If variables share no dimensions, then ncap_var_stretch() will convolve
     
     Terminology--- 
     Broadcast: Inflate smaller conforming variable to larger variable
     Conform: Dimensions of one variable are subset of other variable
     Convolve: Construct array whose rank is sum of non-duplicated dimensions
     Stretch: Union of broadcast and convolve
     
     Logic is pared down version of nco_var_cnf_dmn()
     1. USE_DUMMY_WGT has been eliminated: 
     ncap has no reason not to stretch input variables because grammar
     ensures only arithmetic variables will be stretched.
     
     2. wgt_crr has been eliminated:
     ncap never does anything multiple times so no equivalent to wgt_crr exists
     
     3. ncap_var_stretch(), unlike nco_var_cnf_dmn(), performs memory management
     Variables are var_free'd if they are superceded (replaced)
     
     4. Conformance logic is duplicated from nco_var_cnf_dmn()
     var_gtr plays role of var
     var_lsr plays role of wgt
     var_lsr_out plays role of wgt_out
     var_lsr_out=var_lsr only if variables already conform
     var_gtr_out is required since both variables may change
     var_gtr_out=var_gtr unless convolution is required */
  
  nco_bool CONFORMABLE=False; /* [flg] Whether var_lsr can be made to conform to var_gtr */
  nco_bool CONVOLVE=False; /* [flg] var_1 and var_2 had to be convolved */
  nco_bool DO_CONFORM; /* [flg] Did var_1 and var_2 conform? */
  nco_bool MUST_CONFORM=False; /* [flg] Must var_1 and var_2 conform? */
  
  int idx;
  int idx_dmn;
  int var_lsr_var_gtr_dmn_shr_nbr=0; /* [nbr] Number of dimensions shared by var_lsr and var_gtr */
  
  var_sct *var_gtr=NULL; /* [ptr] Pointer to variable structure of greater rank */
  var_sct *var_lsr=NULL; /* [ptr] Pointer to variable structure to lesser rank */
  var_sct *var_gtr_out=NULL; /* [ptr] Pointer to stretched version of greater rank variable */
  var_sct *var_lsr_out=NULL; /* [ptr] Pointer to stretched version of lesser rank variable */
  
  /* Initialize flag to false. Overwrite by true after successful conformance */
  DO_CONFORM=False;
  
  /* Determine which variable is greater and which lesser rank */
  if((*var_1)->nbr_dim >= (*var_2)->nbr_dim){
    var_gtr=*var_1;
    var_lsr=*var_2;
  }else{
    var_gtr=*var_2;
    var_lsr=*var_1;
  } /* endif */
  
  /* var_gtr_out=var_gtr unless convolution is required */
  var_gtr_out=var_gtr;
  
  /* Does lesser variable (var_lsr) conform to greater variable's dimensions? */
  if(var_lsr_out == NULL){
    if(var_gtr->nbr_dim > 0){
      /* Test that all dimensions in var_lsr appear in var_gtr */
      for(idx=0;idx<var_lsr->nbr_dim;idx++){
        for(idx_dmn=0;idx_dmn<var_gtr->nbr_dim;idx_dmn++){
	  /* Compare names, not dimension IDs */
	  if(!strcmp(var_lsr->dim[idx]->nm,var_gtr->dim[idx_dmn]->nm)){
	    var_lsr_var_gtr_dmn_shr_nbr++; /* var_lsr and var_gtr share this dimension */
	    break;
	  } /* endif */
        } /* end loop over var_gtr dimensions */
      } /* end loop over var_lsr dimensions */
      /* Decide whether var_lsr and var_gtr dimensions conform, are mutually exclusive, or are partially exclusive */ 
      if(var_lsr_var_gtr_dmn_shr_nbr == var_lsr->nbr_dim){
	/* var_lsr and var_gtr conform */
	/* fxm: Variables do not conform when dimension list of one is subset of other if order of dimensions differs, i.e., a(lat,lev,lon) !~ b(lon,lev) */
	CONFORMABLE=True;
      }else if(var_lsr_var_gtr_dmn_shr_nbr == 0){
	/* Dimensions in var_lsr and var_gtr are mutually exclusive */
	CONFORMABLE=False;
	if(MUST_CONFORM){
	  (void)fprintf(stdout,"%s: ERROR %s and template %s share no dimensions\n",prg_nm_get(),var_lsr->nm,var_gtr->nm);
	  nco_exit(EXIT_FAILURE);
	}else{
	  if(dbg_lvl_get() >= 1) (void)fprintf(stdout,"\n%s: DEBUG %s and %s share no dimensions: Attempting to convolve...\n",prg_nm_get(),var_lsr->nm,var_gtr->nm);
	  CONVOLVE=True;
	} /* endif */
      }else if(var_lsr_var_gtr_dmn_shr_nbr > 0 && var_lsr_var_gtr_dmn_shr_nbr < var_lsr->nbr_dim){
	/* Some, but not all, of var_lsr dimensions are in var_gtr */
	CONFORMABLE=False;
	if(MUST_CONFORM){
	  (void)fprintf(stdout,"%s: ERROR %d dimensions of %s belong to template %s but %d dimensions do not\n",prg_nm_get(),var_lsr_var_gtr_dmn_shr_nbr,var_lsr->nm,var_gtr->nm,var_lsr->nbr_dim-var_lsr_var_gtr_dmn_shr_nbr);
	  nco_exit(EXIT_FAILURE);
	}else{
	  if(dbg_lvl_get() >= 1) (void)fprintf(stdout,"\n%s: DEBUG %d dimensions of %s belong to template %s but %d dimensions do not: Not broadcasting %s to %s, could attempt stretching???\n",prg_nm_get(),var_lsr_var_gtr_dmn_shr_nbr,var_lsr->nm,var_gtr->nm,var_lsr->nbr_dim-var_lsr_var_gtr_dmn_shr_nbr,var_lsr->nm,var_gtr->nm);
	  CONVOLVE=True;
	} /* endif */
      } /* end if */
      if(CONFORMABLE){
	if(var_gtr->nbr_dim == var_lsr->nbr_dim){
	  /* var_gtr and var_lsr conform and are same rank */
	  /* Test whether all var_lsr and var_gtr dimensions match in sequence */
	  for(idx=0;idx<var_gtr->nbr_dim;idx++){
	    if(strcmp(var_lsr->dim[idx]->nm,var_gtr->dim[idx]->nm)) break;
	  } /* end loop over dimensions */
	  /* If so, take shortcut and copy var_lsr to var_lsr_out */
	  if(idx == var_gtr->nbr_dim) DO_CONFORM=True;
	}else{
	  /* var_gtr and var_lsr conform but are not same rank, set flag to proceed to generic conform routine */
	  DO_CONFORM=False;
	} /* end else */
      } /* endif CONFORMABLE */
    }else{ /* nbr_dmn == 0 */
      /* var_gtr is scalar, if var_lsr is also then set flag to copy var_lsr to var_lsr_out else proceed to generic conform routine */
      if(var_lsr->nbr_dim == 0) DO_CONFORM=True; else DO_CONFORM=False;
    } /* end else nbr_dmn == 0 */
    if(CONFORMABLE && DO_CONFORM){
      var_lsr_out=nco_var_dpl(var_lsr);
      (void)nco_xrf_var(var_lsr,var_lsr_out);
    } /* end if */
  } /* endif var_lsr_out == NULL */
  
  if(var_lsr_out == NULL && CONVOLVE){
    /* Convolve variables by returned stretched variables with minimum possible number of dimensions */
    int dmn_nbr; /* Number of dimensions in convolution */
    if(dbg_lvl_get() >= 1) (void)fprintf(stdout,"\n%s: WARNING Convolution not yet implemented, results of operation between %s and %s are unpredictable\n",prg_nm_get(),var_lsr->nm,var_gtr->nm);
    /* Dimensions in convolution are union of dimensions in variables */
    dmn_nbr=var_lsr->nbr_dim+var_gtr->nbr_dim-var_lsr_var_gtr_dmn_shr_nbr; /* Number of dimensions in convolution */
    dmn_nbr=dmn_nbr; /* CEWI: Avert compiler warning that variable is set but never used */
    /* fxm: these should go away soon */
    var_lsr_out=nco_var_dpl(var_lsr);
    var_gtr_out=nco_var_dpl(var_gtr);

    /* for(idx_dmn=0;idx_dmn<var_gtr->nbr_dim;idx_dmn++){;}
       if(var_lsr_var_gtr_dmn_shr_nbr == 0); else; */
    
    /* Free calling variables */
    var_lsr=nco_var_free(var_lsr);
    var_gtr=nco_var_free(var_gtr);
  } /* endif STRETCH */
  
  if(var_lsr_out == NULL){
    /* Expand lesser variable (var_lsr) to match size of greater variable */
    const char fnc_nm[]="ncap_var_stretch()"; /* [sng] Function name */
    char *var_lsr_cp;
    char *var_lsr_out_cp;
    
    int idx_var_lsr_var_gtr[NC_MAX_DIMS];
    int var_lsr_nbr_dim;
    int var_gtr_nbr_dmn_m1;
    
    long *var_gtr_cnt;
    long dmn_ss[NC_MAX_DIMS];
    long dmn_var_gtr_map[NC_MAX_DIMS];
    long dmn_var_lsr_map[NC_MAX_DIMS];
    long var_gtr_lmn;
    long var_lsr_lmn;
    long var_gtr_sz;
    
    size_t var_lsr_typ_sz;
    
    /* Copy main attributes of greater variable into lesser variable */
    var_lsr_out=nco_var_dpl(var_gtr);
    (void)nco_xrf_var(var_lsr,var_lsr_out);
    
    /* Modify elements of lesser variable array */
    var_lsr_out->nm=(char *)nco_free(var_lsr_out->nm);
    var_lsr_out->nm=(char *)strdup(var_lsr->nm);
    var_lsr_out->id=var_lsr->id;
    var_lsr_out->type=var_lsr->type;
    /* Added 20050323: 
       Not quite sure why, but var->val.vp may already have values here when LHS-casting
       Perform safety free to guard against memory leaks */
    var_lsr_out->val.vp=nco_free(var_lsr_out->val.vp);
    var_lsr_out->val.vp=(void *)nco_malloc_dbg(var_lsr_out->sz*nco_typ_lng(var_lsr_out->type),"Unable to malloc() value buffer in variable stretching",fnc_nm);
    var_lsr_cp=(char *)var_lsr->val.vp;
    var_lsr_out_cp=(char *)var_lsr_out->val.vp;
    var_lsr_typ_sz=nco_typ_lng(var_lsr_out->type);
    
    if(var_lsr_out->nbr_dim == 0){
      /* Variables are scalars, not arrays */
      (void)memcpy(var_lsr_out_cp,var_lsr_cp,var_lsr_typ_sz);
    }else if(var_lsr->nbr_dim == 0){
      /* Lesser-ranked input variable is scalar 
	 Expansion in this degenerate case needs no index juggling (reverse-mapping)
	 Code as special case to speed-up important applications of ncap
	 for synthetic file creation */
      var_gtr_sz=var_gtr->sz;
      for(var_gtr_lmn=0;var_gtr_lmn<var_gtr_sz;var_gtr_lmn++){
	(void)memcpy(var_lsr_out_cp+var_gtr_lmn*var_lsr_typ_sz,var_lsr_cp,var_lsr_typ_sz);
      } /* end loop over var_gtr_lmn */
    }else{
      /* Variables are arrays, not scalars */
      
      /* Create forward and reverse mappings from greater variable's dimensions to lesser variable's dimensions:
	 
      dmn_var_gtr_map[i] is number of elements between one value of i_th 
      dimension of greater variable and next value of i_th dimension, i.e., 
      number of elements in memory between indicial increments in i_th dimension. 
      This is computed as product of one (1) times size of all dimensions (if any) after i_th 
      dimension in greater variable.
      
      dmn_var_lsr_map[i] contains analogous information, except for lesser variable
      
      idx_var_lsr_var_gtr[i] contains index into greater variable's dimensions of i_th dimension of lesser variable
      idx_var_gtr_var_lsr[i] contains index into lesser variable's dimensions of i_th dimension of greater variable 
      
      Since lesser variable is a subset of greater variable, some elements of idx_var_gtr_var_lsr may be "empty", or unused
      
      Since mapping arrays (dmn_var_gtr_map and dmn_var_lsr_map) are ultimately used for a
      memcpy() operation, they could (read: should) be computed as byte offsets, not type offsets.
      This is why netCDF generic hyperslab routines (ncvarputg(), ncvargetg())
      request imap vector to specify offset (imap) vector in bytes. */
      for(idx=0;idx<var_lsr->nbr_dim;idx++){
	for(idx_dmn=0;idx_dmn<var_gtr->nbr_dim;idx_dmn++){
	  /* Compare names, not dimension IDs */
	  if(!strcmp(var_gtr->dim[idx_dmn]->nm,var_lsr->dim[idx]->nm)){
	    idx_var_lsr_var_gtr[idx]=idx_dmn;
	    /*	    idx_var_gtr_var_lsr[idx_dmn]=idx;*/
	    break;
	  } /* end if */
	  /* Sanity check */
	  if(idx_dmn == var_gtr->nbr_dim-1){
	    (void)fprintf(stdout,"%s: ERROR var_lsr %s has dimension %s but var_gtr %s does not deep in ncap_var_stretch()\n",prg_nm_get(),var_lsr->nm,var_lsr->dim[idx]->nm,var_gtr->nm);
	    nco_exit(EXIT_FAILURE);
	  } /* end if err */
	} /* end loop over greater variable dimensions */
      } /* end loop over lesser variable dimensions */
      
      /* Figure out map for each dimension of greater variable */
      for(idx=0;idx<var_gtr->nbr_dim;idx++) dmn_var_gtr_map[idx]=1L;
      for(idx=0;idx<var_gtr->nbr_dim-1;idx++)
	for(idx_dmn=idx+1;idx_dmn<var_gtr->nbr_dim;idx_dmn++)
	  dmn_var_gtr_map[idx]*=var_gtr->cnt[idx_dmn];
      
      /* Figure out map for each dimension of lesser variable */
      for(idx=0;idx<var_lsr->nbr_dim;idx++) dmn_var_lsr_map[idx]=1L;
      for(idx=0;idx<var_lsr->nbr_dim-1;idx++)
	for(idx_dmn=idx+1;idx_dmn<var_lsr->nbr_dim;idx_dmn++)
	  dmn_var_lsr_map[idx]*=var_lsr->cnt[idx_dmn];
      
      /* Define convenience variables to avoid repetitive indirect addressing */
      var_lsr_nbr_dim=var_lsr->nbr_dim;
      var_gtr_sz=var_gtr->sz;
      var_gtr_cnt=var_gtr->cnt;
      var_gtr_nbr_dmn_m1=var_gtr->nbr_dim-1;
      
      /* var_gtr_lmn is offset into 1-D array corresponding to N-D indices dmn_ss */
      for(var_gtr_lmn=0;var_gtr_lmn<var_gtr_sz;var_gtr_lmn++){
	dmn_ss[var_gtr_nbr_dmn_m1]=var_gtr_lmn%var_gtr_cnt[var_gtr_nbr_dmn_m1];
	for(idx=0;idx<var_gtr_nbr_dmn_m1;idx++){
	  dmn_ss[idx]=(long)(var_gtr_lmn/dmn_var_gtr_map[idx]);
	  dmn_ss[idx]%=var_gtr_cnt[idx];
	} /* end loop over dimensions */
	
	/* Map (shared) N-D array indices into 1-D index into original lesser variable data */
	var_lsr_lmn=0L;
	for(idx=0;idx<var_lsr_nbr_dim;idx++) var_lsr_lmn+=dmn_ss[idx_var_lsr_var_gtr[idx]]*dmn_var_lsr_map[idx];
	
	(void)memcpy(var_lsr_out_cp+var_gtr_lmn*var_lsr_typ_sz,var_lsr_cp+var_lsr_lmn*var_lsr_typ_sz,var_lsr_typ_sz);
	
      } /* end loop over var_gtr_lmn */
      
    } /* end if greater variable (and lesser variable) are arrays, not scalars */
    
    DO_CONFORM=True;
  } /* end if we had to broadcast lesser variable to fit greater variable */
  
  /* Place variables in original order
     Not necessary if variables are used in binary operations that are associative
     But do not want to require that assumption of calling routines */
  if((*var_1)->nbr_dim >= (*var_2)->nbr_dim){
    *var_1=var_gtr_out; /* [ptr] First variable */
    *var_2=var_lsr_out; /* [ptr] Second variable */
  }else{
    *var_1=var_lsr_out; /* [ptr] First variable */
    *var_2=var_gtr_out; /* [ptr] Second variable */
  } /* endif */
  
  /* Variables now conform */
  return DO_CONFORM;
} /* end ncap_var_stretch() */


// check if var is really an attribute
nco_bool ncap_var_is_att( var_sct *var) {
  if( strchr(var->nm,'@') !=NULL ) return true;
  return false;
}


nco_bool          /* O [flg] true if var has been stretched */
ncap_att_stretch  /* stretch a single valued attribute from 1 to sz */
(var_sct* var,    /* I/O [sct] variable */       
 long nw_sz)      /* I [nbr] new var size */
{

  size_t idx;
  size_t var_typ_sz;  
  void* vp;
  char *cp;

  if(var->sz == 1L && nw_sz >1){
    
    var_typ_sz=nco_typ_lng(var->type);
    vp=(void*)nco_malloc(nw_sz*var_typ_sz);
    for(idx=0 ; idx < nw_sz ;idx++){
      cp=(char*)vp + (ptrdiff_t)(idx*var_typ_sz);
      memcpy(cp,var->val.vp ,var_typ_sz);
    }
  
    var->val.vp=(void*)nco_free(var->val.vp);
    var->sz=nw_sz;
    var->val.vp=vp;
    return true;
  }
  
    return false; 

} /* end ncap_att_stretch */


#include "VarOp.hh" 

var_sct *         /* O [sct] Sum of input variables (var1+var2) */
ncap_var_var_op   /* [fnc] Add two variables */
(var_sct *var1,  /* I [sct] Input variable structure containing first operand */
 var_sct *var2,  /* I [sct] Input variable structure containing second operand */
 int op)        /* Operation +-% */
{ 
  nco_bool vb1;
  nco_bool vb2;


  static VarOp<short> Vs;
  static VarOp<nco_int> Vl;
  static VarOp<float> Vf;
  static VarOp<double> Vd;
  
  struct ncoParserTokenTypes TT;
  var_sct *var_ret;

  //If var2 is null then we are dealing with a unary function
  if( var2 == NULL) {

   switch (var1->type) {
    case NC_BYTE:
    /* Do nothing */
      break;
    case NC_CHAR:
    /* Do nothing */
      break;
    case NC_SHORT:
      var_ret=Vs.var_op(var1,op);
      break;
    case NC_INT:
      var_ret=Vl.var_op(var1,op);
      break;            
    case NC_FLOAT:
      var_ret=Vf.var_op(var1 ,op);
      break;
  case NC_DOUBLE:
      var_ret=Vd.var_op(var1, op);
      break;

  default:
    break;

   }
   return var_ret;
  }
  
  
  vb1 = ncap_var_is_att(var1);
  vb2 = ncap_var_is_att(var2);

  // var & var
  if( !vb1 && !vb2 ) { 
    (void)ncap_var_retype(var1,var2);
    (void)ncap_var_cnf_dmn(&var1,&var2);
    // var & att
  }else  if( !vb1 && vb2 ){ 
    var2=nco_var_cnf_typ(var1->type,var2);
    if(var1->sz > 1 && var2->sz==1)
      (void)ncap_var_cnf_dmn(&var1,&var2);
      
    if(var1->sz != var2->sz) {
       (void)fprintf(stderr,"%s: Cannot make variable:%s and attribute:%s conform. So connot perform atrithmetic operation\n",prg_nm_get(),var1->nm,var2->nm);
       nco_exit(EXIT_FAILURE);
	}
       
    // att & var
    }else if( vb1 && !vb2){
      var_sct *var_swp;
      ptr_unn val_swp;  // Used to swap values around

      var1=nco_var_cnf_typ(var2->type,var1);
     if(var2->sz > 1 && var1->sz==1)
      (void)ncap_var_cnf_dmn(&var1,&var2);
      
     if(var1->sz != var2->sz) {
       (void)fprintf(stderr,"%s: Cannot make variable:%s and attribute:%s conform. So connot perform atrithmetic operation\n",prg_nm_get(),var2->nm,var1->nm);
       nco_exit(EXIT_FAILURE);
     }
     // Swap values around in var1 and var2;   
     val_swp=var1->val;
     var1->val=var2->val;
     var2->val=val_swp;;
     // Swap names about 
     var_swp=var1;
     var1=var2;
     var2=var_swp;

     // att && att
    } else if (vb1 && vb2) {
      (void)ncap_var_retype(var1,var2);
     
      if( var1->sz ==1 && var2->sz >1) 
	(void)ncap_att_stretch(var1,var2->sz);
      else if(var1->sz >1 && var2->sz==1)
	(void)ncap_att_stretch(var2,var1->sz);

      // Crash out if atts not equal size
      if(var1->sz != var2->sz) {
       (void)fprintf(stderr,"%s: Cannot make attribute:%s and attribute:%s conform. So connot perform atrithmetic operation\n",prg_nm_get(),var2->nm,var1->nm);
       nco_exit(EXIT_FAILURE);
      }
             
    }

  switch (var1->type) {
    case NC_BYTE:
    /* Do nothing */
      break;
    case NC_CHAR:
    /* Do nothing */
      break;
    case NC_SHORT:
      var_ret=Vs.var_var_op(var1, var2,op);
      break;
    case NC_INT:
      var_ret=Vl.var_var_op(var1, var2,op);
      break;            
    case NC_FLOAT:
      var_ret=Vf.var_var_op(var1, var2,op);
      break;
  case NC_DOUBLE:
      var_ret=Vd.var_var_op(var1, var2,op);
      break;

  default:
    break;

  } 

  var2=nco_var_free(var2);
  return var_ret;
}
  
bool            /* O [flg] true if all var elemenst are true */
ncap_var_lgcl   /* [fnc] calculate a aggregate bool value from a variable */
(var_sct* var)  /* I [sct] input variable */
{
  int idx;
  int sz;
  nc_type type;
  bool bret=true;
  ptr_unn op1;
  
  // Convert to type SHORT
  var=nco_var_cnf_typ((nc_type)NC_SHORT,var);  

  type=NC_SHORT;
  sz = var->sz;
  op1=var->val;
  /* Typecast pointer to values before access */
  (void)cast_void_nctype(type,&op1);
  if(var->has_mss_val) (void)cast_void_nctype(type,&var->mss_val);


    if(!var->has_mss_val){
      for(idx=0;idx<sz;idx++) 
	if(!op1.sp[idx]) break;
    }else{
      const short mss_val_sht=*(var->mss_val.sp);
      for(idx=0;idx<sz;idx++) 
	if( !op1.sp[idx] &&  op1.sp[idx] !=mss_val_sht ) break; 
    }
    
  if(idx <sz) bret=false;

  if(var->has_mss_val) (void)cast_nctype_void(type,&var->mss_val);

  return bret;
}



var_sct*         /* O [sct] casting variable has its own private dims */ 
ncap_mk_cst(   /* [fnc] create casting var from a list of dims */
char **sbs_lst,  /* I [sng] Array of dimension subscripts */
int lst_nbr,     /* I [nbr] size of above list */  
prs_sct *prs_arg)
{

  static const char * const tpl_nm="Internally generated template";
  
  
  int dmn_nbr; /* [nbr] Number of dimensions */
  int idx; /* [idx] Counter */
  
  double val=1.0; /* [frc] Value of template */
  
  var_sct *var; /* [sct] Variable */
  
  dmn_sct **dmn; /* [dmn] Dimension structure list */
  dmn_sct *dmn_item;
  dmn_sct *dmn_new;
  dmn_nbr = lst_nbr;


  //  sbs_lst=lst_prs_2D(sbs_sng,sbs_dlm,&dmn_nbr); 

  dmn=(dmn_sct **)nco_malloc(dmn_nbr*sizeof(dmn_sct *));
  (void)nco_redef(prs_arg->out_id);
  for(idx=0;idx<dmn_nbr;idx++){
    
    // Search dmn_out_vtr for dimension
    dmn_item=prs_arg->ptr_dmn_out_vtr->find(sbs_lst[idx]);
    if(dmn_item != NULL){ 
      dmn[idx]=dmn_item;
      continue;
    }
    // Search dmn_in_vtr for dimension
    dmn_item=prs_arg->ptr_dmn_in_vtr->find(sbs_lst[idx]);
    // die if not in list
    if(dmn_item == NULL) {
      (void)fprintf(stderr,"Warning: Unrecognized dimension \"%s\" in LHS subscripts",sbs_lst[idx]);
      exit(1);
    }  
    dmn_new=nco_dmn_dpl(dmn_item);
    // Define in output file 
    (void)nco_dmn_dfn(prs_arg->fl_out,prs_arg->out_id,&dmn_new,1);
    // add to out list
    (void)prs_arg->ptr_dmn_out_vtr->push(dmn_new);
    (void)nco_dmn_xrf(dmn_new,dmn_item);
    dmn[idx]=dmn_new;
  }
  (void)nco_enddef(prs_arg->out_id);

  /* Check that un-limited dimension is first dimension */
  for(idx=1;idx<dmn_nbr;idx++)
    if(dmn[idx]->is_rec_dmn){
      (void)sprintf(ncap_err_sng,"Warning:\"%s\" is the record dimension. It can only be the first dimension in a list",dmn[idx]->nm);
   (void)fprintf(stderr,ncap_err_sng);
      goto end_LHS_sbs;                     
    } /* endif */

  /* Create template variable to cast all RHS expressions */
  var=(var_sct *)nco_malloc(sizeof(var_sct));
  
  /* Set defaults */
  (void)var_dfl_set(var); /* [fnc] Set defaults for each member of variable structure */
  /* Overwrite with LHS information */
  /* fxm mmr: memory leak with var->nm */
  var->nm=(char *)strdup(tpl_nm);
  var->type=NC_DOUBLE;
  var->nbr_dim=dmn_nbr;
  var->dim=dmn;
  /* Allocate space for dimension structures */
  if(var->nbr_dim > 0) var->dmn_id=(int *)nco_malloc(var->nbr_dim*sizeof(int)); else var->dmn_id=(int *)NULL;
  if(var->nbr_dim > 0) var->cnt=(long *)nco_malloc(var->nbr_dim*sizeof(long)); else var->cnt=(long *)NULL;
  if(var->nbr_dim > 0) var->srt=(long *)nco_malloc(var->nbr_dim*sizeof(long)); else var->srt=(long *)NULL;
  if(var->nbr_dim > 0) var->end=(long *)nco_malloc(var->nbr_dim*sizeof(long)); else var->end=(long *)NULL;
  if(var->nbr_dim > 0) var->srd=(long *)nco_malloc(var->nbr_dim*sizeof(long)); else var->srd=(long *)NULL;
  
  /* Defensive programming */
  var->sz=1L; 
  /* Attach LHS dimensions to variable */
  for(idx=0;idx<dmn_nbr;idx++){
    var->dim[idx]=dmn[idx];
    var->dmn_id[idx]=dmn[idx]->id;
    var->cnt[idx]=dmn[idx]->cnt;
    var->srt[idx]=dmn[idx]->srt;
    var->end[idx]=dmn[idx]->end;
    var->srd[idx]=dmn[idx]->srd;
    var->sz*=var->cnt[idx];
  } /* end loop over dim */


  /* don't initalize val in intial scan  */
  if(prs_arg->ntl_scn) {
    var->val.vp=(void*)NULL;
    goto end_var;
  }

  /* Allocate space for variable values 
     fxm: more efficient and safer to use nco_calloc() and not fill with values? */
  if((var->val.vp=(void *)nco_malloc_flg(var->sz*nco_typ_lng(var->type))) == NULL){
    (void)fprintf(stderr,"%s: ERROR Unable to malloc() %ld*%lu bytes for var_LHS() in lexer\n",prg_nm_get(),var->sz,(unsigned long)nco_typ_lng(var->type));
    nco_exit(EXIT_FAILURE); 
  } /* end if */
  
  /* Copy arbitrary value into variable 
     Placing a uniform value in variable should be unnecessary since variable
     is intended for use solely as dimensional template for nco_var_cnf_dmn() 
     Nevertheless, copy 1.0 into value for safety */
  { /* Change scope to define convenience variables which reduce de-referencing */
    long var_sz; /* [nbr] Number of elements in variable */
    size_t var_typ_sz; /* [B] Size of single element of variable */
    char *var_val_cp; /* [ptr] Pointer to values */
    
    var_sz=var->sz; /* [nbr] Number of elements in variable */
    var_typ_sz=nco_typ_lng(var->type);
    var_val_cp=(char *)var->val.vp;
    for(idx=0;idx<var_sz;idx++) (void)memcpy(var_val_cp+idx,(void *)(&val),var_typ_sz);
  } /* end scope */
 end_var:

   
  if(dbg_lvl_get() > 2) (void)fprintf(stderr,"%s: Lexer creating LHS cast template: Template var->nm %s, var->nbr_dim %d, var->sz %li\n",prg_nm_get(),var->nm,var->nbr_dim,var->sz);
  
  /* Free dimension list memory */

   
 end_LHS_sbs: /* Errors encountered during LHS processing jump to here */
  /* Return to default state, known as INITIAL state or 0 state LMB92 p. 43 */    

  return var;

} // end ncap_mk_cst


var_sct*
ncap_do_cst(
var_sct* var,
var_sct* var_cst,
bool bntlscn)
{

  var_sct* var_tmp;

  if(bntlscn) {
    var_tmp=nco_var_dpl(var_cst);
    var_tmp->id=var->id;
    var_tmp->nm=(char*)nco_free(var_tmp->nm);
    var_tmp->nm=strdup(var->nm);
    var_tmp->type=var->type;
    var_tmp->typ_dsk=var->typ_dsk;
    var_tmp->undefined=False;
    var_tmp->val.vp=(void*)NULL;
    var=nco_var_free(var);
    var=var_tmp;
  
  }else{

   /* User intends LHS to cast RHS to same dimensionality
      Stretch newly initialized variable to size of LHS template */
   var_tmp=var;
   (void)ncap_var_stretch(&var_tmp,&var_cst);
   if(var_tmp != var) { 
     var=nco_var_free(var); 
     var=var_tmp;
   }
  
  if(dbg_lvl_get() > 2) (void)fprintf(stderr,"%s: Stretching variable %s with LHS template: Template var->nm %s, var->nbr_dim %d, var->sz %li\n",prg_nm_get(),var->nm,var_cst->nm,var_cst->nbr_dim,var_cst->sz);
    
   var->undefined=False;
  }

return var;

}













