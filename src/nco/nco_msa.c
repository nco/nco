/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_msa.c,v 1.8 2003-02-20 16:47:38 hmb Exp $ */

/* Purpose: Multi-slabbing algorithm */

/* Copyright (C) 1995--2003 Charlie Zender and Henry Butowsky
   This software is distributed under the terms of the GNU General Public License
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

#include "nco_msa.h" /* Multi-slabbing algorithm */

/* fxm: strings statically allocated with MAX_LEN_FMT_SNG chars are susceptible to buffer overflow attacks */
/* Length should be computed at run time but is a pain */
#define MAX_LEN_FMT_SNG 100


void *
nco_msa_rec_clc /* [fnc] Multi slab algorithm (recursive routine, returns a single slab pointer */
(int dpt_crr, /* current depth, we start at 0 */
 int dpt_crr_max, /* maximium depth (i.e the number of dims in variable (does not change)*/	      
 lmt_sct **lmt, /* limits of the current hyperslabs these change as we recurse */
 lmt_all **lmt_lst, /* list of limits in each dimension (this remains STATIC as we recurse) */
 var_sct *vara) /* Info for routine to read var info and pass info between calls */
{  
  /* Purpose: Multi slab algorithm (recursive routine, returns a single slab pointer */
  
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
    
    for(idx=0;idx < nbr_slb;idx++){
      lmt[dpt_crr]=lmt_lst[dpt_crr]->lmt_dmn[idx];
      cp_wrp[idx]=(char *)nco_msa_rec_clc(dpt_crr+1,dpt_crr_max,lmt,lmt_lst,vara);
      cp_sz[idx]=vara->sz;
    } /* end loop over idx */
    
    for(idx=0;idx < dpt_crr_max;idx++) var_sz*=(idx < dpt_crr ? lmt[idx]->cnt : lmt_lst[idx]->dmn_cnt);
    
    vp=(void *)nco_calloc(var_sz,nco_typ_lng(vara->type));
    
    lcnt=nco_typ_lng(vara->type);
    for(idx=(dpt_crr+1);idx < dpt_crr_max;idx++) lcnt*=lmt_lst[idx]->dmn_cnt;
    
    cp_inc=(ptrdiff_t)(lcnt*lmt_lst[dpt_crr]->dmn_cnt);
    
    cp_max=(ptrdiff_t)(var_sz*nco_typ_lng(vara->type));
    
    for(idx=0;idx < nbr_slb;idx++) indices[idx]=lmt_lst[dpt_crr]->lmt_dmn[idx]->srt;
    
    cp_fst=0L;

    /* deal first with wrapped dims. With true wrapped dims nbr_slb =2 */
    /* They are also "continuous" , and there is no overlapping        */  
  if( lmt_lst[dpt_crr]->WRP) {
      
      for(slb_idx=0 ; slb_idx < 2 ; slb_idx++){
	cp_stp = (char*)vp+cp_fst;
	slb=cp_wrp[slb_idx];
	slb_sz=(ptrdiff_t)(lcnt*(lmt_lst[dpt_crr]->lmt_dmn[slb_idx]->cnt));
      
        while(cp_stp - (char*)vp < cp_max){
	  (void)memcpy(cp_stp,slb,slb_sz);
	  slb+=slb_sz;
	  cp_stp+=cp_inc;
        } /* end while */
        cp_fst+=slb_sz;        
      }

    }else{
      /* deal with multiple hyper-slabs */
      while(nco_msa_clc_idx(True,lmt_lst[dpt_crr],&indices[0],&lmt_ret,&slb_idx)){
	cp_stp = (char*)vp+cp_fst;
	slb=cp_wrp[slb_idx]+(ptrdiff_t)(lmt_ret.srt*lcnt);
	slb_stp=(ptrdiff_t)(lcnt*(lmt_lst[dpt_crr]->lmt_dmn[slb_idx]->cnt));
      
	slb_sz=(ptrdiff_t)(lmt_ret.cnt*lcnt);
      
	while(cp_stp - (char*)vp < cp_max){
	  (void)memcpy(cp_stp,slb,slb_sz);
	  slb+=slb_stp;
	  cp_stp+=cp_inc;
	} /* end while */
	cp_fst+=slb_sz;
      } /* end while */
    } /* end else */  

 
    (void)nco_free(indices);
    (void)nco_free(cp_sz);
    for(idx=0;idx < nbr_slb;idx++) (void)nco_free(cp_wrp[idx]);
    
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
    
    dmn_srt=(long*)nco_malloc(dpt_crr_max*sizeof(long));
    dmn_cnt=(long*)nco_malloc(dpt_crr_max*sizeof(long));
    dmn_srd=(long*)nco_malloc(dpt_crr_max*sizeof(long));
    
    for(idx=0;idx < dpt_crr_max;idx++){
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
nco_msa_prn_idx(lmt_all *lmt_i)
{
  int slb_nbr;
  int idx;
  int size=lmt_i->lmt_dmn_nbr;
  long *indices;
  lmt_sct lmt;
  
  indices=(long *)nco_malloc(size *sizeof(long));
  
  printf("name=%s total size=%ld\n",lmt_i->dmn_nm,lmt_i->dmn_cnt);
  
  for(idx=0;idx < size;idx++) indices[idx]=lmt_i->lmt_dmn[idx]->srt;
  
  while(nco_msa_clc_idx(False,lmt_i,&indices[0],&lmt,&slb_nbr))
    printf("slb_nbr=%d srt=%ld end=%ld cnt=%ld srd=%ld\n",slb_nbr,lmt.srt,lmt.end,lmt.cnt,lmt.srd);
} /* end nco_msa_prn_idx() */

bool /* if false then there are no more limits */
nco_msa_clc_idx
(bool NORMALIZE,
 lmt_all *lmt_a, /* I list of lmts for each dimension  */
 long *indices, /* I/O so routine can keep track of where its at */
 lmt_sct *lmt, /* O  output hyperslab */
 int *slb) /* slab which the above limit refers to */ 
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
    
    for(i=0;i < size;i++){
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
nco_msa_ram_2_dsk(   /* convert hyperslab indices (in ram) to hyperlsab indices relative */
		  long *dmn_sbs_ram,   /* to disk. */
lmt_all **lmt_mult, 
int nbr_dim,
long *dmn_sbs_dsk,
bool FREE )
{                /*  It doenst really convert ram indices to disk indices , but given a set 
                     of ram indices finds the next set of dsk incdices. So it only works if 
                     the indices fed to it are continuous */
  int idx;
  int jdx;
  int size;
  static int initialize;
  static long **dmn_indices;
  static long *dmn_sbs_prv;
  static bool min[100]; 

  if(!initialize){
    dmn_sbs_prv = (long*)nco_malloc(nbr_dim*sizeof(long));
    dmn_indices = (long**)nco_malloc(nbr_dim*sizeof(long*));
    for(idx= 0 ; idx < nbr_dim ; idx++){
      dmn_indices[idx] = (long *)nco_malloc(lmt_mult[idx]->lmt_dmn_nbr*sizeof(long));
      for(jdx = 0 ; jdx < lmt_mult[idx]->lmt_dmn_nbr ; jdx++)
	dmn_indices[idx][jdx] = lmt_mult[idx]->lmt_dmn[jdx]->srt; 
      dmn_sbs_prv[idx] = -1L;
    } 
    initialize = 1;
  }
  for(idx= 0 ; idx <nbr_dim ; idx ++){
    size = lmt_mult[idx]->lmt_dmn_nbr;
    if(dmn_sbs_ram[idx] == dmn_sbs_prv[idx] ) continue;

    if(lmt_mult[idx]->BASIC_DMN) { 
      dmn_sbs_dsk[idx]=dmn_sbs_ram[idx]; 
      continue; 
    }

     
    /* re-initialize indices if 0*/
    if(dmn_sbs_ram[idx] == 0) 
      for(jdx=0 ;jdx < size ; jdx++)
       	dmn_indices[idx][jdx] = lmt_mult[idx]->lmt_dmn[jdx]->srt; 
   
    /* Deal with wrapping - we have 2 hyperlsbas to deal with */
     if(lmt_mult[idx]->WRP){ 
       if(dmn_indices[idx][0] < lmt_mult[idx]->dmn_sz_org){
	 dmn_sbs_dsk[idx]= dmn_indices[idx][0];
         dmn_indices[idx][0]+=lmt_mult[idx]->lmt_dmn[0]->srd;
        }else{
	 dmn_sbs_dsk[idx]= dmn_indices[idx][1];
         dmn_indices[idx][1]+=lmt_mult[idx]->lmt_dmn[1]->srd;
       }
     continue;  
     }
         
    dmn_sbs_dsk[idx] = nco_msa_min_idx(dmn_indices[idx],min,size);

    for(jdx=0;jdx < size;jdx++){
      if(min[jdx]){ 
	dmn_indices[idx][jdx]+=lmt_mult[idx]->lmt_dmn[jdx]->srd;
	if(dmn_indices[idx][jdx] > lmt_mult[idx]->lmt_dmn[jdx]->end) dmn_indices[idx][jdx]=-1;
      }
    } /* end for  jdx */
  } /* end for idx */ 

  for(idx=0 ;idx < nbr_dim ; idx++ ) dmn_sbs_prv[idx] = dmn_sbs_ram[idx];

  /* Free static space on last call */
  if(FREE){
    (void)nco_free(dmn_sbs_prv);
    for(idx=0 ; idx <nbr_dim ; idx++)
      (void)nco_free(dmn_indices[idx]);
    (void)nco_free(dmn_indices);
    initialize =0;
  }

}
    

void 
nco_msa_clc_cnt(lmt_all *lmt_a)
{
  int idx;
  long cnt=0;
  int  size=lmt_a->lmt_dmn_nbr;
  long *indices;
  bool *min;
  
  indices=(long *)nco_malloc(size*sizeof(long));
  min=(bool *)nco_malloc(size*sizeof(bool));
  
  if(size == 1){
    lmt_a->dmn_cnt=lmt_a->lmt_dmn[0]->cnt;
    return;
  } /* end if */
  /* initialise indices with srt from    */
  for(idx=0;idx < size;idx++)
    indices[idx]=lmt_a->lmt_dmn[idx]->srt;
  
  while(nco_msa_min_idx(indices,min,size) != LONG_MAX){
    for(idx=0;idx < size;idx++){
      if(min[idx]){ 
	indices[idx]+=lmt_a->lmt_dmn[idx]->srd;
	if(indices[idx] > lmt_a->lmt_dmn[idx]->end) indices[idx]=-1;
      }
    } /* end loop over idx */
    cnt++;
  } /* end while */
  lmt_a->dmn_cnt=cnt;
} /* end nco_msa_clc_cnt() */

void
nco_msa_wrp_splt
(lmt_all* lmt_a )  /* Split wrapped dimensions  */
{
  int idx;
  int jdx;
  int size = lmt_a->lmt_dmn_nbr;
  long dmn_sz_org = lmt_a->dmn_sz_org;
  long srt;
  long end; 
  long cnt;
  long srd;
  long index;
  lmt_sct *lmt_wrp;

  for(idx =0 ; idx<size ; idx++) {

    if( lmt_a->lmt_dmn[idx]->srt > lmt_a->lmt_dmn[idx]->end) {

      lmt_wrp = (lmt_sct *)nco_malloc(2*sizeof(lmt_sct)); 

      srt= lmt_a->lmt_dmn[idx]->srt;
      end= lmt_a->lmt_dmn[idx]->end;
      cnt= lmt_a->lmt_dmn[idx]->cnt;
      srd= lmt_a->lmt_dmn[idx]->srd;
    
     
    for(jdx=0;jdx < cnt ; jdx++){
      index=(srt+srd*jdx)%dmn_sz_org;
      if(index < srt) break;
    } /* end loop over jdx */
    
    lmt_wrp[0] = *(lmt_a->lmt_dmn[idx]);
    lmt_wrp[1] = *(lmt_a->lmt_dmn[idx]);

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
    
    lmt_wrp[1].srt=index;
    
    lmt_wrp[1].cnt=cnt-lmt_wrp[0].cnt;
    if(lmt_wrp[1].cnt == 1L){
      lmt_wrp[1].end=index;
      lmt_wrp[1].srd=1L;
    }else{
      lmt_wrp[1].end=index+(lmt_wrp[1].cnt-1)*srd;
      lmt_wrp[1].srd=srd;
    } /* end else */

    /* insert the  new limits into the array */
    lmt_a->lmt_dmn[idx] = lmt_wrp;

    lmt_a->lmt_dmn  = (lmt_sct **) nco_realloc(lmt_a->lmt_dmn,((lmt_a->lmt_dmn_nbr) +1)*sizeof(lmt_sct *));
       lmt_a->lmt_dmn[(lmt_a->lmt_dmn_nbr)++]=++lmt_wrp;

    }

  }

  /* Check if we have a genuine wrapped co-ordinate */
  if(size==1 && lmt_a->lmt_dmn_nbr==2) lmt_a->WRP=True;

}

long
nco_msa_min_idx /* find min values in current */
(long *current, /* current indices */
 bool *min, /* element true if a minimum */
 int size) /* size of current and min */
{
  int i;
  long min_val=LONG_MAX;
  
  for(i=0;i< size;i++)
    if(current[i] != -1 && current[i] < min_val) min_val=current[i];
  
  for(i=0;i < size;i++)
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
 lmt_all * const lmt_lst, /* I multi-hyperslab limits */
 int nbr_dmn_fl) /* I [nbr] Number of multi-hyperslab limits */
{
  /* Purpose: Copy variable data from input netCDF file to output netCDF file 
     Routine truncates dimensions in variable definition in output file according to user-specified limits.
     Routine copies_variable by variable, old-style, used only by ncks */
  
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
  
  lmt_all **lmt_mult;
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
  
  /* deal with scalar vars */
  if(nbr_dim == 0){
    var_sz=1L;
    void_ptr=nco_malloc(nco_typ_lng(var_type));
    (void)nco_get_var1(in_id,var_in_id,0L,void_ptr,var_type);
    (void)nco_put_var1(out_id,var_out_id,0L,void_ptr,var_type);
    if(NCO_BNR_WRT) nco_bnr_wrt(fp_bnr,"",var_nm,var_sz,var_type,void_ptr);
    (void)nco_free(void_ptr);
    return;
  } /* end if */
  
  dmn_map_in=(long *)nco_malloc(nbr_dim*sizeof(long));
  dmn_map_out=(long *)nco_malloc(nbr_dim*sizeof(long));
  dmn_map_srt=(long *)nco_malloc(nbr_dim*sizeof(long));
  dmn_id=(int *)nco_malloc(nbr_dim*sizeof(int));
  
  lmt_mult=(lmt_all **)nco_malloc(nbr_dim*sizeof(lmt_all *));
  lmt=(lmt_sct **)nco_malloc(nbr_dim*sizeof(lmt_sct *));
  
  /* Get dimension IDs from input file */
  (void)nco_inq_vardimid(in_id,var_in_id,dmn_id);
  
  /* iniialize lmt_mult with multi-limits frpm lmt_lst  limits */
  /* Get dimension sizes from input file */
  for(idx=0;idx< nbr_dim;idx++){
    
    for(jdx=0;jdx <nbr_dmn_fl;jdx++){
      if(dmn_id[idx] == lmt_lst[jdx].lmt_dmn[0]->id){
	lmt_mult[idx]=&lmt_lst[jdx];
        break;
      }
    } /* end loop over jdx */
    /* create maps now - they maybe useful later */ 
    (void)nco_inq_dimlen(in_id,dmn_id[idx],&dmn_map_in[idx]);
    dmn_map_out[idx]=lmt_mult[idx]->dmn_cnt;
    dmn_map_srt[idx]=0L;
  } /* end for */
  
  /* initalize vara with in_id, var_in_id, nctype etc so recursive routine can read data*/
  vara.nm=var_nm;
  vara.id=var_in_id;
  vara.nc_id=in_id;
  vara.type=var_type;
  
  /* Call the super dooper recursive routine */
  void_ptr=nco_msa_rec_clc(0,nbr_dim,lmt,lmt_mult,&vara);
  var_sz=vara.sz;
  
  (void)nco_put_vara(out_id,var_out_id,dmn_map_srt,dmn_map_out,void_ptr,var_type);
  
  if(NCO_BNR_WRT) nco_bnr_wrt(fp_bnr,"",var_nm,var_sz,var_type,void_ptr);
  
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
nco_msa_c_2_f(char *s)
{
  while (*s) {
    if (*s == '(' ) *s = '[' ;
    if( *s == ')' ) *s = ']' ;
    s++;
  } 


}


void
nco_msa_prn_var_val   /* [fnc] Print variable data */
(const int in_id, /* I [id] netCDF input file ID */
 const char * const var_nm, /* I [sng] Variable name */
 const lmt_all * const lmt_lst, /* I [sct] Dimension limits */
 const int lmt_nbr, /* I [nbr] number of dimensions with user-specified limits */
 char * const dlm_sng, /* I [sng] User-specified delimiter string, if any */
 const bool FORTRAN_STYLE, /* I [flg] Hyperslab indices obey Fortran convention */
 const bool PRINT_DIMENSIONAL_UNITS, /* I [flg] Print units attribute, if any */
 const bool PRN_DMN_IDX_CRD_VAL) /* I [flg] Print dimension/coordinate indices/values */
{

  /* Summary of Function */
 
  /* Get variable with limits from input file */
  /* User supplied dlm_sng, print var (includes nbr_dmim == 0)
  /* Get dimensional units      */
  /* if nbr_dim ==0 and dlm_sng==NULL  print variable */
  /* if PRN.. = False print var taking account of FORTRAN (need var indices) */
  /* if PRN_DMN_IDX_CRD_VAL then read in co-ord dims */ 
  /* if PRN.. = True print var taking account of FORTRAN  (Use dims to calculate var indices */
  
/**************************************************************************/

/* Get variable with limits from input file */
  
  int rcd;
  int idx;
  int jdx;
  int *dmn_id=NULL_CEWI;
  
  /* For regular data */
  long lmn;
  
  char *unit_sng="";  
  char var_sng[MAX_LEN_FMT_SNG];

  
  var_sct var;

  lmt_all **lmt_mult=NULL_CEWI;
  lmt_sct **lmt=NULL_CEWI;
  
  dmn_sct *dim=NULL_CEWI;
     
  /* Get var_id for requested variable  */
  var.nm = (char *)strdup(var_nm);
  var.nc_id=in_id;
   
  nco_inq_varid(in_id,var_nm,&var.id);
  
  /* Get type and number of dimensions for variable */

  (void)nco_inq_var(in_id,var.id,(char *)NULL,&var.type,&var.nbr_dim,(int *)NULL,(int *)NULL);

  
  /* deal with scalar vars */
  if(var.nbr_dim == 0){
    var.sz=1L;
    var.val.vp=nco_malloc(nco_typ_lng(var.type));
    (void)nco_get_var1(in_id,var.id ,0L,var.val.vp,var.type);

  } /* end if */
 
 

  dmn_id=(int *)nco_malloc(var.nbr_dim*sizeof(int));
  
  lmt_mult=(lmt_all **)nco_malloc(var.nbr_dim*sizeof(lmt_all *));
  lmt=(lmt_sct **)nco_malloc(var.nbr_dim*sizeof(lmt_sct *));
  
  /* Get dimension IDs from input file */
  (void)nco_inq_vardimid(in_id,var.id,dmn_id);
  
  /* iniialize lmt_mult with multi-limits frpm lmt_lst  limits */
  /* Get dimension sizes from input file */
  for(idx=0;idx< var.nbr_dim;idx++)
    for(jdx=0; jdx< lmt_nbr ; jdx++){
      if(dmn_id[idx] == lmt_lst[jdx].lmt_dmn[0]->id){
	lmt_mult[idx]=&lmt_lst[jdx];
        break;
      }
    } /* end loop over jdx */

  
  
  /* Call the super dooper recursive routine */
  var.val.vp=nco_msa_rec_clc(0,var.nbr_dim,lmt,lmt_mult,&var);
/* call also initializes var.sz with the final size */


/*************************************************************************/

/* User supplied dlm_sng, print var (includes nbr_dmim == 0) */  
  
  if(dlm_sng != NULL){
    /* Print each element with user-supplied formatting code */
    PRN_DMN_IDX_CRD_VAL =False;
    /* Replace C language '\X' escape codes with ASCII bytes */
    (void)sng_ascii_trn(dlm_sng);

    /* Assume -s argument (dlm_sng) formats entire string
       Otherwise, one could assume that field will be printed with format nco_typ_fmt_sng(var.type),
       and that user is only allowed to affect text between fields. 
       This would be accomplished with:
       (void)sprintf(var_sng,"%s%s",nco_typ_fmt_sng(var.type),dlm_sng); */

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

/*************************************************************************/

  if(PRINT_DIMENSIONAL_UNITS){
    char units_nm[]="units"; /* [sng] Name of units attribute */
    int rcd; /* [rcd] Return code */
    int att_id; /* [id] Attribute ID */
    long att_sz;
    nc_type att_typ;

    /* Does variable have character attribute named units_nm? */
    rcd=nco_inq_attid_flg(in_id,var.id,units_nm,&att_id);
    if(rcd == NC_NOERR){
      (void)nco_inq_att(in_id,var.id,units_nm,&att_typ,&att_sz);
      if(att_typ == NC_CHAR){
	unit_sng=(char *)nco_malloc((att_sz+1)*nco_typ_lng(att_typ));
	(void)nco_get_att(in_id,var.id,units_nm,unit_sng,att_typ);
	unit_sng[(att_sz+1)*nco_typ_lng(att_typ)-1]='\0';
      } /* end if */
    } /* end if */
  } /* end if PRINT_DIMENSIONAL_UNITS */


/*************************************************************************/
/* if nbr_dim ==0 and dlm_sng==NULL  print variable */


  if(var.nbr_dim == 0 && dlm_sng == NULL){ 
    /* Variable is a scalar, byte, or character */
    lmn=0;
    (void)sprintf(var_sng,"%%s = %s %%s\n",nco_typ_fmt_sng(var.type));
    switch(var.type){
    case NC_FLOAT: (void)fprintf(stdout,var_sng,var_nm,var.val.fp[lmn],unit_sng); break;
    case NC_DOUBLE: (void)fprintf(stdout,var_sng,var_nm,var.val.dp[lmn],unit_sng); break;
    case NC_SHORT: (void)fprintf(stdout,var_sng,var_nm,var.val.sp[lmn],unit_sng); break;
    case NC_INT: (void)fprintf(stdout,var_sng,var_nm,var.val.lp[lmn],unit_sng); break;
    case NC_CHAR:
      (void)sprintf(var_sng,"%%s='%s' %%s\n",nco_typ_fmt_sng(var.type));
      (void)fprintf(stdout,var_sng,var_nm,var.val.cp[lmn],unit_sng);
      break;
    case NC_BYTE: (void)fprintf(stdout,var_sng,var_nm,(unsigned char)var.val.bp[lmn],unit_sng); break;
    default: nco_dfl_case_nc_type_err(); break;
    } /* end switch */
  } /* end if variable is a scalar, byte, or character */

/*************************************************************************/

  if( var.nbr_dim > 0 && dlm_sng == NULL ) {

    long *mod_map_in;
    long *mod_map_out;
    long *dmn_sbs_ram;              /* indices in the hyperslab */
    long *dmn_sbs_dsk;           /* indices of the hyperslab relative to original on disk */  
    long var_dsk;


    mod_map_in =(long*)nco_malloc(var.nbr_dim * sizeof(long));
    mod_map_out=(long*)nco_malloc(var.nbr_dim * sizeof(long));
    dmn_sbs_ram=(long*)nco_malloc(var.nbr_dim * sizeof(long));
    dmn_sbs_dsk=(long*)nco_malloc(var.nbr_dim * sizeof(long));
   

    /* Create mod_map_in */
    for(idx=0 ; idx< var.nbr_dim ; idx++) mod_map_in[idx]=1L;
    for(idx=0 ; idx< var.nbr_dim ; idx++)
      for(jdx=idx+1 ; jdx < var.nbr_dim ; jdx++)
	mod_map_in[idx] *= lmt_mult[jdx]->dmn_sz_org;
       
       
    /* Create mod_map_out */
    for(idx=0 ; idx< var.nbr_dim ; idx++) mod_map_out[idx]=1L;
    for(idx=0 ; idx< var.nbr_dim ; idx++) 
      for(jdx=idx ; jdx < var.nbr_dim ; jdx++)
	mod_map_out[idx] *= lmt_mult[jdx]->dmn_cnt;
      

    /* Read in co-ord dims if required */
    if(PRN_DMN_IDX_CRD_VAL) { 
      var_sct var_crd;

      dim=(dmn_sct *)nco_malloc(var.nbr_dim*sizeof(dmn_sct));
      for(idx=0 ; idx <var.nbr_dim ; idx++){
	dim[idx].val.vp=NULL;
        dim[idx].nm = lmt_mult[idx]->dmn_nm; 
        rcd=nco_inq_varid_flg(in_id,dim[idx].nm,&dim[idx].cid);
          /* if not a variable */
        if(rcd != NC_NOERR){
          dim[idx].is_crd_dmn=False;
          dim[idx].cid=-1;
          continue;
        }
	  
        dim[idx].is_crd_dmn=True;
        (void)nco_inq_vartype(in_id,dim[idx].cid, &dim[idx].type);
        var_crd.nc_id = in_id;
        var_crd.nm = dim[idx].nm;
        var_crd.type = dim[idx].type;
        var_crd.id = dim[idx].cid;
        /* Read in co-ord var with limits applied */
	dim[idx].val.vp=nco_msa_rec_clc(0,1,lmt,lmt_mult+idx ,&var_crd);     

        /* typecast pointer before use */  
        (void)cast_void_nctype(dim[idx].type,&dim[idx].val);
      }/* end for */
    } /* end if */
   

    for(lmn=0 ; lmn < var.sz ; lmn++) {

      /* Caculate ram indices from current limit */
      for(idx= 0 ;idx <var.nbr_dim; idx++) 
	dmn_sbs_ram[idx] = ( lmn % mod_map_out[idx] ) / ( idx == var.nbr_dim -1 ? 1L : mod_map_out[idx+1]);
	
      /* Calculate disk indices from ram indices */
      (void)nco_msa_ram_2_dsk(dmn_sbs_ram, lmt_mult,var.nbr_dim,dmn_sbs_dsk,(lmn==var.sz -1) );
     
      /* Find variable index relative to disk */
      var_dsk = 0;
      for(idx=0 ; idx <var.nbr_dim ; idx++)
	var_dsk+= dmn_sbs_dsk[idx]*mod_map_in[idx];
             

      /* print the dims with indices along with values if they are co-ordinate vars */
      if(PRN_DMN_IDX_CRD_VAL){
        int dmn_idx;
        long dmn_sbs_prn; 
	long crd_idx_crr;
	char dmn_sng[MAX_LEN_FMT_SNG];
      
	/* Loop over dimensions whose coordinates are to be printed */
	for(idx=0 ;idx<var.nbr_dim ;idx++){
	  
	  /* Reverse dimension ordering for Fortran convention */
	  if(FORTRAN_STYLE) dmn_idx=var.nbr_dim-1-idx; else dmn_idx=idx;
	  
	  /* Format and print dimension part of output string for non-coordinate variables */
	  if(dim[dmn_idx].cid == var.id) continue;  /* If variable is also  a coordinate...skip printing till later */
	  if(!dim[dmn_idx].is_crd_dmn){ /* If dimension is not a coordinate... */
 	     if(FORTRAN_STYLE)
	       (void)fprintf(stdout,"%s[%ld] ",dim[dmn_idx].nm,dmn_sbs_dsk[dmn_idx]+1L);
	     else
	       (void)fprintf(stdout,"%s(%ld) ",dim[dmn_idx].nm,dmn_sbs_dsk[dmn_idx]);
              
             continue;
	     }
               
	  (void)sprintf(dmn_sng,"%%s(%%ld)=%s ",nco_typ_fmt_sng(dim[dmn_idx].type));
          dmn_sbs_prn = dmn_sbs_dsk[dmn_idx];

	  if(FORTRAN_STYLE) { 
            (void)nco_msa_c_2_f(dmn_sng);
	     dmn_sbs_prn++;
	  }
         
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
      

      /* Now print actual variable name, index and value. */
      (void)sprintf(var_sng,"%%s(%%ld)=%s %%s\n",nco_typ_fmt_sng(var.type)); 

      if(FORTRAN_STYLE){ (void)nco_msa_c_2_f(var_sng); var_dsk++;}


      switch(var.type){
      case NC_FLOAT: (void)fprintf(stdout,var_sng,var_nm,var_dsk,var.val.fp[lmn],unit_sng); break;
      case NC_DOUBLE: (void)fprintf(stdout,var_sng,var_nm,var_dsk,var.val.dp[lmn],unit_sng); break;
      case NC_SHORT: (void)fprintf(stdout,var_sng,var_nm,var_dsk,var.val.sp[lmn],unit_sng); break;
      case NC_INT: (void)fprintf(stdout,var_sng,var_nm,var_dsk,var.val.lp[lmn],unit_sng); break;
      case NC_CHAR: (void)fprintf(stdout,var_sng,var_nm,var_dsk,var.val.cp[lmn],unit_sng); break;
      case NC_BYTE: (void)fprintf(stdout,var_sng,var_nm,var_dsk,(unsigned char)var.val.bp[lmn],unit_sng); break;
      default: nco_dfl_case_nc_type_err(); break;
      } /* end switch */


      
   
    } /*end loop over elemnts */

    (void)nco_free(mod_map_in); 
    (void)nco_free(mod_map_out);
    (void)nco_free(dmn_sbs_ram);
    (void)nco_free(dmn_sbs_dsk);

    /* Additional newline between consecutive variables or final variable and prompt */
    (void)fprintf(stdout,"\n");
    (void)fflush(stdout);
  }

  

  var.val.vp = nco_free(var.val.vp);
  var.nm = (char *)nco_free(var.nm);
 
  if(var.nbr_dim >0 ) {
    (void)nco_free(dmn_id);
    (void)nco_free(lmt_mult);
    (void)nco_free(lmt);
  }
  if(PRN_DMN_IDX_CRD_VAL) {
    for(idx=0 ; idx < var.nbr_dim ; idx++)
      dim[idx].val.vp =nco_free(dim[idx].val.vp);
      dim=(dmn_sct *)nco_free(dim);
    }

    

} /* nco_msa_prn_var_val */
  
