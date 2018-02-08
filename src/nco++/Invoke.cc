/* Different version of calculator which parses command line arguments.
   To do this argv[] strings are first written to an ostringstream then
   an istringstream is constructed with string from the ostringstream and
   fed to lexer. */

// Define an anonymous enum containing parser tokens
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <sstream>
#include <antlr/AST.hpp>
#include "ncoEnumTokenTypes.hpp"
#include "libnco++.hh"
#include "ncoLexer.hpp"
#include "ncoParser.hpp"
#include "ncoTree.hpp"
#include <antlr/CharStreamException.hpp>
#include <antlr/TokenStreamException.hpp>
#include "antlr/TokenStreamSelector.hpp"
#include "ExitException.hpp"

#include "Invoke.hh"

#include "sdo_utl.hh" // SDO stand-alone utilities: dbg/err/wrn_prn()

TokenStreamSelector selector;
ncoLexer *lexer=NULL;
ncoParser *parser=NULL;

// Forward declaration
int /* Evaluate expressions -execute nb - contains static members*/
ncap_omp_exe(
std::vector< std::vector<RefAST> > &all_ast_vtr,
ncoTree** wlk_ptr_in,
int wlk_nbr_in)
{
int idx;
int jdx;
int kdx;
int mdx;
int lcl_sz;
int nbr_sz;
RefAST tr;

 static int wlk_nbr; // Same as number of threads
 static ncoTree** wlk_ptr;

 ncoTree* wlk_lcl;

 std::vector<RefAST> inn_vtr;

// Initialize statics then exit
 if( wlk_nbr_in > 0) {
   wlk_nbr=wlk_nbr_in;
   wlk_ptr=wlk_ptr_in;
   return 2;
 }

 // Set all symbol table refs to ntl_scn=false;
 for(idx=0;idx<wlk_nbr;idx++) wlk_ptr[idx]->prs_arg->ntl_scn=False;
  
 // Each block has two lists
 // The first list is of the expressions that contain Lvalues which 
 // are NOT defined in Output (nb this also applies to RAM vars)
 // The second list if of expressions that have all Lvalues defined in
 // output.
        
 for(idx=0 ; idx<(int)all_ast_vtr.size();idx+=2){

   // even block 
   for(jdx=0 ; jdx< (int)all_ast_vtr[idx].size();jdx++)
      (void)wlk_ptr[0]->statements(all_ast_vtr[idx][jdx]); 

   nbr_sz=(int)all_ast_vtr[idx+1].size(); 
   // odd block
   if(nbr_sz==0) continue;
   if(nbr_sz==1) {
     (void)wlk_ptr[0]->statements(all_ast_vtr[idx+1][0]);  
     continue; 
   } 

   // do nc_sync() for all output threads
   (void)nco_sync(wlk_ptr[0]->prs_arg->out_id);      
   for(mdx=0; mdx<wlk_nbr; mdx++) (void)nco_sync(wlk_ptr[mdx]->prs_arg->out_id_readonly); 
   
   inn_vtr=all_ast_vtr[idx+1];
   /*
   for(mdx=0; mdx<inn_vtr.size(); mdx++){
     tr=wlk_ptr[0]->nco_dupList(inn_vtr[mdx]);
     inn_vtr[mdx]=tr;
   } 
   */
#ifdef _OPENMP
#pragma omp parallel for default(none) private(kdx,wlk_lcl,tr) shared(wlk_ptr,idx,nbr_sz,inn_vtr )
#endif
   for(kdx=0 ;kdx< nbr_sz; kdx++) {      
     wlk_lcl= wlk_ptr[omp_get_thread_num()];
       tr=inn_vtr[kdx];
     wlk_lcl->statements(tr);

   } //end OPENMP parallel loop

   // Copy all atts defined in thread in to var_vtr
   for(kdx=0; kdx<wlk_nbr; kdx++){
     //dereference
     NcapVarVector &lcl_vtr=wlk_ptr[kdx]->prs_arg->thr_vtr;
     if(lcl_vtr.empty())
       continue;   
     lcl_sz=(int)lcl_vtr.size();
      for(mdx=0 ; mdx<lcl_sz ; mdx++)
       wlk_ptr[0]->prs_arg->var_vtr.push_ow(lcl_vtr[mdx]); 
     lcl_vtr.clear();   
   }

   // do an nc_sync for all output threads
   (void)nco_sync(wlk_ptr[0]->prs_arg->out_id);      
   for(mdx=0; mdx<wlk_nbr; mdx++) (void)nco_sync(wlk_ptr[mdx]->prs_arg->out_id_readonly); 

 } // end for idx

 return 1;
} /* end ncap_omp_exe() */

int parse_antlr(std::vector<prs_cls> &prs_vtr,char *fl_spt_usr,char *cmd_ln_sng)
{
  ANTLR_USING_NAMESPACE(std);
  ANTLR_USING_NAMESPACE(antlr);
  
  const std::string fnc_nm("parse_antlr"); // [sng] Function name
  bool bExit=false;
  int iret;
  int idx;
  int thd_nbr=(int)prs_vtr.size();  
  std::string filename(fl_spt_usr);
  
  prs_cls *prs_arg;


  istringstream *sin=NULL;
  ifstream *in=NULL;
  
  // ncoLexer *lexer=NULL;
  // ncoParser *parser=NULL;
  
  RefAST t,a;
  ASTFactory ast_factory;

  prs_arg=&prs_vtr[0]; 
  
  std::vector<ncoTree*> wlk_vtr;
 
  // filename=strdup(fl_spt_usr);   
  
  std::vector< std::vector<RefAST> > all_ast_vtr(0);

  try {
    if( cmd_ln_sng ){
      sin= new  istringstream(cmd_ln_sng);
      lexer= new ncoLexer( *sin, prs_arg);
      selector.addInputStream(lexer,cmd_ln_sng);
      selector.select(cmd_ln_sng);
    }else {
      in=new ifstream(filename.c_str());          
      lexer= new ncoLexer( *in, prs_arg);
      selector.addInputStream(lexer,filename);
      selector.select(filename);

    }     

    lexer->setFilename(filename);
    
    parser= new ncoParser(selector);
    parser->setFilename(filename);
    parser->inc_vtr.push_back(filename);
    

    parser->initializeASTFactory(ast_factory);
    parser->setASTFactory(&ast_factory);
    
    
    // Parse the input expressions
    parser->program();
    a = parser->getAST();
    t=a;

    // Print parser tree
    if(nco_dbg_lvl_get() >= nco_dbg_scl){
      dbg_prn(fnc_nm,"Printing parser tree...");
      while( t ) {
	cout << t->toStringTree() << endl;
	t=t->getNextSibling();
      }
      dbg_prn(fnc_nm,"Parser tree printed");
    } // endif dbg
  }  
  
  catch (RecognitionException& pe) {
    parser->reportError(pe);
    // bomb out
    nco_exit(EXIT_FAILURE);
  }
  
  catch (TokenStreamException& te) {
    cerr << te.getMessage();
    // bomb out
    nco_exit(EXIT_FAILURE);
  }
  
  catch (CharStreamException& ce) {
    cerr << ce.getMessage();
    // bomb out
    nco_exit(EXIT_FAILURE);
  }
  
  t=a;
  
  try 
  {   
    ncoTree* wlk_obj;    
    for(idx=0 ; idx< thd_nbr; idx++)
    {
      wlk_obj=new ncoTree(&prs_vtr[idx]);  
      wlk_obj->initializeASTFactory(ast_factory);
      wlk_obj->setASTFactory(&ast_factory);
      wlk_vtr.push_back(wlk_obj); 
    }      

    // initialize static members 
    (void)ncap_omp_exe(all_ast_vtr,&wlk_vtr[0],thd_nbr);
    if(nco_dbg_lvl_get() >= nco_dbg_fl) dbg_prn(fnc_nm,"Walkers initialized");
  
    wlk_vtr[0]->run_exe(t,0);
  }  
    catch (ExitException &strExit)   
    {
      bExit=true;
      // remember ExitEXception ONLY returns an int in a string
      iret=atoi(strExit.getMessage().c_str());
      if(nco_dbg_lvl_get() >= nco_dbg_fl) 
         cerr<<"Exit Exception "<<iret<< endl;
    }
    catch(std::exception& e) 
    {
      cerr << "exception: " << e.what() << endl;
    } 
  
  if(nco_dbg_lvl_get() >= nco_dbg_fl) dbg_prn(fnc_nm,"Walkers completed");
  
  // delete walker pointers
  for(idx=0 ; idx<(int)wlk_vtr.size() ; idx++) delete wlk_vtr[idx];

  delete lexer;
  delete parser;        
  if(sin) delete sin;
  if(in) delete in;

  //(void)nco_free(filename);
  
  if(bExit)
    return iret;
  else
    return 0;  

} /* end parse_antlr() */
