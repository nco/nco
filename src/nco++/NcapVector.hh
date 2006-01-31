#include <string>
#include <vector>

template<class T>
class NcapVector {

protected:
  typename std::vector<T> t_vector;

public:
  void push(const T &x) {
    t_vector.push_back(x);
  }
  T pop() {
    T tret(0);
    if(t_vector.size() > 0 ) {
      tret=t_vector.back();
      t_vector.pop_back();
      return tret;
    }else{
      return NULL;
    }
  }

  T find (const char *nm) {
    int idx;
    int sz=t_vector.size();
    for(idx=0 ; idx < sz; idx++)
      if(!strcmp(nm, t_vector[idx]->nm)) break;
    if(idx < sz) 
	return t_vector[idx]; 
    else
	 return NULL;
  }

  T find(std::string snm) {
    return find(snm.c_str());
  }
  T operator[] (long idx) const {
    return t_vector[idx];
  }
  T* ptr(long idx) {
    return &t_vector[idx];
  }

  long size(){
    return t_vector.size();
  }
};

