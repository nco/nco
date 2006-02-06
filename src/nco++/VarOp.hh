#include <math.h>
#include "ncap.h"
#include "ncoParserTokenTypes.hpp"

template<class T>
class VarOp: public ncoParserTokenTypes {

  //private:
  //struct ncoParserTokenTypes TT;

public:
  VarOp() {;}
  // binary Operations
   public:  var_sct* var_var_op(var_sct* var1 , var_sct* var2, int op);

  // Unary Operations
  public:   var_sct* var_op(var_sct* var1 , int op);

};


template<class T>
var_sct* VarOp<T>::var_var_op(var_sct* var1, var_sct* var2, int op) {

    bool bmss=false;
    long idx;
    long sz;
    T tmss=T(0);
    T* tp1;
    T* tp2;

    sz=var1->sz;
    
    tp1=(T*)(var1->val.vp);
    tp2=(T*)(var2->val.vp);  


    if(var1->has_mss_val) {
      tmss=((T*)(var1->mss_val.vp))[0];
      bmss=true;
    }else if(var2->has_mss_val) {
	
      tmss=((T*)(var2->mss_val.vp))[0];
      bmss=true;
      // Now copy missing value to var1 as on return var2 will be freed 
      (void)nco_mss_val_cnf(var2,var1);
    }  
 
    switch(op) {

      case PLUS:
	if(!bmss) {
	  for(idx=0 ; idx<sz ; idx++) tp1[idx]+=tp2[idx];
	}else{
	  for(idx=0 ; idx<sz ; idx++){
	    if( tp1[idx] != tmss && tp2[idx] != tmss)
	      tp1[idx]+=tp2[idx];
	  }
	}  
        break;
      
      case MINUS:
	if(!bmss) {
	  for(idx=0 ; idx<sz ; idx++) tp1[idx]-=tp2[idx];
	}else{
	  for(idx=0 ; idx<sz ; idx++){
	    if( tp1[idx] != tmss &&  tp2[idx] != tmss)
	      tp1[idx]-=tp2[idx];
	  }
	}  

        break;
      
      case TIMES:
	if(!bmss) {
	  for(idx=0 ; idx<sz ; idx++) tp1[idx]*=tp2[idx];
	}else{
	  for(idx=0 ; idx<sz ; idx++){
	    if( tp1[idx] != tmss && tp2[idx] != tmss)
	      tp1[idx]*=tp2[idx];
	  }
	}  

        break;

      case DIVIDE:
	if(!bmss) {
	  for(idx=0 ; idx<sz ; idx++) tp1[idx] /= tp2[idx];
	}else{
	  for(idx=0 ; idx<sz ; idx++){
	    if( tp1[idx] != tmss && tp2[idx] != tmss)
	      tp1[idx] /= tp2[idx];
	  }
	}  


        break;

      case MOD:
	// deal with else-where - can't template this function
        break;

      case CARET:
	// deal with else-where - can't template this function
        break;

	// Logical & Comparison Operators

      case LAND:
	if(!bmss) {
	  for(idx=0 ; idx<sz ; idx++) tp1[idx]=tp1[idx] &&  tp2[idx];
	}else{
	  for(idx=0 ; idx<sz ; idx++){
	    if( tp1[idx] != tmss && tp2[idx] != tmss)
	      tp1[idx]=tp1[idx] &&  tp2[idx];
	  }
	}  
        break;

      case LOR:
	if(!bmss) {
	  for(idx=0 ; idx<sz ; idx++) tp1[idx]=tp1[idx] ||  tp2[idx];
	}else{
	  for(idx=0 ; idx<sz ; idx++){
	    if( tp1[idx] != tmss && tp2[idx] != tmss)
	      tp1[idx]=tp1[idx] ||  tp2[idx];
	  }
	}  
        break;

      case LTHAN:
	if(!bmss) {
	  for(idx=0 ; idx<sz ; idx++) tp1[idx]=tp1[idx] < tp2[idx];
	}else{
	  for(idx=0 ; idx<sz ; idx++){
	    if( tp1[idx] != tmss && tp2[idx] != tmss)
	      tp1[idx]=tp1[idx] < tp2[idx];
	  }
	}  
        break;

      case GTHAN:
	if(!bmss) {
	  for(idx=0 ; idx<sz ; idx++) tp1[idx]=tp1[idx] > tp2[idx];
	}else{
	  for(idx=0 ; idx<sz ; idx++){
	    if( tp1[idx] != tmss && tp2[idx] != tmss)
	      tp1[idx]=tp1[idx] >  tp2[idx];
	  }
	}  
        break;

      case GEQ:
	if(!bmss) {
	  for(idx=0 ; idx<sz ; idx++) tp1[idx]=tp1[idx] >= tp2[idx];
	}else{
	  for(idx=0 ; idx<sz ; idx++){
	    if( tp1[idx] != tmss && tp2[idx] != tmss)
	      tp1[idx]=tp1[idx] >= tp2[idx];
	  }
	}  
        break;

      case LEQ:
	if(!bmss) {
	  for(idx=0 ; idx<sz ; idx++) tp1[idx]=tp1[idx] <= tp2[idx];
	}else{
	  for(idx=0 ; idx<sz ; idx++){
	    if( tp1[idx] != tmss && tp2[idx] != tmss)
	      tp1[idx]=tp1[idx] <= tp2[idx];
	  }
	}  
        break;

      case EQ:
	if(!bmss) {
	  for(idx=0 ; idx<sz ; idx++) tp1[idx]=tp1[idx] == tp2[idx];
	}else{
	  for(idx=0 ; idx<sz ; idx++){
	    if( tp1[idx] != tmss && tp2[idx] != tmss)
	      tp1[idx]=tp1[idx] == tp2[idx];
	  }
	}  
        break;

      case NEQ:
	if(!bmss) {
	  for(idx=0 ; idx<sz ; idx++) tp1[idx]=tp1[idx] != tp2[idx];
	}else{
	  for(idx=0 ; idx<sz ; idx++){
	    if( tp1[idx] != tmss && tp2[idx] != tmss)
	      tp1[idx]=tp1[idx] != tp2[idx];
	  }
	}  
        break;

    } // end switch
    
    return var1;
  } // end var_var_op


template<class T>
var_sct*  VarOp<T>::var_op(var_sct* var1, int op) {
    bool bmss=false;
    long idx;
    long sz;
    T tmss=T(0);
    T* tp1;

    sz=var1->sz;
    
    tp1=(T*)(var1->val.vp);



    if(var1->has_mss_val) {
      bmss=true;
      tmss=((T*)(var1->mss_val.vp))[0];
    }  

    switch(op) {


      case PLUS:
      // Do nothing
        break;
      
      case MINUS:
	if(!bmss) {
	  for(idx=0 ; idx<sz ; idx++) tp1[idx]*=-1;
	}else{
	  for(idx=0 ; idx<sz ; idx++)
	    if( tp1[idx] != tmss ) tp1[idx]*=-1;
	    
	}
        break;
       
    case LNOT: 	
	if(!bmss) {
	  for(idx=0 ; idx<sz ; idx++) tp1[idx]=!tp1[idx];
	}else{
	  for(idx=0 ; idx<sz ; idx++){
	    if( tp1[idx] != tmss ) tp1[idx]=!tp1[idx];
	  }  
	}
        break;

      default: break;	

    } // end switch	

    return var1;
  } // end var_op

