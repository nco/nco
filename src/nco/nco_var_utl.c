/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_var_utl.c,v 1.21 2002-09-09 03:40:00 zender Exp $ */

/* Purpose: Variable utilities */

/* Copyright (C) 1995--2002 Charlie Zender
   This software is distributed under the terms of the GNU General Public License
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

#include "nco_var_utl.h" /* Variable utilities */

int /* O [id] Output file variable ID */
nco_cpy_var_dfn /* [fnc] Copy variable metadata from input to output file */
(const int in_id, /* I [id] netCDF input file ID */
 const int out_id, /* I [id] netCDF output file ID */
 const int rec_dmn_id, /* I [id] Input file record dimension ID  */
 const char * const var_nm) /* I [sng] Input variable name */
{
  /* Purpose: Copy variable metadata from input netCDF file to output netCDF file
     Routine does not take into account any user-specified limits
     It just copies what it finds
     Routine copies_variable by variable, old-style, used only by ncks */

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
  dmn_in_id=(int *)nco_free(dmn_in_id);
  dmn_out_id=(int *)nco_free(dmn_out_id);
  
  return var_out_id;
} /* end nco_cpy_var_dfn() */

int /* O [id] Output file variable ID */
nco_cpy_var_dfn_lmt /* Copy variable metadata from input to output file */
(const int in_id, /* I [id] netCDF input file ID */
 const int out_id, /* I [id] netCDF output file ID */
 const int rec_dmn_id, /* I [id] Input file record dimension ID  */
 const char * const var_nm, /* I [sng] Input variable name */
 const lmt_sct * const lmt, /* I [sct] Hyperslab limits */
 const int lmt_nbr) /* I [nbr] Number of hyperslab limits */
{
  /* Purpose: Copy variable metadata from input netCDF file to output netCDF file
     This routine truncates dimensions in variable definition in output file according to user-specified limits.
     Routine copies_variable by variable, old-style, used only by ncks */

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
  dmn_in_id=(int *)nco_free(dmn_in_id);
  dmn_out_id=(int *)nco_free(dmn_out_id);
  
  return var_out_id;
} /* end nco_cpy_var_dfn_lmt() */

void
nco_cpy_var_val /* [fnc] Copy variable data from input to output file */
(int in_id, /* I [id] netCDF input file ID */
 int out_id, /* I [id] netCDF output file ID */
 FILE * const fp_bnr, /* I [fl] Unformatted binary output file handle */
 const bool NCO_BNR_WRT, /* I [flg] Write binary file */
 char *var_nm) /* I [sng] Variable name */
{
  /* Purpose: Copy variable data from input netCDF file to output netCDF file
     This routine does not take into account any user-specified limits, it just copies what it finds.
     Routine copies_variable by variable, old-style, used only by ncks */

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
  /* Write unformatted binary data */
  if(NCO_BNR_WRT) nco_bnr_wrt(fp_bnr,"",var_nm,var_sz,var_type,void_ptr);

  /* Free the space that held dimension IDs */
  dmn_cnt=(long *)nco_free(dmn_cnt);
  dmn_id=(int *)nco_free(dmn_id);
  dmn_sz=(long *)nco_free(dmn_sz);
  dmn_srt=(long *)nco_free(dmn_srt);

  /* Free the space that held variable */
  void_ptr=nco_free(void_ptr);

} /* end nco_cpy_var_val() */

void
nco_cpy_var_val_lmt /* [fnc] Copy variable data from input to output file */
(const int in_id, /* I [id] netCDF input file ID */
 const int out_id, /* I [id] netCDF output file ID */
 FILE * const fp_bnr, /* I [fl] Unformatted binary output file handle */
 const bool NCO_BNR_WRT, /* I [flg] Write binary file */
 char *var_nm, /* I [sng] Variable name */
 const lmt_sct * const lmt, /* I [sct] Hyperslab limits */
 const int lmt_nbr) /* I [nbr] Number of hyperslab limits */
{
  /* Routine to copy variable data from input netCDF file to output netCDF file. 
     Routine truncates dimensions in variable definition in output file according to user-specified limits.
     Routine copies_variable by variable, old-style, used only by ncks */

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
  if(nbr_dim==0){ /* Copy scalar */
    nco_get_var1(in_id,var_in_id,0L,void_ptr,var_type);
    nco_put_var1(out_id,var_out_id,0L,void_ptr,var_type);
    if(NCO_BNR_WRT) nco_bnr_wrt(fp_bnr,"",var_nm,var_sz,var_type,void_ptr);
  }else if(!WRP){ /* Copy contiguous array */
    if(!SRD) nco_get_vara(in_id,var_in_id,dmn_in_srt,dmn_cnt,void_ptr,var_type); else nco_get_varm(in_id,var_in_id,dmn_in_srt,dmn_cnt,dmn_srd,(long *)NULL,void_ptr,var_type);
    nco_put_vara(out_id,var_out_id,dmn_out_srt,dmn_cnt,void_ptr,var_type);
    if(NCO_BNR_WRT) nco_bnr_wrt(fp_bnr,"",var_nm,var_sz,var_type,void_ptr);
  }else if(WRP){ /* Copy wrapped array */
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
	      /* first_good_idx_2nd_hyp_slb=(last_good_idx_1st_hyp_slb+lmt[lmt_idx].srd)%dmn_sz[dmn_idx];*/ /* Variable is unused but instructive anyway */
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
    
    if(dbg_lvl_get() >= 5){
      (void)fprintf(stderr,"\nvar = %s\n",var_nm);
      (void)fprintf(stderr,"dim\tcnt\tsrtin1\tcnt1\tsrtout1\tsrtin2\tcnt2\tsrtout2\n");
      for(dmn_idx=0;dmn_idx<nbr_dim;dmn_idx++) (void)fprintf(stderr,"%d\t%ld\t%ld\t%ld\t%ld\t%ld\t%ld\t%ld\t\n",dmn_idx,dmn_cnt[dmn_idx],dmn_in_srt_1[dmn_idx],dmn_cnt_1[dmn_idx],dmn_out_srt_1[dmn_idx],dmn_in_srt_2[dmn_idx],dmn_cnt_2[dmn_idx],dmn_out_srt_2[dmn_idx]);
      (void)fflush(stderr);
    } /* end if dbg */

    if(False){
      /* If coordinate variable, perform monotonicity check */
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
      
      if(CRD && MNT){ /* If requested, apply monotonicity filter to wrapped coordinate */
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
	    (void)fprintf(stdout,"%s: ERROR Unknown nc_type %d in nco_cpy_var_val_lmt()\n",prg_nm_get(),var_type);
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
    
    /* fxm: Binary writes will not work for wrapped and stride variables until var_sz is changed to reflect actual size */
    if(!SRD){
      (void)nco_get_vara(in_id,var_in_id,dmn_in_srt_1,dmn_cnt_1,void_ptr,var_type);
      (void)nco_put_vara(out_id,var_out_id,dmn_out_srt_1,dmn_cnt_1,void_ptr,var_type);
      if(NCO_BNR_WRT) nco_bnr_wrt(fp_bnr,"",var_nm,var_sz,var_type,void_ptr);
      (void)nco_get_vara(in_id,var_in_id,dmn_in_srt_2,dmn_cnt_2,void_ptr,var_type);
      (void)nco_put_vara(out_id,var_out_id,dmn_out_srt_2,dmn_cnt_2,void_ptr,var_type);
      if(NCO_BNR_WRT) nco_bnr_wrt(fp_bnr,"",var_nm,var_sz,var_type,void_ptr);
    }else{ /* SRD */
      (void)nco_get_varm(in_id,var_in_id,dmn_in_srt_1,dmn_cnt_1,dmn_srd,(long *)NULL,void_ptr,var_type);
      (void)nco_put_vara(out_id,var_out_id,dmn_out_srt_1,dmn_cnt_1,void_ptr,var_type);
      if(NCO_BNR_WRT) nco_bnr_wrt(fp_bnr,"",var_nm,var_sz,var_type,void_ptr);
      (void)nco_get_varm(in_id,var_in_id,dmn_in_srt_2,dmn_cnt_2,dmn_srd,(long *)NULL,void_ptr,var_type);
      (void)nco_put_vara(out_id,var_out_id,dmn_out_srt_2,dmn_cnt_2,void_ptr,var_type);
      if(NCO_BNR_WRT) nco_bnr_wrt(fp_bnr,"",var_nm,var_sz,var_type,void_ptr);
    } /* end else SRD */
    
    dmn_in_srt_1=(long *)nco_free(dmn_in_srt_1);
    dmn_in_srt_2=(long *)nco_free(dmn_in_srt_2);
    dmn_out_srt_1=(long *)nco_free(dmn_out_srt_1);
    dmn_out_srt_2=(long *)nco_free(dmn_out_srt_2);
    dmn_cnt_1=(long *)nco_free(dmn_cnt_1);
    dmn_cnt_2=(long *)nco_free(dmn_cnt_2);

  } /* end if WRP */

  /* Free space that held dimension IDs */
  dmn_map=(long *)nco_free(dmn_map);
  dmn_srd=(long *)nco_free(dmn_srd);
  dmn_cnt=(long *)nco_free(dmn_cnt);
  dmn_id=(int *)nco_free(dmn_id);
  dmn_in_srt=(long *)nco_free(dmn_in_srt);
  dmn_out_srt=(long *)nco_free(dmn_out_srt);
  dmn_sz=(long *)nco_free(dmn_sz);

  /* Free space that held variable */
  void_ptr=nco_free(void_ptr);

} /* end nco_cpy_var_val_lmt() */

var_sct * /* O [sct] Copy of input variable */
nco_var_dpl /* [fnc] Duplicate input variable */
(const var_sct * const var) /* I [sct] Variable to duplicate */
{
  /* Threads: Routine is thread safe and calls no unsafe routines */
  /* Purpose: nco_malloc() and return duplicate of input var_sct
     Duplicate is deep copy of original so original may always be free()'d */

  var_sct *var_cpy;

  var_cpy=(var_sct *)nco_malloc(sizeof(var_sct));

  /* Shallow copy structure */
  (void)memcpy((void *)var_cpy,(void *)var,sizeof(var_sct));

  /* fxm: Should copy name as well, but var_free does not free it, and 
     var_lists do not strdup() user input so must make all changes at once 
     in order to avoid adding memory leak */

  /* Deep copy dyamically allocated arrays currently defined in original */
  if(var->val.vp != NULL){
    if((var_cpy->val.vp=(void *)malloc(var_cpy->sz*nco_typ_lng(var_cpy->type))) == NULL){
      (void)fprintf(stdout,"%s: ERROR Unable to malloc() %ld*%zu bytes for value buffer for variable %s in nco_var_dpl()\n",prg_nm_get(),var_cpy->sz,nco_typ_lng(var_cpy->type),var_cpy->nm);
      nco_exit(EXIT_FAILURE); 
    } /* end if */
    (void)memcpy((void *)(var_cpy->val.vp),(void *)(var->val.vp),var_cpy->sz*nco_typ_lng(var_cpy->type));
  } /* end if */
  if(var->mss_val.vp != NULL){
    var_cpy->mss_val.vp=(void *)nco_malloc(nco_typ_lng(var_cpy->type));
    (void)memcpy((void *)(var_cpy->mss_val.vp),(void *)(var->mss_val.vp),nco_typ_lng(var_cpy->type));
  } /* end if */
  if(var->tally != NULL){
    if((var_cpy->tally=(long *)malloc(var_cpy->sz*sizeof(long))) == NULL){
      (void)fprintf(stdout,"%s: ERROR Unable to malloc() %ld*%ld bytes for tally buffer for variable %s in var_cpy()\n",prg_nm_get(),var_cpy->sz,(long)sizeof(long),var_cpy->nm);
      nco_exit(EXIT_FAILURE); 
    } /* end if */
    (void)memcpy((void *)(var_cpy->tally),(void *)(var->tally),var_cpy->sz*sizeof(long));
  } /* end if */
  if(var->dim != NULL){
    var_cpy->dim=(dmn_sct **)nco_malloc(var_cpy->nbr_dim*sizeof(dmn_sct *));
    (void)memcpy((void *)(var_cpy->dim),(void *)(var->dim),var_cpy->nbr_dim*sizeof(var->dim[0]));
  } /* end if */
  if(var->dmn_id != NULL){
    var_cpy->dmn_id=(int *)nco_malloc(var_cpy->nbr_dim*sizeof(int));
    (void)memcpy((void *)(var_cpy->dmn_id),(void *)(var->dmn_id),var_cpy->nbr_dim*sizeof(var->dmn_id[0]));
  } /* end if */
  if(var->cnt != NULL){
    var_cpy->cnt=(long *)nco_malloc(var_cpy->nbr_dim*sizeof(long));
    (void)memcpy((void *)(var_cpy->cnt),(void *)(var->cnt),var_cpy->nbr_dim*sizeof(var->cnt[0]));
  } /* end if */
  if(var->srt != NULL){
    var_cpy->srt=(long *)nco_malloc(var_cpy->nbr_dim*sizeof(long));
    (void)memcpy((void *)(var_cpy->srt),(void *)(var->srt),var_cpy->nbr_dim*sizeof(var->srt[0]));
  } /* end if */
  if(var->end != NULL){
    var_cpy->end=(long *)nco_malloc(var_cpy->nbr_dim*sizeof(long));
    (void)memcpy((void *)(var_cpy->end),(void *)(var->end),var_cpy->nbr_dim*sizeof(var->end[0]));
  } /* end if */
  if(var->srd != NULL){
    var_cpy->srd=(long *)nco_malloc(var_cpy->nbr_dim*sizeof(long));
    (void)memcpy((void *)(var_cpy->srd),(void *)(var->srd),var_cpy->nbr_dim*sizeof(var->srd[0]));
  } /* end if */
  if(var->scl_fct.vp != NULL){
    var_cpy->scl_fct.vp=(void *)nco_malloc(nco_typ_lng(var_cpy->typ_upk));
    (void)memcpy((void *)(var_cpy->scl_fct.vp),(void *)(var->scl_fct.vp),nco_typ_lng(var_cpy->typ_upk));
  } /* end if */
  if(var->add_fst.vp != NULL){
    var_cpy->add_fst.vp=(void *)nco_malloc(nco_typ_lng(var_cpy->typ_upk));
    (void)memcpy((void *)(var_cpy->add_fst.vp),(void *)(var->add_fst.vp),nco_typ_lng(var_cpy->typ_upk));
  } /* end if */

  return var_cpy;

} /* end nco_var_dpl() */

void
nco_var_get /* [fnc] Allocate, retrieve variable hyperslab from disk to memory */
(const int nc_id, /* I [id] netCDF file ID */
 var_sct *var) /* I [sct] Variable to get */
{
  /* Threads: Routine contains thread-unsafe calls protected by critical regions */
  /* Purpose: Allocate and retrieve given variable hyperslab from disk into memory
     If variable is packed on disk then inquire about scale_factor and add_offset */

  if((var->val.vp=(void *)malloc(var->sz*nco_typ_lng(var->typ_dsk))) == NULL){
    (void)fprintf(stdout,"%s: ERROR Unable to malloc() %ld*%zu bytes in nco_var_get()\n",prg_nm_get(),var->sz,nco_typ_lng(var->type));
    nco_exit(EXIT_FAILURE);
  } /* end if */

#ifdef _OPENMP
#pragma omp critical
#endif /* _OPENMP */
  { /* begin OpenMP critical */
    if(var->sz > 1){
      (void)nco_get_vara(nc_id,var->id,var->srt,var->cnt,var->val.vp,var->typ_dsk);
    }else{
      (void)nco_get_var1(nc_id,var->id,var->srt,var->val.vp,var->typ_dsk);
    } /* end else */
  } /* end OpenMP critical */

  /* Packing properties of variable initially obtained from pck_dsk_inq() call in nco_var_fll()
     Multifile operators call nco_var_get() multiple times for each variable
     In between subsequent calls to nco_var_get(), variable may be unpacked 
     When this occurs, packing flags in variable structure will not match disk
     Thus it is important to refresh (some) packing attributes on each read */

  /* Synchronize missing value type with (possibly) new disk type pck_dbg */
  /* fxm: pck_dbg big bug on non-packed types in ncra here, double conversion
     of missing_value occurs */
  if(var->pck_dsk) var=nco_cnv_mss_val_typ(var,var->typ_dsk);

  /* Type of variable and missing value in memory are now same as type on disk */
  var->type=var->typ_dsk; /* Type of variable in RAM */

  /* Packing in RAM is now same as packing on disk pck_dbg 
     fxm: Following call to pck_dsk_inq() is never necessary for non-packed variables */
  (void)pck_dsk_inq(nc_id,var);
  
  /* Packing/Unpacking */
  if(is_rth_opr(prg_get())){
    /* fxm: Automatic unpacking is in beta testing for all arithmetic operators */
#ifdef _OPENMP
#pragma omp critical
#endif /* _OPENMP */
    if(var->pck_dsk) var=nco_var_upk(var);
  } /* endif arithmetic operator */

} /* end nco_var_get() */

void
nco_xrf_dmn /* [fnc] Switch pointers to dimension structures so var->dim points to var->dim->xrf */
(var_sct * const var) /* I [sct] Variable to manipulate */
{
  /* Purpose: Switch pointers to dimension structures so var->dim points to var->dim->xrf.
     Routine makes dim element of variable structure from nco_var_dpl() refer to counterparts
     of dimensions directly associated with variable it was duplicated from */
  
  int idx;
  
  for(idx=0;idx<var->nbr_dim;idx++) var->dim[idx]=var->dim[idx]->xrf;
  
} /* end nco_xrf_dmn() */

void
nco_xrf_var /* [fnc] Make xrf elements of variable structures point to eachother */
(var_sct * const var_1, /* I/O [sct] Variable */
 var_sct * const var_2) /* I/O [sct] Related variable */
{
  /* Purpose: Make xrf elements of variable structures point to eachother */

  var_1->xrf=var_2;
  var_2->xrf=var_1;

} /* end nco_xrf_var() */

var_sct * /* O [sct] Pointer to free'd variable */
nco_var_free /* [fnc] Free all memory associated with variable structure */
(var_sct *var) /* I [sct] Variable to free */
{
  /* Threads: Routine is thread safe and calls no unsafe routines */
  /* Purpose: Free all memory associated with a dynamically allocated variable structure */
  
  /* fxm: var->nm is not freed because names may be owned by optarg list (system)
     This assumption needs to be changed before freeing name pointer */
  
  var->val.vp=nco_free(var->val.vp);
  var->mss_val.vp=nco_free(var->mss_val.vp);
  var->tally=(long *)nco_free(var->tally);
  var->dmn_id=(int *)nco_free(var->dmn_id);
  var->dim=(dmn_sct **)nco_free(var->dim);
  var->srt=(long *)nco_free(var->srt);
  var->end=(long *)nco_free(var->end);
  var->cnt=(long *)nco_free(var->cnt);
  var->srd=(long *)nco_free(var->srd);
  var->scl_fct.vp=nco_free(var->scl_fct.vp);
  var->add_fst.vp=nco_free(var->add_fst.vp);

  /* Free structure pointer last */
  var=(var_sct *)nco_free(var);

  return NULL;

} /* end nco_var_free() */

int /* O [enm] Return code */
var_dfl_set /* [fnc] Set defaults for each member of variable structure */
(var_sct * const var) /* I [sct] Variable strucutre to initialize to defaults */
{
  /* Purpose: Set defaults for each member of variable structure
     var_dfl_set() should be called by all routines that create variables */

  int rcd=0; /* [enm] Return code */

  /* Set defaults to be overridden by available information */
  var->nm=NULL;
  var->id=-1;
  var->nc_id=-1;
  var->type=NC_NAT; /* Type of variable in RAM */
  var->typ_dsk=NC_NAT; /* Type of variable on disk */
  var->typ_pck=NC_NAT; /* Type of variable when packed (on disk). This should be same as typ_dsk except in cases where variable is packed in input file and unpacked in output file. */
  var->typ_upk=NC_NAT; /* Type of variable when unpacked (expanded) (in memory) */
  var->is_rec_var=False;
  var->is_crd_var=False;
  /* Size of 1 is assumed in nco_var_fll() */
  var->sz=1L;
  var->sz_rec=1L;
  var->cid=-1;
  var->has_mss_val=False;
  var->mss_val.vp=NULL;
  var->val.vp=NULL;
  var->tally=NULL;
  var->xrf=NULL;
  var->nbr_dim=-1;
  var->nbr_att=-1;
  var->dim=(dmn_sct **)NULL;
  var->dmn_id=(int *)NULL;
  var->cnt=(long *)NULL;
  var->srt=(long *)NULL;
  var->end=(long *)NULL;
  var->srd=(long *)NULL;

  /* Members related to packing */
  var->has_scl_fct=False; /* [flg] Valid scale_factor attribute exists */
  var->has_add_fst=False; /* [flg] Valid add_offset attribute exists */
  var->pck_dsk=False; /* [flg] Variable is packed on disk */
  var->pck_ram=False; /* [flg] Variable is packed in memory */
  var->scl_fct.vp=NULL; /* [ptr] Value of scale_factor attribute, if any */
  var->add_fst.vp=NULL; /* [ptr] Value of add_offset attribute, if any */

  return rcd; /* [enm] Return code */
} /* end var_dfl_set() */

void 
var_copy /* [fnc] Copy hyperslab variables of type var_typ from op1 to op2 */
(const nc_type var_typ, /* I [enm] netCDF type */
 const long sz, /* I [nbr] Number of elements to copy */
 const ptr_unn op1, /* I [sct] Values to copy */
 ptr_unn op2) /* O [sct] Destination to copy values to */
{
  /* Purpose: Copy hyperslab variables of type var_typ from op1 to op2
     Assumes memory area in op2 has already been malloc()'d */
  (void)memcpy((void *)(op2.vp),(void *)(op1.vp),sz*nco_typ_lng(var_typ));
} /* end var_copy() */

void
nco_var_dfn /* [fnc] Define variables and write their attributes to output file */
(const int in_id, /* I [enm] netCDF input-file ID */
 const char * const fl_out, /* I [sng] Name of output file */
 const int out_id, /* I [enm] netCDF output-file ID */
 var_sct * const * const var, /* I/O [sct] Variables to be defined in output file */
 const int nbr_var, /* I [nbr] Number of variables to be defined */
 CST_CST_CST_FNC(dmn_sct,dmn_ncl), /* I [sct] Dimensions included in output file */
 const int nbr_dmn_ncl) /* I [nbr] Number of dimensions in list */
{
  /* Purpose: Define variables in output file, copy their attributes */

  /* This function is unusual (for me) in that dimension arguments are only intended
     to be used by certain programs, those that alter the rank of input variables. 
     If program does not alter input variable rank (dimensionality) then it should
     call this function with NULL dimension list. Otherwise, this routine attempts
     to define variable correctly in output file (allowing variable to be
     defined with only those dimensions that are in dimension inclusion list) 
     without altering variable structures. 

     Moreover, this function is intended to be called with var_prc_out, not var_prc
     So local variable var usually refers to var_prc_out in calling function 
     Hence names may look reversed in this function, and xrf is frequently used */

  bool PCK_ATT_CPY; /* [flg] Copy attributes "scale_factor", "add_offset" */

  int idx_dmn;
  int dmn_id_vec[NC_MAX_DIMS];
  int idx;
  int rcd=NC_NOERR; /* [rcd] Return code */

  nc_type typ_out; /* [enm] Type in output file */
  
  for(idx=0;idx<nbr_var;idx++){

    /* Assume arithmetic operators store values as unpacked pck_dbg */
    if(is_rth_opr(prg_get())) typ_out=var[idx]->typ_upk; else typ_out=var[idx]->type;

    /* Is requested variable already in output file? */
    rcd=nco_inq_varid_flg(out_id,var[idx]->nm,&var[idx]->id);

    /* If variable has not been defined, define it */
    if(rcd != NC_NOERR){
      
      /* TODO #116: There is a problem here in that var_out[idx]->nbr_dim is never explicitly set to the actual number of output dimensions, rather, it is simply copied from var[idx]. When var_out[idx] actually has 0 dimensions, the loop executes once anyway, and an erroneous index into the dmn_out[idx] array is attempted. Fix is to explicitly define var_out[idx]->nbr_dim. Until this is done, anything in ncwa that explicitly depends on var_out[idx]->nbr_dim is suspect. The real problem is that, in ncwa, nco_var_avg() expects var_out[idx]->nbr_dim to contain the input, rather than output, number of dimensions. The routine, nco_var_dfn() was designed to call the simple branch when dmn_ncl == 0, i.e., for operators besides ncwa. However, when ncwa averages all dimensions in output file, nbr_dmn_ncl == 0 so the wrong branch would get called unless we specifically use this branch whenever ncwa is calling. */
      if(dmn_ncl != NULL || prg_get() == ncwa){
	int nbr_var_dim=0;
	int idx_ncl;
	/* Rank of output variable may need to be reduced */
	for(idx_dmn=0;idx_dmn<var[idx]->nbr_dim;idx_dmn++){
	  /* Is dimension allowed in output file? */
	  for(idx_ncl=0;idx_ncl<nbr_dmn_ncl;idx_ncl++){
	    if(var[idx]->xrf->dim[idx_dmn]->id == dmn_ncl[idx_ncl]->xrf->id) break;
	  } /* end loop over idx_ncl */
	  if(idx_ncl != nbr_dmn_ncl) dmn_id_vec[nbr_var_dim++]=var[idx]->dim[idx_dmn]->id;
	} /* end loop over idx_dmn */
	(void)nco_def_var(out_id,var[idx]->nm,typ_out,nbr_var_dim,dmn_id_vec,&var[idx]->id);
      }else{ /* ...operator is not ncwa which needs special handling... */
	/* More straightforward definition used by operators besides ncwa */
	for(idx_dmn=0;idx_dmn<var[idx]->nbr_dim;idx_dmn++){
	  dmn_id_vec[idx_dmn]=var[idx]->dim[idx_dmn]->id;
	} /* end loop over idx_dmn */
	(void)nco_def_var(out_id,var[idx]->nm,typ_out,var[idx]->nbr_dim,dmn_id_vec,&var[idx]->id);
      } /* end else */
      
      /* endif if variable has not yet been defined in output file */
    }else{
      /* Variable is already in output file---use existing definition
	 This branch is executed, e.g., by operators in append mode */
      (void)fprintf(stderr,"%s: WARNING Using existing definition of variable \"%s\" in %s\n",prg_nm_get(),var[idx]->nm,fl_out);
    } /* end if */

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
       --> Copy all attributes, define scale_factor and add_offset now, write their values later
    */

    /* var refers to output variable structure, var->xrf refers to input variable structure */
    /* Do not copy packing attributes "scale_factor" and "add_offset" 
       if variable is packed in input file but unpacked in output file */
    PCK_ATT_CPY=(is_rth_opr(prg_get()) && var[idx]->xrf->pck_dsk) ? False : True;
    (void)nco_att_cpy(in_id,out_id,var[idx]->xrf->id,var[idx]->id,PCK_ATT_CPY);

#undef FALSE
#ifdef FALSE
    /* Set nco_pck_typ based on program */
    switch(prg){
    case ncea:
      nco_pck_typ=nco_pck_all_new_att;
      break;
    case ncap:
    case ncra:
    case ncdiff:
    case ncflint:
    case ncwa:
    case ncrcat:
    case ncecat:
    default:
      nco_pck_typ=nco_pck_nil;
      break;
    } /* end switch */

    switch(nco_pck_typ){
    case nco_pck_all_xst_att:
    case nco_pck_all_new_att:
      break;
    case nco_pck_xst_xst_att:
    case nco_pck_xst_new_att:
      break;
    case nco_pck_upk:
      break;
    default:
      break;
    } /* end switch */
#endif /* not FALSE */

  } /* end loop over variables to define */
  
} /* end nco_var_dfn() */

void
nco_var_val_cpy /* [fnc] Copy variables data from input to output file */
(const int in_id, /* I [enm] netCDF file ID */
 const int out_id, /* I [enm] netCDF output file ID */
 var_sct ** const var, /* I/O [sct] Variables to copy to output file */
 const int nbr_var) /* I [nbr] Number of variables */
{
  /* Purpose: Copy variable data for every variable in input variable structure list
     from input file to output file */

  int idx;

  for(idx=0;idx<nbr_var;idx++){
    var[idx]->xrf->val.vp=var[idx]->val.vp=(void *)nco_malloc(var[idx]->sz*nco_typ_lng(var[idx]->type));
    if(var[idx]->nbr_dim==0){
      nco_get_var1(in_id,var[idx]->id,var[idx]->srt,var[idx]->val.vp,var[idx]->type);
      nco_put_var1(out_id,var[idx]->xrf->id,var[idx]->xrf->srt,var[idx]->xrf->val.vp,var[idx]->type);
    }else{ /* end if variable is a scalar */
      nco_get_vara(in_id,var[idx]->id,var[idx]->srt,var[idx]->cnt,var[idx]->val.vp,var[idx]->type);
      nco_put_vara(out_id,var[idx]->xrf->id,var[idx]->xrf->srt,var[idx]->xrf->cnt,var[idx]->xrf->val.vp,var[idx]->type);
    } /* end if variable is an array */
    var[idx]->val.vp=nco_free(var[idx]->val.vp); var[idx]->xrf->val.vp=var[idx]->val.vp=NULL;
  } /* end loop over idx */

} /* end nco_var_val_cpy() */

nm_id_sct * /* O [sct] List with coordinate excluded */
nco_var_lst_crd_xcl /* [fnc] Exclude given coordinates from extraction list */
(const int nc_id, /* I [id] netCDF file ID */
 const int dmn_id, /* I [id] Dimension ID of coordinate to remove from extraction list */
 nm_id_sct *xtr_lst, /* I/O [sct] Current extraction list (destroyed) */
 int * const nbr_xtr) /* I/O [nbr] Number of variables in extraction list */
{
  /* Purpose: Modify extraction list to exclude coordinate, if any, associated with given dimension ID */
  
  char crd_nm[NC_MAX_NAME];

  int idx;
  int crd_id=-1;
  int rcd=NC_NOERR; /* [rcd] Return code */
  
  /* What is variable ID of record coordinate, if any? */
  (void)nco_inq_dimname(nc_id,dmn_id,crd_nm);
   
  rcd=nco_inq_varid_flg(nc_id,crd_nm,&crd_id);
  if(rcd == NC_NOERR){
    /* Is coordinate on extraction list? */
    for(idx=0;idx<*nbr_xtr;idx++){
      if(xtr_lst[idx].id == crd_id) break;
    } /* end loop over idx */
    if(idx != *nbr_xtr){
      nm_id_sct *var_lst_tmp;
      
      var_lst_tmp=(nm_id_sct *)nco_malloc(*nbr_xtr*sizeof(nm_id_sct));
      /* Copy the extract list to the temporary extract list and reallocate the extract list */
      (void)memcpy((void *)var_lst_tmp,(void *)xtr_lst,*nbr_xtr*sizeof(nm_id_sct));
      (*nbr_xtr)--;
      xtr_lst=(nm_id_sct *)nco_realloc((void *)xtr_lst,*nbr_xtr*sizeof(nm_id_sct));
      /* Collapse the temporary extract list into the permanent list by copying 
	 all but the coordinate. NB: the ordering of the list is conserved. */
      (void)memcpy((void *)xtr_lst,(void *)var_lst_tmp,idx*sizeof(nm_id_sct));
      (void)memcpy((void *)(xtr_lst+idx),(void *)(var_lst_tmp+idx+1),(*nbr_xtr-idx)*sizeof(nm_id_sct));
      /* Free the memory for coordinate name in the extract list before losing the pointer */
      var_lst_tmp[idx].nm=(char *)nco_free(var_lst_tmp[idx].nm);
      var_lst_tmp=(nm_id_sct *)nco_free(var_lst_tmp);
    } /* end if */
  } /* end if */
  
  return xtr_lst;
  
} /* end nco_var_lst_crd_xcl() */

nm_id_sct * /* O [sct] Extraction list */
nco_var_lst_ass_crd_add /* [fnc] Add coordinates associated extracted variables to extraction list */
(const int nc_id, /* I netCDF file ID */
 nm_id_sct *xtr_lst, /* I/O current extraction list (destroyed) */
 int * const nbr_xtr) /* I/O number of variables in current extraction list */
{
  /* Purpose: Add coordinates associated extracted variables to extraction list
     This helps with making concise nco_malloc() calls down road */

  char dmn_nm[NC_MAX_NAME];

  int crd_id;
  int dmn_id[NC_MAX_DIMS];
  int idx_dmn;
  int idx_var_dim;
  int idx_var;
  int nbr_dim;
  int nbr_var_dim;
  int rcd=NC_NOERR; /* [rcd] Return code */

  /* Get number of dimensions */
  (void)nco_inq(nc_id,&nbr_dim,(int *)NULL,(int *)NULL,(int *)NULL);

  /* ...for each dimension in input file... */
  for(idx_dmn=0;idx_dmn<nbr_dim;idx_dmn++){
    /* ...see if it is a coordinate dimension... */
    (void)nco_inq_dimname(nc_id,idx_dmn,dmn_nm);
     
    rcd=nco_inq_varid_flg(nc_id,dmn_nm,&crd_id);
    if(rcd == NC_NOERR){
      /* Is this coordinate already on extraction list? */
      for(idx_var=0;idx_var<*nbr_xtr;idx_var++){
	if(crd_id == xtr_lst[idx_var].id) break;
      } /* end loop over idx_var */
      if(idx_var == *nbr_xtr){
	/* ...the coordinate is not on the list, is it associated with any variables?... */
	for(idx_var=0;idx_var<*nbr_xtr;idx_var++){
	  /* Get number of dimensions and dimension IDs for variable. */
	  (void)nco_inq_var(nc_id,xtr_lst[idx_var].id,(char *)NULL,(nc_type *)NULL,&nbr_var_dim,dmn_id,(int *)NULL);
	  for(idx_var_dim=0;idx_var_dim<nbr_var_dim;idx_var_dim++){
	    if(idx_dmn == dmn_id[idx_var_dim]) break;
	  } /* end loop over idx_var_dim */
	  if(idx_var_dim != nbr_var_dim){
	    /* Add the coordinate to the list */
	    xtr_lst=(nm_id_sct *)nco_realloc((void *)xtr_lst,(*nbr_xtr+1)*sizeof(nm_id_sct));
	    xtr_lst[*nbr_xtr].nm=(char *)strdup(dmn_nm);
	    xtr_lst[*nbr_xtr].id=crd_id;
	    (*nbr_xtr)++;
	    break;
	  } /* end if */
	} /* end loop over idx_var */
      } /* end if coordinate was not already on the list */
    } /* end if dimension is a coordinate */
  } /* end loop over idx_dmn */
  
  return xtr_lst;
  
} /* end nco_var_lst_ass_crd_add() */

var_sct * /* O [sct] Variable structure */
nco_var_fll /* [fnc] Allocate variable structure and fill with metadata */
(const int nc_id, /* I [id] netCDF file ID */
 const int var_id, /* I [id] variable ID */
 const char * const var_nm, /* I [sng] Variable name */
 dmn_sct * const * const dim, /* I [sct] Dimensions available to variable */
 const int nbr_dim) /* I [nbr] Number of dimensions in list */
{
  /* Purpose: nco_malloc() and return a completed var_sct */
  char dmn_nm[NC_MAX_NAME];

  int dmn_idx;
  int idx;
  int rec_dmn_id;

  var_sct *var;

  /* Get record dimension ID */
  (void)nco_inq(nc_id,(int *)NULL,(int *)NULL,(int *)NULL,&rec_dmn_id);
  
  /* Allocate space for variable structure */
  var=(var_sct *)nco_malloc(sizeof(var_sct));
  (void)var_dfl_set(var); /* [fnc] Set defaults for each member of variable structure */

  /* Fill in known fields */
  var->nm=(char *)strdup(var_nm);
  var->id=var_id;
  var->nc_id=nc_id;

  /* Get type and number of dimensions and attributes for variable */
  (void)nco_inq_var(var->nc_id,var->id,(char *)NULL,&var->typ_dsk,&var->nbr_dim,(int *)NULL,&var->nbr_att);

  /* Allocate space for dimension information */
  if(var->nbr_dim > 0) var->dim=(dmn_sct **)nco_malloc(var->nbr_dim*sizeof(dmn_sct *)); else var->dim=(dmn_sct **)NULL;
  if(var->nbr_dim > 0) var->dmn_id=(int *)nco_malloc(var->nbr_dim*sizeof(int)); else var->dmn_id=(int *)NULL;
  if(var->nbr_dim > 0) var->cnt=(long *)nco_malloc(var->nbr_dim*sizeof(long)); else var->cnt=(long *)NULL;
  if(var->nbr_dim > 0) var->srt=(long *)nco_malloc(var->nbr_dim*sizeof(long)); else var->srt=(long *)NULL;
  if(var->nbr_dim > 0) var->end=(long *)nco_malloc(var->nbr_dim*sizeof(long)); else var->end=(long *)NULL;
  if(var->nbr_dim > 0) var->srd=(long *)nco_malloc(var->nbr_dim*sizeof(long)); else var->srd=(long *)NULL;

  /* Get dimension IDs from input file */
  (void)nco_inq_vardimid(var->nc_id,var->id,var->dmn_id);
  
  /* Type in memory begins as same type as on disk */
  var->type=var->typ_dsk; /* [enm] Type of variable in RAM */
  /* Type of packed data on disk */
  var->typ_pck=var->type;  /* [enm] Type of variable when packed (on disk). This should be same as typ_dsk except in cases where variable is packed in input file and unpacked in output file. */

  /* Refresh number of attributes and missing value attribute, if any */
  var->has_mss_val=nco_mss_val_get(var->nc_id,var);

  /* Size defaults to 1 in var_dfl_set(), but set to 1 here for safety */
  var->sz=1L; 
  for(idx=0;idx<var->nbr_dim;idx++){
    (void)nco_inq_dimname(nc_id,var->dmn_id[idx],dmn_nm);
    /* Search input dimension list for matching name */
    for(dmn_idx=0;dmn_idx<nbr_dim;dmn_idx++){
      if(!strcmp(dmn_nm,dim[dmn_idx]->nm)) break;
    } /* end for */
    if(dmn_idx == nbr_dim){
      (void)fprintf(stdout,"%s: ERROR dimension %s is not in list of dimensions available to nco_var_fll()\n",prg_nm_get(),dmn_nm);
      (void)fprintf(stdout,"%s: HINT This could be the problem identified in TODO #111. Workaround is to make sure each dimension in the weighting and masking variable(s) appears in a variable to be processed.\n",prg_nm_get());
      nco_exit(EXIT_FAILURE);
    } /* end if */

    /* fxm: hmb, what is this for? */
    /* Re-define dim_id so that if dim is dimension list from output file
       then we get correct dim_id. Should not affect normal running of 
       routine as usually dim is dimension list from input file */
    var->dmn_id[idx]=dim[dmn_idx]->id;

    var->dim[idx]=dim[dmn_idx];
    var->cnt[idx]=dim[dmn_idx]->cnt;
    var->srt[idx]=dim[dmn_idx]->srt;
    var->end[idx]=dim[dmn_idx]->end;
    var->srd[idx]=dim[dmn_idx]->srd;

    if(var->dmn_id[idx] == rec_dmn_id) var->is_rec_var=True; else var->sz_rec*=var->cnt[idx];

    if(var->dim[idx]->is_crd_dmn && var->id == var->dim[idx]->cid){
      var->is_crd_var=True;
      var->cid=var->dmn_id[idx];
    } /* end if */

    /* NB: This assumes default var->sz begins as 1 */
    var->sz*=var->cnt[idx];
  } /* end loop over dim */

  /* Portions of variable structure depend on packing properties, e.g., typ_upk
     pck_dsk_inq() fills in these portions harmlessly */
  (void)pck_dsk_inq(nc_id,var);

  return var;
} /* end nco_var_fll() */

void
nco_var_refresh /* [fnc] Update variable metadata (var ID, dmn_nbr, mss_val) */
(const int nc_id, /* I [id] netCDF input-file ID */
 var_sct * const var) /* I/O [sct] Variable to update */
{
  /* Threads: Routine contains thread-unsafe calls protected by critical regions */
  /* Purpose: Update variable ID, number of dimensions, and missing_value attribute for given variable
     nco_var_refresh() is called in file loop in multi-file operators because each new file may have 
     different variable ID and missing_value for same variable.
     This is necessary, for example, if computer model runs on one machine, e.g., SGI,
     and then run is restarted on another, e.g., Cray. 
     If internal floating point representations differ between these architecture, 
     e.g., UNICOS vs. IEEE, then missing_value representation may differ. 
     Variable IDs may change when some but not all model output files are 
     manipulated (i.e., variables added or deleted), followed by processing
     all files are processed in a batch.
     NCO is the only tool I know of which makes this all user-transparent
     Thus this capability is very important to maintain */

  /* Refresh variable ID */
  var->nc_id=nc_id;

#ifdef _OPENMP
#pragma omp critical
#endif /* _OPENMP */
  { /* begin OpenMP critical */
    (void)nco_inq_varid(var->nc_id,var->nm,&var->id);
    
    /* fxm: Not sure if/why necessary to refresh number of dimensions...but it should not hurt */
    /* Refresh number of dimensions in variable */
    (void)nco_inq_varndims(var->nc_id,var->id,&var->nbr_dim);
    
    /* Refresh number of attributes and missing value attribute, if any */
    var->has_mss_val=nco_mss_val_get(var->nc_id,var);
  } /* end OpenMP critical */

  /* PJR requested warning to be added when multiple file operators worked on 
     variables with missing_value since so many things could go wrong */
#if 0
  if(is_rth_opr(prg_get()) && var->pck_dsk){
    if(var->has_mss_val) (void)fprintf(stderr,"%s: WARNING Variable \"%s\" is packed and has valid \"missing_value\" attribute in multi-file arithmetic operator. Arithmetic on this variable will only be correct if...  \n",prg_nm_get(),var_nm);
  } /* endif variable is packed */
#endif /* endif False */

} /* end nco_var_refresh() */

void
nco_var_srt_zero /* [fnc] Zero srt array of variable structure */
(var_sct ** const var, /* I [sct] Variables whose srt arrays will be zeroed */
 const int nbr_var) /* I [nbr] Number of structures in variable structure list */
{
  /* Purpose: Zero srt array of variable structure */

  int idx;
  int idx_dmn;

  for(idx=0;idx<nbr_var;idx++)
    for(idx_dmn=0;idx_dmn<var[idx]->nbr_dim;idx_dmn++)
      var[idx]->srt[idx_dmn]=0L;

} /* end nco_var_srt_zero() */
