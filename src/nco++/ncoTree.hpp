#ifndef INC_ncoTree_hpp_
#define INC_ncoTree_hpp_

#include <antlr/config.hpp>
#include "ncoParserTokenTypes.hpp"
/* $ANTLR 2.7.7 (20200513): "ncoGrammer.g" -> "ncoTree.hpp"$ */
#include <antlr/TreeParser.hpp>

#line 1 "ncoGrammer.g"

/* $Header$ */

/* Purpose: ANTLR Grammar and support files for ncap2 */

/* Copyright (C) 1995--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

    // C Standard Headers
    #include <assert.h>
    #include <ctype.h>
	#include <stdlib.h>
    #include <math.h>
    #if !(defined __xlC__) && !(defined SGIMP64) // C++ compilers that do not allow stdint.h
    #include <stdint.h> // Required by g++ for LLONG_MAX, ULLONG_MAX, by icpc for int64_t    
    #endif // C++ compilers that do not allow stdint.h

    // C++ Standard Headers 
    #include <algorithm>
    #include <iostream>
    #include <sstream>
    #include <fstream>
    #include <string>
  
    // custom exception -used for "exit" function
    #include "ExitException.hpp" 
    // Custom Headers
    #include "prs_cls.hh"
    #include "ncap2_utl.hh"
    #include "ncap2_att.hh"
    #include "fmc_cls.hh"
    #include "NcapVar.hh"
    #include "NcapVarVector.hh"
    #include "sdo_utl.hh" // SDO stand-alone utilities: dbg/err/wrn_prn()
    #include "NcapVector.hh"
    #include "antlr/TokenStreamSelector.hpp"
    #include "ncoParser.hpp"
    #include "Invoke.hh"

    ANTLR_USING_NAMESPACE(std);
    ANTLR_USING_NAMESPACE(antlr);

#line 55 "ncoTree.hpp"
class CUSTOM_API ncoTree : public ANTLR_USE_NAMESPACE(antlr)TreeParser, public ncoParserTokenTypes
{
#line 697 "ncoGrammer.g"


private:
    //prs_cls *prs_arg;
    bool bcst;
    var_sct* var_cst;
public:
    prs_cls *prs_arg;
    ASTFactory myFactory;

     //Structure to hold AST pointers to indices in hyperslabs -only temporary 
     typedef struct{
        ANTLR_USE_NAMESPACE(antlr)RefAST ind[3];
     } ast_lmt_sct;   

    void setTable(prs_cls *prs_in){
        prs_arg=prs_in; 

    }
   // Customized Constructor
    ncoTree(prs_cls *prs_in){
        prs_arg=prs_in;
        // default is NO. Casting variable set to true 
        // causes casting in function out(). var_cst must 
        // then be defined 
        bcst=false; 
        ncoTree();
    }

int 
lmt_init(
RefAST aRef, 
vector<ast_lmt_sct> &ast_lmt_vtr) 
{
      const std::string fnc_nm("lmt_init"); // [sng] Function name   

      int idx;
      int nbr_dmn;   
      int nbr_cln; // Number of colons in limit
      RefAST lRef;
      RefAST eRef;
      RefAST cRef;
      ast_lmt_sct hyp;

      if(aRef->getType() != LMT_LIST)
         return 0;
    
      lRef=aRef->getFirstChild();

      nbr_dmn=lmt_peek(aRef);      
      //nbr_dmn=lRef->getNumberOfChildren();

      for(idx=0 ; idx < nbr_dmn ; idx++)
      {
         hyp.ind[0]=ANTLR_USE_NAMESPACE(antlr)nullAST;
         hyp.ind[1]=ANTLR_USE_NAMESPACE(antlr)nullAST;
         hyp.ind[2]=ANTLR_USE_NAMESPACE(antlr)nullAST;
             
       if(lRef->getType()!=LMT) 
            return 0;
       
       eRef=lRef->getFirstChild();
       nbr_cln=0;
        
       while(eRef) 
       {
          if(eRef->getType() == COLON)
          {
            cRef=eRef;        
            nbr_cln++;
          }
           eRef=eRef->getNextSibling();
        }
      
       // Initialise  to default markers
       switch(nbr_cln) 
       {
          case 0: 
             break;
                
         case 1: hyp.ind[0]=cRef;
                 hyp.ind[1]=cRef;
                 break;

         case 2: hyp.ind[0]=cRef;
                 hyp.ind[1]=cRef;
                 hyp.ind[2]=cRef;
                 break;

         default: err_prn(fnc_nm,"Too many hyperslab indices");
                  break;  
        }

       eRef=lRef->getFirstChild();

       // point inidices to any expressions that exist
       nbr_cln=0;
       while(eRef) 
       {
          if(eRef->getType() == COLON) 
             nbr_cln++; 
           else   
             hyp.ind[nbr_cln]=eRef;
           
           eRef=eRef->getNextSibling();
       }
       // save indices 
       ast_lmt_vtr.push_back(hyp);
       lRef=lRef->getNextSibling();

      }

     return nbr_dmn;
} 

bool
lmt_var_mk(
int nbr_dmn,
RefAST lmt,
NcapVector<lmt_sct*> &lmt_vtr ) 
{
	long idx;
	long jdx;
	long sz;   
    long dmn_sz;     
	var_sct *var;
	std::string fnc_nm="lmt_var_mk"; 
	
	
	// calculate variables
	var=out(lmt->getFirstChild()->getFirstChild());
	// convert result to type int
	var=nco_var_cnf_typ(NC_INT64,var);    
	(void)cast_void_nctype((nc_type)NC_INT64,&var->val);
	sz=var->sz;
	dmn_sz=var->sz / nbr_dmn;
    
    if(nco_dbg_lvl_get() >= nco_dbg_scl)
       dbg_prn("lmt_var_mk","using hyperslab indices from a single var"); 

    // shape of var must be (nbr_dmn) or (nbr_dmn,2) or (nbr_dmn,3) 
    if( dmn_sz * nbr_dmn != sz )
	{
	  var=nco_var_free(var);
      return false;  
    }

    
	
    for(idx=0; idx<sz;idx+=dmn_sz)
	 {

		// fill out lmt structure
		// use same logic as nco_lmt_prs 
		lmt_sct* lmt_ptr=(lmt_sct*)nco_calloc((size_t)1,sizeof(lmt_sct));
		lmt_ptr->nm=NULL;
		//lmt_ptr->lmt_typ=-1;
		lmt_ptr->is_usr_spc_lmt=True; /* True if any part of limit is user-specified, else False */
		lmt_ptr->min_sng=NULL;
		lmt_ptr->max_sng=NULL;
		lmt_ptr->srd_sng=NULL;
		lmt_ptr->is_usr_spc_min=False;
		lmt_ptr->is_usr_spc_max=False;
		/* rec_skp_ntl_spf is used for record dimension in multi-file operators */
		lmt_ptr->rec_skp_ntl_spf=0L; /* Number of records skipped in initial superfluous files */

	   
       for(jdx=0;jdx<dmn_sz;jdx++)
		{     
		  
         nco_int64 ival= var->val.i64p[idx+jdx];
         switch(jdx){
           case 0: 
             lmt_ptr->is_usr_spc_min=True;
             lmt_ptr->srt=ival;
             break;
           case 1: //end
             lmt_ptr->is_usr_spc_max=True;
             lmt_ptr->end=ival;             break;
           case 2: //srd
             lmt_ptr->srd_sng=strdup("~fill_in");
             lmt_ptr->srd=ival;
             break;			
	       }

		}
		/* need to deal with situation where only start is defined -- ie picking only a single value */
		if( lmt_ptr->is_usr_spc_min==True && lmt_ptr->is_usr_spc_max==False && lmt_ptr->srd_sng==NULL)
		{
			lmt_ptr->is_usr_spc_max=True;
			lmt_ptr->end=lmt_ptr->srt; 
       }    

       lmt_vtr.push_back(lmt_ptr);		
	}
	 
	cast_nctype_void((nc_type)NC_INT64,&var->val);
	var=nco_var_free(var);  
	
   return true;

}



bool 
lmt_mk(
int nbr_dmn,
RefAST lmt,
NcapVector<lmt_sct*> &lmt_vtr ) 
{   

int idx;
int jdx;
long ldx=0L;
var_sct *var_out;
lmt_sct *lmt_ptr;
RefAST aRef;
vector<ast_lmt_sct> ast_lmt_vtr;

// deal with a single expression containing all indicies
if(lmt->getNumberOfChildren()==1 && 
   lmt->getFirstChild()->getNumberOfChildren()==1 &&
   lmt->getFirstChild()->getFirstChild()->getType() != COLON)
{
   return lmt_var_mk(nbr_dmn,lmt,lmt_vtr);   ;            
}


// populate ast_lmt_vtr
if(nbr_dmn != lmt_init(lmt,ast_lmt_vtr))
    return false;	

for(idx=0;idx<nbr_dmn;idx++){

    // fill out lmt structure
    // use same logic as nco_lmt_prs 
    lmt_ptr=(lmt_sct*)nco_calloc((size_t)1,sizeof(lmt_sct));
    lmt_ptr->nm=NULL;
    //lmt_ptr->lmt_typ=-1;
    lmt_ptr->is_usr_spc_lmt=True; /* True if any part of limit is user-specified, else False */
    lmt_ptr->min_sng=NULL;
    lmt_ptr->max_sng=NULL;
    lmt_ptr->srd_sng=NULL;
    lmt_ptr->is_usr_spc_min=False;
    lmt_ptr->is_usr_spc_max=False;
    /* rec_skp_ntl_spf is used for record dimension in multi-file operators */
    lmt_ptr->rec_skp_ntl_spf=0L; /* Number of records skipped in initial superfluous files */

    // for a var limit  with a single index (or var) -(1D or nD) this case is handled by lmt_var_mk
    //  we are here when  there is a single index for one of an nD specification
    // a single limit mean a single index -by the parser this is 
    // guaranteed to be non null  (that is ast_lmt_vtr[idx].ind[0] not null) 
    if( ast_lmt_vtr[idx].ind[1]==ANTLR_USE_NAMESPACE(antlr)nullAST)
    {  
      var_out=out( ast_lmt_vtr[idx].ind[0] );
      // convert result to type int
      var_out=nco_var_cnf_typ(NC_INT64,var_out);    
      (void)cast_void_nctype((nc_type)NC_INT64,&var_out->val);
      // only interested in the first value.
      ldx=var_out->val.i64p[0];
      var_out=nco_var_free(var_out);

      lmt_ptr->is_usr_spc_min=True;
      lmt_ptr->srt=ldx;
            
      lmt_ptr->is_usr_spc_max=True;
      lmt_ptr->end=ldx; 

    } 
    else 

    for(jdx=0 ; jdx <3 ; jdx++)
    {

      aRef=ast_lmt_vtr[idx].ind[jdx];

      if(!aRef || aRef->getType() == COLON)
        continue;

      // Calculate number using out()
      var_out=out(aRef);
      // convert result to type int
      var_out=nco_var_cnf_typ(NC_INT64,var_out);    
      (void)cast_void_nctype((nc_type)NC_INT64,&var_out->val);
         // only interested in the first value.
      ldx=var_out->val.i64p[0];
      var_out=nco_var_free(var_out);

      // switch jdx 0-srt,1-end,2-srd
      switch(jdx)
      {
          //srt
          case 0: 
              lmt_ptr->is_usr_spc_min=True;
              lmt_ptr->srt=ldx;
              break;
          // end
          case 1: 
              lmt_ptr->is_usr_spc_max=True;
              lmt_ptr->end=ldx;
              break;
          //srd 
          case 2: 
              lmt_ptr->srd_sng=strdup("~fill_in");
              lmt_ptr->srd=ldx;         
              break;
      }

    }  

    lmt_vtr.push_back(lmt_ptr);
  } // end idx

   return true;;
} /* end lmt_mk */


    void run_dbl(RefAST tr,int icnt){
     int idx=0;
     RefAST ntr=tr;

     extern int      
     ncap_omp_exe(
     std::vector< std::vector<RefAST> > &all_ast_vtr,
     ncoTree** wlk_ptr,
     int wlk_nbr);

     if(tr== ANTLR_USE_NAMESPACE(antlr)nullAST)
        err_prn("run_dbl"," REPORTS given a null AST Reference\n");
            
     //small list dont bother with double parsing     
     // just do a final parse
     if(icnt <4){
       //Final scan
       prs_arg->ntl_scn=False;
       while(idx++ < icnt){
          (void)statements(ntr);   
          ntr=ntr->getNextSibling();   
       }
       return; 
     }
     
     //Initial scan
     prs_arg->ntl_scn=True;
     while(idx++ < icnt){
         (void)statements(ntr);   
       ntr=ntr->getNextSibling();   
     }

     //Define variables in output
    (void)nco_redef(prs_arg->out_id);  
    (void)prs_arg->ncap_def_ntl_scn();
    (void)nco_enddef(prs_arg->out_id);  

    // see if below does anything ? 
    (void)nco_sync(prs_arg->out_id); 

    //Final scan
    if(!prs_arg->NCAP_MPI_SORT)
    { 
      idx=0;
      ntr=tr;
      prs_arg->ntl_scn=False;
      while(idx++ < icnt){
        (void)statements(ntr);   
        ntr=ntr->getNextSibling();   
      }
    }
    else
    {  
    // Sort expressions - MPI preparation
       prs_arg->ntl_scn=False;
      // nb A vector of vectors
      std::vector< std::vector<RefAST> > all_ast_vtr;
      ncoTree **wlk_vtr=(ncoTree**)NULL; 

      // Populate and sort  vector 
      (void)ncap_mpi_srt(tr,icnt,all_ast_vtr,prs_arg);   
      // Evaluate expressions (execute)
      (void)ncap_omp_exe(all_ast_vtr,wlk_vtr,0);  

      /*  
      for(unsigned vtr_idx=0 ; vtr_idx<all_ast_vtr.size(); vtr_idx++)
        for(unsigned jdx=0 ; jdx<all_ast_vtr[vtr_idx].size(); jdx++)
	     (void)statements(all_ast_vtr[vtr_idx][jdx]);
      */
    }
    return; 

    }
 
    int run_exe(RefAST tr, int nbr_dpt){
    // number of statements in block
    int nbr_stmt=0;
    int idx;
    int icnt=0;
    int ntyp;
    int iret=0;
    
    RefAST etr=ANTLR_USE_NAMESPACE(antlr)nullAST;
    RefAST ntr;
   
    ntr=tr;
    do nbr_stmt++; 
    while((ntr=ntr->getNextSibling())); // 20101027 csz "while result of assignment is true" (not an equality comparison) 
    
    if(nbr_stmt <4 || nbr_dpt>0 ){
        prs_arg->ntl_scn=False;
        ntr=tr;
        do{ 
          iret=statements(ntr);
          // break if jump statement   
          if(iret==BREAK || iret==CONTINUE) 
           break; 
        } while((ntr=ntr->getNextSibling())); // 20101027 csz "while result of assignment is true" (not an equality comparison) 
        goto exit;
    }
  
    ntr=tr;

    for(idx=0 ; idx < nbr_stmt; idx++){
      ntyp=ntr->getType();
      // we have hit an IF or a basic block
      if(ntyp==BLOCK || ntyp==IF ||ntyp==DEFDIM || ntyp==WHILE ||ntyp==FOR || ntyp==FEXPR ||ntyp==WHERE || ntyp==EXIT) {
      //  if(ntyp != EXPR ){ 
        if(icnt>0) 
         (void)run_dbl(etr,icnt);
        icnt=0;
        etr=ANTLR_USE_NAMESPACE(antlr)nullAST;; 
        prs_arg->ntl_scn=False;
        iret=statements(ntr);      
      }else{
        if(icnt++==0) etr=ntr;
       }        

     ntr=ntr->getNextSibling();
      
    } // end for

    if(icnt >0){
       iret=0;
       (void)run_dbl(etr,icnt);      
    }
      
exit: return iret;     
            

    } // end run_exe

RefAST nco_dupList(RefAST tr){
      RefAST otr;  
      // nb astFactory is protected- must call from within class
      otr=astFactory->dupList(tr);      
      //otr->setNextSibling( ANTLR_USE_NAMESPACE(antlr)nullAST ) ;
      /*  
      if(otr->getNextSibling()!= ANTLR_USE_NAMESPACE(antlr)ASTNULL )     
        err_prn("nco_dupList", "NON NULL AST SIBLING\n");
      */ 
      return otr; 
     }
#line 59 "ncoTree.hpp"
public:
	ncoTree();
	static void initializeASTFactory( ANTLR_USE_NAMESPACE(antlr)ASTFactory& factory );
	int getNumTokens() const
	{
		return ncoTree::NUM_TOKENS;
	}
	const char* getTokenName( int type ) const
	{
		if( type > getNumTokens() ) return 0;
		return ncoTree::tokenNames[type];
	}
	const char* const* getTokenNames() const
	{
		return ncoTree::tokenNames;
	}
	public: int  lmt_peek(ANTLR_USE_NAMESPACE(antlr)RefAST _t);
	public: int  statements(ANTLR_USE_NAMESPACE(antlr)RefAST _t);
	public: var_sct * out(ANTLR_USE_NAMESPACE(antlr)RefAST _t);
	public: var_sct * assign_ntl(ANTLR_USE_NAMESPACE(antlr)RefAST _t,
		bool bram,bool bret
	);
	public: var_sct * assign(ANTLR_USE_NAMESPACE(antlr)RefAST _t,
		bool bram,bool bret
	);
	public: var_sct * out_asn(ANTLR_USE_NAMESPACE(antlr)RefAST _t);
	public: var_sct * att_plain(ANTLR_USE_NAMESPACE(antlr)RefAST _t);
	public:  RefAST  att2var(ANTLR_USE_NAMESPACE(antlr)RefAST _t);
	public: var_sct * value_list(ANTLR_USE_NAMESPACE(antlr)RefAST _t);
	public: var_sct * value_list_string(ANTLR_USE_NAMESPACE(antlr)RefAST _t);
	public: var_sct * value_list_old(ANTLR_USE_NAMESPACE(antlr)RefAST _t);
	public: var_sct * value_list_string_old(ANTLR_USE_NAMESPACE(antlr)RefAST _t,
		 std::vector<var_sct*> &exp_vtr
	);
	public: bool  where_assign(ANTLR_USE_NAMESPACE(antlr)RefAST _t,
		var_sct *var_msk
	);
	public: var_sct * var_lmt_one(ANTLR_USE_NAMESPACE(antlr)RefAST _t);
	public: var_sct * var_lmt_one_lhs(ANTLR_USE_NAMESPACE(antlr)RefAST _t,
		bool bram
	);
	public: var_sct * var_lmt(ANTLR_USE_NAMESPACE(antlr)RefAST _t);
public:
	ANTLR_USE_NAMESPACE(antlr)RefAST getAST()
	{
		return returnAST;
	}
	
protected:
	ANTLR_USE_NAMESPACE(antlr)RefAST returnAST;
	ANTLR_USE_NAMESPACE(antlr)RefAST _retTree;
private:
	static const char* tokenNames[];
#ifndef NO_STATIC_CONSTS
	static const int NUM_TOKENS = 118;
#else
	enum {
		NUM_TOKENS = 118
	};
#endif
	
};

#endif /*INC_ncoTree_hpp_*/
