#include "NcapVarVector.hh"

NcapVar* NcapVarVector::find(const char*nm){
  long idx;
  long sz=t_vector.size();
    for(idx=0; idx<sz;idx++)
      if(!strcmp(nm, t_vector[idx]->s_va_nm.c_str()))
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
      if( s_fnm==t_vector[idx]->s_va_nm)
	return idx;
      return -1;
}



// if Var exists overwrite, else add to vector
void NcapVarVector::push_ow(NcapVar *Nvar){
  long lret;
  NcapVar *Ntmp;
  lret=findi(Nvar->s_va_nm);
  if(lret >= 0 ) {
    // delete current value
    Ntmp= t_vector[lret];
    // re-assign
    t_vector[lret]=Nvar;
    delete Ntmp;
  }else   
    (void)t_vector.push_back(Nvar);

}
