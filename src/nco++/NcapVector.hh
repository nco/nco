#ifndef INC_NcapVector_hh_
#define INC_NcapVector_hh_

#include <stdlib.h>
#include <string.h>
#include <functional>
#include <string>
#include <vector>
#include <algorithm>

template<class T>
class NcapVector : public std::vector<T> {


public:

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


  T find(const char *nm) {
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


  int findi(const char *nm){
    int idx;
    int sz=this->size();

    for(idx=0 ; idx < sz; idx++)
      if(!strcmp(nm, (*this)[idx]->nm)) break;
    if(idx < sz) 
	return idx; 
    else
	 return -1;

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

  
  // returns index -- nb vector must be sorted 
  int findis( const char *nm){
  int idx;
  T t2;
 
  t2=(T)malloc(sizeof(*t2));
  t2->nm=strdup(nm);

  idx=lower_bound( this->begin(), this->end(), t2, less_mag() )-this->begin();

  if(idx ==size() || strcmp(t2->nm, (*this)[idx]->nm))
    idx=-1;
  
  free(t2->nm);
  free(t2);

  return idx;      

  }



  int findis(std::string snm){
    return findis(snm.c_str());
  }


    
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
      
     bret= std::binary_search( this->begin(),this->end(),t1,less_mag());

     return bret;
}       

};

#endif
