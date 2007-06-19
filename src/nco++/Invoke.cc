/*
 * Different version of the calculator which parses the command line arguments.
 * To do this the argv[] strings are first written to a ostringstream then
 * a istringstream is constructed with the string from the ostringstream and
 * fed to the lexer.
 */


// this defines an anonymous enum containing parser tokens

#include <stdio.h>
#include <malloc.h>
#include <fstream>
#include <sstream>
#include <antlr/AST.hpp>
#include "ncoParserTokenTypes.hpp"
#include "ncoLexer.hpp"
#include "ncoParser.hpp"
#include "ncoTree.hpp"
#include <antlr/CharStreamException.hpp>
#include <antlr/TokenStreamException.hpp>
#include "ncap2.hh"
#include "sdo_utl.hh" // SDO stand-alone utilities: dbg/err/wrn_prn()





//forward declaration
void ncap_def_ntl_scn(prs_sct*);

int parse_antlr(prs_sct *prs_arg,char* fl_spt_usr,char *cmd_ln_sng)
{
  
  ANTLR_USING_NAMESPACE(std);
  ANTLR_USING_NAMESPACE(antlr);
  
  const std::string fnc_nm("parse_antlr"); // [sng] Function name
  
  char *filename;

  istringstream *sin=NULL;
  ifstream *in=NULL;
  
  ncoLexer *lexer=NULL;
  ncoParser *parser=NULL;
  
  RefAST t,a;
  ASTFactory ast_factory;
  
  filename=strdup(fl_spt_usr);   
  
  try {
    
    if( cmd_ln_sng ){
      sin= new  istringstream(cmd_ln_sng);
      lexer= new ncoLexer( *sin, prs_arg);
    }else {
      in=new ifstream(filename);          
      lexer= new ncoLexer( *in, prs_arg);
    }     
    
    
    lexer->setFilename(filename);
    
    parser= new ncoParser(*lexer);
    parser->setFilename(filename);
    

    parser->initializeASTFactory(ast_factory);
    parser->setASTFactory(&ast_factory);
    
    
    // Parse the input expressions
    parser->program();
    a = parser->getAST();
    t=a;

    // Print parser tree
    if(dbg_lvl_get() > 0){
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
  
  try {   


    ncoTree walker(prs_arg);
    walker.initializeASTFactory(ast_factory);
    walker.setASTFactory(&ast_factory);

    if(dbg_lvl_get() > 0) dbg_prn(fnc_nm,"Walker initialized");
  

    // Run script Initial scan
    /* 
      
    cout<<"INITAL SCAN\n";
    prs_arg->ntl_scn=True;
    walker.run(t);
  
    (void)nco_redef(prs_arg->out_id);  
    (void)ncap_def_ntl_scn(prs_arg);
    (void)nco_enddef(prs_arg->out_id);  
 
    cout<<"FINAL SCAN\n"; 
    prs_arg->ntl_scn=False;
    */

    walker.run_exe(t,0);


    
  }  catch(std::exception& e) {
    cerr << "exception: " << e.what() << endl;
  }	
  
  if(dbg_lvl_get() > 0) dbg_prn(fnc_nm,"Walker completed");
  
  delete lexer;
  delete parser;        
  if(sin) delete sin;
  if(in) delete in;

  (void)nco_free(filename);
  
  return 1;
}

