#ifndef MAP_SRT_TMP_hh_
#define MAP_SRT_TMP_hh_

#include <algorithm>

template<class T>
class map_srt{
    public:
     long _imp;
     T _V;

     //Use for sorting
     bool operator<(const map_srt &right)const {
      return ( _V < right._V);
   } 
};


template<typename T> 
void ncap_sort_and_map(var_sct *var, var_sct *var_mp)
{
            
  long idx;  
  long sz=var->sz;
  T  *tp;
  map_srt<T> *mp;

  tp=(T*)var->val.vp;  
  mp=new map_srt<T>[sz];          

  // initalize array
  for(idx=0 ; idx<sz; idx++){
    mp[idx]._imp=idx;
    mp[idx]._V=tp[idx];
  }
            
  // sort array
  std::sort(mp,mp+sz);  

  // Currently only two types for the mapping
  if(var_mp->type==NC_INT){
    // var_mp will hold the mapping
    (void)cast_void_nctype(NC_INT,&var_mp->val);

    for(idx=0 ; idx<sz; idx++){
      // nb reverse the map
      var_mp->val.lp[ mp[idx]._imp] =idx;
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
   tp[idx]=srt;
   srt+=inc;
 }
 
}

#endif
