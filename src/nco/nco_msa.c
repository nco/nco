/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_msa.c,v 1.21 2004-07-27 06:16:36 zender Exp $ */

/* Purpose: Multi-slabbing algorithm */

/* Copyright (C) 1995--2004 Charlie Zender and Henry Butowsky
   This software may be modified and/or re-distributed under the terms of the GNU General Public License (GPL) Version 2
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

#include "nco_msa.h" /* Multi-slabbing algorithm */

/* fxm: strings statically allocated with MAX_LEN_FMT_SNG chars are susceptible to buffer overflow attacks */
/* Length should be computed at run time but is a pain */
#define MAX_LEN_FMT_SNG 100

void *
nco_msa_rec_clc /* [fnc] Multi-slab algorithm (recursive routine, returns a single slab pointer */
(int dpt_crr, /* [nbr] Current depth, we start at 0 */
 int dpt_crr_max, /* [nbr] Maximium depth (i.e., number of dimensions in variable (does not change) */
 lmt_sct **lmt, /* [sct] Limits of current hyperslabs (these change as we recurse) */
 lmt_all_sct **lmt_lst, /* [sct] List of limits in each dimension (this remains STATIC as we recurse) */
 var_sct *vara) /* [sct] Information for routine to read variable information and pass information between calls */
{
  /* Purpose: Multi-slab algorithm (recursive routine, returns a single slab pointer */
  int idx;
  int nbr_slb;
  void *vp;
  
  if(dpt_crr == dpt_crr_max) goto read_lbl;
  
  nbr_slb=lmt_lst[dpt_crr]->lmt_dmn_nbr;

  if(nbr_slb == 1){
    lmt[dpt_crr]=lmt_lst[dpt_crr]->lmt_dmn[0];
    vp=nco_msa_rec_clc(dpt_crr+1,dpt_crr_max,lmt,lmt_lst,vara);
    return vp;
  } /* end if */
  
  /* Here we deal with multiple hyperslabs */
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
      cp_wrp[idx]=(char *)nco_msa_rec_clc(dpt_crr+1,dpt_crr_max,lmt,lmt_lst,vara);
      cp_sz[idx]=vara->sz;
    } /* end loop over idx */
    
    for(idx=0;idx<dpt_crr_max;idx++) 
      var_sz*=(idx<dpt_crr ? lmt[idx]->cnt : lmt_lst[idx]->dmn_cnt);
    
    vp=(void *)nco_calloc((size_t)var_sz,nco_typ_lng(vara->type));
    
    lcnt=nco_typ_lng(vara->type);
    for(idx=dpt_crr+1;idx<dpt_crr_max;idx++) 
      lcnt*=lmt_lst[idx]->dmn_cnt;
    
    cp_inc=(ptrdiff_t)(lcnt*lmt_lst[dpt_crr]->dmn_cnt);
    
    cp_max=(ptrdiff_t)(var_sz*nco_typ_lng(vara->type));
    
    for(idx=0;idx<nbr_slb;idx++) indices[idx]=lmt_lst[dpt_crr]->lmt_dmn[idx]->srt;
    
    cp_fst=0L;

    /* Deal first with wrapped dimensions
       True wrapped dimensions have nbr_slb = 2, are "continuous", and do not overlap */
    if(lmt_lst[dpt_crr]->WRP){
      for(slb_idx=0;slb_idx<2;slb_idx++){
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
    }else{
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
 
    (void)nco_free(indices);
    (void)nco_free(cp_sz);
    for(idx=0;idx<nbr_slb;idx++) (void)nco_free(cp_wrp[idx]);
    
    vara->sz=var_sz;
    return vp;
  } /* endif multiple hyperslabs */

 read_lbl:
 { 
    long var_sz=1L;
    long mult_srd=1L;
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
      mult_srd*=lmt[idx]->srd;
    } /* end loop over idx */
    
    vp=(void*)nco_malloc(var_sz *nco_typ_lng(vara->type));
    
    /* Check for stride */
    if(mult_srd == 1L)
      (void)nco_get_vara(vara->nc_id,vara->id,dmn_srt,dmn_cnt,vp,vara->type);
    else
      (void)nco_get_varm(vara->nc_id,vara->id,dmn_srt,dmn_cnt,dmn_srd,(long *)NULL,vp,vara->type);
    
    /* Put size into vara */
    vara->sz=var_sz;
    return vp;
  }/* end read_lbl */
  
} /* end nco_msa_rec_clc() */

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
  
  while(nco_msa_clc_idx(False,lmt_i,&indices[0],&lmt,&slb_nbr))
    printf("slb_nbr=%d srt=%ld end=%ld cnt=%ld srd=%ld\n",slb_nbr,lmt.srt,lmt.end,lmt.cnt,lmt.srd);
} /* end nco_msa_prn_idx() */

bool /* [flg] There are more limits to process in the slab */
nco_msa_clc_idx
(bool NORMALIZE,
 lmt_all_sct *lmt_a, /* I list of lmts for each dimension  */
 long *indices, /* I/O so routine can keep track of where its at */
 lmt_sct *lmt, /* O Output hyperslab */
 int *slb) /* slab which above limit refers to */ 
{
  /* A very messy unclear in-efficient routine that needs re-writing
     if NORMALIZE then It returns the slab number and the hyperslab in lmt- Note
     VERY IMPORTANT - This is the slab WHITH-IN the slab
     So the stride is ALWAYS 1 */
  int i;
  int size=lmt_a->lmt_dmn_nbr;
  bool *min;
  
  int prv_slb=0;
  int crr_slb=0;
  long crr_idx;
  long prv_idx=long_CEWI;
  long cnt=0L;
  
  min=(bool*)nco_malloc(size*sizeof(bool));
  
  lmt->srt=-1L;
  lmt->cnt=0L;
  lmt->srd=0L;
  
  while(++cnt){
    crr_idx=nco_msa_min_idx(indices,min,size);
    
    crr_slb=-1;
    for(i=0;i <size;i++)
      if(min[i]){crr_slb=i;break;}
    
    if(crr_slb == -1){
      if(lmt->srt == -1) return False;
      else break;
    }
    
    if(min[prv_slb]) crr_slb=prv_slb;
    
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
    
    for(i=0;i<size;i++){
      if(min[i]){
	indices[i]+=lmt_a->lmt_dmn[i]->srd;
	if(indices[i] > lmt_a->lmt_dmn[i]->end) indices[i]=-1;
      }
    } /* end loop over i */
    prv_idx=crr_idx;
    prv_slb=crr_slb;
  } /* end while */
  
  *slb=prv_slb;
  
  /* normalize the slab */
  if(NORMALIZE){
    lmt->srt=(lmt->srt-lmt_a->lmt_dmn[*slb]->srt)/(lmt_a->lmt_dmn[*slb]->srd);
    lmt->end=(lmt->end-lmt_a->lmt_dmn[*slb]->srt)/(lmt_a->lmt_dmn[*slb]->srd);
    lmt->srd=1L;
  } /* end if */
  return True;
} /* end nco_msa_clc_idx() */

void
nco_msa_ram_2_dsk(  /* convert hyperslab indices (in RAM) to hyperlsab indices relative */
		  long *dmn_sbs_ram,   /* to disk. */
lmt_all_sct **lmt_mult, 
int nbr_dim,
long *dmn_sbs_dsk,
bool FREE){
  /*  It doenst really convert RAM indices to disk indices , but given a set 
      of RAM indices finds the next set of dsk incdices. So it only works if 
      the indices fed to it are continuous */
  int idx;
  int jdx;
  int size;
  static int initialize;
  static long **dmn_indices;
  static long *dmn_sbs_prv;
  static bool min[100];

  if(!initialize){
    dmn_sbs_prv=(long *)nco_malloc(nbr_dim*sizeof(long));
    dmn_indices=(long **)nco_malloc(nbr_dim*sizeof(long *));
    for(idx=0;idx<nbr_dim;idx++){
      dmn_indices[idx]=(long *)nco_malloc(lmt_mult[idx]->lmt_dmn_nbr*sizeof(long));
      for(jdx=0;jdx<lmt_mult[idx]->lmt_dmn_nbr;jdx++)
	dmn_indices[idx][jdx]=lmt_mult[idx]->lmt_dmn[jdx]->srt;
      dmn_sbs_prv[idx]=-1L;
    }
    initialize=1;
  }
  for(idx=0;idx <nbr_dim;idx ++){
    size=lmt_mult[idx]->lmt_dmn_nbr;
    if(dmn_sbs_ram[idx] == dmn_sbs_prv[idx]) continue;

    if(lmt_mult[idx]->BASIC_DMN){
      dmn_sbs_dsk[idx]=dmn_sbs_ram[idx];
      continue;
    }
     
    /* re-initialize indices if 0*/
    if(dmn_sbs_ram[idx] == 0) 
      for(jdx=0;jdx<size;jdx++)
       	dmn_indices[idx][jdx]=lmt_mult[idx]->lmt_dmn[jdx]->srt;
   
    /* Deal with wrapping - we have 2 hyperlsbas to deal with */
     if(lmt_mult[idx]->WRP){
       if(dmn_indices[idx][0]<lmt_mult[idx]->dmn_sz_org){
	 dmn_sbs_dsk[idx]=dmn_indices[idx][0];
         dmn_indices[idx][0]+=lmt_mult[idx]->lmt_dmn[0]->srd;
        }else{
	 dmn_sbs_dsk[idx]=dmn_indices[idx][1];
         dmn_indices[idx][1]+=lmt_mult[idx]->lmt_dmn[1]->srd;
       }
     continue;
     }
         
    dmn_sbs_dsk[idx]=nco_msa_min_idx(dmn_indices[idx],min,size);

    for(jdx=0;jdx<size;jdx++){
      if(min[jdx]){
	dmn_indices[idx][jdx]+=lmt_mult[idx]->lmt_dmn[jdx]->srd;
	if(dmn_indices[idx][jdx] > lmt_mult[idx]->lmt_dmn[jdx]->end) dmn_indices[idx][jdx]=-1;
      }
    } /* end for  jdx */
  } /* end for idx */ 

  for(idx=0;idx<nbr_dim;idx++) dmn_sbs_prv[idx]=dmn_sbs_ram[idx];

  /* Free static space on last call */
  if(FREE){
    (void)nco_free(dmn_sbs_prv);
    for(idx=0;idx <nbr_dim;idx++)
      (void)nco_free(dmn_indices[idx]);
    (void)nco_free(dmn_indices);
    initialize=0;
  }
}

void 
nco_msa_clc_cnt(lmt_all_sct *lmt_lst)
{
  int idx;
  long cnt=0;
  int  size=lmt_lst->lmt_dmn_nbr;
  long *indices;
  bool *min;
  
  indices=(long *)nco_malloc(size*sizeof(long));
  min=(bool *)nco_malloc(size*sizeof(bool));
  
  if(size == 1){
    lmt_lst->dmn_cnt=lmt_lst->lmt_dmn[0]->cnt;
    return;
  } /* end if */
  /* initialise indices with srt from    */
  for(idx=0;idx<size;idx++)
    indices[idx]=lmt_lst->lmt_dmn[idx]->srt;
  
  while(nco_msa_min_idx(indices,min,size) != LONG_MAX){
    for(idx=0;idx<size;idx++){
      if(min[idx]){
	indices[idx]+=lmt_lst->lmt_dmn[idx]->srd;
	if(indices[idx] > lmt_lst->lmt_dmn[idx]->end) indices[idx]=-1;
      } /* end if */
    } /* end loop over idx */
    cnt++;
  } /* end while */
  lmt_lst->dmn_cnt=cnt;
} /* end nco_msa_clc_cnt() */

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

long
nco_msa_min_idx /* [fnc] Find minimum values in current */
(long *current, /* [idx] Current indices */
 bool *min, /* [flg] Minimum */
 int size) /* [nbr] Size of current and min */
{
  int i;
  long min_val=LONG_MAX;
  
  for(i=0;i< size;i++)
    if(current[i] != -1 && current[i]<min_val) min_val=current[i];
  
  for(i=0;i<size;i++)
    min[i]=((current[i] != -1 && current[i]== min_val) ? True : False);
  
  return min_val;
} /* end nco_msa_min_idx() */

void
nco_cpy_var_val_mlt_lmt /* [fnc] Copy variable data from input to output file */
(const int in_id, /* I [id] netCDF input file ID */
 const int out_id, /* I [id] netCDF output file ID */
 FILE * const fp_bnr, /* I [fl] Unformatted binary output file handle */
 const bool NCO_BNR_WRT, /* I [flg] Write binary file */
 char *var_nm, /* I [sng] Variable name */
 lmt_all_sct * const lmt_lst, /* I multi-hyperslab limits */
 int nbr_dmn_fl) /* I [nbr] Number of multi-hyperslab limits */
{
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
  long *dmn_map_out;
  long *dmn_map_srt;
  long var_sz=1L;
  
  nc_type var_type;
  
  var_sct vara;/* To hold basic data in_id, var_id, nctype for recusive routine */
  
  void *void_ptr;
  
  lmt_all_sct **lmt_mult;
  lmt_sct **lmt;
  
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
  
  /* Deal with scalar variables */
  if(nbr_dim == 0){
    var_sz=1L;
    void_ptr=nco_malloc(nco_typ_lng(var_type));
    (void)nco_get_var1(in_id,var_in_id,0L,void_ptr,var_type);
    (void)nco_put_var1(out_id,var_out_id,0L,void_ptr,var_type);
    if(NCO_BNR_WRT) nco_bnr_wrt(fp_bnr,var_nm,var_sz,var_type,void_ptr);
    (void)nco_free(void_ptr);
    return;
  } /* end if */
  
  dmn_map_in=(long *)nco_malloc(nbr_dim*sizeof(long));
  dmn_map_out=(long *)nco_malloc(nbr_dim*sizeof(long));
  dmn_map_srt=(long *)nco_malloc(nbr_dim*sizeof(long));
  dmn_id=(int *)nco_malloc(nbr_dim*sizeof(int));
  
  lmt_mult=(lmt_all_sct **)nco_malloc(nbr_dim*sizeof(lmt_all_sct *));
  lmt=(lmt_sct **)nco_malloc(nbr_dim*sizeof(lmt_sct *));
  
  /* Get dimension IDs from input file */
  (void)nco_inq_vardimid(in_id,var_in_id,dmn_id);
  
  /* Initialize lmt_mult with multi-limits from lmt_lst limits */
  /* Get dimension sizes from input file */
  for(idx=0;idx<nbr_dim;idx++){
    for(jdx=0;jdx<nbr_dmn_fl;jdx++){
      if(dmn_id[idx] == lmt_lst[jdx].lmt_dmn[0]->id){
	lmt_mult[idx]=&lmt_lst[jdx];
        break;
      } /* end if */
    } /* end loop over jdx */
    /* Create maps now---they maybe useful later */ 
    (void)nco_inq_dimlen(in_id,dmn_id[idx],&dmn_map_in[idx]);
    dmn_map_out[idx]=lmt_mult[idx]->dmn_cnt;
    dmn_map_srt[idx]=0L;
  } /* end for */
  
  /* Initalize vara with in_id, var_in_id, nctype, etc. so recursive routine can read data */
  vara.nm=var_nm;
  vara.id=var_in_id;
  vara.nc_id=in_id;
  vara.type=var_type;
  
  /* Call super-dooper recursive routine */
  void_ptr=nco_msa_rec_clc(0,nbr_dim,lmt,lmt_mult,&vara);
  var_sz=vara.sz;
  
  (void)nco_put_vara(out_id,var_out_id,dmn_map_srt,dmn_map_out,void_ptr,var_type);
  
  if(NCO_BNR_WRT) nco_bnr_wrt(fp_bnr,var_nm,var_sz,var_type,void_ptr);
  
  (void)nco_free(void_ptr);
  (void)nco_free(dmn_map_in);
  (void)nco_free(dmn_map_out);
  (void)nco_free(dmn_map_srt);
  (void)nco_free(dmn_id);
  (void)nco_free(lmt_mult);
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
(const int in_id, /* I [id] netCDF input file ID */
 const char * const var_nm, /* I [sng] Variable name */
 lmt_all_sct * const lmt_lst, /* I [sct] Dimension limits */
 const int lmt_nbr, /* I [nbr] number of dimensions with user-specified limits */
 char * const dlm_sng, /* I [sng] User-specified delimiter string, if any */
 const bool FORTRAN_IDX_CNV, /* I [flg] Hyperslab indices obey Fortran convention */
 const bool PRINT_DIMENSIONAL_UNITS, /* I [flg] Print units attribute, if any */
 const bool PRN_DMN_IDX_CRD_VAL) /* I [flg] Print dimension/coordinate indices/values */
{
  /* Purpose:
     Get variable with limits from input file
     User supplied dlm_sng, print var (includes nbr_dmim == 0)
     Get dimensional units
     if nbr_dim ==0 and dlm_sng==NULL  print variable
     if PRN.. = False print var taking account of FORTRAN (need var indices)
     if PRN_DMN_IDX_CRD_VAL then read in co-ord dims
     if PRN.. = True print var taking account of FORTRAN (Use dims to calculate var indices */
  
  char nul_chr='\0';
  char var_sng[MAX_LEN_FMT_SNG];
  char *unit_sng;
  
  /* Get variable with limits from input file */
  int rcd;
  int idx;
  int jdx;
  int *dmn_id=NULL_CEWI;
  
  /* For regular data */
  long lmn;
  
  dmn_sct *dim=NULL_CEWI;
     
  lmt_all_sct **lmt_mult=NULL_CEWI;
  lmt_sct **lmt=NULL_CEWI;
  
  var_sct var;

  /* Initialize units string, overwrite later if necessary */
  unit_sng=&nul_chr;

  /* Get var_id for requested variable */
  var.nm=(char *)strdup(var_nm);
  var.nc_id=in_id;
  nco_inq_varid(in_id,var_nm,&var.id);
  
  /* Get type and number of dimensions for variable */
  (void)nco_inq_var(in_id,var.id,(char *)NULL,&var.type,&var.nbr_dim,(int *)NULL,(int *)NULL);
  
  /* Deal with scalar variables */
  if(var.nbr_dim == 0){
    var.sz=1L;
    var.val.vp=nco_malloc(nco_typ_lng(var.type));
    (void)nco_get_var1(in_id,var.id ,0L,var.val.vp,var.type);
  } /* end if */

  dmn_id=(int *)nco_malloc(var.nbr_dim*sizeof(int));
  lmt_mult=(lmt_all_sct **)nco_malloc(var.nbr_dim*sizeof(lmt_all_sct *));
  lmt=(lmt_sct **)nco_malloc(var.nbr_dim*sizeof(lmt_sct *));
  
  /* Get dimension IDs from input file */
  (void)nco_inq_vardimid(in_id,var.id,dmn_id);
  
  /* Initialize lmt_mult with multi-limits from lmt_lst limits */
  /* Get dimension sizes from input file */
  for(idx=0;idx< var.nbr_dim;idx++)
    for(jdx=0;jdx< lmt_nbr;jdx++){
      if(dmn_id[idx] == lmt_lst[jdx].lmt_dmn[0]->id){
	lmt_mult[idx]=&lmt_lst[jdx];
        break;
      } /* end if */
    } /* end loop over jdx */

  /* Call super-dooper recursive routine */
  var.val.vp=nco_msa_rec_clc(0,var.nbr_dim,lmt,lmt_mult,&var);
  /* Call also initializes var.sz with final size */

  /* User supplied dlm_sng, print var (includes nbr_dmim == 0) */  
  if(dlm_sng != NULL){
    /* Print each element with user-supplied formatting code */
    /* Replace C language '\X' escape codes with ASCII bytes */
    (void)sng_ascii_trn(dlm_sng);

    /* Assume -s argument (dlm_sng) formats entire string
       Otherwise, one could assume that field will be printed with format nco_typ_fmt_sng(var.type),
       and that user is only allowed to affect text between fields. 
       This would be accomplished with:
       (void)sprintf(var_sng,"%s%s",nco_typ_fmt_sng(var.type),dlm_sng);*/

    for(lmn=0;lmn<var.sz;lmn++){
      switch(var.type){
      case NC_FLOAT: (void)fprintf(stdout,dlm_sng,var.val.fp[lmn]); break;
      case NC_DOUBLE: (void)fprintf(stdout,dlm_sng,var.val.dp[lmn]); break;
      case NC_SHORT: (void)fprintf(stdout,dlm_sng,var.val.sp[lmn]); break;
      case NC_INT: (void)fprintf(stdout,dlm_sng,var.val.lp[lmn]); break;
      case NC_CHAR: (void)fprintf(stdout,dlm_sng,var.val.cp[lmn]); break;
      case NC_BYTE: (void)fprintf(stdout,dlm_sng,var.val.bp[lmn]); break;
      default: nco_dfl_case_nc_type_err(); break;
      } /* end switch */
    } /* end loop over element */

  } /* end if dlm_sng */

  if(PRINT_DIMENSIONAL_UNITS){
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
      } /* end if */
    } /* end if */
  } /* end if PRINT_DIMENSIONAL_UNITS */

  /* if nbr_dim ==0 and dlm_sng==NULL print variable */
  if(var.nbr_dim == 0 && dlm_sng == NULL){
    /* Variable is scalar, byte, or character */
    lmn=0;
    (void)sprintf(var_sng,"%%s = %s %%s\n",nco_typ_fmt_sng(var.type));
    switch(var.type){
    case NC_FLOAT: (void)fprintf(stdout,var_sng,var_nm,var.val.fp[lmn],unit_sng); break;
    case NC_DOUBLE: (void)fprintf(stdout,var_sng,var_nm,var.val.dp[lmn],unit_sng); break;
    case NC_SHORT: (void)fprintf(stdout,var_sng,var_nm,var.val.sp[lmn],unit_sng); break;
    case NC_INT: (void)fprintf(stdout,var_sng,var_nm,var.val.lp[lmn],unit_sng); break;
    case NC_CHAR:
      if(var.val.cp[lmn] != '\0'){
        (void)sprintf(var_sng,"%%s='%s' %%s\n",nco_typ_fmt_sng(var.type));
        (void)fprintf(stdout,var_sng,var_nm,var.val.cp[lmn],unit_sng);
      }else{ /* Deal with NUL character here */
        (void)fprintf(stdout, "%s=\"\" %s\n",var_nm,unit_sng);
      } /* end if */
      break;
    case NC_BYTE: (void)fprintf(stdout,var_sng,var_nm,(unsigned char)var.val.bp[lmn],unit_sng); break;
    default: nco_dfl_case_nc_type_err(); break;
    } /* end switch */
  } /* end if variable is a scalar, byte, or character */

  if(var.nbr_dim > 0 && dlm_sng == NULL){
    long *mod_map_in;
    long *mod_map_out;
    long *dmn_sbs_ram; /* Indices in hyperslab */
    long *dmn_sbs_dsk; /* Indices of hyperslab relative to original on disk */  
    long var_dsk;

    mod_map_in=(long *)nco_malloc(var.nbr_dim*sizeof(long));
    mod_map_out=(long *)nco_malloc(var.nbr_dim*sizeof(long));
    dmn_sbs_ram=(long *)nco_malloc(var.nbr_dim*sizeof(long));
    dmn_sbs_dsk=(long *)nco_malloc(var.nbr_dim*sizeof(long));

    /* Create mod_map_in */
    for(idx=0;idx<var.nbr_dim;idx++) mod_map_in[idx]=1L;
    for(idx=0;idx<var.nbr_dim;idx++)
      for(jdx=idx+1;jdx<var.nbr_dim;jdx++)
	mod_map_in[idx]*=lmt_mult[jdx]->dmn_sz_org;
       
    /* Create mod_map_out */
    for(idx=0;idx< var.nbr_dim;idx++) mod_map_out[idx]=1L;
    for(idx=0;idx< var.nbr_dim;idx++) 
      for(jdx=idx;jdx<var.nbr_dim;jdx++)
	mod_map_out[idx]*=lmt_mult[jdx]->dmn_cnt;
      
    /* Read in co-ord dims if required */
    if(PRN_DMN_IDX_CRD_VAL){
      var_sct var_crd;

      dim=(dmn_sct *)nco_malloc(var.nbr_dim*sizeof(dmn_sct));
      for(idx=0;idx <var.nbr_dim;idx++){
	dim[idx].val.vp=NULL;
        dim[idx].nm=lmt_mult[idx]->dmn_nm;
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
        /* Read in coordinate var with limits applied */
	dim[idx].val.vp=nco_msa_rec_clc(0,1,lmt,lmt_mult+idx,&var_crd);

        /* typecast pointer before use */  
        (void)cast_void_nctype(dim[idx].type,&dim[idx].val);
      }/* end for */
    } /* end if */
   
    for(lmn=0;lmn<var.sz;lmn++){

      /* Caculate RAM indices from current limit */
      for(idx=0;idx <var.nbr_dim;idx++) 
	dmn_sbs_ram[idx]=(lmn%mod_map_out[idx])/(idx == var.nbr_dim-1 ? 1L : mod_map_out[idx+1]);
      /* Calculate disk indices from RAM indices */
      (void)nco_msa_ram_2_dsk(dmn_sbs_ram,lmt_mult,var.nbr_dim,dmn_sbs_dsk,(lmn==var.sz-1));
     
      /* Find variable index relative to disk */
      var_dsk=0;
      for(idx=0;idx <var.nbr_dim;idx++)	var_dsk+=dmn_sbs_dsk[idx]*mod_map_in[idx];

      /* Skip rest of loop unless element is first in string */
      if(var.type == NC_CHAR && dmn_sbs_ram[var.nbr_dim-1] > 0) goto lbl_char_prn;
             
      /* Print dimensions with indices along with values if they are coordinate variables */
      if(PRN_DMN_IDX_CRD_VAL){
        int dmn_idx;
        long dmn_sbs_prn;
	long crd_idx_crr;
	char dmn_sng[MAX_LEN_FMT_SNG];
      
	/* Loop over dimensions whose coordinates are to be printed */
	for(idx=0;idx<var.nbr_dim;idx++){
	  
	  /* Reverse dimension ordering for Fortran convention */
	  if(FORTRAN_IDX_CNV) dmn_idx=var.nbr_dim-1-idx; else dmn_idx=idx;
	  
	  /* Format and print dimension part of output string for non-coordinate variables */
	  if(dim[dmn_idx].cid == var.id) continue; /* If variable is a coordinate then skip printing until later */
	  if(!dim[dmn_idx].is_crd_dmn){ /* If dimension is not a coordinate... */
	    if(FORTRAN_IDX_CNV) (void)fprintf(stdout,"%s(%ld) ",dim[dmn_idx].nm,dmn_sbs_dsk[dmn_idx]+1L); else (void)fprintf(stdout,"%s[%ld] ",dim[dmn_idx].nm,dmn_sbs_dsk[dmn_idx]);
	    continue;
	  } /* end if */
               
	  (void)sprintf(dmn_sng,"%%s[%%ld]=%s ",nco_typ_fmt_sng(dim[dmn_idx].type));
          dmn_sbs_prn=dmn_sbs_dsk[dmn_idx];

	  if(FORTRAN_IDX_CNV){
            (void)nco_msa_c_2_f(dmn_sng);
	    dmn_sbs_prn++;
	  } /* end if */
         
	  /* Account for hyperslab offset in coordinate values*/
	  crd_idx_crr=dmn_sbs_ram[dmn_idx];
	  switch(dim[dmn_idx].type){
	  case NC_FLOAT: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,dim[dmn_idx].val.fp[crd_idx_crr]); break;
	  case NC_DOUBLE: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,dim[dmn_idx].val.dp[crd_idx_crr]); break;
	  case NC_SHORT: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,dim[dmn_idx].val.sp[crd_idx_crr]); break;
	  case NC_INT: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,dim[dmn_idx].val.lp[crd_idx_crr]); break;
	  case NC_CHAR: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,dim[dmn_idx].val.cp[crd_idx_crr]); break;
	  case NC_BYTE: (void)fprintf(stdout,dmn_sng,dim[dmn_idx].nm,dmn_sbs_prn,(unsigned char)dim[dmn_idx].val.bp[crd_idx_crr]); break;
	      default: nco_dfl_case_nc_type_err(); break;
	      } /* end switch */
	} /* end loop over dimensions */
      } /* end if PRN_DMN_IDX_CRD_VAL */
      
      /* Print all characters in last dimension each time penultimate dimension subscript changes to its start value */
    lbl_char_prn:

      if(var.type == NC_CHAR){
        static bool NULL_IN_SLAB;
	static char *prn_sng;
        static int chr_cnt;
        static long dmn_sz;
        static long var_dsk_srt;
        static long var_dsk_end;

        /* At beginning of Character array */
	if(dmn_sbs_ram[var.nbr_dim-1] == 0L) {
          dmn_sz=lmt_mult[var.nbr_dim-1]->dmn_cnt;
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
      (void)sprintf(var_sng,"%%s[%%ld]=%s %%s\n",nco_typ_fmt_sng(var.type));
      if(FORTRAN_IDX_CNV){
	(void)nco_msa_c_2_f(var_sng);
	var_dsk++;
      } /* end if FORTRAN_IDX_CNV */

      switch(var.type){
      case NC_FLOAT: (void)fprintf(stdout,var_sng,var_nm,var_dsk,var.val.fp[lmn],unit_sng); break;
      case NC_DOUBLE: (void)fprintf(stdout,var_sng,var_nm,var_dsk,var.val.dp[lmn],unit_sng); break;
      case NC_SHORT: (void)fprintf(stdout,var_sng,var_nm,var_dsk,var.val.sp[lmn],unit_sng); break;
      case NC_INT: (void)fprintf(stdout,var_sng,var_nm,var_dsk,var.val.lp[lmn],unit_sng); break;
      case NC_CHAR: (void)fprintf(stdout,var_sng,var_nm,var_dsk,var.val.cp[lmn],unit_sng); break;
      case NC_BYTE: (void)fprintf(stdout,var_sng,var_nm,var_dsk,(unsigned char)var.val.bp[lmn],unit_sng); break;
      default: nco_dfl_case_nc_type_err(); break;
      } /* end switch */
    } /* end loop over elements */

    (void)nco_free(mod_map_in);
    (void)nco_free(mod_map_out);
    (void)nco_free(dmn_sbs_ram);
    (void)nco_free(dmn_sbs_dsk);

    /* Additional newline between consecutive variables or final variable and prompt */
    (void)fprintf(stdout,"\n");
    (void)fflush(stdout);
  } /* end if variable has more than one dimension */
  var.val.vp=nco_free(var.val.vp);
  var.nm=(char *)nco_free(var.nm);
 
  if(var.nbr_dim > 0){
    (void)nco_free(dmn_id);
    (void)nco_free(lmt_mult);
    (void)nco_free(lmt);
  } /* end if */
  if(PRN_DMN_IDX_CRD_VAL && dlm_sng==NULL){
    for(idx=0;idx<var.nbr_dim;idx++)
      dim[idx].val.vp=nco_free(dim[idx].val.vp);

    dim=(dmn_sct *)nco_free(dim);
  } /* end if */

} /* end nco_msa_prn_var_val() */
