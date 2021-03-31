/* $Header$ */

/* Purpose: Group utilities */

/* Copyright (C) 1995--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

/* Testing:
   eos52nc4 ~/nco/data/in.he5 ~/in.nc4
   export HDF5_DISABLE_VERSION_CHECK=1
   ncdump -h ~/in.nc4
   ncks -D 1 -m ~/in.nc4
   ncks -O -D 3 -g HIRDLS -m ~/in.nc4 ~/foo.nc
   ncks -O -D 3 -m ~/in_grp.nc ~/foo.nc
   ncks -O -D 3 -v 'Q.?' ~/nco/data/in.nc ~/foo.nc */

#include "nco_grp_utl.h"  /* Group utilities */

void
nco_flg_set_grp_var_ass               /* [fnc] Set flags for groups and variables associated with matched object */
(const char * const grp_nm_fll,       /* I [sng] Full name of group */
 const nco_obj_typ obj_typ,           /* I [enm] Object type (group or variable) */
 trv_tbl_sct * const trv_tbl)         /* I/O [sct] GTT (Group Traversal Table) */
{
  /* Purpose: Set flags for groups and variables associated with matched object */  

  trv_sct trv_obj; /* [sct] Traversal table object */

  for(unsigned int obj_idx=0;obj_idx<trv_tbl->nbr;obj_idx++){
    /* Create shallow copy to avoid indirection */
    trv_obj=trv_tbl->lst[obj_idx];
    
    /* If group was user-specified, flag all variables in group */
    if(obj_typ == nco_obj_typ_grp && trv_obj.nco_typ == nco_obj_typ_var)
      if(!strcmp(grp_nm_fll,trv_obj.grp_nm_fll)) trv_tbl->lst[obj_idx].flg_vsg=True;

    /* If variable was user-specified, flag group containing variable */
    if(obj_typ == nco_obj_typ_var && trv_obj.nco_typ == nco_obj_typ_grp)
      if(!strcmp(grp_nm_fll,trv_obj.grp_nm_fll)) trv_tbl->lst[obj_idx].flg_gcv=True;

    /* Flag ancestor groups of all user-specified groups and variables */
    if(strstr(grp_nm_fll,trv_obj.grp_nm_fll)) trv_tbl->lst[obj_idx].flg_ncs=True;
  } /* end loop over obj_idx */
  
} /* end nco_flg_set_grp_var_ass() */

int                                   /* [rcd] Return code */
nco_inq_grps_full                     /* [fnc] Discover and return IDs of apex and all sub-groups */
(const int grp_id,                    /* I [ID] Apex group */
 int * const grp_nbr,                 /* O [nbr] Number of groups */
 int * const grp_ids)                 /* O [ID] Group IDs of children */
{
  /* Purpose: Discover and return IDs of apex and all sub-groups
     If grp_nbr is NULL, it is ignored 
     If grp_ids is NULL, it is ignored
     grp_ids must contain enough space to hold grp_nbr IDs */
  int rcd=NC_NOERR; /* [rcd] Return code */
  int grp_nbr_crr; /* [nbr] Number of groups counted so far */
  int grp_id_crr; /* [ID] Current group ID */

  grp_stk_sct *grp_stk=NULL_CEWI; /* [sct] Group stack pointer */
  
  /* Initialize variables that are incremented */
  grp_nbr_crr=0; /* [nbr] Number of groups counted (i.e., stored in grp_ids array) so far */

  /* Initialize and obtain group iterator */
  rcd+=nco_grp_stk_get(grp_id,&grp_stk);

  /* Find and return next group ID */
  rcd+=nco_grp_stk_nxt(grp_stk,&grp_id_crr);
  while(grp_id_crr != NCO_LST_GRP){
    /* Store last popped value into group ID array */
    if(grp_ids) grp_ids[grp_nbr_crr]=grp_id_crr; /* [ID] Group IDs of children */

    /* Increment counter */
    grp_nbr_crr++;

    /* Find and return next group ID */
    rcd+=nco_grp_stk_nxt(grp_stk,&grp_id_crr);
  } /* end while */
  if(grp_nbr) *grp_nbr=grp_nbr_crr; /* O [nbr] Number of groups */

  /* Free group iterator */
  nco_grp_itr_free(grp_stk);

  return rcd; /* [rcd] Return code */
} /* end nco_inq_grps_full() */

int                                   /* O [rcd] Return code */
nco_def_grp_full                      /* [fnc] Ensure all components of group path are defined */
(const int nc_id,                     /* I [ID] netCDF output-file ID */
 const char * const grp_nm_fll,       /* I [sng] Full group name */
 int * const grp_out_id)              /* O [ID] Deepest group ID */
{
  /* Purpose: Ensure all components of full path name exist and return ID of deepest group */

  char *grp_pth=NULL; /* [sng] Full group path */
  char *grp_pth_dpl=NULL; /* [sng] Full group path memory duplicate */
  char *sls_ptr; /* [sng] Pointer to slash */

  int grp_id_prn; /* [ID] Parent group ID */
  int rcd=NC_NOERR;

  /* Initialize defaults */
  *grp_out_id=nc_id;

  grp_pth_dpl=(char *)strdup(grp_nm_fll);
  grp_pth=grp_pth_dpl;

  /* No need to try to define root group */
  if(grp_pth[0] == '/') grp_pth++;

  /* Define everything necessary beneath root group */
  while(strlen(grp_pth)){
    /* Terminate path at next slash, if any */
    sls_ptr=strchr(grp_pth,'/');

    /* Replace slash by NUL */
    if(sls_ptr) *sls_ptr='\0';
    
    /* Identify parent group */
    grp_id_prn=*grp_out_id;
    
    /* If current group is not defined, define it */
    if(nco_inq_ncid_flg(grp_id_prn,grp_pth,grp_out_id)) nco_def_grp(grp_id_prn,grp_pth,grp_out_id);

    /* Point to next group, if any */
    if(sls_ptr) grp_pth=sls_ptr+1; else break;
  } /* end while */

  grp_pth_dpl=(char *)nco_free(grp_pth_dpl);
  return rcd;
} /* end nco_def_grp_full() */

void
nco_grp_itr_free                      /* [fnc] Free group iterator */
(grp_stk_sct * const grp_stk)         /* O [sct] Group stack pointer */
{
  /* Purpose: Free group iterator
     Call a function that hides the iterator implementation behind the API */
  nco_grp_stk_free(grp_stk);
} /* end nco_grp_itr_free() */

int                                   /* [rcd] Return code */
nco_grp_stk_get                       /* [fnc] Initialize and obtain group iterator */
(const int grp_id,                    /* I [ID] Apex group */
 grp_stk_sct ** const grp_stk)        /* O [sct] Group stack pointer */
{
  /* Purpose: Initialize and obtain group iterator
     Routine pushes apex group onto stack
     The "Apex group" is normally (though not necessarily) the netCDF file ID, 
     aka, root ID, and is the top group in the hierarchy.
     Returned iterator contains one valid group, the apex group,
     and the stack counter equals one */
  int rcd=NC_NOERR; /* [rcd] Return code */
  
  rcd+=nco_inq_grps(grp_id,(int *)NULL,(int *)NULL);

  /* These error codes would cause an abort in the netCDF wrapper layer anyway,
     so this condition is purely for defensive programming. */
  if(rcd != NC_EBADID && rcd != NC_EBADGRPID){
    *grp_stk=nco_grp_stk_ntl();

    /* [fnc] Push group ID onto stack */
    (void)nco_grp_stk_psh(*grp_stk,grp_id);
  } /* endif */

  return rcd; /* [rcd] Return code */
} /* end nco_grp_stk_get() */

int                                   /* [rcd] Return code */
nco_grp_stk_nxt                       /* [fnc] Find and return next group ID */
(grp_stk_sct * const grp_stk,         /* I/O [sct] Group stack pointer */
 int * const grp_id)                  /* O [ID] Group ID */
{
  /* Purpose: Find and return next group ID
     Next group ID is popped off stack 
     If this group contains sub-groups, these sub-groups are pushed onto stack before return
     Hence routine ensures every group is eventually examined for sub-groups */
  int *grp_ids; /* [ID] Group IDs of children */
  int idx;
  int grp_nbr; /* I [nbr] Number of sub-groups */
  int rcd=NC_NOERR; /* [rcd] Return code */
 
  /* Is stack empty? */
  if(grp_stk->grp_nbr == 0){
    /* Return flag showing iterator has reached the end, i.e., no more groups */
    *grp_id=NCO_LST_GRP;
  }else{
    /* Return current stack top */
    *grp_id=nco_grp_stk_pop(grp_stk);
    /* Replenish stack with sub-group IDs, if available */
    rcd+=nco_inq_grps(*grp_id,&grp_nbr,(int *)NULL);
    if(grp_nbr > 0){
      /* Add sub-groups of current stack top */
      grp_ids=(int *)nco_malloc(grp_nbr*sizeof(int));
      rcd+=nco_inq_grps(*grp_id,(int *)NULL,grp_ids);
      /* Push sub-group IDs in reverse order so when popped will come out in original order */
      for(idx=grp_nbr-1;idx>=0;idx--) (void)nco_grp_stk_psh(grp_stk,grp_ids[idx]);
      /* Clean up memory */
      grp_ids=(int *)nco_free(grp_ids);
    } /* endif sub-groups exist */
  } /* endelse */

  return rcd; /* [rcd] Return code */
} /* end nco_grp_stk_nxt() */

grp_stk_sct *                         /* O [sct] Group stack pointer */
nco_grp_stk_ntl                       /* [fnc] Initialize group stack */
(void)
{
  /* Purpose: Initialize dynamic array implementation of group stack */
  grp_stk_sct *grp_stk; /* O [sct] Group stack pointer */
  grp_stk=(grp_stk_sct *)nco_malloc(sizeof(grp_stk)); /* O [sct] Group stack pointer */
  grp_stk->grp_nbr=0; /* [nbr] Number of items in stack = number of elements in grp_id array */
  grp_stk->grp_id=NULL; /* [ID] Group ID */
  return grp_stk;/* O [sct] Group stack pointer */
} /* end nco_grp_stk_ntl() */

void
nco_grp_stk_psh                       /* [fnc] Push group ID onto stack */
(grp_stk_sct * const grp_stk,         /* I/O [sct] Group stack pointer */
 const int grp_id)                    /* I [ID] Group ID to push */
{
  /* Purpose: Push group ID onto dynamic array implementation of stack */
  grp_stk->grp_nbr++; /* [nbr] Number of items in stack = number of elements in grp_id array */
  grp_stk->grp_id=(int *)nco_realloc(grp_stk->grp_id,(grp_stk->grp_nbr)*sizeof(int)); /* O [sct] Pointer to group IDs */
  grp_stk->grp_id[grp_stk->grp_nbr-1]=grp_id; /* [ID] Group ID */
} /* end nco_grp_stk_psh() */

int /* O [ID] Group ID that was popped */
nco_grp_stk_pop /* [fnc] Remove and return group ID from stack */
(grp_stk_sct * const grp_stk) /* I/O [sct] Pointer to top of stack */
{
  /* Purpose: Remove and return group ID from dynamic array implementation of stack */
  int grp_id; /* [ID] Group ID that was popped */
  grp_id=grp_stk->grp_id[grp_stk->grp_nbr-1]; /* [ID] Group ID that was popped */

  if(grp_stk->grp_nbr == 0){
    (void)fprintf(stderr,"%s: ERROR nco_grp_stk_pop() asked to pop empty stack\n",nco_prg_nm_get());
    nco_exit(EXIT_FAILURE);
  } /* endif */
  grp_stk->grp_nbr--; /* [nbr] Number of items in stack = number of elements in grp_id array */
  grp_stk->grp_id=(int *)nco_realloc(grp_stk->grp_id,(grp_stk->grp_nbr)*sizeof(int)); /* O [sct] Pointer to group IDs */

  return grp_id; /* [ID] Group ID that was popped */
} /* end nco_grp_stk_pop() */

void
nco_grp_stk_free                      /* [fnc] Free group stack */
(grp_stk_sct * const grp_stk)         /* O [sct] Group stack pointer */
{
  /* Purpose: Free dynamic array implementation of stack */
  grp_stk->grp_id=(int *)nco_free(grp_stk->grp_id);
} /* end nco_grp_stk_free() */

void
nco_prn_grp_nm_fll                    /* [fnc] Debug function to print group full name from ID */
(const int grp_id)                    /* I [ID] Group ID */
{
#if defined(HAVE_NETCDF4_H)
  size_t grp_nm_lng;
  char *grp_nm_fll;
  (void)nco_inq_grpname_full(grp_id,&grp_nm_lng,NULL);
  grp_nm_fll=(char*)nco_malloc(grp_nm_lng+1L);
  (void)nco_inq_grpname_full(grp_id,&grp_nm_lng,grp_nm_fll);
  (void)fprintf(stdout,"%s",grp_nm_fll);
  grp_nm_fll=(char*)nco_free(grp_nm_fll);
#endif
} /* nco_prn_grp_nm_fll() */

int                                  /* [rcd] Return code */
nco_grp_dfn                          /* [fnc] Define groups in output file */
(const int out_id,                   /* I [ID] netCDF output-file ID */
 nm_id_sct *grp_xtr_lst,             /* [grp] Groups to be defined */
 const int grp_nbr)                  /* I [nbr] Number of groups to be defined */
{
  /* Purpose: Define groups in output file */
  int idx;
  int rcd=NC_NOERR; /* [rcd] Return code */
  int rcr_lvl=1; /* [nbr] Recursion level */

  if(nco_dbg_lvl_get() >= nco_dbg_scl) (void)fprintf(stderr,"%s: INFO nco_grp_dfn() reports file level = 0 parent group = / (root group) will have %d sub-group%s\n",nco_prg_nm_get(),grp_nbr,(grp_nbr == 1) ? "" : "s");

  /* For each (possibly user-specified) top-level group, define group and all subgroups */
  for(idx=0;idx<grp_nbr;idx++) 
    rcd+=nco_def_grp_rcr(grp_xtr_lst[idx].id,out_id,grp_xtr_lst[idx].nm,rcr_lvl);

  return rcd;
} /* end nco_grp_dfn() */

int                                   /* [rcd] Return code */
nco_def_grp_rcr                       /* [fnc] Define groups */
(const int in_id,                     /* I [ID] netCDF input-file ID */
 const int out_id,                    /* I [ID] netCDF output-file ID */
 const char * const prn_nm,           /* I [sng] Parent group name */
 const int rcr_lvl)                   /* I [nbr] Recursion level */
{
  /* Purpose: Recursively define parent group and all subgroups in output file */
  char grp_nm[NC_MAX_NAME+1];

  int idx;
  int grp_nbr; /* I [nbr] Number of sub-groups */
  int rcd=NC_NOERR; /* [rcd] Return code */
  int grp_in_ids[NC_MAX_DIMS]; /* [ID] Sub-group IDs in input file */ /* fxm: NC_MAX_GRPS? */
  int grp_out_ids[NC_MAX_DIMS]; /* [ID] Sub-group IDs in output file */

  /* How many and which sub-groups are in this group? */
  rcd+=nco_inq_grps(in_id,&grp_nbr,grp_in_ids);

  if(nco_dbg_lvl_get() >= nco_dbg_scl) (void)fprintf(stderr,"%s: INFO nco_def_grp_rcr() reports file level = %d parent group = %s will have %d sub-group%s\n",nco_prg_nm_get(),rcr_lvl,prn_nm,grp_nbr,(grp_nbr == 1) ? "" : "s");

  /* Define each group, recursively, in output file */
  for(idx=0;idx<grp_nbr;idx++){
    /* Obtain name of current group in input file */
    rcd+=nco_inq_grpname(grp_in_ids[idx],grp_nm);

    /* Define group of same name in output file */
    rcd+=nco_def_grp(out_id,grp_nm,grp_out_ids+idx);

    /* Define group and all sub-groups */
    rcd+=nco_def_grp_rcr(grp_in_ids[idx],grp_out_ids[idx],grp_nm,rcr_lvl+1);
  } /* end loop over sub-groups groups */

  return rcd;
} /* end nco_grp_dfn_rcr() */

int /* [nbr] Number of slash characters */
nco_get_sls_chr_cnt /* [fnc] Count slash characters in a string path */
(char * const nm_fll) /* I [sng] Full name  */
{
  const char sls_chr='/'; /* [chr] Slash character */
  char *ptr_chr; /* [sng] Pointer to character '/' in full name */
  int sls_nbr=0; /* [nbr] Number of of slash characterrs in  string path */
 
  ptr_chr=strchr(nm_fll,sls_chr);
  while(ptr_chr){
    ptr_chr=strchr(ptr_chr+1,sls_chr);
    sls_nbr++;
  } /* end while */

  return sls_nbr;
} /* nco_get_sls_chr_cnt() */

int
nco_get_sng_pth_sct                   /* [fnc] Get full name token structure (path components) */
(char * const nm_fll,                 /* I [sng] Full name  */ 
 sng_pth_sct ***sng_pth_lst)          /* I/O [sct] List of path components  */    
{
  /* Purpose: Break full path name into components separated by slash character (netCDF4 path separator) 
     Uses strtok() to split string into tokens, sequences of contiguous characters separated by specified delimiters
     Uses strchr() to get position of delimiter that separates tokens

  Use case: "/g16/g16g1/lon1"
  Token 0: g16
  Token 1: g16g1
  Token 2: lon1

  Usage:
  Get number of tokens in variable full name
  sls_nbr_var=nco_get_sls_chr_cnt(var_trv->nm_fll); 

  Allocation:
  sng_pth_lst_var=(sng_pth_sct **)nco_malloc(sls_nbr_var*sizeof(sng_pth_sct *)); 

  Get token list in variable full name:
  (void)nco_get_sng_pth_sct(var_trv->nm_fll,&sng_pth_lst_var); */

  char *ptr_chr;      /* [sng] Pointer to character '/' in full name */
  char *ptr_chr_tkn;  /* [sng] Pointer to character */
  int sls_nbr=0;  /* [nbr] Number of of slash characterrs in  string path */
  int psn_chr;        /* [nbr] Position of character '/' in in full name */
  const char sls_chr='/';   /* [chr] Slash character */
  const char sls_sng[]="/"; /* [sng] Slash string */
 
  /* Duplicate original, since strtok() changes it */
  char *sng=strdup(nm_fll);

  if(nco_dbg_lvl_get() == nco_dbg_old) (void)fprintf(stdout,"Splitting \"%s\" into tokens:\n",sng);

  /* Get first token */
  ptr_chr_tkn=strtok(sng,sls_sng);
  ptr_chr=strchr(nm_fll,sls_chr);

  while(ptr_chr){
    if(nco_dbg_lvl_get() == nco_dbg_old) (void)fprintf(stdout,"%s ",ptr_chr_tkn);

    psn_chr=ptr_chr-nm_fll;
    
    /* Store token and position */
    (*sng_pth_lst)[sls_nbr]=(sng_pth_sct *)nco_malloc(sizeof(sng_pth_sct));
    (*sng_pth_lst)[sls_nbr]->nm=strdup(ptr_chr_tkn);
    (*sng_pth_lst)[sls_nbr]->psn=psn_chr;

    /* Point where last token was found is kept internally by function */
    ptr_chr_tkn=strtok(NULL,sls_sng);
    ptr_chr=strchr(ptr_chr+1,sls_chr);
    sls_nbr++;   
  } /* end while */

  if(nco_dbg_lvl_get() == nco_dbg_old) (void)fprintf(stdout,"\n");

  if(sng) sng=(char *)nco_free(sng);

  return sls_nbr;
} /* nco_get_sls_chr_cnt() */

void 
nco_prn_att_trv /* [fnc] Traverse tree to print all group and global attributes */
(const int nc_id, /* I [id] netCDF file ID */
 const prn_fmt_sct * const prn_flg, /* I [sct] Print-format information */
 const trv_tbl_sct * const trv_tbl) /* I [sct] GTT (Group Traversal Table) */
{
  int grp_id;                 /* [ID] Group ID */
  int nbr_att;                /* [nbr] Number of attributes */
  int nbr_dmn;                /* [nbr] Number of dimensions */
  int nbr_var;                /* [nbr] Number of variables */

  for(unsigned idx_tbl=0;idx_tbl<trv_tbl->nbr;idx_tbl++){
    trv_sct trv=trv_tbl->lst[idx_tbl];
    if(trv.nco_typ == nco_obj_typ_grp && trv.flg_xtr){
      /* Obtain group ID */
      (void)nco_inq_grp_full_ncid(nc_id,trv.nm_fll,&grp_id);

      /* Obtain info for group */
      (void)nco_inq(grp_id,&nbr_dmn,&nbr_var,&nbr_att,(int *)NULL);

      /* List attributes using obtained group ID */
      if(nbr_att){
        if(trv.grp_dpt > 0) (void)fprintf(stdout,"Group %s attributes:\n",trv.nm_fll); else (void)fprintf(stdout,"Global attributes:\n"); 
        (void)nco_prn_att(grp_id,prn_flg,NC_GLOBAL); 
      } /* nbr_att */
    } /* end nco_obj_typ_grp */
  } /* end idx_tbl */
} /* end nco_prn_att_trv() */

int /* O [nbr] Number of matches to current rx */
nco_trv_rx_search /* [fnc] Search for pattern matches in traversal table */
(const char * const rx_sng, /* I [sng] Regular expression pattern */
 const nco_obj_typ obj_typ, /* I [enm] Object type (group or variable) */
 trv_tbl_sct * const trv_tbl) /* I/O [sct] Traversal table */
{
  /* Purpose: Set flags indicating whether each list member matches given regular expression
     NB: This function only writes True to match flag, it never writes False
     Input flags are assumed to be stateful, and may contain Trues from previous calls */

  int mch_nbr=0;

#ifndef NCO_HAVE_REGEX_FUNCTIONALITY
  (void)fprintf(stdout,"%s: ERROR: Sorry, wildcarding (extended regular expression matches to variables) was not built into this NCO executable, so unable to compile regular expression \"%s\".\nHINT: Make sure libregex.a is on path and re-build NCO.\n",nco_prg_nm_get(),rx_sng);
  nco_exit(EXIT_FAILURE);
  /* Avoid compiler warning of unused variables by using obj_typ and trv_tbl */
  if(trv_tbl->lst) trv_tbl->lst[0].nco_typ = obj_typ;
#else /* NCO_HAVE_REGEX_FUNCTIONALITY */
  const char fnc_nm[]="nco_trv_rx_search()"; /* [sng] Function name  */
  const char sls_chr='/'; /* [chr] Slash character */
  char *sng2mch; /* [sng] String to match to regular expression */

  int err_id;
  int flg_cmp; /* Comparison flags */
  int flg_exe; /* Execution flages */
  
  regex_t *rx;
  regmatch_t *result;

  size_t obj_idx;
  size_t rx_prn_sub_xpr_nbr;

  rx=(regex_t *)nco_malloc(sizeof(regex_t));

  /* Choose RE_SYNTAX_POSIX_EXTENDED regular expression type */
  flg_cmp=(REG_EXTENDED | REG_NEWLINE);
  /* Set execution flags */
  flg_exe=0;

  /* Compile regular expression */
  if((err_id=regcomp(rx,rx_sng,flg_cmp))){ /* Compile regular expression */
    char const * rx_err_sng;  
    /* POSIX regcomp return error codes */
    switch(err_id){
    case REG_BADPAT: rx_err_sng="Invalid pattern"; break;  
    case REG_ECOLLATE: rx_err_sng="Not implemented"; break;
    case REG_ECTYPE: rx_err_sng="Invalid character class name"; break;
    case REG_EESCAPE: rx_err_sng="Trailing backslash"; break;
    case REG_ESUBREG: rx_err_sng="Invalid back reference"; break;
    case REG_EBRACK: rx_err_sng="Unmatched left bracket"; break;
    case REG_EPAREN: rx_err_sng="Parenthesis imbalance"; break;
    case REG_EBRACE: rx_err_sng="Unmatched {"; break;
    case REG_BADBR: rx_err_sng="Invalid contents of { }"; break;
    case REG_ERANGE: rx_err_sng="Invalid range end"; break;
    case REG_ESPACE: rx_err_sng="Ran out of memory"; break;
    case REG_BADRPT: rx_err_sng="No preceding re for repetition op"; break;
    default: rx_err_sng="Invalid pattern"; break;  
    } /* end switch */
    (void)fprintf(stdout,"%s: ERROR %s error in regular expression \"%s\" %s\n",nco_prg_nm_get(),fnc_nm,rx_sng,rx_err_sng); 
    nco_exit(EXIT_FAILURE);
  } /* end if err */

  rx_prn_sub_xpr_nbr=rx->re_nsub+1L; /* Number of parenthesized sub-expressions */

  /* Search string */
  result=(regmatch_t *)nco_malloc(sizeof(regmatch_t)*rx_prn_sub_xpr_nbr);

  /* Check each object string for match to rx */
  for(obj_idx=0;obj_idx<trv_tbl->nbr;obj_idx++){  
    /* Check apples against apples and oranges against oranges */
    if(trv_tbl->lst[obj_idx].nco_typ == obj_typ){
      /* NB: Here is where match flag would be set to False if input were stateless */
      /* Regular expressions embedded in simple strings (without forward slashes) apply to stubs
	 Presence of slash indicates that regular expression applies to full pathname */
      if(strchr(rx_sng,sls_chr)) sng2mch=trv_tbl->lst[obj_idx].nm_fll; else sng2mch=trv_tbl->lst[obj_idx].nm; 
      if(!regexec(rx,sng2mch,rx_prn_sub_xpr_nbr,result,flg_exe)){
        trv_tbl->lst[obj_idx].flg_mch=True;
        mch_nbr++;
      } /* end if match */
    } /* end if obj_typ */
  } /* end loop over variables */

  regfree(rx); /* Free regular expression data structure */
  rx=(regex_t *)nco_free(rx);
  result=(regmatch_t *)nco_free(result);
#endif /* NCO_HAVE_REGEX_FUNCTIONALITY */

  return mch_nbr;
} /* end nco_trv_rx_search() */

nco_bool                                                         
nco_pth_mch                            /* [fnc] Name component in full path matches user string  */
(char * const nm_fll,                  /* I [sng] Full name (path) */
 char * const nm,                      /* I [sng] Name (relative) */
 char * const usr_sng)                 /* [sng] User-supplied object name */
{
  const char sls_chr='/';              /* [chr] Slash character */

  char *sbs_end;                       /* [sng] Location of user-string match end   in object path */
  char *sbs_srt;                       /* [sng] Location of user-string match start in object path */
  char *sbs_srt_nxt;                   /* [sng] String to search next for match */                     
  char *var_mch_srt;                   /* [sng] Location of variable short name in user-string */

  nco_bool flg_pth_end_bnd;            /* [flg] String ends   at path component boundary */
  nco_bool flg_pth_srt_bnd;            /* [flg] String begins at path component boundary */
  nco_bool flg_var_cnd;                /* [flg] Match meets addition condition(s) for variable */

  size_t usr_sng_lng;                  /* [nbr] Length of user-supplied string */
  size_t nm_fll_lng;                   /* [sng] Length of full name */
  size_t nm_lng;                       /* [sng] Length of relative name */

  /* Initialize (combined return) flags */
  flg_pth_srt_bnd=flg_pth_end_bnd=flg_var_cnd=False;

  /* Get lenghts */
  nm_fll_lng=strlen(nm_fll);
  nm_lng=strlen(nm);
  usr_sng_lng=strlen(usr_sng);

  /* Look for partial match, not necessarily on path boundaries */
  /* 20130829: Variables and group names may be proper subsets of ancestor group names
     e.g., variable named g9 in group named g90 is /g90/g9
     e.g., group named g1 in group named g10 is g10/g1
     Search algorithm must test same full name multiple times in such cases
     For variables, only final match (closest to end full name) need be fully tested */
  sbs_srt=NULL;
  sbs_srt_nxt=nm_fll;
  while((sbs_srt_nxt=strstr(sbs_srt_nxt,usr_sng))){
    /* Object name contains usr_sng at least once */
    /* Complete path-check below will begin at this substring ... */
    sbs_srt=sbs_srt_nxt; 

    /* ...and also here for variables unless match is found in next iteration after advancing substring... */
    if(sbs_srt_nxt+usr_sng_lng <= nm_fll+nm_fll_lng) sbs_srt_nxt+=usr_sng_lng; else break;
  } /* end while */

  /* Does object name contain usr_sng? */
  if(sbs_srt){
    /* Ensure match spans (begins and ends on) whole path-component boundaries
       Full path-check starts at current substring */

    /* Does match begin at path component boundary ... directly on a slash? */
    if(*sbs_srt == sls_chr) flg_pth_srt_bnd=True;

    /* ...or one after a component boundary? */
    if((sbs_srt > nm_fll) && (*(sbs_srt-1L) == sls_chr)) flg_pth_srt_bnd=True;

    /* Does match end at path component boundary ... directly on a slash? */
    sbs_end=sbs_srt+usr_sng_lng-1L;

    if(*sbs_end == sls_chr) flg_pth_end_bnd=True;

    /* ...or one before a component boundary? */
    if(sbs_end <= nm_fll+nm_fll_lng-1L)
      if((*(sbs_end+1L) == sls_chr) || (*(sbs_end+1L) == '\0'))
        flg_pth_end_bnd=True;

    /* Additional condition is user-supplied string must end with short form of name */
    if(nm_lng <= usr_sng_lng){
      var_mch_srt=usr_sng+usr_sng_lng-nm_lng;
      if(!strcmp(var_mch_srt,nm)) flg_var_cnd=True;
    } /* endif */     
  } /* !sbs_srt */

  /* Must meet necessary flags  */
  if(flg_pth_srt_bnd && flg_pth_end_bnd && flg_var_cnd) return True;

  return False;

} /* nco_pth_mch() */

nco_bool                              /* O [flg] All names are in file */
nco_xtr_mk                            /* [fnc] Check -v and -g input names and create extraction list */
(char ** grp_lst_in,                  /* I [sng] User-specified list of groups */
 const int grp_xtr_nbr,               /* I [nbr] Number of groups in list */
 char ** var_lst_in,                  /* I [sng] User-specified list of variables */
 const int var_xtr_nbr,               /* I [nbr] Number of variables in list */
 const nco_bool EXCLUDE_INPUT_LIST,   /* I [flg] Exclude rather than extract groups and variables */
 const nco_bool EXTRACT_ALL_COORDINATES, /* I [flg] Process all coordinates */ 
 const nco_bool flg_unn,              /* I [flg] Select union of specified groups and variables */
 trv_tbl_sct * const trv_tbl)         /* I/O [sct] GTT (Group Traversal Table) */
{
  /* Purpose: Verify all user-specified objects exist in file and create extraction list
     Verify both variables and groups
     Handle regular expressions 
     Set flags in traversal table to help generate extraction list
     Unmatched exclusion lists should succeed

     Tests:
     ncks -O -D 5 -g / ~/nco/data/in_grp.nc ~/foo.nc
     ncks -O -D 5 -g '' ~/nco/data/in_grp.nc ~/foo.nc
     ncks -O -D 5 -g g1 ~/nco/data/in_grp.nc ~/foo.nc
     ncks -O -D 5 -g /g1 ~/nco/data/in_grp.nc ~/foo.nc
     ncks -O -D 5 -g /g1/g1 ~/nco/data/in_grp.nc ~/foo.nc
     ncks -O -D 5 -g g1.+ ~/nco/data/in_grp.nc ~/foo.nc
     ncks -O -D 5 -v v1 ~/nco/data/in_grp.nc ~/foo.nc
     ncks -O -D 5 -x -v lat2 ~/nco/data/in_grp.nc ~/foo.nc
     ncks -O -D 5 -g g1.+ -v v1,sc. ~/nco/data/in_grp.nc ~/foo.nc
     ncks -O -D 5 -v scl,/g1/g1g1/v1 ~/nco/data/in_grp.nc ~/foo.nc
     ncks -O -D 5 -g g3g.+,g9/ -v scl,/g1/g1g1/v1 ~/nco/data/in_grp.nc ~/foo.nc */

  const char fnc_nm[]="nco_xtr_mk()"; /* [sng] Function name */
  const char sls_chr='/'; /* [chr] Slash character */
  
  char **obj_lst_in; /* [sng] User-specified list of objects */

  char *sbs_end; /* [sng] Location of user-string match end   in object path */
  char *sbs_srt; /* [sng] Location of user-string match start in object path */
  char *sbs_srt_nxt; /* [sng] String to search next for match */
  char *usr_sng; /* [sng] User-supplied object name */
  char *var_mch_srt; /* [sng] Location of variable short name in user-string */
  
  int obj_nbr; /* [nbr] Number of objects in list */

#ifdef NCO_HAVE_REGEX_FUNCTIONALITY
  int rx_mch_nbr;
#endif /* !NCO_HAVE_REGEX_FUNCTIONALITY */

  nco_bool flg_ncr_mch_crr; /* [flg] Current group meets anchoring properties of this user-supplied string */
  nco_bool flg_ncr_mch_grp; /* [flg] User-supplied string anchors at root */
  nco_bool flg_pth_end_bnd; /* [flg] String ends   at path component boundary */
  nco_bool flg_pth_srt_bnd; /* [flg] String begins at path component boundary */
  nco_bool flg_rcr_mch_crr; /* [flg] Current group meets recursion criteria of this user-supplied string */
  nco_bool flg_rcr_mch_grp; /* [flg] User-supplied string will match groups recursively */
  nco_bool flg_unn_ffc; /* [flg] Union or Effective Union (not intersection) */
  nco_bool flg_usr_mch_obj; /* [flg] One or more objects match each user-supplied string */
  nco_bool flg_var_cnd; /* [flg] Match meets addition condition(s) for variable */

  nco_obj_typ obj_typ; /* [enm] Object type (group or variable) */
   
  size_t usr_sng_lng; /* [nbr] Length of user-supplied string */

  trv_sct grp_obj; /* [sct] Traversal table object assumed to be group */
  trv_sct trv_obj; /* [sct] Traversal table object */
  trv_sct var_obj; /* [sct] Traversal table object assumed to be variable */

  if(nco_dbg_lvl_get() == nco_dbg_old) (void)fprintf(stdout,"%s: INFO %s Extraction list will be formed as %s of group and variable specifications if both are given\n",nco_prg_nm_get(),fnc_nm,(flg_unn) ? "union" : "intersection");

  /* Specifying no groups or variables is equivalent to requesting all */
  if(!grp_xtr_nbr && !var_xtr_nbr && !EXTRACT_ALL_COORDINATES)
    for(unsigned int obj_idx=0;obj_idx<trv_tbl->nbr;obj_idx++) trv_tbl->lst[obj_idx].flg_dfl=True;

  for(unsigned int itr_idx=0;itr_idx<2;itr_idx++){
    
    /* Set object type (group or variable) */
    if(itr_idx == 0){
      obj_typ=nco_obj_typ_grp;
      obj_lst_in=(char **)grp_lst_in;
      obj_nbr=grp_xtr_nbr;
    }else if(itr_idx == 1){
      obj_typ=nco_obj_typ_var;
      obj_lst_in=(char **)var_lst_in;
      obj_nbr=var_xtr_nbr;
    } /* endelse */
    
    for(int obj_idx=0;obj_idx<obj_nbr;obj_idx++){
      
      /* Initialize state for current user-specified string */
      flg_ncr_mch_grp=False;
      flg_rcr_mch_grp=True;
      flg_usr_mch_obj=False;

      if(!obj_lst_in[obj_idx]){
        (void)fprintf(stderr,"%s: ERROR %s reports user-supplied %s name is empty. HINT: Eliminate consecutive or trailing commas from the extraction list.\n",nco_prg_nm_get(),fnc_nm,(obj_typ == nco_obj_typ_grp) ? "group" : "variable");
        nco_exit(EXIT_FAILURE);
      } /* end if */

      usr_sng=strdup(obj_lst_in[obj_idx]); 
      usr_sng_lng=strlen(usr_sng);

      /* Turn-off recursion for groups? */
      if(obj_typ == nco_obj_typ_grp){
        if(usr_sng_lng > 1L && usr_sng[usr_sng_lng-1L] == sls_chr){
          /* Remove trailing slash for subsequent searches since canonical group names do not end with slash */
          flg_rcr_mch_grp=False;
          usr_sng[usr_sng_lng-1L]='\0';
          usr_sng_lng--;
        } /* flg_rcr_mch_grp */
      } /* !nco_obj_typ_grp */
      
      /* Turn-on root-anchoring for groups? */
      if(obj_typ == nco_obj_typ_grp)
	if(usr_sng[0L] == sls_chr)
	  flg_ncr_mch_grp=True;

      /* Convert pound signs (back) to commas */
      nco_hash2comma(usr_sng);

      /* If usr_sng is regular expression ... */
      if(strpbrk(usr_sng,".*^$\\[]()<>+?|{}")){
	/* ... and regular expression library is present */
#ifdef NCO_HAVE_REGEX_FUNCTIONALITY
	if((rx_mch_nbr=nco_trv_rx_search(usr_sng,obj_typ,trv_tbl))) flg_usr_mch_obj=True;
	if(!rx_mch_nbr) (void)fprintf(stdout,"%s: WARNING: Regular expression \"%s\" does not match any %s\nHINT: See regular expression syntax examples at http://nco.sf.net/nco.html#rx\n",nco_prg_nm_get(),usr_sng,(obj_typ == nco_obj_typ_grp) ? "group" : "variable"); 
	continue;
#else /* !NCO_HAVE_REGEX_FUNCTIONALITY */
	(void)fprintf(stdout,"%s: ERROR: Sorry, wildcarding (extended regular expression matches to variables) was not built into this NCO executable, so unable to compile regular expression \"%s\".\nHINT: Make sure libregex.a is on path and re-build NCO.\n",nco_prg_nm_get(),usr_sng);
	nco_exit(EXIT_FAILURE);
#endif /* !NCO_HAVE_REGEX_FUNCTIONALITY */
      } /* end if regular expression */
      
        /* usr_sng is not rx, so manually search for multicomponent matches */
      for(unsigned int tbl_idx=0;tbl_idx<trv_tbl->nbr;tbl_idx++){
	
	/* Create shallow copy to avoid indirection */
	trv_obj=trv_tbl->lst[tbl_idx];
	
	if(trv_obj.nco_typ == obj_typ){
	  
	  /* Initialize defaults for current candidate path to match */
	  flg_pth_srt_bnd=False;
	  flg_pth_end_bnd=False;
	  flg_var_cnd=False;
	  flg_rcr_mch_crr=True;
	  flg_ncr_mch_crr=True;
	  
	  /* Look for partial match, not necessarily on path boundaries */
	  /* 20130829: Variables and group names may be proper subsets of ancestor group names
	     e.g., variable named g9 in group named g90 is /g90/g9
	     e.g., group named g1 in group named g10 is g10/g1
	     Search algorithm must test same full name multiple times in such cases
	     For variables, only final match (closest to end full name) need be fully tested */
	  sbs_srt=NULL;
	  sbs_srt_nxt=trv_obj.nm_fll;
	  while((sbs_srt_nxt=strstr(sbs_srt_nxt,usr_sng))){
	    /* Object name contains usr_sng at least once */
	    /* Complete path-check below will begin at this substring ... */
	    sbs_srt=sbs_srt_nxt; 
	    /* ...for groups always at first occurence of substring... */
	    if(obj_typ == nco_obj_typ_grp) break;
	    /* ...and also here for variables unless match is found in next iteration after advancing substring... */
	    if(sbs_srt_nxt+usr_sng_lng <= trv_obj.nm_fll+trv_obj.nm_fll_lng) sbs_srt_nxt+=usr_sng_lng; else break;
	  } /* end while */
	  
            /* Does object name contain usr_sng? */
	  if(sbs_srt){
	    /* Ensure match spans (begins and ends on) whole path-component boundaries
	       Full path-check starts at current substring */
	    
	    /* Does match begin at path component boundary ... directly on a slash? */
	    if(*sbs_srt == sls_chr) flg_pth_srt_bnd=True;
	    
	    /* ...or one after a component boundary? */
	    if((sbs_srt > trv_obj.nm_fll) && (*(sbs_srt-1L) == sls_chr)) flg_pth_srt_bnd=True;
	    
	    /* Does match end at path component boundary ... directly on a slash? */
	    sbs_end=sbs_srt+usr_sng_lng-1L;
	    
	    if(*sbs_end == sls_chr) flg_pth_end_bnd=True;
	    
	    /* ...or one before a component boundary? */
	    if(sbs_end <= trv_obj.nm_fll+trv_obj.nm_fll_lng-1L)
	      if((*(sbs_end+1L) == sls_chr) || (*(sbs_end+1L) == '\0'))
		flg_pth_end_bnd=True;
	    
	    /* Additional condition for variables is user-supplied string must end with short form of variable name */
	    if(obj_typ == nco_obj_typ_var){
	      if(trv_obj.nm_lng <= usr_sng_lng){
		var_mch_srt=usr_sng+usr_sng_lng-trv_obj.nm_lng;
		if(!strcmp(var_mch_srt,trv_obj.nm)) flg_var_cnd=True;
	      } /* endif */
	      if(nco_dbg_lvl_get() >= nco_dbg_sbr && nco_dbg_lvl_get() != nco_dbg_dev) (void)fprintf(stderr,"%s: INFO %s reports variable %s %s additional conditions for variable match with %s.\n",nco_prg_nm_get(),fnc_nm,usr_sng,(flg_var_cnd) ? "meets" : "fails",trv_obj.nm_fll);
	    } /* endif var */
	    
              /* If anchoring, match must begin at root */
	    if(flg_ncr_mch_grp && *sbs_srt != sls_chr) flg_ncr_mch_crr=False;
	    
	    /* If no recursion, match must terminate user-supplied string */
	    if(!flg_rcr_mch_grp && *(sbs_end+1L)) flg_rcr_mch_crr=False;
	    
	    /* Set traversal table flags */
	    if(obj_typ == nco_obj_typ_var){
	      /* Variables must meet necessary flags for variables */
	      if(flg_pth_srt_bnd && flg_pth_end_bnd && flg_var_cnd){
		trv_tbl->lst[tbl_idx].flg_mch=True;
		trv_tbl->lst[tbl_idx].flg_rcr=False;
		/* Was matched variable specified as full path (i.e., beginning with slash?) */
		if(*usr_sng == sls_chr) trv_tbl->lst[tbl_idx].flg_vfp=True;
	      } /* end flags */
	    }else if (obj_typ == nco_obj_typ_grp){ /* !nco_obj_typ_var */
	      /* Groups must meet necessary flags for groups */
	      if(flg_pth_srt_bnd && flg_pth_end_bnd && flg_ncr_mch_crr && flg_rcr_mch_crr){
		trv_tbl->lst[tbl_idx].flg_mch=True;
		trv_tbl->lst[tbl_idx].flg_rcr=flg_rcr_mch_grp;
	      } /* end flags */
	    } /* !nco_obj_typ_var */
	    
              /* Set function return condition */
	    if(trv_tbl->lst[tbl_idx].flg_mch) flg_usr_mch_obj=True;
	    
	    if(nco_dbg_lvl_get() == nco_dbg_old){
	      /* Redundant call to nco_flg_set_grp_var_ass() here in debugging mode only to set flags for following print statements 
		 Essential call to nco_flg_set_grp_var_ass() occurs after itr loop
		 Most debugging info is available in debugging section at routine end
		 However, group boundary/anchoring/recursion info is only available here */
	      if(trv_tbl->lst[tbl_idx].flg_mch) nco_flg_set_grp_var_ass(trv_obj.grp_nm_fll,obj_typ,trv_tbl);
	      (void)fprintf(stderr,"%s: INFO %s reports %s %s matches filepath %s. Begins on boundary? %s. Ends on boundary? %s. Extract? %s.",nco_prg_nm_get(),fnc_nm,(obj_typ == nco_obj_typ_grp) ? "group" : "variable",usr_sng,trv_obj.nm_fll,(flg_pth_srt_bnd) ? "Yes" : "No",(flg_pth_end_bnd) ? "Yes" : "No",(trv_tbl->lst[tbl_idx].flg_mch) ?  "Yes" : "No");
	      if(obj_typ == nco_obj_typ_grp) (void)fprintf(stderr," Anchored? %s.",(flg_ncr_mch_grp) ? "Yes" : "No");
	      if(obj_typ == nco_obj_typ_grp) (void)fprintf(stderr," Recursive? %s.",(trv_tbl->lst[tbl_idx].flg_rcr) ? "Yes" : "No");
	      if(obj_typ == nco_obj_typ_grp) (void)fprintf(stderr," flg_gcv? %s.",(trv_tbl->lst[tbl_idx].flg_gcv) ? "Yes" : "No");
	      if(obj_typ == nco_obj_typ_grp) (void)fprintf(stderr," flg_ncs? %s.",(trv_tbl->lst[tbl_idx].flg_ncs) ? "Yes" : "No");
	      if(obj_typ == nco_obj_typ_var) (void)fprintf(stderr," flg_vfp? %s.",(trv_tbl->lst[tbl_idx].flg_vfp) ? "Yes" : "No");
	      if(obj_typ == nco_obj_typ_var) (void)fprintf(stderr," flg_vsg? %s.",(trv_tbl->lst[tbl_idx].flg_vsg) ? "Yes" : "No");
	      (void)fprintf(stderr,"\n");
	    } /* end if dbg */
	    
	  } /* endif strstr() */
	} /* endif nco_obj_typ */
      } /* end loop over tbl_idx */
      
      if(!flg_usr_mch_obj && !EXCLUDE_INPUT_LIST){
	(void)fprintf(stderr,"%s: ERROR %s reports user-supplied %s name or regular expression \'%s\' is not in and/or does not match contents of input file\n",nco_prg_nm_get(),fnc_nm,(obj_typ == nco_obj_typ_grp) ? "group" : "variable",usr_sng);
	nco_exit(EXIT_FAILURE);
      } /* flg_usr_mch_obj */
        /* Free dynamic memory */
      if(usr_sng) usr_sng=(char *)nco_free(usr_sng);
      
    } /* obj_idx */
    
    if(nco_dbg_lvl_get() >= nco_dbg_sbr && nco_dbg_lvl_get() != nco_dbg_dev){
      (void)fprintf(stdout,"%s: INFO %s reports following %s match sub-setting and regular expressions:\n",nco_prg_nm_get(),fnc_nm,(obj_typ == nco_obj_typ_grp) ? "groups" : "variables");
      trv_tbl_prn_flg_mch(trv_tbl,obj_typ);
    } /* endif dbg */
    
  } /* itr_idx */
  
  /* Compute intersection of groups and variables if necessary
     Intersection criteria flag, flg_nsx, is satisfied by default. Unset later when necessary. */
  for(unsigned int obj_idx=0;obj_idx<trv_tbl->nbr;obj_idx++) trv_tbl->lst[obj_idx].flg_nsx=True;
  
  /* Each object with flg_mch set needs to have its associated objects set
     An object (group or variable) may have had its flg_mch set multiple times, e.g., once via rx and once via explicit listing
     And since rx's often match multiple objects, no sense in flagging associated objects inside rx loop
     Now all -g and -v constraints have been evaluated (after itr loop)
     Now speed through table once and set associated objects for all groups and variables flagged with flg_mch */
  for(unsigned int obj_idx=0;obj_idx<trv_tbl->nbr;obj_idx++)
    if(trv_tbl->lst[obj_idx].flg_mch) nco_flg_set_grp_var_ass(trv_tbl->lst[obj_idx].grp_nm_fll,trv_tbl->lst[obj_idx].nco_typ,trv_tbl);
  
  /* Union is same as intersection if either variable or group list is empty, otherwise check intersection criteria */
  if(flg_unn || !grp_xtr_nbr || !var_xtr_nbr) flg_unn_ffc=True; else flg_unn_ffc=False;
  if(!flg_unn_ffc){
    for(unsigned int obj_idx=0;obj_idx<trv_tbl->nbr;obj_idx++){
      var_obj=trv_tbl->lst[obj_idx];
      if(var_obj.nco_typ == nco_obj_typ_var){
        /* Cancel (non-full-path) variable intersection match unless variable is also in user-specified group */
        if(var_obj.flg_mch && !var_obj.flg_vfp){
	  grp_obj=trv_tbl->lst[0]; /* CEWI */
          for(unsigned int obj2_idx=0;obj2_idx<trv_tbl->nbr;obj2_idx++){
            grp_obj=trv_tbl->lst[obj2_idx];
            if(grp_obj.nco_typ == nco_obj_typ_grp && !strcmp(var_obj.grp_nm_fll,grp_obj.grp_nm_fll)) break;
          } /* end loop over obj2_idx */
          if(!grp_obj.flg_mch) trv_tbl->lst[obj_idx].flg_nsx=False;
        } /* flg_mch && flg_vfp */
      } /* nco_obj_typ_grp */
    } /* end loop over obj_idx */
  } /* flg_unn */
  
  /* Does matched or default group contain only metadata? 
     Flag used in nco_xtr_grp_mrk() to preserve metadata-only groups on extraction list */
  for(unsigned int obj_idx=0;obj_idx<trv_tbl->nbr;obj_idx++)
    if(trv_tbl->lst[obj_idx].nco_typ == nco_obj_typ_grp)
      if(trv_tbl->lst[obj_idx].flg_mch || trv_tbl->lst[obj_idx].flg_dfl)
        if(!trv_tbl->lst[obj_idx].nbr_var) 
          trv_tbl->lst[obj_idx].flg_mtd=True;

  /* Combine previous flags into initial extraction flag */
  for(unsigned int obj_idx=0;obj_idx<trv_tbl->nbr;obj_idx++){
    /* Extract object if ... */
    if(
       /* No subsetting */
       (trv_tbl->lst[obj_idx].flg_dfl) || /* ...user-specified no sub-setting... */
       /* Intersection mode (default mode) */
       (!flg_unn_ffc && trv_tbl->lst[obj_idx].flg_mch && trv_tbl->lst[obj_idx].flg_nsx) || /* ...intersection mode variable matches group... */
       /* Union mode */
       (flg_unn_ffc && trv_tbl->lst[obj_idx].flg_mch) || /* ...union mode object matches user-specified string... */
       (flg_unn_ffc && trv_tbl->lst[obj_idx].flg_vsg) || /* ...union mode variable selected because group matches... */
       (flg_unn_ffc && trv_tbl->lst[obj_idx].flg_gcv) || /* ...union mode group contains matched variable... */
       False) 
      trv_tbl->lst[obj_idx].flg_xtr=True;
  } /* end loop over obj_idx */

  if(nco_dbg_lvl_get() == nco_dbg_vrb){
    for(unsigned int obj_idx=0;obj_idx<trv_tbl->nbr;obj_idx++){
      /* Create shallow copy to avoid indirection */
      trv_obj=trv_tbl->lst[obj_idx];

      (void)fprintf(stderr,"%s: INFO %s final flags of %s %s:\n",nco_prg_nm_get(),fnc_nm,(trv_obj.nco_typ == nco_obj_typ_grp) ? "group" : "variable",trv_obj.nm_fll);
      (void)fprintf(stderr," flg_dfl? %s.",(trv_obj.flg_dfl) ? "Yes" : "No");
      (void)fprintf(stderr," flg_mch? %s.",(trv_obj.flg_mch) ? "Yes" : "No");
      (void)fprintf(stderr," flg_xtr? %s.",(trv_obj.flg_xtr) ? "Yes" : "No");
      if(trv_obj.nco_typ == nco_obj_typ_var) (void)fprintf(stderr," flg_vfp? %s.",(trv_obj.flg_vfp) ? "Yes" : "No");
      if(trv_obj.nco_typ == nco_obj_typ_var) (void)fprintf(stderr," flg_vsg? %s.",(trv_obj.flg_vsg) ? "Yes" : "No");
      if(trv_obj.nco_typ == nco_obj_typ_grp) (void)fprintf(stderr," flg_gcv? %s.",(trv_obj.flg_gcv) ? "Yes" : "No");
      if(trv_obj.nco_typ == nco_obj_typ_grp) (void)fprintf(stderr," flg_ncs? %s.",(trv_obj.flg_ncs) ? "Yes" : "No");
      if(trv_obj.nco_typ == nco_obj_typ_grp) (void)fprintf(stderr," flg_nsx? %s.",(trv_obj.flg_nsx) ? "Yes" : "No");
      (void)fprintf(stderr,"\n");
    } /* end loop over obj_idx */
  } /* endif dbg */

  if(nco_dbg_lvl_get() == nco_dbg_old) (void)trv_tbl_prn_xtr(trv_tbl,fnc_nm);

  return (nco_bool)True;

} /* end nco_xtr_mk() */

void
nco_xtr_xcl /* [fnc] Convert extraction list to exclusion list */
(nco_bool EXTRACT_ASSOCIATED_COORDINATES, /* I [flg] Extract all coordinates associated with extracted variables? */
 nco_bool GRP_XTR_VAR_XCL, /* I [flg] Extract matching groups, exclude matching variables */
 trv_tbl_sct * const trv_tbl) /* I/O [sct] GTT (Group Traversal Table) */
{
  /* Purpose: Convert extraction list to exclusion list
     Exclusion is ambiguous for groups
     Consider, e.g., ncks -x -v /g1/v1 
     Should that exclude g1 completely, e.g., exclude /g1/v2 as well? Probably not. 
     Extraction list for this case is nearly unambiguous: ~(/g1/v1)
     Now consider a different way of specifying the same thing, i.e., ncks -x -g g1 -v v1 
     Since g1 is explicitly specified, it is ambiguous whether the -x should apply to all of g1
     Extraction list for this case, as of 20140520, is same as above: ~(/g1/v1)
     I.e., extract all except exclude intersection of -g and -v arguments
     However, one could also decide to implement this extraction list: *g1*, ~(v1)
     (here *g1* means the full group path contains a whole component named g1)
     In other words elements matching -v arguments could be excluded, and those matching -g could be extracted
     This would make it easy to extract certain groups, while excluding certain variables from those groups

     There are use-cases where the exclusion flag should exclude groups
     Consider, e.g., ncks -x -g g1
     In this case g1 and all descendents should be excluded

     ncdismember needs to create extraction lists like: *g1*, ~(/g1/v1); or *g1*, ~v1;
     Potential avenues to implement this include
     1. ncks -x -g g1 -v v1
     2. ncks -x -g g1 -v /g1/v1
     3. ncks --grp_xtr_var_xcl -g g1 -v v1 (method implemented in NCO 4.4.4 in 201405)

     Given that, here is how this routine and NCO actually uses flg_xcl:
     For variables, -x sets the exclusion flag and is "permanent", i.e., removes variable from extraction list
     For groups, -x sets the exclusion flag but is not "permanent"---it does not remove group from extraction list
     Instead it is used to help determine whether group should be excluded for metadata-only containing groups 
     Group extraction is reset and done from scratch (except for flg_xcl/flg_mtd) in nco_xtr_grp_mrk() */

  const char fnc_nm[]="nco_xtr_xcl()"; /* [sng] Function name */

  if(GRP_XTR_VAR_XCL){
    /* Block implements GRP_XTR_VAR_XCL option introduced 20140521 */
    for(unsigned idx_tbl=0;idx_tbl<trv_tbl->nbr;idx_tbl++){
      if(trv_tbl->lst[idx_tbl].nco_typ == nco_obj_typ_var){
	/* Toggle variable extraction under any of following conditions */
	if(
	   /* Variable set to be extracted because it matches explicit -v extraction string */
	   (trv_tbl->lst[idx_tbl].flg_xtr) ||
	   /* Variable does not match explicit -v extraction string (so will not be excluded)
	      and variable is in group that does match explicit -g extraction string */
	   (!trv_tbl->lst[idx_tbl].flg_mch && trv_tbl->lst[idx_tbl].flg_vsg) ||
	   False){
	  trv_tbl->lst[idx_tbl].flg_xtr=!trv_tbl->lst[idx_tbl].flg_xtr; /* Toggle extraction flag */
	  trv_tbl->lst[idx_tbl].flg_xcl=True; /* Extraction flag was toggled by exclusion option */
	} /* end if */
      } /* end if */
    } /* end for */
  }else{
    /* Block implements default (and only) behavior until 20140521,
       i.e., extract all except exclude intersection of -g and -v arguments */
    static short FIRST_WARNING=True;
    for(unsigned idx_tbl=0;idx_tbl<trv_tbl->nbr;idx_tbl++){
      trv_tbl->lst[idx_tbl].flg_xtr=!trv_tbl->lst[idx_tbl].flg_xtr; /* Toggle extraction flag */
      trv_tbl->lst[idx_tbl].flg_xcl=True; /* Extraction flag was toggled by exclusion option */
      if(!trv_tbl->lst[idx_tbl].flg_xtr && trv_tbl->lst[idx_tbl].nco_typ == nco_obj_typ_var && trv_tbl->lst[idx_tbl].is_crd_var){
	if(nco_dbg_lvl_get() >= nco_dbg_std && FIRST_WARNING && EXTRACT_ASSOCIATED_COORDINATES){
	  (void)fprintf(stdout,"%s: HINT Explicitly excluding (with -x) a coordinate variable (like \"%s\") from the extraction list does not always remove the coordinate from output unless the -C option is also invoked to turn off extraction of coordinates associated with other variables. Otherwise, a coordinate you wish to exclude may be extracted in its capacity as coordinate-information for other extracted variables. Use \"-C -x -v crd_nm\" to guarantee that crd_nm will not be output. See http://nco.sf.net/nco.html#xmp_xtr_xcl for more information.\n",nco_prg_nm_get(),trv_tbl->lst[idx_tbl].nm);
	  FIRST_WARNING=False;
	} /* !FIRST_WARNING */
      } /* end if */
    } /* end for */
  } /* !GRP_XTR_VAR_XCL */

  if(nco_dbg_lvl_get() == nco_dbg_old) (void)trv_tbl_prn_xtr(trv_tbl,fnc_nm);

  return;
} /* end nco_xtr_xcl() */

void
nco_xtr_xcl_chk /* [fnc] Convert extraction list to exclusion list */
(char ** var_lst_in, /* I [sng] User-specified list of variables */
 const int var_xtr_nbr, /* I [nbr] Number of variables in list */
 trv_tbl_sct * const trv_tbl) /* I [sct] GTT (Group Traversal Table) */
{
  /* Purpose: Die with helpful hint if extraction rules lead to retaining user-excluded variables
     See discussion at https://github.com/nco/nco/issues/211 
     Test cases: 
     ncks -O -x -v one ~/nco/data/in.nc ~/foo.nc;ncks -m -v one ~/foo.nc # Should extract one
     ncks -O -x -v lat ~/nco/data/in.nc ~/foo.nc;ncks -m -v lat ~/foo.nc # Should die on lat
     ncks -O -x -v time_bnds ~/nco/data/in.nc ~/foo.nc;ncks -m -v time_bnds ~/foo.nc # Should die */

  const char fnc_nm[]="nco_xtr_xcl_chk()"; /* [sng] Function name */

  /* Extract all except exclude intersection of -g and -v arguments */
  for(unsigned idx_tbl=0;idx_tbl<trv_tbl->nbr;idx_tbl++){
    if(trv_tbl->lst[idx_tbl].flg_xtr && trv_tbl->lst[idx_tbl].flg_mch && trv_tbl->lst[idx_tbl].nco_typ == nco_obj_typ_var){
      (void)fprintf(stdout,"%s: ERROR %s reports explicitly excluding (with -x or synonyms --xcl, --exclude) the variable \"%s\" from the output fails because \"%s\" is a coordinate, auxiliary coordinate, and/or CF-defined (Climate & Forecast Metadata Conventions) variable associated with or ancillary to at least one other variable that would be output. NCO's paradigm is to, by default, extract all ancillary variables associated with requested (whether implicitly or explicitly) variables unless explicitly instructed otherwise. To exclude \"%s\" from output please explicitly invoke the -C (or synonyms --no_crd, --xcl_ass_var) option like this: \"-C -x -v %s\". This turns-off the default behavior of adding associated variables to the extraction list.\nHINT: See http://nco.sf.net/nco.html#xmp_xtr_xcl for more information on this option and for the why's and wherefore's of associated variables.\n",nco_prg_nm_get(),fnc_nm,trv_tbl->lst[idx_tbl].nm,trv_tbl->lst[idx_tbl].nm,trv_tbl->lst[idx_tbl].nm,trv_tbl->lst[idx_tbl].nm);
	nco_exit(EXIT_FAILURE);
    } /* end if */
  } /* end for */

  return;
} /* end nco_xtr_xcl_chk() */

void
nco_xtr_crd_add                       /* [fnc] Add all coordinates to extraction list */
(trv_tbl_sct * const trv_tbl)         /* I/O [sct] GTT (Group Traversal Table) */
{
  /* Purpose: Add all coordinates to extraction list
     Find all coordinates (variables with same names and sizes as dimensions) and
     ensure they are marked for extraction */

  const char fnc_nm[]="nco_xtr_crd_add()"; /* [sng] Function name */

  /* If variable is coordinate variable then mark it for extraction */
  for(unsigned idx_var=0;idx_var<trv_tbl->nbr;idx_var++)
    if(trv_tbl->lst[idx_var].nco_typ == nco_obj_typ_var)
      if(trv_tbl->lst[idx_var].is_crd_var) trv_tbl->lst[idx_var].flg_xtr=True;

  /* Print extraction list in debug mode */
  if(nco_dbg_lvl_get() == nco_dbg_old) (void)trv_tbl_prn_xtr(trv_tbl,fnc_nm);

  return;
} /* end nco_xtr_crd_add() */

void
nco_xtr_ilev_add                      /* [fnc] Add ilev coordinate to extraction list */
(trv_tbl_sct * const trv_tbl)          /* I/O [sct] Traversal table */
{
  /* Purpose: Add ilev coordinate to extraction list if lev coordinate is already there
     20180920: CESM-family models forgot to add lev:bounds="ilev" attribute
     This workaround ensures extraction of ilev occurs in tandem with lev */

  //  const char fnc_nm[]="nco_xtr_ilev_add()"; /* [sng] Function name */

  unsigned int tbl_nbr;

  unsigned int tbl_idx;

  tbl_nbr=(unsigned int)trv_tbl->nbr;

  /* Does extraction list contain lev? */
  for(tbl_idx=0;tbl_idx<tbl_nbr;tbl_idx++){
    trv_sct var_trv=trv_tbl->lst[tbl_idx];
    if(var_trv.nco_typ == nco_obj_typ_var)
      if(var_trv.flg_xtr)
	if(!strcmp("lev",var_trv.nm))
	  break;
  } /* !tbl_idx */
  
  /* If so, and ilev is in file, then extract ilev */
  if(tbl_idx != tbl_nbr){
    for(tbl_idx=0;tbl_idx<tbl_nbr;tbl_idx++){
      trv_sct var_trv=trv_tbl->lst[tbl_idx];
      if(var_trv.nco_typ == nco_obj_typ_var)
	if(!strcmp("ilev",var_trv.nm)){
	  trv_tbl->lst[tbl_idx].flg_xtr=True;
	  return;
	} /* !ilev */
    } /* !tbl_idx */
  } /*  !xtr_lev */
	
  return;
} /* end nco_xtr_ilev_add() */

void
nco_xtr_lst /* [fnc] Print extraction list and exit */
(trv_tbl_sct * const trv_tbl) /* I [sct] GTT (Group Traversal Table) */
{
  /* Purpose: Print extraction list and exit
     ncclimo and ncremap invoke this ncks function to process regular expressions in variable lists
     Usage:
     ncks -v three.? --lst_xtr ~/nco/data/in.nc
     ncks -v FSNT,TREFHT --lst_xtr ~/data/ne30/raw/famipc5_ne30_v0.3_00003.cam.h0.1979-01.nc
     ncks -v ^[a-bA-B].? --lst_xtr ~/data/ne30/raw/famipc5_ne30_v0.3_00003.cam.h0.1979-01.nc */

  const char fnc_nm[]="nco_xtr_lst()"; /* [sng] Function name */

  int xtr_nbr_crr=0; /* [nbr] Number of N>=D variables found so far */

  int grp_id; /* [id] Group ID */
  int nc_id; /* [id] File ID */
  int var_id; /* [id] Variable ID */

  trv_sct var_trv;

  nc_id=trv_tbl->in_id_arr[0];

  /* If variable is on extraction list, print it to stdout */
  for(unsigned idx_var=0;idx_var<trv_tbl->nbr;idx_var++){
    var_trv=trv_tbl->lst[idx_var];
    if(var_trv.nco_typ == nco_obj_typ_var && var_trv.flg_xtr){
      (void)nco_inq_grp_full_ncid(nc_id,var_trv.grp_nm_fll,&grp_id);
      (void)nco_inq_varid(grp_id,var_trv.nm,&var_id);
      /* 20170829: Eliminate bounds variables from stdout list
	 Intended to keep time bounds variables from reaching ncclimo 
	 Variables like time_bnds may be replaced by, e.g., climatology_bounds in seasonal files
	 Hence time bounds variables should not be specifically requested because they may not exist
	 NCO's associated coordinate feature will extract them anyway
	 So best not to explicitly request them in ncclimo */
      if(!nco_is_spc_in_cf_att(grp_id,"bounds",var_id,NULL)){
	(void)fprintf(stdout,"%s%s",(xtr_nbr_crr > 0) ? "," : "",var_trv.nm);
	xtr_nbr_crr++;
      } /* !bounds */
    } /* !flg_xtr */
  } /* !idx_var */
  
  if(xtr_nbr_crr > 0){
    (void)fprintf(stdout,"\n");
    nco_exit(EXIT_SUCCESS);
  }else{
    (void)fprintf(stdout,"%s: ERROR %s reports empty extraction list\n",nco_prg_nm_get(),fnc_nm);
    nco_exit(EXIT_FAILURE);
  } /* !xtr_nbr_crr */
  
  return;
} /* end nco_xtr_lst() */

void
nco_xtr_ND_lst /* [fnc] Print extraction list of N>=D variables and exit */
(trv_tbl_sct * const trv_tbl) /* I [sct] GTT (Group Traversal Table) */
{
  /* Purpose: Print extraction list of N>=D variables and exit
     Used by ncks to supply arguments to splitter
     Hence we restrict returned list to non-coordinate record variables
     Usage:
     ncks --lst_rnk_ge2 ~/nco/data/in.nc
     ncks --lst_rnk_ge2 ~/data/ne30/raw/famipc5_ne30_v0.3_00003.cam.h0.1979-01.nc
     ncks --lst_rnk_ge2 ~/data/ne30/rgr/famipc5_ne30_v0.3_00003.cam.h0.1979-01.nc */

  const char fnc_nm[]="nco_xtr_ND_lst()"; /* [sng] Function name */

  const int rnk_xtr=2; /* [nbr] Minimum rank to extract */

  int xtr_nbr_crr=0; /* [nbr] Number of N>=D variables found so far */

  int grp_id; /* [id] Group ID */
  int nc_id; /* [id] File ID */
  int var_id; /* [id] Variable ID */

  trv_sct var_trv;

  nc_id=trv_tbl->in_id_arr[0];

  /* 20170414: csz add new definitions is_crd_lk_var and is_rec_lk_var, avoid PVN definitions for sanity */
  for(unsigned idx_var=0;idx_var<trv_tbl->nbr;idx_var++){
    var_trv=trv_tbl->lst[idx_var];
    if(var_trv.nco_typ == nco_obj_typ_var){
      (void)nco_inq_grp_full_ncid(nc_id,var_trv.grp_nm_fll,&grp_id);
      (void)nco_inq_varid(grp_id,var_trv.nm,&var_id);
      trv_tbl->lst[idx_var].is_crd_lk_var=trv_tbl->lst[idx_var].is_crd_lk_var;
      if(nco_is_spc_in_cf_att(grp_id,"bounds",var_id,NULL)) trv_tbl->lst[idx_var].is_crd_lk_var=True;
      if(nco_is_spc_in_cf_att(grp_id,"cell_measures",var_id,NULL)) trv_tbl->lst[idx_var].is_crd_lk_var=True;
      if(nco_is_spc_in_cf_att(grp_id,"climatology",var_id,NULL)) trv_tbl->lst[idx_var].is_crd_lk_var=True;
      for(int dmn_idx=0;dmn_idx<var_trv.nbr_dmn;dmn_idx++){
	if(var_trv.var_dmn[dmn_idx].is_rec_dmn) trv_tbl->lst[idx_var].is_rec_lk_var=True;
      } /* !dmn_idx */
    } /* !nco_typ */
  } /* !idx_var */

  /* If variable has N>=D dimensions, add it to list */
  for(unsigned idx_var=0;idx_var<trv_tbl->nbr;idx_var++){
    if(trv_tbl->lst[idx_var].nco_typ == nco_obj_typ_var){
      if((trv_tbl->lst[idx_var].nbr_dmn >= rnk_xtr) && /* Rank at least 2 */
	 (!trv_tbl->lst[idx_var].is_crd_lk_var) && /* Not a coordinate-like variable */
	 (trv_tbl->lst[idx_var].is_rec_lk_var) && /* Is a record variable */
	 (trv_tbl->lst[idx_var].var_typ != NC_CHAR) && /* Not an array of characters */
	 True){
	(void)fprintf(stdout,"%s%s",(xtr_nbr_crr > 0) ? "," : "",trv_tbl->lst[idx_var].nm);
	xtr_nbr_crr++;
      } /* !N>=D */
    } /* !nco_typ */
  } /* !idx_var */
  if(xtr_nbr_crr > 0){
    (void)fprintf(stdout,"\n");
    nco_exit(EXIT_SUCCESS);
  }else{
    (void)fprintf(stdout,"%s: ERROR %s reports no variables found with rank >= %d\n",nco_prg_nm_get(),fnc_nm,rnk_xtr);
    nco_exit(EXIT_FAILURE);
  } /* !xtr_nbr_crr */
    
  return;
} /* end nco_xtr_ND_lst() */

void
nco_xtr_cf_add /* [fnc] Add to extraction list variables associated with CF convention */
(const int nc_id, /* I [ID] netCDF file ID */
 const char * const cf_nm, /* I [sng] CF convention ("ancillary_variables", "bounds", "climatology", "coordinates", or "grid_mapping") */
 trv_tbl_sct * const trv_tbl) /* I/O [sct] GTT (Group Traversal Table) */
{
  /* Add to extraction list all variables associated with specified CF convention
     Driver routine for nco_xtr_cf_var_add()
     Detect associated coordinates specified by CF "ancillary_variables", "bounds", and "coordinates" conventions
     http://cf-pcmdi.llnl.gov/documents/cf-conventions/1.1/cf-conventions.html#coordinate-system */ 

  const char fnc_nm[]="nco_xtr_cf_add()"; /* [sng] Function name */

  /* Search for and add CF-compliant bounds and coordinates to extraction list */
  for(unsigned idx_tbl=0;idx_tbl<trv_tbl->nbr;idx_tbl++)
    if(trv_tbl->lst[idx_tbl].nco_typ == nco_obj_typ_var && trv_tbl->lst[idx_tbl].flg_xtr)
      (void)nco_xtr_cf_var_add(nc_id,&trv_tbl->lst[idx_tbl],cf_nm,trv_tbl);

  /* Print extraction list in debug mode */
  if(nco_dbg_lvl_get() == nco_dbg_old) (void)trv_tbl_prn_xtr(trv_tbl,fnc_nm);

  return;
} /* nco_xtr_cf_add() */

void
nco_xtr_cf_var_add /* [fnc] Add variables associated (via CF) with specified variable to extraction list */
(const int nc_id, /* I [ID] netCDF file ID */
 const trv_sct * const var_trv, /* I [sct] Variable (object) */
 const char * const cf_nm, /* I [sng] CF convention ("ancillary_variables", "bounds", "climatology", "coordinates", or "grid_mapping") */
 trv_tbl_sct * const trv_tbl) /* I/O [sct] GTT (Group Traversal Table) */
{
  /* Detect associated variables specified by CF "ancillary_variables", "bounds", "climatology", "coordinates", and "grid_mapping" conventions
     Private routine called by nco_xtr_cf_add()
     http://cfconventions.org/1.6.html#ancillary-data
     http://cf-pcmdi.llnl.gov/documents/cf-conventions/1.1/cf-conventions.html#coordinate-system */ 

  char **cf_lst=NULL_CEWI; /* [sng] 1D array of list elements */
  char att_nm[NC_MAX_NAME+1L]; /* [sng] Attribute name */

  const char dlm_sng[]=" "; /* [sng] Delimiter string */
  const char fnc_nm[]="nco_xtr_cf_var_add()"; /* [sng] Function name */

  int grp_id; /* [id] Group ID */
  int nbr_att; /* [nbr] Number of attributes */
  int nbr_cf; /* [nbr] Number of variables specified in CF attribute ("ancillary_variables", "bounds", "climatology", "coordinates", and "grid_mapping") */
  int var_id; /* [id] Variable ID */

  assert(var_trv->nco_typ == nco_obj_typ_var);
  (void)nco_inq_grp_full_ncid(nc_id,var_trv->grp_nm_fll,&grp_id);
  (void)nco_inq_varid(grp_id,var_trv->nm,&var_id);
  (void)nco_inq_varnatts(grp_id,var_id,&nbr_att);
  assert(nbr_att == var_trv->nbr_att);

  for(int idx_att=0;idx_att<nbr_att;idx_att++){

    /* Get attribute name */
    (void)nco_inq_attname(grp_id,var_id,idx_att,att_nm);

    /* Is attribute part of CF convention? */
    if(!strcmp(att_nm,cf_nm)){
      char *att_val;
      long att_sz;
      nc_type att_typ;

      /* Yes, get list of specified attributes */
      (void)nco_inq_att(grp_id,var_id,att_nm,&att_typ,&att_sz);
      if(att_typ != NC_CHAR){
        (void)fprintf(stderr,"%s: WARNING \"%s\" attribute for variable %s is type %s, not %s. This violates the CF convention for allowed datatypes (http://cfconventions.org/cf-conventions/cf-conventions.html#_data_types). Therefore %s will skip this attribute. If you want CF to support NC_STRING attributes, please tell CF and CC: NCO.\n",nco_prg_nm_get(),att_nm,var_trv->nm_fll,nco_typ_sng(att_typ),nco_typ_sng(NC_CHAR),fnc_nm);
        return;
      } /* end if */
      att_val=(char *)nco_malloc((att_sz+1L)*sizeof(char));
      if(att_sz > 0L) (void)nco_get_att(grp_id,var_id,att_nm,(void *)att_val,NC_CHAR);	  
      /* NUL-terminate attribute */
      att_val[att_sz]='\0';

      /* Split list into separate coordinate names
	 Use nco_lst_prs_sgl_2D() not nco_lst_prs_2D() to avert TODO nco944 */
      if(!strcmp("cell_measures",cf_nm) || !strcmp("formula_terms",cf_nm)){
	/* cell_measures and formula_terms use this syntax to list variables required to know grid or evaluate formula:
	   orog:cell_measures = "area: areacella"
	   lev:standard_name = "atmosphere_hybrid_sigma_pressure_coordinate"
	   lev:formula_terms = "a: hyam b: hybm p0: P0 ps: PS" */
	/* static short FIRST_WARNING=True;
        if(FIRST_WARNING) (void)fprintf(stderr,"%s: WARNING %s reports that variables necessary to evaluate \"%s\" formula for variable %s are not yet extracted. This WARNING is printed only once per invocation.\n",nco_prg_nm_get(),fnc_nm,att_nm,var_trv->nm_fll);
	FIRST_WARNING=False; */
	nbr_cf=0;
	char *cln_ptr=att_val;
	char *spc_ptr=NULL;
	long var_lng;
	while((cln_ptr=strstr(cln_ptr,": "))){
	  spc_ptr=strchr(cln_ptr+2L,' ');
	  if(spc_ptr) var_lng=spc_ptr-cln_ptr-2L; else var_lng=strlen(cln_ptr+2L);
	  cf_lst=(char **)nco_realloc(cf_lst,(nbr_cf+1)*sizeof(char *));
	  cf_lst[nbr_cf]=(char *)nco_malloc(var_lng*sizeof(char)+1L);
          *(cf_lst[nbr_cf]+var_lng)='\0';
          strncpy(cf_lst[nbr_cf],cln_ptr+2L,var_lng);
	  cln_ptr+=var_lng;
	  if(nco_dbg_lvl_get() >= nco_dbg_io) (void)fprintf(stderr,"%s: DEBUG %s reports variable %s %s variable #%d is %s\n",nco_prg_nm_get(),fnc_nm,var_trv->nm_fll,att_nm,nbr_cf,cf_lst[nbr_cf]);
	  nbr_cf++;
	} /* !att_val */
      }else{
	/* All CF attributes that NCO handles besides "cell_measures" and "formula_terms" are space-separated lists */
	cf_lst=nco_lst_prs_sgl_2D(att_val,dlm_sng,&nbr_cf);
      } /* !formula_terms */
      
      /* ...for each variable in CF convention attribute, i.e., for each variable listed in "ancillary_variables", or in "bounds", or in "coordinates", or in "grid_mapping", ... */
      for(int idx_cf=0;idx_cf<nbr_cf;idx_cf++){
        char *cf_lst_var=cf_lst[idx_cf];
        if(!cf_lst_var) continue;

        char *cf_lst_var_nm_fll;       /* [sng] Built full name of 'CF' variable to find */
        const char sls_chr='/';        /* [chr] Slash character */
        const char sls_sng[]="/";      /* [sng] Slash string */
	const char cur_dir_sng[]="./"; /* [sng] current dir */  
	const char up_dir_sng[]="../"; /* [sng] current dir */  					 
        char *ptr_chr;                 /* [sng] Pointer to character '/' in full name */
        int psn_chr;                   /* [nbr] Position of character '/' in in full name */

	/* Create memory for full name of CF variable */
        cf_lst_var_nm_fll=(char *)nco_malloc(strlen(var_trv->grp_nm_fll)+strlen(cf_lst_var)+2L);
        cf_lst_var_nm_fll[0]='\0'; 
	
        /* Does cf_lst_var have a path of some kind? */
	ptr_chr=strchr(cf_lst_var,sls_chr);
        if(ptr_chr){
          /* Does cf_lst start with '/' an absolute path? */
	  if(cf_lst_var[0] == '/'){
	    strcpy(cf_lst_var_nm_fll,cf_lst_var);
	  }
          /* Does cf_lst_var start with "./"? */
	  else if(strncmp(cf_lst_var,cur_dir_sng,strlen(cur_dir_sng)) == 0){
            if(strcmp(var_trv->grp_nm_fll,sls_sng))    
	      strcpy(cf_lst_var_nm_fll,var_trv->grp_nm_fll);
	    strcat(cf_lst_var_nm_fll, cf_lst_var+(size_t)1);
	  }
	  /* Does cf_lst_var start with "../"? */
          else if(strncmp(cf_lst_var,up_dir_sng,strlen(up_dir_sng)) == 0){
	    /* Remove last dirname from var_trv->grp_nm_fll */
	    strcpy(cf_lst_var_nm_fll,var_trv->grp_nm_fll);
	    /* Search for final '/' */
	    ptr_chr=strrchr(cf_lst_var_nm_fll,sls_chr);      
	    if(ptr_chr) *ptr_chr='\0';
	    strcat(cf_lst_var_nm_fll,cf_lst_var+(size_t)2);     

	  }
	  /* '/' somewhere in middle of string */
	  else{
	    strcpy(cf_lst_var_nm_fll,var_trv->grp_nm_fll);
	    if(strcmp(var_trv->grp_nm_fll,sls_sng)) strcat(cf_lst_var_nm_fll,sls_sng);
	    strcat(cf_lst_var_nm_fll,cf_lst_var);
          } 	  
          /* If variable is on list */
          if(trv_tbl_fnd_var_nm_fll(cf_lst_var_nm_fll,trv_tbl))
            /* Mark it for extraction */
            (void)trv_tbl_mrk_xtr(cf_lst_var_nm_fll,True,trv_tbl);

	  cf_lst_var_nm_fll=(char *)nco_free(cf_lst_var_nm_fll);
	  
          continue;
	}  

        strcpy(cf_lst_var_nm_fll,var_trv->grp_nm_fll);
        if(strcmp(var_trv->grp_nm_fll,sls_sng)) strcat(cf_lst_var_nm_fll,sls_sng);
        strcat(cf_lst_var_nm_fll,cf_lst_var);
	
        /* Find last occurence of '/' */
        ptr_chr=strrchr(cf_lst_var_nm_fll,sls_chr);
        psn_chr=ptr_chr-cf_lst_var_nm_fll;
        while(ptr_chr){

          /* If variable is on list */
          if(trv_tbl_fnd_var_nm_fll(cf_lst_var_nm_fll,trv_tbl)){
            /* Mark it for extraction */
            (void)trv_tbl_mrk_xtr(cf_lst_var_nm_fll,True,trv_tbl);
            /* Exclude ancestor (greater scope, closer to root) variables, add only most in-scope (usually in same group) */
            break;
          } /* end if */

          cf_lst_var_nm_fll[psn_chr]='\0';
          ptr_chr=strrchr(cf_lst_var_nm_fll,sls_chr);
          /* Re-add variable name to shortened path */
          if(ptr_chr){
            psn_chr=ptr_chr-cf_lst_var_nm_fll;
            cf_lst_var_nm_fll[psn_chr]='\0';
            if(strcmp(var_trv->grp_nm_fll,sls_sng)) strcat(cf_lst_var_nm_fll,sls_sng);
            strcat(cf_lst_var_nm_fll,cf_lst_var);
            ptr_chr=strrchr(cf_lst_var_nm_fll,sls_chr);
            psn_chr=ptr_chr-cf_lst_var_nm_fll;
          } /* !ptr_chr */
        } /* end while */

        /* Free allocated */
        if(cf_lst_var_nm_fll) cf_lst_var_nm_fll=(char *)nco_free(cf_lst_var_nm_fll);
    
      } /* end loop over idx_cf */

      /* Free allocated memory */
      att_val=(char *)nco_free(att_val);
      cf_lst=nco_sng_lst_free(cf_lst,nbr_cf);

    } /* end strcmp() */
  } /* end loop over attributes */

  return;
} /* nco_xtr_cf_var_add() */

nm_id_sct *                           /* O [sct] Extraction list */  
nco_trv_tbl_nm_id                     /* [fnc] Create extraction list of nm_id_sct from traversal table */
(const int nc_id_in,                  /* I [ID] netCDF input file ID */
 const int nc_id_out,                 /* I [ID] netCDF output file ID */
 const gpe_sct * const gpe,           /* I [sct] GPE structure */
 int * const xtr_nbr,                 /* I/O [nbr] Number of variables in extraction list */
 const trv_tbl_sct * const trv_tbl)   /* I [sct] GTT (Group Traversal Table) */
{
  /* Purpose: Create extraction list of nm_id_sct from traversal table */

  nm_id_sct *xtr_lst; /* [sct] Extraction list */

  int nbr_tbl=0; 
  for(unsigned idx_tbl=0;idx_tbl<trv_tbl->nbr;idx_tbl++)
    if(trv_tbl->lst[idx_tbl].nco_typ == nco_obj_typ_var && trv_tbl->lst[idx_tbl].flg_xtr) nbr_tbl++;

  xtr_lst=(nm_id_sct *)nco_malloc(nbr_tbl*sizeof(nm_id_sct));

  nbr_tbl=0;
  for(unsigned idx_tbl=0;idx_tbl<trv_tbl->nbr;idx_tbl++){
    if(trv_tbl->lst[idx_tbl].nco_typ == nco_obj_typ_var && trv_tbl->lst[idx_tbl].flg_xtr){
      int var_id;
      int grp_id_in;
      int grp_id_out;
      char *grp_out_fll;
      /* Get input group ID */
      (void)nco_inq_grp_full_ncid(nc_id_in,trv_tbl->lst[idx_tbl].grp_nm_fll,&grp_id_in);
      /* Edit group name for output */
      if(gpe) grp_out_fll=nco_gpe_evl(gpe,trv_tbl->lst[idx_tbl].grp_nm_fll); else grp_out_fll=(char *)strdup(trv_tbl->lst[idx_tbl].grp_nm_fll);
      /* Get output group ID */
      (void)nco_inq_grp_full_ncid(nc_id_out,grp_out_fll,&grp_id_out);
      /* Get variable ID */
      (void)nco_inq_varid(grp_id_in,trv_tbl->lst[idx_tbl].nm,&var_id);

      grp_out_fll=(char *)nco_free(grp_out_fll);

      /* 20130213: Necessary to allow MM3->MM4 and MM4->MM3 workarounds
	 Store in/out group IDs as determined in nco_xtr_dfn() 
	 In MM3/4 cases, either grp_in_id or grp_out_id are always root
	 Other is always root unless GPE is used */
      xtr_lst[nbr_tbl].grp_id_in=grp_id_in;
      xtr_lst[nbr_tbl].grp_id_out=grp_id_out;
      xtr_lst[nbr_tbl].id=var_id;
      xtr_lst[nbr_tbl].nm=(char *)strdup(trv_tbl->lst[idx_tbl].nm);

      nbr_tbl++;
    } /* end flg == True */
  } /* end loop over idx_tbl */

  *xtr_nbr=nbr_tbl;
  return xtr_lst;
} /* end nco_trv_tbl_nm_id() */

void
nco_xtr_crd_ass_add                   /* [fnc] Add to extraction list all coordinates associated with extracted variables */
(const int nc_id,                     /* I [ID] netCDF file ID */
 trv_tbl_sct * const trv_tbl)         /* I/O [sct] GTT (Group Traversal Table) */
{
  /* Purpose: Add to extraction list all coordinates associated with extracted variables */

  const char fnc_nm[]="nco_xtr_crd_ass_add()"; /* [sng] Function name */

  char dmn_nm_var[NC_MAX_NAME+1];    /* [sng] Dimension name for *variable* */ 

  int *dmn_id_var;      /* [ID] Dimensions IDs array for variable */
  int grp_id;           /* [ID] Group ID */
  int nbr_dmn_var;      /* [nbr] Number of dimensions associated with current matched variable */
  int var_id;           /* [ID] Variable ID */

  long dmn_sz;          /* [nbr] Dimension size */  

  /* Loop table */
  for(unsigned idx_tbl=0;idx_tbl<trv_tbl->nbr;idx_tbl++){
    trv_sct var_trv=trv_tbl->lst[idx_tbl];

    /* Filter variables to extract */
    if(var_trv.nco_typ == nco_obj_typ_var && var_trv.flg_xtr){

      /* Obtain group ID */
      (void)nco_inq_grp_full_ncid(nc_id,var_trv.grp_nm_fll,&grp_id);

      /* Obtain variable ID */
      (void)nco_inq_varid(grp_id,var_trv.nm,&var_id);

      /* Get number of dimensions for *variable* */
      (void)nco_inq_varndims(grp_id,var_id,&nbr_dmn_var);

      if(nco_dbg_lvl_get() >= nco_dbg_dev && nco_dbg_lvl_get() <= nco_dbg_nbr){
        (void)fprintf(stdout,"%s: DEBUG %s <%s> nbr_dmn_var=%d var_trv.nbr_dmn=%d\n",nco_prg_nm_get(),fnc_nm,var_trv.nm_fll,nbr_dmn_var,var_trv.nbr_dmn); 

        if(nbr_dmn_var != var_trv.nbr_dmn){
          (void)fprintf(stdout,"%s: ERROR %s <%s> nbr_dmn_var=%d var_trv.nbr_dmn=%d\n",nco_prg_nm_get(),fnc_nm,var_trv.nm_fll,nbr_dmn_var,var_trv.nbr_dmn); 
          (void)nco_prn_dmn(nc_id,var_trv.grp_nm_fll,var_trv.nm,var_trv.nm_fll,trv_tbl);
        } /* !nbr_dmn_var */
        (void)fflush(stdout);
      } /* !dbg */

      if(nco_dbg_lvl_get() >= nco_dbg_dev && nco_dbg_lvl_get() <= nco_dbg_nbr) (void)nco_prn_dmn(nc_id,var_trv.grp_nm_fll,var_trv.nm,var_trv.nm_fll,trv_tbl);

      assert(nbr_dmn_var == var_trv.nbr_dmn);

      dmn_id_var=(int *)nco_malloc(nbr_dmn_var*sizeof(int)); 

      /* Get dimension IDs for variable */
      (void)nco_inq_vardimid(grp_id,var_id,dmn_id_var);

      /* Loop over dimensions of variable */
      for(int idx_var_dim=0;idx_var_dim<nbr_dmn_var;idx_var_dim++){

        /* Get dimension name */
        (void)nco_inq_dim(grp_id,dmn_id_var[idx_var_dim],dmn_nm_var,&dmn_sz);

        char dmn_nm_grp[NC_MAX_NAME+1];    /* [sng] Dimension name for *group*  */ 
        
        const int flg_prn=1;         /* [flg] Dimensions in all parent groups will also be retrieved */ 

        int dmn_id_grp[NC_MAX_DIMS]; /* [id] Dimensions IDs array */
        int nbr_dmn_grp;             /* [nbr] Number of dimensions for *group* */

        /* Obtain number of dimensions visible to group */
        (void)nco_inq(grp_id,&nbr_dmn_grp,NULL,NULL,NULL);

        /* Obtain dimension IDs */
        (void)nco_inq_dimids(grp_id,&nbr_dmn_grp,dmn_id_grp,flg_prn);

        /* Loop dimensions visible to group  */
        for(int idx_dmn=0;idx_dmn<nbr_dmn_grp;idx_dmn++){

          /* Get dimension info */
          (void)nco_inq_dim(grp_id,dmn_id_grp[idx_dmn],dmn_nm_grp,&dmn_sz);

          /* Does dimension match requested variable name (i.e., is it a coordinate variable?) */ 
          if(!strcmp(dmn_nm_grp,dmn_nm_var)){
            char *dmn_nm_fll;         /* [sng] Built dimension full name */
            const char sls_chr='/';   /* [chr] Slash character */
            const char sls_sng[]="/"; /* [sng] Slash string */
            char *ptr_chr;            /* [sng] Pointer to character '/' in full name */
            int psn_chr;              /* [nbr] Position of character '/' in in full name */

            /* Construct full (dimension/coordinate) name using the full group name where original variable resides */
            dmn_nm_fll=(char *)nco_malloc(strlen(var_trv.grp_nm_fll)+strlen(dmn_nm_grp)+2L);
            strcpy(dmn_nm_fll,var_trv.grp_nm_fll);
            if(strcmp(var_trv.grp_nm_fll,sls_sng)) strcat(dmn_nm_fll,sls_sng);
            strcat(dmn_nm_fll,dmn_nm_grp);

            /* Brute-force approach to find valid "dmn_nm_fll":
            Start at grp_nm_fll/var_nm and build all possible paths with var_nm. 
            Use case is /g5/g5g1/rz variable with /g5/rlev coordinate var. */

            /* Find last occurence of '/' */
            ptr_chr=strrchr(dmn_nm_fll,sls_chr);
            psn_chr=ptr_chr-dmn_nm_fll;
            while(ptr_chr){

              /* If variable is on list */
              if(trv_tbl_fnd_var_nm_fll(dmn_nm_fll,trv_tbl)){

                /* Mark it for extraction */
                (void)trv_tbl_mrk_xtr(dmn_nm_fll,True,trv_tbl);

                /* Subsetting should exclude ancestor with lower scope (closer to root) coordinates, add only the most in-scopee (usually in same group) */
                break;
              } /* If variable is on list, mark it for extraction */

              dmn_nm_fll[psn_chr]='\0';
              ptr_chr=strrchr(dmn_nm_fll,sls_chr);
              if(ptr_chr){
                psn_chr=ptr_chr-dmn_nm_fll;
                dmn_nm_fll[psn_chr]='\0';
                /* Re-add variable name to shortened path */
                if(strcmp(var_trv.grp_nm_fll,sls_sng)) strcat(dmn_nm_fll,sls_sng);
                strcat(dmn_nm_fll,dmn_nm_grp);
                ptr_chr=strrchr(dmn_nm_fll,sls_chr);
                psn_chr=ptr_chr-dmn_nm_fll;
              } /* !ptr_chr */
            } /* end while */

            /* Free allocated */
            if(dmn_nm_fll) dmn_nm_fll=(char *)nco_free(dmn_nm_fll);

          } /* Does dimension match requested variable name (i.e., is it a coordinate variable?) */ 
        } /* Loop dimensions visible to group  */    
      } /* Loop over dimensions of variable */
      /* Free dimension IDs array */
      dmn_id_var=(int *)nco_free(dmn_id_var);
    } /* Filter variables to extract */
  } /* Loop table */

  return;
} /* end nco_xtr_crd_ass_add() */

void 
nco_xtr_lst_prn /* [fnc] Print name-ID structure list */
(nm_id_sct * const nm_id_lst, /* I [sct] Name-ID structure list */
 const int nm_id_nbr) /* I [nbr] Number of name-ID structures in list */
{
  (void)fprintf(stdout,"%s: INFO List: %d extraction variables\n",nco_prg_nm_get(),nm_id_nbr); 
  for(int idx=0;idx<nm_id_nbr;idx++) (void)fprintf(stdout,"[%d] %s\n",idx,nm_id_lst[idx].nm); 
} /* end nco_xtr_lst_prn() */

void
nco_prn_xtr_mtd /* [fnc] Print variable metadata */
(const int nc_id, /* I [id] netCDF file ID */
 const prn_fmt_sct * const prn_flg, /* I [sct] Print-format information */
 const trv_tbl_sct * const trv_tbl) /* I [sct] GTT (Group Traversal Table) */
{ 
  for(unsigned idx_tbl=0;idx_tbl<trv_tbl->nbr;idx_tbl++){
    trv_sct var_trv=trv_tbl->lst[idx_tbl];
    if(var_trv.flg_xtr && var_trv.nco_typ == nco_obj_typ_var){

      /* Print full name of variable */
      if(var_trv.grp_dpt > 0) (void)fprintf(stdout,"%s\n",var_trv.nm_fll);

      /* Print variable metadata */ 
      (void)nco_prn_var_dfn(nc_id,prn_flg,&var_trv); 

      int grp_id; /* [id] Group ID */
      int var_id; /* [id] Variable ID */

      /* Obtain group ID */
      (void)nco_inq_grp_full_ncid(nc_id,var_trv.grp_nm_fll,&grp_id);

      /* Obtain variable ID */
      (void)nco_inq_varid(grp_id,var_trv.nm,&var_id);

      /* Print variable attributes */
      (void)nco_prn_att(grp_id,prn_flg,var_id);
    } /* end flg_xtr */
  } /* end idx_tbl */

  return;
} /* end nco_prn_xtr_mtd() */

void
nco_prn_xtr_val                       /* [fnc] Print variable data */
(const int nc_id,                     /* I netCDF file ID */
 prn_fmt_sct * const prn_flg,         /* I/O [sct] Print-format information */
 const trv_tbl_sct * const trv_tbl)   /* I [sct] GTT (Group Traversal Table) */
{
  /* Purpose: Print variable data */

  /* Loop variables in table */
  for(unsigned idx_var=0;idx_var<trv_tbl->nbr;idx_var++){
    trv_sct var_trv=trv_tbl->lst[idx_var];
    if(var_trv.flg_xtr && var_trv.nco_typ == nco_obj_typ_var){

      /* Print full name of variable */
      if(!prn_flg->dlm_sng && var_trv.grp_dpt > 0) (void)fprintf(stdout,"%s\n",var_trv.nm_fll);
      
      /* Print variable values */
      (void)nco_prn_var_val_trv(nc_id,prn_flg,&var_trv,trv_tbl);
    } /* End flg_xtr */
  } /* End Loop variables in table */

  return;
} /* end nco_prn_xtr_val() */

void
nco_xtr_dmn_mrk                      /* [fnc] Mark extracted dimensions */
(trv_tbl_sct * const trv_tbl)        /* I/O [sct] GTT (Group Traversal Table) */
{
  /* Purpose: Set flag for dimensions to be extracted
     ncks print functions need dimension extraction flag to print CDL files */

  unsigned int dmn_idx; /* [idx] Index over dimensions */
  unsigned int dmn_nbr; /* [nbr] Number of dimensions defined in file */
  unsigned int dmn_var_idx; /* [idx] Index over dimensions in variable */
  unsigned int dmn_var_nbr; /* [nbr] Number of dimensions in variable */
  unsigned int obj_idx; /* [idx] Index over objects */
  unsigned int obj_nbr; /* [nbr] Number of objects in table */

  dmn_nbr=trv_tbl->nbr_dmn;
  obj_nbr=trv_tbl->nbr;

  /* Set extraction flag for groups if ancestors of extracted variables */

  for(dmn_idx=0;dmn_idx<dmn_nbr;dmn_idx++){
    /* Set extraction flag to False then override below if found */
    trv_tbl->lst_dmn[dmn_idx].flg_xtr=False;
    for(obj_idx=0;obj_idx<obj_nbr;obj_idx++){
      trv_sct var_trv=trv_tbl->lst[obj_idx];
      /* For each variable to be extracted ... */
      if(var_trv.nco_typ != nco_obj_typ_grp && var_trv.flg_xtr){
        dmn_var_nbr=var_trv.nbr_dmn;
        for(dmn_var_idx=0;dmn_var_idx<dmn_var_nbr;dmn_var_idx++){
          if(var_trv.var_dmn[dmn_var_idx].dmn_id == trv_tbl->lst_dmn[dmn_idx].dmn_id){
            trv_tbl->lst_dmn[dmn_idx].flg_xtr=True;
            /* Break from loop over dmn_var_idx into loop over obj_idx */
            break;
          } /* endif match */
        } /* end loop over dmn_var_idx */
      } /* endif extracted variable */
      /* Break from loop over obj_idx into loop over dmn_idx */
      if(trv_tbl->lst_dmn[dmn_idx].flg_xtr) break;
    } /* end loop over obj_idx */
  } /* end loop over dmn_idx */
} /* end nco_xtr_dmn_mrk() */

void
nco_xtr_grp_mrk                      /* [fnc] Mark extracted groups */
(trv_tbl_sct * const trv_tbl)        /* I/O [sct] GTT (Group Traversal Table) */
{
  /* Purpose: Set flag for groups to be extracted
     Could be performed before or after writing variables
     Used to be part of nco_xtr_dfn()
     However, ncks print functions need group extraction flag set for printing
     As of 20130716 we isolate this flag-setting in nco_xtr_grp_mrk()
     Actual group copying still done in nco_xtr_dfn() */

  const char sls_sng[]="/"; /* [sng] Slash string */

  char *grp_fll_sls; /* [sng] Full group name with slash appended */
  char *sbs_srt; /* [sng] Location of user-string match start in object path */

  nco_bool flg_pth_srt_bnd; /* [flg] String begins at path component boundary */
  nco_bool flg_pth_end_bnd; /* [flg] String ends   at path component boundary */

  unsigned int grp_idx;
  unsigned int obj_idx;

  /* Goal here is to annotate which groups will appear in output
     Need to know in order to efficiently copy their metadata
     Definition of flags in extraction table is operational
     Could create a new flag just for this
     Instead, we re-purpose the extraction flag, flg_xtr, for groups
     Could re-purpose flg_ncs too with same effect
     nco_xtr_mk() sets flg_xtr for groups, like variables, that match user-specified strings
     Later processing makes flg_xtr for groups unreliable
     For instance, the exclusion flag (-x) is ambiguous for groups
     Also identification of associated coordinates and auxiliary coordinates occurs after nco_xtr_mk()
     Associated and auxiliary coordinates may be in distant groups
     Hence no better place than nco_xtr_grp_mrk() to finally identify ancestor groups */
  
  /* Set extraction flag for groups if ancestors of extracted variables */
  for(grp_idx=0;grp_idx<trv_tbl->nbr;grp_idx++){
    /* For each group ... */
    if(trv_tbl->lst[grp_idx].nco_typ == nco_obj_typ_grp){
      /* Metadata-only containing groups already have flg_mtd set in nco_xtr_mk()
	 Variable ancestry may not affect such groups, especially if they are leaf groups
	 Set extraction flag to True (then continue) iff matching groups contain only metadata
	 Otherwise set initialize extraction flag to False and overwrite later based on descendent variables */
      if((trv_tbl->lst[grp_idx].flg_xtr=(!trv_tbl->lst[grp_idx].flg_xcl && trv_tbl->lst[grp_idx].flg_mtd))) continue;
      if(!strcmp(trv_tbl->lst[grp_idx].grp_nm_fll,sls_sng)){
        /* Manually mark root group as extracted because matching algorithm below fails for root group 
	   (it looks for "//" in variable names) */
        trv_tbl->lst[grp_idx].flg_xtr=True;
        continue;
      } /* endif root group */
      grp_fll_sls=(char *)strdup(trv_tbl->lst[grp_idx].nm_fll);
      grp_fll_sls=(char *)nco_realloc(grp_fll_sls,(trv_tbl->lst[grp_idx].nm_fll_lng+2L)*sizeof(char));
      strcat(grp_fll_sls,sls_sng);
      /* ... loop through ... */
      for(unsigned idx_var=0;idx_var<trv_tbl->nbr;idx_var++){
        /* ... all variables to be extracted ... */
        if(trv_tbl->lst[idx_var].nco_typ == nco_obj_typ_var && trv_tbl->lst[idx_var].flg_xtr){
          /* ... finds that full path to current group is contained in an extracted variable path ... */
          if((sbs_srt=strstr(trv_tbl->lst[idx_var].nm_fll,grp_fll_sls))){
            flg_pth_srt_bnd=False;
            flg_pth_end_bnd=False;
            /* ... and _begins_ a full group path of that variable ... */
            if(sbs_srt == trv_tbl->lst[idx_var].nm_fll) flg_pth_srt_bnd=True;
            /* Match ends on path component boundary, directly on a slash, because we added slash before matching */
            flg_pth_end_bnd=True;
            if(flg_pth_srt_bnd && flg_pth_end_bnd){
              /* ... and mark _only_ those groups for extraction... */
              trv_tbl->lst[grp_idx].flg_xtr=True;
              break;
            } /* endif */
          } /* endif full group path */
        } /* endif extracted variable */ 
      } /* end loop over idx_var */
      if(grp_fll_sls) grp_fll_sls=(char *)nco_free(grp_fll_sls);
    } /* endif group */
  } /* end loop over grp_idx */

  /* Current status of flg_xtr for groups is that it has been set 
     1. For all user-specified groups
     2. For default (non user-specified) groups
     3. For metadata-only groups that do not conflict with the above
     What is lacking here is extraction flags for ancestors of all extracted groups
     Ancestor flags were set in nco_xtr_mk() but must be discarded and reset
     for same reasons that group flags in nco_xtr_mk() are unreliable for final list.
     This loop ensures groups are marked for extraction if any descendents are marked
     Mainly (only?) this catches ancestors of metadata only groups
     This loop is not necessary for _copying_ files because nco_xtr_dfn() algorithm handles ancestors
     This loop _is_ necessary for _printing_ files because nco_grp_prn() algorithm does not handle ancestors
     Set extraction flag for groups if ancestors of extracted groups */

  for(grp_idx=0;grp_idx<trv_tbl->nbr;grp_idx++){
    /* For each group that is not yet on extraction list ... */
    if(trv_tbl->lst[grp_idx].nco_typ == nco_obj_typ_grp && !trv_tbl->lst[grp_idx].flg_xtr){
      grp_fll_sls=(char *)strdup(trv_tbl->lst[grp_idx].nm_fll);
      grp_fll_sls=(char *)nco_realloc(grp_fll_sls,(trv_tbl->lst[grp_idx].nm_fll_lng+2L)*sizeof(char));
      strcat(grp_fll_sls,sls_sng);
      /* Search for its path as a component of an extracted group path */
      for(obj_idx=0;obj_idx<trv_tbl->nbr;obj_idx++){
        if(trv_tbl->lst[obj_idx].nco_typ == nco_obj_typ_grp && trv_tbl->lst[obj_idx].flg_xtr){
          if((sbs_srt=strstr(trv_tbl->lst[obj_idx].nm_fll,grp_fll_sls))){
            flg_pth_srt_bnd=False;
            flg_pth_end_bnd=False;
            /* Ancestor groups must match start of extracted group */
            if(sbs_srt == trv_tbl->lst[obj_idx].nm_fll) flg_pth_srt_bnd=True;
            /* Match ends on path component boundary, directly on a slash, because we added slash before matching */
            flg_pth_end_bnd=True;
            if(flg_pth_srt_bnd && flg_pth_end_bnd){
              trv_tbl->lst[grp_idx].flg_ncs=True;
              trv_tbl->lst[grp_idx].flg_xtr=True;
              continue;
            } /* endif current group is ancestor of extracted group */
          } /* endif current group may be ancestor of extracted group */
        } /* endif extracted group */
      } /* end loop over obj_idx */
      if(grp_fll_sls) grp_fll_sls=(char *)nco_free(grp_fll_sls);
    } /* endif group */
  } /* end loop over grp_idx */

} /* end nco_xtr_grp_mrk() */

void
nco_xtr_dfn                          /* [fnc] Define extracted groups, variables, and attributes in output file */
(const int nc_id,                    /* I [ID] netCDF input file ID */
 const int nc_out_id,                /* I [ID] netCDF output file ID */
 const cnk_sct * const cnk,          /* I [sct] Chunking structure */
 const int dfl_lvl,                  /* I [enm] Deflate level [0..9] */
 const gpe_sct * const gpe,          /* I [sct] GPE structure */
 const md5_sct * const md5,          /* I [sct] MD5 configuration */
 const nco_bool CPY_GRP_METADATA,    /* I [flg] Copy group metadata (attributes) */
 const nco_bool CPY_VAR_METADATA,    /* I [flg] Copy variable metadata (attributes) */
 const nco_bool RETAIN_ALL_DIMS,     /* I [flg] Retain all dimensions */
 const int nco_pck_plc,              /* I [enm] Packing policy */
 const char * const rec_dmn_nm,      /* I [sng] Record dimension name */
 trv_tbl_sct * const trv_tbl)        /* I/O [sct] GTT (Group Traversal Table) */
{
  /* Purpose: Define groups, variables, and attributes in output file
     rec_dmn_nm, if any, is name requested for (netCDF3) sole record dimension */

  const char fnc_nm[]="nco_xtr_dfn()"; /* [sng] Function name */

  char *grp_out_fll;                   /* [sng] Group name */

  dmn_cmn_sct *dmn_cmn_out;            /* [sct] List of all dimensions in output file (used for RETAIN_ALL_DIMS) */
  gpe_nm_sct *gpe_nm;                  /* [sct] GPE name duplicate check array */

  int fl_fmt;                          /* [enm] netCDF file format */
  int grp_id;                          /* [ID] Group ID in input file */
  int grp_out_id;                      /* [ID] Group ID in output file */ 
  int nbr_gpe_nm;                      /* [nbr] Number of GPE entries */
  int var_out_id;                      /* [ID] Variable ID in output file */
  int nco_prg_id;                      /* [enm] Program ID */
  int nbr_dmn_cmn_out;                 /* [sct] Number of all dimensions in output file (used for RETAIN_ALL_DIMS) */

  nco_bool PCK_ATT_CPY;                /* [flg] Copy attributes "scale_factor", "add_offset" */

  nco_prg_id=nco_prg_id_get(); 
  nbr_gpe_nm=0;
  gpe_nm=NULL;
  nbr_dmn_cmn_out=0;
  dmn_cmn_out=NULL;

  /* Get file format */
  (void)nco_inq_format(nc_out_id,&fl_fmt);

  /* Isolate extra complexity of copying group metadata */
  if(CPY_GRP_METADATA){
    /* Extraction flag for groups was set in nco_xtr_grp_mrk() 
       This loop defines those groups in output file and copies their metadata */
    for(unsigned idx_tbl=0;idx_tbl<trv_tbl->nbr;idx_tbl++){
      trv_sct grp_trv=trv_tbl->lst[idx_tbl];

      /* If object is group ancestor of extracted variable */
      if(grp_trv.nco_typ == nco_obj_typ_grp && grp_trv.flg_xtr){

        /* Obtain group ID */
        (void)nco_inq_grp_full_ncid(nc_id,grp_trv.grp_nm_fll,&grp_id);

        /* ncge groups require special handling */ 
	if(nco_prg_id == ncge && grp_trv.flg_nsm_prn){
	  /* Ensemble parent groups */
	  if(trv_tbl->nsm_sfx){
	    /* Define new name by appending suffix (e.g., /cesm + _avg) */
	    char *nm_fll_sfx=nco_bld_nsm_sfx(grp_trv.grp_nm_fll_prn,trv_tbl);
	    /* Use then delete new name */
	    if(gpe) grp_out_fll=nco_gpe_evl(gpe,nm_fll_sfx); else grp_out_fll=(char *)strdup(nm_fll_sfx);
	    nm_fll_sfx=(char *)nco_free(nm_fll_sfx);
	  }else{
	    if(gpe) grp_out_fll=nco_gpe_evl(gpe,grp_trv.grp_nm_fll_prn); else grp_out_fll=(char *)strdup(grp_trv.grp_nm_fll_prn);
	  } /* !nsm_sfx */
	}else if(nco_prg_id == ncge && grp_trv.flg_nsm_mbr){
	  /* Ensemble member groups are not written to output */
	  continue;
	}else{
	  /* Regular (non-ensemble-related) group (within ncge or any other operator) */
	  if(gpe) grp_out_fll=nco_gpe_evl(gpe,grp_trv.grp_nm_fll); else grp_out_fll=(char *)strdup(grp_trv.grp_nm_fll);
	} /* !flg_nsm */

        /* If output group does not exist, create it */
        if(nco_inq_grp_full_ncid_flg(nc_out_id,grp_out_fll,&grp_out_id)) nco_def_grp_full(nc_out_id,grp_out_fll,&grp_out_id);

        /* Copy group attributes */
        if(grp_trv.nbr_att){
          PCK_ATT_CPY=True;
          (void)nco_att_cpy(grp_id,grp_out_id,NC_GLOBAL,NC_GLOBAL,PCK_ATT_CPY);
        } /* grp_trv.nbr_att */

        /* Memory management after current extracted group */
        if(grp_out_fll) grp_out_fll=(char *)nco_free(grp_out_fll);

      } /* end if group and flg_xtr */
    } /* end loop over traversal table */
  } /* !CPY_GRP_METADATA */

  /* Define variables */
  for(unsigned idx_tbl=0;idx_tbl<trv_tbl->nbr;idx_tbl++){
    trv_sct var_trv=trv_tbl->lst[idx_tbl];

    /* If object is an extracted variable... */
    if(var_trv.nco_typ == nco_obj_typ_var && var_trv.flg_xtr){

      /* Obtain group ID */
      (void)nco_inq_grp_full_ncid(nc_id,var_trv.grp_nm_fll,&grp_id);

      /* Transform ensemble member group paths as above */
      if(nco_prg_id == ncge && var_trv.flg_nsm_mbr){
	if(trv_tbl->nsm_sfx){
	  /* Define new name by appending suffix (e.g., /cesm + _avg) */
	  char *nm_fll_sfx=nco_bld_nsm_sfx(var_trv.grp_nm_fll_prn,trv_tbl);
	  /* Use then delete new name */
	  if(gpe) grp_out_fll=nco_gpe_evl(gpe,nm_fll_sfx); else grp_out_fll=(char *)strdup(nm_fll_sfx);
	  nm_fll_sfx=(char *)nco_free(nm_fll_sfx);
	}else{ /* Non-suffix case */
	  if(gpe) grp_out_fll=nco_gpe_evl(gpe,var_trv.nsm_nm); else grp_out_fll=(char *)strdup(var_trv.nsm_nm);
	} /* !trv_tbl->nsm_sfx */
      }else{
	/* Variable not in ensembles */
	if(gpe) grp_out_fll=nco_gpe_evl(gpe,var_trv.grp_nm_fll); else grp_out_fll=(char *)strdup(var_trv.grp_nm_fll);
      } /* !ncge */

      /* If output group does not exist, create it */
      if(nco_inq_grp_full_ncid_flg(nc_out_id,grp_out_fll,&grp_out_id)) nco_def_grp_full(nc_out_id,grp_out_fll,&grp_out_id);

      if(nco_prg_id == ncge){
        /* Is requested variable already in output file?
	   fxm: Seems wasteful. Can ncge restrict definition to only variables in template group? i.e., satisfying both flg_xtr and flg_nsm_tpl */
        int rcd=nco_inq_varid_flg(grp_out_id,var_trv.nm,&var_out_id);
        /* Yes, get outta' Dodge... avoid GPE failure on duplicate definition */
        if(rcd == NC_NOERR) continue;
      } /* ncge */

      /* Detect duplicate GPE names in advance, then exit() with helpful error */
      if(gpe) nco_gpe_chk(grp_out_fll,var_trv.nm,&gpe_nm,&nbr_gpe_nm);

      /* Define variable in output file */
      var_out_id=nco_cpy_var_dfn_trv(nc_id,nc_out_id,cnk,grp_out_fll,dfl_lvl,gpe,rec_dmn_nm,&var_trv,&dmn_cmn_out,&nbr_dmn_cmn_out,trv_tbl);

      /* Copy variable's attributes */
      if(CPY_VAR_METADATA){
        int var_id;        /* [id] Variable ID */
        var_sct *var_prc;  /* [sct] Variable to process */

        /* Obtain group ID */
        (void)nco_inq_grp_full_ncid(nc_id,var_trv.grp_nm_fll,&grp_id);

        /* Get variable ID */
        (void)nco_inq_varid(grp_id,var_trv.nm,&var_id);

        /* Allocate variable structure and fill with metadata */
        var_prc=nco_var_fll_trv(grp_id,var_id,&var_trv,trv_tbl);     

        PCK_ATT_CPY=nco_pck_cpy_att(nco_prg_id,nco_pck_plc,var_prc);

        (void)nco_att_cpy(grp_id,grp_out_id,var_id,var_out_id,PCK_ATT_CPY);

        for(int idx_dmn=0;idx_dmn<var_prc->nbr_dim;idx_dmn++){
          var_prc->dim[idx_dmn]->xrf=(dmn_sct *)nco_dmn_free(var_prc->dim[idx_dmn]->xrf);
          var_prc->dim[idx_dmn]=(dmn_sct *)nco_dmn_free(var_prc->dim[idx_dmn]);   
        } /* !idx_dmn */

        var_prc=(var_sct *)nco_var_free(var_prc);
      } /* !CPY_VAR_METADATA */

      /* Pre-allocate space for MD5 attributes */
      if(md5){
        if(md5->wrt){
          /* Save time with netCDF3 files by pre-allocating header space */
          aed_sct aed_md5;
          char md5_dgs_hxd_sng_ram[]="01234567890123456789012345678901"; /* [sng] Placeholder name for actual digest */
          aed_md5.att_nm=md5->att_nm;
          aed_md5.var_nm=var_trv.nm;
          aed_md5.id=var_out_id;
          aed_md5.sz=NCO_MD5_DGS_SZ*2L;
          aed_md5.type=NC_CHAR;
          aed_md5.val.cp=md5_dgs_hxd_sng_ram;
          aed_md5.mode=aed_overwrite;
          (void)nco_aed_prc(grp_out_id,var_out_id,aed_md5);
        } /* !wrt */
      } /* !md5 */

      /* Write PPC attribute */
      if(var_trv.ppc != NC_MAX_INT){
	aed_sct aed_ppc;
	char att_nm_dsd[]="least_significant_digit";
	char att_nm_nsd[]="number_of_significant_digits";
	int ppc_old;
	int rcd;
	if(var_trv.flg_nsd) aed_ppc.att_nm=att_nm_nsd; else aed_ppc.att_nm=att_nm_dsd;
	aed_ppc.var_nm=var_trv.nm;
	aed_ppc.id=var_out_id;
	aed_ppc.val.ip=&var_trv.ppc;
	rcd=nco_inq_att_flg(grp_out_id,aed_ppc.id,aed_ppc.att_nm,&aed_ppc.type,&aed_ppc.sz);
	if(rcd != NC_NOERR){
	  /* No PPC attribute yet exists */
	  aed_ppc.sz=1L;
	  aed_ppc.type=NC_INT;
	  aed_ppc.mode=aed_create;
	  (void)nco_aed_prc(grp_out_id,var_out_id,aed_ppc);
	}else{
	  if(aed_ppc.sz == 1L && aed_ppc.type == NC_INT){
	    /* Conforming PPC attribute already exists, only replace with new value if rounder */
	    (void)nco_get_att(grp_out_id,aed_ppc.id,aed_ppc.att_nm,&ppc_old,NC_INT);
	    if(var_trv.ppc < ppc_old){
	      aed_ppc.mode=aed_modify;
	      (void)nco_aed_prc(grp_out_id,var_out_id,aed_ppc);
	    } /* endif */
	  }else{ /* !conforming */
	    (void)fprintf(stderr,"%s: WARNING Non-conforming %s attribute found in variable %s, skipping...\n",nco_prg_nm_get(),aed_ppc.att_nm,var_trv.nm_fll);
	  }  /* !conforming */
	} /* !rcd */
      } /* !PPC */

      /* Memory management after current extracted variable */
      if(grp_out_fll) grp_out_fll=(char *)nco_free(grp_out_fll);
    } /* !flg_xtr */
  } /* !idx_tbl */

  /* Retain all dimensions */
  if(RETAIN_ALL_DIMS) (void)nco_rad(nc_out_id,nbr_dmn_cmn_out,dmn_cmn_out,trv_tbl);

  /* Memory management for GPE names */
  for(int idx=0;idx<nbr_gpe_nm;idx++) gpe_nm[idx].var_nm_fll=(char *)nco_free(gpe_nm[idx].var_nm_fll);

  for(int idx=0;idx<nbr_dmn_cmn_out;idx++) dmn_cmn_out[idx].nm_fll=(char *)nco_free(dmn_cmn_out[idx].nm_fll);
  dmn_cmn_out=(dmn_cmn_sct *)nco_free(dmn_cmn_out);

  /* Print extraction list in developer mode */
  if(nco_dbg_lvl_get() == nco_dbg_old) (void)trv_tbl_prn_xtr(trv_tbl,fnc_nm);

} /* end nco_xtr_dfn() */

void
nco_chk_nan                           /* [fnc] Check file for NaNs */
(const int nc_id,                     /* I [ID] netCDF input file ID */
 const trv_tbl_sct * const trv_tbl)   /* I [sct] GTT (Group Traversal Table) */
{
  /* Purpose: Check file for NaNs 
     ncks --chk_nan ~/nco/data/in.nc */

  const char fnc_nm[]="nco_chk_nan()"; /* [sng] Function name */

  char var_nm[NC_MAX_NAME+1]; /* [sng] Variable name (used for validation only) */ 

  int grp_id; /* [ID] Group ID where variable resides (passed to MSA) */

  lmt_msa_sct **lmt_msa=NULL_CEWI; /* [sct] MSA Limits for only for variable dimensions  */          
  lmt_sct **lmt=NULL_CEWI; /* [sct] Auxiliary Limit used in MSA */

  long lmn; /* [nbr] Index to print variable data */

  var_sct *var=NULL_CEWI; /* [sct] Variable structure */

  for(unsigned idx_tbl=0;idx_tbl<trv_tbl->nbr;idx_tbl++){
    trv_sct var_trv=trv_tbl->lst[idx_tbl]; 
    if(var_trv.flg_xtr && var_trv.nco_typ == nco_obj_typ_var && (var_trv.var_typ == NC_FLOAT || var_trv.var_typ == NC_DOUBLE)){
      if(nco_dbg_lvl_get() >= nco_dbg_var) (void)fprintf(stdout,"%s: DEBUG %s checking variable %s for NaNs...\n",nco_prg_nm_get(),fnc_nm,var_trv.nm_fll);

      /* NB: Following block to get variable copied from nco_prn_var_val_trv() */
      /* Obtain group ID where variable is located */
      (void)nco_inq_grp_full_ncid(nc_id,var_trv.grp_nm_fll,&grp_id);
      /* malloc space */
      var=(var_sct *)nco_malloc(sizeof(var_sct));
      /* Set defaults */
      (void)var_dfl_set(var);
      /* Get ID for requested variable */
      var->nm=(char *)strdup(var_trv.nm);
      var->nc_id=grp_id;
      (void)nco_inq_varid(grp_id,var_trv.nm,&var->id);
      /* Get type of variable (get also name and number of dimensions for validation against parameter object) */
      (void)nco_inq_var(grp_id,var->id,var_nm,&var->type,&var->nbr_dim,(int *)NULL,(int *)NULL);
      /* Scalars */
      if(var->nbr_dim == 0){
	var->sz=1L;
	var->val.vp=nco_malloc(nco_typ_lng_udt(nc_id,var->type));
	/* Block is critical/thread-safe for identical/distinct grp_id's */
	{ /* begin potential OpenMP critical */
	  (void)nco_get_var1(grp_id,var->id,0L,var->val.vp,var->type);
	} /* end potential OpenMP critical */
      }else{ /* ! Scalars */
	/* Allocate local MSA */
	lmt_msa=(lmt_msa_sct **)nco_malloc(var_trv.nbr_dmn*sizeof(lmt_msa_sct *));
	lmt=(lmt_sct **)nco_malloc(var_trv.nbr_dmn*sizeof(lmt_sct *));
	/* Copy from table to local MSA */
	(void)nco_cpy_msa_lmt(&var_trv,&lmt_msa);
	/* Call super-dooper recursive routine */
	var->val.vp=nco_msa_rcr_clc((int)0,var->nbr_dim,lmt,lmt_msa,var);
      } /* ! Scalars */
      /* Refresh missing value attribute, if any */
      var->has_mss_val=nco_mss_val_get(var->nc_id,var);
      
      switch(var->type){
      case NC_FLOAT:
	for(lmn=0;lmn<var->sz;lmn++){
	  if(isnan(var->val.fp[lmn])){
	    if(nco_dbg_lvl_get() >= nco_dbg_quiet) (void)fprintf(stdout,"%s: INFO %s reports variable %s has first NaNf at hyperslab element %ld, exiting now.\n",nco_prg_nm_get(),fnc_nm,var_trv.nm_fll,lmn);
	    nco_exit(EXIT_FAILURE);
	  } /* !isnan */
	} /* !lmn */
	    break;
      case NC_DOUBLE:
	for(lmn=0;lmn<var->sz;lmn++){
	  if(isnan(var->val.dp[lmn])){
	    if(nco_dbg_lvl_get() >= nco_dbg_quiet) (void)fprintf(stdout,"%s: INFO %s reports variable %s has first NaN at hyperslab element %ld, exiting now.\n",nco_prg_nm_get(),fnc_nm,var_trv.nm_fll,lmn);
	    nco_exit(EXIT_FAILURE);
	  } /* !isnan */
      } /* !lmn */
	  break;
      } /* end switch */
      var=nco_var_free(var);
    } /* !var */
  } /* !idx_tbl */
  return;
} /* end nco_chk_nan() */

void
nco_xtr_wrt                           /* [fnc] Write extracted data to output file */
(const int nc_id_in,                  /* I [ID] netCDF input file ID */
 const int nc_id_out,                 /* I [ID] netCDF output file ID */
 const gpe_sct * const gpe,           /* I [sct] GPE structure */
 FILE * const fp_bnr,                 /* I [fl] Unformatted binary output file handle */
 const md5_sct * const md5,           /* I [flg] MD5 Configuration */
 const nco_bool HAVE_LIMITS,          /* I [flg] Dimension limits exist */
 const trv_tbl_sct * const trv_tbl)   /* I [sct] GTT (Group Traversal Table) */
{
  /* Purpose: Write extracted variables to output file */

  const char fnc_nm[]="nco_xtr_wrt()"; /* [sng] Function name */

  int fl_out_fmt; /* [enm] File format */

  nco_bool USE_MM3_WORKAROUND=False; /* [flg] Faster copy on Multi-record Multi-variable netCDF3 files */

  (void)nco_inq_format(nc_id_out,&fl_out_fmt);

  /* 20120309 Special case to improve copy speed on large blocksize filesystems (MM3s) */
  USE_MM3_WORKAROUND=nco_use_mm3_workaround(nc_id_in,fl_out_fmt);
  /* Workaround currently copies variable-at-a-time or record-at-a-time does not handle limits
     The workaround could be modified to handle limits 
     Until then, though, the workaround is incompatible with limits */
  if(HAVE_LIMITS) USE_MM3_WORKAROUND=False; 

  if(USE_MM3_WORKAROUND){  
    int fix_nbr; /* [nbr] Number of fixed-length variables */
    int rec_nbr; /* [nbr] Number of record variables */
    int xtr_nbr; /* [nbr] Number of extracted variables */
    int idx_var; /* [idx] */

    nm_id_sct **fix_lst=NULL; /* [sct] Fixed-length variables to be extracted */
    nm_id_sct **rec_lst=NULL; /* [sct] Record variables to be extracted */
    nm_id_sct *xtr_lst=NULL; /* [sct] Variables to be extracted */

    if(nco_dbg_lvl_get() >= nco_dbg_fl) (void)fprintf(stderr,"%s: INFO Using MM3-workaround to hasten copying of record variables\n",nco_prg_nm_get());

    /* Convert extraction list from traversal table to nm_id_sct format to re-use old code */
    xtr_lst=nco_trv_tbl_nm_id(nc_id_in,nc_id_out,gpe,&xtr_nbr,trv_tbl);

    /* Split list into fixed-length and record variables */
    (void)nco_var_lst_fix_rec_dvd(nc_id_in,xtr_lst,xtr_nbr,&fix_lst,&fix_nbr,&rec_lst,&rec_nbr);

    /* Copy fixed-length data variable-by-variable */
    for(idx_var=0;idx_var<fix_nbr;idx_var++){
      if(nco_dbg_lvl_get() >= nco_dbg_var && !fp_bnr) (void)fprintf(stderr,"%s, ",fix_lst[idx_var]->nm);
      if(nco_dbg_lvl_get() >= nco_dbg_var) (void)fflush(stderr);
      (void)nco_cpy_var_val(fix_lst[idx_var]->grp_id_in,fix_lst[idx_var]->grp_id_out,fp_bnr,md5,fix_lst[idx_var]->nm,trv_tbl);
    } /* end loop over idx_var */

    /* Copy record data record-by-record */
    (void)nco_cpy_rec_var_val(nc_id_in,fp_bnr,md5,rec_lst,rec_nbr,trv_tbl);

    /* Extraction lists no longer needed */
    if(fix_lst) fix_lst=(nm_id_sct **)nco_free(fix_lst);
    if(rec_lst) rec_lst=(nm_id_sct **)nco_free(rec_lst);
    if(xtr_lst) xtr_lst=nco_nm_id_lst_free(xtr_lst,xtr_nbr);

  }else{ /* !USE_MM3_WORKAROUND */

    for(unsigned idx_tbl=0;idx_tbl<trv_tbl->nbr;idx_tbl++){
      trv_sct trv=trv_tbl->lst[idx_tbl];

      /* If object is an extracted variable... */ 
      if(trv.nco_typ == nco_obj_typ_var && trv.flg_xtr){
        int grp_id_in;
        int grp_id_out;
        char *grp_out_fll;
        /* Get input group ID */
        (void)nco_inq_grp_full_ncid(nc_id_in,trv_tbl->lst[idx_tbl].grp_nm_fll,&grp_id_in);
        /* Edit group name for output */
        if(gpe) grp_out_fll=nco_gpe_evl(gpe,trv_tbl->lst[idx_tbl].grp_nm_fll); else grp_out_fll=(char *)strdup(trv_tbl->lst[idx_tbl].grp_nm_fll);
        /* Get output group ID */
        (void)nco_inq_grp_full_ncid(nc_id_out,grp_out_fll,&grp_id_out);

        /* Copy variable data from input netCDF file to output netCDF file */
        (void)nco_cpy_var_val_mlt_lmt_trv(grp_id_in,grp_id_out,fp_bnr,md5,&trv); 

        /* Memory management */
        if(grp_out_fll) grp_out_fll=(char *)nco_free(grp_out_fll);

      } /* endif */

    } /* end loop over idx_tbl */
  } /* !USE_MM3_WORKAROUND */

  /* Print extraction list in developer mode */
  if(nco_dbg_lvl_get() == nco_dbg_old) (void)trv_tbl_prn_xtr(trv_tbl,fnc_nm);

} /* end nco_xtr_wrt() */

void                          
nco_prn_dmn_grp /* [fnc] Print dimensions for a group  */
(const int nc_id, /* I [ID] File ID */
 const char * const grp_nm_fll) /* I [sng] Full name of group */
{
  char dmn_nm[NC_MAX_NAME+1];     /* [sng] Dimension name */ 

  int *dmn_ids;                 /* [nbr] Dimensions IDs array */
  int *dmn_ids_ult;             /* [nbr] Unlimited dimensions IDs array */
  int grp_id;                   /* [ID]  Group ID */
  int nbr_dmn;                  /* [nbr] Number of dimensions */
  int nbr_dmn_ult;              /* [nbr] Number of unlimited dimensions */

  long dmn_sz;                  /* [nbr] Dimension size */

  /* Obtain group ID */
  (void)nco_inq_grp_full_ncid(nc_id,grp_nm_fll,&grp_id);

  (void)nco_inq_unlimdims(grp_id,&nbr_dmn_ult,NULL);

  /* Alloc dimension IDs array */
  dmn_ids_ult=(int *)nco_malloc(nbr_dmn_ult*sizeof(int));

  /* Obtain unlimited dimensions for group */
  (void)nco_inq_unlimdims(grp_id,&nbr_dmn_ult,dmn_ids_ult);

  /* Obtain dimensions IDs for group */
  dmn_ids=nco_dmn_malloc(nc_id,grp_nm_fll,&nbr_dmn);

  /* List dimensions using obtained group ID */
  for(int dnm_idx=0;dnm_idx<nbr_dmn;dnm_idx++){
    nco_bool is_rec_dim=False;
    (void)nco_inq_dim(grp_id,dmn_ids[dnm_idx],dmn_nm,&dmn_sz);

    /* Check if dimension is unlimited (record dimension) */
    for(int dnm_ult_idx=0;dnm_ult_idx<nbr_dmn_ult;dnm_ult_idx++){ 
      if(dmn_ids[dnm_idx] == dmn_ids_ult[dnm_ult_idx]){ 
        is_rec_dim=True;
        (void)fprintf(stdout,"Record dimension name, size, ID = %s, %li, %d\n",dmn_nm,dmn_sz,dmn_ids[dnm_idx]);
      } /* end if */
    } /* end dnm_ult_idx dimensions */

    /* An unlimited ID was not matched, so dimension is a plain vanilla dimension */
    if(!is_rec_dim) (void)fprintf(stdout,"Fixed dimension name, size, ID = %s, %li, %d\n",dmn_nm,dmn_sz,dmn_ids[dnm_idx]);

  } /* end dnm_idx dimensions */

  dmn_ids=(int *)nco_free(dmn_ids);
  dmn_ids_ult=(int *)nco_free(dmn_ids_ult);
} /* end nco_prn_dmn() */

void                          
nco_bld_dmn_ids_trv                   /* [fnc] Build dimension info for all variables */
(const int nc_id,                     /* I [ID] netCDF file ID */
 trv_tbl_sct * const trv_tbl)         /* I/O [sct] GTT (Group Traversal Table) */
{
  /* Purpose: a netCDF4 variable can have its dimensions located anywhere below *in the group path*
  Construction of this list *must* be done after traversal table is build in nco_grp_itr(),
  where we know the full picture of the file tree
  Compare unique dimension IDs from variables with unique dimension IDs from groups 
  */

  const char fnc_nm[]="nco_bld_dmn_ids_trv()"; /* [sng] Function name  */

  /* Loop objects  */
  for(unsigned idx_var=0;idx_var<trv_tbl->nbr;idx_var++){

    /* Filter variables  */
    if(trv_tbl->lst[idx_var].nco_typ == nco_obj_typ_var){
      trv_sct var_trv=trv_tbl->lst[idx_var];   

      if(nco_dbg_lvl_get() == nco_dbg_old){
        (void)fprintf(stdout,"%s: INFO %s reports variable dimensions\n",nco_prg_nm_get(),fnc_nm);
        (void)fprintf(stdout,"%s:",var_trv.nm_fll); 
        (void)fprintf(stdout," %d dimensions:\n",var_trv.nbr_dmn);
      } /* endif dbg */

      /* Full dimension names for each variable */
      for(int idx_dmn_var=0;idx_dmn_var<var_trv.nbr_dmn;idx_dmn_var++){

        int var_dmn_id=var_trv.var_dmn[idx_dmn_var].dmn_id;

        /* Get unique dimension object from unique dimension ID */
        dmn_trv_sct *dmn_trv=nco_dmn_trv_sct(var_dmn_id,trv_tbl);

        if(nco_dbg_lvl_get() == nco_dbg_old){
          (void)fprintf(stdout,"[%d]%s#%d ",idx_dmn_var,var_trv.var_dmn[idx_dmn_var].dmn_nm,var_dmn_id);    
          (void)fprintf(stdout,"<%s>\n",dmn_trv->nm_fll);
        } /* endif dbg */
        if(strcmp(var_trv.var_dmn[idx_dmn_var].dmn_nm,dmn_trv->nm)){

	  /* Test case generates duplicated dimension IDs in netCDF file:
	     
	     ncks -O  -v two_dmn_rec_var in_grp.nc out.nc
	     
	     defines new dimensions for the file, as
	     
	     ID=0 index [0]:</time> 
	     ID=1 index [1]:</lev> 
	     ID=2 index [0]:</g8/lev> 
	     ID=3 index [1]:</g8/vrt_nbr> 
	     ID=4 index [1]:</vrt_nbr> 
	     
	     but the resulting file, when read, has the following IDs
	     
	     dimensions:
	     #0,time = UNLIMITED ; // (10 currently)
	     #1,lev = 3 ;
	     #4,vrt_nbr = 2 ;
	     
	     group: g8 {
	     dimensions:
	     #0,lev = 3 ;
	     #1,vrt_nbr = 2 ;
	     
	     From: "Unidata netCDF Support" <support-netcdf@unidata.ucar.edu>
	     To: <pvicente@uci.edu>
	     Sent: Tuesday, March 12, 2013 5:02 AM
	     Subject: [netCDF #SHH-257980]: Re: [netcdfgroup] Dimensions IDs
	     
	     > Your Ticket has been received, and a Unidata staff member will review it and reply accordingly. Listed below are details of this new Ticket. Please make sure the Ticket ID remains in the Subject: line on all correspondence related to this Ticket.
	     > 
	     >    Ticket ID: SHH-257980
	     >    Subject: Re: [netcdfgroup] Dimensions IDs
	     >    Department: Support netCDF
	     >    Priority: Normal
	     >    Status: Open
	  */
	  
          (void)fprintf(stdout,"%s: INFO %s reports variable <%s> with duplicate dimensions\n",nco_prg_nm_get(),fnc_nm,var_trv.nm_fll);
          (void)fprintf(stdout,"%s: ERROR netCDF file with duplicate dimension IDs detected. Please use netCDF version at least 4.3.0. NB: Simultaneously renaming multiple dimensions with ncrename can trigger this bug with netCDF versions up to 4.6.0.1 (current as of 20180201).\n",nco_prg_nm_get());
          (void)nco_prn_trv_tbl(nc_id,trv_tbl);
          nco_exit(EXIT_FAILURE);
        }

        /* Store full dimension name */
        trv_tbl->lst[idx_var].var_dmn[idx_dmn_var].dmn_nm_fll=strdup(dmn_trv->nm_fll);

#ifdef DEBUG_LEAKS
        assert(trv_tbl->lst[idx_var].var_dmn[idx_dmn_var].grp_nm_fll == NULL);
#endif

        /* Store full group name where dimension is located */
        trv_tbl->lst[idx_var].var_dmn[idx_dmn_var].grp_nm_fll=strdup(dmn_trv->grp_nm_fll);

        /* Mark as record dimension if so */
        trv_tbl->lst[idx_var].var_dmn[idx_dmn_var].is_rec_dmn=dmn_trv->is_rec_dmn;
      }

    } /* Filter variables */
  } /* Variables */

} /* end nco_blb_dmn_ids_trv() */

int                                    /* [rcd] Return code */
nco_grp_itr                            /* [fnc] Populate traversal table by examining, recursively, subgroups of parent */
(const int grp_id,                     /* I [ID] Group ID */
 char * const grp_nm_fll_prn,          /* I [sng] Absolute group name of parent (path) */
 char * const grp_nm_fll,              /* I [sng] Absolute group name (path) */
 trv_tbl_sct * const trv_tbl)          /* I/O [sct] GTT (Group Traversal Table) */
{
  /* Purpose: Populate traversal table by examining, recursively, subgroups of parent */
  const char fnc_nm[]="nco_grp_itr()"; /* [sng] Function name */
  const char sls_sng[]="/";        /* [sng] Slash string */

  char grp_nm[NC_MAX_NAME+1L];     /* [sng] Group name */
  char var_nm[NC_MAX_NAME+1L];     /* [sng] Variable name */ 
  char dmn_nm[NC_MAX_NAME+1L];     /* [sng] Dimension name */ 
  char rec_nm[NC_MAX_NAME+1L];     /* [sng] Record dimension name */ 
  char typ_nm[NC_MAX_NAME+1L];     /* [sng] Type name used in CDL "types" declaration (e.g., "vlen_t") */
  char *var_nm_fll;                /* [sng] Full path for variable */
  char *dmn_nm_fll;                /* [sng] Full path for dimension */
  char *sls_psn;                   /* [sng] Current position of group path search */

  const int flg_prn=0;             /* [flg] All the dimensions in all parent groups will also be retrieved */    

  int *dmn_ids_grp=NULL;           /* [ID] Dimension IDs array for group */ 
  int *dmn_ids_grp_ult=NULL;       /* [ID] Unlimited (record) dimensions IDs array for group */
  int *dmn_id_var=NULL;            /* [ID] Dimensions IDs array for variable */
  int *grp_ids;                    /* [ID] Sub-group IDs array */  
  int cls_typ; /* [enm] netCDF class type, same as var_typ except contiguous from 0..16 */
  int grp_dpt=0;                   /* [nbr] Depth of group (root = 0) */
  int nbr_att;                     /* [nbr] Number of attributes */
  int nbr_dmn_grp;                 /* [nbr] Number of dimensions for group */
  int nbr_dmn_var;                 /* [nbr] Number of dimensions for variable */
  int nbr_grp;                     /* [nbr] Number of sub-groups in this group */
  int nbr_typ;                     /* [nbr] Number of user-defined types defined in this group */
  int nbr_rec;                     /* [nbr] Number of record dimensions in this group */
  int nbr_var;                     /* [nbr] Number of variables */
  int rcd=NC_NOERR;                /* [rcd] Return code */

  long dmn_sz;                     /* [nbr] Dimension size */ 
  long rec_sz;                     /* [nbr] Record dimension size */ 

  nc_type bs_typ; /* [enm] netCDF atomic type underlying vlen and enum types */
  nc_type var_typ;                 /* O [enm] NetCDF type */

  nco_obj_typ obj_typ;             /* [enm] Object type (group or variable) */

  size_t fld_nbr; /* [nbr] Number of fields in enum and compound types */
  size_t typ_sz; /* [B] Size of user-defined type */

  /* Get all information for this group */

  /* Get group name */
  rcd+=nco_inq_grpname(grp_id,grp_nm);

  /* Get number of sub-groups */
  rcd+=nco_inq_grps(grp_id,&nbr_grp,(int *)NULL);

  /* Get number of user-defined types defined in group */
  rcd+=nco_inq_typeids(grp_id,&nbr_typ,(int *)NULL);

  /* Obtain number of dimensions/variable/attributes for group; NB: ignore record dimension ID */
  rcd+=nco_inq(grp_id,&nbr_dmn_grp,&nbr_var,&nbr_att,(int *)NULL);

  /* Alloc dimension IDs array */
  dmn_ids_grp=(int *)nco_malloc(nbr_dmn_grp*sizeof(int));

  /* Obtain dimensions IDs for group */
  rcd+=nco_inq_dimids(grp_id,&nbr_dmn_grp,dmn_ids_grp,flg_prn);

  rcd+=nco_inq_unlimdims(grp_id,&nbr_rec,NULL);

   /* Alloc dimension IDs array */
  dmn_ids_grp_ult=(int *)nco_malloc(nbr_rec*sizeof(int));

  /* Obtain unlimited dimensions for group */
  rcd+=nco_inq_unlimdims(grp_id,&nbr_rec,dmn_ids_grp_ult);

  /* Compute group depth */
  sls_psn=grp_nm_fll;
  if(!strcmp(grp_nm_fll,sls_sng)) grp_dpt=0; else grp_dpt=1;
  while((sls_psn=strchr(sls_psn+1,'/'))) grp_dpt++;
  if(nco_dbg_lvl_get() >= nco_dbg_io) (void)fprintf(stderr,"%s: INFO %s found group %s at level %d\n",nco_prg_nm_get(),fnc_nm,grp_nm_fll,grp_dpt);

  /* Keep old table size for insertion */
  unsigned int idx;
  idx=trv_tbl->nbr;

  /* Add one more element to GTT */
  trv_tbl->nbr++;
  trv_tbl->lst=(trv_sct *)nco_realloc(trv_tbl->lst,trv_tbl->nbr*sizeof(trv_sct));

  /* Add this element (a group) to table */
  trv_tbl->lst[idx].nco_typ=nco_obj_typ_grp;      /* [enm] netCDF4 object type: group or variable */

  trv_tbl->lst[idx].nm=strdup(grp_nm);            /* [sng] Relative name (i.e., variable name or last component of path name for groups) */
  trv_tbl->lst[idx].grp_nm=strdup(grp_nm);        /* [sng] Group name */
  trv_tbl->lst[idx].nm_lng=strlen(grp_nm);        /* [sng] Length of short name */
  trv_tbl->lst[idx].grp_nm_fll=strdup(grp_nm_fll);/* [sng] Full group name (for groups, same as nm_fll) */
  trv_tbl->lst[idx].nm_fll=strdup(grp_nm_fll);    /* [sng] Fully qualified name (path) */
  trv_tbl->lst[idx].nm_fll_lng=strlen(grp_nm_fll);/* [sng] Length of full name */

  trv_tbl->lst[idx].typ_nm=NULL; /* [sng] Type name used in CDL "types" declaration (e.g., "vlen_t") */
  trv_tbl->lst[idx].cls_typ=NC_NAT; /* [enm] netCDF class type, same as var_typ except contiguous from 0..16 */
  trv_tbl->lst[idx].bs_typ=NC_NAT; /* [enm] netCDF atomic type underlying vlen and enum types */
  trv_tbl->lst[idx].fld_nbr=0L; /* [nbr] Number of fields in enum and compound types */
  trv_tbl->lst[idx].typ_sz=0L; /* [B] Size of user-defined type */

  trv_tbl->lst[idx].flg_cf=False;                 /* [flg] Object matches CF-metadata extraction criteria */
  trv_tbl->lst[idx].flg_crd=False;                /* [flg] Object matches coordinate extraction criteria */
  trv_tbl->lst[idx].flg_dfl=False;                /* [flg] Object meets default subsetting criteria */
  trv_tbl->lst[idx].flg_gcv=False;                /* [flg] Group contains matched variable */
  trv_tbl->lst[idx].flg_mch=False;                /* [flg] Object matches user-specified strings */
  trv_tbl->lst[idx].flg_mtd=False;                /* [flg] Group contains only metadata */
  trv_tbl->lst[idx].flg_ncs=False;                /* [flg] Group is ancestor of specified group or variable */
  trv_tbl->lst[idx].flg_nsx=False;                /* [flg] Object matches intersection criteria */
  trv_tbl->lst[idx].flg_udt=False;                /* [flg] Variable is non-atomic */
  trv_tbl->lst[idx].flg_rgr=False;                /* [flg] Regrid variable */
  trv_tbl->lst[idx].flg_xtn=False;                /* [flg] Extensive variable */
  trv_tbl->lst[idx].flg_mrv=False;                /* [flg] Most-Rapidly-Varying horizontal dimensions variable */ 
  trv_tbl->lst[idx].flg_rcr=False;                /* [flg] Extract group recursively */
  trv_tbl->lst[idx].flg_unn=False;                /* [flg] Object matches union criteria */
  trv_tbl->lst[idx].flg_vfp=False;                /* [flg] Variable matches full path specification */
  trv_tbl->lst[idx].flg_vsg=False;                /* [flg] Variable selected because group matches */
  trv_tbl->lst[idx].flg_xcl=False;                /* [flg] Object matches exclusion criteria */
  trv_tbl->lst[idx].flg_xtr=False;                /* [flg] Extract object */
  trv_tbl->lst[idx].flg_rdr=False;                /* [flg] Variable has dimensions to re-order (ncpdq) */
  trv_tbl->lst[idx].flg_aux=False;                /* [flg] Variable contains auxiliary coordinates */
  trv_tbl->lst[idx].flg_std_att_lat=False;        /* [flg] Variable contains 'standard_name' attribute "latitude" */ 
  trv_tbl->lst[idx].flg_std_att_lon=False;        /* [flg] Variable contains 'standard_name' attribute "longitude" */ 

  trv_tbl->lst[idx].rec_dmn_nm_out=NULL;          /* [sng] Record dimension name, re-ordered */ 
  trv_tbl->lst[idx].grp_dpt=grp_dpt;              /* [nbr] Depth of group (root = 0) */
  trv_tbl->lst[idx].nbr_dmn=nbr_dmn_grp;          /* [nbr] Number of dimensions */
  trv_tbl->lst[idx].nbr_att=nbr_att;              /* [nbr] Number of attributes */
  trv_tbl->lst[idx].nbr_grp=nbr_grp;              /* [nbr] Number of sub-groups (for group) */
  trv_tbl->lst[idx].nbr_rec=nbr_rec;              /* [nbr] Number of record dimensions */
  trv_tbl->lst[idx].nbr_typ=nbr_typ;              /* [nbr] Number of types (for group) */
  trv_tbl->lst[idx].nbr_var=nbr_var;              /* [nbr] Number of variables (for group) */
  trv_tbl->lst[idx].ppc=NC_MAX_INT;               /* [nbr] Precision-preserving compression, i.e., number of total or decimal significant digits */
  trv_tbl->lst[idx].flg_nsd=True;                 /* [flg] PPC is NSD */

  trv_tbl->lst[idx].is_crd_lk_var=nco_obj_typ_err; /* [flg] Is a coordinate-like variable (same as var_sct is_crd_var: crd, 2D, bounds...) */
  trv_tbl->lst[idx].is_crd_var=nco_obj_typ_err;   /* [flg] (For variables only) Is this a coordinate variable? (unique dimension exists in-scope) */
  trv_tbl->lst[idx].is_rec_var=nco_obj_typ_err;   /* [flg] (For variables only) Is a record variable? (is_crd_var must be True) */
  trv_tbl->lst[idx].var_typ=(nc_type)nco_obj_typ_err; /* [enm] (For variables only) NetCDF type */
  trv_tbl->lst[idx].enm_prc_typ=err_typ;          /* [enm] (For variables only) Processing type enumerator */
  trv_tbl->lst[idx].var_typ_out=(nc_type)err_typ; /* [enm] (For variables only) NetCDF type in output file (used by ncflint, ncpdq) */
  if(grp_nm_fll_prn) trv_tbl->lst[idx].grp_nm_fll_prn=strdup(grp_nm_fll_prn); /* [sng] (ncge) Parent group full name */
  else trv_tbl->lst[idx].grp_nm_fll_prn=NULL;
  trv_tbl->lst[idx].flg_nsm_prn=False;            /* [flg] (ncge) Group is, or variable is in, ensemble parent group */
  trv_tbl->lst[idx].flg_nsm_mbr=False;            /* [flg] (ncge) Group is, or variable is in, ensemble member group */  
  trv_tbl->lst[idx].flg_nsm_tpl=False;            /* [flg] Group is, or variable is in, template member group */
  trv_tbl->lst[idx].nsm_nm=NULL;                  /* [sng] (ncge) Ensemble parent group name i.e., full path to ensemble parent */

  /* Variable dimensions. For groups there are no variable dimensions  */
  trv_tbl->lst[idx].var_dmn=NULL;

  /* ncpdq dimension arrays  */
  trv_tbl->lst[idx].dmn_idx_out_in=NULL;
  trv_tbl->lst[idx].dmn_rvr_in=NULL;

  /* Iterate variables for this group */
  for(int idx_var=0;idx_var<nbr_var;idx_var++){

    dmn_id_var=NULL;

    /* Get type of variable and number of dimensions */
    rcd+=nco_inq_var(grp_id,idx_var,var_nm,&var_typ,&nbr_dmn_var,(int *)NULL,&nbr_att);

    /* Alloc dimension IDs array */
    dmn_id_var=(int *)nco_malloc(nbr_dmn_var*sizeof(int));

    /* Get dimension IDs for variable */
    (void)nco_inq_vardimid(grp_id,idx_var,dmn_id_var);

    /* Allocate path buffer and include space for trailing NUL */ 
    var_nm_fll=(char *)nco_malloc(strlen(grp_nm_fll)+strlen(var_nm)+2L);

    /* Initialize path with current absolute group path */
    strcpy(var_nm_fll,grp_nm_fll);

    /* If not root group, concatenate separator */
    if(strcmp(grp_nm_fll,sls_sng)) strcat(var_nm_fll,sls_sng);

    /* Concatenate variable to absolute group path */
    strcat(var_nm_fll,var_nm);

    if(var_typ <= NC_MAX_ATOMIC_TYPE){
      obj_typ=nco_obj_typ_var;
      typ_nm[0]='\0'; /* [sng] Type name used in CDL "types" declaration (e.g., "vlen_t") */
      cls_typ=NC_NAT; /* [enm] netCDF class type, same as var_typ except contiguous from 0..16 */
      bs_typ=NC_NAT; /* [enm] netCDF atomic type underlying vlen and enum types */
      fld_nbr=0L; /* [nbr] Number of fields in enum and compound types */
      typ_sz=0L; /* [B] Size of user-defined type */
    }else{ /* > NC_MAX_ATOMIC_TYPE */
      obj_typ=nco_obj_typ_nonatomic_var;
      if(nco_dbg_lvl_get() >= nco_dbg_var){
        (void)fprintf(stderr,"%s: WARNING NCO only supports netCDF4 atomic-type variables. Variable %s is type %d = %s, and will be ignored in subsequent processing.\n",nco_prg_nm_get(),var_nm_fll,var_typ,nco_typ_sng(var_typ));
      } /* endif */
      rcd+=nco_inq_user_type(grp_id,var_typ,typ_nm,&typ_sz,&bs_typ,&fld_nbr,&cls_typ);
    } /* > NC_MAX_ATOMIC_TYPE */

    /* Keep old table objects size for insertion */
    idx=trv_tbl->nbr;

    /* Add new object to GTT */
    trv_tbl->nbr++;
    trv_tbl->lst=(trv_sct *)nco_realloc(trv_tbl->lst,trv_tbl->nbr*sizeof(trv_sct));

    /* Add this element, a variable, to table. NB: nbr_var, nbr_grp, nbr_typ, flg_rcr not valid here */
    trv_tbl->lst[idx].nco_typ=obj_typ;

    trv_tbl->lst[idx].nm=strdup(var_nm); 
    trv_tbl->lst[idx].nm_lng=strlen(var_nm);
    trv_tbl->lst[idx].grp_nm_fll=strdup(grp_nm_fll);
    trv_tbl->lst[idx].grp_nm=strdup(grp_nm);        
    trv_tbl->lst[idx].nm_fll=strdup(var_nm_fll);
    trv_tbl->lst[idx].nm_fll_lng=strlen(var_nm_fll);  

    trv_tbl->lst[idx].flg_udt=var_typ > NC_MAX_ATOMIC_TYPE ? True : False;
    trv_tbl->lst[idx].typ_nm=var_typ > NC_MAX_ATOMIC_TYPE ? strdup(typ_nm) : NULL; /* [sng] Type name used in CDL "types" declaration (e.g., "vlen_t") */
    trv_tbl->lst[idx].cls_typ=cls_typ; /* [enm] netCDF class type, same as var_typ except contiguous from 0..16 */
    trv_tbl->lst[idx].bs_typ=bs_typ; /* [enm] netCDF atomic type underlying vlen and enum types */
    trv_tbl->lst[idx].fld_nbr=fld_nbr; /* [nbr] Number of fields in enum and compound types */
    trv_tbl->lst[idx].typ_sz=typ_sz; /* [B] Size of user-defined type */

    trv_tbl->lst[idx].flg_cf=False; 
    trv_tbl->lst[idx].flg_crd=False; 
    trv_tbl->lst[idx].flg_dfl=False; 
    trv_tbl->lst[idx].flg_gcv=False; 
    trv_tbl->lst[idx].flg_mch=False; 
    trv_tbl->lst[idx].flg_mtd=False; 
    trv_tbl->lst[idx].flg_ncs=False; 
    trv_tbl->lst[idx].flg_nsx=False; 
    trv_tbl->lst[idx].flg_rcr=False; 
    trv_tbl->lst[idx].flg_rgr=False; 
    trv_tbl->lst[idx].flg_xtn=False; 
    trv_tbl->lst[idx].flg_mrv=False;
    trv_tbl->lst[idx].flg_unn=False; 
    trv_tbl->lst[idx].flg_vfp=False; 
    trv_tbl->lst[idx].flg_vsg=False; 
    trv_tbl->lst[idx].flg_xcl=False; 
    trv_tbl->lst[idx].flg_xtr=False;
    trv_tbl->lst[idx].flg_rdr=False;
    trv_tbl->lst[idx].flg_aux=False;
    trv_tbl->lst[idx].flg_std_att_lat=False;   
    trv_tbl->lst[idx].flg_std_att_lon=False; 

    trv_tbl->lst[idx].rec_dmn_nm_out=NULL;                     
    trv_tbl->lst[idx].grp_dpt=grp_dpt; 
    trv_tbl->lst[idx].nbr_att=nbr_att;
    trv_tbl->lst[idx].nbr_dmn=nbr_dmn_var;
    trv_tbl->lst[idx].nbr_grp=nco_obj_typ_err;
    trv_tbl->lst[idx].nbr_rec=nbr_rec; /* NB: broken fxm should be record dimensions used by this variable */
    trv_tbl->lst[idx].nbr_typ=nco_obj_typ_err; /* NB: Could eventually be number of user-defined types in variable */
    trv_tbl->lst[idx].nbr_var=nco_obj_typ_err;
    trv_tbl->lst[idx].ppc=NC_MAX_INT; /* [nbr] Precision-preserving compression, i.e., number of total or decimal significant digits */
    trv_tbl->lst[idx].flg_nsd=True; /* [flg] PPC is NSD */
    
    trv_tbl->lst[idx].is_crd_lk_var=False;             
    trv_tbl->lst[idx].is_crd_var=False;             
    trv_tbl->lst[idx].is_rec_var=False; 
    trv_tbl->lst[idx].var_typ=var_typ; 
    trv_tbl->lst[idx].enm_prc_typ=err_typ;
    trv_tbl->lst[idx].var_typ_out=(nc_type)err_typ; 
    trv_tbl->lst[idx].grp_nm_fll_prn=grp_nm_fll_prn ? strdup(grp_nm_fll_prn) : NULL;
    trv_tbl->lst[idx].flg_nsm_prn=False;
    trv_tbl->lst[idx].flg_nsm_mbr=False;
    trv_tbl->lst[idx].flg_nsm_tpl=False;
    trv_tbl->lst[idx].nsm_nm=NULL;

    /* Variable dimensions */
    trv_tbl->lst[idx].var_dmn=(var_dmn_sct *)nco_malloc(nbr_dmn_var*sizeof(var_dmn_sct));

    trv_tbl->lst[idx].dmn_idx_out_in=NULL;
    trv_tbl->lst[idx].dmn_rvr_in=NULL;

    for(int idx_dmn_var=0;idx_dmn_var<nbr_dmn_var;idx_dmn_var++){
      trv_tbl->lst[idx].var_dmn[idx_dmn_var].dmn_nm=NULL;
      trv_tbl->lst[idx].var_dmn[idx_dmn_var].dmn_nm_fll=NULL;
      trv_tbl->lst[idx].var_dmn[idx_dmn_var].grp_nm_fll=NULL;
      trv_tbl->lst[idx].var_dmn[idx_dmn_var].is_crd_var=nco_obj_typ_err;
      trv_tbl->lst[idx].var_dmn[idx_dmn_var].is_rec_dmn=nco_obj_typ_err;
      trv_tbl->lst[idx].var_dmn[idx_dmn_var].crd=NULL;
      trv_tbl->lst[idx].var_dmn[idx_dmn_var].ncd=NULL;
      trv_tbl->lst[idx].var_dmn[idx_dmn_var].dmn_id=nco_obj_typ_err;
      /* Assume dimension will be kept on output */
      trv_tbl->lst[idx].var_dmn[idx_dmn_var].flg_dmn_avg=False;
      trv_tbl->lst[idx].var_dmn[idx_dmn_var].flg_rdd=False;  
      trv_tbl->lst[idx].var_dmn[idx_dmn_var].lat_crd=NULL;
      trv_tbl->lst[idx].var_dmn[idx_dmn_var].lon_crd=NULL;
      trv_tbl->lst[idx].var_dmn[idx_dmn_var].nbr_lat_crd=0;
      trv_tbl->lst[idx].var_dmn[idx_dmn_var].nbr_lon_crd=0;
    } /* !idx_dmn_var */

    /* Variable dimensions; store what we know at this time: relative name and ID */
    for(int idx_dmn_var=0;idx_dmn_var<nbr_dmn_var;idx_dmn_var++){
      char dmn_nm_var[NC_MAX_NAME+1]; /* [sng] Dimension name */

      long dmn_sz_var;                /* [nbr] Dimension size */ 

      /* Get dimension name; netCDF nc_inq_dimname() currently relative name */
      (void)nco_inq_dim(grp_id,dmn_id_var[idx_dmn_var],dmn_nm_var,&dmn_sz_var);

      trv_tbl->lst[idx].var_dmn[idx_dmn_var].dmn_nm=strdup(dmn_nm_var);
      trv_tbl->lst[idx].var_dmn[idx_dmn_var].dmn_id=dmn_id_var[idx_dmn_var];
    } /* !idx_dmn_var */

    /* Free constructed name */
    var_nm_fll=(char *)nco_free(var_nm_fll);

    /* Free dimension IDs array */
    dmn_id_var=(int *)nco_free(dmn_id_var);
  } /* !idx_var */

  /* Add dimension objects */ 

  /* Iterate dimensions (for group; dimensions are defined *for* groups) */
  for(int idx_dmn=0;idx_dmn<nbr_dmn_grp;idx_dmn++){
    /* Keep old table dimension size for insertion */
    idx=trv_tbl->nbr_dmn;

    /* Add one more element to dimension list of GTT */
    trv_tbl->nbr_dmn++;
    trv_tbl->lst_dmn=(dmn_trv_sct *)nco_realloc(trv_tbl->lst_dmn,trv_tbl->nbr_dmn*sizeof(dmn_trv_sct));

    /* Initialize as non-record dimension */
    trv_tbl->lst_dmn[idx].is_rec_dmn=False;

    /* Get dimension name */
    rcd+=nco_inq_dim(grp_id,dmn_ids_grp[idx_dmn],dmn_nm,&dmn_sz);

    /* Iterate unlimited dimensions to detect if dimension is record */
    for(int rec_idx=0;rec_idx<nbr_rec;rec_idx++){
      /* Get record dimension name */
      (void)nco_inq_dim(grp_id,dmn_ids_grp_ult[rec_idx],rec_nm,&rec_sz);

      /* Current dimension name matches current record dimension name? */
      if(!strcmp(rec_nm,dmn_nm)){
        /* Dimension is a record dimension */
        trv_tbl->lst_dmn[idx].is_rec_dmn=True;
        /* Exit record dimension loop; we found it */
        break;
      } /* end match record dimension name */
    } /* end record dimension loop */

    /* Allocate path buffer and include space for trailing NUL */ 
    dmn_nm_fll=(char *)nco_malloc(strlen(grp_nm_fll)+strlen(dmn_nm)+2L);

    /* Initialize path with current absolute group path */
    strcpy(dmn_nm_fll,grp_nm_fll);

    /* If not root group, concatenate separator */
    if(strcmp(grp_nm_fll,sls_sng)) strcat(dmn_nm_fll,sls_sng);

    /* Concatenate dimension name to absolute group path */
    strcat(dmn_nm_fll,dmn_nm);

    /* Store object */
    trv_tbl->lst_dmn[idx].nm=strdup(dmn_nm); 
    trv_tbl->lst_dmn[idx].grp_nm_fll=strdup(grp_nm_fll); 
    trv_tbl->lst_dmn[idx].nm_fll=strdup(dmn_nm_fll);    
    trv_tbl->lst_dmn[idx].sz=dmn_sz;                            
    trv_tbl->lst_dmn[idx].lmt_msa.dmn_nm=strdup(dmn_nm); 
    trv_tbl->lst_dmn[idx].lmt_msa.dmn_sz_org=dmn_sz;
    trv_tbl->lst_dmn[idx].lmt_msa.dmn_cnt=dmn_sz;
    trv_tbl->lst_dmn[idx].lmt_msa.WRP=False;
    trv_tbl->lst_dmn[idx].lmt_msa.NON_HYP_DMN=True;
    trv_tbl->lst_dmn[idx].lmt_msa.MSA_USR_RDR=False; 
    trv_tbl->lst_dmn[idx].lmt_msa.lmt_dmn_nbr=0;
    trv_tbl->lst_dmn[idx].lmt_msa.lmt_crr=0;
    trv_tbl->lst_dmn[idx].lmt_msa.lmt_dmn=NULL;
    trv_tbl->lst_dmn[idx].crd_nbr=0;         
    trv_tbl->lst_dmn[idx].crd=NULL; 
    trv_tbl->lst_dmn[idx].dmn_id=dmn_ids_grp[idx_dmn];
    trv_tbl->lst_dmn[idx].has_crd_scp=nco_obj_typ_err;

    /* Free constructed name */
    dmn_nm_fll=(char *)nco_free(dmn_nm_fll);
  } /* !idx_dmn */

  /* Go to sub-groups */ 
  grp_ids=(int *)nco_malloc(nbr_grp*sizeof(int)); 
  rcd+=nco_inq_grps(grp_id,&nbr_grp,grp_ids);

  /* Heart of traversal construction: 
     Construct new sub-group path and call function recursively with new name */
  for(int grp_idx=0;grp_idx<nbr_grp;grp_idx++){
    char *sub_grp_nm_fll=NULL; /* [sng] Sub-group path */
    int gid=grp_ids[grp_idx]; /* [id] Current group ID */  

    /* Get sub-group name */
    rcd+=nco_inq_grpname(gid,grp_nm);

    /* Allocate path buffer including space for trailing NUL */ 
    sub_grp_nm_fll=(char *)nco_malloc(strlen(grp_nm_fll)+strlen(grp_nm)+2L);

    /* Initialize path with current absolute group path */
    strcpy(sub_grp_nm_fll,grp_nm_fll);

    /* If not root group, concatenate separator */
    if(strcmp(grp_nm_fll,sls_sng)) strcat(sub_grp_nm_fll,sls_sng);

    /* Concatenate current group to absolute group path */
    strcat(sub_grp_nm_fll,grp_nm); 

    /* Recursively process subgroups; NB: pass new absolute group name */
    rcd+=nco_grp_itr(gid,grp_nm_fll,sub_grp_nm_fll,trv_tbl);

    /* Free constructed name */
    sub_grp_nm_fll=(char *)nco_free(sub_grp_nm_fll);
  } /* !grp_idx */

  grp_ids=(int *)nco_free(grp_ids); 
  dmn_ids_grp=(int *)nco_free(dmn_ids_grp);
  dmn_ids_grp_ult=(int *)nco_free(dmn_ids_grp_ult);

  return rcd;
} /* end nco_grp_itr() */

void                      
nco_bld_crd_rec_var_trv /* [fnc] Build dimension information for all variables */
(const trv_tbl_sct * const trv_tbl) /* I [sct] GTT (Group Traversal Table) */
{
  /* Purpose: Build "is_crd_var" and "is_rec_var" members for all variables */
  const char fnc_nm[]="nco_bld_crd_var_trv()"; /* [sng] Function name */

  /* Loop all objects */
  for(unsigned idx_var=0;idx_var<trv_tbl->nbr;idx_var++){
    trv_sct var_trv=trv_tbl->lst[idx_var];

    /* Interested in variables only */
    if(var_trv.nco_typ == nco_obj_typ_var){

      /* Loop unique dimensions list in groups */
      for(unsigned idx_dmn=0;idx_dmn<trv_tbl->nbr_dmn;idx_dmn++){
        dmn_trv_sct dmn_trv=trv_tbl->lst_dmn[idx_dmn]; 

        /* Is there a variable with this dimension name anywhere? (relative name) */
        if(!strcmp(dmn_trv.nm,var_trv.nm)){
          /* Is variable in-scope of dimension? */
          if(nco_crd_var_dmn_scp(&var_trv,&dmn_trv,trv_tbl)){
            /* Mark this variable as coordinate variable. NB: True coordinate variables are 1D */
            if(var_trv.nbr_dmn == 1) trv_tbl->lst[idx_var].is_crd_var=True; else trv_tbl->lst[idx_var].is_crd_var=False;
            /* 20170411: fxm this algorithm detects is_rec_crd not is_rec_var
	       If group dimension is a record dimension then coordinate is a record coordinate */
            trv_tbl->lst[idx_var].is_rec_var=dmn_trv.is_rec_dmn;
            if(nco_dbg_lvl_get() == nco_dbg_old){
              (void)fprintf(stdout,"%s: INFO %s reports %s is ",nco_prg_nm_get(),fnc_nm,var_trv.nm_fll);
              if(dmn_trv.is_rec_dmn) (void)fprintf(stdout,"(record) ");
              (void)fprintf(stdout,"coordinate\n");
            } /* endif dbg */
            /* Go to next variable */
            break;
          } /* Is variable in-scope of dimension? */
        } /* Is there a variable with this dimension name anywhere? (relative name) */
      } /* Loop unique dimensions list in groups */
    } /* Interested in variables only */
  } /* Loop all variables */

} /* nco_bld_crd_rec_var_trv() */

void                      
nco_bld_crd_var_trv /* [fnc] Build GTT "crd_sct" coordinate variable structure */
(trv_tbl_sct * const trv_tbl) /* I/O [sct] GTT (Group Traversal Table) */
{
  /* Purpose: Build GTT "crd_sct" coordinate variable structure */

  const char fnc_nm[]="nco_bld_crd_var_trv()"; /* [sng] Function name */

  /* Step 1) Find the total number of coordinate variables for every dimension */

  /* Loop unique dimensions list in groups */
  for(unsigned idx_dmn=0;idx_dmn<trv_tbl->nbr_dmn;idx_dmn++){
    dmn_trv_sct dmn_trv=trv_tbl->lst_dmn[idx_dmn]; 

    /* Loop all objects */
    for(unsigned idx_var=0;idx_var<trv_tbl->nbr;idx_var++){
      trv_sct var_trv=trv_tbl->lst[idx_var];

      /* Interested in variables only */
      if(var_trv.nco_typ == nco_obj_typ_var){

        /* Is there a variable with this dimension name anywhere? (relative name)  */
        if(!strcmp(dmn_trv.nm,var_trv.nm)){

          /* Is variable in-scope of dimension ? */
          if(nco_crd_var_dmn_scp(&var_trv,&dmn_trv,trv_tbl)){

            /* Increment the number of coordinate variables for this dimension */
            trv_tbl->lst_dmn[idx_dmn].crd_nbr++;

          } /* Is variable in-scope of dimension ? */
        } /* Is there a variable with this dimension name anywhere? (relative name)  */
      } /* Interested in variables only */
    } /* Loop all objects */
  } /* Loop unique dimensions list in groups */

  /* Step 2) Allocate coordinate variables array (crd_sct **) for every dimension */

  /* Loop unique dimensions list in groups */
  for(unsigned idx_dmn=0;idx_dmn<trv_tbl->nbr_dmn;idx_dmn++){

    /* Total number of coordinate variables for this dimension */
    int crd_nbr=trv_tbl->lst_dmn[idx_dmn].crd_nbr;

    /* Alloc coordinate array if there are any coordinates */
    if(crd_nbr) trv_tbl->lst_dmn[idx_dmn].crd=(crd_sct **)nco_malloc(crd_nbr*sizeof(crd_sct *));

  } /* Loop unique dimensions list in groups */

  /* Step 3) Allocate/Initialize every coordinate variable array for every dimension */

  /* Loop unique dimensions list in groups */
  for(unsigned idx_dmn=0;idx_dmn<trv_tbl->nbr_dmn;idx_dmn++){
    dmn_trv_sct dmn_trv=trv_tbl->lst_dmn[idx_dmn]; 

    int crd_idx=0; /* [nbr] Coordinate index for current dimension */

    /* Loop all objects */
    for(unsigned idx_var=0;idx_var<trv_tbl->nbr;idx_var++){
      trv_sct var_trv=trv_tbl->lst[idx_var];

      /* Interested in variables only */
      if(var_trv.nco_typ == nco_obj_typ_var){

        /* Is there a variable with this dimension name anywhere? (relative name)  */
        if(!strcmp(dmn_trv.nm,var_trv.nm)){

          /* Is variable in-scope of dimension ? */
          if(nco_crd_var_dmn_scp(&var_trv,&dmn_trv,trv_tbl)){

            /* Alloc this coordinate */
            trv_tbl->lst_dmn[idx_dmn].crd[crd_idx]=(crd_sct *)nco_malloc(sizeof(crd_sct));

            /* The coordinate full name is the variable full name found in-scope */
            trv_tbl->lst_dmn[idx_dmn].crd[crd_idx]->crd_nm_fll=strdup(var_trv.nm_fll);

            /* The coordinate dimension full name is the dimension full name */
            trv_tbl->lst_dmn[idx_dmn].crd[crd_idx]->dmn_nm_fll=strdup(dmn_trv.nm_fll);

            /* The coordinate ID is the dimension unique ID */
            trv_tbl->lst_dmn[idx_dmn].crd[crd_idx]->dmn_id=dmn_trv.dmn_id;

            /* Full group name where coordinate is located is the variable full group name  */
            trv_tbl->lst_dmn[idx_dmn].crd[crd_idx]->crd_grp_nm_fll=strdup(var_trv.grp_nm_fll);

            /* Full group name where dimension of *this* coordinate is located is the full group name of the dimension  */
            trv_tbl->lst_dmn[idx_dmn].crd[crd_idx]->dmn_grp_nm_fll=strdup(dmn_trv.grp_nm_fll);

            /* Store relative name (same for dimension and variable) */
            trv_tbl->lst_dmn[idx_dmn].crd[crd_idx]->nm=strdup(var_trv.nm); 

            /* Is a record dimension(variable) if the dimension is a record dimension */
            trv_tbl->lst_dmn[idx_dmn].crd[crd_idx]->is_rec_dmn=dmn_trv.is_rec_dmn;

            /* Size is size */
            trv_tbl->lst_dmn[idx_dmn].crd[crd_idx]->sz=dmn_trv.sz;

            /* Type */
            trv_tbl->lst_dmn[idx_dmn].crd[crd_idx]->var_typ=var_trv.var_typ;

            /* Group depth */
            trv_tbl->lst_dmn[idx_dmn].crd[crd_idx]->grp_dpt=var_trv.grp_dpt;

            /* MSA */     
            trv_tbl->lst_dmn[idx_dmn].crd[crd_idx]->lmt_msa.dmn_nm=strdup(var_trv.nm);
            trv_tbl->lst_dmn[idx_dmn].crd[crd_idx]->lmt_msa.dmn_cnt=dmn_trv.sz;
            trv_tbl->lst_dmn[idx_dmn].crd[crd_idx]->lmt_msa.dmn_sz_org=dmn_trv.sz;
            trv_tbl->lst_dmn[idx_dmn].crd[crd_idx]->lmt_msa.WRP=False;
            trv_tbl->lst_dmn[idx_dmn].crd[crd_idx]->lmt_msa.NON_HYP_DMN=True;
            trv_tbl->lst_dmn[idx_dmn].crd[crd_idx]->lmt_msa.MSA_USR_RDR=False;  
            trv_tbl->lst_dmn[idx_dmn].crd[crd_idx]->lmt_msa.lmt_dmn_nbr=0;
            trv_tbl->lst_dmn[idx_dmn].crd[crd_idx]->lmt_msa.lmt_crr=0;
            trv_tbl->lst_dmn[idx_dmn].crd[crd_idx]->lmt_msa.lmt_dmn=NULL;

            if(nco_dbg_lvl_get() == nco_dbg_old){     
              crd_sct *crd=trv_tbl->lst_dmn[idx_dmn].crd[crd_idx];
              (void)fprintf(stdout,"%s: INFO %s variable <%s> has coordinate <%s> from dimension <%s>\n",nco_prg_nm_get(),fnc_nm,var_trv.nm_fll,crd->crd_nm_fll,crd->dmn_nm_fll);
            } /* endif */

            /* Increment coordinate index for current dimension */
            crd_idx++;

          } /* Is variable in-scope of dimension ? */
        } /* Is there a variable with this dimension name anywhere? (relative name)  */
      } /* Interested in variables only */
    } /* Loop all objects */
  } /* Loop unique dimensions list in groups */
} /* nco_bld_crd_var_trv() */

static void
prt_lmt                             /* [fnc] Print limit  */
(const int lmt_idx,                 /* I [sct] Index */
 lmt_sct *lmt)                      /* I [sct] Limit */
{
  (void)fprintf(stdout," LIMIT[%d]%s(%li,%li,%li) ",lmt_idx,lmt->nm,lmt->srt,lmt->cnt,lmt->srd);
} /* prt_lmt() */

void                          
nco_prn_trv_tbl                      /* [fnc] Print GTT (Group Traversal Table) */
(const int nc_id,                    /* I [ID] File ID */
 const trv_tbl_sct * const trv_tbl)  /* I [sct] GTT (Group Traversal Table) */
{
  /* Purpose: print GTT (Group Traversal Table); usage ncks --get_grp_info ~/nco/data/in_grp.nc  */

  int nbr_dmn;      /* [nbr] Total number of unique dimensions */
  int nbr_crd;      /* [nbr] Total number of coordinate variables */
  int nbr_crd_var;  /* [nbr] Total number of coordinate variables */

  nbr_dmn=0;
  (void)fprintf(stdout,"%s: INFO reports group information\n",nco_prg_nm_get());
  for(unsigned grp_idx=0;grp_idx<trv_tbl->nbr;grp_idx++){

    /* Filter groups */
    if(trv_tbl->lst[grp_idx].nco_typ == nco_obj_typ_grp){
      trv_sct trv=trv_tbl->lst[grp_idx];            
      (void)fprintf(stdout,"%s: %d subgroups, %d dimensions, %d record dimensions, %d attributes, %d variables\n",trv.nm_fll,trv.nbr_grp,trv.nbr_dmn,trv.nbr_rec,trv.nbr_att,trv.nbr_var); 

      /* Print dimensions for group */
      (void)nco_prn_dmn_grp(nc_id,trv.nm_fll);
      nbr_dmn+=trv.nbr_dmn;
    } /* Filter groups */
  } /* Loop groups */

  assert((unsigned int)nbr_dmn == trv_tbl->nbr_dmn);

  /* Variables */
  nbr_crd=0;
  (void)fprintf(stdout,"\n");
  (void)fprintf(stdout,"%s: INFO reports variable information\n",nco_prg_nm_get());
  for(unsigned idx_var=0;idx_var<trv_tbl->nbr;idx_var++){
    trv_sct var_trv=trv_tbl->lst[idx_var];   
    if(var_trv.nco_typ == nco_obj_typ_var){
      (void)fprintf(stdout,"%s:",var_trv.nm_fll);
      if(var_trv.is_crd_var){
        (void)fprintf(stdout," (coordinate)");
        nbr_crd++;
      } /* endif */
      if(var_trv.is_rec_var) (void)fprintf(stdout," (record)");

      /* 20170411: WRONG!!! Following line confuses generic record variables with record coordinates
	 If record variable must be coordinate variable */
      if(var_trv.is_rec_var) assert(var_trv.is_crd_var);
      (void)fprintf(stdout," %d dimensions: ",var_trv.nbr_dmn); 

      for(int idx_dmn_var=0;idx_dmn_var<var_trv.nbr_dmn;idx_dmn_var++){
        var_dmn_sct var_dmn=var_trv.var_dmn[idx_dmn_var];
        (void)fprintf(stdout,"[%d]%s#%d ",idx_dmn_var,var_dmn.dmn_nm_fll,var_dmn.dmn_id); 
        if(var_dmn.is_crd_var) (void)fprintf(stdout," (coordinate) : ");

        /* Segregate limits into two cases */
        if(var_dmn.crd){
	  /* Dimension has coordinate variables */
          for(int lmt_idx=0;lmt_idx<var_dmn.crd->lmt_msa.lmt_dmn_nbr;lmt_idx++)
            (void)prt_lmt(lmt_idx,var_dmn.crd->lmt_msa.lmt_dmn[lmt_idx]);
        }else{
          /* Dimension has no coordinate variables */
          for(int lmt_idx=0;lmt_idx<var_dmn.ncd->lmt_msa.lmt_dmn_nbr;lmt_idx++)
            (void)prt_lmt(lmt_idx,var_dmn.ncd->lmt_msa.lmt_dmn[lmt_idx]);
        } /* !crd */
      } /* !dmn */
      (void)fprintf(stdout,"\n");
    } /* !var  */
  } /* !var */

  /* Unique dimension list, Coordinate variables stored in unique dimension list, limits */
  nbr_crd_var=0;
  (void)fprintf(stdout,"\n");
  (void)fprintf(stdout,"%s: INFO reports coordinate variables and limits listed by dimension:\n",nco_prg_nm_get());
  for(unsigned idx_dmn=0;idx_dmn<trv_tbl->nbr_dmn;idx_dmn++){
    dmn_trv_sct dmn_trv=trv_tbl->lst_dmn[idx_dmn]; 

    /* Dimension ID and  full name */
    (void)fprintf(stdout,"(#%d%s)",dmn_trv.dmn_id,dmn_trv.nm_fll);

    /* Filter output */
    if(dmn_trv.is_rec_dmn) (void)fprintf(stdout," record dimension(%lu):: ",(unsigned long)dmn_trv.sz); else (void)fprintf(stdout," dimension(%lu):: ",(unsigned long)dmn_trv.sz);

    nbr_crd_var+=dmn_trv.crd_nbr;

    /* Loop coordinates */
    for(int crd_idx=0;crd_idx<dmn_trv.crd_nbr;crd_idx++){
      crd_sct *crd=dmn_trv.crd[crd_idx];

      /* Coordinate full name */
      (void)fprintf(stdout,"%s ",crd->crd_nm_fll);

      /* Dimension full name */
      (void)fprintf(stdout,"(#%d%s) ",crd->dmn_id,crd->dmn_nm_fll);

      /* Limits */
      for(int lmt_idx=0;lmt_idx<crd->lmt_msa.lmt_dmn_nbr;lmt_idx++)
        (void)prt_lmt(lmt_idx,crd->lmt_msa.lmt_dmn[lmt_idx]);

      /* Terminate this coordinate with "::" */
      if(dmn_trv.crd_nbr>1) (void)fprintf(stdout,":: ");

    } /* Loop coordinates */

    /* Terminate line */
    (void)fprintf(stdout,"\n");

  } /* Coordinate variables stored in unique dimension list */

  assert(nbr_crd_var == nbr_crd);

} /* nco_prn_trv_tbl() */

void                          
nco_has_crd_dmn_scp /* [fnc] Is there a variable with same name in dimension's scope?   */
(const trv_tbl_sct * const trv_tbl) /* I [sct] GTT (Group Traversal Table) */
{
  const char fnc_nm[]="nco_has_crd_dmn_scp()"; /* [sng] Function name  */

  /* Unique dimension list */

  if(nco_dbg_lvl_get() == nco_dbg_old)(void)fprintf(stdout,"%s: INFO reports dimension information with limits: %d dimensions\n",nco_prg_nm_get(),trv_tbl->nbr_dmn);
  for(unsigned idx_dmn=0;idx_dmn<trv_tbl->nbr_dmn;idx_dmn++){
    dmn_trv_sct dmn_trv=trv_tbl->lst_dmn[idx_dmn]; 

    /* Dimension #/name first */
    if(nco_dbg_lvl_get() == nco_dbg_old) (void)fprintf(stdout,"#%d%s\n",dmn_trv.dmn_id,dmn_trv.nm_fll);

    nco_bool in_scp=False;

    /* Loop object table */
    for(unsigned idx_var=0;idx_var<trv_tbl->nbr;idx_var++){

      /* Filter variables  */
      if(trv_tbl->lst[idx_var].nco_typ == nco_obj_typ_var){
        trv_sct var_trv=trv_tbl->lst[idx_var];   

        /* Is there a variable with this dimension name (a coordinate varible) anywhere (relative name)  */
        if(!strcmp(dmn_trv.nm,var_trv.nm)){

          /* Is variable in-scope of dimension ? */
          if(nco_crd_var_dmn_scp(&var_trv,&dmn_trv,trv_tbl)){

            if(nco_dbg_lvl_get() == nco_dbg_old) (void)fprintf(stdout,"%s: INFO %s reports variable <%s> in-scope of dimension <%s>\n",nco_prg_nm_get(),fnc_nm,var_trv.nm_fll,dmn_trv.nm_fll);

            trv_tbl->lst_dmn[idx_dmn].has_crd_scp=True;
            /* Built before; variable must be a cordinate */
            assert(var_trv.is_crd_var);
            in_scp=True;
          } /* Is variable in-scope of dimension ? */
        } /* Is there a variable with this dimension name anywhere? (relative name)  */
      } /* Filter variables  */
    } /* Loop object table */

    if(nco_dbg_lvl_get() == nco_dbg_old)
      if(!in_scp) (void)fprintf(stdout,"%s: INFO %s dimension <%s> with no in-scope variables\n",nco_prg_nm_get(),fnc_nm,dmn_trv.nm_fll);        

    trv_tbl->lst_dmn[idx_dmn].has_crd_scp=in_scp;
  } /* Unique dimension list */

  /* Unique dimension list */
  for(unsigned idx_dmn=0;idx_dmn<trv_tbl->nbr_dmn;idx_dmn++) assert(trv_tbl->lst_dmn[idx_dmn].has_crd_scp != nco_obj_typ_err);

} /* nco_has_crd_dmn_scp() */

nco_bool                               /* O [flg] True if variable is in-scope of dimension */
nco_crd_var_dmn_scp                    /* [fnc] Is coordinate variable in dimension scope */
(const trv_sct * const var_trv,        /* I [sct] GTT Object Variable */
 const dmn_trv_sct * const dmn_trv,    /* I [sct] GTT unique dimension */
 const trv_tbl_sct * const trv_tbl)    /* I [sct] GTT (Group Traversal Table) */
{
  /* Purpose: Find if variable is in-scope of the dimension: 
     Use case in-scope:
     dimension /lon 
     variable /g1/lon
     Use case not in-scope:
     variable /lon
     dimension /g1/lon
     
     NOTE: deal with cases like
     dimension: /lon
     variable:  /g8/lon 
     dimension: /g8/lon */
  
  const char fnc_nm[]="nco_crd_var_dmn_scp()";   /* [sng] Function name */
  
  const char sls_chr='/';                    /* [chr] Slash character */
  char *sbs_srt;                             /* [sng] Location of user-string match start in object path */
  char *sbs_end;                             /* [sng] Location of user-string match end   in object path */
  
  nco_bool flg_pth_srt_bnd=False;            /* [flg] String begins at path component boundary */
  nco_bool flg_pth_end_bnd=False;            /* [flg] String ends   at path component boundary */
  
  size_t var_sng_lng;                        /* [nbr] Length of variable name */
  size_t var_nm_fll_lng;                     /* [nbr] Length of full variable name */
  size_t dmn_nm_fll_lng;                     /* [nbr] Length of of full dimension name */
  
  /* True Coordinate variables are 1D */
  if(var_trv->nbr_dmn != 1) return False;
  
  /* Most common case is that unique dimension full name matches full variable name */
  if(!strcmp(var_trv->nm_fll,dmn_trv->nm_fll)){
    if(nco_dbg_lvl_get() == nco_dbg_old) (void)fprintf(stdout,"%s: INFO %s found absolute match of variable <%s> and dimension <%s>:\n",nco_prg_nm_get(),fnc_nm,var_trv->nm_fll,dmn_trv->nm_fll);
    return True;
  } /* !strcmp() */
  
  /* Deal with in-scope cases */
  var_nm_fll_lng=strlen(var_trv->nm_fll);
  dmn_nm_fll_lng=strlen(dmn_trv->nm_fll);
  var_sng_lng=strlen(var_trv->nm);
  
  /* Look for partial match, not necessarily on path boundaries; locate variable (str2) in full dimension name (str1) */
  if((sbs_srt=strstr(dmn_trv->nm_fll,var_trv->nm))){
    
    /* Ensure match spans (begins and ends on) whole path-component boundaries */
    
    /* Does match begin at path component boundary ... directly on a slash? */
    if(*sbs_srt == sls_chr) flg_pth_srt_bnd=True;
    
    /* ...or one after a component boundary? */
    if((sbs_srt > dmn_trv->nm_fll) && (*(sbs_srt-1L) == sls_chr)) flg_pth_srt_bnd=True;
    
    /* Does match end at path component boundary ... directly on a slash? */
    sbs_end=sbs_srt+var_sng_lng-1L;
    
    if(*sbs_end == sls_chr) flg_pth_end_bnd=True;
    
    /* ...or one before a component boundary? */
    if(sbs_end <= dmn_trv->nm_fll+dmn_nm_fll_lng-1L)
      if((*(sbs_end+1L) == sls_chr) || (*(sbs_end+1L) == '\0'))
        flg_pth_end_bnd=True;
    
    /* If '/' bounds both ends of match then it is "real", not for example "lat_lon" as a variable looking for "lon" */
    if(flg_pth_srt_bnd && flg_pth_end_bnd){
      
      /* Absolute match (equality redundant); strcmp() deals cases like /g3/rlev/ and /g5/rlev */
      if(var_nm_fll_lng == dmn_nm_fll_lng && !strcmp(var_trv->nm_fll,dmn_trv->nm_fll)){
        if(nco_dbg_lvl_get() == nco_dbg_old) (void)fprintf(stdout,"%s: INFO %s found absolute match of variable <%s> and dimension <%s>:\n",nco_prg_nm_get(),fnc_nm,var_trv->nm_fll,dmn_trv->nm_fll);
        return True;
	
	/* Variable in-scope of dimension */
      }else if(var_nm_fll_lng>dmn_nm_fll_lng){
	
        /* NOTE: deal with cases like
	   dimension: /lon
	   variable:  /g8/lon 
	   dimension: /g8/lon */
	
        /* Loop unique dimensions list in groups */
        for(unsigned idx_dmn=0;idx_dmn<trv_tbl->nbr_dmn;idx_dmn++){
          dmn_trv_sct dmn=trv_tbl->lst_dmn[idx_dmn]; 
          /* Loop all objects */
          for(unsigned idx_var=0;idx_var<trv_tbl->nbr;idx_var++){
            trv_sct var=trv_tbl->lst[idx_var];
            /* Interested in variables only */
            if(var.nco_typ != nco_obj_typ_grp){
              /* Is there a full match already for input dimension? */
              if(!strcmp(var_trv->nm_fll,dmn.nm_fll)){
                if(nco_dbg_lvl_get() == nco_dbg_old) (void)fprintf(stdout,"%s: INFO %s variable <%s> has another dimension full match <%s>:\n",nco_prg_nm_get(),fnc_nm,var_trv->nm_fll,dmn.nm_fll);
                return False;
              } /* Is there a *full* match already?  */
            } /* Interested in variables only */
          } /* Loop all objects */
        } /* Loop unique dimensions list in groups */

        if(nco_dbg_lvl_get() == nco_dbg_old) (void)fprintf(stdout,"%s: INFO %s found variable <%s> in-scope of dimension <%s>:\n",nco_prg_nm_get(),fnc_nm,var_trv->nm_fll,dmn_trv->nm_fll);
        return True;

        /* Variable out of scope of dimension */
      }else if(var_nm_fll_lng < dmn_nm_fll_lng){
        if(nco_dbg_lvl_get() == nco_dbg_old) (void)fprintf(stdout,"%s: INFO %s found variable <%s> out of scope of dimension <%s>:\n",nco_prg_nm_get(),fnc_nm,var_trv->nm_fll,dmn_trv->nm_fll);
        return False;

      } /* Absolute match  */
    } /* If match is on both ends of '/' then it's a "real" name, not for example "lat_lon" as a variable looking for "lon" */
  }/* Look for partial match, not necessarily on path boundaries */

  return False;
} /* nco_crd_var_dmn_scp() */

int /* O [enm] Comparison result [<,=,>] 0 iff val_1 [<,==,>] val_2 */
nco_cmp_crd_dpt /* [fnc] Compare two crd_sct's by group depth */
(const void *val_1, /* I [sct] crd_sct * to compare */
 const void *val_2) /* I [sct] crd_sct * to compare */
{
  /* Purpose: Compare two crd_sct's by group depth structure member
     Function is suitable for argument to ANSI C qsort() routine in stdlib.h
     crd_sct **crd is an array of pointers to unique dimension coordinates */

  const crd_sct * const * const crd1=(const crd_sct * const *)val_1;
  const crd_sct * const * const crd2=(const crd_sct * const *)val_2;

  if((*crd1)->grp_dpt > (*crd2)->grp_dpt) return -1;
  else if((*crd1)->grp_dpt < (*crd2)->grp_dpt) return 1;
  else return 0;
} /* end nco_cmp_crd_dpt() */

crd_sct *                             /* O [sct] Coordinate object */
nco_scp_var_crd                       /* [fnc] Return in-scope coordinate for variable  */
(const trv_sct * const var_trv,       /* I [sct] Variable object */
 dmn_trv_sct * const dmn_trv)         /* I [sct] Dimension object */
{

  /* Purpose: Choose one coordinate from the dimension object to assign as a valid coordinate
     to the variable dimension
     Scope definition: In the same group of the variable or beneath (closer to root) 
     Above: out of scope (no luck)
     
     Use cases:
     dimension lon4;
     variable lon4_var(lon4)
     
     Variable /g16/g16g4/g16g4g4/g16g4g4g4/lon4_var
     2 coordinates down in-scope 
     /g16/g16g4/g16g4g4/lon4
     /g16/g16g4/lon4 */


  /* If more than one coordinate, sort them by group depth */
  if(dmn_trv->crd_nbr>1) qsort(dmn_trv->crd,(size_t)dmn_trv->crd_nbr,sizeof(crd_sct *),nco_cmp_crd_dpt);

  /* Loop over coordinates; they all have unique dimension ID of variable dimension */
  for(int crd_idx=0;crd_idx<dmn_trv->crd_nbr;crd_idx++){
    crd_sct *crd=dmn_trv->crd[crd_idx];
    /* Absolute match: in-scope */ 
    if(!strcmp(var_trv->nm_fll,crd->crd_nm_fll)){ 
      /* Variable must be coordinate for this to happen */
      assert(var_trv->is_crd_var);
      return crd;
    }else if(!strcmp(var_trv->grp_nm_fll,crd->crd_grp_nm_fll)){ 
      /* Same group: in-scope  */ 
      return crd;
    }
    else if(crd->grp_dpt < var_trv->grp_dpt){
      /* Level below: in-scope  */
      return crd;
    }
  } /* Loop coordinates */

  return NULL;
} /* nco_scp_var_crd() */

void
nco_bld_var_dmn                       /* [fnc] Assign variables dimensions to either coordinates or dimension structs */
(trv_tbl_sct * const trv_tbl)         /* I/O [sct] Traversal table */
{
  /* Purpose: Fill variable dimensions with pointers to either a coordinate variable or dimension structs */

  const char fnc_nm[]="nco_bld_var_dmn()"; /* [sng] Function name  */

#ifdef DEBUG_LEAKS
  int crt_counter=0;
#endif

  /* Loop table */
  for(unsigned idx_var=0;idx_var<trv_tbl->nbr;idx_var++){

    /* Filter variables  */
    if(trv_tbl->lst[idx_var].nco_typ != nco_obj_typ_grp){
      trv_sct var_trv=trv_tbl->lst[idx_var];   

      /* Loop dimensions for object (variable)  */
      for(int idx_dmn_var=0;idx_dmn_var<var_trv.nbr_dmn;idx_dmn_var++){

        trv_tbl->lst[idx_var].var_dmn[idx_dmn_var].crd=NULL;

        /* Get unique dimension ID from variable dimension */
        int var_dmn_id=var_trv.var_dmn[idx_dmn_var].dmn_id;

        /* Get unique dimension object from unique dimension ID */
        dmn_trv_sct *dmn_trv=nco_dmn_trv_sct(var_dmn_id,trv_tbl);

        /* No coordinates */
        if(dmn_trv->crd_nbr == 0){

          if(nco_dbg_lvl_get() == nco_dbg_old) (void)fprintf(stdout,"%s: INFO %s reports variable <%s> with *NON* coordinate dimension [%d]%s\n",nco_prg_nm_get(),fnc_nm,var_trv.nm_fll,idx_dmn_var,var_trv.var_dmn[idx_dmn_var].dmn_nm_fll);        

          /* Mark as False the position of the bool array coordinate/non coordinate */
          trv_tbl->lst[idx_var].var_dmn[idx_dmn_var].is_crd_var=False;

          /* Store unique dimension (non coordinate) */

          /* Deep-copy dimension */
          trv_tbl->lst[idx_var].var_dmn[idx_dmn_var].ncd=(dmn_trv_sct *)nco_malloc(sizeof(dmn_trv_sct));
          trv_tbl->lst[idx_var].var_dmn[idx_dmn_var].ncd->grp_nm_fll=(char *)strdup(dmn_trv->grp_nm_fll);
          trv_tbl->lst[idx_var].var_dmn[idx_dmn_var].ncd->nm_fll=(char *)strdup(dmn_trv->nm_fll);
          trv_tbl->lst[idx_var].var_dmn[idx_dmn_var].ncd->nm=(char *)strdup(dmn_trv->nm);
          trv_tbl->lst[idx_var].var_dmn[idx_dmn_var].ncd->crd=NULL;
          trv_tbl->lst[idx_var].var_dmn[idx_dmn_var].ncd->crd_nbr=0;
          trv_tbl->lst[idx_var].var_dmn[idx_dmn_var].ncd->dmn_id=dmn_trv->dmn_id;
          trv_tbl->lst[idx_var].var_dmn[idx_dmn_var].ncd->flg_xtr=dmn_trv->flg_xtr;
          trv_tbl->lst[idx_var].var_dmn[idx_dmn_var].ncd->has_crd_scp=dmn_trv->has_crd_scp;
          trv_tbl->lst[idx_var].var_dmn[idx_dmn_var].ncd->is_rec_dmn=dmn_trv->is_rec_dmn;
          trv_tbl->lst[idx_var].var_dmn[idx_dmn_var].ncd->sz=dmn_trv->sz;

          /* MSA */
          trv_tbl->lst[idx_var].var_dmn[idx_dmn_var].ncd->lmt_msa.NON_HYP_DMN=dmn_trv->lmt_msa.NON_HYP_DMN;
          trv_tbl->lst[idx_var].var_dmn[idx_dmn_var].ncd->lmt_msa.dmn_cnt=dmn_trv->lmt_msa.dmn_cnt;
          trv_tbl->lst[idx_var].var_dmn[idx_dmn_var].ncd->lmt_msa.dmn_nm=(char *)strdup(dmn_trv->lmt_msa.dmn_nm);
          trv_tbl->lst[idx_var].var_dmn[idx_dmn_var].ncd->lmt_msa.dmn_sz_org=dmn_trv->lmt_msa.dmn_sz_org;
          trv_tbl->lst[idx_var].var_dmn[idx_dmn_var].ncd->lmt_msa.lmt_crr=dmn_trv->lmt_msa.lmt_crr;
          trv_tbl->lst[idx_var].var_dmn[idx_dmn_var].ncd->lmt_msa.lmt_dmn=NULL;
          trv_tbl->lst[idx_var].var_dmn[idx_dmn_var].ncd->lmt_msa.lmt_dmn_nbr=0;
          trv_tbl->lst[idx_var].var_dmn[idx_dmn_var].ncd->lmt_msa.MSA_USR_RDR=dmn_trv->lmt_msa.MSA_USR_RDR;
          trv_tbl->lst[idx_var].var_dmn[idx_dmn_var].ncd->lmt_msa.WRP=dmn_trv->lmt_msa.WRP;

        }else if(dmn_trv->crd_nbr > 0){
          /* Choose in-scope coordinates
	     Scope definition: In the same group of the variable or above (closer to root) 
	     Below: out-of-scope */

          crd_sct *crd=NULL; /* [sct] Coordinate to assign to dimension of variable */

          /* Choose the "in-scope" coordinate for the variable and assign it to the variable dimension */
          crd=nco_scp_var_crd(&var_trv,dmn_trv);

          /* The "in-scope" coordinate is returned */
          if(crd){
            if(nco_dbg_lvl_get() == nco_dbg_old) (void)fprintf(stdout,"%s: INFO %s reports dimension [%d]%s of variable <%s> in-scope of coordinate <%s>\n",nco_prg_nm_get(),fnc_nm,idx_dmn_var,var_trv.var_dmn[idx_dmn_var].dmn_nm_fll,var_trv.nm_fll,crd->crd_nm_fll);
	    
            /* Mark as True */
            trv_tbl->lst[idx_var].var_dmn[idx_dmn_var].is_crd_var=True;

#ifdef DEBUG_LEAKS
            crt_counter++;
            assert(trv_tbl->lst[idx_var].var_dmn[idx_dmn_var].crd == NULL);
#endif

            /* Deep-copy coordinate */
            trv_tbl->lst[idx_var].var_dmn[idx_dmn_var].crd=(crd_sct *)nco_malloc(sizeof(crd_sct));
            trv_tbl->lst[idx_var].var_dmn[idx_dmn_var].crd->crd_nm_fll=(char *)strdup(crd->crd_nm_fll);
            trv_tbl->lst[idx_var].var_dmn[idx_dmn_var].crd->dmn_nm_fll=(char *)strdup(crd->dmn_nm_fll);
            trv_tbl->lst[idx_var].var_dmn[idx_dmn_var].crd->crd_grp_nm_fll=(char *)strdup(crd->crd_grp_nm_fll);
            trv_tbl->lst[idx_var].var_dmn[idx_dmn_var].crd->dmn_grp_nm_fll=(char *)strdup(crd->dmn_grp_nm_fll);
            trv_tbl->lst[idx_var].var_dmn[idx_dmn_var].crd->nm=(char *)strdup(crd->nm);

            trv_tbl->lst[idx_var].var_dmn[idx_dmn_var].crd->is_rec_dmn=crd->is_rec_dmn;
            trv_tbl->lst[idx_var].var_dmn[idx_dmn_var].crd->sz=crd->sz;
            trv_tbl->lst[idx_var].var_dmn[idx_dmn_var].crd->var_typ=crd->var_typ;
            trv_tbl->lst[idx_var].var_dmn[idx_dmn_var].crd->dmn_id=crd->dmn_id;
            trv_tbl->lst[idx_var].var_dmn[idx_dmn_var].crd->grp_dpt=crd->grp_dpt;
            /* MSA */
            trv_tbl->lst[idx_var].var_dmn[idx_dmn_var].crd->lmt_msa.NON_HYP_DMN=crd->lmt_msa.NON_HYP_DMN;
            trv_tbl->lst[idx_var].var_dmn[idx_dmn_var].crd->lmt_msa.dmn_cnt=crd->lmt_msa.dmn_cnt;
            trv_tbl->lst[idx_var].var_dmn[idx_dmn_var].crd->lmt_msa.dmn_nm=(char *)strdup(crd->lmt_msa.dmn_nm);
            trv_tbl->lst[idx_var].var_dmn[idx_dmn_var].crd->lmt_msa.dmn_sz_org=crd->lmt_msa.dmn_sz_org;
            trv_tbl->lst[idx_var].var_dmn[idx_dmn_var].crd->lmt_msa.lmt_crr=crd->lmt_msa.lmt_crr;
            trv_tbl->lst[idx_var].var_dmn[idx_dmn_var].crd->lmt_msa.lmt_dmn=NULL;
            trv_tbl->lst[idx_var].var_dmn[idx_dmn_var].crd->lmt_msa.lmt_dmn_nbr=0;
            trv_tbl->lst[idx_var].var_dmn[idx_dmn_var].crd->lmt_msa.MSA_USR_RDR=crd->lmt_msa.MSA_USR_RDR;
            trv_tbl->lst[idx_var].var_dmn[idx_dmn_var].crd->lmt_msa.WRP=crd->lmt_msa.WRP;

          }else{

            if(nco_dbg_lvl_get() == nco_dbg_old) (void)fprintf(stdout,"%s: INFO %s reports dimension [%d]%s of variable <%s> with out-of-scope coordinate\n",nco_prg_nm_get(),fnc_nm,idx_dmn_var,var_trv.var_dmn[idx_dmn_var].dmn_nm_fll,var_trv.nm_fll);         

            /* Mark as False */
            trv_tbl->lst[idx_var].var_dmn[idx_dmn_var].is_crd_var=False;

            /* Store the unique dimension as if it was a non coordinate */

            /* Deep-copy dimension  */
            trv_tbl->lst[idx_var].var_dmn[idx_dmn_var].ncd=(dmn_trv_sct *)nco_malloc(sizeof(dmn_trv_sct));
            trv_tbl->lst[idx_var].var_dmn[idx_dmn_var].ncd->grp_nm_fll=(char *)strdup(dmn_trv->grp_nm_fll);
            trv_tbl->lst[idx_var].var_dmn[idx_dmn_var].ncd->nm_fll=(char *)strdup(dmn_trv->nm_fll);
            trv_tbl->lst[idx_var].var_dmn[idx_dmn_var].ncd->nm=(char *)strdup(dmn_trv->nm);
            trv_tbl->lst[idx_var].var_dmn[idx_dmn_var].ncd->crd=NULL;
            trv_tbl->lst[idx_var].var_dmn[idx_dmn_var].ncd->crd_nbr=0;
            trv_tbl->lst[idx_var].var_dmn[idx_dmn_var].ncd->dmn_id=dmn_trv->dmn_id;
            trv_tbl->lst[idx_var].var_dmn[idx_dmn_var].ncd->flg_xtr=dmn_trv->flg_xtr;
            trv_tbl->lst[idx_var].var_dmn[idx_dmn_var].ncd->has_crd_scp=dmn_trv->has_crd_scp;
            trv_tbl->lst[idx_var].var_dmn[idx_dmn_var].ncd->is_rec_dmn=dmn_trv->is_rec_dmn;
            trv_tbl->lst[idx_var].var_dmn[idx_dmn_var].ncd->sz=dmn_trv->sz;

            /* MSA */
            trv_tbl->lst[idx_var].var_dmn[idx_dmn_var].ncd->lmt_msa.NON_HYP_DMN=dmn_trv->lmt_msa.NON_HYP_DMN;
            trv_tbl->lst[idx_var].var_dmn[idx_dmn_var].ncd->lmt_msa.dmn_cnt=dmn_trv->lmt_msa.dmn_cnt;
            trv_tbl->lst[idx_var].var_dmn[idx_dmn_var].ncd->lmt_msa.dmn_nm=(char *)strdup(dmn_trv->lmt_msa.dmn_nm);
            trv_tbl->lst[idx_var].var_dmn[idx_dmn_var].ncd->lmt_msa.dmn_sz_org=dmn_trv->lmt_msa.dmn_sz_org;
            trv_tbl->lst[idx_var].var_dmn[idx_dmn_var].ncd->lmt_msa.lmt_crr=dmn_trv->lmt_msa.lmt_crr;
            trv_tbl->lst[idx_var].var_dmn[idx_dmn_var].ncd->lmt_msa.lmt_dmn=NULL;
            trv_tbl->lst[idx_var].var_dmn[idx_dmn_var].ncd->lmt_msa.lmt_dmn_nbr=0;
            trv_tbl->lst[idx_var].var_dmn[idx_dmn_var].ncd->lmt_msa.MSA_USR_RDR=dmn_trv->lmt_msa.MSA_USR_RDR;
            trv_tbl->lst[idx_var].var_dmn[idx_dmn_var].ncd->lmt_msa.WRP=dmn_trv->lmt_msa.WRP;

          } /* None was found in-scope */
        } /* There are coordinates; one must be chosen */
      } /* Loop dimensions for object (variable)  */
    } /* Filter variables  */
  } /* Loop table */

  /* Check if bool array is all filled  */

  /* Loop table */
  for(unsigned idx_var=0;idx_var<trv_tbl->nbr;idx_var++)
    if(trv_tbl->lst[idx_var].nco_typ != nco_obj_typ_grp)
      for(int idx_dmn_var=0;idx_dmn_var<trv_tbl->lst[idx_var].nbr_dmn;idx_dmn_var++)
        if(trv_tbl->lst[idx_var].var_dmn[idx_dmn_var].is_crd_var == nco_obj_typ_err)
          if(nco_dbg_lvl_get() == nco_dbg_old) (void)fprintf(stdout,"%s: OOPSY %s reports variable <%s> with NOT filled dimension [%d]%s\n",nco_prg_nm_get(),fnc_nm,trv_tbl->lst[idx_var].nm_fll,idx_dmn_var,trv_tbl->lst[idx_var].var_dmn[idx_dmn_var].dmn_nm_fll);        

  /* Check if bool array is all filled  */
  /* Loop table */
  for(unsigned idx_var=0;idx_var<trv_tbl->nbr;idx_var++)
    if(trv_tbl->lst[idx_var].nco_typ != nco_obj_typ_grp)
      for(int idx_dmn_var=0;idx_dmn_var<trv_tbl->lst[idx_var].nbr_dmn;idx_dmn_var++)
        assert(trv_tbl->lst[idx_var].var_dmn[idx_dmn_var].is_crd_var != nco_obj_typ_err);

#ifdef DEBUG_LEAKS
  if(nco_dbg_lvl_get() >= nco_dbg_sup)(void)fprintf(stdout,"%s: %s DEBUG %d crd",nco_prg_nm_get(),fnc_nm,crt_counter);
#endif

} /* nco_bld_var_dmn() */

void                          
nco_wrt_trv_tbl                      /* [fnc] Obtain file information from GTT (Group Traversal Table) for debugging  */
(const int nc_id,                    /* I [ID] File ID */
 const trv_tbl_sct * const trv_tbl,  /* I [sct] GTT (Group Traversal Table) */
 nco_bool use_flg_xtr)               /* I [flg] Use flg_xtr in selection */
{

  const char fnc_nm[]="nco_wrt_trv_tbl()"; /* [sng] Function name  */

  int nbr_dmn_var;             /* [nbr] Number of variables in group */
  int grp_id;                  /* [id] Group ID */
  int var_id;                  /* [id] Variable ID */
  int *dmn_id_var;             /* [id] Dimensions IDs array for variable */

  for(unsigned idx_var=0;idx_var<trv_tbl->nbr;idx_var++){
    trv_sct var_trv=trv_tbl->lst[idx_var];

    nco_bool flg_xtr;
    
    if(use_flg_xtr) flg_xtr=var_trv.flg_xtr; else flg_xtr=True;

    /* If object is an extracted variable... */ 
    if(var_trv.nco_typ != nco_obj_typ_grp && flg_xtr){

      if(nco_dbg_lvl_get() == nco_dbg_old) (void)fprintf(stdout,"%s: INFO %s variable <%s>",nco_prg_nm_get(),fnc_nm,var_trv.nm_fll);

      /* Obtain group ID where variable is located */
      (void)nco_inq_grp_full_ncid(nc_id,var_trv.grp_nm_fll,&grp_id);

      /* Obtain variable ID */
      (void)nco_inq_varid(grp_id,var_trv.nm,&var_id);

      /* Get type of variable and number of dimensions */
      (void)nco_inq_var(grp_id,var_id,(char *)NULL,(nc_type *)NULL,&nbr_dmn_var,(int *)NULL,(int *)NULL);

      /* Alloc dimension IDs array */
      dmn_id_var=(int *)nco_malloc(nbr_dmn_var*sizeof(int));

      /* Get dimension IDs for variable */
      (void)nco_inq_vardimid(grp_id,var_id,dmn_id_var);

      if(nco_dbg_lvl_get() == nco_dbg_old) (void)fprintf(stdout," %d dimensions: ",nbr_dmn_var);        

      /* Variable dimensions */
      for(int idx_dmn_var=0;idx_dmn_var<nbr_dmn_var;idx_dmn_var++){

        char dmn_nm_var[NC_MAX_NAME+1]; /* [sng] Dimension name */
        long dmn_sz_var;                /* [nbr] Dimension size */ 

        /* Get dimension name */
        (void)nco_inq_dim(grp_id,dmn_id_var[idx_dmn_var],dmn_nm_var,&dmn_sz_var);
        if(nco_dbg_lvl_get() == nco_dbg_old) (void)fprintf(stdout,"#%d'%s' ",dmn_id_var[idx_dmn_var],dmn_nm_var);
      } /* end for */

      if(nco_dbg_lvl_get() == nco_dbg_old) (void)fprintf(stdout,"\n");
      dmn_id_var=(int *)nco_free(dmn_id_var);

    } /* endif */
  } /* end loop over idx_tbl */
} /* nco_wrt_trv_tbl() */

void
nco_gpe_chk                            /* [fnc] Check valid GPE new name  */
(const char * const grp_out_fll,       /* I [sng] Group name */
 const char * const var_nm,            /* I [sng] Variable name */
 gpe_nm_sct ** gpe_nm,                 /* I/O [sct] GPE name duplicate check array */
 int * nbr_gpe_nm)                     /* I/O [nbr] Number of GPE entries */  
{
  /* Detect duplicate GPE names in advance, then exit with helpful error */

  const char fnc_nm[]="nco_gpe_chk()"; /* [sng] Function name */
  const char sls_sng[]="/";        /* [sng] Slash string */

  char *gpe_var_nm_fll=NULL;       /* [sng] absolute GPE variable path */

  int nbr_gpe = *nbr_gpe_nm;
  
  /* Construct absolute GPE variable path */
  gpe_var_nm_fll=(char*)nco_malloc(strlen(grp_out_fll)+strlen(var_nm)+2L);
  strcpy(gpe_var_nm_fll,grp_out_fll);
  /* If not root group, concatenate separator */
  if(strcmp(grp_out_fll,sls_sng)) strcat(gpe_var_nm_fll,sls_sng);
  strcat(gpe_var_nm_fll,var_nm);

  /* GPE name is not already on list, put it there */
  if(nbr_gpe == 0){
    (*gpe_nm)=(gpe_nm_sct *)nco_malloc((nbr_gpe+1)*sizeof(gpe_nm_sct)); 
    (*gpe_nm)[nbr_gpe].var_nm_fll=strdup(gpe_var_nm_fll);
    nbr_gpe++;
  }else{
    /* Put GPE on list only if not already there */
    for(int idx_gpe=0;idx_gpe<nbr_gpe;idx_gpe++){
      if(!strcmp(gpe_var_nm_fll,(*gpe_nm)[idx_gpe].var_nm_fll)){
        (void)fprintf(stdout,"%s: ERROR %s reports variable %s already defined in output file. HINT: Removing groups to flatten files can lead to over-determined situations where a single object name (e.g., a variable name) must refer to multiple objects in the same output group. The user's intent is ambiguous so instead of arbitrarily picking which (e.g., the last) variable of that name to place in the output file, NCO simply fails. User should re-try command after ensuring multiple objects of the same name will not be placed in the same group.\n",nco_prg_nm_get(),fnc_nm,gpe_var_nm_fll);
        for(int idx=0;idx<nbr_gpe;idx++) (*gpe_nm)[idx].var_nm_fll=(char *)nco_free((*gpe_nm)[idx].var_nm_fll);
        nco_exit(EXIT_FAILURE);
      } /* strcmp() */
    } /* end loop over gpe_nm */
    (*gpe_nm)=(gpe_nm_sct *)nco_realloc((void *)(*gpe_nm),(nbr_gpe+1)*sizeof(gpe_nm_sct));
    (*gpe_nm)[nbr_gpe].var_nm_fll=strdup(gpe_var_nm_fll);
    nbr_gpe++;
  } /* nbr_gpe_nm */

  *nbr_gpe_nm=nbr_gpe;

} /* nco_gpe_chk() */

void
nco_get_rec_dmn_nm                     /* [fnc] Return array of record names  */
(const trv_sct * const var_trv,        /* I [sct] Variable object */
 const trv_tbl_sct * const trv_tbl,    /* I [sct] GTT (Group Traversal Table) */
 nm_lst_sct ** rec_dmn_nm)             /* I/O [sct] Array of record names */ 
{
  /* Return array of record names */

  int nbr_rec;          /* [nbr] Number of entries in array */  

  dmn_trv_sct *dmn_trv; /* [sct] Unique dimension object */  

  assert(var_trv->nco_typ != nco_obj_typ_grp);

  if(*rec_dmn_nm){
    nbr_rec=(*rec_dmn_nm)->nbr;
  } else {
    nbr_rec=0;
    (*rec_dmn_nm)=(nm_lst_sct *)nco_malloc(sizeof(nm_lst_sct));
    (*rec_dmn_nm)->nbr=0;
    (*rec_dmn_nm)->lst=NULL; /* Must be NULL to nco_realloc() correct handling */
  }

  /* Loop dimensions for object (variable)  */
  for(int idx_dmn=0;idx_dmn<var_trv->nbr_dmn;idx_dmn++) {

    /* Get unique dimension object from unique dimension ID, in input list */
    dmn_trv=nco_dmn_trv_sct(var_trv->var_dmn[idx_dmn].dmn_id,trv_tbl);

    /* Dimension is a record dimension */
    if(dmn_trv->is_rec_dmn){

      /* Add one more element to table (nco_realloc nicely handles first time/not first time insertions) */
      (*rec_dmn_nm)->lst=(nm_sct *)nco_realloc((*rec_dmn_nm)->lst,(nbr_rec+1)*sizeof(nm_sct));

      /* Duplicate string into list */
      (*rec_dmn_nm)->lst[nbr_rec].nm=strdup(dmn_trv->nm);

      nbr_rec++;

    } /* Dimension is a record dimension */
  } /* Loop dimensions for object (variable)  */

  if(*rec_dmn_nm) (*rec_dmn_nm)->nbr=nbr_rec;

} /* nco_get_rec_dmn_nm() */

var_sct **                            /* O [sct] Variable list */  
nco_fll_var_trv                       /* [fnc] Fill-in variable structure list for all extracted variables */
(const int nc_id,                     /* I [id] netCDF file ID */
 int * const xtr_nbr,                 /* I/O [nbr] Number of variables in extraction list */
 const trv_tbl_sct * const trv_tbl)   /* I [sct] Traversal table */
{
  int grp_id; /* [ID] Group ID */
  int var_id; /* [ID] Variable ID */
  int idx_var;
  int nbr_xtr;

  var_sct **var=NULL;

  nbr_xtr=0;

  /* Filter variables to extract  */
  for(unsigned tbl_idx=0;tbl_idx<trv_tbl->nbr;tbl_idx++)
    if(trv_tbl->lst[tbl_idx].nco_typ != nco_obj_typ_grp && trv_tbl->lst[tbl_idx].flg_xtr) nbr_xtr++;
  
  /* Fill-in variable structure list for all extracted variables */
  var=(var_sct **)nco_malloc(nbr_xtr*sizeof(var_sct *));

  idx_var=0;

  /* Loop table */
  for(unsigned tbl_idx=0;tbl_idx<trv_tbl->nbr;tbl_idx++){

    /* Filter variables  */
    if(trv_tbl->lst[tbl_idx].nco_typ != nco_obj_typ_grp && trv_tbl->lst[tbl_idx].flg_xtr){
      trv_sct var_trv=trv_tbl->lst[tbl_idx]; 

      /* Obtain group ID from API */
      (void)nco_inq_grp_full_ncid(nc_id,var_trv.grp_nm_fll,&grp_id);

      /* Get variable ID */
      (void)nco_inq_varid(grp_id,var_trv.nm,&var_id);

      /* Transfer from table to local variable array; nco_var_fll() needs location ID and name */
      var[idx_var]=nco_var_fll_trv(grp_id,var_id,&var_trv,trv_tbl);

      /* Transfer dimensions  */
      for (int idx_dmn = 0; idx_dmn < var[idx_var]->nbr_dim; idx_dmn++)
        var[idx_var]->dim[idx_dmn]->nm_fll = strdup(var_trv.var_dmn[idx_dmn].dmn_nm_fll);

      idx_var++;

    } /* Filter variables  */
  } /* Loop table */

  *xtr_nbr=nbr_xtr;
  return var;

} /* nco_fll_var_trv() */

var_sct **                            /* O [sct] Variable list */  
nco_var_trv                           /* [fnc] Fill-in variable structure list for all variables named "var_nm" */
(const int nc_id,                     /* I [id] netCDF file ID */
 const char * const var_nm,           /* I [sng] Variable name (relative) */
 int * const xtr_nbr,                 /* I/O [nbr] Number of variables in extraction list */
 const trv_tbl_sct * const trv_tbl)   /* I [sct] Traversal table */
{
  int idx_var;
  int nbr_xtr;

  var_sct **var=NULL;

  nbr_xtr=0;

  /* Filter variables to extract */
  for(unsigned tbl_idx=0;tbl_idx<trv_tbl->nbr;tbl_idx++)
    if(trv_tbl->lst[tbl_idx].nco_typ != nco_obj_typ_grp && !strcmp(trv_tbl->lst[tbl_idx].nm,var_nm))
      nbr_xtr++;

  /* Fill-in variable structure list for all extracted variables */
  var=(var_sct **)nco_malloc(nbr_xtr*sizeof(var_sct *));

  idx_var=0;

  /* Loop table */
  for(unsigned tbl_idx=0;tbl_idx<trv_tbl->nbr;tbl_idx++){

    /* Filter variables  */
    if(trv_tbl->lst[tbl_idx].nco_typ == nco_obj_typ_var && !strcmp(trv_tbl->lst[tbl_idx].nm,var_nm)){
      trv_sct var_trv=trv_tbl->lst[tbl_idx]; 

      int grp_id; /* [ID] Group ID */
      int var_id; /* [ID] Variable ID */

      /* Obtain group ID from API */
      (void)nco_inq_grp_full_ncid(nc_id,var_trv.grp_nm_fll,&grp_id);

      /* Get variable ID */
      (void)nco_inq_varid(grp_id,var_trv.nm,&var_id);

      /* Transfer from table to local variable array; nco_var_fll() needs location ID and name */
      var[idx_var]=nco_var_fll_trv(grp_id,var_id,&var_trv,trv_tbl);

      idx_var++;
    } /* Filter variables  */
  } /* Loop table */

  *xtr_nbr=nbr_xtr;
  return var;
} /* nco_var_trv() */

void
nco_cpy_fix_var_trv                   /* [fnc] Copy fixed variables from input to output file */
(const int nc_id,                     /* I [ID] netCDF input file ID */
 const int out_id,                    /* I [ID] netCDF output file ID */
 const gpe_sct * const gpe,           /* I [sng] GPE structure */
 const trv_tbl_sct * const trv_tbl)   /* I [sct] GTT (Group Traversal Table) */
{
  char *grp_out_fll; /* [sng] Group name */

  int grp_id_in;  /* [ID] Group ID */
  int grp_id_out; /* [ID] Group ID */

  md5_sct *md5=NULL; /* [sct] MD5 configuration */

  /* Loop table */
  for(unsigned idx_tbl=0;idx_tbl<trv_tbl->nbr;idx_tbl++){
    trv_sct var_trv=trv_tbl->lst[idx_tbl];

    /* If object is a fixed variable... */ 
    if(var_trv.nco_typ == nco_obj_typ_var && var_trv.enm_prc_typ == fix_typ){
     
      /* If variable is an ensemble member, do not create it in the same location as input */
      if(var_trv.flg_nsm_mbr){
        assert(nco_prg_id_get() == ncge);
        continue;
      } /* endif */

      /* Obtain group IDs */
      (void)nco_inq_grp_full_ncid(nc_id,var_trv.grp_nm_fll,&grp_id_in);

      /* Edit group name for output */
      if(gpe) grp_out_fll=nco_gpe_evl(gpe,var_trv.grp_nm_fll); else grp_out_fll=(char *)strdup(var_trv.grp_nm_fll);

      (void)nco_inq_grp_full_ncid(out_id,grp_out_fll,&grp_id_out);

      if(nco_dbg_lvl_get() == nco_dbg_old){
        (void)fprintf(stdout,"%s: INFO writing fixed variable <%s> from ",nco_prg_nm_get(),var_trv.nm_fll);        
        (void)nco_prn_grp_nm_fll(grp_id_in);
        (void)fprintf(stdout," to ");   
        (void)nco_prn_grp_nm_fll(grp_id_out);
        (void)fprintf(stdout,"\n");
      } /* endif dbg */       

      /* Copy variable data */
      (void)nco_cpy_var_val_mlt_lmt_trv(grp_id_in,grp_id_out,(FILE *)NULL,md5,&var_trv);  

      /* Memory management after current extracted group */
      if(grp_out_fll) grp_out_fll=(char *)nco_free(grp_out_fll);

    } /* If object is a fixed variable... */ 
  } /* Loop table */

} /* nco_cpy_fix_var_trv() */

void                          
nco_prc_cmn                            /* [fnc] Process objects (ncbo only) */
(const int nc_id_1,                    /* I [id] netCDF input-file ID */
 const int nc_id_2,                    /* I [id] netCDF input-file ID */
 const int nc_out_id,                  /* I [id] netCDF output-file ID */
 const cnk_sct * const cnk,            /* I [sct] Chunking structure */
 const int dfl_lvl,                    /* I [enm] Deflate level [0..9] */
 const gpe_sct * const gpe,            /* I [sct] GPE structure */
 gpe_nm_sct *gpe_nm,                   /* I/O [sct] GPE name duplicate check array */
 int nbr_gpe_nm,                       /* I/O [nbr] Number of GPE entries */  
 const cnv_sct * const cnv,       /* I [flg] File adheres to NCAR CCM/CCSM/CF conventions */
 const nco_bool FIX_REC_CRD,           /* I [flg] Do not interpolate/multiply record coordinate variables (ncflint only) */
 CST_X_PTR_CST_PTR_CST_Y(dmn_sct,dmn_xcl),   /* I [sct] Dimensions not allowed in fixed variables */
 const int nbr_dmn_xcl,                /* I [nbr] Number of altered dimensions */
 const int nco_op_typ,                 /* I [enm] Operation type (command line -y) */
 trv_sct * trv_1,                      /* I [sct] Table object */
 trv_sct * trv_2,                      /* I [sct] Table object */
 trv_tbl_sct * const trv_tbl_1,        /* I/O [sct] GTT (Group Traversal Table) */
 trv_tbl_sct * const trv_tbl_2,        /* I/O [sct] GTT (Group Traversal Table) */
 const nco_bool flg_grp_1,             /* I [flg] Use table 1 as template for group creation on True, otherwise use table 2 */
 const nco_bool flg_dfn)               /* I [flg] Action type (True for define variables, False when write variables ) */
{
  const char fnc_nm[]="nco_prc_cmn()"; /* [sng] Function name */
  char *grp_out_fll;             /* [sng] Group name */

  int fl_fmt;                    /* [enm] netCDF file format */
  int grp_id_1;                  /* [id] Group ID in input file */
  int grp_id_2;                  /* [id] Group ID in input file */
  int grp_out_id;                /* [id] Group ID in output file */ 
  int nco_prg_id;                /* [enm] Program ID */
  int var_id_1;                  /* [id] Variable ID in input file */
  int var_id_2;                  /* [id] Variable ID in input file */
  int var_out_id;                /* [id] Variable ID in output file */

  var_sct *var_prc_1=NULL;            /* [sct] Variable to process in file 1 */
  var_sct *var_prc_2=NULL;            /* [sct] Variable to process in file 2 */
  var_sct *var_prc_out=NULL;          /* [sct] Variable to process in output */
  var_sct *var_prc_gtr;          /* [sct] Greater rank variable to process */
  var_sct *var_prc_lsr;          /* [sct] Lesser  rank variable to process */

  nco_bool RNK_1_GTR;            /* [flg] Rank of variable in file 1 variable greater than or equal to file 2 */

  prc_typ_enm prc_typ_1;         /* [enm] Processing type */
  prc_typ_enm prc_typ_2;         /* [enm] Processing type */

  assert(trv_1->nco_typ == nco_obj_typ_var);
  assert(trv_1->flg_xtr);

  assert(trv_2->nco_typ == nco_obj_typ_var);
  assert(trv_2->flg_xtr);

  /* Get Program ID */
  nco_prg_id=nco_prg_id_get(); 

  assert(nco_prg_id == ncbo);

  /* Get output file format */
  (void)nco_inq_format(nc_out_id,&fl_fmt);

  /* Edit group name for output */
  if(flg_grp_1){
    if(gpe) grp_out_fll=nco_gpe_evl(gpe,trv_1->grp_nm_fll); else grp_out_fll=(char *)strdup(trv_1->grp_nm_fll);
  }else{ /* !flg_grp_1 */
    if(gpe) grp_out_fll=nco_gpe_evl(gpe,trv_2->grp_nm_fll); else grp_out_fll=(char *)strdup(trv_2->grp_nm_fll);
  } /* !flg_grp_1 */

  /* Obtain group ID */
  (void)nco_inq_grp_full_ncid(nc_id_1,trv_1->grp_nm_fll,&grp_id_1);
  (void)nco_inq_grp_full_ncid(nc_id_2,trv_2->grp_nm_fll,&grp_id_2);

  /* Get variable ID */
  (void)nco_inq_varid(grp_id_1,trv_1->nm,&var_id_1);
  (void)nco_inq_varid(grp_id_2,trv_2->nm,&var_id_2);

  /* Allocate variable structure and fill with metadata */
  var_prc_1=nco_var_fll_trv(grp_id_1,var_id_1,trv_1,trv_tbl_1);     
  var_prc_2=nco_var_fll_trv(grp_id_2,var_id_2,trv_2,trv_tbl_2);

  if(var_prc_1->nbr_dim >= var_prc_2->nbr_dim) RNK_1_GTR=True; else RNK_1_GTR=False;
  var_prc_gtr= (RNK_1_GTR) ? var_prc_1 : var_prc_2;
  var_prc_lsr= (RNK_1_GTR) ? var_prc_2 : var_prc_1;
  var_prc_out= (RNK_1_GTR) ? nco_var_dpl(var_prc_1) : nco_var_dpl(var_prc_2);

  /* Get processing type */
  (void)nco_var_lst_dvd_ncbo(var_prc_1,var_prc_out,cnv,FIX_REC_CRD,nco_pck_map_nil,nco_pck_plc_nil,dmn_xcl,nbr_dmn_xcl,&prc_typ_1); 
  (void)nco_var_lst_dvd_ncbo(var_prc_2,var_prc_out,cnv,FIX_REC_CRD,nco_pck_map_nil,nco_pck_plc_nil,dmn_xcl,nbr_dmn_xcl,&prc_typ_2); 

  /* Conform type and rank for processed variables */
  if(prc_typ_1 == prc_typ && prc_typ_2 == prc_typ){

    int dmn_idx_gtr;
    int dmn_idx_lsr;

    /* Check that all dims in var_prc_lsr are in var_prc_gtr */
    for(dmn_idx_lsr=0;dmn_idx_lsr<var_prc_lsr->nbr_dim;dmn_idx_lsr++){
      for(dmn_idx_gtr=0;dmn_idx_gtr<var_prc_gtr->nbr_dim;dmn_idx_gtr++)  
        if(!strcmp(var_prc_lsr->dim[dmn_idx_lsr]->nm,var_prc_gtr->dim[dmn_idx_gtr]->nm)) break;
      if(dmn_idx_gtr == var_prc_gtr->nbr_dim){
        (void)fprintf(stdout,"%s: ERROR Variables do not conform: variable %s has dimension %s not present in variable %s\n",nco_prg_nm_get(),var_prc_lsr->nm,var_prc_lsr->dim[dmn_idx_lsr]->nm,var_prc_gtr->nm);
        nco_exit(EXIT_FAILURE);
      } /* endif error */
    } /* end loop over idx */

    /* Read */
    (void)nco_msa_var_get_trv(nc_id_1,var_prc_1,trv_tbl_1);
    (void)nco_msa_var_get_trv(nc_id_2,var_prc_2,trv_tbl_2);

    nc_type typ_hgh;
    typ_hgh=ncap_typ_hgh(var_prc_1->type,var_prc_2->type);

    /* Make sure variables conform in type */
    if(var_prc_1->type != var_prc_2->type)
      if(nco_dbg_lvl_get() >= nco_dbg_std && flg_dfn) (void)fprintf(stderr,"%s: INFO Input variables do not conform in type: file 1 variable %s has type %s, file 2 variable %s has type %s, output variable %s will have type %s\n",nco_prg_nm_get(),var_prc_1->nm,nco_typ_sng(var_prc_1->type),var_prc_2->nm,nco_typ_sng(var_prc_2->type),var_prc_gtr->nm,nco_typ_sng(typ_hgh));

    typ_hgh=ncap_var_retype(var_prc_1,var_prc_2);
    trv_1->var_typ=trv_2->var_typ=typ_hgh;

    /* Broadcast lesser to greater variable. NB: Pointers may change so _gtr, _lsr not valid */
    if(var_prc_1->nbr_dim != var_prc_2->nbr_dim){
      (void)ncap_var_cnf_dmn(&var_prc_1,&var_prc_2);
    }else{
      /* Still possible that variables are mismatched even if ranks are equal
	 One or more dimensions could be degenerate
	 Before subtraction must make sure dimension sizes match
	 Or re-code ncap_var_cnf_dmn() so that it understands how to broadcast degenerate dimensions */
      for(int idx_dmn=0;idx_dmn<var_prc_1->nbr_dim;idx_dmn++){
        if(var_prc_1->dim[idx_dmn]->sz != var_prc_2->dim[idx_dmn]->sz){
          (void)fprintf(stdout,"%s: ERROR Variables do not conform: Variable \"%s\" has C-based dimension #%d name and size of \"%s\" and %ld in file one, and name and size of \"%s\" and %ld in file two. One of these dimensions is not like the other. HINT: Be sure the variable's dimensions are not permuted in one file relative to the other. If they are, first use ncpdq to order the dimensions the same way in both input files, then re-try command.\n",nco_prg_nm_get(),var_prc_1->nm_fll,idx_dmn,var_prc_1->dim[idx_dmn]->nm,var_prc_1->dim[idx_dmn]->sz,var_prc_2->dim[idx_dmn]->nm,var_prc_2->dim[idx_dmn]->sz);
          nco_exit(EXIT_FAILURE);
        } /* endif error */
      } /* end loop over idx_dmn */
    } /* end else */

    /* var1 and var2 now conform in size and type to eachother and are in memory */
    assert(var_prc_1->type == var_prc_2->type);
    assert(trv_1->var_typ == trv_2->var_typ);
    assert(trv_1->var_typ == var_prc_1->type);

  } /* Conform type and rank for process variables */

  /* Define mode */
  if(flg_dfn){  
    char *rec_dmn_nm=NULL; /* [sng] Record dimension name */

    nm_lst_sct *rec_dmn_nm_1=NULL; /* [sct] Record dimension names array */
    nm_lst_sct *rec_dmn_nm_2=NULL; /* [sct] Record dimension names array */

    nco_bool PCK_ATT_CPY; /* [flg] Copy attributes "scale_factor", "add_offset" */

    PCK_ATT_CPY=nco_pck_cpy_att(nco_prg_id,nco_pck_plc_nil,var_prc_1);

    /* If output group does not exist, create it */
    if(nco_inq_grp_full_ncid_flg(nc_out_id,grp_out_fll,&grp_out_id)) nco_def_grp_full(nc_out_id,grp_out_fll,&grp_out_id);

    /* Detect duplicate GPE names in advance, then exit with helpful error */
    if(gpe) (void)nco_gpe_chk(grp_out_fll,trv_1->nm,&gpe_nm,&nbr_gpe_nm);  

    /* Get array of record names for object */
    (void)nco_get_rec_dmn_nm(trv_1,trv_tbl_1,&rec_dmn_nm_1);               
    (void)nco_get_rec_dmn_nm(trv_2,trv_tbl_2,&rec_dmn_nm_2);    

    /* Use for record dimension name the first in array */
    if(rec_dmn_nm_1->lst) rec_dmn_nm=(char *)strdup(rec_dmn_nm_1->lst[0].nm);
    if(!rec_dmn_nm && rec_dmn_nm_2->lst) rec_dmn_nm=(char *)strdup(rec_dmn_nm_2->lst[0].nm);

    /* Define variable in output file */
    if(RNK_1_GTR) var_out_id=nco_cpy_var_dfn_trv(nc_id_1,nc_out_id,cnk,grp_out_fll,dfl_lvl,gpe,rec_dmn_nm,trv_1,NULL,0,trv_tbl_1); else var_out_id=nco_cpy_var_dfn_trv(nc_id_2,nc_out_id,cnk,grp_out_fll,dfl_lvl,gpe,rec_dmn_nm,trv_2,NULL,0,trv_tbl_2);

    /* Copy variable's attributes */
    if(RNK_1_GTR) (void)nco_att_cpy(grp_id_1,grp_out_id,var_id_1,var_out_id,PCK_ATT_CPY); else (void)nco_att_cpy(grp_id_2,grp_out_id,var_id_2,var_out_id,PCK_ATT_CPY);

    /* Memory management for record dimension names */
    if(rec_dmn_nm) rec_dmn_nm=(char *)nco_free(rec_dmn_nm);
    if(rec_dmn_nm_1){
      for(int idx=0;idx<rec_dmn_nm_1->nbr;idx++) rec_dmn_nm_1->lst[idx].nm=(char *)nco_free(rec_dmn_nm_1->lst[idx].nm);
      rec_dmn_nm_1=(nm_lst_sct *)nco_free(rec_dmn_nm_1);
    } /* !rec_dmn_nm_1 */
    if(rec_dmn_nm_2){
      for(int idx=0;idx<rec_dmn_nm_2->nbr;idx++) rec_dmn_nm_2->lst[idx].nm=(char *)nco_free(rec_dmn_nm_2->lst[idx].nm);
      rec_dmn_nm_2=(nm_lst_sct *)nco_free(rec_dmn_nm_2);
    } /* !rec_dmn_nm_2 */

  }else{ /* !flg_dfn */
    /* Write mode */
    int has_mss_val; /* [flg] Variable has missing value */

    md5_sct *md5=NULL; /* [sct] MD5 configuration */

    ptr_unn mss_val; /* [sct] Missing value */

    /* Get group ID */
    (void)nco_inq_grp_full_ncid(nc_out_id,grp_out_fll,&grp_out_id);

    /* Get variable ID */
    (void)nco_inq_varid(grp_out_id,trv_1->nm,&var_out_id);         

    if(nco_dbg_lvl_get() >= nco_dbg_vrb) (void)fprintf(stdout,"%s: INFO %s reports operation type %d for %s\n",nco_prg_nm_get(),fnc_nm,prc_typ_1,trv_1->nm_fll);

    /* Non-processed variable */
    if(prc_typ_1 == fix_typ || prc_typ_2 == fix_typ){
      if(RNK_1_GTR) (void)nco_cpy_var_val_mlt_lmt_trv(grp_id_1,grp_out_id,(FILE *)NULL,md5,trv_1); else (void)nco_cpy_var_val_mlt_lmt_trv(grp_id_2,grp_out_id,(FILE *)NULL,md5,trv_2);
    } /* endif fix */

    /* Processed variable */
    if(prc_typ_1 == prc_typ && prc_typ_2 == prc_typ){

      var_prc_out->id=var_out_id;

      /* fxm: gtr or lsr? */
      for(int idx_dmn=0;idx_dmn<var_prc_out->nbr_dim;idx_dmn++){
        var_prc_out->srt[idx_dmn]=var_prc_gtr->srt[idx_dmn];
        var_prc_out->cnt[idx_dmn]=var_prc_gtr->cnt[idx_dmn];
      } /* endfor */

      /* Set missing value */
      has_mss_val=var_prc_gtr->has_mss_val;

      /* Change missing_value, if any, of lesser rank to missing_value, if any, of greater rank */
      if(RNK_1_GTR) has_mss_val=nco_mss_val_cnf(var_prc_1,var_prc_2); else has_mss_val=nco_mss_val_cnf(var_prc_2,var_prc_1);

      /* mss_val of larger rank, if any, overrides mss_val of smaller rank */
      if(has_mss_val) mss_val= (RNK_1_GTR) ? var_prc_1->mss_val : var_prc_2->mss_val;

      /* Perform specified binary operation */
      switch(nco_op_typ){
      case nco_op_add: /* [enm] Add file_1 to file_2 */
        (void)nco_var_add(var_prc_1->type,var_prc_1->sz,has_mss_val,mss_val,var_prc_2->val,var_prc_1->val); break;
      case nco_op_mlt: /* [enm] Multiply file_1 by file_2 */
        (void)nco_var_mlt(var_prc_1->type,var_prc_1->sz,has_mss_val,mss_val,var_prc_2->val,var_prc_1->val); break;
      case nco_op_dvd: /* [enm] Divide file_1 by file_2 */
        (void)nco_var_dvd(var_prc_1->type,var_prc_1->sz,has_mss_val,mss_val,var_prc_2->val,var_prc_1->val); break;
      case nco_op_sbt: /* [enm] Subtract file_2 from file_1 */
        (void)nco_var_sbt(var_prc_1->type,var_prc_1->sz,has_mss_val,mss_val,var_prc_2->val,var_prc_1->val); break;
      default: /* Other defined nco_op_typ values are valid for ncra(), ncrcat(), ncwa(), not ncbo() */
        (void)fprintf(stdout,"%s: ERROR Illegal nco_op_typ in binary operation\n",nco_prg_nm_get());
        nco_exit(EXIT_FAILURE);
        break;
      } /* end case */

      if(trv_1->ppc != NC_MAX_INT){
	if(trv_1->flg_nsd) (void)nco_ppc_bitmask(trv_1->ppc,var_prc_1->type,var_prc_out->sz,var_prc_out->has_mss_val,var_prc_out->mss_val,var_prc_1->val); else (void)nco_ppc_around(trv_1->ppc,var_prc_1->type,var_prc_out->sz,var_prc_out->has_mss_val,var_prc_out->mss_val,var_prc_1->val);
      } /* endif ppc */
      if(nco_is_xcp(trv_1->nm)) nco_xcp_prc(trv_1->nm,var_prc_1->type,var_prc_out->sz,(char *)var_prc_1->val.vp);

      /* Copy result to output file */
      if(var_prc_1->nbr_dim == 0) (void)nco_put_var1(grp_out_id,var_prc_out->id,var_prc_out->srt,var_prc_1->val.vp,var_prc_1->type); else (void)nco_put_vara(grp_out_id,var_prc_out->id,var_prc_out->srt,var_prc_out->cnt,var_prc_1->val.vp,var_prc_1->type);

    } /* !prc_typ */
  } /* !flg_dfn */

  for(int idx_dmn=0;idx_dmn<var_prc_1->nbr_dim;idx_dmn++){
    var_prc_1->dim[idx_dmn]->xrf=(dmn_sct *)nco_dmn_free(var_prc_1->dim[idx_dmn]->xrf);
    var_prc_1->dim[idx_dmn]=(dmn_sct *)nco_dmn_free(var_prc_1->dim[idx_dmn]);   
  } /* endfor */

  /* Free allocated variable structures */
  var_prc_out=(var_sct *)nco_var_free(var_prc_out);
  var_prc_1=(var_sct *)nco_var_free(var_prc_1);
  var_prc_2=(var_sct *)nco_var_free(var_prc_2);

  /* Free output path name */
  grp_out_fll=(char *)nco_free(grp_out_fll);
} /* nco_prc_cmn() */

void                          
nco_cpy_fix                            /* [fnc] Copy fixed object (ncbo only) */
(const int nc_id_1,                    /* I [id] netCDF input-file ID */
 const int nc_out_id,                  /* I [id] netCDF output-file ID */
 const cnk_sct * const cnk,            /* I [sct] Chunking structure */
 const int dfl_lvl,                    /* I [enm] Deflate level [0..9] */
 const gpe_sct * const gpe,            /* I [sct] GPE structure */
 gpe_nm_sct *gpe_nm,                   /* I/O [sct] GPE name duplicate check array */
 int nbr_gpe_nm,                       /* I/O [nbr] Number of GPE entries */  
 const cnv_sct * const cnv,       /* I [flg] File adheres to NCAR CCM/CCSM/CF conventions */
 const nco_bool FIX_REC_CRD,           /* I [flg] Do not interpolate/multiply record coordinate variables (ncflint only) */
 CST_X_PTR_CST_PTR_CST_Y(dmn_sct,dmn_xcl),   /* I [sct] Dimensions not allowed in fixed variables */
 const int nbr_dmn_xcl,                /* I [nbr] Number of altered dimensions */
 trv_sct *trv_1,                       /* I/O [sct] Table object (nco_cpy_var_dfn_trv) */
 trv_tbl_sct * const trv_tbl_1,        /* I/O [sct] GTT (Group Traversal Table) */
 const nco_bool flg_dfn)               /* I [flg] Action type (True for define variables, False when write variables ) */
{
  char *grp_out_fll;             /* [sng] Group name */

  int fl_fmt;                    /* [enm] netCDF file format */
  int grp_id_1;                  /* [id] Group ID in input file */
  int grp_out_id;                /* [id] Group ID in output file */  
  int nco_prg_id;                /* [enm] Program ID */
  int var_id_1;                  /* [id] Variable ID in input file */
  int var_out_id;                /* [id] Variable ID in output file */

  md5_sct *md5=NULL; /* [sct] MD5 configuration */

  var_sct *var_prc_1;            /* [sct] Variable to process in file 1 */
  var_sct *var_prc_out;          /* [sct] Variable to process in output */

  prc_typ_enm prc_typ_1; /* [enm] Processing type */

  assert(trv_1->nco_typ == nco_obj_typ_var);
  assert(trv_1->flg_xtr);

  /* Get Program ID */
  nco_prg_id=nco_prg_id_get(); 

  /* Get output file format */
  (void)nco_inq_format(nc_out_id,&fl_fmt);

  /* Edit group name for output */
  if(gpe) grp_out_fll=nco_gpe_evl(gpe,trv_1->grp_nm_fll); else grp_out_fll=(char *)strdup(trv_1->grp_nm_fll);

  /* Obtain group ID */
  (void)nco_inq_grp_full_ncid(nc_id_1,trv_1->grp_nm_fll,&grp_id_1);

  /* Get variable ID */
  (void)nco_inq_varid(grp_id_1,trv_1->nm,&var_id_1);

  /* Allocate variable structure and fill with metadata */
  var_prc_1=nco_var_fll_trv(grp_id_1,var_id_1,trv_1,trv_tbl_1);     

  var_prc_out=nco_var_dpl(var_prc_1);
  (void)nco_var_lst_dvd_ncbo(var_prc_1,var_prc_out,cnv,FIX_REC_CRD,nco_pck_map_nil,nco_pck_plc_nil,dmn_xcl,nbr_dmn_xcl,&prc_typ_1); 

  if(prc_typ_1 != fix_typ){
    var_prc_1=(var_sct *)nco_var_free(var_prc_1);
    var_prc_out=(var_sct *)nco_var_free(var_prc_out);
    grp_out_fll=(char *)nco_free(grp_out_fll);
    return;
  } /* endif */

  if(flg_dfn){  
    /* Define mode */
    nco_bool PCK_ATT_CPY; /* [flg] Copy attributes "scale_factor", "add_offset" */

    PCK_ATT_CPY=nco_pck_cpy_att(nco_prg_id,nco_pck_plc_nil,var_prc_1);

    /* If output group does not exist, create it */
    if(nco_inq_grp_full_ncid_flg(nc_out_id,grp_out_fll,&grp_out_id)) nco_def_grp_full(nc_out_id,grp_out_fll,&grp_out_id);

    /* Detect duplicate GPE names in advance, then exit with helpful error */
    if(gpe)(void)nco_gpe_chk(grp_out_fll,trv_1->nm,&gpe_nm,&nbr_gpe_nm);

    /* Define variable in output file */
    var_out_id=nco_cpy_var_dfn_trv(nc_id_1,nc_out_id,cnk,grp_out_fll,dfl_lvl,gpe,(char *)NULL,trv_1,NULL,0,trv_tbl_1);

    /* Copy variable's attributes */
    (void)nco_att_cpy(grp_id_1,grp_out_id,var_id_1,var_out_id,PCK_ATT_CPY); 

  }else{ /* !flg_dfn */
    /* Write mode */
    /* Get group ID */
    (void)nco_inq_grp_full_ncid(nc_out_id,grp_out_fll,&grp_out_id);

    /* Get variable ID */
    (void)nco_inq_varid(grp_out_id,trv_1->nm,&var_out_id);         

    /* Copy non-processed variable */
    (void)nco_cpy_var_val_mlt_lmt_trv(grp_id_1,grp_out_id,(FILE *)NULL,md5,trv_1); 
  } /* !flg_dfn */

  /* Free allocated variable structures */
  var_prc_1=(var_sct *)nco_var_free(var_prc_1);
  var_prc_out=(var_sct *)nco_var_free(var_prc_out);

  /* Free output path name */
  grp_out_fll=(char *)nco_free(grp_out_fll);
} /* nco_cpy_fix() */

nco_bool                               /* O [flg] Copy packing attributes */
nco_pck_cpy_att                        /* [fnc] Inquire about copying packing attributes  */
(const int nco_prg_id,                     /* I [enm] Program ID */
 const int nco_pck_plc,                /* I [enm] Packing policy */
 const var_sct * const var_prc)        /* I [sct] Variable */
{
  nco_bool PCK_ATT_CPY=True; /* [flg] Copy attributes "scale_factor", "add_offset" */
  
  /* Copy all attributes except in cases where packing/unpacking is involved
     0. Variable is unpacked on input, unpacked on output
     --> Copy all attributes
     1. Variable is packed on input, is not altered, and remains packed on output
     --> Copy all attributes
     2. Variable is packed on input, is unpacked for some reason, and will be unpacked on output
     --> Copy all attributes except scale_factor and add_offset
     3. Variable is packed on input, is unpacked for some reason, and will be packed on output (possibly with new packing attributes)
     --> Copy all attributes, but scale_factor and add_offset must be overwritten later with new values
     4. Variable is not packed on input, packing is performed, and output is packed
     --> Copy all attributes, define dummy values for scale_factor and add_offset now, and write those values later, when they are known */

  /* Do not copy packing attributes "scale_factor" and "add_offset" 
     if variable is packed in input file and unpacked in output file 
     Arithmetic operators calling nco_var_dfn() with fixed variables should leave them fixed
     Currently ncap calls nco_var_dfn() only for fixed variables, so handle exception with ncap-specific condition */
  /* Copy exising packing attributes, if any, unless... */
  if(nco_is_rth_opr(nco_prg_id) && /* ...operator is arithmetic... */
    nco_prg_id != ncap && /* ...and is not ncap (hence it must be, e.g., ncra, ncbo)... */
    !var_prc->is_fix_var && /* ...and variable is processed (not fixed)... */
    var_prc->pck_dsk) /* ...and variable is packed in input file... */
    PCK_ATT_CPY=False;

  /* Do not copy packing attributes when unpacking variables 
     ncpdq is currently only operator that passes values other than nco_pck_plc_nil */
  if(nco_pck_plc == nco_pck_plc_upk) /* ...and variable will be _unpacked_ ... */
    PCK_ATT_CPY=False;  

  return PCK_ATT_CPY;
} /* nco_pck_cpy_att() */

void
nco_var_prc_fix_trv                    /* [fnc] Store processed and fixed variables info into GTT */
(const int nbr_var_prc,                /* I [nbr] Number of processed variables */
 var_sct **var_prc,                    /* I [sct] Array of processed variables */
 const int nbr_var_fix,                /* I [nbr] Number of fixed variables */
 var_sct **var_fix,                    /* I [sct] Array of fixed variables */
 trv_tbl_sct * const trv_tbl)          /* I/O [sct] Traversal table */
{
  /* Purpose: Store processed and fixed variables info into GTT */

  /* Store processed variables info into table */
  for(int idx_var=0;idx_var<nbr_var_prc;idx_var++)
    (void)trv_tbl_mrk_prc_fix(var_prc[idx_var]->nm_fll,prc_typ,trv_tbl);

  /* Store fixed variables info into table */
  for(int idx_var=0;idx_var<nbr_var_fix;idx_var++)
    (void)trv_tbl_mrk_prc_fix(var_fix[idx_var]->nm_fll,fix_typ,trv_tbl);

  return;
} /* end nco_var_prc_fix_trv() */

void
nco_var_typ_trv /* [fnc] Transfer variable type into GTT */
(const int prc_nbr,                    /* I [nbr] Number of processed variables */
 CST_X_PTR_CST_PTR_CST_Y(var_sct,var), /* I [sct] Array of extracted variables */
 trv_tbl_sct * const trv_tbl)          /* I/O [sct] Traversal table */
{
  /* Purpose: Transfer variable type to table */

  /* Loop table */
  for(int idx_var=0;idx_var<prc_nbr;idx_var++){

    nc_type typ_out;         /* [enm] Type in output file */
    assert(var[idx_var]);

    /* Obtain netCDF type to define variable from NCO program ID */
    typ_out=nco_get_typ(var[idx_var]);

    /* Mark output type in table for "nm_fll" */
    for(unsigned idx_tbl=0;idx_tbl<trv_tbl->nbr;idx_tbl++){
      if(!strcmp(var[idx_var]->nm_fll,trv_tbl->lst[idx_tbl].nm_fll)){
        trv_tbl->lst[idx_tbl].var_typ_out=typ_out;
        break;
      } /* Match */
    } /* !idx_tbl */

  } /* !idx_var */

  return;
} /* end nco_var_typ_trv() */

void
nco_set_prm_typ_out /* [fnc] Set GTT variable output type to unpacked, arithmetically promoted type for integers */
(nco_bool const PROMOTE_INTS, /* I/O [flg] Promote integers to floating point in output */
 const int prc_nbr, /* I [nbr] Number of processed variables */
 CST_X_PTR_CST_PTR_CST_Y(var_sct,var), /* I [sct] Array of extracted variables */
 trv_tbl_sct * const trv_tbl) /* I/O [sct] Traversal table */
{
  /* Purpose: Set GTT variable output type to unpacked, arithmetically promoted type for integers
     20200830: Feature first introduced in ncra so small types like NC_BYTE can be output as, e.g., NC_DOUBLE 
     Paul Ullrich uses NC_BYTE to hold Boolean flags and averages these over many files 
     Demoting back to NC_BYTE on output discards floating point averages */
  const char fnc_nm[]="nco_set_prm_typ_out()"; /* [sng] Function name */

  nc_type var_typ_out=NC_NAT; /* [enm] Type in output file */

  /* Loop table */
  for(int idx_var=0;idx_var<prc_nbr;idx_var++){

    assert(var[idx_var]);
    /* Output Type depends on input type */
    if(var[idx_var]->is_fix_var){
      var_typ_out=var[idx_var]->type;
    }else if(!PROMOTE_INTS){
      var_typ_out=var[idx_var]->typ_upk;
    }else{
      switch(var[idx_var]->typ_upk){
      case NC_FLOAT: 
      case NC_DOUBLE: 
      case NC_CHAR:
      case NC_STRING:
	var_typ_out=var[idx_var]->typ_upk;
	break;
	/* Do not un-promote integers after processing */
      case NC_INT:
      case NC_SHORT:
      case NC_BYTE:
      case NC_UBYTE: 
      case NC_USHORT:
      case NC_UINT: 
      case NC_INT64: 
      case NC_UINT64: 
	/* Archiving processed integers as NC_DOUBLE preserves most precision yet is probably overkill
	   NC_FLOAT saves 2x space and still gives ~7 digits precision */
	var_typ_out=NC_FLOAT;
	//	var_typ_out=NC_DOUBLE;
	break;
      default: nco_dfl_case_nc_type_err(); break;
      } /* end switch */
    } /* !PROMOTE_INTS */
    
    if(nco_dbg_lvl_get() >= nco_dbg_var) (void)fprintf(stdout,"%s: %s reports var[%d]=%s, type=%s, typ_dsk=%s, typ_pck=%s, typ_upk=%s, var_typ_out=%s\n",nco_prg_nm_get(),fnc_nm,idx_var,var[idx_var]->nm_fll,nco_typ_sng(var[idx_var]->type),nco_typ_sng(var[idx_var]->typ_dsk),nco_typ_sng(var[idx_var]->typ_pck),nco_typ_sng(var[idx_var]->typ_upk),nco_typ_sng(var_typ_out));

    /* Mark output type in table for "nm_fll" */
    for(unsigned idx_tbl=0;idx_tbl<trv_tbl->nbr;idx_tbl++){
      if(!strcmp(var[idx_var]->nm_fll,trv_tbl->lst[idx_tbl].nm_fll)){
	trv_tbl->lst[idx_tbl].var_typ_out=var_typ_out;
	break;
      } /* !strcmp() */
    } /* !idx_tbl */
  } /* !idx_var */
  
  return;
} /* end nco_set_prm_typ_out() */

var_sct *                             /* O [sct] Variable structure */
nco_var_fll_trv                       /* [fnc] Allocate variable structure and fill with metadata */
(const int grp_id,                    /* I [id] Group ID */
 const int var_id,                    /* I [id] Variable ID */
 const trv_sct * const var_trv,       /* I [sct] Object to write (variable) */
 const trv_tbl_sct * const trv_tbl)   /* I [sct] GTT (Group Traversal Table) */
{
  /* Purpose: Allocate and return a completed var_sct; traversal version of nco_var_fll() */

  char dmn_nm[NC_MAX_NAME+1];      /* [sng] Dimension name  */  

  int fl_fmt;                    /* [enm] File format */  
  int *dmn_in_id_var;            /* [id] Dimension IDs array for variable */
  int nco_prg_id;                /* [enm] Program ID */
  int dmn_id;                    /* [nbr] Dimension ID */
  
  long dmn_cnt;                  /* [nbr] Dimensio hyperslabbed count (size) */
  long dmn_sz;                   /* [nbr] Dimension size  */  

  var_sct *var;                  /* [sct] Variable structure (output) */   
  dmn_trv_sct *dmn_trv;          /* [sct] GTT unique dimension object */ 
  dmn_sct *dim;                  /* [sct] Dimension structure */  
   
  /* Get Program ID */
  nco_prg_id=nco_prg_id_get(); 

  assert(var_trv->nco_typ == nco_obj_typ_var);

  /* Get file format */
  (void)nco_inq_format(grp_id,&fl_fmt);

  /* Allocate space for variable structure */
  var=(var_sct *)nco_malloc(sizeof(var_sct));

  /* Set defaults for each member of variable structure */
  (void)var_dfl_set(var); 

  /* Fill-in known fields */
  var->nm=(char *)strdup(var_trv->nm);
  var->nm_fll=(char *)strdup(var_trv->nm_fll);
  var->id=var_id;
  var->nc_id=grp_id;
  var->is_crd_var=var_trv->is_crd_var; 

  /* Get type and number of dimensions and attributes for variable */
  (void)nco_inq_var(var->nc_id,var->id,(char *)NULL,&var->typ_dsk,&var->nbr_dim,(int *)NULL,&var->nbr_att);

  if(nco_prg_id == ncks) assert(var->typ_dsk == var_trv->var_typ);

  assert(var->nbr_dim == var_trv->nbr_dmn);
  assert(var->nbr_att == var_trv->nbr_att);

  dmn_in_id_var=(int *)nco_malloc(var->nbr_dim*sizeof(int));

  /* Get dimension IDs for *variable* */
  (void)nco_inq_vardimid(var->nc_id,var->id,dmn_in_id_var); 

  /* Allocate space for dimension information */
  if(var->nbr_dim > 0) var->dim=(dmn_sct **)nco_malloc(var->nbr_dim*sizeof(dmn_sct *)); else var->dim=(dmn_sct **)NULL;
  if(var->nbr_dim > 0) var->dmn_id=(int *)nco_malloc(var->nbr_dim*sizeof(int)); else var->dmn_id=(int *)NULL;
  if(var->nbr_dim > 0) var->cnk_sz=(size_t *)nco_malloc(var->nbr_dim*sizeof(size_t)); else var->cnk_sz=(size_t *)NULL;
  if(var->nbr_dim > 0) var->cnt=(long *)nco_malloc(var->nbr_dim*sizeof(long)); else var->cnt=(long *)NULL;
  if(var->nbr_dim > 0) var->srt=(long *)nco_malloc(var->nbr_dim*sizeof(long)); else var->srt=(long *)NULL;
  if(var->nbr_dim > 0) var->end=(long *)nco_malloc(var->nbr_dim*sizeof(long)); else var->end=(long *)NULL;
  if(var->nbr_dim > 0) var->srd=(long *)nco_malloc(var->nbr_dim*sizeof(long)); else var->srd=(long *)NULL;

  /* Get dimension IDs from input file */
  (void)nco_inq_vardimid(var->nc_id,var->id,var->dmn_id); 

  /* Size defaults to 1 in var_dfl_set(), and set to 1 here for extra safety */
  var->sz=1L;
  var->sz_rec=1L;

  /* Uninitialized values */ 
  var->cid=-1;
  var->fmt[0]='\0';

  /* Loop dimensions */
  for(int idx_dmn=0;idx_dmn<var->nbr_dim;idx_dmn++){

    /* Dimension ID for variable, used to get dimension object in input list */
    dmn_id=dmn_in_id_var[idx_dmn];

    assert(var->dmn_id[idx_dmn] == dmn_id);

    /* Get unique dimension object from unique dimension ID, in input list */
    dmn_trv=nco_dmn_trv_sct(dmn_id,trv_tbl);

    /* Get dimension name and size from ID in group */
    (void)nco_inq_dim(grp_id,dmn_id,dmn_nm,&dmn_sz);

    assert((size_t)dmn_sz == dmn_trv->sz);
    assert(!strcmp(dmn_nm,dmn_trv->nm));

    /* Get hyperslabbed count */
    dmn_cnt=-1L;
    if(var_trv->var_dmn[idx_dmn].crd) dmn_cnt=var_trv->var_dmn[idx_dmn].crd->lmt_msa.dmn_cnt;
    else if(var_trv->var_dmn[idx_dmn].ncd) dmn_cnt=var_trv->var_dmn[idx_dmn].ncd->lmt_msa.dmn_cnt;

    var->cnt[idx_dmn]=dmn_cnt;
    var->end[idx_dmn]=dmn_cnt-1L;
    var->srt[idx_dmn]=0L;
    var->srd[idx_dmn]=1L;
    var->sz*=dmn_cnt;
    
    /* This definition of "is_rec_var" says if any of the dimensions is a record then the variable is marked as so 
       20170411: Yes, because that IS the correct definition! */
    if(dmn_trv->is_rec_dmn) var->is_rec_var=True; else var->sz_rec*=var->cnt[idx_dmn];

    /* Return a completed dmn_sct, use dimension ID and name from TRV object */
    dim=nco_dmn_fll(grp_id,dmn_id,dmn_trv->nm);

    assert(!strcmp(dim->nm,dmn_trv->nm));
    assert((size_t)dim->sz == dmn_trv->sz);  
    assert(dim->id == var->dmn_id[idx_dmn]);  

    /* Set the hyperslabbed size for this dimension */
    dim->cnt=dmn_cnt;

    /* Set the *real* (NB: var->sz is hyperslabbed size) size for this dimension */
    dim->sz=dmn_sz;
 
    /* Use info from GTT unique dimension */
    dim->is_rec_dmn=dmn_trv->is_rec_dmn;

     /* Use info from GTT variable dimension */
    dim->is_crd_dmn=var_trv->var_dmn[idx_dmn].is_crd_var; 

    var->dim[idx_dmn]=(dmn_sct *)nco_malloc(sizeof(dmn_sct));
    var->dim[idx_dmn]->nm=(char *)strdup(dim->nm);
    var->dim[idx_dmn]->nm_fll=NULL;
    var->dim[idx_dmn]->id=dim->id;
    var->dim[idx_dmn]->cnk_sz=dim->cnk_sz;
    var->dim[idx_dmn]->srt=dim->srt;
    var->dim[idx_dmn]->end=dim->end;
    var->dim[idx_dmn]->srd=dim->srd;
    var->dim[idx_dmn]->cnt=dim->cnt;
    var->dim[idx_dmn]->sz=dmn_sz;
    var->dim[idx_dmn]->is_rec_dmn=dim->is_rec_dmn;
    var->dim[idx_dmn]->is_crd_dmn=dim->is_crd_dmn;

    dim->xrf=(dmn_sct *)nco_malloc(sizeof(dmn_sct));
    dim->xrf->nm=(char *)strdup(dim->nm);
    dim->xrf->id=dim->id;
    dim->xrf->cnk_sz=dim->cnk_sz;
    dim->xrf->srt=dim->srt;
    dim->xrf->end=dim->end;
    dim->xrf->srd=dim->srd;
    dim->xrf->cnt=dim->cnt;
    dim->xrf->sz=dim->sz;
    dim->xrf->is_rec_dmn=dim->is_rec_dmn;
    dim->xrf->is_crd_dmn=dim->is_crd_dmn;

    var->dim[idx_dmn]->xrf=nco_dmn_dpl(dim->xrf);
    dim->xrf=nco_dmn_free(dim->xrf);
    dim=nco_dmn_free(dim);

  } /* Loop dimensions */

  /* Type in memory begins as same type as on disk */
  /* Type of variable in RAM */
  var->type=var->typ_dsk; 

  /* Type of packed data on disk */
  /* Type of variable when packed (on disk). This should be same as typ_dsk except in cases where variable is packed in input file and unpacked in output file. */
  var->typ_pck=var->type;  

  /* Refresh number of attributes and missing value attribute, if any */
  var->has_mss_val=nco_mss_val_get(var->nc_id,var);

  /* Check variable for duplicate dimensions */
  for(int idx_var=0;idx_var<var->nbr_dim;idx_var++){
    int idx_dmn;
    for(idx_dmn=0;idx_dmn<var->nbr_dim;idx_dmn++){
      if(idx_var != idx_dmn){
        if(var->dmn_id[idx_var] == var->dmn_id[idx_dmn]){
          /* Dimensions are duplicated when IDs for different ordinal dimensions are equal */
          var->has_dpl_dmn=True;
          break;
        } /* endif IDs are equal */
      } /* endif navel gazing */
    } /* endif inner dimension */
    /* Found a duplicate, so stop looking */
    if(idx_dmn != var->nbr_dim) break;
  } /* Check variable for duplicate dimensions */

  /* Treat variables associated with "bounds", "climatology", and "coordinates" attributes as coordinates */
  if(nco_is_spc_in_cf_att(var->nc_id, "bounds", var->id, NULL)) var->is_crd_var=True;
  if(nco_is_spc_in_cf_att(var->nc_id, "climatology", var->id, NULL)) var->is_crd_var=True;
  if(nco_is_spc_in_cf_att(var->nc_id, "coordinates", var->id, NULL)) var->is_crd_var=True;

  /* Portions of variable structure depend on packing properties, e.g., typ_upk nco_pck_dsk_inq() fills in these portions harmlessly */
  (void)nco_pck_dsk_inq(grp_id,var);

  /* Set deflate and chunking to defaults */  
  var->dfl_lvl=NCO_DFL_LVL_UNDEFINED; /* [enm] Deflate level */
  var->shuffle=NC_NOSHUFFLE; /* [flg] Turn-on shuffle filter */

  for(int idx=0;idx<var->nbr_dim;idx++) var->cnk_sz[idx]=(size_t)0L;

  /* Read deflate levels and chunking (if any) */  
  if((nco_fmt_xtn_get() != nco_fmt_xtn_hdf4 || (NC_LIB_VERSION >= 433 && NC_LIB_VERSION != 474)) && (fl_fmt == NC_FORMAT_NETCDF4 || fl_fmt == NC_FORMAT_NETCDF4_CLASSIC)){
    int deflate; /* [enm] Deflate filter is on */
    int srg_typ; /* [enm] Storage type */
    (void)nco_inq_var_deflate(grp_id,var->id,&var->shuffle,&deflate,&var->dfl_lvl);    
    (void)nco_inq_var_chunking(grp_id,var->id,&srg_typ,var->cnk_sz);   
  } /* endif */

  /* Get enm_prc_typ from GTT */
  for(unsigned idx_tbl=0;idx_tbl<trv_tbl->nbr;idx_tbl++){
    if(!strcmp(var->nm_fll,trv_tbl->lst[idx_tbl].nm_fll)){
      if(trv_tbl->lst[idx_tbl].enm_prc_typ == prc_typ) var->is_fix_var=False; 
      else if(trv_tbl->lst[idx_tbl].enm_prc_typ == fix_typ) var->is_fix_var=True;
      break;
    } /* endif */
  } /* endfor */

  var->undefined=False; /* [flg] Used by ncap parser */

  dmn_in_id_var=(int *)nco_free(dmn_in_id_var);

  return var;
} /* nco_var_fll_trv() */

int                                 /* O [id] Output file variable ID */
nco_cpy_var_dfn_trv                 /* [fnc] Define specified variable in output file */
(const int nc_in_id,                /* I [ID] netCDF input file ID */
 const int nc_out_id,               /* I [ID] netCDF output file ID */
 const cnk_sct * const cnk,         /* I [sct] Chunking structure */
 const char * const grp_out_fll,    /* I [sng] Output group name */
 const int dfl_lvl,                 /* I [enm] Deflate level [0..9] */
 const gpe_sct * const gpe,         /* I [sct] GPE structure */
 const char * const rec_dmn_nm_cst, /* I [sng] User-specified record dimension, if any, to create or fix in output file */
 trv_sct *var_trv,                  /* I/O [sct] Object to write (variable) trv_map_dmn_set() is O */
 dmn_cmn_sct **dmn_cmn_out,         /* I/O [sct] List of all dimensions in output file (used for RETAIN_ALL_DIMS) */
 int *nbr_dmn_cmn_out,              /* I/O [sct] Number of all dimensions in output file (used for RETAIN_ALL_DIMS) */
 trv_tbl_sct * const trv_tbl)       /* I/O [sct] GTT (Group Traversal Table) */
{
  /* Purpose: Copy variable metadata from input netCDF file to output netCDF file
     Routine was based on nco_cpy_var_dfn_lmt(), and differed trivially from it
     Routine truncates dimensions in variable definition in output file according to user-specified limits
     Routine copies variable-by-variable
     20130126: csz 
     Behavior until today required rec_dmn_nm even if not changing it
     As of today, rec_dmn_nm passed only when user-specified
     Otherwise, re-use old record dimension name
     20130222: csz
     Same routine is called with or without limits
     Routine works with GTT instead of plain names */

  const char fnc_nm[]="nco_cpy_var_dfn_trv()"; /* [sng] Function name */

  char *dmn_nm_fll_out=NULL;             /* [sng] Full name of dimension in output */
  char *grp_dmn_out_fll=NULL;            /* [sng] Group name of dimension in output */
  char *rec_dmn_nm=NULL;                 /* [sng] User-specified record dimension name */
  char *rec_dmn_nm_mlc=NULL;             /* [sng] Local copy of rec_dmn_nm_cst, which may be encoded */
  char dmn_nm[NC_MAX_NAME+1];            /* [sng] Dimension name  */
  char dmn_nm_grp[NC_MAX_NAME+1];        /* [sng] Dimension name for group */  
  char var_nm[NC_MAX_NAME+1];            /* [sng] Variable name (local copy of object name) */ 

  dmn_cmn_sct dmn_cmn[NC_MAX_VAR_DIMS];      /* [sct] Dimension information on output (for a variable) */

  dmn_trv_sct *dmn_trv;                  /* [sct] Unique dimension object */
  
  int *dmn_idx_in_out;                   /* [idx] Dimension correspondence, input->output (ncpdq) */
  int *dmn_in_id_var;                    /* [id] Dimension IDs array for input variable */
  int *dmn_out_id_grp;                   /* [id] Dimension IDs array in output group */ 
  int *dmn_out_id_tmp;                   /* [idx] Copy of dmn_out_id (ncpdq) */
  int *udm_out_id_grp; /* [enm] Unlimited dimension IDs */
  int dmn_id_out;                        /* [id] Dimension ID defined in outout group */  
  int dmn_out_id[NC_MAX_VAR_DIMS];           /* [id] Dimension IDs array for output variable */
  int fl_fmt;                            /* [enm] Output file format */
  int grp_dmn_out_id;                    /* [id] Group ID where dimension visible to specified group is defined */
  int grp_in_id;                         /* [id] Group ID */
  int grp_out_id;                        /* [id] Group ID */
  int idx_dmn_out; /* [idx] Index for output dimensions */
  int idx_udm_grp; /* [idx] Index for unlimited dimensions */
  int nbr_dmn_out_grp;                   /* [id] Number of dimensions in group */
  int nbr_dmn_var;                       /* [nbr] Number of dimensions for variable */
  int nbr_dmn_var_out;                   /* [nbr] Number of dimensions for variable on output ( can change for ncwa) */
  int nbr_udm_out_grp; /* [nbr] Number of unlimited dimensions */
  int nco_prg_id;                        /* [enm] Program ID */
  int rcd=NC_NOERR;                      /* [rcd] Return code */
  int rcd_lcl;                           /* [rcd] Return code */
  int rec_dmn_out_id;                    /* [id] Record dimension for output variable */
  int rec_id_out;                        /* [id] Dimension ID for ncecat "record" dimension */  
  int var_dim_id;                        /* [id] Variable dimension ID */   
  int var_in_id;                         /* [id] Variable ID */
  int var_out_id;                        /* [id] Variable ID */

  long dmn_cnt=NC_UNLIMITED;             /* [nbr] Hyperslabbed size of dimension */  
  long dmn_sz;                           /* [nbr] Size of dimension (on input)  */  
  long dmn_sz_grp;                       /* [sng] Dimension size for group  */  

  nc_type var_typ;                       /* [enm] netCDF type of input variable (usually same as output) */
  nc_type var_typ_out;                   /* [enm] netCDF type of output variable (usually same as input) */ 

  nco_bool CRR_DMN_IS_REC_IN_INPUT;      /* [flg] Current dimension of variable is record dimension of variable in input file/group */
  nco_bool DFN_CRR_DMN_AS_REC_IN_OUTPUT=False; /* [flg] Define current dimension as record dimension in output file */
  nco_bool FIX_ALL_REC_DMN=False;        /* [flg] Fix all record dimensions */
  nco_bool FIX_REC_DMN=False;            /* [flg] Fix record dimension (opposite of MK_REC_DMN) */
  nco_bool NEED_TO_DEFINE_DIM;           /* [flg] Dimension needs to be defined in *this* group */  
  nco_bool DEFINE_DIM[NC_MAX_DIMS];      /* [flg] Defined dimension (always True, except for ncwa)  */  

  rec_dmn_out_id=NCO_REC_DMN_UNDEFINED;

  for(int idx_dmn=0;idx_dmn<NC_MAX_DIMS;idx_dmn++) dmn_out_id[idx_dmn]=NCO_REC_DMN_UNDEFINED;

  /* File format needed for decision tree and to enable netCDF4 features like chunking */
  (void)nco_inq_format(nc_out_id,&fl_fmt);

  /* Local copy of object name */ 
  strcpy(var_nm,var_trv->nm);     

  /* Initialize output dimensions to input sizes */
  for(int idx_dmn=0;idx_dmn<var_trv->nbr_dmn;idx_dmn++){
    /* Dimensions exist */
    if(var_trv->var_dmn){
      dmn_trv=NULL; /* [sct] Unique dimension */
      crd_sct *crd=NULL; /* [sct] Coordinate dimension */
      dmn_cmn[idx_dmn].nm_fll=var_trv->var_dmn[idx_dmn].dmn_nm_fll;
      dmn_cmn[idx_dmn].id=nco_obj_typ_err;
      /* This dimension has a coordinate variable */
      if(var_trv->var_dmn[idx_dmn].is_crd_var){
        /* Get coordinate from table */
        crd=var_trv->var_dmn[idx_dmn].crd;
        dmn_cmn[idx_dmn].sz=crd->sz;
        dmn_cmn[idx_dmn].is_rec_dmn=crd->is_rec_dmn;
        dmn_cmn[idx_dmn].NON_HYP_DMN=crd->lmt_msa.NON_HYP_DMN;
        dmn_cmn[idx_dmn].dmn_cnt=crd->lmt_msa.dmn_cnt;
        strcpy(dmn_cmn[idx_dmn].nm,crd->nm);
      }else{
        /* Get unique dimension from table */
        dmn_trv=var_trv->var_dmn[idx_dmn].ncd;
        dmn_cmn[idx_dmn].sz=dmn_trv->sz;
        dmn_cmn[idx_dmn].is_rec_dmn=dmn_trv->is_rec_dmn;
        dmn_cmn[idx_dmn].NON_HYP_DMN=dmn_trv->lmt_msa.NON_HYP_DMN;
        dmn_cmn[idx_dmn].dmn_cnt=dmn_trv->lmt_msa.dmn_cnt;
        strcpy(dmn_cmn[idx_dmn].nm,dmn_trv->nm);
      } /* This dimension has a coordinate variable */
    } /* Dimensions exist */
  } /* Loop over dimensions */

  /* Recall:
     1. Dimensions must be defined before variables
     2. Variables must be defined before attributes */

  /* Get output group ID */
  (void)nco_inq_grp_full_ncid(nc_out_id,grp_out_fll,&grp_out_id);
  /* Is requested variable already in output file? 
     This situation legally occurs with appends, i.e., when -A = FORCE_APPEND is used */
  rcd=nco_inq_varid_flg(grp_out_id,var_nm,&var_out_id);
  if(rcd == NC_NOERR){
    /* 20141201: Due diligence to warn user when output type does not match input type
       Again, this can happen when appending, e.g., from lower-to-higher precision */
    (void)nco_inq_vartype(grp_out_id,var_out_id,&var_typ_out);
    if(var_typ_out != var_trv->var_typ) (void)fprintf(stdout,"%s: WARNING %s reports variable \"%s\" output type = %s does not equal input type = %s. This is legal yet usually ill-advised when appending variables (i.e., with -A). Writing values into slots created for a different type is begging for trouble (e.g., data corruption, truncation, gingivitis).\n",nco_prg_nm_get(),fnc_nm,var_nm,nco_typ_sng(var_typ_out),nco_typ_sng(var_trv->var_typ));
    /* 20160930: Potential bug? Function returns var_out_id without other outputs (var_trv_dmn_cmn_out,nbr_dmn_cmn_out)
       Inexplicable errors in Append mode, when variable is already defined in output file, could be due to early return here */
    return var_out_id;
  } /* endif */

  /* Get input group ID */
  (void)nco_inq_grp_full_ncid(nc_in_id,var_trv->grp_nm_fll,&grp_in_id);
  /* Is requested variable in input file? */
  rcd=nco_inq_varid_flg(grp_in_id,var_nm,&var_in_id);
  if(rcd != NC_NOERR) (void)fprintf(stdout,"%s: %s reports ERROR unable to find variable \"%s\"\n",nco_prg_nm_get(),fnc_nm,var_nm);

  /* Get type of variable and number of dimensions from input */
  (void)nco_inq_var(grp_in_id,var_in_id,(char *)NULL,&var_typ,&nbr_dmn_var,(int *)NULL,(int *)NULL);  

  /* Get Program ID */
  nco_prg_id=nco_prg_id_get(); 

  if(nco_prg_id == ncks) assert(var_typ == var_trv->var_typ);
  assert(nbr_dmn_var == var_trv->nbr_dmn);

  nbr_dmn_var_out=nbr_dmn_var;

  var_typ=var_trv->var_typ;
  nbr_dmn_var=var_trv->nbr_dmn;

  /* Alloc array */
  dmn_in_id_var=(int *)nco_malloc(nbr_dmn_var*sizeof(int));
  dmn_idx_in_out=(int *)nco_malloc(nbr_dmn_var*sizeof(int));                 
  dmn_out_id_tmp=(int *)nco_malloc(nbr_dmn_var*sizeof(int)); 

  /* Get dimension IDs for variable */
  (void)nco_inq_vardimid(grp_in_id,var_in_id,dmn_in_id_var);

  if(nco_dbg_lvl_get() == nco_dbg_old){
    (void)fprintf(stdout,"%s: DEBUG %s defining variable <%s> with dimensions: ",nco_prg_nm_get(),fnc_nm,var_trv->nm_fll);
    for(int idx_dmn=0;idx_dmn<var_trv->nbr_dmn;idx_dmn++) (void)fprintf(stdout,"#%d<%s> : ",var_trv->var_dmn[idx_dmn].dmn_id,var_trv->var_dmn[idx_dmn].dmn_nm);
    (void)fprintf(stdout,"\n");
  } /* endif */

  /* Does user want a record dimension to receive special handling? */
  if(rec_dmn_nm_cst){
    /* Create (and later free()) local copy to preserve const-ness of passed value
       For simplicity, work with canonical name rec_dmn_nm */
    rec_dmn_nm_mlc=strdup(rec_dmn_nm_cst);
    /* Parse rec_dmn_nm argument */
    if(!strcmp("fix_all",rec_dmn_nm_mlc)){
      FIX_ALL_REC_DMN=True;
      FIX_REC_DMN=True;
      rec_dmn_nm=rec_dmn_nm_mlc+4;
    }else if(!strncmp("fix_",rec_dmn_nm_mlc,(size_t)4)){
      FIX_REC_DMN=True; /* [flg] Fix record dimension */
      rec_dmn_nm=rec_dmn_nm_mlc+4;
    }else{
      FIX_REC_DMN=False; /* [flg] Fix record dimension */
      rec_dmn_nm=rec_dmn_nm_mlc;
    } /* strncmp() */    
  } /* !rec_dmn_nm_cst */

  /* If variable has re-defined record dimension. NB: this implies passing NULL as user-specified record dimension parameter */
  if(var_trv->rec_dmn_nm_out){
    /* Must be ncpdq */
    assert(nco_prg_id == ncpdq);
    rec_dmn_nm=(char *)strdup(var_trv->rec_dmn_nm_out);
  } /* endif */

  /* Is requested record dimension in this group? */
  if(rec_dmn_nm){

    if(nco_prg_id == ncks){
      if(!FIX_ALL_REC_DMN){
        int rec_dmn_id_dmy;
        rcd=nco_inq_dimid_flg(grp_in_id,rec_dmn_nm,&rec_dmn_id_dmy);
        if(rcd == NC_NOERR){
          /* Does variable contain requested record dimension? */
          for(int idx_dmn=0;idx_dmn<nbr_dmn_var;idx_dmn++){
            if(dmn_in_id_var[idx_dmn] == rec_dmn_id_dmy){
              if(nco_dbg_lvl_get() == nco_dbg_old) (void)fprintf(stderr,"%s: INFO %s reports variable %s contains user-specified record dimension %s\n",nco_prg_nm_get(),fnc_nm,var_nm,rec_dmn_nm);
              break;
            } /* endif */
          } /* end loop over idx_dmn */
        } /* endif rcd == NC_NOERR */
      } /* FIX_ALL_REC_DMN */

    }else if(nco_prg_id == ncecat){
      /* Is dimension already defined in output? If not, define it */
      rcd_lcl=nco_inq_dimid_flg(nc_out_id,rec_dmn_nm,&rec_id_out);
      if(rcd_lcl != NC_NOERR){
        if(nco_dbg_lvl_get() == nco_dbg_old) (void)fprintf(stdout,"%s: INFO %s defining ncecat dimension: <%s>\n",nco_prg_nm_get(),fnc_nm,rec_dmn_nm);
        (void)nco_def_dim(nc_out_id,rec_dmn_nm,NC_UNLIMITED,&rec_id_out);
      } /* !dimension */
    } /* !ncecat */
  } /* !rec_dmn_nm */

  /* Loop over each dimension in variable */
  for(int idx_dmn=0;idx_dmn<nbr_dmn_var;idx_dmn++){

    /* Always define all dimensions, except possibly for ncwa */
    DEFINE_DIM[idx_dmn]=True;

    /* Dimension needs to be defined in this group? Assume yes... */
    NEED_TO_DEFINE_DIM=True;   

    /* Initialize dimension ID to be obtained */
    dmn_id_out=nco_obj_typ_err;

    /* Dimension ID for variable, used to get dimension object in input list  */
    var_dim_id=dmn_in_id_var[idx_dmn];

    /* Get dimension name and size from ID in input group */
    (void)nco_inq_dim(grp_in_id,var_dim_id,dmn_nm,&dmn_sz);

    /* Get unique dimension object from unique dimension ID, in input list */
    dmn_trv=nco_dmn_trv_sct(var_dim_id,trv_tbl);

    /* Determine where to place new dimension in output file */
    if(nco_prg_id == ncge){
      grp_dmn_out_fll=(char *)strdup(grp_out_fll);
    }else{
      if(gpe) grp_dmn_out_fll=nco_gpe_evl(gpe,dmn_trv->grp_nm_fll); else grp_dmn_out_fll=(char *)strdup(dmn_trv->grp_nm_fll);
    } /* !ncge */

    /* Test existence of group and create if necessary */
    if(nco_inq_grp_full_ncid_flg(nc_out_id,grp_dmn_out_fll,&grp_dmn_out_id)) nco_def_grp_full(nc_out_id,grp_dmn_out_fll,&grp_dmn_out_id);

    /* Inquire if dimension defined in output using obtained group ID (return value not used in logic) */
    rcd_lcl=nco_inq_dimid_flg(grp_dmn_out_id,dmn_nm,&dmn_id_out);

    if(nco_dbg_lvl_get() == nco_dbg_old){
      if(rcd_lcl == NC_NOERR) (void)fprintf(stdout,"%s: DEBUG %s dimension is visible (by parents or group) #%d<%s>\n",nco_prg_nm_get(),fnc_nm,var_dim_id,dmn_trv->nm_fll); else (void)fprintf(stdout,"%s: DEBUG %s dimension is not visible (by parents or group) #%d<%s>\n",nco_prg_nm_get(),fnc_nm,var_dim_id,dmn_trv->nm_fll);
    } /* endif dbg */

    /* Check output group (only, not parent) dimensions */
    (void)nco_inq_dimids(grp_dmn_out_id,&nbr_dmn_out_grp,(int *)NULL,0);
    dmn_out_id_grp=(int *)nco_malloc(nbr_dmn_out_grp*sizeof(int));
    (void)nco_inq_dimids(grp_dmn_out_id,&nbr_dmn_out_grp,dmn_out_id_grp,0);

    /* Get unlimited dimension IDs for output group */
    (void)nco_inq_unlimdims(grp_dmn_out_id,&nbr_udm_out_grp,(int *)NULL);
    udm_out_id_grp=(int *)nco_malloc(nbr_udm_out_grp*sizeof(int));
    (void)nco_inq_unlimdims(grp_dmn_out_id,&nbr_udm_out_grp,udm_out_id_grp);

    /* Is dimension already defined? */
    for(int idx_dmn_grp=0;idx_dmn_grp<nbr_dmn_out_grp;idx_dmn_grp++){

      /* Get dimension name and size from ID */
      (void)nco_inq_dim(grp_dmn_out_id,dmn_out_id_grp[idx_dmn_grp],dmn_nm_grp,&dmn_sz_grp);

      /* Relative name for variable and group exists for this group...dimension is already defined */
      if(!strcmp(dmn_nm_grp,dmn_nm)){
        NEED_TO_DEFINE_DIM=False;
        dmn_id_out=dmn_out_id_grp[idx_dmn_grp];
        /* Assign defined ID to dimension ID array for variable */
        dmn_out_id[idx_dmn]=dmn_id_out;

	/* Type of dimension may be different in input file than output file (e.g., --fix_rec_dim already applied) */
	for(idx_udm_grp=0;idx_udm_grp<nbr_udm_out_grp;idx_udm_grp++)
	  if(dmn_id_out == udm_out_id_grp[idx_udm_grp]) break;
	if(idx_udm_grp < nbr_udm_out_grp) dmn_cmn[idx_dmn].is_rec_dmn=True; else dmn_cmn[idx_dmn].is_rec_dmn=False;

      } /* strcmp() */
    } /* end loop over dimensions */
    dmn_out_id_grp=(int *)nco_free(dmn_out_id_grp);
    udm_out_id_grp=(int *)nco_free(udm_out_id_grp);

    /* Define dimension in output file if necessary */
    if(NEED_TO_DEFINE_DIM){

      if(nco_dbg_lvl_get() == nco_dbg_old){
        (void)fprintf(stdout,"%s: DEBUG %s need to define dimension '%s' in ",nco_prg_nm_get(),fnc_nm,dmn_nm);
        (void)nco_prn_grp_nm_fll(grp_dmn_out_id);
        (void)fprintf(stdout,"\n");
      } /* endif dbg */

      /* Here begins a complex tree to decide a simple, binary output:
	 Will current input dimension be defined as an output record dimension or as a fixed dimension?
	 Decision tree outputs flag DFN_CRR_DMN_AS_REC_IN_OUTPUT that controls subsequent netCDF actions
	 Otherwise would repeat netCDF action code too many times */

      /* Is dimension unlimited in input file? Handy unique dimension has all this info */
      CRR_DMN_IS_REC_IN_INPUT=dmn_trv->is_rec_dmn;

      if(FIX_ALL_REC_DMN){
        DFN_CRR_DMN_AS_REC_IN_OUTPUT=False;
        if(CRR_DMN_IS_REC_IN_INPUT && nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stderr,"%s: INFO %s is defining all input record dimensions including this one, %s, as fixed dimensions in output file per user request\n",nco_prg_nm_get(),fnc_nm,dmn_nm);
      }else if(rec_dmn_nm){
        /* User requested (with --fix_rec_dmn or --mk_rec_dmn) to treat a certain dimension specially */
        /* ... and this dimension is that dimension, i.e., the user-specified dimension ... */
        if(!strcmp(dmn_nm,rec_dmn_nm)){
          /* ... then honor user's request to define it as a fixed or record dimension ... */
          if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stderr,"%s: INFO %s is defining dimension %s as %s dimension in output file per user request\n",nco_prg_nm_get(),fnc_nm,rec_dmn_nm,(FIX_REC_DMN) ? "fixed" : "record");
          if(FIX_REC_DMN) DFN_CRR_DMN_AS_REC_IN_OUTPUT=False; else DFN_CRR_DMN_AS_REC_IN_OUTPUT=True;
        }else{ /* strcmp() */
          if(FIX_REC_DMN){
            /* ... fix_rec_dmn case is straightforward: output dimension has same format as input dimension */
            if(CRR_DMN_IS_REC_IN_INPUT) DFN_CRR_DMN_AS_REC_IN_OUTPUT=True; else DFN_CRR_DMN_AS_REC_IN_OUTPUT=False;
          }else{ /* !FIX_REC_DMN */
            /* ... otherwise we are in the --mk_rec_dmn case where things get complicated ... 
	       This dimension can be a record dimension only if it would not conflict with the requested 
	       record dimension being defined a record dimension, and that depends on file format. Uggh.
	       1. netCDF3 API allows only one record-dimension so conflicts are possible
	       2. netCDF4 API permits any number of unlimited dimensions so conflicts are impossible */
            if(fl_fmt == NC_FORMAT_NETCDF4){
              /* ... no conflicts possible so define dimension in output same as in input ... */
              if(CRR_DMN_IS_REC_IN_INPUT) DFN_CRR_DMN_AS_REC_IN_OUTPUT=True; else DFN_CRR_DMN_AS_REC_IN_OUTPUT=False;
            }else{ /* !netCDF4 */
              /* ... output file adheres to netCDF3 API so there can be only one record dimension.
		 In other words, define all other dimensions as fixed, non-record dimensions, even
		 if they are a record dimension in the input file ... */
	      if(CRR_DMN_IS_REC_IN_INPUT && nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stderr,"%s: INFO %s is defining dimension %s as fixed (non-record) in output file even though it is a record dimension in the input file. This is necessary to satisfy user request that %s be the record dimension in the output file which adheres to the netCDF3 API where the record dimension, if any, must be a variable's first dimension.\n",nco_prg_nm_get(),fnc_nm,dmn_nm,rec_dmn_nm);
              DFN_CRR_DMN_AS_REC_IN_OUTPUT=False;
            } /* !netCDF4 */

            /* Impose special cases to limit production of excessive additional record dimensions
	       How might excessive additional record dimensions be produced?
	       ncpdq reorders try to preserve the "record" property of record variables
	       ncpdq tries to define as a record dimension whichever dimension ends up first in a record variable, and, in netCDF4 files, this becomes an additional record dimension unless the original record dimension is changed to a fixed dimension (as must be done in netCDF3 files).
	       ncecat (in record aggregate mode) defines a new leading record dimension
	       In netCDF4 files this becomes an additional record dimension unless the original record dimension is changed to a fixed dimension (as must be done in netCDF3 files).
	       Easier if ncpdq and ncecat do not increase number of record dimensions in a variable
	       So NCO defaults to prevent production of additional record dimensions by ncecat, ncpdq
	       User can override this with --mrd (multiple record dimension) switch */

            /* Undefine dimension as record if current dimension (e.g., name "time") is also record */
            if(nco_prg_id == ncecat || nco_prg_id == ncpdq)
              if(dmn_trv->is_rec_dmn && nco_mrd_cnv_get() == nco_mrd_restrict)
                DFN_CRR_DMN_AS_REC_IN_OUTPUT=False;

          } /* !FIX_REC_DMN */
        } /* strcmp() */

      }else{ /* !rec_dmn_nm */
        /* ... no user-specified record dimension so define dimension in output same as in input ... */
        if(CRR_DMN_IS_REC_IN_INPUT) DFN_CRR_DMN_AS_REC_IN_OUTPUT=True; else DFN_CRR_DMN_AS_REC_IN_OUTPUT=False;
      } /* !rec_dmn_nm && !FIX_ALL_REC_DMN */ 

      /* At long last ... */

      /* Define current index dimension size */

      /* If current dimension is to be defined as record dimension in output file */
      if(DFN_CRR_DMN_AS_REC_IN_OUTPUT){  
        dmn_cnt=NC_UNLIMITED;
        long cnt;
        if(var_trv->var_dmn[idx_dmn].is_crd_var) cnt=var_trv->var_dmn[idx_dmn].crd->lmt_msa.dmn_cnt; else cnt=var_trv->var_dmn[idx_dmn].ncd->lmt_msa.dmn_cnt;
        (void)nco_dmn_set_msa(var_dim_id,cnt,trv_tbl);  
        /* Update output dimension info */
        dmn_cmn[idx_dmn].is_rec_dmn=True;
      }else{ /* Get size from GTT */
        /* Update output dimension info */
        dmn_cmn[idx_dmn].is_rec_dmn=False;
        if(var_trv->var_dmn[idx_dmn].is_crd_var) dmn_cnt=var_trv->var_dmn[idx_dmn].crd->lmt_msa.dmn_cnt; else dmn_cnt=var_trv->var_dmn[idx_dmn].ncd->lmt_msa.dmn_cnt;
        /* 20140108: Converting variables with multiple record dimensions and empty input to all fixed dimensions and netCDF3 output
	   Empty input means size is zero so dmn_cnt looks like NC_UNLIMITED, i.e., a record dimension
	   In other words, there is no good way to "fix" a record dimension for a variable with no records/data
	   Workaround is to arbitrarily assign such dimensions a size of 1 so they are fixed output, and still have no data
	   This ugly hack seems to work and is only invoked in this extreme corner case so may be OK */
        if(dmn_cnt == 0) dmn_cnt=1L;
        /* Update GTT dimension */
        (void)nco_dmn_set_msa(var_dim_id,dmn_cnt,trv_tbl);  
      } /* !DFN_CRR_DMN_AS_REC_IN_OUTPUT */

      /* ncwa */
      if(nco_prg_id == ncwa){
        nco_bool found_dim=False;
        /* Degenerate dimensions */
        for(int idx_dmn_dgn=0;idx_dmn_dgn<trv_tbl->nbr_dmn_dgn;idx_dmn_dgn++){
          /* Compare ID */
          if(trv_tbl->dmn_dgn[idx_dmn_dgn].id == var_dim_id){
            found_dim=True;
            dmn_cnt=trv_tbl->dmn_dgn[idx_dmn_dgn].cnt;
            /* If dimension is record keep it that way */
            if(dmn_trv->is_rec_dmn) dmn_cnt=NC_UNLIMITED;
            break;
          } /* Compare ID */
        } /* !Degenerate */
        if(var_trv->var_dmn[idx_dmn].flg_rdd){
          found_dim=True;
          /* If dimension was to be record keep it that way, otherwise define degenerate size of 1 */
          if(DFN_CRR_DMN_AS_REC_IN_OUTPUT) dmn_cnt=NC_UNLIMITED; else dmn_cnt=1L;
        } /* endif */
        if(!found_dim){
          DEFINE_DIM[idx_dmn]=False;
          nbr_dmn_var_out--;    
        } /* found_dim */
      } /* !ncwa */

      /* Always define, except maybe for ncwa */
      if(DEFINE_DIM[idx_dmn]){
        /* Define dimension and obtain dimension ID */
        (void)nco_def_dim(grp_dmn_out_id,dmn_nm,dmn_cnt,&dmn_id_out);
	if(dmn_cnt == NC_UNLIMITED) rec_dmn_out_id=dmn_id_out; else rec_dmn_out_id=NCO_REC_DMN_UNDEFINED;

        if(nco_dbg_lvl_get() == nco_dbg_dev) (void)fprintf(stdout,"%s: DEBUG %s Defined dimension %s/%s ID %d\n",nco_prg_nm_get(),fnc_nm,grp_dmn_out_fll,dmn_nm,dmn_id_out);

        /* Assign defined ID to dimension ID array for the variable */
        dmn_out_id[idx_dmn]=dmn_id_out; 
      } /* !DEFINE_DIM */

    } /* !NEED_TO_DEFINE_DIM */

    /* pvn 20140824 Always redefine output dimension array; repeat logic above regarding value of "dmn_cnt" for both non ncwa and ncwa cases */
    if(nco_prg_id != ncwa){
      if(var_trv->var_dmn[idx_dmn].is_crd_var) dmn_cnt=var_trv->var_dmn[idx_dmn].crd->lmt_msa.dmn_cnt; else dmn_cnt=var_trv->var_dmn[idx_dmn].ncd->lmt_msa.dmn_cnt;
    }else{
      /* Degenerate dimensions */
      for(int idx_dmn_dgn=0;idx_dmn_dgn<trv_tbl->nbr_dmn_dgn;idx_dmn_dgn++){
        /* Compare ID */
        if(trv_tbl->dmn_dgn[idx_dmn_dgn].id == var_dim_id){
          dmn_cnt=trv_tbl->dmn_dgn[idx_dmn_dgn].cnt;
          /* If dimension is record keep it that way */
          if(dmn_trv->is_rec_dmn) dmn_cnt=NC_UNLIMITED;
          break;
        } /* Compare ID */
      } /* !Degenerate */
      if(var_trv->var_dmn[idx_dmn].flg_rdd){
        /* If dimension was to be record keep it that way, otherwise define degenerate size of 1 */
        if(DFN_CRR_DMN_AS_REC_IN_OUTPUT) dmn_cnt=NC_UNLIMITED; else dmn_cnt=1L;
      } /* endif */
    } /* !ncwa */

    /* Construct full dimension name using full group name */
    dmn_nm_fll_out=(char *)nco_malloc(strlen(grp_dmn_out_fll)+strlen(dmn_nm)+2L);
    strcpy(dmn_nm_fll_out,grp_dmn_out_fll);
    if(strcmp(grp_dmn_out_fll,"/")) strcat(dmn_nm_fll_out,"/");
    strcat(dmn_nm_fll_out,dmn_nm);

    /* Redefine output dimension array for this dimension */
    (void)nco_dfn_dmn(dmn_nm_fll_out,dmn_cnt,dmn_id_out,dmn_cmn,var_trv->nbr_dmn);

    /* Die informatively if record dimension is not first dimension in netCDF3 output */
    /* 20160122: This would be a helpful error message yet it currently triggers five false-positive ncpdq failures
       Suspect incorrect conditions tested, if-clause entered, and program exits and causes regression test failures */
    if(False && idx_dmn > 0 && dmn_out_id[idx_dmn] == rec_dmn_out_id && DEFINE_DIM[idx_dmn] && fl_fmt != NC_FORMAT_NETCDF4){
      (void)fprintf(stdout,"%s: ERROR User defined output record dimension to be \"%s\". Yet in the variable \"%s\" this is dimension number %d. The output file adheres to the netCDF3 API which only supports the record dimension as the first (i.e., least rapidly varying) dimension. Consider using ncpdq to permute the location of the record dimension in the output file.\n",nco_prg_nm_get(),rec_dmn_nm,var_nm,idx_dmn+1);  
      nco_exit(EXIT_FAILURE);
    } /* end if err */

    /* Memory management after defining current output dimension */
    if(grp_dmn_out_fll) grp_dmn_out_fll=(char *)nco_free(grp_dmn_out_fll); 
    if(dmn_nm_fll_out) dmn_nm_fll_out=(char *)nco_free(dmn_nm_fll_out); 

  } /* End of the very important dimension loop */

  /* If variable needs dimension re-ordering */
  if(var_trv->flg_rdr){
    /* Must be ncpdq */
    assert(nco_prg_id == ncpdq);
    for(int idx_dmn=0;idx_dmn<nbr_dmn_var;idx_dmn++)
      dmn_idx_in_out[var_trv->dmn_idx_out_in[idx_dmn]]=idx_dmn;

    for(int idx_dmn=0;idx_dmn<nbr_dmn_var;idx_dmn++)
      dmn_out_id_tmp[idx_dmn]=dmn_out_id[idx_dmn];

    for(int idx_dmn=0;idx_dmn<nbr_dmn_var;idx_dmn++)
      dmn_out_id[dmn_idx_in_out[idx_dmn]]=dmn_out_id_tmp[idx_dmn];

    if(nco_dbg_lvl_get() >= nco_dbg_dev){
      (void)fprintf(stdout, "%s: DEBUG %s dimensions for %s:\n",nco_prg_nm_get(),fnc_nm,var_trv->nm_fll);
      for (int idx_dmn=0; idx_dmn<nbr_dmn_var;idx_dmn++)
        (void)fprintf(stdout, "%s %d\n",nco_get_dmn_nm_fll(dmn_out_id[idx_dmn],dmn_cmn,nbr_dmn_var),dmn_cmn[idx_dmn].id);
    } /* !dbg */
  } /* !var_trv->rdr */

  if(nco_prg_id == ncecat && rec_dmn_nm && var_trv->enm_prc_typ == prc_typ){ 
    /* Insert extra "record" dimension in dimension array if...  
       ...is ncecat and
       ...user requested (with --fix_rec_dmn or --mk_rec_dmn) to treat a certain dimension specially and
       ...variable is processing type */

    /* Temporary store for old IDs */
    int dmn_tmp_id[NC_MAX_VAR_DIMS];
    for(int idx_dmn=0;idx_dmn<nbr_dmn_var;idx_dmn++) dmn_tmp_id[idx_dmn]=dmn_out_id[idx_dmn];

    /* Increment number of dimensions for this variable */
    nbr_dmn_var++;
    nbr_dmn_var_out++;

    /* Insert previously obtained record dimension ID at start */
    dmn_out_id[0]=rec_id_out;

    /* Loop dimensions */
    for(int idx_dmn=0;idx_dmn<nbr_dmn_var;idx_dmn++) dmn_out_id[idx_dmn+1]=dmn_tmp_id[idx_dmn];
  } /* !ncecat */

  /* netCDF type in output variable (usually same as input) */ 
  var_typ_out=var_typ;

  /* Some operators change output netCDF variable type */
  if(nco_prg_id == ncflint || nco_prg_id == ncpdq){
  //if(nco_prg_id == ncflint || nco_prg_id == ncpdq || nco_prg_id == ncra || nco_prg_id == ncfe || nco_prg_id == ncge){

    /* If initialization value was changed, then set output type to new type */
    if(var_trv->var_typ_out != (nc_type)err_typ) var_typ_out=var_trv->var_typ_out; else var_typ_out=var_typ;

  }else if(nco_prg_id == ncra){

    /* If initialization value was changed, then set output type to new type */
    if(var_trv->var_typ_out != (nc_type)err_typ) var_typ_out=var_trv->var_typ_out;

  }else if(nco_prg_id != ncbo){
    int var_id;        /* [id] Variable ID */

    var_sct *var_prc;  /* [sct] Variable to process */

    /* Get variable ID */
    (void)nco_inq_varid(grp_in_id,var_trv->nm,&var_id);

    /* Allocate variable structure and fill with metadata */
    var_prc=nco_var_fll_trv(grp_in_id,var_id,var_trv,trv_tbl);     

    /* Obtain netCDF type to define variable from NCO program ID */
    var_typ_out=nco_get_typ(var_prc);

    for(int idx_dmn=0;idx_dmn<var_prc->nbr_dim;idx_dmn++){
      var_prc->dim[idx_dmn]->xrf=(dmn_sct *)nco_dmn_free(var_prc->dim[idx_dmn]->xrf);
      var_prc->dim[idx_dmn]=(dmn_sct *)nco_dmn_free(var_prc->dim[idx_dmn]);   
    }
    var_prc=(var_sct *)nco_var_free(var_prc);

  } /* !(ncflint || ncpdq) */

  /* Special case for ncwa */
  if(nco_prg_id == ncwa){
    int dmn_ids_out[NC_MAX_VAR_DIMS];  /* [id] Dimension IDs array for output variable (ncwa can skip some dimensions, rearrange) */
    int idx_dmn_def=0;
    for(int idx_dmn=0;idx_dmn<nbr_dmn_var;idx_dmn++){
      if(DEFINE_DIM[idx_dmn]){
        dmn_ids_out[idx_dmn_def]=dmn_out_id[idx_dmn];
        idx_dmn_def++;
      } /* DEFINE_DIM[idx_dmn]) */
    } /* end loop over dmn */

    /* Finally... define variable in output file */
    (void)nco_def_var(grp_out_id,var_nm,var_typ_out,nbr_dmn_var_out,dmn_ids_out,&var_out_id);

  }else{ /* !ncwa */

    /* Allow ncks to autoconvert any netCDF4-supported atomic type to netCDF3 or netCDF5-supported output type ... */
    if(nco_prg_id == ncks){
      if(fl_fmt == NC_FORMAT_CLASSIC || fl_fmt == NC_FORMAT_64BIT_OFFSET || fl_fmt == NC_FORMAT_NETCDF4_CLASSIC) var_typ_out=nco_typ_nc4_nc3(var_typ_out); else if(fl_fmt == NC_FORMAT_64BIT_DATA) var_typ_out=nco_typ_nc4_nc5(var_typ_out);
    } /* !ncks */
    
    /* fxm TODO nco1106 too complicated--need phony dimensions for NC_CHAR array length */
    /* if(var_typ_out == NC_STRING) nbr_dmn_var_out++; */ 

    (void)nco_def_var(grp_out_id,var_nm,var_typ_out,nbr_dmn_var_out,dmn_out_id,&var_out_id);

  } /* !ncwa */

  if(nco_dbg_lvl_get() == nco_dbg_dev){
    (void)fprintf(stdout,"%s: DEBUG %s Defined variable %s/%s: ",nco_prg_nm_get(),fnc_nm,grp_out_fll,var_nm);
    for(int idx_dmn=0;idx_dmn<nbr_dmn_var;idx_dmn++) (void)fprintf(stdout,"name,ID,sz = %s,%d,%ld : ",dmn_cmn[idx_dmn].nm_fll,dmn_out_id[idx_dmn],dmn_cmn[idx_dmn].sz);
    (void)fprintf(stdout,"\n");
  } /* endif dbg */ 

  /* If output dimensions array exists */
  if(dmn_cmn_out != NULL && nco_prg_id == ncks){

    for(int idx_dmn=0;idx_dmn<nbr_dmn_var;idx_dmn++){

      /* Is output dimension already in output file? Match by dimension ID since dimension IDs are unique */
      for(idx_dmn_out=0;idx_dmn_out<*nbr_dmn_cmn_out;idx_dmn_out++)
        if((*dmn_cmn_out)[idx_dmn_out].id == dmn_cmn[idx_dmn].id) break;

      /* If this dimension is not yet in output array */
      if(idx_dmn_out == *nbr_dmn_cmn_out){
        int nbr_dmn_out_tmp=*nbr_dmn_cmn_out;

        /* Add one more element to array */
        (*dmn_cmn_out)=(dmn_cmn_sct *)nco_realloc((*dmn_cmn_out),(nbr_dmn_out_tmp+1)*sizeof(dmn_cmn_sct));
        (*dmn_cmn_out)[nbr_dmn_out_tmp].id=dmn_cmn[idx_dmn].id;
        (*dmn_cmn_out)[nbr_dmn_out_tmp].nm_fll=(char *)strdup(dmn_cmn[idx_dmn].nm_fll);
        strcpy((*dmn_cmn_out)[nbr_dmn_out_tmp].nm,dmn_cmn[idx_dmn].nm);
        (*dmn_cmn_out)[nbr_dmn_out_tmp].dmn_cnt=dmn_cmn[idx_dmn].dmn_cnt;
        (*dmn_cmn_out)[nbr_dmn_out_tmp].is_rec_dmn=dmn_cmn[idx_dmn].is_rec_dmn;
        (*dmn_cmn_out)[nbr_dmn_out_tmp].NON_HYP_DMN=dmn_cmn[idx_dmn].NON_HYP_DMN;
        (*dmn_cmn_out)[nbr_dmn_out_tmp].sz=dmn_cmn[idx_dmn].sz;
        nbr_dmn_out_tmp++;
        *nbr_dmn_cmn_out=nbr_dmn_out_tmp;

        if(nco_dbg_lvl_get() == nco_dbg_old) (void)fprintf(stdout,"%s: DEBUG %s Inserted dimension #%d to output list\n",nco_prg_nm_get(),fnc_nm,dmn_cmn[idx_dmn].id);

      } /* endif */
    } /* end loop over dimensions */
  } /* endif dmn_cmn_out */

  /* Duplicate netCDF4 settings when possible */
  if(fl_fmt == NC_FORMAT_NETCDF4 || fl_fmt == NC_FORMAT_NETCDF4_CLASSIC){

    /* Deflation */
    if(nbr_dmn_var > 0){
      int deflate; /* [flg] Turn-on deflate filter */
      int dfl_lvl_in; /* [enm] Deflate level [0..9] */
      int shuffle; /* [flg] Turn-on shuffle filter */
      rcd=nco_inq_var_deflate(grp_in_id,var_in_id,&shuffle,&deflate,&dfl_lvl_in);
      /* Copy original deflation settings */
      if(deflate || shuffle) (void)nco_def_var_deflate(grp_out_id,var_out_id,shuffle,deflate,dfl_lvl_in);
      /* Overwrite HDF Lempel-Ziv compression level, if requested */
      if(dfl_lvl == 0) deflate=(int)False; else deflate=(int)True;
      /* Turn-off shuffle when uncompressing otherwise chunking requests may fail */
      if(dfl_lvl == 0) shuffle=NC_NOSHUFFLE;
      /* Shuffle never, to my knowledge, increases filesize, so shuffle by default when manually deflating */
      if(dfl_lvl >= 0) shuffle=NC_SHUFFLE;
      if(dfl_lvl >= 0) (void)nco_def_var_deflate(grp_out_id,var_out_id,shuffle,deflate,dfl_lvl);
    } /* endif */

    /* 20141013 Previously called chunking only when user selected a chunking switch
       Now always call chunking because default is to preserve input chunking 
       This requires explicitly overriding netCDF defaults */

    /* Define extra dimension on output (e.g., ncecat adds "record" dimension) */
    if(nco_prg_id == ncecat && rec_dmn_nm && var_trv->enm_prc_typ == prc_typ){ 
      /* Temporary store for old dimensions */
      dmn_cmn_sct dmn_cmn_tmp[NC_MAX_VAR_DIMS];
      for(int idx_dmn=0;idx_dmn<nbr_dmn_var_out;idx_dmn++) dmn_cmn_tmp[idx_dmn]=dmn_cmn[idx_dmn];
      /* Define record dimension made for ncecat */
      dmn_cmn[0].sz=NC_UNLIMITED;
      dmn_cmn[0].is_rec_dmn=True;
      dmn_cmn[0].NON_HYP_DMN=True;
      dmn_cmn[0].dmn_cnt=NC_UNLIMITED;
      strcpy(dmn_cmn[0].nm,rec_dmn_nm);
      /* Define full name */
      dmn_cmn[0].nm_fll=nco_bld_nm_fll(var_trv->grp_nm_fll,rec_dmn_nm);
      /* Make room for inserted dimension at 0 index */
      for(int idx_dmn=0;idx_dmn<nbr_dmn_var_out;idx_dmn++) dmn_cmn[idx_dmn+1]=dmn_cmn_tmp[idx_dmn];
    } /* !ncecat */
    
    /* Special case for ncwa */
    if(nco_prg_id == ncwa){
      int idx_dmn_def=0;
      /* Loop over input dimensions */
      for(int idx_dmn=0;idx_dmn<var_trv->nbr_dmn;idx_dmn++){
	if(DEFINE_DIM[idx_dmn]){
	  /* Redefine the array */
	  dmn_cmn[idx_dmn_def]=dmn_cmn[idx_dmn];
	  idx_dmn_def++;
	} /* DEFINE_DIM[idx_dmn]) */
      } /* Loop dimensions */
    } /* !ncwa */
    
    /* Special case for ncpdq */
    if(nco_prg_id == ncpdq && nbr_dmn_var > 1){
      int var_id_out; /* [id] Variable ID */
      int var_dmn_nbr; /* [nbr] Number of dimensions */
      int var_dimid[NC_MAX_VAR_DIMS]; /* [lst] Dimension IDs */
      (void)nco_inq_varid(grp_out_id,var_trv->nm,&var_id_out);
      (void)nco_inq_var(grp_out_id,var_id_out,var_nm,&var_typ,&var_dmn_nbr,var_dimid,NULL);
      /* Sanity check */
      assert(nbr_dmn_var == var_trv->nbr_dmn);
      assert(nbr_dmn_var == var_dmn_nbr);
      for(int idx_dmn=0;idx_dmn<nbr_dmn_var;idx_dmn++){
	(void)nco_inq_dim(grp_out_id,var_dimid[idx_dmn],dmn_nm,&dmn_sz);
	/* If output dimension name differs from input there was a swap, so swap array for these two names */
	if(strcmp(dmn_nm,dmn_cmn[idx_dmn].nm)) (void)nco_dmn_swap(dmn_nm,dmn_cmn[idx_dmn].nm,dmn_cmn,nbr_dmn_var);
      } /* idx_dmn */
    } /* !ncpdq */
    
    /* Set chunksize parameters */
    (void)nco_cnk_sz_set_trv(grp_in_id,grp_out_id,cnk,var_trv->nm,dmn_cmn);
    
    if(nco_prg_id == ncecat && rec_dmn_nm && var_trv->enm_prc_typ == prc_typ) dmn_cmn[0].nm_fll=(char *)nco_free(dmn_cmn[0].nm_fll);
    
  } /* !NC_FORMAT_NETCDF4 */ 
  
  /* Free locally allocated space */
  if(rec_dmn_nm_mlc) rec_dmn_nm_mlc=(char *)nco_free(rec_dmn_nm_mlc);
  
  dmn_in_id_var=(int *)nco_free(dmn_in_id_var);
  dmn_idx_in_out=(int *)nco_free(dmn_idx_in_out);                 
  dmn_out_id_tmp=(int *)nco_free(dmn_out_id_tmp); 

  return var_out_id;
} /* !nco_cpy_var_dfn_trv() */

void
nco_dmn_swap                           /* [fnc] Swap dimensions */
(const char * const dmn_nm_1,          /* I [sng] Name of dimension 1 */
 const char * const dmn_nm_2,          /* I [sng] Name of dimension 2 */
 dmn_cmn_sct *dmn_cmn,                 /* I/O [sct] Dimension structure array */
 const int nbr_dmn)                    /* I [nbr] Number of dimensions (size of above array) */
{
  int dmn_nm_1_idx=int_CEWI;
  int dmn_nm_2_idx=int_CEWI;

  dmn_cmn_sct dmn_cmn_tmp;

  for(int idx_dmn=0;idx_dmn<nbr_dmn;idx_dmn++)
    if(!strcmp(dmn_nm_1,dmn_cmn[idx_dmn].nm)) dmn_nm_1_idx=idx_dmn;

  for(int idx_dmn=0;idx_dmn<nbr_dmn;idx_dmn++)
    if(!strcmp(dmn_nm_2,dmn_cmn[idx_dmn].nm)) dmn_nm_2_idx=idx_dmn;

  /* Swap */
  dmn_cmn_tmp=dmn_cmn[dmn_nm_1_idx];
  dmn_cmn[dmn_nm_1_idx]=dmn_cmn[dmn_nm_2_idx];
  dmn_cmn[dmn_nm_2_idx]=dmn_cmn_tmp;
} /* nco_dmn_swap */

void
nco_dfn_dmn                            /* [fnc] Define dimension size and ID in array */
(const char * const dmn_nm_fll_out,    /* I [sng] Full name of dimension in output */
 const long dmn_sz,                    /* I [nbr] Size of dimension */
 const int dmn_id_out,                 /* I [id] ID of dimension in output */
 dmn_cmn_sct *dmn_cmn,                 /* I/O [sct] Dimension structure array */
 const int nbr_dmn)                    /* I [nbr] Number of dimensions (size of above array) */
{
  /* Loop dimensions */
  for(int idx_dmn=0;idx_dmn<nbr_dmn;idx_dmn++){
    /* Find dimension */
    if(!strcmp(dmn_nm_fll_out,dmn_cmn[idx_dmn].nm_fll)){
      dmn_cmn[idx_dmn].sz=dmn_sz;
      dmn_cmn[idx_dmn].id=dmn_id_out;
      return;
    } /* Find dimension */
  } /* Loop dimensions */

} /* nco_dmn_dfn */

const char *
nco_get_dmn_nm_fll                     /* [fnc] Return name corresponding to input dimension ID (debug) */
(const int dmn_id,                     /* I [id] ID of dimension */
 const dmn_cmn_sct * const dmn_cmn,    /* I [sct] Dimension structure array */
 const int nbr_dmn)                    /* I [nbr] Number of dimensions (size of above array) */
{
  /* Loop dimensions */
  for(int idx_dmn=0;idx_dmn<nbr_dmn;idx_dmn++)
    if(dmn_id == dmn_cmn[idx_dmn].id) return dmn_cmn[idx_dmn].nm_fll;

  return "ERROR";
} /* nco_get_dmn_nm_fll() */

void
nco_var_dmn_rdr_mtd_trv /* [fnc] Set new dimensionality in metadata of each re-ordered variable */
(trv_tbl_sct * const trv_tbl,         /* I/O [sct] GTT (Group Traversal Table) */
 const int nbr_var_prc,               /* I [nbr] Number of processed variables */
 var_sct **var_prc,                   /* I/O [sct] Processed variables */
 var_sct **var_prc_out,               /* I/O [sct] Processed variables */
 const int nbr_var_fix,               /* I [nbr] Number of processed variables */
 var_sct **var_fix,                   /* I/O [sct] Fixed variables */
 dmn_sct **dmn_rdr,                   /* I [sct] Dimension structures to be re-ordered */
 const int dmn_rdr_nbr,               /* I [nbr] Number of dimension to re-order */
 const nco_bool *dmn_rvr_rdr)         /* I [flg] Reverse dimension */
{
  /* Purpose: Determine and set new dimensionality in metadata of each re-ordered variable
     Based on nco_var_dmn_rdr_mtd()
     NB: first record dimension for target variable is used
     Test case: ncpdq -O -a lev,time -v two_dmn_rec_var in.nc out.nc
     Mark lev as record and un-mark time as record (by setting record name to lev) */

  char *rec_dmn_nm_out_crr; /* [sng] Name of record dimension, if any, required by re-order */
  char *rec_dmn_nm_in; /* [sng] Record dimension name, original */
  char *rec_dmn_nm_out; /* [sng] Record dimension name, re-ordered */
  int nco_prg_id; /* [enm] Program ID */
  nco_bool REDEFINED_RECORD_DIMENSION; /* [flg] Re-defined record dimension */
  nm_lst_sct *rec_dmn_nm; /* [sct] Record dimension names array */

  /* Get Program ID */
  nco_prg_id=nco_prg_id_get(); 
  assert(nco_prg_id == ncpdq);
  CEWI_unused(nco_prg_id);
  
  /* Initialize for this variable */
  REDEFINED_RECORD_DIMENSION=False;

  /* Loop processed variables */
  for(int idx_var_prc=0;idx_var_prc<nbr_var_prc;idx_var_prc++){

    int *dmn_idx_out_in=NULL; /* [idx] Dimension correspondence, output->input CEWI */
    nco_bool *dmn_rvr_in=NULL; /* [flg] Reverse dimension */

    /* Obtain variable GTT *pointer using full variable name */
    trv_sct *var_trv=trv_tbl_var_nm_fll(var_prc[idx_var_prc]->nm_fll,trv_tbl);

    assert(var_trv->flg_xtr); 
    assert(var_trv->nbr_dmn == var_prc_out[idx_var_prc]->nbr_dim);

    /* Mark re-order flag */
    var_trv->flg_rdr=True;

    /* Initialize record names for this object */
    rec_dmn_nm=NULL;
    rec_dmn_nm_out_crr=NULL;
    rec_dmn_nm_in=NULL;
    rec_dmn_nm_out=NULL;

    /* Get array of record names for object */
    (void)nco_get_rec_dmn_nm(var_trv,trv_tbl,&rec_dmn_nm);                

    /* Use for record dimension name the first in array */
    if(rec_dmn_nm && rec_dmn_nm->lst){
      rec_dmn_nm_in=(char *)strdup(rec_dmn_nm->lst[0].nm);
      rec_dmn_nm_out=(char *)strdup(rec_dmn_nm->lst[0].nm);
    } /* !rec_dmn_nm->lst */

    dmn_idx_out_in=(int *)nco_malloc(var_prc[idx_var_prc]->nbr_dim*sizeof(int));
    dmn_rvr_in=(nco_bool *)nco_malloc(var_prc[idx_var_prc]->nbr_dim*sizeof(nco_bool));
    /* nco_var_dmn_rdr_mtd() does re-order heavy lifting */
    rec_dmn_nm_out_crr=nco_var_dmn_rdr_mtd(var_prc[idx_var_prc],var_prc_out[idx_var_prc],dmn_rdr,dmn_rdr_nbr,dmn_idx_out_in,dmn_rvr_rdr,dmn_rvr_in);

    /* Transfer dimension structures to be re-ordered into GTT */
    var_trv->dmn_idx_out_in=(int *)nco_malloc(var_trv->nbr_dmn*sizeof(int));
    var_trv->dmn_rvr_in=(nco_bool *)nco_malloc(var_trv->nbr_dmn*sizeof(nco_bool));
    for(int idx_dmn=0;idx_dmn<var_trv->nbr_dmn;idx_dmn++){
      var_trv->dmn_idx_out_in[idx_dmn]=dmn_idx_out_in[idx_dmn];
      var_trv->dmn_rvr_in[idx_dmn]=dmn_rvr_in[idx_dmn];
    } /* end loop over dimensions */

    /* If record dimension required by current variable re-order...
       ...and variable is multi-dimensional (one dimensional arrays cannot request record dimension changes)... */
    if(rec_dmn_nm_in && rec_dmn_nm_out_crr && var_prc_out[idx_var_prc]->nbr_dim > 1){
      /* ...differs from input and current output record dimension(s)... */
      if(strcmp(rec_dmn_nm_out_crr,rec_dmn_nm_in) && strcmp(rec_dmn_nm_out_crr,rec_dmn_nm_out)){
        /* ...and current output record dimension already differs from input record dimension... */
        if(REDEFINED_RECORD_DIMENSION){
          /* ...then requested re-order requires multiple record dimensions... */
          if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stdout,"%s: WARNING Re-order requests multiple record dimensions\n. Only first request will be honored (netCDF3 allows only one record dimension). Record dimensions involved [original,first change request (honored),latest change request (made by variable %s)]=[%s,%s,%s]\n",nco_prg_nm_get(),var_prc[idx_var_prc]->nm,rec_dmn_nm_in,rec_dmn_nm_out,rec_dmn_nm_out_crr);
          continue;
        }else{ /* !REDEFINED_RECORD_DIMENSION */
          /* ...otherwise, update output record dimension name... */
          rec_dmn_nm_out=rec_dmn_nm_out_crr;
          /* ...and set new dimensions... */
          var_prc_out[idx_var_prc]->dim[0]->is_rec_dmn=True;

          /* ...and set flag that record dimension has been re-defined... */
          REDEFINED_RECORD_DIMENSION=True;

          /* ...store the name of the record dimension on output... */
          var_trv->rec_dmn_nm_out=(char *)strdup(rec_dmn_nm_out);

        } /* !REDEFINED_RECORD_DIMENSION */
      } /* endif new and old record dimensions differ */
    } /* endif current variable is record variable */

    /* Memory management for record dimension names */
    if(rec_dmn_nm){
      for(int idx=0;idx<rec_dmn_nm->nbr;idx++) rec_dmn_nm->lst[idx].nm=(char *)nco_free(rec_dmn_nm->lst[idx].nm);
      rec_dmn_nm=(nm_lst_sct *)nco_free(rec_dmn_nm);
    } /* !rec_dmn_nm */

    /* Free current dimension correspondence */
    dmn_idx_out_in=(int *)nco_free(dmn_idx_out_in);
    dmn_rvr_in=(nco_bool *)nco_free(dmn_rvr_in);
    if(rec_dmn_nm_in)rec_dmn_nm_in=(char *)nco_free(rec_dmn_nm_in);
    if(rec_dmn_nm_out)rec_dmn_nm_out=(char *)nco_free(rec_dmn_nm_out);

  } /* Loop processed variables */

  /* Loop to deal with REDEFINED_RECORD_DIMENSION */

  /* Loop table */
  for(unsigned idx_var=0;idx_var<trv_tbl->nbr;idx_var++){
    trv_sct var_trv=trv_tbl->lst[idx_var];

    /* Changing record dimension may invalidate is_rec_var flag
       Updating is_rec_var flag to correct value, even if value is ignored,
       helps keep user appraised of unexpected dimension re-orders.
       is_rec_var may change both for "fixed" and "processed" variables
       When is_rec_var changes for processed variables, may also need to change
       ancillary information and to check for duplicate dimensions.
       Ancillary information (dmn_idx_out_in) is available only for var_prc!
       Hence must update is_rec_var flag for var_fix and var_prc separately */
    
    /*  Update is_rec_var flag for var_fix */
    for(int idx_var_fix=0;idx_var_fix<nbr_var_fix;idx_var_fix++){

      /* Match by full variable name  */
      if(strcmp(var_fix[idx_var_fix]->nm_fll,var_trv.nm_fll) == 0){

        /* ...get the name of the record dimension on output... stored to GTT in first loop above */
        rec_dmn_nm_out=trv_tbl->lst[idx_var].rec_dmn_nm_out;   

        int dmn_out_idx;
        /* Search all dimensions in variable for new record dimension */
        for(dmn_out_idx=0;dmn_out_idx<var_fix[idx_var_fix]->nbr_dim;dmn_out_idx++){
          if(rec_dmn_nm_out && !strcmp(var_fix[idx_var_fix]->dim[dmn_out_idx]->nm,rec_dmn_nm_out)){
            break;
          }
        }
        /* ...Will variable be record variable in output file?... */
        if(dmn_out_idx == var_fix[idx_var_fix]->nbr_dim){
          /* ...No. Variable will be non-record---does this change its status?... */
          if(nco_dbg_lvl_get() >= nco_dbg_var){
            if(var_fix[idx_var_fix]->is_rec_var) (void)fprintf(stdout,"%s: INFO Requested re-order will change variable %s from record to non-record variable\n",
              nco_prg_nm_get(),var_fix[idx_var_fix]->nm);
          }
          /* Assign record flag dictated by re-order */
          var_fix[idx_var_fix]->is_rec_var=False; 
        }else{ /* ...otherwise variable will be record variable... */
          /* ...Yes. Variable will be record... */
          /* ...Will becoming record variable change its status?... */
          if(!var_fix[idx_var_fix]->is_rec_var){
            if(nco_dbg_lvl_get() >= nco_dbg_var) (void)fprintf(stdout,"%s: INFO Requested re-order will change variable %s from non-record to record variable\n",
              nco_prg_nm_get(),var_fix[idx_var_fix]->nm);
            /* Change record flag to status dictated by re-order */
            var_fix[idx_var_fix]->is_rec_var=True;
          } /* endif status changing from non-record to record */
        } /* endif variable will be record variable */

      } /* Match by full variable name  */
    } /* end loop over var_fix */
  } /* Loop table */

  /* Loop table */
  for(unsigned idx_var=0;idx_var<trv_tbl->nbr;idx_var++){
    trv_sct var_trv=trv_tbl->lst[idx_var];

    /* Update is_rec_var flag for var_prc */
    for(int idx_var_prc=0;idx_var_prc<nbr_var_prc;idx_var_prc++){

      int *dmn_idx_out_in=NULL; /* [idx] Dimension correspondence, output->input CEWI */
      dmn_idx_out_in=(int *)nco_malloc(var_prc[idx_var_prc]->nbr_dim*sizeof(int));

      /* Match by full variable name  */
      if(strcmp(var_prc_out[idx_var_prc]->nm_fll,var_trv.nm_fll) == 0){

        /* ...get the name of the record dimension on output... stored to GTT in first loop above */
        rec_dmn_nm_out=trv_tbl->lst[idx_var].rec_dmn_nm_out;    

        /* Loop table, search for other variables that share the same dimension name */
        for(unsigned idx_var_mrk=0;idx_var_mrk<trv_tbl->nbr;idx_var_mrk++){

          /* Looking for this variable */
          trv_sct var_trv_mrk=trv_tbl->lst[idx_var_mrk];

          /* Avoid same variable ...
          ...look if there is a record name to find 
          ...and look for variables only
          ...and look for extracted variables only
          ...and look for variables with dimensions > 1 */
          if(rec_dmn_nm_out 
            && strcmp(var_trv.nm_fll,var_trv_mrk.nm_fll)
            && var_trv_mrk.nco_typ == nco_obj_typ_var 
            && var_trv_mrk.flg_xtr
            && var_trv_mrk.nbr_dmn > 1){

              nco_bool NEEDS_REORDER;   /* [flg] Variable needs re-ordering */
              int idx_var_prc_out;      /* [nbr] Index of variable that needs re-ordering */

              NEEDS_REORDER=False;

              /* Loop dimensions of to search variable  */
              for(int idx_dmn=0;idx_dmn<var_trv_mrk.nbr_dmn;idx_dmn++){

                /* Match name */
                if(!strcmp(var_trv_mrk.var_dmn[idx_dmn].dmn_nm,var_trv.rec_dmn_nm_out)){ 
                  NEEDS_REORDER=True;
                  break;
                } /*  Match name */
              }/* Loop dimensions of to search variable  */

              /* NEEDS_REORDER */
              if(NEEDS_REORDER){

                /* NB:
                 --- use found index of processed idx_var_prc_out
                 --- use search index of GTT idx_var_mrk */

                /* Find index of processed variables that corresponds to found GTT variable */
                nco_var_prc_idx_trv(var_trv_mrk.nm_fll,var_prc_out,nbr_var_prc,&idx_var_prc_out);        

                /* Transfer dimension structures to be re-ordered *from* GTT (opposite of above)  */
                for(int idx_dmn=0;idx_dmn<var_trv_mrk.nbr_dmn;idx_dmn++)
                  dmn_idx_out_in[idx_dmn]=var_trv_mrk.dmn_idx_out_in[idx_dmn];

                int dmn_out_idx;
                /* Search all dimensions in variable for new record dimension */
                for(dmn_out_idx=0;dmn_out_idx<var_prc_out[idx_var_prc_out]->nbr_dim;dmn_out_idx++)
                  if(!strcmp(var_prc_out[idx_var_prc_out]->dim[dmn_out_idx]->nm,rec_dmn_nm_out)) break;

                /* ...Will variable be record variable in output file?... */
                if(dmn_out_idx == var_prc_out[idx_var_prc_out]->nbr_dim){
                  /* ...No. Variable will be non-record---does this change its status?... */
                  if(nco_dbg_lvl_get() >= nco_dbg_var)
                    if(var_prc_out[idx_var_prc_out]->is_rec_var)
                      (void)fprintf(stdout, "%s: INFO Requested re-order will change variable %s from record to non-record variable\n", nco_prg_nm_get(), var_prc_out[idx_var_prc_out]->nm);
                  /* Assign record flag dictated by re-order */
                  var_prc_out[idx_var_prc_out]->is_rec_var=False; 
                }else{ /* ...otherwise variable will be record variable... */
                  /* ...Yes. Variable will be record... */
                  /* ...must ensure new record dimension is not duplicate dimension... */
                  if(var_prc_out[idx_var_prc_out]->has_dpl_dmn){
                    int dmn_dpl_idx;
                    for(dmn_dpl_idx=1;dmn_dpl_idx<var_prc_out[idx_var_prc_out]->nbr_dim;dmn_dpl_idx++){ /* NB: loop starts from 1 */
                      if(var_prc_out[idx_var_prc_out]->dmn_id[0] == var_prc_out[idx_var_prc_out]->dmn_id[dmn_dpl_idx]){
                        (void)fprintf(stdout,"%s: ERROR Requested re-order turns duplicate non-record dimension %s in variable %s into output record dimension. netCDF does not support duplicate record dimensions in a single variable.\n%s: HINT: Exclude variable %s from extraction list with \"-x -v %s\".\n",nco_prg_nm_get(),rec_dmn_nm_out,var_prc_out[idx_var_prc_out]->nm,nco_prg_nm_get(),var_prc_out[idx_var_prc_out]->nm,var_prc_out[idx_var_prc_out]->nm);
                        nco_exit(EXIT_FAILURE);
                      } /* endif err */
                    } /* end loop over dmn_out */
                  } /* endif has_dpl_dmn */
                  /* ...Will becoming record variable change its status?... */
                  if(!var_prc_out[idx_var_prc_out]->is_rec_var){
                    if(nco_dbg_lvl_get() >= nco_dbg_var) (void)fprintf(stdout,"%s: INFO Requested re-order will change variable %s from non-record to record variable\n",nco_prg_nm_get(),var_prc_out[idx_var_prc_out]->nm);
                    /* Change record flag to status dictated by re-order */
                    var_prc_out[idx_var_prc_out]->is_rec_var=True;
                    /* ...Swap dimension information for multi-dimensional variables... */
                    if(var_prc_out[idx_var_prc_out]->nbr_dim > 1){
                      /* Swap dimension information when turning multi-dimensional 
                      non-record variable into record variable. 
                      Single dimensional non-record variables that turn into 
                      record variables already have correct dimension information */
                      dmn_sct *dmn_swp; /* [sct] Dimension structure for swapping */
                      int dmn_idx_rec_in; /* [idx] Record dimension index in input variable */
                      int dmn_idx_rec_out; /* [idx] Record dimension index in output variable */
                      int dmn_idx_swp; /* [idx] Dimension index for swapping */
                      /* If necessary, swap new record dimension to first position */
                      /* Label indices with standard names */
                      dmn_idx_rec_in=dmn_out_idx;
                      dmn_idx_rec_out=0;
                      /* Swap indices in map */
                      dmn_idx_swp=dmn_idx_out_in[dmn_idx_rec_out];
                      dmn_idx_out_in[dmn_idx_rec_out]=dmn_idx_rec_in;
                      dmn_idx_out_in[dmn_idx_rec_in]=dmn_idx_swp;

                      /* Transfer dimension structures (re-ordered again) into GTT */
                      for(int idx_dmn=0;idx_dmn<var_trv_mrk.nbr_dmn;idx_dmn++)
                        trv_tbl->lst[ idx_var_mrk ].dmn_idx_out_in[idx_dmn]=dmn_idx_out_in[idx_dmn];

                      /* Swap dimensions in list */
                      dmn_swp=var_prc_out[idx_var_prc_out]->dim[dmn_idx_rec_out];
                      var_prc_out[idx_var_prc_out]->dim[dmn_idx_rec_out]=var_prc_out[idx_var_prc_out]->dim[dmn_idx_rec_in];
                      var_prc_out[idx_var_prc_out]->dim[dmn_idx_rec_in]=dmn_swp;
                      /* NB: Change dmn_id,cnt,srt,end,srd together to minimize chances of forgetting one */
                      /* Correct output variable structure copy of output record dimension information */
                      var_prc_out[idx_var_prc_out]->dmn_id[dmn_idx_rec_out]=var_prc_out[idx_var_prc_out]->dim[dmn_idx_rec_out]->id;
                      var_prc_out[idx_var_prc_out]->cnt[dmn_idx_rec_out]=var_prc_out[idx_var_prc_out]->dim[dmn_idx_rec_out]->cnt;
                      var_prc_out[idx_var_prc_out]->srt[dmn_idx_rec_out]=var_prc_out[idx_var_prc_out]->dim[dmn_idx_rec_out]->srt;
                      var_prc_out[idx_var_prc_out]->end[dmn_idx_rec_out]=var_prc_out[idx_var_prc_out]->dim[dmn_idx_rec_out]->end;
                      var_prc_out[idx_var_prc_out]->srd[dmn_idx_rec_out]=var_prc_out[idx_var_prc_out]->dim[dmn_idx_rec_out]->srd;
                      /* Correct output variable structure copy of input record dimension information */
                      var_prc_out[idx_var_prc_out]->dmn_id[dmn_idx_rec_in]=var_prc_out[idx_var_prc_out]->dim[dmn_idx_rec_in]->id;
                      var_prc_out[idx_var_prc_out]->cnt[dmn_idx_rec_in]=var_prc_out[idx_var_prc_out]->dim[dmn_idx_rec_in]->cnt;
                      var_prc_out[idx_var_prc_out]->srt[dmn_idx_rec_in]=var_prc_out[idx_var_prc_out]->dim[dmn_idx_rec_in]->srt;
                      var_prc_out[idx_var_prc_out]->end[dmn_idx_rec_in]=var_prc_out[idx_var_prc_out]->dim[dmn_idx_rec_in]->end;
                      var_prc_out[idx_var_prc_out]->srd[dmn_idx_rec_in]=var_prc_out[idx_var_prc_out]->dim[dmn_idx_rec_in]->srd;

                    } /* endif multi-dimensional */
                  } /* endif status changing from non-record to record */
                } /* endif variable will be record variable */
              } /* Loop table, search for other variables that share same dimension name */
          } /* NEEDS_REORDER */
        } /* ...look if there is a record name to find  */
      } /* Match by full variable name  */
      /* Free current dimension correspondence */
      dmn_idx_out_in=(int *)nco_free(dmn_idx_out_in);
    } /* end loop over var_prc */
  } /* Loop table */

  /* Final step: search for all redefined record dimension variables and mark other variables */

  if(nco_dbg_lvl_get() == nco_dbg_dev) trv_tbl_prn_dbg("nco_var_dmn_rdr_mtd_trv", trv_tbl);

  /* Loop table */
  for(unsigned idx_var=0;idx_var<trv_tbl->nbr;idx_var++){
    trv_sct var_trv=trv_tbl->lst[idx_var];

    /* Has re-defined record dimension */
    if(var_trv.rec_dmn_nm_out){

      rec_dmn_nm_out=trv_tbl->lst[idx_var].rec_dmn_nm_out;

      /* Loop table, search for other variables that share same dimension name */
      for(unsigned idx_var_mrk=0;idx_var_mrk<trv_tbl->nbr;idx_var_mrk++){

        /* Looking for this variable */
        trv_sct var_trv_mrk=trv_tbl->lst[idx_var_mrk];

        /* Avoid same variable ...
        ...and look for variables only
        ...and look for extracted variables only */
        if(strcmp(var_trv.nm_fll,var_trv_mrk.nm_fll) && var_trv_mrk.nco_typ == nco_obj_typ_var && var_trv_mrk.flg_xtr){

          /* Loop dimensions of to search variable  */
          for(int idx_dmn=0;idx_dmn<var_trv_mrk.nbr_dmn;idx_dmn++){

            dmn_trv_sct *dmn_trv;

            /* Store name of record dimension on output */
            if(!strcmp(var_trv_mrk.var_dmn[idx_dmn].dmn_nm, rec_dmn_nm_out)){
              trv_tbl->lst[idx_var_mrk].rec_dmn_nm_out=(char *)strdup(rec_dmn_nm_out);
            }

            /* Get unique dimension object from unique dimension ID, in input list */
            dmn_trv=nco_dmn_trv_sct(var_trv_mrk.var_dmn[idx_dmn].dmn_id,trv_tbl);

            /* Is record? */
            /* Store record dimension name on output */
            if(dmn_trv->is_rec_dmn){
              trv_tbl->lst[idx_var_mrk].rec_dmn_nm_out=(char *)strdup(rec_dmn_nm_out);
            }

          } /* Loop variable dimensions */
        } /* Avoid same */
      } /* Loop table */
    } /* Has re-defined record dimension */
  } /* Loop table */

  if(nco_dbg_lvl_get() == nco_dbg_dev) trv_tbl_prn_dbg("nco_var_dmn_rdr_mtd_trv", trv_tbl);

  return;

} /* nco_var_dmn_rdr_mtd_trv() */

nco_bool                              /* [flg] Name was found */
nco_var_prc_idx_trv                   /* [fnc] Find index of processed variable that matches full name */
(const char * const var_nm_fll,       /* I [nbr] Full name of variable */
 var_sct **var_prc_out,               /* I [sct] Processed variables */
 const int nbr_var_prc,               /* I [nbr] Number of processed variables */
 int * idx_var_prc_out)               /* O [nbr] Number of dimension to re-order */
{

  /* Loop processed variables  */
  for(int idx_var_prc=0;idx_var_prc<nbr_var_prc;idx_var_prc++){

    /* Match by full variable name  */
    if(strcmp(var_prc_out[idx_var_prc]->nm_fll,var_nm_fll) == 0){

      *idx_var_prc_out=idx_var_prc;

      return True;

    } /* Match by full variable name  */
  } /* Loop processed variables  */

  assert(0);
  return False;

} /* nco_var_prc_idx_trv() */

nco_bool                              /* O [flg] Re-define dimension ordering */
nco_rdf_dmn_trv                       /* [fnc] Re-define dimension ordering */
(trv_sct var_trv,                     /* I [sct] varible with record dimension name, re-ordered */
 const trv_tbl_sct * const trv_tbl,   /* I [sct] GTT (Group Traversal Table) */
 int * idx_var_mrk_out)               /* O [nbr] Index in GTT where name was found */
{

  if(var_trv.rec_dmn_nm_out == NULL) return False;

  /* Loop table, search for other variables that share same dimension name */
  for(unsigned idx_var_mrk=0;idx_var_mrk<trv_tbl->nbr;idx_var_mrk++){

    /* Looking for this variable */
    trv_sct var_trv_mrk=trv_tbl->lst[idx_var_mrk];

    /* Avoid same variable ...
    ...and look for variables only
    ...and look for extracted variables only
    ...and look for variables with dimensions > 1 */
    if(strcmp(var_trv.nm_fll,var_trv_mrk.nm_fll)
      && var_trv_mrk.nco_typ == nco_obj_typ_var 
      && var_trv_mrk.flg_xtr
      && var_trv_mrk.nbr_dmn > 1){

        /* Loop dimensions of to search variable  */
        for(int idx_dmn=0;idx_dmn<var_trv_mrk.nbr_dmn;idx_dmn++){

          /*  Match name */
          if(!strcmp(var_trv_mrk.var_dmn[idx_dmn].dmn_nm,var_trv.rec_dmn_nm_out)){ 

            *idx_var_mrk_out=idx_var_mrk;
            return True;

          } /*  Match name */
        } /* Loop variable dimensions */
    } /* Avoid same */
  } /* Loop table */

  return False;

} /* nco_had_rdf_dmn_trv() */

void
nco_var_dmn_rdr_val_trv               /* [fnc] Change dimension ordering of variable values */
(const var_sct * const var_in,        /* I [ptr] Variable with metadata and data in original order */
 var_sct * const var_out,             /* I/O [ptr] Variable whose data will be re-ordered */
 const trv_tbl_sct * const trv_tbl)   /* I [sct] GTT (Group Traversal Table) */
{
  /* Purpose: Re-order values in given variable according to supplied dimension map
  Description of re-ordering concepts is in nco_var_dmn_rdr_mtd()
  Description of actual re-ordering algorithm is in nco_var_dmn_rdr_val() */

  const char fnc_nm[]="nco_var_dmn_rdr_val_trv()"; /* [sng] Function name */

  nco_bool IDENTITY_REORDER=False; /* [flg] User requested identity re-ordering */

  char *val_in_cp;                 /* [ptr] Input data location as char pointer */
  char *val_out_cp;                /* [ptr] Output data location as char pointer */

  dmn_sct **dmn_out;               /* [sct] List of dimension structures in output order */

  int dmn_idx;                     /* [idx] Index over dimensions */
  int dmn_in_idx;                  /* [idx] Counting index for dmn_in */
  int dmn_in_nbr;                  /* [nbr] Number of dimensions in input variable */
  int dmn_in_nbr_m1;               /* [nbr] Number of dimensions in input variable, less one */
  int dmn_out_idx;                 /* [idx] Counting index for dmn_out */
  int dmn_out_nbr;                 /* [nbr] Number of dimensions in output variable */
  int typ_sz;                      /* [B] Size of data element in memory */

  long dmn_in_map[NC_MAX_VAR_DIMS];    /* [idx] Map for each dimension of input variable */
  long dmn_out_map[NC_MAX_VAR_DIMS];   /* [idx] Map for each dimension of output variable */
  long dmn_in_sbs[NC_MAX_VAR_DIMS];    /* [idx] Dimension subscripts into N-D input array */
  long var_in_lmn;                 /* [idx] Offset into 1-D input array */
  long var_out_lmn;                /* [idx] Offset into 1-D output array */
  long *var_in_cnt;                /* [nbr] Number of valid elements in this dimension (including effects of stride and wrapping) */
  long var_sz;                     /* [nbr] Number of elements (NOT bytes) in hyperslab (NOT full size of variable in input file!) */

  int *dmn_idx_out_in;             /* [idx] Dimension correspondence, output->input  (Stored in GTT ) */

  nco_bool *dmn_rvr_in;            /* [flg] Reverse dimension  (Stored in GTT ) */

  for(unsigned idx_var=0;idx_var<trv_tbl->nbr;idx_var++){
    trv_sct var_trv=trv_tbl->lst[idx_var];

    /* Match by full variable name  */
    if(!strcmp(var_out->nm_fll,var_trv.nm_fll)){

      assert(var_trv.nco_typ == nco_obj_typ_var);
      assert(var_trv.flg_xtr); 
      assert(var_trv.nbr_dmn==var_out->nbr_dim);

      /* Transfer dimension structures to be re-ordered *from* GTT */
      dmn_idx_out_in=(int *)nco_malloc(var_trv.nbr_dmn*sizeof(int));
      dmn_rvr_in=(nco_bool *)nco_malloc(var_trv.nbr_dmn*sizeof(nco_bool));

      for(int idx_dmn=0;idx_dmn<var_trv.nbr_dmn;idx_dmn++){
        dmn_idx_out_in[idx_dmn]=trv_tbl->lst[idx_var].dmn_idx_out_in[idx_dmn];
        dmn_rvr_in[idx_dmn]=trv_tbl->lst[idx_var].dmn_rvr_in[idx_dmn];
      } /* !idx_dmn */

      /* Initialize variables to reduce indirection */
      /* NB: Number of input and output dimensions are equal for pure re-orders
	 However, keep dimension numbers in separate variables to ease relax this rule in future */
      dmn_in_nbr=var_in->nbr_dim;
      dmn_out_nbr=var_out->nbr_dim;

      /* On entry to this section of code, we assume:
	 1. var_out metadata are re-ordered
	 2. var_out->val buffer has been allocated (calling routine must do this) */

      /* Get ready to re-order */
      /* dmn_id_out=var_out->dmn_id; */
      /* dmn_in=var_in->dim; */
      dmn_in_nbr_m1=dmn_in_nbr-1;
      dmn_out=var_out->dim;
      typ_sz=nco_typ_lng(var_out->type);
      val_in_cp=(char *)var_in->val.vp;
      val_out_cp=(char *)var_out->val.vp;
      var_in_cnt=var_in->cnt;
      var_sz=var_in->sz;

      /* As explained in nco_var_dmn_rdr_mtd(),
	 "Hence, we must re-update dmn_out->id after nco_dmn_dfn() in nco_cnf_dmn_rdr_val()
	 Structures should be completely consistent at that point
	 Not updating these structures (at least dmn_out->id) is equivalent to assuming that
	 dmn_out->id does not depend on record dimension identity, which is an ASSUMPTION
	 that may currently be true, but is not guaranteed by the netCDF API to always be true." */
      for(dmn_out_idx=0;dmn_out_idx<dmn_out_nbr;dmn_out_idx++){
        /* NB: Change dmn_id,cnt,srt,end,srd together to minimize chances of forgetting one */
        var_out->dmn_id[dmn_out_idx]=dmn_out[dmn_out_idx]->id;
        var_out->cnt[dmn_out_idx]=dmn_out[dmn_out_idx]->cnt;
        var_out->srt[dmn_out_idx]=dmn_out[dmn_out_idx]->srt;
        var_out->end[dmn_out_idx]=dmn_out[dmn_out_idx]->end;
        var_out->srd[dmn_out_idx]=dmn_out[dmn_out_idx]->srd;
      } /* end loop over dmn_out */

      /* Report full metadata re-order, if requested */
      if(nco_dbg_lvl_get() > 3){
        int dmn_idx_in_out[NC_MAX_VAR_DIMS]; /* [idx] Dimension correspondence, input->output */
        /* Create reverse correspondence */
        for(dmn_out_idx=0;dmn_out_idx<dmn_out_nbr;dmn_out_idx++)
          dmn_idx_in_out[dmn_idx_out_in[dmn_out_idx]]=dmn_out_idx;

        for(dmn_in_idx=0;dmn_in_idx<dmn_in_nbr;dmn_in_idx++)
          (void)fprintf(stdout,"%s: DEBUG %s variable %s re-order maps dimension %s from (ordinal,ID)=(%d,%d) to (%d,%d)\n",nco_prg_nm_get(),fnc_nm,var_in->nm,var_in->dim[dmn_in_idx]->nm,dmn_in_idx,var_in->dmn_id[dmn_in_idx],dmn_idx_in_out[dmn_in_idx],var_out->dmn_id[dmn_idx_in_out[dmn_in_idx]]);
      } /* endif dbg */

      /* Is identity re-ordering requested? */
      for(dmn_out_idx=0;dmn_out_idx<dmn_out_nbr;dmn_out_idx++)
        if(dmn_out_idx != dmn_idx_out_in[dmn_out_idx]) break;
      if(dmn_out_idx == dmn_out_nbr) IDENTITY_REORDER=True;

      /* Dimension reversal breaks identity re-ordering */
      if(IDENTITY_REORDER){
        for(dmn_in_idx=0;dmn_in_idx<dmn_in_nbr;dmn_in_idx++)
          if(dmn_rvr_in[dmn_in_idx]) break;
        if(dmn_in_idx != dmn_in_nbr) IDENTITY_REORDER=False;
      } /* !IDENTITY_REORDER */

      if(IDENTITY_REORDER){
        if(nco_dbg_lvl_get() >= nco_dbg_scl) (void)fprintf(stdout,"%s: INFO %s reports re-order is identity transformation for variable %s\n",nco_prg_nm_get(),fnc_nm,var_in->nm);
        /* Copy in one fell swoop then return */
        (void)memcpy((void *)(var_out->val.vp),(void *)(var_in->val.vp),var_out->sz*nco_typ_lng(var_out->type));
        return;
      } /* !IDENTITY_REORDER */

      if(var_in->has_dpl_dmn) (void)fprintf(stdout,"%s: WARNING %s reports non-identity re-order for variable with duplicate dimensions %s.\n%s does not support non-identity re-orders of variables with duplicate dimensions\n",nco_prg_nm_get(),fnc_nm,var_in->nm,nco_prg_nm_get());

      /* Compute map for each dimension of input variable */
      for(dmn_in_idx=0;dmn_in_idx<dmn_in_nbr;dmn_in_idx++) dmn_in_map[dmn_in_idx]=1L;
      for(dmn_in_idx=0;dmn_in_idx<dmn_in_nbr-1;dmn_in_idx++)
        for(dmn_idx=dmn_in_idx+1;dmn_idx<dmn_in_nbr;dmn_idx++)
          dmn_in_map[dmn_in_idx]*=var_in->cnt[dmn_idx];

      /* Compute map for each dimension of output variable */
      for(dmn_out_idx=0;dmn_out_idx<dmn_out_nbr;dmn_out_idx++) dmn_out_map[dmn_out_idx]=1L;
      for(dmn_out_idx=0;dmn_out_idx<dmn_out_nbr-1;dmn_out_idx++)
        for(dmn_idx=dmn_out_idx+1;dmn_idx<dmn_out_nbr;dmn_idx++)
          dmn_out_map[dmn_out_idx]*=var_out->cnt[dmn_idx];

      /* There is more than one method to re-order dimensions
	 Output dimensionality is known in advance, unlike nco_var_avg()
	 Hence outer loop may be over dimensions or over elements
	 Method 1: Loop over input elements 
	 1a. Loop over 1-D input array offsets
	 1b. Invert 1-D input array offset to get N-D input subscripts
	 1c. Turn N-D input subscripts into N-D output subscripts
	 1d. Map N-D output subscripts to get 1-D output element
	 1e. Copy input element to output element
	 This method is simplified from method used in nco_var_avg()
	 Method 2: Loop over input dimensions
	 1a. Loop over input dimensions, from slowest to fastest varying
	 1b. 
      */
      
      /* Begin Method 1: Loop over input elements */
      /* var_in_lmn is offset into 1-D array */
      for(var_in_lmn=0;var_in_lmn<var_sz;var_in_lmn++){
	
        /* dmn_in_sbs are corresponding indices (subscripts) into N-D array */
        dmn_in_sbs[dmn_in_nbr_m1]=var_in_lmn%var_in_cnt[dmn_in_nbr_m1];
        for(dmn_in_idx=0;dmn_in_idx<dmn_in_nbr_m1;dmn_in_idx++){
          dmn_in_sbs[dmn_in_idx]=(long int)(var_in_lmn/dmn_in_map[dmn_in_idx]);
          dmn_in_sbs[dmn_in_idx]%=var_in_cnt[dmn_in_idx];
        } /* end loop over dimensions */

        /* Dimension reversal:
	   Reversing a dimension changes subscripts along that dimension
	   Consider dimension of size N indexed by [0,1,2,...k-1,k,k+1,...,N-2,N-1] 
	   Reversal maps element k to element N-1-k=N-k-1 
	   Enhance speed by using that all elements along dimension share reversal */
        for(dmn_in_idx=0;dmn_in_idx<dmn_in_nbr;dmn_in_idx++)
          if(dmn_rvr_in[dmn_in_idx]) dmn_in_sbs[dmn_in_idx]=var_in_cnt[dmn_in_idx]-dmn_in_sbs[dmn_in_idx]-1L;
	
        /* Map variable's N-D array indices to get 1-D index into output data */
        var_out_lmn=0L;
        for(dmn_out_idx=0;dmn_out_idx<dmn_out_nbr;dmn_out_idx++) 
          var_out_lmn+=dmn_in_sbs[dmn_idx_out_in[dmn_out_idx]]*dmn_out_map[dmn_out_idx];

        /* Copy current input element into its slot in output array */
        (void)memcpy(val_out_cp+var_out_lmn*typ_sz,val_in_cp+var_in_lmn*typ_sz,(size_t)typ_sz);
      } /* end loop over var_in_lmn */
      /* End Method 1: Loop over input elements */

      /* Begin Method 2: Loop over input dimensions */
      /* End Method 2: Loop over input dimensions */

      dmn_idx_out_in=(int *)nco_free(dmn_idx_out_in);
      dmn_rvr_in=(nco_bool *)nco_free(dmn_rvr_in);

    } /* Match by full variable name  */
  } /* !idx_tbl */

  return;

} /* nco_var_dmn_rdr_val_trv() */
  
void
nco_dmn_trv_msa_tbl                   /* [fnc] Update all GTT dimensions with hyperslabbed size */
(const int nc_id,                     /* I [ID] netCDF input file ID */
 const char * const rec_dmn_nm,       /* I [sng] Record dimension name */
 trv_tbl_sct * const trv_tbl)         /* I/O [sct] GTT (Group Traversal Table) */
{

  int grp_id; 

  /* Loop table */
  for(unsigned idx_tbl=0;idx_tbl<trv_tbl->nbr;idx_tbl++){
    trv_sct var_trv=trv_tbl->lst[idx_tbl];

    /* If object is an extracted variable... */
    if(var_trv.nco_typ == nco_obj_typ_var && var_trv.flg_xtr){

      /* Obtain group ID */
      (void)nco_inq_grp_full_ncid(nc_id,var_trv.grp_nm_fll,&grp_id);

      /* Update for this variable */
      (void)nco_dmn_msa_tbl(grp_id,rec_dmn_nm,&var_trv,trv_tbl);

    } /* If object is an extracted variable... */
  } /* Loop table */

} /* end nco_dmn_trv_msa_tbl() */

void                                  
nco_dmn_msa_tbl                       /* [fnc] Update all GTT dimensions with hyperslabbed size */
(const int grp_in_id,                 /* I [id] netCDF input group ID */
 const char * const rec_dmn_nm_cst,   /* I [sng] User-specified record dimension, if any, to create or fix in output file */
 trv_sct *var_trv,                    /* I/O [sct] Object to write (variable) trv_map_dmn_set() is O */
 trv_tbl_sct * const trv_tbl)         /* I/O [sct] GTT (Group Traversal Table) */
{
  /* Purpose: Update all GTT dimensions with hyperslabbed size */

  const char fnc_nm[]="nco_dmn_msa_tbl()"; /* [sng] Function name */

  char var_nm[NC_MAX_NAME+1L];          /* [sng] Variable name (local copy of object name) */ 
  char *rec_dmn_nm=NULL;                 /* [sng] User-specified record dimension name */
  char *rec_dmn_nm_mlc=NULL;             /* [sng] Local copy of rec_dmn_nm_cst, which may be encoded */
  char dmn_nm[NC_MAX_NAME+1L];            /* [sng] Dimension names  */

  int *dmn_in_id_var;                    /* [id] Dimension IDs array for input variable */
  int var_in_id;                         /* [id] Variable ID */
  int fl_fmt;                            /* [enm] Output file format */
  int nbr_dmn_var;                       /* [nbr] Number of dimensions for variable */
  int rcd=NC_NOERR;                      /* [rcd] Return code */
  int nco_prg_id;                        /* [enm] Program ID */
  int var_dim_id;                        /* [id] Variable dimension ID */ 

  long dmn_cnt;                          /* [sng] Hyperslabbed dimension size  */  
  long dmn_sz;                           /* [sng] Dimension size  */  

  nc_type var_typ;                       /* [enm] netCDF type in input variable (usually same as output) */

  nco_bool CRR_DMN_IS_REC_IN_INPUT;      /* [flg] Current dimension of variable is record dimension of variable in input file/group */
  nco_bool DFN_CRR_DMN_AS_REC_IN_OUTPUT; /* [flg] Define current dimension as record dimension in output file */
  nco_bool FIX_ALL_REC_DMN=False;        /* [flg] Fix all record dimensions */
  nco_bool FIX_REC_DMN=False;            /* [flg] Fix record dimension (opposite of MK_REC_DMN) */
  nco_bool NEED_TO_DEFINE_DIM;           /* [flg] Dimension needs to be defined in *this* group */  

  dmn_trv_sct *dmn_trv;                  /* [sct] Unique dimension object */

  /* File format needed for decision tree and to enable netCDF4 features */
  (void)nco_inq_format(grp_in_id,&fl_fmt);

  /* Local copy of object name */ 
  strcpy(var_nm,var_trv->nm);       

  /* Is requested variable in input file? */
  rcd=nco_inq_varid_flg(grp_in_id,var_nm,&var_in_id);
  if(rcd != NC_NOERR) (void)fprintf(stdout,"%s: %s reports ERROR unable to find variable \"%s\"\n",nco_prg_nm_get(),fnc_nm,var_nm);

  /* Get type of variable and number of dimensions from input */
  (void)nco_inq_var(grp_in_id,var_in_id,(char *)NULL,&var_typ,&nbr_dmn_var,(int *)NULL,(int *)NULL);  

  dmn_in_id_var=(int *)nco_malloc(nbr_dmn_var*sizeof(int));

  /* Get Program ID */
  nco_prg_id=nco_prg_id_get(); 

  if(nco_prg_id == ncks) assert(var_typ == var_trv->var_typ);
  assert(nbr_dmn_var == var_trv->nbr_dmn);

  var_typ=var_trv->var_typ;
  nbr_dmn_var=var_trv->nbr_dmn;

  /* Get dimension IDs for *variable* */
  (void)nco_inq_vardimid(grp_in_id,var_in_id,dmn_in_id_var);

  /* Does user want a record dimension to receive special handling? */
  if(rec_dmn_nm_cst){
    /* Create (and later free()) local copy to preserve const-ness of passed value
    For simplicity, work with canonical name rec_dmn_nm */
    rec_dmn_nm_mlc=strdup(rec_dmn_nm_cst);
    /* Parse rec_dmn_nm argument */
    if(!strcmp("fix_all",rec_dmn_nm_mlc)){
      FIX_ALL_REC_DMN=True;
      FIX_REC_DMN=True;
      rec_dmn_nm=rec_dmn_nm_mlc+4;
    }else if(!strncmp("fix_",rec_dmn_nm_mlc,(size_t)4)){
      FIX_REC_DMN=True; /* [flg] Fix record dimension */
      rec_dmn_nm=rec_dmn_nm_mlc+4;
    }else{
      FIX_REC_DMN=False; /* [flg] Fix record dimension */
      rec_dmn_nm=rec_dmn_nm_mlc;
    } /* strncmp() */    
  } /* !rec_dmn_nm_cst */

  /* If variable has a re-defined record dimension. NOTE: this implies passing NULL as User-specified record dimension parameter  */
  if(var_trv->rec_dmn_nm_out){
    assert(nco_prg_id == ncpdq);
    rec_dmn_nm=(char *)strdup(var_trv->rec_dmn_nm_out);
  } /* If variable has a re-defined record dimension */

  /* Is requested record dimension in input file? */
  if(rec_dmn_nm){

    /* ncks */
    if(nco_prg_id == ncks){
      if(!FIX_ALL_REC_DMN){
        int rec_dmn_id_dmy;   
        rcd=nco_inq_dimid_flg(grp_in_id,rec_dmn_nm,&rec_dmn_id_dmy);
        /* Record name not found in this group */
        if(rcd != NC_NOERR){
          /* Nothing to do, error cheking for invalid dimension names was made at start */     
        } /* endif rcd != NC_NOERR */

        if(rcd == NC_NOERR){
          /* Does variable contain requested record dimension? */
          for(int idx_dmn=0;idx_dmn<nbr_dmn_var;idx_dmn++){
            if(dmn_in_id_var[idx_dmn] == rec_dmn_id_dmy){
              if(nco_dbg_lvl_get() == nco_dbg_old) (void)fprintf(stderr,"%s: INFO %s reports variable %s contains user-specified record dimension %s\n",nco_prg_nm_get(),fnc_nm,var_nm,rec_dmn_nm);
              break;
            } /* endif */
          } /* end loop over idx_dmn */
        } /* endif rcd == NC_NOERR */
      } /* FIX_ALL_REC_DMN */

      /* ncecat */
    }else if(nco_prg_id == ncecat){

    } /* ncecat */
  } /* Is requested record dimension in input file? */

  /* The very important dimension loop... */
  for(int idx_dmn=0;idx_dmn<nbr_dmn_var;idx_dmn++){

    /* Dimension needs to be defined in *this* group? Assume yes... */
    NEED_TO_DEFINE_DIM=True;   

    /* Dimension ID for variable, used to get dimension object in input list  */
    var_dim_id=dmn_in_id_var[idx_dmn];

    /* Get dimension name and size from ID in *input* group */
    (void)nco_inq_dim(grp_in_id,var_dim_id,dmn_nm,&dmn_sz);

    /* Get unique dimension object from unique dimension ID, in input list */
    dmn_trv=nco_dmn_trv_sct(var_dim_id,trv_tbl);

    /* Define dimension in output file if necessary */
    if(NEED_TO_DEFINE_DIM){

      /* Here begins a complex tree to decide a simple, binary output:
	 Will current input dimension be defined as an output record dimension or as a fixed dimension?
	 Decision tree outputs flag DFN_CRR_CMN_AS_REC_IN_OUTPUT that controls subsequent netCDF actions
	 Otherwise would repeat netCDF action code too many times */

      /* Is dimension unlimited in input file? Handy unique dimension has all this info */
      CRR_DMN_IS_REC_IN_INPUT=dmn_trv->is_rec_dmn;

      if(FIX_ALL_REC_DMN){
        DFN_CRR_DMN_AS_REC_IN_OUTPUT=False;
        if(CRR_DMN_IS_REC_IN_INPUT && nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stderr,"%s: INFO %s is defining all input record dimensions, including this one, %s, as fixed dimensions in output file per user request\n",nco_prg_nm_get(),fnc_nm,dmn_nm);
      }else if(rec_dmn_nm){
        /* User requested (with --fix_rec_dmn or --mk_rec_dmn) to treat a certain dimension specially */
        /* ... and this dimension is that dimension, i.e., the user-specified dimension ... */
        if(!strcmp(dmn_nm,rec_dmn_nm)){
          /* ... then honor user's request to define it as a fixed or record dimension ... */
          if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stderr,"%s: INFO %s is defining dimension %s as %s dimension in output file per user request\n",nco_prg_nm_get(),fnc_nm,rec_dmn_nm,(FIX_REC_DMN) ? "fixed" : "record");
          if(FIX_REC_DMN) DFN_CRR_DMN_AS_REC_IN_OUTPUT=False; else DFN_CRR_DMN_AS_REC_IN_OUTPUT=True;
        }else{ /* strcmp() */
          if(FIX_REC_DMN){
            /* ... fix_rec_dmn case is straightforward: output dimension has same format as input dimension */
            if(CRR_DMN_IS_REC_IN_INPUT) DFN_CRR_DMN_AS_REC_IN_OUTPUT=True; else DFN_CRR_DMN_AS_REC_IN_OUTPUT=False;
          }else{ /* !FIX_REC_DMN */
            /* ... otherwise we are in the --mk_rec_dmn case where things get complicated ... 
	       This dimension can be a record dimension only if it would not conflict with the requested 
	       record dimension being defined a record dimension, and that depends on file format. Uggh.
	       1. netCDF3 API allows only one record-dimension so conflicts are possible
	       2. netCDF4 API permits any number of unlimited dimensions so conflicts are impossible */
            if(fl_fmt == NC_FORMAT_NETCDF4){
              /* ... no conflicts possible so define dimension in output same as in input ... */
              if(CRR_DMN_IS_REC_IN_INPUT) DFN_CRR_DMN_AS_REC_IN_OUTPUT=True; else DFN_CRR_DMN_AS_REC_IN_OUTPUT=False;
            }else{ /* !netCDF4 */
              /* ... output file adheres to netCDF3 API so there can be only one record dimension.
		 In other words, define all other dimensions as fixed, non-record dimensions, even
		 if they are a record dimension in the input file ... */
              if(CRR_DMN_IS_REC_IN_INPUT && nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stderr,"%s: INFO %s is defining dimension %s as fixed (non-record) in output file even though it is a record dimension in the input file. This is necessary to satisfy user request that %s be the record dimension in the output file which adheres to the netCDF3 API where the record dimension, if any, must be a variable's first dimension.\n",nco_prg_nm_get(),fnc_nm,dmn_nm,rec_dmn_nm);
              DFN_CRR_DMN_AS_REC_IN_OUTPUT=False;
            } /* !netCDF4 */
          } /* !FIX_REC_DMN */
        } /* strcmp() */

      }else{ /* !rec_dmn_nm */
        /* ... no user-specified record dimension so define dimension in output same as in input ... */
        if(CRR_DMN_IS_REC_IN_INPUT) DFN_CRR_DMN_AS_REC_IN_OUTPUT=True; else DFN_CRR_DMN_AS_REC_IN_OUTPUT=False;
      } /* !rec_dmn_nm && !FIX_ALL_REC_DMN */ 

      /* At long last ... */

      /* Define current index dimension size */

      /* If current dimension is to be defined as record dimension in output file */
      if(DFN_CRR_DMN_AS_REC_IN_OUTPUT){  

        if(var_trv->var_dmn[idx_dmn].is_crd_var){
          dmn_cnt=var_trv->var_dmn[idx_dmn].crd->lmt_msa.dmn_cnt;
        } else {
          dmn_cnt=var_trv->var_dmn[idx_dmn].ncd->lmt_msa.dmn_cnt;
        }
        (void)nco_dmn_set_msa(var_dim_id,dmn_cnt,trv_tbl);  


        /* ! DFN_CRR_DMN_AS_REC_IN_OUTPUT */
      }else{
        /* Get size from GTT */
        if(var_trv->var_dmn[idx_dmn].is_crd_var){
          /* Set size */
          dmn_cnt=var_trv->var_dmn[idx_dmn].crd->lmt_msa.dmn_cnt;
          /* Update GTT dimension */
          (void)nco_dmn_set_msa(var_dim_id,dmn_cnt,trv_tbl);        
        }else{
          /* Set size */
          dmn_cnt=var_trv->var_dmn[idx_dmn].ncd->lmt_msa.dmn_cnt;
          /* Update GTT dimension */
          (void)nco_dmn_set_msa(var_dim_id,dmn_cnt,trv_tbl);  
        } /* end else */
      } /* Define dimension size */
    } /* end if dimension is not yet defined */
  } /* End of the very important dimension loop */

  dmn_in_id_var=(int *)nco_free(dmn_in_id_var);

} /* end nco_dmn_msa_tbl() */

void                          
nco_dmn_dgn_tbl                       /* [fnc] Transfer degenerated dimensions information into GTT  */
(dmn_sct **dmn_dgn,                   /* [sct] Degenerate (size 1) dimensions used by ncwa */
 const int nbr_dmn_dgn,               /* I [nbr] Total number of dimensions in list */
 trv_tbl_sct *trv_tbl)                /* I/O [sct] GTT (Group Traversal Table) */
{
  trv_tbl->nbr_dmn_dgn=nbr_dmn_dgn;
  trv_tbl->dmn_dgn=(dmn_sct *)nco_malloc(nbr_dmn_dgn*sizeof(dmn_sct));

  /* Loop dimensions */
  for(int idx_dmn=0;idx_dmn<nbr_dmn_dgn;idx_dmn++){
    trv_tbl->dmn_dgn[idx_dmn].id=dmn_dgn[idx_dmn]->id;
    trv_tbl->dmn_dgn[idx_dmn].cnt=dmn_dgn[idx_dmn]->cnt;
  } /* Loop dimensions */

} /* nco_dmn_dgn_tbl() */

void
nco_dmn_lst_ass_var_trv                /* [fnc] Create list of all dimensions associated with input variable list  (ncpdq, ncwa) */
(const int nc_id,                      /* I [id] netCDF file ID */
 const trv_tbl_sct * const trv_tbl,    /* I [sct] GTT (Group Traversal Table) */
 int *nbr_dmn_xtr,                     /* O [nbr] Number of dimensions associated with variables to be extracted  */
 dmn_sct ***dmn)                       /* O [sct] Array of dimensions associated with variables to be extracted  */
{
  /* Purpose: Create list of all dimensions associated with input variable list */

  const char fnc_nm[]="nco_dmn_lst_ass_var_trv()"; /* [sng] Function name */

  int nbr_dmn; /* [nbr] Number of dimensions associated with variables to be extracted */

  long dmn_cnt; /* [nbr] Hyperslabbed size of dimension */  
  long dmn_sz; /* [nbr] Size of dimension  */  

  nco_bool dmn_flg; /* [flg] Is dimension already inserted in output array  */  

  nbr_dmn=0;

  /* Traverse table and match relative dimension names */

  /* Loop table */
  for(unsigned idx_var=0;idx_var<trv_tbl->nbr;idx_var++){
    trv_sct var_trv=trv_tbl->lst[idx_var];

    /* If GTT variable object is to extract */
    if(var_trv.nco_typ == nco_obj_typ_var && var_trv.flg_xtr){ 

      /* Loop variable dimensions  */
      for(int idx_dmn_var=0;idx_dmn_var<var_trv.nbr_dmn;idx_dmn_var++){

        dmn_flg=False;

        /* Get unique dimension object from unique dimension ID */
        dmn_trv_sct *dmn_trv=nco_dmn_trv_sct(var_trv.var_dmn[idx_dmn_var].dmn_id,trv_tbl);

        assert(dmn_trv);
        assert(!strcmp(dmn_trv->nm,var_trv.var_dmn[idx_dmn_var].dmn_nm));

        /* Loop constructed array of output dimensions to see if already inserted  */
        for(int idx_dmn_out=0;idx_dmn_out<nbr_dmn;idx_dmn_out++){

          /* Match by ID */
          if(var_trv.var_dmn[idx_dmn_var].dmn_id==(*dmn)[idx_dmn_out]->id){
            dmn_flg=True;
            break;
          } /* Match by ID */
        } /* Loop constructed array of output dimensions to see if already inserted  */ 

        /* If this dimension is not in output array */
        if(!dmn_flg){

          /* Add one more element to array  */
          (*dmn)=(dmn_sct **)nco_realloc((*dmn),(nbr_dmn+1)*sizeof(dmn_sct *));
          (*dmn)[nbr_dmn]=(dmn_sct *)nco_malloc(sizeof(dmn_sct));

          /* Get size from GTT. NOTE use index idx_dmn_var */
          if(var_trv.var_dmn[idx_dmn_var].is_crd_var){
            dmn_cnt=var_trv.var_dmn[idx_dmn_var].crd->lmt_msa.dmn_cnt;
            dmn_sz=var_trv.var_dmn[idx_dmn_var].crd->sz;
            (*dmn)[nbr_dmn]->is_crd_dmn=True;
          }else{
            dmn_cnt=var_trv.var_dmn[idx_dmn_var].ncd->lmt_msa.dmn_cnt;
            dmn_sz=var_trv.var_dmn[idx_dmn_var].ncd->sz;
            (*dmn)[nbr_dmn]->is_crd_dmn=False;
          } /* end else */

          (*dmn)[nbr_dmn]->nm=(char *)strdup(var_trv.var_dmn[idx_dmn_var].dmn_nm);
          (*dmn)[nbr_dmn]->id=var_trv.var_dmn[idx_dmn_var].dmn_id;
          (*dmn)[nbr_dmn]->nc_id=nc_id;
          (*dmn)[nbr_dmn]->xrf=NULL;
          (*dmn)[nbr_dmn]->val.vp=NULL;
          (*dmn)[nbr_dmn]->is_rec_dmn=dmn_trv->is_rec_dmn;
          (*dmn)[nbr_dmn]->cnt=dmn_cnt;
          (*dmn)[nbr_dmn]->sz=dmn_sz;
          (*dmn)[nbr_dmn]->srt=0L;
          (*dmn)[nbr_dmn]->end=dmn_cnt-1L;
          (*dmn)[nbr_dmn]->srd=1L;

          (*dmn)[nbr_dmn]->cid=-1;
          (*dmn)[nbr_dmn]->cnk_sz=0L;
          (*dmn)[nbr_dmn]->type=(nc_type)-1;

          nbr_dmn++;
        } /* If this dimension is not in output array */
      } /* Loop variable dimensions  */
    } /* Filter variables  */
  } /* Loop table */

  /* Export */
  *nbr_dmn_xtr=nbr_dmn;

  if(nco_dbg_lvl_get() >= nco_dbg_dev){ 
    (void)fprintf(stdout,"%s: DEBUG %s dimensions to export: ",nco_prg_nm_get(),fnc_nm);        
    for(int idx_dmn=0;idx_dmn<nbr_dmn;idx_dmn++) (void)fprintf(stdout,"#%d<%s> : ",(*dmn)[idx_dmn]->id,(*dmn)[idx_dmn]->nm);        
    (void)fprintf(stdout,"\n");    
  } /* endif */

  return;
} /* end nco_dmn_lst_ass_var_trv() */

void
nco_dmn_avg_mk                         /* [fnc] Build dimensions to average(ncwa)/re-order(ncpdq) array from input dimension names */
(const int nc_id,                      /* I [id] netCDF file ID */
 char **obj_lst_in,                    /* I [sng] User-specified list of dimension names (-a names) */
 const int nbr_dmn_in,                 /* I [nbr] Total number of dimensions in input list (size of above array) */
 const nco_bool flg_dmn_prc_usr_spc,   /* I [flg] Processed dimensions specified on command line */
 const nco_bool flg_rdd,               /* I [flg] Retain degenerate dimensions */
 const trv_tbl_sct * const trv_tbl,    /* I [sct] GTT (Group Traversal Table) */
 dmn_sct ***dmn_avg,                   /* O [sct] Array of dimensions to average */
 int *nbr_dmn_avg)                     /* O [nbr] Number of dimensions to average (size of above array) */
{
  /* Purpose: Create list of dimensions from list of dimension name strings. Function based on nco_xtr_mk(). */

  /* Dimensions to average/not average are built using these 3 functions:
     
     nco_dmn_avg_mk() Build dimensions to average array from input dimension names 
     nco_dmn_out_mk() Build dimensions array to keep on output
     nco_dmn_id_mk()  Mark flag average for all dimensions that have the input ID 
     
     nco_dmn_avg_mk() parses -a names and exports an array of dmn_sct; it marks the flag "flg_dmn_avg" of "var_dmn_sct"
     as True, if the dimension is to be averaged.
     
     Since variables share dimensions, this flag has to be marked to all variable's dimensions that have it;
     This broadcast is made in nco_dmn_id_mk(), using the unique dimension ID as key.
     
     nco_dmn_out_mk() checks this flag, and if the dimension is not to be averaged, it is added to an array of dmn_sct,
     dimensions on output. */
  
  char *usr_sng;            /* [sng] User-supplied object name */

  int obj_nbr;              /* [nbr] Number of objects in list */
  int nbr_avg_dmn;          /* [nbr] Number of dimensions to average (output) */

  long dmn_cnt;             /* [nbr] Hyperslabbed size of dimension */  
  long dmn_sz;              /* [nbr] Size of dimension  */  

  /* Used only by ncpdq , ncwa */
  assert(nco_prg_id_get() == ncpdq || nco_prg_id_get() == ncwa);

  /* Initialize values */
  obj_nbr=nbr_dmn_in;
  nbr_avg_dmn=0;

  /* Loop input dimension name list */
  for(int idx_obj=0;idx_obj<obj_nbr;idx_obj++){

    usr_sng=strdup(obj_lst_in[idx_obj]); 

    /* Convert pound signs (back) to commas */
    nco_hash2comma(usr_sng);

    /* Look for regular expressions only when user explicitly specified dimensions */
    if(flg_dmn_prc_usr_spc){
      /* If usr_sng is regular expression ... */
      if(strpbrk(usr_sng,".*^$\\[]()<>+?|{}")){
        /* ... and regular expression library is present */
#ifdef NCO_HAVE_REGEX_FUNCTIONALITY
        /* fxm 20131217 TODO */ 
        (void)fprintf(stdout,"%s: ERROR: Sorry, wildcarding (extended regular expression matches to dimension names) is not implemented for -a option.\n",nco_prg_nm_get()); 
        nco_exit(EXIT_FAILURE); 
#else /* !NCO_HAVE_REGEX_FUNCTIONALITY */
        (void)fprintf(stdout,"%s: ERROR: Sorry, wildcarding (extended regular expression matches to dimension names) was not built into this NCO executable, so unable to compile regular expression \"%s\".\nHINT: Make sure libregex.a is on path and re-build NCO.\n",nco_prg_nm_get(),usr_sng);
        nco_exit(EXIT_FAILURE);
#endif /* !NCO_HAVE_REGEX_FUNCTIONALITY */
      } /* end if regular expression */
    } /* flg_dmn_prc_usr_spc */    

    /* Loop table */
    for(unsigned int idx_tbl=0;idx_tbl<trv_tbl->nbr;idx_tbl++){

      trv_sct trv_obj=trv_tbl->lst[idx_tbl];

      /* Variable to extract */
      if(trv_obj.nco_typ == nco_obj_typ_var && trv_obj.flg_xtr){

        /* Loop variable dimensions */
        for(int idx_var_dmn=0;idx_var_dmn<trv_obj.nbr_dmn;idx_var_dmn++){

          /* Get unique dimension object from unique dimension ID */
          dmn_trv_sct *dmn_trv=nco_dmn_trv_sct(trv_obj.var_dmn[idx_var_dmn].dmn_id,trv_tbl);

          assert(dmn_trv);
          assert(strcmp(dmn_trv->nm,trv_obj.var_dmn[idx_var_dmn].dmn_nm) == 0);

          /* Dimension ID, used to avoid duplicate insertions */
          int dmn_id=trv_obj.var_dmn[idx_var_dmn].dmn_id;

          /* Dimension name full */
          char *dmn_nm_fll=trv_obj.var_dmn[idx_var_dmn].dmn_nm_fll;

          /* Dimension name relative */
          char *dmn_nm=trv_obj.var_dmn[idx_var_dmn].dmn_nm;

          /* Must meet necessary flags */
          nco_bool pth_mth=nco_pth_mch(dmn_nm_fll,dmn_nm,usr_sng);

          if(pth_mth){
            int idx_dmn_out; /* [idx] Index for output dimensions */
            
            /* Is dimension already in output array? Match by dimension ID since dimension IDs are unique */
            for(idx_dmn_out=0;idx_dmn_out<nbr_avg_dmn;idx_dmn_out++)
              if(dmn_id == (*dmn_avg)[idx_dmn_out]->id) break;
            
            /* If this dimension is not yet in output array */
            if(idx_dmn_out == nbr_avg_dmn){
              
              /* Change flag to mark that dimension is to be averaged instead of to keep on output */
              trv_tbl->lst[idx_tbl].var_dmn[idx_var_dmn].flg_dmn_avg=True;
              
              /* Add one more element to array  */
              (*dmn_avg)=(dmn_sct **)nco_realloc((*dmn_avg),(nbr_avg_dmn+1)*sizeof(dmn_sct *));
              (*dmn_avg)[nbr_avg_dmn]=(dmn_sct *)nco_malloc(sizeof(dmn_sct));
              
              /* Get size from GTT. NOTE use index idx_var_dmn */
              if(trv_obj.var_dmn[idx_var_dmn].is_crd_var){
                dmn_cnt=trv_obj.var_dmn[idx_var_dmn].crd->lmt_msa.dmn_cnt;
                dmn_sz=trv_obj.var_dmn[idx_var_dmn].crd->sz;
                (*dmn_avg)[nbr_avg_dmn]->is_crd_dmn=True;
              }else{
                dmn_cnt=trv_obj.var_dmn[idx_var_dmn].ncd->lmt_msa.dmn_cnt;
                dmn_sz=trv_obj.var_dmn[idx_var_dmn].ncd->sz;
                (*dmn_avg)[nbr_avg_dmn]->is_crd_dmn=False;
              } /* end else */
              
              (*dmn_avg)[nbr_avg_dmn]->nm=(char *)strdup(trv_obj.var_dmn[idx_var_dmn].dmn_nm);
              (*dmn_avg)[nbr_avg_dmn]->nm_fll=(char *)strdup(trv_obj.var_dmn[idx_var_dmn].dmn_nm_fll);
              (*dmn_avg)[nbr_avg_dmn]->id=trv_obj.var_dmn[idx_var_dmn].dmn_id;
              (*dmn_avg)[nbr_avg_dmn]->nc_id=nc_id;
              (*dmn_avg)[nbr_avg_dmn]->xrf=NULL;
              (*dmn_avg)[nbr_avg_dmn]->val.vp=NULL;
              (*dmn_avg)[nbr_avg_dmn]->is_rec_dmn=dmn_trv->is_rec_dmn;
              (*dmn_avg)[nbr_avg_dmn]->cnt=dmn_cnt;
              (*dmn_avg)[nbr_avg_dmn]->sz=dmn_sz;
              (*dmn_avg)[nbr_avg_dmn]->srt=0L;
              (*dmn_avg)[nbr_avg_dmn]->end=dmn_cnt-1L;
              (*dmn_avg)[nbr_avg_dmn]->srd=1L;
              (*dmn_avg)[nbr_avg_dmn]->cid=-1;
              (*dmn_avg)[nbr_avg_dmn]->cnk_sz=0L;
              (*dmn_avg)[nbr_avg_dmn]->type=(nc_type)-1;
        
              /* Broadcast flag average/keep using dimension ID; variables share dimensions */
              (void)nco_dmn_id_mk(dmn_id,flg_rdd,trv_tbl);
        
              /* Increment number of dimensions found */
              nbr_avg_dmn++;
            } /* If this dimension is not in output array */
          } /* Must meet necessary flags */
        } /* Loop variable dimensions */ 
      } /* Variable to extract */
    } /* Loop table */
  } /* Loop input dimension name list */
  
  /* Export */
  *nbr_dmn_avg=nbr_avg_dmn;
  
  if(nco_dbg_lvl_get() >= nco_dbg_var){ 
    (void)fprintf(stdout,"%s: INFO dimensions to average: ",nco_prg_nm_get());        
    for(int idx_dmn=0;idx_dmn<nbr_avg_dmn;idx_dmn++) (void)fprintf(stdout,"<%s>",(*dmn_avg)[idx_dmn]->nm);
    (void)fprintf(stdout,"\n");    
  } /* endif dbg */
  
  return;
  
} /* nco_dmn_avg_mk() */

void
nco_dmn_out_mk                         /* [fnc] Build dimensions array to keep on output */
(dmn_sct **dmn_xtr,                    /* I [sct] Array of dimensions associated with variables to be extracted  */
 const int nbr_dmn_xtr,                /* I [nbr] Number of dimensions associated with variables to be extracted (size of above array) */
 const trv_tbl_sct * const trv_tbl,    /* I [sct] GTT (Group Traversal Table) */
 dmn_sct ***dmn_out,                   /* O [sct] Array of dimensions on ouput */
 int *nbr_dmn_out)                     /* O [nbr] Number of dimensions on output (size of above array) */
{
  /* Purpose: Create list of dimensions from list of dimension name strings */

  const char fnc_nm[]="nco_dmn_out_mk()"; /* [sng] Function name  */

  int nbr_out_dmn;          /* [nbr] Number of dimensions to keep in output */

  nco_bool flg_dmn_ins;     /* [flg] Is dimension already inserted in output array  */  

  /* Used only by ncpdq , ncwa */
  assert(nco_prg_id_get() == ncpdq || nco_prg_id_get() == ncwa);

  /* Initialize values */
  nbr_out_dmn=0;

  /* Loop table */
  for(unsigned int idx_tbl=0;idx_tbl<trv_tbl->nbr;idx_tbl++){

    trv_sct trv_obj=trv_tbl->lst[idx_tbl];

    /* Variable to extract */
    if(trv_obj.nco_typ == nco_obj_typ_var && trv_obj.flg_xtr){

      /* Loop variable dimensions */
      for(int idx_var_dmn=0;idx_var_dmn<trv_obj.nbr_dmn;idx_var_dmn++){

        /* This dimension is not to be averaged, it is to be kept on output */
        if(!trv_obj.var_dmn[idx_var_dmn].flg_dmn_avg){

          /* Search dimensions to be extracted  */
          for(int idx_xtr_dmn=0;idx_xtr_dmn<nbr_dmn_xtr;idx_xtr_dmn++){

            /* Dimension ID, match key */
            int dmn_id=trv_obj.var_dmn[idx_var_dmn].dmn_id;

            /* Match by ID */
            if(dmn_id == dmn_xtr[idx_xtr_dmn]->id){

              /* Assume dimension is not yet inserted in array */
              flg_dmn_ins=False;

              /* Loop constructed array of output dimensions to see if already inserted  */
              for(int idx_dmn_out=0;idx_dmn_out<nbr_out_dmn;idx_dmn_out++){

                /* Match by ID */
                if(dmn_id == (*dmn_out)[idx_dmn_out]->id){
                  /* Mark as inserted in array */
                  flg_dmn_ins=True;
                  break;
                } /* Match by ID */
              } /* Loop constructed array of output dimensions to see if already inserted  */ 

              /* If this dimension is not in output array */
              if(!flg_dmn_ins){
                /* Output list comprises non-averaged and, if specified, degenerate dimensions */
                (*dmn_out)[nbr_out_dmn]=nco_dmn_dpl(dmn_xtr[idx_xtr_dmn]);
                (void)nco_dmn_xrf(dmn_xtr[idx_xtr_dmn],(*dmn_out)[nbr_out_dmn]);
                nbr_out_dmn++;
              } /* If this dimension is not in output array */
            } /* Match by ID */
          } /* Search dimensions to be extracted  */
        } /* This dimension is not to be averaged, it is to be kept on output */ 
      } /* Loop variable dimensions */ 
    } /* Variable to extract */
  } /* Loop table */

  /* Export */
  *nbr_dmn_out=nbr_out_dmn;

  if(nco_dbg_lvl_get() >= nco_dbg_dev){ 
    (void)fprintf(stdout,"%s: DEBUG %s dimensions to keep on output: ",nco_prg_nm_get(),fnc_nm);        
    for(int idx_dmn=0;idx_dmn<nbr_out_dmn;idx_dmn++) (void)fprintf(stdout,"#%d<%s> : ",(*dmn_out)[idx_dmn]->id,(*dmn_out)[idx_dmn]->nm);
    (void)fprintf(stdout,"\n");       
  } /* endif dbg */

  return;

} /* nco_dmn_out_mk() */

void
nco_dmn_id_mk                          /* [fnc] Mark flag average, optionally flag degenerate for all dimensions that have the input ID */
(const int dmn_id,                     /* I [nbr] Number of dimensions associated with variables to be extracted (size of above array) */
 const nco_bool flg_rdd,               /* I [flg] Mark flag retain degenerate dimension */
 const trv_tbl_sct * const trv_tbl)    /* I [sct] GTT (Group Traversal Table) */
{
  /* Purpose: Mark flag average, optionally flag degenerate for all dimensions that have the input ID */

  /* Used only by ncpdq , ncwa */
  assert(nco_prg_id_get() == ncpdq || nco_prg_id_get() == ncwa);

  /* Loop table */
  for(unsigned int idx_tbl=0;idx_tbl<trv_tbl->nbr;idx_tbl++){

    trv_sct trv_obj=trv_tbl->lst[idx_tbl];

    /* Variable to extract */
    if(trv_obj.nco_typ == nco_obj_typ_var && trv_obj.flg_xtr){

      /* Loop variable dimensions */
      for(int idx_var_dmn=0;idx_var_dmn<trv_obj.nbr_dmn;idx_var_dmn++){

        /* Match ID */
        if(dmn_id == trv_obj.var_dmn[idx_var_dmn].dmn_id){

          /* Change flag to mark that dimension is to be averaged instead of to keep on output */
          trv_tbl->lst[idx_tbl].var_dmn[idx_var_dmn].flg_dmn_avg=True;

          /* Change flag to retain degenerate dimension */
          if(flg_rdd) trv_tbl->lst[idx_tbl].var_dmn[idx_var_dmn].flg_rdd=True;

        } /* Match ID */
      } /* Loop variable dimensions */
    } /* Variable to extract */
  } /* Loop table */

  return;

} /* nco_dmn_id_mk() */

void                          
nco_bld_rec_dmn                       /* [fnc] Build record dimensions array */
(const int nc_id,                     /* I [ID] netCDF input file ID */
 const nco_bool FORTRAN_IDX_CNV,      /* I [flg] Hyperslab indices obey Fortran convention */
 lmt_sct ***lmt_rec,                  /* I/O [lst] (ncra) Record dimensions */
 int *nbr_rec,                        /* I/O [nbr] (ncra) Number of record dimensions (size of above array) */
 trv_tbl_sct * trv_tbl)               /* I/O [sct] GTT (Group Traversal Table) */
{
  const char fnc_nm[]="nco_bld_rec_dmn()"; /* [sng] Function name  */

  int var_id;              /* [id] Variable ID */
  int grp_id;              /* [id] Group ID */

  dmn_trv_sct *dmn_trv;    /* [sct] Unique dimension object */

  nco_bool flg_dmn_ins;    /* [flg] Is dimension already inserted in output array  */  

  int rec_nbr;             /* [sct] Number of record dimensions to export */

#ifndef ENABLE_UDUNITS
  nco_bool flg_prn=False;
#endif /* !ENABLE_UDUNITS */

  /* Initialize */
  rec_nbr=0;

  /* Used only by record operators ncra,ncrcat */
  assert(nco_prg_id_get() == ncra || nco_prg_id_get() == ncrcat);

  for(unsigned int idx_tbl=0;idx_tbl<trv_tbl->nbr;idx_tbl++){

    trv_sct var_trv=trv_tbl->lst[idx_tbl];

    /* Filter extracted variables  */
    if(var_trv.nco_typ == nco_obj_typ_var && var_trv.flg_xtr){

      for(int idx_var_dmn=0;idx_var_dmn<var_trv.nbr_dmn;idx_var_dmn++){

        /* Assume dimension is not yet inserted in array */
        flg_dmn_ins=False;

        /* Dimension ID */
        int dmn_id=var_trv.var_dmn[idx_var_dmn].dmn_id;

        /* Get unique dimension object from unique dimension ID, in input list */
        dmn_trv=nco_dmn_trv_sct(dmn_id,trv_tbl);

        /* Is this a record dimension? */
        if(dmn_trv->is_rec_dmn){

          /* Has this record dimension already been inserted in the array of output dimensions? */
          for(int idx_dmn_out=0;idx_dmn_out<rec_nbr;idx_dmn_out++){

            /* Match by ID */
            if(dmn_id == (*lmt_rec)[idx_dmn_out]->id){
              /* Mark as inserted in array */
              flg_dmn_ins=True;
              break;
            } /* Match by ID */
          } /* !idx_dmn_out */

          /* If this dimension is not in output array */
          if(!flg_dmn_ins){

            /* Add one more element to array  */
            (*lmt_rec)=(lmt_sct **)nco_realloc((*lmt_rec),(rec_nbr+1)*sizeof(lmt_sct *));
            (*lmt_rec)[rec_nbr]=(lmt_sct *)nco_malloc(sizeof(lmt_sct));

            /* Obtain group ID */
            (void)nco_inq_grp_full_ncid(nc_id,var_trv.grp_nm_fll,&grp_id);

            /* a) case where the dimension has coordinate variables */
            if(var_trv.var_dmn[idx_var_dmn].crd){

              crd_sct *crd=var_trv.var_dmn[idx_var_dmn].crd;

              /* Create stand-alone limit structure for given dimension */
              (*lmt_rec)[rec_nbr]=nco_lmt_sct_mk(grp_id,dmn_id,crd->lmt_msa.lmt_dmn,crd->lmt_msa.lmt_dmn_nbr,FORTRAN_IDX_CNV);

              /* Insert full group name, key for group ID match */
              (*lmt_rec)[rec_nbr]->grp_nm_fll=(char *)strdup(crd->crd_grp_nm_fll);
              (*lmt_rec)[rec_nbr]->nm_fll=(char *)strdup(crd->dmn_nm_fll);

              /* b) case of dimension only (there is no coordinate variable for this dimension */
            }else{

              dmn_trv_sct *ncd=var_trv.var_dmn[idx_var_dmn].ncd;

              /* Create stand-alone limit structure for given dimension */
              (*lmt_rec)[rec_nbr]=nco_lmt_sct_mk(grp_id,dmn_id,ncd->lmt_msa.lmt_dmn,ncd->lmt_msa.lmt_dmn_nbr,FORTRAN_IDX_CNV);

              /* Insert full group name, key for group ID match */
              (*lmt_rec)[rec_nbr]->grp_nm_fll=(char *)strdup(ncd->grp_nm_fll);
              (*lmt_rec)[rec_nbr]->nm_fll=(char *)strdup(ncd->nm_fll);

            } /* b) case of dimension only (there is no coordinate variable for this dimension */
            
            (*lmt_rec)[rec_nbr]->cln_typ=cln_nil; 
            (*lmt_rec)[rec_nbr]->origin=0.0;
            (*lmt_rec)[rec_nbr]->rbs_sng=NULL;

            /* Check if coordinate variable */
            int rcd=nco_inq_varid_flg(grp_id,var_trv.var_dmn[idx_var_dmn].dmn_nm,&var_id);

            /* Obtain record coordinate metadata */
            if(rcd == NC_NOERR){ 
              char *cln_att_sng=NULL;     
              (*lmt_rec)[rec_nbr]->rbs_sng=nco_lmt_get_udu_att(grp_id,var_id,"units"); 
              cln_att_sng=nco_lmt_get_udu_att(grp_id,var_id,"calendar"); 
              (*lmt_rec)[rec_nbr]->cln_typ=nco_cln_get_cln_typ(cln_att_sng); 
              if(cln_att_sng) cln_att_sng=(char*)nco_free(cln_att_sng);  
            } /* endif */

            /* Store ID */
            (*lmt_rec)[rec_nbr]->id=dmn_id;

#ifndef ENABLE_UDUNITS
            if(nco_dbg_lvl_get() >= nco_dbg_vrb && nco_dbg_lvl_get() != nco_dbg_dev && flg_prn == False){
              if((*lmt_rec)[rec_nbr]->rbs_sng) (void)fprintf(stderr,"%s: WARNING Record coordinate %s has a \"units\" attribute but NCO was built without UDUnits. NCO is therefore unable to detect and correct for inter-file unit re-basing issues. See http://nco.sf.net/nco.html#rbs for more information.\n%s: HINT Re-build or re-install NCO enabled with UDUnits.\n",
                nco_prg_nm_get(),(*lmt_rec)[rec_nbr]->nm,nco_prg_nm_get());
              flg_prn=True;
            }
#endif /* !ENABLE_UDUNITS */

            /* Increase array size */
            rec_nbr++;

          } /* If this dimension is not in output array */

        } /* Is record */
      } /* Loop variable dimensions */ 
    } /* Variable to extract */
  } /* Loop table */

  if(nco_dbg_lvl_get() >= nco_dbg_dev){ 
    (void)fprintf(stdout,"%s: DEBUG %s record dimensions to process: ",nco_prg_nm_get(),fnc_nm);   
    for(int idx_rec=0;idx_rec<rec_nbr;idx_rec++)
      (void)fprintf(stdout,"#%d<%s/%s> : ",(*lmt_rec)[idx_rec]->id,(*lmt_rec)[idx_rec]->grp_nm_fll,(*lmt_rec)[idx_rec]->nm);        
    (void)fprintf(stdout,"\n");       
  } /* endif dbg */

  /* Export */
  *nbr_rec=rec_nbr;
  return;
} /* nco_bld_rec_dmn() */

void
nco_prn_tbl_lmt                       /* [fnc] Print table limits */
(trv_tbl_sct * const trv_tbl)         /* I/O [sct] Traversal table */
{
  const char fnc_nm[]="nco_prn_tbl_lmt()"; /* [sng] Function name  */

  /* Loop table */
  for(unsigned int idx_tbl=0;idx_tbl<trv_tbl->nbr;idx_tbl++){

    trv_sct var_trv=trv_tbl->lst[idx_tbl];

    /* Is variable */
    if(var_trv.nco_typ == nco_obj_typ_var){

      /* Loop variable dimensions */
      for(int idx_var_dmn=0;idx_var_dmn<var_trv.nbr_dmn;idx_var_dmn++){

        /* a) case where the dimension has coordinate variables */
        if(var_trv.var_dmn[idx_var_dmn].crd){

          crd_sct *crd=trv_tbl->lst[idx_tbl].var_dmn[idx_var_dmn].crd;

          int lmt_dmn_nbr=crd->lmt_msa.lmt_dmn_nbr;

          if(lmt_dmn_nbr){
            (void)fprintf(stdout,"%s: INFO %s : <%s> : %s : limits:%d ->",nco_prg_nm_get(),fnc_nm,var_trv.nm_fll,var_trv.var_dmn[idx_var_dmn].dmn_nm,lmt_dmn_nbr);
            for(int lmt_idx=0;lmt_idx<lmt_dmn_nbr;lmt_idx++){ 
              lmt_sct *lmt_dmn=crd->lmt_msa.lmt_dmn[lmt_idx];
              (void)fprintf(stdout," [%d]%s(%li,%li,%li) :",lmt_idx,lmt_dmn->nm,lmt_dmn->srt,lmt_dmn->cnt,lmt_dmn->srd);
            } /* endfor */
            (void)fprintf(stdout,"\n");
          } /* endif */

          /* b) case of dimension only (there is no coordinate variable for this dimension */
        }else{

          dmn_trv_sct *ncd=trv_tbl->lst[idx_tbl].var_dmn[idx_var_dmn].ncd;

          int lmt_dmn_nbr=ncd->lmt_msa.lmt_dmn_nbr;

          if(lmt_dmn_nbr){
            (void)fprintf(stdout,"%s: INFO %s : <%s> : %s :limits: %d->",nco_prg_nm_get(),fnc_nm,var_trv.nm_fll,var_trv.var_dmn[idx_var_dmn].dmn_nm,lmt_dmn_nbr);
            for(int lmt_idx=0;lmt_idx<lmt_dmn_nbr;lmt_idx++){ 
              lmt_sct *lmt_dmn=ncd->lmt_msa.lmt_dmn[lmt_idx];
              (void)fprintf(stdout," [%d]%s(%li,%li,%li) :",lmt_idx,lmt_dmn->nm,lmt_dmn->srt,lmt_dmn->cnt,lmt_dmn->srd);
            } /* endfor */
            (void)fprintf(stdout,"\n");
          } /* endif */

        } /* b) case of dimension only (there is no coordinate variable for this dimension */

      } /* Loop variable dimensions */
    } /* Is variable */
  } /* Loop table */

} /* nco_prn_tbl_lmt() */

void
nco_bld_trv_tbl                       /* [fnc] Construct GTT, Group Traversal Table (groups,variables,dimensions, limits)   */
(const int nc_id,                     /* I [ID] netCDF file ID */
 char * const grp_pth,                /* I [sng] Absolute group path where to start build (root typically) */
 int lmt_nbr,                         /* I [nbr] number of dimensions with limits */
 CST_X_PTR_CST_PTR_CST_Y(char,lmt_arg), /* I [sng] List of user-specified dimension limits */
 const int aux_nbr,                   /* I [nbr] Number of auxiliary coordinates */
 char *aux_arg[],                     /* I [sng] Auxiliary coordinates */
 nco_bool MSA_USR_RDR,                /* I [flg] Multi-Slab Algorithm returns hyperslabs in user-specified order */
 nco_bool FORTRAN_IDX_CNV,            /* I [flg] Hyperslab indices obey Fortran convention */
 char **grp_lst_in,                   /* I [sng] User-specified list of groups */
 const int grp_lst_in_nbr,            /* I [nbr] Number of groups in list */
 char **var_lst_in,                   /* I [sng] User-specified list of variables */
 const int var_xtr_nbr,               /* I [nbr] Number of variables in list */
 const nco_bool EXTRACT_ALL_COORDINATES, /* I [flg] Process all coordinates */ 
 const nco_bool flg_unn,              /* I [flg] Select union of specified groups and variables */
 const nco_bool GRP_XTR_VAR_XCL,      /* I [flg] Extract matching groups, exclude matching variables */
 const nco_bool EXCLUDE_INPUT_LIST,   /* I [flg] Exclude rather than extract groups and variables specified with -v */ 
 const nco_bool EXTRACT_ASSOCIATED_COORDINATES, /* I [flg] Extract all coordinates associated with extracted variables? */
 const nco_bool EXTRACT_CLL_MSR,      /* I [flg] Extract cell_measures variables */
 const nco_bool EXTRACT_FRM_TRM,      /* I [flg] Extract formula_terms variables */
 const int nco_pck_plc,               /* I [enm] Packing policy */
 nco_dmn_dne_t **flg_dne,             /* I/O [lst] Flag to check if input dimension -d "does not exist" */
 trv_tbl_sct * const trv_tbl)         /* I/O [sct] Traversal table */
{
  /* Purpose: Construct GTT, Group Traversal Table (groups, variables, dimensions, limits) 
     
     Return value: Check for valid input; NC_NOERR (0) or > 0 for a user input error, used to go to close_and_free on main
     
     Notes:
     Dimension limit structures are handled internally in this function and not exported
     Call sequence is important: 
     1) nco_trv_hsh_bld() must be called after nco_grp_itr() because other functions use hash table 
     2) nco_grp_itr() must be first, main iterator function. Then
     nco_bld_dmn_ids_trv()
     nco_bld_crd_rec_var_trv()
     nco_bld_crd_var_trv()
     nco_has_crd_dmn_scp()
     nco_bld_var_dmn()
     Traversal table now has all necessary _structures_ complete
     Next, call functions dependent on user-supplied options:
     nco_xtr_mk()
     nco_xtr_xcl()
     nco_xtr_crd_add()
     nco_xtr_crd_ass_add()
     nco_xtr_cf_add()
     nco_xtr_xcl_chk()
     Limit-related functions must be called in order:
     nco_lmt_prs()
     nco_prs_aux_crd()
     nco_chk_dmn_in()
     Functions called for specific operators include:
     ncbo: trv_tbl_srt()
     nces: nco_bld_nsm() */
  
  const char fnc_nm[]="nco_bld_trv_tbl()"; /* [sng] Function name  */

  cnv_sct *cnv; /* [flg] File adheres to NCAR CCM/CCSM/CF conventions */

  lmt_sct **lmt=NULL_CEWI;  /* [sct] User defined limits */

  /* Construct traversal table objects (groups, variables) */
  (void)nco_grp_itr(nc_id,(char *)NULL,grp_pth,trv_tbl);

  /* Build dimension information for all variables (match dimension IDs) */
  (void)nco_bld_dmn_ids_trv(nc_id,trv_tbl);

  /* Build "is_crd_var" and "is_rec_var" members for all variables */
  (void)nco_bld_crd_rec_var_trv(trv_tbl);

  /* Build GTT "crd_sct" coordinate variable structure */
  (void)nco_bld_crd_var_trv(trv_tbl);

  /* Variables in dimension's scope? */
  (void)nco_has_crd_dmn_scp(trv_tbl);

  /* Assign variables' dimensions to either coordinates or dimension structs */
  (void)nco_bld_var_dmn(trv_tbl);       

  /* ncbo co-sequential match algorithm requires alphabetical sorted full names. Do it here, to avoid rebuilding hash table */
  if(nco_prg_id_get() == ncbo) (void)trv_tbl_srt((int)0,trv_tbl);

  /* Hash traversal table for faster access */
  (void)nco_trv_hsh_bld(trv_tbl);

  /* Build auxiliary coordinates information into table */
  if(aux_nbr){
    if(!nco_bld_crd_aux(nc_id,trv_tbl))
       if(!nco_bld_crd_nm_aux(nc_id,"lat","lon",trv_tbl))
         if(!nco_bld_crd_nm_aux(nc_id,"latitude","longitude",trv_tbl))
           if(!nco_bld_crd_nm_aux(nc_id,"Latitude","Longitude",trv_tbl))
	     if(!nco_bld_crd_nm_aux(nc_id,"lat_gds","lon_gds",trv_tbl)){
	        (void)fprintf(stderr,"%s: %s reports unable to find lat/lon coordinates with standard_name's = \"latitude/longitude\". Nor able to find appropriate auxiliary coordinates named \"lat/lon\", \"latitude/longitude\" or \"Latitude/Longitude\" or \"lat_gds/lon_gds\".\n",nco_prg_nm_get(),fnc_nm);
	        nco_exit(EXIT_FAILURE);  
	     }
    
  } /* !aux_nbr */
  
  /* Check -v and -g input names and create extraction list */
  (void)nco_xtr_mk(grp_lst_in,grp_lst_in_nbr,var_lst_in,var_xtr_nbr,EXCLUDE_INPUT_LIST,EXTRACT_ALL_COORDINATES,flg_unn,trv_tbl);

  /* Change included variables to excluded variables */
  if(EXCLUDE_INPUT_LIST) (void)nco_xtr_xcl(EXTRACT_ASSOCIATED_COORDINATES,GRP_XTR_VAR_XCL,trv_tbl);

  /* Add all coordinate variables to extraction list */
  if(EXTRACT_ALL_COORDINATES) (void)nco_xtr_crd_add(trv_tbl);

  /* Extract coordinates associated with extracted variables */
  if(EXTRACT_ASSOCIATED_COORDINATES) (void)nco_xtr_crd_ass_add(nc_id,trv_tbl);

  /* Determine conventions (ARM/CCM/CCSM/CF/MPAS) for treating file */
  cnv=nco_cnv_ini(nc_id);
  if(!cnv->CCM_CCSM_CF && aux_nbr){
    (void)fprintf(stderr,"%s: WARNING -X option selected on input lacking global \"Conventions=CF-1.X\" attribute. Assuming CF-compliance intended in order to exploit -X. HINT: To fix this warning, add conformant Conventions attribute with, e.g., \"ncatted -a Conventions,global,c,c,CF-1.0 in.nc\"\n",nco_prg_nm_get());
    cnv->CCM_CCSM_CF=True;
  } /* endif */
  if(cnv->CCM_CCSM_CF && EXTRACT_ASSOCIATED_COORDINATES){
    /* Workaround CCSM "feature" that lev lacks bounds="ilev" attribute */
    (void)nco_xtr_ilev_add(trv_tbl);
    /* Implement CF "ancillary_variables", "bounds", "climatology", "coordinates", and "grid_mapping" */
    if(EXTRACT_CLL_MSR) (void)nco_xtr_cf_add(nc_id,"cell_measures",trv_tbl);
    if(EXTRACT_FRM_TRM) (void)nco_xtr_cf_add(nc_id,"formula_terms",trv_tbl);
    (void)nco_xtr_cf_add(nc_id,"ancillary_variables",trv_tbl);
    (void)nco_xtr_cf_add(nc_id,"bounds",trv_tbl);
    (void)nco_xtr_cf_add(nc_id,"climatology",trv_tbl);
    (void)nco_xtr_cf_add(nc_id,"coordinates",trv_tbl);
    (void)nco_xtr_cf_add(nc_id,"grid_mapping",trv_tbl);
    /* Do all twice, so that, e.g., auxiliary coordinates retrieved because of "coordinates" come with their "bounds" variables */
    if(EXTRACT_CLL_MSR) (void)nco_xtr_cf_add(nc_id,"cell_measures",trv_tbl);
    if(EXTRACT_FRM_TRM) (void)nco_xtr_cf_add(nc_id,"formula_terms",trv_tbl);
    (void)nco_xtr_cf_add(nc_id,"ancillary_variables",trv_tbl);
    (void)nco_xtr_cf_add(nc_id,"climatology",trv_tbl);
    (void)nco_xtr_cf_add(nc_id,"coordinates",trv_tbl);
    (void)nco_xtr_cf_add(nc_id,"bounds",trv_tbl);
    (void)nco_xtr_cf_add(nc_id,"grid_mapping",trv_tbl);
  } /* cnv->CCM_CCSM_CF */

  /* Die with helpful hint if extraction rules lead to retaining user-excluded variables
     See discussion at https://github.com/nco/nco/issues/211 */
  if(EXCLUDE_INPUT_LIST && EXTRACT_ASSOCIATED_COORDINATES) (void)nco_xtr_xcl_chk(var_lst_in,var_xtr_nbr,trv_tbl);

  /* Mark extracted dimensions */
  (void)nco_xtr_dmn_mrk(trv_tbl);

  /* Mark extracted groups */
  (void)nco_xtr_grp_mrk(trv_tbl);

  /* Parse auxiliary coordinates and build found limits directly into table (auxiliary limits are not merged into regular limits) */
  if(aux_nbr) (void)nco_prs_aux_crd(nc_id,aux_nbr,aux_arg,FORTRAN_IDX_CNV,MSA_USR_RDR,EXTRACT_ASSOCIATED_COORDINATES,trv_tbl);

  /* Add dimension limits */
  if(lmt_nbr){
    lmt=nco_lmt_prs(lmt_nbr,lmt_arg);
    (void)nco_bld_lmt(nc_id,MSA_USR_RDR,lmt_nbr,lmt,FORTRAN_IDX_CNV,trv_tbl);
  } /* !lmt_nbr */

  /* Build ensembles */
  if(nco_prg_id_get() == ncge) (void)nco_bld_nsm(nc_id,True,cnv,nco_pck_plc,trv_tbl);
  if(nco_prg_id_get() == ncbo) (void)nco_bld_nsm(nc_id,False,cnv,nco_pck_plc,trv_tbl);

   /* Check valid input (limits) */
  if(lmt_nbr) (void)nco_chk_dmn_in(lmt_nbr,lmt,flg_dne,trv_tbl);

  /* Free limits */
  if(lmt_nbr > 0) lmt=nco_lmt_lst_free(lmt,lmt_nbr);

  if(nco_dbg_lvl_get() == nco_dbg_vrb) trv_tbl_prn_flg_xtr(fnc_nm,trv_tbl);

  return;
} /* nco_bld_trv_tbl() */

void
nco_chk_dmn                           /* [fnc] Check valid dimension names */
(const int lmt_nbr,                   /* I [nbr] number of dimensions with limits */
 nco_dmn_dne_t * flg_dne)             /* I/O [lst] Flag to check if input dimension -d "does not exist" */
{
  /* Were all user-specified dimensions found? */ 
  for(int lmt_idx=0;lmt_idx<lmt_nbr;lmt_idx++){
    /* Check this flag */
    if(flg_dne[lmt_idx].flg_dne){
      (void)fprintf(stdout,"%s: ERROR dimension %s is not in input file\n",nco_prg_nm_get(),flg_dne[lmt_idx].dim_nm);
      flg_dne=(nco_dmn_dne_t *)nco_free(flg_dne);
      nco_exit(EXIT_FAILURE);
    } /* Check this flag */
  } /* Were all user-specified dimensions found? */
} /* nco_chk_dmn() */

void
nco_chk_dmn_in                        /* [fnc] Check input dimensions */
(const int lmt_nbr,                   /* I [nbr] Number of user-specified dimension limits */
 CST_X_PTR_CST_PTR_CST_Y(lmt_sct,lmt), /* I [sct] List of user-specified dimension limits */
 nco_dmn_dne_t ** const dne_lst,      /* I/O [lst] Input dimension does not exist */
 const trv_tbl_sct * const trv_tbl)   /* I [sct] Traversal table */
{ 
  *dne_lst=(nco_dmn_dne_t *)nco_malloc(lmt_nbr*sizeof(nco_dmn_dne_t));

  /* Be pessimistic and assume invalid user input */
  for(int lmt_idx=0;lmt_idx<lmt_nbr;lmt_idx++) (*dne_lst)[lmt_idx].flg_dne=True; 

  /* Loop over input names */
  for(int lmt_idx=0;lmt_idx<lmt_nbr;lmt_idx++){
    assert(lmt[lmt_idx]->nm);

    (*dne_lst)[lmt_idx].dim_nm=(char *) strdup(lmt[lmt_idx]->nm);

    /* Match input relative name to dimension relative name */ 
    for(unsigned int dmn_idx=0;dmn_idx<trv_tbl->nbr_dmn;dmn_idx++)
      if(!strcmp(lmt[lmt_idx]->nm,trv_tbl->lst_dmn[dmn_idx].nm)) (*dne_lst)[lmt_idx].flg_dne=False; 
  } /* end for */
} /* nco_chk_dmn_in() */

void
nco_bld_lmt                           /* [fnc] Assign user specified dimension limits to traversal table */
(const int nc_id,                     /* I [ID] netCDF file ID */
 nco_bool MSA_USR_RDR,                /* I [flg] Multi-Slab Algorithm returns hyperslabs in user-specified order */
 int lmt_nbr,                         /* I [nbr] Number of user-specified dimension limits */
 lmt_sct **lmt,                       /* I [sct] Limit array. Structure comming from nco_lmt_prs() */
 nco_bool FORTRAN_IDX_CNV,            /* I [flg] Hyperslab indices obey Fortran convention */
 trv_tbl_sct * const trv_tbl)         /* I/O [sct] Traversal table */
{
  /* Purpose: Assign user-specified dimension limits to traversal table  
     At this point "lmt" was parsed from nco_lmt_prs(); only the relative names and  min, max, stride are known 
     Steps:

     Find total numbers of matches for a dimension
     ncks -d lon,0,0,1 ~/nco/data/in_grp.nc
     Here "lmt_nbr" is 1 and there is 1 match at most
     ncks -d lon,0,0,1 -d lon,0,0,1 -d lat,0,0,1  ~/nco/data/in_grp.nc
     Here "lmt_nbr" is 3 and there are 2 matches at most for "lon" and 1 match at most for "lat"
     The limits have to be separated to 
     a) case of coordinate variables
     b) case of dimension only (there is no coordinate variable for that dimension)
     
     Deep copy matches to table, match at the current index, increment current index
     
     Apply MSA for each Dimension in a new cycle (that now has all its limits in place :-) ) 
     At this point lmt_sct is no longer needed;  
     
     Tests:
     ncks -D 11 -d lon,0,0,1 -d lon,1,1,1 -d lat,0,0,1 -d time,1,2,1 -d time,6,7,1 -v lon,lat,time -H ~/nco/data/in_grp.nc
     ncks -D 11 -d time,8,9 -d time,0,2  -v time -H ~/nco/data/in_grp.nc
     ncks -D 11 -d time,8,2 -v time -H ~/nco/data/in_grp.nc # wrapped limit */

  for(unsigned int idx_tbl=0;idx_tbl<trv_tbl->nbr;idx_tbl++){

    trv_sct var_trv=trv_tbl->lst[idx_tbl];

    /* Is variable to extract  */
    if(var_trv.nco_typ == nco_obj_typ_var && var_trv.flg_xtr){

      /* Loop variable dimensions */
      for(int idx_var_dmn=0;idx_var_dmn<var_trv.nbr_dmn;idx_var_dmn++){

        /* Loop input name list */
        for(int lmt_idx=0;lmt_idx<lmt_nbr;lmt_idx++){

          /* Match input relative name to dimension relative name */ 
          if(strcmp(lmt[lmt_idx]->nm,var_trv.var_dmn[idx_var_dmn].dmn_nm) == 0){

            /*  The limits have to be separated to */

            /* a) case where the dimension has coordinate variables */
            if(var_trv.var_dmn[idx_var_dmn].crd){

              /* Increment number of dimension limits for this dimension */
              trv_tbl->lst[idx_tbl].var_dmn[idx_var_dmn].crd->lmt_msa.lmt_dmn_nbr++;

              int nbr_lmt=trv_tbl->lst[idx_tbl].var_dmn[idx_var_dmn].crd->lmt_msa.lmt_dmn_nbr;
              trv_tbl->lst[idx_tbl].var_dmn[idx_var_dmn].crd->lmt_msa.lmt_dmn=(lmt_sct **)nco_realloc(trv_tbl->lst[idx_tbl].var_dmn[idx_var_dmn].crd->lmt_msa.lmt_dmn,nbr_lmt*sizeof(lmt_sct *));

              /* b) case of dimension only (there is no coordinate variable for this dimension */
            }else{

              /* Increment number of dimension limits for this dimension */
              trv_tbl->lst[idx_tbl].var_dmn[idx_var_dmn].ncd->lmt_msa.lmt_dmn_nbr++;

              int nbr_lmt=trv_tbl->lst[idx_tbl].var_dmn[idx_var_dmn].ncd->lmt_msa.lmt_dmn_nbr;
              trv_tbl->lst[idx_tbl].var_dmn[idx_var_dmn].ncd->lmt_msa.lmt_dmn=(lmt_sct **)nco_realloc(trv_tbl->lst[idx_tbl].var_dmn[idx_var_dmn].ncd->lmt_msa.lmt_dmn,nbr_lmt*sizeof(lmt_sct *));

            } /* b) case of dimension only (there is no coordinate variable for this dimension */

          } /* Match input relative name to dimension relative name */ 
        } /* Loop input name list */
      } /* Loop variable dimensions */
    } /* Is variable to extract  */
  } /* Loop table  */

  /* Store matches in table, match at the current index, increment current index */

  /* Loop table  */
  for(unsigned int idx_tbl=0;idx_tbl<trv_tbl->nbr;idx_tbl++){

    trv_sct var_trv=trv_tbl->lst[idx_tbl];

    /* Is variable to extract  */
    if(var_trv.nco_typ == nco_obj_typ_var && var_trv.flg_xtr){

      /* Loop variable dimensions */
      for(int idx_var_dmn=0;idx_var_dmn<var_trv.nbr_dmn;idx_var_dmn++){

        /* Loop input name list */
        for(int lmt_idx=0;lmt_idx<lmt_nbr;lmt_idx++){

          /* Match input relative name to dimension relative name */ 
          if(!strcmp(lmt[lmt_idx]->nm,var_trv.var_dmn[idx_var_dmn].dmn_nm)){

            /* Divide limits into two different cases */

            /* a) Dimension has coordinate variables */
            if(var_trv.var_dmn[idx_var_dmn].crd){

              crd_sct *crd=trv_tbl->lst[idx_tbl].var_dmn[idx_var_dmn].crd;

              /* Limit is same as dimension in input file? */
              trv_tbl->lst[idx_tbl].var_dmn[idx_var_dmn].crd->lmt_msa.NON_HYP_DMN=False;

              /* Parse user-specified limits into hyperslab specifications. NOTE: Use True parameter and "crd" */
              (void)nco_lmt_evl_dmn_crd(nc_id,0L,FORTRAN_IDX_CNV,crd->crd_grp_nm_fll,crd->nm,crd->sz,crd->is_rec_dmn,True,lmt[lmt_idx]);

              /* Current index (lmt_crr) of dimension limits for this (idx_dmn) table dimension  */
              int lmt_crr=crd->lmt_msa.lmt_crr;

              /* Increment current index being initialized  */
              trv_tbl->lst[idx_tbl].var_dmn[idx_var_dmn].crd->lmt_msa.lmt_crr++;

              /* Allocate this limit */
              trv_tbl->lst[idx_tbl].var_dmn[idx_var_dmn].crd->lmt_msa.lmt_dmn[lmt_crr]=(lmt_sct *)nco_malloc(sizeof(lmt_sct));

              /* Initialize this entry */
              (void)nco_lmt_init(trv_tbl->lst[idx_tbl].var_dmn[idx_var_dmn].crd->lmt_msa.lmt_dmn[lmt_crr]);

              /* Store dimension ID */
              lmt[lmt_idx]->id=crd->dmn_id;

              /* Store this valid input; deep-copy to table */ 
              (void)nco_lmt_cpy(lmt[lmt_idx],trv_tbl->lst[idx_tbl].var_dmn[idx_var_dmn].crd->lmt_msa.lmt_dmn[lmt_crr]);

            }else{
              /* b) Dimension only (no coordinate variable for this dimension) */
              dmn_trv_sct *ncd=trv_tbl->lst[idx_tbl].var_dmn[idx_var_dmn].ncd;

              /* Limit is same as dimension in input file ? */
              trv_tbl->lst[idx_tbl].var_dmn[idx_var_dmn].ncd->lmt_msa.NON_HYP_DMN=False;

              /* Parse user-specified limits into hyperslab specifications. NOTE: Use False parameter and "dmn" */
              (void)nco_lmt_evl_dmn_crd(nc_id,0L,FORTRAN_IDX_CNV,ncd->grp_nm_fll,ncd->nm,ncd->sz,ncd->is_rec_dmn,False,lmt[lmt_idx]);

              /* Current index (lmt_crr) of dimension limits for this (idx_dmn) table dimension  */
              int lmt_crr=ncd->lmt_msa.lmt_crr;

              /* Increment current index being initialized  */
              trv_tbl->lst[idx_tbl].var_dmn[idx_var_dmn].ncd->lmt_msa.lmt_crr++;

              /* Alloc this limit */
              trv_tbl->lst[idx_tbl].var_dmn[idx_var_dmn].ncd->lmt_msa.lmt_dmn[lmt_crr]=(lmt_sct *)nco_malloc(sizeof(lmt_sct));

              /* Initialize this entry */
              (void)nco_lmt_init(trv_tbl->lst[idx_tbl].var_dmn[idx_var_dmn].ncd->lmt_msa.lmt_dmn[lmt_crr]);

              /* Store dimension ID */
              lmt[lmt_idx]->id=ncd->dmn_id;

              /* Store this valid input; deep-copy to table */ 
              (void)nco_lmt_cpy(lmt[lmt_idx],trv_tbl->lst[idx_tbl].var_dmn[idx_var_dmn].ncd->lmt_msa.lmt_dmn[lmt_crr]);

            } /* b) case of dimension only (there is no coordinate variable for this dimension */

          } /* Match input relative name to dimension relative name */ 
        } /* Loop input name list */
      } /* Loop variable dimensions */
    } /* Is variable to extract  */
  } /* Loop table step 2 */

  /*  Apply MSA for each dimension in new cycle (that now has all its limits in place) */

  /* Loop table step 3 */
  for(unsigned int idx_tbl=0;idx_tbl<trv_tbl->nbr;idx_tbl++){

    trv_sct var_trv=trv_tbl->lst[idx_tbl];

    /* Is variable to extract  */
    if(var_trv.nco_typ == nco_obj_typ_var && var_trv.flg_xtr){

      /* Loop variable dimensions */
      for(int idx_var_dmn=0;idx_var_dmn<var_trv.nbr_dmn;idx_var_dmn++){

        /* Loop input name list */
        for(int lmt_idx=0;lmt_idx<lmt_nbr;lmt_idx++){

          /* Match input relative name to dimension relative name */ 
          if(strcmp(lmt[lmt_idx]->nm,var_trv.var_dmn[idx_var_dmn].dmn_nm) == 0){

            /* Limits divide into two cases */

            /* a) Dimension has coordinate variables */
            if(var_trv.var_dmn[idx_var_dmn].crd){

              /* Adapted from original MSA loop in nco_msa_lmt_all_ntl(); differences are marked GTT specific */

              nco_bool flg_ovl; /* [flg] Limits overlap */

              crd_sct *crd=trv_tbl->lst[idx_tbl].var_dmn[idx_var_dmn].crd;

              /* GTT: If this coordinate has no limits, continue */
              if(crd->lmt_msa.lmt_dmn_nbr == 0) continue;

              /* ncra/ncrcat have only one limit for record dimension so skip evaluation otherwise this messes up multi-file operation */
              if(crd->is_rec_dmn && (nco_prg_id_get() == ncra || nco_prg_id_get() == ncrcat)) continue;

              /* Split-up wrapped limits. NOTE: using deep copy version nco_msa_wrp_splt_cpy() */   
              (void)nco_msa_wrp_splt_cpy(&trv_tbl->lst[idx_tbl].var_dmn[idx_var_dmn].crd->lmt_msa);

              /* Wrapped hyperslabs are dimensions broken into the "wrong" order, e.g., from
              -d time,8,2 broken into -d time,8,9 -d time,0,2 
              WRP flag set only when list contains dimensions split as above */
              if(trv_tbl->lst[idx_tbl].var_dmn[idx_var_dmn].crd->lmt_msa.WRP){

                /* Find and store size of output dim */  
                (void)nco_msa_clc_cnt(&trv_tbl->lst[idx_tbl].var_dmn[idx_var_dmn].crd->lmt_msa); 

                continue;
              } /* End WRP flag set */

              /* Single slab---no analysis needed */  
              if(trv_tbl->lst[idx_tbl].var_dmn[idx_var_dmn].crd->lmt_msa.lmt_dmn_nbr == 1){

                (void)nco_msa_clc_cnt(&trv_tbl->lst[idx_tbl].var_dmn[idx_var_dmn].crd->lmt_msa);  

                continue;    
              } /* End Single slab */

              /* Does Multi-Slab Algorithm returns hyperslabs in user-specified order? */
              if(MSA_USR_RDR){
                trv_tbl->lst[idx_tbl].var_dmn[idx_var_dmn].crd->lmt_msa.MSA_USR_RDR=True;

                /* Find and store size of output dimension */  
                (void)nco_msa_clc_cnt(&trv_tbl->lst[idx_tbl].var_dmn[idx_var_dmn].crd->lmt_msa);  

                continue;
              } /* End MSA_USR_RDR */

              /* Sort limits */
              (void)nco_msa_qsort_srt(&trv_tbl->lst[idx_tbl].var_dmn[idx_var_dmn].crd->lmt_msa);

              /* Check for overlap */
              flg_ovl=nco_msa_ovl(&trv_tbl->lst[idx_tbl].var_dmn[idx_var_dmn].crd->lmt_msa); 

              /* Find and store size of output dimension */  
              (void)nco_msa_clc_cnt(&trv_tbl->lst[idx_tbl].var_dmn[idx_var_dmn].crd->lmt_msa);

              if(nco_dbg_lvl_get() >= nco_dbg_fl){
                if(flg_ovl) (void)fprintf(stdout,"%s: coordinate \"%s\" has overlapping hyperslabs\n",nco_prg_nm_get(),crd->nm); else (void)fprintf(stdout,"%s: coordinate \"%s\" has distinct hyperslabs\n",nco_prg_nm_get(),crd->nm); 
              } /* endif */

            }else{

              /* b) Dimension only (no coordinate variable for this dimension) */

              dmn_trv_sct *ncd=trv_tbl->lst[idx_tbl].var_dmn[idx_var_dmn].ncd;


              /* Adapted from the original MSA loop in nco_msa_lmt_all_ntl(); differences are marked GTT specific */
              nco_bool flg_ovl; /* [flg] Limits overlap */

              /* GTT: If this dimension has no limits, continue */
              if(ncd->lmt_msa.lmt_dmn_nbr == 0) continue;

              /* ncra/ncrcat have only one limit for record dimension so skip evaluation otherwise this messes up multi-file operation */
              if(ncd->is_rec_dmn && (nco_prg_id_get() == ncra || nco_prg_id_get() == ncrcat)) continue;

              /* Split-up wrapped limits */   
              (void)nco_msa_wrp_splt_trv(trv_tbl->lst[idx_tbl].var_dmn[idx_var_dmn].ncd);

              /* Wrapped hyperslabs are dimensions broken into the "wrong" order,e.g. from
              -d time,8,2 broken into -d time,8,9 -d time,0,2 
              WRP flag set only when list contains dimensions split as above */
              if(trv_tbl->lst[idx_tbl].var_dmn[idx_var_dmn].ncd->lmt_msa.WRP){

                /* Find and store size of output dim */  
                (void)nco_msa_clc_cnt_trv(trv_tbl->lst[idx_tbl].var_dmn[idx_var_dmn].ncd); 

                continue;
              } /* End WRP flag set */

              /* Single slab---no analysis needed */  
              if(trv_tbl->lst[idx_tbl].var_dmn[idx_var_dmn].ncd->lmt_msa.lmt_dmn_nbr == 1){

                (void)nco_msa_clc_cnt_trv(trv_tbl->lst[idx_tbl].var_dmn[idx_var_dmn].ncd);  

                continue;    
              } /* End Single slab */

              /* Does Multi-Slab Algorithm returns hyperslabs in user-specified order ? */
              if(MSA_USR_RDR){
                trv_tbl->lst[idx_tbl].var_dmn[idx_var_dmn].ncd->lmt_msa.MSA_USR_RDR=True;

                /* Find and store size of output dimension */  
                (void)nco_msa_clc_cnt_trv(trv_tbl->lst[idx_tbl].var_dmn[idx_var_dmn].ncd);  

                continue;
              } /* End MSA_USR_RDR */

              /* Sort limits */
              (void)nco_msa_qsort_srt_trv(trv_tbl->lst[idx_tbl].var_dmn[idx_var_dmn].ncd);

              /* Check for overlap */
              flg_ovl=nco_msa_ovl_trv(trv_tbl->lst[idx_tbl].var_dmn[idx_var_dmn].ncd);  

              if(!flg_ovl) trv_tbl->lst[idx_tbl].var_dmn[idx_var_dmn].ncd->lmt_msa.MSA_USR_RDR=True;

              /* Find and store size of output dimension */  
              (void)nco_msa_clc_cnt_trv(trv_tbl->lst[idx_tbl].var_dmn[idx_var_dmn].ncd);

              if(nco_dbg_lvl_get() >= nco_dbg_fl){
                if(flg_ovl) (void)fprintf(stdout,"%s: dimension \"%s\" has overlapping hyperslabs\n",nco_prg_nm_get(),ncd->nm); else (void)fprintf(stdout,"%s: dimension \"%s\" has distinct hyperslabs\n",nco_prg_nm_get(),ncd->nm); 
              } /* endif */

            } /* b) case of dimension only (there is no coordinate variable for this dimension */

          } /* Match input relative name to dimension relative name */ 
        } /* Loop input name list */
      } /* Loop variable dimensions */
    } /* Is variable to extract  */
  } /* Loop table step 3 */

} /* nco_bld_lmt() */

void
nco_bld_lmt_var                       /* [fnc] Assign user specified dimension limits to one GTT variable */
(const int nc_id,                     /* I [ID] netCDF file ID */
  nco_bool MSA_USR_RDR,               /* I [flg] Multi-Slab Algorithm returns hyperslabs in user-specified order */
  int lmt_nbr,                        /* I [nbr] Number of user-specified dimension limits */
  lmt_sct **lmt,                      /* I [sct] Structure comming from nco_lmt_prs() */
  nco_bool FORTRAN_IDX_CNV,           /* I [flg] Hyperslab indices obey Fortran convention */
  trv_sct *var_trv)                  /* I/O [sct] GTT variable (used for weight/mask) */
{
  /* Purpose: Assign user-specified dimension limits to to one GTT variable
  Same as nco_bld_lmt(), with 3 step loops, but for one variable, 'var_trv'
  At this point "lmt" was parsed from nco_lmt_prs(); only the relative names and  min, max, stride are known */

  /* Remove and initialize MSA structures for the dimensions. Members that so require are:
  lmt_sct **lmt_dmn: list of limit structures associated with each dimension, must be free(d)
  int lmt_dmn_nbr: number of lmt arguments, must be reset to zero
  int lmt_crr: index of current limit structure being initialized (helper to initialze lmt_sct*), must be reset to zero
  Other members of 'lmt_msa_sct' remain the same or are not incremented
  */

  for (int dmn_idx = 0; dmn_idx < var_trv->nbr_dmn; dmn_idx++) {
    if (var_trv->var_dmn[dmn_idx].crd) {
      /* Free first ! */
      for (int lmt_idx = 0; lmt_idx < var_trv->var_dmn[dmn_idx].crd->lmt_msa.lmt_dmn_nbr; lmt_idx++) {
        var_trv->var_dmn[dmn_idx].crd->lmt_msa.lmt_dmn[lmt_idx] = nco_lmt_free(var_trv->var_dmn[dmn_idx].crd->lmt_msa.lmt_dmn[lmt_idx]);
      }
      var_trv->var_dmn[dmn_idx].crd->lmt_msa.lmt_dmn = (lmt_sct **)nco_free(var_trv->var_dmn[dmn_idx].crd->lmt_msa.lmt_dmn);
      /* Reset after ! */
      var_trv->var_dmn[dmn_idx].crd->lmt_msa.lmt_dmn_nbr = 0;
      var_trv->var_dmn[dmn_idx].crd->lmt_msa.lmt_crr = 0;
    }
    else if (var_trv->var_dmn[dmn_idx].ncd) {
      /* Free first ! */
      for (int lmt_idx = 0; lmt_idx < var_trv->var_dmn[dmn_idx].ncd->lmt_msa.lmt_dmn_nbr; lmt_idx++) {
        var_trv->var_dmn[dmn_idx].ncd->lmt_msa.lmt_dmn[lmt_idx] = nco_lmt_free(var_trv->var_dmn[dmn_idx].ncd->lmt_msa.lmt_dmn[lmt_idx]);
      }
      var_trv->var_dmn[dmn_idx].ncd->lmt_msa.lmt_dmn = (lmt_sct **)nco_free(var_trv->var_dmn[dmn_idx].ncd->lmt_msa.lmt_dmn);
      /* Reset after ! */
      var_trv->var_dmn[dmn_idx].ncd->lmt_msa.lmt_dmn_nbr = 0;
      var_trv->var_dmn[dmn_idx].ncd->lmt_msa.lmt_crr = 0;
    }
  }

  /* Loop step 1, alloc 'lmt_dmn' */

  /* Loop variable dimensions */
  for (int idx_var_dmn = 0; idx_var_dmn < var_trv->nbr_dmn; idx_var_dmn++) {
    /* Loop input name list */
    for (int lmt_idx = 0; lmt_idx < lmt_nbr; lmt_idx++)
      /* Match input relative name to dimension relative name */
      if (strcmp(lmt[lmt_idx]->nm, var_trv->var_dmn[idx_var_dmn].dmn_nm) == 0) {
        /* Coordinate variable structure case */
        if (var_trv->var_dmn[idx_var_dmn].crd) {
          /* Increment number of dimension limits for this dimension */
          var_trv->var_dmn[idx_var_dmn].crd->lmt_msa.lmt_dmn_nbr++;
          int nbr_lmt = var_trv->var_dmn[idx_var_dmn].crd->lmt_msa.lmt_dmn_nbr;
          var_trv->var_dmn[idx_var_dmn].crd->lmt_msa.lmt_dmn = (lmt_sct **)nco_realloc(var_trv->var_dmn[idx_var_dmn].crd->lmt_msa.lmt_dmn, nbr_lmt * sizeof(lmt_sct *));
        }
        else {
          /* Non coordinate variable structure case */
          /* Increment number of dimension limits for this dimension */
          var_trv->var_dmn[idx_var_dmn].ncd->lmt_msa.lmt_dmn_nbr++;
          int nbr_lmt = var_trv->var_dmn[idx_var_dmn].ncd->lmt_msa.lmt_dmn_nbr;
          var_trv->var_dmn[idx_var_dmn].ncd->lmt_msa.lmt_dmn = (lmt_sct **)nco_realloc(var_trv->var_dmn[idx_var_dmn].ncd->lmt_msa.lmt_dmn, nbr_lmt * sizeof(lmt_sct *));
        }
      }
  }

  /* Loop step 2, store matches in table, match at the current index, increment current index */

  /* Loop variable dimensions */
  for (int idx_var_dmn = 0; idx_var_dmn < var_trv->nbr_dmn; idx_var_dmn++) {
    /* Loop input name list */
    for (int lmt_idx = 0; lmt_idx < lmt_nbr; lmt_idx++) {
      /* Match input relative name to dimension relative name */
      if (!strcmp(lmt[lmt_idx]->nm, var_trv->var_dmn[idx_var_dmn].dmn_nm)) {
        /* Coordinate variable structure case */
        if (var_trv->var_dmn[idx_var_dmn].crd) {
          crd_sct *crd = var_trv->var_dmn[idx_var_dmn].crd;
          /* Limit is same as dimension in input file? */
          var_trv->var_dmn[idx_var_dmn].crd->lmt_msa.NON_HYP_DMN = False;
          /* Parse user-specified limits into hyperslab specifications. NOTE: Use True parameter and "crd" */
          (void)nco_lmt_evl_dmn_crd(nc_id, 0L, FORTRAN_IDX_CNV, crd->crd_grp_nm_fll, crd->nm, crd->sz, crd->is_rec_dmn, True, lmt[lmt_idx]);
          /* Current index (lmt_crr) of dimension limits for this (idx_dmn) table dimension  */
          int lmt_crr = crd->lmt_msa.lmt_crr;
          /* Increment current index being initialized  */
          var_trv->var_dmn[idx_var_dmn].crd->lmt_msa.lmt_crr++;
          /* Allocate this limit */
          var_trv->var_dmn[idx_var_dmn].crd->lmt_msa.lmt_dmn[lmt_crr] = (lmt_sct *)nco_malloc(sizeof(lmt_sct));
          /* Initialize this entry */
          (void)nco_lmt_init(var_trv->var_dmn[idx_var_dmn].crd->lmt_msa.lmt_dmn[lmt_crr]);
          /* Store dimension ID */
          lmt[lmt_idx]->id = crd->dmn_id;
          /* Store this valid input; deep-copy */
          (void)nco_lmt_cpy(lmt[lmt_idx], var_trv->var_dmn[idx_var_dmn].crd->lmt_msa.lmt_dmn[lmt_crr]);
        }
        else {
          /* Non coordinate variable structure case */
          dmn_trv_sct *ncd = var_trv->var_dmn[idx_var_dmn].ncd;
          /* Limit is same as dimension in input file ? */
          var_trv->var_dmn[idx_var_dmn].ncd->lmt_msa.NON_HYP_DMN = False;
          /* Parse user-specified limits into hyperslab specifications. NOTE: Use False parameter and "dmn" */
          (void)nco_lmt_evl_dmn_crd(nc_id, 0L, FORTRAN_IDX_CNV, ncd->grp_nm_fll, ncd->nm, ncd->sz, ncd->is_rec_dmn, False, lmt[lmt_idx]);
          /* Current index (lmt_crr) of dimension limits for this (idx_dmn) table dimension  */
          int lmt_crr = ncd->lmt_msa.lmt_crr;
          /* Increment current index being initialized  */
          var_trv->var_dmn[idx_var_dmn].ncd->lmt_msa.lmt_crr++;
          /* Alloc this limit */
          var_trv->var_dmn[idx_var_dmn].ncd->lmt_msa.lmt_dmn[lmt_crr] = (lmt_sct *)nco_malloc(sizeof(lmt_sct));
          /* Initialize this entry */
          (void)nco_lmt_init(var_trv->var_dmn[idx_var_dmn].ncd->lmt_msa.lmt_dmn[lmt_crr]);
          /* Store dimension ID */
          lmt[lmt_idx]->id = ncd->dmn_id;
          /* Store this valid input; deep-copy to table */
          (void)nco_lmt_cpy(lmt[lmt_idx], var_trv->var_dmn[idx_var_dmn].ncd->lmt_msa.lmt_dmn[lmt_crr]);
        }
      }
    }
  }

  /* Apply MSA for each dimension in new cycle (that now has all its limits in place) */
  /* Loop step 3 */

  /* Loop variable dimensions */
  for (int idx_var_dmn = 0; idx_var_dmn < var_trv->nbr_dmn; idx_var_dmn++) {
    /* Loop input name list */
    for (int lmt_idx = 0; lmt_idx < lmt_nbr; lmt_idx++) {
      /* Match input relative name to dimension relative name */
      if (strcmp(lmt[lmt_idx]->nm, var_trv->var_dmn[idx_var_dmn].dmn_nm) == 0) {
        /* Coordinate variable structure case */
        if (var_trv->var_dmn[idx_var_dmn].crd) {
          /* Adapted from original MSA loop in nco_msa_lmt_all_ntl(); differences are marked GTT specific */
          //nco_bool flg_ovl; /* [flg] Limits overlap */
          crd_sct *crd = var_trv->var_dmn[idx_var_dmn].crd;
          /* GTT: If this coordinate has no limits, continue */
          if (crd->lmt_msa.lmt_dmn_nbr == 0) {
            continue;
          }
          /* ncra/ncrcat have only one limit for record dimension so skip evaluation otherwise this messes up multi-file operation */
          if (crd->is_rec_dmn && (nco_prg_id_get() == ncra || nco_prg_id_get() == ncrcat)) {
            continue;
          }
          /* Split-up wrapped limits. NOTE: using deep copy version nco_msa_wrp_splt_cpy() */
          (void)nco_msa_wrp_splt_cpy(&var_trv->var_dmn[idx_var_dmn].crd->lmt_msa);
          /* Wrapped hyperslabs are dimensions broken into the "wrong" order, e.g., from
          -d time,8,2 broken into -d time,8,9 -d time,0,2
          WRP flag set only when list contains dimensions split as above */
          if (var_trv->var_dmn[idx_var_dmn].crd->lmt_msa.WRP) {
            /* Find and store size of output dim */
            (void)nco_msa_clc_cnt(&var_trv->var_dmn[idx_var_dmn].crd->lmt_msa);
            continue;
          } /* End WRP flag set */
            /* Single slab---no analysis needed */
          if (var_trv->var_dmn[idx_var_dmn].crd->lmt_msa.lmt_dmn_nbr == 1) {
            (void)nco_msa_clc_cnt(&var_trv->var_dmn[idx_var_dmn].crd->lmt_msa);
            continue;
          } /* End Single slab */
            /* Does Multi-Slab Algorithm returns hyperslabs in user-specified order? */
          if (MSA_USR_RDR) {
            var_trv->var_dmn[idx_var_dmn].crd->lmt_msa.MSA_USR_RDR = True;
            /* Find and store size of output dimension */
            (void)nco_msa_clc_cnt(&var_trv->var_dmn[idx_var_dmn].crd->lmt_msa);
            continue;
          } /* End MSA_USR_RDR */
            /* Sort limits */
          (void)nco_msa_qsort_srt(&var_trv->var_dmn[idx_var_dmn].crd->lmt_msa);
          /* Check for overlap */
          //flg_ovl = nco_msa_ovl(&var_trv->var_dmn[idx_var_dmn].crd->lmt_msa);
          /* Find and store size of output dimension */
          (void)nco_msa_clc_cnt(&var_trv->var_dmn[idx_var_dmn].crd->lmt_msa);
        }
        else {
          /* Non coordinate variable structure case */
          dmn_trv_sct *ncd = var_trv->var_dmn[idx_var_dmn].ncd;
          /* Adapted from the original MSA loop in nco_msa_lmt_all_ntl(); differences are marked GTT specific */
          nco_bool flg_ovl; /* [flg] Limits overlap */
                            /* GTT: If this dimension has no limits, continue */
          if (ncd->lmt_msa.lmt_dmn_nbr == 0) {
            continue;
          }
          /* ncra/ncrcat have only one limit for record dimension so skip evaluation otherwise this messes up multi-file operation */
          if (ncd->is_rec_dmn && (nco_prg_id_get() == ncra || nco_prg_id_get() == ncrcat)) {
            continue;
          }
          /* Split-up wrapped limits */
          (void)nco_msa_wrp_splt_trv(var_trv->var_dmn[idx_var_dmn].ncd);
          /* Wrapped hyperslabs are dimensions broken into the "wrong" order,e.g. from
          -d time,8,2 broken into -d time,8,9 -d time,0,2
          WRP flag set only when list contains dimensions split as above */
          if (var_trv->var_dmn[idx_var_dmn].ncd->lmt_msa.WRP) {
            /* Find and store size of output dim */
            (void)nco_msa_clc_cnt_trv(var_trv->var_dmn[idx_var_dmn].ncd);
            continue;
          } /* End WRP flag set */
            /* Single slab---no analysis needed */
          if (var_trv->var_dmn[idx_var_dmn].ncd->lmt_msa.lmt_dmn_nbr == 1) {
            (void)nco_msa_clc_cnt_trv(var_trv->var_dmn[idx_var_dmn].ncd);
            continue;
          } /* End Single slab */
            /* Does Multi-Slab Algorithm returns hyperslabs in user-specified order ? */
          if (MSA_USR_RDR) {
            var_trv->var_dmn[idx_var_dmn].ncd->lmt_msa.MSA_USR_RDR = True;
            /* Find and store size of output dimension */
            (void)nco_msa_clc_cnt_trv(var_trv->var_dmn[idx_var_dmn].ncd);
            continue;
          } /* End MSA_USR_RDR */
            /* Sort limits */
          (void)nco_msa_qsort_srt_trv(var_trv->var_dmn[idx_var_dmn].ncd);
          /* Check for overlap */
          flg_ovl = nco_msa_ovl_trv(var_trv->var_dmn[idx_var_dmn].ncd);
          if (!flg_ovl) var_trv->var_dmn[idx_var_dmn].ncd->lmt_msa.MSA_USR_RDR = True;
          /* Find and store size of output dimension */
          (void)nco_msa_clc_cnt_trv(var_trv->var_dmn[idx_var_dmn].ncd);
        }
      }
    }
  }

} /* end nco_bld_lmt_var() */

void 
nco_msa_var_get_rec_trv             /* [fnc] Read a user-defined limit */
(const int nc_id,                   /* I [ID] netCDF file ID */
 var_sct *var_prc,                  /* I/O [sct] Variable */
 const char * const rec_nm_fll,     /* I [sng] Record dimension name */
 const long idx_rec_crr_in,         /* [idx] Index of current record in current input file */
 const trv_tbl_sct * const trv_tbl) /* I [sct] GTT (Group Traversal Table) */
{
  /* Routine acts as a "shim" for nco_msa_var_get_trv()
     nco_msa_var_get_trv() typically reads entire variable as hyperslabbed by user
     Yet ncra and ncrcat require only one record at a time, and that record changes
     nco_msa_var_get_rec_trv() defines a temporary, artificial MSA limit that corresponds to desired single record
     It then calls nco_msa_var_get_trv() with this limit structure */

  int lmt_dmn_nbr;

  nco_bool flg_lmt=False; /* [flg] Allocate custom limit */

  trv_sct *var_trv;

  /* Obtain variable GTT object using full variable name */
  var_trv=trv_tbl_var_nm_fll(var_prc->nm_fll,trv_tbl);

  for(int idx_dmn=0;idx_dmn<var_trv->nbr_dmn;idx_dmn++){

    /* Match current record by name since ID may differ for records across files */
    if(!strcmp(var_trv->var_dmn[idx_dmn].dmn_nm_fll,rec_nm_fll)){

      if(var_trv->var_dmn[idx_dmn].crd){
	/* Dimension is coordinate */
        lmt_dmn_nbr=var_trv->var_dmn[idx_dmn].crd->lmt_msa.lmt_dmn_nbr;

        /* Case of previously existing limits */
        if(lmt_dmn_nbr > 0){
          for(int idx_lmt=0;idx_lmt<lmt_dmn_nbr;idx_lmt++){
            /* Set start, count, stride to match current record */
            var_trv->var_dmn[idx_dmn].crd->lmt_msa.lmt_dmn[idx_lmt]->srt=idx_rec_crr_in;
            var_trv->var_dmn[idx_dmn].crd->lmt_msa.lmt_dmn[idx_lmt]->end=idx_rec_crr_in;
            var_trv->var_dmn[idx_dmn].crd->lmt_msa.lmt_dmn[idx_lmt]->cnt=1L;
            var_trv->var_dmn[idx_dmn].crd->lmt_msa.lmt_dmn[idx_lmt]->srd=1L;
          } /* !idx_lmt */
        }else{ /* lmt_dmn_nbr == 0 */
          flg_lmt=True;
          /* Alloc dummy limit */
          var_trv->var_dmn[idx_dmn].crd->lmt_msa.lmt_dmn_nbr=1;
          var_trv->var_dmn[idx_dmn].crd->lmt_msa.lmt_dmn=(lmt_sct **)nco_malloc(sizeof(lmt_sct *));
	  /* fxm: 20140918 TODO nco1114 subscript [0] fails unless record is first dimension, generalize with [rec_dmn_idx] */
          var_trv->var_dmn[idx_dmn].crd->lmt_msa.lmt_dmn[0]=(lmt_sct *)nco_malloc(sizeof(lmt_sct));
          /* Initialize NULL/invalid */
          (void)nco_lmt_init(var_trv->var_dmn[idx_dmn].crd->lmt_msa.lmt_dmn[0]);
	  /* Set start, count, stride to match current record */
	  /* fxm: 20140918 TODO nco1114 subscript [0] fails unless record is first dimension, generalize with [rec_dmn_idx] */
          var_trv->var_dmn[idx_dmn].crd->lmt_msa.lmt_dmn[0]->srt=idx_rec_crr_in;
          var_trv->var_dmn[idx_dmn].crd->lmt_msa.lmt_dmn[0]->end=idx_rec_crr_in;
          var_trv->var_dmn[idx_dmn].crd->lmt_msa.lmt_dmn[0]->cnt=1L;
          var_trv->var_dmn[idx_dmn].crd->lmt_msa.lmt_dmn[0]->srd=1L;
          var_trv->var_dmn[idx_dmn].crd->lmt_msa.lmt_dmn[0]->nm=strdup("record_limit");
        } /* lmt_dmn_nbr == 0 */
      }else{ /* Dimension is not coordinate */
        assert(!var_trv->var_dmn[idx_dmn].is_crd_var);
        lmt_dmn_nbr=var_trv->var_dmn[idx_dmn].ncd->lmt_msa.lmt_dmn_nbr;

        if(lmt_dmn_nbr > 0){
          for(int idx_lmt=0;idx_lmt<lmt_dmn_nbr;idx_lmt++){
	    /* Set start, count, stride to match current record */
            var_trv->var_dmn[idx_dmn].ncd->lmt_msa.lmt_dmn[idx_lmt]->srt=idx_rec_crr_in;
            var_trv->var_dmn[idx_dmn].ncd->lmt_msa.lmt_dmn[idx_lmt]->end=idx_rec_crr_in;
            var_trv->var_dmn[idx_dmn].ncd->lmt_msa.lmt_dmn[idx_lmt]->cnt=1L;
            var_trv->var_dmn[idx_dmn].ncd->lmt_msa.lmt_dmn[idx_lmt]->srd=1L;
          } /* !idx_lmt */
        }else{ /* lmt_dmn_nbr == 0 */
          flg_lmt=True;
          /* Allocate dummy limit */
          var_trv->var_dmn[idx_dmn].ncd->lmt_msa.lmt_dmn_nbr=1;
          var_trv->var_dmn[idx_dmn].ncd->lmt_msa.lmt_dmn=(lmt_sct **)nco_malloc(1*sizeof(lmt_sct *));
	  /* fxm: 20140918 TODO nco1114 subscript [0] fails unless record is first dimension, generalize with [rec_dmn_idx] */
          var_trv->var_dmn[idx_dmn].ncd->lmt_msa.lmt_dmn[0]=(lmt_sct *)nco_malloc(sizeof(lmt_sct));
          /* Initialize NULL/invalid */
          (void)nco_lmt_init(var_trv->var_dmn[idx_dmn].ncd->lmt_msa.lmt_dmn[0]);
	  /* Set start, count, stride to match current record */
	  /* fxm: 20140918 TODO nco1114 subscript [0] fails unless record is first dimension, generalize with [rec_dmn_idx] */
          var_trv->var_dmn[idx_dmn].ncd->lmt_msa.lmt_dmn[0]->srt=idx_rec_crr_in;
          var_trv->var_dmn[idx_dmn].ncd->lmt_msa.lmt_dmn[0]->end=idx_rec_crr_in;
          var_trv->var_dmn[idx_dmn].ncd->lmt_msa.lmt_dmn[0]->cnt=1L;
          var_trv->var_dmn[idx_dmn].ncd->lmt_msa.lmt_dmn[0]->srd=1L;
          var_trv->var_dmn[idx_dmn].ncd->lmt_msa.lmt_dmn[0]->nm=strdup("record_limit");
        } /* lmt_dmn_nbr == 0 */
      } /* end Dimension is not coordinate */
      break;
    } /* end record dimension */
  } /* end loop over dimensions */

  /* Retrieve variable from disk into memory */
  (void)nco_msa_var_get_trv(nc_id,var_prc,trv_tbl);

  /* Free artificial limit and reset number of limits */
  for(int idx_dmn=0;idx_dmn<var_trv->nbr_dmn;idx_dmn++){
    /* Match current record by name since ID may differ for records across files */
    if(!strcmp(var_trv->var_dmn[idx_dmn].dmn_nm_fll,rec_nm_fll)){
      /* Custom limit */
      if(flg_lmt){
        if(var_trv->var_dmn[idx_dmn].is_crd_var){
	  /* Dimension is coordinate */
          var_trv->var_dmn[idx_dmn].crd->lmt_msa.lmt_dmn_nbr=0;
	  /* fxm: 20140918 TODO nco1114 subscript [0] fails unless record is first dimension, generalize with [rec_dmn_idx] */
          var_trv->var_dmn[idx_dmn].crd->lmt_msa.lmt_dmn[0]=(lmt_sct *)nco_lmt_free(var_trv->var_dmn[idx_dmn].crd->lmt_msa.lmt_dmn[0]);
          var_trv->var_dmn[idx_dmn].crd->lmt_msa.lmt_dmn=(lmt_sct **)nco_free(var_trv->var_dmn[idx_dmn].crd->lmt_msa.lmt_dmn);
        }else if(!var_trv->var_dmn[idx_dmn].is_crd_var){
	  /* Dimension is not coordinate */
          var_trv->var_dmn[idx_dmn].ncd->lmt_msa.lmt_dmn_nbr=0;
	  /* fxm: 20140918 TODO nco1114 subscript [0] fails unless record is first dimension, generalize with [rec_dmn_idx] */
          var_trv->var_dmn[idx_dmn].ncd->lmt_msa.lmt_dmn[0]=(lmt_sct *)nco_lmt_free(var_trv->var_dmn[idx_dmn].ncd->lmt_msa.lmt_dmn[0]);
          var_trv->var_dmn[idx_dmn].ncd->lmt_msa.lmt_dmn=(lmt_sct **)nco_free(var_trv->var_dmn[idx_dmn].ncd->lmt_msa.lmt_dmn);
        } /* Dimension is not coordinate */
        break;
      } /* Custom limit */
    } /* end record dimension */
  } /* end loop over dimensions */

} /* end nco_msa_var_get_rec_trv() */

nco_bool                             /* O [flg] Skip variable  */
nco_skp_var                          /* [fnc] Skip variable while doing record   */
(const var_sct * const var_prc,      /* I [sct] Processed variable */
 const char * const rec_nm_fll,      /* I [sng] Full name of record being done in loop  */
 const trv_tbl_sct * const trv_tbl)  /* I [sct] Traversal table */
{
  nco_bool flg_skp;     /* [flg] Skip variable  */

  dmn_trv_sct *dmn_trv; /* [sct] GTT dimension structure */

  /* Variable must contain one record */
  assert(var_prc->is_rec_var);

  flg_skp=False;

  /* Loop dimensions */
  for(int idx_dmn=0;idx_dmn<var_prc->nbr_dim;idx_dmn++){

    /* Is this the record dimension ? */
    if(var_prc->dim[idx_dmn]->is_rec_dmn){

      /* Get unique dimension object from unique dimension ID, in input list (NB: this is needed because dmn_sct does not have name full) */
      dmn_trv=nco_dmn_trv_sct(var_prc->dim[idx_dmn]->id,trv_tbl);

      /* And it is not the same as the input record dimension name currently being done then skip it */
      if(strcmp(dmn_trv->nm_fll,rec_nm_fll)) flg_skp=True;

    } /* Is this the record dimension */
  } /* Loop dimensions */

  return flg_skp;

} /* nco_skp_var() */

var_sct *                             /* O [sct] Variable (weight or mask) */
nco_var_get_wgt_trv                   /* [fnc] Retrieve weighting or mask variable */
(const int nc_id,                     /* I [id] netCDF file ID */
  const int lmt_nbr,                   /* I [nbr] number of dimensions with limits */
  CST_X_PTR_CST_PTR_CST_Y(char, lmt_arg), /* I [sng] List of user-specified dimension limits */
  nco_bool MSA_USR_RDR,                /* I [flg] Multi-Slab Algorithm returns hyperslabs in user-specified order */
  nco_bool FORTRAN_IDX_CNV,            /* I [flg] Hyperslab indices obey Fortran convention */
  const char * const wgt_nm,           /* I [sng] Weight or mask variable name (relative or absolute) */
  const var_sct * const var,           /* I [sct] Variable that needs weight/mask */
  const trv_tbl_sct * const trv_tbl)   /* I [lst] Traversal table */
{
  /* Purpose: Return weight or mask variable closest in scope to specified variable */

  int grp_id; /* [ID] Group ID */
  int var_id; /* [ID] Variable ID */
  int idx_wgt; /* [nbr] Weight array counter */
  var_sct *wgt_var; /* O [sct] Variable (weight/mask) */

  /* 201707015 pvn nco1138. Detect the cases where
  a) a variable <var> was requested with -v <var>
  b) an hyperslab was requested on the dimensions of the variable, with -d <dim>
  c) a mask <wgt_var> was requested on a variable that is *not* <var> in the -v list
  The consequence of this case is that <wgt_var> was not hyperslabled in the table traversal
  Use case:
  ncwa -O -C -y ttl -v orog2 -d lat,0.,90. -m lat -M 0.0 -T gt ~/nco/data/in.nc ~/foo.nc
  ncks -H -v orog ~/foo.nc # Correct answer is 4
  For this case define here the hyperslab for <wgt_var> from the original input
  user name list 'lmt_arg'
  This detection uses a new function nco_msa_var_get_sct()
  that is the same as nco_msa_var_get_trv() but with input 'var_trv' '
  TODO: Deprecate nco_msa_var_get_trv() and use this new function nco_msa_var_get_sct()
  because some code is repeated
  NB: this detection is done below for both cases of absolute and relative weight path
  */

  /* If first character is '/' then weight name is absolute path */
  if (wgt_nm[0] == '/') {
    /* Absolute name given for weight. Straightforward extract and copy */
    trv_sct *wgt_trv;
    wgt_trv = trv_tbl_var_nm_fll(wgt_nm, trv_tbl);
    (void)nco_inq_grp_full_ncid(nc_id, wgt_trv->grp_nm_fll, &grp_id);
    (void)nco_inq_varid(grp_id, wgt_trv->nm, &var_id);
    /* Case of input limits, reconstruct limits and hyperslab */
    if (lmt_nbr) {
      lmt_sct **lmt = NULL_CEWI;  /* [sct] User defined limits */
      lmt = nco_lmt_prs(lmt_nbr, lmt_arg);
      nco_bld_lmt_var(nc_id, MSA_USR_RDR, lmt_nbr, lmt, FORTRAN_IDX_CNV, wgt_trv);
      /* Transfer from table to local variable */
      wgt_var = nco_var_fll_trv(grp_id, var_id, wgt_trv, trv_tbl);
      /* Assign the hyperslab information for a variable 'var_sct'  from the obtained GTT variable */
      /* Similar to nco_msa_var_get_trv() but just with one input GTT variable */
      (void)nco_msa_var_get_sct(nc_id, wgt_var, wgt_trv);
      lmt = nco_lmt_lst_free(lmt, lmt_nbr);
    }
    else {
      /* Transfer from table to local variable */
      wgt_var = nco_var_fll_trv(grp_id, var_id, wgt_trv, trv_tbl);
      /* Retrieve variable NB: use GTT version, that "knows" all limits */
      (void)nco_msa_var_get_trv(nc_id, wgt_var, trv_tbl);
    }
    return wgt_var;
  }
  else {
    /* Relative name given for weight. Must identify most-in-scope match... */
    int wgt_nbr = 0; /* [nbr] Number of weight/mask variables in file */
    trv_sct **wgt_tbl = NULL; /* [sct] Weight/mask list */

    /* Count matching weight names in order to allocate space */
    for (unsigned tbl_idx = 0; tbl_idx < trv_tbl->nbr; tbl_idx++)
      if (trv_tbl->lst[tbl_idx].nco_typ == nco_obj_typ_var && (!strcmp(trv_tbl->lst[tbl_idx].nm, wgt_nm))) wgt_nbr++;

    /* Fill-in variable structure list for all weights */
    wgt_tbl = (trv_sct **)nco_malloc(wgt_nbr * sizeof(trv_sct *));
    idx_wgt = 0;

    /* Create list of potential weight structures */
    for (unsigned tbl_idx = 0; tbl_idx < trv_tbl->nbr; tbl_idx++) {
      if (trv_tbl->lst[tbl_idx].nco_typ == nco_obj_typ_var && !strcmp(trv_tbl->lst[tbl_idx].nm, wgt_nm)) {
        wgt_tbl[idx_wgt] = &trv_tbl->lst[tbl_idx];
        idx_wgt++;
      } /* endif */
    } /* !tbl_idx */

    for (unsigned idx_var = 0; idx_var < trv_tbl->nbr; idx_var++) {
      /* Find variable that needs weight/mask */
      if (trv_tbl->lst[idx_var].nco_typ == nco_obj_typ_var
        && trv_tbl->lst[idx_var].flg_xtr
        && !strcmp(trv_tbl->lst[idx_var].nm_fll, var->nm_fll)) {
        trv_sct var_trv = trv_tbl->lst[idx_var];

        /* 20150711: This is buggy, at best it returns last weight found, not closest-in-scope */
        /* 20170620: Broken because it requires that weight and variable be in same group */
        /* Which weight is closest-in-scope to variable? */
        for (idx_wgt = 0; idx_wgt < wgt_nbr; idx_wgt++) {
          trv_sct *wgt_trv = wgt_tbl[idx_wgt];
          /* 20170620: Change from strcmp() to strstr() so weight can be in any ancestor group
          This still does NOT have the desired behavior of selecting the _closest-in-scope_,
          but at least it allows weights to be in ancestor groups */
          if (strstr(wgt_trv->grp_nm_fll, var_trv.grp_nm_fll)) {
            (void)nco_inq_grp_full_ncid(nc_id, wgt_trv->grp_nm_fll, &grp_id);
            (void)nco_inq_varid(grp_id, wgt_trv->nm, &var_id);

            /* Case of input limits, reconstruct limits and hyperslab */
            if (lmt_nbr) {
              lmt_sct **lmt = NULL_CEWI;  /* [sct] User defined limits */
              lmt = nco_lmt_prs(lmt_nbr,lmt_arg);
              nco_bld_lmt_var(nc_id,MSA_USR_RDR,lmt_nbr,lmt,FORTRAN_IDX_CNV,wgt_trv);
              /* Transfer from table to local variable */
              wgt_var = nco_var_fll_trv(grp_id,var_id,wgt_trv,trv_tbl);
              /* Assign the hyperslab information for a variable 'var_sct'  from the obtained GTT variable */
              /* Similar to nco_msa_var_get_trv() but just with one input GTT variable */
              (void)nco_msa_var_get_sct(nc_id,wgt_var,wgt_trv);
              lmt = nco_lmt_lst_free(lmt,lmt_nbr);
            }
            else {
              /* Retrieve hyperslabs from table */
              /* Transfer from table to local variable */
              wgt_var = nco_var_fll_trv(grp_id,var_id,wgt_trv,trv_tbl);
              /* Retrieve variable NB: use GTT version, that "knows" all limits */
              (void)nco_msa_var_get_trv(nc_id,wgt_var,trv_tbl);
            }

            wgt_tbl = (trv_sct **)nco_free(wgt_tbl);
            return wgt_var;
          } /* !strcmp() */
        } /* !idx_wgt */
      } /* !var */
    } /* !idx_var */
  } /* !Relative name */

  /* If function has not yet returned, then variable was not found */
  (void)fprintf(stdout, "%s: ERROR nco_var_get_wgt_trv() reports unable to find specified weight or mask variable \"%s\"\n", nco_prg_nm_get(), wgt_nm);
  nco_exit(EXIT_FAILURE);

  return NULL;
} /* nco_var_get_wgt_trv() */

void                                    
nco_grp_var_lst                        /* [fnc] Export list of variable names for group */
(const int nc_id,                      /* I [id] netCDF file ID */
 const char * const grp_nm_fll,        /* I [sng] Absolute group name */
 char ***nm_lst,                       /* I/O [sng] List of names (relative) */
 int *nm_lst_nbr)                      /* I/O [nbr] Number of items in list */
{
  /* Purpose: Export list of variable names for group */

  char var_nm[NC_MAX_NAME+1L];      /* [sng] Variable name */ 

  int nbr_var;                     /* [nbr] Number of variables */
  int grp_id;                      /* [id] Group ID */

  /* Get group ID */
  (void)nco_inq_grp_full_ncid(nc_id,grp_nm_fll,&grp_id);
  /* Obtain number of variable for group */
  (void)nco_inq(grp_id,(int *)NULL,&nbr_var,(int *)NULL,(int *)NULL);

  /* Allocate list */ 
  (*nm_lst)=(char **)nco_malloc(nbr_var*sizeof(char *));

  /* Iterate variables for this group */
  for(int idx_var=0;idx_var<nbr_var;idx_var++){

    /* Get variable name */
    (void)nco_inq_var(grp_id,idx_var,var_nm,(nc_type *)NULL,(int *)NULL,(int *)NULL,(int *)NULL);

    /* Add to list */ 
    (*nm_lst)[idx_var]=(char *)strdup(var_nm);

  } /* Iterate variables for this group */

  *nm_lst_nbr=nbr_var;

} /* end nco_grp_var_lst() */

char * /* O [sng] Name of variable   */
nco_var_has_cf /* [fnc] Variable has CF-compliant attributes ("ancillary_variables", "bounds", "climatology", "coordinates" and "grid_mapping") */
(const int nc_id, /* I [ID] netCDF file ID */
 const trv_sct * const var_trv, /* I [sct] Variable (object) */
 const char * const cf_nm, /* I [sng] CF convention ("ancillary_variables", "bounds", "climatology", "coordinates", and "grid_mapping") */
 nco_bool *flg_cf_fnd) /* I/O [flg] CF variable was found */
{
  /* Detect associated variables specified by CF "ancillary_variables", "bounds", "climatology", "coordinates", and "grid_mapping" conventions
     http://cf-pcmdi.llnl.gov/documents/cf-conventions/1.1/cf-conventions.html#coordinate-system */ 

  const char dlm_sng[]=" "; /* [sng] Delimiter string */
  const char fnc_nm[]="nco_var_has_cf()"; /* [sng] Function name */

  char **cf_lst; /* [sng] 1D array of list elements */
  char att_nm[NC_MAX_NAME+1L]; /* [sng] Attribute name */

  int grp_id; /* [id] Group ID */
  int nbr_att; /* [nbr] Number of attributes */
  int nbr_cf; /* [nbr] Number of coordinates specified in "bounds" or "coordinates" attributes */
  int var_id; /* [id] Variable ID */

  assert(var_trv->nco_typ == nco_obj_typ_var);

  /* Initialize return value */
  *flg_cf_fnd=False;

  /* Obtain group ID */
  (void)nco_inq_grp_full_ncid(nc_id,var_trv->grp_nm_fll,&grp_id);

  /* Obtain variable ID */
  (void)nco_inq_varid(grp_id,var_trv->nm,&var_id);

  /* Find number of attributes */
  (void)nco_inq_varnatts(grp_id,var_id,&nbr_att);

  assert(nbr_att == var_trv->nbr_att);

  /* Loop attributes */
  for(int idx_att=0;idx_att<nbr_att;idx_att++){

    /* Get attribute name */
    (void)nco_inq_attname(grp_id,var_id,idx_att,att_nm);

    /* Is attribute part of CF convention? */
    if(!strcmp(att_nm,cf_nm)){
      char *att_val;
      long att_sz;
      nc_type att_typ;

      *flg_cf_fnd=True;

      /* Yes, get list of specified attributes */
      (void)nco_inq_att(grp_id,var_id,att_nm,&att_typ,&att_sz);
      if(att_typ != NC_CHAR){
        (void)fprintf(stderr,"%s: WARNING \"%s\" attribute for variable %s is type %s, not %s. This violates the CF convention for allowed datatypes (http://cfconventions.org/cf-conventions/cf-conventions.html#_data_types). Therefore %s will skip this attribute.\n",nco_prg_nm_get(),att_nm,var_trv->nm_fll,nco_typ_sng(att_typ),nco_typ_sng(NC_CHAR),fnc_nm);
        return NULL;
      } /* end if */
      att_val=(char *)nco_malloc((att_sz+1L)*sizeof(char));
      if(att_sz > 0L) (void)nco_get_att(grp_id,var_id,att_nm,(void *)att_val,NC_CHAR);	  
      /* NUL-terminate attribute */
      att_val[att_sz]='\0';

      /* Split list into separate coordinate names
	 Use nco_lst_prs_sgl_2D() not nco_lst_prs_2D() to avert TODO nco944 */
      cf_lst=nco_lst_prs_sgl_2D(att_val,dlm_sng,&nbr_cf);
      /* ...for each associated variable in CF convention attribute, i.e., "ancillary_variables", "bounds", "climatology", "coordinates", and "grid_mapping", ... */
      for(int idx_cf=0;idx_cf<nbr_cf;idx_cf++){
        char *cf_lst_var=cf_lst[idx_cf];
        if(!cf_lst_var) continue;
      } /* end loop over idx_cf */

      /* Return cf_lst_var, associated name (e.g., "lat_bounds"). NB: Assumption only one associated name */
      char *cf_lst_var=strdup(cf_lst[0]);

      /* Free allocated memory */
      att_val=(char *)nco_free(att_val);
      cf_lst=nco_sng_lst_free(cf_lst,nbr_cf);

      return cf_lst_var;
    } /* end strcmp() */
  } /* end loop over attributes */

  return NULL;

} /* nco_var_has_cf() */

int                                    /* O [enm] Comparison result [<,=,>] 0 iff val_1 [<,==,>] val_2 */
nco_cmp_aux_crd_dpt                    /* [fnc] Compare two aux_crd_sct's by group depth */
(const void *val_1,                    /* I [sct] aux_crd_sct * to compare */
 const void *val_2)                    /* I [sct] aux_crd_sct * to compare */
{
  /* Purpose: Compare two aux_crd_sct's by group depth structure member
     Function is suitable for argument to ANSI C qsort() routine in stdlib.h */

  const aux_crd_sct * const crd_1=(const aux_crd_sct *)val_1;
  const aux_crd_sct * const crd_2=(const aux_crd_sct *)val_2;

  if(crd_1->grp_dpt > crd_2->grp_dpt) return 1;
  else if(crd_1->grp_dpt < crd_2->grp_dpt) return -1;
  else return 0;

} /* nco_cmp_aux_crd_dpt() */

void
nco_prs_aux_crd                       /* [fnc] Parse auxiliary coordinates */
(const int nc_id,                     /* I [ID] netCDF file ID */
 const int aux_nbr,                   /* I [nbr] Number of auxiliary coordinates */
 char *aux_arg[],                     /* I [sng] Auxiliary coordinates */
 const nco_bool FORTRAN_IDX_CNV,      /* I [flg] Hyperslab indices obey Fortran convention */
 const nco_bool MSA_USR_RDR,          /* I [flg] Multi-Slab Algorithm returns hyperslabs in user-specified order */
 const nco_bool EXTRACT_ASSOCIATED_COORDINATES,  /* I [flg] Extract all coordinates associated with extracted variables? */
 trv_tbl_sct * const trv_tbl)         /* I/O [sct] GTT (Group Traversal Table) */
{
  const char fnc_nm[]="nco_prs_aux_crd()"; /* [sng] Function name */

  for(unsigned idx_tbl=0;idx_tbl<trv_tbl->nbr;idx_tbl++){

    /* Search extracted variables */ 
    if(trv_tbl->lst[idx_tbl].nco_typ == nco_obj_typ_var && trv_tbl->lst[idx_tbl].flg_xtr){

      trv_sct var_trv=trv_tbl->lst[idx_tbl];

      /* Search variables with auxiliary coordinates */ 
      if(var_trv.flg_aux){

        if(nco_dbg_lvl_get() >= nco_dbg_dev) (void)fprintf(stdout,"%s: DEBUG %s reports variable with auxiliary coordinates %s\n",nco_prg_nm_get(),fnc_nm,trv_tbl->lst[idx_tbl].nm_fll);

        int dmn_idx_fnd=-1; /* [nbr] Index of dimension that has the coordinate */
        int dmn_id_fnd_lat=-1; /* [id] ID of dimension that has the latitude coordinate */
        int dmn_id_fnd_lon=-1; /* [id] ID of dimension that has the longituee coordinate */

        trv_sct *lat_trv=NULL;
        trv_sct *lon_trv=NULL;      

        /* Look for latitude dimension */
        for(int idx_dmn=0;idx_dmn<var_trv.nbr_dmn;idx_dmn++){
          if(var_trv.var_dmn[idx_dmn].nbr_lat_crd){
            /* Use coordinate with lower group depth (index 0) (These were already sorted) */
            lat_trv=trv_tbl_var_nm_fll(var_trv.var_dmn[idx_dmn].lat_crd[0].nm_fll,trv_tbl);
            dmn_idx_fnd=idx_dmn;
            dmn_id_fnd_lat=var_trv.var_dmn[idx_dmn].lat_crd[0].dmn_id;
            break;
          } /* !nbr_lat_crd */
        } /* !idx_dmn */

        /* Look for longitude dimension */
        for(int idx_dmn=0;idx_dmn<var_trv.nbr_dmn;idx_dmn++){
          if(var_trv.var_dmn[idx_dmn].nbr_lon_crd){
            /* Use coordinate with lower group depth (index 0) (These were already sorted) */
            lon_trv=trv_tbl_var_nm_fll(var_trv.var_dmn[idx_dmn].lon_crd[0].nm_fll,trv_tbl);
            dmn_idx_fnd=idx_dmn;
            dmn_id_fnd_lon=var_trv.var_dmn[idx_dmn].lon_crd[0].dmn_id;
            break;
          } /* !nbr_lon_crd */
        } /* !idx_dmn */

        /* Auxiliary coordinates found */
        if(lat_trv && lon_trv){

          lmt_sct **aux=NULL_CEWI;   /* Auxiliary coordinate limits */
          int aux_lmt_nbr;           /* Number of auxiliary coordinate limits */
          nc_type crd_typ;           /* [enm] netCDF type of both "latitude" and "longitude" */
          char units[NC_MAX_NAME+1L];

          aux_lmt_nbr=0;
          crd_typ=trv_tbl->lst[idx_tbl].var_dmn[dmn_idx_fnd].lat_crd[0].crd_typ;
          strcpy(units,trv_tbl->lst[idx_tbl].var_dmn[dmn_idx_fnd].lat_crd[0].units);

          aux=nco_aux_evl_trv(nc_id,aux_nbr,aux_arg,lat_trv,lon_trv,crd_typ,units,&aux_lmt_nbr);

          /* Mark both 'latitude' and 'longitude' for extraction */
          if(EXTRACT_ASSOCIATED_COORDINATES){
            (void)trv_tbl_mrk_xtr(lat_trv->nm_fll,True,trv_tbl);
            (void)trv_tbl_mrk_xtr(lon_trv->nm_fll,True,trv_tbl);
          } /* !EXTRACT_ASSOCIATED_COORDINATES */

          /* Found limits */
          if(aux_lmt_nbr > 0){
            if(nco_dbg_lvl_get() >= nco_dbg_dev) (void)fprintf(stdout,"%s: DEBUG %s variable <%s> (%d) limits\n",nco_prg_nm_get(),fnc_nm,trv_tbl->lst[idx_tbl].nm_fll,aux_lmt_nbr); 

            lmt_sct **lmt=aux;
            int lmt_dmn_nbr=aux_lmt_nbr;

            /* Dimension IDs of both 'latitude' and 'longitude' must refer to same dimemsion (e.g., 'gds_crd') */
            assert(dmn_id_fnd_lon == dmn_id_fnd_lat);
	    CEWI_unused(dmn_id_fnd_lon);
	    
            /* Apply limits to variable in table */
            (void)nco_lmt_aux_tbl(nc_id,lmt,lmt_dmn_nbr,var_trv.nm_fll,dmn_id_fnd_lat,FORTRAN_IDX_CNV,MSA_USR_RDR,trv_tbl);

            /* Apply limits to *all* 'latitude', 'longitude' variables that share same ID */
            (void)nco_lmt_std_att_lat_lon(nc_id,lmt,lmt_dmn_nbr,dmn_id_fnd_lat,FORTRAN_IDX_CNV,MSA_USR_RDR,trv_tbl);   

            /* Get unique dimension object from unique dimension ID (e.g., 'gds_crd') */
            dmn_trv_sct *dmn_trv=nco_dmn_trv_sct(dmn_id_fnd_lat,trv_tbl);

            /* Dimension IDs of both 'latitude' and 'longitude' must refer to same dimemsion (e.g., gds_crd) */
            assert(dmn_id_fnd_lon == dmn_trv->dmn_id);

            /* Apply limits to coordinate (e.g., gds_crd) */
            (void)nco_lmt_aux_tbl(nc_id,lmt,lmt_dmn_nbr,dmn_trv->nm_fll,dmn_id_fnd_lat,FORTRAN_IDX_CNV,MSA_USR_RDR,trv_tbl);

            if(nco_dbg_lvl_get() == nco_dbg_old){
              for(int idx_lmt=0;idx_lmt<lmt_dmn_nbr;idx_lmt++){
                (void)fprintf(stdout,"\nlimit index %d\n",idx_lmt);
                nco_lmt_prn(lmt[idx_lmt]);
              }
            }
          } /* !aux_lmt_nbr */

          /* Free limits exported from nco_aux_evl_trv() */
          aux=(lmt_sct **)nco_free(aux);  

        } /* lat_trv && lon_trv */
      } /* !flg_aux */ 
    } /* !flg_xtr */ 
  } /* !idx_tbl */

  return;
} /* nco_prs_aux_crd() */

void
nco_lmt_aux_tbl                       /* [fnc] Apply limits to variable in table */
(const int nc_id,                     /* I [ID] netCDF file ID */
 lmt_sct **lmt,                       /* I [sct] Limits */
 const int nbr_lmt,                   /* I [nbr] Number of limits */
 const char * const var_nm_fll,       /* I [sng] Variable full name */
 const int dmn_id,                    /* I [id] ID of dimension to apply the limits */
 nco_bool FORTRAN_IDX_CNV,            /* I [flg] Hyperslab indices obey Fortran convention */
 nco_bool MSA_USR_RDR,                /* I [flg] Multi-Slab Algorithm returns hyperslabs in user-specified order */
 trv_tbl_sct * const trv_tbl)         /* I/O [sct] GTT (Group Traversal Table) */
{
  for(unsigned idx_tbl=0;idx_tbl<trv_tbl->nbr;idx_tbl++){
    /* Match variable */
    if(trv_tbl->lst[idx_tbl].nco_typ == nco_obj_typ_var && !strcmp(var_nm_fll,trv_tbl->lst[idx_tbl].nm_fll)){
      trv_sct var_trv=trv_tbl->lst[idx_tbl];
      for(int idx_dmn=0;idx_dmn<var_trv.nbr_dmn;idx_dmn++)
        if(dmn_id == var_trv.var_dmn[idx_dmn].dmn_id)
          (void)nco_lmt_aux(nc_id,lmt,nbr_lmt,FORTRAN_IDX_CNV,MSA_USR_RDR,idx_tbl,idx_dmn,trv_tbl);    
    } /* !strcmp() */
  } /* !idx_tbl */

  return;
} /* nco_lmt_aux_tbl() */

void
nco_lmt_std_att_lat_lon               /* [fnc] Apply limits to variable in table that contains 'standard_name' attribute "latitude" */
(const int nc_id,                     /* I [ID] netCDF file ID */
 lmt_sct **lmt,                       /* I [sct] Limits */
 const int nbr_nbr,                   /* I [nbr] Number of limits */
 const int dmn_id,                    /* I [id] ID of dimension to apply the limits */
 nco_bool FORTRAN_IDX_CNV,            /* I [flg] Hyperslab indices obey Fortran convention */
 nco_bool MSA_USR_RDR,                /* I [flg] Multi-Slab Algorithm returns hyperslabs in user-specified order */
 trv_tbl_sct * const trv_tbl)         /* I/O [sct] GTT (Group Traversal Table) */
{
  /* Loop table  */
  for(unsigned idx_tbl=0;idx_tbl<trv_tbl->nbr;idx_tbl++){
    /* Match variable that has 'standard_name' flag */
    if(trv_tbl->lst[idx_tbl].nco_typ == nco_obj_typ_var && (trv_tbl->lst[idx_tbl].flg_std_att_lat || trv_tbl->lst[idx_tbl].flg_std_att_lon)){
        trv_sct var_trv=trv_tbl->lst[idx_tbl];
        for(int idx_dmn=0;idx_dmn<var_trv.nbr_dmn;idx_dmn++)
          if(dmn_id == var_trv.var_dmn[idx_dmn].dmn_id)
	    (void)nco_lmt_aux(nc_id,lmt,nbr_nbr,FORTRAN_IDX_CNV,MSA_USR_RDR,idx_tbl,idx_dmn,trv_tbl);    
    } /* Match variable  */
  } /* Loop table  */

  return;
} /* nco_lmt_std_att_lat_lon() */

void
nco_lmt_aux                           /* [fnc] Apply auxiliary -X limits (Auxiliary function called by different functions ) */
(const int nc_id,                     /* I [ID] netCDF file ID */
 lmt_sct **lmt,                       /* I [sct] Limits */
 const int nbr_nbr,                   /* I [nbr] Number of limits */
 nco_bool FORTRAN_IDX_CNV,            /* I [flg] Hyperslab indices obey Fortran convention */
 nco_bool MSA_USR_RDR,                /* I [flg] Multi-Slab Algorithm returns hyperslabs in user-specified order */
 const int idx_tbl,                   /* I [nbr] Table index */
 const int idx_dmn,                   /* I [nbr] Dimension index */
 trv_tbl_sct * const trv_tbl)         /* I/O [sct] GTT (Group Traversal Table) */
{
  /* a) case where the dimension has coordinate variables */
  if(trv_tbl->lst[idx_tbl].var_dmn[idx_dmn].crd){

    /* For this call (-X) the *same* limits are applied to all coordinates, and other variables might apply
       them too, so make sure they are applied only once by setting -1 in lmt_crr */

    if (trv_tbl->lst[idx_tbl].var_dmn[idx_dmn].crd->lmt_msa.lmt_crr == -1) return;

    trv_tbl->lst[idx_tbl].var_dmn[idx_dmn].crd->lmt_msa.lmt_crr=-1;

    /* Loop limits */
    for(int idx_lmt=0;idx_lmt<nbr_nbr;idx_lmt++){

      /* Increment number of dimension limits for this dimension */
      trv_tbl->lst[idx_tbl].var_dmn[idx_dmn].crd->lmt_msa.lmt_dmn_nbr++;

      int nbr_lmt=trv_tbl->lst[idx_tbl].var_dmn[idx_dmn].crd->lmt_msa.lmt_dmn_nbr;
      trv_tbl->lst[idx_tbl].var_dmn[idx_dmn].crd->lmt_msa.lmt_dmn=(lmt_sct **)nco_realloc(trv_tbl->lst[idx_tbl].var_dmn[idx_dmn].crd->lmt_msa.lmt_dmn,nbr_lmt*sizeof(lmt_sct *));

      crd_sct *crd=trv_tbl->lst[idx_tbl].var_dmn[idx_dmn].crd;

      /* Limit is same as dimension in input file? */
      trv_tbl->lst[idx_tbl].var_dmn[idx_dmn].crd->lmt_msa.NON_HYP_DMN=False;

      /* Parse user-specified limits into hyperslab specifications. NOTE: Use True parameter and "crd" */
      (void)nco_lmt_evl_dmn_crd(nc_id,0L,FORTRAN_IDX_CNV,crd->crd_grp_nm_fll,crd->nm,crd->sz,crd->is_rec_dmn,True,lmt[idx_lmt]);

      /* Alloc this limit */
      trv_tbl->lst[idx_tbl].var_dmn[idx_dmn].crd->lmt_msa.lmt_dmn[idx_lmt]=(lmt_sct *)nco_malloc(sizeof(lmt_sct));

      /* Initialize this entry */
      (void)nco_lmt_init(trv_tbl->lst[idx_tbl].var_dmn[idx_dmn].crd->lmt_msa.lmt_dmn[idx_lmt]);

      /* Store dimension ID */
      lmt[idx_lmt]->id=crd->dmn_id;

      /* Store this valid input; deep-copy to table */ 
      (void)nco_lmt_cpy(lmt[idx_lmt],trv_tbl->lst[idx_tbl].var_dmn[idx_dmn].crd->lmt_msa.lmt_dmn[idx_lmt]);

    } /* Loop limits */

    /* Apply MSA for each Dimension in new cycle (that now has all its limits in place) */

    /* Loop limits */
    for(int idx_lmt=0;idx_lmt<nbr_nbr;idx_lmt++){

      crd_sct *crd=trv_tbl->lst[idx_tbl].var_dmn[idx_dmn].crd;

      /* Adapted from original MSA loop in nco_msa_lmt_all_ntl(); differences are marked GTT specific */
      nco_bool flg_ovl; /* [flg] Limits overlap */

      /* GTT: If this coordinate has no limits, continue */
      if(trv_tbl->lst[idx_tbl].var_dmn[idx_dmn].crd->lmt_msa.lmt_dmn_nbr == 0) continue;

      /* ncra/ncrcat have only one limit for record dimension so skip evaluation otherwise this messes up multi-file operation */
      if(crd->is_rec_dmn && (nco_prg_id_get() == ncra || nco_prg_id_get() == ncrcat)) continue;

      /* Split-up wrapped limits. NOTE: using deep copy version nco_msa_wrp_splt_cpy() */   
      (void)nco_msa_wrp_splt_cpy(&trv_tbl->lst[idx_tbl].var_dmn[idx_dmn].crd->lmt_msa);

      /* Wrapped hyperslabs are dimensions broken into the "wrong" order, e.g., from
	 -d time,8,2 broken into -d time,8,9 -d time,0,2 
	 WRP flag set only when list contains dimensions split as above */
      if(trv_tbl->lst[idx_tbl].var_dmn[idx_dmn].crd->lmt_msa.WRP){
        /* Find and store size of output dim */  
        (void)nco_msa_clc_cnt(&trv_tbl->lst[idx_tbl].var_dmn[idx_dmn].crd->lmt_msa); 
        continue;
      } /* End WRP flag set */

      /* Single slab---no analysis needed */  
      if(trv_tbl->lst[idx_tbl].var_dmn[idx_dmn].crd->lmt_msa.lmt_dmn_nbr == 1){
        (void)nco_msa_clc_cnt(&trv_tbl->lst[idx_tbl].var_dmn[idx_dmn].crd->lmt_msa);  
        continue;    
      } /* End Single slab */

      /* Does Multi-Slab Algorithm returns hyperslabs in user-specified order? */
      if(MSA_USR_RDR){
        trv_tbl->lst[idx_tbl].var_dmn[idx_dmn].crd->lmt_msa.MSA_USR_RDR=True;

        /* Find and store size of output dimension */  
        (void)nco_msa_clc_cnt(&trv_tbl->lst[idx_tbl].var_dmn[idx_dmn].crd->lmt_msa);  
        continue;
      } /* End MSA_USR_RDR */

      /* Sort limits */
      (void)nco_msa_qsort_srt(&trv_tbl->lst[idx_tbl].var_dmn[idx_dmn].crd->lmt_msa);

      /* Check for overlap */
      flg_ovl=nco_msa_ovl(&trv_tbl->lst[idx_tbl].var_dmn[idx_dmn].crd->lmt_msa); 

      /* Find and store size of output dimension */  
      (void)nco_msa_clc_cnt(&trv_tbl->lst[idx_tbl].var_dmn[idx_dmn].crd->lmt_msa);

      if(nco_dbg_lvl_get() >= nco_dbg_fl){
        if(flg_ovl) (void)fprintf(stdout,"%s: coordinate \"%s\" has overlapping hyperslabs\n",nco_prg_nm_get(),crd->nm); else (void)fprintf(stdout,"%s: coordinate \"%s\" has distinct hyperslabs\n",nco_prg_nm_get(),crd->nm); 
      } /* endif */

    } /* Loop limits */
  } /* a) case where the dimension has coordinate variables */

  /* b) Dimension only (no coordinate variable for this dimension) */
  else if(trv_tbl->lst[idx_tbl].var_dmn[idx_dmn].ncd){

    /* For this call (-X) the *same* limits are applied to all coordinates, and other variables might apply
       them too, so make sure they are applied only once by setting -1 in lmt_crr */

    if (trv_tbl->lst[idx_tbl].var_dmn[idx_dmn].ncd->lmt_msa.lmt_crr == -1) return;

    trv_tbl->lst[idx_tbl].var_dmn[idx_dmn].ncd->lmt_msa.lmt_crr=-1;

    /* Loop limits */
    for(int idx_lmt=0;idx_lmt<nbr_nbr;idx_lmt++){
      /* Increment number of dimension limits for this dimension */
      trv_tbl->lst[idx_tbl].var_dmn[idx_dmn].ncd->lmt_msa.lmt_dmn_nbr++;

      int nbr_lmt=trv_tbl->lst[idx_tbl].var_dmn[idx_dmn].ncd->lmt_msa.lmt_dmn_nbr;
      trv_tbl->lst[idx_tbl].var_dmn[idx_dmn].ncd->lmt_msa.lmt_dmn=(lmt_sct **)nco_realloc(trv_tbl->lst[idx_tbl].var_dmn[idx_dmn].ncd->lmt_msa.lmt_dmn,nbr_lmt*sizeof(lmt_sct *));

      dmn_trv_sct *ncd=trv_tbl->lst[idx_tbl].var_dmn[idx_dmn].ncd;

      /* Limit is same as dimension in input file? */
      trv_tbl->lst[idx_tbl].var_dmn[idx_dmn].ncd->lmt_msa.NON_HYP_DMN=False;

      /* Parse user-specified limits into hyperslab specifications. NOTE: Use False parameter */
      (void)nco_lmt_evl_dmn_crd(nc_id,0L,FORTRAN_IDX_CNV,ncd->grp_nm_fll,ncd->nm,ncd->sz,ncd->is_rec_dmn,False,lmt[idx_lmt]);

      /* Alloc this limit */
      trv_tbl->lst[idx_tbl].var_dmn[idx_dmn].ncd->lmt_msa.lmt_dmn[idx_lmt]=(lmt_sct *)nco_malloc(sizeof(lmt_sct));

      /* Initialize this entry */
      (void)nco_lmt_init(trv_tbl->lst[idx_tbl].var_dmn[idx_dmn].ncd->lmt_msa.lmt_dmn[idx_lmt]);

      /* Store dimension ID */
      lmt[idx_lmt]->id=ncd->dmn_id;

      /* Store this valid input; deep-copy to table */ 
      (void)nco_lmt_cpy(lmt[idx_lmt],trv_tbl->lst[idx_tbl].var_dmn[idx_dmn].ncd->lmt_msa.lmt_dmn[idx_lmt]);

    } /* Loop limits */

    /* Apply MSA for each Dimension in new cycle (that now has all its limits in place) */

    /* Loop limits */
    for(int idx_lmt=0;idx_lmt<nbr_nbr;idx_lmt++){
      dmn_trv_sct *ncd=trv_tbl->lst[idx_tbl].var_dmn[idx_dmn].ncd;

      /* Adapted from original MSA loop in nco_msa_lmt_all_ntl(); differences are marked GTT specific */
      nco_bool flg_ovl; /* [flg] Limits overlap */

      /* GTT: If this coordinate has no limits, continue */
      if(trv_tbl->lst[idx_tbl].var_dmn[idx_dmn].ncd->lmt_msa.lmt_dmn_nbr == 0) continue;

      /* ncra/ncrcat have only one limit for record dimension so skip evaluation otherwise this messes up multi-file operation */
      if(ncd->is_rec_dmn && (nco_prg_id_get() == ncra || nco_prg_id_get() == ncrcat)) continue;

      /* Split-up wrapped limits. NOTE: using deep copy version nco_msa_wrp_splt_cpy() */   
      (void)nco_msa_wrp_splt_cpy(&trv_tbl->lst[idx_tbl].var_dmn[idx_dmn].ncd->lmt_msa);

      /* Wrapped hyperslabs are dimensions broken into the "wrong" order, e.g., from
      -d time,8,2 broken into -d time,8,9 -d time,0,2 
      WRP flag set only when list contains dimensions split as above */
      if(trv_tbl->lst[idx_tbl].var_dmn[idx_dmn].ncd->lmt_msa.WRP){
        /* Find and store size of output dim */  
        (void)nco_msa_clc_cnt(&trv_tbl->lst[idx_tbl].var_dmn[idx_dmn].ncd->lmt_msa); 
        continue;
      } /* End WRP flag set */

      /* Single slab---no analysis needed */  
      if(trv_tbl->lst[idx_tbl].var_dmn[idx_dmn].ncd->lmt_msa.lmt_dmn_nbr == 1){
        (void)nco_msa_clc_cnt(&trv_tbl->lst[idx_tbl].var_dmn[idx_dmn].ncd->lmt_msa);  
        continue;    
      } /* End Single slab */

      /* Does Multi-Slab Algorithm return hyperslabs in user-specified order? */
      if(MSA_USR_RDR){
        trv_tbl->lst[idx_tbl].var_dmn[idx_dmn].ncd->lmt_msa.MSA_USR_RDR=True;
        /* Find and store size of output dimension */  
        (void)nco_msa_clc_cnt(&trv_tbl->lst[idx_tbl].var_dmn[idx_dmn].ncd->lmt_msa);  
        continue;
      } /* End MSA_USR_RDR */

      /* Sort limits */
      (void)nco_msa_qsort_srt(&trv_tbl->lst[idx_tbl].var_dmn[idx_dmn].ncd->lmt_msa);

      /* Check for overlap */
      flg_ovl=nco_msa_ovl(&trv_tbl->lst[idx_tbl].var_dmn[idx_dmn].ncd->lmt_msa); 

      /* Find and store size of output dimension */  
      (void)nco_msa_clc_cnt(&trv_tbl->lst[idx_tbl].var_dmn[idx_dmn].ncd->lmt_msa);

      if(nco_dbg_lvl_get() >= nco_dbg_fl){
	if(flg_ovl) (void)fprintf(stdout,"%s: coordinate \"%s\" has overlapping hyperslabs\n",nco_prg_nm_get(),ncd->nm); else (void)fprintf(stdout,"%s: coordinate \"%s\" has distinct hyperslabs\n",nco_prg_nm_get(),ncd->nm);
      } /* endif dbg */

    } /* Loop limits */

  } /* b) Dimension only (no coordinate variable for this dimension) */

  return;
} /* nco_lmt_aux() */

int
nco_bld_crd_aux /* [fnc] Build auxiliary coordinates information into table */
(const int nc_id, /* I [ID] netCDF file ID */
 trv_tbl_sct *trv_tbl) /* I [sct] GTT (Group Traversal Table) */
{
  const char fnc_nm[]="nco_bld_crd_aux()"; /* [sng] Function name */

  /* Look for attributes 'standard_name' == 'latitude' and 'longitude' */
  char *var_nm_fll=NULL;

  int dmn_id; /* [id] Dimension ID for 'latitude' and 'longitude' coordinate variables, e.g., lat_gds(gds_crd) */
  int nbr_lat=0;
  int nbr_lon=0; 
  nc_type crd_typ;

  for(unsigned idx_var=0;idx_var<trv_tbl->nbr;idx_var++){
    nco_bool has_lat=False;
    nco_bool has_lon=False;
    trv_sct *var_trv=&trv_tbl->lst[idx_var];

    if(var_trv->nco_typ == nco_obj_typ_var){
      char units_lat[NC_MAX_NAME+1L];
      char units_lon[NC_MAX_NAME+1L];

      has_lat=nco_find_lat_lon_trv(nc_id,var_trv,"latitude",&var_nm_fll,&dmn_id,&crd_typ,units_lat);

               
      /* has_lat and has_lon mutually exclusive */
      if(!has_lat)    
          has_lon=nco_find_lat_lon_trv(nc_id,var_trv,"longitude",&var_nm_fll,&dmn_id,&crd_typ,units_lon);


      if(has_lat){
	
	nbr_lat++;
        /* Variable contains 'standard_name' attribute 'latitude' */ 
        var_trv->flg_std_att_lat=True; 

        if(nco_dbg_lvl_get() >= nco_dbg_dev) (void)fprintf(stdout,"%s: DEBUG %s 'standard_name' attribute is 'latitude' for variable %s with dimension ID = %d\n",nco_prg_nm_get(),fnc_nm,var_trv->nm_fll,dmn_id); 

        for(unsigned idx_crd=0;idx_crd<trv_tbl->nbr;idx_crd++){
          /* Detect 'standard_name' attribute 'latitude' in the compared variable, to avoid inserting it */
          nco_bool has_lat_lcl=False;
          nco_bool has_lon_lcl=False;
          char *var_nm_fll_lcl=NULL;
          int dmn_id_lcl;

          if(trv_tbl->lst[idx_crd].nco_typ == nco_obj_typ_var){
            has_lat_lcl=nco_find_lat_lon_trv(nc_id,&trv_tbl->lst[idx_crd],"latitude",&var_nm_fll_lcl,&dmn_id_lcl,&crd_typ,units_lat);
            has_lon_lcl=nco_find_lat_lon_trv(nc_id,&trv_tbl->lst[idx_crd],"longitude",&var_nm_fll_lcl,&dmn_id_lcl,&crd_typ,units_lon);
          } /* endif */

          if(nco_dbg_lvl_get() >= nco_dbg_dev){ 
            if(has_lat_lcl) (void)fprintf(stdout,"%s: DEBUG %s \t AVOID 'standard_name' attribute 'latitude' for variable %s with dimension ID = %d\n",nco_prg_nm_get(),fnc_nm,var_nm_fll_lcl,dmn_id_lcl);
            if(has_lon_lcl) (void)fprintf(stdout,"%s: DEBUG %s \t AVOID 'standard_name' attribute 'longitude' for variable %s with dimension ID = %d\n",nco_prg_nm_get(),fnc_nm,var_nm_fll_lcl,dmn_id_lcl);
	  } /* endif dbg */

          if(trv_tbl->lst[idx_crd].nco_typ == nco_obj_typ_var &&
	     (!has_lat_lcl && !has_lon_lcl) && /* Avoid inserting 'lat_gds' or 'lon_gds' */
	     !trv_tbl->lst[idx_crd].is_crd_var){
	    
	    for(int idx_dmn=0;idx_dmn<trv_tbl->lst[idx_crd].nbr_dmn;idx_dmn++){
	      
	      /* Match dimension */
	      int var_dim_id=trv_tbl->lst[idx_crd].var_dmn[idx_dmn].dmn_id;
	      if(var_dim_id == dmn_id){
		
		/* Check if possible 'latitude' (var_trv) is in-scope */
		if(nco_var_scp(&trv_tbl->lst[idx_crd],var_trv,trv_tbl)){
		  
		  /* Mark variable (e.g., gds_var, gds_3dvar) as containing auxiliary coordinates */
		  trv_tbl->lst[idx_crd].flg_aux=True;
		  
		  if(nco_dbg_lvl_get() >= nco_dbg_dev) (void)fprintf(stdout,"%s: DEBUG %s reports variable %s has auxiliary coordinates with dimension ID = %d\n",nco_prg_nm_get(),fnc_nm,trv_tbl->lst[idx_crd].nm_fll,var_dim_id); 

		  /* Insert item into list */
		  trv_tbl->lst[idx_crd].var_dmn[idx_dmn].nbr_lat_crd++;
		  int nbr_lat_crd=trv_tbl->lst[idx_crd].var_dmn[idx_dmn].nbr_lat_crd;
		  trv_tbl->lst[idx_crd].var_dmn[idx_dmn].lat_crd=(aux_crd_sct *)nco_realloc(trv_tbl->lst[idx_crd].var_dmn[idx_dmn].lat_crd,nbr_lat_crd*sizeof(aux_crd_sct));
		  trv_tbl->lst[idx_crd].var_dmn[idx_dmn].lat_crd[nbr_lat_crd-1].nm_fll=strdup(var_nm_fll);
		  trv_tbl->lst[idx_crd].var_dmn[idx_dmn].lat_crd[nbr_lat_crd-1].dmn_id=dmn_id;
		  trv_tbl->lst[idx_crd].var_dmn[idx_dmn].lat_crd[nbr_lat_crd-1].grp_dpt=var_trv->grp_dpt;
		  trv_tbl->lst[idx_crd].var_dmn[idx_dmn].lat_crd[nbr_lat_crd-1].crd_typ=crd_typ;
		  strcpy(trv_tbl->lst[idx_crd].var_dmn[idx_dmn].lat_crd[nbr_lat_crd-1].units,units_lat);
		  
		} /* Is in-scope */
	      } /* Match dimension */
	    } /* Loop dimensions  */
          } /* variables only */
        } /* end inner loop over possible coordinates */
      } /* !has_lat */
      

      
      if(has_lon){
	
	nbr_lon++;
        /* Variable contains 'standard_name' attribute 'longitude' */ 
        var_trv->flg_std_att_lon=True; 

        if(nco_dbg_lvl_get() >= nco_dbg_dev) (void)fprintf(stdout,"%s: DEBUG %s 'standard_name' attribute is 'longitude' for variable %s with dimension ID = %d\n",nco_prg_nm_get(),fnc_nm,var_trv->nm_fll,dmn_id); 

        for(unsigned idx_crd=0;idx_crd<trv_tbl->nbr;idx_crd++){

	  /* Detect 'standard_name' attribute 'latitude' in the compared variable, to avoid inserting it */
          nco_bool has_lat_lcl=False;
          nco_bool has_lon_lcl=False;
          char *var_nm_fll_lcl=NULL;
          int dmn_id_lcl;

          if(trv_tbl->lst[idx_crd].nco_typ == nco_obj_typ_var){
            has_lat_lcl=nco_find_lat_lon_trv(nc_id,&trv_tbl->lst[idx_crd],"latitude",&var_nm_fll_lcl,&dmn_id_lcl,&crd_typ,units_lat);
            has_lon_lcl=nco_find_lat_lon_trv(nc_id,&trv_tbl->lst[idx_crd],"longitude",&var_nm_fll_lcl,&dmn_id_lcl,&crd_typ,units_lon);
          } /* endif */

          if(trv_tbl->lst[idx_crd].nco_typ == nco_obj_typ_var &&
	     (!has_lat_lcl && !has_lon_lcl) && /* Avoid inserting 'lat_gds' or 'lon_gds' */
	     !trv_tbl->lst[idx_crd].is_crd_var){

	    for(int idx_dmn=0;idx_dmn<trv_tbl->lst[idx_crd].nbr_dmn;idx_dmn++){
	      
	      /* Match dimension */
	      int var_dim_id=trv_tbl->lst[idx_crd].var_dmn[idx_dmn].dmn_id;
	      if (var_dim_id == dmn_id){
		/* Check if possible 'longitude' (var_trv) is in-scope */
		if(nco_var_scp(&trv_tbl->lst[idx_crd],var_trv,trv_tbl)){
		  
		  /* Mark variable (e.g., gds_var, gds_3dvar) as containing auxiliary coordinates */
		  trv_tbl->lst[idx_crd].flg_aux=True;
		  
		  if(nco_dbg_lvl_get() >= nco_dbg_dev) (void)fprintf(stdout,"%s: DEBUG %s reports variable %s has auxiliary coordinates with dimension ID = %d\n",nco_prg_nm_get(),fnc_nm,trv_tbl->lst[idx_crd].nm_fll,var_dim_id); 

		  /* Insert item into list */
		  trv_tbl->lst[idx_crd].var_dmn[idx_dmn].nbr_lon_crd++;
		  int nbr_lon_crd=trv_tbl->lst[idx_crd].var_dmn[idx_dmn].nbr_lon_crd;
		  trv_tbl->lst[idx_crd].var_dmn[idx_dmn].lon_crd=(aux_crd_sct *)nco_realloc(trv_tbl->lst[idx_crd].var_dmn[idx_dmn].lon_crd,nbr_lon_crd*sizeof(aux_crd_sct));
		  trv_tbl->lst[idx_crd].var_dmn[idx_dmn].lon_crd[nbr_lon_crd-1].nm_fll=strdup(var_nm_fll);
		  trv_tbl->lst[idx_crd].var_dmn[idx_dmn].lon_crd[nbr_lon_crd-1].dmn_id=dmn_id;
		  trv_tbl->lst[idx_crd].var_dmn[idx_dmn].lon_crd[nbr_lon_crd-1].grp_dpt=var_trv->grp_dpt;
		  trv_tbl->lst[idx_crd].var_dmn[idx_dmn].lon_crd[nbr_lon_crd-1].crd_typ=crd_typ;
		  strcpy(trv_tbl->lst[idx_crd].var_dmn[idx_dmn].lon_crd[nbr_lon_crd-1].units,units_lat);
		  
		} /* Is in-scope */
	      } /* Match dimension */
	    } /* Loop dimensions  */
          } /* variables only */
        } /* end inner loop over possible coordinates */
      } /* has_lon */
    } /* Filter variables */ 
  } /* Loop table 1*/
  
  /* Sort array of 'latitude' and 'longitude' coordinate variables by group depth and choose the most in-scope variables */
  if(nbr_lat ||  nbr_lon)
    nco_srt_aux(trv_tbl);    

  /* return True if only latitude and longitude found */ 
  return (nbr_lat && nbr_lon);
} /* nco_bld_crd_aux() */

int
nco_bld_crd_nm_aux                     /* [fnc] Build auxiliary coordinates information into table using named latitude and longitude*/
(const int nc_id,                      /* I [ID] netCDF file ID */
 const char * const nm_lat,            /* I [sng] name of "latitude" variable to find  */
 const char * const nm_lon,            /* I [sng] name of "longitude" variable to find  */
 trv_tbl_sct *trv_tbl)                 /* I [sct] GTT (Group Traversal Table) */
{
  /*
   Purpose: 
   Look for 'latitude' variable named  nm_lat, if it passes criteria in nco_check_nm_aux() then flag it as an auxiliary coordinate
   look for 'longitude' variable named nm_lon, if it passes criteria in nco_check_nm_aux() then flag it as an auxiliary coordinate
   If auxiliary coordinates are "in scope" (at same depth or above) of a variable AND the variable has the matching dimension,
   then add references to the auxiliary coordinates in var_trv->var_dmn. */

  const char fnc_nm[]="nco_bld_crd_nm_aux()"; /* [sng] Function name */

  int dmn_id; /* [id] Dimension ID for 'latitude' and 'longitude' coordinate variables, e.g., lat_gds(gds_crd) */
  int nbr_lat=0;
  int nbr_lon=0; 
  nc_type crd_typ;

  if(nco_dbg_lvl_get() >= nco_dbg_dev) (void)fprintf(stdout,"%s: DEBUG %s just entered function\n",nco_prg_nm_get(),fnc_nm); 
  
  for(unsigned idx_var=0;idx_var<trv_tbl->nbr;idx_var++){
    nco_bool has_lat=False;
    nco_bool has_lon=False;

    char units_lat[NC_MAX_NAME+1L];
    char units_lon[NC_MAX_NAME+1L];

    trv_sct *var_trv=&trv_tbl->lst[idx_var];
    
    if(var_trv->nco_typ != nco_obj_typ_var) continue;

    if(!strcmp(var_trv->nm, nm_lat)) has_lat=nco_check_nm_aux(nc_id,var_trv,&dmn_id,&crd_typ,units_lat);
    
    /* has_lat and has_lon are mutually exclusive */
    if(!has_lat && !strcmp(var_trv->nm, nm_lon)) has_lon=nco_check_nm_aux(nc_id,var_trv,&dmn_id,&crd_typ,units_lon);
    
    if(has_lat){
      nbr_lat++;
      /* Variable contains 'standard_name' attribute 'latitude' */ 
      var_trv->flg_std_att_lat=True; 
      
      if(nco_dbg_lvl_get() >= nco_dbg_dev) (void)fprintf(stdout,"%s: DEBUG %s variable %s with dimension ID = %d has been recognized as a auxiliary coordinate\n",nco_prg_nm_get(),fnc_nm,var_trv->nm_fll,dmn_id); 

      for(unsigned idx_crd=0;idx_crd<trv_tbl->nbr;idx_crd++){
	/* Detect 'standard_name' attribute 'latitude' in the compared variable, to avoid inserting it */
	trv_sct *crd_trv=&trv_tbl->lst[idx_crd];
	
	/* check if var and is in scope */
	if(crd_trv->nco_typ == nco_obj_typ_var &&  nco_var_scp(crd_trv ,var_trv,trv_tbl)){
	  /* Detect if nm_lat or nm_lon already in scope */
	  if(!strcmp(crd_trv->nm, nm_lat) || !strcmp(crd_trv->nm, nm_lon)) continue;
	  
	  for(int idx_dmn=0;idx_dmn<crd_trv->nbr_dmn;idx_dmn++){
	    int nbr_lat_crd=0;
	    /* Match dimension */
	    if(crd_trv->var_dmn[idx_dmn].dmn_id == dmn_id){
	      
	      /* Mark variable (e.g., gds_var, gds_3dvar) as containing auxiliary coordinates */
	      crd_trv->flg_aux=True;
	      
	      if(nco_dbg_lvl_get() >= nco_dbg_dev) (void)fprintf(stdout,"%s: DEBUG %s reports variable %s has auxiliary dimension ID = %d\n",nco_prg_nm_get(),fnc_nm,crd_trv->nm_fll,dmn_id); 
		
	      /* Insert item into list */
	      crd_trv->var_dmn[idx_dmn].nbr_lat_crd++;
	      nbr_lat_crd=crd_trv->var_dmn[idx_dmn].nbr_lat_crd;
	      crd_trv->var_dmn[idx_dmn].lat_crd=(aux_crd_sct *)nco_realloc(crd_trv->var_dmn[idx_dmn].lat_crd,nbr_lat_crd*sizeof(aux_crd_sct));
	      crd_trv->var_dmn[idx_dmn].lat_crd[nbr_lat_crd-1].nm_fll=strdup(var_trv->nm_fll);
	      crd_trv->var_dmn[idx_dmn].lat_crd[nbr_lat_crd-1].dmn_id=dmn_id;
	      crd_trv->var_dmn[idx_dmn].lat_crd[nbr_lat_crd-1].grp_dpt=var_trv->grp_dpt;
	      crd_trv->var_dmn[idx_dmn].lat_crd[nbr_lat_crd-1].crd_typ=crd_typ;
	      strcpy(crd_trv->var_dmn[idx_dmn].lat_crd[nbr_lat_crd-1].units,units_lat);
	      
	    } /* Match dimension */
	  } /* Loop dimensions  */
	} /* variables only */
      } /* !has_lat */
    }
    
    if(has_lon){
      
      nbr_lon++;
      /* Variable contains 'standard_name' attribute 'latitude' */ 
      var_trv->flg_std_att_lon=True; 
      
      if(nco_dbg_lvl_get() >= nco_dbg_dev) (void)fprintf(stdout,"%s: DEBUG %s variable %s with dimension ID = %d has been recognized as a auxiliary coordinate\n",nco_prg_nm_get(),fnc_nm,var_trv->nm_fll,dmn_id); 
      
      for(unsigned idx_crd=0;idx_crd<trv_tbl->nbr;idx_crd++){
	
	trv_sct *crd_trv=&trv_tbl->lst[idx_crd];
	
	/* check if var and is in scope */
	if(crd_trv->nco_typ == nco_obj_typ_var && nco_var_scp(crd_trv ,var_trv,trv_tbl)){
	  
	  /* Detect if nm_lat or nm_lon already in scope */
	  if(!strcmp(crd_trv->nm, nm_lat) || !strcmp(crd_trv->nm, nm_lon)) continue;
	  
	  for(int idx_dmn=0;idx_dmn<crd_trv->nbr_dmn;idx_dmn++){
	    int nbr_lon_crd=0;
	    /* Match dimension */
	    if(crd_trv->var_dmn[idx_dmn].dmn_id == dmn_id){
	      
	      /* Mark variable (e.g., gds_var, gds_3dvar) as containing auxiliary coordinates */
	      crd_trv->flg_aux=True;
	      
	      if(nco_dbg_lvl_get() >= nco_dbg_dev) (void)fprintf(stdout,"%s: DEBUG %s reports variable %s has auxiliary dimension ID = %d\n",nco_prg_nm_get(),fnc_nm,crd_trv->nm_fll,dmn_id); 
		  
	      /* Insert item into list */
	      crd_trv->var_dmn[idx_dmn].nbr_lon_crd++;
	      nbr_lon_crd=crd_trv->var_dmn[idx_dmn].nbr_lon_crd;
	      
	      crd_trv->var_dmn[idx_dmn].lon_crd=(aux_crd_sct *)nco_realloc(crd_trv->var_dmn[idx_dmn].lon_crd,nbr_lon_crd*sizeof(aux_crd_sct));
	      crd_trv->var_dmn[idx_dmn].lon_crd[nbr_lon_crd-1].nm_fll=strdup(var_trv->nm_fll);
	      crd_trv->var_dmn[idx_dmn].lon_crd[nbr_lon_crd-1].dmn_id=dmn_id;
	      crd_trv->var_dmn[idx_dmn].lon_crd[nbr_lon_crd-1].grp_dpt=var_trv->grp_dpt;
	      crd_trv->var_dmn[idx_dmn].lon_crd[nbr_lon_crd-1].crd_typ=crd_typ;
	      strcpy(crd_trv->var_dmn[idx_dmn].lon_crd[nbr_lon_crd-1].units,units_lon);
	      
	    } /* Match dimension */
	  } /* Loop dimensions  */
	} /* variables only */
      } /* !has_lon */
    }	
  }
  
  /* Sort array of 'latitude' and 'longitude' coordinate variables by group depth and choose the most in-scope variables */
  if(nbr_lat && nbr_lon) nco_srt_aux(trv_tbl);
  
  /* return True if only latitude and longitude found */ 
  return (nbr_lat && nbr_lon);
} /* !nco_bld_crd_nm_aux() */

nco_bool                               /* O [flg] True if variable 1 is in-scope of variable 2 */
nco_var_scp                            /* [fnc] Is variable 1 is in-scope of variable 2 */
(const trv_sct * const var_trv_1,      /* I [sct] Variable 1 */
 const trv_sct * const var_trv_2,      /* I [sct] Variable 2 (use case , find 'latitude in-scope)*/
 const trv_tbl_sct * const trv_tbl)    /* I [sct] Traversal table */
{
  
  /* Same group */
  if(!strcmp(var_trv_1->grp_nm_fll,var_trv_2->grp_nm_fll)) return True;
  
  /* Different groups: traverse down the higher and look for a group match name */  
  if(var_trv_2->grp_dpt > var_trv_1->grp_dpt){
    
    char *grp_nm_fll_prn_2=var_trv_2->grp_nm_fll_prn; /* [sct] Parent group */
    trv_sct *grp_prn_2;
    
    grp_prn_2=trv_tbl_grp_nm_fll(grp_nm_fll_prn_2,trv_tbl);
    
    /* Look for same group name in hierarchy */
    while (grp_nm_fll_prn_2){
      /* Same group in hierarchy */
      if(!strcmp(grp_prn_2->grp_nm,var_trv_1->grp_nm)) return True;
      
      /* Exit loop when root reached */
      if(grp_prn_2->grp_dpt == 0) break; 
      
      grp_nm_fll_prn_2=grp_prn_2->grp_nm_fll_prn;
      grp_prn_2=trv_tbl_grp_nm_fll(grp_nm_fll_prn_2,trv_tbl);
    } /* end while */
  } /* end if */

  return False;
} /* end nco_var_scp() */

int *
nco_dmn_malloc                        /* [fnc] Inquire about number of dimensions in group and return dynamic array of dimension IDs */
(const int nc_id,                     /* I [ID] netCDF file ID */
 const char * const grp_nm_fll,       /* I [sng] Group full name */
 int *dmn_nbr)                        /* I/O [nbr] Number of dimensions in group */
{
  const int flg_prn=0;   /* [flg] Retrieve all dimensions in parent groups */        

  int *dmn_ids;          /* [nbr] Dimensions IDs array */
  int grp_id;            /* [ID]  Group ID */
  int nbr_dmn;           /* [nbr] Number of dimensions */

  /* Obtain group ID */
  (void)nco_inq_grp_full_ncid(nc_id,grp_nm_fll,&grp_id);

  /* Obtain number of dimensions for group */
  (void)nco_inq(grp_id,&nbr_dmn,(int *)NULL,(int *)NULL,(int *)NULL);

  /* Alloc array */
  dmn_ids=(int *)nco_malloc(nbr_dmn*sizeof(int));

  /* Obtain dimensions IDs for group */
  (void)nco_inq_dimids(grp_id,&nbr_dmn,dmn_ids,flg_prn);

  /* Export number of dimensions */
  *dmn_nbr=nbr_dmn;

  return dmn_ids;
} /* end nco_dmn_malloc() */

void
nco_dmn_lmt                            /* [fnc] Convert a lmt_sct array to dmn_sct (name only) */
(lmt_sct **lmt,                        /* I [lst] lmt_sct array  */
 const int nbr_lmt,                    /* I [nbr] Size of lmt_sct array  */
 dmn_sct ***dmn)                       /* O [sct] dmn_sct array  */
{
  for(int idx=0;idx<nbr_lmt;idx++){
    (*dmn)[idx]=(dmn_sct *)nco_malloc(sizeof(dmn_sct));
    (*dmn)[idx]->nm=(char *)strdup(lmt[idx]->nm);
    (*dmn)[idx]->nm_fll=(char *)strdup(lmt[idx]->nm_fll);
    (*dmn)[idx]->id=-1;
    (*dmn)[idx]->nc_id=-1;
    (*dmn)[idx]->xrf=NULL;
    (*dmn)[idx]->val.vp=NULL;
    (*dmn)[idx]->is_rec_dmn=lmt[idx]->is_rec_dmn;
    (*dmn)[idx]->cnt=-1L;
    (*dmn)[idx]->sz=-1L;
    (*dmn)[idx]->srt=-1L;
    (*dmn)[idx]->end=-1L;
    (*dmn)[idx]->srd=-1L;
    (*dmn)[idx]->cid=-1;
    (*dmn)[idx]->cnk_sz=-1L;
    (*dmn)[idx]->type=(nc_type)-1;
  } /* endfor */
  return;
} /* end nco_dmn_lmt() */

void
nco_prn_nsm                                 /* [fnc] Print ensembles  */                                
(const trv_tbl_sct * const trv_tbl)         /* I [sct] Traversal table */
{
  if(!trv_tbl->nsm_nbr) return;

  (void)fprintf(stdout,"%s: list of ensembles\n",nco_prg_nm_get()); 
  for(int idx_nsm=0;idx_nsm<trv_tbl->nsm_nbr;idx_nsm++) (void)fprintf(stdout,"%s: <%s>\n",nco_prg_nm_get(),trv_tbl->nsm[idx_nsm].grp_nm_fll_prn);

  (void)fprintf(stdout,"%s: list of fixed templates\n",nco_prg_nm_get()); 
  for(int idx_skp=0;idx_skp<trv_tbl->nsm->skp_nbr;idx_skp++) (void)fprintf(stdout,"%s: <template> %d <%s>\n",nco_prg_nm_get(),idx_skp,trv_tbl->nsm->skp_nm_fll[idx_skp]); 

  (void)fprintf(stdout,"%s: list of templates\n",nco_prg_nm_get()); 
  int idx_tpl=0;
  for(unsigned idx_tbl=0;idx_tbl<trv_tbl->nbr;idx_tbl++)
    if(trv_tbl->lst[idx_tbl].flg_nsm_tpl){
      (void)fprintf(stdout,"%s: <template> %d <%s>\n",nco_prg_nm_get(),idx_tpl,trv_tbl->lst[idx_tbl].nm_fll); 
      idx_tpl++;
    } /* endif */

  (void)fprintf(stdout,"%s: list of ensemble members\n",nco_prg_nm_get()); 
  for(int idx_nsm=0;idx_nsm<trv_tbl->nsm_nbr;idx_nsm++){
    (void)fprintf(stdout,"%s: <ensemble %d> <%s>\n",nco_prg_nm_get(),idx_nsm,trv_tbl->nsm[idx_nsm].grp_nm_fll_prn);
    for(int idx_mbr=0;idx_mbr<trv_tbl->nsm[idx_nsm].mbr_nbr;idx_mbr++){
      (void)fprintf(stdout,"%s: \t <member %d> <%s>\n",nco_prg_nm_get(),idx_mbr,trv_tbl->nsm[idx_nsm].mbr[idx_mbr].mbr_nm_fll); 
      for(int idx_var=0;idx_var<trv_tbl->nsm[idx_nsm].mbr[idx_mbr].var_nbr;idx_var++){
        (void)fprintf(stdout,"%s: \t <variable %d> <%s>\n",nco_prg_nm_get(),idx_var,trv_tbl->nsm[idx_nsm].mbr[idx_mbr].var_nm_fll[idx_var]); 
      }
    }
  }
} /* nco_prn_nsm() */

void
nco_bld_nsm                           /* [fnc] Build ensembles */
(const int nc_id,                     /* I [id] netCDF file ID */
 const nco_bool flg_fix_xtr,          /* I [flg] Mark fized variables as extracted  */
 const cnv_sct * const cnv,           /* I [flg] File adheres to NCAR CCM/CCSM/CF conventions */
 const int nco_pck_plc,               /* I [enm] Packing policy */
 trv_tbl_sct * const trv_tbl)         /* I/O [sct] Traversal table */
{
  /* Purpose: Build ensembles  */
  const char fnc_nm[]="nco_bld_nsm()"; /* [sng] Function name */

  char **nm_lst_1;                     /* [sng] List of names */
  char **nm_lst_2;                     /* [sng] List of names */

  int nm_lst_1_nbr;                    /* [nbr] Number of items in list */
  int nm_lst_2_nbr;                    /* [nbr] Number of items in list */
  int nbr_cmn_nm;                      /* [nbr] Number of common entries */
  int nbr_nm;                          /* [nbr] Number of total entries */
  int nco_prg_id=nco_prg_id_get();     /* [enm] Program key */

  nco_bool flg_nsm_tpl;                /* [flg] Variable is template */       

  nco_cmn_t *cmn_lst=NULL;             /* [sct] A list of common names */ 

  /* Insert ensembles (parent group name is key), template variables and fixed template variables */

  /* Loop table  */
  for(unsigned idx_tbl_1=0;idx_tbl_1<trv_tbl->nbr;idx_tbl_1++){
    trv_sct trv_1=trv_tbl->lst[idx_tbl_1];
    /* Group (not root, with variables) */
    if(trv_1.nco_typ == nco_obj_typ_grp && trv_1.grp_dpt > 0 && trv_1.nbr_var > 0){     
      /* Export list of variable names for group */
      (void)nco_grp_var_lst(nc_id,trv_1.grp_nm_fll,&nm_lst_1,&nm_lst_1_nbr);

      for(unsigned idx_tbl_2=0;idx_tbl_2<trv_tbl->nbr;idx_tbl_2++){
	int idx_nsm; /* [idx] Ensemble index */
	nco_bool flg_ins;
        trv_sct trv_2=trv_tbl->lst[idx_tbl_2];

        /* Same depth, same number of variables, same parent group, lower group depth */
        if(trv_1.nco_typ == nco_obj_typ_grp && 
	   trv_2.nco_typ == nco_obj_typ_grp && 
	   trv_1.grp_dpt == trv_2.grp_dpt && 
	   trv_1.nbr_var == trv_2.nbr_var &&
	   trv_1.grp_dpt > 1 &&
	   strcmp(trv_1.grp_nm_fll,trv_2.grp_nm_fll) && /* Yes, test that comparison fails (operands are unequal) */
	   !strcmp(trv_1.grp_nm_fll_prn,trv_2.grp_nm_fll_prn)){
	  
	  /* Is member group already inserted into ensemble array? */
	  for(idx_nsm=0;idx_nsm<trv_tbl->nsm_nbr;idx_nsm++)
	    if(!strcmp(trv_tbl->nsm[idx_nsm].grp_nm_fll_prn,trv_2.grp_nm_fll_prn)) break;

	  if(idx_nsm < trv_tbl->nsm_nbr) flg_ins=True; else flg_ins=False;

	  /* Export list of variable names for group */
	  (void)nco_grp_var_lst(nc_id,trv_2.grp_nm_fll,&nm_lst_2,&nm_lst_2_nbr);
	  /* Match two lists of variable names and export common names */
	  (void)nco_nm_mch(nm_lst_1,nm_lst_1_nbr,nm_lst_2,nm_lst_2_nbr,&cmn_lst,&nbr_nm,&nbr_cmn_nm);
	  /* Found common names */
	  if(nbr_cmn_nm && nm_lst_1_nbr == nm_lst_2_nbr && nm_lst_1_nbr == nbr_cmn_nm && !flg_ins){
	    trv_tbl->nsm_nbr++;
	    trv_tbl->nsm=(nsm_sct *)nco_realloc(trv_tbl->nsm,trv_tbl->nsm_nbr*sizeof(nsm_sct));
	    
	    trv_tbl->nsm[trv_tbl->nsm_nbr-1].grp_nm_fll_prn=(char *)strdup(trv_2.grp_nm_fll_prn);
	    trv_tbl->nsm[trv_tbl->nsm_nbr-1].mbr_nbr=0;
	    trv_tbl->nsm[trv_tbl->nsm_nbr-1].mbr=NULL;
	    
	    trv_tbl->nsm[trv_tbl->nsm_nbr-1].tpl_mbr_nm=NULL;
	    trv_tbl->nsm[trv_tbl->nsm_nbr-1].tpl_nbr=0;
	    trv_tbl->nsm[trv_tbl->nsm_nbr-1].skp_nm_fll=NULL;
	    trv_tbl->nsm[trv_tbl->nsm_nbr-1].skp_nbr=0;
	    
	    trv_tbl->nsm[trv_tbl->nsm_nbr-1].mbr_srt=0;
	    trv_tbl->nsm[trv_tbl->nsm_nbr-1].mbr_end=0;
	    
	    /* Group (NB: outer loop) is ensemble parent group */
	    trv_tbl->lst[idx_tbl_1].flg_nsm_prn=True;

	    if(nco_dbg_lvl_get() == nco_dbg_old) (void)fprintf(stdout,"%s: DEBUG %s inserted ensemble for <%s>\n",nco_prg_nm_get(),fnc_nm,trv_2.grp_nm_fll_prn);

	    /* Loop common names, insert template and fixed template variables */
	    for(int idx_nm=0;idx_nm<nbr_cmn_nm;idx_nm++){

                /* Define variable full name (use trv_1) */
                char *var_nm_fll=nco_bld_nm_fll(trv_1.grp_nm_fll,cmn_lst[idx_nm].nm);

                trv_sct *var_trv=trv_tbl_var_nm_fll(var_nm_fll,trv_tbl);

                nco_bool var_is_fix=False;  /* [fnc] Variable should be treated as a fixed variable */

                if(cnv->CCM_CCSM_CF || cnv->MPAS) var_is_fix=nco_var_is_fix(var_trv->nm,nco_prg_id,nco_pck_plc,cnv);

                /* Define as either fixed template or template  */
                if(var_trv->is_crd_var || var_trv->is_rec_var || var_is_fix){

                  trv_tbl->nsm[trv_tbl->nsm_nbr-1].skp_nbr++;
                  int skp_nbr=trv_tbl->nsm[trv_tbl->nsm_nbr-1].skp_nbr;
                  trv_tbl->nsm[trv_tbl->nsm_nbr-1].skp_nm_fll=(char **)nco_realloc(trv_tbl->nsm[trv_tbl->nsm_nbr-1].skp_nm_fll,skp_nbr*sizeof(char *));
                  trv_tbl->nsm[trv_tbl->nsm_nbr-1].skp_nm_fll[skp_nbr-1]=(char *)strdup(var_trv->nm_fll);

                  if(nco_dbg_lvl_get() >= nco_dbg_dev) (void)fprintf(stdout,"%s: DEBUG %s inserted fixed template <%s>\n",nco_prg_nm_get(),fnc_nm,trv_tbl->nsm[trv_tbl->nsm_nbr-1].skp_nm_fll[skp_nbr-1]);

                }else{

                  trv_tbl->nsm[trv_tbl->nsm_nbr-1].tpl_nbr++;
                  int tpl_nbr=trv_tbl->nsm[trv_tbl->nsm_nbr-1].tpl_nbr;
                  trv_tbl->nsm[trv_tbl->nsm_nbr-1].tpl_mbr_nm=(char **)nco_realloc(trv_tbl->nsm[trv_tbl->nsm_nbr-1].tpl_mbr_nm,tpl_nbr*sizeof(char *));
                  trv_tbl->nsm[trv_tbl->nsm_nbr-1].tpl_mbr_nm[tpl_nbr-1]=(char *)strdup(var_trv->nm);

                  if(nco_dbg_lvl_get() == nco_dbg_old) (void)fprintf(stdout,"%s: DEBUG %s inserted template <%s>\n",nco_prg_nm_get(),fnc_nm,trv_tbl->nsm[trv_tbl->nsm_nbr-1].tpl_mbr_nm[tpl_nbr-1]=(char *)strdup(var_trv->nm));

                } /* Define as either fixed template or template  */
              } /* Loop common names, insert template and fixed template variables */
            } /* Found common names */

            /* Free list 2 */
            for(int idx_nm=0;idx_nm<nm_lst_2_nbr;idx_nm++) nm_lst_2[idx_nm]=(char *)nco_free(nm_lst_2[idx_nm]);
            nm_lst_2=(char **)nco_free(nm_lst_2);

        } /* Same depth, same number of variables */
      } /* Loop table  */

      /* Free list 1 */
      for(int idx_nm=0;idx_nm<nm_lst_1_nbr;idx_nm++) nm_lst_1[idx_nm]=(char *)nco_free(nm_lst_1[idx_nm]);
      nm_lst_1=(char **)nco_free(nm_lst_1);

    } /* Group (not root) */
  } /* Loop table */

  if(nco_dbg_lvl_get() >= nco_dbg_dev){
    (void)fprintf(stdout,"%s: DEBUG %s list of ensembles\n",nco_prg_nm_get(),fnc_nm); 
    for(int idx_nsm=0;idx_nsm<trv_tbl->nsm_nbr;idx_nsm++) (void)fprintf(stdout,"%s: DEBUG %s <%s>\n",nco_prg_nm_get(),fnc_nm,trv_tbl->nsm[idx_nsm].grp_nm_fll_prn);
  } /* endif dbg */

  if(trv_tbl->nsm_nbr == 0) return;

  /* Insert names in ensembles */

  /* Loop ensembles */
  for(int idx_nsm=0;idx_nsm<trv_tbl->nsm_nbr;idx_nsm++){

    /* Loop table  */
    for(unsigned idx_tbl=0;idx_tbl<trv_tbl->nbr;idx_tbl++){
      trv_sct trv=trv_tbl->lst[idx_tbl];

      /* Match group parent name with ensemble name */
      if(trv.nco_typ == nco_obj_typ_grp && 
	 trv.grp_dpt > 1 && 
	 trv.nbr_var > 0 && 
	 strcmp(trv_tbl->nsm[idx_nsm].grp_nm_fll_prn,trv.grp_nm_fll_prn) == 0){

          int mbr_nbr=trv_tbl->nsm[idx_nsm].mbr_nbr;
          trv_tbl->nsm[idx_nsm].mbr_nbr++;
          trv_tbl->nsm[idx_nsm].mbr=(nsm_grp_sct *)nco_realloc(trv_tbl->nsm[idx_nsm].mbr,(mbr_nbr+1)*sizeof(nsm_grp_sct));
          trv_tbl->nsm[idx_nsm].mbr[mbr_nbr].mbr_nm_fll=(char *)strdup(trv.grp_nm_fll);
          trv_tbl->nsm[idx_nsm].mbr[mbr_nbr].var_nbr=0;
          trv_tbl->nsm[idx_nsm].mbr[mbr_nbr].var_nm_fll=NULL;

          /* Define offsets */
          trv_tbl->nsm[idx_nsm].mbr_srt=0;
          trv_tbl->nsm[idx_nsm].mbr_end=trv_tbl->nsm[idx_nsm].mbr_nbr;

          int tpl_nbr=trv_tbl->nsm[idx_nsm].tpl_nbr;

          /* Insert members by builing name from group and template */
          for(int idx_tpl=0;idx_tpl<tpl_nbr;idx_tpl++){ 

            /* Define variable full name */
            char *var_nm_fll=nco_bld_nm_fll(trv.nm_fll,trv_tbl->nsm[idx_nsm].tpl_mbr_nm[idx_tpl]);

            /* Inquire if variable exists */
            trv_sct *var_trv=trv_tbl_var_nm_fll(var_nm_fll,trv_tbl);

            if(!var_trv){
              (void)fprintf(stderr,"%s: ERROR expected member ensemble <%s> does not exist\n",nco_prg_nm_get(),var_nm_fll);
              nco_exit(EXIT_FAILURE);
            } /* endif */

            /* Mark variables as ensemble members */

            /* If not the first group member, then it's not a template */
            if(mbr_nbr == 0) flg_nsm_tpl=True; else flg_nsm_tpl=False;

            /* Mark ensemble member flag in table for "var_nm_fll" */
            (void)trv_tbl_mrk_nsm_mbr(var_nm_fll,flg_nsm_tpl,trv.grp_nm_fll_prn,trv_tbl); 

            /* Insert variable in table ensemble struct */
            trv_tbl->nsm[idx_nsm].mbr[mbr_nbr].var_nbr++;
            trv_tbl->nsm[idx_nsm].mbr[mbr_nbr].var_nm_fll=(char **)nco_realloc(trv_tbl->nsm[idx_nsm].mbr[mbr_nbr].var_nm_fll,trv_tbl->nsm[idx_nsm].mbr[mbr_nbr].var_nbr*sizeof(char *));
            trv_tbl->nsm[idx_nsm].mbr[mbr_nbr].var_nm_fll[idx_tpl]=(char *)strdup(var_nm_fll);

            if(nco_dbg_lvl_get() == nco_dbg_old) (void)fprintf(stdout,"%s: DEBUG %s inserted ensemble variable <%s>\n",nco_prg_nm_get(),fnc_nm,trv_tbl->nsm[idx_nsm].mbr[mbr_nbr].var_nm_fll[idx_tpl]);             

            /* Free */
            var_nm_fll=(char *)nco_free(var_nm_fll);

          } /* Insert members by building name from group and template */

          /* Mark fixed templates as non extracted */
          if(flg_fix_xtr){
            /* List of fixed templates  */
            for(int idx_skp=0;idx_skp<trv_tbl->nsm[idx_nsm].skp_nbr;idx_skp++){
              /* Get variable  */
              trv_sct *var_trv=trv_tbl_var_nm_fll(trv_tbl->nsm[idx_nsm].skp_nm_fll[idx_skp],trv_tbl);
              /* Define variable full name (using group name and relative name of fixed template) */
              char *skp_nm_fll=nco_bld_nm_fll(trv.nm_fll,var_trv->nm);
              /* Mark the skip names as non extracted variables */ 
              (void)trv_tbl_mrk_xtr(skp_nm_fll,False,trv_tbl); 
              /*And its group too... */ 
              (void)trv_tbl_mrk_grp_xtr(trv.nm_fll,False,trv_tbl); 
              /* Free */
              skp_nm_fll=(char *)nco_free(skp_nm_fll);
            } /* List of fixed templates  */
          } /* Mark fixed templates as non extracted */
      } /* Match */
    } /* Loop table */
  } /* Loop ensembles */
  if(nco_dbg_lvl_get() >= nco_dbg_fl) nco_prn_nsm(trv_tbl);
} /* nco_bld_nsm() */

void
nco_wrt_atr
(const int nc_id,                    /* I [id] netCDF input file ID */
 const int grp_out_id,               /* I [id] netCDF output group ID */
 const int var_out_id,               /* I [id] netCDF output variable ID */
 const trv_sct *var_trv)             /* I [sct] traversal variable */
{
  /* Copy variable's attributes */
  int var_id; /* [id] Variable ID */
  int grp_id; /* [id] Group ID */

  /* Obtain group ID */
  (void)nco_inq_grp_full_ncid(nc_id,var_trv->grp_nm_fll,&grp_id);

  /* Get variable ID */
  (void)nco_inq_varid(grp_id,var_trv->nm,&var_id);

  (void)nco_att_cpy(grp_id,grp_out_id,var_id,var_out_id,True);
} /* nco_wrt_atr() */

void
nco_nsm_dfn_wrt                      /* [fnc] Define OR write ensemble fixed variables */
(const int nc_id,                    /* I [ID] netCDF input file ID */
 const int nc_out_id,                /* I [ID] netCDF output file ID */
 const cnk_sct * const cnk,          /* I [sct] Chunking structure */
 const int dfl_lvl,                  /* I [enm] Deflate level [0..9] */
 const gpe_sct * const gpe,          /* I [sct] GPE structure */
 const nco_bool flg_def,             /* I [flg] Define OR write */
 trv_tbl_sct * const trv_tbl)        /* I/O [sct] Traversal table */
{
  char *grp_out_fll;  /* [sng] Group name */

  int grp_id_in;      /* [ID] Group ID */
  int grp_id_out;     /* [ID] Group ID */

  /* Ensembles */
  for(int idx_nsm=0;idx_nsm<trv_tbl->nsm_nbr;idx_nsm++){

    /* Get output group */
    if(trv_tbl->nsm_sfx){
      /* Define new name by appending suffix (e.g., /cesm + _avg) */
      char *nm_fll_sfx=nco_bld_nsm_sfx(trv_tbl->nsm[idx_nsm].grp_nm_fll_prn,trv_tbl);
      /* Use then delete new name */
      if(gpe) grp_out_fll=nco_gpe_evl(gpe,nm_fll_sfx); else grp_out_fll=(char *)strdup(nm_fll_sfx);
      nm_fll_sfx=(char *)nco_free(nm_fll_sfx);
    }else{ /* Non suffix case */
      if(gpe) grp_out_fll=nco_gpe_evl(gpe,trv_tbl->nsm[idx_nsm].grp_nm_fll_prn); else grp_out_fll=(char *)strdup(trv_tbl->nsm[idx_nsm].grp_nm_fll_prn);
    } /* !trv_tbl->nsm_sfx */

    /* List of fixed templates */
    for(int idx_skp=0;idx_skp<trv_tbl->nsm[idx_nsm].skp_nbr;idx_skp++){

      /* Get variable  */
      trv_sct *var_trv=trv_tbl_var_nm_fll(trv_tbl->nsm[idx_nsm].skp_nm_fll[idx_skp],trv_tbl);

      /* Obtain group IDs */
      (void)nco_inq_grp_full_ncid(nc_id,var_trv->grp_nm_fll,&grp_id_in);
      (void)nco_inq_grp_full_ncid(nc_out_id,grp_out_fll,&grp_id_out);

      /* Define variable  */
      if(flg_def){
        int var_out_id=nco_cpy_var_dfn_trv(nc_id,nc_out_id,cnk,grp_out_fll,dfl_lvl,gpe,NULL,var_trv,NULL,0,trv_tbl);
        /* Copy attributes */
        (void)nco_wrt_atr(nc_id,grp_id_out,var_out_id,var_trv);
      } /* endif */

      /* Copy variable data */
      if(!flg_def) (void)nco_cpy_var_val_mlt_lmt_trv(grp_id_in,grp_id_out,(FILE *)NULL,NULL,var_trv);

      if(nco_dbg_lvl_get() >= nco_dbg_vrb && nco_dbg_lvl_get() != nco_dbg_dev) (void)fprintf(stdout,"%s: INFO creating fixed variables <%s> in ensemble parent group <%s>\n",nco_prg_nm_get(),trv_tbl->nsm[idx_nsm].skp_nm_fll[idx_skp],grp_out_fll);

    } /* List of fixed templates  */

    /* Memory management after current extracted group */
    if(grp_out_fll) grp_out_fll=(char *)nco_free(grp_out_fll);

  } /* Ensembles */

} /* nco_nsm_dfn_wrt() */

void
nco_nsm_ncr                           /* [fnc] Increase ensembles (more than 1 file cases) */
(const int nc_id,                     /* I [id] netCDF file ID ( new file ) */
 trv_tbl_sct * const trv_tbl)         /* I/O [sct] Traversal table */
{
  const char fnc_nm[]="nco_nsm_ncr()"; /* [sng] Function name */

  char **nm_lst_1;                     /* [sng] List of names */
  char *grp_nm_fll;                    /* I [sng] Full group name */
  char *grp_nm;                        /* I [sng] Group name */

  int nm_lst_1_nbr;                    /* [nbr] Number of items in list */
  int grp_id;                          /* [id] Group ID */
  int nbr_grp;                         /* [nbr] Number of sub-groups */
  int *grp_ids;                        /* [id] Sub-group IDs array */
  int mbr_srt;                         /* [nbr] Offset */
  int rcd=NC_NOERR;                    /* [rcd] Return code */
  int nbr_dmn_var_2;                   /* [nbr] Number of dimensions for variable */

  size_t grp_nm_lng;                   /* [nbr] Group name length */

  /* Loop over ensembles in table */
  for(int idx_nsm=0;idx_nsm<trv_tbl->nsm_nbr;idx_nsm++){ 

    /* Update offsets */
    mbr_srt=trv_tbl->nsm[idx_nsm].mbr_end;
    trv_tbl->nsm[idx_nsm].mbr_srt=mbr_srt;

    if(nco_dbg_lvl_get() >= nco_dbg_dev){
      (void)fprintf(stdout,"%s: DEBUG %s <ensemble %d> <%s>\n",nco_prg_nm_get(),fnc_nm,idx_nsm,trv_tbl->nsm[idx_nsm].grp_nm_fll_prn);
    }

    /* Obtain group ID of current ensemble */
    rcd+=nco_inq_grp_full_ncid_flg(nc_id,trv_tbl->nsm[idx_nsm].grp_nm_fll_prn,&grp_id);

    /* Group must exist (file # call > 1 ), if not exit */
    if(rcd != NC_NOERR){
      (void)fprintf(stdout,"%s: ERROR ensemble <%s> does not exist\n",nco_prg_nm_get(),trv_tbl->nsm[idx_nsm].grp_nm_fll_prn); 
      (void)fprintf(stdout,"%s: List of ensembles is\n",nco_prg_nm_get()); 
      for(int idx=0;idx<trv_tbl->nsm_nbr;idx++){
        (void)fprintf(stdout,"%s: <%s>\n",nco_prg_nm_get(),trv_tbl->nsm[idx].grp_nm_fll_prn);
        nco_exit(EXIT_FAILURE);
      } 
    }

    /* Get number of sub-groups */
    (void)nco_inq_grps(grp_id,&nbr_grp,(int *)NULL);
    grp_ids=(int *)nco_malloc(nbr_grp*sizeof(int)); 
    (void)nco_inq_grps(grp_id,(int *)NULL,grp_ids);

    /* Loop sub-groups */
    for(int idx_grp=0;idx_grp<nbr_grp;idx_grp++){ 

      /* Get group name length */
      (void)nco_inq_grpname_len(grp_ids[idx_grp],&grp_nm_lng);
      grp_nm=(char *)nco_malloc(grp_nm_lng+1L);

      /* Get group name */
      (void)nco_inq_grpname(grp_ids[idx_grp],grp_nm);

      /* Construct full name  */
      grp_nm_fll=(char *)nco_malloc(grp_nm_lng+strlen(trv_tbl->nsm[idx_nsm].grp_nm_fll_prn)+2L);
      strcpy(grp_nm_fll,trv_tbl->nsm[idx_nsm].grp_nm_fll_prn);
      strcat(grp_nm_fll,"/");
      strcat(grp_nm_fll,grp_nm);

      /* Export list of variable names for group */
      (void)nco_grp_var_lst(nc_id,grp_nm_fll,&nm_lst_1,&nm_lst_1_nbr);

      int tpl_nbr=trv_tbl->nsm[idx_nsm].tpl_nbr;

      int mbr_nbr=trv_tbl->nsm[idx_nsm].mbr_nbr;
      trv_tbl->nsm[idx_nsm].mbr_nbr++;
      trv_tbl->nsm[idx_nsm].mbr=(nsm_grp_sct *)nco_realloc(trv_tbl->nsm[idx_nsm].mbr,(mbr_nbr+1)*sizeof(nsm_grp_sct));
      trv_tbl->nsm[idx_nsm].mbr[mbr_nbr].mbr_nm_fll=(char *)strdup(grp_nm_fll);
      trv_tbl->nsm[idx_nsm].mbr[mbr_nbr].var_nbr=0;
      trv_tbl->nsm[idx_nsm].mbr[mbr_nbr].var_nm_fll=NULL;

      /* Update offsets */
      trv_tbl->nsm[idx_nsm].mbr_end=trv_tbl->nsm[idx_nsm].mbr_nbr;

      /* Insert members by builing name from group and template */
      for(int idx_tpl=0;idx_tpl<tpl_nbr;idx_tpl++){ 

        /* Loop variables in group */
        for(int idx_var=0;idx_var<nm_lst_1_nbr;idx_var++){ 

          /* Match relative name of template variable and variable found in new file  */
          if(strcmp(nm_lst_1[idx_var],trv_tbl->nsm[idx_nsm].tpl_mbr_nm[idx_tpl]) == 0){

            /* Build new variable name */
            char *var_nm_fll=nco_bld_nm_fll(grp_nm_fll,nm_lst_1[idx_var]);

            /* Check variables from 2nd file (no table, using API) */

            /* Get number of dimensions */
            (void)nco_inq_var(grp_ids[idx_grp],idx_var,trv_tbl->nsm[idx_nsm].tpl_mbr_nm[idx_tpl],NULL,&nbr_dmn_var_2,(int *)NULL,(int *)NULL);

            /* Insert variable in table ensemble struct */
            trv_tbl->nsm[idx_nsm].mbr[mbr_nbr].var_nbr++;
            trv_tbl->nsm[idx_nsm].mbr[mbr_nbr].var_nm_fll=(char **)nco_realloc(trv_tbl->nsm[idx_nsm].mbr[mbr_nbr].var_nm_fll,trv_tbl->nsm[idx_nsm].mbr[mbr_nbr].var_nbr*sizeof(char *));
            trv_tbl->nsm[idx_nsm].mbr[mbr_nbr].var_nm_fll[idx_tpl]=(char *)strdup(var_nm_fll);

            if(nco_dbg_lvl_get() >= nco_dbg_dev) (void)fprintf(stdout,"%s: DEBUG %s inserted ensemble variable <%s>\n",nco_prg_nm_get(),fnc_nm,trv_tbl->nsm[idx_nsm].mbr[mbr_nbr].var_nm_fll[idx_tpl]);             

            var_nm_fll=(char *)nco_free(var_nm_fll);

            break;

          } /* Match relative name  */

        } /* Loop variables in group */
      } /* Insert members by builing name from group and template */

      /* Free list */
      for(int idx_nm=0;idx_nm<nm_lst_1_nbr;idx_nm++) nm_lst_1[idx_nm]=(char *)nco_free(nm_lst_1[idx_nm]);
      nm_lst_1=(char **)nco_free(nm_lst_1);
      grp_nm_fll=(char *)nco_free(grp_nm_fll);

    } /* Loop sub-groups */

    /* Clean up memory */
    grp_ids=(int *)nco_free(grp_ids);

  } /* Loop over ensembles in table */

  if(nco_dbg_lvl_get() >= nco_dbg_fl){
    (void)fprintf(stdout,"%s: New list of ensembles\n",nco_prg_nm_get()); 
    nco_prn_nsm(trv_tbl);
  }

} /* nco_nsm_ncr() */

nco_bool                               /* O [flg] True for match found */
nco_prc_rel_mch                        /* [fnc] Relative match of object in table 1 to table 2  */
(const int nc_id_1,                    /* I [id] netCDF input-file ID from file 1 */
 const int nc_id_2,                    /* I [id] netCDF input-file ID from file 2 */
 const int nc_out_id,                  /* I [id] netCDF output-file ID */
 const cnk_sct * const cnk,            /* I [sct] Chunking structure */
 const int dfl_lvl,                    /* I [enm] Deflate level [0..9] */
 const gpe_sct * const gpe,            /* I [sct] GPE structure */
 gpe_nm_sct *gpe_nm,                   /* I/O [sct] GPE name duplicate check array */
 int nbr_gpe_nm,                       /* I/O [nbr] Number of GPE entries */  
 const cnv_sct * const cnv,       /* I [flg] File adheres to NCAR CCM/CCSM/CF conventions */
 const int nco_op_typ,                 /* I [enm] Operation type (command line -y) */
 trv_sct * var_trv,                    /* I [sct] Table variable object (can be from table 1 or 2) */
 const nco_bool flg_tbl_1,             /* I [flg] Table variable object is from table1 for True, otherwise is from table 2 */
 const nco_bool flg_grp_1,             /* I [flg] Use table 1 as template for group creation on True, otherwise use table 2 */
 trv_tbl_sct * const trv_tbl_1,        /* I/O [sct] GTT (Group Traversal Table) */
 trv_tbl_sct * const trv_tbl_2,        /* I/O [sct] GTT (Group Traversal Table) */
 const nco_bool flg_dfn)               /* I [flg] Action type (True for define variables, False when write variables ) */
{
  nco_bool rel_mch; /* [flg] A match was found */

  rel_mch=False;

  if(flg_tbl_1){

    for(unsigned idx_tbl=0;idx_tbl<trv_tbl_2->nbr;idx_tbl++){
      if(trv_tbl_2->lst[idx_tbl].nco_typ == nco_obj_typ_var && !strcmp(var_trv->nm,trv_tbl_2->lst[idx_tbl].nm)){
        trv_sct *trv_2=&trv_tbl_2->lst[idx_tbl];
        rel_mch=True;
     
        if(nco_dbg_lvl_get() >= nco_dbg_var) (void)fprintf(stdout,"%s: INFO processing <%s> (file 1) and <%s> (file 2)\n",nco_prg_nm_get(),var_trv->nm_fll,trv_2->nm_fll);

        (void)nco_prc_cmn(nc_id_1,nc_id_2,nc_out_id,cnk,dfl_lvl,gpe,gpe_nm,nbr_gpe_nm,cnv,(nco_bool)False,(dmn_sct **)NULL,(int)0,nco_op_typ,var_trv,trv_2,trv_tbl_1,trv_tbl_2,flg_grp_1,flg_dfn);
      } /* A relative match was found */
    } /* Loop table  */

  }else if(!flg_tbl_1){

    for(unsigned idx_tbl=0;idx_tbl<trv_tbl_1->nbr;idx_tbl++){
      if(trv_tbl_1->lst[idx_tbl].nco_typ == nco_obj_typ_var && !strcmp(var_trv->nm,trv_tbl_1->lst[idx_tbl].nm)){
        trv_sct *trv_1=&trv_tbl_1->lst[idx_tbl];
        rel_mch=True;

        if(nco_dbg_lvl_get() >= nco_dbg_var) (void)fprintf(stdout,"%s: INFO processing <%s> (file 1) and <%s> (file 2)\n",nco_prg_nm_get(),trv_1->nm_fll,var_trv->nm_fll);

        (void)nco_prc_cmn(nc_id_1,nc_id_2,nc_out_id,cnk,dfl_lvl,gpe,gpe_nm,nbr_gpe_nm,cnv,(nco_bool)False,(dmn_sct **)NULL,(int)0,nco_op_typ,trv_1,var_trv,trv_tbl_1,trv_tbl_2,flg_grp_1,flg_dfn);
      } /* A relative match was found */
    } /* Loop table  */
  } /* !flg_tbl_1 */

  return rel_mch;

} /* nco_prc_rel_mch() */

void                          
nco_cmn_var                            /* [fnc] Does the list of common names contain absolute/relative variables match? */
(const trv_tbl_sct * const trv_tbl_1,  /* I [sct] GTT (Group Traversal Table) */
 const trv_tbl_sct * const trv_tbl_2,  /* I [sct] GTT (Group Traversal Table) */
 const nco_cmn_t * const cmn_lst,      /* I [sct] List of common names */
 const int nbr_cmn_nm,                 /* I [nbr] Number of common names entries */
 nco_bool *flg_cmn_abs,                /* I/O [flg] Does the list of common names contain absolute variables match? */
 nco_bool *flg_cmn_rel)                /* I/O [flg] Does the list of common names contain relative variables match? */
{
  /* Purpose: Does the list of *any* common names contain absolute/relative variables match? (ncbo only) */

  nco_bool has_mch;  /* [flg] A relative match was found in file 1 or 2 */

  *flg_cmn_abs=False;
  *flg_cmn_rel=False;

  /* Process objects in list */
  for(int idx_cmn=0;idx_cmn<nbr_cmn_nm;idx_cmn++){

    trv_sct *trv_1;    /* [sct] Table object */
    trv_sct *trv_2;    /* [sct] Table object */

    /* Inquire existence of these (full names, the common list contains group and variables) */
    trv_1=trv_tbl_var_nm_fll(cmn_lst[idx_cmn].nm,trv_tbl_1);
    trv_2=trv_tbl_var_nm_fll(cmn_lst[idx_cmn].nm,trv_tbl_2);

    /* Both variables exist in same location, both are to extract */
    if(trv_1 && trv_2 && trv_1->flg_xtr && trv_2->flg_xtr && cmn_lst[idx_cmn].flg_in_fl[0] && cmn_lst[idx_cmn].flg_in_fl[1]){
      *flg_cmn_abs=True;
    }

    /* Object exists and is flagged for extraction only in file 1 */
    else if(trv_1 && trv_1->flg_xtr && cmn_lst[idx_cmn].flg_in_fl[0] == True && cmn_lst[idx_cmn].flg_in_fl[1] == False ){

      nco_bool flg_tbl_1=True;
      has_mch=nco_rel_mch(trv_1,flg_tbl_1,trv_tbl_1,trv_tbl_2);
      *flg_cmn_rel=has_mch;
    }

    /* Object exists and is flagged for extraction only in file 2 */
    else if(trv_2 && trv_2->flg_xtr && cmn_lst[idx_cmn].flg_in_fl[0] == False && cmn_lst[idx_cmn].flg_in_fl[1] == True){

      nco_bool flg_tbl_1=False;
      has_mch=nco_rel_mch(trv_2,flg_tbl_1,trv_tbl_1,trv_tbl_2);
      *flg_cmn_rel=has_mch;
    }

  } /* Process objects in list */

} /* nco_cmn_var() */

void                                               
nco_prc_cmn_var_nm_fll                 /* [fnc] Process (define, write) absolute variables in both files (same path) (ncbo) */
(const int nc_id_1,                    /* I [id] netCDF input-file ID */
 const int nc_id_2,                    /* I [id] netCDF input-file ID */
 const int nc_out_id,                  /* I [id] netCDF output-file ID */
 const cnk_sct * const cnk,            /* I [sct] Chunking structure */
 const int dfl_lvl,                    /* I [enm] Deflate level [0..9] */
 const gpe_sct * const gpe,            /* I [sct] GPE structure */
 gpe_nm_sct *gpe_nm,                   /* I/O [sct] GPE name duplicate check array */
 int nbr_gpe_nm,                       /* I/O [nbr] Number of GPE entries */  
 const cnv_sct * const cnv,       /* I [flg] File adheres to NCAR CCM/CCSM/CF conventions */
 const int nco_op_typ,                 /* I [enm] Operation type (command line -y) */
 trv_tbl_sct * const trv_tbl_1,        /* I/O [sct] GTT (Group Traversal Table) */
 trv_tbl_sct * const trv_tbl_2,        /* I/O [sct] GTT (Group Traversal Table) */
 const nco_cmn_t * const cmn_lst,      /* I [sct] List of common names */
 const int nbr_cmn_nm,                 /* I [nbr] Number of common names entries */
 const nco_bool flg_dfn)               /* I [flg] Action type (True for define variables, False for write variables ) */
{
  /* Purpose: Process (define, write) absolute variables in both files (same path) (ncbo). NB: using table 1 as template */

  nco_bool flg_grp_1=True; /* [flg] Use table 1 as template for group creation */

  /* Process objects in list */
  for(int idx_cmn=0;idx_cmn<nbr_cmn_nm;idx_cmn++){

    trv_sct *trv_1;    /* [sct] Table object */
    trv_sct *trv_2;    /* [sct] Table object */

    /* Inquire existence of these (full names, the common list contains group and variables) */
    trv_1=trv_tbl_var_nm_fll(cmn_lst[idx_cmn].nm,trv_tbl_1);
    trv_2=trv_tbl_var_nm_fll(cmn_lst[idx_cmn].nm,trv_tbl_2);

    /* Both variables exist in same location, both are to extract */
    if(trv_1 && trv_2 && trv_1->flg_xtr && trv_2->flg_xtr){

      if(nco_dbg_lvl_get() >= nco_dbg_var) (void)fprintf(stdout,"%s: INFO common variable to output <%s>\n",nco_prg_nm_get(),trv_1->nm_fll); 

      /* Process common object */
      (void)nco_prc_cmn(nc_id_1,nc_id_2,nc_out_id,cnk,dfl_lvl,gpe,gpe_nm,nbr_gpe_nm,cnv,(nco_bool)False,(dmn_sct **)NULL,(int)0,nco_op_typ,trv_1,trv_2,trv_tbl_1,trv_tbl_2,flg_grp_1,flg_dfn);

    } /* Both variables exist in same location, both are to extract */
  } /* Process objects in list */

} /* nco_prc_cmn_var_nm_fll() */

void                          
nco_cmn_nsm_var                        /* [fnc] Common variable exist in ensembles (ncbo only) */
(nco_bool *flg_var_cmn,                /* I/O [flg] Common variable exists */
 nco_bool *flg_var_cmn_rth,            /* I/O [flg] Common variable exists at root */
 nm_lst_sct **var_nm,                  /* I/O [sct] Array of common variable names */ 
 nm_lst_sct **var_nm_rth,              /* I/O [sct] Array of common variable names at root */ 
 const trv_tbl_sct * const trv_tbl_1,  /* I [sct] GTT (Group Traversal Table) */
 const trv_tbl_sct * const trv_tbl_2)  /* I [sct] GTT (Group Traversal Table) */
{
  *flg_var_cmn=False;
  *flg_var_cmn_rth=False;

  (*var_nm)=(nm_lst_sct *)nco_malloc(sizeof(nm_lst_sct));
  (*var_nm)->nbr=0;
  (*var_nm)->lst=NULL; /* Must be NULL to nco_realloc() correct handling */

  (*var_nm_rth)=(nm_lst_sct *)nco_malloc(sizeof(nm_lst_sct));
  (*var_nm_rth)->nbr=0;
  (*var_nm_rth)->lst=NULL; /* Must be NULL to nco_realloc() correct handling */

  int nbr_var=0;
  int nbr_var_rth=0;

  /* Loop over ensembles in table 1 */
  for(int idx_nsm_1=0;idx_nsm_1<trv_tbl_1->nsm_nbr;idx_nsm_1++){ 

    /* Loop over ensemble members in table 1 */
    for(int idx_mbr_1=0;idx_mbr_1<trv_tbl_1->nsm[idx_nsm_1].mbr_nbr;idx_mbr_1++){ 

      /* Loop over variables in table 1 */
      for(int idx_var=0;idx_var<trv_tbl_1->nsm[idx_nsm_1].mbr[idx_mbr_1].var_nbr;idx_var++){

        char *var_nm_fll=trv_tbl_1->nsm[idx_nsm_1].mbr[idx_mbr_1].var_nm_fll[idx_var];

        /* Get GTT object */
        trv_sct *var_trv=trv_tbl_var_nm_fll(var_nm_fll,trv_tbl_1);

        /* Loop over table 2 */
        for(unsigned idx_tbl_2=0;idx_tbl_2<trv_tbl_2->nbr;idx_tbl_2++){

          trv_sct var_trv_2=trv_tbl_2->lst[idx_tbl_2];

          /* Match template name from table 2 in table 1 */
          if(var_trv_2.nco_typ == nco_obj_typ_var && strcmp(var_trv->nm,var_trv_2.nm) == 0){

            /* Root variable */
            if(var_trv_2.grp_dpt == 0){

              *flg_var_cmn_rth=True;

              /* Add to list  */
              (*var_nm_rth)->lst=(nm_sct *)nco_realloc((*var_nm_rth)->lst,(nbr_var_rth+1)*sizeof(nm_sct));
              (*var_nm_rth)->lst[nbr_var_rth].nm=strdup(var_trv_2.nm_fll);
              (*var_nm_rth)->nbr++;
              nbr_var_rth++;

            } else {

              *flg_var_cmn=True;

              /* Add to list  */
              (*var_nm)->lst=(nm_sct *)nco_realloc((*var_nm)->lst,(nbr_var+1)*sizeof(nm_sct));
              (*var_nm)->lst[nbr_var].nm=strdup(var_trv_2.nm_fll);
              (*var_nm)->nbr++;
              nbr_var++;

            } /* ! Root variable */

            break;

          } /* Match template name from table 2 in table 1 */
        } /* Loop over table 2 */
      } /* Loop variables table 1 */
    } /* Loop ensemble members table 1 */

  }/* Loop ensembles table 1  */

}/* nco_cmn_nsm_var() */

void                          
nco_grp_brd                            /* [fnc] Group broadcasting (ncbo only) */
(const int nc_id_1,                    /* I [id] netCDF input-file ID */
 const int nc_id_2,                    /* I [id] netCDF input-file ID */
 const int nc_out_id,                  /* I [id] netCDF output-file ID */
 const cnk_sct * const cnk,            /* I [sct] Chunking structure */
 const int dfl_lvl,                    /* I [enm] Deflate level [0..9] */
 const gpe_sct * const gpe,            /* I [sct] GPE structure */
 gpe_nm_sct *gpe_nm,                   /* I/O [sct] GPE name duplicate check array */
 int nbr_gpe_nm,                       /* I/O [nbr] Number of GPE entries */  
 const cnv_sct * const cnv,       /* I [flg] File adheres to NCAR CCM/CCSM/CF conventions */
 const int nco_op_typ,                 /* I [enm] Operation type (command line -y) */
 trv_tbl_sct * const trv_tbl_1,        /* I/O [sct] GTT (Group Traversal Table) */
 trv_tbl_sct * const trv_tbl_2,        /* I/O [sct] GTT (Group Traversal Table) */
 const nco_bool flg_dfn)               /* I [flg] Action type (True for define variables, False for write variables ) */
{
  /* Purpose: Group broadcasting OR variable matching (ncbo only) */

  /* Broadcasting: file 1 contains multiple groups each with the variable v1, while file 2 contains v1
  only in its top-level (i.e., root) group. Then ncbo will replicate the group structure of file 1
  in the output file, file 3. Each group in file 3 contains the output of the corresponding
  group in file 1 operating on the data in the single group in file 2. */

  const char fnc_nm[]="nco_grp_brd()";  /* [sng] Function name */

  int nbr_cmn_nm=0;                      /* [nbr] Number of common entries */

  nco_bool flg_cmn_abs;                  /* [flg] Is there a variable with same absolute path in both files? */
  nco_bool flg_cmn_rel;                  /* [flg] Is there a variable with same relative name */
  nco_bool flg_nsm_fl_1;                 /* [flg] File 1 contains ensemble members */
  nco_bool flg_nsm_fl_2;                 /* [flg] File 2 contains ensemble members */
  nco_bool flg_var_cmn;                  /* [flg] Common variable exists */
  nco_bool flg_var_cmn_rth;              /* [flg] Common variable exists at root */
  nco_bool flg_grp_1;                    /* [flg] Use table 1 as template for group creation on True, otherwise use table 2 */
  nco_bool flg_nsm_att_1;                /* [flg] "ensemble" attribute exists in table 1 */
  nco_bool flg_nsm_att_2;                /* [flg] "ensemble" attribute exists in table 2 */

  nco_cmn_t *cmn_lst=NULL;               /* [lst] A list of common variable names */ 
  nm_lst_sct *var_nm=NULL;               /* [lst] A list of variable names */
  nm_lst_sct *var_nm_rth=NULL;           /* [lst] A list of variable names found at root */
  nm_lst_sct *nsm_grp_nm_fll_prn_1=NULL; /* I/O [sct] Array of ensemble paths read in the attributes */ 
  nm_lst_sct *nsm_grp_nm_fll_prn_2=NULL; /* I/O [sct] Array of ensemble paths read in the attributes */ 

  /* Sanity check */
  assert(nco_prg_id_get() == ncbo);

  /* Match two tables (find common objects) and export common objects */
  (void)trv_tbl_mch(trv_tbl_1,trv_tbl_2,&cmn_lst,&nbr_cmn_nm);

  /* Is there a variable with same absolute/relative path in both files? */
  (void)nco_cmn_var(trv_tbl_1,trv_tbl_2,cmn_lst,nbr_cmn_nm,&flg_cmn_abs,&flg_cmn_rel); 

  /* Inquire if ensembles have "ensemble" attribute (meaning they were done by ncge already) */
  (void)nco_nsm_att(nc_id_1,trv_tbl_1,&flg_nsm_att_1,&nsm_grp_nm_fll_prn_1); 
  (void)nco_nsm_att(nc_id_2,trv_tbl_2,&flg_nsm_att_2,&nsm_grp_nm_fll_prn_2);

  /* Process variables with same absolute path in both files. Do them and return */
  if(flg_cmn_abs) (void)nco_prc_cmn_var_nm_fll(nc_id_1,nc_id_2,nc_out_id,cnk,dfl_lvl,gpe,gpe_nm,nbr_gpe_nm,cnv,nco_op_typ,trv_tbl_1,trv_tbl_2,cmn_lst,nbr_cmn_nm,flg_dfn);           

  /* Inquire about group broadcasting (ensembles and not ensembles) */

  /* Inquire about ensembles */
  if(trv_tbl_1->nsm_nbr) flg_nsm_fl_1=True; else flg_nsm_fl_1=False;
  if(trv_tbl_2->nsm_nbr) flg_nsm_fl_2=True; else flg_nsm_fl_2=False;

  /* There are ensembles somewhere */
  if(flg_nsm_fl_1 == True || flg_nsm_fl_2 == True){

    /* File 1 has ensembles */
    if(flg_nsm_fl_1 == True){

      if(nco_dbg_lvl_get() >= nco_dbg_dev){
        (void)fprintf(stdout,"%s: DEBUG %s ensembles from file 1\n",nco_prg_nm_get(),fnc_nm);
        nco_prn_nsm(trv_tbl_1);             
      } /* endif dbg */

      /* File 2 has ensembles */
      if(flg_nsm_fl_2 == True){

        if(nco_dbg_lvl_get() >= nco_dbg_dev){
          (void)fprintf(stdout,"%s: DEBUG %s ensembles from file 2\n",nco_prg_nm_get(),fnc_nm);
          nco_prn_nsm(trv_tbl_2);             
        } /* endif dbg */

        /* File 2 has ensembles in "special" places, defined in attributes */
        if(flg_nsm_att_2){

          if(nco_dbg_lvl_get() >= nco_dbg_dev){
            (void)fprintf(stdout,"%s: DEBUG %s ensemble names read from attributes from file 2\n",nco_prg_nm_get(),fnc_nm);
            for(int idx_nm=0;idx_nm<nsm_grp_nm_fll_prn_2->nbr;idx_nm++) (void)fprintf(stdout,"%s: DEBUG %s %s\n",nco_prg_nm_get(),fnc_nm,nsm_grp_nm_fll_prn_2->lst[idx_nm].nm);          
          } /* endif dbg */

          /* Use table 1 as template for group creation */
          flg_grp_1=True;

          /* Process (define, write) variables belonging to ensembles in *both* files (special attribute version)   */
          (void)nco_prc_cmn_nsm_att(nc_id_1,nc_id_2,nc_out_id,cnk,dfl_lvl,gpe,gpe_nm,nbr_gpe_nm,cnv,nco_op_typ,trv_tbl_1,trv_tbl_2,flg_grp_1,flg_dfn,nsm_grp_nm_fll_prn_2);              

          /* File 2 has ensembles in the expected places */
        }else{
          /* ncbo -O mdl_1.nc mdl_2.nc out.nc */
          /* Use table 1 as template for group creation */
          flg_grp_1=True;
          /* Process (define, write) variables belonging to ensembles in *both* files  */
          (void)nco_prc_cmn_nsm(nc_id_1,nc_id_2,nc_out_id,cnk,dfl_lvl,gpe,gpe_nm,nbr_gpe_nm,cnv,nco_op_typ,trv_tbl_1,trv_tbl_2,flg_grp_1,flg_dfn);
        } /* File 2 has ensembles in the expected places */

      }else if(!flg_nsm_fl_2){
        /* File 2 does NOT have ensembles */
        /* Inquire about file 2 having a common object from list of file 1 ensembles  */
        (void)nco_cmn_nsm_var(&flg_var_cmn,&flg_var_cmn_rth,&var_nm,&var_nm_rth,trv_tbl_1,trv_tbl_2);

        /* Common variables at root */
        if(flg_var_cmn_rth){
          /* file 2 has a common object at root  */
          /* ncbo -O mdl_1.nc obs.nc out.nc */
          /* Use table 1 as template for group creation */
          flg_grp_1=True;

          /* Process (define, write) variables belonging to ensembles only in 1 file  */
          (void)nco_prc_nsm(nc_id_1,nc_id_2,nc_out_id,cnk,dfl_lvl,gpe,gpe_nm,nbr_gpe_nm,cnv,nco_op_typ,trv_tbl_1,trv_tbl_2,var_nm_rth,flg_grp_1,flg_dfn);              
          /* Common variables not at root */
        }else if(flg_var_cmn){
          /* Use table 1 as template for group creation */
          flg_grp_1=True;

          /* Process (define, write) variables belonging to ensembles only in 1 file  */
          (void)nco_prc_nsm(nc_id_1,nc_id_2,nc_out_id,cnk,dfl_lvl,gpe,gpe_nm,nbr_gpe_nm,cnv,nco_op_typ,trv_tbl_1,trv_tbl_2,var_nm,flg_grp_1,flg_dfn);              
        }else{
          /* file 2 has no common objects   */
          (void)fprintf(stdout,"%s: ERROR no common variables found. HINT: %s expects to find at least one variable of the same name in similar locations in both input files. When such variables are not found in identical locations (i.e., on the same path) then %s attempts group broadcasting to find comparable variables in sub-groups and ensembles. This search for comparable variables has failed. Read more about group broadcasting at http://nco.sf.net/nco.html#grp_brd\n",nco_prg_nm_get(),nco_prg_nm_get(),nco_prg_nm_get());
          nco_exit(EXIT_FAILURE);
        } /* ! flg_var_cmn_rth */

      } /* File 2 does NOT have ensembles */

    }else if(!flg_nsm_fl_1){
      /* File 1 does NOT have ensembles */
      /* File 2 has ensembles */
      if(flg_nsm_fl_2){

        if(nco_dbg_lvl_get() >= nco_dbg_dev){
          (void)fprintf(stdout,"%s: DEBUG %s ensembles from file 2\n",nco_prg_nm_get(),fnc_nm);
          nco_prn_nsm(trv_tbl_2);             
        } /* endif */

        /* Inquire about file 1 having a common object from list of file 2 ensembles (NB: order of tables in parameter switched)  */
        (void)nco_cmn_nsm_var(&flg_var_cmn,&flg_var_cmn_rth,&var_nm,&var_nm_rth,trv_tbl_2,trv_tbl_1);

         /* Common variables at root */
        if(flg_var_cmn_rth){
          /* file 1 has a common object at root  */
          /* ncbo -O obs.nc mdl_1.nc  out.nc */
          /* Do NOT use table 1 as template for group creation */
          flg_grp_1=False;

          /* Process (define, write) variables belonging to ensembles only in 1 file  */
          (void)nco_prc_nsm(nc_id_1,nc_id_2,nc_out_id,cnk,dfl_lvl,gpe,gpe_nm,nbr_gpe_nm,cnv,nco_op_typ,trv_tbl_1,trv_tbl_2,var_nm_rth,flg_grp_1,flg_dfn);              
          /* Common variables not at root */
        }else if(flg_var_cmn){
          /* file 1 has a common object not at root  */
          /* ncra  -Y ncge -O mdl_3.nc ncge_out.nc 
          ncbo  -O --op_typ=add ncge_out.nc  mdl_3.nc  out.nc */

          /* Do NOT use table 1 as template for group creation */
          flg_grp_1=False;

          /* Process (define, write) variables belonging to ensembles only in 1 file  */
          (void)nco_prc_nsm(nc_id_1,nc_id_2,nc_out_id,cnk,dfl_lvl,gpe,gpe_nm,nbr_gpe_nm,cnv,nco_op_typ,trv_tbl_1,trv_tbl_2,var_nm,flg_grp_1,flg_dfn);              
        }else{
          /* file 1 has no common objects */
          (void)fprintf(stdout,"%s: ERROR no common variables found. HINT: %s expects to find at least one variable of the same name in similar locations in both input files. When such variables are not found in identical locations (i.e., on the same path) then %s attempts group broadcasting to find comparable variables in sub-groups and ensembles. This search for comparable variables has failed. Read more about group broadcasting at http://nco.sf.net/nco.html#grp_brd\n",nco_prg_nm_get(),nco_prg_nm_get(),nco_prg_nm_get());
          nco_exit(EXIT_FAILURE);
        } /* ! flg_var_cmn_rth */
      } /* File 2 has ensembles */
    } /* File 1 does NOT have ensembles */
  } /* There are ensembles somewhere */

  /* There are NOT ensembles anywhere, but there are relative matches */
  if(!flg_nsm_fl_1 && !flg_nsm_fl_2 && flg_cmn_rel){
    if(nco_dbg_lvl_get() >= nco_dbg_dev) (void)fprintf(stdout,"%s: DEBUG %s Processing relative matches\n",nco_prg_nm_get(),fnc_nm);

    /* Process relative common objects (define or write) */
    (void)nco_prc_rel_cmn_nm(nc_id_1,nc_id_2,nc_out_id,cnk,dfl_lvl,gpe,gpe_nm,nbr_gpe_nm,cnv,nco_op_typ,trv_tbl_1,trv_tbl_2,cmn_lst,nbr_cmn_nm,flg_dfn);
  } /* There are NOT ensembles anywhere, but there are relative matches */

  /* Memory management for common names list */
  for(int idx_cmn=0;idx_cmn<nbr_cmn_nm;idx_cmn++) cmn_lst[idx_cmn].nm=(char *)nco_free(cmn_lst[idx_cmn].nm);
  if(nbr_cmn_nm > 0) cmn_lst=(nco_cmn_t *)nco_free(cmn_lst);

  if(var_nm){
    for(int idx=0;idx<var_nm->nbr;idx++) var_nm->lst[idx].nm=(char *)nco_free(var_nm->lst[idx].nm);
    var_nm=(nm_lst_sct *)nco_free(var_nm);
  } /* endif */

  if(var_nm_rth){
    for(int idx=0;idx<var_nm_rth->nbr;idx++) var_nm_rth->lst[idx].nm=(char *)nco_free(var_nm_rth->lst[idx].nm);
    var_nm_rth=(nm_lst_sct *)nco_free(var_nm_rth);
  } 

  for(int idx_nm=0;idx_nm<nsm_grp_nm_fll_prn_2->nbr;idx_nm++)
    nsm_grp_nm_fll_prn_2->lst[idx_nm].nm=(char *)nco_free(nsm_grp_nm_fll_prn_2->lst[idx_nm].nm);
  nsm_grp_nm_fll_prn_2=(nm_lst_sct *)nco_free(nsm_grp_nm_fll_prn_2);

  for(int idx_nm=0;idx_nm<nsm_grp_nm_fll_prn_1->nbr;idx_nm++)
    nsm_grp_nm_fll_prn_1->lst[idx_nm].nm=(char *)nco_free(nsm_grp_nm_fll_prn_1->lst[idx_nm].nm);   
  nsm_grp_nm_fll_prn_1=(nm_lst_sct *)nco_free(nsm_grp_nm_fll_prn_1);
} /* nco_grp_brd() */

void                                               
nco_prc_cmn_nsm                        /* [fnc] Process (define, write) variables belonging to ensembles in both files (ncbo) */
(const int nc_id_1,                    /* I [id] netCDF input-file ID */
 const int nc_id_2,                    /* I [id] netCDF input-file ID */
 const int nc_out_id,                  /* I [id] netCDF output-file ID */
 const cnk_sct * const cnk,            /* I [sct] Chunking structure */
 const int dfl_lvl,                    /* I [enm] Deflate level [0..9] */
 const gpe_sct * const gpe,            /* I [sct] GPE structure */
 gpe_nm_sct *gpe_nm,                   /* I/O [sct] GPE name duplicate check array */
 int nbr_gpe_nm,                       /* I/O [nbr] Number of GPE entries */  
 const cnv_sct * const cnv,       /* I [flg] File adheres to NCAR CCM/CCSM/CF conventions */
 const int nco_op_typ,                 /* I [enm] Operation type (command line -y) */
 trv_tbl_sct * const trv_tbl_1,        /* I/O [sct] GTT (Group Traversal Table) */
 trv_tbl_sct * const trv_tbl_2,        /* I/O [sct] GTT (Group Traversal Table) */
 const nco_bool flg_grp_1,             /* I [flg] Use table 1 as template for group creation on True, otherwise use table 2 */
 const nco_bool flg_dfn)               /* I [flg] Action type (True for define variables, False for write variables ) */
{
  /* Purpose: Process (define, write) variables belonging to ensembles in *both* files (both files have ensembles) */

  const char fnc_nm[]="nco_prc_cmn_nsm()"; /* [sng] Function name */

  trv_sct *trv_1;    /* [sct] Table object */
  trv_sct *trv_2;    /* [sct] Table object */

  if(flg_grp_1 == True){

    if(nco_dbg_lvl_get() >= nco_dbg_var){
      (void)fprintf(stdout,"%s: Processing ensembles from file 1\n",nco_prg_nm_get());
    }

    /* Loop ensembles */
    for(int idx_nsm=0;idx_nsm<trv_tbl_1->nsm_nbr;idx_nsm++){

      if(nco_dbg_lvl_get() >= nco_dbg_dev){
        (void)fprintf(stdout,"%s: DEBUG %s <ensemble %d> <%s>\n",nco_prg_nm_get(),fnc_nm,
          idx_nsm,trv_tbl_1->nsm[idx_nsm].grp_nm_fll_prn);
      }

      /* Loop group members */
      for(int idx_mbr=0;idx_mbr<trv_tbl_1->nsm[idx_nsm].mbr_nbr;idx_mbr++){

        if(nco_dbg_lvl_get() >= nco_dbg_dev){
          (void)fprintf(stdout,"%s: DEBUG %s \t <member %d> <%s>\n",nco_prg_nm_get(),fnc_nm,
            idx_mbr,trv_tbl_1->nsm[idx_nsm].mbr[idx_mbr].mbr_nm_fll); 
        }

        /* Loop variables */
        for(int idx_var=0;idx_var<trv_tbl_1->nsm[idx_nsm].mbr[idx_mbr].var_nbr;idx_var++){

          if(nco_dbg_lvl_get() >= nco_dbg_dev){
            (void)fprintf(stdout,"%s: DEBUG %s \t <variable %d> <%s>\n",nco_prg_nm_get(),fnc_nm,
              idx_var,trv_tbl_1->nsm[idx_nsm].mbr[idx_mbr].var_nm_fll[idx_var]); 
          }

          /* Inquire existence of these objects in tables  */
          trv_1=trv_tbl_var_nm_fll(trv_tbl_1->nsm[idx_nsm].mbr[idx_mbr].var_nm_fll[idx_var],trv_tbl_1);

          assert(trv_1);

          /* Since we're using table 1 as template, for table 2 object has to be searched (using relative name and ensemble parent name) */
          trv_2=trv_tbl_nsm_nm(trv_1->nm,trv_tbl_1->nsm[idx_nsm].grp_nm_fll_prn,trv_tbl_2);

          /* If match object in table 2 does not exist, that's an error  */
        
          if(!trv_2){
            (void)fprintf(stdout,"%s: ERROR No match variable found for <%s>\n",nco_prg_nm_get(),trv_1->nm_fll);
            nco_exit(EXIT_FAILURE);
          }
         
          /* Both variables exist  */
          if(trv_1 && trv_2){

            if(nco_dbg_lvl_get() >= nco_dbg_var) (void)fprintf(stdout,"%s: INFO common variable to output <%s>\n",nco_prg_nm_get(),trv_1->nm_fll); 

            /* Process common object */
            (void)nco_prc_cmn(nc_id_1,nc_id_2,nc_out_id,cnk,dfl_lvl,gpe,gpe_nm,nbr_gpe_nm,cnv,(nco_bool)False,(dmn_sct **)NULL,(int)0,nco_op_typ,trv_1,trv_2,trv_tbl_1,trv_tbl_2,flg_grp_1,flg_dfn);

          } /* Both variables exist */

        } /* Loop variables */

        /* List of fixed templates  */
        for(int idx_skp=0;idx_skp<trv_tbl_1->nsm[idx_nsm].skp_nbr;idx_skp++){

          /* Get variable  */
          trv_sct *var_trv=trv_tbl_var_nm_fll(trv_tbl_1->nsm[idx_nsm].skp_nm_fll[idx_skp],trv_tbl_1);

          /* Define variable full name (using group name and relative name of fixed template) */
          char *skp_nm_fll=nco_bld_nm_fll(trv_tbl_1->nsm[idx_nsm].mbr[idx_mbr].mbr_nm_fll,var_trv->nm);

          /* Get variable  */
          trv_sct *skp_trv=trv_tbl_var_nm_fll(skp_nm_fll,trv_tbl_1);

	  /* Define/write fixed variables (ncbo) */
          if(skp_trv) (void)nco_fix_dfn_wrt(nc_id_1,nc_out_id,cnk,dfl_lvl,gpe,gpe_nm,nbr_gpe_nm,skp_trv,trv_tbl_1,flg_dfn);

        } /* List of fixed templates  */

      } /* Loop group members */
    } /* Loop ensembles */

  }else if(flg_grp_1 == False){

  } /* ! flg_grp_1 */

} /* nco_prc_cmn_nsm() */

void                                               
nco_prc_nsm                            /* [fnc] Process (define, write) variables belonging to ensembles only in 1 file (ncbo) */
(const int nc_id_1,                    /* I [id] netCDF input-file ID */
 const int nc_id_2,                    /* I [id] netCDF input-file ID */
 const int nc_out_id,                  /* I [id] netCDF output-file ID */
 const cnk_sct * const cnk,            /* I [sct] Chunking structure */
 const int dfl_lvl,                    /* I [enm] Deflate level [0..9] */
 const gpe_sct * const gpe,            /* I [sct] GPE structure */
 gpe_nm_sct *gpe_nm,                   /* I/O [sct] GPE name duplicate check array */
 int nbr_gpe_nm,                       /* I/O [nbr] Number of GPE entries */  
 const cnv_sct * const cnv,       /* I [flg] File adheres to NCAR CCM/CCSM/CF conventions */
 const int nco_op_typ,                 /* I [enm] Operation type (command line -y) */
 trv_tbl_sct * const trv_tbl_1,        /* I/O [sct] GTT (Group Traversal Table) */
 trv_tbl_sct * const trv_tbl_2,        /* I/O [sct] GTT (Group Traversal Table) */
 const nm_lst_sct * const var_lst,     /* I [sct] Array of common variable names from file not having ensembles */ 
 const nco_bool flg_grp_1,             /* I [flg] Use table 1 as template for group creation on True, otherwise use table 2 */
 const nco_bool flg_dfn)               /* I [flg] Action type (True for define variables, False for write variables ) */
{

  const char fnc_nm[]="nco_prc_nsm()"; /* [sng] Function name */

  trv_sct *trv_1;    /* [sct] Table object */
  trv_sct *trv_2;    /* [sct] Table object */

  /* Using table 1 as template */
  if(flg_grp_1 == True){

    if(nco_dbg_lvl_get() >= nco_dbg_var){
      (void)fprintf(stdout,"%s: Processing ensembles from table 1\n",nco_prg_nm_get());
    }

    /* Loop ensembles */
    for(int idx_nsm=0;idx_nsm<trv_tbl_1->nsm_nbr;idx_nsm++){

      if(nco_dbg_lvl_get() >= nco_dbg_dev){
        (void)fprintf(stdout,"%s: DEBUG %s <ensemble %d> <%s>\n",nco_prg_nm_get(),fnc_nm,
          idx_nsm,trv_tbl_1->nsm[idx_nsm].grp_nm_fll_prn);
      }

      /* Loop group members */
      for(int idx_mbr=0;idx_mbr<trv_tbl_1->nsm[idx_nsm].mbr_nbr;idx_mbr++){

        if(nco_dbg_lvl_get() >= nco_dbg_dev){
          (void)fprintf(stdout,"%s: DEBUG %s \t <member %d> <%s>\n",nco_prg_nm_get(),fnc_nm,
            idx_mbr,trv_tbl_1->nsm[idx_nsm].mbr[idx_mbr].mbr_nm_fll); 
        }

        /* Loop variables */
        for(int idx_var=0;idx_var<trv_tbl_1->nsm[idx_nsm].mbr[idx_mbr].var_nbr;idx_var++){

          if(nco_dbg_lvl_get() >= nco_dbg_dev){
            (void)fprintf(stdout,"%s: DEBUG %s \t <variable %d> <%s>\n",nco_prg_nm_get(),fnc_nm,
              idx_var,trv_tbl_1->nsm[idx_nsm].mbr[idx_mbr].var_nm_fll[idx_var]); 
          }

          trv_1=NULL;
          trv_2=NULL;

          /* Inquire existence of these objects in tables  */
          trv_1=trv_tbl_var_nm_fll(trv_tbl_1->nsm[idx_nsm].mbr[idx_mbr].var_nm_fll[idx_var],trv_tbl_1);

          assert(trv_1);

          /* List of fixed templates  */
          for(int idx_skp=0;idx_skp<trv_tbl_1->nsm[idx_nsm].skp_nbr;idx_skp++){

            /* Get variable  */
            trv_sct *var_trv=trv_tbl_var_nm_fll(trv_tbl_1->nsm[idx_nsm].skp_nm_fll[idx_skp],trv_tbl_1);

            /* Define variable full name (using group name and relative name of fixed template) */
            char *skp_nm_fll=nco_bld_nm_fll(trv_tbl_1->nsm[idx_nsm].mbr[idx_mbr].mbr_nm_fll,var_trv->nm);

            /* Get variable  */
            trv_sct *skp_trv=trv_tbl_var_nm_fll(skp_nm_fll,trv_tbl_1);

	    /* Define/write fixed variables (ncbo) */
            if(skp_trv) (void)nco_fix_dfn_wrt(nc_id_1,nc_out_id,cnk,dfl_lvl,gpe,gpe_nm,nbr_gpe_nm,skp_trv,trv_tbl_1,flg_dfn);

          } /* List of fixed templates  */

          /* Loop list of variables from other file */
          for(int idx_var_2=0;idx_var_2<var_lst->nbr;idx_var_2++){

            /* Inquire existence of object in other table  */
            trv_2=trv_tbl_var_nm_fll(var_lst->lst[idx_var_2].nm,trv_tbl_2);

            assert(trv_2);

            /* Match name (relative, since they are not absolute matches)  */
            if(strcmp(trv_2->nm,trv_1->nm) == 0){

              if(nco_dbg_lvl_get() >= nco_dbg_var) (void)fprintf(stdout,"%s: INFO common variable to output <%s>\n",nco_prg_nm_get(),trv_1->nm_fll); 

              /* Process common object */
              (void)nco_prc_cmn(nc_id_1,nc_id_2,nc_out_id,cnk,dfl_lvl,gpe,gpe_nm,nbr_gpe_nm,cnv,(nco_bool)False,(dmn_sct **)NULL,(int)0,nco_op_typ,trv_1,trv_2,trv_tbl_1,trv_tbl_2,flg_grp_1,flg_dfn);

              break;
            } /* Match name  */
          } /* Loop list of variables from other file */

        } /* Loop variables */

       
      } /* Loop group members */
    } /* Loop ensembles */

    /* Using table 2 as template; same logic looping table 2 */

  } else if(flg_grp_1 == False){

    if(nco_dbg_lvl_get() >= nco_dbg_var){
      (void)fprintf(stdout,"%s: Processing ensembles from table 2\n",nco_prg_nm_get());
    }

    /* Loop ensembles */
    for(int idx_nsm=0;idx_nsm<trv_tbl_2->nsm_nbr;idx_nsm++){

      if(nco_dbg_lvl_get() >= nco_dbg_dev){
        (void)fprintf(stdout,"%s: DEBUG %s <ensemble %d> <%s>\n",nco_prg_nm_get(),fnc_nm,
          idx_nsm,trv_tbl_2->nsm[idx_nsm].grp_nm_fll_prn);
      }

      /* Loop group members */
      for(int idx_mbr=0;idx_mbr<trv_tbl_2->nsm[idx_nsm].mbr_nbr;idx_mbr++){

        if(nco_dbg_lvl_get() >= nco_dbg_dev){
          (void)fprintf(stdout,"%s: DEBUG %s \t <member %d> <%s>\n",nco_prg_nm_get(),fnc_nm,
            idx_mbr,trv_tbl_2->nsm[idx_nsm].mbr[idx_mbr].mbr_nm_fll); 
        }

        /* Loop variables */
        for(int idx_var=0;idx_var<trv_tbl_2->nsm[idx_nsm].mbr[idx_mbr].var_nbr;idx_var++){

          if(nco_dbg_lvl_get() >= nco_dbg_dev){
            (void)fprintf(stdout,"%s: DEBUG %s \t <variable %d> <%s>\n",nco_prg_nm_get(),fnc_nm,
              idx_var,trv_tbl_2->nsm[idx_nsm].mbr[idx_mbr].var_nm_fll[idx_var]); 
          }

          trv_1=NULL;
          trv_2=NULL;

          /* Inquire existence of these objects in tables  */
          trv_2=trv_tbl_var_nm_fll(trv_tbl_2->nsm[idx_nsm].mbr[idx_mbr].var_nm_fll[idx_var],trv_tbl_2);

          assert(trv_2);

          /* List of fixed templates  */
          for(int idx_skp=0;idx_skp<trv_tbl_2->nsm[idx_nsm].skp_nbr;idx_skp++){

            /* Get variable  */
            trv_sct *var_trv=trv_tbl_var_nm_fll(trv_tbl_2->nsm[idx_nsm].skp_nm_fll[idx_skp],trv_tbl_2);

            /* Define variable full name (using group name and relative name of fixed template) */
            char *skp_nm_fll=nco_bld_nm_fll(trv_tbl_2->nsm[idx_nsm].mbr[idx_mbr].mbr_nm_fll,var_trv->nm);

            /* Get variable  */
            trv_sct *skp_trv=trv_tbl_var_nm_fll(skp_nm_fll,trv_tbl_2);

	    /* Define/write fixed variables (ncbo) */
            if(skp_trv) (void)nco_fix_dfn_wrt(nc_id_2,nc_out_id,cnk,dfl_lvl,gpe,gpe_nm,nbr_gpe_nm,skp_trv,trv_tbl_2,flg_dfn);

          } /* List of fixed templates  */

          /* Loop list of variables from other file */
          for(int idx_var_1=0;idx_var_1<var_lst->nbr;idx_var_1++){

            /* Inquire existence of object in other table  */
            trv_1=trv_tbl_var_nm_fll(var_lst->lst[idx_var_1].nm,trv_tbl_1);

            assert(trv_1);

            /* Match name (relative, since they are not absolute matches)  */
            if(strcmp(trv_2->nm,trv_1->nm) == 0){

              if(nco_dbg_lvl_get() >= nco_dbg_var) (void)fprintf(stdout,"%s: INFO common variable to output <%s>\n",nco_prg_nm_get(),trv_2->nm_fll); 

              /* Process common object */
              (void)nco_prc_cmn(nc_id_1,nc_id_2,nc_out_id,cnk,dfl_lvl,gpe,gpe_nm,nbr_gpe_nm,cnv,(nco_bool)False,(dmn_sct **)NULL,(int)0,nco_op_typ,trv_1,trv_2,trv_tbl_1,trv_tbl_2,flg_grp_1,flg_dfn);

              break;
            } /* Match name  */
          } /* Loop list of variables from other file */

        } /* Loop variables */

      } /* Loop group members */
    } /* Loop ensembles */

  } /* ! Using table 1 as template */ 

} /* nco_prc_nsm() */

void                          
nco_fix_dfn_wrt                        /* [fnc] Define/write fixed variables (ncbo) */
(const int nc_id,                      /* I [id] netCDF input-file ID */
 const int nc_out_id,                  /* I [id] netCDF output-file ID */
 const cnk_sct * const cnk,            /* I [sct] Chunking structure */
 const int dfl_lvl,                    /* I [enm] Deflate level [0..9] */
 const gpe_sct * const gpe,            /* I [sct] GPE structure */
 gpe_nm_sct *gpe_nm,                   /* I/O [sct] GPE name duplicate check array */
 int nbr_gpe_nm,                       /* I/O [nbr] Number of GPE entries */  
 trv_sct * trv,                        /* I [sct] Table object */
 trv_tbl_sct * const trv_tbl,          /* I/O [sct] GTT (Group Traversal Table) */
 const nco_bool flg_dfn)               /* I [flg] Action type (True for define variables, False when write variables ) */
{
  char *grp_out_fll;             /* [sng] Group name */

  int grp_id;                    /* [id] Group ID in input file */
  int grp_out_id;                /* [id] Group ID in output file */ 
  int var_id;                    /* [id] Variable ID in input file */
  int var_out_id;                /* [id] Variable ID in output file */

  /* Edit group name for output */
  if(gpe) grp_out_fll=nco_gpe_evl(gpe,trv->grp_nm_fll); else grp_out_fll=(char *)strdup(trv->grp_nm_fll);

  /* Obtain group ID */
  (void)nco_inq_grp_full_ncid(nc_id,trv->grp_nm_fll,&grp_id);

  /* Get variable ID */
  (void)nco_inq_varid(grp_id,trv->nm,&var_id);

  /* Define mode */
  if(flg_dfn){  
    char *rec_dmn_nm=NULL; /* [sng] Record dimension name */

    /* If output group does not exist, create it */
    if(nco_inq_grp_full_ncid_flg(nc_out_id,grp_out_fll,&grp_out_id)) nco_def_grp_full(nc_out_id,grp_out_fll,&grp_out_id);

    /* Detect duplicate GPE names in advance, then exit with helpful error */
    if(gpe) (void)nco_gpe_chk(grp_out_fll,trv->nm,&gpe_nm,&nbr_gpe_nm);  

    /* Define variable in output file */
    var_out_id=nco_cpy_var_dfn_trv(nc_id,nc_out_id,cnk,grp_out_fll,dfl_lvl,gpe,rec_dmn_nm,trv,NULL,0,trv_tbl);

    /* Copy variable's attributes */
    (void)nco_att_cpy(grp_id,grp_out_id,var_id,var_out_id,True); 

  }else{ /* !flg_dfn */
    /* Write mode */
    md5_sct *md5=NULL; /* [sct] MD5 configuration */

    /* Get group ID */
    (void)nco_inq_grp_full_ncid(nc_out_id,grp_out_fll,&grp_out_id);

    /* Get variable ID */
    (void)nco_inq_varid(grp_out_id,trv->nm,&var_out_id);         

    /* Non-processed variable */
    (void)nco_cpy_var_val_mlt_lmt_trv(grp_id,grp_out_id,(FILE *)NULL,md5,trv);

  } /* !flg_dfn */

  /* Free output path name */
  grp_out_fll=(char *)nco_free(grp_out_fll);

} /* nco_fix_dfn_wrt() */

nco_bool                               /* O [flg] True for match found */
nco_rel_mch                            /* [fnc] Relative match of object in table 1 to table 2  */
(trv_sct * var_trv,                    /* I [sct] Table variable object (can be from table 1 or 2) */
 const nco_bool flg_tbl_1,             /* I [flg] Table variable object is from table1 for True, otherwise is from table 2 */
 const trv_tbl_sct * const trv_tbl_1,  /* I [sct] GTT (Group Traversal Table) */
 const trv_tbl_sct * const trv_tbl_2)  /* I [sct] GTT (Group Traversal Table) */
{
  nco_bool rel_mch; /* [flg] A match was found */

  rel_mch=False;

  /* Object is from table 1, look in table 2  */
  if(flg_tbl_1 == True){

    /* Loop table  */
    for(unsigned idx_tbl=0;idx_tbl<trv_tbl_2->nbr;idx_tbl++){
      if(trv_tbl_2->lst[idx_tbl].nco_typ == nco_obj_typ_var && !strcmp(var_trv->nm,trv_tbl_2->lst[idx_tbl].nm)){
        rel_mch=True;  
      } /* A relative match was found */
    } /* Loop table  */

    /* Object is from table 2, look in table 1  */
  }else if(flg_tbl_1 == False){

    /* Loop table  */
    for(unsigned idx_tbl=0;idx_tbl<trv_tbl_1->nbr;idx_tbl++){
      if(trv_tbl_1->lst[idx_tbl].nco_typ == nco_obj_typ_var && !strcmp(var_trv->nm,trv_tbl_1->lst[idx_tbl].nm)){
        rel_mch=True;
      } /* A relative match was found */
    } /* Loop table  */

  } /* !flg_tbl_1 */

  return rel_mch;

} /* nco_rel_mch() */

void                          
nco_prc_rel_cmn_nm                     /* [fnc] Process common relative objects from a common mames list (ncbo only) */
(const int nc_id_1,                    /* I [id] netCDF input-file ID */
 const int nc_id_2,                    /* I [id] netCDF input-file ID */
 const int nc_out_id,                  /* I [id] netCDF output-file ID */
 const cnk_sct * const cnk,            /* I [sct] Chunking structure */
 const int dfl_lvl,                    /* I [enm] Deflate level [0..9] */
 const gpe_sct * const gpe,            /* I [sct] GPE structure */
 gpe_nm_sct *gpe_nm,                   /* I/O [sct] GPE name duplicate check array */
 int nbr_gpe_nm,                       /* I/O [nbr] Number of GPE entries */  
 const cnv_sct * const cnv,       /* I [flg] File adheres to NCAR CCM/CCSM/CF conventions */
 const int nco_op_typ,                 /* I [enm] Operation type (command line -y) */
 trv_tbl_sct * const trv_tbl_1,        /* I/O [sct] GTT (Group Traversal Table) */
 trv_tbl_sct * const trv_tbl_2,        /* I/O [sct] GTT (Group Traversal Table) */
 const nco_cmn_t * const cmn_lst,      /* I [sct] List of common names (includes groups/datasets in absolute path */
 const int nbr_cmn_nm,                 /* I [nbr] Number of common names entries */
 const nco_bool flg_dfn)               /* I [flg] Action type (True for define variables, False when write variables ) */
{
  /* Purpose: Process common objects from a common mames list (ncbo only) 
  Criteria for "larger" file is number of depth 1 groups (e.g cmip5.nc compared to obs.nc ) */

  const char fnc_nm[]="nco_prc_rel_cmn_nm()"; /* [sng] Function name */

  int nbr_grp_dpt_1;  /* [nbr] Number of depth 1 groups (root = 0)  */
  int nbr_grp_dpt_2;  /* [nbr] Number of depth 1 groups (root = 0)  */

  nco_bool flg_grt_1; /* [flg] File 1 is "larger" */
  nco_bool has_mch;   /* [flg] A relative match was found in file 1 or 2 */

  nco_bool flg_tbl_1; /* [flg] Table variable object is from table1 for True, otherwise is from table 2 */
  nco_bool flg_grp_1; /* [flg] Use table 1 as template for group creation on True, otherwise use table 2 */

  trv_sct *trv_1;     /* [sct] Table object */
  trv_sct *trv_2;     /* [sct] Table object */

  nbr_grp_dpt_1=trv_tbl_inq_dpt(trv_tbl_1);    
  nbr_grp_dpt_2=trv_tbl_inq_dpt(trv_tbl_2);

  if (nbr_grp_dpt_1 > nbr_grp_dpt_2) flg_grt_1=True; else flg_grt_1=False;

  /* File 1 "larger" (typically model file) (e.g ncbo -O cmip5.nc obs.nc out.nc ) */
  if(flg_grt_1 == True){

    /* Process objects in list */
    for(int idx_cmn=0;idx_cmn<nbr_cmn_nm;idx_cmn++){

      /* Get GTT objects from full names */
      trv_1=trv_tbl_var_nm_fll(cmn_lst[idx_cmn].nm,trv_tbl_1);
      trv_2=trv_tbl_var_nm_fll(cmn_lst[idx_cmn].nm,trv_tbl_2);

      if(nco_dbg_lvl_get() >= nco_dbg_dev) (void)fprintf(stdout,"%s: DEBUG %s processing common name <%s>\n",nco_prg_nm_get(),fnc_nm,
        cmn_lst[idx_cmn].nm);

      if(!trv_1 && !trv_2){
        continue;
      }

      /* Both objects exist in same location, both flagged for extraction */
      if(trv_1 && trv_2 && cmn_lst[idx_cmn].flg_in_fl[0] && cmn_lst[idx_cmn].flg_in_fl[1] && trv_1->flg_xtr && trv_2->flg_xtr){
        continue;
      }

      /* Object exists and is flagged for extraction only in file 1 */
      if(trv_1 && trv_1->flg_xtr && cmn_lst[idx_cmn].flg_in_fl[0] == True && cmn_lst[idx_cmn].flg_in_fl[1] == False){

        if(nco_dbg_lvl_get() >= nco_dbg_dev) (void)fprintf(stdout,"%s: DEBUG %s reports element in file 1 to output <%s>\n",nco_prg_nm_get(),fnc_nm,trv_1->nm_fll);

        flg_tbl_1=True;
        flg_grp_1=True;

        /* Try relative match in file 2 */
        has_mch=nco_prc_rel_mch(nc_id_1,nc_id_2,nc_out_id,cnk,dfl_lvl,gpe,gpe_nm,nbr_gpe_nm,cnv,nco_op_typ,trv_1,flg_tbl_1,flg_grp_1,trv_tbl_1,trv_tbl_2,flg_dfn);

        /* Match not found in file 2, copy instead object from file 1 as fixed to output */
        if(!has_mch) (void)nco_cpy_fix(nc_id_1,nc_out_id,cnk,dfl_lvl,gpe,gpe_nm,nbr_gpe_nm,cnv,(nco_bool)False,(dmn_sct **)NULL,(int)0,trv_1,trv_tbl_1,flg_dfn);

      } /* Object exists and is flagged for extraction only in file 1 */
    }/* Process objects in list */

    /* File 2 "larger" (typically model file) (e.g ncbo -O obs.nc cmip5.nc out.nc ) */
  } else if(flg_grt_1 == False) { 

    /* Process objects in list */
    for(int idx_cmn=0;idx_cmn<nbr_cmn_nm;idx_cmn++){

      /* Get GTT objects from full names */
      trv_1=trv_tbl_var_nm_fll(cmn_lst[idx_cmn].nm,trv_tbl_1);
      trv_2=trv_tbl_var_nm_fll(cmn_lst[idx_cmn].nm,trv_tbl_2);

      if(nco_dbg_lvl_get() >= nco_dbg_dev) (void)fprintf(stdout,"%s: DEBUG %s processing common name <%s>\n",nco_prg_nm_get(),fnc_nm,
        cmn_lst[idx_cmn].nm);

      if(!trv_1 && !trv_2){
        continue;
      }

      /* Both objects exist in same location, both flagged for extraction */
      if(trv_1 && trv_2 && cmn_lst[idx_cmn].flg_in_fl[0] && cmn_lst[idx_cmn].flg_in_fl[1] && trv_1->flg_xtr && trv_2->flg_xtr){
        continue;
      }

      /* Object exists and is flagged for extraction only in file 2 */
      if(trv_2 && trv_2->flg_xtr && cmn_lst[idx_cmn].flg_in_fl[0] == False && cmn_lst[idx_cmn].flg_in_fl[1] == True){

        if(nco_dbg_lvl_get() >= nco_dbg_dev) (void)fprintf(stdout,"%s: DEBUG %s reports element in file 2 to output <%s>\n",nco_prg_nm_get(),fnc_nm,trv_2->nm_fll);

        flg_tbl_1=False;
        flg_grp_1=False;

        /* Try relative match in file 1 */
        has_mch=nco_prc_rel_mch(nc_id_1,nc_id_2,nc_out_id,cnk,dfl_lvl,gpe,gpe_nm,nbr_gpe_nm,cnv,nco_op_typ,trv_2,flg_tbl_1,flg_grp_1,trv_tbl_1,trv_tbl_2,flg_dfn);

        /* Match not found in file 2, copy instead object from file 2 as fixed to output */
        if(!has_mch) (void)nco_cpy_fix(nc_id_2,nc_out_id,cnk,dfl_lvl,gpe,gpe_nm,nbr_gpe_nm,cnv,(nco_bool)False,(dmn_sct **)NULL,(int)0,trv_2,trv_tbl_2,flg_dfn);

      } /* Object exists and is flagged for extraction only in file 2 */
    } /* Process objects in list */
  } /* File 2 "larger" (typically model file) (e.g ncbo -O obs.nc cmip5.nc out.nc ) */

} /* nco_prc_rel_cmn_nm() */

void                  
nco_chk_nsm                            /* [fnc] Check if ensembles are valid  */                                
(const int in_id,                      /* I [id] netCDF input-file ID of current file, starting with first */
 const int fl_idx,                     /* I [nbr] Index of file loop  */
 const trv_tbl_sct * const trv_tbl)    /* I [sct] GTT (Group Traversal Table) of *first* file */
{
  /* Check if ensembles are valid. Ensemble parent group exists, variables have the same dimension sizes and names */

  /* Tests:
  ncra -Y ncge -O in_grp_4.nc in_grp_5.nc out.nc
  ncra -Y ncge -O -d lon,0,2,1 -d time,0,5,1 in_grp_4.nc in_grp_5.nc out.nc
  */

  const char fnc_nm[]="nco_chk_nsm()"; /* [sng] Function name */

  char **var_nm_lst;                   /* [sng] List of variable names found in ensemble group */
  char *grp_nm_fll;                    /* I [sng] Full group name */
  char *grp_nm;                        /* I [sng] Group name */

  int nbr_var_nm_lst;                  /* [nbr] Number of items in list */
  int grp_id;                          /* [id] Group ID */
  int nbr_grp;                         /* [nbr] Number of sub-groups */
  int *grp_ids;                        /* [id] Sub-group IDs array */
  int *dmn_id_var;                     /* [ID] Dimensions IDs array for variable */
  int nbr_dmn_var;                     /* [nbr] Number of dimensions for variable */
  int rcd=NC_NOERR;                    /* [rcd] Return code */

  size_t grp_nm_lng;                   /* [nbr] Group name length */

  /* Loop ensembles */
  for(int idx_nsm=0;idx_nsm<trv_tbl->nsm_nbr;idx_nsm++){

    trv_sct **var_tpl_trv;

    int nbr_var_mbr=trv_tbl->nsm[idx_nsm].mbr[0].var_nbr;

    var_tpl_trv=(trv_sct **)nco_malloc(nbr_var_mbr*sizeof(trv_sct *));

    if(nco_dbg_lvl_get() >= nco_dbg_dev) (void)fprintf(stdout,"%s: DEBUG %s <ensemble %d> <%s>\n",nco_prg_nm_get(),fnc_nm,
      idx_nsm,trv_tbl->nsm[idx_nsm].grp_nm_fll_prn);

    /* Obtain group ID of current ensemble */
    rcd+=nco_inq_grp_full_ncid_flg(in_id,trv_tbl->nsm[idx_nsm].grp_nm_fll_prn,&grp_id);

    /* Group must exist, if not exit */
    if(rcd != NC_NOERR){
      (void)fprintf(stdout,"%s: ERROR ensemble <%s> does not exist\n",nco_prg_nm_get(),trv_tbl->nsm[idx_nsm].grp_nm_fll_prn); 
      (void)fprintf(stdout,"%s: List of ensembles is\n",nco_prg_nm_get()); 
      for(int idx=0;idx<trv_tbl->nsm_nbr;idx++){
        (void)fprintf(stdout,"%s: <%s>\n",nco_prg_nm_get(),trv_tbl->nsm[idx].grp_nm_fll_prn);
        nco_exit(EXIT_FAILURE);
      } 
    }

    /* Get number of sub-groups */
    (void)nco_inq_grps(grp_id,&nbr_grp,(int *)NULL);
    grp_ids=(int *)nco_malloc(nbr_grp*sizeof(int)); 
    (void)nco_inq_grps(grp_id,(int *)NULL,grp_ids);

    /* Loop sub-groups */
    for(int idx_grp=0;idx_grp<nbr_grp;idx_grp++){ 

      /* Get group name length */
      (void)nco_inq_grpname_len(grp_ids[idx_grp],&grp_nm_lng);
      grp_nm=(char *)nco_malloc(grp_nm_lng+1L);

      /* Get group name */
      (void)nco_inq_grpname(grp_ids[idx_grp],grp_nm);

      /* Construct full name  */
      grp_nm_fll=(char *)nco_malloc(grp_nm_lng+strlen(trv_tbl->nsm[idx_nsm].grp_nm_fll_prn)+2L);
      strcpy(grp_nm_fll,trv_tbl->nsm[idx_nsm].grp_nm_fll_prn);
      strcat(grp_nm_fll,"/");
      strcat(grp_nm_fll,grp_nm);

      /* Export list of variable names for group */
      (void)nco_grp_var_lst(in_id,grp_nm_fll,&var_nm_lst,&nbr_var_nm_lst);

      int tpl_nbr=trv_tbl->nsm[idx_nsm].tpl_nbr;

      /* Loop templates */
      for(int idx_tpl=0;idx_tpl<tpl_nbr;idx_tpl++){ 

        /* Obtain GTT check variable objects (for first member, index 0) */         
        var_tpl_trv[idx_tpl]=trv_tbl_var_nm_fll(trv_tbl->nsm[idx_nsm].mbr[0].var_nm_fll[idx_tpl],trv_tbl);

        assert(var_tpl_trv[idx_tpl]);

        /* Loop variables in group */
        for(int idx_var=0;idx_var<nbr_var_nm_lst;idx_var++){ 

          /* Match relative name of template variable and variable found in file  */
          if(strcmp(var_nm_lst[idx_var],trv_tbl->nsm[idx_nsm].tpl_mbr_nm[idx_tpl]) == 0){

            trv_sct *var_trv;

            size_t lmn_cnt=1; /* [nbr] Total number of elements */

            /* Build new variable name */
            char *var_nm_fll=nco_bld_nm_fll(grp_nm_fll,var_nm_lst[idx_var]);

            /* For first file, GTT was built */
            if (fl_idx == 0){
              var_trv=trv_tbl_var_nm_fll(var_nm_fll,trv_tbl);
              assert(var_trv);
            }

            /* Get number of dimensions */
            (void)nco_inq_var(grp_ids[idx_grp],idx_var,var_nm_lst[idx_var],NULL,&nbr_dmn_var,(int *)NULL,(int *)NULL);

            /* Alloc dimension IDs array */
            dmn_id_var=(int *)nco_malloc(nbr_dmn_var*sizeof(int));

            /* Get dimension IDs for variable */
            (void)nco_inq_vardimid(grp_ids[idx_grp],idx_var,dmn_id_var);

            /* Loop dimensions and check GTT template (first) with current variable */
            for(int idx_dmn=0;idx_dmn<nbr_dmn_var;idx_dmn++){

              char dmn_nm[NC_MAX_NAME+1L];     /* [nbr] Name of coordinate */
              char tpl_dmn_nm[NC_MAX_NAME+1L]; /* [nbr] Name of template coordinate */

              size_t tpl_sz;                   /* [nbr] Size of template dimension */

              long dmn_sz;                     /* [nbr] Size of dimension */
              long dmn_cnt_gtt=-1;             /* [nbr] Hyperslabbed size of dimension of GTT variable (first file only) */  

              nco_bool flg_has_lmt;            /* [flg] Dimension has hyperslab */

              flg_has_lmt=False;

              /* Get size of template variable */
              if (var_tpl_trv[idx_tpl]->var_dmn[idx_dmn].crd){
                tpl_sz=var_tpl_trv[idx_tpl]->var_dmn[idx_dmn].crd->sz;
                strcpy(tpl_dmn_nm,var_tpl_trv[idx_tpl]->var_dmn[idx_dmn].crd->nm);
                /* Inquire about hyperslabs*/
                if(var_tpl_trv[idx_tpl]->var_dmn[idx_dmn].crd->lmt_msa.lmt_dmn_nbr){
                  flg_has_lmt=True;
                }
              }else if (var_tpl_trv[idx_tpl]->var_dmn[idx_dmn].ncd){
                tpl_sz=var_tpl_trv[idx_tpl]->var_dmn[idx_dmn].ncd->sz;
                strcpy(tpl_dmn_nm,var_tpl_trv[idx_tpl]->var_dmn[idx_dmn].ncd->nm);
                /* Inquire about hyperslabs*/
                if(var_tpl_trv[idx_tpl]->var_dmn[idx_dmn].ncd->lmt_msa.lmt_dmn_nbr){
                  flg_has_lmt=True;
                }
              }else assert(0);

              /* Get name/size of variable */
              (void)nco_inq_dim(grp_ids[idx_grp],dmn_id_var[idx_dmn],dmn_nm,&dmn_sz);

              /* Compare names */
              if (strcmp(dmn_nm,tpl_dmn_nm) != 0 ){
                (void)fprintf(stdout,"%s: ERROR Variables do not conform: variable <%s> has dimension named <%s>, expecting <%s>\n",nco_prg_nm_get(),
                  var_nm_fll,dmn_nm,tpl_dmn_nm);
                nco_exit(EXIT_FAILURE);
              }

              /* No hyperslab, compare dimension */
              if(flg_has_lmt == False){
                lmn_cnt*=dmn_sz;
                /* Compare sizes */
                if (dmn_sz != (long)tpl_sz){
                  (void)fprintf(stdout,"%s: ERROR Variables do not conform: variable <%s> has dimension <%s> with size %ld, expecting size %ld\n",nco_prg_nm_get(),
                    var_nm_fll,dmn_nm,dmn_sz,tpl_sz);
                  nco_exit(EXIT_FAILURE);
                }
              } else {

                /* Hyperslab */

                /* For first file, GTT was built */
                if (fl_idx == 0){

                  /* Get hyperslabbed size of GTT variable */
                  if (var_trv->var_dmn[idx_dmn].crd){
                    dmn_cnt_gtt=var_trv->var_dmn[idx_dmn].crd->lmt_msa.dmn_cnt;
                  }else if (var_tpl_trv[idx_tpl]->var_dmn[idx_dmn].ncd){
                    dmn_cnt_gtt=var_trv->var_dmn[idx_dmn].ncd->lmt_msa.dmn_cnt;
                  }else assert(0);

                  if(nco_dbg_lvl_get() >= nco_dbg_dev) (void)fprintf(stdout,"%s: DEBUG %s <%s> <%s> hyperslabbed size %ld\n",nco_prg_nm_get(),fnc_nm,
                    var_trv->nm_fll,dmn_nm,dmn_cnt_gtt);

                  lmn_cnt*=dmn_cnt_gtt;

                } /* For first file, GTT was built */
              } /* Hyperslab */
            } /* Loop dimensions */

            /* Free dimension IDs array */
            dmn_id_var=(int *)nco_free(dmn_id_var);

            if(nco_dbg_lvl_get() >= nco_dbg_dev){
              if (fl_idx == 0) (void)fprintf(stdout,"%s: DEBUG %s <%s> elements %ld\n",nco_prg_nm_get(),fnc_nm,var_trv->nm_fll,lmn_cnt);
            }

            var_nm_fll=(char *)nco_free(var_nm_fll);
            break;

          } /* Match relative name  */
        } /* Loop variables in group */
      } /* Loop templates */

      /* Free list */
      for(int idx_nm=0;idx_nm<nbr_var_nm_lst;idx_nm++) var_nm_lst[idx_nm]=(char *)nco_free(var_nm_lst[idx_nm]);
      var_nm_lst=(char **)nco_free(var_nm_lst);
      grp_nm_fll=(char *)nco_free(grp_nm_fll);

    } /* Loop sub-groups */

    /* Clean up memory */
    grp_ids=(int *)nco_free(grp_ids);
    var_tpl_trv=(trv_sct **)nco_free(var_tpl_trv);

  } /* Loop ensembles */
} /* nco_chk_nsm() */


void                          
nco_nsm_att                            /* [fnc] Inquire if ensemble parent group has "ensemble_source" attribute (ncbo only) */
(const int nc_id,                      /* I [id] netCDF file ID  */
 const trv_tbl_sct * const trv_tbl,    /* I [sct] GTT (Group Traversal Table) */
 nco_bool *flg_nsm_att,                /* I/O [flg] "ensemble" attribute exists */
 nm_lst_sct **nsm_grp_nm_fll_prn)      /* I/O [sct] Array of ensemble paths read in the attributes */ 
{
  /* Purpose: Inquire if ensemble parent group has "ensemble" attribute (ncbo only) */

  int grp_id;      /* [id] Group ID  */
  int rcd;         /* [rcd] Return code */
  int nbr_nm;      /* [nbr] Number of names in list */ 

  long att_sz;

  nc_type att_typ; /* [nbr] Attribute type */

  *flg_nsm_att=False;

  (*nsm_grp_nm_fll_prn)=(nm_lst_sct *)nco_malloc(sizeof(nm_lst_sct));
  (*nsm_grp_nm_fll_prn)->nbr=0;
  (*nsm_grp_nm_fll_prn)->lst=NULL; /* Must be NULL to nco_realloc() correct handling */

  nbr_nm=0;

  /* Loop table  */
  for(unsigned idx_tbl=0;idx_tbl<trv_tbl->nbr;idx_tbl++){

      /* Filter groups */
      if(trv_tbl->lst[idx_tbl].nco_typ == nco_obj_typ_grp){

          trv_sct trv=trv_tbl->lst[idx_tbl];

          /* Obtain output group ID */
          (void)nco_inq_grp_full_ncid(nc_id,trv.grp_nm_fll,&grp_id);

          /* Does attribute "ensemble_source" exist? (saved in ncge); string is hard coded and duplicated  */
          rcd=nco_inq_att_flg(grp_id,NC_GLOBAL,"ensemble_source",&att_typ,&att_sz);

          /* Check retrun code */
          if(rcd == NC_NOERR){
            *flg_nsm_att=True;

            if(nco_dbg_lvl_get() >= nco_dbg_dev) (void)fprintf(stdout,"%s: ATTRIBUTE ensemble_source in <%s>\n",nco_prg_nm_get(),trv.grp_nm_fll);
     
            /* Add one more element to table */
            (*nsm_grp_nm_fll_prn)->lst=(nm_sct *)nco_realloc((*nsm_grp_nm_fll_prn)->lst,(nbr_nm+1)*sizeof(nm_sct));

            char *attr_sng=NULL_CEWI;

            /* Get attribute */
            attr_sng=(char *)nco_malloc((att_sz+1UL)*sizeof(char));

            (void)nco_get_att(grp_id,NC_GLOBAL,"ensemble_source",attr_sng,att_typ);
            attr_sng[att_sz]='\0';

            /* Duplicate string into list */
            (*nsm_grp_nm_fll_prn)->lst[nbr_nm].nm=strdup(attr_sng);

            nbr_nm++;
            (*nsm_grp_nm_fll_prn)->nbr=nbr_nm;

            attr_sng=(char *)nco_free(attr_sng);
          } /* Check retrun code */
      } /* Filter groups */
  } /* Loop table  */
} /* nco_nsm_att() */

void                      
nco_rad                                /* [fnc] Retain all dimensions */
(const int nc_out_id,                  /* I [ID] netCDF output file ID */
 const int nbr_dmn_var_out,            /* I [nbr] Number of dimensions for variable on output  */
 const dmn_cmn_sct * const dmn_cmn,    /* I [sct] Dimension structure in output file */
 const trv_tbl_sct * const trv_tbl)    /* I [sct] GTT (Group Traversal Table) */
{
  /* Purpose: Retain all dimensions */

  const char fnc_nm[]="nco_rad()"; /* [sng] Function name */

  int grp_dmn_out_id;              /* [id] Group ID where dimension visible to specified group is defined */
  int dmn_id_out;                  /* [id] Dimension ID defined in outout group */  

  char *grp_dmn_out_fll;            /* [sng] Group name of dimension in output */

  /* Loop unique dimensions list */
  for(unsigned idx_dmn_tbl=0;idx_dmn_tbl<trv_tbl->nbr_dmn;idx_dmn_tbl++){
    dmn_trv_sct dmn_trv=trv_tbl->lst_dmn[idx_dmn_tbl]; 

    nco_bool has_dmn=False;

    /* Loop variable dimensions on output  */
    for(int idx_dmn=0;idx_dmn<nbr_dmn_var_out;idx_dmn++){

      /* Match full name */
      if(!strcmp(dmn_trv.nm_fll,dmn_cmn[idx_dmn].nm_fll)){
        has_dmn=True;
        break;
      }
    } /* Loop variable dimensions on output  */

    /* Dimension not found, make it */
    if(!has_dmn){

      if(nco_dbg_lvl_get() >= nco_dbg_dev) (void)fprintf(stdout,"%s: DEBUG %s making <%s> to output\n",nco_prg_nm_get(),fnc_nm,dmn_trv.nm_fll);
      grp_dmn_out_fll=(char *)strdup(dmn_trv.grp_nm_fll);

      /* Test existence of group and create if not existent */
      if(nco_inq_grp_full_ncid_flg(nc_out_id,grp_dmn_out_fll,&grp_dmn_out_id))
        nco_def_grp_full(nc_out_id,grp_dmn_out_fll,&grp_dmn_out_id);

      /* Define dimension and obtain dimension ID */
      (void)nco_def_dim(grp_dmn_out_id,dmn_trv.nm,dmn_trv.sz,&dmn_id_out);

      if(nco_dbg_lvl_get() >= nco_dbg_dev) (void)fprintf(stdout,"%s: DEBUG %s Defined dimension <%s><%s>#%d\n",nco_prg_nm_get(),fnc_nm,grp_dmn_out_fll,dmn_trv.nm,dmn_id_out);

      /* Memory management after defining current output dimension */
      if(grp_dmn_out_fll) grp_dmn_out_fll=(char *)nco_free(grp_dmn_out_fll);

    } /* Dimension not found, make it */
  } /* Loop unique dimensions list */
} /* nco_rad() */

void                                               
nco_prc_cmn_nsm_att                    /* [fnc] Process (define, write) variables belonging to ensembles in both files (ncbo) */
(const int nc_id_1,                    /* I [id] netCDF input-file ID */
 const int nc_id_2,                    /* I [id] netCDF input-file ID */
 const int nc_out_id,                  /* I [id] netCDF output-file ID */
 const cnk_sct * const cnk,            /* I [sct] Chunking structure */
 const int dfl_lvl,                    /* I [enm] Deflate level [0..9] */
 const gpe_sct * const gpe,            /* I [sct] GPE structure */
 gpe_nm_sct *gpe_nm,                   /* I/O [sct] GPE name duplicate check array */
 int nbr_gpe_nm,                       /* I/O [nbr] Number of GPE entries */  
 const cnv_sct * const cnv,       /* I [flg] File adheres to NCAR CCM/CCSM/CF conventions */
 const int nco_op_typ,                 /* I [enm] Operation type (command line -y) */
 trv_tbl_sct * const trv_tbl_1,        /* I/O [sct] GTT (Group Traversal Table) */
 trv_tbl_sct * const trv_tbl_2,        /* I/O [sct] GTT (Group Traversal Table) */
 const nco_bool flg_grp_1,             /* I [flg] Use table 1 as template for group creation on True, otherwise use table 2 */
 const nco_bool flg_dfn,               /* I [flg] Action type (True for define variables, False for write variables ) */
 nm_lst_sct *nsm_grp_nm_fll_prn)       /* I/O [sct] Array of ensemble paths read in the attributes */ 
{
  /* Purpose: Process (define, write) variables belonging to ensembles in *both* files (both files have ensembles)
  special attribute ensemble version */

  const char fnc_nm[]="nco_prc_cmn_nsm()"; /* [sng] Function name */

  trv_sct *trv_1;    /* [sct] Table object */
  trv_sct *trv_2;    /* [sct] Table object */

  if(flg_grp_1){

    if(nco_dbg_lvl_get() >= nco_dbg_var) (void)fprintf(stdout,"%s: Processing ensembles from file 1\n",nco_prg_nm_get());

    /* Loop ensembles */
    for(int idx_nsm=0;idx_nsm<trv_tbl_1->nsm_nbr;idx_nsm++){

      if(nco_dbg_lvl_get() >= nco_dbg_dev) (void)fprintf(stdout,"%s: DEBUG %s <ensemble %d> <%s>\n",nco_prg_nm_get(),fnc_nm,idx_nsm,trv_tbl_1->nsm[idx_nsm].grp_nm_fll_prn);

      /* Loop group members */
      for(int idx_mbr=0;idx_mbr<trv_tbl_1->nsm[idx_nsm].mbr_nbr;idx_mbr++){

        if(nco_dbg_lvl_get() >= nco_dbg_dev) (void)fprintf(stdout,"%s: DEBUG %s \t <member %d> <%s>\n",nco_prg_nm_get(),fnc_nm,idx_mbr,trv_tbl_1->nsm[idx_nsm].mbr[idx_mbr].mbr_nm_fll); 

        /* Loop variables */
        for(int idx_var=0;idx_var<trv_tbl_1->nsm[idx_nsm].mbr[idx_mbr].var_nbr;idx_var++){

          trv_1=NULL;
          trv_2=NULL;

          if(nco_dbg_lvl_get() >= nco_dbg_dev) (void)fprintf(stdout,"%s: DEBUG %s \t <variable %d> <%s>\n",nco_prg_nm_get(),fnc_nm,idx_var,trv_tbl_1->nsm[idx_nsm].mbr[idx_mbr].var_nm_fll[idx_var]); 

          /* Inquire existence of these objects in tables  */
          trv_1=trv_tbl_var_nm_fll(trv_tbl_1->nsm[idx_nsm].mbr[idx_mbr].var_nm_fll[idx_var],trv_tbl_1);
          assert(trv_1);

          /* For file 2, a list with names is argument */
          for(int idx_nm=0;idx_nm<nsm_grp_nm_fll_prn->nbr;idx_nm++){

            /* Match full name */
            if(!strcmp(nsm_grp_nm_fll_prn->lst[idx_nm].nm,trv_1->grp_nm_fll_prn)){

              if(nco_dbg_lvl_get() >= nco_dbg_dev) (void)fprintf(stdout,"%s: DEBUG %s Found name for <%s>\n",nco_prg_nm_get(),fnc_nm,nsm_grp_nm_fll_prn->lst[idx_nm].nm);   
  
              /* Since we're using table 1 as template, for table 2 object has to be searched (using relative name and ensemble parent name) */
              trv_2=trv_tbl_nsm_nm_att(trv_1->nm,nsm_grp_nm_fll_prn->lst[idx_nm].nm,trv_tbl_2);

              /* Both variables exist  */
              if(trv_1 && trv_2){
                if(nco_dbg_lvl_get() >= nco_dbg_var) (void)fprintf(stdout,"%s: INFO common variable to output <%s>\n",nco_prg_nm_get(),trv_1->nm_fll); 
                /* Process common object */
                (void)nco_prc_cmn(nc_id_1,nc_id_2,nc_out_id,cnk,dfl_lvl,gpe,gpe_nm,nbr_gpe_nm,cnv,(nco_bool)False,(dmn_sct **)NULL,(int)0,nco_op_typ,trv_1,trv_2,trv_tbl_1,trv_tbl_2,flg_grp_1,flg_dfn);
              } /* Both variables exist */

              break;

            } /* Match full name */
          } /* For file 2, a list with names is argument   */
        } /* Loop variables */

        /* List of fixed templates  */
        for(int idx_skp=0;idx_skp<trv_tbl_1->nsm[idx_nsm].skp_nbr;idx_skp++){

          /* Get variable  */
          trv_sct *var_trv=trv_tbl_var_nm_fll(trv_tbl_1->nsm[idx_nsm].skp_nm_fll[idx_skp],trv_tbl_1);

          /* Define variable full name (using group name and relative name of fixed template) */
          char *skp_nm_fll=nco_bld_nm_fll(trv_tbl_1->nsm[idx_nsm].mbr[idx_mbr].mbr_nm_fll,var_trv->nm);

          /* Get variable  */
          trv_sct *skp_trv=trv_tbl_var_nm_fll(skp_nm_fll,trv_tbl_1);

          /* Define/write fixed variables (ncbo) */
          if(skp_trv) (void)nco_fix_dfn_wrt(nc_id_1,nc_out_id,cnk,dfl_lvl,gpe,gpe_nm,nbr_gpe_nm,skp_trv,trv_tbl_1,flg_dfn);   
                      
        } /* List of fixed templates  */
      } /* Loop group members */
    } /* Loop ensembles */
  } /* !flg_grp_1 */

} /* nco_prc_cmn_nsm_att() */

void
nco_prn_dmn                            /* [fnc] Print dimensions (debug) */
(const int nc_id,                      /* I [ID] netCDF file ID */
 const char * const grp_nm_fll,        /* I [sng] Group name full */
 const char * const var_nm,            /* I [sng] Variable name relative */
 const char * const var_nm_fll,        /* I [sng] Variable name full */
 trv_tbl_sct * const trv_tbl)          /* I/O [sct] GTT (Group Traversal Table) */
{
  /* Purpose: Print dimensions (debug) */

  const char fnc_nm[]="nco_prn_dmn()"; /* [sng] Function name */

  char dmn_nm_var[NC_MAX_NAME+1];      /* [sng] Dimension name for *variable* */ 

  int *dmn_id_var;                    /* [ID] Dimensions IDs array for variable */

  int grp_id;                          /* [ID] Group ID */
  int nbr_dmn_var;                     /* [nbr] Number of dimensions associated with current matched variable */
  int var_id;                          /* [ID] Variable ID */

  long dmn_sz;                         /* [nbr] Dimension size */  

  /* Obtain group ID */
  (void)nco_inq_grp_full_ncid(nc_id,grp_nm_fll,&grp_id);

  /* Obtain variable ID */
  (void)nco_inq_varid(grp_id,var_nm,&var_id);

  /* Get number of dimensions for variable */
  (void)nco_inq_varndims(grp_id,var_id,&nbr_dmn_var);

  dmn_id_var=(int *)nco_malloc(nbr_dmn_var*sizeof(int)); 

  /* Get dimension IDs for variable */
  (void)nco_inq_vardimid(grp_id,var_id,dmn_id_var);

  /* Loop over dimensions of variable */
  for(int idx_var_dim=0;idx_var_dim<nbr_dmn_var;idx_var_dim++){
    /* Get dimension name */
    (void)nco_inq_dim(grp_id,dmn_id_var[idx_var_dim],dmn_nm_var,&dmn_sz);
    if(nco_dbg_lvl_get() == nco_dbg_old) (void)fprintf(stdout,"%s: DEBUG %s <%s><%s> #%d API size is %ld\n",nco_prg_nm_get(),fnc_nm,var_nm_fll,dmn_nm_var,dmn_id_var[idx_var_dim],dmn_sz); 
  } /* Loop over dimensions of variable */

  trv_sct *var_trv=NULL;

  /* Obtain variable GTT object using full variable name */
  var_trv=trv_tbl_var_nm_fll(var_nm_fll,trv_tbl);

  assert(var_trv);

  /* Loop over dimensions of variable */
  for(int idx_var_dim=0;idx_var_dim<var_trv->nbr_dmn;idx_var_dim++){

    int dmn_id=-1;

    if(var_trv->var_dmn[idx_var_dim].crd){
      dmn_sz=var_trv->var_dmn[idx_var_dim].crd->sz;
      dmn_id=var_trv->var_dmn[idx_var_dim].crd->dmn_id;
    }else if(var_trv->var_dmn[idx_var_dim].ncd){
      dmn_sz=var_trv->var_dmn[idx_var_dim].ncd->sz;
      dmn_id=var_trv->var_dmn[idx_var_dim].ncd->dmn_id;
    }else assert(0);

    (void)fprintf(stdout,"%s: DEBUG %s <%s><%s> #%d table size is %ld\n",nco_prg_nm_get(),fnc_nm,var_nm_fll,var_trv->var_dmn[idx_var_dim].dmn_nm_fll,dmn_id,dmn_sz); 

  } /* Loop over dimensions of variable */

  dmn_id_var=(int *)nco_free(dmn_id_var); 
} /* nco_prn_dmn() */

void
nco_nsm_wrt_att                  /* [fnc] Save ncge metadata attribute */
(const int nc_id,                    /* I [ID] netCDF input file ID */
 const int out_id,                   /* I [ID] netCDF output file ID */
 const gpe_sct * const gpe,          /* I [sct] GPE structure */
 trv_tbl_sct * const trv_tbl)        /* I/O [sct] GTT (Group Traversal Table) */
{
  /* Purpose:  Write ensemble parent group full path as attribute value in attribute named  "ensemble_source" */

  char *grp_out_fll;                   /* [sng] Group name */

  int grp_id;                          /* [ID] Group ID in input file */
  int grp_out_id;                      /* [ID] Group ID in output file */ 

  aed_sct aed; /* [sct] Attribute-edit information */

  for(unsigned idx_tbl=0;idx_tbl<trv_tbl->nbr;idx_tbl++){
    trv_sct grp_trv=trv_tbl->lst[idx_tbl];

    /* If object is group  */
    if(grp_trv.nco_typ == nco_obj_typ_grp){

      /* Obtain group ID */
      (void)nco_inq_grp_full_ncid(nc_id,grp_trv.grp_nm_fll,&grp_id);

      if(grp_trv.flg_nsm_prn){

        grp_out_fll=NULL;

        /* Ensemble parent groups */
        if(trv_tbl->nsm_sfx){
          /* Define new name by appending suffix (e.g., /cesm + _avg) */
          char *nm_fll_sfx=nco_bld_nsm_sfx(grp_trv.grp_nm_fll_prn,trv_tbl);
          /* Use then delete new name */
          if(gpe) grp_out_fll=nco_gpe_evl(gpe,nm_fll_sfx); else grp_out_fll=(char *)strdup(nm_fll_sfx);
          nm_fll_sfx=(char *)nco_free(nm_fll_sfx);
        }else{
          if(gpe) grp_out_fll=nco_gpe_evl(gpe,grp_trv.grp_nm_fll_prn); else grp_out_fll=(char *)strdup(grp_trv.grp_nm_fll_prn);
        } /* !nsm_sfx */

        /* Obtain output group ID */
        (void)nco_inq_grp_full_ncid(out_id,grp_out_fll,&grp_out_id);   

        aed.att_nm=strdup("ensemble_source");
        aed.type=NC_CHAR;
        aed.var_nm=NULL;
        aed.id=NC_GLOBAL;
        aed.sz=strlen(grp_out_fll);
        aed.val.cp=(char *)nco_malloc((aed.sz+1L)*sizeof(char));
        (void)strcpy(aed.val.cp,grp_out_fll);
        aed.mode=aed_create;

        /* Create attribute to note ensenmble average */
        (void)nco_aed_prc(grp_out_id,NC_GLOBAL,aed);

        if(aed.att_nm) aed.att_nm=(char *)nco_free(aed.att_nm);
        if(aed.val.cp) aed.val.cp=(char *)nco_free(aed.val.cp); 

        /* Memory management after current extracted group */
        if(grp_out_fll) grp_out_fll=(char *)nco_free(grp_out_fll);

      } /* !flg_nsm_prn */

    } /* end if group and flg_xtr */
  } /* end loop to define group attributes */
} /* nco_nsm_wrt_att() */

void
nco_var_xtr_trv                       /* [fnc] Print all variables to extract (debug) */
(const trv_tbl_sct * const trv_tbl)   /* I [sct] Traversal table */
{
  int nbr_xtr=0;
  /* Loop table */
  for(unsigned tbl_idx=0;tbl_idx<trv_tbl->nbr;tbl_idx++){
    /* Filter variables to extract  */
    if(trv_tbl->lst[tbl_idx].nco_typ == nco_obj_typ_var && trv_tbl->lst[tbl_idx].flg_xtr){
      (void)fprintf(stdout, "%s\n",trv_tbl->lst[tbl_idx].nm_fll);
      nbr_xtr++;
    } /* Filter variables  */
  } /* Loop table */
} /* nco_var_xtr_trv() */

crd_sct*
nco_get_crd_sct                       /* [fnc] Return a coordinate variable crd_sct for a given table variable var_trv */
(trv_sct * const var_trv,             /* I [sct] GTT Variable */
 int lmt_nbr,                         /* I [nbr] Number of user-specified dimension limits */
 lmt_sct **lmt)                       /* I [sct] Limit array. Structure comming from nco_lmt_prs() */
{
  /* Return a coordinate variable crd_sct for a given table variable var_trv that matches a specified limit name
  NB: assumed the limit name is a coordinate variable */

  /* Loop input variable dimensions */
  for (int idx_var_dmn = 0; idx_var_dmn < var_trv->nbr_dmn; idx_var_dmn++) {

    /* Loop input name list */
    for (int lmt_idx = 0; lmt_idx < lmt_nbr; lmt_idx++) {

      /* Match input relative name to dimension relative name */
      if (!strcmp(lmt[lmt_idx]->nm, var_trv->var_dmn[idx_var_dmn].dmn_nm)) {

        /* Dimension has coordinate variables */
        if (var_trv->var_dmn[idx_var_dmn].crd) {
          crd_sct *crd = var_trv->var_dmn[idx_var_dmn].crd;
          return crd;
        }
      }
    }
  }
  return NULL;
} /* !nco_get_crd_sct() */

void
nco_srt_aux /* [fnc] Sort auxiliary coordinates */
(const trv_tbl_sct * const trv_tbl)  /* I [sct] Traversal table */
{
  const char fnc_nm[]="nco_srt_aux()"; /* [sng] Function name  */
   
  for(unsigned idx_var=0;idx_var<trv_tbl->nbr;idx_var++){
    
    trv_sct *var_trv=&trv_tbl->lst[idx_var];

    /* Filter variables with auxiliary coordinates */ 
    if(var_trv->flg_aux && var_trv->nco_typ == nco_obj_typ_var){

      for(int idx_dmn=0;idx_dmn<var_trv->nbr_dmn;idx_dmn++){
	
        var_dmn_sct *lcl_dmn=&var_trv->var_dmn[idx_dmn];
	
        int nbr_lon_crd=lcl_dmn->nbr_lon_crd;	
        int nbr_lat_crd=lcl_dmn->nbr_lat_crd;	

	/* Sort by group depth */
        if(nbr_lat_crd > 1) qsort(lcl_dmn->lat_crd,(size_t)nbr_lat_crd,sizeof(lcl_dmn->lat_crd[0]),nco_cmp_aux_crd_dpt);

        if(nco_dbg_lvl_get() >= nco_dbg_dev && nbr_lat_crd ){ 
          (void)fprintf(stdout,"%s: DEBUG %s 'latitude' auxiliary coordinates %s: %s\n",nco_prg_nm_get(),fnc_nm,var_trv->nm_fll,lcl_dmn->dmn_nm_fll); 
          for(int idx_crd=0;idx_crd<nbr_lat_crd;idx_crd++) (void)fprintf(stdout,"%s: DEBUG %s %s dpt=%d\n",nco_prg_nm_get(),fnc_nm,lcl_dmn->lat_crd[idx_crd].nm_fll,lcl_dmn->lat_crd[idx_crd].grp_dpt);
        } /* endif dbg */

        /* Sort them by group depth */
        if(nbr_lon_crd >1 ) qsort(lcl_dmn->lon_crd,(size_t)nbr_lon_crd,sizeof(lcl_dmn->lon_crd[0]),nco_cmp_aux_crd_dpt);

        if(nco_dbg_lvl_get() >= nco_dbg_dev && nbr_lon_crd){ 
          (void)fprintf(stdout,"%s: DEBUG %s 'longitude' auxiliary coordinates %s: %s\n",nco_prg_nm_get(),fnc_nm,var_trv->nm_fll,lcl_dmn->dmn_nm_fll); 
          for(int idx_crd=0;idx_crd<nbr_lon_crd;idx_crd++) (void)fprintf(stdout,"%s: DEBUG %s %s dpt=%d\n",nco_prg_nm_get(),fnc_nm,lcl_dmn->lon_crd[idx_crd].nm_fll,lcl_dmn->lon_crd[idx_crd].grp_dpt);
        } /* endif dbg */

      } /* !idx_dmn */
    } /* !flg_aux */ 
  } /* !idx */

} /* !nco_srt_aux() */

