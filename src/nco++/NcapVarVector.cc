#include "NcapVarVector.hh"

long NcapVarVector::size(){

  return (long)std::vector<NcapVar*>::size();

}


NcapVar* NcapVarVector::find(std::string s_fnm){
long lret;
  lret=findi(s_fnm);
  if(lret >=0)
    return (*this)[lret];

  return NULL;

}

NcapVar* NcapVarVector::find(const char*nm){
long lret;
  lret=findi( std::string(nm));
  if(lret >=0)
    return (*this)[lret];

  return NULL;
}

long NcapVarVector::findi(std::string s_fnm){
    long lret;
    NcapVar* Ntmp;
    std::vector<NcapVar*>::iterator we;

    if(size()==0)
      return -1;

    Ntmp=new NcapVar( (var_sct*)NULL ,s_fnm);
    
    we=std::lower_bound(begin(),end(),Ntmp,less_mag());
   

    // Item already in vector
    if( we !=end() && (*we)->getFll()==s_fnm )
      lret=we-begin(); 
    else
      lret=-1;
    
    // nb var_in also freed here  
    delete Ntmp;     
    
    return lret;

}

// used in attribute propagation
long NcapVarVector::find_lwr(std::string s_fnm){
    long lret;
    std::string s_fl;
    NcapVar* Ntmp;
    std::vector<NcapVar*>::iterator we;

    if(size()==0)
      return -1;

    Ntmp=new NcapVar( (var_sct*)NULL ,s_fnm);
    
    we=std::lower_bound(begin(),end(),Ntmp,less_mag());
     
    // Return a partial match
    lret=-1; 
    if( we!=end()){
      s_fl=(*we)->getFll();
      if(s_fl.find(s_fnm,0) !=std::string::npos)
	lret=we-begin();
    }
    
    // nb var_in also freed here  
    delete Ntmp;     
    
    return lret;

}



void NcapVarVector::push_ow(NcapVar *Nvar){
    push(Nvar);
}


// add to vector
void NcapVarVector::push(NcapVar *Nvar){
    std::vector<NcapVar*>::iterator we;


    if(size()==0){
      push_back(Nvar);
	return;
    }
    
    we=std::lower_bound(begin(),end(),Nvar,less_mag());
   

    // Item already in vector
    if( we !=end() && (*we)->getFll()== Nvar->getFll()){
      delete *we;
      *we=Nvar;
    // Insert Item
    } else{
      (void)std::vector<NcapVar*>::insert(we,Nvar);
    }

}


// Delete an item
void NcapVarVector::erase(std::string s_fnm){
  long idx;
  idx=findi(s_fnm);
  
  if(idx >=0) erase(idx);

}



// Delete an item
void NcapVarVector::erase(long idx){
  std::vector<NcapVar*>::iterator we=begin();
  we=begin();
  we+=idx;
  delete (*this)[idx];
  std::vector<NcapVar*>::erase(we);

}

void NcapVarVector::sort(){
  std::sort(begin(),end(),less_mag());
}


// takes a variable name as an argument and 
// deletes that and all associated attributes 
// of the form var_m@
void NcapVarVector::erase_all(std::string var_nm){
  long sz;
  long idx;
  long srt=-1;
  long end=-1;
  idx=findi(var_nm);
  
  if(idx >=0) erase(idx);
  
  sz=this->size();

  for(idx=0; idx<sz;idx++)
  {
    if( (*this)[idx]->xpr_typ==ncap_att &&  (*this)[idx]->getVar()==var_nm)
    {  
      if(srt==-1) srt=idx;    
       else if(srt>=0)
       {    
	end=idx-1;
        break; 
       }
    }  
  }  

  if(srt >=0) 
    // call base class 
    std::vector<NcapVar*>::erase(begin()+srt,begin()+end);


}
/*
NcapVar* NcapVarVector::find(const char*nm){
  long idx;
  long sz=this->size();
    for(idx=0; idx<sz;idx++)
      if(!strcmp(nm, (*this)[idx]->getFll().c_str()))
	return (*this)[idx];
      return NULL;
}

*/

/*
NcapVar* NcapVarVector::find(std::string s_fnm){
  NcapVar* Nvar;
  Nvar=find(s_fnm.c_str());
  return Nvar;
}

*/


/*
long NcapVarVector::findi(std::string s_fnm){
  long idx;
  long sz=this->size();
    for(idx=0; idx<sz;idx++)
      if( s_fnm==(*this)[idx]->getFll())
	return idx;
      return -1;
}

*/

/*
void NcapVarVector::push_ow(NcapVar *Nvar){
  long lret;
  NcapVar *Ntmp;
  lret=findi(Nvar->getFll());
  if(lret >= 0 ) {
    // delete current value
    Ntmp= (*this)[lret];
    // re-assign
    (*this)[lret]=Nvar;
    delete Ntmp;
  }else   
    push_back(Nvar);

}
*/

