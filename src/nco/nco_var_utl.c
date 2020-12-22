/* $Header$ */

/* Purpose: Variable utilities */

/* Copyright (C) 1995--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

#include "nco_var_utl.h" /* Variable utilities */

void
nco_cpy_var_val /* [fnc] Copy variable from input to output file, no limits */
(const int in_id, /* I [id] netCDF input file ID */
 const int out_id, /* I [id] netCDF output file ID */
 FILE * const fp_bnr, /* I [fl] Unformatted binary output file handle */
 const md5_sct * const md5, /* I [flg] MD5 Configuration */
 const char *var_nm, /* I [sng] Variable name */
 const trv_tbl_sct * const trv_tbl) /* I [sct] GTT (Group Traversal Table) */
{
  /* NB: nco_cpy_var_val() contains OpenMP critical region */
  /* Purpose: Copy single variable from input netCDF file to output netCDF file
     Routine does not account for user-specified limits, it just copies what it finds
     Routine copies variable-by-variable, old-style
     As of 2013, routine (only) used in USE_MM3_WORKAROUND copying in nco_xtr_wrt()
     As of 2015, routine (additionally) used for non-regridded variables in nco_rgr_wgt(), nco_ntp_vrt() */

  const char fnc_nm[]="nco_cpy_var_val()"; /* [sng] Function name */

  int *dmn_id;

  int dmn_nbr;
  int idx;
  int nbr_dmn_in;
  int nbr_dmn_out;
  int var_in_id;
  int var_out_id;

  long *dmn_cnt;
  long *dmn_sz;
  long *dmn_srt;

  long var_sz=1L;

  nc_type var_typ;

  void *void_ptr;

  /* Get var_id for requested variable from both files */
  (void)nco_inq_varid(in_id,var_nm,&var_in_id);
  (void)nco_inq_varid(out_id,var_nm,&var_out_id);

  /* Get type and number of dimensions for variable */
  (void)nco_inq_var(out_id,var_out_id,(char *)NULL,&var_typ,&nbr_dmn_out,(int *)NULL,(int *)NULL);
  (void)nco_inq_var(in_id,var_in_id,(char *)NULL,&var_typ,&nbr_dmn_in,(int *)NULL,(int *)NULL);
  if(nbr_dmn_out != nbr_dmn_in){
    (void)fprintf(stderr,"%s: ERROR attempt to write %d-dimensional input variable %s to %d-dimensional space in output file\nHINT: When using -A (append) option, all appended variables must be the same rank in the input file as in the output file. The ncwa operator is useful at ridding variables of extraneous (size = 1) dimensions. See how at http://nco.sf.net/nco.html#ncwa\nIf you wish to completely replace the existing output file definition and values of the variable %s by those in the input file, then first remove %s from the output file using, e.g., ncks -x -v %s. See more on subsetting at http://nco.sf.net/nco.html#sbs\n",nco_prg_nm_get(),nbr_dmn_in,var_nm,nbr_dmn_out,var_nm,var_nm,var_nm);
    nco_exit(EXIT_FAILURE);
  } /* endif */
  dmn_nbr=nbr_dmn_out;

  /* Allocate space to hold dimension IDs */
  dmn_cnt=(long *)nco_malloc(dmn_nbr*sizeof(long));
  dmn_id=(int *)nco_malloc(dmn_nbr*sizeof(int));
  dmn_sz=(long *)nco_malloc(dmn_nbr*sizeof(long));
  dmn_srt=(long *)nco_malloc(dmn_nbr*sizeof(long));

  /* Get dimension IDs from input file */
  (void)nco_inq_vardimid(in_id,var_in_id,dmn_id);

  /* Get dimension sizes from input file */
  for(idx=0;idx<dmn_nbr;idx++){
    /* nc_inq_dimlen() returns maximum value used so far in writing record dimension data
       Until record variable has been written, nc_inq_dimlen() returns dmn_sz=0 for record dimension in output file
       Thus we read input file for dimension sizes */
    (void)nco_inq_dimlen(in_id,dmn_id[idx],dmn_cnt+idx);

    /* Initialize indicial offset and stride arrays */
    dmn_srt[idx]=0L;
    var_sz*=dmn_cnt[idx];
  } /* end loop over dim */

  /* Allocate enough space to hold variable */
  void_ptr=(void *)nco_malloc_dbg(var_sz*nco_typ_lng(var_typ),"Unable to malloc() value buffer when copying hyperslab from input to output file",fnc_nm);

  /* 20150114: Keep PPC code in single block for easier reuse */
  int ppc=NC_MAX_INT; /* [nbr] Precision-preserving compression, i.e., number of total or decimal significant digits */
  nco_bool flg_ppc=False; /* [flg] Activate PPC with this variable and output file */
  nco_bool flg_nsd; /* [flg] PPC algorithm is NSD */
  var_sct var_out; /* [sct] Variable structure */
  if(True){
    /* This ugliness backs-out the ppc element from the traversal table for this variable */
    char *var_nm_fll;
    trv_sct *var_trv;
    var_nm_fll=nco_gid_var_nm_2_var_nm_fll(in_id,var_nm);
    // 20150115 fxm: hash table broken---use brute force
    var_trv=trv_tbl_var_nm_fll(var_nm_fll,trv_tbl);
    //(void)fprintf(stderr,"%s reports var_nm_fll = %s, var_trv->var_nm_fll = %s\n",fnc_nm,var_nm_fll,var_trv->nm_fll);
    assert(var_trv != NULL);
    if(var_trv) ppc=var_trv->ppc;
    if(var_trv) flg_nsd=var_trv->flg_nsd;
    if(var_nm_fll) var_nm_fll=(char *)nco_free(var_nm_fll);
    if(ppc != NC_MAX_INT){
      /* Initialize variable structure with minimal information for nco_mss_val_get() */
      flg_ppc=True;
      var_out.nm=(char *)strdup(var_nm);
      var_out.type=var_typ;
      var_out.id=var_out_id;
      var_out.sz=var_sz;
      var_out.has_mss_val=False;
      var_out.val.vp=void_ptr;
      nco_mss_val_get(out_id,&var_out);
      if(var_out.nm) var_out.nm=(char *)nco_free(var_out.nm);
    } /* endif ppc */
  } /* endif True */

  nco_bool flg_xcp; /* [flg] Variable requires exception processing */
  flg_xcp=nco_is_xcp(var_nm);

  /* Get variable */
  if(dmn_nbr == 0){
    nco_get_var1(in_id,var_in_id,0L,void_ptr,var_typ);
    if(flg_ppc){
      if(flg_nsd) (void)nco_ppc_bitmask(ppc,var_out.type,var_out.sz,var_out.has_mss_val,var_out.mss_val,var_out.val); else (void)nco_ppc_around(ppc,var_out.type,var_out.sz,var_out.has_mss_val,var_out.mss_val,var_out.val);
    } /* !PPC */
    nco_put_var1(out_id,var_out_id,0L,void_ptr,var_typ);
  }else{ /* end if variable is scalar */
    if(var_sz > 0){ /* Allow for zero-size record variables */
      nco_get_vara(in_id,var_in_id,dmn_srt,dmn_cnt,void_ptr,var_typ);
      if(flg_ppc){
	if(flg_nsd) (void)nco_ppc_bitmask(ppc,var_out.type,var_out.sz,var_out.has_mss_val,var_out.mss_val,var_out.val); else (void)nco_ppc_around(ppc,var_out.type,var_out.sz,var_out.has_mss_val,var_out.mss_val,var_out.val);
      } /* !PPC */
      if(flg_xcp) nco_xcp_prc(var_nm,var_typ,var_sz,(char *)void_ptr);
      nco_put_vara(out_id,var_out_id,dmn_srt,dmn_cnt,void_ptr,var_typ);
    } /* end if var_sz */
  } /* end if variable is an array */
  /* Perform MD5 digest of input and output data if requested */
  if(md5) (void)nco_md5_chk(md5,var_nm,var_sz*nco_typ_lng(var_typ),out_id,dmn_srt,dmn_cnt,void_ptr);
  /* Write unformatted binary data */
  if(fp_bnr) nco_bnr_wrt(fp_bnr,var_nm,var_sz,var_typ,void_ptr);

  /* 20111130 Fixes TODO nco1029: Warn on ncks -A when dim(old_record) != dim(new_record) */
  if(dmn_nbr > 0){
    int rec_dmn_id=NCO_REC_DMN_UNDEFINED; /* [id] Record dimension ID in input file */
    int rcd=NC_NOERR; /* [rcd] Return code */
    long rec_dmn_sz=0L; /* [nbr] Record dimension size in output file */
    rcd+=nco_inq_unlimdim(in_id,&rec_dmn_id); 
    /* If input file has record dimension ... */
    if(rec_dmn_id != NCO_REC_DMN_UNDEFINED){
      /* ... used as record dimension of this variable...  */
      if(rec_dmn_id == dmn_id[0]){
        rcd+=nco_inq_unlimdim(out_id,&rec_dmn_id); 
        /* ... and if output file has record dimension ... */
        if(rec_dmn_id != NCO_REC_DMN_UNDEFINED){
          (void)nco_inq_dimlen(out_id,rec_dmn_id,&rec_dmn_sz);
          /* ... and record dimension size in output file is non-zero (meaning at least one record has been written) ... */
          if(rec_dmn_sz > 0L){
            /* ... then check input vs. output record dimension sizes ... */
            if(rec_dmn_sz != dmn_cnt[0]){
              (void)fprintf(stderr,"%s: WARNING record dimension size of %s changes between input and output files from %ld to %ld. This is expected only when user manually changes record dimensions. Otherwise, output variable %s may be corrupt.\n",nco_prg_nm_get(),var_nm,dmn_cnt[0],rec_dmn_sz,var_nm);
            } /* endif sizes are incommensurate */
          } /* endif records exist in output file */
        } /* endif output file has record dimension */
      } /* endif this variable uses input file record dimension */
    } /* endif input file has record dimension */
  } /* endif this variable is not a scalar */

  /* Free space that held dimension IDs */
  dmn_cnt=(long *)nco_free(dmn_cnt);
  dmn_id=(int *)nco_free(dmn_id);
  dmn_sz=(long *)nco_free(dmn_sz);
  dmn_srt=(long *)nco_free(dmn_srt);

  /* Free space that held variable */
  void_ptr=nco_free(void_ptr);

} /* end nco_cpy_var_val() */

nco_bool /* O [flg] Faster copy on Multi-record Multi-variable netCDF3 files */
nco_use_mm3_workaround /* [fnc] Use faster copy on Multi-record Multi-variable netCDF3 files? */
(const int in_id, /* I [id] Input file ID */
 const int fl_out_fmt) /* I [enm] Output file format */
{
  /* Purpose: Determine whether to use copy algorithm designed to speed writes on 
     netCDF3 files containing multiple record variables.
     In such cases massive slowdowns are common on Multi-record Multi-variable netCDF3 files
     Also the problem can occur with normal (4096 B) Blocksize Filesystems
     Based on Russ Rew's code in nccopy.c 20120306

     Testing:
     ncks -O -C -D 4 -v one,two,one_dmn_rec_var,two_dmn_rec_var ~/nco/data/in.nc ~/foo.nc

     20120307: NCO Open Discussion Forum exchanges with Russ Rew:
     "When accessing data from netCDF classic or 64-bit offset format files
     that have multiple record variables and a lot of records on a file
     system with large disk block size relative to a record's worth of data
     for one or more record variables, access the data a record at a time
     instead of a variable at a time."

     20120312:
     "Hi Russ,
     
     I'm tidying up that patch in NCO, and have some further questions.
     In order to prioritize patching more of NCO, I want to know how the
     slowdown reading compares to the slowdown writing.
     To simplify my questions, let's use the abbreviations MM3 and
     MM4 for netCDF3 and netCDF4 Multi-record Multi-variable files,
     respectively. My understanding is that MM3s are susceptible to the  
     slowdown, while MM4s are not, and that writing MM3s without the
     patch incurs incurs more of a penalty than reading MM3s.
     So this is how I prioritize implementing the MM3 patch:
     
     1. When copying MM3 to MM3. Done in ncks 4.1.0, TBD in others.
     2. When copying MM4 to MM3. Done in ncks 4.1.0, TBD in others.
     3. When copying MM3 to MM4. Done in ncks 4.2.6, TBD in others.
     4. When reading MM3 and not writing anything. Not done anywhere.
     
     Currently ncks always uses the algorithm for cases 1 and 2 (i.e.,
     whenever writing to an MM3), but not for cases 3 and 4.
     
     The rest of NCO does not yet use the MM3 algorithm, yet there are many 
     places where it would potentially benefit. I've heard through the
     years that sometimes ncecat slows to a crawl. Perhaps the MM3 slowdown
     is responsible. On the bright side, ncra and ncrcat are immune from
     the slowdown because they already read/write all record variables 
     record-by-record. 
     
     Does the prioritization above make sense? If so I will next patch
     the rest of NCO to do cases 1 and 2, before patching anything to do
     cases 3 and 4."
     
     20120315:
     "Hi Charlie,
     That sounds right to me, because when you're just reading a small
     portion of a disk block, you only incur the extra time for reading
     data you won't use., but when you're writing, you have to read it all
     in and rewrite the part you're changing as well as the data you didn't
     change.  So writing with large disk blocks would seem to require twice
     the I/O of reading.
     
     In nccopy, I just implemented the algorithm in cases 1 and 3; case 4
     doesn't occur.  I had thought case 2 currently wasn't very common, so
     it could wait, but your question has led me to rethink this.  A fairly
     common use of case 2 is converting a compressed netCDF-4 classic model
     file to an uncompressed classic file, for use with applications that
     haven't been linked to a netCDF-4 library or in archives that will
     continue to use classic format.
     
     I've been trying to figure out whether implementing case 2 for
     compressed input could require significantly more chunk cache than not
     using the MM3 algorithm, if you want to avoid uncompressing the same
     data over and over again.  But I think just having enough chunk cache
     to hold the largest compressed chunk for any record variable would be
     sufficient, so I've tentatively concluded that it's not an issue.
     (Where things get complicated is copying MM4 to MM4 while rechunking,
     to improve access times for read access patterns that don't match the
     way the data was written.)
     
     Thanks for presenting your prioritization.  It looks like I've got
     some more work to do, implementing case 2 in nccopy." */

  int dmn_nbr;
  int fl_in_fmt; /* [enm] Input file format */
  int idx;
  int rec_dmn_id=NCO_REC_DMN_UNDEFINED;
  int rcd=NC_NOERR; /* [rcd] Return code */
  int rec_var_nbr=0; /* [nbr] Number of record variables */
  int var_nbr=0; /* [nbr] Number of variables */

  int *dmn_id;

  nco_bool USE_MM3_WORKAROUND=False; /* [flg] Faster copy on Multi-record Multi-variable netCDF3 files */

  (void)nco_inq_format(in_id,&fl_in_fmt);

  /* No advantage to workaround unless reading from or writing to netCDF3 file */
  if(
    (fl_out_fmt == NC_FORMAT_CLASSIC || fl_out_fmt == NC_FORMAT_64BIT) || /* Cases 1 & 2 above, i.e., MM3->MM3 & MM4->MM3 */
    /* 20150515: Investigating an unusually sluggish response to compressing one of Gary Strand's netCDF3 files,
       I learned that the MM3 workaround in combination with odd chunk sizes (like map=rew produces), 
       explain most or all of the sluggishness. 
       Executive decision: turn-off MM3 workaround when in Case 3 above (MM3->MM4 copying)
       This speeds-up compression-induced chunking for users converting large netCDF3 files to netCDF4 
       To restore MM3 workaround for Case 3, uncomment next two lines */
    //    ((fl_in_fmt == NC_FORMAT_CLASSIC || fl_in_fmt == NC_FORMAT_64BIT) && /* Case 3 above, i.e., MM3->MM4 */
    //     (fl_out_fmt == NC_FORMAT_NETCDF4 || fl_out_fmt == NC_FORMAT_NETCDF4_CLASSIC)) ||
    False)
  {
    /* Subsequently, assume output is netCDF3 or classic-compatible netCDF4
       If file contains record dimension (and netCDF3 files can have only one record dimension)
       NB: fxm Following check only detects cases where MM3 conditions exist root group (not sub-groups)
       Copying MM3-worthy subgroup from netCDF4 file to netCDF3 flat file produces false-negative */
    rcd=nco_inq_unlimdim(in_id,&rec_dmn_id);
    if(rec_dmn_id != NCO_REC_DMN_UNDEFINED){
      /* Slowdown only occurs in files with more than one record variable */
      rcd+=nco_inq_nvars(in_id,&var_nbr);
      if(var_nbr > 0){
        for(idx=0;idx<var_nbr;idx++){
          rcd+=nco_inq_varndims(in_id,idx,&dmn_nbr);
          if(dmn_nbr > 0){
            dmn_id=(int *)nco_malloc(dmn_nbr*sizeof(int));
            rcd+=nco_inq_vardimid(in_id,idx,dmn_id);
            /* netCDF3 requires record dimension to be first dimension */
            if(dmn_id[0] == rec_dmn_id){
              rec_var_nbr++;
              if(rec_var_nbr > 1) USE_MM3_WORKAROUND=True;
            } /* endif record dimnesion */
            if(dmn_id) dmn_id=(int*)nco_free(dmn_id);
          } /* endif dmn_nbr > 0 */
          if(USE_MM3_WORKAROUND) break;
        } /* end loop over variables */
      } /* endif var_nbr > 0 */
    } /* endif file contains record dimnsion */
  } /* endif file is netCDF3 */

  return USE_MM3_WORKAROUND;
} /* end nco_use_mm3_workaround() */

void
nco_cpy_rec_var_val /* [fnc] Copy all record variables, record-by-record, from input to output file, no limits */
(const int in_id, /* I [id] netCDF input file ID */
 FILE * const fp_bnr, /* I [fl] Unformatted binary output file handle */
 const md5_sct * const md5, /* I [flg] MD5 Configuration */
 CST_X_PTR_CST_PTR_CST_Y(nm_id_sct,var_lst), /* I [sct] Record variables to be extracted */
 const int var_nbr, /* I [nbr] Number of record variables */
 const trv_tbl_sct * const trv_tbl) /* I [sct] GTT (Group Traversal Table) */
{
  /* Purpose: Copy all record variables from input netCDF file to output netCDF file
     Routine does not account for user-specified limits, it just copies what it finds
     Routine copies record-by-record, for all variables, old-style, called only by ncks
     Used only by MM3 workaround in nco_xtr_wrt() and therefore routine assumes:
     1. Input file is netCDF3
     2. All variables in var_lst are record variables
     NB: Rationale for MM3 workaround is kept in header to routine nco_use_mm3_workaround() */

  const char fnc_nm[]="nco_cpy_rec_var_val()"; /* [sng] Function name */

  int *dmn_id;

  int dmn_idx;
  int dmn_nbr;
  int fl_fmt; /* [enm] Output file format */
  int nbr_dmn_in;
  int nbr_dmn_out;
  int rec_dmn_id;
  int rec_dmn_out_id;
  int rcd=NC_NOERR; /* [rcd] Return code */
  int var_idx;
  int var_in_id;
  int var_out_id;

  long *dmn_cnt;
  long *dmn_sz;
  long *dmn_srt;

  long rec_idx;
  long rec_sz; /* [nbr] Size of record-dimension in input file */
  long rec_out_sz; /* [nbr] Size of record-dimension in output file */
  long var_sz=1L;

  nc_type var_typ;

  void *void_ptr;

  /* Assume file contains record dimension (and netCDF3 files can have only one record dimension) */
  rcd+=nco_inq_unlimdim(in_id,&rec_dmn_id);
  assert(rec_dmn_id != NCO_REC_DMN_UNDEFINED);
  rcd+=nco_inq_dimlen(in_id,rec_dmn_id,&rec_sz);

  /* File format needed to enable netCDF4 features */
  if(var_nbr > 0) (void)nco_inq_format(var_lst[0]->grp_id_out,&fl_fmt);

  for(rec_idx=0;rec_idx<rec_sz;rec_idx++){
    for(var_idx=0;var_idx<var_nbr;var_idx++){
      /* Re-initialize accumulated variables */
      var_sz=1L;
      /* Mimic standard code path debugging information */
      if(nco_dbg_lvl_get() >= nco_dbg_var && !fp_bnr && rec_idx == 0) (void)fprintf(stderr,"%s, ",var_lst[var_idx]->nm);
      if(nco_dbg_lvl_get() >= nco_dbg_var && rec_idx == 0) (void)fflush(stderr);
      if(nco_dbg_lvl_get() >= nco_dbg_crr) (void)fprintf(stderr,".");

      /* Get ID of requested variable from both files */
      (void)nco_inq_varid(var_lst[var_idx]->grp_id_in,var_lst[var_idx]->nm,&var_in_id);
      (void)nco_inq_varid(var_lst[var_idx]->grp_id_out,var_lst[var_idx]->nm,&var_out_id);
      (void)nco_inq_var(var_lst[var_idx]->grp_id_out,var_out_id,(char *)NULL,&var_typ,&nbr_dmn_out,(int *)NULL,(int *)NULL);
      (void)nco_inq_var(var_lst[var_idx]->grp_id_in,var_in_id,(char *)NULL,&var_typ,&nbr_dmn_in,(int *)NULL,(int *)NULL);
      if(nbr_dmn_out != nbr_dmn_in){
        (void)fprintf(stderr,"%s: ERROR attempt to write %d-dimensional input variable %s to %d-dimensional space in output file\nHINT: When using -A (append) option, all appended variables must be the same rank in the input file as in the output file. The ncwa operator is useful at ridding variables of extraneous (size = 1) dimensions. See how at http://nco.sf.net/nco.html#ncwa\nIf you wish to completely replace the existing output file definition and values of the variable %s by those in the input file, then first remove %s from the output file using, e.g., ncks -x -v %s. See more on subsetting at http://nco.sf.net/nco.html#sbs\n",nco_prg_nm_get(),nbr_dmn_in,var_lst[var_idx]->nm,nbr_dmn_out,var_lst[var_idx]->nm,var_lst[var_idx]->nm,var_lst[var_idx]->nm);
        nco_exit(EXIT_FAILURE);
      } /* endif */
      dmn_nbr=nbr_dmn_out;

      /* Allocate space to hold dimension IDs */
      dmn_cnt=(long *)nco_malloc(dmn_nbr*sizeof(long));
      dmn_id=(int *)nco_malloc(dmn_nbr*sizeof(int));
      dmn_sz=(long *)nco_malloc(dmn_nbr*sizeof(long));
      dmn_srt=(long *)nco_malloc(dmn_nbr*sizeof(long));

      /* Get dimension IDs from input file */
      (void)nco_inq_vardimid(var_lst[var_idx]->grp_id_in,var_in_id,dmn_id);

      /* Get non-record dimension sizes from input file */
      for(dmn_idx=1;dmn_idx<dmn_nbr;dmn_idx++){
        (void)nco_inq_dimlen(var_lst[var_idx]->grp_id_in,dmn_id[dmn_idx],dmn_cnt+dmn_idx);
        /* Initialize indicial offset and stride arrays */
        dmn_srt[dmn_idx]=0L;
        var_sz*=dmn_cnt[dmn_idx];
      } /* end loop over dim */
      /* Configure hyperslab access for current record */
      dmn_id[0]=rec_dmn_id;
      dmn_cnt[0]=1L;
      dmn_srt[0]=rec_idx;

      /* Allocate enough space to hold one record of this variable */
      void_ptr=(void *)nco_malloc_dbg(var_sz*nco_typ_lng(var_typ),"Unable to malloc() value buffer when copying hyperslab from input to output file",fnc_nm);

      /* 20150114: Keep PPC code in single block for easier reuse */
      int ppc=NC_MAX_INT; /* [nbr] Precision-preserving compression, i.e., number of total or decimal significant digits */
      nco_bool flg_ppc=False; /* [sct] Activate PPC with this variable and output file */
      nco_bool flg_nsd; /* [flg] PPC algorithm is NSD */
      var_sct var_out; /* [sct] Variable structure */
      if(True){
	/* This ugliness backs-out the ppc element from the traversal table for this variable */
	char *var_nm_fll;
	trv_sct *var_trv=NULL;
	var_nm_fll=nco_gid_var_nm_2_var_nm_fll(var_lst[var_idx]->grp_id_in,var_lst[var_idx]->nm);
	// 20150115 fxm: hash table broken---use brute force
	var_trv=trv_tbl_var_nm_fll(var_nm_fll,trv_tbl);
	//(void)fprintf(stderr,"%s reports var_nm_fll = %s, var_trv->var_nm_fll = %s\n",fnc_nm,var_nm_fll,var_trv->nm_fll);
	assert(var_trv != NULL);
	if(var_trv) ppc=var_trv->ppc;
	if(var_trv) flg_nsd=var_trv->flg_nsd;
	if(var_nm_fll) var_nm_fll=(char *)nco_free(var_nm_fll);
	if(ppc != NC_MAX_INT){
	  /* Initialize variable structure with minimal information for nco_mss_val_get() */
	  flg_ppc=True;
	  var_out.nm=(char *)strdup(var_lst[var_idx]->nm);
	  var_out.type=var_typ;
	  var_out.id=var_out_id;
	  var_out.sz=var_sz;
	  var_out.has_mss_val=False;
	  var_out.val.vp=void_ptr;
	  nco_mss_val_get(var_lst[var_idx]->grp_id_out,&var_out);
	  if(var_out.nm) var_out.nm=(char *)nco_free(var_out.nm);
	} /* endif ppc */
      } /* endif True */
      
      /* Get and put one record of variable */
      if(var_sz > 0){ /* Allow for zero-size record variables */
        nco_get_vara(var_lst[var_idx]->grp_id_in,var_in_id,dmn_srt,dmn_cnt,void_ptr,var_typ);
	if(flg_ppc){
	  if(flg_nsd) (void)nco_ppc_bitmask(ppc,var_out.type,var_out.sz,var_out.has_mss_val,var_out.mss_val,var_out.val); else (void)nco_ppc_around(ppc,var_out.type,var_out.sz,var_out.has_mss_val,var_out.mss_val,var_out.val);
	} /* !PPC */
	if(nco_is_xcp(var_lst[var_idx]->nm)) nco_xcp_prc(var_lst[var_idx]->nm,var_typ,var_sz,(char *)void_ptr);
        nco_put_vara(var_lst[var_idx]->grp_id_out,var_out_id,dmn_srt,dmn_cnt,void_ptr,var_typ);
      } /* end if var_sz */

      /* 20111130 TODO nco1029 warn on ncks -A when dim(old_record) != dim(new_record)
	 One check of this condition, per variable, is enough
	 In regular (non-MM3 workaround) case, we check this condition after reading/writing whole variable
	 In MM3 workaround-case, check condition after writing last record
	 20130127: fxm bug here when user eliminates record variables using --fix_rec_dmn
	 In that case output netCDF3 file does not have record variable so nco_inq_unlimdim() and nco_inq_dimlen() fail
	 Since following code is purely diagnostic, assume that these failures are due to using --fix_rec_dmn 
	 And therefore, well, ignore them :) */
      if(rec_idx == rec_sz-1L){ 
        rcd=nco_inq_unlimdim(var_lst[var_idx]->grp_id_out,&rec_dmn_out_id); 
        if(rec_dmn_out_id != NCO_REC_DMN_UNDEFINED){
          /* ... and if output file has record dimension ... */
          (void)nco_inq_dimlen(var_lst[var_idx]->grp_id_out,rec_dmn_out_id,&rec_out_sz);
          /* ... and record dimension size in output file is non-zero (meaning at least one record has been written) ... */
          if(rec_out_sz > 0L){
            /* ... then check input vs. output record dimension sizes ... */
            if(rec_sz != rec_out_sz){
              (void)fprintf(stderr,"%s: WARNING record dimension size of %s changes between input and output files from %ld to %ld. This is expected only when user manually changes record dimensions. Otherwise, output variable %s may be corrupt.\n",nco_prg_nm_get(),var_lst[var_idx]->nm,rec_sz,rec_out_sz,var_lst[var_idx]->nm);
            } /* endif sizes are incommensurate */
          } /* endif records have already been written to output file */
        } /* endif record dimension exists in output file */
      } /* endif last record in variable in input file */

      /* Free space that held dimension IDs */
      dmn_cnt=(long *)nco_free(dmn_cnt);
      dmn_id=(int *)nco_free(dmn_id);
      dmn_sz=(long *)nco_free(dmn_sz);
      dmn_srt=(long *)nco_free(dmn_srt);

      /* Free space that held variable */
      void_ptr=nco_free(void_ptr);

    } /* end loop over variables */
  } /* end loop over records */

  /* Corner cases require a loop over variables but not records */
  if(md5 || fp_bnr){
    for(var_idx=0;var_idx<var_nbr;var_idx++){
      /* Re-initialize accumulated variables */
      var_sz=1L;
      /* Get ID of requested variable from both files */
      (void)nco_inq_varid(var_lst[var_idx]->grp_id_in,var_lst[var_idx]->nm,&var_in_id);
      (void)nco_inq_var(var_lst[var_idx]->grp_id_in,var_in_id,(char *)NULL,&var_typ,&dmn_nbr,(int *)NULL,(int *)NULL);
      /* Allocate space to hold dimension IDs */
      dmn_cnt=(long *)nco_malloc(dmn_nbr*sizeof(long));
      dmn_id=(int *)nco_malloc(dmn_nbr*sizeof(int));
      dmn_sz=(long *)nco_malloc(dmn_nbr*sizeof(long));
      dmn_srt=(long *)nco_malloc(dmn_nbr*sizeof(long));
      /* Get dimension IDs from input file */
      (void)nco_inq_vardimid(var_lst[var_idx]->grp_id_in,var_in_id,dmn_id);
      /* Get dimension sizes from input file */
      for(dmn_idx=0;dmn_idx<dmn_nbr;dmn_idx++){
        (void)nco_inq_dimlen(var_lst[var_idx]->grp_id_in,dmn_id[dmn_idx],dmn_cnt+dmn_idx);
        /* Initialize indicial offset and stride arrays */
        dmn_srt[dmn_idx]=0L;
        var_sz*=dmn_cnt[dmn_idx];
      } /* end loop over dim */
      /* Allocate enough space to hold this entire variable */
      void_ptr=(void *)nco_malloc_dbg(var_sz*nco_typ_lng(var_typ),"Unable to malloc() value buffer when doing MD5 or binary write on variable",fnc_nm);
      /* Get variable */
      if(var_sz > 0) nco_get_vara(var_lst[var_idx]->grp_id_in,var_in_id,dmn_srt,dmn_cnt,void_ptr,var_typ);
      /* Perform MD5 digest of input and output data if requested */
      if(md5) (void)nco_md5_chk(md5,var_lst[var_idx]->nm,var_sz*nco_typ_lng(var_typ),var_lst[var_idx]->grp_id_out,dmn_srt,dmn_cnt,void_ptr);
      /* Write unformatted binary data */
      if(fp_bnr) nco_bnr_wrt(fp_bnr,var_lst[var_idx]->nm,var_sz,var_typ,void_ptr);
      /* Free space that held dimension IDs */
      dmn_cnt=(long *)nco_free(dmn_cnt);
      dmn_id=(int *)nco_free(dmn_id);
      dmn_sz=(long *)nco_free(dmn_sz);
      dmn_srt=(long *)nco_free(dmn_srt);
      /* Free space that held variable */
      void_ptr=nco_free(void_ptr);
    } /* end loop over variables */
  } /* end if */
    
} /* end nco_cpy_rec_var_val() */

void
nco_cpy_var_val_lmt /* [fnc] Copy variable data from input to output file, simple hyperslabs */
(const int in_id, /* I [id] netCDF input file ID */
 const int out_id, /* I [id] netCDF output file ID */
 FILE * const fp_bnr, /* I [fl] Unformatted binary output file handle */
 char *var_nm, /* I [sng] Variable name */
 const lmt_sct * const lmt, /* I [sct] Hyperslab limits */
 const int lmt_nbr) /* I [nbr] Number of hyperslab limits */
{
  /* NB: nco_cpy_var_val_lmt() contains OpenMP critical region */
  /* Purpose: Copy variable data from input netCDF file to output netCDF file 
     Truncate dimensions in variable definition in output file according to user-specified limits
     Copy variable-by-variable, old-style, 
     Routine was used by ncks until MSA implementation in ~2005 
     Functionality now extended and superceded by nco_cpy_var_val_mlt_lmt() */

  nco_bool SRD=False;
  nco_bool WRP=False;

  const char fnc_nm[]="nco_cpy_var_val_lmt()"; /* [sng] Function name */

  int *dmn_id;

  int dmn_idx;
  int dmn_nbr;
  int lmt_idx;
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

  nc_type var_typ;

  void *void_ptr;

  /* Get var_id for requested variable from both files */
  nco_inq_varid(in_id,var_nm,&var_in_id);
  nco_inq_varid(out_id,var_nm,&var_out_id);

  /* Get type and number of dimensions for variable */
  (void)nco_inq_var(out_id,var_out_id,(char *)NULL,&var_typ,&nbr_dmn_out,(int *)NULL,(int *)NULL);
  (void)nco_inq_var(in_id,var_in_id,(char *)NULL,&var_typ,&nbr_dmn_in,(int *)NULL,(int *)NULL);
  if(nbr_dmn_out != nbr_dmn_in){
    (void)fprintf(stderr,"%s: ERROR attempt to write %d-dimensional input variable %s to %d-dimensional space in output file\nHINT: When using -A (append) option, all appended variables must be the same rank in the input file as in the output file. The ncwa operator is useful at ridding variables of extraneous (size = 1) dimensions. See how at http://nco.sf.net/nco.html#ncwa\nIf you wish to completely replace the existing output file definition and values of the variable %s by those in the input file, then first remove %s from the output file using, e.g., ncks -x -v %s. See more on subsetting at http://nco.sf.net/nco.html#sbs\n",nco_prg_nm_get(),nbr_dmn_in,var_nm,nbr_dmn_out,var_nm,var_nm,var_nm);
    nco_exit(EXIT_FAILURE);
  } /* endif */
  dmn_nbr=nbr_dmn_out;

  /* Allocate space to hold dimension IDs */
  dmn_cnt=(long *)nco_malloc(dmn_nbr*sizeof(long));
  dmn_id=(int *)nco_malloc(dmn_nbr*sizeof(int));
  dmn_in_srt=(long *)nco_malloc(dmn_nbr*sizeof(long));
  dmn_map=(long *)nco_malloc(dmn_nbr*sizeof(long));
  dmn_out_srt=(long *)nco_malloc(dmn_nbr*sizeof(long));
  dmn_srd=(long *)nco_malloc(dmn_nbr*sizeof(long));
  dmn_sz=(long *)nco_malloc(dmn_nbr*sizeof(long));

  /* Get dimension IDs from input file */
  (void)nco_inq_vardimid(in_id,var_in_id,dmn_id);

  /* Get dimension sizes from input file */
  for(dmn_idx=0;dmn_idx<dmn_nbr;dmn_idx++){
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

    /* Decide whether this dimension has user-specified limits */
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
  void_ptr=(void *)nco_malloc_dbg(var_sz*nco_typ_lng(var_typ),"Unable to malloc() value buffer when copying hyperslab from input to output file",fnc_nm);

  /* Copy variable */
  if(dmn_nbr == 0){ /* Copy scalar */
    nco_get_var1(in_id,var_in_id,0L,void_ptr,var_typ);
    nco_put_var1(out_id,var_out_id,0L,void_ptr,var_typ);
    if(fp_bnr) nco_bnr_wrt(fp_bnr,var_nm,var_sz,var_typ,void_ptr);
  }else if(!WRP){ /* Copy contiguous array */
    if(!SRD) nco_get_vara(in_id,var_in_id,dmn_in_srt,dmn_cnt,void_ptr,var_typ); else nco_get_vars(in_id,var_in_id,dmn_in_srt,dmn_cnt,dmn_srd,void_ptr,var_typ);
    nco_put_vara(out_id,var_out_id,dmn_out_srt,dmn_cnt,void_ptr,var_typ);
    if(fp_bnr) nco_bnr_wrt(fp_bnr,var_nm,var_sz,var_typ,void_ptr);
  }else if(WRP){ /* Copy wrapped array */
    /* For wrapped data */
    long *dmn_in_srt_1=NULL;
    long *dmn_in_srt_2=NULL;
    long *dmn_out_srt_1=NULL;
    long *dmn_out_srt_2=NULL;
    long *dmn_cnt_1=NULL;
    long *dmn_cnt_2=NULL;

    dmn_in_srt_1=(long *)nco_malloc(dmn_nbr*sizeof(long));
    dmn_in_srt_2=(long *)nco_malloc(dmn_nbr*sizeof(long));
    dmn_out_srt_1=(long *)nco_malloc(dmn_nbr*sizeof(long));
    dmn_out_srt_2=(long *)nco_malloc(dmn_nbr*sizeof(long));
    dmn_cnt_1=(long *)nco_malloc(dmn_nbr*sizeof(long));
    dmn_cnt_2=(long *)nco_malloc(dmn_nbr*sizeof(long));

    /* Variable contains a wrapped dimension, requires two reads */
    /* For each dimension in the input variable */
    for(dmn_idx=0;dmn_idx<dmn_nbr;dmn_idx++){

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
              long last_good_idx_1st_hyp_slb; /* C-index of last valid member of 1st hyperslab (= srt+m*srd) */
              long left_over_idx_1st_hyp_slb; /* # elements from first hyperslab to count towards current stride */
              /* long first_good_idx_2nd_hyp_slb; *//* C-index of first valid member of 2nd hyperslab, if any */

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

    if(nco_dbg_lvl_get() >= 5){
      (void)fprintf(stderr,"\nvar = %s\n",var_nm);
      (void)fprintf(stderr,"dim\tcnt\tsrtin1\tcnt1\tsrtout1\tsrtin2\tcnt2\tsrtout2\n");
      for(dmn_idx=0;dmn_idx<dmn_nbr;dmn_idx++) (void)fprintf(stderr,"%d\t%ld\t%ld\t%ld\t%ld\t%ld\t%ld\t%ld\t\n",dmn_idx,dmn_cnt[dmn_idx],dmn_in_srt_1[dmn_idx],dmn_cnt_1[dmn_idx],dmn_out_srt_1[dmn_idx],dmn_in_srt_2[dmn_idx],dmn_cnt_2[dmn_idx],dmn_out_srt_2[dmn_idx]);
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

      if(dmn_nbr == 1){
        char dmn_nm[NC_MAX_NAME];

        (void)nco_inq_dimname(in_id,dmn_id[0],dmn_nm);
        if(!strcmp(dmn_nm,var_nm)) CRD=True; else CRD=False;
      } /* end if */      

      if(CRD && MNT){ /* If requested, apply monotonicity filter to wrapped coordinate */
        (void)nco_get_vara(in_id,var_in_id,dmn_in_srt_1,dmn_cnt_1,void_ptr,var_typ);
        /* Convert coordinate to double */
        for(idx=0;idx<var_sz;idx++){
          switch(var_typ){
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
      (void)nco_get_vara(in_id,var_in_id,dmn_in_srt_1,dmn_cnt_1,void_ptr,var_typ);
      (void)nco_put_vara(out_id,var_out_id,dmn_out_srt_1,dmn_cnt_1,void_ptr,var_typ);
      if(fp_bnr) nco_bnr_wrt(fp_bnr,var_nm,var_sz,var_typ,void_ptr);
      (void)nco_get_vara(in_id,var_in_id,dmn_in_srt_2,dmn_cnt_2,void_ptr,var_typ);
      (void)nco_put_vara(out_id,var_out_id,dmn_out_srt_2,dmn_cnt_2,void_ptr,var_typ);
      if(fp_bnr) nco_bnr_wrt(fp_bnr,var_nm,var_sz,var_typ,void_ptr);
    }else{ /* SRD */
      (void)nco_get_vars(in_id,var_in_id,dmn_in_srt_1,dmn_cnt_1,dmn_srd,void_ptr,var_typ);
      (void)nco_put_vara(out_id,var_out_id,dmn_out_srt_1,dmn_cnt_1,void_ptr,var_typ);
      if(fp_bnr) nco_bnr_wrt(fp_bnr,var_nm,var_sz,var_typ,void_ptr);
      (void)nco_get_vars(in_id,var_in_id,dmn_in_srt_2,dmn_cnt_2,dmn_srd,void_ptr,var_typ);
      (void)nco_put_vara(out_id,var_out_id,dmn_out_srt_2,dmn_cnt_2,void_ptr,var_typ);
      if(fp_bnr) nco_bnr_wrt(fp_bnr,var_nm,var_sz,var_typ,void_ptr);
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
  
  /* Make sure nco_var_free() frees names when variable is destructed */
  if(var->nm) var_cpy->nm=(char *)strdup(var->nm);
  if(var->nm_fll) var_cpy->nm_fll=(char *)strdup(var->nm_fll);
  
  /* Deep-copy dynamically allocated arrays from original to copy */
  if(var->val.vp){
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
  if(var->mss_val.vp){
    var_cpy->mss_val.vp=(void *)nco_malloc(nco_typ_lng(var_cpy->type));
    (void)memcpy((void *)(var_cpy->mss_val.vp),(void *)(var->mss_val.vp),nco_typ_lng(var_cpy->type));
  } /* end if */
  if(var->tally){
    var_cpy->tally=(long *)nco_malloc_dbg(var_cpy->sz*sizeof(long),"Unable to malloc() tally buffer in variable deep-copy",fnc_nm);
    (void)memcpy((void *)(var_cpy->tally),(void *)(var->tally),var_cpy->sz*sizeof(long));
  } /* end if */
  if(var->wgt_sum){
    var_cpy->wgt_sum=(double *)nco_malloc_dbg(var_cpy->sz*sizeof(double),"Unable to malloc() wgt_sum buffer in variable deep-copy",fnc_nm);
    (void)memcpy((void *)(var_cpy->wgt_sum),(void *)(var->wgt_sum),var_cpy->sz*sizeof(double));
  } /* end if */
  if(var->dim){
    var_cpy->dim=(dmn_sct **)nco_malloc(var_cpy->nbr_dim*sizeof(dmn_sct *));
    (void)memcpy((void *)(var_cpy->dim),(void *)(var->dim),var_cpy->nbr_dim*sizeof(var->dim[0]));
  } /* end if */
  if(var->dmn_id){
    var_cpy->dmn_id=(int *)nco_malloc(var_cpy->nbr_dim*sizeof(int));
    (void)memcpy((void *)(var_cpy->dmn_id),(void *)(var->dmn_id),var_cpy->nbr_dim*sizeof(var->dmn_id[0]));
  } /* end if */
  if(var->cnk_sz){
    var_cpy->cnk_sz=(size_t *)nco_malloc(var_cpy->nbr_dim*sizeof(size_t));
    (void)memcpy((void *)(var_cpy->cnk_sz),(void *)(var->cnk_sz),var_cpy->nbr_dim*sizeof(var->cnk_sz[0]));
  } /* end if */
  if(var->cnt){
    var_cpy->cnt=(long *)nco_malloc(var_cpy->nbr_dim*sizeof(long));
    (void)memcpy((void *)(var_cpy->cnt),(void *)(var->cnt),var_cpy->nbr_dim*sizeof(var->cnt[0]));
  } /* end if */
  if(var->srt){
    var_cpy->srt=(long *)nco_malloc(var_cpy->nbr_dim*sizeof(long));
    (void)memcpy((void *)(var_cpy->srt),(void *)(var->srt),var_cpy->nbr_dim*sizeof(var->srt[0]));
  } /* end if */
  if(var->end){
    var_cpy->end=(long *)nco_malloc(var_cpy->nbr_dim*sizeof(long));
    (void)memcpy((void *)(var_cpy->end),(void *)(var->end),var_cpy->nbr_dim*sizeof(var->end[0]));
  } /* end if */
  if(var->srd){
    var_cpy->srd=(long *)nco_malloc(var_cpy->nbr_dim*sizeof(long));
    (void)memcpy((void *)(var_cpy->srd),(void *)(var->srd),var_cpy->nbr_dim*sizeof(var->srd[0]));
  } /* end if */
  if(var->scl_fct.vp){
    var_cpy->scl_fct.vp=(void *)nco_malloc(nco_typ_lng(var_cpy->typ_upk));
    (void)memcpy((void *)(var_cpy->scl_fct.vp),(void *)(var->scl_fct.vp),nco_typ_lng(var_cpy->typ_upk));
  } /* end if */
  if(var->add_fst.vp){
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

  if(False) (void)fprintf(stdout,"%s: DEBUG: fxm TODO nco354. Calling nco_get_vara() for %s with nc_id=%d, var_id=%d, var_srt=%li, var_cnt = %li, var_val = %g, var_typ = %s\n",nco_prg_nm_get(),var->nm,nc_id,var->id,var->srt[0],var->cnt[0],var->val.fp[0],nco_typ_sng(var->typ_dsk));

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
      if(var->sz > 1L) (void)nco_get_vara(nc_id,var->id,var->srt,var->cnt,var->val.vp,var->typ_dsk); else (void)nco_get_var1(nc_id,var->id,var->srt,var->val.vp,var->typ_dsk);
    }else{ 
      (void)nco_get_vars(nc_id,var->id,var->srt,var->cnt,var->srd,var->val.vp,var->typ_dsk);
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
  if(nco_is_rth_opr(nco_prg_id_get())){
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
  for(idx=0;idx<var->nbr_dim;idx++){
    var->dim[idx]=var->dim[idx]->xrf;
  }
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
  if(var->type == (nc_type)NC_STRING){
    /* 20140212: ncwa may free this memory twice because reduced variable not created correctly in nco_var_avg() as per TODO nco1127
       Temporarily only free string variables during debugging
       20160115: Henry using var_sct for new ncap2 var lists, needs string memory free'd, so deep-free() strings in ncap2, and only deep-free with others during debugging */
    if(nco_dbg_lvl_get() == nco_dbg_crr || nco_prg_id_get() == ncap)
      if(var->val.vp) var->val.vp=(void *)nco_sng_lst_free((char **)var->val.vp,var->sz);
  }else{
    if(var->val.vp) var->val.vp=nco_free(var->val.vp);
  } /* endif */

  var->nm=(char *)nco_free(var->nm);
  var->nm_fll=(char *)nco_free(var->nm_fll);
  var->mss_val.vp=nco_free(var->mss_val.vp);
  var->tally=(long *)nco_free(var->tally);
  var->wgt_sum=(double *)nco_free(var->wgt_sum);
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
  var->nm_fll=NULL;
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
  var->wgt_sum=NULL; /* [frc] Running sum of per-file weights (ncra/ncea only) */
  var->wgt_crr=0.0; /* [frc] Weight of current record (ncra/ncea only) */
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
  var->undefined=False; /* [flg] Used by ncap parser */
  var->is_fix_var=True; /* Is this a fixed (non-processed) variable? */
  var->dfl_lvl=NCO_DFL_LVL_UNDEFINED; /* [enm] Deflate level */
  var->shuffle=NC_NOSHUFFLE; /* [flg] Turn-on shuffle filter */
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
nco_var_copy /* [fnc] Copy hyperslab variables of type var_typ from op1 to op2 */
(const nc_type var_typ, /* I [enm] netCDF type */
 const long sz, /* I [nbr] Number of elements to copy */
 const ptr_unn op1, /* I [sct] Values to copy */
 ptr_unn op2) /* O [sct] Destination to copy values to */
{
  /* Purpose: Copy hyperslab variables of type var_typ from op1 to op2
     Assumes memory area in op2 has already been malloc()'d
     nco_var_copy(): Does nothing with missing values and tallies
     nco_var_copy_tll(): Accounts for missing values in tally */
  (void)memcpy((void *)(op2.vp),(void *)(op1.vp),sz*nco_typ_lng(var_typ));
} /* end nco_var_copy() */

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
     to be used by certain programs---those that alter the rank of input variables. 
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
  int dmn_id_vec[NC_MAX_VAR_DIMS];
  int idx;
  int dmn_idx;
  int fl_fmt; /* [enm] Output file format */
  int nco_prg_id; /* [enm] Program ID */
  int rcd=NC_NOERR; /* [rcd] Return code */

  nc_type typ_out; /* [enm] Type in output file */

  nco_prg_id=nco_prg_id_get(); /* [enm] Program ID */

  for(idx=0;idx<nbr_var;idx++){

    /* Obtain netCDF type to define variable from NCO program ID */
    typ_out=nco_get_typ(var[idx]);

    /* Is requested variable already in output file? */
    rcd=nco_inq_varid_flg(out_id,var[idx]->nm,&var[idx]->id);

    /* If variable has not been defined, define it */
    if(rcd != NC_NOERR){

      /* TODO #116: There is a problem here in that var_out[idx]->nbr_dim is never explicitly set to the actual number of output dimensions, rather, it is simply copied from var[idx]. When var_out[idx] actually has 0 dimensions, the loop executes once anyway, and an erroneous index into the dmn_out[idx] array is attempted. Fix is to explicitly define var_out[idx]->nbr_dim. Until this is done, anything in ncwa that explicitly depends on var_out[idx]->nbr_dim is suspect. The real problem is that, in ncwa, nco_var_avg() expects var_out[idx]->nbr_dim to contain the input, rather than output, number of dimensions. The routine, nco_var_dfn() was designed to call the simple branch when dmn_ncl == 0, i.e., for operators besides ncwa. However, when ncwa averages all dimensions in output file, nbr_dmn_ncl == 0 so the wrong branch would get called unless we specifically use this branch whenever ncwa is calling. */
      if(dmn_ncl || nco_prg_id == ncwa){
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
            if(var[idx]->xrf->dim[dmn_idx]->id == dmn_ncl[idx_ncl]->xrf->id){
              break;
            } /* endif */
          } /* end loop over idx_ncl */
          if(idx_ncl != nbr_dmn_ncl){
            dmn_id_vec[dmn_nbr++]=var[idx]->dim[dmn_idx]->id;
          } /* endif */
        } /* end loop over dmn_idx */
      }else{ /* ...operator does not change variable rank so handle normally... */
        /* More straightforward definition used by operators besides ncwa */
        for(dmn_idx=0;dmn_idx<var[idx]->nbr_dim;dmn_idx++){
          dmn_id_vec[dmn_idx]=var[idx]->dim[dmn_idx]->id;
        } /* end loop over dmn_idx */
        dmn_nbr=var[idx]->nbr_dim;
      } /* end else */

      if(nco_dbg_lvl_get() > 3 && nco_prg_id != ncwa){
        /* fxm TODO nco374 diagnostic information fails for ncwa since var[idx]->dim[dmn_idx]->nm
	   contains _wrong name_ when variables will be averaged.
	   ncwa does contain write name information now if retain_degenerate_dimensions 
	   option is in effect, but this is the exception rather than the rule. */
        (void)fprintf(stdout,"%s: DEBUG %s about to define variable %s with %d dimension%s%s",nco_prg_nm_get(),fnc_nm,var[idx]->nm,dmn_nbr,(dmn_nbr == 1) ? "" : "s",(dmn_nbr > 0) ? " (ordinal,output ID): " : "");
        for(dmn_idx=0;dmn_idx<dmn_nbr;dmn_idx++){
          (void)fprintf(stdout,"%s (%d,%s)%s",var[idx]->dim[dmn_idx]->nm,dmn_idx,"unknown",(dmn_idx < dmn_nbr-1) ? ", " : "");
        } /* end loop over dmn */
        (void)fprintf(stdout,"\n");
      } /* endif dbg */

      /* The all-important variable definition call itself... */
      (void)nco_def_var(out_id,var[idx]->nm,typ_out,dmn_nbr,dmn_id_vec,&var[idx]->id);

      /* Set HDF Lempel-Ziv compression level, if requested */
      rcd=nco_inq_format(out_id,&fl_fmt);
      if(fl_fmt == NC_FORMAT_NETCDF4 || fl_fmt == NC_FORMAT_NETCDF4_CLASSIC){
        /* Deflation */
        if(dmn_nbr > 0){
          int shuffle; /* [flg] Turn-on shuffle filter */
          int deflate; /* [flg] Turn-on deflate filter */
          int dfl_lvl_in; /* [enm] Deflate level [0..9] */
          int var_in_id;
          /* Uncertain that output name always exists in input file */
          rcd=nco_inq_varid_flg(in_id,var[idx]->nm,&var_in_id);
          if(rcd == NC_NOERR){
            /* When output name is in input file, inquire input deflation level */
            rcd=nco_inq_var_deflate(in_id,var_in_id,&shuffle,&deflate,&dfl_lvl_in);
            /* Copy original deflation settings */
            if(deflate || shuffle) (void)nco_def_var_deflate(out_id,var[idx]->id,shuffle,deflate,dfl_lvl_in);
          }else{
	    /* Shuffle never, to my knowledge, increases filesize, so shuffle by default when manually deflating */
	    shuffle=NC_SHUFFLE;
	  } /* endelse */
          /* Overwrite HDF Lempel-Ziv compression level, if requested */
	  if(dfl_lvl == 0) deflate=(int)False; else deflate=(int)True;
	  /* Turn-off shuffle when uncompressing otherwise chunking requests may fail */
	  if(dfl_lvl == 0) shuffle=NC_NOSHUFFLE; 
#if ENABLE_CCR
	  const nco_flt_typ_enm nco_flt_enm=(nco_flt_typ_enm)nco_flt_glb_get();
	  /* Build list of filters available through the CCR API */
	  char ccr_flt_lst[200]; /* [sng] List of available CCR filters */
	  nco_bool ccr_has_flt=True; /* [flg] CCR has requested filter */
	  ccr_flt_lst[0]='\0';
	  strcat(ccr_flt_lst,"DEFLATE");
#if CCR_HAS_BZIP2
	  strcat(ccr_flt_lst,", Bzip2");
#endif /* !CCR_HAS_BZIP2 */
#if CCR_HAS_LZ4
	  strcat(ccr_flt_lst,", LZ4");
#endif /* !CCR_HAS_LZ4 */
#if CCR_HAS_BITGROOM
	  strcat(ccr_flt_lst,", BitGroom");
#endif /* !CCR_HAS_BITGROOM */
#if CCR_HAS_ZSTD
	  strcat(ccr_flt_lst,", Zstd");
#endif /* !CCR_HAS_ZSTD */
	  //(void)fprintf(stdout,"DEBUG quark: nco_flt_enm=%d, dfl_lvl=%d\n",(int)nco_flt_enm,dfl_lvl);
	  switch(nco_flt_enm){
	  case nco_flt_nil: /* If user did not select a filter then default to DEFLATE */
	  case nco_flt_dfl: /* DEFLATE */
	    if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,var[idx]->id,shuffle,deflate,dfl_lvl);
	    break;
	  case nco_flt_bzp: /* Bzip2 */
#if CCR_HAS_BZIP2
	    if(dfl_lvl > 0) (void)nc_def_var_bzip2(out_id,var[idx]->id,dfl_lvl);
#else /* !CCR_HAS_BZIP2 */
	    ccr_has_flt=False;
#endif /* !CCR_HAS_BZIP2 */
	    break;
	  case nco_flt_lz4: /* LZ4 */ 
#if CCR_HAS_LZ4
	    if(dfl_lvl > 0) (void)nc_def_var_lz4(out_id,var[idx]->id,dfl_lvl);
#else /* !CCR_HAS_LZ4 */
	    ccr_has_flt=False;
#endif /* !CCR_HAS_LZ4 */
	    break;
	  case nco_flt_bgr: /* Bit Grooming */
#if CCR_HAS_BITGROOM
	    if(dfl_lvl > 0) (void)nc_def_var_bitgroom(out_id,var[idx]->id,dfl_lvl);
#else /* !CCR_HAS_BITGROOM */
	    ccr_has_flt=False;
#endif /* !CCR_HAS_BITGROOM */
	    break;
	  case nco_flt_zst: /* Zstandard */
#if CCR_HAS_ZSTD
	    /* Zstandard accepts negative compression levels */
	    (void)nc_def_var_zstandard(out_id,var[idx]->id,dfl_lvl);
#else /* !CCR_HAS_ZSTD */
	    ccr_has_flt=False;
#endif /* !CCR_HAS_ZSTD */
	    break;
	  case nco_flt_dgr: /* Digit Rounding */
	  case nco_flt_btr: /* Bit Rounding */
	    ccr_has_flt=False;
	    if(dfl_lvl <= 0) break;
	  default: nco_dfl_case_flt_err(); break;
	  } /* !nco_flt_enm */

	  if(!ccr_has_flt){
	    (void)fprintf(stdout,"%s: ERROR %s reports CCR library does not define API for requested filter \"%s\". If this filter name was not a typo, then probably this filter was not built and installed in the CCR when this NCO was built/installed. If the filter is newer, you might try updating the installed CCR then updating the installed NCO. Otherwise, re-try this command and specify an already-installed filter from this list: %s\n",nco_prg_nm_get(),fnc_nm,nco_flt_enm2sng(nco_flt_enm),ccr_flt_lst);
	    nco_exit(EXIT_FAILURE);
	  } /* !ccr_has_flt */

#else /* !ENABLE_CCR */
	  if(dfl_lvl >= 0) (void)nco_def_var_deflate(out_id,var[idx]->id,shuffle,deflate,dfl_lvl);
#endif /* !ENABLE_CCR */
        } /* !dmn_nbr */
      } /* !netCDF4 */
      
      if(nco_dbg_lvl_get() > 3 && nco_prg_id != ncwa){
        /* fxm TODO nco374 diagnostic information fails for ncwa since var[idx]->dim[dmn_idx]->nm
	   contains _wrong name_ when variables will be averaged.
	   ncwa does contain write name information now if retain_degenerate_dimensions 
	   option is in effect, but this is the exception rather than the rule. */
        (void)fprintf(stdout,"%s: DEBUG %s defined variable %s with %d dimension%s%s",nco_prg_nm_get(),fnc_nm,var[idx]->nm,dmn_nbr,(dmn_nbr == 1) ? "" : "s",(dmn_nbr > 0) ? " (ordinal,output ID): " : "");
        for(dmn_idx=0;dmn_idx<dmn_nbr;dmn_idx++){
          (void)fprintf(stdout,"%s (%d,%d)%s",var[idx]->dim[dmn_idx]->nm,dmn_idx,dmn_id_vec[dmn_idx],(dmn_idx < dmn_nbr-1) ? ", " : "");
        } /* end loop over dmn */
        (void)fprintf(stdout,"\n");
      } /* endif dbg */

      /* endif variable has not yet been defined in output file */
    }else{
      /* Variable is already in output file---use existing definition
      This branch is executed, e.g., by operators in append mode */
      (void)fprintf(stdout,"%s: WARNING Using existing definition of variable \"%s\" in %s\n",nco_prg_nm_get(),var[idx]->nm,fl_out);
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
       --> Copy all attributes, define dummy values for scale_factor and add_offset now, and write those values later, when they are known */

    /* Do not copy packing attributes "scale_factor" and "add_offset" 
       if variable is packed in input file and unpacked in output file 
       Arithmetic operators calling nco_var_dfn() with fixed variables should leave them fixed
       Currently ncap calls nco_var_dfn() only for fixed variables, so handle exception with ncap-specific condition */
    /* Copy exising packing attributes, if any, unless... */
    if(nco_is_rth_opr(nco_prg_id) && /* ...operator is arithmetic... */
       nco_prg_id != ncap && /* ...and is not ncap (hence it must be, e.g., ncra, ncbo)... */
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
       If variable is not fixed (e.g., coordinate variables)...*/
    if(!var[idx]->is_fix_var){
      /* ...and operator will attempt to pack some variables... */
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
    } /* endif variable is processed (not fixed) */
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
          (void)nco_get_vars(in_id,var[idx]->id,var[idx]->srt,var[idx]->cnt,var[idx]->srd,var[idx]->val.vp,var[idx]->type);
          (void)nco_put_vars(out_id,var[idx]->xrf->id,var[idx]->xrf->srt,var[idx]->xrf->cnt,var[idx]->xrf->srd,var[idx]->xrf->val.vp,var[idx]->type);
        } /* endif variable has non-unity stride */
      } /* end if var_sz */
    } /* end if variable is an array */
    var[idx]->val.vp=var[idx]->xrf->val.vp=nco_free(var[idx]->val.vp);
  } /* end loop over idx */
    
} /* end nco_var_val_cpy() */

nco_bool /* [flg] Variable is listed in this CF attribute, thereby associated */
nco_is_spc_in_cf_att /* [fnc] Variable is listed in this CF attribute, thereby associated */
(const int nc_id,    /* I [id] netCDF file ID */
 const char *const cf_nm, /* I [sng] cf att name */
 const int var_trg_id,  /* I [id] CF Variable ID */
 int *cf_var_id) /* O [id] CF Variable ID */
{
  /* Purpose: Is variable specified in an associated attribute?
     Associated attributes include "ancillary_variables", "bounds", "climatology", "coordinates", "grid_mapping"
     One of these ("ancillary_variables") can contain "non-grid" variables
     The others contain variables that should, more or less, be treated as coordinates
     However this function does not care about such distinctions
     It simply returns true or false depending on whether the variable appears in the indicated attribute value
     This function coaslesces (and makes obsolete) four earlier functions with the same purpose
     Those functions were identical except for the attribute name, so this function takes the attribute name as an argument
     It is based on nco_is_spc_in_crd_att() */
  nco_bool IS_SPC_IN_CF_ATT=False; /* [flg] Variable is listed in this CF attribute  */

  const char dlm_sng[]=" "; /* [sng] Delimiter string */
  const char fnc_nm[]="nco_is_spc_in_cf_att()"; /* [sng] Function name */
  char **cf_lst; /* [sng] 1D array of list elements */
  char *att_val;
  char att_nm[NC_MAX_NAME];
  char var_nm[NC_MAX_NAME];
  char var_trg_nm[NC_MAX_NAME];
  int idx_att;
  int idx_cf;
  int idx_var;
  int nbr_att;
  int nbr_cf; /* [nbr] Number of variables listed in this CF attribute */
  int nbr_var; /* [nbr] Number of variables in file */
  int rcd=NC_NOERR; /* [rcd] Return code */
  int var_id; /* [id] Variable ID */
  long att_sz;
  nc_type att_typ;
  static nco_bool FIRST_WARNING=True;

  /* May need variable name for later comparison to those listed in this attribute */
  rcd+=nco_inq_varname(nc_id,var_trg_id,var_trg_nm);
  rcd+=nco_inq_nvars(nc_id,&nbr_var);

  for(idx_var=0;idx_var<nbr_var;idx_var++){
    /* This assumption, praise the Lord, is valid in netCDF2, netCDF3, and netCDF4 */
    var_id=idx_var;

    /* Find number of attributes */
    rcd+=nco_inq_varnatts(nc_id,var_id,&nbr_att);
    for(idx_att=0;idx_att<nbr_att;idx_att++){
      rcd+=nco_inq_attname(nc_id,var_id,idx_att,att_nm);
      /* Is attribute part of CF convention? */
      if(!strcmp(att_nm,cf_nm)){
        /* Yes, get list of specified attributes */
        rcd+=nco_inq_att(nc_id,var_id,att_nm,&att_typ,&att_sz);
        if(att_typ != NC_CHAR){
          rcd=nco_inq_varname(nc_id,var_id,var_nm);
          if(FIRST_WARNING) (void)fprintf(stderr,"%s: WARNING the \"%s\" attribute for variable %s is type %s, not %s. This violates the CF convention for allowed datatypes (http://cfconventions.org/cf-conventions/cf-conventions.html#_data_types). Therefore %s will skip this attribute. NB: To avoid excessive noise, NCO prints this WARNING at most once per dataset.\n",nco_prg_nm_get(),att_nm,var_nm,nco_typ_sng(att_typ),nco_typ_sng(NC_CHAR),fnc_nm);
	  FIRST_WARNING=False;
          return IS_SPC_IN_CF_ATT;
        } /* end if */
        att_val=(char *)nco_malloc((att_sz+1L)*sizeof(char));
        if(att_sz > 0) rcd=nco_get_att(nc_id,var_id,att_nm,(void *)att_val,NC_CHAR);	  
        /* NUL-terminate attribute */
        att_val[att_sz]='\0';
        /* Split list into separate variable names
	   Use nco_lst_prs_sgl_2D() not nco_lst_prs_2D() to avert TODO nco944 */
        cf_lst=nco_lst_prs_sgl_2D(att_val,dlm_sng,&nbr_cf);
        /* ...for each variable in this CF attribute... */
        for(idx_cf=0;idx_cf<nbr_cf;idx_cf++){
          /* Does variable match name specified in CF attribute list? */
          if(!strcmp(var_trg_nm,cf_lst[idx_cf])) break;
        } /* end loop over coordinates in list */
        /* Free allocated memory */
        att_val=(char *)nco_free(att_val);
        cf_lst=nco_sng_lst_free(cf_lst,nbr_cf);

        if(idx_cf!=nbr_cf){
           IS_SPC_IN_CF_ATT = True;
           if(cf_var_id) *cf_var_id=var_id;
           goto end_lbl; /* break out of all loops */
        }
      } /* !coordinates */
    } /* end loop over attributes */
  } /* end loop over idx_var */

  end_lbl: ;

  return IS_SPC_IN_CF_ATT; /* [flg] Variable is listed in this CF attribute */
} /* end nco_is_spc_in_cf_att() */


char *** /* O [ptr] List of lists - each ragged array terminated with empty string */
nco_lst_cf_att /* [fnc] look in all vars for att cf_nm */
(const int nc_id, /* I [id] netCDF file ID */
 const char *const cf_nm, /* I [sng] CF att name */
 int *nbr_lst) /* O [nbr] number of ragged arrays returned */
{
  /* Purpose: Is variable specified in an associated attribute?
     Associated attributes include "ancillary_variables", "bounds", "climatology", "coordinates", "grid_mapping"
     One of these ("ancillary_variables") can contain "non-grid" variables
     The others contain variables that should, more or less, be treated as coordinates
     However this function does not care about such distinctions
     It simply returns true or false depending on whether the variable appears in the indicated attribute value
     This function coaslesces (and makes obsolete) four earlier functions with the same purpose
     Those functions were identical except for the attribute name, so this function takes the attribute name as an argument
     It is based on nco_is_spc_in_crd_att() */

  const char dlm_sng[]=" "; /* [sng] Delimiter string */
  char **cf_lst; /* [sng] 1D array of list elements */
  char **int_lst=NULL_CEWI; /* store pointer to ragged array */
  char *att_val;
  char ***ra_lst=NULL_CEWI; /* array of ragged array */
  char att_nm[NC_MAX_NAME];
  char var_nm[NC_MAX_NAME];
  int idx;
  int jdx;
  int nbr_att;
  int nbr_cf; /* [nbr] Number of variables listed in this CF attribute */
  int nbr_var; /* [nbr] Number of variables in file */
  int rcd=NC_NOERR; /* [rcd] Return code */
  int var_id; /* [id] Variable ID */
  long att_sz;
  nc_type att_typ;

  *nbr_lst=0;
  rcd+=nco_inq_nvars(nc_id,&nbr_var);

  /* This assumption, praise the Lord, is valid in netCDF2, netCDF3, and netCDF4 */
  for(var_id=0;var_id<nbr_var;var_id++){

    nco_inq_varname(nc_id,var_id,var_nm);

    /* Find number of attributes */
    rcd+=nco_inq_varnatts(nc_id,var_id,&nbr_att);
    for(idx=0;idx<nbr_att;idx++){
      rcd+=nco_inq_attname(nc_id,var_id,idx,att_nm);
      /* Is attribute part of CF convention? */
      if(!strcmp(att_nm,cf_nm)){
        /* Yes, get list of specified attributes */
        rcd+=nco_inq_att(nc_id,var_id,att_nm,&att_typ,&att_sz);
        if(att_typ != NC_CHAR) continue;

        att_val=(char *)nco_malloc((att_sz+1L)*sizeof(char));
        if(att_sz > 0) rcd=nco_get_att(nc_id,var_id,att_nm,(void *)att_val,NC_CHAR);
        /* NUL-terminate attribute */
        att_val[att_sz]='\0';
        /* Split list into separate variable names
	   Use nco_lst_prs_sgl_2D() not nco_lst_prs_2D() to avert TODO nco944 */
        cf_lst=nco_lst_prs_sgl_2D(att_val,dlm_sng,&nbr_cf);

        int_lst=(char **)nco_malloc((nbr_cf+3)*sizeof(char *));
        int_lst[0]=strdup(var_nm);
        int_lst[1]=strdup(cf_nm);

        for(jdx=0;jdx<nbr_cf;jdx++)
          int_lst[jdx+2]=strdup(cf_lst[jdx]);

        /* very important - ragged array is terminated by an empty string */
        int_lst[nbr_cf+2]=strdup("");

        /* add ragged array to list */
        ra_lst=(char ***)nco_realloc(ra_lst,sizeof(char**)*(*nbr_lst+1));
        ra_lst[*nbr_lst]=int_lst;
        ++*nbr_lst;

        att_val=(char *)nco_free(att_val);
        cf_lst=nco_sng_lst_free(cf_lst,nbr_cf);
        // cf_lst=nco_free(cf_lst);
        int_lst=(char **)NULL_CEWI;
      }
    } /* end loop over attributes */
  } /* end loop over var_id */

  return ra_lst;
} /* end nco_lst_cf_att() */

nco_bool /* [flg] Variable is listed in a "coordinates" attribute */
nco_is_spc_in_crd_att /* [fnc] Variable is listed in a "coordinates" attribute */
(const int nc_id, /* I [id] netCDF file ID */
 const int var_trg_id) /* I [id] Variable ID */
{
  /* Purpose: Is variable specified in a "coordinates" attribute?
     Typical variables that appear in a "coordinates" attribute include ...
     If so, it may be a "multi-dimensional coordinate" that should
     undergo special treatment by arithmetic operators. */
  nco_bool IS_SPC_IN_CRD_ATT=False; /* [flg] Variable is listed in a "coordinates" attribute  */

  const char dlm_sng[]=" "; /* [sng] Delimiter string */
  const char fnc_nm[]="nco_is_spc_in_crd_att()"; /* [sng] Function name */
  char **crd_lst; /* [sng] 1D array of list elements */
  char *att_val;
  char att_nm[NC_MAX_NAME];
  char var_nm[NC_MAX_NAME];
  char var_trg_nm[NC_MAX_NAME];
  int idx_att;
  int idx_crd;
  int idx_var;
  int nbr_att;
  int nbr_crd; /* [nbr] Number of coordinates specified in "coordinates" attribute */
  int nbr_var; /* [nbr] Number of variables in file */
  int rcd=NC_NOERR; /* [rcd] Return code */
  int var_id; /* [id] Variable ID */
  long att_sz;
  nc_type att_typ;

  /* May need variable name for later comparison to "coordinates" attribute */
  rcd+=nco_inq_varname(nc_id,var_trg_id,var_trg_nm);
  rcd+=nco_inq_nvars(nc_id,&nbr_var);

  for(idx_var=0;idx_var<nbr_var;idx_var++){
    /* This assumption, praise the Lord, is valid in netCDF2, netCDF3, and netCDF4 */
    var_id=idx_var;

    /* Find number of attributes */
    rcd+=nco_inq_varnatts(nc_id,var_id,&nbr_att);
    for(idx_att=0;idx_att<nbr_att;idx_att++){
      rcd+=nco_inq_attname(nc_id,var_id,idx_att,att_nm);
      /* Is attribute part of CF convention? */
      if(!strcmp(att_nm,"coordinates")){
        /* Yes, get list of specified attributes */
        rcd+=nco_inq_att(nc_id,var_id,att_nm,&att_typ,&att_sz);
        if(att_typ != NC_CHAR){
          rcd=nco_inq_varname(nc_id,var_id,var_nm);
          (void)fprintf(stderr,"%s: WARNING the \"%s\" attribute for variable %s is type %s, not %s. This violates the CF convention for allowed datatypes (http://cfconventions.org/cf-conventions/cf-conventions.html#_data_types). Therefore %s will skip this attribute.\n",nco_prg_nm_get(),att_nm,var_nm,nco_typ_sng(att_typ),nco_typ_sng(NC_CHAR),fnc_nm);
          return IS_SPC_IN_CRD_ATT;
        } /* end if */
        att_val=(char *)nco_malloc((att_sz+1L)*sizeof(char));
        if(att_sz > 0) rcd=nco_get_att(nc_id,var_id,att_nm,(void *)att_val,NC_CHAR);	  
        /* NUL-terminate attribute */
        att_val[att_sz]='\0';
        /* Split list into separate coordinate names
	   Use nco_lst_prs_sgl_2D() not nco_lst_prs_2D() to avert TODO nco944 */
        crd_lst=nco_lst_prs_sgl_2D(att_val,dlm_sng,&nbr_crd);
        /* ...for each coordinate in "coordinates" attribute... */
        for(idx_crd=0;idx_crd<nbr_crd;idx_crd++){
          /* Does variable match name specified in coordinate list? */
          if(!strcmp(var_trg_nm,crd_lst[idx_crd])) break;
        } /* end loop over coordinates in list */
        if(idx_crd!=nbr_crd) IS_SPC_IN_CRD_ATT=True;
        /* Free allocated memory */
        att_val=(char *)nco_free(att_val);
        crd_lst=nco_sng_lst_free(crd_lst,nbr_crd);
      } /* !coordinates */
    } /* end loop over attributes */
  } /* end loop over idx_var */

  return IS_SPC_IN_CRD_ATT; /* [flg] Variable is listed in a "coordinates" attribute  */
} /* end nco_is_spc_in_crd_att() */

nco_bool /* [flg] Variable is listed in a "bounds" attribute */
nco_is_spc_in_bnd_att /* [fnc] Variable is listed in a "bounds" attribute */
(const int nc_id, /* I [id] netCDF file ID */
 const int var_trg_id) /* I [id] Variable ID */
{
  /* Purpose: Is variable specified in a "bounds" attribute?
     Typical variables that appear in a "bounds" attribute include "lat_bnds", "lon_bnds", "time_bnds", etc.
     Such variables may be "multi-dimensional coordinates" that should
     undergo special treatment by arithmetic operators.
     Routine based on nco_is_spc_in_crd_att() */
  nco_bool IS_SPC_IN_BND_ATT=False; /* [flg] Variable is listed in a "bounds" attribute  */

  const char dlm_sng[]=" "; /* [sng] Delimiter string */
  const char fnc_nm[]="nco_is_spc_in_bnd_att()"; /* [sng] Function name */
  char **bnd_lst; /* [sng] 1D array of list elements */
  char *att_val;
  char att_nm[NC_MAX_NAME];
  char var_nm[NC_MAX_NAME];
  char var_trg_nm[NC_MAX_NAME];
  int idx_att;
  int idx_bnd;
  int idx_var;
  int nbr_att;
  int nbr_bnd; /* [nbr] Number of coordinates specified in "bounds" attribute */
  int nbr_var; /* [nbr] Number of variables in file */
  int rcd=NC_NOERR; /* [rcd] Return code */
  int var_id; /* [id] Variable ID */
  long att_sz;
  nc_type att_typ;

  /* May need variable name for later comparison to "bounds" attribute */
  rcd+=nco_inq_varname(nc_id,var_trg_id,var_trg_nm);
  rcd+=nco_inq_nvars(nc_id,&nbr_var);

  for(idx_var=0;idx_var<nbr_var;idx_var++){
    /* This assumption, praise the Lord, is valid in netCDF2, netCDF3, and netCDF4 */
    var_id=idx_var;

    /* Find number of attributes */
    rcd+=nco_inq_varnatts(nc_id,var_id,&nbr_att);
    for(idx_att=0;idx_att<nbr_att;idx_att++){
      rcd+=nco_inq_attname(nc_id,var_id,idx_att,att_nm);
      /* Is attribute part of CF convention? */
      if(!strcmp(att_nm,"bounds")){
        /* Yes, get list of specified attributes */
        rcd+=nco_inq_att(nc_id,var_id,att_nm,&att_typ,&att_sz);
        if(att_typ != NC_CHAR){
          rcd=nco_inq_varname(nc_id,var_id,var_nm);
          (void)fprintf(stderr,"%s: WARNING the \"%s\" attribute for variable %s is type %s, not %s. This violates the CF convention for allowed datatypes (http://cfconventions.org/cf-conventions/cf-conventions.html#_data_types). Therefore %s will skip this attribute.\n",nco_prg_nm_get(),att_nm,var_nm,nco_typ_sng(att_typ),nco_typ_sng(NC_CHAR),fnc_nm);
          return IS_SPC_IN_BND_ATT;
        } /* end if */
        att_val=(char *)nco_malloc((att_sz+1L)*sizeof(char));
        if(att_sz > 0) rcd=nco_get_att(nc_id,var_id,att_nm,(void *)att_val,NC_CHAR);	  
        /* NUL-terminate attribute */
        att_val[att_sz]='\0';
        /* Split list into separate coordinate names
	   Use nco_lst_prs_sgl_2D() not nco_lst_prs_2D() to avert TODO nco944 */
        bnd_lst=nco_lst_prs_sgl_2D(att_val,dlm_sng,&nbr_bnd);
        /* ...for each coordinate in "bounds" attribute... */
        for(idx_bnd=0;idx_bnd<nbr_bnd;idx_bnd++){
          /* Does variable match name specified in coordinate list? */
          if(!strcmp(var_trg_nm,bnd_lst[idx_bnd])) break;
        } /* end loop over coordinates in list */
        if(idx_bnd!=nbr_bnd) IS_SPC_IN_BND_ATT=True;
        /* Free allocated memory */
        att_val=(char *)nco_free(att_val);
        bnd_lst=nco_sng_lst_free(bnd_lst,nbr_bnd);
      } /* !coordinates */
    } /* end loop over attributes */
  } /* end loop over idx_var */

  return IS_SPC_IN_BND_ATT; /* [flg] Variable is listed in a "bounds" attribute  */
} /* end nco_is_spc_in_bnd_att() */

nco_bool /* [flg] Variable is listed in a "climatology" attribute */
nco_is_spc_in_clm_att /* [fnc] Variable is listed in a "climatology" attribute */
(const int nc_id, /* I [id] netCDF file ID */
 const int var_trg_id) /* I [id] Variable ID */
{
  /* Purpose: Is variable specified in a "climatology" attribute?
     Typical variables that appear in a "climatology" attribute include "climatology_bounds"
     Such variables may be "multi-dimensional coordinates" that should
     undergo special treatment by arithmetic operators.
     Routine based on nco_is_spc_in_crd_att() */
  nco_bool IS_SPC_IN_CLM_ATT=False; /* [flg] Variable is listed in a "climatology" attribute  */

  const char dlm_sng[]=" "; /* [sng] Delimiter string */
  const char fnc_nm[]="nco_is_spc_in_clm_att()"; /* [sng] Function name */
  char **clm_lst; /* [sng] 1D array of list elements */
  char *att_val;
  char att_nm[NC_MAX_NAME];
  char var_nm[NC_MAX_NAME];
  char var_trg_nm[NC_MAX_NAME];
  int idx_att;
  int idx_clm;
  int idx_var;
  int nbr_att;
  int nbr_clm; /* [nbr] Number of coordinates specified in "climatology" attribute */
  int nbr_var; /* [nbr] Number of variables in file */
  int rcd=NC_NOERR; /* [rcd] Return code */
  int var_id; /* [id] Variable ID */
  long att_sz;
  nc_type att_typ;

  /* May need variable name for later comparison to "climatology" attribute */
  rcd+=nco_inq_varname(nc_id,var_trg_id,var_trg_nm);
  rcd+=nco_inq_nvars(nc_id,&nbr_var);

  for(idx_var=0;idx_var<nbr_var;idx_var++){
    /* This assumption, praise the Lord, is valid in netCDF2, netCDF3, and netCDF4 */
    var_id=idx_var;

    /* Find number of attributes */
    rcd+=nco_inq_varnatts(nc_id,var_id,&nbr_att);
    for(idx_att=0;idx_att<nbr_att;idx_att++){
      rcd+=nco_inq_attname(nc_id,var_id,idx_att,att_nm);
      /* Is attribute part of CF convention? */
      if(!strcmp(att_nm,"climatology")){
        /* Yes, get list of specified attributes */
        rcd+=nco_inq_att(nc_id,var_id,att_nm,&att_typ,&att_sz);
        if(att_typ != NC_CHAR){
          rcd=nco_inq_varname(nc_id,var_id,var_nm);
          (void)fprintf(stderr,"%s: WARNING the \"%s\" attribute for variable %s is type %s, not %s. This violates the CF convention for allowed datatypes (http://cfconventions.org/cf-conventions/cf-conventions.html#_data_types). Therefore %s will skip this attribute.\n",nco_prg_nm_get(),att_nm,var_nm,nco_typ_sng(att_typ),nco_typ_sng(NC_CHAR),fnc_nm);
          return IS_SPC_IN_CLM_ATT;
        } /* end if */
        att_val=(char *)nco_malloc((att_sz+1L)*sizeof(char));
        if(att_sz > 0) rcd=nco_get_att(nc_id,var_id,att_nm,(void *)att_val,NC_CHAR);	  
        /* NUL-terminate attribute */
        att_val[att_sz]='\0';
        /* Split list into separate coordinate names
	   Use nco_lst_prs_sgl_2D() not nco_lst_prs_2D() to avert TODO nco944 */
        clm_lst=nco_lst_prs_sgl_2D(att_val,dlm_sng,&nbr_clm);
        /* ...for each coordinate in "climatology" attribute... */
        for(idx_clm=0;idx_clm<nbr_clm;idx_clm++){
          /* Does variable match name specified in coordinate list? */
          if(!strcmp(var_trg_nm,clm_lst[idx_clm])) break;
        } /* end loop over coordinates in list */
        if(idx_clm!=nbr_clm) IS_SPC_IN_CLM_ATT=True;
        /* Free allocated memory */
        att_val=(char *)nco_free(att_val);
        clm_lst=nco_sng_lst_free(clm_lst,nbr_clm);
      } /* !coordinates */
    } /* end loop over attributes */
  } /* end loop over idx_var */

  return IS_SPC_IN_CLM_ATT; /* [flg] Variable is listed in a "climatology" attribute  */
} /* end nco_is_spc_in_clm_att() */

nco_bool /* [flg] Variable is listed in a "grid_mapping" attribute */
nco_is_spc_in_grd_att /* [fnc] Variable is listed in a "grid_mapping" attribute */
(const int nc_id, /* I [id] netCDF file ID */
 const int var_trg_id) /* I [id] Variable ID */
{
  /* Purpose: Is variable specified in a "grid_mapping" attribute?
     Typical variables that appear in a "grid_mapping" attribute include "albers_conical_equal_area"
     Such variables may be "multi-dimensional coordinates" that should
     undergo special treatment by arithmetic operators.
     Routine based on nco_is_spc_in_crd_att() */
  nco_bool IS_SPC_IN_GRD_ATT=False; /* [flg] Variable is listed in a "grid_mapping" attribute  */

  const char dlm_sng[]=" "; /* [sng] Delimiter string */
  const char fnc_nm[]="nco_is_spc_in_grd_att()"; /* [sng] Function name */
  char **grd_lst; /* [sng] 1D array of list elements */
  char *att_val;
  char att_nm[NC_MAX_NAME];
  char var_nm[NC_MAX_NAME];
  char var_trg_nm[NC_MAX_NAME];
  int idx_att;
  int idx_grd;
  int idx_var;
  int nbr_att;
  int nbr_grd; /* [nbr] Number of coordinates specified in "grid_mapping" attribute */
  int nbr_var; /* [nbr] Number of variables in file */
  int rcd=NC_NOERR; /* [rcd] Return code */
  int var_id; /* [id] Variable ID */
  long att_sz;
  nc_type att_typ;

  /* May need variable name for later comparison to "grid_mapping" attribute */
  rcd+=nco_inq_varname(nc_id,var_trg_id,var_trg_nm);
  rcd+=nco_inq_nvars(nc_id,&nbr_var);

  for(idx_var=0;idx_var<nbr_var;idx_var++){
    /* This assumption, praise the Lord, is valid in netCDF2, netCDF3, and netCDF4 */
    var_id=idx_var;

    /* Find number of attributes */
    rcd+=nco_inq_varnatts(nc_id,var_id,&nbr_att);
    for(idx_att=0;idx_att<nbr_att;idx_att++){
      rcd+=nco_inq_attname(nc_id,var_id,idx_att,att_nm);
      /* Is attribute part of CF convention? */
      if(!strcmp(att_nm,"grid_mapping")){
        /* Yes, get list of specified attributes */
        rcd+=nco_inq_att(nc_id,var_id,att_nm,&att_typ,&att_sz);
        if(att_typ != NC_CHAR){
          rcd=nco_inq_varname(nc_id,var_id,var_nm);
          (void)fprintf(stderr,"%s: WARNING the \"%s\" attribute for variable %s is type %s, not %s. This violates the CF convention for allowed datatypes (http://cfconventions.org/cf-conventions/cf-conventions.html#_data_types). Therefore %s will skip this attribute.\n",nco_prg_nm_get(),att_nm,var_nm,nco_typ_sng(att_typ),nco_typ_sng(NC_CHAR),fnc_nm);
          return IS_SPC_IN_GRD_ATT;
        } /* end if */
        att_val=(char *)nco_malloc((att_sz+1L)*sizeof(char));
        if(att_sz > 0) rcd=nco_get_att(nc_id,var_id,att_nm,(void *)att_val,NC_CHAR);	  
        /* NUL-terminate attribute */
        att_val[att_sz]='\0';
        /* Split list into separate coordinate names
	   Use nco_lst_prs_sgl_2D() not nco_lst_prs_2D() to avert TODO nco944 */
        grd_lst=nco_lst_prs_sgl_2D(att_val,dlm_sng,&nbr_grd);
        /* ...for each coordinate in "grid_mapping" attribute... */
        for(idx_grd=0;idx_grd<nbr_grd;idx_grd++){
          /* Does variable match name specified in coordinate list? */
          if(!strcmp(var_trg_nm,grd_lst[idx_grd])) break;
        } /* end loop over coordinates in list */
        if(idx_grd!=nbr_grd) IS_SPC_IN_GRD_ATT=True;
        /* Free allocated memory */
        att_val=(char *)nco_free(att_val);
        grd_lst=nco_sng_lst_free(grd_lst,nbr_grd);
      } /* !coordinates */
    } /* end loop over attributes */
  } /* end loop over idx_var */

  return IS_SPC_IN_GRD_ATT; /* [flg] Variable is listed in a "grid_mapping" attribute  */
} /* end nco_is_spc_in_grd_att() */

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
  int rcd=NC_NOERR; /* [rcd] Return code */
  int nbr_dim_old;

  var->nc_id=nc_id;

  /* 20050519: Not sure why I originally made next four lines SMP-critical
     20050629: Making next four lines multi-threaded causes no problems */
  /* Refresh variable ID first */
  rcd+=nco_inq_varid(var->nc_id,var->nm,&var->id);

  /* fxm: Not sure if/why necessary to refresh number of dimensions...though it should not hurt */
  /* Refresh number of dimensions in variable */
  nbr_dim_old=var->nbr_dim;
  rcd+=nco_inq_varndims(var->nc_id,var->id,&var->nbr_dim);
  if(nbr_dim_old != var->nbr_dim){
    (void)fprintf(stdout,"%s: ERROR Variable \"%s\" changed number of dimensions from %d to %d\n",nco_prg_nm_get(),var->nm,nbr_dim_old,var->nbr_dim);
    nco_err_exit(0,"nco_var_mtd_refresh()");
  } /* endif err */

  /* 20100923: Any need to refresh storage properties (shuffle,deflate,dfl_lvl,cnk_sz) here?
     Certainly they can change between files, that alone is not reason to refresh them
     Unlike missing values, storage properties in input are transparent to arithmetic
     The netCDF/HDF5 I/O layer handles all this transparently
     Moreover, output storage properties must be set just after variable definition, long before nco_var_mtd_refresh()
     So storage properties of variable in current file cannot affect arithmetic, nor output
     Hence there is no reason to track current storage properties in var_sct
     However, if that ever changes, here are hooks to do so */
  if(False && var->nbr_dim > 0){
    int deflate; /* [flg] Turn-on deflate filter */
    int srg_typ; /* [enm] Storage type */
    rcd+=nco_inq_var_deflate(var->nc_id,var->id,&var->shuffle,&deflate,&var->dfl_lvl);
    rcd+=nco_inq_var_chunking(var->nc_id,var->id,&srg_typ,var->cnk_sz);
  } /* endif */

  /* Set variable type so following nco_mss_val_get() casts missing_value to correct type */
  rcd+=nco_inq_vartype(var->nc_id,var->id,&var->type);

  /* Refresh number of attributes and missing value attribute, if any */
  var->has_mss_val=nco_mss_val_get(var->nc_id,var);

#if 0
  /* PJR requested warning to be added when multiple file operators worked on 
     variables with missing_value since so many things could go wrong
     Now un-necessary since multi-file packing ostensibly works
     Leave code here in case we find it does not work */
  if(nco_is_rth_opr(nco_prg_id_get()) && var->pck_dsk){
    if(var->has_mss_val) (void)fprintf(stdout,"%s: WARNING Variable \"%s\" is packed and has valid \"NCO_MSS_VAL_SNG\" attribute in multi-file arithmetic operator. Arithmetic on this variable will only be correct if...\n",nco_prg_nm_get(),var_nm);
  } /* endif variable is packed */
#endif /* endif False */

} /* end nco_var_mtd_refresh() */

void
nco_var_srd_srt_set /* [fnc] Assign zero to start and unity to stride vectors in variables */
(var_sct ** const var, /* I [sct] Variables whose subcycle, start, and stride arrays to set */
 const int nbr_var) /* I [nbr] Number of structures in variable structure list */
{
  /* Purpose: Zero start (srt) and stride (srd) arrays of variable 
     This is useful for setting variables to default output state, 
     in which data is written in a block with no offset */

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
    sz=1L;
    sz_rec=1L;
    var_sct *var_tmp;
    var_tmp=var[idx];
    
    /* 20100923: Any need to refresh storage properties (shuffle,deflate,dfl_lvl,cnk_sz) here? */

    for(jdx=0;jdx<var_tmp->nbr_dim;jdx++){
      var_tmp->srt[jdx]=var_tmp->dim[jdx]->srt; 
      var_tmp->end[jdx]=var_tmp->dim[jdx]->end;
      var_tmp->cnt[jdx]=var_tmp->dim[jdx]->cnt;
      var_tmp->srd[jdx]=var_tmp->dim[jdx]->srd;
      sz*=var_tmp->dim[jdx]->cnt;
      if(jdx > 0) sz_rec*=var_tmp->dim[jdx]->cnt;
    } /* end loop over dimensions */
    var_tmp->sz=sz; 
    var_tmp->sz_rec=sz_rec;
  } /* end loop over variables */
  
}  /* end nco_var_dmn_refresh() */

var_sct * /* O [sct] Variable structure */
nco_var_fll /* [fnc] Allocate variable structure and fill with metadata */
(const int nc_id, /* I [id] netCDF file ID */
 const int var_id, /* I [id] Variable ID */
 const char * const var_nm, /* I [sng] Variable name */
 dmn_sct * const * const dim, /* I [sct] Dimensions available to variable */
 const int nbr_dim) /* I [nbr] Number of dimensions in list */
{
  /* Purpose: nco_malloc() and return a completed var_sct */
  char dmn_nm[NC_MAX_NAME];
  
  int fl_fmt;
  int dmn_idx;
  int idx;
  int rec_dmn_id;
  
  var_sct *var;
  
  /* Get file format */
  (void)nco_inq_format(nc_id,&fl_fmt);
  
  /* Get record dimension ID */
  (void)nco_inq(nc_id,(int *)NULL,(int *)NULL,(int *)NULL,&rec_dmn_id);
  
  /* Allocate space for variable structure */
  var=(var_sct *)nco_malloc(sizeof(var_sct));
  (void)var_dfl_set(var); /* [fnc] Set defaults for each member of variable structure */
  
  /* Fill-in known fields */
  /* Make sure var_free() frees names when variable is destroyed */
  var->nm_fll=NULL;
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
      (void)fprintf(stdout,"%s: ERROR dimension %s is not in list of dimensions available to nco_var_fll()\n",nco_prg_nm_get(),dmn_nm);
      if(nco_prg_id_get() == ncap) (void)fprintf(stdout,"%s: HINT This could be a symptom of TODO nco1045. Workaround is avoid use of append mode (i.e., -A switch) in ncap2.\n",nco_prg_nm_get()); else (void)fprintf(stdout,"%s: HINT This could be a symptom of TODO nco111. Workaround is to make sure each dimension in the weighting and masking variable(s) appears in a variable to be processed.\n",nco_prg_nm_get());
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
    
    /* NB: dim[idx]->cid will be uninitialized unless dim[idx] is a coordinate 
       Hence divide this into to sequential if statements so valgrind does not
       complain about relying on uninitialized values */
    if(var->dim[idx]->is_crd_dmn){
      if(var->id == var->dim[idx]->cid){
        var->is_crd_var=True;
        var->cid=var->dmn_id[idx];
      } /* end if */
    } /* end if */
    
    /* NB: This assumes default var->sz begins as 1 */
    var->sz*=var->cnt[idx];
  } /* end loop over dim */
  
  /* 20130112: Variables associated with "bounds", "climatology", "coordinates", and "grid_mapping" attributes should,
     in most cases, be treated as coordinates */
  if(nco_is_spc_in_cf_att(var->nc_id, "bounds", var->id, NULL)) var->is_crd_var=True;
  if(nco_is_spc_in_cf_att(var->nc_id, "climatology", var->id, NULL)) var->is_crd_var=True;
  if(nco_is_spc_in_cf_att(var->nc_id, "coordinates", var->id, NULL)) var->is_crd_var=True;
  if(nco_is_spc_in_cf_att(var->nc_id, "grid_mapping", var->id, NULL)) var->is_crd_var=True;
  
  /* Portions of variable structure depend on packing properties, e.g., typ_upk
     nco_pck_dsk_inq() fills in these portions harmlessly */
  (void)nco_pck_dsk_inq(nc_id,var);
  
  /* Set deflate and chunking to defaults */  
  var->dfl_lvl=NCO_DFL_LVL_UNDEFINED; /* [enm] Deflate level */
  var->shuffle=NC_NOSHUFFLE; /* [flg] Turn-on shuffle filter */
  
  for(idx=0;idx<var->nbr_dim;idx++) var->cnk_sz[idx]=(size_t)0L;
  
  /* Read deflate levels and chunking (if any) */  
  if(fl_fmt == NC_FORMAT_NETCDF4 || fl_fmt == NC_FORMAT_NETCDF4_CLASSIC){
    int deflate; /* [enm] Deflate filter is on */
    int srg_typ; /* [enm] Storage type */
    (void)nco_inq_var_deflate(nc_id,var->id,&var->shuffle,&deflate,&var->dfl_lvl);    
    (void)nco_inq_var_chunking(nc_id,var->id,&srg_typ,var->cnk_sz);   
  } /* endif */

  var->undefined=False; /* [flg] Used by ncap parser */
  return var;
} /* end nco_var_fll() */

nc_type
nco_get_typ                           /* [fnc] Obtain netCDF type to define variable from NCO program ID */
(const var_sct * const var)           /* I [sct] Variable to be defined in output file */
{
  int nco_prg_id; /* [enm] Program ID */

  nc_type typ_out=NC_NAT; /* [enm] Type in output file */

  nco_prg_id=nco_prg_id_get(); /* [enm] Program ID */
  
  /* Checking only nco_is_rth_opr() is too simplistic
     1. All variables handled by arithmetic operators are currently unpacked on reading
     2. However "fixed variables" appear in many arithmetic operators
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
  if(nco_is_rth_opr(nco_prg_id)){
    /* Arithmetic operators store values as unpacked... */
    typ_out=var->typ_upk; 
    /* ...with two exceptions...
       ncap [un-]packing precedes nco_var_dfn() call, sets var->type appropriately */
    if(nco_prg_id == ncap) typ_out=var->type;
    /* ...and pass through fixed (non-processed) variables untouched... */
    if(var->is_fix_var) typ_out=var->type;
  }else{
    /* Non-arithmetic operators leave things alone by default
       ncpdq first modifies var_out->type, then calls nco_var_dfn(), then [un-]packs */
    typ_out=var->type;
  } /* endif arithmetic operator */
  
  return typ_out;
} /* nco_get_typ() */
