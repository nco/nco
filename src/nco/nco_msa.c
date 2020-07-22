/* $Header$ */

/* Purpose: Multi-slabbing algorithm */

/* Copyright (C) 1995--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

#include "nco_msa.h" /* Multi-slabbing algorithm */

void *
nco_msa_rcr_clc /* [fnc] Multi-slab algorithm (recursive routine, returns a single slab pointer */
(int dpt_crr, /* [nbr] Current depth, we start at 0 */
 int dpt_crr_max, /* [nbr] Maximum depth (i.e., number of dimensions in variable (does not change) */
 lmt_sct **lmt, /* [sct] Limits of current hyperslabs (these change as we recurse) */
 lmt_msa_sct **lmt_lst, /* [sct] List of limits in each dimension (this remains STATIC as we recurse) */
 var_sct *vara) /* [sct] Information for routine to read variable information and pass information between calls */
{
  /* NB: nco_msa_rcr_clc() with same nc_id contains OpenMP critical region */
  /* Purpose: Multi-slab algorithm (recursive routine, returns a single slab pointer) */

  const char fnc_nm[]="nco_msa_rcr_clc()"; /* [sng] Function name */

  int idx;
  int nbr_slb;
  void *vp;

  if(dpt_crr == dpt_crr_max) goto read_lbl;

  nbr_slb=lmt_lst[dpt_crr]->lmt_dmn_nbr;

  if(nbr_slb == 1){
    lmt[dpt_crr]=lmt_lst[dpt_crr]->lmt_dmn[0];
    vp=nco_msa_rcr_clc(dpt_crr+1,dpt_crr_max,lmt,lmt_lst,vara);
    return vp;
  } /* end if */

  /* Multiple hyperslabs */
  if(nbr_slb > 1){

  /* True if wrapped dims or slabs DO NOT overlap or user-specified order */
  if(lmt_lst[dpt_crr]->WRP || lmt_lst[dpt_crr]->MSA_USR_RDR){
    long var_sz=1L;
    long lcnt;

    char *cp_wrp;
    char *cp_stp;
    char *slb;

    ptrdiff_t slb_sz;
    ptrdiff_t cp_inc;
    ptrdiff_t cp_max;
    ptrdiff_t cp_fst;

    for(idx=0;idx<dpt_crr_max;idx++)
       var_sz*=(idx<dpt_crr ? lmt[idx]->cnt : lmt_lst[idx]->dmn_cnt);

    /* Used nco_callloc() for unknown reasons until 20140930 */
    /*    vp=(void *)nco_calloc((size_t)var_sz,nco_typ_lng(vara->type));*/
    vp=(void *)nco_malloc(var_sz*nco_typ_lng_udt(vara->nc_id,vara->type));

    lcnt=nco_typ_lng_udt(vara->nc_id,vara->type);
    for(idx=dpt_crr+1;idx<dpt_crr_max;idx++) 
       lcnt*=lmt_lst[idx]->dmn_cnt;

    cp_inc=(ptrdiff_t)(lcnt*lmt_lst[dpt_crr]->dmn_cnt);
    cp_max=(ptrdiff_t)(var_sz*nco_typ_lng_udt(vara->nc_id,vara->type));

    cp_fst=0L;

    for(idx=0;idx<nbr_slb;idx++){
      lmt[dpt_crr]=lmt_lst[dpt_crr]->lmt_dmn[idx];
      /* NB: nco_msa_rcr_clc() with same nc_id contains OpenMP critical region */
      cp_wrp=(char *)nco_msa_rcr_clc(dpt_crr+1,dpt_crr_max,lmt,lmt_lst,vara);

      cp_stp=(char *)vp+cp_fst;
      slb=cp_wrp;
      slb_sz=(ptrdiff_t)(lcnt*(lmt_lst[dpt_crr]->lmt_dmn[idx]->cnt));
      while(cp_stp-(char *)vp < cp_max)
      {
	(void)memcpy(cp_stp,slb,(size_t)slb_sz);
	slb+=slb_sz;
	cp_stp+=cp_inc;
      } 
      cp_fst+=slb_sz;      
      cp_wrp=(char*)nco_free(cp_wrp); 
    } 
    vara->sz=var_sz; 
  }
  /* deal with possibly overlapping slabs */
  else
  { 
  
    int slb_idx;
    long var_sz=1L;
    long lcnt;
    long *indices;

    ptrdiff_t slb_sz;
    ptrdiff_t slb_stp;
    ptrdiff_t cp_inc;
    ptrdiff_t cp_max;
    ptrdiff_t cp_fst;

    char **cp_wrp;
    char *cp_stp;
    char *slb;
    lmt_sct lmt_ret;

    indices=(long *)nco_malloc(nbr_slb*sizeof(long));
    cp_wrp=(char **)nco_malloc(nbr_slb*sizeof(void *));

    for(idx=0;idx<nbr_slb;idx++)
    {
      lmt[dpt_crr]=lmt_lst[dpt_crr]->lmt_dmn[idx];
      /* NB: nco_msa_rcr_clc() with same nc_id contains OpenMP critical region */
      cp_wrp[idx]=(char *)nco_msa_rcr_clc(dpt_crr+1,dpt_crr_max,lmt,lmt_lst,vara);
    } /* end loop over idx */

    for(idx=0;idx<dpt_crr_max;idx++) var_sz*=(idx<dpt_crr ? lmt[idx]->cnt : lmt_lst[idx]->dmn_cnt);

    /* Used nco_callloc() for unknown reasons until 20140930 */
    /*    vp=(void *)nco_calloc((size_t)var_sz,nco_typ_lng(vara->type));*/
    vp=(void *)nco_malloc(var_sz*nco_typ_lng_udt(vara->nc_id,vara->type));

    lcnt=nco_typ_lng_udt(vara->nc_id,vara->type);
    for(idx=dpt_crr+1;idx<dpt_crr_max;idx++) lcnt*=lmt_lst[idx]->dmn_cnt;

    cp_inc=(ptrdiff_t)(lcnt*lmt_lst[dpt_crr]->dmn_cnt);
    cp_max=(ptrdiff_t)(var_sz*nco_typ_lng_udt(vara->nc_id,vara->type));

    for(idx=0;idx<nbr_slb;idx++) indices[idx]=lmt_lst[dpt_crr]->lmt_dmn[idx]->srt;

    cp_fst=0L;

    /* Multiple hyper-slabs */
    while(nco_msa_clc_idx(True,lmt_lst[dpt_crr],&indices[0],&lmt_ret,&slb_idx))
    {
      cp_stp=(char *)vp+cp_fst;
      slb=cp_wrp[slb_idx]+(ptrdiff_t)(lmt_ret.srt*lcnt);
      slb_stp=(ptrdiff_t)(lcnt*(lmt_lst[dpt_crr]->lmt_dmn[slb_idx]->cnt));
      slb_sz=(ptrdiff_t)(lmt_ret.cnt*lcnt);

      while(cp_stp-(char *)vp < cp_max)
      {
	(void)memcpy(cp_stp,slb,(size_t)slb_sz);
	slb+=slb_stp;
	cp_stp+=cp_inc;
      } /* end while */
      cp_fst+=slb_sz;
    } /* end while */


    for(idx=0;idx<nbr_slb;idx++) 
       cp_wrp[idx]=(char *)nco_free(cp_wrp[idx]);

    indices=(long *)nco_free(indices);
    cp_wrp=(char **)nco_free(cp_wrp);

    vara->sz=var_sz;

  } /* endif multiple hyperslabs */
   
  return vp;
  } 

read_lbl:
  { 
    int dmn_srd_nbr=0;
    long var_sz=1L;
    long srd_prd=1L; /* Product of strides */
    long *dmn_srt;
    long *dmn_cnt;
    long *dmn_srd;

    dmn_srt=(long *)nco_malloc(dpt_crr_max*sizeof(long));
    dmn_cnt=(long *)nco_malloc(dpt_crr_max*sizeof(long));
    dmn_srd=(long *)nco_malloc(dpt_crr_max*sizeof(long));

    for(idx=0;idx<dpt_crr_max;idx++){
      dmn_srt[idx]=lmt[idx]->srt;
      dmn_cnt[idx]=lmt[idx]->cnt;
      dmn_srd[idx]=lmt[idx]->srd;
      var_sz*=dmn_cnt[idx];
      srd_prd*=lmt[idx]->srd;
      if(lmt[idx]->srd > 1L) dmn_srd_nbr++;
    } /* end loop over idx */

    vp=(void *)nco_malloc(var_sz*nco_typ_lng_udt(vara->nc_id,vara->type));

    /* Block is critical/thread-safe for identical/distinct in_id's */
    { /* begin potential OpenMP critical */
      /* Check for stride */
      if(var_sz > 0L){ /* Allow for zero-size record variables TODO nco711 */
	/* 20140304: User reported extreme slowdown accessing 3-D data with strides across DAP connection
	   Info from R. Signell and J. Whitaker suggests using nco_get_vars() instead of nco_get_varm() may be crucial
	   Tests bear this out: get_varm() is tortuously slow across DAP 
	   Always use get_vars() for strides unless using full get_varm() mapping vector */
	/*          (void)nco_get_varm(vara->nc_id,vara->id,dmn_srt,dmn_cnt,dmn_srd,(long *)NULL,vp,vara->type); */
        if(srd_prd == 1L){
	  (void)nco_get_vara(vara->nc_id,vara->id,dmn_srt,dmn_cnt,vp,vara->type); 
	}else{
	  /* 20140926: nc_get_vars() performs poorly on netCDF4 files
	     Long investigation sparked by Parker Norton on 20140718 revealed nc_get_vars() calls nc_get_vara() once 
	     per element in the strided hyperslab. This quickly becomes unusable for large hyperslabs.
	     Ultimate fix may be using HDF5 algorithm H5S_set_hyperslab() as described in Jira NCF-301
	     Until then, should decompose a single-stride request into a loop over contiguous non-stride requests
	     20180921: netCDF 4.6.2 appears to have the necessary fix
	     https://github.com/Unidata/netcdf-c/pull/1001 */

	  int fl_in_fmt; /* [enm] Input file format */

	  nco_bool USE_NC4_SRD_WORKAROUND=False; /* [flg] Use faster access for strided hyperslabs in netCDF4 files */

	  (void)nco_inq_format(vara->nc_id,&fl_in_fmt);

	  /* 20170207: Turn-off USE_NC4_SRD_WORKAROUND unless non-unity stride is only in first dimension
	     20180921: Allow USE_NC4_SRD_WORKAROUND only for netCDF <= 4.6.1 */
	  if(NC_LIB_VERSION <= 461 && (fl_in_fmt == NC_FORMAT_NETCDF4 || fl_in_fmt == NC_FORMAT_NETCDF4_CLASSIC) && (dmn_srd_nbr == 1) && (dmn_srd[0] != 1L)) USE_NC4_SRD_WORKAROUND=True;

	  if(!USE_NC4_SRD_WORKAROUND){
	    if(nco_dbg_lvl_get() >= nco_dbg_var && srd_prd > 1L) (void)fprintf(stderr,"%s: INFO %s reports calling nco_get_vars() for strided hyperslab access. In case of slow response, please ask NCO developers to extend USE_NC4_SRD_WORKAROUND to handle your use-case.\n",nco_prg_nm_get(),fnc_nm);
	    (void)nco_get_vars(vara->nc_id,vara->id,dmn_srt,dmn_cnt,dmn_srd,vp,vara->type);
	  }else{
	    /* 20170207: Original USE_NC4_SRD_WORKAROUND code was broken because it was applied to interior dimensions but did not account for interleaving
	       20170208: New USE_NC4_SRD_WORKAROUND code only applies to non-unity stride in first dimension */
	    int srd_nbr; /* [nbr] Number of strides requested in the single strided dimension (not number of strided dimensions) */
	    int srd_idx; /* [idx] Counter for how many times to call nco_get_var() */
	    int idx_srd; /* [idx] Index of strided dimension */
	    int dmn_idx; /* [idx] Index for dimensions */
	    long srd_sz=1L; /* [nbr] Size of hyperslab of one stride */
	    char *cp_srd; /* [ptr] Use char pointer because arithmetic with void pointer is non-standard */

	    if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stderr,"%s: INFO %s using USE_NC4_SRD_WORKAROUND for faster access to strided hyperslabs in netCDF4 datasets\n",nco_prg_nm_get(),fnc_nm);

	    /* Strided dimension must be first dimension */
	    idx_srd=0;

	    /* Find size of hyperslab of each stride */
	    for(dmn_idx=0;dmn_idx<dpt_crr_max;dmn_idx++)
	      if(dmn_idx != idx_srd) srd_sz*=dmn_cnt[dmn_idx];

	    /* Number of strides within memory */
	    srd_nbr=var_sz/srd_sz;
	    assert(srd_nbr == dmn_cnt[idx_srd]);

	    if(nco_dbg_lvl_get() >= nco_dbg_io){
	      (void)fprintf(stderr,"%s: INFO %s USE_NC4_SRD_WORKAROUND reports variable %s has dmn_nbr = %d, dpt_crr_max = %d, idx_srd = %d, srd_nbr = %d, srd_sz = %ld\n",nco_prg_nm_get(),fnc_nm,vara->nm,vara->nbr_dim,dpt_crr_max,idx_srd,srd_nbr,srd_sz);
	      (void)fprintf(stdout,"%s: %s hyperslabbing variable %s, var_sz=%ld\n",nco_prg_nm_get(),fnc_nm,vara->nm,var_sz);
	      for(idx=0;idx<dpt_crr_max;idx++) (void)fprintf(stdout,"dmn_idx=%d srt=%ld, cnt=%ld, srd=%ld\n",idx,dmn_srt[idx],dmn_cnt[idx],dmn_srd[idx]);
	    } /* endif dbg */

	    cp_srd=(char *)vp;
	    /* Replace multiple strides by loop over single strides */
	    dmn_cnt[idx_srd]=1L;
	    for(srd_idx=0;srd_idx<srd_nbr;srd_idx++){
	      /* Point start vector to next stride on disk */
	      if(srd_idx != 0) dmn_srt[idx_srd]+=dmn_srd[idx_srd];
	      /* Place results in next portion of RAM */
	      if(srd_idx != 0) cp_srd+=srd_sz*nco_typ_lng_udt(vara->nc_id,vara->type);
	      (void)nco_get_vara(vara->nc_id,vara->id,dmn_srt,dmn_cnt,(void *)cp_srd,vara->type);
	    } /* end loop over srd */
	  } /* endif workaround */
	} /* srd_prd != 1L */
      } /* end if var_sz */
    } /* end potential OpenMP critical */

    dmn_srt=(long *)nco_free(dmn_srt);
    dmn_cnt=(long *)nco_free(dmn_cnt);
    dmn_srd=(long *)nco_free(dmn_srd);

    /* Put size into vara */
    vara->sz=var_sz;
    return vp;
  } /* end read_lbl */

} /* end nco_msa_rcr_clc() */

void 
nco_msa_prn_idx(lmt_msa_sct *lmt_i)
{
  int slb_nbr;
  int idx;
  int size=lmt_i->lmt_dmn_nbr;
  long *indices;
  lmt_sct lmt;

  indices=(long *)nco_malloc(size *sizeof(long));

  (void)fprintf(stdout,"name=%s total size=%ld\n",lmt_i->dmn_nm,lmt_i->dmn_cnt);

  for(idx=0;idx<size;idx++) indices[idx]=lmt_i->lmt_dmn[idx]->srt;

  while(nco_msa_clc_idx(False,lmt_i,&indices[0],&lmt,&slb_nbr)) (void)fprintf(stdout,"slb_nbr=%d, srt=%ld, end=%ld, cnt=%ld, srd=%ld\n",slb_nbr,lmt.srt,lmt.end,lmt.cnt,lmt.srd);
} /* end nco_msa_prn_idx() */

nco_bool /* [flg] There are more limits to process in the slab */
nco_msa_clc_idx
(nco_bool NORMALIZE,
 lmt_msa_sct *lmt_a, /* I list of lmts for each dimension  */
 long *indices, /* I/O so routine can keep track of where its at */
 lmt_sct *lmt, /* O Output hyperslab */
 int *slb) /* slab which above limit refers to */ 
{
  /* A messy, unclear, inefficient routine that needs re-writing
     if NORMALIZE then return the slab number and the hyperslab in lmt
     NB: This is the slab _WITHIN_ the slab so stride is ALWAYS 1 */
  int sz_idx;
  int size=lmt_a->lmt_dmn_nbr;
  int prv_slb=0;
  int crr_slb=0;

  long crr_idx;
  long prv_idx=long_CEWI;
  long cnt=0L;

  nco_bool *mnm;
  nco_bool rcd;

  mnm=(nco_bool *)nco_malloc(size*sizeof(nco_bool));

  lmt->srt=-1L;
  lmt->cnt=0L;
  lmt->srd=0L;

  while(++cnt){
    crr_idx=nco_msa_min_idx(indices,mnm,size);

    crr_slb=-1;
    for(sz_idx=0;sz_idx<size;sz_idx++){
      if(mnm[sz_idx]){crr_slb=sz_idx;break;}
    } /* !sz_idx */
    
    if(crr_slb == -1){
      if(lmt->srt == -1L){
	rcd=False;
	goto cln_and_xit;
      }else break;
    } /* endif */
    
    if(mnm[prv_slb]) crr_slb=prv_slb;
    
    if(lmt->srt > -1L && crr_slb != prv_slb) break;
    
    if(lmt->cnt > 1L){
      (lmt->cnt)++;
      lmt->end=crr_idx;
    } /* end if */
    
    if(lmt->cnt == 1L){
      lmt->cnt=2L;
      lmt->srd=crr_idx-prv_idx;
      lmt->end=crr_idx;
    } /* end if */
    
    if(lmt->srt == -1L){
      lmt->srt=crr_idx;
      lmt->cnt=1L;
      lmt->end=crr_idx;
      lmt->srd=1L;
    } /* end if */
    
    for(sz_idx=0;sz_idx<size;sz_idx++){
      if(mnm[sz_idx]){
	indices[sz_idx]+=lmt_a->lmt_dmn[sz_idx]->srd;
	if(indices[sz_idx] > lmt_a->lmt_dmn[sz_idx]->end) indices[sz_idx]=-1L;
      }
    } /* end loop over sz_idx */
    prv_idx=crr_idx;
    prv_slb=crr_slb;
  } /* end while */
  
  *slb=prv_slb;
  
  /* Normalize slab */
  if(NORMALIZE){
    lmt->srt=(lmt->srt-lmt_a->lmt_dmn[*slb]->srt)/(lmt_a->lmt_dmn[*slb]->srd);
    lmt->end=(lmt->end-lmt_a->lmt_dmn[*slb]->srt)/(lmt_a->lmt_dmn[*slb]->srd);
    lmt->srd=1L;
  } /* end if */
  
  rcd=True;
  
  /* Jump here if only one string */
 cln_and_xit:
  mnm=(nco_bool *)nco_free(mnm);

  return rcd;
} /* end nco_msa_clc_idx() */

void
nco_msa_ram_2_dsk /* Convert hyperslab indices (in RAM) to hyperlsab indices relative to disk */
(long *dmn_sbs_ram,   
 lmt_msa_sct **lmt_msa, 
 int nbr_dim,
 long *dmn_sbs_dsk,
 nco_bool flg_free)
{
  /* Given a set of RAM indices, routine finds next set of disk indices
     Only works if the indices fed to it are continuous */
  int idx;
  int jdx;
  int size;
  static int initialize;
  static long **dmn_indices;
  static long *dmn_sbs_prv;
  static nco_bool mnm[100];

  if(!initialize){
    dmn_sbs_prv=(long *)nco_malloc(nbr_dim*sizeof(long));
    dmn_indices=(long **)nco_malloc(nbr_dim*sizeof(long *));
    for(idx=0;idx<nbr_dim;idx++){
      dmn_indices[idx]=(long *)nco_malloc(lmt_msa[idx]->lmt_dmn_nbr*sizeof(long));
      for(jdx=0;jdx<lmt_msa[idx]->lmt_dmn_nbr;jdx++)
        dmn_indices[idx][jdx]=lmt_msa[idx]->lmt_dmn[jdx]->srt;
      dmn_sbs_prv[idx]=-1L;
    }
    initialize=True;
  }
  for(idx=0;idx <nbr_dim;idx ++){
    size=lmt_msa[idx]->lmt_dmn_nbr;
    if(dmn_sbs_ram[idx] == dmn_sbs_prv[idx]) continue;

    if(lmt_msa[idx]->NON_HYP_DMN){
      dmn_sbs_dsk[idx]=dmn_sbs_ram[idx];
      continue;
    }

    /* Re-initialize indices if 0 */
    if(dmn_sbs_ram[idx] == 0) 
      for(jdx=0;jdx<size;jdx++)
        dmn_indices[idx][jdx]=lmt_msa[idx]->lmt_dmn[jdx]->srt;

    /* Deal with wrapping, i.e., we have 2 hyperslabs */
    if(lmt_msa[idx]->WRP){
      if(dmn_indices[idx][0]<lmt_msa[idx]->dmn_sz_org){
        dmn_sbs_dsk[idx]=dmn_indices[idx][0];
        dmn_indices[idx][0]+=lmt_msa[idx]->lmt_dmn[0]->srd;
      }else{
        dmn_sbs_dsk[idx]=dmn_indices[idx][1];
        dmn_indices[idx][1]+=lmt_msa[idx]->lmt_dmn[1]->srd;
      }
      continue;
    }

    dmn_sbs_dsk[idx]=nco_msa_min_idx(dmn_indices[idx],mnm,size);

    for(jdx=0;jdx<size;jdx++){
      if(mnm[jdx]){
        dmn_indices[idx][jdx]+=lmt_msa[idx]->lmt_dmn[jdx]->srd;
        if(dmn_indices[idx][jdx] > lmt_msa[idx]->lmt_dmn[jdx]->end) dmn_indices[idx][jdx]=-1L;
      }
    } /* end for  jdx */
  } /* end for idx */ 

  for(idx=0;idx<nbr_dim;idx++) dmn_sbs_prv[idx]=dmn_sbs_ram[idx];

  /* Free static space on last call */
  if(flg_free){
    (void)nco_free(dmn_sbs_prv);
    for(idx=0;idx<nbr_dim;idx++) (void)nco_free(dmn_indices[idx]);
    (void)nco_free(dmn_indices);
    initialize=False;
  }
} /* end nco_msa_ram_2_dsk() */

void 
nco_msa_clc_cnt
(lmt_msa_sct *lmt_lst)
{
  int idx;
  long cnt=0;
  int size=lmt_lst->lmt_dmn_nbr;
  long *indices;
  nco_bool *mnm;

  /* Degenerate case */
  if(size == 1){
    lmt_lst->dmn_cnt=lmt_lst->lmt_dmn[0]->cnt;
    return;
  } /* end if */

  /* If slabs remain in user-specified order */
  if(lmt_lst->MSA_USR_RDR){
    for(idx=0;idx<size;idx++){
      cnt+=lmt_lst->lmt_dmn[idx]->cnt;
     }
    lmt_lst->dmn_cnt=cnt;
  }else{
    indices=(long *)nco_malloc(size*sizeof(long));
    mnm=(nco_bool *)nco_malloc(size*sizeof(nco_bool));

    /* Initialize indices with srt */
    for(idx=0;idx<size;idx++) indices[idx]=lmt_lst->lmt_dmn[idx]->srt;

    while(nco_msa_min_idx(indices,mnm,size) != LONG_MAX){
      for(idx=0;idx<size;idx++){
        if(mnm[idx]){
          indices[idx]+=lmt_lst->lmt_dmn[idx]->srd;
          if(indices[idx] > lmt_lst->lmt_dmn[idx]->end) indices[idx]=-1L;
        } /* end if */
      } /* end loop over idx */
      cnt++;
    } /* end while */
    lmt_lst->dmn_cnt=cnt;

    indices=(long *)nco_free(indices);
    mnm=(nco_bool  *)nco_free(mnm);
  } /* end else */

  return; /* 20050109: return squelches erroneous gcc-3.4.2 warning */ 
} /* end nco_msa_clc_cnt() */

nco_bool
nco_msa_ovl
(lmt_msa_sct *lmt_lst)
{
  /* Purpose: Return true if limits overlap
     NB: Assumes that limits have been sorted */

  long idx;
  long jdx;
  long sz=lmt_lst->lmt_dmn_nbr;

  lmt_sct **lmt;
  /* defererence */
  lmt=lmt_lst->lmt_dmn;

  for(idx=0L;idx<sz;idx++)
    for(jdx=idx+1L;jdx<sz;jdx++)
      if(lmt[jdx]->srt <= lmt[idx]->end) return True;  

  return False;
} /* end nco_msa_ovl() */

int /* O [enm] Comparison result [<,=,>] 0 iff val_1 [<,==,>] val_2 */
nco_cmp_lmt_srt /* [fnc] Compare two lmt_sct's by srt member */
(const void * vp1, /* I [sct] lmt_sct to compare */
 const void * vp2) /* I [sct] lmt_sct to compare */
{
  const lmt_sct * const lmt1=*((const lmt_sct * const *)vp1);
  const lmt_sct * const lmt2=*((const lmt_sct * const *)vp2); 
  /* fxm: need to avoid compiler warnings. will following work?
  const lmt_sct * const lmt1=(const lmt_sct *)vp1;
  const lmt_sct * const lmt2=(const lmt_sct *)vp2; */

  return lmt1->srt < lmt2->srt ? -1 : (lmt1->srt > lmt2->srt);
} /* end nco_cmp_lmt_srt() */

void nco_msa_qsort_srt
(lmt_msa_sct *lmt_lst)
{
  lmt_sct **lmt;
  long sz;

  sz=lmt_lst->lmt_dmn_nbr;
  lmt=lmt_lst->lmt_dmn;

  if(sz <= 1) return;

  (void)qsort(lmt,(size_t)sz,sizeof(lmt_sct *),nco_cmp_lmt_srt);
} /* end nco_msa_qsort_srt() */

long /* O [idx] Minimum value */
nco_msa_min_idx /* [fnc] Find minimum values in current and return minimum value */
(const long * const current, /* I [idx] Current indices */
 nco_bool * const mnm, /* O [flg] Minimum */
 const int size) /* I [nbr] Size of current and mnm */
{
  int sz_idx;
  long min_val=LONG_MAX;

  for(sz_idx=0;sz_idx<size;sz_idx++)
    if(current[sz_idx] != -1L && current[sz_idx] < min_val) min_val=current[sz_idx];

  for(sz_idx=0;sz_idx<size;sz_idx++)
    mnm[sz_idx]=((current[sz_idx] != -1L && current[sz_idx] == min_val) ? True : False);

  return min_val;
} /* end nco_msa_min_idx() */

void
nco_msa_var_get    /* [fnc] Get variable data from disk taking account of multi-hyperslabs */
(const int in_id,  /* I [id] netCDF input file ID */
 var_sct *var_in,
 lmt_msa_sct * const * lmt_lst, /* I [sct] multi-hyperslab limits */
 int nbr_dmn_fl) /* I [nbr] Number of multi-hyperslab limits */
{
  int idx;
  int jdx;
  int nbr_dim;
  nc_type typ_tmp;
  void *void_ptr;
  lmt_msa_sct **lmt_msa;
  lmt_sct **lmt;

  nbr_dim=var_in->nbr_dim;

  /* Refresh nc_id with in_id, NB: makes OpenMP threading work
     Should have been included in release 3.9.5
     Fixes TODO nco956 */
  var_in->nc_id=in_id; 

  /* Scalars */
  if(nbr_dim == 0){
    var_in->val.vp=nco_malloc(nco_typ_lng_udt(in_id,var_in->typ_dsk));
    (void)nco_get_var1(in_id,var_in->id,0L,var_in->val.vp,var_in->typ_dsk);
    goto do_upk;
  } /* end if scalar */

  lmt_msa=(lmt_msa_sct **)nco_malloc(nbr_dim*sizeof(lmt_msa_sct *));
  lmt=(lmt_sct **)nco_malloc(nbr_dim*sizeof(lmt_sct *));

  /* Initialize lmt_msa with multi-limits from lmt_lst limits */
  for(idx=0;idx<nbr_dim;idx++){
    for(jdx=0;jdx<nbr_dmn_fl;jdx++){
      if(!strcmp(var_in->dim[idx]->nm,lmt_lst[jdx]->dmn_nm)){
        lmt_msa[idx]=lmt_lst[jdx];
        break;
      } /* end if */
    } /* end loop over jdx */
  } /* end idx */

  /* Call super-dooper recursive routine */
  /* NB: nco_msa_rcr_clc() with same nc_id contains OpenMP critical region */
  typ_tmp=var_in->type;
  var_in->type=var_in->typ_dsk; 
  void_ptr=nco_msa_rcr_clc((int)0,nbr_dim,lmt,lmt_msa,var_in);

  var_in->type=typ_tmp;
  var_in->val.vp=void_ptr;

  (void)nco_free(lmt_msa);
  (void)nco_free(lmt);

do_upk:
  /* Following code copied from nco_var_get() */
  if(var_in->pck_dsk) var_in=nco_cnv_mss_val_typ(var_in,var_in->typ_dsk);

  /* Type of variable and missing value in memory are now same as type on disk */
  var_in->type=var_in->typ_dsk; /* [enm] Type of variable in RAM */

  /* Packing in RAM is now same as packing on disk pck_dbg 
     fxm: This nco_pck_dsk_inq() call is never necessary for non-packed variables */
  (void)nco_pck_dsk_inq(in_id,var_in);

  /* Packing/Unpacking */
  if(nco_is_rth_opr(nco_prg_id_get())){
    /* Arithmetic operators must unpack variables before performing arithmetic
       Otherwise arithmetic will produce garbage results */
    /* 20050519: Not sure why I originally made nco_var_upk() call SMP-critical
       20050629: Making this region multi-threaded causes no problems */
    if(var_in->pck_dsk) var_in=nco_var_upk(var_in);
  } /* endif arithmetic operator */

  return;
} /* end nco_msa_var_get() */

void /* Initilaize lmt_msa_sct's */ 
nco_msa_lmt_all_ntl
(int in_id,
 nco_bool MSA_USR_RDR,
 lmt_msa_sct **lmt_all_lst,
 int nbr_dmn_fl,
 lmt_sct** lmt,
 int lmt_nbr)
{
  int idx;
  int jdx;
  int rec_dmn_id=NCO_REC_DMN_UNDEFINED;
  long dmn_sz;
  char dmn_nm[NC_MAX_NAME];

  lmt_sct *lmt_rgl;
  lmt_msa_sct * lmt_all_crr;

  (void)nco_inq(in_id,(int*)NULL,(int*)NULL,(int *)NULL,&rec_dmn_id);

  for(idx=0;idx<nbr_dmn_fl;idx++){
    (void)nco_inq_dim(in_id,idx,dmn_nm,&dmn_sz);
    lmt_all_crr=lmt_all_lst[idx]=(lmt_msa_sct *)nco_malloc(sizeof(lmt_msa_sct));
    lmt_all_crr->lmt_dmn=(lmt_sct **)nco_malloc(sizeof(lmt_sct *));
    lmt_all_crr->dmn_nm=strdup(dmn_nm);
    lmt_all_crr->lmt_dmn_nbr=1;
    lmt_all_crr->dmn_sz_org=dmn_sz;
    lmt_all_crr->WRP=False;
    lmt_all_crr->NON_HYP_DMN=True;
    lmt_all_crr->MSA_USR_RDR=False;    

    lmt_all_crr->lmt_dmn[0]=(lmt_sct *)nco_malloc(sizeof(lmt_sct)); 
    /* Dereference */
    lmt_rgl=lmt_all_crr->lmt_dmn[0]; 
    lmt_rgl->nm=strdup(lmt_all_crr->dmn_nm);
    lmt_rgl->id=idx;

    /* NB: nco_lmt_evl() may alter this */
    if(idx==rec_dmn_id) lmt_rgl->is_rec_dmn=True; else lmt_rgl->is_rec_dmn=False;

    lmt_rgl->srt=0L;
    lmt_rgl->end=dmn_sz-1L;
    lmt_rgl->cnt=dmn_sz;
    lmt_rgl->srd=1L;
    lmt_rgl->ssc=1L;
    lmt_rgl->ilv=1L;
    lmt_rgl->flg_mro=False;
    lmt_rgl->flg_ilv=False;
    lmt_rgl->min_sng=NULL;
    lmt_rgl->max_sng=NULL;
    lmt_rgl->srd_sng=NULL;
    lmt_rgl->ssc_sng=NULL;
    lmt_rgl->ilv_sng=NULL;
    lmt_rgl->rbs_sng=NULL;
    lmt_rgl->origin=0.0;

    /* A hack so we know structure has been initialized */
    lmt_rgl->lmt_typ=-1;
  } /* end loop over dimensions */

  /* fxm: subroutine-ize this MSA code block for portability TODO nco926 */
  /* Add user specified limits lmt_all_lst */
  for(idx=0;idx<lmt_nbr;idx++){
    for(jdx=0;jdx<nbr_dmn_fl;jdx++){
      if(!strcmp(lmt[idx]->nm,lmt_all_lst[jdx]->dmn_nm)){   
        lmt_all_crr=lmt_all_lst[jdx];
        lmt_all_crr->NON_HYP_DMN=False;
        if(lmt_all_crr->lmt_dmn[0]->lmt_typ == -1) { 
          /* Free defualt limit set above structure first */
          lmt_all_crr->lmt_dmn[0]=(lmt_sct*)nco_lmt_free(lmt_all_crr->lmt_dmn[0]);
          lmt_all_crr->lmt_dmn[0]=lmt[idx]; 
        }else{ 
          lmt_all_crr->lmt_dmn=(lmt_sct**)nco_realloc(lmt_all_crr->lmt_dmn,((lmt_all_crr->lmt_dmn_nbr)+1)*sizeof(lmt_sct *));
          lmt_all_crr->lmt_dmn[(lmt_all_crr->lmt_dmn_nbr)++]=lmt[idx];
        } /* endif */
        break;
      } /* end if */
    } /* end loop over dimensions */
    /* Dimension in limit not found */
    if(jdx == nbr_dmn_fl){
      (void)fprintf(stderr,"Unable to find limit dimension %s in list\n",lmt[idx]->nm);
      nco_exit(EXIT_FAILURE);
    } /* end if err */
  } /* end loop over idx */       

  /* fxm: subroutine-ize this MSA code block for portability TODO nco926 */
  for(idx=0;idx<nbr_dmn_fl;idx++){
    nco_bool flg_ovl;

    /* NB: ncra/ncrcat have only one limit for record dimension so skip 
    evaluation otherwise this messes up multi-file operation */
    if(lmt_all_lst[idx]->lmt_dmn[0]->is_rec_dmn && (nco_prg_id_get() == ncra || nco_prg_id_get() == ncrcat)) continue;

    /* Split-up wrapped limits */   
    (void)nco_msa_wrp_splt(lmt_all_lst[idx]);

    /* NB: Wrapped hyperslabs are dimensions broken into the "wrong" order,
    e.g., from -d time,8,2 broken into -d time,8,9 -d time,0,2
    WRP flag set only when list contains dimensions split as above */
    if(lmt_all_lst[idx]->WRP == True){
      /* Find and store size of output dim */  
      (void)nco_msa_clc_cnt(lmt_all_lst[idx]);       
      continue;
    } /* endif */

    /* Single slab---no analysis needed */  
    if(lmt_all_lst[idx]->lmt_dmn_nbr == 1){
      (void)nco_msa_clc_cnt(lmt_all_lst[idx]);       
      continue;    
    } /* endif */

    if(MSA_USR_RDR){
      lmt_all_lst[idx]->MSA_USR_RDR=True;
      /* Find and store size of output dimension */  
      (void)nco_msa_clc_cnt(lmt_all_lst[idx]);       
      continue;
    } /* endif */

    /* Sort limits */
    (void)nco_msa_qsort_srt(lmt_all_lst[idx]);
    /* Check for overlap */
    flg_ovl=nco_msa_ovl(lmt_all_lst[idx]);  
    if(flg_ovl==False) lmt_all_lst[idx]->MSA_USR_RDR=True;

    /* Find and store size of output dimension */  
    (void)nco_msa_clc_cnt(lmt_all_lst[idx]);       
    if(nco_dbg_lvl_get() >= nco_dbg_fl){
      if(flg_ovl) (void)fprintf(stdout,"%s: dimension \"%s\" has overlapping hyperslabs\n",nco_prg_nm_get(),lmt_all_lst[idx]->dmn_nm); else (void)fprintf(stdout,"%s: dimension \"%s\" has distinct hyperslabs\n",nco_prg_nm_get(),lmt_all_lst[idx]->dmn_nm); 
    } /* endif */

  } /* end idx */    

} /* end nco_msa_lmt_all_ntl() */

void
nco_msa_var_val_cpy /* [fnc] Copy variables data from input to output file */
(const int in_id, /* I [enm] netCDF file ID */
 const int out_id, /* I [enm] netCDF output file ID */
 var_sct ** const var, /* I/O [sct] Variables to copy to output file */
 const int nbr_var,  /* I [nbr] Number of variables */
 lmt_msa_sct * const * lmt_lst, /* I multi-hyperslab limits */
 int nbr_dmn_fl) /* I [nbr] Number of multi-hyperslab limits */
{
  /* NB: nco_msa_var_val_cpy() contains OpenMP critical region */
  /* Purpose: Copy every variable in input variable structure list 
  from input file to output file. Only data (not metadata) are copied. */

  int idx;
  int jdx;
  int kdx;
  int nbr_dim;
  int dmn_idx; 
  long srd_prd=1L;  /* [nbr] Product of strides */

  for(idx=0;idx<nbr_var;idx++){

    nbr_dim=var[idx]->nbr_dim;

    /* Scalars */
    if(nbr_dim==0){
      var[idx]->val.vp=nco_malloc(nco_typ_lng_udt(in_id,var[idx]->type));
      (void)nco_get_var1(in_id,var[idx]->id,0L,var[idx]->val.vp,var[idx]->type);
    }else{
      lmt_msa_sct **lmt_msa;
      lmt_sct **lmt;

      lmt_msa=(lmt_msa_sct **)nco_malloc(nbr_dim*sizeof(lmt_msa_sct *));
      lmt=(lmt_sct **)nco_malloc(nbr_dim*sizeof(lmt_sct *));

      /* Initialize lmt_msa with multi-limits from lmt_lst limits */
      for(jdx=0;jdx<nbr_dim;jdx++){
        for(kdx=0;kdx<nbr_dmn_fl;kdx++){
          if(!strcmp(var[idx]->dim[jdx]->nm,lmt_lst[kdx]->dmn_nm ) ){
            lmt_msa[jdx]=lmt_lst[kdx];
            break;
          } /* end if */
        } /* end loop over jdx */
      } /* end idx */

      /* Call super-dooper recursive routine */
      /* NB: nco_msa_rcr_clc() with same nc_id contains OpenMP critical region */
      var[idx]->val.vp=nco_msa_rcr_clc((int)0,nbr_dim,lmt,lmt_msa,var[idx]);

      (void)nco_free(lmt_msa);
      (void)nco_free(lmt);
    } /* end else nbr_dim */

    /*(void)nco_msa_var_get(in_id,var[idx],lmt_lst,nbr_dmn_fl); */  

    /* Put variable data */
    if(var[idx]->nbr_dim == 0)
      nco_put_var1(out_id,var[idx]->xrf->id,var[idx]->xrf->srt,var[idx]->val.vp,var[idx]->type);
    else{ /* end if variable is scalar */
      if(var[idx]->sz > 0){ /* Do nothing for zero-size record variables */
        /* Is stride > 1? */
        for(dmn_idx=0;dmn_idx<var[idx]->nbr_dim;dmn_idx++) srd_prd*=var[idx]->srd[dmn_idx];

        if(srd_prd == 1L)
          nco_put_vara(out_id,var[idx]->xrf->id,var[idx]->xrf->srt,var[idx]->xrf->cnt,var[idx]->val.vp,var[idx]->type);
        else
          nco_put_vars(out_id,var[idx]->xrf->id,var[idx]->xrf->srt,var[idx]->xrf->cnt,var[idx]->xrf->srd,var[idx]->val.vp,var[idx]->type);

      } /* end if var_sz */
    } /* end if variable is an array */
    var[idx]->val.vp=var[idx]->xrf->val.vp=nco_free(var[idx]->val.vp);
  } /* end loop over idx */

} /* end nco_msa_var_val_cpy() */

void
nco_msa_wrp_splt_trv   /* [fnc] Split wrapped dimensions (GTT version) */
(dmn_trv_sct *dmn_trv) /* [sct] Dimension structure from GTT */
{
  /* Purpose: Same as nco_msa_wrp_splt() but applied to the Dimension structure from GTT 
  Differences from nco_msa_wrp_splt() are marked "trv" 
  Goal here is to replace a wrapped limit by 2 non-wrapped limits 
  Wrapped hyperslabs are dimensions broken into the "wrong" order,e.g. from
  -d time,8,2 broken into -d time,8,9 -d time,0,2 
  WRP flag set only when list contains dimensions split as above

  Tests:
  ncks -d time,8,2 -v time -H ~/nco/data/in_grp.nc */

  const char fnc_nm[]="nco_msa_wrp_splt_trv()"; /* [sng] Function name  */

  int idx;
  int jdx;
  int size=dmn_trv->lmt_msa.lmt_dmn_nbr; /* [nbr] Number of limit structures */
  long dmn_sz_org=dmn_trv->sz; /* [nbr] Size of dimension */
  long srt;
  long cnt;
  long srd;
  long kdx=0; 
  lmt_sct *lmt_wrp;

  for(idx=0;idx<size;idx++){

    if(dmn_trv->lmt_msa.lmt_dmn[idx]->srt > dmn_trv->lmt_msa.lmt_dmn[idx]->end){

      if(nco_dbg_lvl_get() == nco_dbg_dev) (void)fprintf(stdout,"%s: INFO %s dimension <%s> has wrapped limits (%li->%li):\n",nco_prg_nm_get(),fnc_nm,dmn_trv->nm_fll,dmn_trv->lmt_msa.lmt_dmn[idx]->srt,dmn_trv->lmt_msa.lmt_dmn[idx]->end);

      lmt_wrp=(lmt_sct *)nco_malloc(2*sizeof(lmt_sct));

      /* "trv": Initialize */
      (void)nco_lmt_init(&lmt_wrp[0]);
      (void)nco_lmt_init(&lmt_wrp[1]);

      srt=dmn_trv->lmt_msa.lmt_dmn[idx]->srt;
      cnt=dmn_trv->lmt_msa.lmt_dmn[idx]->cnt;
      srd=dmn_trv->lmt_msa.lmt_dmn[idx]->srd;

      for(jdx=0;jdx<cnt;jdx++){
        kdx=(srt+srd*jdx)%dmn_sz_org;
        if(kdx<srt) break;
      } /* end loop over jdx */

      /* "trv": Instead of shallow copy in nco_msa_wrp_splt(), make a deep copy to the 2 new limits lmt_wrp */ 
      (void)nco_lmt_cpy(dmn_trv->lmt_msa.lmt_dmn[idx],&lmt_wrp[0]);
      (void)nco_lmt_cpy(dmn_trv->lmt_msa.lmt_dmn[idx],&lmt_wrp[1]);

      lmt_wrp[0].srt=srt;

      if(jdx == 1){
        lmt_wrp[0].end=srt;
        lmt_wrp[0].cnt=1L;
        lmt_wrp[0].srd=1L;
      }else{
        lmt_wrp[0].end=srt+srd*(jdx-1);
        lmt_wrp[0].cnt=jdx;
        lmt_wrp[0].srd=srd;
      } /* end else */

      lmt_wrp[1].srt=kdx;
      lmt_wrp[1].cnt=cnt-lmt_wrp[0].cnt;
      if(lmt_wrp[1].cnt == 1L){
        lmt_wrp[1].end=kdx;
        lmt_wrp[1].srd=1L;
      }else{
        lmt_wrp[1].end=kdx+(lmt_wrp[1].cnt-1)*srd;
        lmt_wrp[1].srd=srd;
      } /* end else */

      if(nco_dbg_lvl_get() == nco_dbg_dev){
        (void)fprintf(stdout,"%s: INFO %s wrapped limits for <%s> found: ",nco_prg_nm_get(),fnc_nm,dmn_trv->nm_fll);
        (void)fprintf(stdout,"%d:\n",dmn_trv->lmt_msa.lmt_dmn_nbr);
      }

      /* "trv": Insert 2 non-wrapped limits */ 

      /* Current number of dimension limits for this table dimension  */
      int lmt_dmn_nbr=dmn_trv->lmt_msa.lmt_dmn_nbr;

      /* Index of new limit  */
      int lmt_new_idx=idx+1;

      /* Make space for 1 more limit  */
      dmn_trv->lmt_msa.lmt_dmn=(lmt_sct **)nco_realloc(dmn_trv->lmt_msa.lmt_dmn,(lmt_dmn_nbr+1)*sizeof(lmt_sct *));

      /* Alloc the extra limit  */
      dmn_trv->lmt_msa.lmt_dmn[lmt_new_idx]=(lmt_sct *)nco_malloc(sizeof(lmt_sct));

      /* Initialize the extra limit */
      (void)nco_lmt_init(dmn_trv->lmt_msa.lmt_dmn[lmt_new_idx]);

      /* Insert the limits to table (allocated; idx was already there; lmt_new_idx was alloced here)   */
      (void)nco_lmt_cpy(&lmt_wrp[0],dmn_trv->lmt_msa.lmt_dmn[idx]);
      (void)nco_lmt_cpy(&lmt_wrp[1],dmn_trv->lmt_msa.lmt_dmn[lmt_new_idx]);

      /* Update number of dimension limits for this table dimension  */
      dmn_trv->lmt_msa.lmt_dmn_nbr++;

      /* Update current index of dimension limits for this table dimension  */
      dmn_trv->lmt_msa.lmt_crr++;

      if (nco_dbg_lvl_get() == nco_dbg_dev){
        (void)fprintf(stdout, "%s: INFO %s dimension <%s> new limits inserted (%li->%li) - (%li->%li):\n", 
          nco_prg_nm_get(), fnc_nm, dmn_trv->nm_fll, dmn_trv->lmt_msa.lmt_dmn[idx]->srt, dmn_trv->lmt_msa.lmt_dmn[idx]->end, dmn_trv->lmt_msa.lmt_dmn[lmt_new_idx]->srt, dmn_trv->lmt_msa.lmt_dmn[lmt_new_idx]->end);
      }
    } /* endif srt > end */
  } /* end loop over size */

  /* Check if genuine wrapped co-ordinate */
  if(size == 1 && dmn_trv->lmt_msa.lmt_dmn_nbr == 2) dmn_trv->lmt_msa.WRP=True;

} /* End nco_msa_wrp_splt_trv() */


void 
nco_msa_clc_cnt_trv     /* [fnc] Calculate size of  multiple hyperslab (GTT version) */ 
(dmn_trv_sct *dmn_trv)  /* [sct] Dimension structure from GTT */
{
  /* Purpose: Same as nco_msa_clc_cnt() but applied to the Dimension structure from GTT */

  int idx;
  long cnt=0;
  int size=dmn_trv->lmt_msa.lmt_dmn_nbr;   /* [nbr] Number of limit structures */
  long *indices;
  nco_bool *mnm;

  /* Degenerate case */
  if(size == 1){
    dmn_trv->lmt_msa.dmn_cnt=dmn_trv->lmt_msa.lmt_dmn[0]->cnt;
    return;
  } /* end if */

  /* If slabs remain in user-specified order */
  if(dmn_trv->lmt_msa.MSA_USR_RDR){
    for(idx=0;idx<size;idx++){
      cnt+=dmn_trv->lmt_msa.lmt_dmn[idx]->cnt;
     }
    dmn_trv->lmt_msa.dmn_cnt=cnt;
  }else{
    indices=(long *)nco_malloc(size*sizeof(long));
    mnm=(nco_bool *)nco_malloc(size*sizeof(nco_bool));

    /* Initialize indices with srt */
    for(idx=0;idx<size;idx++) indices[idx]=dmn_trv->lmt_msa.lmt_dmn[idx]->srt;

    while(nco_msa_min_idx(indices,mnm,size) != LONG_MAX){
      for(idx=0;idx<size;idx++){
        if(mnm[idx]){
          indices[idx]+=dmn_trv->lmt_msa.lmt_dmn[idx]->srd;
          if(indices[idx] > dmn_trv->lmt_msa.lmt_dmn[idx]->end) indices[idx]=-1;
        } /* end if */
      } /* end loop over idx */
      cnt++;
    } /* end while */
    dmn_trv->lmt_msa.dmn_cnt=cnt;

    indices=(long *)nco_free(indices);
    mnm=(nco_bool  *)nco_free(mnm);
  } /* end else */

  return; 
} /* End nco_msa_clc_cnt_trv() */

void             
nco_msa_qsort_srt_trv  /* [fnc] Sort limits by srt values (GTT version) */
(dmn_trv_sct *dmn_trv) /* [sct] Dimension structure from GTT */
{
  /* Purpose: Same as nco_msa_qsort_srt() but applied to the Dimension structure from GTT */

  lmt_sct **lmt;
  long sz;           /* [nbr] Number of limit structures */

  sz=dmn_trv->lmt_msa.lmt_dmn_nbr;
  lmt=dmn_trv->lmt_msa.lmt_dmn;

  if(sz <= 1) return;

  (void)qsort(lmt,(size_t)sz,sizeof(lmt_sct *),nco_cmp_lmt_srt);

} /* End nco_msa_qsort_srt_trv() */

nco_bool                /* O [flg] Return true if limits overlap (GTT version) */
nco_msa_ovl_trv         /* [fnc] See if limits overlap */ 
(dmn_trv_sct *dmn_trv)  /* [sct] Dimension structure from GTT */
{
  /* Purpose: Same as nco_msa_ovl() but applied to the Dimension structure from GTT 
  Return true if limits overlap NB: Assumes that limits have been sorted */

  long idx;
  long jdx;
  long sz=dmn_trv->lmt_msa.lmt_dmn_nbr; /* [nbr] Number of limit structures */

  lmt_sct **lmt;
  /* defererence */
  lmt=dmn_trv->lmt_msa.lmt_dmn;

  for(idx=0; idx<sz; idx++)
    for(jdx=idx+1; jdx<sz ;jdx++)
      if( lmt[jdx]->srt <= lmt[idx]->end) return True;  

  return False;

} /* End nco_msa_ovl_trv() */

void
nco_msa_wrp_splt /* [fnc] Split wrapped dimensions */
(lmt_msa_sct *lmt_lst)
{
  int idx;
  int jdx;
  int size=lmt_lst->lmt_dmn_nbr;
  long dmn_sz_org=lmt_lst->dmn_sz_org;
  long srt;
  long cnt;
  long srd;
  long kdx=0; /*  */
  lmt_sct *lmt_wrp;

  for(idx=0;idx<size;idx++){

    if(lmt_lst->lmt_dmn[idx]->srt > lmt_lst->lmt_dmn[idx]->end){

      lmt_wrp=(lmt_sct *)nco_malloc(2*sizeof(lmt_sct));
      srt=lmt_lst->lmt_dmn[idx]->srt;
      cnt=lmt_lst->lmt_dmn[idx]->cnt;
      srd=lmt_lst->lmt_dmn[idx]->srd;

      for(jdx=0;jdx<cnt;jdx++){
        kdx=(srt+srd*jdx)%dmn_sz_org;
        if(kdx<srt) break;
      } /* end loop over jdx */

      lmt_wrp[0]=*(lmt_lst->lmt_dmn[idx]);
      lmt_wrp[1]=*(lmt_lst->lmt_dmn[idx]);
      lmt_wrp[0].srt=srt;

      if(jdx == 1){
        lmt_wrp[0].end=srt;
        lmt_wrp[0].cnt=1L;
        lmt_wrp[0].srd=1L;
      }else{
        lmt_wrp[0].end=srt+srd*(jdx-1);
        lmt_wrp[0].cnt=jdx;
        lmt_wrp[0].srd=srd;
      } /* end else */

      lmt_wrp[1].srt=kdx;
      lmt_wrp[1].cnt=cnt-lmt_wrp[0].cnt;
      if(lmt_wrp[1].cnt == 1L){
        lmt_wrp[1].end=kdx;
        lmt_wrp[1].srd=1L;
      }else{
        lmt_wrp[1].end=kdx+(lmt_wrp[1].cnt-1)*srd;
        lmt_wrp[1].srd=srd;
      } /* end else */

      /* Insert new limits into array */
      lmt_lst->lmt_dmn[idx]=lmt_wrp;
      lmt_lst->lmt_dmn=(lmt_sct **)nco_realloc(lmt_lst->lmt_dmn,((lmt_lst->lmt_dmn_nbr)+1)*sizeof(lmt_sct *));
      lmt_lst->lmt_dmn[(lmt_lst->lmt_dmn_nbr)++]=++lmt_wrp;
    } /* endif srt > end */
  } /* end loop over size */

  /* Check if genuine wrapped co-ordinate */
  if(size==1 && lmt_lst->lmt_dmn_nbr==2) lmt_lst->WRP=True;
} /* end nco_msa_wrp_splt() */

void
nco_msa_wrp_splt_cpy    /* [fnc] Split wrapped dimensions (make deep copy of new wrapped limits) */
(lmt_msa_sct *lmt_lst)  /* [sct] MSA */
{
  /* Purpose: Same as nco_msa_wrp_splt() but makes a deep copy of new wrapped limits
  Differences from nco_msa_wrp_splt() are marked "trv" 
  Goal here is to replace a wrapped limit by 2 non-wrapped limits 
  Wrapped hyperslabs are dimensions broken into the "wrong" order,e.g. from
  -d time,8,2 broken into -d time,8,9 -d time,0,2 
  WRP flag set only when list contains dimensions split as above

  Tests:
  ncks -d time,8,2 -v time -H ~/nco/data/in_grp.nc
  */

  const char fnc_nm[]="nco_msa_wrp_splt_trv()"; /* [sng] Function name  */

  int idx;
  int jdx;
  int size=lmt_lst->lmt_dmn_nbr;         /* [nbr] Number of limit structures */
  long dmn_sz_org=lmt_lst->dmn_sz_org;   /* [nbr] Size of dimension */
  long srt;
  long cnt;
  long srd;
  long kdx=0; 
  lmt_sct *lmt_wrp;

  for(idx=0;idx<size;idx++){

    if(lmt_lst->lmt_dmn[idx]->srt > lmt_lst->lmt_dmn[idx]->end){

      if(nco_dbg_lvl_get() == nco_dbg_old){
        (void)fprintf(stdout,"%s: INFO %s dimension <%s> has wrapped limits (%li->%li):\n",
          nco_prg_nm_get(),fnc_nm,lmt_lst->dmn_nm,lmt_lst->lmt_dmn[idx]->srt,lmt_lst->lmt_dmn[idx]->end);
      }

      lmt_wrp=(lmt_sct *)nco_malloc(2*sizeof(lmt_sct));

      /* "trv": Initialize  */
      (void)nco_lmt_init(&lmt_wrp[0]);
      (void)nco_lmt_init(&lmt_wrp[1]);

      srt=lmt_lst->lmt_dmn[idx]->srt;
      cnt=lmt_lst->lmt_dmn[idx]->cnt;
      srd=lmt_lst->lmt_dmn[idx]->srd;

      for(jdx=0;jdx<cnt;jdx++){
        kdx=(srt+srd*jdx)%dmn_sz_org;
        if(kdx<srt) break;
      } /* end loop over jdx */

      /* "trv": Instead of shallow copy in nco_msa_wrp_splt(), make a deep copy to the 2 new limits lmt_wrp */ 
      (void)nco_lmt_cpy(lmt_lst->lmt_dmn[idx],&lmt_wrp[0]);
      (void)nco_lmt_cpy(lmt_lst->lmt_dmn[idx],&lmt_wrp[1]);

      lmt_wrp[0].srt=srt;

      if(jdx == 1){
        lmt_wrp[0].end=srt;
        lmt_wrp[0].cnt=1L;
        lmt_wrp[0].srd=1L;
      }else{
        lmt_wrp[0].end=srt+srd*(jdx-1);
        lmt_wrp[0].cnt=jdx;
        lmt_wrp[0].srd=srd;
      } /* end else */

      lmt_wrp[1].srt=kdx;
      lmt_wrp[1].cnt=cnt-lmt_wrp[0].cnt;
      if(lmt_wrp[1].cnt == 1L){
        lmt_wrp[1].end=kdx;
        lmt_wrp[1].srd=1L;
      }else{
        lmt_wrp[1].end=kdx+(lmt_wrp[1].cnt-1)*srd;
        lmt_wrp[1].srd=srd;
      } /* end else */

      if(nco_dbg_lvl_get() == nco_dbg_old){
        (void)fprintf(stdout,"%s: INFO %s wrapped limits for <%s> found: ",nco_prg_nm_get(),fnc_nm,lmt_lst->dmn_nm);
        (void)fprintf(stdout,"current limits=%d:\n",lmt_lst->lmt_dmn_nbr);
      }

      /* "trv": Insert 2 non-wrapped limits */ 

      /* Current number of dimension limits for this table dimension  */
      int lmt_dmn_nbr=lmt_lst->lmt_dmn_nbr;

      /* Index of new limit  (insert at last index) */
      int lmt_new_idx=lmt_lst->lmt_crr;

      /* Make space for 1 more limit  */
      lmt_lst->lmt_dmn=(lmt_sct **)nco_realloc(lmt_lst->lmt_dmn,(lmt_dmn_nbr+1)*sizeof(lmt_sct *));

      /* Alloc the extra limit  */
      lmt_lst->lmt_dmn[lmt_new_idx]=(lmt_sct *)nco_malloc(sizeof(lmt_sct));

      /* Initialize the extra limit (insert at last index) */
      (void)nco_lmt_init(lmt_lst->lmt_dmn[lmt_new_idx]);

      /* Insert the limits to table (allocated; idx was already there; lmt_new_idx was alloced here)   */
      (void)nco_lmt_cpy(&lmt_wrp[0],lmt_lst->lmt_dmn[idx]);
      (void)nco_lmt_cpy(&lmt_wrp[1],lmt_lst->lmt_dmn[lmt_new_idx]);

      /* Update number of dimension limits for this table dimension  */
      lmt_lst->lmt_dmn_nbr++;

      /* Update current index of dimension limits for this table dimension  */
      lmt_lst->lmt_crr++;

      if(nco_dbg_lvl_get() == nco_dbg_old) (void)fprintf(stdout,"%s: INFO %s dimension <%s> new limits inserted (%li->%li) - (%li->%li):\n",nco_prg_nm_get(),fnc_nm,lmt_lst->dmn_nm,lmt_lst->lmt_dmn[idx]->srt,lmt_lst->lmt_dmn[idx]->end,lmt_lst->lmt_dmn[lmt_new_idx]->srt,lmt_lst->lmt_dmn[lmt_new_idx]->end);

    } /* endif srt > end */
  } /* end loop over size */

  /* Check if genuine wrapped co-ordinate */
  if(size == 1 && lmt_lst->lmt_dmn_nbr == 2) lmt_lst->WRP=True;

} /* End nco_msa_wrp_splt_trv() */

void
nco_cpy_var_val_mlt_lmt_trv         /* [fnc] Copy variable data from input to output file */
(const int in_id,                   /* I [id] Input group ID */
 const int out_id,                  /* I [id] Output group ID */
 FILE * const fp_bnr,               /* I [flg] Unformatted binary output file handle */
 const md5_sct * const md5,         /* I [flg] MD5 Configuration */
 const trv_sct * const var_trv)     /* I [sct] Object to write (variable) */
{
  /* Purpose: Copy variable data from input netCDF file to output netCDF file 
     Routine truncates dimensions in variable definition in output file according to user-specified limits
     Routine copies variable-by-variable, old-style, used only by ncks
     Routine does not handle strides (srd)

     "GTT" changes from the original nco_cpy_var_val_mlt_lmt():
     Object to write (variable) is passed as parameter */

  const char fnc_nm[]="nco_cpy_var_val_mlt_lmt_trv()"; /* [sng] Function name  */

  char var_nm[NC_MAX_NAME+1];      /* [sng] Variable name (local copy of object name) */ 

  int fl_fmt;                      /* [enm] Output file format */
  int nbr_dim;                     /* [nbr] Number of dimensions */
  int nbr_dmn_in;                  /* [nbr] Number of dimensions */
  int nbr_dmn_out;                 /* [nbr] Number of dimensions */
  int var_in_id;                   /* [nbr] Variable ID */
  int var_out_id;                  /* [nbr] Variable ID */

  long *dmn_map_cnt=NULL;          /* [nbr] Contiguous vector of lengths of hyperslab on disk */
  long *dmn_map_srt=NULL;          /* [nbr] Contiguous vector of indices to start of hyperslab on disk */

  nc_type var_typ_in;              /* [nbr] Variable type input */
  nc_type var_typ_out;             /* [nbr] Variable type output */

  var_sct var_in;                  /* [sct] Variable structure */
  var_sct var_out;                 /* [sct] Variable structure */
  var_sct *var_out_ptr;            /* [sct] Variable structure, used for nco_var_cnf_typ() */

  lmt_msa_sct **lmt_msa=NULL_CEWI; /* [sct] MSA Limits for only for variable dimensions  */          
  lmt_sct **lmt=NULL_CEWI;         /* [sct] Auxiliary Limit used in MSA */

  nco_bool flg_write=True;

  assert(nco_obj_typ_var == var_trv->nco_typ);

  /* Local copy of object name */
  strcpy(var_nm,var_trv->nm);                

  /* Get ID for requested variable from both files */
  (void)nco_inq_varid(in_id,var_nm,&var_in_id);
  (void)nco_inq_varid(out_id,var_nm,&var_out_id);

  /* Get type and number of dimensions for variable */
  (void)nco_inq_var(in_id,var_in_id,(char *)NULL,&var_typ_in,&nbr_dmn_in,(int *)NULL,(int *)NULL);
  (void)nco_inq_var(out_id,var_out_id,(char *)NULL,&var_typ_out,&nbr_dmn_out,(int *)NULL,(int *)NULL);
  if(nbr_dmn_out != nbr_dmn_in){
    (void)fprintf(stderr,"%s: ERROR attempt to write %d-dimensional input variable %s to %d-dimensional space in output file\nHINT: When using -A (append) option, all appended variables must be the same rank in the input file as in the output file. The ncwa operator is useful at ridding variables of extraneous (size = 1) dimensions. See how at http://nco.sf.net/nco.html#ncwa\nIf you wish to completely replace the existing output file definition and values of the variable %s by those in the input file, then first remove %s from the output file using, e.g., ncks -x -v %s. See more on subsetting at http://nco.sf.net/nco.html#sbs\n",nco_prg_nm_get(),nbr_dmn_in,var_nm,nbr_dmn_out,var_nm,var_nm,var_nm);
    nco_exit(EXIT_FAILURE);
  } /* endif */
  nbr_dim=nbr_dmn_out;

  /* Initialize variable structure with minimal info. for nco_msa_rcr_clc() and possibly nco_var_cnf_typ() */
  var_in.nm=var_nm;
  var_in.id=var_in_id;
  var_in.nc_id=in_id;
  var_in.type=var_typ_in;
  var_in.has_mss_val=False;

  /* Scalar */
  if(nbr_dim == 0){
    var_in.sz=1L;
    var_in.val.vp=nco_malloc(nco_typ_lng_udt(in_id,var_typ_in));
    (void)nco_get_var1(in_id,var_in_id,0L,var_in.val.vp,var_typ_in);
    var_out=var_in;
  } /* !Scalar */

  /* Array */
  if(nbr_dim > 0){
    /* Allocate local MSA */
    lmt_msa=(lmt_msa_sct **)nco_malloc(var_trv->nbr_dmn*sizeof(lmt_msa_sct *));
    lmt=(lmt_sct **)nco_malloc(var_trv->nbr_dmn*sizeof(lmt_sct *));
    
    /* Copy from table to local MSA */
    (void)nco_cpy_msa_lmt(var_trv,&lmt_msa);
    
    /* Dimension vectors */
    dmn_map_cnt=(long *)nco_malloc(nbr_dim*sizeof(long));
    dmn_map_srt=(long *)nco_malloc(nbr_dim*sizeof(long));
    
    for(int dmn_idx=0;dmn_idx<nbr_dim;dmn_idx++){
      /* Store in arrays */
      dmn_map_cnt[dmn_idx]=lmt_msa[dmn_idx]->dmn_cnt;
      dmn_map_srt[dmn_idx]=0L;
    } /* end loop over dmn_idx */

    /* Call super-dooper recursive routine (this sets var_in.sz) */
    var_in.val.vp=nco_msa_rcr_clc((int)0,nbr_dim,lmt,lmt_msa,&var_in);
    var_out=var_in;
  } /* !Array */

  /* Allow ncks to autoconvert netCDF4 atomic types to netCDF3- or CDF5-supported output type ... */
  if(nco_prg_id_get() == ncks){
    /* File format needed for decision tree and to enable netCDF4 features */
    (void)nco_inq_format(out_id,&fl_fmt);

    if(fl_fmt == NC_FORMAT_NETCDF4) var_typ_out=var_typ_in;
    else if(fl_fmt == NC_FORMAT_CLASSIC || fl_fmt == NC_FORMAT_64BIT_OFFSET || fl_fmt == NC_FORMAT_NETCDF4_CLASSIC) var_typ_out=nco_typ_nc4_nc3(var_typ_in);
    else if(fl_fmt == NC_FORMAT_64BIT_DATA) var_typ_out=nco_typ_nc4_nc5(var_typ_in);

    if(var_typ_out != var_typ_in){
      if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stdout,"%s: INFO Autoconverting variable %s from %s of type %s to %s-supported type %s\n",nco_prg_nm_get(),var_nm,(nbr_dim > 0) ? "array" : "scalar",nco_typ_sng(var_typ_in),nco_fmt_sng(fl_fmt),nco_typ_sng(var_typ_out));

      if(var_typ_in == NC_STRING && var_typ_out == NC_CHAR){
	/* Special case for string conversion:
	   20131227: Currently limited to translating string variables that contain only one string
	   Too many other limits on string translation to list them all :)
	   This only handles plain strings */
	if(var_out.sz > 1L){
	  (void)fprintf(stdout,"%s: ERROR Unable to autoconvert. %s reports string variable %s is an array of %li strings. Autoconversion of string variables is currently limited to scalar string variables (that contain a single string), and does not work on arrays of strings. Even single strings are currently translated incorrectly because each string is typically a distinct size, meaning a distinct phony dimension would need to be created for every single string and NCO is loathe to do that. Instead, NCO curretly translates single strings to a single character (instead of, say, creating a new string dimension of some arbitrary size). Complaints? Suggestions? Let us know.\n",nco_prg_nm_get(),fnc_nm,var_nm,var_out.sz);
	  nco_exit(EXIT_FAILURE);
	} /* endif err */

	var_out=var_in;
	var_out.sz=strlen(var_out.val.sngp[0]);
	nbr_dim++;
	if(nbr_dim == 1){
	  dmn_map_cnt=(long *)nco_malloc(nbr_dim*sizeof(long));
	  dmn_map_srt=(long *)nco_malloc(nbr_dim*sizeof(long));
	} /* nbr_dim != 1 */
	/* Following line would be correct were an adequately sized phony dimension declared and ready to accept this many characters */
	/* dmn_map_cnt[0]=var_out.sz; */
	dmn_map_cnt[0]=1L;
	dmn_map_srt[0]=0L;
	(void)nco_put_vara(out_id,var_out_id,dmn_map_srt,dmn_map_cnt,var_out.val.sngp[0],var_typ_out);
	(void)cast_nctype_void(var_typ_out,&var_out.val);
	flg_write=False;
      }else{ /* !NC_STRING */
	var_out_ptr=nco_var_cnf_typ(var_typ_out,&var_in);
	var_out=*var_out_ptr;
      } /* !NC_STRING */
      
    } /* !autoconvert */
  } /* !ncks */

  /* Write */
  if(flg_write){
    int ppc; /* [nbr] Precision-preserving compression, i.e., number of total or decimal significant digits */
    nco_bool flg_nsd; /* [flg] PPC algorithm is NSD */
    ppc=var_trv->ppc;
    flg_nsd=var_trv->flg_nsd;
    if(ppc != NC_MAX_INT){
      var_out.type=var_typ_out;
      var_out.id=var_out_id;
      nco_mss_val_get(out_id,&var_out);
      if(flg_nsd) (void)nco_ppc_bitmask(ppc,var_out.type,var_out.sz,var_out.has_mss_val,var_out.mss_val,var_out.val); else (void)nco_ppc_around(ppc,var_out.type,var_out.sz,var_out.has_mss_val,var_out.mss_val,var_out.val);
    } /* endif */
    if(nco_is_xcp(var_nm)) nco_xcp_prc(var_nm,var_typ_out,var_out.sz,(char *)var_out.val.vp);

    if(nbr_dim == 0) (void)nco_put_var1(out_id,var_out_id,0L,var_out.val.vp,var_typ_out); else (void)nco_put_vara(out_id,var_out_id,dmn_map_srt,dmn_map_cnt,var_out.val.vp,var_typ_out);
  } /* !flg_write */

  /* Perform MD5 digest of input and output data if requested */
  if(md5)
    if(md5->dgs)
      (void)nco_md5_chk(md5,var_nm,var_out.sz*nco_typ_lng_udt(out_id,var_typ_out),out_id,dmn_map_srt,dmn_map_cnt,var_out.val.vp);

  /* Write unformatted binary data */
  if(fp_bnr) nco_bnr_wrt(fp_bnr,var_nm,var_out.sz,var_typ_in,var_out.val.vp);

  /* Free */
  if(var_out.val.vp) var_out.val.vp=(void *)nco_free(var_out.val.vp);
  if(dmn_map_cnt) dmn_map_cnt=(long *)nco_free(dmn_map_cnt);
  if(dmn_map_srt) dmn_map_srt=(long *)nco_free(dmn_map_srt);

  if(lmt){
    (void)nco_lmt_msa_free(var_trv->nbr_dmn,lmt_msa);
    lmt=(lmt_sct **)nco_free(lmt);
  } /* endif */

  return;
} /* end nco_cpy_var_val_mlt_lmt_trv() */

void
nco_cpy_msa_lmt                     /* [fnc] Copy MSA struct from table to local function (print or write) */
(const trv_sct * const var_trv,     /* I [sct] Object to write (variable) */
 lmt_msa_sct ***lmt_msa)            /* O [sct] MSA array for dimensions */
{
  /* Purpose: Deep copy MSA structs from table to local array used in print or write */
  
  /* Loop dimensions for object (variable)  */
  for(int dmn_idx_var=0;dmn_idx_var<var_trv->nbr_dmn;dmn_idx_var++){
    
    (*lmt_msa)[dmn_idx_var]=(lmt_msa_sct *)nco_malloc(sizeof(lmt_msa_sct));
    
    if(var_trv->var_dmn[dmn_idx_var].is_crd_var == True){
      /* This dimension has a coordinate variable */
      
      /* Get number of limits */
      int lmt_dmn_nbr=var_trv->var_dmn[dmn_idx_var].crd->lmt_msa.lmt_dmn_nbr;
      
      /* Initialize to NULL the limit array */
      (*lmt_msa)[dmn_idx_var]->lmt_dmn=NULL;
      
      /* If limits, make space for them */
      if(lmt_dmn_nbr) (*lmt_msa)[dmn_idx_var]->lmt_dmn=(lmt_sct **)nco_malloc(lmt_dmn_nbr*sizeof(lmt_sct *));
      
      /* And copy the structure made while building limits  */
      (*lmt_msa)[dmn_idx_var]->dmn_nm=strdup(var_trv->var_dmn[dmn_idx_var].crd->nm);
      
      (*lmt_msa)[dmn_idx_var]->NON_HYP_DMN=var_trv->var_dmn[dmn_idx_var].crd->lmt_msa.NON_HYP_DMN;
      (*lmt_msa)[dmn_idx_var]->dmn_cnt=var_trv->var_dmn[dmn_idx_var].crd->lmt_msa.dmn_cnt;
      (*lmt_msa)[dmn_idx_var]->dmn_sz_org=var_trv->var_dmn[dmn_idx_var].crd->sz;
      (*lmt_msa)[dmn_idx_var]->lmt_dmn_nbr=var_trv->var_dmn[dmn_idx_var].crd->lmt_msa.lmt_dmn_nbr;
      (*lmt_msa)[dmn_idx_var]->MSA_USR_RDR=var_trv->var_dmn[dmn_idx_var].crd->lmt_msa.MSA_USR_RDR;
      (*lmt_msa)[dmn_idx_var]->WRP=var_trv->var_dmn[dmn_idx_var].crd->lmt_msa.WRP;
      
      /* Loop needed limits */
      for(int lmt_idx=0;lmt_idx<lmt_dmn_nbr;lmt_idx++){
	
        /* Alloc new limit */
        (*lmt_msa)[dmn_idx_var]->lmt_dmn[lmt_idx]=(lmt_sct *)nco_malloc(sizeof(lmt_sct));
	
        /* Initialize NULL/invalid */
        (void)nco_lmt_init((*lmt_msa)[dmn_idx_var]->lmt_dmn[lmt_idx]);
	
        /* Deep copy from table to local array */ 
        (void)nco_lmt_cpy(var_trv->var_dmn[dmn_idx_var].crd->lmt_msa.lmt_dmn[lmt_idx],(*lmt_msa)[dmn_idx_var]->lmt_dmn[lmt_idx]);
      } /* !lmt_idx */
      
      if((*lmt_msa)[dmn_idx_var]->lmt_dmn_nbr == 0){
	/* No limits so ake a limit to read all */
        if(nco_dbg_lvl_get() == nco_dbg_old) (void)fprintf(stdout,"Warning...no limit zone\n "); 
	
        /* Allocate one dummy limit */
        (*lmt_msa)[dmn_idx_var]->lmt_dmn_nbr=1;
        (*lmt_msa)[dmn_idx_var]->lmt_dmn=(lmt_sct **)nco_malloc(1*sizeof(lmt_sct *));
        (*lmt_msa)[dmn_idx_var]->lmt_dmn[0]=(lmt_sct *)nco_malloc(sizeof(lmt_sct));
	
        /* Initialize NULL/invalid */
        (void)nco_lmt_init((*lmt_msa)[dmn_idx_var]->lmt_dmn[0]);
	
        /* And set start,count,stride to read everything ...major success */
        (*lmt_msa)[dmn_idx_var]->lmt_dmn[0]->srt=0L;
        (*lmt_msa)[dmn_idx_var]->lmt_dmn[0]->cnt=(*lmt_msa)[dmn_idx_var]->dmn_sz_org;
        (*lmt_msa)[dmn_idx_var]->lmt_dmn[0]->srd=1L;
      } /* !no limites */
    }else if(var_trv->var_dmn[dmn_idx_var].is_crd_var == False){
      /* Dimension does not have a coordinate variable, it must have a unique dimension pointer */
      
      /* Get number of limits */
      int lmt_dmn_nbr=var_trv->var_dmn[dmn_idx_var].ncd->lmt_msa.lmt_dmn_nbr;
      
      /* If limits, make space for them */
      if(lmt_dmn_nbr) (*lmt_msa)[dmn_idx_var]->lmt_dmn=(lmt_sct **)nco_malloc(lmt_dmn_nbr*sizeof(lmt_sct *));
      
      /* And copy the structure made while building limits  */
      (*lmt_msa)[dmn_idx_var]->dmn_nm=strdup(var_trv->var_dmn[dmn_idx_var].ncd->nm);
      
      (*lmt_msa)[dmn_idx_var]->NON_HYP_DMN=var_trv->var_dmn[dmn_idx_var].ncd->lmt_msa.NON_HYP_DMN;
      (*lmt_msa)[dmn_idx_var]->dmn_cnt=var_trv->var_dmn[dmn_idx_var].ncd->lmt_msa.dmn_cnt;
      (*lmt_msa)[dmn_idx_var]->dmn_sz_org=var_trv->var_dmn[dmn_idx_var].ncd->sz;
      (*lmt_msa)[dmn_idx_var]->lmt_dmn_nbr=var_trv->var_dmn[dmn_idx_var].ncd->lmt_msa.lmt_dmn_nbr;
      (*lmt_msa)[dmn_idx_var]->MSA_USR_RDR=var_trv->var_dmn[dmn_idx_var].ncd->lmt_msa.MSA_USR_RDR;
      (*lmt_msa)[dmn_idx_var]->WRP=var_trv->var_dmn[dmn_idx_var].ncd->lmt_msa.WRP;
      
      /* Loop needed limits */
      for(int lmt_idx=0;lmt_idx<lmt_dmn_nbr;lmt_idx++){
	
        /* Alloc new limit */
        (*lmt_msa)[dmn_idx_var]->lmt_dmn[lmt_idx]=(lmt_sct *)nco_malloc(sizeof(lmt_sct));
	
        /* Initialize NULL/invalid */
        (void)nco_lmt_init((*lmt_msa)[dmn_idx_var]->lmt_dmn[lmt_idx]);
	
        /* Deep copy from table to local array */ 
        (void)nco_lmt_cpy(var_trv->var_dmn[dmn_idx_var].ncd->lmt_msa.lmt_dmn[lmt_idx],(*lmt_msa)[dmn_idx_var]->lmt_dmn[lmt_idx]);
      } /* !lmt_idx */
      
      if((*lmt_msa)[dmn_idx_var]->lmt_dmn_nbr == 0){
	/* No limits so ake a limit to read all */
        if(nco_dbg_lvl_get() == nco_dbg_old) (void)fprintf(stdout,"Warning...no limit zone\n "); 
        /* Alloc 1 dummy limit */
        (*lmt_msa)[dmn_idx_var]->lmt_dmn_nbr=1;
        (*lmt_msa)[dmn_idx_var]->lmt_dmn=(lmt_sct **)nco_malloc(1*sizeof(lmt_sct *));
        (*lmt_msa)[dmn_idx_var]->lmt_dmn[0]=(lmt_sct *)nco_malloc(sizeof(lmt_sct));
	
        /* Initialize NULL/invalid */
        (void)nco_lmt_init((*lmt_msa)[dmn_idx_var]->lmt_dmn[0]);
	
        /* And set start,count,stride to read everything ...major success */
        (*lmt_msa)[dmn_idx_var]->lmt_dmn[0]->srt=0L;
        (*lmt_msa)[dmn_idx_var]->lmt_dmn[0]->cnt=(*lmt_msa)[dmn_idx_var]->dmn_sz_org;
        (*lmt_msa)[dmn_idx_var]->lmt_dmn[0]->srd=1L;
      } /* !no limits */
    }else{
      /* This dimension must have either a coordinate or a dimension pointer */
      assert(False);
    } /* end else */
    
  } /* !dmn_idx_var */
       
} /* nco_cpy_msa_lmt() */

void
nco_msa_var_get_trv                 /* [fnc] Define a 'var_sct' hyperslab fields from a GTT variable */
(const int nc_id,                   /* I [ID] netCDF file ID */
 var_sct *var_in,                   /* I/O [sct] Variable */
 const trv_tbl_sct * const trv_tbl) /* I [sct] GTT (Group Traversal Table) */
{
  const char fnc_nm[]="nco_msa_var_get_trv()"; /* [sng] Function name  */
  
  int nbr_dim;
  int grp_id;
  
  lmt_msa_sct **lmt_msa;
  lmt_sct **lmt;
  
  nc_type mss_typ_tmp=NC_NAT; /* CEWI */
  
  trv_sct *var_trv;
  
  /* Obtain variable GTT object using full variable name */
  var_trv=trv_tbl_var_nm_fll(var_in->nm_fll,trv_tbl);
  assert(var_trv);
  
  /* Obtain group ID */
  (void)nco_inq_grp_full_ncid(nc_id,var_trv->grp_nm_fll,&grp_id);
  
  nbr_dim=var_in->nbr_dim;	
  var_in->nc_id=grp_id; 
  
  assert(nbr_dim == var_trv->nbr_dmn);
  assert(!strcmp(var_in->nm_fll,var_trv->nm_fll));
  
  /* Scalars */
  if(nbr_dim == 0){
    var_in->val.vp=nco_malloc(nco_typ_lng_udt(var_in->nc_id,var_in->typ_dsk));
    (void)nco_get_var1(var_in->nc_id,var_in->id,0L,var_in->val.vp,var_in->typ_dsk);
    goto do_upk;
  } /* end if scalar */
  
    /* Allocate local MSA */
  lmt_msa=(lmt_msa_sct **)nco_malloc(var_trv->nbr_dmn*sizeof(lmt_msa_sct *));
  lmt=(lmt_sct **)nco_malloc(var_trv->nbr_dmn*sizeof(lmt_sct *));
  
  /* Copy from table to local MSA */
  (void)nco_cpy_msa_lmt(var_trv,&lmt_msa);
  
  if(nco_dbg_lvl_get() == nco_dbg_old){
    (void)fprintf(stdout,"%s: DEBUG %s reports reading %s\n",nco_prg_nm_get(),fnc_nm,var_trv->nm_fll);
    for(int idx_dmn=0;idx_dmn<var_trv->nbr_dmn;idx_dmn++){
      (void)fprintf(stdout,"%s: DEBUG %s reports dimension %s has dmn_cnt = %ld",nco_prg_nm_get(),fnc_nm,lmt_msa[idx_dmn]->dmn_nm,lmt_msa[idx_dmn]->dmn_cnt);
  for(int idx_lmt=0;idx_lmt<lmt_msa[idx_dmn]->lmt_dmn_nbr;idx_lmt++) (void)fprintf(stdout," : %ld (%ld->%ld)",lmt_msa[idx_dmn]->lmt_dmn[idx_lmt]->cnt,lmt_msa[idx_dmn]->lmt_dmn[idx_lmt]->srt,lmt_msa[idx_dmn]->lmt_dmn[idx_lmt]->end);
      (void)fprintf(stdout,"\n");
    } /* end loop over dimensions */
  } /* endif dbg */

  /* Call super-dooper recursive routine
     nco_msa_rcr_clc requires that var_in->type be on-disk type
     Could replace var->type by var->typ_dsk in nco_msa_rcr_clc() but that seems inelegant 
     Instead, risk putting val and mss_val types briefly out-of-sync by pretending var->type is typ_dsk
     Save current type of missing value in RAM in temporary variable and conform new variable to that below
     20140930: This is (too?) confusing and hard-to-follow, a better solution is to add a field mss_val_typ 
     to var_sct and then separately and explicitly track types of both val and mss_val members. */
  mss_typ_tmp=var_in->type;
  var_in->type=var_in->typ_dsk;
  var_in->val.vp=nco_msa_rcr_clc((int)0,nbr_dim,lmt,lmt_msa,var_in);
  var_in->type=mss_typ_tmp;

  /* Free */
  (void)nco_lmt_msa_free(var_trv->nbr_dmn,lmt_msa);
  lmt=(lmt_sct **)nco_free(lmt);

do_upk:
  /* Missing value type synchronization:
     Avoid re-reading missing value every ncra record by converting input value to disk type
     var_in->type still reflects missing value type, not variable value type */
  if(var_in->pck_dsk && (mss_typ_tmp != var_in->typ_dsk)) var_in=nco_cnv_mss_val_typ(var_in,var_in->typ_dsk);
  var_in->type=var_in->typ_dsk;

  /* Type of variable and missing value in memory are now same as type on disk */

  /* Packing in RAM is now same as packing on disk pck_dbg 
     fxm: This nco_pck_dsk_inq() call is never necessary for non-packed variables */
  (void)nco_pck_dsk_inq(grp_id,var_in);

  /* Packing/Unpacking */
  if(nco_is_rth_opr(nco_prg_id_get())){
    /* Arithmetic operators must unpack variables before performing arithmetic
       Otherwise arithmetic will produce garbage results */
    /* 20050519: Not sure why I originally made nco_var_upk() call SMP-critical
       20050629: Making this region multi-threaded causes no problems */
    if(var_in->pck_dsk) var_in=nco_var_upk(var_in);
  } /* endif arithmetic operator */

  return;
} /* end nco_msa_var_get_trv() */

void
nco_msa_var_get_sct                 /* [fnc] Define a 'var_sct' hyperslab fields from a GTT variable ('trv_sct')*/
(const int nc_id,                   /* I [ID] netCDF file ID */
  var_sct *var_in,                  /* I/O [sct] Variable */
  const trv_sct * const var_trv)    /* I [sct] GTT variable */
{
  /* Same as nco_msa_var_get_trv() but with input 'var_trv' '
     TODO Deprecate nco_msa_var_get_trv() and use this function */

  const char fnc_nm[] = "nco_msa_var_get_sct()"; /* [sng] Function name  */

  int nbr_dim;
  int grp_id;

  lmt_msa_sct **lmt_msa;
  lmt_sct **lmt;

  nc_type mss_typ_tmp = NC_NAT; /* CEWI */

  /* Obtain group ID */
  (void)nco_inq_grp_full_ncid(nc_id, var_trv->grp_nm_fll, &grp_id);

  nbr_dim = var_in->nbr_dim;
  var_in->nc_id = grp_id;

  assert(nbr_dim == var_trv->nbr_dmn);
  assert(!strcmp(var_in->nm_fll, var_trv->nm_fll));

  /* Scalars */
  if (nbr_dim == 0) {
    var_in->val.vp = nco_malloc(nco_typ_lng_udt(var_in->nc_id,var_in->typ_dsk));
    (void)nco_get_var1(var_in->nc_id, var_in->id, 0L, var_in->val.vp, var_in->typ_dsk);
    goto do_upk;
  } /* end if scalar */

    /* Allocate local MSA */
  lmt_msa = (lmt_msa_sct **)nco_malloc(var_trv->nbr_dmn * sizeof(lmt_msa_sct *));
  lmt = (lmt_sct **)nco_malloc(var_trv->nbr_dmn * sizeof(lmt_sct *));

  /* Copy from table to local MSA */
  (void)nco_cpy_msa_lmt(var_trv, &lmt_msa);

  if (nco_dbg_lvl_get() == nco_dbg_old) {
    (void)fprintf(stdout, "%s: DEBUG %s reports reading %s\n", nco_prg_nm_get(), fnc_nm, var_trv->nm_fll);
    for (int idx_dmn = 0; idx_dmn < var_trv->nbr_dmn; idx_dmn++) {
      (void)fprintf(stdout, "%s: DEBUG %s reports dimension %s has dmn_cnt = %ld", nco_prg_nm_get(), fnc_nm, lmt_msa[idx_dmn]->dmn_nm, lmt_msa[idx_dmn]->dmn_cnt);
      for (int idx_lmt = 0; idx_lmt < lmt_msa[idx_dmn]->lmt_dmn_nbr; idx_lmt++) (void)fprintf(stdout, " : %ld (%ld->%ld)", lmt_msa[idx_dmn]->lmt_dmn[idx_lmt]->cnt, lmt_msa[idx_dmn]->lmt_dmn[idx_lmt]->srt, lmt_msa[idx_dmn]->lmt_dmn[idx_lmt]->end);
      (void)fprintf(stdout, "\n");
    } /* end loop over dimensions */
  } /* endif dbg */

    /* Call super-dooper recursive routine
    nco_msa_rcr_clc requires that var_in->type be on-disk type
    Could replace var->type by var->typ_dsk in nco_msa_rcr_clc() but that seems inelegant
    Instead, risk putting val and mss_val types briefly out-of-sync by pretending var->type is typ_dsk
    Save current type of missing value in RAM in temporary variable and conform new variable to that below
    20140930: This is (too?) confusing and hard-to-follow, a better solution is to add a field mss_val_typ
    to var_sct and then separately and explicitly track types of both val and mss_val members. */
  mss_typ_tmp = var_in->type;
  var_in->type = var_in->typ_dsk;
  var_in->val.vp = nco_msa_rcr_clc((int)0, nbr_dim, lmt, lmt_msa, var_in);
  var_in->type = mss_typ_tmp;

  /* Free */
  (void)nco_lmt_msa_free(var_trv->nbr_dmn, lmt_msa);
  lmt = (lmt_sct **)nco_free(lmt);

do_upk:
  /* Missing value type synchronization:
  Avoid re-reading missing value every ncra record by converting input value to disk type
  var_in->type still reflects missing value type, not variable value type */
  if (var_in->pck_dsk && (mss_typ_tmp != var_in->typ_dsk)) var_in = nco_cnv_mss_val_typ(var_in, var_in->typ_dsk);
  var_in->type = var_in->typ_dsk;

  /* Type of variable and missing value in memory are now same as type on disk */

  /* Packing in RAM is now same as packing on disk pck_dbg
  fxm: This nco_pck_dsk_inq() call is never necessary for non-packed variables */
  (void)nco_pck_dsk_inq(grp_id, var_in);

  /* Packing/Unpacking */
  if(nco_is_rth_opr(nco_prg_id_get())){
    /* Arithmetic operators must unpack variables before performing arithmetic
    Otherwise arithmetic will produce garbage results */
    /* 20050519: Not sure why I originally made nco_var_upk() call SMP-critical
    20050629: Making this region multi-threaded causes no problems */
    if(var_in->pck_dsk) var_in=nco_var_upk(var_in);
  } /* endif arithmetic operator */

  return;
} /* end nco_msa_var_get_sct() */

void
nco_lmt_msa_free                    /* [fnc] Free MSA */
(const int nbr_dmn,                 /* I [nbr] Number of dimensions */
 lmt_msa_sct **lmt_msa)             /* I [sct] MSA */
{
  /* Free  */
  for(int idx_dmn=0;idx_dmn<nbr_dmn;idx_dmn++){
    lmt_msa[idx_dmn]->dmn_nm=(char *)nco_free(lmt_msa[idx_dmn]->dmn_nm);
    for(int lmt_idx=0;lmt_idx<lmt_msa[idx_dmn]->lmt_dmn_nbr;lmt_idx++) lmt_msa[idx_dmn]->lmt_dmn[lmt_idx]=nco_lmt_free(lmt_msa[idx_dmn]->lmt_dmn[lmt_idx]);
    lmt_msa[idx_dmn]->lmt_dmn=(lmt_sct **)nco_free(lmt_msa[idx_dmn]->lmt_dmn);
    lmt_msa[idx_dmn]=(lmt_msa_sct *)nco_free(lmt_msa[idx_dmn]);
  }
  lmt_msa=(lmt_msa_sct **)nco_free(lmt_msa);
} /* end nco_lmt_msa_free() */

