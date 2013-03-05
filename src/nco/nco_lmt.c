/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_lmt.c,v 1.181 2013-03-05 17:40:49 zender Exp $ */

/* Purpose: Hyperslab limits */

/* Copyright (C) 1995--2013 Charlie Zender
   License: GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */

#include "nco_lmt.h" /* Hyperslab limits */

void
nco_lmt_init /* [fnc] Initialize limit to NULL/invalid values */
(lmt_sct *lmt) /* I/O [sct] Limit structure to initialize */
{
  lmt->nm=NULL;              /* [sng] Dimension name */
  lmt->drn_sng=NULL;         /* [sng] User-specified string for dimension duration */
  lmt->max_sng=NULL;         /* [sng] User-specified string for dimension maximum */
  lmt->min_sng=NULL;         /* [sng] User-specified string for dimension minimum */
  lmt->mro_sng=NULL;         /* [sng] User-specified string for multi-record output */
  lmt->rbs_sng=NULL;         /* [sng] Used by ncra, ncrcat to re-base record coordinate (holds unit attribute from first file) */
  lmt->srd_sng=NULL;         /* [sng] User-specified string for dimension stride */

  lmt->max_val=-1;           /* [nbr] Double precision representation of maximum value of coordinate requested or implied */
  lmt->min_val=-1;           /* [nbr] Double precision representation of minimum value of coordinate requested or implied */
  lmt->origin=-1;            /* [nbr] Used by ncra, ncrcat to re-base record coordinate */

  lmt->id=-1;                /* [ID] Dimension ID */

  lmt->lmt_typ=-1;           /* [enm] Limit type (0, Coordinate value limit, 1, Dimension index limit, 2, UDUnits string )  */

  lmt->cnt=-1;               /* [nbr] Valid elements in this dimension (including effects of stride and wrapping) */
  lmt->drn=-1;               /* [nbr] Duration of hyperslab */
  lmt->end=-1;               /* [nbr] Index to end of hyperslab */
  lmt->max_idx=-1;           /* [nbr] Index of maximum requested value in dimension */
  lmt->min_idx=-1;           /* [nbr] Index of minimum requested value in dimension */
  lmt->rec_dmn_sz=-1;        /* [nbr] Number of records in this file (multi-file record dimension only) */
  lmt->rec_in_cml=-1;        /* [nbr] Cumulative number of records in all files opened so far (multi-file record dimension only) */
  lmt->idx_end_max_abs=-1;   /* [[nbr]] Maximum allowed index in record dimension (multi-file record dimension only) */
  lmt->rec_skp_ntl_spf=-1;   /* [nbr] Records skipped in initial superfluous files (multi-file record dimension only) */
  lmt->rec_skp_vld_prv=-1;   /* [nbr] Records skipped since previous good one (multi-file record dimension only) */
  lmt->rec_rmn_prv_drn=-1;   /* [nbr] Records remaining-to-be-read to complete duration group from previous file (multi-file record dimension only) */
  lmt->srd=-1;               /* [nbr] Stride of hyperslab */
  lmt->srt=-1;               /* [nbr] Index to start of hyperslab */

  lmt->flg_mro=-1;           /* [flg] True for multi-record output (used by ncra only) */
  lmt->flg_input_complete=-1;/* [flg] True for multi-file operators when no more files need be opened */
  lmt->is_rec_dmn=-1;        /* [flg]  True if record dimension, else False */
  lmt->is_usr_spc_lmt=-1;    /* [flg]  True if any part of limit is user-specified, else False */
  lmt->is_usr_spc_max=-1;    /* [flg]  True if user-specified, else False */
  lmt->is_usr_spc_min=-1;    /* [flg]  True if user-specified, else False */
  lmt->lmt_cln=cln_nil;      /* [flg]  Used by ncra, ncrcat to store enum of calendar-type attribute */

} /* end nco_lmt_init() */


void
nco_lmt_prt /* [fnc] Print a Limit structure */
(lmt_sct *lmt) /* I/O [sct] Limit structure to print */
{
  (void)fprintf(stdout,"Name: %s\n",lmt->nm);
  (void)fprintf(stdout,"User-specified string for dimension duration: %s\n",lmt->drn_sng);
  (void)fprintf(stdout,"User-specified string for dimension maximum : %s\n",lmt->max_sng);
  (void)fprintf(stdout,"User-specified string for dimension minimum: %s\n",lmt->min_sng);
  (void)fprintf(stdout,"User-specified string for multi-record output: %s\n",lmt->mro_sng);
  (void)fprintf(stdout,"Unit attribute from first file: %s\n",lmt->rbs_sng);
  (void)fprintf(stdout,"User-specified string for dimension stride: %s\n",lmt->srd_sng);

  (void)fprintf(stdout,"Maximum value of coordinate: %f\n",lmt->max_val);
  (void)fprintf(stdout,"Minimum value of coordinate: %f\n",lmt->min_val);
  (void)fprintf(stdout,"Origin: %f\n",lmt->origin);

  (void)fprintf(stdout,"ID: %d\n",lmt->id);

  (void)fprintf(stdout,"Limit type: %d\n",lmt->lmt_typ);

  (void)fprintf(stdout,"Valid elements: %li\n",lmt->cnt);
  (void)fprintf(stdout,"Duration of hyperslab: %li\n",lmt->drn);
  (void)fprintf(stdout,"Index to end of hyperslab: %li\n",lmt->end);
  (void)fprintf(stdout,"Index of maximum requested value: %li\n",lmt->max_idx);
  (void)fprintf(stdout,"Index of minimum requested value: %li\n",lmt->min_idx);
  (void)fprintf(stdout,"Number of records in this file: %li\n",lmt->rec_dmn_sz);
  (void)fprintf(stdout,"Cumulative number of records in all files: %li\n",lmt->rec_in_cml);
  (void)fprintf(stdout,"Maximum allowed index in record dimension: %li\n",lmt->idx_end_max_abs);
  (void)fprintf(stdout,"Records skipped in initial superfluous files: %li\n",lmt->rec_skp_ntl_spf);
  (void)fprintf(stdout,"Records skipped since previous good one: %li\n",lmt->rec_skp_vld_prv);
  (void)fprintf(stdout,"Records remaining-to-be-read: %li\n",lmt->rec_rmn_prv_drn);
  (void)fprintf(stdout,"Stride of hyperslab: %li\n",lmt->srd);
  (void)fprintf(stdout,"Index to start of hyperslab: %li\n",lmt->srt);

  (void)fprintf(stdout,"Is multi-record output: %d\n",lmt->flg_mro);
  (void)fprintf(stdout,"No more files need be opened: %d\n",lmt->flg_input_complete);
  (void)fprintf(stdout,"Is record dimension: %d\n",lmt->is_rec_dmn);
  (void)fprintf(stdout,"Any part is user-specified: %d\n",lmt->is_usr_spc_lmt);
  (void)fprintf(stdout,"Is user-specified maximum: %d\n",lmt->is_usr_spc_max);
  (void)fprintf(stdout,"Is user-specified minimum: %d\n",lmt->is_usr_spc_min);
  (void)fprintf(stdout,"Calendar-type attribute: %d\n",lmt->lmt_cln);

} /* end nco_lmt_prt() */


void
nco_lmt_cpy /* [fnc] Deep-copy a Limit structure */
(const lmt_sct * const lmt1, /* I [sct] Limit structure to copy */
 lmt_sct *lmt2) /* O [sct] New limit structure (must be alloced before) */
{
  assert(lmt1->nm);

  /* Initialize to NULL/invalid */
  (void)nco_lmt_init(lmt2);

  lmt2->nm=(char *)strdup(lmt1->nm);
  if(lmt1->drn_sng) lmt2->drn_sng=(char *)strdup(lmt1->drn_sng);      
  if(lmt1->max_sng) lmt2->max_sng=(char *)strdup(lmt1->max_sng);
  if(lmt1->min_sng) lmt2->min_sng=(char *)strdup(lmt1->min_sng);
  if(lmt1->mro_sng) lmt2->mro_sng=(char *)strdup(lmt1->mro_sng);
  if(lmt1->rbs_sng) lmt2->rbs_sng=(char *)strdup(lmt1->rbs_sng);
  if(lmt1->srd_sng) lmt2->srd_sng=(char *)strdup(lmt1->srd_sng);

  lmt2->max_val=lmt1->max_val;
  lmt2->min_val=lmt1->min_val;
  lmt2->origin=lmt1->origin;

  lmt2->id=lmt1->id;

  lmt2->lmt_typ=lmt1->lmt_typ;

  lmt2->cnt=lmt1->cnt;
  lmt2->drn=lmt1->drn;
  lmt2->end=lmt1->end;
  lmt2->max_idx=lmt1->max_idx;
  lmt2->min_idx=lmt1->min_idx;
  lmt2->rec_dmn_sz=lmt1->rec_dmn_sz;
  lmt2->rec_in_cml=lmt1->rec_in_cml;
  lmt2->idx_end_max_abs=lmt1->idx_end_max_abs;
  lmt2->rec_skp_ntl_spf=lmt1->rec_skp_ntl_spf;
  lmt2->rec_skp_vld_prv=lmt1->rec_skp_vld_prv;
  lmt2->rec_rmn_prv_drn=lmt1->rec_rmn_prv_drn;
  lmt2->srd=lmt1->srd;
  lmt2->srt=lmt1->srt;

  lmt2->flg_mro=lmt1->flg_mro;
  lmt2->flg_input_complete=lmt1->flg_input_complete;
  lmt2->is_rec_dmn=lmt1->is_rec_dmn;
  lmt2->is_usr_spc_lmt=lmt1->is_usr_spc_lmt;
  lmt2->is_usr_spc_max=lmt1->is_usr_spc_max;
  lmt2->is_usr_spc_min=lmt1->is_usr_spc_min;
  lmt2->lmt_cln=lmt1->lmt_cln;

} /* end nco_lmt_cpy() */

lmt_sct * /* O [sct] Pointer to free'd structure */
nco_lmt_free /* [fnc] Free memory associated with limit structure */
(lmt_sct *lmt) /* I/O [sct] Limit structure to free */
{
  /* Threads: Routine is thread safe and calls no unsafe routines */
  /* Purpose: Free all memory associated with dynamically allocated limit structure */
  lmt->nm=(char *)nco_free(lmt->nm);
  lmt->drn_sng=(char *)nco_free(lmt->drn_sng);
  lmt->max_sng=(char *)nco_free(lmt->max_sng);
  lmt->min_sng=(char *)nco_free(lmt->min_sng);
  lmt->mro_sng=(char *)nco_free(lmt->mro_sng);
  lmt->rbs_sng=(char *)nco_free(lmt->rbs_sng);   
  lmt->srd_sng=(char *)nco_free(lmt->srd_sng);

  lmt=(lmt_sct *)nco_free(lmt);

  return lmt;
} /* end nco_lmt_free() */

lmt_sct ** /* O [sct] Pointer to free'd structure list */
nco_lmt_lst_free /* [fnc] Free memory associated with limit structure list */
(lmt_sct **lmt_lst, /* I/O [sct] Limit structure list to free */
 const int lmt_nbr) /* I [nbr] Number of limit structures in list */
{
  /* Threads: Routine is thread safe and calls no unsafe routines */
  /* Purpose: Free all memory associated with dynamically allocated limit structure list */
  int idx;

  for(idx=0;idx<lmt_nbr;idx++) lmt_lst[idx]=nco_lmt_free(lmt_lst[idx]);

  /* Free structure pointer last */
  lmt_lst=(lmt_sct **)nco_free(lmt_lst);

  return lmt_lst;
} /* end nco_lmt_lst_free() */

lmt_msa_sct * /* O [sct] Pointer to free'd structure */
nco_lmt_all_free /* [fnc] Free memory associated with limit structure */
(lmt_msa_sct *lmt_all) /* I/O [sct] Limit structure to free */
{
  /* Threads: Routine is thread safe and calls no unsafe routines */
  /* Purpose: Free all memory associated with dynamically allocated lmt_all structure */
  lmt_all->dmn_nm=(char *)nco_free(lmt_all->dmn_nm);
  /* NB: lmt_dmn[idx] are free'd by nco_lmt_lst_free() in calling routine */
  lmt_all->lmt_dmn=(lmt_sct **)nco_free(lmt_all->lmt_dmn);

  lmt_all=(lmt_msa_sct *)nco_free(lmt_all);

  return lmt_all;
} /* end nco_lmt_all_free() */

lmt_msa_sct ** /* O [sct] Pointer to free'd structure list */
nco_lmt_all_lst_free /* [fnc] Free memory associated with lmt_all structure list */
(lmt_msa_sct **lmt_all_lst, /* I/O [sct] Limit structure list to free */
 const int lmt_all_nbr) /* I [nbr] Number of limit strucgtures in list */
{
  /* Threads: Routine is thread safe and calls no unsafe routines */
  /* Purpose: Free all memory associated with dynamically allocated lmt_msa_sct structure list */
  int idx;

  for(idx=0;idx<lmt_all_nbr;idx++) lmt_all_lst[idx]=nco_lmt_all_free(lmt_all_lst[idx]);

  /* Free structure pointer last */
  lmt_all_lst=(lmt_msa_sct **)nco_free(lmt_all_lst);

  return lmt_all_lst;
} /* end nco_lmt_all_lst_free() */

lmt_sct * /* [sct] Limit structure for dimension */
nco_lmt_sct_mk /* [fnc] Create stand-alone limit structure for given dimension */
(const int nc_id, /* I [idx] netCDF file ID */
 const int dmn_id, /* I [idx] ID of dimension for this limit structure */
 CST_X_PTR_CST_PTR_CST_Y(lmt_sct,lmt), /* I [sct] Array of limit structures from nco_lmt_evl() */ 
 int lmt_nbr, /* I [nbr] Number of limit structures */
 const nco_bool FORTRAN_IDX_CNV) /* I [flg] Hyperslab indices obey Fortran convention */
{
  /* Purpose: Create stand-alone limit structure just for given dimension
  ncra.c calls nco_lmt_sct_mk() to generate record dimension limit structure

  This is a complex routine fundamental to most of NCO
  It is easy to make subtle errors when changing it
  Please ask CSZ to review any significant patches to this routine */

  int idx;
  int rcd; /* [rcd] Return code */

  lmt_sct *lmt_dim;

  lmt_dim=(lmt_sct *)nco_malloc(sizeof(lmt_sct));

  /* Initialize defaults to False, override later if warranted */
  lmt_dim->is_usr_spc_lmt=False; /* True if any part of limit is user-specified, else False */
  lmt_dim->is_usr_spc_max=False; /* True if user-specified, else False */
  lmt_dim->is_usr_spc_min=False; /* True if user-specified, else False */
  /* rec_skp_ntl_spf, rec_skp_vld_prv, rec_in_cml, and rec_rmn_prv_drn only used for MFO record dimension */
  lmt_dim->rec_skp_ntl_spf=0L; /* Number of records skipped in initial superfluous files */
  lmt_dim->rec_skp_vld_prv=0L; /* Number of records skipped since previous good one */
  lmt_dim->rec_in_cml=0L; /* Number of records, read or not, in previously processed files */
  lmt_dim->rec_rmn_prv_drn=0L; /* Records remaining-to-be-read to complete duration group from previous file */

  for(idx=0;idx<lmt_nbr;idx++){
    /* Copy user-specified limits, if any */
    if(lmt[idx]->id == dmn_id){
      lmt_dim->is_usr_spc_lmt=True; /* True if any part of limit is user-specified, else False */
      if(lmt[idx]->max_sng == NULL){
        lmt_dim->max_sng=NULL;
      }else{
        lmt_dim->max_sng=(char *)strdup(lmt[idx]->max_sng);
        lmt_dim->is_usr_spc_max=True; /* True if user-specified, else False */
      } /* end if */
      if(lmt[idx]->min_sng == NULL){
        lmt_dim->min_sng=NULL;
      }else{
        lmt_dim->min_sng=(char *)strdup(lmt[idx]->min_sng);
        lmt_dim->is_usr_spc_min=True; /* True if user-specified, else False */
      } /* end if */
      if(lmt[idx]->srd_sng) lmt_dim->srd_sng=(char *)strdup(lmt[idx]->srd_sng); else lmt_dim->srd_sng=NULL;
      if(lmt[idx]->drn_sng) lmt_dim->drn_sng=(char *)strdup(lmt[idx]->drn_sng); else lmt_dim->drn_sng=NULL;
      if(lmt[idx]->mro_sng) lmt_dim->mro_sng=(char *)strdup(lmt[idx]->mro_sng); else lmt_dim->mro_sng=NULL;
      lmt_dim->nm=(char *)strdup(lmt[idx]->nm);
      break;
    } /* end if */
  } /* end loop over idx */

  /* If this limit was not user-specified, then ... */
  if(idx == lmt_nbr){
    /* Create default limits to look as though user-specified them */
    char dmn_nm[NC_MAX_NAME];
    long cnt;
    int max_sng_sz;

    /* Fill-in limits with default parsing information */
    rcd=nco_inq_dim_flg(nc_id,dmn_id,dmn_nm,&cnt);

    if(rcd == NC_EBADDIM){
      (void)fprintf(stdout,"%s: ERROR attempting to find non-existent dimension with ID = %d in nco_lmt_sct_mk()\n",prg_nm_get(),dmn_id);
      return False;
    } /* end if */

    lmt_dim->nm=(char *)strdup(dmn_nm);
    lmt_dim->srd_sng=NULL;
    lmt_dim->drn_sng=NULL;
    lmt_dim->mro_sng=NULL;
    /* Generate min and max strings to look as if user had specified them
    Adjust accordingly if FORTRAN_IDX_CNV was requested for other dimensions
    These sizes will later be decremented in nco_lmt_evl() where all information
    is converted internally to C-based indexing representation.
    Ultimately this problem arises because I want nco_lmt_evl() to think the
    user always did specify this dimension's hyperslab.
    Otherwise, problems arise when FORTRAN_IDX_CNV is specified by the user 
    along with explicit hypersalbs for some dimensions excluding the record
    dimension.
    Then, when nco_lmt_sct_mk() creates the record dimension structure, it must
    be created consistently with the FORTRAN_IDX_CNV flag for the other dimensions.
    In order to do that, fill in max_sng, min_sng, and srd_sng
    arguments with strings as if they had been read from keyboard.
    An alternate solution is to add flag to lmt_sct indicating whether this
    limit struct had been automatically generated and then act accordingly. */
    /* Decrement cnt to C-index value if necessary */
    if(!FORTRAN_IDX_CNV) cnt--; 
    if(cnt < 0L){
      if(cnt == -1L) (void)fprintf(stdout,"%s: ERROR nco_lmt_sct_mk() reports record variable exists and is size zero, i.e., has no records yet.\n",prg_nm_get());
      (void)fprintf(stdout,"%s: HINT: Perform record-oriented operations only after file has valid records.\n",prg_nm_get());
      (void)fprintf(stdout,"%s: cnt < 0 in nco_lmt_sct_mk()\n",prg_nm_get());
      return False;
    } /* end if */
    /* cnt < 10 covers negative numbers and SIGFPE from log10(cnt==0) 
    Adding 1 is required for cnt=10,100,1000... */
    if(cnt < 10L) max_sng_sz=1; else max_sng_sz=1+(int)ceil(log10((double)cnt));
    /* Add one for NUL terminator */
    lmt_dim->max_sng=(char *)nco_malloc(sizeof(char)*(max_sng_sz+1));
    (void)sprintf(lmt_dim->max_sng,"%ld",cnt);
    if(FORTRAN_IDX_CNV){
      lmt_dim->min_sng=(char *)strdup("1");
    }else{
      lmt_dim->min_sng=(char *)strdup("0");
    } /* end else */
  } /* end if user did not explicity specify limits for this dimension */

  return lmt_dim;

} /* end nco_lmt_sct_mk() */

lmt_sct ** /* O [sct] Structure list with user-specified strings for min and max limits */
nco_lmt_prs /* [fnc] Create limit structures with name, min_sng, max_sng elements */
(const int lmt_nbr, /* I [nbr] number of dimensions with limits */
 CST_X_PTR_CST_PTR_CST_Y(char,lmt_arg)) /* I [sng] List of user-specified dimension limits */
{
  /* Purpose: Set name, min_sng, max_sng elements of comma separated list of names and ranges. 
  Routine merely evaluates syntax of input expressions and does validate dimensions or
  ranges against those present in input netCDF file. */

  /* Valid syntax adheres to nm,[min_sng][,[max_sng][,[srd_sng][,[drn_sng]]]] */

  char **arg_lst;

  char *msg_sng=NULL_CEWI; /* [sng] Error message */

  const char dlm_sng[]=",";

  lmt_sct **lmt=NULL_CEWI;

  int idx;
  int arg_nbr;

  nco_bool NCO_SYNTAX_ERROR=False; /* [flg] Syntax error in hyperslab specification */

  if(lmt_nbr > 0) lmt=(lmt_sct **)nco_malloc(lmt_nbr*sizeof(lmt_sct *));
  for(idx=0;idx<lmt_nbr;idx++){
    /* Process hyperslab specifications as normal text list */
    arg_lst=nco_lst_prs_2D(lmt_arg[idx],dlm_sng,&arg_nbr);

    /* Check syntax */
    if(arg_nbr < 2){ /* Need more than just dimension name */
      msg_sng=strdup("Need more than just dimension name");
      NCO_SYNTAX_ERROR=True;
    }else if(arg_nbr > 6){ /* Too much information */
      msg_sng=strdup("Too many (more than 6) arguments");
      NCO_SYNTAX_ERROR=True;
    }else if(arg_lst[0] == NULL){ /* Dimension name not specified */
      msg_sng=strdup("Dimension name not specified");
      NCO_SYNTAX_ERROR=True;
    }else if(arg_nbr == 2 && arg_lst[1] == NULL){ /* No min specified */
      msg_sng=strdup("Must specify minimum value");
      NCO_SYNTAX_ERROR=True;
    }else if(arg_nbr == 3 && arg_lst[1] == NULL && arg_lst[2] == NULL){ /* No min or max when stride not specified */
      msg_sng=strdup("Must specify minimum and/or maximum value since stride is also empty");
      NCO_SYNTAX_ERROR=True;
    }else if(arg_nbr == 4 && arg_lst[3] == NULL){ /* Stride should be specified */
      msg_sng=strdup("Stride must be specified (and be a positive integer)");
      NCO_SYNTAX_ERROR=True;
    }else if(arg_nbr == 5 && arg_lst[4] == NULL){ /* Duration should be specified */
      msg_sng=strdup("Duration must be specified (and be a positive integer)");
      NCO_SYNTAX_ERROR=True;
    }else if(arg_nbr == 6 && arg_lst[5] == NULL){ /* Group-mode should be specified */
      msg_sng=strdup("Group-mode must be specified (as 'm' or 'M')");
      NCO_SYNTAX_ERROR=True;
    } /* end else */

    if(NCO_SYNTAX_ERROR){
      (void)fprintf(stdout,"%s: ERROR parsing hyperslab specification for dimension %s\n%s\n%s: HINT Conform request to hyperslab documentation at http://nco.sf.net/nco.html#hyp\n",prg_nm_get(),lmt_arg[idx],msg_sng,prg_nm_get());
      msg_sng=(char *)nco_free(msg_sng);
      nco_exit(EXIT_FAILURE);
    } /* !NCO_SYNTAX_ERROR */

    /* Initialize structure */
    /* lmt strings that are not explicitly set by user remain NULL, i.e., 
    specifying default setting will appear as if nothing at all was set.
    Hopefully, in routines that follow, branch followed when dimension has
    all default settings specified (e.g.,"-d foo,,,,") yields same answer
    as branch for which no hyperslab along that dimension was set. */
    lmt[idx]=(lmt_sct *)nco_malloc(sizeof(lmt_sct));
    lmt[idx]->nm=NULL;
    lmt[idx]->is_usr_spc_lmt=True; /* True if any part of limit is user-specified, else False */
    lmt[idx]->min_sng=NULL;
    lmt[idx]->max_sng=NULL;
    lmt[idx]->srd_sng=NULL;
    lmt[idx]->drn_sng=NULL;
    lmt[idx]->mro_sng=NULL;
    /* rec_skp_ntl_spf is used for record dimension in multi-file operators */
    lmt[idx]->rec_skp_ntl_spf=0L; /* Number of records skipped in initial superfluous files */

    /* Fill-in structure */
    lmt[idx]->nm=arg_lst[0];
    lmt[idx]->min_sng=arg_lst[1];
    /* Setting min_sng and max_sng to same pointer would lead to dangerous double-free() condition */
    if(arg_nbr <= 2) lmt[idx]->max_sng=(char *)strdup(arg_lst[1]);
    if(arg_nbr > 2) lmt[idx]->max_sng=arg_lst[2]; 
    if(arg_nbr > 3) lmt[idx]->srd_sng=arg_lst[3];
    if(arg_nbr > 4) lmt[idx]->drn_sng=arg_lst[4];
    if(arg_nbr > 5) lmt[idx]->mro_sng=arg_lst[5];

    if(lmt[idx]->max_sng == NULL) lmt[idx]->is_usr_spc_max=False; else lmt[idx]->is_usr_spc_max=True;
    if(lmt[idx]->min_sng == NULL) lmt[idx]->is_usr_spc_min=False; else lmt[idx]->is_usr_spc_min=True;

    /* Initialize types used to re-base coordinate variables */
    lmt[idx]->origin=0.0;
    lmt[idx]->rbs_sng=NULL_CEWI;
    lmt[idx]->lmt_cln=cln_nil;

    /* Free current pointer array to strings, leaving untouched the strings themselves
    They will be free()'d with limit structures in nco_lmt_lst_free() */
    arg_lst=(char **)nco_free(arg_lst);
  } /* End loop over lmt structure list */

  return lmt;
} /* end nco_lmt_prs() */

int /* O [enm] Limit type */
nco_lmt_typ /* [fnc] Determine limit type */
(char *sng) /* I [ptr] Pointer to limit string */
{
  /* Purpose: Determine type of user-specified limit */

  /* Test for UDUnits unit string, then for simple coordinate, 
  then date/time string (i.e., YYYY-MM-DD), else default to dimensional index */
  if(strchr(sng,' ')) /* Space delimits user-specified units */
    return lmt_udu_sng;
  if(strchr(sng,'.') ) /* Decimal point (most common so check first) */
    return lmt_crd_val;
  if(strchr(sng,'E') || strchr(sng,'e') || /* Exponential */
    strchr(sng,'D') || strchr(sng,'d')) /* Double */
    /* Limit is "simple" (non-UDUnits) coordinate value */
    return lmt_crd_val;

  /* Check for date-like string */   
  if(strchr(sng,'-') && ((char *)strchr(sng,'-') != (char *)sng)){
    int yyyy,mm,dd;
    if(sscanf(sng,"%d-%d-%d",&yyyy,&mm,&dd) == 3) return lmt_udu_sng;
  }  /* endif date-like string */

  /* Default: Limit is dimension index */
  return lmt_dmn_idx;
} /* end nco_lmt_typ() */

char * /* O [sng] Units string */
nco_lmt_get_udu_att /* Returns specified attribute otherwise NULL */
(const int nc_id, /* I [idx] netCDF file ID */
 const int var_id,
 const char *att_nm) /* I [id] Variable ID whose attribute to read */
{
  /* Grab units attribute from disk */
  nc_type att_typ; 
  long att_sz;
  char *fl_udu_sng=NULL_CEWI;

  if(nco_inq_att_flg(nc_id,var_id,att_nm,&att_typ,&att_sz) == NC_NOERR){
    /* Allocate memory for attribute */
    if(att_typ == NC_CHAR){
      fl_udu_sng=(char *)nco_malloc((att_sz+1UL)*sizeof(char));
      /* Get 'units' attribute */
      (void)nco_get_att(nc_id,var_id,att_nm,fl_udu_sng,att_typ);
      fl_udu_sng[att_sz]='\0';
    } /* !NC_CHAR */
  } /* endif */
  return fl_udu_sng;
} /* end nco_lmt_get_udu_att() */


void
nco_prt_lmt                    /* [fnc] Print limit information */
(lmt_sct lmt,                  /* I [sct] Limit structure */
 int min_lmt_typ,              /* I [nbr] Limit type */
 nco_bool FORTRAN_IDX_CNV,     /* I [flg] Hyperslab indices obey Fortran convention */
 nco_bool flg_no_data_ok,      /* I [flg] True if file contains no data for hyperslab */
 long rec_usd_cml,             /* I [nbr] Number of valid records already processed (only used for record dimensions in multi-file operators) */
 monotonic_direction_enm monotonic_direction, /* I [enm] Monotonic_direction */
 nco_bool rec_dmn_and_mfo,     /* I [flg] True if record dimension in multi-file operator */
 long cnt_rmn_ttl,             /* I [nbr] Total records to be read from this and all remaining files */
 long cnt_rmn_crr,             /* I [nbr] Records to extract from current file */
 long rec_skp_vld_prv_dgn)     /* I [nbr] Records skipped at end of previous valid file, if any (diagnostic only) */
{
  /* Purpose: Print limit information */

  (void)fprintf(stderr,"Dimension hyperslabber nco_lmt_evl() diagnostics:\n");
  (void)fprintf(stderr,"Dimension name = %s\n",lmt.nm);
  (void)fprintf(stderr,"Limit type is %s\n",(min_lmt_typ == lmt_crd_val) ? "coordinate value" : (FORTRAN_IDX_CNV) ? "one-based dimension index" : "zero-based dimension index");
  (void)fprintf(stderr,"Limit %s user-specified\n",(lmt.is_usr_spc_lmt) ? "is" : "is not");
  (void)fprintf(stderr,"Limit %s record dimension\n",(lmt.is_rec_dmn) ? "is" : "is not");
  (void)fprintf(stderr,"Current file %s specified hyperslab, data %s be read\n",(flg_no_data_ok) ? "is superfluous to" : "is required by",(flg_no_data_ok) ? "will not" : "will");
  if(rec_dmn_and_mfo) (void)fprintf(stderr,"Cumulative number of records in all input files opened including this one = %li\n",lmt.rec_in_cml);
  if(rec_dmn_and_mfo) (void)fprintf(stderr,"Records skipped in initial superfluous files = %li\n",lmt.rec_skp_ntl_spf);
  if(rec_dmn_and_mfo) (void)fprintf(stderr,"Valid records read (and used) from previous files = %li\n",rec_usd_cml);
  if(cnt_rmn_ttl != -1L) (void)fprintf(stderr,"Total records to be read from this and all following files = %li\n",cnt_rmn_ttl);
  if(cnt_rmn_crr != -1L) (void)fprintf(stderr,"Records to be read from this file = %li\n",cnt_rmn_crr);
  if(rec_skp_vld_prv_dgn != -1L) (void)fprintf(stderr,"rec_skp_vld_prv_dgn (previous file, if any) = %li \n",rec_skp_vld_prv_dgn);
  if(rec_skp_vld_prv_dgn != -1L) (void)fprintf(stderr,"rec_skp_vld_prv (this file) = %li \n",lmt.rec_skp_vld_prv);
  (void)fprintf(stderr,"min_sng = %s\n",lmt.min_sng == NULL ? "NULL" : lmt.min_sng);
  (void)fprintf(stderr,"max_sng = %s\n",lmt.max_sng == NULL ? "NULL" : lmt.max_sng);
  (void)fprintf(stderr,"srd_sng = %s\n",lmt.srd_sng == NULL ? "NULL" : lmt.srd_sng);
  (void)fprintf(stderr,"drn_sng = %s\n",lmt.drn_sng == NULL ? "NULL" : lmt.drn_sng);
  (void)fprintf(stderr,"mro_sng = %s\n",lmt.drn_sng == NULL ? "NULL" : lmt.mro_sng);
  (void)fprintf(stderr,"monotonic_direction = %s\n",(monotonic_direction == not_checked) ? "not checked" : (monotonic_direction == increasing) ? "increasing" : "decreasing");
  (void)fprintf(stderr,"min_val = %g\n",lmt.min_val);
  (void)fprintf(stderr,"max_val = %g\n",lmt.max_val);
  (void)fprintf(stderr,"min_idx = %li\n",lmt.min_idx);
  (void)fprintf(stderr,"max_idx = %li\n",lmt.max_idx);
  (void)fprintf(stderr,"srt = %li\n",lmt.srt);
  (void)fprintf(stderr,"end = %li\n",lmt.end);
  (void)fprintf(stderr,"cnt = %li\n",lmt.cnt);
  (void)fprintf(stderr,"srd = %li\n",lmt.srd);
  (void)fprintf(stderr,"drn = %li\n",lmt.drn);
  (void)fprintf(stderr,"WRP = %s\n",lmt.srt > lmt.end ? "YES" : "NO");
  (void)fprintf(stderr,"SRD = %s\n",lmt.srd != 1L ? "YES" : "NO");
  (void)fprintf(stderr,"DRN = %s\n",lmt.drn != 1L ? "YES" : "NO");
  (void)fprintf(stderr,"MRO = %s\n\n",lmt.flg_mro ? "YES" : "NO");
}

void
nco_lmt_evl /* [fnc] Parse user-specified limits into hyperslab specifications */
(int nc_id, /* I [idx] netCDF file ID */
 lmt_sct *lmt_ptr, /* I/O [sct] Structure from nco_lmt_prs() or from nco_lmt_sct_mk() to hold dimension limit information */
 long rec_usd_cml, /* I [nbr] Number of valid records already processed (only used for record dimensions in multi-file operators) */
 nco_bool FORTRAN_IDX_CNV) /* I [flg] Hyperslab indices obey Fortran convention */
{
  /* NB: nco_lmt_evl() with same nc_id contains OpenMP critical region */
  /* Purpose: Take parsed list of dimension names, minima, and
  maxima strings and find appropriate indices into dimensions 
  for formulation of dimension start and count vectors, or fail trying. */

  char *fl_udu_sng=NULL_CEWI;   /* Store units attribute of coordinate dimension */
  char *msg_sng=NULL_CEWI; /* [sng] Error message */
  char *sng_cnv_rcd=NULL_CEWI; /* [sng] strtol()/strtoul() return code */

  nco_bool flg_no_data_err=False; /* True if domain brackets no data (and not an MFO/record coordinate) */
  nco_bool flg_no_data_ok=False; /* True if file contains no data for hyperslab */
  nco_bool rec_dmn_and_mfo=False; /* True if record dimension in multi-file operator */
  nco_bool NCO_SYNTAX_ERROR=False; /* [flg] Syntax error in hyperslab specification */

  dmn_sct dim;

  lmt_sct lmt;

  int min_lmt_typ=int_CEWI;
  int max_lmt_typ=int_CEWI;
  monotonic_direction_enm monotonic_direction=not_checked; /* CEWI */
  int prg_id; /* Program ID */
  int rcd=NC_NOERR; /* [enm] Return code */
  int rec_dmn_id; /* [idx] Variable ID of record dimension, if any */

  long dmn_sz;
  long cnt_rmn_crr=-1L; /* Records to extract from current file */
  long cnt_rmn_ttl=-1L; /* Total records to be read from this and all remaining files */
  long rec_skp_vld_prv_dgn=-1L; /* Records skipped at end of previous valid file, if any (diagnostic only) */

  lmt=*lmt_ptr;

  prg_id=prg_get(); /* Program ID */

  /* Initialize limit structure */
  lmt.flg_mro=False;
  lmt.max_val=0.0;
  lmt.min_val=0.0;
  lmt.drn=1L;
  lmt.srd=1L;
  lmt.flg_input_complete=False;

  /* Get dimension ID */
  rcd=nco_inq_dimid_flg(nc_id,lmt.nm,&lmt.id);
  if(rcd != NC_NOERR){
    (void)fprintf(stdout,"%s: ERROR dimension %s is not in input file\n",prg_nm_get(),lmt.nm);
    nco_exit(EXIT_FAILURE);
  } /* endif */

  /* Logic on whether to allow skipping current file depends on whether limit
  is specified for record dimension in multi-file operators.
  This information is not used in single-file operators, though whether
  the limit is a record limit may be tested.
  Program defensively and define this flag in all cases. */
  (void)nco_inq(nc_id,(int *)NULL,(int *)NULL,(int *)NULL,&rec_dmn_id);
  if(lmt.id == rec_dmn_id) lmt.is_rec_dmn=True; else lmt.is_rec_dmn=False;
  if(lmt.is_rec_dmn && (prg_id == ncra || prg_id == ncrcat)) rec_dmn_and_mfo=True; else rec_dmn_and_mfo=False;

  /* Get dimension size */
  (void)nco_inq_dimlen(nc_id,lmt.id,&dim.sz);

  /* Shortcut to avoid indirection */
  dmn_sz=dim.sz;
  if(rec_dmn_and_mfo){
    lmt.rec_dmn_sz=dmn_sz;
    lmt.idx_end_max_abs=lmt.rec_in_cml+dmn_sz-1L; /* Maximum allowed index in record dimension */
  } /* !rec_dmn_and_mfo */

  /* Bomb if dmn_sz < 1 */
  if(dmn_sz < 1L){
    (void)fprintf(stdout,"%s: ERROR Size of dimension %s is %li in input file, but must be > 0 in order to apply limits.\n",prg_nm_get(),lmt.nm,dmn_sz);
    nco_exit(EXIT_FAILURE);
  } /* end if */

  if(lmt.srd_sng){
    if(strchr(lmt.srd_sng,'.') || strchr(lmt.srd_sng,'e') || strchr(lmt.srd_sng,'E') || strchr(lmt.srd_sng,'d') || strchr(lmt.srd_sng,'D')){
      (void)fprintf(stdout,"%s: ERROR Requested stride for %s, %s, must be integer\n",prg_nm_get(),lmt.nm,lmt.srd_sng);
      nco_exit(EXIT_FAILURE);
    } /* end if */
    lmt.srd=strtol(lmt.srd_sng,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
    if(*sng_cnv_rcd) nco_sng_cnv_err(lmt.srd_sng,"strtol",sng_cnv_rcd);
    if(lmt.srd < 1L){
      (void)fprintf(stdout,"%s: ERROR Stride for %s is %li but must be > 0\n",prg_nm_get(),lmt.nm,lmt.srd);
      nco_exit(EXIT_FAILURE);
    } /* end if */
  } /* !lmt.srd_sng */

  if(lmt.drn_sng){
    if(strchr(lmt.drn_sng,'.') || strchr(lmt.drn_sng,'e') || strchr(lmt.drn_sng,'E') || strchr(lmt.drn_sng,'d') || strchr(lmt.drn_sng,'D')){
      (void)fprintf(stdout,"%s: ERROR Requested duration for %s, %s, must be integer\n",prg_nm_get(),lmt.nm,lmt.drn_sng);
      nco_exit(EXIT_FAILURE);
    } /* end if */
    lmt.drn=strtol(lmt.drn_sng,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
    if(*sng_cnv_rcd) nco_sng_cnv_err(lmt.drn_sng,"strtol",sng_cnv_rcd);
    if(lmt.drn < 1L){
      (void)fprintf(stdout,"%s: ERROR Duration for %s is %li but must be > 0\n",prg_nm_get(),lmt.nm,lmt.drn);
      nco_exit(EXIT_FAILURE);
    } /* end if */
    if(prg_id != ncra && prg_id != ncrcat){
      (void)fprintf(stdout,"%s: ERROR Duration only implemented for ncra and ncrcat\n",prg_nm_get());
      nco_exit(EXIT_FAILURE);
    } /* end ncra */
  } /* !lmt.drn_sng */

  if(lmt.mro_sng){
    if(strcasecmp(lmt.mro_sng,"m")){
      (void)fprintf(stdout,"%s: ERROR Requested MRO flag for %s, \"%s\", must be 'm' or 'M'\n",prg_nm_get(),lmt.nm,lmt.mro_sng);
      nco_exit(EXIT_FAILURE);
    } /* end if */
    lmt.flg_mro=True;
  } /* !lmt.mro_sng */

  /* In case flg_mro is set in ncra2.c by --mro */
  if(lmt.flg_mro){
    if(prg_id == ncrcat){
      (void)fprintf(stdout,"%s: INFO Specifying Multi-Record Output (MRO) option ('m', 'M', or --mro) is redundant. MRO is always true for ncrcat.\n",prg_nm_get());
    }else if(prg_id != ncra){
      (void)fprintf(stdout,"%s: ERROR Multi-Record Output (MRO) ('m', 'M', or --mro) is only valid for ncra.\n",prg_nm_get());
      nco_exit(EXIT_FAILURE);
    } /* end else */
  } /* !lmt.mro_sng */

  /* If min_sng and max_sng are both NULL then set type to lmt_dmn_idx */
  if(lmt.min_sng == NULL && lmt.max_sng == NULL){
    /* Limiting indices will be set to default extrema a bit later */
    min_lmt_typ=max_lmt_typ=lmt_dmn_idx;
  }else{
    /* min_sng and max_sng are not both NULL */
    /* Limit is coordinate value if string contains decimal point or is in exponential format 
    Otherwise limit is interpreted as zero-based dimension offset */
    if(lmt.min_sng) min_lmt_typ=nco_lmt_typ(lmt.min_sng);
    if(lmt.max_sng) max_lmt_typ=nco_lmt_typ(lmt.max_sng);

    /* Copy lmt_typ from defined limit to undefined */
    if(lmt.min_sng == NULL) min_lmt_typ=max_lmt_typ;
    if(lmt.max_sng == NULL) max_lmt_typ=min_lmt_typ;
  } /* end else */

  /* Both min_lmt_typ and max_lmt_typ are now defined
  Continue only if both limits are of the same type */
  if(min_lmt_typ != max_lmt_typ){
    (void)fprintf(stdout,"%s: ERROR -d %s,%s,%s\n",prg_nm_get(),lmt.nm,lmt.min_sng,lmt.max_sng);
    (void)fprintf(stdout,"Limits on dimension \"%s\" must be of same numeric type:\n",lmt.nm);
    (void)fprintf(stdout,"\"%s\" was interpreted as a %s.\n",lmt.min_sng,(min_lmt_typ == lmt_crd_val) ? "coordinate value" : (FORTRAN_IDX_CNV) ? "one-based dimension index" : "zero-based dimension index");
    (void)fprintf(stdout,"\"%s\" was interpreted as a %s.\n",lmt.max_sng,(max_lmt_typ == lmt_crd_val) ? "coordinate value" : (FORTRAN_IDX_CNV) ? "one-based dimension index" : "zero-based dimension index");
    (void)fprintf(stdout,"(Limit arguments containing a decimal point (or in exponential format) are interpreted as coordinate values; arguments without a decimal point are interpreted as zero-based or one-based (depending on -F switch) dimensional indices.)\n");
    nco_exit(EXIT_FAILURE);
  } /* end if */
  lmt.lmt_typ=min_lmt_typ;

  /* Coordinate re-basing code */
  lmt.origin=0.0;
  /* Get variable ID of coordinate */
  rcd=nco_inq_varid_flg(nc_id,lmt.nm,&dim.cid);
  if(rcd == NC_NOERR){
    char *cln_sng=NULL_CEWI;

    fl_udu_sng=nco_lmt_get_udu_att(nc_id,dim.cid,"units"); /* Units attribute of coordinate variable */
    cln_sng=nco_lmt_get_udu_att(nc_id,dim.cid,"calendar"); /* Calendar attribute */

    if(rec_dmn_and_mfo && fl_udu_sng && lmt.rbs_sng){ 
#ifdef ENABLE_UDUNITS
      /* Re-base and reset origin to 0.0 if re-basing fails */
      if(nco_cln_clc_org(fl_udu_sng,lmt.rbs_sng,lmt.lmt_cln,&lmt.origin) != EXIT_SUCCESS) lmt.origin=0.0;
#endif /* !ENABLE_UDUNITS */
    } /* endif */

    /* ncra and ncrcat read the "calendar" attribute in main() 
       Avoid multiple reads of calendar attribute in multi-file operations */
    if(!rec_dmn_and_mfo){
      if(cln_sng) lmt.lmt_cln=nco_cln_get_cln_typ(cln_sng); else lmt.lmt_cln=cln_nil;
    } /* endif */
    if(cln_sng) cln_sng=(char *)nco_free(cln_sng);
  } /* end if limit is coordinate */

  if((lmt.lmt_typ == lmt_crd_val) || (lmt.lmt_typ == lmt_udu_sng)){
    double *dmn_val_dp;

    double dmn_max;
    double dmn_min;

    long max_idx;
    long min_idx;
    long tmp_idx;
    long dmn_srt=0L;

    /* Get coordinate type */
    (void)nco_inq_vartype(nc_id,dim.cid,&dim.type);

    /* Warn when coordinate type is weird */
    if(dim.type == NC_BYTE || dim.type == NC_UBYTE || dim.type == NC_CHAR || dim.type == NC_STRING) (void)fprintf(stderr,"\n%s: WARNING Coordinate %s is type %s. Dimension truncation is unpredictable.\n",prg_nm_get(),lmt.nm,nco_typ_sng(dim.type));

    /* if(lmt.drn != 1L) (void)fprintf(stderr,"\n%s: WARNING Hyperslabs for %s are based on coordinate values rather than dimension indices. The behavior of the duration hyperslab argument is ill-defined, unpredictable, and unsupported for coordinate-based hyperslabs. Only min, max, and stride are supported for coordinate-value based hyperslabbing. Duration may or may not work as you intend. Use at your own risk.\n",prg_nm_get(),lmt.nm); */

    /* Allocate enough space to hold coordinate */
    dmn_val_dp=(double *)nco_malloc(dmn_sz*nco_typ_lng(NC_DOUBLE));

#ifdef _OPENMP
#pragma omp critical
#endif /* _OPENMP */
    { /* begin OpenMP critical */
      /* Block is critical for identical in_id's
      Block is thread-safe for distinct in_id's */
      /* 20110221: replace nco_get_vara() with nc_get_vara_double() */
      /* Retrieve this coordinate */
      nc_get_vara_double(nc_id,dim.cid,(const size_t *)&dmn_srt,(const size_t *)&dmn_sz,dmn_val_dp);
    } /* end OpenMP critical */

    /* Officially change type */
    dim.type=NC_DOUBLE;

    /* Assuming coordinate is monotonic, direction of monotonicity is determined by first two elements */
    if(dmn_sz == 1L){
      monotonic_direction=increasing;
    }else{
      if(dmn_val_dp[0] > dmn_val_dp[1]) monotonic_direction=decreasing; else monotonic_direction=increasing;
    } /* end else */

    if(monotonic_direction == increasing){
      min_idx=0L;
      max_idx=dmn_sz-1L;
    }else{
      min_idx=dmn_sz-1L;
      max_idx=0L;
    } /* end else */

    /* Determine min and max values of entire coordinate */
    dmn_min=dmn_val_dp[min_idx];
    dmn_max=dmn_val_dp[max_idx];

    /* Set defaults */  
    lmt.min_val=dmn_val_dp[min_idx]; 
    lmt.max_val=dmn_val_dp[max_idx];

    /* Convert UDUnits strings if necessary */
    /* If we are here then either min_sng or max_sng or both are set */
    if(lmt.lmt_typ == lmt_udu_sng){

      if(!fl_udu_sng){ 
        (void)fprintf(stdout,"%s: ERROR attempting to read units attribute from variable \"%s\" \n",prg_nm_get(),dim.nm);
        nco_exit(EXIT_FAILURE);
      } /* end if */

      if(lmt.min_sng && nco_cln_clc_org(lmt.min_sng,fl_udu_sng,lmt.lmt_cln,&lmt.min_val)) nco_exit(EXIT_FAILURE);
      if(lmt.max_sng && nco_cln_clc_org(lmt.max_sng,fl_udu_sng,lmt.lmt_cln,&lmt.max_val)) nco_exit(EXIT_FAILURE);

    }else{ /* end UDUnits conversion */
      /* Convert user-specified limits into double precision numeric values, or supply defaults */
      if(lmt.min_sng){
        lmt.min_val=strtod(lmt.min_sng,&sng_cnv_rcd);
        if(*sng_cnv_rcd) nco_sng_cnv_err(lmt.min_sng,"strtod",sng_cnv_rcd);
      } /* !lmt.min_sng */
      if(lmt.max_sng){
        lmt.max_val=strtod(lmt.max_sng,&sng_cnv_rcd);
        if(*sng_cnv_rcd) nco_sng_cnv_err(lmt.max_sng,"strtod",sng_cnv_rcd);
      } /* !lmt.max_sng */

      /* Re-base coordinates as necessary in multi-file operatators (MFOs)
      lmt.origin was calculated earlier in routine */
      if(rec_dmn_and_mfo){ 
        if(lmt.min_sng) lmt.min_val-=lmt.origin;
        if(lmt.max_sng) lmt.max_val-=lmt.origin;   
      }  /* endif MFO */
    } /* end UDUnits conversion */

    /* Warn when min_val > max_val (i.e., wrapped coordinate) */
    if(dbg_lvl_get() > nco_dbg_std && lmt.min_val > lmt.max_val) (void)fprintf(stderr,"%s: INFO Interpreting hyperslab specifications as wrapped coordinates [%s <= %g] and [%s >= %g]\n",prg_nm_get(),lmt.nm,lmt.max_val,lmt.nm,lmt.min_val);

    /* Fail when... */
    if(
      /* Following condition added 20000508, changes behavior of single point 
      hyperslabs depending on whether hyperslab occurs in record dimension
      during multi-file operator operation.
      Altered behavior of single point hyperslabs so that single point
      hyperslabs in the record coordinate (i.e., -d time,1.0,1.0) may be
      treated differently than single point hyperslabs in other
      coordinates. Multifile operators will skip files if single point
      hyperslabs in record coordinate lay outside record coordinate
      range of file. For non-record coordinates (and for all operators
      besides ncra and ncrcat on record coordinates), single point
      hyperslabs will choose the closest value rather than skip the file
      (I believe). This should be verified. */
      /* User specified single point, coordinate is not wrapped, and both extrema fall outside valid crd range */
      (rec_dmn_and_mfo && (lmt.min_val == lmt.max_val) && ((lmt.min_val > dmn_max) || (lmt.max_val < dmn_min))) ||
      /* User did not specify single point, coordinate is not wrapped, and either extrema falls outside valid crd range */
      ((lmt.min_val < lmt.max_val) && ((lmt.min_val > dmn_max) || (lmt.max_val < dmn_min))) ||
      /* User did not specify single point, coordinate is wrapped, and both extrema fall outside valid crd range */
      ((lmt.min_val > lmt.max_val) && ((lmt.min_val > dmn_max) && (lmt.max_val < dmn_min))) ||
      False){
        /* Allow for possibility that current file is superfluous */
        if(rec_dmn_and_mfo){
          flg_no_data_ok=True;
          goto no_data_ok;
        }else{
          (void)fprintf(stdout,"%s: ERROR User-specified coordinate value range %g <= %s <= %g does not fall within valid coordinate range %g <= %s <= %g\n",prg_nm_get(),lmt.min_val,lmt.nm,lmt.max_val,dmn_min,lmt.nm,dmn_max);
          nco_exit(EXIT_FAILURE);
        } /* end else */
    } /* end if */

    /* Armed with target coordinate minima and maxima, we are ready to bracket user-specified range */

    /* If min_sng or max_sng were omitted, use extrema */
    if(lmt.min_sng == NULL) lmt.min_idx=min_idx;
    if(lmt.max_sng == NULL) lmt.max_idx=max_idx;

    /* Single slice requires finding the closest coordinate */
    if(lmt.min_val == lmt.max_val){
      double dst_new;
      double dst_old;

      lmt.min_idx=0L;
      dst_old=fabs(lmt.min_val-dmn_val_dp[0]);
      for(tmp_idx=1L;tmp_idx<dmn_sz;tmp_idx++){
        if((dst_new=fabs(lmt.min_val-dmn_val_dp[tmp_idx])) < dst_old){
          dst_old=dst_new;
          lmt.min_idx=tmp_idx;
        } /* end if */
      } /* end loop over tmp_idx */
      lmt.max_idx=lmt.min_idx;

    }else{ /* min_val != max_val */

      /* Bracket specified extrema:
      Should no coordinate values match the given criteria, flag the index with -1L
      We defined the valid syntax such that single half range with -1L is not an error
      This causes "-d lon,100.0,-100.0" to select [-180.0] when lon=[-180.0,-90.0,0.0,90.0] 
      because one of the specified half-ranges is valid (there are coordinates < -100.0).
      However, "-d lon,100.0,-200.0" should fail when lon=[-180.0,-90.0,0.0,90.0] because both 
      of the specified half-ranges are invalid (no coordinate is > 100.0 or < -200.0).
      -1L flags are replaced with correct indices (0L or dmn_sz-1L) following search loop block.
      Overwriting -1L flags with 0L or dmn_sz-1L later is more heuristic than setting them = 0L here,
      since 0L is valid search result. */
      if(monotonic_direction == increasing){
        if(lmt.min_sng){
          /* Find index of smallest coordinate greater than min_val */
          tmp_idx=0L;
          while((dmn_val_dp[tmp_idx] < lmt.min_val) && (tmp_idx < dmn_sz)) tmp_idx++;
          if(tmp_idx != dmn_sz) lmt.min_idx=tmp_idx; else lmt.min_idx=-1L;
        } /* end if */
        if(lmt.max_sng){
          /* Find index of largest coordinate less than max_val */
          tmp_idx=dmn_sz-1L;
          while((dmn_val_dp[tmp_idx] > lmt.max_val) && (tmp_idx > -1L)) tmp_idx--;
          if(tmp_idx != -1L) lmt.max_idx=tmp_idx; else lmt.max_idx=-1L;
        } /* end if */
        /* 20110221: csz fix hyperslab bug TODO nco1007 triggered by
        ncks -O -v lat -d lat,20.,20.001 ~/nco/data/in.nc ~/foo.nc
        This returned all values but should have returned none
        Algorithm was broken because, although valid min and max indices existed,
        they contained the empty set. 
        Now when this happens, set flg_no_data_err block */
        if( /* Points are not wrapped ... */
          (lmt.min_val < lmt.max_val) && 
          /* ... and valid indices were found for both bracketing points... */
          (lmt.min_idx != -1L && lmt.max_idx != -1L) &&
          /* ...and indices contain empty set, i.e., min_idx > max_idx for increasing data... */
          lmt.min_idx > lmt.max_idx) flg_no_data_err=True;
        /* end if monotonic_direction == increasing */
      }else{ /* monotonic_direction == decreasing */
        if(lmt.min_sng){
          /* Find index of smallest coordinate greater than min_val */
          tmp_idx=dmn_sz-1L;
          while((dmn_val_dp[tmp_idx] < lmt.min_val) && (tmp_idx > -1L)) tmp_idx--;
          if(tmp_idx != -1L) lmt.min_idx=tmp_idx; else lmt.min_idx=-1L;
        } /* end if */
        if(lmt.max_sng){
          /* Find index of largest coordinate less than max_val */
          tmp_idx=0L;
          while((dmn_val_dp[tmp_idx] > lmt.max_val) && (tmp_idx < dmn_sz)) tmp_idx++;
          if(tmp_idx != dmn_sz) lmt.max_idx=tmp_idx; else lmt.max_idx=-1L;
        } /* end if */
        if( /* Points are not wrapped ... */
          (lmt.min_val > lmt.max_val) && 
          /* ... and valid indices were found for both bracketing points... */
          (lmt.min_idx != -1L && lmt.max_idx != -1L) &&
          /* ...and indices contain empty set, i.e., min_idx < max_idx for decreasing data... */
          lmt.min_idx < lmt.max_idx) flg_no_data_err=True;
      } /* end else monotonic_direction == decreasing */

      /* Case where both min_idx and max_idx = -1 was flagged as error above
      Case of wrapped coordinate: Either, but not both, of min_idx or max_idx will be flagged with -1
      See explanation above */
      if(lmt.min_idx == -1L && (lmt.min_val > lmt.max_val)) lmt.min_idx=0L;
      if(lmt.max_idx == -1L && (lmt.min_val > lmt.max_val)) lmt.max_idx=dmn_sz-1L;

    } /* end if min_val != max_val */

    /* User-specified ranges are now bracketed */

    /* Convert indices of minima and maxima to srt and end indices */
    if(monotonic_direction == increasing){
      lmt.srt=lmt.min_idx;
      lmt.end=lmt.max_idx;
    }else{
      lmt.srt=lmt.max_idx;
      lmt.end=lmt.min_idx;
    }  /* end else */

    /* Free space allocated for dimension */
    dmn_val_dp=(double*)nco_free(dmn_val_dp);

    if(rec_dmn_and_mfo){ 
      /* No wrapping with multi-file operators */ 
      if((monotonic_direction == increasing && lmt.min_val > lmt.max_val) ||
        (monotonic_direction == decreasing && lmt.min_val < lmt.max_val)){
          flg_no_data_ok=True; 
          goto no_data_ok;   
      } /* endif */

      if(rec_usd_cml == 0L){
        /* Skipped records remains zero until valid records are processed */
        lmt.rec_skp_vld_prv=0L;  
      }else if(rec_usd_cml > 0L){
        /* Otherwise, adjust starting index by records skipped in jumps across file boundaries */
        lmt.srt+=lmt.srd-1L-lmt.rec_skp_vld_prv%lmt.srd; 
        if(lmt.srt>lmt.end){
          /* Do not allow record dimension wrapping in MFOs */
          flg_no_data_ok=True;
          goto no_data_ok;
        } /* endif  */
      } /* endif */

      /* If we are here then there are valid records in current file */

    } /* end if rec_dmn_and_mfo */

  }else{ /* end if limit arguments were coordinate values */
    /* Convert limit strings to zero-based indicial offsets */

    /* Specifying stride alone, but not min or max, is legal, e.g., -d time,,,2
    Thus is_usr_spc_lmt may be True, even though one or both of min_sng, max_sng is NULL
    Furthermore, both min_sng and max_sng are artifically created by nco_lmt_sct_mk()
    for record dimensions when the user does not explicitly specify limits.
    In this case, min_sng_and max_sng are non-NULL though no limits were specified
    In fact, min_sng and max_sng are set to the minimum and maximum string
    values of the first file processed.
    However, we can tell if these strings were artificially generated because 
    nco_lmt_sct_mk() sets the is_usr_spc_lmt flag to False in such cases.
    Subsequent files may have different numbers of records, but nco_lmt_sct_mk()
    is only called once.
    Thus we must update min_idx and max_idx here for each file
    This causes min_idx and max_idx to be out of sync with min_sng and max_sng, 
    which are only set in nco_lmt_sct_mk() for the first file.
    In hindsight, artificially generating min_sng and max_sng may be bad idea */
    /* Following logic is messy, but hard to simplify */
    if(lmt.min_sng == NULL || !lmt.is_usr_spc_lmt){
      /* No user-specified value available--generate minimal dimension index */
      if(FORTRAN_IDX_CNV) lmt.min_idx=1L; else lmt.min_idx=0L;
    }else{
      /* Use user-specified limit when available */
      lmt.min_idx=strtol(lmt.min_sng,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
      if(*sng_cnv_rcd) nco_sng_cnv_err(lmt.min_sng,"strtol",sng_cnv_rcd);
    } /* end if */
    if(lmt.max_sng == NULL || !lmt.is_usr_spc_lmt){
      /* No user-specified value available---generate maximal dimension index */
      if(FORTRAN_IDX_CNV) lmt.max_idx=dmn_sz; else lmt.max_idx=dmn_sz-1L;
    }else{
      /* Use user-specified limit when available */
      lmt.max_idx=strtol(lmt.max_sng,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
      if(*sng_cnv_rcd) nco_sng_cnv_err(lmt.max_sng,"strtol",sng_cnv_rcd);
    } /* end if */

    /* Adjust indices if FORTRAN style input was specified */
    if(FORTRAN_IDX_CNV){
      /* 20120726: Die when Fortran index is zero */
      if(lmt.min_idx == 0L || lmt.max_idx == 0L){
        (void)fprintf(stdout,"%s: ERROR User-specified Fortran (1-based) index for dimension %s = 0.\n",prg_nm_get(),lmt.nm);
        msg_sng=strdup("Fortran indices must be >= 1");
        NCO_SYNTAX_ERROR=True;
      } /* endif illegal Fortran index */
      /* 20120709: Adjust positive indices only */
      if(lmt.min_idx > 0L) lmt.min_idx--;
      if(lmt.max_idx > 0L) lmt.max_idx--;
    } /* end if */

    /* 20120709 Negative integer as min or max element of hyperslab specification indicates offset from end */
    if(lmt.min_idx < 0L) lmt.min_idx+=dmn_sz-1L;
    if(lmt.max_idx < 0L) lmt.max_idx+=dmn_sz-1L;

    /* Exit if requested indices are always invalid for all operators... */
    if(lmt.min_idx < 0L){
      msg_sng=strdup("Minimum index is too negative");
      NCO_SYNTAX_ERROR=True;
    }else if(lmt.max_idx < 0L){
      msg_sng=strdup("Maximum index is too negative");
      NCO_SYNTAX_ERROR=True;
    }else if(lmt.drn > lmt.srd){
      (void)fprintf(stdout,"%s: ERROR User-specified duration exceeds stride for %s: %li > %li\n",prg_nm_get(),lmt.nm,lmt.drn,lmt.srd);
      msg_sng=strdup("Duration exceeds stride");
      NCO_SYNTAX_ERROR=True;
    }else if(!rec_dmn_and_mfo && lmt.min_idx >= dmn_sz){
      msg_sng=strdup("Minimum index greater than size in non-MFO");
      NCO_SYNTAX_ERROR=True;
      (void)fprintf(stdout,"%s: ERROR User-specified dimension index range %li <= %s <= %li does not fall within valid dimension index range 0 <= %s <= %li\n",prg_nm_get(),lmt.min_idx,lmt.nm,lmt.max_idx,lmt.nm,dmn_sz-1L);
    } /* end if impossible indices */

    if(NCO_SYNTAX_ERROR){
      (void)fprintf(stdout,"%s: ERROR evaluating hyperslab specification for %s: %s\n%s: HINT Conform request to hyperslab documentation at http://nco.sf.net/nco.html#hyp\n",prg_nm_get(),lmt.nm,msg_sng,prg_nm_get());
      msg_sng=(char *)nco_free(msg_sng);
      nco_exit(EXIT_FAILURE);
    } /* !NCO_SYNTAX_ERROR */

    /* NB: Duration is officially supported only for ncra and ncrcat (record dimension only) */
    if(lmt.drn != 1L && !rec_dmn_and_mfo) (void)fprintf(stderr,"%s: WARNING Duration argument is only supported for the record dimension on ncra and ncrcat operations\n",prg_nm_get());

    /* Logic depends on whether this is record dimension in multi-file operator */
    if(!rec_dmn_and_mfo || !lmt.is_usr_spc_lmt){
      /* For non-record dimensions and for record dimensions where limit 
      was automatically generated (to include whole file), starting
      and ending indices are simply minimum and maximum indices already 
      in structure */
      lmt.srt=lmt.min_idx;
      lmt.end=lmt.max_idx;
    }else{
      /* Initialize rec_skp_vld_prv to 0L on first call to nco_lmt_evl() 
      This is necessary due to intrinsic hysterisis of rec_skp_vld_prv
      rec_skp_vld_prv is used only by multi-file operators
      rec_skp_vld_prv counts records skipped at end of previous valid file
      rec_usd_cml and rec_skp_ntl_spf are both zero only for first file */
      if(rec_usd_cml == 0L && lmt.rec_skp_ntl_spf == 0L) lmt.rec_skp_vld_prv=0L;

      /* For record dimensions with user-specified limit, allow possibility 
      that limit pertains to record dimension in a multi-file operator.
      Then user-specified maximum index may exceed number of records in any one file
      Thus lmt.srt does not necessarily equal lmt.min_idx and 
      lmt.end does not necessarily equal lmt.max_idx */
      /* NB: Stride is officially supported for ncks (all dimensions) and for ncra and ncrcat (record dimension only) */
      if(lmt.srd != 1L && prg_id != ncks && !lmt.is_rec_dmn) (void)fprintf(stderr,"%s: WARNING Stride argument for non-record dimension is only supported by ncks, use at your own risk...\n",prg_nm_get());

      { /* Block hides scope of local internal variables */
        long srt_min_lcl; /* [idx] Minimum start index (in absolute index space, i.e., relative to first file) for current file */
        long end_max_lcl; /* [idx] Maximum end   index (in absolute index space, i.e., relative to first file) for current file */

        srt_min_lcl=(lmt.is_usr_spc_min ? lmt.min_idx : lmt.rec_in_cml+0L); 
        end_max_lcl=(lmt.is_usr_spc_max ? lmt.max_idx : lmt.rec_in_cml+dmn_sz-1L); 

        /* Maximum allowed index in record dimension */
        lmt.idx_end_max_abs=end_max_lcl;

        /* Are we past file containing end_max_lcl yet? */
        if(end_max_lcl < lmt.rec_in_cml){
          /* This and all subsequent files are superfluous because all requested records have already been read 
          Optimize MFOs by checking "input complete" flag to jump out of file loop
          Saves time because no other input files will be opened */
          lmt.flg_input_complete=True;
          flg_no_data_ok=True;
          goto no_data_ok;
        } /* endif past end_max_lcl */

        /* Have we reached file containing srt_min_lcl yet? */
        if(srt_min_lcl > lmt.rec_in_cml+dmn_sz-1L){
          /* This and all previous files are superfluous because the starting record is in a subsequent file */
          flg_no_data_ok=True;
          goto no_data_ok;
        } /* endif srt_min_lcl in future file */

        /* Until records have been used, start index is srt_min_lcl adjusted for records contained in all previous files
        Thereafter start index loses memory of/dependence on absolute start index, and only cares for how many records,
        if any, were skipped since last valid record. This number, modulo stride, is new start index. */
        if(rec_usd_cml == 0L) lmt.srt=srt_min_lcl-lmt.rec_in_cml; else lmt.srt=lmt.srd-1L-lmt.rec_skp_vld_prv%lmt.srd;

        if(lmt.srt > dmn_sz-1L){
          /* Perhaps data were read in previous file(s) yet next record is in future file due to long stride */
          flg_no_data_ok=True;
          goto no_data_ok;
        } /* endif */

        lmt.end=(end_max_lcl < lmt.rec_in_cml+dmn_sz) ? end_max_lcl-lmt.rec_in_cml : dmn_sz-1L;

      } /* end block hides scope of local internal variables */

      /* If we are here then there are valid records in current file */

    } /* endif user-specified limits to record dimension */

  } /* end else limit arguments are hyperslab indices */

  /* NB: MFO record dimension never reaches this block if current file is superfluous
  In that case code has already branched down to flg_data_ok or flg_data_err */
  if(rec_dmn_and_mfo){ 
    /* NB: This is---and must be---performed as integer arithmetic */ 
    cnt_rmn_crr=1L+(lmt.end-lmt.srt)/lmt.srd;  
    /* This fixes "sloppy" specification of end index by user, i.e., ensures that end index coincides with a stride */
    lmt.end=lmt.srt+(cnt_rmn_crr-1L)*lmt.srd;   
    /* Save current rec_skp_vld_prv for diagnostics (printed below) for this file */
    rec_skp_vld_prv_dgn=lmt.rec_skp_vld_prv;
    /* Next file must know how many records in this file come after (and thus will be skipped) last used record in this file */
    lmt.rec_skp_vld_prv=dmn_sz-1L-lmt.end;
  } /* !rec_dmn_and_mfo */      

  /* Compute cnt from srt, end, and srd
  This is fine for multi-file record dimensions since those operators read-in one
  record at a time and thus never actually use lmt.cnt for record dimension. */
  if(lmt.srd == 1L){
    if(lmt.srt <= lmt.end) lmt.cnt=lmt.end-lmt.srt+1L; else lmt.cnt=dmn_sz-lmt.srt+lmt.end+1L;
  }else{
    if(lmt.srt <= lmt.end) lmt.cnt=1L+(lmt.end-lmt.srt)/lmt.srd; else lmt.cnt=1L+((dmn_sz-lmt.srt)+lmt.end)/lmt.srd;
  } /* end else */

  /* NB: Degenerate cases of WRP && SRD exist for which dmn_cnt_2 == 0
  This occurs when srd is large enough, or max_idx small enough, 
  such that no values are selected in the second read. 
  e.g., "-d lon,60,0,10" if sz(lon)=128 has dmn_cnt_2 == 0
  Since netCDF library reports an error reading and writing cnt=0 dimensions, kludge is necessary
  Syntax ensures that it is always the second read, not the first, which is obviated
  Therefore we convert these degenerate cases into non-wrapped coordinates to be processed by single read 
  For these degenerate cases only, [srt,end] are not a permutation of [min_idx,max_idx] */
  if(
    (lmt.srd != 1L) && /* SRD */
    (lmt.srt > lmt.end) && /* WRP */
    (lmt.cnt == (1L+(dmn_sz-lmt.srt-1L)/lmt.srd)) && /* dmn_cnt_1 == cnt -> dmn_cnt_2 == 0 */
    True){
      long greatest_srd_multiplier_1st_hyp_slb; /* Greatest integer m such that srt+m*srd < dmn_sz */
      long last_good_idx_1st_hyp_slb; /* C-index of last valid member of 1st hyperslab (= srt+m*srd) */
      /* long left_over_idx_1st_hyp_slb;*/ /* # of elements from first hyperslab that count towards current stride */
      long first_good_idx_2nd_hyp_slb; /* C-index of first valid member of 2nd hyperslab, if any */

      /* NB: Perform these operations with integer arithmetic or else! */
      /* Wrapped dimensions with stride may not start at idx 0 on second read */
      greatest_srd_multiplier_1st_hyp_slb=(dmn_sz-lmt.srt-1L)/lmt.srd;
      last_good_idx_1st_hyp_slb=lmt.srt+lmt.srd*greatest_srd_multiplier_1st_hyp_slb;
      /* left_over_idx_1st_hyp_slb=dmn_sz-last_good_idx_1st_hyp_slb-1L;*/
      first_good_idx_2nd_hyp_slb=(last_good_idx_1st_hyp_slb+lmt.srd)%dmn_sz;

      /* Conditions causing dmn_cnt_2 == 0 */
      if(first_good_idx_2nd_hyp_slb > lmt.end) lmt.end=last_good_idx_1st_hyp_slb;
  } /* end if */

  /* Cases where domain brackets no data, in error, have counts set to zero here
  This kludge allows codepaths for both WRP and out-of-domain to flow without goto statements
  Out-of-domain errors will soon exit with error, while WRP conditions will proceed */
  if(flg_no_data_err) lmt.cnt=0L;

  /* Exit when valid bracketed range contains no coordinates and that is not legal,
  i.e., this is not a superfluous file in an MFO */
  if(lmt.cnt == 0){
    if(lmt.lmt_typ == lmt_crd_val) (void)fprintf(stdout,"%s: ERROR Domain %g <= %s <= %g brackets no coordinate values.\n",prg_nm_get(),lmt.min_val,lmt.nm,lmt.max_val); 
    if(lmt.lmt_typ == lmt_dmn_idx) (void)fprintf(stdout,"%s: ERROR Empty domain for %s\n",prg_nm_get(),lmt.nm); 
    nco_exit(EXIT_FAILURE);
  } /* end if */

  /* Coordinate-valued limits that bracket no values in current file jump here with goto
  Index-valued limits with no values in current file flow here naturally */
no_data_ok: /* end goto */
  if(flg_no_data_ok){
    /* File is superfluous (contributes no data) to specified hyperslab
    Set output parameters to well-defined state
    This state must not cause ncra or ncrcat to retrieve any data
    ncra and ncrcat use loops for the record dimension, so this is
    accomplished by setting loop control values (lmt_rec.srt > lmt_rec.end)
    that cause record loop always to be skipped (never entered) */
    lmt.srt=-1L;
    lmt.end=lmt.srt-1L;
    lmt.cnt=-1L;
    /* Augment number of records skipped in initial superfluous files */
    if(rec_usd_cml == 0L) lmt.rec_skp_ntl_spf+=dmn_sz; 
    /* Augment records skipped since last good one */ 
    lmt.rec_skp_vld_prv+=dmn_sz;
    /* Set variables to preserve utility of diagnostics at end of routine */
    cnt_rmn_crr=rec_skp_vld_prv_dgn=0L;
  } /* endif */

  /* Accumulate count of records in all opened files, including this one
  Increment here at end so this structure member includes records from current file 
  only at end of this routine, where it can only be used diagnostically
  NB: Location of this augmentation is important! Moving it would have side-effects! */
  lmt.rec_in_cml+=dmn_sz;

  /* Place contents of working structure in location of returned structure */
  *lmt_ptr=lmt;

  if(dbg_lvl_get() == nco_dbg_io){
    (void)nco_prt_lmt(lmt,min_lmt_typ,FORTRAN_IDX_CNV,flg_no_data_ok,rec_usd_cml,monotonic_direction,rec_dmn_and_mfo,cnt_rmn_ttl,cnt_rmn_crr,rec_skp_vld_prv_dgn);
  } /* end dbg */

  if(lmt.srt > lmt.end && !flg_no_data_ok){
    if(prg_id != ncks) (void)fprintf(stderr,"WARNING: Possible instance of Schweitzer data hole requiring better diagnostics TODO #148\n");
    if(prg_id != ncks) (void)fprintf(stderr,"HINT: If operation fails, try multislabbing (http://nco.sf.net/nco.html#msa) wrapped dimension using ncks first, and then apply %s to the resulting file\n",prg_nm_get());
  } /* end dbg */

  fl_udu_sng=(char *)nco_free(fl_udu_sng);

} /* end nco_lmt_evl() */


/* Deprecate */
void                      
nco_lmt_evl_dmn_trv            /* [fnc] Parse user-specified limits into hyperslab specifications (trv version) */
(const int nc_id,              /* I [ID] netCDF file ID */
 lmt_sct *lmt_ptr,             /* I/O [sct] Structure from nco_lmt_prs()  */
 long rec_usd_cml,             /* I [nbr] Number of valid records already processed (only used for record dimensions in multi-file operators) */
 nco_bool FORTRAN_IDX_CNV,     /* I [flg] Hyperslab indices obey Fortran convention */
 dmn_trv_sct *dmn_trv)         /* I [sct] Structure from GTT (unique dimension in table list, defined in a *group*) */
{
  /* Purpose: Take parsed list of dimension names, minima, and maxima strings and find appropriate indices into dimensions 
  for formulation of dimension start and count vectors, or fail trying. 

  Based on original nco_lmt_evl(). Differrences are marked GTT

  Goal here is to take input from both "lmt_ptr" and "dmn_trv" and match them on output 
  Use case example:
  /lon(4)
  /g8/lon(2)
  ncks -d lon,0,3,1 -v lon -H ~/nco/data/in_grp.nc
  "-d lon,0,3,1" is valid for /lon(4) but not for /g8/lon(2)

  Reminder:
  Coordinate values should be specified using real notation with a decimal point required in the value, 
  whereas dimension indices are specified using integer notation without a decimal point.

  ncks -d lat,-90.,90.,1 -H -v area ~/nco/data/in_grp.nc  # limit type is defined as lmt_crd_val
  ncks -d lat,0,1,1 -H -v area ~/nco/data/in_grp.nc  # limit type is defined as lmt_dmn_idx

  lmt_crd_val,  0, Coordinate value limit 
  lmt_dmn_idx,  1, Dimension index limit 
  lmt_udu_sng   2, UDUnits string 

  Tests:
  ncks -D 11 -d lon,0.,90.,1 -v lon -H ~/nco/data/in_grp.nc
  ncks -D 11 -d lon,0,1,1 -v lon -H ~/nco/data/in_grp.nc
  */

  const char fnc_nm[]="nco_lmt_evl_dmn_trv()"; /* [sng] Function name */

  char *fl_udu_sng=NULL_CEWI;     /* [sng] Store units attribute of coordinate dimension */
  char *msg_sng=NULL_CEWI;        /* [sng] Error message */
  char *sng_cnv_rcd=NULL_CEWI;    /* [sng] strtol()/strtoul() return code */

  nco_bool flg_no_data_err=False; /* [flg] True if domain brackets no data (and not an MFO/record coordinate) */
  nco_bool flg_no_data_ok=False;  /* [flg] True if file contains no data for hyperslab */
  nco_bool rec_dmn_and_mfo=False; /* [flg] True if record dimension in multi-file operator */
  nco_bool NCO_SYNTAX_ERROR=False;/* [flg] Syntax error in hyperslab specification */

  dmn_sct dim;                     /* [sct] Dimension Structure  */

  lmt_sct lmt;                     /* [sct] Structure from nco_lmt_prs()  */

  int min_lmt_typ=int_CEWI;
  int max_lmt_typ=int_CEWI; 
  int prg_id;                      /* [enm] Program ID */
  int rcd=NC_NOERR;                /* [rcd] Return code */

  monotonic_direction_enm monotonic_direction=not_checked; /* CEWI */

  long dmn_sz;                     /* [nbr] Dimension size */
  long cnt_rmn_crr=-1L;            /* [nbr] Records to extract from current file */
  long cnt_rmn_ttl=-1L;            /* [nbr] Total records to be read from this and all remaining files */
  long rec_skp_vld_prv_dgn=-1L;    /* [nbr] Records skipped at end of previous valid file, if any (diagnostic only) */

  int var_id=-1;                   /* [id] ID of variable */
  int grp_id=-1;                   /* [id] ID of group */

  nc_type var_typ=NC_NAT;          /* [enm] Type of variable */

  nco_bool is_crd_var;             /* [flg] Does the dimension have a coordinate variable ? */         

  lmt=*lmt_ptr;

  prg_id=prg_get(); 

  /* Initialize limit structure */
  /* TODO: fxm pvn Initialize all members of "lmt" */

  lmt.flg_mro=False;
  lmt.max_val=0.0;
  lmt.min_val=0.0;
  lmt.drn=1L;
  lmt.srd=1L;
  lmt.flg_input_complete=False;

  /* Obtain group ID using full group name */
  (void)nco_inq_grp_full_ncid(nc_id,dmn_trv->grp_nm_fll,&grp_id);

  /* Obtain variable ID using group ID */
  rcd=nco_inq_varid_flg(grp_id,dmn_trv->nm,&var_id);
  if(rcd != NC_NOERR) is_crd_var=False; else is_crd_var=True;

  if(dbg_lvl_get() == nco_dbg_old){
    if (is_crd_var == False )
      (void)fprintf(stdout,"%s: INFO %s dimension <%s> does not have a coordinate variable:\n",prg_nm_get(),fnc_nm,
      dmn_trv->nm_fll);
  }

  /* Get coordinate type */
  if (is_crd_var == True ) (void)nco_inq_vartype(grp_id,var_id,&var_typ);

  /* Get dimension ID. NOTE: using group ID */
  rcd=nco_inq_dimid_flg(grp_id,lmt.nm,&lmt.id);
  if(rcd != NC_NOERR){
    (void)fprintf(stdout,"%s: ERROR dimension %s is not in input file\n",prg_nm_get(),lmt.nm);
    nco_exit(EXIT_FAILURE);
  } /* endif */

  /* Logic on whether to allow skipping current file depends on whether limit
  is specified for record dimension in multi-file operators.
  This information is not used in single-file operators, though whether
  the limit is a record limit may be tested.
  Program defensively and define this flag in all cases. */

#ifdef IDS_NOT_ALLOWED
  (void)nco_inq(nc_id,(int *)NULL,(int *)NULL,(int *)NULL,&rec_dmn_id);
  if(lmt.id == rec_dmn_id) lmt.is_rec_dmn=True; else lmt.is_rec_dmn=False;
  if(lmt.is_rec_dmn && (prg_id == ncra || prg_id == ncrcat)) rec_dmn_and_mfo=True; else rec_dmn_and_mfo=False;
#else /* IDS_NOT_ALLOWED */

  if(prg_id == ncks) rec_dmn_and_mfo=False;

  /* Use info from unique dimension */
  lmt.is_rec_dmn=dmn_trv->is_rec_dmn;

  if(lmt.is_rec_dmn && (prg_id == ncra || prg_id == ncrcat)) rec_dmn_and_mfo=True; else rec_dmn_and_mfo=False;

#endif /* IDS_NOT_ALLOWED */

  /* Get dimension size. NOTE: using group ID */
  (void)nco_inq_dimlen(grp_id,lmt.id,&dim.sz);

  /* Shortcut to avoid indirection */
  dmn_sz=dim.sz;

  /* No need to inquire netCDF: "dmn_trv" already has the dimension size, but do it for sanity check */
  assert(dmn_sz == dmn_trv->sz);

  if(rec_dmn_and_mfo){
    lmt.rec_dmn_sz=dmn_sz;
    lmt.idx_end_max_abs=lmt.rec_in_cml+dmn_sz-1L; /* Maximum allowed index in record dimension */
  } /* !rec_dmn_and_mfo */

  /* Bomb if dmn_sz < 1 */
  if(dmn_sz < 1L){
    (void)fprintf(stdout,"%s: ERROR Size of dimension %s is %li in input file, but must be > 0 in order to apply limits.\n",prg_nm_get(),lmt.nm,dmn_sz);
    nco_exit(EXIT_FAILURE);
  } /* end if */

  if(lmt.srd_sng){
    if(strchr(lmt.srd_sng,'.') || strchr(lmt.srd_sng,'e') || strchr(lmt.srd_sng,'E') || strchr(lmt.srd_sng,'d') || strchr(lmt.srd_sng,'D')){
      (void)fprintf(stdout,"%s: ERROR Requested stride for %s, %s, must be integer\n",prg_nm_get(),lmt.nm,lmt.srd_sng);
      nco_exit(EXIT_FAILURE);
    } /* end if */
    lmt.srd=strtol(lmt.srd_sng,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
    if(*sng_cnv_rcd) nco_sng_cnv_err(lmt.srd_sng,"strtol",sng_cnv_rcd);
    if(lmt.srd < 1L){
      (void)fprintf(stdout,"%s: ERROR Stride for %s is %li but must be > 0\n",prg_nm_get(),lmt.nm,lmt.srd);
      nco_exit(EXIT_FAILURE);
    } /* end if */
  } /* !lmt.srd_sng */

  if(lmt.drn_sng){
    if(strchr(lmt.drn_sng,'.') || strchr(lmt.drn_sng,'e') || strchr(lmt.drn_sng,'E') || strchr(lmt.drn_sng,'d') || strchr(lmt.drn_sng,'D')){
      (void)fprintf(stdout,"%s: ERROR Requested duration for %s, %s, must be integer\n",prg_nm_get(),lmt.nm,lmt.drn_sng);
      nco_exit(EXIT_FAILURE);
    } /* end if */
    lmt.drn=strtol(lmt.drn_sng,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
    if(*sng_cnv_rcd) nco_sng_cnv_err(lmt.drn_sng,"strtol",sng_cnv_rcd);
    if(lmt.drn < 1L){
      (void)fprintf(stdout,"%s: ERROR Duration for %s is %li but must be > 0\n",prg_nm_get(),lmt.nm,lmt.drn);
      nco_exit(EXIT_FAILURE);
    } /* end if */
    if(prg_id != ncra && prg_id != ncrcat){
      (void)fprintf(stdout,"%s: ERROR Duration only implemented for ncra and ncrcat\n",prg_nm_get());
      nco_exit(EXIT_FAILURE);
    } /* end ncra */
  } /* !lmt.drn_sng */

  if(lmt.mro_sng){
    if(strcasecmp(lmt.mro_sng,"m")){
      (void)fprintf(stdout,"%s: ERROR Requested MRO flag for %s, \"%s\", must be 'm' or 'M'\n",prg_nm_get(),lmt.nm,lmt.mro_sng);
      nco_exit(EXIT_FAILURE);
    } /* end if */
    lmt.flg_mro=True;
  } /* !lmt.mro_sng */

  /* In case flg_mro is set in ncra2.c by --mro */
  if(lmt.flg_mro){
    if(prg_id == ncrcat){
      (void)fprintf(stdout,"%s: INFO Specifying Multi-Record Output (MRO) option ('m', 'M', or --mro) is redundant. MRO is always true for ncrcat.\n",prg_nm_get());
    }else if(prg_id != ncra){
      (void)fprintf(stdout,"%s: ERROR Multi-Record Output (MRO) ('m', 'M', or --mro) is only valid for ncra.\n",prg_nm_get());
      nco_exit(EXIT_FAILURE);
    } /* end else */
  } /* !lmt.mro_sng */

  /* If min_sng and max_sng are both NULL then set type to lmt_dmn_idx */
  if(lmt.min_sng == NULL && lmt.max_sng == NULL){
    /* Limiting indices will be set to default extrema a bit later */
    min_lmt_typ=max_lmt_typ=lmt_dmn_idx;
  }else{
    /* min_sng and max_sng are not both NULL */
    /* Limit is coordinate value if string contains decimal point or is in exponential format 
    Otherwise limit is interpreted as zero-based dimension offset */
    if(lmt.min_sng) min_lmt_typ=nco_lmt_typ(lmt.min_sng);
    if(lmt.max_sng) max_lmt_typ=nco_lmt_typ(lmt.max_sng);

    /* Copy lmt_typ from defined limit to undefined */
    if(lmt.min_sng == NULL) min_lmt_typ=max_lmt_typ;
    if(lmt.max_sng == NULL) max_lmt_typ=min_lmt_typ;
  } /* end else */

  /* Both min_lmt_typ and max_lmt_typ are now defined
  Continue only if both limits are of the same type */
  if(min_lmt_typ != max_lmt_typ){
    (void)fprintf(stdout,"%s: ERROR -d %s,%s,%s\n",prg_nm_get(),lmt.nm,lmt.min_sng,lmt.max_sng);
    (void)fprintf(stdout,"Limits on dimension \"%s\" must be of same numeric type:\n",lmt.nm);
    (void)fprintf(stdout,"\"%s\" was interpreted as a %s.\n",lmt.min_sng,(min_lmt_typ == lmt_crd_val) ? "coordinate value" : (FORTRAN_IDX_CNV) ? "one-based dimension index" : "zero-based dimension index");
    (void)fprintf(stdout,"\"%s\" was interpreted as a %s.\n",lmt.max_sng,(max_lmt_typ == lmt_crd_val) ? "coordinate value" : (FORTRAN_IDX_CNV) ? "one-based dimension index" : "zero-based dimension index");
    (void)fprintf(stdout,"(Limit arguments containing a decimal point (or in exponential format) are interpreted as coordinate values; arguments without a decimal point are interpreted as zero-based or one-based (depending on -F switch) dimensional indices.)\n");
    nco_exit(EXIT_FAILURE);
  } /* end if */
  lmt.lmt_typ=min_lmt_typ;

  /* Coordinate re-basing code */
  lmt.origin=0.0;

  /* Get variable ID of coordinate. NOTE: using group ID */
  rcd=nco_inq_varid_flg(grp_id,lmt.nm,&dim.cid);

  /* If there is a coordinate variable */
  if(rcd == NC_NOERR){

    char *cln_sng=NULL_CEWI;

    fl_udu_sng=nco_lmt_get_udu_att(grp_id,var_id,"units"); /* Units attribute of coordinate variable */
    cln_sng=nco_lmt_get_udu_att(grp_id,var_id,"calendar"); /* Calendar attribute */

    if(rec_dmn_and_mfo && fl_udu_sng && lmt.rbs_sng){ 
#ifdef ENABLE_UDUNITS
      /* Re-base and reset origin to 0.0 if re-basing fails */
      if(nco_cln_clc_org(fl_udu_sng,lmt.rbs_sng,lmt.lmt_cln,&lmt.origin) != EXIT_SUCCESS) lmt.origin=0.0;
#endif /* !ENABLE_UDUNITS */
    } /* endif */

    /* ncra and ncrcat read the "calendar" attribute in main() 
    Avoid multiple reads of calendar attritbute in multi-file operations */
    if(!rec_dmn_and_mfo){
      if(cln_sng) lmt.lmt_cln=nco_cln_get_cln_typ(cln_sng); else lmt.lmt_cln=cln_nil;
    } /* endif */
    if(cln_sng) cln_sng=(char *)nco_free(cln_sng);
  } /* End Needed only to read variable, if dimension is a coordinate variable */


  if((lmt.lmt_typ == lmt_crd_val) || (lmt.lmt_typ == lmt_udu_sng)){
    double *dmn_val_dp=NULL;

    double dmn_max;
    double dmn_min;

    long max_idx;
    long min_idx;
    long tmp_idx;
    long dmn_srt=0L;    

    /* Warn when coordinate type is weird */
    if(var_typ == NC_BYTE || var_typ == NC_UBYTE || var_typ == NC_CHAR || var_typ == NC_STRING) (void)fprintf(stderr,"\n%s: WARNING Coordinate %s is type %s. Dimension truncation is unpredictable.\n",prg_nm_get(),lmt.nm,nco_typ_sng(var_typ));

    /* Allocate enough space to hold coordinate */
    dmn_val_dp=(double *)nco_malloc(dmn_sz*nco_typ_lng(NC_DOUBLE));

#ifdef _OPENMP
#pragma omp critical
#endif /* _OPENMP */
    { /* begin OpenMP critical */
      /* Block is critical for identical in_id's
      Block is thread-safe for distinct in_id's */
      /* 20110221: replace nco_get_vara() with nc_get_vara_double() */
      /* Retrieve this coordinate */
      rcd=nc_get_vara_double(grp_id,var_id,(const size_t *)&dmn_srt,(const size_t *)&dmn_sz,dmn_val_dp);

      /* Exit if read error */
      if(rcd != NC_NOERR) nco_err_exit(rcd,"nc_get_vara_double()");

    } /* end OpenMP critical */

    /* Officially change type */
    var_typ=NC_DOUBLE;

    /* Assuming coordinate is monotonic, direction of monotonicity is determined by first two elements */
    if(dmn_sz == 1L){
      monotonic_direction=increasing;
    }else{
      if(dmn_val_dp[0] > dmn_val_dp[1]) monotonic_direction=decreasing; else monotonic_direction=increasing;
    } /* end else */

    if(monotonic_direction == increasing){
      min_idx=0L;
      max_idx=dmn_sz-1L;
    }else{
      min_idx=dmn_sz-1L;
      max_idx=0L;
    } /* end else */

    /* Determine min and max values of entire coordinate */
    dmn_min=dmn_val_dp[min_idx];
    dmn_max=dmn_val_dp[max_idx];

    /* Set defaults */  
    lmt.min_val=dmn_val_dp[min_idx]; 
    lmt.max_val=dmn_val_dp[max_idx];

    /* Convert UDUnits strings if necessary */
    /* If we are here then either min_sng or max_sng or both are set */
    if(lmt.lmt_typ == lmt_udu_sng){

      if(!fl_udu_sng){ 
        (void)fprintf(stdout,"%s: ERROR attempting to read units attribute from variable \"%s\" \n",prg_nm_get(),lmt.nm);
        nco_exit(EXIT_FAILURE);
      } /* end if */

      if(lmt.min_sng && nco_cln_clc_org(lmt.min_sng,fl_udu_sng,lmt.lmt_cln,&lmt.min_val)) nco_exit(EXIT_FAILURE);
      if(lmt.max_sng && nco_cln_clc_org(lmt.max_sng,fl_udu_sng,lmt.lmt_cln,&lmt.max_val)) nco_exit(EXIT_FAILURE);

    }else{ /* end UDUnits conversion */
      /* Convert user-specified limits into double precision numeric values, or supply defaults */
      if(lmt.min_sng){
        lmt.min_val=strtod(lmt.min_sng,&sng_cnv_rcd);
        if(*sng_cnv_rcd) nco_sng_cnv_err(lmt.min_sng,"strtod",sng_cnv_rcd);
      } /* !lmt.min_sng */
      if(lmt.max_sng){
        lmt.max_val=strtod(lmt.max_sng,&sng_cnv_rcd);
        if(*sng_cnv_rcd) nco_sng_cnv_err(lmt.max_sng,"strtod",sng_cnv_rcd);
      } /* !lmt.max_sng */

      /* Re-base coordinates as necessary in multi-file operatators (MFOs)
      lmt.origin was calculated earlier in routine */
      if(rec_dmn_and_mfo){ 
        if(lmt.min_sng) lmt.min_val-=lmt.origin;
        if(lmt.max_sng) lmt.max_val-=lmt.origin;   
      }  /* endif MFO */
    } /* end UDUnits conversion */

    /* Warn when min_val > max_val (i.e., wrapped coordinate) */
    if(dbg_lvl_get() > nco_dbg_std && lmt.min_val > lmt.max_val) (void)fprintf(stderr,"%s: INFO Interpreting hyperslab specifications as wrapped coordinates [%s <= %g] and [%s >= %g]\n",prg_nm_get(),lmt.nm,lmt.max_val,lmt.nm,lmt.min_val);

    /* Fail when... */
    if(
      /* Following condition added 20000508, changes behavior of single point 
      hyperslabs depending on whether hyperslab occurs in record dimension
      during multi-file operator operation.
      Altered behavior of single point hyperslabs so that single point
      hyperslabs in the record coordinate (i.e., -d time,1.0,1.0) may be
      treated differently than single point hyperslabs in other
      coordinates. Multifile operators will skip files if single point
      hyperslabs in record coordinate lay outside record coordinate
      range of file. For non-record coordinates (and for all operators
      besides ncra and ncrcat on record coordinates), single point
      hyperslabs will choose the closest value rather than skip the file
      (I believe). This should be verified. */
      /* User specified single point, coordinate is not wrapped, and both extrema fall outside valid crd range */
      (rec_dmn_and_mfo && (lmt.min_val == lmt.max_val) && ((lmt.min_val > dmn_max) || (lmt.max_val < dmn_min))) ||
      /* User did not specify single point, coordinate is not wrapped, and either extrema falls outside valid crd range */
      ((lmt.min_val < lmt.max_val) && ((lmt.min_val > dmn_max) || (lmt.max_val < dmn_min))) ||
      /* User did not specify single point, coordinate is wrapped, and both extrema fall outside valid crd range */
      ((lmt.min_val > lmt.max_val) && ((lmt.min_val > dmn_max) && (lmt.max_val < dmn_min))) ||
      False){
        /* Allow for possibility that current file is superfluous */
        if(rec_dmn_and_mfo){
          flg_no_data_ok=True;
          goto no_data_ok;
        }else{
          (void)fprintf(stdout,"%s: ERROR User-specified coordinate value range %g <= %s <= %g does not fall within valid coordinate range %g <= %s <= %g\n",prg_nm_get(),lmt.min_val,lmt.nm,lmt.max_val,dmn_min,lmt.nm,dmn_max);
          nco_exit(EXIT_FAILURE);
        } /* end else */
    } /* end if */

    /* Armed with target coordinate minima and maxima, we are ready to bracket user-specified range */

    /* If min_sng or max_sng were omitted, use extrema */
    if(lmt.min_sng == NULL) lmt.min_idx=min_idx;
    if(lmt.max_sng == NULL) lmt.max_idx=max_idx;

    /* Single slice requires finding the closest coordinate */
    if(lmt.min_val == lmt.max_val){
      double dst_new;
      double dst_old;

      lmt.min_idx=0L;
      dst_old=fabs(lmt.min_val-dmn_val_dp[0]);
      for(tmp_idx=1L;tmp_idx<dmn_sz;tmp_idx++){
        if((dst_new=fabs(lmt.min_val-dmn_val_dp[tmp_idx])) < dst_old){
          dst_old=dst_new;
          lmt.min_idx=tmp_idx;
        } /* end if */
      } /* end loop over tmp_idx */
      lmt.max_idx=lmt.min_idx;

    }else{ /* min_val != max_val */

      /* Bracket specified extrema:
      Should no coordinate values match the given criteria, flag the index with -1L
      We defined the valid syntax such that single half range with -1L is not an error
      This causes "-d lon,100.0,-100.0" to select [-180.0] when lon=[-180.0,-90.0,0.0,90.0] 
      because one of the specified half-ranges is valid (there are coordinates < -100.0).
      However, "-d lon,100.0,-200.0" should fail when lon=[-180.0,-90.0,0.0,90.0] because both 
      of the specified half-ranges are invalid (no coordinate is > 100.0 or < -200.0).
      -1L flags are replaced with correct indices (0L or dmn_sz-1L) following search loop block.
      Overwriting -1L flags with 0L or dmn_sz-1L later is more heuristic than setting them = 0L here,
      since 0L is valid search result. */
      if(monotonic_direction == increasing){
        if(lmt.min_sng){
          /* Find index of smallest coordinate greater than min_val */
          tmp_idx=0L;
          while((dmn_val_dp[tmp_idx] < lmt.min_val) && (tmp_idx < dmn_sz)) tmp_idx++;
          if(tmp_idx != dmn_sz) lmt.min_idx=tmp_idx; else lmt.min_idx=-1L;
        } /* end if */
        if(lmt.max_sng){
          /* Find index of largest coordinate less than max_val */
          tmp_idx=dmn_sz-1L;
          while((dmn_val_dp[tmp_idx] > lmt.max_val) && (tmp_idx > -1L)) tmp_idx--;
          if(tmp_idx != -1L) lmt.max_idx=tmp_idx; else lmt.max_idx=-1L;
        } /* end if */
        /* 20110221: csz fix hyperslab bug TODO nco1007 triggered by
        ncks -O -v lat -d lat,20.,20.001 ~/nco/data/in.nc ~/foo.nc
        This returned all values but should have returned none
        Algorithm was broken because, although valid min and max indices existed,
        they contained the empty set. 
        Now when this happens, set flg_no_data_err block */
        if( /* Points are not wrapped ... */
          (lmt.min_val < lmt.max_val) && 
          /* ... and valid indices were found for both bracketing points... */
          (lmt.min_idx != -1L && lmt.max_idx != -1L) &&
          /* ...and indices contain empty set, i.e., min_idx > max_idx for increasing data... */
          lmt.min_idx > lmt.max_idx) flg_no_data_err=True;
        /* end if monotonic_direction == increasing */
      }else{ /* monotonic_direction == decreasing */
        if(lmt.min_sng){
          /* Find index of smallest coordinate greater than min_val */
          tmp_idx=dmn_sz-1L;
          while((dmn_val_dp[tmp_idx] < lmt.min_val) && (tmp_idx > -1L)) tmp_idx--;
          if(tmp_idx != -1L) lmt.min_idx=tmp_idx; else lmt.min_idx=-1L;
        } /* end if */
        if(lmt.max_sng){
          /* Find index of largest coordinate less than max_val */
          tmp_idx=0L;
          while((dmn_val_dp[tmp_idx] > lmt.max_val) && (tmp_idx < dmn_sz)) tmp_idx++;
          if(tmp_idx != dmn_sz) lmt.max_idx=tmp_idx; else lmt.max_idx=-1L;
        } /* end if */
        if( /* Points are not wrapped ... */
          (lmt.min_val > lmt.max_val) && 
          /* ... and valid indices were found for both bracketing points... */
          (lmt.min_idx != -1L && lmt.max_idx != -1L) &&
          /* ...and indices contain empty set, i.e., min_idx < max_idx for decreasing data... */
          lmt.min_idx < lmt.max_idx) flg_no_data_err=True;
      } /* end else monotonic_direction == decreasing */

      /* Case where both min_idx and max_idx = -1 was flagged as error above
      Case of wrapped coordinate: Either, but not both, of min_idx or max_idx will be flagged with -1
      See explanation above */
      if(lmt.min_idx == -1L && (lmt.min_val > lmt.max_val)) lmt.min_idx=0L;
      if(lmt.max_idx == -1L && (lmt.min_val > lmt.max_val)) lmt.max_idx=dmn_sz-1L;

    } /* end if min_val != max_val */

    /* User-specified ranges are now bracketed */

    /* Convert indices of minima and maxima to srt and end indices */
    if(monotonic_direction == increasing){
      lmt.srt=lmt.min_idx;
      lmt.end=lmt.max_idx;
    }else{
      lmt.srt=lmt.max_idx;
      lmt.end=lmt.min_idx;
    }  /* end else */

    /* Free space allocated for dimension */
    dmn_val_dp=(double*)nco_free(dmn_val_dp);

    if(rec_dmn_and_mfo){ 
      /* No wrapping with multi-file operators */ 
      if((monotonic_direction == increasing && lmt.min_val > lmt.max_val) ||
        (monotonic_direction == decreasing && lmt.min_val < lmt.max_val)){
          flg_no_data_ok=True; 
          goto no_data_ok;   
      } /* endif */

      if(rec_usd_cml == 0L){
        /* Skipped records remains zero until valid records are processed */
        lmt.rec_skp_vld_prv=0L;  
      }else if(rec_usd_cml > 0L){
        /* Otherwise, adjust starting index by records skipped in jumps across file boundaries */
        lmt.srt+=lmt.srd-1L-lmt.rec_skp_vld_prv%lmt.srd; 
        if(lmt.srt>lmt.end){
          /* Do not allow record dimension wrapping in MFOs */
          flg_no_data_ok=True;
          goto no_data_ok;
        } /* endif  */
      } /* endif */

      /* If we are here then there are valid records in current file */

    } /* end if rec_dmn_and_mfo */

  }else{ /* end if limit arguments were coordinate values */
    /* Convert limit strings to zero-based indicial offsets */

    /* Specifying stride alone, but not min or max, is legal, e.g., -d time,,,2
    Thus is_usr_spc_lmt may be True, even though one or both of min_sng, max_sng is NULL
    Furthermore, both min_sng and max_sng are artifically created by nco_lmt_sct_mk()
    for record dimensions when the user does not explicitly specify limits.
    In this case, min_sng_and max_sng are non-NULL though no limits were specified
    In fact, min_sng and max_sng are set to the minimum and maximum string
    values of the first file processed.
    However, we can tell if these strings were artificially generated because 
    nco_lmt_sct_mk() sets the is_usr_spc_lmt flag to False in such cases.
    Subsequent files may have different numbers of records, but nco_lmt_sct_mk()
    is only called once.
    Thus we must update min_idx and max_idx here for each file
    This causes min_idx and max_idx to be out of sync with min_sng and max_sng, 
    which are only set in nco_lmt_sct_mk() for the first file.
    In hindsight, artificially generating min_sng and max_sng may be bad idea */
    /* Following logic is messy, but hard to simplify */
    if(lmt.min_sng == NULL || !lmt.is_usr_spc_lmt){
      /* No user-specified value available--generate minimal dimension index */
      if(FORTRAN_IDX_CNV) lmt.min_idx=1L; else lmt.min_idx=0L;
    }else{
      /* Use user-specified limit when available */
      lmt.min_idx=strtol(lmt.min_sng,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
      if(*sng_cnv_rcd) nco_sng_cnv_err(lmt.min_sng,"strtol",sng_cnv_rcd);
    } /* end if */
    if(lmt.max_sng == NULL || !lmt.is_usr_spc_lmt){
      /* No user-specified value available---generate maximal dimension index */
      if(FORTRAN_IDX_CNV) lmt.max_idx=dmn_sz; else lmt.max_idx=dmn_sz-1L;
    }else{
      /* Use user-specified limit when available */
      lmt.max_idx=strtol(lmt.max_sng,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
      if(*sng_cnv_rcd) nco_sng_cnv_err(lmt.max_sng,"strtol",sng_cnv_rcd);
    } /* end if */

    /* Adjust indices if FORTRAN style input was specified */
    if(FORTRAN_IDX_CNV){
      /* 20120726: Die when Fortran index is zero */
      if(lmt.min_idx == 0L || lmt.max_idx == 0L){
        (void)fprintf(stdout,"%s: ERROR User-specified Fortran (1-based) index for dimension %s = 0.\n",prg_nm_get(),lmt.nm);
        msg_sng=strdup("Fortran indices must be >= 1");
        NCO_SYNTAX_ERROR=True;
      } /* endif illegal Fortran index */
      /* 20120709: Adjust positive indices only */
      if(lmt.min_idx > 0L) lmt.min_idx--;
      if(lmt.max_idx > 0L) lmt.max_idx--;
    } /* end if */

    /* 20120709 Negative integer as min or max element of hyperslab specification indicates offset from end */
    if(lmt.min_idx < 0L) lmt.min_idx+=dmn_sz-1L;
    if(lmt.max_idx < 0L) lmt.max_idx+=dmn_sz-1L;

    /* Exit if requested indices are always invalid for all operators... */
    if(lmt.min_idx < 0L){
      msg_sng=strdup("Minimum index is too negative");
      NCO_SYNTAX_ERROR=True;
    }else if(lmt.max_idx < 0L){
      msg_sng=strdup("Maximum index is too negative");
      NCO_SYNTAX_ERROR=True;
    }else if(lmt.drn > lmt.srd){
      (void)fprintf(stdout,"%s: ERROR User-specified duration exceeds stride for %s: %li > %li\n",prg_nm_get(),lmt.nm,lmt.drn,lmt.srd);
      msg_sng=strdup("Duration exceeds stride");
      NCO_SYNTAX_ERROR=True;
    }else if(!rec_dmn_and_mfo && lmt.min_idx >= dmn_sz){
      msg_sng=strdup("Minimum index greater than size in non-MFO");
      NCO_SYNTAX_ERROR=True;
      (void)fprintf(stdout,"%s: ERROR User-specified dimension index range %li <= %s <= %li does not fall within valid dimension index range 0 <= %s <= %li\n",prg_nm_get(),lmt.min_idx,lmt.nm,lmt.max_idx,lmt.nm,dmn_sz-1L);
    }else if(lmt.max_idx >= dmn_sz && prg_id == ncks){
      /* 20130203 pvn Check for -d max > dimension size; check fortran case; check multi file operators */
      msg_sng=strdup("ERROR: Maximum index exceeds dimension size");
      NCO_SYNTAX_ERROR=True;
    } /* end if impossible indices */

    if(NCO_SYNTAX_ERROR){
      (void)fprintf(stdout,"%s: ERROR evaluating hyperslab specification for %s: %s\n%s: HINT Conform request to hyperslab documentation at http://nco.sf.net/nco.html#hyp\n",prg_nm_get(),lmt.nm,msg_sng,prg_nm_get());
      msg_sng=(char *)nco_free(msg_sng);
      nco_exit(EXIT_FAILURE);
    } /* !NCO_SYNTAX_ERROR */

    /* NB: Duration is officially supported only for ncra and ncrcat (record dimension only) */
    if(lmt.drn != 1L && !rec_dmn_and_mfo) (void)fprintf(stderr,"%s: WARNING Duration argument is only supported for the record dimension on ncra and ncrcat operations\n",prg_nm_get());

    /* Logic depends on whether this is record dimension in multi-file operator */
    if(!rec_dmn_and_mfo || !lmt.is_usr_spc_lmt){
      /* For non-record dimensions and for record dimensions where limit 
      was automatically generated (to include whole file), starting
      and ending indices are simply minimum and maximum indices already 
      in structure */
      lmt.srt=lmt.min_idx;
      lmt.end=lmt.max_idx;
    }else{
      /* Initialize rec_skp_vld_prv to 0L on first call to nco_lmt_evl() 
      This is necessary due to intrinsic hysterisis of rec_skp_vld_prv
      rec_skp_vld_prv is used only by multi-file operators
      rec_skp_vld_prv counts records skipped at end of previous valid file
      rec_usd_cml and rec_skp_ntl_spf are both zero only for first file */
      if(rec_usd_cml == 0L && lmt.rec_skp_ntl_spf == 0L) lmt.rec_skp_vld_prv=0L;

      /* For record dimensions with user-specified limit, allow possibility 
      that limit pertains to record dimension in a multi-file operator.
      Then user-specified maximum index may exceed number of records in any one file
      Thus lmt.srt does not necessarily equal lmt.min_idx and 
      lmt.end does not necessarily equal lmt.max_idx */
      /* NB: Stride is officially supported for ncks (all dimensions) and for ncra and ncrcat (record dimension only) */
      if(lmt.srd != 1L && prg_id != ncks && !lmt.is_rec_dmn) (void)fprintf(stderr,"%s: WARNING Stride argument for non-record dimension is only supported by ncks, use at your own risk...\n",prg_nm_get());

      { /* Block hides scope of local internal variables */
        long srt_min_lcl; /* [idx] Minimum start index (in absolute index space, i.e., relative to first file) for current file */
        long end_max_lcl; /* [idx] Maximum end   index (in absolute index space, i.e., relative to first file) for current file */

        srt_min_lcl=(lmt.is_usr_spc_min ? lmt.min_idx : lmt.rec_in_cml+0L); 
        end_max_lcl=(lmt.is_usr_spc_max ? lmt.max_idx : lmt.rec_in_cml+dmn_sz-1L); 

        /* Maximum allowed index in record dimension */
        lmt.idx_end_max_abs=end_max_lcl;

        /* Are we past file containing end_max_lcl yet? */
        if(end_max_lcl < lmt.rec_in_cml){
          /* This and all subsequent files are superfluous because all requested records have already been read 
          Optimize MFOs by checking "input complete" flag to jump out of file loop
          Saves time because no other input files will be opened */
          lmt.flg_input_complete=True;
          flg_no_data_ok=True;
          goto no_data_ok;
        } /* endif past end_max_lcl */

        /* Have we reached file containing srt_min_lcl yet? */
        if(srt_min_lcl > lmt.rec_in_cml+dmn_sz-1L){
          /* This and all previous files are superfluous because the starting record is in a subsequent file */
          flg_no_data_ok=True;
          goto no_data_ok;
        } /* endif srt_min_lcl in future file */

        /* Until records have been used, start index is srt_min_lcl adjusted for records contained in all previous files
        Thereafter start index loses memory of/dependence on absolute start index, and only cares for how many records,
        if any, were skipped since last valid record. This number, modulo stride, is new start index. */
        if(rec_usd_cml == 0L) lmt.srt=srt_min_lcl-lmt.rec_in_cml; else lmt.srt=lmt.srd-1L-lmt.rec_skp_vld_prv%lmt.srd;

        if(lmt.srt > dmn_sz-1L){
          /* Perhaps data were read in previous file(s) yet next record is in future file due to long stride */
          flg_no_data_ok=True;
          goto no_data_ok;
        } /* endif */

        lmt.end=(end_max_lcl < lmt.rec_in_cml+dmn_sz) ? end_max_lcl-lmt.rec_in_cml : dmn_sz-1L;

      } /* end block hides scope of local internal variables */

      /* If we are here then there are valid records in current file */

    } /* endif user-specified limits to record dimension */

  } /* end else limit arguments are hyperslab indices */

  /* NB: MFO record dimension never reaches this block if current file is superfluous
  In that case code has already branched down to flg_data_ok or flg_data_err */
  if(rec_dmn_and_mfo){ 
    /* NB: This is---and must be---performed as integer arithmetic */ 
    cnt_rmn_crr=1L+(lmt.end-lmt.srt)/lmt.srd;  
    /* This fixes "sloppy" specification of end index by user, i.e., ensures that end index coincides with a stride */
    lmt.end=lmt.srt+(cnt_rmn_crr-1L)*lmt.srd;   
    /* Save current rec_skp_vld_prv for diagnostics (printed below) for this file */
    rec_skp_vld_prv_dgn=lmt.rec_skp_vld_prv;
    /* Next file must know how many records in this file come after (and thus will be skipped) last used record in this file */
    lmt.rec_skp_vld_prv=dmn_sz-1L-lmt.end;
  } /* !rec_dmn_and_mfo */      

  /* Compute cnt from srt, end, and srd
  This is fine for multi-file record dimensions since those operators read-in one
  record at a time and thus never actually use lmt.cnt for record dimension. */
  if(lmt.srd == 1L){
    if(lmt.srt <= lmt.end) lmt.cnt=lmt.end-lmt.srt+1L; else lmt.cnt=dmn_sz-lmt.srt+lmt.end+1L;
  }else{
    if(lmt.srt <= lmt.end) lmt.cnt=1L+(lmt.end-lmt.srt)/lmt.srd; else lmt.cnt=1L+((dmn_sz-lmt.srt)+lmt.end)/lmt.srd;
  } /* end else */

  /* NB: Degenerate cases of WRP && SRD exist for which dmn_cnt_2 == 0
  This occurs when srd is large enough, or max_idx small enough, 
  such that no values are selected in the second read. 
  e.g., "-d lon,60,0,10" if sz(lon)=128 has dmn_cnt_2 == 0
  Since netCDF library reports an error reading and writing cnt=0 dimensions, kludge is necessary
  Syntax ensures that it is always the second read, not the first, which is obviated
  Therefore we convert these degenerate cases into non-wrapped coordinates to be processed by single read 
  For these degenerate cases only, [srt,end] are not a permutation of [min_idx,max_idx] */
  if(
    (lmt.srd != 1L) && /* SRD */
    (lmt.srt > lmt.end) && /* WRP */
    (lmt.cnt == (1L+(dmn_sz-lmt.srt-1L)/lmt.srd)) && /* dmn_cnt_1 == cnt -> dmn_cnt_2 == 0 */
    True){
      long greatest_srd_multiplier_1st_hyp_slb; /* Greatest integer m such that srt+m*srd < dmn_sz */
      long last_good_idx_1st_hyp_slb; /* C-index of last valid member of 1st hyperslab (= srt+m*srd) */
      /* long left_over_idx_1st_hyp_slb;*/ /* # of elements from first hyperslab that count towards current stride */
      long first_good_idx_2nd_hyp_slb; /* C-index of first valid member of 2nd hyperslab, if any */

      /* NB: Perform these operations with integer arithmetic or else! */
      /* Wrapped dimensions with stride may not start at idx 0 on second read */
      greatest_srd_multiplier_1st_hyp_slb=(dmn_sz-lmt.srt-1L)/lmt.srd;
      last_good_idx_1st_hyp_slb=lmt.srt+lmt.srd*greatest_srd_multiplier_1st_hyp_slb;
      /* left_over_idx_1st_hyp_slb=dmn_sz-last_good_idx_1st_hyp_slb-1L;*/
      first_good_idx_2nd_hyp_slb=(last_good_idx_1st_hyp_slb+lmt.srd)%dmn_sz;

      /* Conditions causing dmn_cnt_2 == 0 */
      if(first_good_idx_2nd_hyp_slb > lmt.end) lmt.end=last_good_idx_1st_hyp_slb;
  } /* end if */

  /* Cases where domain brackets no data, in error, have counts set to zero here
  This kludge allows codepaths for both WRP and out-of-domain to flow without goto statements
  Out-of-domain errors will soon exit with error, while WRP conditions will proceed */
  if(flg_no_data_err) lmt.cnt=0L;

  /* Exit when valid bracketed range contains no coordinates and that is not legal,
  i.e., this is not a superfluous file in an MFO */
  if(lmt.cnt == 0){
    if(lmt.lmt_typ == lmt_crd_val) (void)fprintf(stdout,"%s: ERROR Domain %g <= %s <= %g brackets no coordinate values.\n",prg_nm_get(),lmt.min_val,lmt.nm,lmt.max_val); 
    if(lmt.lmt_typ == lmt_dmn_idx) (void)fprintf(stdout,"%s: ERROR Empty domain for %s\n",prg_nm_get(),lmt.nm); 
    nco_exit(EXIT_FAILURE);
  } /* end if */

  /* Coordinate-valued limits that bracket no values in current file jump here with goto
  Index-valued limits with no values in current file flow here naturally */
no_data_ok: /* end goto */
  if(flg_no_data_ok){
    /* File is superfluous (contributes no data) to specified hyperslab
    Set output parameters to well-defined state
    This state must not cause ncra or ncrcat to retrieve any data
    ncra and ncrcat use loops for the record dimension, so this is
    accomplished by setting loop control values (lmt_rec.srt > lmt_rec.end)
    that cause record loop always to be skipped (never entered) */
    lmt.srt=-1L;
    lmt.end=lmt.srt-1L;
    lmt.cnt=-1L;
    /* Augment number of records skipped in initial superfluous files */
    if(rec_usd_cml == 0L) lmt.rec_skp_ntl_spf+=dmn_sz; 
    /* Augment records skipped since last good one */ 
    lmt.rec_skp_vld_prv+=dmn_sz;
    /* Set variables to preserve utility of diagnostics at end of routine */
    cnt_rmn_crr=rec_skp_vld_prv_dgn=0L;
  } /* endif */

  /* Accumulate count of records in all opened files, including this one
  Increment here at end so this structure member includes records from current file 
  only at end of this routine, where it can only be used diagnostically
  NB: Location of this augmentation is important! Moving it would have side-effects! */
  lmt.rec_in_cml+=dmn_sz;

  /* Place contents of working structure in location of returned structure */
  *lmt_ptr=lmt;

  if(dbg_lvl_get() == nco_dbg_io){
    (void)nco_prt_lmt(lmt,min_lmt_typ,FORTRAN_IDX_CNV,flg_no_data_ok,rec_usd_cml,monotonic_direction,rec_dmn_and_mfo,cnt_rmn_ttl,cnt_rmn_crr,rec_skp_vld_prv_dgn);
  } /* end dbg */

  if(lmt.srt > lmt.end && !flg_no_data_ok){
    if(prg_id != ncks) (void)fprintf(stderr,"WARNING: Possible instance of Schweitzer data hole requiring better diagnostics TODO #148\n");
    if(prg_id != ncks) (void)fprintf(stderr,"HINT: If operation fails, try multislabbing (http://nco.sf.net/nco.html#msa) wrapped dimension using ncks first, and then apply %s to the resulting file\n",prg_nm_get());
  } /* end dbg */

  fl_udu_sng=(char *)nco_free(fl_udu_sng);

} /* end nco_lmt_evl_dmn_trv() */




void                      
nco_lmt_evl_dmn_crd            /* [fnc] Parse user-specified limits into hyperslab specifications */
(const int nc_id,              /* I [ID] netCDF file ID */
 long rec_usd_cml,             /* I [nbr] Number of valid records already processed (only used for record dimensions in multi-file operators) */
 nco_bool FORTRAN_IDX_CNV,     /* I [flg] Hyperslab indices obey Fortran convention */
 const char * const grp_nm_fll,/* I [sng] Full group name (dimension or coordinate) */
 const char * const nm,        /* I [sng] Name (dimension or coordinate) */
 const size_t sz,              /* I [nbr] Size (dimension or coordinate) */
 const nco_bool is_rec,        /* I [flg] Is a record (dimension or coordinate) ? */
 const nco_bool is_crd,        /* I [flg] Is a coordinate variable ? */
 lmt_sct *lmt_ptr)             /* I/O [sct] Structure from nco_lmt_prs() in input, filled on output  */
{

  /* Purpose: Take parsed list of dimension names, minima, and maxima strings and find appropriate indices into dimensions 
  for formulation of dimension start and count vectors, or fail trying. 

  Based on original nco_lmt_evl(). Used for both dimensions and coordinate variables

  Use case example:
  /lon(4)
  /g8/lon(2)
  ncks -d lon,0,3,1 -v lon -H ~/nco/data/in_grp.nc
  "-d lon,0,3,1" is valid for /lon(4) but not for /g8/lon(2)

  Reminder:
  Coordinate values should be specified using real notation with a decimal point required in the value, 
  whereas dimension indices are specified using integer notation without a decimal point.

  ncks -d lat,-90.,90.,1 -H -v area ~/nco/data/in_grp.nc  # limit type is defined as lmt_crd_val
  ncks -d lat,0,1,1 -H -v area ~/nco/data/in_grp.nc  # limit type is defined as lmt_dmn_idx

  lmt_crd_val,  0, Coordinate value limit 
  lmt_dmn_idx,  1, Dimension index limit 
  lmt_udu_sng   2, UDUnits string 

  Tests:
  ncks -D 11 -d lon,0.,90.,1 -v lon -H ~/nco/data/in_grp.nc
  ncks -D 11 -d lon,0,1,1 -v lon -H ~/nco/data/in_grp.nc
  */

  const char fnc_nm[]="nco_lmt_evl_dmn_crd()"; /* [sng] Function name */

  char *fl_udu_sng=NULL_CEWI;     /* [sng] Store units attribute of coordinate dimension */
  char *msg_sng=NULL_CEWI;        /* [sng] Error message */
  char *sng_cnv_rcd=NULL_CEWI;    /* [sng] strtol()/strtoul() return code */

  nco_bool flg_no_data_err=False; /* [flg] True if domain brackets no data (and not an MFO/record coordinate) */
  nco_bool flg_no_data_ok=False;  /* [flg] True if file contains no data for hyperslab */
  nco_bool rec_dmn_and_mfo=False; /* [flg] True if record dimension in multi-file operator */
  nco_bool NCO_SYNTAX_ERROR=False;/* [flg] Syntax error in hyperslab specification */

  dmn_sct dim;                     /* [sct] Dimension Structure  */

  lmt_sct lmt;                     /* [sct] Structure from nco_lmt_prs()  */

  int min_lmt_typ=int_CEWI;
  int max_lmt_typ=int_CEWI; 
  int prg_id;                      /* [enm] Program ID */
  int rcd=NC_NOERR;                /* [rcd] Return code */

  monotonic_direction_enm monotonic_direction=not_checked; /* CEWI */

  long dmn_sz;                     /* [nbr] Dimension size */
  long cnt_rmn_crr=-1L;            /* [nbr] Records to extract from current file */
  long cnt_rmn_ttl=-1L;            /* [nbr] Total records to be read from this and all remaining files */
  long rec_skp_vld_prv_dgn=-1L;    /* [nbr] Records skipped at end of previous valid file, if any (diagnostic only) */

  int var_id=-1;                   /* [id] ID of variable */
  int grp_id=-1;                   /* [id] ID of group */

  nc_type var_typ=NC_NAT;          /* [enm] Type of variable */

  nco_bool is_crd_var;             /* [flg] Does the dimension have a coordinate variable ? */  

  lmt=*lmt_ptr;

  prg_id=prg_get(); 

  /* Initialize limit structure */
  lmt.flg_mro=False;
  lmt.max_val=0.0;
  lmt.min_val=0.0;
  lmt.drn=1L;
  lmt.srd=1L;
  lmt.flg_input_complete=False;

  /* Obtain group ID using full group name */
  (void)nco_inq_grp_full_ncid(nc_id,grp_nm_fll,&grp_id);

  /* Obtain possible coordinate variable ID using group ID */
  rcd=nco_inq_varid_flg(grp_id,nm,&var_id);
  if(rcd != NC_NOERR){
    is_crd_var=False; 

    /* Just make sure we get the right thing */
    assert(is_crd == False);

  }else{ 
    is_crd_var=True;

    assert(is_crd == True);

    /* Get coordinate type */
    (void)nco_inq_vartype(grp_id,var_id,&var_typ);
  }

  if(dbg_lvl_get() == nco_dbg_old){
    (void)fprintf(stdout,"%s: INFO %s dimension <%s/%s(%li)>:",prg_nm_get(),fnc_nm,grp_nm_fll,nm,sz);
    if (is_crd_var == False )
      (void)fprintf(stdout,"NOT a coordinate variable\n");
    else 
      (void)fprintf(stdout,"HAS a coordinate variable\n");    
  }

  /* Use info from parameter to assign locally used size */
  dmn_sz=sz;

  /* Use info from parameter to assign record/not record to limit */
  lmt.is_rec_dmn=is_rec;

  /* Logic on whether to allow skipping current file depends on whether limit
  is specified for record dimension in multi-file operators.
  This information is not used in single-file operators, though whether
  the limit is a record limit may be tested.
  Program defensively and define this flag in all cases. */

  if(lmt.is_rec_dmn && (prg_id == ncra || prg_id == ncrcat)) rec_dmn_and_mfo=True; else rec_dmn_and_mfo=False;

  if(rec_dmn_and_mfo){
    lmt.rec_dmn_sz=dmn_sz;
    lmt.idx_end_max_abs=lmt.rec_in_cml+dmn_sz-1L; /* Maximum allowed index in record dimension */
  } /* !rec_dmn_and_mfo */

  /* Bomb if dmn_sz < 1 */
  if(dmn_sz < 1L){
    (void)fprintf(stdout,"%s: ERROR Size of dimension %s is %li in input file, but must be > 0 in order to apply limits.\n",prg_nm_get(),lmt.nm,dmn_sz);
    nco_exit(EXIT_FAILURE);
  } /* end if */

  if(lmt.srd_sng){
    if(strchr(lmt.srd_sng,'.') || strchr(lmt.srd_sng,'e') || strchr(lmt.srd_sng,'E') || strchr(lmt.srd_sng,'d') || strchr(lmt.srd_sng,'D')){
      (void)fprintf(stdout,"%s: ERROR Requested stride for %s, %s, must be integer\n",prg_nm_get(),lmt.nm,lmt.srd_sng);
      nco_exit(EXIT_FAILURE);
    } /* end if */
    lmt.srd=strtol(lmt.srd_sng,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
    if(*sng_cnv_rcd) nco_sng_cnv_err(lmt.srd_sng,"strtol",sng_cnv_rcd);
    if(lmt.srd < 1L){
      (void)fprintf(stdout,"%s: ERROR Stride for %s is %li but must be > 0\n",prg_nm_get(),lmt.nm,lmt.srd);
      nco_exit(EXIT_FAILURE);
    } /* end if */
  } /* !lmt.srd_sng */

  if(lmt.drn_sng){
    if(strchr(lmt.drn_sng,'.') || strchr(lmt.drn_sng,'e') || strchr(lmt.drn_sng,'E') || strchr(lmt.drn_sng,'d') || strchr(lmt.drn_sng,'D')){
      (void)fprintf(stdout,"%s: ERROR Requested duration for %s, %s, must be integer\n",prg_nm_get(),lmt.nm,lmt.drn_sng);
      nco_exit(EXIT_FAILURE);
    } /* end if */
    lmt.drn=strtol(lmt.drn_sng,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
    if(*sng_cnv_rcd) nco_sng_cnv_err(lmt.drn_sng,"strtol",sng_cnv_rcd);
    if(lmt.drn < 1L){
      (void)fprintf(stdout,"%s: ERROR Duration for %s is %li but must be > 0\n",prg_nm_get(),lmt.nm,lmt.drn);
      nco_exit(EXIT_FAILURE);
    } /* end if */
    if(prg_id != ncra && prg_id != ncrcat){
      (void)fprintf(stdout,"%s: ERROR Duration only implemented for ncra and ncrcat\n",prg_nm_get());
      nco_exit(EXIT_FAILURE);
    } /* end ncra */
  } /* !lmt.drn_sng */

  if(lmt.mro_sng){
    if(strcasecmp(lmt.mro_sng,"m")){
      (void)fprintf(stdout,"%s: ERROR Requested MRO flag for %s, \"%s\", must be 'm' or 'M'\n",prg_nm_get(),lmt.nm,lmt.mro_sng);
      nco_exit(EXIT_FAILURE);
    } /* end if */
    lmt.flg_mro=True;
  } /* !lmt.mro_sng */

  /* In case flg_mro is set in ncra2.c by --mro */
  if(lmt.flg_mro){
    if(prg_id == ncrcat){
      (void)fprintf(stdout,"%s: INFO Specifying Multi-Record Output (MRO) option ('m', 'M', or --mro) is redundant. MRO is always true for ncrcat.\n",prg_nm_get());
    }else if(prg_id != ncra){
      (void)fprintf(stdout,"%s: ERROR Multi-Record Output (MRO) ('m', 'M', or --mro) is only valid for ncra.\n",prg_nm_get());
      nco_exit(EXIT_FAILURE);
    } /* end else */
  } /* !lmt.mro_sng */

  /* If min_sng and max_sng are both NULL then set type to lmt_dmn_idx */
  if(lmt.min_sng == NULL && lmt.max_sng == NULL){
    /* Limiting indices will be set to default extrema a bit later */
    min_lmt_typ=max_lmt_typ=lmt_dmn_idx;
  }else{
    /* min_sng and max_sng are not both NULL */
    /* Limit is coordinate value if string contains decimal point or is in exponential format 
    Otherwise limit is interpreted as zero-based dimension offset */
    if(lmt.min_sng) min_lmt_typ=nco_lmt_typ(lmt.min_sng);
    if(lmt.max_sng) max_lmt_typ=nco_lmt_typ(lmt.max_sng);

    /* Copy lmt_typ from defined limit to undefined */
    if(lmt.min_sng == NULL) min_lmt_typ=max_lmt_typ;
    if(lmt.max_sng == NULL) max_lmt_typ=min_lmt_typ;
  } /* end else */

  /* Both min_lmt_typ and max_lmt_typ are now defined
  Continue only if both limits are of the same type */
  if(min_lmt_typ != max_lmt_typ){
    (void)fprintf(stdout,"%s: ERROR -d %s,%s,%s\n",prg_nm_get(),lmt.nm,lmt.min_sng,lmt.max_sng);
    (void)fprintf(stdout,"Limits on dimension \"%s\" must be of same numeric type:\n",lmt.nm);
    (void)fprintf(stdout,"\"%s\" was interpreted as a %s.\n",lmt.min_sng,(min_lmt_typ == lmt_crd_val) ? "coordinate value" : (FORTRAN_IDX_CNV) ? "one-based dimension index" : "zero-based dimension index");
    (void)fprintf(stdout,"\"%s\" was interpreted as a %s.\n",lmt.max_sng,(max_lmt_typ == lmt_crd_val) ? "coordinate value" : (FORTRAN_IDX_CNV) ? "one-based dimension index" : "zero-based dimension index");
    (void)fprintf(stdout,"(Limit arguments containing a decimal point (or in exponential format) are interpreted as coordinate values; arguments without a decimal point are interpreted as zero-based or one-based (depending on -F switch) dimensional indices.)\n");
    nco_exit(EXIT_FAILURE);
  } /* end if */
  lmt.lmt_typ=min_lmt_typ;

  /* Coordinate re-basing code */
  lmt.origin=0.0;

  /* Get variable ID of coordinate. NOTE: using group ID */
  rcd=nco_inq_varid_flg(grp_id,lmt.nm,&dim.cid);

  /* If there is a coordinate variable */
  if(rcd == NC_NOERR){

    char *cln_sng=NULL_CEWI;

    fl_udu_sng=nco_lmt_get_udu_att(grp_id,var_id,"units"); /* Units attribute of coordinate variable */
    cln_sng=nco_lmt_get_udu_att(grp_id,var_id,"calendar"); /* Calendar attribute */

    if(rec_dmn_and_mfo && fl_udu_sng && lmt.rbs_sng){ 
#ifdef ENABLE_UDUNITS
      /* Re-base and reset origin to 0.0 if re-basing fails */
      if(nco_cln_clc_org(fl_udu_sng,lmt.rbs_sng,lmt.lmt_cln,&lmt.origin) != EXIT_SUCCESS) lmt.origin=0.0;
#endif /* !ENABLE_UDUNITS */
    } /* endif */

    /* ncra and ncrcat read the "calendar" attribute in main() 
    Avoid multiple reads of calendar attritbute in multi-file operations */
    if(!rec_dmn_and_mfo){
      if(cln_sng) lmt.lmt_cln=nco_cln_get_cln_typ(cln_sng); else lmt.lmt_cln=cln_nil;
    } /* endif */
    if(cln_sng) cln_sng=(char *)nco_free(cln_sng);
  } /* End Needed only to read variable, if dimension is a coordinate variable */


  if((lmt.lmt_typ == lmt_crd_val) || (lmt.lmt_typ == lmt_udu_sng)){
    double *dmn_val_dp=NULL;

    double dmn_max;
    double dmn_min;

    long max_idx;
    long min_idx;
    long tmp_idx;
    long dmn_srt=0L;    

    /* Warn when coordinate type is weird */
    if(var_typ == NC_BYTE || var_typ == NC_UBYTE || var_typ == NC_CHAR || var_typ == NC_STRING) (void)fprintf(stderr,"\n%s: WARNING Coordinate %s is type %s. Dimension truncation is unpredictable.\n",prg_nm_get(),lmt.nm,nco_typ_sng(var_typ));

    /* Allocate enough space to hold coordinate */
    dmn_val_dp=(double *)nco_malloc(dmn_sz*nco_typ_lng(NC_DOUBLE));

#ifdef _OPENMP
#pragma omp critical
#endif /* _OPENMP */
    { /* begin OpenMP critical */
      /* Block is critical for identical in_id's
      Block is thread-safe for distinct in_id's */
      /* 20110221: replace nco_get_vara() with nc_get_vara_double() */
      /* Retrieve this coordinate */
      rcd=nc_get_vara_double(grp_id,var_id,(const size_t *)&dmn_srt,(const size_t *)&dmn_sz,dmn_val_dp);

      /* Exit if read error */
      if(rcd != NC_NOERR) nco_err_exit(rcd,"nc_get_vara_double()");

    } /* end OpenMP critical */

    /* Officially change type */
    var_typ=NC_DOUBLE;

    /* Assuming coordinate is monotonic, direction of monotonicity is determined by first two elements */
    if(dmn_sz == 1L){
      monotonic_direction=increasing;
    }else{
      if(dmn_val_dp[0] > dmn_val_dp[1]) monotonic_direction=decreasing; else monotonic_direction=increasing;
    } /* end else */

    if(monotonic_direction == increasing){
      min_idx=0L;
      max_idx=dmn_sz-1L;
    }else{
      min_idx=dmn_sz-1L;
      max_idx=0L;
    } /* end else */

    /* Determine min and max values of entire coordinate */
    dmn_min=dmn_val_dp[min_idx];
    dmn_max=dmn_val_dp[max_idx];

    /* Set defaults */  
    lmt.min_val=dmn_val_dp[min_idx]; 
    lmt.max_val=dmn_val_dp[max_idx];

    /* Convert UDUnits strings if necessary */
    /* If we are here then either min_sng or max_sng or both are set */
    if(lmt.lmt_typ == lmt_udu_sng){

      if(!fl_udu_sng){ 
        (void)fprintf(stdout,"%s: ERROR attempting to read units attribute from variable \"%s\" \n",prg_nm_get(),lmt.nm);
        nco_exit(EXIT_FAILURE);
      } /* end if */

      if(lmt.min_sng && nco_cln_clc_org(lmt.min_sng,fl_udu_sng,lmt.lmt_cln,&lmt.min_val)) nco_exit(EXIT_FAILURE);
      if(lmt.max_sng && nco_cln_clc_org(lmt.max_sng,fl_udu_sng,lmt.lmt_cln,&lmt.max_val)) nco_exit(EXIT_FAILURE);

    }else{ /* end UDUnits conversion */
      /* Convert user-specified limits into double precision numeric values, or supply defaults */
      if(lmt.min_sng){
        lmt.min_val=strtod(lmt.min_sng,&sng_cnv_rcd);
        if(*sng_cnv_rcd) nco_sng_cnv_err(lmt.min_sng,"strtod",sng_cnv_rcd);
      } /* !lmt.min_sng */
      if(lmt.max_sng){
        lmt.max_val=strtod(lmt.max_sng,&sng_cnv_rcd);
        if(*sng_cnv_rcd) nco_sng_cnv_err(lmt.max_sng,"strtod",sng_cnv_rcd);
      } /* !lmt.max_sng */

      /* Re-base coordinates as necessary in multi-file operatators (MFOs)
      lmt.origin was calculated earlier in routine */
      if(rec_dmn_and_mfo){ 
        if(lmt.min_sng) lmt.min_val-=lmt.origin;
        if(lmt.max_sng) lmt.max_val-=lmt.origin;   
      }  /* endif MFO */
    } /* end UDUnits conversion */

    /* Warn when min_val > max_val (i.e., wrapped coordinate) */
    if(dbg_lvl_get() > nco_dbg_std && lmt.min_val > lmt.max_val) (void)fprintf(stderr,"%s: INFO Interpreting hyperslab specifications as wrapped coordinates [%s <= %g] and [%s >= %g]\n",prg_nm_get(),lmt.nm,lmt.max_val,lmt.nm,lmt.min_val);

    /* Fail when... */
    if(
      /* Following condition added 20000508, changes behavior of single point 
      hyperslabs depending on whether hyperslab occurs in record dimension
      during multi-file operator operation.
      Altered behavior of single point hyperslabs so that single point
      hyperslabs in the record coordinate (i.e., -d time,1.0,1.0) may be
      treated differently than single point hyperslabs in other
      coordinates. Multifile operators will skip files if single point
      hyperslabs in record coordinate lay outside record coordinate
      range of file. For non-record coordinates (and for all operators
      besides ncra and ncrcat on record coordinates), single point
      hyperslabs will choose the closest value rather than skip the file
      (I believe). This should be verified. */
      /* User specified single point, coordinate is not wrapped, and both extrema fall outside valid crd range */
      (rec_dmn_and_mfo && (lmt.min_val == lmt.max_val) && ((lmt.min_val > dmn_max) || (lmt.max_val < dmn_min))) ||
      /* User did not specify single point, coordinate is not wrapped, and either extrema falls outside valid crd range */
      ((lmt.min_val < lmt.max_val) && ((lmt.min_val > dmn_max) || (lmt.max_val < dmn_min))) ||
      /* User did not specify single point, coordinate is wrapped, and both extrema fall outside valid crd range */
      ((lmt.min_val > lmt.max_val) && ((lmt.min_val > dmn_max) && (lmt.max_val < dmn_min))) ||
      False){
        /* Allow for possibility that current file is superfluous */
        if(rec_dmn_and_mfo){
          flg_no_data_ok=True;
          goto no_data_ok;
        }else{
          (void)fprintf(stdout,"%s: ERROR User-specified coordinate value range %g <= %s <= %g does not fall within valid coordinate range %g <= %s <= %g\n",prg_nm_get(),lmt.min_val,lmt.nm,lmt.max_val,dmn_min,lmt.nm,dmn_max);
          nco_exit(EXIT_FAILURE);
        } /* end else */
    } /* end if */

    /* Armed with target coordinate minima and maxima, we are ready to bracket user-specified range */

    /* If min_sng or max_sng were omitted, use extrema */
    if(lmt.min_sng == NULL) lmt.min_idx=min_idx;
    if(lmt.max_sng == NULL) lmt.max_idx=max_idx;

    /* Single slice requires finding the closest coordinate */
    if(lmt.min_val == lmt.max_val){
      double dst_new;
      double dst_old;

      lmt.min_idx=0L;
      dst_old=fabs(lmt.min_val-dmn_val_dp[0]);
      for(tmp_idx=1L;tmp_idx<dmn_sz;tmp_idx++){
        if((dst_new=fabs(lmt.min_val-dmn_val_dp[tmp_idx])) < dst_old){
          dst_old=dst_new;
          lmt.min_idx=tmp_idx;
        } /* end if */
      } /* end loop over tmp_idx */
      lmt.max_idx=lmt.min_idx;

    }else{ /* min_val != max_val */

      /* Bracket specified extrema:
      Should no coordinate values match the given criteria, flag the index with -1L
      We defined the valid syntax such that single half range with -1L is not an error
      This causes "-d lon,100.0,-100.0" to select [-180.0] when lon=[-180.0,-90.0,0.0,90.0] 
      because one of the specified half-ranges is valid (there are coordinates < -100.0).
      However, "-d lon,100.0,-200.0" should fail when lon=[-180.0,-90.0,0.0,90.0] because both 
      of the specified half-ranges are invalid (no coordinate is > 100.0 or < -200.0).
      -1L flags are replaced with correct indices (0L or dmn_sz-1L) following search loop block.
      Overwriting -1L flags with 0L or dmn_sz-1L later is more heuristic than setting them = 0L here,
      since 0L is valid search result. */
      if(monotonic_direction == increasing){
        if(lmt.min_sng){
          /* Find index of smallest coordinate greater than min_val */
          tmp_idx=0L;
          while((dmn_val_dp[tmp_idx] < lmt.min_val) && (tmp_idx < dmn_sz)) tmp_idx++;
          if(tmp_idx != dmn_sz) lmt.min_idx=tmp_idx; else lmt.min_idx=-1L;
        } /* end if */
        if(lmt.max_sng){
          /* Find index of largest coordinate less than max_val */
          tmp_idx=dmn_sz-1L;
          while((dmn_val_dp[tmp_idx] > lmt.max_val) && (tmp_idx > -1L)) tmp_idx--;
          if(tmp_idx != -1L) lmt.max_idx=tmp_idx; else lmt.max_idx=-1L;
        } /* end if */
        /* 20110221: csz fix hyperslab bug TODO nco1007 triggered by
        ncks -O -v lat -d lat,20.,20.001 ~/nco/data/in.nc ~/foo.nc
        This returned all values but should have returned none
        Algorithm was broken because, although valid min and max indices existed,
        they contained the empty set. 
        Now when this happens, set flg_no_data_err block */
        if( /* Points are not wrapped ... */
          (lmt.min_val < lmt.max_val) && 
          /* ... and valid indices were found for both bracketing points... */
          (lmt.min_idx != -1L && lmt.max_idx != -1L) &&
          /* ...and indices contain empty set, i.e., min_idx > max_idx for increasing data... */
          lmt.min_idx > lmt.max_idx) flg_no_data_err=True;
        /* end if monotonic_direction == increasing */
      }else{ /* monotonic_direction == decreasing */
        if(lmt.min_sng){
          /* Find index of smallest coordinate greater than min_val */
          tmp_idx=dmn_sz-1L;
          while((dmn_val_dp[tmp_idx] < lmt.min_val) && (tmp_idx > -1L)) tmp_idx--;
          if(tmp_idx != -1L) lmt.min_idx=tmp_idx; else lmt.min_idx=-1L;
        } /* end if */
        if(lmt.max_sng){
          /* Find index of largest coordinate less than max_val */
          tmp_idx=0L;
          while((dmn_val_dp[tmp_idx] > lmt.max_val) && (tmp_idx < dmn_sz)) tmp_idx++;
          if(tmp_idx != dmn_sz) lmt.max_idx=tmp_idx; else lmt.max_idx=-1L;
        } /* end if */
        if( /* Points are not wrapped ... */
          (lmt.min_val > lmt.max_val) && 
          /* ... and valid indices were found for both bracketing points... */
          (lmt.min_idx != -1L && lmt.max_idx != -1L) &&
          /* ...and indices contain empty set, i.e., min_idx < max_idx for decreasing data... */
          lmt.min_idx < lmt.max_idx) flg_no_data_err=True;
      } /* end else monotonic_direction == decreasing */

      /* Case where both min_idx and max_idx = -1 was flagged as error above
      Case of wrapped coordinate: Either, but not both, of min_idx or max_idx will be flagged with -1
      See explanation above */
      if(lmt.min_idx == -1L && (lmt.min_val > lmt.max_val)) lmt.min_idx=0L;
      if(lmt.max_idx == -1L && (lmt.min_val > lmt.max_val)) lmt.max_idx=dmn_sz-1L;

    } /* end if min_val != max_val */

    /* User-specified ranges are now bracketed */

    /* Convert indices of minima and maxima to srt and end indices */
    if(monotonic_direction == increasing){
      lmt.srt=lmt.min_idx;
      lmt.end=lmt.max_idx;
    }else{
      lmt.srt=lmt.max_idx;
      lmt.end=lmt.min_idx;
    }  /* end else */

    /* Free space allocated for dimension */
    dmn_val_dp=(double*)nco_free(dmn_val_dp);

    if(rec_dmn_and_mfo){ 
      /* No wrapping with multi-file operators */ 
      if((monotonic_direction == increasing && lmt.min_val > lmt.max_val) ||
        (monotonic_direction == decreasing && lmt.min_val < lmt.max_val)){
          flg_no_data_ok=True; 
          goto no_data_ok;   
      } /* endif */

      if(rec_usd_cml == 0L){
        /* Skipped records remains zero until valid records are processed */
        lmt.rec_skp_vld_prv=0L;  
      }else if(rec_usd_cml > 0L){
        /* Otherwise, adjust starting index by records skipped in jumps across file boundaries */
        lmt.srt+=lmt.srd-1L-lmt.rec_skp_vld_prv%lmt.srd; 
        if(lmt.srt>lmt.end){
          /* Do not allow record dimension wrapping in MFOs */
          flg_no_data_ok=True;
          goto no_data_ok;
        } /* endif  */
      } /* endif */

      /* If we are here then there are valid records in current file */

    } /* end if rec_dmn_and_mfo */

  }else{ /* end if limit arguments were coordinate values */
    /* Convert limit strings to zero-based indicial offsets */

    /* Specifying stride alone, but not min or max, is legal, e.g., -d time,,,2
    Thus is_usr_spc_lmt may be True, even though one or both of min_sng, max_sng is NULL
    Furthermore, both min_sng and max_sng are artifically created by nco_lmt_sct_mk()
    for record dimensions when the user does not explicitly specify limits.
    In this case, min_sng_and max_sng are non-NULL though no limits were specified
    In fact, min_sng and max_sng are set to the minimum and maximum string
    values of the first file processed.
    However, we can tell if these strings were artificially generated because 
    nco_lmt_sct_mk() sets the is_usr_spc_lmt flag to False in such cases.
    Subsequent files may have different numbers of records, but nco_lmt_sct_mk()
    is only called once.
    Thus we must update min_idx and max_idx here for each file
    This causes min_idx and max_idx to be out of sync with min_sng and max_sng, 
    which are only set in nco_lmt_sct_mk() for the first file.
    In hindsight, artificially generating min_sng and max_sng may be bad idea */
    /* Following logic is messy, but hard to simplify */
    if(lmt.min_sng == NULL || !lmt.is_usr_spc_lmt){
      /* No user-specified value available--generate minimal dimension index */
      if(FORTRAN_IDX_CNV) lmt.min_idx=1L; else lmt.min_idx=0L;
    }else{
      /* Use user-specified limit when available */
      lmt.min_idx=strtol(lmt.min_sng,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
      if(*sng_cnv_rcd) nco_sng_cnv_err(lmt.min_sng,"strtol",sng_cnv_rcd);
    } /* end if */
    if(lmt.max_sng == NULL || !lmt.is_usr_spc_lmt){
      /* No user-specified value available---generate maximal dimension index */
      if(FORTRAN_IDX_CNV) lmt.max_idx=dmn_sz; else lmt.max_idx=dmn_sz-1L;
    }else{
      /* Use user-specified limit when available */
      lmt.max_idx=strtol(lmt.max_sng,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
      if(*sng_cnv_rcd) nco_sng_cnv_err(lmt.max_sng,"strtol",sng_cnv_rcd);
    } /* end if */

    /* Adjust indices if FORTRAN style input was specified */
    if(FORTRAN_IDX_CNV){
      /* 20120726: Die when Fortran index is zero */
      if(lmt.min_idx == 0L || lmt.max_idx == 0L){
        (void)fprintf(stdout,"%s: ERROR User-specified Fortran (1-based) index for dimension %s = 0.\n",prg_nm_get(),lmt.nm);
        msg_sng=strdup("Fortran indices must be >= 1");
        NCO_SYNTAX_ERROR=True;
      } /* endif illegal Fortran index */
      /* 20120709: Adjust positive indices only */
      if(lmt.min_idx > 0L) lmt.min_idx--;
      if(lmt.max_idx > 0L) lmt.max_idx--;
    } /* end if */

    /* 20120709 Negative integer as min or max element of hyperslab specification indicates offset from end */
    if(lmt.min_idx < 0L) lmt.min_idx+=dmn_sz-1L;
    if(lmt.max_idx < 0L) lmt.max_idx+=dmn_sz-1L;

    /* Exit if requested indices are always invalid for all operators... */
    if(lmt.min_idx < 0L){
      msg_sng=strdup("Minimum index is too negative");
      NCO_SYNTAX_ERROR=True;
    }else if(lmt.max_idx < 0L){
      msg_sng=strdup("Maximum index is too negative");
      NCO_SYNTAX_ERROR=True;
    }else if(lmt.drn > lmt.srd){
      (void)fprintf(stdout,"%s: ERROR User-specified duration exceeds stride for %s: %li > %li\n",prg_nm_get(),lmt.nm,lmt.drn,lmt.srd);
      msg_sng=strdup("Duration exceeds stride");
      NCO_SYNTAX_ERROR=True;
    }else if(!rec_dmn_and_mfo && lmt.min_idx >= dmn_sz){
      msg_sng=strdup("Minimum index greater than size in non-MFO");
      NCO_SYNTAX_ERROR=True;
      (void)fprintf(stdout,"%s: ERROR User-specified dimension index range %li <= %s <= %li does not fall within valid dimension index range 0 <= %s <= %li\n",prg_nm_get(),lmt.min_idx,lmt.nm,lmt.max_idx,lmt.nm,dmn_sz-1L);
    }else if(lmt.max_idx >= dmn_sz && prg_id == ncks){
      /* 20130203 pvn Check for -d max > dimension size; check fortran case; check multi file operators */
      msg_sng=strdup("ERROR: Maximum index exceeds dimension size");
      NCO_SYNTAX_ERROR=True;
    } /* end if impossible indices */

    if(NCO_SYNTAX_ERROR){
      (void)fprintf(stdout,"%s: ERROR evaluating hyperslab specification for %s: %s\n%s: HINT Conform request to hyperslab documentation at http://nco.sf.net/nco.html#hyp\n",prg_nm_get(),lmt.nm,msg_sng,prg_nm_get());
      msg_sng=(char *)nco_free(msg_sng);
      nco_exit(EXIT_FAILURE);
    } /* !NCO_SYNTAX_ERROR */

    /* NB: Duration is officially supported only for ncra and ncrcat (record dimension only) */
    if(lmt.drn != 1L && !rec_dmn_and_mfo) (void)fprintf(stderr,"%s: WARNING Duration argument is only supported for the record dimension on ncra and ncrcat operations\n",prg_nm_get());

    /* Logic depends on whether this is record dimension in multi-file operator */
    if(!rec_dmn_and_mfo || !lmt.is_usr_spc_lmt){
      /* For non-record dimensions and for record dimensions where limit 
      was automatically generated (to include whole file), starting
      and ending indices are simply minimum and maximum indices already 
      in structure */
      lmt.srt=lmt.min_idx;
      lmt.end=lmt.max_idx;
    }else{
      /* Initialize rec_skp_vld_prv to 0L on first call to nco_lmt_evl() 
      This is necessary due to intrinsic hysterisis of rec_skp_vld_prv
      rec_skp_vld_prv is used only by multi-file operators
      rec_skp_vld_prv counts records skipped at end of previous valid file
      rec_usd_cml and rec_skp_ntl_spf are both zero only for first file */
      if(rec_usd_cml == 0L && lmt.rec_skp_ntl_spf == 0L) lmt.rec_skp_vld_prv=0L;

      /* For record dimensions with user-specified limit, allow possibility 
      that limit pertains to record dimension in a multi-file operator.
      Then user-specified maximum index may exceed number of records in any one file
      Thus lmt.srt does not necessarily equal lmt.min_idx and 
      lmt.end does not necessarily equal lmt.max_idx */
      /* NB: Stride is officially supported for ncks (all dimensions) and for ncra and ncrcat (record dimension only) */
      if(lmt.srd != 1L && prg_id != ncks && !lmt.is_rec_dmn) (void)fprintf(stderr,"%s: WARNING Stride argument for non-record dimension is only supported by ncks, use at your own risk...\n",prg_nm_get());

      { /* Block hides scope of local internal variables */
        long srt_min_lcl; /* [idx] Minimum start index (in absolute index space, i.e., relative to first file) for current file */
        long end_max_lcl; /* [idx] Maximum end   index (in absolute index space, i.e., relative to first file) for current file */

        srt_min_lcl=(lmt.is_usr_spc_min ? lmt.min_idx : lmt.rec_in_cml+0L); 
        end_max_lcl=(lmt.is_usr_spc_max ? lmt.max_idx : lmt.rec_in_cml+dmn_sz-1L); 

        /* Maximum allowed index in record dimension */
        lmt.idx_end_max_abs=end_max_lcl;

        /* Are we past file containing end_max_lcl yet? */
        if(end_max_lcl < lmt.rec_in_cml){
          /* This and all subsequent files are superfluous because all requested records have already been read 
          Optimize MFOs by checking "input complete" flag to jump out of file loop
          Saves time because no other input files will be opened */
          lmt.flg_input_complete=True;
          flg_no_data_ok=True;
          goto no_data_ok;
        } /* endif past end_max_lcl */

        /* Have we reached file containing srt_min_lcl yet? */
        if(srt_min_lcl > lmt.rec_in_cml+dmn_sz-1L){
          /* This and all previous files are superfluous because the starting record is in a subsequent file */
          flg_no_data_ok=True;
          goto no_data_ok;
        } /* endif srt_min_lcl in future file */

        /* Until records have been used, start index is srt_min_lcl adjusted for records contained in all previous files
        Thereafter start index loses memory of/dependence on absolute start index, and only cares for how many records,
        if any, were skipped since last valid record. This number, modulo stride, is new start index. */
        if(rec_usd_cml == 0L) lmt.srt=srt_min_lcl-lmt.rec_in_cml; else lmt.srt=lmt.srd-1L-lmt.rec_skp_vld_prv%lmt.srd;

        if(lmt.srt > dmn_sz-1L){
          /* Perhaps data were read in previous file(s) yet next record is in future file due to long stride */
          flg_no_data_ok=True;
          goto no_data_ok;
        } /* endif */

        lmt.end=(end_max_lcl < lmt.rec_in_cml+dmn_sz) ? end_max_lcl-lmt.rec_in_cml : dmn_sz-1L;

      } /* end block hides scope of local internal variables */

      /* If we are here then there are valid records in current file */

    } /* endif user-specified limits to record dimension */

  } /* end else limit arguments are hyperslab indices */

  /* NB: MFO record dimension never reaches this block if current file is superfluous
  In that case code has already branched down to flg_data_ok or flg_data_err */
  if(rec_dmn_and_mfo){ 
    /* NB: This is---and must be---performed as integer arithmetic */ 
    cnt_rmn_crr=1L+(lmt.end-lmt.srt)/lmt.srd;  
    /* This fixes "sloppy" specification of end index by user, i.e., ensures that end index coincides with a stride */
    lmt.end=lmt.srt+(cnt_rmn_crr-1L)*lmt.srd;   
    /* Save current rec_skp_vld_prv for diagnostics (printed below) for this file */
    rec_skp_vld_prv_dgn=lmt.rec_skp_vld_prv;
    /* Next file must know how many records in this file come after (and thus will be skipped) last used record in this file */
    lmt.rec_skp_vld_prv=dmn_sz-1L-lmt.end;
  } /* !rec_dmn_and_mfo */      

  /* Compute cnt from srt, end, and srd
  This is fine for multi-file record dimensions since those operators read-in one
  record at a time and thus never actually use lmt.cnt for record dimension. */
  if(lmt.srd == 1L){
    if(lmt.srt <= lmt.end) lmt.cnt=lmt.end-lmt.srt+1L; else lmt.cnt=dmn_sz-lmt.srt+lmt.end+1L;
  }else{
    if(lmt.srt <= lmt.end) lmt.cnt=1L+(lmt.end-lmt.srt)/lmt.srd; else lmt.cnt=1L+((dmn_sz-lmt.srt)+lmt.end)/lmt.srd;
  } /* end else */

  /* NB: Degenerate cases of WRP && SRD exist for which dmn_cnt_2 == 0
  This occurs when srd is large enough, or max_idx small enough, 
  such that no values are selected in the second read. 
  e.g., "-d lon,60,0,10" if sz(lon)=128 has dmn_cnt_2 == 0
  Since netCDF library reports an error reading and writing cnt=0 dimensions, kludge is necessary
  Syntax ensures that it is always the second read, not the first, which is obviated
  Therefore we convert these degenerate cases into non-wrapped coordinates to be processed by single read 
  For these degenerate cases only, [srt,end] are not a permutation of [min_idx,max_idx] */
  if(
    (lmt.srd != 1L) && /* SRD */
    (lmt.srt > lmt.end) && /* WRP */
    (lmt.cnt == (1L+(dmn_sz-lmt.srt-1L)/lmt.srd)) && /* dmn_cnt_1 == cnt -> dmn_cnt_2 == 0 */
    True){
      long greatest_srd_multiplier_1st_hyp_slb; /* Greatest integer m such that srt+m*srd < dmn_sz */
      long last_good_idx_1st_hyp_slb; /* C-index of last valid member of 1st hyperslab (= srt+m*srd) */
      /* long left_over_idx_1st_hyp_slb;*/ /* # of elements from first hyperslab that count towards current stride */
      long first_good_idx_2nd_hyp_slb; /* C-index of first valid member of 2nd hyperslab, if any */

      /* NB: Perform these operations with integer arithmetic or else! */
      /* Wrapped dimensions with stride may not start at idx 0 on second read */
      greatest_srd_multiplier_1st_hyp_slb=(dmn_sz-lmt.srt-1L)/lmt.srd;
      last_good_idx_1st_hyp_slb=lmt.srt+lmt.srd*greatest_srd_multiplier_1st_hyp_slb;
      /* left_over_idx_1st_hyp_slb=dmn_sz-last_good_idx_1st_hyp_slb-1L;*/
      first_good_idx_2nd_hyp_slb=(last_good_idx_1st_hyp_slb+lmt.srd)%dmn_sz;

      /* Conditions causing dmn_cnt_2 == 0 */
      if(first_good_idx_2nd_hyp_slb > lmt.end) lmt.end=last_good_idx_1st_hyp_slb;
  } /* end if */

  /* Cases where domain brackets no data, in error, have counts set to zero here
  This kludge allows codepaths for both WRP and out-of-domain to flow without goto statements
  Out-of-domain errors will soon exit with error, while WRP conditions will proceed */
  if(flg_no_data_err) lmt.cnt=0L;

  /* Exit when valid bracketed range contains no coordinates and that is not legal,
  i.e., this is not a superfluous file in an MFO */
  if(lmt.cnt == 0){
    if(lmt.lmt_typ == lmt_crd_val) (void)fprintf(stdout,"%s: ERROR Domain %g <= %s <= %g brackets no coordinate values.\n",prg_nm_get(),lmt.min_val,lmt.nm,lmt.max_val); 
    if(lmt.lmt_typ == lmt_dmn_idx) (void)fprintf(stdout,"%s: ERROR Empty domain for %s\n",prg_nm_get(),lmt.nm); 
    nco_exit(EXIT_FAILURE);
  } /* end if */

  /* Coordinate-valued limits that bracket no values in current file jump here with goto
  Index-valued limits with no values in current file flow here naturally */
no_data_ok: /* end goto */
  if(flg_no_data_ok){
    /* File is superfluous (contributes no data) to specified hyperslab
    Set output parameters to well-defined state
    This state must not cause ncra or ncrcat to retrieve any data
    ncra and ncrcat use loops for the record dimension, so this is
    accomplished by setting loop control values (lmt_rec.srt > lmt_rec.end)
    that cause record loop always to be skipped (never entered) */
    lmt.srt=-1L;
    lmt.end=lmt.srt-1L;
    lmt.cnt=-1L;
    /* Augment number of records skipped in initial superfluous files */
    if(rec_usd_cml == 0L) lmt.rec_skp_ntl_spf+=dmn_sz; 
    /* Augment records skipped since last good one */ 
    lmt.rec_skp_vld_prv+=dmn_sz;
    /* Set variables to preserve utility of diagnostics at end of routine */
    cnt_rmn_crr=rec_skp_vld_prv_dgn=0L;
  } /* endif */

  /* Accumulate count of records in all opened files, including this one
  Increment here at end so this structure member includes records from current file 
  only at end of this routine, where it can only be used diagnostically
  NB: Location of this augmentation is important! Moving it would have side-effects! */
  lmt.rec_in_cml+=dmn_sz;

  /* Place contents of working structure in location of returned structure */
  *lmt_ptr=lmt;

  if(dbg_lvl_get() == nco_dbg_io){
    (void)nco_prt_lmt(lmt,min_lmt_typ,FORTRAN_IDX_CNV,flg_no_data_ok,rec_usd_cml,monotonic_direction,rec_dmn_and_mfo,cnt_rmn_ttl,cnt_rmn_crr,rec_skp_vld_prv_dgn);
  } /* end dbg */

  if(lmt.srt > lmt.end && !flg_no_data_ok){
    if(prg_id != ncks) (void)fprintf(stderr,"WARNING: Possible instance of Schweitzer data hole requiring better diagnostics TODO #148\n");
    if(prg_id != ncks) (void)fprintf(stderr,"HINT: If operation fails, try multislabbing (http://nco.sf.net/nco.html#msa) wrapped dimension using ncks first, and then apply %s to the resulting file\n",prg_nm_get());
  } /* end dbg */

  fl_udu_sng=(char *)nco_free(fl_udu_sng);

} /* nco_lmt_evl_dmn_crd() */






