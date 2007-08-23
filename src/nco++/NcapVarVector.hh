#ifndef NCAPVARVECTOR_H
#define NCAPVARVECTOR_H

#include <string>
#include <vector>
#include "ncap2.hh"
#include "NcapVar.hh"

class NcapVarVector {

protected:
  std::vector<NcapVar*> t_vector;

public:
  NcapVar* operator [](long idx)const {
   return t_vector[idx];
  }
  long    size(){ return t_vector.size();}
  NcapVar* find(const char*nm);
  NcapVar* find(std::string s_fnm);
  long     findi(std::string s_fnm);
  void    push_ow(NcapVar *Nvar);
  void    push(NcapVar *Nvar);
  void    erase(long idx);
  void    erase(std::string s_fnm);
  NcapVar* pop();
};

#endif



