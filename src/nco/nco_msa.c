/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_msa.c,v 1.2 2002-12-22 17:45:27 hmb Exp $ */

/* Purpose: Multi-slabbing algorithm */

/* Copyright (C) 1995--2002 Charlie Zender and Henry Butowsky
   This software is distributed under the terms of the GNU General Public License
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

#include "nco_msa.h" /* Multi-slabbing algorithm */


void *
nco_msa_rec_calc(  /* Multi slab algorithm (recursive routine, returns a single slab pointer */
int cd,             /* current depth, we start at 0 */
int cd_max,          /* maximium depth (i.e the number of dims in variable (does not change)*/	      
lmt_sct **lmt,    /* limits of the current hyperslabs these change as we recurse */
lmt_all **lmt_lst, /* list of limits in each dimension (this remains STATIC as we recurse) */
var_sct *vara)     /* Info for routine to read var info and pass info between calls */
{  
  
  int idx;
  int nbr_slb;
  void *vp;

  if( cd == cd_max) goto read;

  nbr_slb = lmt_lst[cd]->lmt_dmn_nbr;       

  if(nbr_slb == 1 ) {

    /* check to see if we have a wrapped dimension */
    if( lmt_lst[cd]->lmt_dmn[0]->srt > lmt_lst[cd]->lmt_dmn[0]->end) goto wrap;

    lmt[cd]=lmt_lst[cd]->lmt_dmn[0];
    vp = nco_msa_rec_calc(cd+1,cd_max,lmt,lmt_lst,vara);
    return vp;
      
  }

  /* Here we deal with multiple hyperslabs */
  if(nbr_slb > 1) {

    int slb_index; 
    long var_sz=1L;   
    long lcnt ;
    long *vp_size;
    long *indices;

    void **vp_wrap;
    void *vp_step;
    void *slab;

    ptrdiff_t vp_offset;
    ptrdiff_t vp_max;
    ptrdiff_t vp_inc;

    ptrdiff_t slab_step;
    ptrdiff_t slab_size;


    lmt_sct lmt_ret;

    vp_size = (long *)nco_malloc(nbr_slb * sizeof(long));
    indices = (long *)nco_malloc(nbr_slb * sizeof(long));
    vp_wrap =(void **)nco_malloc(nbr_slb * sizeof(void *));
    
    for(idx = 0 ; idx < nbr_slb ; idx++) {

      lmt[cd]=lmt_lst[cd]->lmt_dmn[idx];
      vp_wrap[idx]=nco_msa_rec_calc(cd+1,cd_max,lmt,lmt_lst,vara);
      vp_size[idx]=vara->sz;
    }       
    
    for(idx = 0 ; idx < cd_max ; idx++)
      var_sz *=  (idx < cd ? lmt[idx]->cnt : lmt_lst[idx]->dmn_cnt) ;           
    

    vp = nco_calloc(var_sz,nco_typ_lng(vara->type)); 
    
    lcnt = nco_typ_lng(vara->type);
    for(idx=(cd+1) ; idx < cd_max ; idx++)
      lcnt  *=  lmt_lst[idx]->dmn_cnt;


    vp_inc = (ptrdiff_t)( lcnt*(lmt_lst[cd]->dmn_cnt) );
  
    vp_max = (ptrdiff_t)( var_sz * nco_typ_lng(vara->type) );

    for( idx=0 ; idx < nbr_slb ; idx++)
      indices[idx] = lmt_lst[cd]->lmt_dmn[idx]->srt;


  vp_offset = 0L;
  while(nco_msa_calc_indices(True,lmt_lst[cd],&indices[0],&lmt_ret,&slb_index))
    {
     vp_step = vp + (ptrdiff_t)vp_offset ;  
     slab = vp_wrap[slb_index]+(ptrdiff_t)(lmt_ret.srt*lcnt); 
     slab_step = (ptrdiff_t) ( lcnt* (lmt_lst[cd]->lmt_dmn[slb_index]->cnt));  
 
     slab_size = (ptrdiff_t)(lmt_ret.cnt*lcnt);

     while( vp_step - vp < vp_max){
       (void)memcpy(vp_step ,slab,slab_size);
       slab += slab_step;
       vp_step += vp_inc;
      }    
      vp_offset += slab_size;
    }
 
  

  for(idx =0 ; idx < nbr_slb ; idx++)
    (void)nco_free(vp_wrap[idx]);
     
  vara->sz = var_sz;
  return vp; 

  }


  


/* wrapped dimension --needs splitting into 2 */
   wrap:
   {
     /* split into 2 limits */
     long var_sz=1L;
     long dmn_sz;
     long index;
     long srt;
     long end;
     long cnt;
     long srd;
     long vp_size[2]; 
     long lcnt; 
     
     void *vp_step;
     void *vp_tmp[2];  
     void *vp_wrap[2] ;

     ptrdiff_t post_map[2];
     lmt_sct lmt_wrp[2];

     srt = lmt_lst[cd]->lmt_dmn[0]->srt;
     end = lmt_lst[cd]->lmt_dmn[0]->end;
     cnt = lmt_lst[cd]->lmt_dmn[0]->cnt;
     srd = lmt_lst[cd]->lmt_dmn[0]->srd;

    (void)nco_inq_dimlen(vara->nc_id,lmt_lst[cd]->lmt_dmn[0]->id,&dmn_sz);
     

   for(idx=0 ; idx < cnt ; idx++){
      index = (srt + srd * idx) % dmn_sz;
       if(index < srt) break;
    }
    
     lmt_wrp[0].srt = srt;
     
     if(idx == 1) {
      lmt_wrp[0].end=srt;
      lmt_wrp[0].cnt = 1L;
      lmt_wrp[0].srd = 1L; 
     }else{
       lmt_wrp[0].end = srt + srd*(idx -1); 
       lmt_wrp[0].cnt = idx;
       lmt_wrp[0].srd = srd;
     }
    
    lmt_wrp[1].srt = index;

    lmt_wrp[1].cnt = cnt - lmt_wrp[0].cnt;   
    if(lmt_wrp[1].cnt == 1L ) {
      lmt_wrp[1].end = index;
      lmt_wrp[1].srd = 1L;
    }else{
      lmt_wrp[1].end = index + (lmt_wrp[1].cnt -1)*srd;
      lmt_wrp[1].srd =srd; 
    }    
    
     
    /* Get hyperslabs for each element */

    for(idx=0 ; idx < 2 ;idx++){

      lmt[cd] = &lmt_wrp[idx];    
      vp_wrap[idx] = (void *)nco_msa_rec_calc(cd+1,cd_max,lmt,lmt_lst,vara);
      vp_size[idx] = vara->sz;
    }

    
    for(idx = 0 ; idx < cd_max ; idx++)
      var_sz *=  idx < cd ? lmt[idx]->cnt : lmt_lst[idx]->dmn_cnt ;      
    
    /* Sanity check */ 
    if( var_sz != vp_size[0] + vp_size[1] ) {
      fprintf(stderr, "Memory Allocation error in wrap part of  nco_msa_rec_calc\n");
       nco_exit(EXIT_FAILURE);      
    }
 
    /* allocate space to hold both slabs */
    vp = (void*)nco_malloc(var_sz *nco_typ_lng(vara->type));          


   /* Merge slabs into a single entity */
    lcnt = nco_typ_lng(vara->type);

    for(idx=(cd+1) ; idx < cd_max ; idx++)
      lcnt  *=  lmt_lst[idx]->dmn_cnt;
      
    post_map[0] =  (ptrdiff_t)(lcnt * lmt_wrp[0].cnt);
    post_map[1] =  (ptrdiff_t)(lcnt * lmt_wrp[1].cnt);
     
    vp_tmp[0] = vp_wrap[0]; /* we move through hyperslabs in these increments */
    vp_tmp[1] = vp_wrap[1]; /* these blocks represent the slabs already processed */
          
    vp_step = vp;

    lcnt = var_sz * nco_typ_lng(vara->type);

    while( vp_step - vp < (ptrdiff_t)lcnt ) {
      
      (void)memcpy(vp_step,vp_tmp[0],post_map[0]);
      (void)memcpy(vp_step + post_map[0],vp_tmp[1],post_map[1]);
      
      vp_tmp[0] += post_map[0];
      vp_tmp[1] += post_map[1];
      vp_step += (post_map[0] + post_map[1]);
      }

    (void*)nco_free(vp_wrap[0]);  
    (void*)nco_free(vp_wrap[1]);  
 
     vara->sz = var_sz;
   return vp;
   } /* end wrap */


/* read and return hyperslab */
   read:
  {   
  long var_sz = 1L;
  long mult_srd=1L;
  long *dmn_srt;
  long *dmn_cnt;
  long *dmn_srd;

  dmn_srt = (long*)nco_malloc(cd_max * sizeof(long)); 
  dmn_cnt = (long*)nco_malloc(cd_max * sizeof(long)); 
  dmn_srd = (long*)nco_malloc(cd_max * sizeof(long)); 


  for(idx=0 ; idx < cd_max ; idx++) {

    dmn_srt[idx] = lmt[idx]->srt;
    dmn_cnt[idx] = lmt[idx]->cnt;
    dmn_srd[idx] = lmt[idx]->srd; 
    var_sz *= dmn_cnt[idx];
    mult_srd *= lmt[idx]->srd;
  }

  vp = (void*)nco_malloc(var_sz *nco_typ_lng(vara->type)); 

  /* Check for stride */
  if(mult_srd == 1L) 
     (void)nco_get_vara(vara->nc_id,vara->id,dmn_srt,dmn_cnt,vp,vara->type); 
  else
     (void)nco_get_varm(vara->nc_id,vara->id,dmn_srt,dmn_cnt,dmn_srd,(long *)NULL,vp,vara->type);

  /* Put size into vara */
  vara->sz = var_sz;
  return vp;

  }/* end read */
 
  
}

void 
nco_msa_print_indices(lmt_all *lmt_i)

{

  int slb_nbr; 
  int idx;
  int size = lmt_i->lmt_dmn_nbr;
  long *indices;
  lmt_sct lmt;

  indices = (long *)nco_malloc(size *sizeof(long));

  printf("name=%s total size=%ld\n",lmt_i->dmn_nm,lmt_i->dmn_cnt);
  
  for( idx=0 ; idx < size ; idx++)
    indices[idx] = lmt_i->lmt_dmn[idx]->srt;

  
  while(nco_msa_calc_indices(False,lmt_i,&indices[0],&lmt,&slb_nbr))
    printf("slb_nbr=%d srt=%ld end=%ld cnt=%ld srd=%ld\n",slb_nbr, lmt.srt,lmt.end, lmt.cnt,lmt.srd);
  
  
}
       

bool 
nco_msa_calc_indices( /* if false then this is the last limit out */
bool NORMALIZE,
lmt_all *lmt_a,  /* I list of lmts for each dimension  */
long *indices,   /* I/O so routine can keep track of where its at */
lmt_sct *lmt,     /* O  output hyperslab */
int *slb)         /* slab which the above limit refers to */ 
/* A very messy unclear in-efficient routine that needs re-writing */
/* if NORMALIZE then It returns the slab number and the hyperslab in lmt- Note */
/* VERY IMPORTANT - This is the slab WHITH-IN the slab             */
/* So the stride is ALWAYS 1                                       */
{
  int i;
  int  size = lmt_a->lmt_dmn_nbr;
  bool *min;
  
  int previous_slab = 0;
  int current_slab  = 0;
  long current_index;
  long previous_index;
  long cnt=0L;


  min = (bool*)nco_malloc(size*sizeof(bool));

  lmt->srt = -1L;
  lmt->cnt = 0L;
  lmt->srd = 0L;

  while( ++cnt ){

    current_index = nco_msa_min_indices(indices,min,size);
    
    current_slab = -1;
    for(i =0 ; i <size ; i++)
      if(min[i]){ current_slab = i; break; }


    if(current_slab == -1 ){
      if(lmt->srt == -1) return False; 
      else break;
    }      
  
    if( min[previous_slab] ) current_slab = previous_slab;

    if(lmt->srt > -1  && current_slab != previous_slab) break;  
          


    if(lmt->cnt > 1L){
      (lmt->cnt)++;
      lmt->end = current_index;
    }

    if(lmt->cnt == 1L) {
      lmt->cnt = 2L;
      lmt->srd=current_index - previous_index;
      lmt->end = current_index;
    }

    if(lmt->srt == -1L) {
      lmt->srt= current_index;
      lmt->cnt = 1L;
      lmt->end = current_index;    
      lmt->srd = 1L;
    }

    for(i = 0 ; i < size ; i++ ) {
      if( min[i] ) { 
	indices[i] += lmt_a->lmt_dmn[i]->srd;
	if( indices[i] > lmt_a->lmt_dmn[i]->end) indices[i] = -1; 
     
      }
    }   
    previous_index = current_index;
    previous_slab = current_slab;
   

  } /* end while */

  *slb = previous_slab;

  /* normalize the slab */
  if(NORMALIZE){  
    lmt->srt =  (lmt->srt - lmt_a->lmt_dmn[*slb]->srt) / ( lmt_a->lmt_dmn[*slb]->srd );
    lmt->end =  (lmt->end - lmt_a->lmt_dmn[*slb]->srt) / ( lmt_a->lmt_dmn[*slb]->srd );
    lmt->srd = 1L;
  }
  return True;
  
}


void 
nco_msa_calc_cnt(lmt_all *lmt_a)

{
  int idx;
  long cnt=0;
  int  size = lmt_a->lmt_dmn_nbr;
  long *indices;
  bool *min;

  indices = (long *)nco_malloc(size * sizeof(long));
  min = (bool *)nco_malloc(size * sizeof(bool));
  
  if( size == 1) {
    lmt_a->dmn_cnt = lmt_a->lmt_dmn[0]->cnt;
    return;

  }
  /* initialise indices with srt from    */
  for( idx=0 ; idx < size ; idx++)
    indices[idx] = lmt_a->lmt_dmn[idx]->srt;

  
  while( nco_msa_min_indices(indices,min,size) != LONG_MAX ) {
   for(idx = 0 ; idx < size ; idx++ ) {
     if( min[idx] ) { 
	indices[idx] += lmt_a->lmt_dmn[idx]->srd;
	if( indices[idx] > lmt_a->lmt_dmn[idx]->end) indices[idx] = -1; 
        }
   }   
    cnt++;
  } 


  lmt_a->dmn_cnt=cnt;

}


long
nco_msa_min_indices( /* find min values in current */
long *current,   /* current indices */
bool *min,       /* element true if a minimum */
int size) {       /* size of current and min */
 
  int i;
  long min_val = LONG_MAX;
  
  for( i=0 ; i< size ; i++ )
    if( current[i] != -1 && current[i] < min_val ) min_val = current[i];

  for( i = 0 ; i < size ; i++)
    min[i] = (( current[i] != -1 && current[i]== min_val ) ? True : False );
    
  return min_val;

}

 
void
nco_cpy_var_val_multi_lmt /* [fnc] Copy variable data from input to output file */
(const int in_id, /* I [id] netCDF input file ID */
 const int out_id, /* I [id] netCDF output file ID */
 FILE * const fp_bnr, /* I [fl] Unformatted binary output file handle */
 const bool NCO_BNR_WRT, /* I [flg] Write binary file */
 char *var_nm, /* I [sng] Variable name */
 lmt_all * const lmt_lst, /* I multi-hyperslab limits */
 int nbr_dmn_fl) /* I [nbr] Number of multi-hyperslab limits */
{

 /* Routine to copy variable data from input netCDF file to output netCDF file. 
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

  var_sct vara;       /*To hold basic data in_id,var_id ,nctype for recusive routine */

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
  if( nbr_dim == 0 ) {
    var_sz = 1L;
    void_ptr = nco_malloc(nco_typ_lng(var_type));
    (void)nco_get_var1(in_id,var_in_id,0L,void_ptr,var_type);
    (void)nco_put_var1(out_id,var_out_id,0L,void_ptr,var_type);
    if(NCO_BNR_WRT) nco_bnr_wrt(fp_bnr,"",var_nm,var_sz,var_type,void_ptr);
    (void)nco_free(void_ptr);
    return;
     
  }
  
  dmn_map_in =    (long *)nco_malloc(nbr_dim*sizeof(long));
  dmn_map_out =   (long *)nco_malloc(nbr_dim*sizeof(long));
  dmn_map_srt =   (long *)nco_malloc(nbr_dim*sizeof(long));
  dmn_id=         (int *)nco_malloc(nbr_dim*sizeof(int));
    

  lmt_mult =      (lmt_all **)nco_malloc(nbr_dim*sizeof(lmt_all *));
  lmt =           (lmt_sct **)nco_malloc(nbr_dim*sizeof(lmt_sct *));

  /* Get dimension IDs from input file */
  (void)nco_inq_vardimid(in_id,var_in_id,dmn_id);


  /* iniialize lmt_mult with multi-limits frpm lmt_lst  limits */
  /* Get dimension sizes from input file */
  for( idx=0; idx< nbr_dim; idx++){

    for( jdx=0; jdx <nbr_dmn_fl ; jdx++) {
      if( dmn_id[idx] == lmt_lst[jdx].lmt_dmn[0]->id ) {
	lmt_mult[idx] = &lmt_lst[jdx];
        break;
      }
    }
    /* create maps now - they maybe useful later */ 
    (void)nco_inq_dimlen(in_id,dmn_id[idx],&dmn_map_in[idx]);
    dmn_map_out[idx] = lmt_mult[idx]->dmn_cnt;
    dmn_map_srt[idx] = 0L;
  } /* end for */

  /* initalize vara with in_id, var_in_id,nctype etc so recursive routine can read data*/
  vara.nm = var_nm; 
  vara.id = var_in_id;
  vara.nc_id = in_id;
  vara.type = var_type;

  /* Call the super dooper recursive routine */
  void_ptr = nco_msa_rec_calc(0,nbr_dim,lmt,lmt_mult,&vara);
  var_sz = vara.sz;

  (void)nco_put_vara(out_id,var_out_id,dmn_map_srt,dmn_map_out,void_ptr,var_type);

  if(NCO_BNR_WRT) nco_bnr_wrt(fp_bnr,"",var_nm,var_sz,var_type,void_ptr);
  
  (void)nco_free(void_ptr);
  (void)nco_free(dmn_map_in); 
  (void)nco_free(dmn_map_out);
  (void)nco_free(dmn_map_srt);
  (void)nco_free(dmn_id); 
    
  return;    
}
