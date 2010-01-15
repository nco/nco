/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_var_utl.c,v 1.146 2010-01-15 00:57:18 zender Exp $ */

/* Purpose: Variable utilities */

/* Copyright (C) 1995--2010 Charlie Zender
   License: GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */

#include "nco_var_utl.h" /* Variable utilities */

int /* O [id] Output file variable ID */
nco_cpy_var_dfn /* [fnc] Copy variable metadata from input to output file */
(const int in_id, /* I [id] netCDF input file ID */
 const int out_id, /* I [id] netCDF output file ID */
 const int rec_dmn_id, /* I [id] Input file record dimension ID  */
 const char * const var_nm, /* I [sng] Input variable name */
 const int dfl_lvl) /* I [enm] Deflate level [0..9] */
{
  /* Purpose: Copy variable metadata from input netCDF file to output netCDF file
     Routine does not take into account any user-specified limits
     It just copies what it finds
     Routine copies_variable by variable, old-style, used only by ncks */

  const char fnc_nm[]="nco_cpy_var_dfn()"; /* [sng] Function name */

  int *dmn_in_id;
  int *dmn_out_id;
  int fl_fmt; /* [enm] Output file format */
  int idx;
  int nbr_dim;
  int rcd=NC_NOERR; /* [rcd] Return code */
  int var_in_id;
  int var_out_id;
  
  nc_type var_type;
  
  /* Is requested variable already in output file? */
  rcd=nco_inq_varid_flg(out_id,var_nm,&var_out_id);
  if(rcd == NC_NOERR) return var_out_id;
  /* Is requested variable in input file? */
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
    int rcd_lcl; /* [rcd] Return code */
    
    (void)nco_inq_dim(in_id,dmn_in_id[idx],dmn_nm,&dmn_sz);
    
    /* Has dimension been defined in output file? */
    rcd_lcl=nco_inq_dimid_flg(out_id,dmn_nm,dmn_out_id+idx);
    
    /* Define dimension in output file if necessary */
    if(rcd_lcl != NC_NOERR){
      if(dmn_in_id[idx] != rec_dmn_id){
	(void)nco_def_dim(out_id,dmn_nm,dmn_sz,dmn_out_id+idx);
      }else{
	(void)nco_def_dim(out_id,dmn_nm,NC_UNLIMITED,dmn_out_id+idx);
      } /* end else */
    } /* end if */

  } /* end loop over dim */
  
  /* Define variable in output file */
  (void)nco_def_var(out_id,var_nm,var_type,nbr_dim,dmn_out_id,&var_out_id);

  /* Duplicate netCDF4 settings when possible */
  rcd=nco_inq_format(out_id,&fl_fmt);
  if(fl_fmt == NC_FORMAT_NETCDF4){
    /* Deflation */
    if(nbr_dim > 0){
      int shuffle; /* [flg] Turn on shuffle filter */
      int deflate; /* [flg] Turn on deflate filter */
      int dfl_lvl_in; /* [enm] Deflate level [0..9] */
      rcd=nco_inq_var_deflate(in_id,var_in_id,&shuffle,&deflate,&dfl_lvl_in);
      /* Copy original deflation settings */
      if(deflate || shuffle) (void)nco_def_var_deflate(out_id,var_out_id,deflate,shuffle,dfl_lvl_in);
      /* Overwrite HDF Lempel-Ziv compression level, if requested */
      if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,var_out_id,(int)True,(int)True,dfl_lvl);
    } /* endif */
    
    /* Chunking */
    if(nbr_dim > 0){
      int srg_typ; /* [enm] Storage type */
      size_t *cnk_sz; /* [nbr] Chunksize list */
      cnk_sz=(size_t *)nco_malloc(nbr_dim*sizeof(size_t));
      rcd=nco_inq_var_chunking(in_id,var_in_id,&srg_typ,cnk_sz);
      /* Copy original chunking settings */
      if(srg_typ == NC_CHUNKED){
	if(dbg_lvl_get() >= nco_dbg_var) (void)fprintf(stdout,"%s: DEBUG %s copying input-to-ouput chunking information for %s\n",prg_nm_get(),fnc_nm,var_nm);
	(void)nco_def_var_chunking(out_id,var_out_id,srg_typ,cnk_sz);
      } /* endif */
      cnk_sz=(size_t *)nco_free(cnk_sz);
    } /* endif */
  } /* !NC_FORMAT_NETCDF4 */ 
  
  /* Free space holding dimension IDs */
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
 CST_X_PTR_CST_PTR_CST_Y(lmt_all_sct,lmt_all_lst), /* I [sct] Hyperslab limits */
 const int lmt_all_lst_nbr, /* I [nbr] Number of hyperslab limits */
 const int dfl_lvl) /* I [enm] Deflate level [0..9] */
{
  /* Purpose: Copy variable metadata from input netCDF file to output netCDF file
     This routine truncates dimensions in variable definition in output file according to user-specified limits.
     Routine copies_variable by variable, old-style, used only by ncks */

  int *dmn_in_id;
  int *dmn_out_id;
  int fl_fmt; /* [enm] Output file format */
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
    int rcd_lcl; /* [rcd] Return code */
    
    (void)nco_inq_dim(in_id,dmn_in_id[idx],dmn_nm,&dmn_sz);
    
    /* Has dimension been defined in output file? */
    rcd_lcl=nco_inq_dimid_flg(out_id,dmn_nm,dmn_out_id+idx);
    
    /* If dimension has not been defined, copy it */
    if(rcd_lcl != NC_NOERR){
      if(dmn_in_id[idx] != rec_dmn_id){
	int lmt_all_idx;

	/* Does dimension have any user-specified limits? */
	for(lmt_all_idx=0;lmt_all_idx<lmt_all_lst_nbr;lmt_all_idx++){
	  if(lmt_all_lst[lmt_all_idx]->lmt_dmn[0]->id == dmn_in_id[idx]){
	    dmn_sz=lmt_all_lst[lmt_all_idx]->dmn_cnt;
	    break;
	  } /* end if */
	} /* end loop over lmt_all_idx */
	(void)nco_def_dim(out_id,dmn_nm,dmn_sz,dmn_out_id+idx);
      }else{
	(void)nco_def_dim(out_id,dmn_nm,NC_UNLIMITED,dmn_out_id+idx);
      } /* end else */
    } /* end if */
  } /* end loop over dim */
  
  /* Define variable in output file */
  (void)nco_def_var(out_id,var_nm,var_type,nbr_dim,dmn_out_id,&var_out_id);
  
  /* Duplicate netCDF4 settings when possible */
  rcd=nco_inq_format(out_id,&fl_fmt);
  if(fl_fmt == NC_FORMAT_NETCDF4){
    
    /* Deflation */
    if(nbr_dim > 0){
      int shuffle; /* [flg] Turn on shuffle filter */
      int deflate; /* [flg] Turn on deflate filter */
      int dfl_lvl_in; /* [enm] Deflate level [0..9] */
      rcd=nco_inq_var_deflate(in_id,var_in_id,&shuffle,&deflate,&dfl_lvl_in);
      /* Copy original deflation settings */
      if(deflate || shuffle) (void)nco_def_var_deflate(out_id,var_out_id,deflate,shuffle,dfl_lvl_in);
      /* Overwrite HDF Lempel-Ziv compression level, if requested */
      if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,var_out_id,(int)True,(int)True,dfl_lvl);
    } /* endif */
    
    /* Chunking */
    if(nbr_dim > 0){
      int srg_typ; /* [enm] Storage type */
      size_t *cnk_sz; /* [nbr] Chunksize list */
      cnk_sz=(size_t *)nco_malloc(nbr_dim*sizeof(size_t));
      rcd=nco_inq_var_chunking(in_id,var_in_id,&srg_typ,cnk_sz);
      /* Copy original chunking settings */
      if(srg_typ == NC_CHUNKED) (void)nco_def_var_chunking(out_id,var_out_id,srg_typ,cnk_sz);
      cnk_sz=(size_t *)nco_free(cnk_sz);
    } /* endif */
  } /* !NC_FORMAT_NETCDF4 */ 
  
  /* Free space holding dimension IDs */
  dmn_in_id=(int *)nco_free(dmn_in_id);
  dmn_out_id=(int *)nco_free(dmn_out_id);
  
  return var_out_id;
} /* end nco_cpy_var_dfn_lmt() */

void
nco_cpy_var_val /* [fnc] Copy variable from input to output file, no limits */
(const int in_id, /* I [id] netCDF input file ID */
 const int out_id, /* I [id] netCDF output file ID */
 FILE * const fp_bnr, /* I [fl] Unformatted binary output file handle */
 const nco_bool NCO_BNR_WRT, /* I [flg] Write binary file */
 const char *var_nm) /* I [sng] Variable name */
{
  /* NB: nco_cpy_var_val() contains OpenMP critical region */
  /* Purpose: Copy single variable from input netCDF file to output netCDF file
     Routine does not account for user-specified limits, it just copies what it finds
     Routine copies variable-by-variable, old-style, called only by ncks */

  const char fnc_nm[]="nco_cpy_var_val()"; /* [sng] Function name */

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
    (void)fprintf(stdout,"%s: ERROR attempt to write %d dimensional input variable %s to %d dimensional space in output file. \nHINT: When using -A (append) option, all appended variables must be the same rank in the input file as in the output file. The ncwa operator is useful at ridding variables of extraneous (size = 1) dimensions. See how at http://nco.sf.net/nco.html#ncwa\n",prg_nm_get(),nbr_dmn_in,var_nm,nbr_dmn_out);
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
  /* nc_inq_dimlen() returns maximum value used so far in writing record dimension data
     Until a record variable has been written, nc_inq_dimlen() returns dmn_sz=0 for record dimension in output file
     Thus we read input file for dimension sizes */
    (void)nco_inq_dimlen(in_id,dmn_id[idx],dmn_cnt+idx);

    /* Initialize indicial offset and stride arrays */
    dmn_srt[idx]=0L;
    var_sz*=dmn_cnt[idx];
  } /* end loop over dim */
      
  /* Allocate enough space to hold variable */
  void_ptr=(void *)nco_malloc_dbg(var_sz*nco_typ_lng(var_type),"Unable to malloc() value buffer when copying hypserslab from input to output file",fnc_nm);

  /* Get variable */
  if(nbr_dim==0){
    nco_get_var1(in_id,var_in_id,0L,void_ptr,var_type);
    nco_put_var1(out_id,var_out_id,0L,void_ptr,var_type);
  }else{ /* end if variable is scalar */
    if(var_sz > 0){ /* Allow for zero-size record variables */
      nco_get_vara(in_id,var_in_id,dmn_srt,dmn_cnt,void_ptr,var_type);
      nco_put_vara(out_id,var_out_id,dmn_srt,dmn_cnt,void_ptr,var_type);
    } /* end if var_sz */
  } /* end if variable is an array */
  /* Write unformatted binary data */
  if(NCO_BNR_WRT) nco_bnr_wrt(fp_bnr,var_nm,var_sz,var_type,void_ptr);

  /* Free space that held dimension IDs */
  dmn_cnt=(long *)nco_free(dmn_cnt);
  dmn_id=(int *)nco_free(dmn_id);
  dmn_sz=(long *)nco_free(dmn_sz);
  dmn_srt=(long *)nco_free(dmn_srt);

  /* Free space that held variable */
  void_ptr=nco_free(void_ptr);

} /* end nco_cpy_var_val() */

void
nco_cpy_var_val_lmt /* [fnc] Copy variable data from input to output file, simple hyperslabs */
(const int in_id, /* I [id] netCDF input file ID */
 const int out_id, /* I [id] netCDF output file ID */
 FILE * const fp_bnr, /* I [fl] Unformatted binary output file handle */
 const nco_bool NCO_BNR_WRT, /* I [flg] Write binary file */
 char *var_nm, /* I [sng] Variable name */
 const lmt_sct * const lmt, /* I [sct] Hyperslab limits */
 const int lmt_nbr) /* I [nbr] Number of hyperslab limits */
{
  /* NB: nco_cpy_var_val_lmt() contains OpenMP critical region */
  /* Purpose: Copy variable data from input netCDF file to output netCDF file 
     Truncate dimensions in variable definition in output file according to user-specified limits
     Copy variable-by-variable, old-style, used only by ncks */

  nco_bool SRD=False;
  nco_bool WRP=False;

  const char fnc_nm[]="nco_cpy_var_val_lmt()"; /* [sng] Function name */

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
  /* nc_inq_dimlen() returns maximum value used so far in writing record dimension data
     Until a record variable has been written, nc_inq_dimlen() returns dmn_sz=0 for record dimension in output file
     Thus we read input file for dimension sizes */

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
  void_ptr=(void *)nco_malloc_dbg(var_sz*nco_typ_lng(var_type),"Unable to malloc() value buffer when copying hypserslab from input to output file",fnc_nm);

  /* Copy variable */
  if(nbr_dim == 0){ /* Copy scalar */
    nco_get_var1(in_id,var_in_id,0L,void_ptr,var_type);
    nco_put_var1(out_id,var_out_id,0L,void_ptr,var_type);
    if(NCO_BNR_WRT) nco_bnr_wrt(fp_bnr,var_nm,var_sz,var_type,void_ptr);
  }else if(!WRP){ /* Copy contiguous array */
    if(!SRD) nco_get_vara(in_id,var_in_id,dmn_in_srt,dmn_cnt,void_ptr,var_type); else nco_get_varm(in_id,var_in_id,dmn_in_srt,dmn_cnt,dmn_srd,(long *)NULL,void_ptr,var_type);
    nco_put_vara(out_id,var_out_id,dmn_out_srt,dmn_cnt,void_ptr,var_type);
    if(NCO_BNR_WRT) nco_bnr_wrt(fp_bnr,var_nm,var_sz,var_type,void_ptr);
  }else if(WRP){ /* Copy wrapped array */
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

	      /* NB: Perform these operations with integer arithmetic or else! */
	      dmn_cnt_1[dmn_idx]=1L+(dmn_sz[dmn_idx]-lmt[lmt_idx].srt-1L)/lmt[lmt_idx].srd; 
	      /* Wrapped dimensions with stride may not start at idx 0 on second read */
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
      nco_bool CRD=False;
      nco_bool MNT=False;

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
	  case NC_UBYTE: break;
	  case NC_USHORT: break;
	  case NC_UINT: break;
	  case NC_INT64: break;
	  case NC_UINT64: break;
	  case NC_STRING: break;
	  default: nco_dfl_case_nc_type_err(); break;
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
      if(NCO_BNR_WRT) nco_bnr_wrt(fp_bnr,var_nm,var_sz,var_type,void_ptr);
      (void)nco_get_vara(in_id,var_in_id,dmn_in_srt_2,dmn_cnt_2,void_ptr,var_type);
      (void)nco_put_vara(out_id,var_out_id,dmn_out_srt_2,dmn_cnt_2,void_ptr,var_type);
      if(NCO_BNR_WRT) nco_bnr_wrt(fp_bnr,var_nm,var_sz,var_type,void_ptr);
    }else{ /* SRD */
      (void)nco_get_varm(in_id,var_in_id,dmn_in_srt_1,dmn_cnt_1,dmn_srd,(long *)NULL,void_ptr,var_type);
      (void)nco_put_vara(out_id,var_out_id,dmn_out_srt_1,dmn_cnt_1,void_ptr,var_type);
      if(NCO_BNR_WRT) nco_bnr_wrt(fp_bnr,var_nm,var_sz,var_type,void_ptr);
      (void)nco_get_varm(in_id,var_in_id,dmn_in_srt_2,dmn_cnt_2,dmn_srd,(long *)NULL,void_ptr,var_type);
      (void)nco_put_vara(out_id,var_out_id,dmn_out_srt_2,dmn_cnt_2,void_ptr,var_type);
      if(NCO_BNR_WRT) nco_bnr_wrt(fp_bnr,var_nm,var_sz,var_type,void_ptr);
    } /* end else SRD */
    
    dmn_in_srt_1=(long *)nco_free(dmn_in_srt_1);
    dmn_in_srt_2=(long *)nco_free(dmn_in_srt_2);
    dmn_out_srt_1=(long *)nco_free(dmn_out_srt_1);
    dmn_out_srt_2=(long *)nco_free(dmn_out_srt_2);
    dmn_cnt_1=(long *)nco_free(dmn_cnt_1);
    dmn_cnt_2=(long *)nco_free(dmn_cnt_2);

  } /* end if WRP */

  /* Free space that held metadata */
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
     Duplicate is deep-copy of original so original may always be free()'d */

  const char fnc_nm[]="nco_var_dpl()"; /* [sng] Function name */
  var_sct *var_cpy;

  var_cpy=(var_sct *)nco_malloc(sizeof(var_sct));

  /* Shallow-copy structure itself */
  (void)memcpy((void *)var_cpy,(const void *)var,sizeof(var_sct));

  /* Make sure var_free() frees names when variable is destructed */
  if(var->nm != NULL) var_cpy->nm=(char *)strdup(var->nm);

  /* Deep-copy dyamically allocated arrays from original to copy */
  if(var->val.vp != NULL){
    var_cpy->val.vp=(void *)nco_malloc_dbg(var_cpy->sz*nco_typ_lng(var_cpy->type),"Unable to malloc() value buffer in variable deep-copy",fnc_nm);
    (void)memcpy((void *)(var_cpy->val.vp),(void *)(var->val.vp),var_cpy->sz*nco_typ_lng(var_cpy->type));
    /* Deep-copy string data, if any */
    if(var->type == (nc_type)NC_STRING){
      ptr_unn val_in;
      ptr_unn val_out;
      long idx;
      long sz;
      sz=var->sz;  
      val_in=var->val;
      val_out=var_cpy->val;
      /* Typecast pointer to values before access 
	 Use local copies of pointer unions to maintain const-ness of var */
      (void)cast_void_nctype((nc_type)NC_STRING,&val_in); 
      (void)cast_void_nctype((nc_type)NC_STRING,&val_out); 
      for(idx=0;idx<sz;idx++) val_out.sngp[idx]=(char *)strdup(val_in.sngp[idx]);
      /* NB: we operated on local copies of val_in and val_out
	 Neither is used again in this routine
	 Therefore not necessary to un-typecast pointer unions */
    } /* endif type */
  } /* end if val */
  if(var->mss_val.vp != NULL){
    var_cpy->mss_val.vp=(void *)nco_malloc(nco_typ_lng(var_cpy->type));
    (void)memcpy((void *)(var_cpy->mss_val.vp),(void *)(var->mss_val.vp),nco_typ_lng(var_cpy->type));
  } /* end if */
  if(var->tally != NULL){
    var_cpy->tally=(long *)nco_malloc_dbg(var_cpy->sz*sizeof(long),"Unable to malloc() tally buffer in variable deep-copy",fnc_nm);
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
  if(var->cnk_sz != NULL){
    var_cpy->cnk_sz=(size_t *)nco_malloc(var_cpy->nbr_dim*sizeof(size_t));
    (void)memcpy((void *)(var_cpy->cnk_sz),(void *)(var->cnk_sz),var_cpy->nbr_dim*sizeof(var->cnk_sz[0]));
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
  /* NB: nco_var_get() with same nc_id contains OpenMP critical region */
  /* Purpose: Allocate and retrieve given variable hyperslab from disk into memory
     If variable is packed on disk then inquire about scale_factor and add_offset */
  int idx;
  long srd_prd=1L; /* [nbr] Product of strides */
  const char fnc_nm[]="nco_var_get()"; /* [sng] Function name */

  var->val.vp=(void *)nco_malloc_dbg(var->sz*nco_typ_lng(var->typ_dsk),"Unable to malloc() value buffer when retrieving variable from disk",fnc_nm);

  if(False) (void)fprintf(stdout,"%s: DEBUG: fxm TODO nco354. Calling nco_get_vara() for %s with nc_id=%d, var_id=%d, var_srt=%li, var_cnt = %li, var_val = %g, var_typ = %s\n",prg_nm_get(),var->nm,nc_id,var->id,var->srt[0],var->cnt[0],var->val.fp[0],nco_typ_sng(var->typ_dsk));

  /* 20051021: Removed this potentially critical region by parallelizing 
     over in_id's in calling code */
  /* 20051019: nco_get_var*() routines are potentially SMP-critical
     netCDF library allows parallel reads by different processes, not threads
     Parallel reads to the same nc_id by different threads are critical because
     the underlying UNIX file open has limited stdin caching
     Parallel reads to different nc_id's for same underlying file work because
     each UNIX file open (for same file) creates own stdin caching */
  /* 20050629: Removing this critical region and calling with identical nc_id's causes multiple ncwa/ncra regressions */
  { /* begin potential OpenMP critical */
    /* Block is critical/thread-safe for identical/distinct in_id's */

    /* Is stride > 1? */
    for(idx=0;idx<var->nbr_dim;idx++) srd_prd*=var->srd[idx];

    if(srd_prd == 1L){ 
      if(var->sz > 1L)
	(void)nco_get_vara(nc_id,var->id,var->srt,var->cnt,var->val.vp,var->typ_dsk);
      else
	(void)nco_get_var1(nc_id,var->id,var->srt,var->val.vp,var->typ_dsk);
    }else{ 
      (void)nco_get_varm(nc_id,var->id,var->srt,var->cnt,var->srd,(long *)NULL,var->val.vp,var->typ_dsk);
    } /* endif non-unity stride  */
  } /* end potential OpenMP critical */
  
  /* Packing properties initially obtained by nco_pck_dsk_inq() in nco_var_fll()
     Multi-file operators (MFOs) call nco_var_get() multiple times for each variable
     In between subsequent calls to nco_var_get(), variable may be unpacked 
     When this occurs, packing flags in variable structure will not match disk
     Thus it is important to refresh (some) packing attributes on each read */

  /* Synchronize missing value type with (possibly) new disk type */
  /* fxm nco427: pck_dbg potential big bug on non-packed types in ncra here,
     due to potential double conversion of missing_value
     First conversion to typ_dsk occurs when nco_var_fll() reads in mss_val
     Second conversion occurs here mss_val again converted to typ_dsk
     fxm nco457: Why not always convert missing_value to variable type, 
     even when variable is not packed? Answer: because doing this appears
     to break some ncra tests */
  if(var->pck_dsk) var=nco_cnv_mss_val_typ(var,var->typ_dsk);
  /*    var=nco_cnv_mss_val_typ(var,var->typ_dsk);*/

  /* Type of variable and missing value in memory are now same as type on disk */
  var->type=var->typ_dsk; /* [enm] Type of variable in RAM */

  /* Packing in RAM is now same as packing on disk pck_dbg 
     fxm: Following call to nco_pck_dsk_inq() is never necessary for non-packed variables */
  (void)nco_pck_dsk_inq(nc_id,var);
  
  /* Packing/Unpacking */
  if(nco_is_rth_opr(prg_get())){
    /* Arithmetic operators must unpack variables before performing arithmetic
       Otherwise arithmetic will produce garbage results */
    /* 20050519: Not sure why I originally made nco_var_upk() call SMP-critical
       20050629: Making this region multi-threaded causes no problems */
    if(var->pck_dsk) var=nco_var_upk(var);
  } /* endif arithmetic operator */
  
} /* end nco_var_get() */

void
nco_xrf_dmn /* [fnc] Switch pointers to dimension structures so var->dim points to var->dim->xrf */
(var_sct * const var) /* I [sct] Variable to manipulate */
{
  /* Purpose: Switch pointers to dimension structures so var->dim points to var->dim->xrf
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
(var_sct *var) /* I/O [sct] Variable to free */
{
  /* Threads: Routine is thread safe and calls no unsafe routines */
  /* Purpose: Free all memory associated with a dynamically allocated variable structure */

  /* String values must be deep-free'd, everything else is a flat buffer */
  if(var->type == (nc_type)NC_STRING && var->val.vp != (void *)NULL) var->val.vp=(void *)nco_sng_lst_free((char **)var->val.vp,var->sz); else var->val.vp=nco_free(var->val.vp);
  var->nm=(char *)nco_free(var->nm);
  var->mss_val.vp=nco_free(var->mss_val.vp);
  var->tally=(long *)nco_free(var->tally);
  var->dmn_id=(int *)nco_free(var->dmn_id);
  var->cnk_sz=(size_t *)nco_free(var->cnk_sz);
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

var_sct ** /* O [sct] Pointer to free'd structure list */
nco_var_lst_free /* [fnc] Free memory associated with variable structure list */
(var_sct **var_lst, /* I/O [sct] Variable structure list to free */
 const int var_nbr) /* I [nbr] Number of variable structures in list */
{
  /* Threads: Routine is thread safe and calls no unsafe routines */
  /* Purpose: Free all memory associated with dynamically allocated variable structure list */
  int idx;

  for(idx=0;idx<var_nbr;idx++) var_lst[idx]=nco_var_free(var_lst[idx]);

  /* Free structure pointer last */
  var_lst=(var_sct **)nco_free(var_lst);

  return var_lst;
} /* end nco_var_lst_free() */

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
  /* nco_var_fll() assumes size of 1 */
  var->sz=1L;
  var->sz_rec=1L;
  var->cid=-1;
  var->has_dpl_dmn=False;
  var->has_mss_val=False;
  var->mss_val.vp=NULL;
  var->val.vp=NULL;
  var->tally=NULL;
  var->xrf=NULL;
  var->nbr_dim=-1;
  var->nbr_att=-1;
  var->dim=(dmn_sct **)NULL;
  var->dmn_id=(int *)NULL;
  var->cnk_sz=(size_t *)NULL;
  var->cnt=(long *)NULL;
  var->srt=(long *)NULL;
  var->end=(long *)NULL;
  var->srd=(long *)NULL;
  var->undefined=False;
  var->is_fix_var=True; /* Is this a fixed (non-processed) variable? */

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
 CST_X_PTR_CST_PTR_CST_Y(dmn_sct,dmn_ncl), /* I [sct] Dimensions included in output file */
 const int nbr_dmn_ncl, /* I [nbr] Number of dimensions in list */
 const int nco_pck_map, /* I [enm] Packing map */
 const int nco_pck_plc, /* I [enm] Packing policy */
 const int dfl_lvl) /* I [enm] Deflate level [0..9] */
{
  /* Purpose: Define variables in output file, copy their attributes */

  /* This function is unusual (for me) in that dimension arguments are only intended
     to be used by certain programs, those that alter the rank of input variables. 
     Programs that do not alter input variable rank (dimensionality) should
     call this function with NULL dimension list and nbr_dmn_ncl=0. 
     Otherwise, this routine attempts to define variable correctly in output file 
     (allowing variable to be defined with only those dimensions that are in dimension inclusion list) 
     without altering variable structures. 

     Moreover, this function is intended to be called with var_prc_out, not var_prc
     So local variable var usually refers to var_prc_out in calling function 
     Hence names may look reversed in this function, and xrf is frequently used

     20060217: Packed fixed (non-processed) variables are now passed through unaltered */

  nco_bool PCK_ATT_CPY=True; /* [flg] Copy attributes "scale_factor", "add_offset" */

  const char fnc_nm[]="nco_var_dfn()"; /* [sng] Function name */

  int dmn_nbr=0;
  int dmn_id_vec[NC_MAX_DIMS];
  int idx;
  int dmn_idx;
  int prg_id; /* [enm] Program ID */
  int rcd=NC_NOERR; /* [rcd] Return code */

  nc_type typ_out; /* [enm] Type in output file */
  
  prg_id=prg_get(); /* [enm] Program ID */

  for(idx=0;idx<nbr_var;idx++){

    /* Checking only nco_is_rth_opr() is too simplistic
       1. All variables handled by arithmetic operators are currently unpacked on reading
       2. However "fixed variables" appear in many arithemetic operators
	  ncbo treats coordinate variables as fixed (does not subtract them)
          ncra treats non-record variables as fixed (does not average them)
	  ncwa treats variables without averaging dimensions as fixed (does not average them)
	  It is best not to alter [un-]pack fixed (non-processed) variables
       3. ncap, an arithmetic operator, also has "fixed variables", i.e., 
          pre-existing non-LHS variables copied directly to output.
	  These "fixed" ncap variables should remain unaltered
	  However, this is not presently done
	  nco_var_dfn() needs more information to handle "fixed" variables correctly because
	  Some ncap "fixed" variables appear on RHS in definitions of LHS variables
          These RHS fixed variables must be separately unpacked during RHS algebra
	  Currently, ncap only calls nco_var_dfn() for fixed variables
	  ncap uses its own routine, ncap_var_write(), for RHS variable definitions
       4. All variables in non-arithmetic operators (except ncpdq) should remain un-altered
       5. ncpdq is non-arithmetic operator
          However, ncpdq specially handles fine-grained control [un-]packing options */
    if(nco_is_rth_opr(prg_id)){
      /* Arithmetic operators store values as unpacked... */
      typ_out=var[idx]->typ_upk; 
      /* ...with two exceptions...
	 ncap [un-]packing precedes nco_var_dfn() call, sets var->type appropriately */
      if(prg_id == ncap) typ_out=var[idx]->type;
      /* ...and pass through fixed (non-processed) variables untouched... */
      if(var[idx]->is_fix_var) typ_out=var[idx]->type;
    }else{
      /* Non-arithmetic operators leave things alone by default
	 ncpdq first modifies var_out->type, then calls nco_var_dfn(), then [un-]packs */
      typ_out=var[idx]->type;
    } /* endif arithmetic operator */

    /* Is requested variable already in output file? */
    rcd=nco_inq_varid_flg(out_id,var[idx]->nm,&var[idx]->id);

    /* If variable has not been defined, define it */
    if(rcd != NC_NOERR){
      
      /* TODO #116: There is a problem here in that var_out[idx]->nbr_dim is never explicitly set to the actual number of output dimensions, rather, it is simply copied from var[idx]. When var_out[idx] actually has 0 dimensions, the loop executes once anyway, and an erroneous index into the dmn_out[idx] array is attempted. Fix is to explicitly define var_out[idx]->nbr_dim. Until this is done, anything in ncwa that explicitly depends on var_out[idx]->nbr_dim is suspect. The real problem is that, in ncwa, nco_var_avg() expects var_out[idx]->nbr_dim to contain the input, rather than output, number of dimensions. The routine, nco_var_dfn() was designed to call the simple branch when dmn_ncl == 0, i.e., for operators besides ncwa. However, when ncwa averages all dimensions in output file, nbr_dmn_ncl == 0 so the wrong branch would get called unless we specifically use this branch whenever ncwa is calling. */
      if(dmn_ncl != NULL || prg_id == ncwa){
	/* ...operator is ncwa and/or changes variable rank... */
	int idx_ncl;
	/* Initialize number of dimensions for current variable */
	dmn_nbr=0;
	for(dmn_idx=0;dmn_idx<var[idx]->nbr_dim;dmn_idx++){
	  /* Is dimension allowed in output file? */
	  for(idx_ncl=0;idx_ncl<nbr_dmn_ncl;idx_ncl++){
	    /* All I can say about this line, is...Yikes! 
	       No, really, it indicates poor program design
	       fxm: TODO nco374: have ncwa re-arrange output metadata prior to nco_var_dfn()
	       Then delete this branch and use straightforward branch of code */
	    if(var[idx]->xrf->dim[dmn_idx]->id == dmn_ncl[idx_ncl]->xrf->id) break;
	  } /* end loop over idx_ncl */
	  if(idx_ncl != nbr_dmn_ncl) dmn_id_vec[dmn_nbr++]=var[idx]->dim[dmn_idx]->id;
	} /* end loop over dmn_idx */
      }else{ /* ...operator does not change variable rank so handle normally... */
	/* More straightforward definition used by operators besides ncwa */
	for(dmn_idx=0;dmn_idx<var[idx]->nbr_dim;dmn_idx++){
	  dmn_id_vec[dmn_idx]=var[idx]->dim[dmn_idx]->id;
	} /* end loop over dmn_idx */
	dmn_nbr=var[idx]->nbr_dim;
      } /* end else */

      if(dbg_lvl_get() > 3 && prg_id != ncwa){
	/* fxm TODO nco374 diagnostic information fails for ncwa since var[idx]->dim[dmn_idx]->nm
	   contains _wrong name_ when variables will be averaged.
	   ncwa does contain write name information now if retain_degenerate_dimensions 
	   option is in effect, but this is the exception rather than the rule. */
	(void)fprintf(stdout,"%s: DEBUG %s about to define variable %s with %d dimension%s%s",prg_nm_get(),fnc_nm,var[idx]->nm,dmn_nbr,(dmn_nbr == 1) ? "" : "s",(dmn_nbr > 0) ? " (ordinal,output ID): " : "");
	for(dmn_idx=0;dmn_idx<dmn_nbr;dmn_idx++){
	  (void)fprintf(stdout,"%s (%d,%s)%s",var[idx]->dim[dmn_idx]->nm,dmn_idx,"unknown",(dmn_idx < dmn_nbr-1) ? ", " : "");
	} /* end loop over dmn */
	(void)fprintf(stdout,"\n");
      } /* endif dbg */

      /* The all-important variable definition call itself... */
      (void)nco_def_var(out_id,var[idx]->nm,typ_out,dmn_nbr,dmn_id_vec,&var[idx]->id);
      
      /* Set HDF Lempel-Ziv compression level, if requested */
      if(dfl_lvl > 0 && dmn_nbr > 0) (void)nco_def_var_deflate(out_id,var[idx]->id,(int)True,(int)True,dfl_lvl);

      if(dbg_lvl_get() > 3 && prg_id != ncwa){
	/* fxm TODO nco374 diagnostic information fails for ncwa since var[idx]->dim[dmn_idx]->nm
	   contains _wrong name_ when variables will be averaged.
	   ncwa does contain write name information now if retain_degenerate_dimensions 
	   option is in effect, but this is the exception rather than the rule. */
	(void)fprintf(stdout,"%s: DEBUG %s defined variable %s with %d dimension%s%s",prg_nm_get(),fnc_nm,var[idx]->nm,dmn_nbr,(dmn_nbr == 1) ? "" : "s",(dmn_nbr > 0) ? " (ordinal,output ID): " : "");
	for(dmn_idx=0;dmn_idx<dmn_nbr;dmn_idx++){
	  (void)fprintf(stdout,"%s (%d,%d)%s",var[idx]->dim[dmn_idx]->nm,dmn_idx,dmn_id_vec[dmn_idx],(dmn_idx < dmn_nbr-1) ? ", " : "");
	} /* end loop over dmn */
	(void)fprintf(stdout,"\n");
      } /* endif dbg */

      /* endif variable has not yet been defined in output file */
    }else{
      /* Variable is already in output file---use existing definition
	 This branch is executed, e.g., by operators in append mode */
      (void)fprintf(stdout,"%s: WARNING Using existing definition of variable \"%s\" in %s\n",prg_nm_get(),var[idx]->nm,fl_out);
    } /* end if variable is already in output file */

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
       --> Copy all attributes, define dummy values for scale_factor and add_offset now, 
           write real values later */

    /* Do not copy packing attributes "scale_factor" and "add_offset" 
       if variable is packed in input file and unpacked in output file 
       Arithmetic operators calling nco_var_dfn() with fixed variables should leave them fixed
       Currently ncap calls nco_var_dfn() only for fixed variables, so handle exception with ncap-specific condition */
    /* Copy exising packing attributes, if any, unless... */
    if(nco_is_rth_opr(prg_id) && /* ...operator is arithmetic... */
       prg_id != ncap && /* ...and is not ncap (hence it must be, e.g., ncra, ncbo)... */
       !var[idx]->is_fix_var && /* ...and variable is processed (not fixed)... */
       var[idx]->xrf->pck_dsk) /* ...and variable is packed in input file... */
      PCK_ATT_CPY=False;

    /* Do not copy packing attributes when unpacking variables 
       ncpdq is currently only operator that passes values other than nco_pck_plc_nil */
    if(nco_pck_plc == nco_pck_plc_upk) /* ...and variable will be _unpacked_ ... */
      PCK_ATT_CPY=False;
    
    /* Recall that:
       var      refers to output variable structure
       var->xrf refers to input  variable structure 
       ncpdq may pre-define packing attributes below regardless of PCK_ATT_CPY */ 
    (void)nco_att_cpy(in_id,out_id,var[idx]->xrf->id,var[idx]->id,PCK_ATT_CPY);
    
    /* Create dummy packing attributes for ncpdq if necessary 
       Must apply nearly same logic at end of ncpdq when writing final attributes
       Recall ncap calls ncap_var_write() to define newly packed LHS variables 
       If operator will attempt to pack some variables... */
    if(nco_pck_plc != nco_pck_plc_nil && nco_pck_plc != nco_pck_plc_upk){ 
      /* ...and expanded variable is pack-able... */
      if(nco_pck_plc_typ_get(nco_pck_map,var[idx]->typ_upk,(nc_type *)NULL)){
	/* ...and operator will pack this particular variable... */
	if(
	   /* ...either because operator newly packs all variables... */
	   (nco_pck_plc == nco_pck_plc_all_new_att) ||
	   /* ...or because operator newly packs un-packed variables like this one... */
	   (nco_pck_plc == nco_pck_plc_all_xst_att && !var[idx]->pck_ram) ||
	   /* ...or because operator re-packs packed variables like this one... */
	   (nco_pck_plc == nco_pck_plc_xst_new_att && var[idx]->pck_ram)
	   ){
	  
	  /* ...then add/overwrite dummy scale_factor and add_offset attributes
	     Overwrite these with correct values once known
	     Adding dummy attributes of maximum possible size (NC_DOUBLE) now 
	     reduces likelihood that netCDF layer will impose file copy 
	     penalties when final attribute values are written later
	     Either add_offset or scale_factor may be removed in nco_pck_val() 
	     if nco_var_pck() packing algorithm did not require utilizing it */ 
	  const char add_fst_sng[]="add_offset"; /* [sng] Unidata standard string for add offset */
	  const char scl_fct_sng[]="scale_factor"; /* [sng] Unidata standard string for scale factor */
	  val_unn zero_unn; /* [frc] Generic container for value 0.0 */
	  var_sct *zero_var; /* [sct] NCO variable for value 0.0 */
	  zero_unn.d=0.0; /* [frc] Generic container for value 0.0 */
	  zero_var=scl_mk_var(zero_unn,typ_out); /* [sct] NCO variable for value 0.0 */
	  (void)nco_put_att(out_id,var[idx]->id,scl_fct_sng,typ_out,1,zero_var->val.vp);
	  (void)nco_put_att(out_id,var[idx]->id,add_fst_sng,typ_out,1,zero_var->val.vp);
	  zero_var=(var_sct *)nco_var_free(zero_var);
	} /* endif this variable will be packed or re-packed */
      } /* !nco_pck_plc_alw */
    } /* endif nco_pck_plc involves packing */
  } /* end loop over idx variables to define */
} /* end nco_var_dfn() */

void
nco_var_val_cpy /* [fnc] Copy variables data from input to output file */
(const int in_id, /* I [enm] netCDF file ID */
 const int out_id, /* I [enm] netCDF output file ID */
 var_sct ** const var, /* I/O [sct] Variables to copy to output file */
 const int nbr_var) /* I [nbr] Number of variables */
{
  /* NB: nco_var_val_cpy() contains OpenMP critical region */
  /* Purpose: Copy every variable in input variable structure list 
     from input file to output file. Only data (not metadata) are copied. */
  
  int idx;
  int dmn_idx;
  long srd_prd=1L; /* [nbr] Product of strides */
  
  for(idx=0;idx<nbr_var;idx++){
    var[idx]->xrf->val.vp=var[idx]->val.vp=(void *)nco_malloc(var[idx]->sz*nco_typ_lng(var[idx]->type));
    if(var[idx]->nbr_dim == 0){
      nco_get_var1(in_id,var[idx]->id,var[idx]->srt,var[idx]->val.vp,var[idx]->type);
      nco_put_var1(out_id,var[idx]->xrf->id,var[idx]->xrf->srt,var[idx]->xrf->val.vp,var[idx]->type);
    }else{ /* end if variable is scalar */
      if(var[idx]->sz > 0){ /* Do nothing for zero-size record variables */

	/* Is stride > 1? */
	for(dmn_idx=0;dmn_idx<var[idx]->nbr_dim;dmn_idx++) srd_prd*=var[idx]->srd[dmn_idx];

	if(srd_prd == 1L){ 
	  nco_get_vara(in_id,var[idx]->id,var[idx]->srt,var[idx]->cnt,var[idx]->val.vp,var[idx]->type);
	  nco_put_vara(out_id,var[idx]->xrf->id,var[idx]->xrf->srt,var[idx]->xrf->cnt,var[idx]->xrf->val.vp,var[idx]->type);
	}else{
	  (void)nco_get_varm(in_id,var[idx]->id,var[idx]->srt,var[idx]->cnt,var[idx]->srd,(long *)NULL,var[idx]->val.vp,var[idx]->type);
	  (void)nco_put_varm(out_id,var[idx]->xrf->id,var[idx]->xrf->srt,var[idx]->xrf->cnt,var[idx]->xrf->srd,(long *)NULL,var[idx]->xrf->val.vp,var[idx]->type);
	} /* endif variable has non-unity stride */
      } /* end if var_sz */
    } /* end if variable is an array */
    var[idx]->val.vp=var[idx]->xrf->val.vp=nco_free(var[idx]->val.vp);
  } /* end loop over idx */
    
} /* end nco_var_val_cpy() */

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
  /* Make sure var_free() frees names when variable is destructed */
  var->nm=(char *)strdup(var_nm);
  var->id=var_id;
  var->nc_id=nc_id;

  /* Get type and number of dimensions and attributes for variable */
  (void)nco_inq_var(var->nc_id,var->id,(char *)NULL,&var->typ_dsk,&var->nbr_dim,(int *)NULL,&var->nbr_att);

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
  
  /* Type in memory begins as same type as on disk */
  var->type=var->typ_dsk; /* [enm] Type of variable in RAM */
  /* Type of packed data on disk */
  var->typ_pck=var->type;  /* [enm] Type of variable when packed (on disk). This should be same as typ_dsk except in cases where variable is packed in input file and unpacked in output file. */

  /* Refresh number of attributes and missing value attribute, if any */
  var->has_mss_val=nco_mss_val_get(var->nc_id,var);

  /* Check variable for duplicate dimensions */
  for(idx=0;idx<var->nbr_dim;idx++){
    for(dmn_idx=0;dmn_idx<var->nbr_dim;dmn_idx++){
      if(idx != dmn_idx){
	if(var->dmn_id[idx] == var->dmn_id[dmn_idx]){
	  /* Dimensions are duplicated when IDs for different ordinal dimensions are equal */
	  var->has_dpl_dmn=True;
	  break;
	} /* endif IDs are equal */
      } /* endif navel gazing */
    } /* endif inner dimension */
    /* Found a duplicate, so stop looking */
    if(dmn_idx != var->nbr_dim) break;
  } /* endif outer dimension */

  /* Size defaults to 1 in var_dfl_set(), and set to 1 here for extra safety */
  var->sz=1L; 
  for(idx=0;idx<var->nbr_dim;idx++){
    (void)nco_inq_dimname(nc_id,var->dmn_id[idx],dmn_nm);
    /* Search input dimension list for matching name */
    for(dmn_idx=0;dmn_idx<nbr_dim;dmn_idx++)
      if(!strcmp(dmn_nm,dim[dmn_idx]->nm)) break;

    if(dmn_idx == nbr_dim){
      (void)fprintf(stdout,"%s: ERROR dimension %s is not in list of dimensions available to nco_var_fll()\n",prg_nm_get(),dmn_nm);
      (void)fprintf(stdout,"%s: HINT This could be the problem identified in TODO #111. Workaround is to make sure each dimension in the weighting and masking variable(s) appears in a variable to be processed.\n",prg_nm_get());
      nco_exit(EXIT_FAILURE);
    } /* end if */

    /* fxm: hmb, what is this for? */
    /* Re-define dmn_id so that if dim is dimension list from output file
       then we get correct dmn_id. Should not affect normal running of 
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
     nco_pck_dsk_inq() fills in these portions harmlessly */
    (void)nco_pck_dsk_inq(nc_id,var);
  
  /* used when parsing script in ncap */
    var->undefined=False;
  return var;
} /* end nco_var_fll() */

void
nco_var_mtd_refresh /* [fnc] Update variable metadata (dmn_nbr, ID, mss_val, type) */
(const int nc_id, /* I [id] netCDF input-file ID */
 var_sct * const var) /* I/O [sct] Variable to update */
{
  /* Purpose: Update variable ID, number of dimensions, and missing_value attribute for given variable
     nco_var_mtd_refresh() is called in file loop in multi-file operators because each new file may have 
     different variable ID, type, and missing_value for same variable.
     This is necessary, for example, if computer model runs on one machine, e.g., SGI,
     and then run is restarted on another, e.g., Cray. 
     If internal floating point representations differ between these architecture, 
     e.g., UNICOS vs. IEEE, then missing_value representation may differ. 
     Variable IDs may change when some but not all model output files are 
     manipulated (i.e., variables added or deleted), followed by processing
     all files are processed in a batch.
     NCO is only known tool that makes this all user-transparent
     Thus this capability is very important to maintain
     fxm: why isn't variable type set here? */

  /* Refresh variable ID */
  var->nc_id=nc_id;

  /* 20050519: Not sure why I originally made next four lines SMP-critical
     20050629: Making next four lines multi-threaded causes no problems */
  (void)nco_inq_varid(var->nc_id,var->nm,&var->id);
    
  /* fxm: Not sure if/why necessary to refresh number of dimensions...though it should not hurt */
  /* Refresh number of dimensions in variable */
  (void)nco_inq_varndims(var->nc_id,var->id,&var->nbr_dim);
    
  /* Set variable type so following nco_mss_val_get() casts missing_value to correct type */
  (void)nco_inq_vartype(var->nc_id,var->id,&var->type);

  /* Refresh number of attributes and missing value attribute, if any */
  var->has_mss_val=nco_mss_val_get(var->nc_id,var);
  
#if 0
  /* PJR requested warning to be added when multiple file operators worked on 
     variables with missing_value since so many things could go wrong
     Now un-necessary since multi-file packing ostensibly works
     Leave code here in case we find it does not work */
  if(nco_is_rth_opr(prg_get()) && var->pck_dsk){
    if(var->has_mss_val) (void)fprintf(stdout,"%s: WARNING Variable \"%s\" is packed and has valid \"NCO_MSS_VAL_SNG\" attribute in multi-file arithmetic operator. Arithmetic on this variable will only be correct if...\n",prg_nm_get(),var_nm);
  } /* endif variable is packed */
#endif /* endif False */

} /* end nco_var_mtd_refresh() */

void
nco_var_srd_srt_set /* [fnc] Assign zero-start and unity-stride vectors to variables */
(var_sct ** const var, /* I [sct] Variables whose start and stride arrays to set */
 const int nbr_var) /* I [nbr] Number of structures in variable structure list */
{
  /* Purpose: Zero start (srt) and stride (srd) arrays of variable 
     This is useful for setting variables to the default output state, in
     which data is written in a block with no offset */

  int idx;
  int idx_dmn;

  for(idx=0;idx<nbr_var;idx++){
    for(idx_dmn=0;idx_dmn<var[idx]->nbr_dim;idx_dmn++){
      var[idx]->srt[idx_dmn]=0L;
      var[idx]->srd[idx_dmn]=1L;
    } /* end loop over dimensions */
  } /* end loop over variables */
  
} /* end nco_var_srd_srt_set() */


  
void
nco_var_dmn_refresh /* [fnc] Refresh var hyperslab info with var->dim[] info */
(var_sct ** const var, /* I [sct] Variables to refresh */
 const int nbr_var) /* I [nbr] Number of structures in variable structure list */
{
  int idx;
  int jdx;
  for(idx=0;idx<nbr_var;idx++){
    long sz;
    long sz_rec;
    sz=1;
    sz_rec=1;
    var_sct *var_tmp;
    var_tmp=var[idx];
    
    for(jdx=0 ; jdx<var_tmp->nbr_dim ; jdx++){
      var_tmp->srt[jdx]=var_tmp->dim[jdx]->srt; 
      var_tmp->end[jdx]=var_tmp->dim[jdx]->end;
      var_tmp->cnt[jdx]=var_tmp->dim[jdx]->cnt;
      var_tmp->srd[jdx]=var_tmp->dim[jdx]->srd;
      sz*=var_tmp->dim[jdx]->cnt;
      if(jdx >0) sz_rec*=var_tmp->dim[jdx]->cnt;
    }
     var_tmp->sz=sz; 
     var_tmp->sz_rec=sz_rec;
  } 

}  /* end nco_var_dmn_refresh() */
