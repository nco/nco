/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_msa.c,v 1.157 2013-02-10 05:47:29 pvicente Exp $ */

/* Purpose: Multi-slabbing algorithm */

/* Copyright (C) 1995--2013 Charlie Zender
   License: GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */

#include "nco_msa.h" /* Multi-slabbing algorithm */
#include "nco_var_utl.h" /* Variable utilities */
#include "nco_grp_utl.h" /* Group utilities */

/* fxm: strings statically allocated with NCR_MAX_LEN_FMT_SNG chars may be susceptible to buffer overflow attacks */
/* Length should be computed at run time but doing so would be painful */
#define NCO_MAX_LEN_FMT_SNG 100

void *
nco_msa_rcr_clc /* [fnc] Multi-slab algorithm (recursive routine, returns a single slab pointer */
(int dpt_crr, /* [nbr] Current depth, we start at 0 */
 int dpt_crr_max, /* [nbr] Maximium depth (i.e., number of dimensions in variable (does not change) */
 lmt_sct **lmt, /* [sct] Limits of current hyperslabs (these change as we recurse) */
 lmt_all_sct **lmt_lst, /* [sct] List of limits in each dimension (this remains STATIC as we recurse) */
 var_sct *vara) /* [sct] Information for routine to read variable information and pass information between calls */
{
  /* NB: nco_msa_rcr_clc() with same nc_id contains OpenMP critical region */
  /* Purpose: Multi-slab algorithm (recursive routine, returns a single slab pointer) */
  int idx;
  int nbr_slb;
  void *vp;

  if(dpt_crr == dpt_crr_max) goto read_lbl;

  nbr_slb=lmt_lst[dpt_crr]->lmt_dmn_nbr;

  if(nbr_slb == 1){
    lmt[dpt_crr]=lmt_lst[dpt_crr]->lmt_dmn[0];
    /* NB: nco_msa_rcr_clc() with same nc_id contains OpenMP critical region */
    /* ncks -O -g g4g1g1 -v time3 ~/nco/data/in_grp.nc dies in next statement ... */
    vp=nco_msa_rcr_clc(dpt_crr+1,dpt_crr_max,lmt,lmt_lst,vara);
    return vp;
  } /* end if */

  /* Multiple hyperslabs */
  if(nbr_slb > 1){
    int slb_idx;
    long var_sz=1L;
    long lcnt;
    long *cp_sz;
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

    cp_sz=(long *)nco_malloc(nbr_slb*sizeof(long));
    indices=(long *)nco_malloc(nbr_slb*sizeof(long));
    cp_wrp=(char **)nco_malloc(nbr_slb*sizeof(void *));

    for(idx=0;idx<nbr_slb;idx++){
      lmt[dpt_crr]=lmt_lst[dpt_crr]->lmt_dmn[idx];
      /* NB: nco_msa_rcr_clc() with same nc_id contains OpenMP critical region */
      cp_wrp[idx]=(char *)nco_msa_rcr_clc(dpt_crr+1,dpt_crr_max,lmt,lmt_lst,vara);
      cp_sz[idx]=vara->sz;
    } /* end loop over idx */

    for(idx=0;idx<dpt_crr_max;idx++) var_sz*=(idx<dpt_crr ? lmt[idx]->cnt : lmt_lst[idx]->dmn_cnt);

    vp=(void *)nco_calloc((size_t)var_sz,nco_typ_lng(vara->type));

    lcnt=nco_typ_lng(vara->type);
    for(idx=dpt_crr+1;idx<dpt_crr_max;idx++) lcnt*=lmt_lst[idx]->dmn_cnt;

    cp_inc=(ptrdiff_t)(lcnt*lmt_lst[dpt_crr]->dmn_cnt);
    cp_max=(ptrdiff_t)(var_sz*nco_typ_lng(vara->type));

    for(idx=0;idx<nbr_slb;idx++) indices[idx]=lmt_lst[dpt_crr]->lmt_dmn[idx]->srt;

    cp_fst=0L;

    /* Deal first with wrapped dimensions
    True if wrapped dims or slabs DO NOT overlap or user-specified order */
    if(lmt_lst[dpt_crr]->WRP || lmt_lst[dpt_crr]->MSA_USR_RDR){

      for(slb_idx=0;slb_idx<nbr_slb;slb_idx++){
        cp_stp=(char *)vp+cp_fst;
        slb=cp_wrp[slb_idx];
        slb_sz=(ptrdiff_t)(lcnt*(lmt_lst[dpt_crr]->lmt_dmn[slb_idx]->cnt));
        while(cp_stp-(char *)vp < cp_max){
          (void)memcpy(cp_stp,slb,(size_t)slb_sz);
          slb+=slb_sz;
          cp_stp+=cp_inc;
        } /* end while */
        cp_fst+=slb_sz;      
      } /* end loop over two slabs */
    } else { 
      /* Multiple hyper-slabs */
      while(nco_msa_clc_idx(True,lmt_lst[dpt_crr],&indices[0],&lmt_ret,&slb_idx)){
        cp_stp=(char *)vp+cp_fst;
        slb=cp_wrp[slb_idx]+(ptrdiff_t)(lmt_ret.srt*lcnt);
        slb_stp=(ptrdiff_t)(lcnt*(lmt_lst[dpt_crr]->lmt_dmn[slb_idx]->cnt));
        slb_sz=(ptrdiff_t)(lmt_ret.cnt*lcnt);

        while(cp_stp-(char *)vp < cp_max){
          (void)memcpy(cp_stp,slb,(size_t)slb_sz);
          slb+=slb_stp;
          cp_stp+=cp_inc;
        } /* end while */
        cp_fst+=slb_sz;
      } /* end while */
    } /* end else */  

    indices=(long *)nco_free(indices);
    cp_sz=(long *)nco_free(cp_sz);
    for(idx=0;idx<nbr_slb;idx++) cp_wrp[idx]=(char *)nco_free(cp_wrp[idx]);
    cp_wrp=(char **)nco_free(cp_wrp);

    vara->sz=var_sz;
    return vp;
  } /* endif multiple hyperslabs */

read_lbl:
  { 
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
    } /* end loop over idx */

    vp=(void *)nco_malloc(var_sz*nco_typ_lng(vara->type));

    /* Block is critical/thread-safe for identical/distinct in_id's */
    { /* begin potential OpenMP critical */
      /* Check for stride */
      if(var_sz > 0){ /* Allow for zero-size record variables TODO nco711 */
        if(srd_prd == 1L)
          (void)nco_get_vara(vara->nc_id,vara->id,dmn_srt,dmn_cnt,vp,vara->type);
        else
          (void)nco_get_varm(vara->nc_id,vara->id,dmn_srt,dmn_cnt,dmn_srd,(long *)NULL,vp,vara->type);
      } /* end if var_sz */
    } /* end potential OpenMP critical */

    dmn_srt=(long *)nco_free(dmn_srt);
    dmn_cnt=(long *)nco_free(dmn_cnt);
    dmn_srd=(long *)nco_free(dmn_srd);

    /* Put size into vara */
    vara->sz=var_sz;
    return vp;
  }/* end read_lbl */

} /* end nco_msa_rcr_clc() */

void 
nco_msa_prn_idx(lmt_all_sct *lmt_i)
{
  int slb_nbr;
  int idx;
  int size=lmt_i->lmt_dmn_nbr;
  long *indices;
  lmt_sct lmt;

  indices=(long *)nco_malloc(size *sizeof(long));

  printf("name=%s total size=%ld\n",lmt_i->dmn_nm,lmt_i->dmn_cnt);

  for(idx=0;idx<size;idx++) indices[idx]=lmt_i->lmt_dmn[idx]->srt;

  while(nco_msa_clc_idx(False,lmt_i,&indices[0],&lmt,&slb_nbr)) printf("slb_nbr=%d, srt=%ld, end=%ld, cnt=%ld, srd=%ld\n",slb_nbr,lmt.srt,lmt.end,lmt.cnt,lmt.srd);
} /* end nco_msa_prn_idx() */

nco_bool /* [flg] There are more limits to process in the slab */
nco_msa_clc_idx
(nco_bool NORMALIZE,
 lmt_all_sct *lmt_a, /* I list of lmts for each dimension  */
 long *indices, /* I/O so routine can keep track of where its at */
 lmt_sct *lmt, /* O Output hyperslab */
 int *slb) /* slab which above limit refers to */ 
{
  /* A messy, unclear, inefficient routine that needs re-writing
  if NORMALIZE then return the slab number and the hyperslab in lmt
  NB: This is the slab _WITHIN_ the slab so stride is ALWAYS 1 */
  int sz_idx;
  int size=lmt_a->lmt_dmn_nbr;
  nco_bool *mnm;
  nco_bool rcd;

  int prv_slb=0;
  int crr_slb=0;
  long crr_idx;
  long prv_idx=long_CEWI;
  long cnt=0L;

  mnm=(nco_bool *)nco_malloc(size*sizeof(nco_bool));

  lmt->srt=-1L;
  lmt->cnt=0L;
  lmt->srd=0L;

  while(++cnt){
    crr_idx=nco_msa_min_idx(indices,mnm,size);

    crr_slb=-1;
    for(sz_idx=0;sz_idx<size;sz_idx++)
      if(mnm[sz_idx]){crr_slb=sz_idx;break;}

      if(crr_slb == -1){
        if(lmt->srt == -1){
          rcd=False;
          goto cln_and_xit;
        }else break;
      } /* endif */

      if(mnm[prv_slb]) crr_slb=prv_slb;

      if(lmt->srt > -1 && crr_slb != prv_slb) break;

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
          if(indices[sz_idx] > lmt_a->lmt_dmn[sz_idx]->end) indices[sz_idx]=-1;
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
 lmt_all_sct **lmt_msa, 
 int nbr_dim,
 long *dmn_sbs_dsk,
 nco_bool flg_free)
{
  /*  It does not really convert RAM indices to disk indices, but given a set 
  of RAM indices, it finds the next set of disk incdices. 
  So it only works if the indices fed to it are continuous */
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
    initialize=1;
  }
  for(idx=0;idx <nbr_dim;idx ++){
    size=lmt_msa[idx]->lmt_dmn_nbr;
    if(dmn_sbs_ram[idx] == dmn_sbs_prv[idx]) continue;

    if(lmt_msa[idx]->BASIC_DMN){
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
        if(dmn_indices[idx][jdx] > lmt_msa[idx]->lmt_dmn[jdx]->end) dmn_indices[idx][jdx]=-1;
      }
    } /* end for  jdx */
  } /* end for idx */ 

  for(idx=0;idx<nbr_dim;idx++) dmn_sbs_prv[idx]=dmn_sbs_ram[idx];

  /* Free static space on last call */
  if(flg_free){
    (void)nco_free(dmn_sbs_prv);
    for(idx=0;idx <nbr_dim;idx++)
      (void)nco_free(dmn_indices[idx]);
    (void)nco_free(dmn_indices);
    initialize=0;
  }
} /* end nco_msa_ram_2_dsk() */

void 
nco_msa_clc_cnt
(lmt_all_sct *lmt_lst)
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
    for(idx=0;idx<size;idx++) cnt+=lmt_lst->lmt_dmn[idx]->cnt;
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
          if(indices[idx] > lmt_lst->lmt_dmn[idx]->end) indices[idx]=-1;
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
(lmt_all_sct *lmt_lst)
{
  /* Purpose: Return true if limits overlap
  NB: Assumes that limits have been sorted */

  long idx;
  long jdx;
  long sz=lmt_lst->lmt_dmn_nbr;

  lmt_sct **lmt;
  /* defererence */
  lmt=lmt_lst->lmt_dmn;

  for(idx=0; idx<sz; idx++)
    for(jdx=idx+1; jdx<sz ;jdx++)
      if( lmt[jdx]->srt <= lmt[idx]->end) return True;  

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
(lmt_all_sct *lmt_lst)
{
  lmt_sct **lmt;
  long sz;

  sz=lmt_lst->lmt_dmn_nbr;
  lmt=lmt_lst->lmt_dmn;

  if(sz <= 1) return;

  (void)qsort(lmt,(size_t)sz,sizeof(lmt_sct *),nco_cmp_lmt_srt);
} /* end nco_msa_qsort_srt() */

void
nco_msa_wrp_splt /* [fnc] Split wrapped dimensions */
(lmt_all_sct *lmt_lst)
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

long /* O [idx] Minimum value */
nco_msa_min_idx /* [fnc] Find minimum values in current and return minimum value */
(const long * const current, /* I [idx] Current indices */
 nco_bool * const mnm, /* O [flg] Minimum */
 const int size) /* I [nbr] Size of current and mnm */
{
  int sz_idx;
  long min_val=LONG_MAX;

  for(sz_idx=0;sz_idx<size;sz_idx++)
    if(current[sz_idx] != -1 && current[sz_idx] < min_val) min_val=current[sz_idx];

  for(sz_idx=0;sz_idx<size;sz_idx++)
    mnm[sz_idx]=((current[sz_idx] != -1 && current[sz_idx] == min_val) ? True : False);

  return min_val;
} /* end nco_msa_min_idx() */

void
nco_msa_var_get    /* [fnc] Get variable data from disk taking account of multihyperslabs */
(const int in_id,  /* I [id] netCDF input file ID */
 var_sct *var_in,
 lmt_all_sct * const * lmt_lst, /* I [sct] multi-hyperslab limits */
 int nbr_dmn_fl) /* I [nbr] Number of multi-hyperslab limits */
{
  int idx;
  int jdx;
  int nbr_dim;
  nc_type typ_tmp;
  void *void_ptr;
  lmt_all_sct **lmt_msa;
  lmt_sct **lmt;

  nbr_dim=var_in->nbr_dim;	

  /* Refresh nc_id with in_id, NB: makes OpenMP threading work
  Should have been included in release 3.9.5
  Fixes TODO nco956 */
  var_in->nc_id=in_id; 

  /* Scalars */
  if(nbr_dim == 0){
    var_in->val.vp=nco_malloc(nco_typ_lng(var_in->typ_dsk));
    (void)nco_get_var1(in_id,var_in->id,0L,var_in->val.vp,var_in->typ_dsk);
    goto do_upk;
  } /* end if scalar */

  lmt_msa=(lmt_all_sct **)nco_malloc(nbr_dim*sizeof(lmt_all_sct *));
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
  void_ptr=nco_msa_rcr_clc(0,nbr_dim,lmt,lmt_msa,var_in);

  var_in->type=typ_tmp;
  var_in->val.vp=void_ptr;

  (void)nco_free(lmt_msa);
  (void)nco_free(lmt);

do_upk:
  /* Following code copied from nco_var_get() */

  if(var_in->pck_dsk) var_in=nco_cnv_mss_val_typ(var_in,var_in->typ_dsk);
  /*    var=nco_cnv_mss_val_typ(var,var->typ_dsk);*/

  /* Type of variable and missing value in memory are now same as type on disk */
  var_in->type=var_in->typ_dsk; /* [enm] Type of variable in RAM */

  /* Packing in RAM is now same as packing on disk pck_dbg 
  fxm: This nco_pck_dsk_inq() call is never necessary for non-packed variables */
  (void)nco_pck_dsk_inq(in_id,var_in);

  /* Packing/Unpacking */
  if(nco_is_rth_opr(prg_get())){
    /* Arithmetic operators must unpack variables before performing arithmetic
    Otherwise arithmetic will produce garbage results */
    /* 20050519: Not sure why I originally made nco_var_upk() call SMP-critical
    20050629: Making this region multi-threaded causes no problems */
    if(var_in->pck_dsk) var_in=nco_var_upk(var_in);
  } /* endif arithmetic operator */

  return;
} /* end nco_msa_var_get() */

void
nco_cpy_var_val_mlt_lmt /* [fnc] Copy variable data from input to output file */
(const int in_id, /* I [id] netCDF input file ID */
 const int out_id, /* I [id] netCDF output file ID */
 FILE * const fp_bnr, /* I [fl] Unformatted binary output file handle */
 const nco_bool MD5_DIGEST, /* I [flg] Perform MD5 digests */
 char *var_nm, /* I [sng] Variable name */
 lmt_all_sct * const * lmt_lst, /* I multi-hyperslab limits */
 int nbr_dmn_fl) /* I [nbr] Number of multi-hyperslab limits */
{
  /* NB: nco_cpy_var_val_mlt_lmt() contains OpenMP critical region */
  /* Purpose: Copy variable data from input netCDF file to output netCDF file 
  Routine truncates dimensions in variable definition in output file according to user-specified limits.
  Routine copies variable-by-variable, old-style, used only by ncks */

  int *dmn_id;
  int idx;
  int jdx;
  int nbr_dim;
  int nbr_dmn_in;
  int nbr_dmn_out;
  int var_in_id;
  int var_out_id;

  /* For regular data */
  long *dmn_map_in;
  long *dmn_map_cnt;
  long *dmn_map_srt;
  long var_sz=1L;

  nc_type var_typ;

  var_sct vara;/* To hold basic data in_id, var_id, nctype for recusive routine */

  void *void_ptr;

  lmt_all_sct **lmt_msa;
  lmt_sct **lmt;

  /* Get var_id for requested variable from both files */
  nco_inq_varid(in_id,var_nm,&var_in_id);
  nco_inq_varid(out_id,var_nm,&var_out_id);

  /* Get type and number of dimensions for variable */
  (void)nco_inq_var(out_id,var_out_id,(char *)NULL,&var_typ,&nbr_dmn_out,(int *)NULL,(int *)NULL);
  (void)nco_inq_var(in_id,var_in_id,(char *)NULL,&var_typ,&nbr_dmn_in,(int *)NULL,(int *)NULL);
  if(nbr_dmn_out != nbr_dmn_in){
    (void)fprintf(stderr,"%s: ERROR attempt to write %d-dimensional input variable %s to %d-dimensional space in output file\n",prg_nm_get(),nbr_dmn_in,var_nm,nbr_dmn_out);
    nco_exit(EXIT_FAILURE);
  } /* endif */
  nbr_dim=nbr_dmn_out;

  /* Deal with scalar variables */
  if(nbr_dim == 0){
    var_sz=1L;
    void_ptr=nco_malloc(nco_typ_lng(var_typ));
    /* Block is critical/thread-safe for identical/distinct in_id's */
    { /* Begin potential OpenMP critical */
      (void)nco_get_var1(in_id,var_in_id,0L,void_ptr,var_typ);
    } /* end potential OpenMP critical */
    /* Block is always critical */
    { /* Begin OpenMP critical */
      (void)nco_put_var1(out_id,var_out_id,0L,void_ptr,var_typ);
    } /* end OpenMP critical */
    /* Perform MD5 digest of input and output data if requested */
    if(MD5_DIGEST) (void)nco_md5_chk(var_nm,var_sz*nco_typ_lng(var_typ),out_id,(long *)NULL,(long *)NULL,void_ptr);
    if(fp_bnr) nco_bnr_wrt(fp_bnr,var_nm,var_sz,var_typ,void_ptr);
    (void)nco_free(void_ptr);
    return;
  } /* end if */

  dmn_map_in=(long *)nco_malloc(nbr_dim*sizeof(long));
  dmn_map_cnt=(long *)nco_malloc(nbr_dim*sizeof(long));
  dmn_map_srt=(long *)nco_malloc(nbr_dim*sizeof(long));
  dmn_id=(int *)nco_malloc(nbr_dim*sizeof(int));

  lmt_msa=(lmt_all_sct **)nco_malloc(nbr_dim*sizeof(lmt_all_sct *));
  lmt=(lmt_sct **)nco_malloc(nbr_dim*sizeof(lmt_sct *));

  /* Initialize */
  for(idx=0;idx<nbr_dim;idx++) lmt_msa[idx]=NULL;

  /* Get dimension IDs from input file */
  (void)nco_inq_vardimid(in_id,var_in_id,dmn_id);

  /* Get netCDF file format */
  int fl_fmt;
  (void)nco_inq_format(in_id,&fl_fmt);

#ifdef ENABLE_NETCDF4
  if(fl_fmt == NC_FORMAT_NETCDF4){
    char grp_nm[NC_MAX_NAME];/* [sng] Relative group name */
    char *grp_nm_fll;        /* [sng] Fully qualified group name */
    size_t grp_nm_lng;

    /* netCDF API to the rescue; we only have a location ID and a var name as parameters, but we need the full path */
    /* Allocate space for and obtain full name of current group */
    (void)nco_inq_grpname(in_id,grp_nm);
    (void)nco_inq_grpname_len(in_id,&grp_nm_lng);
    grp_nm_fll=(char *)nco_malloc((grp_nm_lng+1L)*sizeof(char));
    (void)nco_inq_grpname_full(in_id,&grp_nm_lng,grp_nm_fll);

    for(idx=0;idx<nbr_dim;idx++){
      char dmn_nm[NC_MAX_NAME];
      long dmn_sz;
      (void)nco_inq_dim(in_id,dmn_id[idx],dmn_nm,&dmn_sz);

      for(jdx=0;jdx<nbr_dmn_fl;jdx++){
        if(!strcmp(dmn_nm,lmt_lst[jdx]->lmt_dmn[0]->nm) && nco_fnd_dmn(in_id,dmn_nm)){
          lmt_msa[idx]=lmt_lst[jdx];
          break;
        } /* end if */
      } /* end loop over jdx */
      /* Create maps now---they maybe useful later */ 
      (void)nco_inq_dimlen(in_id,dmn_id[idx],&dmn_map_in[idx]);
      dmn_map_cnt[idx]=lmt_msa[idx]->dmn_cnt;
      dmn_map_srt[idx]=0L;
    } /* end loop over idx */

    /* Free full group name */
    grp_nm_fll=(char *)nco_free(grp_nm_fll);

#ifdef NCO_SANITY_CHECK
    for(idx=0;idx<nbr_dim;idx++) assert(lmt_msa[idx]);
#endif
  }else{  /* !NC_FORMAT_NETCDF4 */

    /* Initialize lmt_msa with multi-limits from lmt_lst limits */
    /* Get dimension sizes from input file */
    for(idx=0;idx<nbr_dim;idx++){
      for(jdx=0;jdx<nbr_dmn_fl;jdx++){
        if(dmn_id[idx] == lmt_lst[jdx]->lmt_dmn[0]->id){
          lmt_msa[idx]=lmt_lst[jdx];
          break;
        } /* end if */
      } /* end loop over jdx */
      /* Create maps now---they maybe useful later */ 
      (void)nco_inq_dimlen(in_id,dmn_id[idx],&dmn_map_in[idx]);
      dmn_map_cnt[idx]=lmt_msa[idx]->dmn_cnt;
      dmn_map_srt[idx]=0L;
    } /* end for */

  } /* !NC_FORMAT_NETCDF4 */

#else /* !ENABLE_NETCDF4 */

  /* Initialize lmt_msa with multi-limits from lmt_lst limits */
  /* Get dimension sizes from input file */
  for(idx=0;idx<nbr_dim;idx++){
    for(jdx=0;jdx<nbr_dmn_fl;jdx++){
      if(dmn_id[idx] == lmt_lst[jdx]->lmt_dmn[0]->id){
        lmt_msa[idx]=lmt_lst[jdx];
        break;
      } /* end if */
    } /* end loop over jdx */
    /* Create maps now---they maybe useful later */ 
    (void)nco_inq_dimlen(in_id,dmn_id[idx],&dmn_map_in[idx]);
    dmn_map_cnt[idx]=lmt_msa[idx]->dmn_cnt;
    dmn_map_srt[idx]=0L;
  } /* end for */

#endif /* !ENABLE_NETCDF4 */

  /* Initalize vara with in_id, var_in_id, nctype, etc., so recursive routine can read data */
  vara.nm=var_nm;
  vara.id=var_in_id;
  vara.nc_id=in_id;
  vara.type=var_typ;

  /* Call super-dooper recursive routine */
  /* NB: nco_msa_rcr_clc() with same nc_id contains OpenMP critical region */
  void_ptr=nco_msa_rcr_clc(0,nbr_dim,lmt,lmt_msa,&vara);
  var_sz=vara.sz;

  /* Block is always critical */
  { /* begin OpenMP critical */
    (void)nco_put_vara(out_id,var_out_id,dmn_map_srt,dmn_map_cnt,void_ptr,var_typ);
  } /* end OpenMP critical */

  /* Perform MD5 digest of input and output data if requested */
  if(MD5_DIGEST) (void)nco_md5_chk(var_nm,var_sz*nco_typ_lng(var_typ),out_id,dmn_map_srt,dmn_map_cnt,void_ptr);
  if(fp_bnr) nco_bnr_wrt(fp_bnr,var_nm,var_sz,var_typ,void_ptr);

  (void)nco_free(void_ptr);
  (void)nco_free(dmn_map_in);
  (void)nco_free(dmn_map_cnt);
  (void)nco_free(dmn_map_srt);
  (void)nco_free(dmn_id);
  (void)nco_free(lmt_msa);
  (void)nco_free(lmt);

  return;
} /* end nco_cpy_var_val_mlt_lmt() */

void 
nco_msa_c_2_f /* [fnc] Replace brackets with parentheses in a string */
(char *sng) /* [sng] String to change from C to Fortran notation */
{
  /* Purpose: Replace brackets with parentheses in a string */
  while(*sng){
    if(*sng == '[') *sng='(';
    if(*sng == ']') *sng=')';
    sng++;
  } /* end while */
} /* end nco_msa_c_2_f() */

void
nco_msa_prn_var_val   /* [fnc] Print variable data */
(const int in_id, /* I [id] Location ID (netCDF input file ID or group ID) */
 const char * const var_nm, /* I [sng] Variable name */
 lmt_all_sct * const *lmt_lst, /* I [sct] Dimension limits */
 const int lmt_nbr, /* I [nbr] Number of dimensions with user-specified limits */
 char * const dlm_sng, /* I [sng] User-specified delimiter string, if any */
 const nco_bool FORTRAN_IDX_CNV, /* I [flg] Hyperslab indices obey Fortran convention */
 const nco_bool MD5_DIGEST, /* I [flg] Perform MD5 digests */
 const nco_bool PRN_DMN_UNITS, /* I [flg] Print units attribute, if any */
 const nco_bool PRN_DMN_IDX_CRD_VAL, /* I [flg] Print dimension/coordinate indices/values */
 const nco_bool PRN_DMN_VAR_NM, /* I [flg] Print dimension/variable names */
 const nco_bool PRN_MSS_VAL_BLANK) /* I [flg] Print missing values as blanks */
{
  /* NB: nco_msa_prn_var_val() with same nc_id contains OpenMP critical region */
  /* Purpose: Print variable with limits from input file */

  char *unit_sng;                            /* [sng] Units string */ 
  char var_sng[NCO_MAX_LEN_FMT_SNG];         /* [sng] Variable string */
  char mss_val_sng[NCO_MAX_LEN_FMT_SNG]="_"; /* [sng] Print this instead of numerical missing value */
  char nul_chr='\0';                         /* [sng] Character to end string */ 

  int val_sz_byt;                            /* [nbr] Type size */

  long lmn;                                  /* [nbr] Index to print variable data */
  long var_dsk;                              /* [nbr] Variable index relative to disk */

  var_sct var;                               /* [sct] Variable structure */
  var_sct var_crd;                           /* [sct] Variable structure for associated coordinate variable */

  nco_bool is_mss_val=False;                 /* [flg] Current value is missing value */
  nco_bool MALLOC_UNITS_SNG=False;           /* [flg] Allocated memory for units string */

  long mod_map_in[NC_MAX_DIMS];
  long mod_map_cnt[NC_MAX_DIMS];
  long dmn_sbs_ram[NC_MAX_DIMS];             /* [nbr] Indices in hyperslab */
  long dmn_sbs_dsk[NC_MAX_DIMS];             /* [nbr] Indices of hyperslab relative to original on disk */

  dmn_sct dim[NC_MAX_DIMS];                  /* [sct] Dimension structure (make life easier with static arrays) */
  
 
  int *dmn_id=NULL_CEWI;
  int rcd;
  lmt_all_sct **lmt_msa=NULL_CEWI;
  lmt_sct **lmt=NULL_CEWI;

  int nbr_dmn_grp;                  /* [nbr] Number of dimensions in group */
  int dmn_ids_grp[NC_MAX_VAR_DIMS]; /* [id]  Dimension IDs for group */ 

  char dmn_nm[NC_MAX_NAME+1];       /* [sng] Dimension name */
  char grp_nm[NC_MAX_NAME];         /* [sng] Relative group name */
  char *grp_nm_fll;                 /* [sng] Fully qualified group name */

  const int flg_prn=0;              /* [flg] All the dimensions in all parent groups will also be retrieved */        

  size_t grp_nm_lng;                /* [nbr] Lenght of group full name */

  int fl_fmt;                       /* [nbr] File format */

  const char sls_sng[]="/";         /* [sng] Slash string */

  /* Set defaults */
  var_dfl_set(&var); 

  /* Initialize units string, overwrite later if necessary */
  unit_sng=&nul_chr;

  /* Get var_id for requested variable */
  var.nm=(char *)strdup(var_nm);
  var.nc_id=in_id;
  nco_inq_varid(in_id,var_nm,&var.id);

  /* Get type and number of dimensions for variable */
  (void)nco_inq_var(in_id,var.id,(char *)NULL,&var.type,&var.nbr_dim,(int *)NULL,(int *)NULL);

  /* Scalars */
  if(var.nbr_dim == 0){
    var.sz=1L;
    var.val.vp=nco_malloc(nco_typ_lng(var.type));
    /* Block is critical/thread-safe for identical/distinct in_id's */
    { /* begin potential OpenMP critical */
      (void)nco_get_var1(in_id,var.id,0L,var.val.vp,var.type);
    } /* end potential OpenMP critical */
  } /* end if */

  dmn_id=(int *)nco_malloc(var.nbr_dim*sizeof(int));
  lmt_msa=(lmt_all_sct **)nco_malloc(var.nbr_dim*sizeof(lmt_all_sct *));
  lmt=(lmt_sct **)nco_malloc(var.nbr_dim*sizeof(lmt_sct *));

  /* Get dimension IDs for variable */
  (void)nco_inq_vardimid(in_id,var.id,dmn_id);

  /* Get dimension IDs for group */
  (void)nco_inq_dimids(in_id,&nbr_dmn_grp,dmn_ids_grp,flg_prn);

  /* Obtain netCDF file format */
  (void)nco_inq_format(in_id,&fl_fmt);

#ifdef ENABLE_NETCDF4

  if(fl_fmt == NC_FORMAT_NETCDF4){ 

    /* netCDF API to the rescue; we only have a location ID and a var name as parameters, but we need the full path */
    /* Allocate space for and obtain full name of current group */
    (void)nco_inq_grpname(in_id,grp_nm);
    (void)nco_inq_grpname_len(in_id,&grp_nm_lng);
    grp_nm_fll=(char *)nco_malloc((grp_nm_lng+1L)*sizeof(char));
    (void)nco_inq_grpname_full(in_id,&grp_nm_lng,grp_nm_fll);

    /* Initialize lmt_msa with multi-limits from lmt_lst limits */
    /* Get dimension sizes from input file */

    /* Goal here is to match "lmt_msa" with "lmt_lst";
    from the netCDF3 case, this is done with comparing IDs with:
    for(idx=0;idx<var.nbr_dim;idx++)
    for(jdx=0;jdx<lmt_nbr;jdx++){
    if(dmn_id[idx] == lmt_lst[jdx]->lmt_dmn[0]->id){
    lmt_msa[idx]=lmt_lst[jdx];
    break;
    } 
    } 
    1) Build a "dmn_was_found" bool array to false
    2) Search first in a full name match of "dmn_nm_fll" in "lmt_lst" 
    3) If one index of "dmn_was_found" was not matched try to go to ancestors by nco_fnd_dmn() */

    /* This vector must be filled at end  */
    for(int dmn_idx=0;dmn_idx<var.nbr_dim;dmn_idx++) lmt_msa[dmn_idx]=NULL;

    /* This vector must be True at end  */
    nco_bool dmn_was_found[NC_MAX_VAR_DIMS];
    for(int dmn_idx=0;dmn_idx<var.nbr_dim;dmn_idx++) dmn_was_found[dmn_idx]=False;

    for(int dmn_idx=0;dmn_idx<var.nbr_dim;dmn_idx++){
      long dmn_sz;  
      (void)nco_inq_dim(in_id,dmn_id[dmn_idx],dmn_nm,&dmn_sz);

      for(int lmt_all_idx=0;lmt_all_idx<lmt_nbr;lmt_all_idx++){
        char *dmn_nm_fll;

        /* Construct full dimension name */
        dmn_nm_fll=(char *)nco_malloc(strlen(grp_nm_fll)+strlen(dmn_nm)+2L);
        strcpy(dmn_nm_fll,grp_nm_fll);
        if(strcmp(grp_nm_fll,sls_sng)) strcat(dmn_nm_fll,sls_sng);
        strcat(dmn_nm_fll,dmn_nm);

        /* Match full name ? */
        if(!strcmp(dmn_nm_fll,lmt_lst[lmt_all_idx]->dmn_nm_fll)){
          lmt_msa[dmn_idx]=lmt_lst[lmt_all_idx];

          dmn_was_found[dmn_idx]=True;

          if(dbg_lvl_get() == nco_dbg_crr){
            (void)fprintf(stdout,"%s: INFO <%s> Found dmn[%d]:%s(%li)\n",prg_nm_get(),
              var.nm,dmn_idx,lmt_msa[dmn_idx]->dmn_nm_fll,lmt_msa[dmn_idx]->dmn_sz_org); 
          } /* dbg_lvl_get() */

          dmn_nm_fll=(char *)nco_free(dmn_nm_fll);
          break;
        } /* end if */
        /* Free constructed name */
        dmn_nm_fll=(char *)nco_free(dmn_nm_fll);
      } /* end loop over lmt_all_idx */
    } /* end loop over dmn_idx */

    /* In case one of "lmt_msa" was not matched go to netCDF API in nco_fnd_dmn(), ancestor groups */
    for(int dmn_idx=0;dmn_idx<var.nbr_dim;dmn_idx++){

      /* Not found; go to API in nco_fnd_dmn() */
      if (dmn_was_found[dmn_idx] == False ){
        long dmn_sz;  
        (void)nco_inq_dim(in_id,dmn_id[dmn_idx],dmn_nm,&dmn_sz);

        for(int lmt_all_idx=0;lmt_all_idx<lmt_nbr;lmt_all_idx++){
          if(!strcmp(dmn_nm,lmt_lst[lmt_all_idx]->lmt_dmn[0]->nm) && nco_fnd_dmn(in_id,dmn_nm)){
            lmt_msa[dmn_idx]=lmt_lst[lmt_all_idx];

            dmn_was_found[dmn_idx]=True;

            if(dbg_lvl_get() == nco_dbg_crr){
              (void)fprintf(stdout,"%s: INFO <%s> nco_fnd_dmn dmn[%d]:%s(%li)\n",prg_nm_get(),
                var.nm,dmn_idx,lmt_msa[dmn_idx]->dmn_nm_fll,lmt_msa[dmn_idx]->dmn_sz_org); 
            } /* dbg_lvl_get() */

            break;
          } /* end if */
        } /* end loop over jdx */
      } /* end dmn_was_found */
    } /* end loop over idx */

    /* Free full group name */
    grp_nm_fll=(char *)nco_free(grp_nm_fll);

  }else{ /* NC_FORMAT_CLASSIC */

    for(int idx=0;idx<var.nbr_dim;idx++)
      for(int jdx=0;jdx<lmt_nbr;jdx++){
        if(dmn_id[idx] == lmt_lst[jdx]->lmt_dmn[0]->id){
          lmt_msa[idx]=lmt_lst[jdx];
          break;
        } /* end if */
      } /* end loop over jdx */

  } /* NC_FORMAT_CLASSIC */

#else /* !ENABLE_NETCDF4 */
  /* Initialize lmt_msa with multi-limits from lmt_lst limits */
  /* Get dimension sizes from input file */
  for( int idx=0;idx<var.nbr_dim;idx++)
    for(int jdx=0;jdx<lmt_nbr;jdx++){
      if(dmn_id[idx] == lmt_lst[jdx]->lmt_dmn[0]->id){
        lmt_msa[idx]=lmt_lst[jdx];
        break;
      } /* end if */
    } /* end loop over jdx */
#endif /* !ENABLE_NETCDF4 */

#ifdef NCO_SANITY_CHECK
    if(dbg_lvl_get() == nco_dbg_crr){
      dmn_sct dim_dbg[NC_MAX_VAR_DIMS];
      if(var.nbr_dim) (void)fprintf(stdout,"%s: INFO printing coordinates for %s\n",prg_nm_get(),var.nm); 

      for(int dmn_idx=0;dmn_idx<var.nbr_dim;dmn_idx++){
        dim_dbg[dmn_idx].nm=lmt_msa[dmn_idx]->dmn_nm;

        (void)fprintf(stdout,"dimension[%d]:%s\n",dmn_idx,dim_dbg[dmn_idx].nm); 

      }/* end for */
    } /* dbg_lvl_get() */

    /* Did we find all dims ? */ 
    for(int dmn_idx=0;dmn_idx<var.nbr_dim;dmn_idx++) assert(lmt_msa[dmn_idx]);
#endif

    /* Call super-dooper recursive routine */
    /* NB: nco_msa_rcr_clc() with same nc_id contains OpenMP critical region */
    var.val.vp=nco_msa_rcr_clc(0,var.nbr_dim,lmt,lmt_msa,&var);
    /* Call also initializes var.sz with final size */
    if(MD5_DIGEST) (void)nco_md5_chk(var_nm,var.sz*nco_typ_lng(var.type),in_id,(long *)NULL,(long *)NULL,var.val.vp);

    /* Warn if variable is packed */
    if(nco_pck_dsk_inq(in_id,&var)) (void)fprintf(stderr,"%s: WARNING about to print packed contents of variable \"%s\". Consider unpacking variable first using ncpdq -U.\n",prg_nm_get(),var_nm);

    /* Refresh number of attributes and missing value attribute, if any */
    var.has_mss_val=nco_mss_val_get(var.nc_id,&var);
    if(var.has_mss_val) val_sz_byt=nco_typ_lng(var.type);

    /* User supplied dlm_sng, print variable (includes nbr_dmn == 0) */  
    if(dlm_sng){
      char *fmt_sng_mss_val=NULL;

      /* Print each element with user-supplied formatting code */
      /* Replace C language '\X' escape codes with ASCII bytes */
      (void)sng_ascii_trn(dlm_sng);

      /* Assume -s argument (dlm_sng) formats entire string
      Otherwise, one could assume that field will be printed with format nco_typ_fmt_sng(var.type),
      and that user is only allowed to affect text between fields. 
      This would be accomplished with:
      (void)sprintf(var_sng,"%s%s",nco_typ_fmt_sng(var.type),dlm_sng);*/

      /* Find replacement format string at most once, then re-use */
#ifdef NCO_HAVE_REGEX_FUNCTIONALITY
      /* Replace printf()-format statements with format for missing values */
      fmt_sng_mss_val=nco_fmt_sng_printf_subst(dlm_sng);
#endif /* !NCO_HAVE_REGEX_FUNCTIONALITY */

      for(lmn=0;lmn<var.sz;lmn++){

        /* memcmp() triggers pedantic warning unless pointer arithmetic is cast to type char * */
        if(PRN_MSS_VAL_BLANK) is_mss_val = var.has_mss_val ? !memcmp((char *)var.val.vp+lmn*val_sz_byt,var.mss_val.vp,(size_t)val_sz_byt) : False; 

        if(PRN_MSS_VAL_BLANK && is_mss_val){
          if(strcmp(dlm_sng,fmt_sng_mss_val)) (void)fprintf(stdout,fmt_sng_mss_val,mss_val_sng); else (void)fprintf(stdout,"%s, ",mss_val_sng);
        }else{ /* !is_mss_val */
          switch(var.type){
          case NC_FLOAT: (void)fprintf(stdout,dlm_sng,var.val.fp[lmn]); break;
          case NC_DOUBLE: (void)fprintf(stdout,dlm_sng,var.val.dp[lmn]); break;
          case NC_SHORT: (void)fprintf(stdout,dlm_sng,var.val.sp[lmn]); break;
          case NC_INT: (void)fprintf(stdout,dlm_sng,var.val.ip[lmn]); break;
          case NC_CHAR: (void)fprintf(stdout,dlm_sng,var.val.cp[lmn]); break;
          case NC_BYTE: (void)fprintf(stdout,dlm_sng,var.val.bp[lmn]); break;
          case NC_UBYTE: (void)fprintf(stdout,dlm_sng,var.val.ubp[lmn]); break;
          case NC_USHORT: (void)fprintf(stdout,dlm_sng,var.val.usp[lmn]); break;
          case NC_UINT: (void)fprintf(stdout,dlm_sng,var.val.uip[lmn]); break;
          case NC_INT64: (void)fprintf(stdout,dlm_sng,var.val.i64p[lmn]); break;
          case NC_UINT64: (void)fprintf(stdout,dlm_sng,var.val.ui64p[lmn]); break;
          case NC_STRING: (void)fprintf(stdout,dlm_sng,var.val.sngp[lmn]); break;
          default: nco_dfl_case_nc_type_err(); break;
          } /* end switch */
        } /* !is_mss_val */
      } /* end loop over element */
      (void)fprintf(stdout,"\n");

      if(fmt_sng_mss_val) fmt_sng_mss_val=(char *)nco_free(fmt_sng_mss_val);

    } /* end if dlm_sng */

    if(PRN_DMN_UNITS){
      const char units_nm[]="units"; /* [sng] Name of units attribute */
      int rcd_lcl; /* [rcd] Return code */
      int att_id; /* [id] Attribute ID */
      long att_sz;
      nc_type att_typ;

      /* Does variable have character attribute named units_nm? */
      rcd_lcl=nco_inq_attid_flg(in_id,var.id,units_nm,&att_id);
      if(rcd_lcl == NC_NOERR){
        (void)nco_inq_att(in_id,var.id,units_nm,&att_typ,&att_sz);
        if(att_typ == NC_CHAR){
          unit_sng=(char *)nco_malloc((att_sz+1)*nco_typ_lng(att_typ));
          (void)nco_get_att(in_id,var.id,units_nm,unit_sng,att_typ);
          unit_sng[(att_sz+1)*nco_typ_lng(att_typ)-1]='\0';
          MALLOC_UNITS_SNG=True; /* [flg] Allocated memory for units string */
        } /* end if */
      } /* end if */
    } /* end if PRN_DMN_UNITS */

    if(var.nbr_dim == 0 && dlm_sng == NULL){
      /* Variable is scalar, byte, or character */
      lmn=0L;
      if(PRN_MSS_VAL_BLANK) is_mss_val = var.has_mss_val ? !memcmp(var.val.vp,var.mss_val.vp,(size_t)val_sz_byt) : False; 
      if(PRN_DMN_VAR_NM) (void)sprintf(var_sng,"%%s = %s %%s\n",nco_typ_fmt_sng(var.type)); else (void)sprintf(var_sng,"%s\n",nco_typ_fmt_sng(var.type));
      if(PRN_MSS_VAL_BLANK && is_mss_val){
        if(PRN_DMN_VAR_NM) (void)fprintf(stdout,"%s = %s %s\n",var_nm,mss_val_sng,unit_sng); else (void)fprintf(stdout,"%s\n",mss_val_sng); 
      }else{ /* !is_mss_val */
        if(PRN_DMN_VAR_NM){
          switch(var.type){
          case NC_FLOAT: (void)fprintf(stdout,var_sng,var_nm,var.val.fp[lmn],unit_sng); break;
          case NC_DOUBLE: (void)fprintf(stdout,var_sng,var_nm,var.val.dp[lmn],unit_sng); break;
          case NC_SHORT: (void)fprintf(stdout,var_sng,var_nm,var.val.sp[lmn],unit_sng); break;
          case NC_INT: (void)fprintf(stdout,var_sng,var_nm,var.val.ip[lmn],unit_sng); break;
          case NC_CHAR:
            if(var.val.cp[lmn] != '\0'){
              (void)sprintf(var_sng,"%%s = '%s' %%s\n",nco_typ_fmt_sng(var.type));
              (void)fprintf(stdout,var_sng,var_nm,var.val.cp[lmn],unit_sng);
            }else{ /* Deal with NUL character here */
              (void)fprintf(stdout, "%s = \"\" %s\n",var_nm,unit_sng);
            } /* end if */
            break;
          case NC_BYTE: (void)fprintf(stdout,var_sng,var_nm,(unsigned char)var.val.bp[lmn],unit_sng); break;
          case NC_UBYTE: (void)fprintf(stdout,var_sng,var_nm,var.val.ubp[lmn],unit_sng); break;
          case NC_USHORT: (void)fprintf(stdout,var_sng,var_nm,var.val.usp[lmn],unit_sng); break;
          case NC_UINT: (void)fprintf(stdout,var_sng,var_nm,var.val.uip[lmn],unit_sng); break;
          case NC_INT64: (void)fprintf(stdout,var_sng,var_nm,var.val.i64p[lmn],unit_sng); break;
          case NC_UINT64: (void)fprintf(stdout,var_sng,var_nm,var.val.ui64p[lmn],unit_sng); break;
          case NC_STRING: (void)fprintf(stdout,var_sng,var_nm,var.val.sngp[lmn],unit_sng); break;
          default: nco_dfl_case_nc_type_err(); break;
          } /* end switch */
        }else{ /* !PRN_DMN_VAR_NM */
          switch(var.type){
          case NC_FLOAT: (void)fprintf(stdout,var_sng,var.val.fp[lmn]); break;
          case NC_DOUBLE: (void)fprintf(stdout,var_sng,var.val.dp[lmn]); break;
          case NC_SHORT: (void)fprintf(stdout,var_sng,var.val.sp[lmn]); break;
          case NC_INT: (void)fprintf(stdout,var_sng,var.val.ip[lmn]); break;
          case NC_CHAR:
            if(var.val.cp[lmn] != '\0'){
              (void)sprintf(var_sng,"'%s'\n",nco_typ_fmt_sng(var.type));
              (void)fprintf(stdout,var_sng,var.val.cp[lmn]);
            }else{ /* Deal with NUL character here */
              (void)fprintf(stdout, "\"\"\n");
            } /* end if */
            break;
          case NC_BYTE: (void)fprintf(stdout,var_sng,(unsigned char)var.val.bp[lmn]); break;
          case NC_UBYTE: (void)fprintf(stdout,var_sng,var.val.ubp[lmn]); break;
          case NC_USHORT: (void)fprintf(stdout,var_sng,var.val.usp[lmn]); break;
          case NC_UINT: (void)fprintf(stdout,var_sng,var.val.uip[lmn]); break;
          case NC_INT64: (void)fprintf(stdout,var_sng,var.val.i64p[lmn]); break;
          case NC_UINT64: (void)fprintf(stdout,var_sng,var.val.ui64p[lmn]); break;
          case NC_STRING: (void)fprintf(stdout,var_sng,var.val.sngp[lmn]); break;
          default: nco_dfl_case_nc_type_err(); break;
          } /* end switch */
        } /* !PRN_DMN_VAR_NM */
      } /* !is_mss_val */
    } /* end if variable is scalar, byte, or character */

    if(var.nbr_dim > 0 && dlm_sng == NULL){
      
      /* Create mod_map_in */
      for(int idx=0;idx<var.nbr_dim;idx++) mod_map_in[idx]=1L;
      for(int idx=0;idx<var.nbr_dim;idx++)
      {
        for(int jdx=idx+1;jdx<var.nbr_dim;jdx++)
        {
          mod_map_in[idx]*=lmt_msa[jdx]->dmn_sz_org;
        }
      }

      /* Create mod_map_cnt */
      for(int idx=0;idx<var.nbr_dim;idx++) mod_map_cnt[idx]=1L;
      for(int idx=0;idx<var.nbr_dim;idx++)
      {
        for(int jdx=idx;jdx<var.nbr_dim;jdx++)
        {
          mod_map_cnt[idx]*=lmt_msa[jdx]->dmn_cnt;
        }
      }

      /* Read coordinate dimensions if required */
      if(PRN_DMN_IDX_CRD_VAL){

        for(int idx=0;idx<var.nbr_dim;idx++){
          dim[idx].val.vp=NULL;
          dim[idx].nm=lmt_msa[idx]->dmn_nm;
          rcd=nco_inq_varid_flg(in_id,dim[idx].nm,&dim[idx].cid);
          /* If not a variable */
          if(rcd != NC_NOERR){
            dim[idx].is_crd_dmn=False;
            dim[idx].cid=-1;
            continue;
          } /* end if */

          dim[idx].is_crd_dmn=True;
          (void)nco_inq_vartype(in_id,dim[idx].cid,&dim[idx].type);
          var_crd.nc_id=in_id;
          var_crd.nm=dim[idx].nm;
          var_crd.type=dim[idx].type;
          var_crd.id=dim[idx].cid;
          /* Read coordinate variable with limits applied */
          /* NB: nco_msa_rcr_clc() with same nc_id contains OpenMP critical region */
          dim[idx].val.vp=nco_msa_rcr_clc(0,1,lmt,lmt_msa+idx,&var_crd);

          /* Typecast pointer before use */  
          (void)cast_void_nctype(dim[idx].type,&dim[idx].val);
        }/* end for */
      } /* end if */

      for(lmn=0;lmn<var.sz;lmn++){

        /* memcmp() triggers pedantic warning unless pointer arithmetic is cast to type char * */
        if(PRN_MSS_VAL_BLANK) is_mss_val = var.has_mss_val ? !memcmp((char *)var.val.vp+lmn*val_sz_byt,var.mss_val.vp,(size_t)val_sz_byt) : False; 

        /* Calculate RAM indices from current limit */
        for(int idx=0;idx <var.nbr_dim;idx++) 
        {
          dmn_sbs_ram[idx]=(lmn%mod_map_cnt[idx])/(idx == var.nbr_dim-1 ? 1L : mod_map_cnt[idx+1]);
        }

        /* Calculate disk indices from RAM indices */
        (void)nco_msa_ram_2_dsk(dmn_sbs_ram,lmt_msa,var.nbr_dim,dmn_sbs_dsk,(lmn==var.sz-1));

        /* Find variable index relative to disk */
        var_dsk=0;
        for(int idx=0;idx <var.nbr_dim;idx++)
        {
          var_dsk+=dmn_sbs_dsk[idx]*mod_map_in[idx];
        }

        /* Skip rest of loop unless element is first in string */
        if(var.type == NC_CHAR && dmn_sbs_ram[var.nbr_dim-1] > 0) goto lbl_chr_prn;

        /* Print dimensions with indices along with values if they are coordinate variables */
        if(PRN_DMN_IDX_CRD_VAL){
          int dmn_idx;
          long dmn_sbs_prn;
          long crd_idx_crr;
          char dmn_sng[NCO_MAX_LEN_FMT_SNG];

          /* Loop over dimensions whose coordinates are to be printed */
          for(int idx=0;idx<var.nbr_dim;idx++){

            /* Reverse dimension ordering for Fortran convention */
            if(FORTRAN_IDX_CNV) dmn_idx=var.nbr_dim-1-idx; else dmn_idx=idx;

            /* Format and print dimension part of output string for non-coordinate variables */
            if(dim[dmn_idx].cid == var.id) continue; /* If variable is a coordinate then skip printing until later */
            if(!dim[dmn_idx].is_crd_dmn){ /* If dimension is not a coordinate... */
              if(PRN_DMN_VAR_NM){
                if(FORTRAN_IDX_CNV) (void)fprintf(stdout,"%s(%ld) ",dim[dmn_idx].nm,dmn_sbs_dsk[dmn_idx]+1L); else (void)fprintf(stdout,"%s[%ld] ",dim[dmn_idx].nm,dmn_sbs_dsk[dmn_idx]);
              } /* !PRN_DMN_VAR_NM */
              continue;
            } /* end if */

            if(PRN_DMN_VAR_NM) (void)sprintf(dmn_sng,"%%s[%%ld]=%s ",nco_typ_fmt_sng(dim[dmn_idx].type)); else (void)sprintf(dmn_sng,"%s ",nco_typ_fmt_sng(dim[dmn_idx].type));
            dmn_sbs_prn=dmn_sbs_dsk[dmn_idx];

            if(FORTRAN_IDX_CNV){
              (void)nco_msa_c_2_f(dmn_sng);
              dmn_sbs_prn++;
            } /* end if */

            /* Account for hyperslab offset in coordinate values*/
            crd_idx_crr=dmn_sbs_ram[dmn_idx];
            if(PRN_DMN_VAR_NM){
              switch(dim[dmn_idx].type){
              case NC_FLOAT: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,dim[dmn_idx].val.fp[crd_idx_crr]); break;
              case NC_DOUBLE: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,dim[dmn_idx].val.dp[crd_idx_crr]); break;
              case NC_SHORT: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,dim[dmn_idx].val.sp[crd_idx_crr]); break;
              case NC_INT: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,dim[dmn_idx].val.ip[crd_idx_crr]); break;
              case NC_CHAR: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,dim[dmn_idx].val.cp[crd_idx_crr]); break;
              case NC_BYTE: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,(unsigned char)dim[dmn_idx].val.bp[crd_idx_crr]); break;
              case NC_UBYTE: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,dim[dmn_idx].val.ubp[crd_idx_crr]); break;
              case NC_USHORT: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,dim[dmn_idx].val.usp[crd_idx_crr]); break;
              case NC_UINT: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,dim[dmn_idx].val.uip[crd_idx_crr]); break;
              case NC_INT64: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,dim[dmn_idx].val.i64p[crd_idx_crr]); break;
              case NC_UINT64: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,dim[dmn_idx].val.ui64p[crd_idx_crr]); break;
              case NC_STRING: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,dim[dmn_idx].val.sngp[crd_idx_crr]); break;
              default: nco_dfl_case_nc_type_err(); break;
              } /* end switch */
            }else{ /* !PRN_DMN_VAR_NM */
              switch(dim[dmn_idx].type){
              case NC_FLOAT: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].val.fp[crd_idx_crr]); break;
              case NC_DOUBLE: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].val.dp[crd_idx_crr]); break;
              case NC_SHORT: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].val.sp[crd_idx_crr]); break;
              case NC_INT: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].val.ip[crd_idx_crr]); break;
              case NC_CHAR: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].val.cp[crd_idx_crr]); break;
              case NC_BYTE: (void)fprintf(stdout,dmn_sng,(unsigned char)dim[dmn_idx].val.bp[crd_idx_crr]); break;
              case NC_UBYTE: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].val.ubp[crd_idx_crr]); break;
              case NC_USHORT: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].val.usp[crd_idx_crr]); break;
              case NC_UINT: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].val.uip[crd_idx_crr]); break;
              case NC_INT64: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].val.i64p[crd_idx_crr]); break;
              case NC_UINT64: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].val.ui64p[crd_idx_crr]); break;
              case NC_STRING: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].val.sngp[crd_idx_crr]); break;
              default: nco_dfl_case_nc_type_err(); break;
              } /* end switch */
            } /* !PRN_DMN_VAR_NM */
          } /* end loop over dimensions */
        } /* end if PRN_DMN_IDX_CRD_VAL */

        /* Print all characters in last dimension each time penultimate dimension subscript changes to its start value */
lbl_chr_prn:

        if(var.type == NC_CHAR){
          static nco_bool NULL_IN_SLAB;
          static char *prn_sng;
          static int chr_cnt;
          static long dmn_sz;
          static long var_dsk_srt;
          static long var_dsk_end;

          /* At beginning of character array */
          if(dmn_sbs_ram[var.nbr_dim-1] == 0L) {
            dmn_sz=lmt_msa[var.nbr_dim-1]->dmn_cnt;
            prn_sng=(char *)nco_malloc((size_t)dmn_sz+1UL);
            var_dsk_srt=var_dsk;
            var_dsk_end=var_dsk;
            chr_cnt=0;
            NULL_IN_SLAB=False;
          } /* end if */

          /* In middle of array---save characters to prn_sng */
          prn_sng[chr_cnt++]=var.val.cp[lmn];
          if(var.val.cp[lmn] == '\0' && !NULL_IN_SLAB){
            var_dsk_end=var_dsk;
            NULL_IN_SLAB=True;
          } /* end if */

          /* At end of character array */
          if(dmn_sbs_ram[var.nbr_dim-1] == dmn_sz-1 ){
            if(NULL_IN_SLAB){
              (void)sprintf(var_sng,"%%s[%%ld--%%ld]=\"%%s\" %%s");
            }else{
              (void)sprintf(var_sng,"%%s[%%ld--%%ld]='%%s' %%s");
              prn_sng[chr_cnt]='\0';
              var_dsk_end=var_dsk;   
            } /* end if */
            if(FORTRAN_IDX_CNV){ 
              (void)nco_msa_c_2_f(var_sng);
              var_dsk_srt++; 
              var_dsk_end++; 
            } /* end if */
            (void)fprintf(stdout,var_sng,var_nm,var_dsk_srt,var_dsk_end,prn_sng,unit_sng);
            (void)fprintf(stdout,"\n");
            (void)fflush(stdout);
            (void)nco_free(prn_sng);
          } /* endif */
          continue;
        } /* end if NC_CHAR */

        /* Print variable name, index, and value */
        if(PRN_DMN_VAR_NM) (void)sprintf(var_sng,"%%s[%%ld]=%s %%s\n",nco_typ_fmt_sng(var.type)); else (void)sprintf(var_sng,"%s\n",nco_typ_fmt_sng(var.type));
        if(FORTRAN_IDX_CNV){
          (void)nco_msa_c_2_f(var_sng);
          var_dsk++;
        } /* end if FORTRAN_IDX_CNV */

        if(PRN_MSS_VAL_BLANK && is_mss_val){
          if(PRN_DMN_VAR_NM) (void)fprintf(stdout,"%s[%ld]=%s %s\n",var_nm,var_dsk,mss_val_sng,unit_sng); else (void)fprintf(stdout,"%s\n",mss_val_sng); 
        }else{ /* !is_mss_val */
          if(PRN_DMN_VAR_NM){
            switch(var.type){
            case NC_FLOAT: (void)fprintf(stdout,var_sng,var_nm,var_dsk,var.val.fp[lmn],unit_sng); break;
            case NC_DOUBLE: (void)fprintf(stdout,var_sng,var_nm,var_dsk,var.val.dp[lmn],unit_sng); break;
            case NC_SHORT: (void)fprintf(stdout,var_sng,var_nm,var_dsk,var.val.sp[lmn],unit_sng); break;
            case NC_INT: (void)fprintf(stdout,var_sng,var_nm,var_dsk,var.val.ip[lmn],unit_sng); break;
            case NC_CHAR: (void)fprintf(stdout,var_sng,var_nm,var_dsk,var.val.cp[lmn],unit_sng); break;
            case NC_BYTE: (void)fprintf(stdout,var_sng,var_nm,var_dsk,(unsigned char)var.val.bp[lmn],unit_sng); break;
            case NC_UBYTE: (void)fprintf(stdout,var_sng,var_nm,var_dsk,var.val.ubp[lmn],unit_sng); break;
            case NC_USHORT: (void)fprintf(stdout,var_sng,var_nm,var_dsk,var.val.usp[lmn],unit_sng); break;
            case NC_UINT: (void)fprintf(stdout,var_sng,var_nm,var_dsk,var.val.uip[lmn],unit_sng); break;
            case NC_INT64: (void)fprintf(stdout,var_sng,var_nm,var_dsk,var.val.i64p[lmn],unit_sng); break;
            case NC_UINT64: (void)fprintf(stdout,var_sng,var_nm,var_dsk,var.val.ui64p[lmn],unit_sng); break;
            case NC_STRING: (void)fprintf(stdout,var_sng,var_nm,var_dsk,var.val.sngp[lmn],unit_sng); break;
            default: nco_dfl_case_nc_type_err(); break;
            } /* end switch */
          }else{ /* !PRN_DMN_VAR_NM */
            switch(var.type){
            case NC_FLOAT: (void)fprintf(stdout,var_sng,var.val.fp[lmn],unit_sng); break;
            case NC_DOUBLE: (void)fprintf(stdout,var_sng,var.val.dp[lmn],unit_sng); break;
            case NC_SHORT: (void)fprintf(stdout,var_sng,var.val.sp[lmn],unit_sng); break;
            case NC_INT: (void)fprintf(stdout,var_sng,var.val.ip[lmn],unit_sng); break;
            case NC_CHAR: (void)fprintf(stdout,var_sng,var.val.cp[lmn],unit_sng); break;
            case NC_BYTE: (void)fprintf(stdout,var_sng,(unsigned char)var.val.bp[lmn],unit_sng); break;
            case NC_UBYTE: (void)fprintf(stdout,var_sng,var.val.ubp[lmn],unit_sng); break;
            case NC_USHORT: (void)fprintf(stdout,var_sng,var.val.usp[lmn],unit_sng); break;
            case NC_UINT: (void)fprintf(stdout,var_sng,var.val.uip[lmn],unit_sng); break;
            case NC_INT64: (void)fprintf(stdout,var_sng,var.val.i64p[lmn],unit_sng); break;
            case NC_UINT64: (void)fprintf(stdout,var_sng,var.val.ui64p[lmn],unit_sng); break;
            case NC_STRING: (void)fprintf(stdout,var_sng,var.val.sngp[lmn],unit_sng); break;
            default: nco_dfl_case_nc_type_err(); break;
            } /* end switch */
          } /* !PRN_DMN_VAR_NM */
        } /* !is_mss_val */
      } /* end loop over elements */


      /* Additional newline between consecutive variables or final variable and prompt */
      (void)fprintf(stdout,"\n");
      (void)fflush(stdout);
    } /* end if variable has more than one dimension */

    /* Free value buffer */
    var.val.vp=nco_free(var.val.vp);
    var.mss_val.vp=nco_free(var.mss_val.vp);
    var.nm=(char *)nco_free(var.nm);

    if(MALLOC_UNITS_SNG) unit_sng=(char *)nco_free(unit_sng);

    if(var.nbr_dim > 0){
      (void)nco_free(dmn_id);
      (void)nco_free(lmt_msa);
      (void)nco_free(lmt);
    } /* end if */

    if(PRN_DMN_IDX_CRD_VAL && dlm_sng==NULL){
      for(int idx=0;idx<var.nbr_dim;idx++) dim[idx].val.vp=nco_free(dim[idx].val.vp);
    } /* end if */

} /* end nco_msa_prn_var_val() */

void /* Initilaize lmt_all_sct's */ 
nco_msa_lmt_all_int
(int in_id,
 nco_bool MSA_USR_RDR,
 lmt_all_sct **lmt_all_lst,
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
  lmt_all_sct * lmt_all_crr;

  (void)nco_inq(in_id,(int*)NULL,(int*)NULL,(int *)NULL,&rec_dmn_id);

  for(idx=0;idx<nbr_dmn_fl;idx++){
    (void)nco_inq_dim(in_id,idx,dmn_nm,&dmn_sz);
    lmt_all_crr=lmt_all_lst[idx]=(lmt_all_sct *)nco_malloc(sizeof(lmt_all_sct));
    lmt_all_crr->lmt_dmn=(lmt_sct **)nco_malloc(sizeof(lmt_sct *));
    lmt_all_crr->dmn_nm=strdup(dmn_nm);
    lmt_all_crr->lmt_dmn_nbr=1;
    lmt_all_crr->dmn_sz_org=dmn_sz;
    lmt_all_crr->WRP=False;
    lmt_all_crr->BASIC_DMN=True;
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
    lmt_rgl->drn=1L;
    lmt_rgl->flg_mro=False;
    lmt_rgl->min_sng=NULL;
    lmt_rgl->max_sng=NULL;
    lmt_rgl->srd_sng=NULL;
    lmt_rgl->drn_sng=NULL;
    lmt_rgl->mro_sng=NULL;
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
        lmt_all_crr->BASIC_DMN=False;
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
    if(lmt_all_lst[idx]->lmt_dmn[0]->is_rec_dmn && (prg_get() == ncra || prg_get() == ncrcat)) continue;

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
    if(dbg_lvl_get() > 1){
      if(flg_ovl) (void)fprintf(stdout,"%s: dimension \"%s\" has overlapping hyperslabs\n",prg_nm_get(),lmt_all_lst[idx]->dmn_nm); else (void)fprintf(stdout,"%s: dimension \"%s\" has distinct hyperslabs\n",prg_nm_get(),lmt_all_lst[idx]->dmn_nm); 
    } /* endif */

  } /* end idx */    

} /* end nco_msa_lmt_all_int() */

void
nco_msa_var_val_cpy /* [fnc] Copy variables data from input to output file */
(const int in_id, /* I [enm] netCDF file ID */
 const int out_id, /* I [enm] netCDF output file ID */
 var_sct ** const var, /* I/O [sct] Variables to copy to output file */
 const int nbr_var,  /* I [nbr] Number of variables */
 lmt_all_sct * const * lmt_lst, /* I multi-hyperslab limits */
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
      var[idx]->val.vp=nco_malloc(nco_typ_lng(var[idx]->type));
      (void)nco_get_var1(in_id,var[idx]->id,0L,var[idx]->val.vp,var[idx]->type);
    }else{
      lmt_all_sct **lmt_msa;
      lmt_sct **lmt;

      lmt_msa=(lmt_all_sct **)nco_malloc(nbr_dim*sizeof(lmt_all_sct *));
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
      var[idx]->val.vp=nco_msa_rcr_clc(0,nbr_dim,lmt,lmt_msa,var[idx]);

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
          nco_put_varm(out_id,var[idx]->xrf->id,var[idx]->xrf->srt,var[idx]->xrf->cnt,var[idx]->xrf->srd,(long *)NULL,var[idx]->val.vp,var[idx]->type);

      } /* end if var_sz */
    } /* end if variable is an array */
    var[idx]->val.vp=var[idx]->xrf->val.vp=nco_free(var[idx]->val.vp);
  } /* end loop over idx */

} /* end nco_msa_var_val_cpy() */


void
nco_msa_wrp_splt_trv   /* [fnc] Split wrapped dimensions (traversal table version) */
(dmn_fll_sct *dmn_trv) /* [sct] Dimension structure from traversal table */
{
  /* Purpose: Same as nco_msa_wrp_splt() but applied to the Dimension structure from traversal table 
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
  int size=dmn_trv->lmt_dmn_nbr;  /* [nbr] Number of limit structures */
  long dmn_sz_org=dmn_trv->sz;    /* [nbr] Size of dimension */
  long srt;
  long cnt;
  long srd;
  long kdx=0; 
  lmt_sct *lmt_wrp;

  for(idx=0;idx<size;idx++){

    if(dmn_trv->lmt_dmn[idx]->srt > dmn_trv->lmt_dmn[idx]->end){

      if(dbg_lvl_get() >= nco_dbg_dev){
        (void)fprintf(stdout,"%s: INFO %s dimension <%s> has wrapped limits (%li->%li):\n",
          prg_nm_get(),fnc_nm,dmn_trv->nm_fll,dmn_trv->lmt_dmn[idx]->srt,dmn_trv->lmt_dmn[idx]->end);
      }

      lmt_wrp=(lmt_sct *)nco_malloc(2*sizeof(lmt_sct));

      /* "trv": Initialize  */
      (void)nco_lmt_init(&lmt_wrp[0]);
      (void)nco_lmt_init(&lmt_wrp[1]);

      srt=dmn_trv->lmt_dmn[idx]->srt;
      cnt=dmn_trv->lmt_dmn[idx]->cnt;
      srd=dmn_trv->lmt_dmn[idx]->srd;

      for(jdx=0;jdx<cnt;jdx++){
        kdx=(srt+srd*jdx)%dmn_sz_org;
        if(kdx<srt) break;
      } /* end loop over jdx */

      /* "trv": Instead of shallow copy in nco_msa_wrp_splt(), make a deep copy to the 2 new limits lmt_wrp */ 
      (void)nco_lmt_cpy(dmn_trv->lmt_dmn[idx],&lmt_wrp[0]);
      (void)nco_lmt_cpy(dmn_trv->lmt_dmn[idx],&lmt_wrp[1]);

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

      if(dbg_lvl_get() >= nco_dbg_dev){
        (void)fprintf(stdout,"%s: INFO %s wrapped limits for <%s> found: ",prg_nm_get(),fnc_nm,dmn_trv->nm_fll);
        (void)fprintf(stdout,"%d:\n",dmn_trv->lmt_dmn_nbr);
      }


      /* "trv": Insert 2 non-wrapped limits */ 

      /* Current number of dimension limits for this table dimension  */
      int lmt_dmn_nbr=dmn_trv->lmt_dmn_nbr;

      /* Index of new limit  */
      int lmt_new_idx=idx+1;

      /* Make space for 1 more limit  */
      dmn_trv->lmt_dmn=(lmt_sct **)nco_realloc(dmn_trv->lmt_dmn,(lmt_dmn_nbr+1)*sizeof(lmt_sct *));

      /* Alloc the extra limit  */
      dmn_trv->lmt_dmn[lmt_new_idx]=(lmt_sct *)nco_malloc(sizeof(lmt_sct));

      /* Initialize the extra limit */
      (void)nco_lmt_init(dmn_trv->lmt_dmn[lmt_new_idx]);

      /* Insert the limits to table (allocated; idx was already there; lmt_new_idx was alloced here)   */
      (void)nco_lmt_cpy(&lmt_wrp[0],dmn_trv->lmt_dmn[idx]);
      (void)nco_lmt_cpy(&lmt_wrp[1],dmn_trv->lmt_dmn[lmt_new_idx]);

      /* Update number of dimension limits for this table dimension  */
      dmn_trv->lmt_dmn_nbr++;

      /* Update current index of dimension limits for this table dimension  */
      dmn_trv->lmt_crr++;

      if(dbg_lvl_get() >= nco_dbg_dev){
        (void)fprintf(stdout,"%s: INFO %s dimension <%s> new limits inserted (%li->%li) - (%li->%li):\n",
          prg_nm_get(),fnc_nm,dmn_trv->nm_fll,dmn_trv->lmt_dmn[idx]->srt,dmn_trv->lmt_dmn[idx]->end,
          dmn_trv->lmt_dmn[lmt_new_idx]->srt,dmn_trv->lmt_dmn[lmt_new_idx]->end);
      }

    } /* endif srt > end */
  } /* end loop over size */

  /* Check if genuine wrapped co-ordinate */
  if(size==1 && dmn_trv->lmt_dmn_nbr==2){
    dmn_trv->WRP=True;
  }

} /* End nco_msa_wrp_splt_trv() */


void 
nco_msa_clc_cnt_trv     /* [fnc] Calculate size of  multiple hyperslab (traversal table version) */ 
(dmn_fll_sct *dmn_trv)  /* [sct] Dimension structure from traversal table */
{
  /* Purpose: Same as nco_msa_clc_cnt() but applied to the Dimension structure from traversal table */

  int idx;
  long cnt=0;
  int size=dmn_trv->lmt_dmn_nbr;   /* [nbr] Number of limit structures */
  long *indices;
  nco_bool *mnm;

  /* Degenerate case */
  if(size == 1){
    dmn_trv->dmn_cnt=dmn_trv->lmt_dmn[0]->cnt;
    return;
  } /* end if */

  /* If slabs remain in user-specified order */
  if(dmn_trv->MSA_USR_RDR){
    for(idx=0;idx<size;idx++) cnt+=dmn_trv->lmt_dmn[idx]->cnt;
    dmn_trv->dmn_cnt=cnt;
  }else{
    indices=(long *)nco_malloc(size*sizeof(long));
    mnm=(nco_bool *)nco_malloc(size*sizeof(nco_bool));

    /* Initialize indices with srt */
    for(idx=0;idx<size;idx++) indices[idx]=dmn_trv->lmt_dmn[idx]->srt;

    while(nco_msa_min_idx(indices,mnm,size) != LONG_MAX){
      for(idx=0;idx<size;idx++){
        if(mnm[idx]){
          indices[idx]+=dmn_trv->lmt_dmn[idx]->srd;
          if(indices[idx] > dmn_trv->lmt_dmn[idx]->end) indices[idx]=-1;
        } /* end if */
      } /* end loop over idx */
      cnt++;
    } /* end while */
    dmn_trv->dmn_cnt=cnt;

    indices=(long *)nco_free(indices);
    mnm=(nco_bool  *)nco_free(mnm);
  } /* end else */

  return; 

} /* End nco_msa_clc_cnt_trv() */

void             
nco_msa_qsort_srt_trv  /* [fnc] Sort limits by srt values (traversal table version) */
(dmn_fll_sct *dmn_trv) /* [sct] Dimension structure from traversal table */
{
  /* Purpose: Same as nco_msa_qsort_srt() but applied to the Dimension structure from traversal table */

  lmt_sct **lmt;
  long sz;           /* [nbr] Number of limit structures */

  sz=dmn_trv->lmt_dmn_nbr;
  lmt=dmn_trv->lmt_dmn;

  if(sz <= 1) return;

  (void)qsort(lmt,(size_t)sz,sizeof(lmt_sct *),nco_cmp_lmt_srt);

} /* End nco_msa_qsort_srt_trv() */



nco_bool                /* O [flg] Return true if limits overlap (traversal table version) */
nco_msa_ovl_trv         /* [fnc] See if limits overlap */ 
(dmn_fll_sct *dmn_trv)  /* [sct] Dimension structure from traversal table */
{
  /* Purpose: Same as nco_msa_ovl() but applied to the Dimension structure from traversal table 
  Return true if limits overlap NB: Assumes that limits have been sorted */

  long idx;
  long jdx;
  long sz=dmn_trv->lmt_dmn_nbr; /* [nbr] Number of limit structures */

  lmt_sct **lmt;
  /* defererence */
  lmt=dmn_trv->lmt_dmn;

  for(idx=0; idx<sz; idx++)
    for(jdx=idx+1; jdx<sz ;jdx++)
      if( lmt[jdx]->srt <= lmt[idx]->end) return True;  

  return False;

} /* End nco_msa_ovl_trv() */



void
nco_msa_prn_var_val_trv             /* [fnc] Print variable data (traversal table version) */
(const int in_id,                   /* I [id] Group ID */
 char * const dlm_sng,              /* I [sng] User-specified delimiter string, if any */
 const nco_bool FORTRAN_IDX_CNV,    /* I [flg] Hyperslab indices obey Fortran convention */
 const nco_bool MD5_DIGEST,         /* I [flg] Perform MD5 digests */
 const nco_bool PRN_DMN_UNITS,      /* I [flg] Print units attribute, if any */
 const nco_bool PRN_DMN_IDX_CRD_VAL,/* I [flg] Print dimension/coordinate indices/values */
 const nco_bool PRN_DMN_VAR_NM,     /* I [flg] Print dimension/variable names */
 const nco_bool PRN_MSS_VAL_BLANK,  /* I [flg] Print missing values as blanks */
 const trv_sct * const var_trv,     /* I [sct] Object to print (variable) */
 const trv_tbl_sct * const trv_tbl) /* I [sct] Traversal table */
{
  /* Purpose:
  Get variable with limits from input file
  User supplied dlm_sng, print var (includes nbr_dim == 0)
  Get dimensional units
  if nbr_dim ==0 and dlm_sng==NULL  print variable
  if PRN.. = False print var taking account of FORTRAN (need var indices)
  if PRN_DMN_IDX_CRD_VAL then read in co-ord dims
  if PRN.. = True print var taking account of FORTRAN (Use dims to calculate var indices 

  Similar to nco_msa_prn_var_val() but uses limit information contained in traversal table 
  Differences are marked "trv"
  1) It is not needed to retrieve dimension IDs for variable, these were used in nco_msa_prn_var_val()
  to match limits; Group Traversal Table (GTT)should be "ID free".
  2) GTT contains 2 separate lists: it is needed to traverse both and match by absolute name
  3) MSA: Modulo arrays: Changing the subscript of the first (least rapidly varying) dimension by one moves very quickly through 
  address space. Changing the subscript of the last (most rapidly varying) dimension by one moves exactly one location 
  (e.g., 8 bytes for a double) in address space. Each dimension has its own "stride" or length of RAM space between
  consecutive entries. mod_map_in and mod_map_cnt keep track of these distances. They are mappings between index-based 
  access and RAM-based access. The location of an N-dimensional array element in RAM is the sum of the products of 
  each index (dimensional subscript) times the stride (mod_map) of the corresponding dimension.

  Tests:
  ncks -D 11 -d lat,1,1,1  -v area -H ~/nco/data/in_grp.nc # area(lat)
  ncks -D 11 -v unique -H ~/nco/data/in_grp.nc # scalar
  ncks -D 11 -C -d time,1,2,1 -v two_dmn_rec_var -H ../../data/in_grp.nc # two_dmn_rec_var(time,lev);
  ncks -D 11 -C -d time,1,2,1 -d lev,1,1,1 -v two_dmn_rec_var -H ../../data/in_grp.nc # two_dmn_rec_var(time,lev);

  */
  const char fnc_nm[]="nco_msa_prn_var_val_trv()"; /* [sng] Function name  */

  char *unit_sng;                            /* [sng] Units string */ 
  char var_sng[NCO_MAX_LEN_FMT_SNG];         /* [sng] Variable string */
  char mss_val_sng[NCO_MAX_LEN_FMT_SNG]="_"; /* [sng] Print this instead of numerical missing value */
  char nul_chr='\0';                         /* [sng] Character to end string */ 
  char var_nm[NC_MAX_NAME+1];                /* [sng] Variable name (used for validation only) */ 

  int val_sz_byt;                            /* [nbr] Type size */

  long lmn;                                  /* [nbr] Index to print variable data */
  long var_dsk;                              /* [nbr] Variable index relative to disk */

  var_sct var;                               /* [sct] Variable structure */
  var_sct var_crd;                           /* [sct] Variable structure for associated coordinate variable */

  nco_bool is_mss_val=False;                 /* [flg] Current value is missing value */
  nco_bool MALLOC_UNITS_SNG=False;           /* [flg] Allocated memory for units string */

  long mod_map_in[NC_MAX_DIMS];              /* [nbr] MSA modulo array */
  long mod_map_cnt[NC_MAX_DIMS];             /* [nbr] MSA modulo array */
  long dmn_sbs_ram[NC_MAX_DIMS];             /* [nbr] Indices in hyperslab */
  long dmn_sbs_dsk[NC_MAX_DIMS];             /* [nbr] Indices of hyperslab relative to original on disk */

  dmn_sct dim[NC_MAX_DIMS];                  /* [sct] Dimension structure (make life easier with static arrays) */

  lmt_all_sct **lmt_msa=NULL_CEWI;           /* [sct] MSA Limits for only for variable dimensions  */          
  lmt_sct **lmt=NULL_CEWI;                   /* [sct] Auxiliary Limit used in MSA */

  int rcd;                                   /* [nbr] Return value */

  /* Make a conversion from GTT limits to MSA used local lmt_all_sct limits...MSA uses lmt_all_sct(variable dimensions) 
  Goal here is to distribute limits stored in unique dimensions to variable dimensions;
  A special index match counter must be used, not a regular 2 loop array of variable dimensions and group dimensions...
  Or do double 2 loop sequences to find what we need first...
  */ 

  /* Allocate; we don't know how many limits needed at this point, if any */
  lmt_msa=(lmt_all_sct **)nco_malloc(var_trv->nbr_dmn*sizeof(lmt_all_sct *));
  lmt=(lmt_sct **)nco_malloc(var_trv->nbr_dmn*sizeof(lmt_sct *));

  /* Special index match counter for MSA limits */
  int lmt_msa_idx=0;

  /* Loop dimensions for object (variable)  */
  for(int dmn_idx_var=0;dmn_idx_var<var_trv->nbr_dmn;dmn_idx_var++) {

    if(dbg_lvl_get() >= nco_dbg_dev){
      (void)fprintf(stdout,"%s: INFO %s <%s>:[%d]:%s: ",prg_nm_get(),fnc_nm,
        var_trv->nm_fll,dmn_idx_var,var_trv->var_dmn_fll.dmn_nm_fll[dmn_idx_var]);
    }

    /* Loop unique dimensions list (these contain limits) */
    for(unsigned dmn_idx=0;dmn_idx<trv_tbl->nbr_dmn;dmn_idx++){
      dmn_fll_sct dmn_trv=trv_tbl->lst_dmn[dmn_idx]; 

      /* Match full dimension name; found the dimension and possible limits in it */ 
      if(strcmp(var_trv->var_dmn_fll.dmn_nm_fll[dmn_idx_var],dmn_trv.nm_fll) == 0){

        if(dbg_lvl_get() >= nco_dbg_dev){
          (void)fprintf(stdout," %d limits: ",dmn_trv.lmt_dmn_nbr);
        }

        lmt_msa[lmt_msa_idx]=(lmt_all_sct *)nco_malloc(sizeof(lmt_all_sct));

        /* Initialize to NULL the limit array */
        lmt_msa[lmt_msa_idx]->lmt_dmn=NULL;

        /* Initialize to NULL the auxiliary MSA limit array; crucial to MSA in case of no limits to use all data to read */
        lmt[lmt_msa_idx]=NULL;

        /* If limits, make space for them */
        if (dmn_trv.lmt_dmn_nbr) lmt_msa[lmt_msa_idx]->lmt_dmn=(lmt_sct **)nco_malloc(dmn_trv.lmt_dmn_nbr*sizeof(lmt_sct *));

        /* And deep-copy the structure made while building limits  */
        lmt_msa[lmt_msa_idx]->BASIC_DMN=dmn_trv.BASIC_DMN;
        lmt_msa[lmt_msa_idx]->dmn_cnt=dmn_trv.dmn_cnt;
        lmt_msa[lmt_msa_idx]->dmn_nm=strdup(dmn_trv.nm);
        lmt_msa[lmt_msa_idx]->dmn_sz_org=dmn_trv.sz;
        lmt_msa[lmt_msa_idx]->lmt_dmn_nbr=dmn_trv.lmt_dmn_nbr;
        lmt_msa[lmt_msa_idx]->MSA_USR_RDR=dmn_trv.MSA_USR_RDR;
        lmt_msa[lmt_msa_idx]->WRP=dmn_trv.WRP;

        /* Loop needed limits */
        for(int lmt_idx=0;lmt_idx<dmn_trv.lmt_dmn_nbr;lmt_idx++){

          if(dbg_lvl_get() >= nco_dbg_dev){
            (void)fprintf(stdout,"[%d]:%s->",lmt_idx,trv_tbl->lst_dmn[dmn_idx].lmt_dmn[lmt_idx]->nm);
          }

          /* Alloc new limit */
          lmt_msa[lmt_msa_idx]->lmt_dmn[lmt_idx]=(lmt_sct *)nco_malloc(sizeof(lmt_sct));

          /* Initialize NULL/invalid */
          (void)nco_lmt_init(lmt_msa[lmt_msa_idx]->lmt_dmn[lmt_idx]);

          /* Deep copy from table to local array */ 
          (void)nco_lmt_cpy(trv_tbl->lst_dmn[dmn_idx].lmt_dmn[lmt_idx],lmt_msa[lmt_msa_idx]->lmt_dmn[lmt_idx]);

          if(dbg_lvl_get() >= nco_dbg_dev){
            (void)fprintf(stdout,"<-[%d]:%s: ",lmt_idx,lmt_msa[lmt_msa_idx]->lmt_dmn[lmt_idx]->nm);
          }

        } /* End Loop needed limits */

        /* But... wait... MSA super-dooper recursive function needs an allocated limit always; 2 options here:
        1) Allocate a dummy limit to read all data 
        2) Modify MSA to allow for the simplest case of no limits; MSA passes "var_sct var" while recursing;
        the variable for number of elemnts (.sz) is being incremented...but we can use the member of "lmt_all_sct"
        that stores the *original* size "dmn_sz_org".     
        */

        if (dmn_trv.lmt_dmn_nbr == 0)
        {
          if(dbg_lvl_get() >= nco_dbg_dev) (void)fprintf(stdout,"Warning...no limit zone "); 

          /* Needed for MSA modulo arrays (cannot divide by zero) */ 
          lmt_msa[lmt_msa_idx]->dmn_cnt=1;
        }

        if(dbg_lvl_get() >= nco_dbg_dev) (void)fprintf(stdout,"...done!\n");  

        /* Increment special index match counter for MSA limits */
        lmt_msa_idx++;

        /* Exit found dimension loop */
        break;

      } /* Match full dimension name */ 
    } /* End  Loop unique dimensions (these contain limits)  */
  } /* Loop dimensions for object (variable) */

  /* Set defaults */
  (void)var_dfl_set(&var); 

  /* Initialize units string, overwrite later if necessary */
  unit_sng=&nul_chr;

  /* Get ID for requested variable */
  var.nm=(char *)strdup(var_trv->nm);
  var.nc_id=in_id;
  (void)nco_inq_varid(in_id,var_trv->nm,&var.id);

  /* Get type of variable (get also name and number of dimensions for validation against parameter object) */
  (void)nco_inq_var(in_id,var.id,var_nm,&var.type,&var.nbr_dim,(int *)NULL,(int *)NULL);

  /* Just make sure we got the right variable */
  assert(var.nbr_dim == var_trv->nbr_dmn);
  assert(strcmp(var_nm,var_trv->nm) == 0);


  /* Scalars */
  if(var.nbr_dim == 0){
    var.sz=1L;
    var.val.vp=nco_malloc(nco_typ_lng(var.type));
    /* Block is critical/thread-safe for identical/distinct in_id's */
    { /* begin potential OpenMP critical */
      (void)nco_get_var1(in_id,var.id,0L,var.val.vp,var.type);
    } /* end potential OpenMP critical */
  } /* end if */


  /* Call super-dooper recursive routine */
  var.val.vp=nco_msa_rcr_clc(0,var.nbr_dim,lmt,lmt_msa,&var);
  /* Call also initializes var.sz with final size */
  if(MD5_DIGEST) (void)nco_md5_chk(var_nm,var.sz*nco_typ_lng(var.type),in_id,(long *)NULL,(long *)NULL,var.val.vp);

  /* Warn if variable is packed */
  if(nco_pck_dsk_inq(in_id,&var)) (void)fprintf(stderr,"%s: WARNING about to print packed contents of variable \"%s\". Consider unpacking variable first using ncpdq -U.\n",prg_nm_get(),var_nm);

  /* Refresh number of attributes and missing value attribute, if any */
  var.has_mss_val=nco_mss_val_get(var.nc_id,&var);
  if(var.has_mss_val) val_sz_byt=nco_typ_lng(var.type);

  /* User supplied dlm_sng, print variable (includes nbr_dmn == 0) */  
  if(dlm_sng){
    char *fmt_sng_mss_val=NULL;

    /* Print each element with user-supplied formatting code */
    /* Replace C language '\X' escape codes with ASCII bytes */
    (void)sng_ascii_trn(dlm_sng);

    /* Assume -s argument (dlm_sng) formats entire string
    Otherwise, one could assume that field will be printed with format nco_typ_fmt_sng(var.type),
    and that user is only allowed to affect text between fields. 
    This would be accomplished with:
    (void)sprintf(var_sng,"%s%s",nco_typ_fmt_sng(var.type),dlm_sng);*/

    /* Find replacement format string at most once, then re-use */
#ifdef NCO_HAVE_REGEX_FUNCTIONALITY
    /* Replace printf()-format statements with format for missing values */
    fmt_sng_mss_val=nco_fmt_sng_printf_subst(dlm_sng);
#endif /* !NCO_HAVE_REGEX_FUNCTIONALITY */

    for(lmn=0;lmn<var.sz;lmn++){

      /* memcmp() triggers pedantic warning unless pointer arithmetic is cast to type char * */
      if(PRN_MSS_VAL_BLANK) is_mss_val = var.has_mss_val ? !memcmp((char *)var.val.vp+lmn*val_sz_byt,var.mss_val.vp,(size_t)val_sz_byt) : False; 

      if(PRN_MSS_VAL_BLANK && is_mss_val){
        if(strcmp(dlm_sng,fmt_sng_mss_val)) (void)fprintf(stdout,fmt_sng_mss_val,mss_val_sng); else (void)fprintf(stdout,"%s, ",mss_val_sng);
      }else{ /* !is_mss_val */
        switch(var.type){
        case NC_FLOAT: (void)fprintf(stdout,dlm_sng,var.val.fp[lmn]); break;
        case NC_DOUBLE: (void)fprintf(stdout,dlm_sng,var.val.dp[lmn]); break;
        case NC_SHORT: (void)fprintf(stdout,dlm_sng,var.val.sp[lmn]); break;
        case NC_INT: (void)fprintf(stdout,dlm_sng,var.val.ip[lmn]); break;
        case NC_CHAR: (void)fprintf(stdout,dlm_sng,var.val.cp[lmn]); break;
        case NC_BYTE: (void)fprintf(stdout,dlm_sng,var.val.bp[lmn]); break;
        case NC_UBYTE: (void)fprintf(stdout,dlm_sng,var.val.ubp[lmn]); break;
        case NC_USHORT: (void)fprintf(stdout,dlm_sng,var.val.usp[lmn]); break;
        case NC_UINT: (void)fprintf(stdout,dlm_sng,var.val.uip[lmn]); break;
        case NC_INT64: (void)fprintf(stdout,dlm_sng,var.val.i64p[lmn]); break;
        case NC_UINT64: (void)fprintf(stdout,dlm_sng,var.val.ui64p[lmn]); break;
        case NC_STRING: (void)fprintf(stdout,dlm_sng,var.val.sngp[lmn]); break;
        default: nco_dfl_case_nc_type_err(); break;
        } /* end switch */
      } /* !is_mss_val */
    } /* end loop over element */
    (void)fprintf(stdout,"\n");

    if(fmt_sng_mss_val) fmt_sng_mss_val=(char *)nco_free(fmt_sng_mss_val);

  } /* end if dlm_sng */

  if(PRN_DMN_UNITS){
    const char units_nm[]="units"; /* [sng] Name of units attribute */
    int rcd_lcl; /* [rcd] Return code */
    int att_id; /* [id] Attribute ID */
    long att_sz;
    nc_type att_typ;

    /* Does variable have character attribute named units_nm? */
    rcd_lcl=nco_inq_attid_flg(in_id,var.id,units_nm,&att_id);
    if(rcd_lcl == NC_NOERR){
      (void)nco_inq_att(in_id,var.id,units_nm,&att_typ,&att_sz);
      if(att_typ == NC_CHAR){
        unit_sng=(char *)nco_malloc((att_sz+1)*nco_typ_lng(att_typ));
        (void)nco_get_att(in_id,var.id,units_nm,unit_sng,att_typ);
        unit_sng[(att_sz+1)*nco_typ_lng(att_typ)-1]='\0';
        MALLOC_UNITS_SNG=True; /* [flg] Allocated memory for units string */
      } /* end if */
    } /* end if */
  } /* end if PRN_DMN_UNITS */

  if(var.nbr_dim == 0 && dlm_sng == NULL){
    /* Variable is scalar, byte, or character */
    lmn=0L;
    if(PRN_MSS_VAL_BLANK) is_mss_val = var.has_mss_val ? !memcmp(var.val.vp,var.mss_val.vp,(size_t)val_sz_byt) : False; 
    if(PRN_DMN_VAR_NM) (void)sprintf(var_sng,"%%s = %s %%s\n",nco_typ_fmt_sng(var.type)); else (void)sprintf(var_sng,"%s\n",nco_typ_fmt_sng(var.type));
    if(PRN_MSS_VAL_BLANK && is_mss_val){
      if(PRN_DMN_VAR_NM) (void)fprintf(stdout,"%s = %s %s\n",var_nm,mss_val_sng,unit_sng); else (void)fprintf(stdout,"%s\n",mss_val_sng); 
    }else{ /* !is_mss_val */
      if(PRN_DMN_VAR_NM){
        switch(var.type){
        case NC_FLOAT: (void)fprintf(stdout,var_sng,var_nm,var.val.fp[lmn],unit_sng); break;
        case NC_DOUBLE: (void)fprintf(stdout,var_sng,var_nm,var.val.dp[lmn],unit_sng); break;
        case NC_SHORT: (void)fprintf(stdout,var_sng,var_nm,var.val.sp[lmn],unit_sng); break;
        case NC_INT: (void)fprintf(stdout,var_sng,var_nm,var.val.ip[lmn],unit_sng); break;
        case NC_CHAR:
          if(var.val.cp[lmn] != '\0'){
            (void)sprintf(var_sng,"%%s = '%s' %%s\n",nco_typ_fmt_sng(var.type));
            (void)fprintf(stdout,var_sng,var_nm,var.val.cp[lmn],unit_sng);
          }else{ /* Deal with NUL character here */
            (void)fprintf(stdout, "%s = \"\" %s\n",var_nm,unit_sng);
          } /* end if */
          break;
        case NC_BYTE: (void)fprintf(stdout,var_sng,var_nm,(unsigned char)var.val.bp[lmn],unit_sng); break;
        case NC_UBYTE: (void)fprintf(stdout,var_sng,var_nm,var.val.ubp[lmn],unit_sng); break;
        case NC_USHORT: (void)fprintf(stdout,var_sng,var_nm,var.val.usp[lmn],unit_sng); break;
        case NC_UINT: (void)fprintf(stdout,var_sng,var_nm,var.val.uip[lmn],unit_sng); break;
        case NC_INT64: (void)fprintf(stdout,var_sng,var_nm,var.val.i64p[lmn],unit_sng); break;
        case NC_UINT64: (void)fprintf(stdout,var_sng,var_nm,var.val.ui64p[lmn],unit_sng); break;
        case NC_STRING: (void)fprintf(stdout,var_sng,var_nm,var.val.sngp[lmn],unit_sng); break;
        default: nco_dfl_case_nc_type_err(); break;
        } /* end switch */
      }else{ /* !PRN_DMN_VAR_NM */
        switch(var.type){
        case NC_FLOAT: (void)fprintf(stdout,var_sng,var.val.fp[lmn]); break;
        case NC_DOUBLE: (void)fprintf(stdout,var_sng,var.val.dp[lmn]); break;
        case NC_SHORT: (void)fprintf(stdout,var_sng,var.val.sp[lmn]); break;
        case NC_INT: (void)fprintf(stdout,var_sng,var.val.ip[lmn]); break;
        case NC_CHAR:
          if(var.val.cp[lmn] != '\0'){
            (void)sprintf(var_sng,"'%s'\n",nco_typ_fmt_sng(var.type));
            (void)fprintf(stdout,var_sng,var.val.cp[lmn]);
          }else{ /* Deal with NUL character here */
            (void)fprintf(stdout, "\"\"\n");
          } /* end if */
          break;
        case NC_BYTE: (void)fprintf(stdout,var_sng,(unsigned char)var.val.bp[lmn]); break;
        case NC_UBYTE: (void)fprintf(stdout,var_sng,var.val.ubp[lmn]); break;
        case NC_USHORT: (void)fprintf(stdout,var_sng,var.val.usp[lmn]); break;
        case NC_UINT: (void)fprintf(stdout,var_sng,var.val.uip[lmn]); break;
        case NC_INT64: (void)fprintf(stdout,var_sng,var.val.i64p[lmn]); break;
        case NC_UINT64: (void)fprintf(stdout,var_sng,var.val.ui64p[lmn]); break;
        case NC_STRING: (void)fprintf(stdout,var_sng,var.val.sngp[lmn]); break;
        default: nco_dfl_case_nc_type_err(); break;
        } /* end switch */
      } /* !PRN_DMN_VAR_NM */
    } /* !is_mss_val */
  } /* end if variable is scalar, byte, or character */

  if(var.nbr_dim > 0 && dlm_sng == NULL){

    /* Create mod_map_in */
    for(int idx=0;idx<var.nbr_dim;idx++) mod_map_in[idx]=1L;
    for(int idx=0;idx<var.nbr_dim;idx++)
    {
      for(int jdx=idx+1;jdx<var.nbr_dim;jdx++)
      {
        mod_map_in[idx]*=lmt_msa[jdx]->dmn_sz_org;
      }
    }

    /* Create mod_map_cnt */
    for(int idx=0;idx<var.nbr_dim;idx++) mod_map_cnt[idx]=1L;
    for(int idx=0;idx<var.nbr_dim;idx++)
    {
      for(int jdx=idx;jdx<var.nbr_dim;jdx++)
      {
        mod_map_cnt[idx]*=lmt_msa[jdx]->dmn_cnt;
      }
    }

    /* Read coordinate dimensions if required */
    if(PRN_DMN_IDX_CRD_VAL){

      for(int idx=0;idx<var.nbr_dim;idx++){
        dim[idx].val.vp=NULL;
        dim[idx].nm=lmt_msa[idx]->dmn_nm;
        rcd=nco_inq_varid_flg(in_id,dim[idx].nm,&dim[idx].cid);
        /* If not a variable */
        if(rcd != NC_NOERR){
          dim[idx].is_crd_dmn=False;
          dim[idx].cid=-1;
          continue;
        } /* end if */

        dim[idx].is_crd_dmn=True;
        (void)nco_inq_vartype(in_id,dim[idx].cid,&dim[idx].type);
        var_crd.nc_id=in_id;
        var_crd.nm=dim[idx].nm;
        var_crd.type=dim[idx].type;
        var_crd.id=dim[idx].cid;
        /* Read coordinate variable with limits applied */
        dim[idx].val.vp=nco_msa_rcr_clc(0,1,lmt,lmt_msa+idx,&var_crd);

        /* Typecast pointer before use */  
        (void)cast_void_nctype(dim[idx].type,&dim[idx].val);
      }/* end for */
    } /* end if */

    for(lmn=0;lmn<var.sz;lmn++){

      /* memcmp() triggers pedantic warning unless pointer arithmetic is cast to type char * */
      if(PRN_MSS_VAL_BLANK) is_mss_val = var.has_mss_val ? !memcmp((char *)var.val.vp+lmn*val_sz_byt,var.mss_val.vp,(size_t)val_sz_byt) : False; 

      /* Calculate RAM indices from current limit */
      for(int idx=0;idx <var.nbr_dim;idx++) 
      {
        dmn_sbs_ram[idx]=(lmn%mod_map_cnt[idx])/(idx == var.nbr_dim-1 ? 1L : mod_map_cnt[idx+1]);
      }

      /* Calculate disk indices from RAM indices */
      (void)nco_msa_ram_2_dsk(dmn_sbs_ram,lmt_msa,var.nbr_dim,dmn_sbs_dsk,(lmn==var.sz-1));

      /* Find variable index relative to disk */
      var_dsk=0;
      for(int idx=0;idx <var.nbr_dim;idx++)
      {
        var_dsk+=dmn_sbs_dsk[idx]*mod_map_in[idx];
      }

      /* Skip rest of loop unless element is first in string */
      if(var.type == NC_CHAR && dmn_sbs_ram[var.nbr_dim-1] > 0) goto lbl_chr_prn;

      /* Print dimensions with indices along with values if they are coordinate variables */
      if(PRN_DMN_IDX_CRD_VAL){
        int dmn_idx;
        long dmn_sbs_prn;
        long crd_idx_crr;
        char dmn_sng[NCO_MAX_LEN_FMT_SNG];

        /* Loop over dimensions whose coordinates are to be printed */
        for(int idx=0;idx<var.nbr_dim;idx++){

          /* Reverse dimension ordering for Fortran convention */
          if(FORTRAN_IDX_CNV) dmn_idx=var.nbr_dim-1-idx; else dmn_idx=idx;

          /* Format and print dimension part of output string for non-coordinate variables */
          if(dim[dmn_idx].cid == var.id) continue; /* If variable is a coordinate then skip printing until later */
          if(!dim[dmn_idx].is_crd_dmn){ /* If dimension is not a coordinate... */
            if(PRN_DMN_VAR_NM){
              if(FORTRAN_IDX_CNV) (void)fprintf(stdout,"%s(%ld) ",dim[dmn_idx].nm,dmn_sbs_dsk[dmn_idx]+1L); else (void)fprintf(stdout,"%s[%ld] ",dim[dmn_idx].nm,dmn_sbs_dsk[dmn_idx]);
            } /* !PRN_DMN_VAR_NM */
            continue;
          } /* end if */

          if(PRN_DMN_VAR_NM) (void)sprintf(dmn_sng,"%%s[%%ld]=%s ",nco_typ_fmt_sng(dim[dmn_idx].type)); else (void)sprintf(dmn_sng,"%s ",nco_typ_fmt_sng(dim[dmn_idx].type));
          dmn_sbs_prn=dmn_sbs_dsk[dmn_idx];

          if(FORTRAN_IDX_CNV){
            (void)nco_msa_c_2_f(dmn_sng);
            dmn_sbs_prn++;
          } /* end if */

          /* Account for hyperslab offset in coordinate values*/
          crd_idx_crr=dmn_sbs_ram[dmn_idx];
          if(PRN_DMN_VAR_NM){
            switch(dim[dmn_idx].type){
            case NC_FLOAT: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,dim[dmn_idx].val.fp[crd_idx_crr]); break;
            case NC_DOUBLE: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,dim[dmn_idx].val.dp[crd_idx_crr]); break;
            case NC_SHORT: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,dim[dmn_idx].val.sp[crd_idx_crr]); break;
            case NC_INT: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,dim[dmn_idx].val.ip[crd_idx_crr]); break;
            case NC_CHAR: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,dim[dmn_idx].val.cp[crd_idx_crr]); break;
            case NC_BYTE: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,(unsigned char)dim[dmn_idx].val.bp[crd_idx_crr]); break;
            case NC_UBYTE: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,dim[dmn_idx].val.ubp[crd_idx_crr]); break;
            case NC_USHORT: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,dim[dmn_idx].val.usp[crd_idx_crr]); break;
            case NC_UINT: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,dim[dmn_idx].val.uip[crd_idx_crr]); break;
            case NC_INT64: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,dim[dmn_idx].val.i64p[crd_idx_crr]); break;
            case NC_UINT64: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,dim[dmn_idx].val.ui64p[crd_idx_crr]); break;
            case NC_STRING: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,dim[dmn_idx].val.sngp[crd_idx_crr]); break;
            default: nco_dfl_case_nc_type_err(); break;
            } /* end switch */
          }else{ /* !PRN_DMN_VAR_NM */
            switch(dim[dmn_idx].type){
            case NC_FLOAT: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].val.fp[crd_idx_crr]); break;
            case NC_DOUBLE: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].val.dp[crd_idx_crr]); break;
            case NC_SHORT: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].val.sp[crd_idx_crr]); break;
            case NC_INT: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].val.ip[crd_idx_crr]); break;
            case NC_CHAR: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].val.cp[crd_idx_crr]); break;
            case NC_BYTE: (void)fprintf(stdout,dmn_sng,(unsigned char)dim[dmn_idx].val.bp[crd_idx_crr]); break;
            case NC_UBYTE: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].val.ubp[crd_idx_crr]); break;
            case NC_USHORT: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].val.usp[crd_idx_crr]); break;
            case NC_UINT: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].val.uip[crd_idx_crr]); break;
            case NC_INT64: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].val.i64p[crd_idx_crr]); break;
            case NC_UINT64: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].val.ui64p[crd_idx_crr]); break;
            case NC_STRING: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].val.sngp[crd_idx_crr]); break;
            default: nco_dfl_case_nc_type_err(); break;
            } /* end switch */
          } /* !PRN_DMN_VAR_NM */
        } /* end loop over dimensions */
      } /* end if PRN_DMN_IDX_CRD_VAL */

      /* Print all characters in last dimension each time penultimate dimension subscript changes to its start value */
lbl_chr_prn:

      if(var.type == NC_CHAR){
        static nco_bool NULL_IN_SLAB;
        static char *prn_sng;
        static int chr_cnt;
        static long dmn_sz;
        static long var_dsk_srt;
        static long var_dsk_end;

        /* At beginning of character array */
        if(dmn_sbs_ram[var.nbr_dim-1] == 0L) {
          dmn_sz=lmt_msa[var.nbr_dim-1]->dmn_cnt;
          prn_sng=(char *)nco_malloc((size_t)dmn_sz+1UL);
          var_dsk_srt=var_dsk;
          var_dsk_end=var_dsk;
          chr_cnt=0;
          NULL_IN_SLAB=False;
        } /* end if */

        /* In middle of array---save characters to prn_sng */
        prn_sng[chr_cnt++]=var.val.cp[lmn];
        if(var.val.cp[lmn] == '\0' && !NULL_IN_SLAB){
          var_dsk_end=var_dsk;
          NULL_IN_SLAB=True;
        } /* end if */

        /* At end of character array */
        if(dmn_sbs_ram[var.nbr_dim-1] == dmn_sz-1 ){
          if(NULL_IN_SLAB){
            (void)sprintf(var_sng,"%%s[%%ld--%%ld]=\"%%s\" %%s");
          }else{
            (void)sprintf(var_sng,"%%s[%%ld--%%ld]='%%s' %%s");
            prn_sng[chr_cnt]='\0';
            var_dsk_end=var_dsk;   
          } /* end if */
          if(FORTRAN_IDX_CNV){ 
            (void)nco_msa_c_2_f(var_sng);
            var_dsk_srt++; 
            var_dsk_end++; 
          } /* end if */
          (void)fprintf(stdout,var_sng,var_nm,var_dsk_srt,var_dsk_end,prn_sng,unit_sng);
          (void)fprintf(stdout,"\n");
          (void)fflush(stdout);
          (void)nco_free(prn_sng);
        } /* endif */
        continue;
      } /* end if NC_CHAR */

      /* Print variable name, index, and value */
      if(PRN_DMN_VAR_NM) (void)sprintf(var_sng,"%%s[%%ld]=%s %%s\n",nco_typ_fmt_sng(var.type)); else (void)sprintf(var_sng,"%s\n",nco_typ_fmt_sng(var.type));
      if(FORTRAN_IDX_CNV){
        (void)nco_msa_c_2_f(var_sng);
        var_dsk++;
      } /* end if FORTRAN_IDX_CNV */

      if(PRN_MSS_VAL_BLANK && is_mss_val){
        if(PRN_DMN_VAR_NM) (void)fprintf(stdout,"%s[%ld]=%s %s\n",var_nm,var_dsk,mss_val_sng,unit_sng); else (void)fprintf(stdout,"%s\n",mss_val_sng); 
      }else{ /* !is_mss_val */
        if(PRN_DMN_VAR_NM){
          switch(var.type){
          case NC_FLOAT: (void)fprintf(stdout,var_sng,var_nm,var_dsk,var.val.fp[lmn],unit_sng); break;
          case NC_DOUBLE: (void)fprintf(stdout,var_sng,var_nm,var_dsk,var.val.dp[lmn],unit_sng); break;
          case NC_SHORT: (void)fprintf(stdout,var_sng,var_nm,var_dsk,var.val.sp[lmn],unit_sng); break;
          case NC_INT: (void)fprintf(stdout,var_sng,var_nm,var_dsk,var.val.ip[lmn],unit_sng); break;
          case NC_CHAR: (void)fprintf(stdout,var_sng,var_nm,var_dsk,var.val.cp[lmn],unit_sng); break;
          case NC_BYTE: (void)fprintf(stdout,var_sng,var_nm,var_dsk,(unsigned char)var.val.bp[lmn],unit_sng); break;
          case NC_UBYTE: (void)fprintf(stdout,var_sng,var_nm,var_dsk,var.val.ubp[lmn],unit_sng); break;
          case NC_USHORT: (void)fprintf(stdout,var_sng,var_nm,var_dsk,var.val.usp[lmn],unit_sng); break;
          case NC_UINT: (void)fprintf(stdout,var_sng,var_nm,var_dsk,var.val.uip[lmn],unit_sng); break;
          case NC_INT64: (void)fprintf(stdout,var_sng,var_nm,var_dsk,var.val.i64p[lmn],unit_sng); break;
          case NC_UINT64: (void)fprintf(stdout,var_sng,var_nm,var_dsk,var.val.ui64p[lmn],unit_sng); break;
          case NC_STRING: (void)fprintf(stdout,var_sng,var_nm,var_dsk,var.val.sngp[lmn],unit_sng); break;
          default: nco_dfl_case_nc_type_err(); break;
          } /* end switch */
        }else{ /* !PRN_DMN_VAR_NM */
          switch(var.type){
          case NC_FLOAT: (void)fprintf(stdout,var_sng,var.val.fp[lmn],unit_sng); break;
          case NC_DOUBLE: (void)fprintf(stdout,var_sng,var.val.dp[lmn],unit_sng); break;
          case NC_SHORT: (void)fprintf(stdout,var_sng,var.val.sp[lmn],unit_sng); break;
          case NC_INT: (void)fprintf(stdout,var_sng,var.val.ip[lmn],unit_sng); break;
          case NC_CHAR: (void)fprintf(stdout,var_sng,var.val.cp[lmn],unit_sng); break;
          case NC_BYTE: (void)fprintf(stdout,var_sng,(unsigned char)var.val.bp[lmn],unit_sng); break;
          case NC_UBYTE: (void)fprintf(stdout,var_sng,var.val.ubp[lmn],unit_sng); break;
          case NC_USHORT: (void)fprintf(stdout,var_sng,var.val.usp[lmn],unit_sng); break;
          case NC_UINT: (void)fprintf(stdout,var_sng,var.val.uip[lmn],unit_sng); break;
          case NC_INT64: (void)fprintf(stdout,var_sng,var.val.i64p[lmn],unit_sng); break;
          case NC_UINT64: (void)fprintf(stdout,var_sng,var.val.ui64p[lmn],unit_sng); break;
          case NC_STRING: (void)fprintf(stdout,var_sng,var.val.sngp[lmn],unit_sng); break;
          default: nco_dfl_case_nc_type_err(); break;
          } /* end switch */
        } /* !PRN_DMN_VAR_NM */
      } /* !is_mss_val */
    } /* end loop over elements */


    /* Additional newline between consecutive variables or final variable and prompt */
    (void)fprintf(stdout,"\n");
    (void)fflush(stdout);
  } /* end if variable has more than one dimension */

  /* Free value buffer */
  var.val.vp=nco_free(var.val.vp);
  var.mss_val.vp=nco_free(var.mss_val.vp);
  var.nm=(char *)nco_free(var.nm);

  if(MALLOC_UNITS_SNG) unit_sng=(char *)nco_free(unit_sng);

  if(PRN_DMN_IDX_CRD_VAL && dlm_sng==NULL){
    for(int idx=0;idx<var.nbr_dim;idx++) dim[idx].val.vp=nco_free(dim[idx].val.vp);
  } /* end if */

  /* Loop limits */
  for(int lmt_idx_var=0;lmt_idx_var<lmt_msa_idx;lmt_idx_var++) {
    /* Allocated number of limits */
    int lmt_dmn_nbr=lmt_msa[lmt_idx_var]->lmt_dmn_nbr;

    /* Loop needed limits */
    for(int lmt_idx=0;lmt_idx<lmt_dmn_nbr;lmt_idx++){
      lmt_msa[lmt_idx_var]->lmt_dmn[lmt_idx]=nco_lmt_free(lmt_msa[lmt_idx_var]->lmt_dmn[lmt_idx]);
    } /* End Loop needed limits */

    lmt_msa[lmt_idx_var]->lmt_dmn=(lmt_sct **)nco_free(lmt_msa[lmt_idx_var]->lmt_dmn);
  }/* End Loop limits */

  /* Finally...Phew... */
  if(var.nbr_dim > 0){
    (void)nco_free(lmt_msa);
    (void)nco_free(lmt);
  } /* end if */


} /* end nco_msa_prn_var_val_trv() */






