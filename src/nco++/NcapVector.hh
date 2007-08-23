#ifndef INC_NcapVector_hh_
#define INC_NcapVector_hh_

#include <malloc.h>
#include <string.h>
#include <functional>
#include <string>
#include <vector>
#include <algorithm>

template<class T>
class NcapVector : public std::vector<T> {


public:

  /*
  void push(const T &x) {
    this->push_back(x);
  }
  
  

  T pop() {
    T tret(0);
    if(this->size() > 0 ) {
      tret=this->back();
      this->pop_back();
      return tret;
    }else{
      return NULL;
    }
  }

  */
  

  T find (const char *nm) {
    int idx;
    int sz=this->size();
    for(idx=0 ; idx < sz; idx++)
      if(!strcmp(nm, (*this)[idx]->nm)) break;
    if(idx < sz) 
	return (*this)[idx]; 
    else
	 return NULL;
  }

  T find(std::string snm) {
    return find(snm.c_str());
  }

  int findi(std::string snm){
    int idx;
    int sz=this->size();
    const char *nm=snm.c_str();

    for(idx=0 ; idx < sz; idx++)
      if(!strcmp(nm, (*this)[idx]->nm)) break;
    if(idx < sz) 
	return idx; 
    else
	 return -1;

  }

  // stop compiler warnings
  int size(){
    // Call base class version 
    return (int)(std::vector<T>::size());
  }

 
  struct less_mag : public std::binary_function< T, T, bool> 
   {
       bool operator()( T t1, T t2) { 
         return ( strcmp(t1->nm,t2->nm)<0) ; 
         }
  };

  


    
  bool Cmp(T t1,T t2)
   {
    if( strcmp(t1->nm,t2->nm)<0 )  return true;
    return false;
  }
  
     
 
  void sort(){
    std::sort(this->begin(), this->end(),less_mag()  );
  }

  // search -- only on a sorted vector
  bool bsearch(T t1){

     
      bool bret;
      /*
      char *cpr;
      size_t snbr; 
       T t2;
    */
       //snbr=sizeof(T);
       //std::cout<<"\n\nIndrection sizeof="<<snbr<<std::endl;
       
       //t2= (T)malloc(sizeof(*T));
       //t2->nm=t1->nm;
      // t2=t1;

 
      bret= std::binary_search( this->begin(),this->end(),t1,less_mag());
      //delete t2;

      return bret;
  }       
 
  /*
  T* ptr(long idx) {
    return &((*this)[idx]);
  }
  */

};

#endif
