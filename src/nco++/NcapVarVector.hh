#ifndef NCAPVARVECTOR_H
#define NCAPVARVECTOR_H

#include <string>
#include <vector>
#include <algorithm>
#include "ncap2.hh"
#include "NcapVar.hh"

class NcapVarVector : public std::vector<NcapVar*>  {

public:

  long    size();
  NcapVar* find(const char*nm);
  NcapVar* find(std::string s_fnm);
  long     findi(std::string s_fnm);
  long     find_lwr(std::string s_fnm);
  void    push_ow(NcapVar *Nvar);
  void    push(NcapVar *Nvar);
  void    erase(std::string s_fnm);
  void    erase_all(std::string var_nm);
  void    erase(long idx); 
  void    sort();

  //Use for sorting
  struct less_mag
   {
       bool operator()( NcapVar* v1, NcapVar* v2) { 
         return ( v1->getFll() < v2->getFll()) ; 
       }
  };

 

 };

#endif



