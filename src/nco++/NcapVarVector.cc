#include "NcapVarVector.hh"

NcapVar* NcapVarVector::find(const char*nm){
  long idx;
  long sz=t_vector.size();
    for(idx=0; idx<sz;idx++)
      if(!strcmp(nm, t_vector[idx]->getFll().c_str()))
	return t_vector[idx];
      return NULL;
}




NcapVar* NcapVarVector::find(std::string s_fnm){
  NcapVar* Nvar;
  Nvar=find(s_fnm.c_str());
  return Nvar;
}


long NcapVarVector::findi(std::string s_fnm){
  long idx;
  long sz=t_vector.size();
    for(idx=0; idx<sz;idx++)
      if( s_fnm==t_vector[idx]->getFll())
	return idx;
      return -1;
}


// remove top value
NcapVar *NcapVarVector::pop(){
    NcapVar *Ntmp; 

    if(t_vector.size() > 0 ) {
      Ntmp=t_vector.back();
      t_vector.pop_back();
      return Ntmp;
    }else
      return NULL;
}




// if Var exists overwrite, else add to vector
void NcapVarVector::push_ow(NcapVar *Nvar){
  long lret;
  NcapVar *Ntmp;
  lret=findi(Nvar->getFll());
  if(lret >= 0 ) {
    // delete current value
    Ntmp= t_vector[lret];
    // re-assign
    t_vector[lret]=Nvar;
    delete Ntmp;
  }else   
    (void)t_vector.push_back(Nvar);

}



// add to vector
void NcapVarVector::push(NcapVar *Nvar){
    (void)t_vector.push_back(Nvar);

}
