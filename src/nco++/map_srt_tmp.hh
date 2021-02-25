#ifndef MAP_SRT_TMP_hh_
#define MAP_SRT_TMP_hh_

#include <string.h>
#include <iostream>
#include <algorithm>


template<class T>
class map_srt{
    public:
     long _imp;
     T _V;

     // Use for sorting ascending
     bool operator<(const map_srt &right)const {
      return ( _V < right._V);
     } 


    // sort descending -a hack - function shouldn't be static - but  hey it works
    inline static bool greater(const  map_srt &x, const map_srt &y ) {
      return (x._V > y._V);
    } 

};

// Class template specialization for NC_STRING aka char* aka ragged array
template<>
class map_srt<nco_string>{
    public:
     long _imp;
     nco_string _V;

     // Use for sorting ascending
     bool operator<(const map_srt &right)const {
       return ( strcmp(_V, right._V)<0);
     } 


    // sort descending -a hack - function shouldn't be static - but  hey it works
    inline static bool greater(const  map_srt &x, const map_srt &y ) {
	 return ( strcmp( x._V ,y._V)>0);
    } 

};



template<typename T> 
void ncap_sort_and_map(var_sct *var, var_sct *var_mp, bool bd)
{
            
  long idx;  
  long sz=var->sz;
  T  *tp;
  map_srt<T> *mp;

  tp=(T*)var->val.vp;  

  mp=new map_srt<T>[sz];          
  
  // initialize array
  for(idx=0 ; idx<sz; idx++){
    mp[idx]._imp=idx;
    mp[idx]._V=tp[idx];
  }
            
  // sort array
  if(bd)
    std::sort(mp,mp+sz);  // increasing
  else 
    std::sort(mp,mp+sz,map_srt<T>::greater);  // decreasing

  //The above line may not work with some compilers if so replace with the following
  // { std::sort(mp,mp+sz); std::reverse(mp,mp+sz);}           

  // Currently only two types for the mapping
  if(var_mp->type==NC_INT){
    // var_mp will hold the mapping
    (void)cast_void_nctype(NC_INT,&var_mp->val);

    for(idx=0 ; idx<sz; idx++){
      // nb reverse the map
      var_mp->val.ip[ mp[idx]._imp] =idx;
      tp[idx]=mp[idx]._V;
    }
    (void)cast_nctype_void(NC_INT,&var_mp->val); 
          
  }else if(var_mp->type==NC_UINT64){
    // var_mp will hold the mapping
    (void)cast_void_nctype((nc_type)NC_UINT64,&var_mp->val);

    for(idx=0 ; idx<sz; idx++){
      // nb reverse the map
      var_mp->val.ui64p[ mp[idx]._imp] =idx;
              tp[idx]=mp[idx]._V;
    }
    (void)cast_nctype_void((nc_type)NC_UINT64,&var_mp->val); 
  }

  // delete array
  delete []mp;



  return ;

} // end function


template<typename T> 
void ncap_array(var_sct *var1, var_sct *var2,var_sct *var_ret)
{
long idx;
long sz=var_ret->sz;
T *tp;
T srt;
T inc;
   
 srt=*( (T*)(var1->val.vp));
 inc=*( (T*)(var2->val.vp));

 tp=(T*)var_ret->val.vp; 
  
 for(idx=0 ;idx<sz; idx++){
   tp[idx]=srt+idx*inc;
   //srt+=inc;
 }
 
}

template<typename T>
long ncap_min_index(var_sct *var)
{
bool bmss=false;
long idx;
long min_idx=0L;
long sz=var->sz;
    
T* tp;    
T tmin;
T tmss=T(0);


  
tp=(T*)var->val.vp;  
tmin=tp[0];
 
if(var->has_mss_val) {
    bmss=true;  
    tmss=((T*)(var->mss_val.vp))[0];
}    

if(bmss)
   for(idx=1;idx<sz;idx++) 
      if(tp[idx]< tmin && tp[idx]!=tmss ) { min_idx=idx; tmin=tp[idx]; }

 
if(!bmss)
   for(idx=1;idx<sz;idx++) 
      if(tp[idx]< tmin ) { min_idx=idx; tmin=tp[idx]; }


return min_idx;   
}

template<typename T>
long ncap_max_index(var_sct *var)
{
bool bmss=false;
long idx;
long max_idx=0L;
long sz=var->sz;
    
T* tp;    
T tmax;
T tmss=T(0);

  
tp=(T*)var->val.vp;  
tmax=tp[0];
 
if(var->has_mss_val) {
    bmss=true;  
    tmss=((T*)(var->mss_val.vp))[0];
}    

if(bmss)
   for(idx=1;idx<sz;idx++) 
      if(tp[idx] > tmax && tp[idx]!=tmss ) { max_idx=idx; tmax=tp[idx]; }

 
if(!bmss)
   for(idx=1;idx<sz;idx++) 
      if(tp[idx] >tmax ) { max_idx=idx; tmax=tp[idx]; }


return max_idx;   
}



#endif
