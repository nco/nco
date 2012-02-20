#ifndef INC_VAR_OP_hh_
#define INC_VAR_OP_hh_

#include <algorithm>
#include "nco.h"
#include "ncoParserTokenTypes.hpp"
template<class T>
class VarOp : public ncoParserTokenTypes {

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
      
      case PLUS_ASSIGN:   
      case PLUS:
	if(!bmss) {
	  for(idx=0 ; idx<sz ; idx++) tp1[idx]+=tp2[idx];
	}else{
	  for(idx=0 ; idx<sz ; idx++){
	    if( tp1[idx] != tmss && tp2[idx] != tmss)
	      tp1[idx]+=tp2[idx];
            else tp1[idx]=tmss;
	  }
	}  
        break;

      case MINUS_ASSIGN:
      case MINUS:
	if(!bmss) {
	  for(idx=0 ; idx<sz ; idx++) tp1[idx]-=tp2[idx];
	}else{
	  for(idx=0 ; idx<sz ; idx++){
	    if( tp1[idx] != tmss &&  tp2[idx] != tmss)
	      tp1[idx]-=tp2[idx];
            else tp1[idx]=tmss;
	  }
	}  

        break;

      case TIMES_ASSIGN:
      case TIMES:
	if(!bmss) {
	  for(idx=0 ; idx<sz ; idx++) tp1[idx]*=tp2[idx];
	}else{
	  for(idx=0 ; idx<sz ; idx++){
	    if( tp1[idx] != tmss && tp2[idx] != tmss)
	      tp1[idx]*=tp2[idx];
	    else tp1[idx]=tmss;
	  }
	}  

        break;

      case DIVIDE_ASSIGN:
      case DIVIDE:
	if(!bmss) {
	  for(idx=0 ; idx<sz ; idx++) tp1[idx] /= tp2[idx];
	}else{
	  for(idx=0 ; idx<sz ; idx++){
	    if( tp1[idx] != tmss && tp2[idx] != tmss)
	      tp1[idx] /= tp2[idx];
	    else tp1[idx]=tmss;
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
	    else tp1[idx]=tmss;
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
	    else tp1[idx]=tmss;
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
	    else tp1[idx]=tmss;
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
	    else tp1[idx]=tmss;
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
	    else tp1[idx]=tmss;
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
	    else tp1[idx]=tmss;
	  }
	}  
        break;

      case EQ:
	if(!bmss) {
	  for(idx=0 ; idx<sz ; idx++) tp1[idx]= (tp1[idx]==tp2[idx]);
	}else{
	  for(idx=0 ; idx<sz ; idx++){
	    if( tp1[idx] != tmss && tp2[idx] != tmss)
	      tp1[idx]=tp1[idx] == tp2[idx];
	    else tp1[idx]=tmss;
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
	    else tp1[idx]=tmss;
	  }
	}  
        break;


      case FLTHAN:
	if(!bmss) {
	  for(idx=0 ; idx<sz ; idx++) 
           if(tp1[idx] >= tp2[idx])
             tp1[idx]=tp2[idx];
	}else{
	  for(idx=0 ; idx<sz ; idx++){
            if(tp1[idx]==tmss || tp2[idx]==tmss)
               tp1[idx]=tmss; 
	    else if(tp1[idx] >= tp2[idx]  )
	      tp1[idx]=tp2[idx];
	  }
	}  
        break;

      case FGTHAN:
	if(!bmss) {
	  for(idx=0 ; idx<sz ; idx++) 
           if(tp1[idx] <= tp2[idx])
             tp1[idx]=tp2[idx];
	}else{
	  for(idx=0 ; idx<sz ; idx++){
            if(tp1[idx]==tmss || tp2[idx]==tmss)
               tp1[idx]=tmss; 
            else if(tp1[idx] <= tp2[idx] )
	      tp1[idx]=tp2[idx];
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

    case POST_INC:
    case INC:
	if(!bmss) {
	  for(idx=0 ; idx<sz ; idx++) ++tp1[idx];
	}else{
	  for(idx=0 ; idx<sz ; idx++){
	    if( tp1[idx] != tmss ) ++tp1[idx];
	  }  
	}
        break;

    case POST_DEC:
    case DEC:
	if(!bmss) {
	  for(idx=0 ; idx<sz ; idx++) --tp1[idx];
	}else{
	  for(idx=0 ; idx<sz ; idx++){
	    if( tp1[idx] != tmss ) --tp1[idx];
	  }  
	}
        break;


    case MISS2ZERO:
	if(bmss) 
	  for(idx=0 ; idx<sz ; idx++) 
            if( tp1[idx]==tmss) tp1[idx]=0;

        break;

    case VSQR2:
	if(!bmss) {
	  for(idx=0 ; idx<sz ; idx++) tp1[idx]*=tp1[idx];
	}else{
	  for(idx=0 ; idx<sz ; idx++){
	    if( tp1[idx] != tmss ) tp1[idx]*=tp1[idx];
	  }  
	}
        break;

    case VSORT: 
         std::sort(tp1,tp1+sz );  
         break; 

	 // reverse sort 
    case VRSORT: 
         std::sort(tp1,tp1+sz,std::greater<T>() );  
         break; 

    // return absolute value 
    case VABS:
	if(!bmss) {
	  for(idx=0 ; idx<sz ; idx++)
            if(tp1[idx] < 0) tp1[idx]*=-1;  
	}else{
	  for(idx=0 ; idx<sz ; idx++){
	    if( tp1[idx] != tmss && tp1[idx] < 0 ) tp1[idx]*=-1;
	  }  
	}
        break;


    default: break;	

    } // end switch	

    return var1;
  } // end var_op


#endif
