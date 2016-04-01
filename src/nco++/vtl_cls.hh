
// Standard C++ headers
#ifndef INC_VTL_CLS_hh_
#define INC_VTL_CLS_hh_


#include <vector>

#include <antlr/AST.hpp>
#include "ncoParserTokenTypes.hpp"
#include "ncap2_utl.hh"
#include "ncoTree.hpp"
#include "fmc_cls.hh"

// forward declaration
class ncoTree;


enum vtl_typ { VVAR,VLVAR,VDVAR, VATT,VLATT,VDATT,VDIM, VDIM_SIZE,VEXPR,VCALL_REF,VPOINTER,VSTRING };

class vtl_cls: public ncoParserTokenTypes {
public:
  // expression types,

  std::vector<fmc_cls> fmc_vtr;

  int size(void) { return fmc_vtr.size(); }
  std::vector<fmc_cls> *lst_vtr(void){ return &fmc_vtr;}
  virtual var_sct *fnd(RefAST expr, RefAST fargs,fmc_cls &fmc_obj, ncoTree &walker)=0;
  static vtl_typ expr_typ(RefAST expr){
    int nbr_chd;
    int chd_typ=0;
    vtl_typ lcl_typ;
    

    nbr_chd=expr->getNumberOfChildren(); 

    if(nbr_chd >0) chd_typ=expr->getFirstChild()->getType();
      

    switch(expr->getType()) {

      case VAR_ID: 
	        { if( nbr_chd==0 )          
	             lcl_typ=VVAR;
		  else if ( nbr_chd==1 && chd_typ==LMT_LIST)
		     lcl_typ=VLVAR;
		  else if (nbr_chd==1 &&  chd_typ==DMN_LIST)
		     lcl_typ=VDVAR;
		  else lcl_typ=VEXPR; 
                }
		break;
      case ATT_ID:
	        { if( nbr_chd==0 )          
	             lcl_typ=VATT;
		  else if ( nbr_chd==1 && chd_typ==LMT_LIST)
		     lcl_typ=VLATT;
		  else if (nbr_chd==1 &&  chd_typ==DMN_LIST)
		     lcl_typ=VDATT;
		  else lcl_typ=VEXPR; 
                }
		break;

      case DIM_ID: 
	        lcl_typ=VDIM;
                break;
      

      case DIM_ID_SIZE: 
	        lcl_typ=VDIM_SIZE;
                break;
      
      case CALL_REF:
	        lcl_typ=VCALL_REF;
                break;      

    case UTIMES:
               {
		 if(nbr_chd ==1 && chd_typ==ATT_ID)   
                   lcl_typ=VPOINTER;
		 else
                   lcl_typ=VEXPR;    
               }      
               break;

    case NSTRING:
    case N4STRING:    
              lcl_typ=VSTRING;  
              break; 
      default:
           lcl_typ=VEXPR;
           break;
    }    

    return lcl_typ;
  }


};


#endif
